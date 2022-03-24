#include "../emu/emu_config.h"
#include "ftl_nvme.h"
#include "ftl_taskqueue.h"
#ifndef EMU
#include "../nvme/io_access.h"
#include "../nvme/nvme.h"
#include "../nvme/host_lld.h"
#include "../nvme/nvme_main.h"
#include "../nvme/nvme_admin_cmd.h"
#include "../nvme/nvme_io_cmd.h"

#include "xil_printf.h"
#include "string.h"

volatile NVME_CONTEXT g_nvmeTask;
#else
#include "../emu/emu_log.h"
#include "../hcl/hcl.h"
#include "../emu/fe/shmem.h"
#endif
void FTL_nvme_init(void)
{
#ifndef EMU
    g_nvmeTask.status = NVME_TASK_IDLE;
    g_nvmeTask.cacheEn = 0;

    g_hostDmaStatus.fifoTail.autoDmaRx = 0;
    g_hostDmaStatus.autoDmaRxCnt = 0;
    g_hostDmaStatus.fifoTail.autoDmaTx = 0;
    g_hostDmaStatus.autoDmaTxCnt = 0;

    g_hostDmaAssistStatus.autoDmaRxOverFlowCnt = 0;
    g_hostDmaAssistStatus.autoDmaTxOverFlowCnt = 0;
#endif
}
/**********************************************************************************
Func    Name: FTL_nvme_req_polling
Descriptions: polling nvme娴肩姵娼甸惃鍒d,濮ｅ繑顐紁olling閻ㄥ嫪閲滈弫锟�<= MAX_POLLING_NUM,
楠炶泛鐨o閸涙垝鎶ら崣鎴︼拷浣稿煂check cached閻樿埖锟戒緤绱漚dmin閻樿埖锟戒胶娈戦崨鎴掓姢娴兼艾鎮撳銉ヮ槱閻烇拷
Input   para:
In&Out  Para:
Output  para:
Return value: 鏉╂柨娲杊cmd閹稿洭鎷＄悰銊с仛閹存劕濮涢敍瀛║LL鐞涖劎銇氭径杈Е
***********************************************************************************/
void FTL_nvme_req_polling()
{
#ifdef EMU
    int i;
    static int c = 0;
    shm_index index;
    shm_cmd scmd;

    for (i = 0; i < MAX_POLLING_NUM; i++)
    {
#ifdef EMU
        /* When enable emu, ftl will get cmd from shm and add it to hcl */
        index = CMD_SLOT_NUM;
        SHM_get_rdy_list(&index, &scmd);
        if (index == CMD_SLOT_NUM)
        {
            continue;
        }
#endif
        host_cmd_entry *hcmd = HCL_get_host_cmd_entry();

        if (hcmd != NULL)
        {

            if (scmd.ops == SHM_WRITE_OPS)
            {
                hcmd->op_code = HCE_WRITE;
            }
            else if (scmd.ops == SHM_READ_OPS)
            {
                hcmd->op_code = HCE_READ;
            }
            else
            {
                EMU_log_println(ERR, "get wrong cmd from shm");
            }

            hcmd->nvme_start_lba = scmd.lpn;
            hcmd->nvme_req_num = scmd.size / HOST_BLOCK_SIZE;

            // // do this because fio has duplicated request
            // static u32 tmp_lpn = 0;
            // tmp_lpn++;

            hcmd->start_lpa = hcmd->nvme_start_lba / 4;
            hcmd->req_num = hcmd->nvme_req_num / 4;
            hcmd->cur_lpn = hcmd->start_lpa;
            hcmd->nvme_dma_cpl = 0;

            hcmd->mod_req_num = 0;
            hcmd->mod_start_LA = 0;

            hcmd->cur_cnt = 0;
            hcmd->cpl_cnt = 0;
            hcmd->send_sq_cnt = 0;
            hcmd->buf_cnt = 0;

            hcmd->start_unalign_sqindex = 0;
            hcmd->end_unalign_sqindex = 0;
            // used for finding cmd slot in shm
            hcmd->nvme_cmd_slot = index;
            hcmd->emu_id = ++c;
            EMU_log_println(LOG, "get cmd id: %d from fio", hcmd->emu_id);
            FTL_sendhcmd(hcmd, HCE_CHECK_CACHE);
            SHM_add_free_list(index);
        }
        else
        {
            /* if no hcmd now, we will add cmd back to shm*/
            // xil_printf("There is no host command now ! \n");
            SHM_add_rdy_list(index, &scmd);
        }
    }
#else
    //	xil_printf("ftl nvme req polling 1\n");

    unsigned int exeLlr;
    int i;
    for (i = 0; i < MAX_POLLING_NUM; i++)
    {
        //    	xil_printf("ftl name req polling 2, %u\n",g_nvmeTask.status);

        exeLlr = 1;
        if (g_nvmeTask.status == NVME_TASK_WAIT_CC_EN)
        {
            //        	xil_printf("ftl name req polling 3\n");
            unsigned int ccEn;
            ccEn = check_nvme_cc_en();
            if (ccEn == 1)
            {
                set_nvme_admin_queue(1, 1, 1);
                set_nvme_csts_rdy(1);
                g_nvmeTask.status = NVME_TASK_RUNNING;
                xil_printf("\r\nNVMe ready!!!\r\n");
            }
        }
        else if (g_nvmeTask.status == NVME_TASK_RUNNING)
        {
            //        	xil_printf("ftl name req polling 4\n");
            NVME_COMMAND nvmeCmd;
            unsigned int cmdValid;

            //            xil_printf("ftl name req polling 4.1\n");
            cmdValid = get_nvme_cmd(&nvmeCmd.qID, &nvmeCmd.cmdSlotTag, &nvmeCmd.cmdSeqNum, nvmeCmd.cmdDword);
            //            xil_printf("ftl name req polling 4.4\n");
            if (cmdValid == 1)
            {
                if (nvmeCmd.qID == 0)
                {
                    //                	xil_printf("ftl name req polling 4.2\n");
                    handle_nvme_admin_cmd(&nvmeCmd);
                }
                else
                {
                    //                	xil_printf("ftl name req polling 4.3\n");
                    handle_nvme_io_cmd(&nvmeCmd);
                    exeLlr = 0;
                }
            }
        }
        else if (g_nvmeTask.status == NVME_TASK_SHUTDOWN)
        {
            //        	xil_printf("ftl name req polling 5\n");
            NVME_STATUS_REG nvmeReg;
            nvmeReg.dword = IO_READ32(NVME_STATUS_REG_ADDR);
            if (nvmeReg.ccShn != 0)
            {
                unsigned int qID;
                set_nvme_csts_shst(1);

                for (qID = 0; qID < 8; qID++)
                {
                    set_io_cq(qID, 0, 0, 0, 0, 0, 0);
                    set_io_sq(qID, 0, 0, 0, 0, 0, 0);
                }

                set_nvme_admin_queue(0, 0, 0);
                g_nvmeTask.cacheEn = 0;
                set_nvme_csts_shst(2);
                g_nvmeTask.status = NVME_TASK_WAIT_RESET;
                xil_printf("\r\nNVMe shutdown!!!\r\n");
            }
        }
        else if (g_nvmeTask.status == NVME_TASK_WAIT_RESET)
        {
            //        	xil_printf("ftl name req polling 6\n");
            unsigned int ccEn;
            ccEn = check_nvme_cc_en();
            if (ccEn == 0)
            {
                g_nvmeTask.cacheEn = 0;
                set_nvme_csts_shst(0);
                set_nvme_csts_rdy(0);
                g_nvmeTask.status = NVME_TASK_IDLE;
                xil_printf("\r\nNVMe disable!!!\r\n");
            }
        }
        else if (g_nvmeTask.status == NVME_TASK_RESET)
        {
            //        	xil_printf("ftl name req polling 7\n");
            unsigned int qID;
            for (qID = 0; qID < 8; qID++)
            {
                set_io_cq(qID, 0, 0, 0, 0, 0, 0);
                set_io_sq(qID, 0, 0, 0, 0, 0, 0);
            }
            g_nvmeTask.cacheEn = 0;
            set_nvme_admin_queue(0, 0, 0);
            set_nvme_csts_shst(0);
            set_nvme_csts_rdy(0);
            g_nvmeTask.status = NVME_TASK_IDLE;
            xil_printf("\r\nNVMe reset!!!\r\n");
        }
        //        xil_printf("ftl name req polling 8\n");
        if (exeLlr)
        {
        }
        // ExeLowLevelReq(SUB_REQ_QUEUE);
    }
#endif
}