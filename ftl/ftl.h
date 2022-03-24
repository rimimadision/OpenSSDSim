#ifndef _FTL_H
#define _FTL_H

#include"../config/mem.h"
#include"../lib/type.h"
// #include "../fcl/fcl.h"
// #include "../hcl/hcl.h"

// #include "xil_io.h"

void FTL_core1_main();
// u32 FTL_CQ_polling();
void FTL_setup_core1();
void *FTL_core0_task_run();
void FTL_object_mode_run();


#endif
