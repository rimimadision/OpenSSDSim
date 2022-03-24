#ifndef FTL_CORE1_H
#define FTL_CORE1_H

#include"../lib/type.h"
#include"../hcl/hcl.h"


#include"../config/mem.h"
#include"../lib/type.h"
#include "../fcl/fcl.h"
#include "../hcl/hcl.h"
#include "../emu/emu_config.h"
#ifndef EMU
#include "xil_io.h"
#else
#include "../emu/emu_io.h"
#endif

typedef struct _polling_status
{
	u32 polling_index;
	u32 polling_phase;
	u32 complete_num;
	u32 CQ_addr;
}polling_status;

void FTL_core1_main();
u32 FTL_CQ_polling();
void FTL_setup_core1();

#endif
