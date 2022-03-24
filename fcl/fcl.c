#include "fcl.h"

#include "../lib/lock.h"
#include "../config/config.h"

static hw_queue *hw_SQ[TOTAL_CHANNEL];
static hw_queue *hw_CQ[TOTAL_CHANNEL];

#include "../emu/emu_config.h"
#ifdef EMU
/*
 * When enable EMU, we not emulate the behavior of FCL yet
 * We will send the cmd into backend when using FCL_send_SQ_entry()
 * then write a log by backend, after that, it will change phase to make
 * FTL_CQ_Polling aware that log been written and FCL_free_SQ_entry()
 * So now we haven't use fifo reg yet
 */
#include <string.h>
#include <stdlib.h>
#include "../emu/emu_log.h"
#include "../emu/emu_io.h"
u32 fcl_base;
#include "../emu/be/be.h"
#endif

/**********************************************************************************
Func    Name: FCL_init_flash_controller
Descriptions: initailize flash controller
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_init_flash_controller()
{
#ifndef EMU
    FCL_set_timing();
    FCL_set_mode(1);

    // reset
    for (u32 ch = 0; ch < TOTAL_CHANNEL; ch++)
    {
        for (u32 ce = 0; ce < (TOTAL_CE / TOTAL_CHANNEL); ce++)
        {
            FCL_nand_reset(ch, ce);
        }
    }

    // FCL_wait_rbn(1,7,0);
    usleep(50000);

    FCL_reset_ch();

    FCL_release_ch();

    for (u32 ch = 0; ch < TOTAL_CHANNEL; ch++)
    {
        for (u32 ce = 0; ce < (TOTAL_CE / TOTAL_CHANNEL); ce++)
        {
            FCL_set_feature_ddr(ch, ce);
        }
    }

    FCL_set_mode(0);
#else
    fcl_base = (u32)malloc(16 * MB + 16 * KB);
    fcl_base = ((((u32)(fcl_base)) >> 14) << 14) + 16 * KB;
    memset((void *)fcl_base, 0, 16 * MB);
#endif
}

/**********************************************************************************
Func    Name: FCL_init_hw_queue
Descriptions: initailize SQ CQ and free SQ list
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_init_hw_queue()
{
    for (u32 i = 0; i < TOTAL_CHANNEL; i++)
    {
        hw_SQ[i] = (hw_queue *)(HW_CH_SQ_ENTRY_ADDR + i * SQ_ENTRY_SZ_PER_CHANNEL);
        hw_CQ[i] = (hw_queue *)(HW_CH_CQ_ENTRY_ADDR + i * CQ_ENTRY_SZ_PER_CHANNEL);
        memset(hw_SQ[i], 0x00, sizeof(hw_queue));
        for (u32 j = 0; j < HW_QUEUE_DEPTH; j++)
        {
            hw_SQ[i]->hw_queue[j].next_ptr = j + 1;
            hw_SQ[i]->hw_queue[j].cur_ptr = j;
        }
        hw_SQ[i]->hw_queue[HW_QUEUE_DEPTH - 1].next_ptr = HW_QUEUE_DEPTH - 1;
        u32 *hw_sq_bitmap = (u32 *)(HW_CH_SQ_BITMAP_ADDR + i * SQ_BITMAP_SZ_PER_CHANNEL);
        memset(hw_sq_bitmap, 0x00, HW_QUEUE_DEPTH / 8);
        memset(hw_CQ[i], 0xff, sizeof(hw_queue));
        FCL_set_hw_base_addr(i, (u32)hw_SQ[i], (u32)hw_CQ[i]);
    }
}

void FCL_set_hw_base_addr(u32 ch, u32 sq_addr, u32 cq_addr)
{
    Xil_Out32((FCTL_CH_REG_BASE_ADDR + ch * FCTL_SZ_PER_CH + 0x18), sq_addr);
    Xil_Out32((FCTL_CH_REG_BASE_ADDR + ch * FCTL_SZ_PER_CH + 0x1C), cq_addr);
}

/**********************************************************************************
Func    Name: FCL_get_free_SQ_entry
Descriptions: Get a free SQ entry, from the free SQ entry list銆�
Input   para: None
In&Out  Para: None
Output  para: None
Return value: entry's index
***********************************************************************************/
u32 FCL_get_free_SQ_entry(u32 ch)
{
    u32 ret_index = 0;
    u32 find_flag = 0;
    u32 *hw_sq_bitmap = (u32 *)(HW_CH_SQ_BITMAP_ADDR + ch * SQ_BITMAP_SZ_PER_CHANNEL);

    get_spin_lock((u32 *)(CH_SQ_SPIN_LOCK + ch * SPIN_LOCK_SZ));

    for (u32 i = 0; (i < HW_QUEUE_DEPTH / 32) && (find_flag == 0); i++)
    {
        for (u32 j = 0; (j < 32) && (find_flag == 0); j++)
        {
            if (((hw_sq_bitmap[i]) & (1 << j)) >> j == 0)
            {

                ret_index = 32 * i + j;
                hw_sq_bitmap[i] |= (1 << j);
                find_flag = 1;
            }
        }
    }

    release_spin_lock((u32 *)(CH_SQ_SPIN_LOCK + ch * SPIN_LOCK_SZ));

    if (find_flag == 1)
    {
        return ret_index;
    }
    return INVALID_INDEX;
}

