#include "ftl_core0.h"
#include "ftl_taskqueue.h"

#include "../config/config.h"
#include "../lib/dprint.h"
#include "../lib/lock.h"

#include "../hcl/hcl.h"
#include "../l2p/l2p.h"
#include "../buffer/buffer.h"
#include "../nvme/host_lld.h"
#include "../emu/be/be.h"

u32 tmp_sq_index = 0;

/**********************************************************************************
Func    Name: FTL_handle_checkcache
Descriptions: handle check cache status 's task queue entry
Input   para: host_cmd_entry
In&Out  Para:
Output  para:
Return value: SUCCESS OR FAIL
***********************************************************************************/
u32 FTL_handle_checkcache(host_cmd_entry *hcmd_entry)
{
    //	xil_printf("ftl handle checkcache 1\n");
    if (hcmd_entry == NULL)
    {
        debug_printf("[ASSERT]: [FTL_handle_checkcache]hcmd_entry=NULL \n");
        return FAIL;
    }

    u32 op_code = hcmd_entry->op_code;
    u32 cur_cnt = hcmd_entry->cur_cnt;
    u32 req_num = hcmd_entry->req_num;
    u32 cur_lpn = hcmd_entry->cur_lpn;

    //    xil_printf("ftl handle checkcache 2\n");
    u32 hcmd_id = HCL_get_hcmd_entry_index(hcmd_entry);

    while (cur_cnt < req_num)
    {
        //    	xil_printf("ftl handle checkcache 3\n");
        u32 buf_id = L2P_check_buf_hit(cur_lpn);

        // if there is a cache hit.
        if (buf_id != INVALID_ID)
        {
            //    		xil_printf("ftl handle checkcache 4\n");
            // there should be have the dma translation.
            // L2P_hcmd_add_buffer(hcmd_id, buf_id, cur_lpn);
        }
        else
        {
            //    		xil_printf("ftl handle checkcache 5\n");
            // buf_id = L2P_allocate_buf(cur_lpn);
            // L2P_hcmd_add_buffer(hcmd_id, buf_id, cur_lpn);
        }

        cur_cnt++;
        cur_lpn++;
        hcmd_entry->cur_lpn++;
    }

    hcmd_entry->cur_cnt = cur_cnt;

    if (op_code == HCE_WRITE)
    {
        //    	xil_printf("ftl handle checkcache 6\n");
        FTL_sendhcmd(hcmd_entry, HCE_DATAMOVED);
    }
    else if (op_code == HCE_READ)
    {
        //    	xil_printf("ftl handle checkcache 7\n");
        FTL_sendhcmd(hcmd_entry, HCE_TO_SQ);
    }
    return SUCCESS;
}

/**********************************************************************************
Func    Name: FTL_handle_tosq
Descriptions: handle to sq status 's task queue entry
Input   para: host_cmd_entry
In&Out  Para:
Output  para:
Return value: SUCCESS OR FAIL
***********************************************************************************/
u32 FTL_handle_tosq(host_cmd_entry *hcmd_entry)
{
    if (hcmd_entry == NULL)
    {
        debug_printf("[ASSERT]: [FTL_handle_tosq]hcmd_entry=NULL \n");
        return FAIL;
    }

    u32 op_code = hcmd_entry->op_code;
    u32 hcmd_id = HCL_get_hcmd_entry_index(hcmd_entry);
    hcmd_entry->buf_cnt = 1;//L2P_get_hcmd_buff_cnt(hcmd_id);
    u32 send_sq_cnt = 0;//hcmd_entry->send_sq_cnt;
    u32 buf_cnt = hcmd_entry->buf_cnt;

    phy_page_addr ppa;

    // xil_printf("ftl handle to sq 1\n");

    while (send_sq_cnt < buf_cnt)
    {
        u32 cur_lpn = 20;//L2P_hcmd_buf_get_lpn(hcmd_id, send_sq_cnt);
        u32 cur_ppn;

        // xil_printf("ftl handle to sq 2 hcmd %d\n", hcmd_id);
        // L2P_search_ppn_gc_v1(&cur_ppn, cur_lpn, op_code);
        // L2P_calc_ppa(cur_ppn, &ppa);
//    	static u32 tmp_sq_index = 0;
        /* gc,l2p not done yet, use a fake ppa here for test */
        ppa.ch = 1; 
        ppa.ce = 0;
        ppa.nand_flash_addr.slc_mode.lun = 0;
        ppa.nand_flash_addr.slc_mode.plane = 0;
        ppa.nand_flash_addr.slc_mode.block = 0;
        ppa.nand_flash_addr.slc_mode.page = cur_lpn;
#if (CORE_MODE == SINGLE_CORE_MODE)
        tmp_sq_index++;
        tmp_sq_index = tmp_sq_index % 256;
#else
        tmp_sq_index = FCL_get_free_SQ_entry(ppa.ch);
#endif
        if (tmp_sq_index == INVALID_ID)
        {
            FTL_sendhcmd(hcmd_entry, HCE_TO_SQ);
            EMU_log_println(DEBUG, "cannot get sq");
            return FAIL;
        }

        else
        {
            // DEBUG: something wrong with buf
            u32 buf_index = 0;//L2P_hcmd_get_buffer(hcmd_id, send_sq_cnt);
            FCL_set_SQ_entry(hcmd_id, tmp_sq_index, buf_index, op_code, &ppa, HCMD_SPACE);
            
            // xil_printf("ftl handle to sq 3\n");

            u32 ret = SUCCESS;
            do
            {
                ret = FCL_send_SQ_entry(tmp_sq_index, ppa.ch, ppa.ce);
            } while (ret == FAIL);

            send_sq_cnt++;
        }
    }

    // xil_printf("ftl handle to sq 4\n");
    hcmd_entry->send_sq_cnt = send_sq_cnt;

    // #if (CORE_MODE == SINGLE_CORE_MODE)
    //      FTL_sendhcmd(hcmd_entry, HCE_FROM_CQ);
    // #endif
    return SUCCESS;
}

