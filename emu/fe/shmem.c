#include "shmem.h"
#include "../emu_log.h"
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <stddef.h>

void SHM_init()
{
    /* ========== init shm ========== */
    int shm_fd;

    /* if SHM already exist, then delete it */
    if (shm_open(SHM_NAME, O_RDONLY, 0666) != -1)
    {
        shm_unlink(SHM_NAME);
    }

    /* create the shared memory segment as if it was a file */
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        EMU_log_println(ERR, "Shared memory failed");
        shm_unlink(SHM_NAME);
        exit(1);
    }

    /* configure the size of the shared memory segment */
    if (ftruncate(shm_fd, SHM_SIZE) == -1)
    {
        EMU_log_println(ERR, "Ftruncate failed");
        shm_unlink(SHM_NAME);
        exit(1);
    }

    /* map the shared memory segment to the address space of the process */
    shm_base = (u32)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if ((void *)shm_base == MAP_FAILED)
    {
        EMU_log_println(ERR, "Map failed");
        munmap((void *)shm_base, SHM_SIZE);
        shm_unlink(SHM_NAME);
        exit(1);
    }

    /* initialize all 0 */
    memset((void *)SHM_BASE, 0, SHM_SIZE);

    /* init list heads in shm */
    FREE_LIST_HEAD->prev = FREE_LIST_HEAD->next = CMD_SLOT_NUM;
    RDY_LIST_HEAD->prev = RDY_LIST_HEAD->next = CMD_SLOT_NUM;
    PROC_LIST_HEAD->prev = PROC_LIST_HEAD->next = CMD_SLOT_NUM;

    /* add all slot into free_list */
    for (int i = 0; i < CMD_SLOT_NUM; i++)
    {
        shm_list_add(i, FREE_LIST);
    }

    /* set atomic lock */
    *CTRL_BYTE = 0;

    // while (1)
    // {
    //     shm_get();
    //     if (shm_list_empty(RDY_LIST))
    //     {
    //         shm_release();
    //         continue;
    //     }
    //     shm_index i = shm_list_remove(RDY_LIST);
    //     shm_list_add(i, FREE_LIST);
    //     emu_log_println(LOG, "%d", SHM_SLOT(i)->lpn);
    //     // emu_log_println(LOG, "count %d KB", (count += 4));
    //     shm_release();
    // }
}

void SHM_get_rdy_list(shm_index *pindex, shm_cmd *scmd)
{
    shm_get();
    if (shm_list_empty(RDY_LIST))
    {
        *pindex = CMD_SLOT_NUM;
        shm_release();
        return;
    }

    *pindex = shm_list_remove(RDY_LIST);
    shm_cmd *cmd = SHM_SLOT(*pindex);
    if (!cmd)
    {
        EMU_log_println(ERR, "NULL cmd in shm");
    }
    memcpy(scmd, cmd, sizeof(shm_cmd));
    /*
     * NOTE: 目前还未添加模拟释放NVMe槽中命令的操作，
     * 这个过程应该是在数据传输完成后自动进行
     * 可能以后需要？？？
     */
    // shm_list_add(*index, PROC_LIST);
    shm_release();
}

void SHM_add_rdy_list(shm_index index, shm_cmd *scmd)
{
    shm_get();
    shm_cmd *cmd = SHM_SLOT(index);
    if (!cmd)
    {
        EMU_log_println(ERR, "NULL cmd in shm");
    }
    memcpy(cmd, scmd, sizeof(shm_cmd));
    shm_list_add(index, RDY_LIST);
    shm_release();
}

void SHM_add_free_list(shm_index index)
{
    shm_get();
    shm_list_add(index, FREE_LIST);
    shm_release();
}