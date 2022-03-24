#include "ftl_core1.h"

#include "../lib/dprint.h"
#include "../lib/lock.h"
#include "../buffer/buffer.h"

#include "../ftl/ftl_taskqueue.h"
#include "../gc/gc.h"
#include "../emu/be/be.h"
#include <stdlib.h>

/**********************************************************************************
Func    Name: FTL_core1_main
Descriptions: core1ִ�е�ѭ��
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FTL_core1_main()
{
#ifndef EMU
	init_core1_mmu();

//	xil_printf("ftl_core1_init_mmu, ok!\n");
#endif
	while (1)
	{
		//		xil_printf("ftl_cq_polling, ok!\n");
		FTL_CQ_polling();
		//		xil_printf("ftl_cq_polling, end!\n");
		// FTL_core1task_polling();
	}
}

/**********************************************************************************
Func    Name: FTL_setup_core1
Descriptions: ����core1
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void FTL_setup_core1()
{
#ifndef EMU
	__asm__ __volatile__(
		"ldr	r0,=0x41 \n"
		"mcr	p15,0,r0,c1,c0,1 \n"
		"dsb \n"
		"isb \n"
		:
		:
		: "cc");

	u32 core1_start_address = (u32)FTL_core1_main;
	Xil_Out32(0xfffffff0, core1_start_address);
	__asm__("sev");
	dmb();
#endif
}

/**********************************************************************************
Func    Name: FTL_CQ_polling
Descriptions: polling CQ
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
u32 FTL_CQ_polling()
{
#ifdef EMU
	static int c = 0;
	u32 ret = FAIL;
	pthread_mutex_lock(&fcl_be_mutex);
	// emu_log_println(LOG, "CQ here");
	list_node *next_node;

	for (list_node *head_node = complete_list.next; head_node != &complete_list; head_node = complete_list.next)
	{
		ret = SUCCESS;
		EMU_sq *sq = container_of(head_node, EMU_sq, node);
		FCL_free_SQ_entry(sq->ch, sq->sq_index);
		host_cmd_entry *hcmd_entry = HCL_get_hcmd_entry_addr(sq->sq_entry.hcmd_index);
		EMU_log_println(DEBUG, "complete sq of hcmd %u", hcmd_entry->emu_id);
		hcmd_entry->cpl_cnt++;
		if (hcmd_entry->cpl_cnt == hcmd_entry->req_num)
		{
			FTL_sendhcmd(hcmd_entry, HCE_FROM_CQ);
		}

		list_delete_head(&complete_list);
		free(sq);
	}

	pthread_mutex_unlock(&fcl_be_mutex);

	return SUCCESS;
#endif
}
