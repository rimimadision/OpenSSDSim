#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "../lib/type.h"
#include "fe/fe.h"
#include "emu_log.h"
#include "../ftl/ftl.h"
#include "../hcl/hcl.h"
#include "../ftl/ftl_taskqueue.h"
#include "../l2p/l2p.h"
#include "../lib/lock.h"
#include "../buffer/buffer.h"
#include "../gc/gc.h"
#include "../fcl/fcl.h"
#include "be/be.h"

/* mem_base of simulated DRAM */
u32 mem_base;
void EMU_init_ftl_moudles()
{
     /* init moudules for FTL */
    L2P_init_buffer();
	HCL_init_host_cmd_entry_queue();
	FTL_int_task_queue();
	FCL_init_flash_controller();
	FCL_init_hw_queue();
	// FTL_setup_core1();
	init_all_spin_lock();
// //	FTL_erase_init();
	// GC_init();
	L2P_init();
	// GC_gather_init();
}

int main()
{
    EMU_log_println(LOG, "SSDEmu begin");
    /* alloc 1 GB memory aligned 16KB to simulate DRAM */
    mem_base = (u32)malloc(1 * GB + 16 * KB);
    mem_base = ((((u32)(mem_base)) >> 14) << 14) + 16 * KB;
    memset((void *)mem_base, 0, 1 * GB);

    EMU_init_ftl_moudles();
    FE_init();
    BE_init();
    
    EMU_log_println(LOG, "Emu init done");
    
    pthread_t fe_pid, ftl_pid, be_pid;
    pthread_create(&fe_pid, NULL, fe, NULL);
    pthread_create(&ftl_pid, NULL, FTL_core0_task_run, NULL);
    pthread_create(&be_pid, NULL, be, NULL);
    pthread_join(fe_pid, NULL);
    pthread_join(ftl_pid, NULL);
    pthread_join(be_pid, NULL);

    return 0;
}