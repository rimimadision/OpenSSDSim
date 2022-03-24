#ifndef _HCL_H
#define _HCL_H

#include"../config/config.h"
#include"../config/mem.h"
#include"../lib/type.h"

#define HCMD_NUM (256)

extern u32 cache_invalide_tag_flag;

typedef enum _hcmd_opcode
{
	HCE_READ = 1,
    HCE_WRITE,
    HCE_ERASE
}hcmd_opcode;

typedef enum _host_cmd_entry_status
{
    HCE_CHECK_CACHE, //core 0
	HCE_TO_SQ,    //core 0
    HCE_DATAMOVED,   //core 0polling ,core0 and core1 add
    HCE_FROM_CQ,     //core 1
	HCE_FINISH,
    HCE_WAIT_CQ,     //core0 add and core1 remove

    HCE_INIT         //core 0
} host_cmd_entry_status;

typedef struct _host_cmd_entry
{
#if (FTL_MODE == OBJECT_MODE)
	u32 obj_id;
	u32 obj_offset;
	u32 cur_lpn;
#else
    u32 nvme_start_lba; //from nvme cmd
    u32 nvme_req_num; //from nvme cmd +1

    u32 start_lpa;
    u32 cur_lpn;

    u32 mod_start_LA : 16;
    u32 mod_req_num : 16;
    u32 start_unalign_sqindex : 16;
    u32 end_unalign_sqindex : 16;
#endif

    u32 req_num;
    u32 cur_cnt;     //
    u32 cpl_cnt;     //
    u32 send_sq_cnt; //
    u32 buf_cnt;
    u32 nvme_dma_cpl;
    u32 nvme_cmd_slot; //

    hcmd_opcode op_code;
    host_cmd_entry_status status;
    u32 emu_id;
} host_cmd_entry;

u32 HCL_get_hcmd_entry_index(host_cmd_entry *hcmd_entry);
u32 HCL_init_host_cmd_entry_queue();
host_cmd_entry *HCL_get_hcmd_entry_addr(u32 hcmd_id);
host_cmd_entry *HCL_get_host_cmd_entry();

//u32 HCL_set_host_cmd_entry(host_cmd_entry **new_host_cmd_entry, u32 opcode, u32 startLA, u32 num, u32 cmdSlotTag);

u32 HCL_reclaim_hcmd_entry(host_cmd_entry *host_cmd_entry);
#endif
