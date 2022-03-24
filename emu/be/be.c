#include "be.h"
#include "../../ftl/ftl_core1.h"
#include "../../lib/list.h"
#include "../../config/config.h"
#include "../../emu/emu_log.h"
#include "../../hcl/hcl.h"
#include "../../emu/emu_config.h"
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* TODO: 自动计算Log长度 */
#define PPA_LOG_LEN 58
#ifdef EMU_DATA_FLOW
#define LOG_LEN (PPA_LOG_LEN + 16 * KB)
#else
#define LOG_LEN (PPA_LOG_LEN)
#endif
int flash_log_fd = -1;

/* completely same as my_polling_status in ftl_core1 */
list_head register_list = {&register_list, &register_list};
list_head complete_list = {&complete_list, &complete_list};
pthread_mutex_t fcl_be_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t list_cond = PTHREAD_COND_INITIALIZER;

void BE_init()
{
    /* use unlink to delete former flash log */
    // unlink(FLASH_LOG_NAME);

    /* open or create file */
    /* TODO: no need to open/close file everytime with fsync() */
    int fd = open(FLASH_LOG_NAME, O_CREAT);
    if (fd == -1)
    {
        EMU_log_println(ERR, "can not open %s", FLASH_LOG_NAME);
        exit(0);
    }
    close(fd);
    system("chmod 777 " FLASH_LOG_NAME);
}

void *be()
{
    while (1)
    {
        pthread_mutex_lock(&fcl_be_mutex);
        while (list_empty(&register_list))
        {
            pthread_cond_wait(&list_cond, &fcl_be_mutex);
        }

        /* ========== polling sq_list ============ */
        EMU_sq *r_sq = container_of(register_list.next, EMU_sq, node);
        list_delete_head(&register_list);
        EMU_log_println(LOG, "deal sq of hcmd %d", HCL_get_hcmd_entry_addr(r_sq->sq_entry.hcmd_index)->emu_id);
        pthread_mutex_unlock(&fcl_be_mutex);

        /* ========== r/w in emu flash ============= */

        /* ========== get ppa in sq_entry ========== */
        phy_page_addr ppa;
        ppa.ch = r_sq->ch;
        ppa.ce = r_sq->ce;
        ppa.nand_flash_addr.array[0] = r_sq->sq_entry.a0;
        ppa.nand_flash_addr.array[1] = r_sq->sq_entry.a1;
        ppa.nand_flash_addr.array[2] = r_sq->sq_entry.a2;
        ppa.nand_flash_addr.array[3] = r_sq->sq_entry.a3;
        ppa.nand_flash_addr.array[4] = r_sq->sq_entry.a4;
        ppa.nand_flash_addr.array[5] = r_sq->sq_entry.a5;

        /* NOTE: Only for SLC_MODE now */
        if (r_sq->sq_entry.op == HCE_READ)
        {
            BE_read_log(ppa.ch,
                        ppa.ce,
                        ppa.nand_flash_addr.slc_mode.lun,
                        ppa.nand_flash_addr.slc_mode.plane,
                        ppa.nand_flash_addr.slc_mode.block,
                        ppa.nand_flash_addr.slc_mode.page,
                        (r_sq->sq_entry.buf_adr << 14));
        }
        else if (r_sq->sq_entry.op == HCE_WRITE)
        {
            BE_write_log(ppa.ch,
                         ppa.ce,
                         ppa.nand_flash_addr.slc_mode.lun,
                         ppa.nand_flash_addr.slc_mode.plane,
                         ppa.nand_flash_addr.slc_mode.block,
                         ppa.nand_flash_addr.slc_mode.page,
                         (r_sq->sq_entry.buf_adr << 14));
        }

        /* tell FTL sq completion */
        pthread_mutex_lock(&fcl_be_mutex);
        list_add_tail(&r_sq->node, &complete_list);
        pthread_mutex_unlock(&fcl_be_mutex);
    }
}

/* used by FTL to register a sq entry to backend */
void BE_send_sq(u32 sq_index, hw_queue_entry *sq_set, int ch, int ce)
{
    pthread_mutex_lock(&fcl_be_mutex);

    /* ========== copy sq from FCTL_MEM ========== */
    /* TODO: can just sq in FCTL_MEM, copy it just for debug */
    EMU_sq *sq = malloc(sizeof(EMU_sq));
    sq->ch = ch;
    sq->ce = ce;
    sq->sq_index = sq_index;
    memcpy(&sq->sq_entry, sq_set, sizeof(hw_queue_entry));

    /* ========== add EMU_sq to register_list ========== */
    list_add_tail(&sq->node, &register_list);
    EMU_log_println(LOG, "set sq of hcmd %d", HCL_get_hcmd_entry_addr(sq->sq_entry.hcmd_index)->emu_id);

    pthread_cond_signal(&list_cond);
    pthread_mutex_unlock(&fcl_be_mutex);
}

/* write log in flash_log file */
void BE_write_log(u32 ch, u32 ce, u32 lun, u32 plane, u32 block, u32 page, u32 buf_addr)
{
    /* ========== open file ========== */
    FILE *fp = fopen(FLASH_LOG_NAME, "a");
    if (!fp)
    {
        EMU_log_println(ERR, "can not open %s", FLASH_LOG_NAME);
        exit(0);
    }

    /* ========== write log to end of file ========== */
    // fseek(fp, 0, SEEK_END);
    fprintf(fp, "\nch:%4u ce:%4u lun:%4u plane:%4u block:%4u page:%4u\n", ch, ce, lun, plane, block, page);
    // memset((void *)buf_addr, 'a', 10);

#ifdef EMU_DATA_FLOW
    fwrite((void *)buf_addr, 16 * KB, 1, fp);
#endif
    fclose(fp);
}

/* read latest update log in flash_log file */
void BE_read_log(u32 ch, u32 ce, u32 lun, u32 plane, u32 block, u32 page, u32 buf_addr)
{
    /* ========== open file ========== */
    FILE *fp = fopen(FLASH_LOG_NAME, "r");
    if (!fp)
    {
        EMU_log_println(ERR, "can not open %s", FLASH_LOG_NAME);
        exit(0);
    }

    /* ========== find log from end of file ========== */
    fseek(fp, 0, SEEK_END);
    u32 _ch, _ce, _lun, _plane, _block, _page;

    while (!fseek(fp, -(LOG_LEN), SEEK_CUR))
    {
        fscanf(fp, "\nch:%4u ce:%4u lun:%4u plane:%4u block:%4u page:%4u\n", &_ch, &_ce, &_lun, &_plane, &_block, &_page);
        if (ch == _ch &&
            ce == _ce &&
            lun == _lun &&
            plane == _plane &&
            block == _block &&
            page == _page)
        {
#ifdef EMU_DATA_FLOW
            fread((void *)buf_addr, 16 * KB, 1, fp);
#endif
            goto find_log;
        }
    }
not_find_log:
    EMU_log_println(ERR, "not find log");
find_log:
    fclose(fp);
}
