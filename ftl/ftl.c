#include"ftl.h"
#include "../emu/emu_log.h"
#include"ftl_taskqueue.h"
#include"ftl_nvme.h"
#include "ftl_core1.h"
// #include"../config/config.h"
// #include"../config/mem.h"
// #include"../lib/dprint.h"
// #include"../lib/lock.h"
// #include"../fcl/fcl.h"
// #include"../hcl/hcl.h"
// #include"../nvme/host_lld.h"
// #include"../gc/gc.h"

// #include"xil_cache.h"
// #include"xil_mmu.h"

void *FTL_core0_task_run()
{
	xil_printf("ftl_core0_task_run start!\n");
	int cnt = 0;
	while(1)
	{
//		cnt++;
//		if (cnt%100000 == 0)
//			xil_printf("ftl nvme req polling!\n");
		FTL_nvme_req_polling();

//		if (cnt%100000 == 0)
//			xil_printf("ftl core0 task polling!\n");
		FTL_core0task_polling();

//		if (cnt%100000 == 0)
//			xil_printf("gc task run!\n");
		// GC_task_run();

		FTL_CQ_polling();
	}
}

void FTL_core1_task_run()
{
	while(1)
	{
		// FTL_CQ_polling();
	}
}