u32 FCL_SQ_empty(u32 ch)
{
    u32 *hw_sq_bitmap = (u32 *)(HW_CH_SQ_BITMAP_ADDR + ch * SQ_BITMAP_SZ_PER_CHANNEL);

    for (u32 i = 0; (i < HW_QUEUE_DEPTH / 32); i++)
    {
        for (u32 j = 0; (j < 32); j++)
        {
            if (((hw_sq_bitmap[i]) & (1 << j)) >> j == 0)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/**********************************************************************************
Func    Name: FCL_set_SQ_entry
Descriptions: 鏍规嵁host command濉厖buffer淇℃伅
Input   para: hcmd_entry_index銆丼Q_entry_index銆乥uffer_index銆乷pcode銆乶and_flash_address
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_set_SQ_entry(u32 hcmd_entry_index, u32 SQ_entry_index, u32 buffer_index, u32 opcode, phy_page_addr *ppa, u32 cmd_sp)
{
    u32 ch = ppa->ch;

    hw_SQ[ch]->hw_queue[SQ_entry_index].op = opcode;
    hw_SQ[ch]->hw_queue[SQ_entry_index].t0_add_da = 1;
    hw_SQ[ch]->hw_queue[SQ_entry_index].cmd_space = cmd_sp;

    if (opcode != HCE_ERASE)
    {
        u32 buffer_addr = L2P_get_buffer_addr(buffer_index);
        hw_SQ[ch]->hw_queue[SQ_entry_index].buf_adr = buffer_addr >> 14;
        hw_SQ[ch]->hw_queue[SQ_entry_index].a0 = ppa->nand_flash_addr.array[0];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a1 = ppa->nand_flash_addr.array[1];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a2 = ppa->nand_flash_addr.array[2];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a3 = ppa->nand_flash_addr.array[3];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a4 = ppa->nand_flash_addr.array[4];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a5 = ppa->nand_flash_addr.array[5];
    }
    else
    {
        hw_SQ[ch]->hw_queue[SQ_entry_index].a0 = ppa->nand_flash_addr.array[2];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a1 = ppa->nand_flash_addr.array[3];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a2 = ppa->nand_flash_addr.array[4];
        hw_SQ[ch]->hw_queue[SQ_entry_index].a3 = ppa->nand_flash_addr.array[5];
    }
    // 灏唄ost command鐨刬ndex浼犻�掑埌SQ entry涓��
    hw_SQ[ch]->hw_queue[SQ_entry_index].hcmd_index = hcmd_entry_index;
}

/**********************************************************************************
Func    Name: FCL_get_ce_address
Descriptions: 鑾峰彇ch瀵瑰簲鐨刢e瀵勫瓨鍣ㄥ湴鍧�
Input   para: channal
In&Out  Para: None
Output  para: None
Return value: ce瀵勫瓨鍣ㄥ湴鍧�
***********************************************************************************/
u32 FCL_get_fifo_status(u32 ch)
{
    return Xil_In32(FCTL_CH_REG_BASE_ADDR + ch * FCTL_SZ_PER_CH + 0x24) & 0x000000ff;
}

/**********************************************************************************
Func    Name: FCL_set_fifo
Descriptions: 缁檆h ce瀵瑰簲鐨刦ifo鍙戦�丼Q entry
Input   para: index銆乧h銆乧e
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_set_fifo(u32 index, u32 ch, u32 ce)
{
    Xil_Out32((FCTL_CH_QUEUE_BASE_ADDR + ch * FCTL_SZ_PER_CH + 0x04 * ce), index);
}

/**********************************************************************************
Func    Name: FCL_send_SQ_entry
Descriptions: 鍙戦�佽姹傝嚦flash controller
Input   para: index銆乧h銆乧e
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
u32 FCL_send_SQ_entry(u32 index, u32 ch, u32 ce)
{
#ifdef EMU
    BE_send_sq(index, &(hw_SQ[ch]->hw_queue[index]), ch, ce);
    return SUCCESS;
#endif
    //
    // xil_printf("index ch ce %d %d %d\n",index,ch,ce);
    u32 reg_data = FCL_get_fifo_status(ch);
    reg_data &= 0x000000ff;

    u32 bit = (1 << ce);
    //
    if ((reg_data & bit) == 0)
    {
        FCL_set_fifo(index, ch, ce);
        return SUCCESS;
    }

    else
    {
        return FAIL;
    }
}

/**********************************************************************************
Func    Name: FTL_free_SQ_entry
Descriptions: 灏哠Q entry鏀惧洖free list涓�
Input   para: index
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_free_SQ_entry(u32 ch, u32 index)
{
    u32 tmp_bit_val = 0;
	u32 *hw_sq_bitmap = (u32 *)(HW_CH_SQ_BITMAP_ADDR + ch * SQ_BITMAP_SZ_PER_CHANNEL);
	
	get_spin_lock((u32 *)(CH_SQ_SPIN_LOCK + ch * SPIN_LOCK_SZ));

	tmp_bit_val = ~(1<<(index%32));
	hw_sq_bitmap[index/32] &= tmp_bit_val;

	release_spin_lock((u32 *)(CH_SQ_SPIN_LOCK + ch * SPIN_LOCK_SZ));
}

/**********************************************************************************
Func    Name: FCL_set_timing
Descriptions: set flash controller鐨則iming
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_set_timing(void)
{
    u32 addr;
    u32 data;
    u32 is_bypass_ecc = 0;
    u32 is_bypass_scrm = 0;
    data = (is_bypass_ecc << 24) + (is_bypass_scrm << 28) + 0x03050F;
    for ( u32 ch = 0; ch < TOTAL_CHANNEL; ch ++ ){
		addr = FCTL_CH_REG_BASE_ADDR + ch * FCTL_SZ_PER_CH;
        Xil_Out32(addr, data); // 00: ERASE_ADDR_NUM, ADDR_NUM, tADL
        addr = addr + 4 ;
        Xil_Out32(addr, 0x04080804); // 04: CA
        addr = addr + 4 ;
        Xil_Out32(addr, 0x04060404); // 08:
        addr = addr + 4 ;
        Xil_Out32(addr, 0x0404);     // 0C:
        addr = addr + 4 ;
        Xil_Out32(addr, 0x0000);     // 10: RE_PHASE, DQS_DELAY, DQ_DELAY
        addr = addr + 4 ;
        Xil_Out32(addr, 0x04040404); // 14: DIN
    }
}

/**********************************************************************************
Func    Name: FCL_nand_reset
Descriptions: reset nand flash 棰楃矑
Input   para: ch銆乧e
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_nand_reset(u32 ch, u32 ce)
{
    u32 addr;
    addr = FCTL_BASE_SWQ_CMD_SRAM;
    Xil_Out32(addr, 0x180000FF); // CMD FFH, RESET, LAST
    addr = addr + 4;
    Xil_Out32(addr, 0x880000FF); // IDLE(FF), SEQ_END, LAST
    addr = addr + 4;
    FCL_swq_exec_req(ch, ce);
}

/**********************************************************************************
Func    Name: FCL_get_rbn
Descriptions: 璇诲彇nand 棰楃矑鐨勭姸鎬�
Input   para: ch銆乧e銆乮s_ddr
In&Out  Para: None
Output  para: None
Return value: 璇诲彇鍒扮殑瀵勫瓨鍣ㄧ殑鍊�
***********************************************************************************/
u32 FCL_get_rbn(u32 ch, u32 ce, u32 is_ddr)
{
    u32 addr;
    u32 data;

    addr = FCTL_BASE_SWQ_CMD_SRAM;

    Xil_Out32(addr, 0x18000070); // CMD 70
    addr = addr + 4;
    Xil_Out32(addr, 0x0800000F); // IDLE(FF) for tWHR
    addr = addr + 4;
    if (is_ddr)
    {
        Xil_Out32(addr, 0x68000000); // DOUT/LEN=0
        addr = addr + 4;
    }
    else
    {
        Xil_Out32(addr, 0x48000000);
        addr = addr + 4;
    }
    Xil_Out32(addr, 0x88000000); // IDLE(0) LAST/SEQ_END, for OP Done
    addr = addr + 4;

    FCL_swq_exec_req(ch, ce);

    if (is_ddr)
    {
        addr = FCTL_BASE_SWQ_DDR_SRAM;
    }
    else
    {
        addr = FCTL_BASE_SWQ_SDR_SRAM;
    }

    data = Xil_In32(addr);
    xil_printf("[GET RBN] data = %x\n\r", data);
    data = data & 0x00FF;

    return data;
}

/**********************************************************************************
Func    Name: FCL_wait_rbn
Descriptions: 绛夊緟璇诲彇nand棰楃矑鐘舵�佸畬鎴�
Input   para: ch銆乧e銆乮s_ddr
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_wait_rbn(u32 ch, u32 ce, u32 is_ddr)
{
    u32 data;

    data = FCL_get_rbn(ch, ce, is_ddr);
    xil_printf("[DEBUG] DDR=%d, NAND%d_%d Status = %x\n\r", is_ddr, ch, ce, data);
    while (data != 0xe0)
    {
        data = FCL_get_rbn(ch, ce, is_ddr);
        xil_printf("[DEBUG] DDR=%d, NAND%d_%d Status = %x    ;\n\r", is_ddr, ch, ce, data);
    }
}

/**********************************************************************************
Func    Name: FCL_swq_exec_req
Descriptions: 鎵ц杞欢MOP鍛戒护
Input   para: ch銆乧e
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_swq_exec_req(u32 ch, u32 ce)
{
    u32 addr;
    u32 data;
    u32 read_data;

    addr = (FCTL_BASE_SYS_REG + 0x0004);
    data = (ch << 16) + (ce << 8) + 0x1;
    Xil_Out32(addr, data);

    read_data = Xil_In32(addr);
    // xil_printf("EXEC_REQ=%x\n\r", read_data);
    while ((read_data & 0x01) != 0)
    {
        read_data = Xil_In32(addr);
        // xil_printf("EXEC_REQ=%x\n\r", read_data);
    }
}

/**********************************************************************************
Func    Name: FCL_set_mode
Descriptions: 璁剧疆flash controller鐨勬ā寮�
Input   para: mode
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_set_mode(u32 mode)
{
    u32 addr;
    addr = FCTL_BASE_SYS_REG;
    Xil_Out32(addr, mode);
}

/**********************************************************************************
Func    Name: FCL_reset_ch
Descriptions: reset channal
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_reset_ch()
{
    u32 addr;
    u32 data;
    addr = (FCTL_BASE_SYS_REG + 8);
    data = 0x01010101;
    Xil_Out32(addr, data);
};

/**********************************************************************************
Func    Name: FCL_release_ch
Descriptions: release channal
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_release_ch()
{
    u32 addr;
    u32 data;
    addr = (FCTL_BASE_SYS_REG + 8);
    data = 0x0;
    Xil_Out32(addr, data);
};

/**********************************************************************************
Func    Name: FCL_set_feature_ddr
Descriptions: 璁剧疆ch ce涓篸dr妯″紡
Input   para: ch銆乧e
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FCL_set_feature_ddr(u32 ch, u32 ce)
{
    u32 addr;

    // FA = 02
    addr = FCTL_BASE_SWQ_CMD_SRAM;
    Xil_Out32(addr, 0x100000EF); // CMD EFH
    addr = addr + 4;
    Xil_Out32(addr, 0x38000002); // FA=02, TM, LAST
    addr = addr + 4;
    Xil_Out32(addr, 0x080000FF); // IDLE(FF), LAST, For tADL
    addr = addr + 4;
    Xil_Out32(addr, 0x50000006); // SDR DIN P1 - DQS/RE C, VEN
    addr = addr + 4;
    Xil_Out32(addr, 0x50000000); // SDR DIN P2(RSVD)
    addr = addr + 4;
    Xil_Out32(addr, 0x50000000); // SDR DIN P3(RSVD)
    addr = addr + 4;
    Xil_Out32(addr, 0x58000000); // SDR DIN P4(RSVD), Last
    addr = addr + 4;
    Xil_Out32(addr, 0x880000FF); // IDLE(FF), LAST, SEQ_END (tFEAT)
    addr = addr + 4;
    FCL_swq_exec_req(ch, ce);

    // FA = 01
    addr = FCTL_BASE_SWQ_CMD_SRAM;
    Xil_Out32(addr, 0x100000EF); // CMD EFH
    addr = addr + 4;
    Xil_Out32(addr, 0x38000001); // FA=01, TM, LAST
    addr = addr + 4;
    Xil_Out32(addr, 0x080000FF); // IDLE(FF), LAST, For tADL
    addr = addr + 4;
    Xil_Out32(addr, 0x50000024); // SDR DIN P1 - DDR2/TM4
    addr = addr + 4;
    Xil_Out32(addr, 0x50000000); // SDR DIN P2(RSVD)
    addr = addr + 4;
    Xil_Out32(addr, 0x50000000); // SDR DIN P3(RSVD)
    addr = addr + 4;
    Xil_Out32(addr, 0x58000000); // SDR DIN P4(RSVD), Last
    addr = addr + 4;
    Xil_Out32(addr, 0x880000FF); // IDLE(FF), LAST (tFEAT)
    addr = addr + 4;

    FCL_swq_exec_req(ch, ce);
}
