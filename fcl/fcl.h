#ifndef FCL_H
#define FCL_H
#include "../emu/emu_config.h"
#ifdef EMU
#include "../emu/emu_io.h"
#else
#include "xil_io.h"
#endif
#include"../lib/type.h"
#include"../hcl/hcl.h"
#include"../config/config.h"
#include"../config/mem.h"
#include"../buffer/buffer.h"
#include"../config/reg.h"

void FCL_set_fifo(u32 index,u32 ch,u32 ce);

typedef enum _cmd_space {
	HCMD_SPACE,
	GCMD_SPACE
}cmd_space;

typedef union  _hw_queue_entry{
	struct{
		u32 cur_ptr   :  8;   // entry's index
		u32 t0_add_da   :1;	 // DW0 8    - SLC
		u32 t1_add_df   :1;	 // DW0 9    - TLC
		u32 t2_use_1a   :1; 	// DW0 10   - TLC One Pass
        u32             :5;     // DW0 15:11
		u32 err_sum     :9;	 // DW0 24:16 - ErrSum
        u32             :1;     // DW0 25    - Reserved
		u32 ph          :2;     // DW0 27:26
		u32 op          :2;     // DW0 29:29
//         u32             :1;     // Reserved
		u32	cmd_space   :1;  // DW0:30 - cmd sapce {host cmd, gc cmd}
		u32 nulptr      :1;

	    u32 ce       :  8;   // ce's index
	    u32          :  6;
	    u32 buf_adr  : 18;   // buffer's address

	    u32 a0       :  8;   // for pa
	    u32 a1       :  8;   // for pa
	    u32 a2       :  8;   // for pa
	    u32 a3       :  8;   // for pa

	    u32 a4       :  8;   // for pa
	    u32 a5       :  8;   // for pa
	    u32 hcmd_index : 8;
		u32 next_ptr   :  8;   // next entry
	};
	u32 array[4];
}hw_queue_entry;

typedef union _nand_flash_addr_cmd{
    struct {
        u32 page_offset   :15;   // size: 16384 + 2048
        u32               :1;
        u32 page          :11;   // 1152 pages per block
        u32 plane         :1;    // 2 planes per lun
        u32 block         :10;   // 1006 blocks per plane
        u32               :1;
        u32 lun           :3;    // lun
        u32               :6;
    }tlc_mode;
    struct {
		u32 page_offset   :15;   // size: 16384 + 2048
		u32               :1;
		u32 page          :9;   // 1152 pages per block
		u32               :2;
		u32 plane         :1;    // 2 planes per lun
		u32 block         :10;   // 1006 blocks per plane
		u32               :1;
		u32 lun           :3;    // lun
		u32               :6;
	}slc_mode;
    u8 array[6];
}nand_flash_addr_cmd;

typedef struct _phy_page_addr
{
	u16 ch;
	u16 ce;
	nand_flash_addr_cmd nand_flash_addr;
}phy_page_addr;


typedef struct _hw_queue{
	hw_queue_entry hw_queue[HW_QUEUE_DEPTH];
}hw_queue;

void FCL_set_hw_base_addr(u32 ch, u32 sq_addr, u32 cq_addr);
void FCL_init_hw_queue();
u32  FCL_get_free_SQ_entry(u32 ch);
// void FCL_set_SQ_entry(u32 hcmd_entry_index, u32 SQ_entry_index, u32 buffer_index , u32 opcode ,phy_page_addr * ppa);
void FCL_set_SQ_entry(u32 hcmd_entry_index, u32 SQ_entry_index, u32 buffer_index, u32 opcode, phy_page_addr* ppa, u32 cmd_sp);
u32 FCL_send_SQ_entry(u32 index,u32 ch,u32 ce);
void FCL_free_SQ_entry(u32 ch, u32 index);
void FCL_init_flash_controller();
u32 FCL_SQ_empty(u32 ch);

void FCL_set_timing(void);
void FCL_nand_reset(u32 ch, u32 ce);

u32 FCL_get_rbn(u32 ch, u32 ce, u32 is_ddr);
void FCL_wait_rbn (u32 ch, u32 ce, u32 is_ddr);

void FCL_set_mode(u32 mode);
void FCL_set_feature_ddr(u32 ch, u32 ce);
void FCL_swq_exec_req(u32 ch, u32 ce);


void FCL_reset_ch(void);
void FCL_release_ch(void);

#endif