/**********************************************************************************
Func    Name: FTL_handle_datamove
Descriptions: handle data move status 's task queue entry
Input   para: host_cmd_entry
In&Out  Para:
Output  para:
Return value: SUCCESS OR FAIL
***********************************************************************************/
u32 FTL_handle_datamove(host_cmd_entry *hcmd_entry)
{
    if (hcmd_entry == NULL)
    {
        debug_printf("[FTL_handle_checkcache]hcmd_entry=NULL \n");
        return FAIL;
    }

    u32 op_code = hcmd_entry->op_code;
    u32 hcmd_id = HCL_get_hcmd_entry_index(hcmd_entry);
    hcmd_entry->buf_cnt = 1;//L2P_get_hcmd_buff_cnt(hcmd_id);
    u32 buf_cnt = hcmd_entry->buf_cnt;
    u32 cmd_slot_tag = hcmd_entry->nvme_cmd_slot;

    if (op_code == HCE_READ)
    {
        while (hcmd_entry->nvme_dma_cpl < buf_cnt)
        {
            // u32 buf_id = L2P_hcmd_get_buffer(hcmd_id, hcmd_entry->nvme_dma_cpl);
            // u32 buf_4KB_addr = L2P_get_buffer_addr(buf_id);

            // for (u32 i = 0; i < PAGE_SIZE / TU_SIZE; i++)
            // {
            //     set_auto_tx_dma(cmd_slot_tag, i, buf_4KB_addr + 4096 * i);
            // }
            hcmd_entry->nvme_dma_cpl++;
        }

        FTL_sendhcmd(hcmd_entry, HCE_FINISH);
    }
    else
    {
        while (hcmd_entry->nvme_dma_cpl < buf_cnt)
        {
            // u32 buf_id = L2P_hcmd_get_buffer(hcmd_id, hcmd_entry->nvme_dma_cpl);
            // u32 buf_4KB_addr = L2P_get_buffer_addr(buf_id);

            // for (u32 i = 0; i < 4; i++)
            // {
            //     set_auto_rx_dma(cmd_slot_tag, i, buf_4KB_addr + 4096 * i);
            //     // there is 5.12us for each 4KB dma translation.
            //     //#include"sleep.h"
            //     //                usleep(8);
            // }
            hcmd_entry->nvme_dma_cpl++;
        }

        u32 ret = FTL_sendhcmd(hcmd_entry, HCE_TO_SQ); // HCE_TO_SQ  HCE_FINISH
        if (ret == FAIL)
        {
            //	xil_printf("handle_datamove has error !\n");
        }
    }
    return SUCCESS;
}

/**********************************************************************************
Func    Name: FTL_handle_fromcq
Descriptions: handle from cq status 's task queue entry
Input   para: host_cmd_entry
In&Out  Para:
Output  para:
Return value: SUCCESS OR FAIL
***********************************************************************************/
u32 FTL_handle_fromcq(host_cmd_entry *hcmd_entry)
{
    // emu_log_println(LOG, "begine handle fromcq");
    if (hcmd_entry == NULL)
    {
        // debug_printf("[INFO] [FTL_handle_fromcq]hcmd_entry=NULL \n");
        return FAIL;
    }

    if (hcmd_entry->op_code == HCE_WRITE)
    {
        hcmd_entry->status = HCE_FINISH;
        FTL_sendhcmd(hcmd_entry, HCE_FINISH);
    }
    else
    {
        hcmd_entry->status = HCE_DATAMOVED;
        FTL_sendhcmd(hcmd_entry, HCE_DATAMOVED);
    }

    return SUCCESS;
}

/**********************************************************************************
Func    Name: FTL_handle_fromcq
Descriptions: handle finish status 's task queue entry
Input   para: host_cmd_entry
In&Out  Para:
Output  para:
Return value: SUCCESS OR FAIL
***********************************************************************************/
u32 FTL_handle_finish(host_cmd_entry *hcmd_entry)
{
    static int cc = 0;
    EMU_log_println(LOG, "finish %u count %d", hcmd_entry->emu_id, ++cc);
    //  emu_log_println(LOG, "begine handle finish");
    if (hcmd_entry == NULL)
    {
        printf("[INFO] [FTL_handle_finish]hcmd_entry=NULL \n");
        return FAIL;
    }

    // L2P_hcmd_free_buf(HCL_get_hcmd_entry_index(hcmd_entry));
    HCL_reclaim_hcmd_entry(hcmd_entry);

    return SUCCESS;
}
