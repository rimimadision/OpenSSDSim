#ifndef FTL_CORE0_H
#define FTL_CORE0_H

#include "../lib/type.h"
#include "../hcl/hcl.h"
// #include "../fcl/fcl.h"

extern u32 tmp_sq_index;

u32 FTL_handle_datamove(host_cmd_entry *hcmd_entry);
u32 FTL_handle_tosq(host_cmd_entry *hcmd_entry);
u32 FTL_handle_checkcache(host_cmd_entry *hcmd_entry);
u32 FTL_handle_fromcq(host_cmd_entry *hcmd_entry);
u32 FTL_handle_finish(host_cmd_entry *hcmd_entry);

#endif
