// /*
// * gc.h
// */

// #include "../config/config.h"
// #include "../config/mem.h"
// #include "../hcl/hcl.h"
// // #include "../lib/type.h"
// #include "../fcl/fcl.h"
// #include "../lib/type.h"
// // #include "../fcl/fcl.h"
// #include "../l2p/l2p.h"

// //#include"../lib/type.h"
// //#include"../fcl/fcl.h"
// //#include"../config/config.h"
// //#include"../config/mem.h"


// #include <string.h>

// typedef struct _block_entry {
//     u32 invalid_pages_num : 9; //
//     u32 erase_num : 16;
//     u32 age : 16;
//     u32 block_state : 2;
//     u32 prev_block : 16;
//     u32 next_block : 16;
//     u32 current_page : 16;
//     u32 locked : 1;
// }block_entry;


// typedef struct _block_list {
//     block_entry block_enrty[CH_NUM][BLOCK_PER_CH];
// }block_list;


// typedef struct _free_block_entry {
//     u32 head_block : 16;
//     u32 tail_block : 16;
// }free_block_entry;


// // typedef struct _die_entry {
// // 	u32 free_block_num:16;
// // 	u32 current_block:16;

// // 	free_block_entry free_block_entry;
// // }die_entry;


// // typedef struct _die_list {
// // 	die_entry die_entry[DIE_NUM];
// // }die_list;

// typedef struct _channel_entry {
//     u32 free_block_num : 16;
//     u32 current_block : 16;

//     free_block_entry free_block_entry;
// }channel_entry;


// typedef struct _channel_list {
//     channel_entry channel_entry[CH_NUM];
// }channel_list;



// typedef struct _victim_block_entry {
//     u32 head_block:16;
//     u32 tail_block:16;
// }victim_block_entry;


// typedef struct _victim_block_list {
//     victim_block_entry victim_block_entry[CH_NUM][PAGE_PER_BLOCK + 1];
// }victim_block_list;


// // typedef struct _free_block_list {
// // 	free_block_entry free_block_entry[DIE_NUM];
// // }free_block_list;


// typedef enum _gcmd_opcode
// {
//     GCE_READ = 4,
//     GCE_WRITE,
//     GCE_NONE
// }gcmd_opcode;

// typedef enum _gcmd_entry_status
// {
//     GCE_GC_CMD, //core 0
//     GCE_GC_TO_SQ,    //core 0
//     GCE_GC_FROM_CQ,     //core 0
//     GCE_GC_FINISH,
//     GCE_GC_NONE
// } gcmd_entry_status;

// // typedef struct _gc_sq_entry {
// //     u32 start_lpa;
// //     u32 cur_lpn;

// //     u32 req_num;	//request buffer number
// //     u32 cur_cnt;     //current alloc buffer count
// //     u32 cpl_cnt;     //current return cq entry count
// //     u32 send_sq_cnt; // already sended sq number
// //     u32 buf_cnt;   // buffer count
// //     u32 nvme_dma_cpl;   //nvme dma  complition  transfer number
// //     u32 nvme_cmd_slot; //when nvme  dma transfer use, one host cmd one slot

// //     u32 gc_command_id;

// //     gcmd_opcode op_code;
// //     gcmd_entry_status status;
// // }gc_sq_entry;


// // typedef struct _gc_sq_list {
// //     gc_sq_entry gc_sq_entry[MAX_GC_SQ_NUM];
// // }gc_sq_list;

// typedef struct _gc_command_enrty {
//     u32 locked;

//     u32 gc_victim_block_num; // max : 384
//     u32 gc_valid_page_num; // max : 384
//     u32 gc_valid_page_index; // max : 384

//     u32 read_cpl_cnt;
//     u32 write_cpl_cnt;

//     u32 valid_page[PAGE_PER_BLOCK];
//     u32 victim_block_ch[PAGE_PER_BLOCK];
//     u32 victim_block_block[PAGE_PER_BLOCK];
//     u32 target_page[PAGE_PER_BLOCK];

//     gcmd_opcode op_code;
//     gcmd_entry_status status;
// //    u32 rest_page_num : 11;
// }gc_command_entry;


// typedef struct _gc_command_list {
//     gc_command_entry gc_command_entry[MAX_GC_COMMAND_NUM];
// }gc_command_list;


// extern block_list* block_list_ptr;
// extern channel_list* channel_list_ptr;
// // die_list *die_list_ptr;
// extern victim_block_list* victim_block_list_ptr;
// // free_block_list *free_block_list_ptr;
// // extern gc_sq_list* gc_sq_list_ptr;
// extern gc_command_list* gc_command_list_ptr;
// extern gc_command_entry* gcmd_entry;

// extern u32 used_page;

// void GC_task_run();

// void GC_init_hash_table();

// void GC_init_block();

// void GC_init_channel();

// void GC_init_victim_block();

// // void GC_init_free_block();

// //void GC_init_gc_sq();

// void GC_init_gc_command();

// void GC_init();

// void GC_gather_init();

// void GC_gabage_collection();

// u32 GC_get_victim_block(u32 ch_no, u32* ret_ch, u32* ret_block, u32* ret_valid_page_num);

// u32 GC_add_victim_block(u32 ch_no, u32 block_no, u32 invalid_page_num);

// u32 GC_repair_victim_list(u32 ch_no, u32 block_no);

// u32 GC_remove_victim_block(u32 ch_no, u32 block_no);

// // u32 GC_get_free_block(u32 ch_no);

// u32 GC_get_free_page();

// u32 GC_add_free_block(u32 ch_no, u32 block_no);

// u32 GC_remove_free_block(u32 ch_no, u32 block_no);

// void GC_erase_block(u32 ch_no, u32 block_no);

// void GC_command_2_sq(gc_command_entry gc_command);

// // void GC_set_sq(gc_sq_entry gc_sq);

// // void GC_send_sq_2_fcl(gc_sq_entry gc_sq);

// u32 GC_get_ch_no();

// u32 GC_cal_ppn_2_lpn(u32 ppn_index);

// void GC_command_entry_listen(gcmd_entry_status gcmd_entry);

// u32 GC_handle_cmd(gc_command_entry* gcmd_entry);

// u32 GC_handle_to_sq(gc_command_entry* gcmd_entry);

// u32 GC_handle_from_cq(gc_command_entry* gcmd_entry);

// u32 GC_handle_finish(gc_command_entry* gcmd_entry);

// u32 GC_cal_ppn_index(u32 ch_no, u32 block_no, u32 page_no);

// // u32 GC_move_data_2_newblock(u32 old_block_addr, u32 data_length);
// // void GC_get_gc_sq_4_cq(u32 sq_index);
