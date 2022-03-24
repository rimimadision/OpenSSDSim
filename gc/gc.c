// /*
// * gc.c
// */



// #include "gc.h"

// #include "../ftl/ftl_core0.h"

// //#include <string.h>
// #include "../emu/emu_log.h"
// #include "../l2p/l2p.h"

// extern u32 tmp_sq_index;

// u32 gc_counter = 100000;
// u32 used_page = TOTAL_PAGE;

// //u32 gc_counter = 0;
// //u32 used_page = 0;

// u32 flag = 0;
// const u32 up_time = 99999;

// block_list* block_list_ptr;
// channel_list* channel_list_ptr;
// // die_list *die_list_ptr;
// victim_block_list* victim_block_list_ptr;
// // free_block_list *free_block_list_ptr;
// // gc_sq_list* gc_sq_list_ptr;
// //gc_command_list* gc_command_list_ptr;

// gc_command_entry* gcmd_entry;


// void GC_print_gcmd(char *s, gc_command_entry *gcmd) {
// 	xil_printf("==================================\n");
// 	xil_printf("gcmd info : \n");
// 	xil_printf("%s\n",s);
// 	xil_printf("valid page index : %d\n",gcmd->gc_valid_page_index);
// 	xil_printf("valid page num : %d\n",gcmd->gc_valid_page_num);
// 	xil_printf("victim block num : %d\n",gcmd->gc_victim_block_num);
// 	xil_printf("locked : %d\n",gcmd->locked);
// 	xil_printf("op code : %d\n",gcmd->op_code);
// //	xil_printf("",gcmd->read_cpl_cnt);
// 	xil_printf("status : %d\n",gcmd->status);
// 	xil_printf("read cpl cnt : %d\n",gcmd->read_cpl_cnt);
// 	xil_printf("write cpl cnt : %d\n",gcmd->write_cpl_cnt);
// //	xil_printf("status : %d\n",gcmd->status);
// 	xil_printf("gcmd info print over!\n");
// 	xil_printf("----------------------------------\n");
// }

// /**********************************************************************************
// Func    Name: GC_task_run
// Descriptions: judge whether GC has started and run the gc process if so
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_task_run()
// {
//     // GC start or not?
//     // yes -> run the gc process
//     // no -> whether to start gc




// 	if (gc_counter%100000 == 0) {
// 		xil_printf("gc_counter : %d\n",gc_counter);
// 	}

//     if (flag == 0 && gc_counter >= up_time && used_page >= AVA_PAGE) {

//     	xil_printf("GC ready!\n");

//         flag = 1;
//         gc_counter = gc_counter%up_time;

//         u32 ch_no;
//         u32 block_no;
//         u32 valid_page_num;
//         gcmd_entry->status = GCE_GC_CMD;
//         gcmd_entry->op_code = GCE_READ;
//         GC_print_gcmd("gcmd ",gcmd_entry);
//         while (GC_get_victim_block(0, &ch_no, &block_no, &valid_page_num) == SUCCESS) {
// //            xil_printf("get victim block info : %d %d %d\n",ch_no,block_no,valid_page_num);

//             if (gcmd_entry->gc_valid_page_num + valid_page_num >= PAGE_PER_BLOCK) {
//                 break;
//             }

//             gcmd_entry->victim_block_ch[gcmd_entry->gc_victim_block_num] = ch_no;
//             gcmd_entry->victim_block_block[gcmd_entry->gc_victim_block_num] = block_no;
//             gcmd_entry->gc_victim_block_num++;


//             GC_remove_victim_block(ch_no, block_no);
//             gcmd_entry->gc_valid_page_num = gcmd_entry->gc_valid_page_num + valid_page_num;

// //            GC_print_gcmd("gc get victim block", gcmd_entry);

//             if (gcmd_entry->gc_victim_block_num >= PAGE_PER_BLOCK) {
//                 break;
//             }
//         }
// //        xil_printf("GC flag = 1\n");

//         GC_print_gcmd("gc ready over!",gcmd_entry);

//     } else if (flag == 1) {

// //    	xil_printf("GC entered\n");
// //    	GC_print_gcmd("gc flag = 1",gcmd_entry);

//         if (gcmd_entry->status == GCE_GC_CMD) {
//         	xil_printf("GC cmd\n");
// //        	GC_print_gcmd("gc cmd", gcmd_entry);
//         	GC_handle_cmd(gcmd_entry);
//         	xil_printf("gc cmd over\n");
//         } else if (gcmd_entry->status == GCE_GC_TO_SQ && gcmd_entry->locked == 0) {
//         	xil_printf("GC tosq\n");
// //        	GC_print_gcmd("gc tosq", gcmd_entry);
//         	GC_handle_to_sq(gcmd_entry);
//         	xil_printf("gc tosq over\n");
//         } else if (gcmd_entry->status == GCE_GC_FROM_CQ) {
//         	xil_printf("GC fromcq\n");
// //        	GC_print_gcmd("gc fromcq", gcmd_entry);
//         	GC_handle_from_cq(gcmd_entry);
//         	xil_printf("gc fromcq over\n");
//         } else if (gcmd_entry->status == GCE_GC_FINISH) {
//         	xil_printf("GC finish\n");
//         	GC_print_gcmd("gc finish", gcmd_entry);
//         	GC_handle_finish(gcmd_entry);
//         	xil_printf("gc finish over\n");
//         }
//     } else {
//     	gc_counter = (gc_counter + 1)%up_time;
//     }
// }


// /**********************************************************************************
// Func    Name: GC_init_hash_table
// Descriptions:
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_init_hash_table()
// {
// }


// /**********************************************************************************
// Func    Name: GC_init_block
// Descriptions: init block list
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_init_block()
// {
//     for (u32 j = 0; j < CH_PER_BOARD; j++) {
//         for (u32 i = 0; i < BLOCK_PER_CH; i++) {
//             block_list_ptr->block_enrty[j][i].invalid_pages_num = 0;
//             block_list_ptr->block_enrty[j][i].erase_num = 0;
//             block_list_ptr->block_enrty[j][i].age = 0;
//             block_list_ptr->block_enrty[j][i].block_state = NORMAL_BLOCK;
//             block_list_ptr->block_enrty[j][i].prev_block = BLOCK_PTR_NONE;
//             block_list_ptr->block_enrty[j][i].next_block = BLOCK_PTR_NONE;
//             block_list_ptr->block_enrty[j][i].current_page = 0;
//             block_list_ptr->block_enrty[j][i].locked = 0;
//         }
//     }
// }


// /**********************************************************************************
// Func    Name: GC_init_channel
// Descriptions: init die list
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_init_channel()
// {
//     // die_list_ptr = (ch_list *)(GC_DIE_MAP_BASE_ADDR);
//     for (u32 i = 0; i < CH_PER_BOARD; i++) {
//         channel_list_ptr->channel_entry[i].current_block = 0;
//         channel_list_ptr->channel_entry[i].free_block_num = 0;
//         channel_list_ptr->channel_entry[i].free_block_entry.head_block = BLOCK_PTR_NONE;
//         channel_list_ptr->channel_entry[i].free_block_entry.tail_block = BLOCK_PTR_NONE;

//         // die_list_ptr->die_entry[i].free_block_num = 0;
//         // die_list_ptr->die_entry[i].current_block = 0;
//         // die_list_ptr->die_entry[i].head_free_block = 0;
//         // die_list_ptr->die_entry[i].tail_free_block = 0;
//     }
// }


// /**********************************************************************************
// Func    Name: GC_init_victim_block
// Descriptions: init victim block list
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_init_victim_block()
// {

// 	for (u32 j = 0; j < CH_NUM; j++) {
// 		for (u32 i = 0; i < PAGE_PER_BLOCK + 1; i++) {
//             victim_block_list_ptr->victim_block_entry[j][i].head_block = BLOCK_PTR_NONE;
//             victim_block_list_ptr->victim_block_entry[j][i].tail_block = BLOCK_PTR_NONE;
//         }
//     }
// }


// /**********************************************************************************
// Func    Name: GC_init_free_block
// Descriptions: init free block list
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// // void GC_init_free_block()
// // {
// //     // free_block_list_ptr = (free_block_list *)(GC_FREE_BLOCK_BASE_ADDR);
// //     for (u32 i = 0; i < DIE_NUM; i++) {
// //         // free_block_list_ptr->free_block_entry[i].head_block = 0;
// //         // free_block_list_ptr->free_block_entry[i].tail_block = 0;
// //     }
// // }


// /**********************************************************************************
// Func    Name: GC_init_gc_sq
// Descriptions: init gc sq list
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// // void GC_init_gc_sq()
// // {
// //     for (u32 i = 0; i < MAX_GC_SQ_NUM; i++) {
// //         gc_sq_list_ptr->gc_sq_entry[i].start_lpa = 0;
// //         gc_sq_list_ptr->gc_sq_entry[i].cur_lpn = 0;

// //         gc_sq_list_ptr->gc_sq_entry[i].req_num = 0;
// //         gc_sq_list_ptr->gc_sq_entry[i].cur_cnt = 0;
// //         gc_sq_list_ptr->gc_sq_entry[i].cpl_cnt = 0;
// //         gc_sq_list_ptr->gc_sq_entry[i].send_sq_cnt = 0;
// //         gc_sq_list_ptr->gc_sq_entry[i].buf_cnt = 0;
// //         gc_sq_list_ptr->gc_sq_entry[i].nvme_dma_cpl = 0;
// //         gc_sq_list_ptr->gc_sq_entry[i].nvme_cmd_slot = 0;

// //         gc_sq_list_ptr->gc_sq_entry[i].gc_command_id = 0;

// //         gc_sq_list_ptr->gc_sq_entry[i].op_code = GCE_NONE;
// //         gc_sq_list_ptr->gc_sq_entry[i].status = GCE_GC_NONE;
// //     }
// // }


// /**********************************************************************************
// Func    Name: GC_init_gc_command
// Descriptions: init gc command list
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_init_gc_command()
// {

//     memset(gcmd_entry, 0, sizeof(gc_command_entry));
// //    GC_print_gcmd("gcmd init",gcmd_entry);
// //    memset(gc_command_list_ptr, 0, sizeof(gc_command_list_ptr));
//     // gcmd_opcode op_code;
//     // gcmd_entry_status status;

//     // u32 gc_victim_block_num : 16;
//     // u32 gc_valid_page_num : 16;
//     // u32 gc_valid_page_index : 16;

//     // u32 read_cpl_cnt : 16;
//     // u32 write_cpl_cnt : 16;

//     // u32 valid_page[PAGE_PER_BLOCK];
//     // u32 victim_block_ch[PAGE_PER_BLOCK];
//     // u32 victim_block_block[PAGE_PER_BLOCK];

//     // u32 rest_page_num : 11;

//     // for (u32 i = 0; i < MAX_GC_COMMAND_NUM; i++) {
//     //     // gc_command_list_ptr->gc_command_entry[i].head_gc_entry = GC_CMD_PTR_NONE;
//     //     // gc_command_list_ptr->gc_command_entry[i].tail_gc_entry = GC_CMD_PTR_NONE;

//     //     // gc_command_list_ptr->gc_command_entry[i].victim_block_num = 0;
//     //     // gc_command_list_ptr->gc_command_entry[i].target_block_num = 0;

//     //     // gc_command_list_ptr->gc_command_entry[i].rest_page_num = 0;

//     //     // memset(gc_command_list_ptr->gc_command_entry[i].victim_block_array, 0, PAGE_PER_BLOCK);
//     // }
// }


// /**********************************************************************************
// Func    Name: GC_init
// Descriptions: init the data structure related to GC
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_init()
// {
//     block_list_ptr = (block_list*)(GC_BLOCK_MAP_BASE_ADDR);
//     channel_list_ptr = (channel_list*)GC_CH_MAP_BASE_ADDR;
//     victim_block_list_ptr = (victim_block_list*)(GC_VICTIM_BLOCK_BASE_ADDR);
//     // gc_sq_list_ptr = (gc_sq_list*)(GC_SQ_LIST_ADDR);
// //    gc_command_list_ptr = (gc_command_list*)(GC_COMMAND_LIST_ADDR);
//     gcmd_entry = (gc_command_entry*)(GC_COMMAND_LIST_ENTRY);

// //    memset(gcmd_entry, 0, sizeof(gcmd_entry));

//     // GC_init_hash_table();
//     GC_init_block();
//     GC_init_channel();
//     GC_init_victim_block();
//     // GC_init_free_block();
// //    GC_init_gc_sq();
//     GC_init_gc_command();
// }


// /**********************************************************************************
// Func    Name: GC_gather_init
// Descriptions: post-process the data structure after l2p init completion related to GC
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_gather_init()
// {
//     for (u32 ch = 0; ch < CH_PER_BOARD; ch++) {
//         for (u32 block = 0; block < BLOCK_PER_CH; block++) {
//             if (block_list_ptr->block_enrty[ch][block].current_page == PAGE_PER_BLOCK) {
// //            	xil_printf("add victim block : %d %d %d\n",ch,block,block_list_ptr->block_enrty[ch][block].current_page);
//                 GC_add_victim_block(ch, block, block_list_ptr->block_enrty[ch][block].invalid_pages_num);
//                 // channel_list_ptr->channel_entry[ch].
//             }
//             // else if (block_list_ptr->block_enrty[ch][block].current_page == 0)
//             else {
// //            	xil_printf("add free block : %d %d %d\n",ch,block,block_list_ptr->block_enrty[ch][block].current_page);
//                 GC_add_free_block(ch, block);
//             }

// //            if (block % 1006 == 0) {
// //            	xil_printf("%d %d, ipn : %d\n",ch,block,block_list_ptr->block_enrty[ch][block].invalid_pages_num);
// //            }
//         }
//         if (channel_list_ptr->channel_entry[ch].free_block_entry.head_block == BLOCK_PTR_NONE) {
//             channel_list_ptr->channel_entry[ch].current_block = BLOCK_PTR_NONE;
//         } else {
//             channel_list_ptr->channel_entry[ch].current_block = channel_list_ptr->channel_entry[ch].free_block_entry.head_block;
//         }
//     }
// }



// /**********************************************************************************
// Func    Name: GC_gabage_collection
// Descriptions: start gabage collection
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_gabage_collection()
// {
//     // GC_get_victim_block(dieno);
//     // move data from victim block to DRAM on board
//     // GC_get_free_block(dieno);
//     // write date from DRAM to flash
//     // GC_erase_block(blockno);
//     // update the  map
// }


// /**********************************************************************************
// Func    Name: GC_get_victim_block
// Descriptions: get one victim block from victim_block_list
// Input   para: u32
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_get_victim_block(u32 _, u32* ret_ch, u32* ret_block, u32* ret_valid_page_num)
// {
//     // GC_get_dieno();
//     // choose one victim block in victim_block_list from large to small by invalid page num
//     // return blockno;

//     for (u32 invalid_page = PAGE_PER_BLOCK; invalid_page >= 0; invalid_page--) {
//         for (u32 iter = 0; iter < CH_PER_BOARD; iter++) {
//             u32 ch_no = GC_get_ch_no();
//             u32 block_no = victim_block_list_ptr->victim_block_entry[ch_no][invalid_page].head_block;
// //            xil_printf("ch = %d,%d %d %d ==\n",ch_no,block_no,BLOCK_PTR_NONE,invalid_page);
//             if (block_no != BLOCK_PTR_NONE) {
//                 *ret_ch = ch_no;
//                 *ret_block = block_no;
//                 *ret_valid_page_num = PAGE_PER_BLOCK - invalid_page;
// //                xil_printf("gc_get_victim_block, ch : %d, block : %d, vpn : %d\n",*ret_ch,*ret_block,*ret_valid_page_num);
//                 return SUCCESS;
//             }
//         }
//     }
//     return FAIL;
// }


// /**********************************************************************************
// Func    Name: GC_add_victim_block
// Descriptions: add one new victim block to victi_block_list
// Input   para: u32 ch_no, u32 block_no, u32 invalid_page_num
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_add_victim_block(u32 ch_no, u32 block_no, u32 invalid_page_num)
// {
//     // GC_repair_victim_list();
//     // add victim block by invalid page num
//     // return SUCCESS/FAIL;

//     victim_block_entry* vbe = &victim_block_list_ptr->victim_block_entry[ch_no][invalid_page_num];
//     if (vbe->tail_block == BLOCK_PTR_NONE) {
//         vbe->head_block = block_no;
//         vbe->tail_block = block_no;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         return SUCCESS;
//     } else {
//         u32 cur_block_no = vbe->tail_block;
//         block_list_ptr->block_enrty[ch_no][cur_block_no].next_block = block_no;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = cur_block_no;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_page_num].tail_block = block_no;
//         return SUCCESS;
//     }
//     return FAIL;
// }


// /**********************************************************************************
// Func    Name: GC_repair_victim_list
// Descriptions: repair victim_block_list because victim block invalid_page_num change
// Input   para: u32 ch_no, u32 block_no
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_repair_victim_list(u32 ch_no, u32 block_no)
// {
//     // unlink old victim block in victim_block_list

//     u32 invalid_pages_num = block_list_ptr->block_enrty[ch_no][block_no].invalid_pages_num;
//     u32 next_block = block_list_ptr->block_enrty[ch_no][block_no].next_block;
//     u32 prev_block = block_list_ptr->block_enrty[ch_no][block_no].prev_block;

//     if (prev_block == BLOCK_PTR_NONE && next_block == BLOCK_PTR_NONE) {
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].head_block = BLOCK_PTR_NONE;
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].tail_block = BLOCK_PTR_NONE;

//     } else if (prev_block == BLOCK_PTR_NONE && next_block != BLOCK_PTR_NONE) {
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].head_block = next_block;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//     } else if (prev_block != BLOCK_PTR_NONE && next_block == BLOCK_PTR_NONE) {
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].tail_block = prev_block;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//     } else if (prev_block != BLOCK_PTR_NONE && next_block != BLOCK_PTR_NONE) {
//         block_list_ptr->block_enrty[ch_no][prev_block].next_block = next_block;
//         block_list_ptr->block_enrty[ch_no][next_block].prev_block = prev_block;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//     }
//     // return FAIL;
//     return SUCCESS;
// }


// /**********************************************************************************
// Func    Name: GC_remove_victim_block
// Descriptions: remove one victim block from victim block list
// Input   para: u32 ch_no, u32 block_no
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_remove_victim_block(u32 ch_no, u32 block_no)
// {
//     // u32 cur_block_no = channel_list_ptr->channel_entry[ch_no].free_block_entry.head_block;
//     u32 invalid_pages_num = block_list_ptr->block_enrty[ch_no][block_no].invalid_pages_num;
//     u32 next_block = block_list_ptr->block_enrty[ch_no][block_no].next_block;
//     u32 prev_block = block_list_ptr->block_enrty[ch_no][block_no].prev_block;

//     if (prev_block == BLOCK_PTR_NONE && next_block == BLOCK_PTR_NONE) {
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].head_block = BLOCK_PTR_NONE;
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].tail_block = BLOCK_PTR_NONE;
//     } else if (prev_block == BLOCK_PTR_NONE && next_block != BLOCK_PTR_NONE) {
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].head_block = next_block;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][next_block].prev_block = BLOCK_PTR_NONE;
//     } else if (prev_block != BLOCK_PTR_NONE && next_block == BLOCK_PTR_NONE) {
//         victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].tail_block = prev_block;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][prev_block].next_block = BLOCK_PTR_NONE;
//     } else if (prev_block != BLOCK_PTR_NONE && next_block != BLOCK_PTR_NONE) {
//         block_list_ptr->block_enrty[ch_no][prev_block].next_block = next_block;
//         block_list_ptr->block_enrty[ch_no][next_block].prev_block = prev_block;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//     }
//     // return FAIL;
//     return SUCCESS;
// }


// /**********************************************************************************
// Func    Name: GC_get_free_page
// Descriptions: get one free page from free_block_list
// Input   para: u32 ch_no
// In&Out  Para:
// Output  para:
// Return value: u32 ppn_index
// ***********************************************************************************/
// u32 GC_get_free_page()
// {
//     for (u32 iter = 0; iter < CH_PER_BOARD; iter++) {
//         u32 ch_no = GC_get_ch_no();
//         u32 cur_block_no = channel_list_ptr->channel_entry[ch_no].current_block;

//         if (cur_block_no == BLOCK_PTR_NONE) {
//         	continue;
//         }

//         u32 cur_page = block_list_ptr->block_enrty[ch_no][cur_block_no].current_page;
//         block_list_ptr->block_enrty[ch_no][cur_block_no].current_page++;

//         if (cur_page == PAGE_PER_BLOCK) {
//             GC_remove_free_block(ch_no, cur_block_no);
//             GC_add_victim_block(ch_no, cur_block_no, block_list_ptr->block_enrty[ch_no][cur_block_no].invalid_pages_num);
//             channel_list_ptr->channel_entry[ch_no].current_block = channel_list_ptr->channel_entry[ch_no].free_block_entry.head_block;
// //            continue;
//         }

//         u32 ret_ppn = GC_cal_ppn_index(ch_no, cur_block_no, cur_page);
// //        xil_printf("gc_get_free_page, ppn_index : %d\n",ret_ppn);
//         used_page = used_page + 1;
//         return ret_ppn;
//     }
// }


// /**********************************************************************************
// Func    Name: GC_add_free_block
// Descriptions: add one new free block to free_block_list
// Input   para: u32 ch_no, u32 block_no
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_add_free_block(u32 ch_no, u32 block_no)
// {
//     if (channel_list_ptr->channel_entry[ch_no].free_block_entry.tail_block != BLOCK_PTR_NONE) {
//         u32 cur_block_no = channel_list_ptr->channel_entry[ch_no].free_block_entry.tail_block;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = cur_block_no;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][cur_block_no].next_block = block_no;
//         channel_list_ptr->channel_entry[ch_no].free_block_entry.tail_block = block_no;
//         // channel_list_ptr->channel_entry[ch_no].
//         return SUCCESS;
//     } else {
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         channel_list_ptr->channel_entry[ch_no].free_block_entry.head_block = block_no;
//         channel_list_ptr->channel_entry[ch_no].free_block_entry.tail_block = block_no;
//         return SUCCESS;
//     }
//     return FAIL;
// }


// /**********************************************************************************
// Func    Name: GC_remove_free_block
// Descriptions: remove one block from free block list
// Input   para: u32 ch_no, u32 block_no
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_remove_free_block(u32 ch_no, u32 block_no)
// {
//     // u32 invalid_pages_num = block_list_ptr->block_enrty[ch_no][block_no].invalid_pages_num;
//     u32 next_block = block_list_ptr->block_enrty[ch_no][block_no].next_block;
//     u32 prev_block = block_list_ptr->block_enrty[ch_no][block_no].prev_block;

//     if (prev_block == BLOCK_PTR_NONE && next_block == BLOCK_PTR_NONE) {
//         channel_list_ptr->channel_entry[ch_no].free_block_entry.head_block = BLOCK_PTR_NONE;
//         channel_list_ptr->channel_entry[ch_no].free_block_entry.tail_block = BLOCK_PTR_NONE;
//         // victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].head_block = BLOCK_PTR_NONE;
//         // victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].tail_block = BLOCK_PTR_NONE;

//     } else if (prev_block == BLOCK_PTR_NONE && next_block != BLOCK_PTR_NONE) {
//         channel_list_ptr->channel_entry[ch_no].free_block_entry.head_block = next_block;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][next_block].prev_block = BLOCK_PTR_NONE;
//         // victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].head_block = next_block;
//         // block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//     } else if (prev_block != BLOCK_PTR_NONE && next_block == BLOCK_PTR_NONE) {
//         channel_list_ptr->channel_entry[ch_no].free_block_entry.tail_block = prev_block;
//         // victim_block_list_ptr->victim_block_entry[ch_no][invalid_pages_num].tail_block = prev_block;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][prev_block].next_block = BLOCK_PTR_NONE;
//     } else if (prev_block != BLOCK_PTR_NONE && next_block != BLOCK_PTR_NONE) {
//         block_list_ptr->block_enrty[ch_no][prev_block].next_block = next_block;
//         block_list_ptr->block_enrty[ch_no][next_block].prev_block = prev_block;
//         block_list_ptr->block_enrty[ch_no][block_no].next_block = BLOCK_PTR_NONE;
//         block_list_ptr->block_enrty[ch_no][block_no].prev_block = BLOCK_PTR_NONE;
//     }
//     // return FAIL;
//     return SUCCESS;
// }



// /**********************************************************************************
// Func    Name: GC_erase_block
// Descriptions: erase specific block by {ch,ce,die,plain,block}
// Input   para: u32 ch_no, u32 block_no
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// void GC_erase_block(u32 ch_no, u32 block_no)
// {
//     // generate one erase SQ
//     // FCL perform the request

//     u32 ch, ce, lun, plane, block;
//     u32 sq_id = INVALID_INDEX;
//     phy_page_addr phyaddr;
//     u32 hcmd_id = 0;
//     u32 buff_id = 0;
//     u32 ret = FAIL;
//     memset(&phyaddr, 0, sizeof(nand_flash_addr_cmd));

//     while (sq_id == INVALID_INDEX) {
//         sq_id = FCL_get_free_SQ_entry(0);
//     }

//     // u32 ppn_index = ch_no;
//     // ch 4, ce 8, lun/die 1, plane 2, block 1006, page 384 or 1152
//     u32 ce_div = 1 * 2 * 1006;
//     u32 lun_div = ce_div / 1;
//     u32 plane_div = lun_div / 2;
//     u32 block_div = plane_div / 1006;

//     ch = ch_no;
//     ce = block_no / ce_div;
//     lun = block_no % ce_div / lun_div;
//     plane = block_no % ce_div % lun_div / plane_div;
//     block = block_no % ce_div % lun_div % plane_div / block_div;

//     phyaddr.nand_flash_addr.tlc_mode.lun = lun;
//     phyaddr.nand_flash_addr.tlc_mode.plane = plane;
//     phyaddr.nand_flash_addr.tlc_mode.block = block;

//     // FCL_set_SQ_entry(hcmd_id, sq_id, buff_id, 3, &phyaddr);
//     FCL_set_SQ_entry(hcmd_id, sq_id, buff_id, HCE_ERASE, &phyaddr, HCMD_SPACE);

//     while (ret == FAIL) {
//         //TO DO ADD TIME OUT CHECK
//         ret = FCL_send_SQ_entry(sq_id, ch, ce);
//     }

// }


// /**********************************************************************************
// Func    Name: GC_command_2_sq
// Descriptions: turn the command to several sub requests
// Input   para: gc_command_entry gc_command
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_command_2_sq(gc_command_entry gc_command)
// {
//     // one gc_command maybe contain more than one page size of data,
//     // split this command to many sub requests by page size
//     // GC_set_sq()
// }


// /**********************************************************************************
// Func    Name: GC_set_sq
// Descriptions: set the gc_sq and its struct, and then use it for FCL
// Input   para: gc_sq_entry gc_sq
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// // void GC_set_sq(gc_sq_entry gc_sq)
// // {
// //     // pack one sq entry by the info obtained
// // }


// /**********************************************************************************
// Func    Name: GC_send_sq_2_fcl
// Descriptions: send sq to fcl, used by GC
// Input   para: gc_sq_entry gc_sq
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// // void GC_send_sq_2_fcl(gc_sq_entry gc_sq)
// // {
// //     // send sq to fifo queue
// //     // then use it for FCL
// // }


// /**********************************************************************************
// Func    Name: GC_get_ch_no
// Descriptions: switch the ch number func(get_free_block) used
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: u32 ch_no
// ***********************************************************************************/
// u32 GC_get_ch_no()
// {
//     static u8 ch_no = 0;

//     u8 ret_ch_no;
//     ret_ch_no = ch_no;

//     ch_no = (ch_no + 1) % CH_NUM;
//     return ret_ch_no;
// }

// /**********************************************************************************
// Func    Name: GC_cal_ppn_2_lpn
// Descriptions:
// Input   para:
// In&Out  Para:
// Output  para:
// Return value: u32 lpn
// ***********************************************************************************/
// u32 GC_cal_ppn_2_lpn(u32 ppn_index)
// {
//     // u32 cur_lpn = 0;
//     // u32 cur_ppn = 0;

//     //load_64(&lpn_ppn_map, ppa, HCE_READ);

//     if (lpn_ppn[ppn_index] == ppn_index) {
//         return ppn_index;
//     }

//     for (u32 i = ppn_index; i >= 0; i--) {
//         if (lpn_ppn[i] == ppn_index) {
//             return i;
//         }
//     }

//     for (u32 i = ppn_index+1; i < TOTAL_PAGE; i++) {
// 		if (lpn_ppn[i] == ppn_index) {
// 			return i;
// 		}
// 	}

//     return -1;
// }


// /**********************************************************************************
// Func    Name: GC_command_entry_listen
// Descriptions: check the gc_command status and choose fit func to handle gc_command
// Input   para: gcmd_entry_status gcmd_entry
// In&Out  Para:
// Output  para:
// Return value: none
// ***********************************************************************************/
// void GC_command_entry_listen(gcmd_entry_status gcmd_entry)
// {
//     // traverse gc_command_queue and check the status
//     // change gc_command to new status by current status 
// }


// /**********************************************************************************
// Func    Name: GC_handle_cmd
// Descriptions: handle gc_command of cmd staus
// Input   para: gcmd_entry_status gcmd_entry
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_handle_cmd(gc_command_entry* gcmd_entry)
// {
//     // gc_command 2 gc_sq

//     // u32 page_index = 0;
//     u32 valid_page_num = gcmd_entry->gc_valid_page_num;
//     u32 victim_block_num = gcmd_entry->gc_victim_block_num;
//     for (u32 iter = 0; iter < victim_block_num; iter++) {
//     	u32 st_ppn_index = GC_cal_ppn_index(gcmd_entry->victim_block_ch[iter], gcmd_entry->victim_block_block[iter], 0);
// //        u32 st_ppn_index = gcmd_entry->victim_block_ch[iter] * BLOCKS_PER_CHANNEL * PAGE_PER_BLOCK
// //            + gcmd_entry->victim_block_block[iter] * PAGE_PER_BLOCK;
//         for (u32 iterr = 0; iterr < PAGE_PER_BLOCK; iterr++) {
//             u32 cur_ppn_index = st_ppn_index + iterr;
//             u8 page_state = ppn_state[cur_ppn_index];
//             if (page_state == V) {
//                 gcmd_entry->valid_page[gcmd_entry->gc_valid_page_index] = cur_ppn_index;
//                 gcmd_entry->gc_valid_page_index++;
//             }
//         }
//     }
//     gcmd_entry->status = GCE_GC_TO_SQ;
//     gcmd_entry->op_code = GCE_READ;
//     return SUCCESS;
// }


// /**********************************************************************************
// Func    Name: GC_handle_to_sq
// Descriptions: handle gc_command of tosq status
// Input   para: gcmd_entry_status gcmd_entry
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_handle_to_sq(gc_command_entry* gcmd_entry)
// {
//     // send read/write sq 2 FCL

//     // u32 buff_index_read = DATA_BUFFER_ADDR_GC_READ;
//     // u32 buff_index_write = DATA_BUFFER_ADDR_GC_WRITE;
//     phy_page_addr ppa;
//     // u32 cur_lpn;
//     u32 ppn_index;
//     u32 buff_index = DATA_BUFFER_ADDR_GC_READ;

//     u32 valid_page_index = gcmd_entry->gc_valid_page_index;

//     for (u32 i = 0; i < valid_page_index; i++) {
// //    	xil_printf("cur : %d/%d\n",i,valid_page_num);
//         ppn_index = gcmd_entry->valid_page[i];
//         // cur_lpn = GC_cal_ppn_2_lpn(ppn_index);
//         L2P_calc_ppa(ppn_index, &ppa);

// #if (CORE_MODE == SINGLE_CORE_MODE)
//         tmp_sq_index++;
//         tmp_sq_index = tmp_sq_index % 256;
// #else
//         tmp_sq_index = FCL_get_free_SQ_entry(ppa.ch);
// #endif

//         // buffer_entry* buff = search_hash_table_entry(cur_lpn);
//         // buff->start_buffer_align = BE_ALIGN_0;
//         // buff->end_buffer_align = BE_ALIGN_3;

//         u32 cur_buff_index = buff_index + i * PAGE_SIZE;

//         if (gcmd_entry->op_code == GCE_READ) {
//             FCL_set_SQ_entry(0xff, tmp_sq_index, cur_buff_index, GCE_READ, &ppa, GCMD_SPACE);
//         } else if (gcmd_entry->op_code == GCE_WRITE) {
//             u32 cur_target_page = GC_get_free_page();
//             // u32 cur_target_page;
//             gcmd_entry->target_page[i] = cur_target_page;
//             L2P_calc_ppa(cur_target_page, &ppa);
//             FCL_set_SQ_entry(0xff, tmp_sq_index, cur_buff_index, GCE_WRITE, &ppa, GCMD_SPACE);
//         }

//         u32 ret = FAIL;
//         do {
//             ret = FCL_send_SQ_entry(tmp_sq_index, ppa.ch, ppa.ce);
//         } while (ret == FAIL);
//     }

//     gcmd_entry->locked = 1;
// }


// /**********************************************************************************
// Func    Name: GC_handle_from_cq
// Descriptions: handle gc_command of fromcq status
// Input   para: gcmd_entry_status gcmd_entry
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_handle_from_cq(gc_command_entry* gcmd_entry)
// {
//     // change status of gc_command 2 finish or tosq(write sq)

//     if (gcmd_entry->op_code == GCE_READ) {
//         gcmd_entry->op_code = GCE_WRITE;
//         gcmd_entry->status = GCE_GC_TO_SQ;
//         gcmd_entry->locked = 0;
//     } else if (gcmd_entry->op_code == GCE_WRITE) {
//         gcmd_entry->status = GCE_GC_FINISH;
//     }
// }


// /**********************************************************************************
// Func    Name: GC_handle_finish
// Descriptions: gc post-process
// Input   para: gcmd_entry_status gcmd_entry
// In&Out  Para:
// Output  para:
// Return value: SUCCESS or FAIL
// ***********************************************************************************/
// u32 GC_handle_finish(gc_command_entry* gcmd_entry)
// {
//     // update the mapping tables
//     // erase victim block
//     // remove free block from free_block_list
//     // complete this gc_command

// 	// xil_printf("gc finish 1\n");

//     u32 valid_page_num = gcmd_entry->gc_valid_page_index;
//     u32 ppn_index;
//     phy_page_addr ppa;

//     // xil_printf("gc finish 2\n");

//     for (u32 i = 0; i < gcmd_entry->gc_victim_block_num; i++) {
//         u32 ch_no = gcmd_entry->victim_block_ch[i];
//         u32 block_no = gcmd_entry->victim_block_block[i];

//         u32 st_page_index = GC_cal_ppn_index(ch_no,block_no,0);

//         for (u32 offset = 0; offset < PAGE_PER_BLOCK; offset++) {
//             u32 cur_ppn = st_page_index + offset;
//             u32 src_lpn = GC_cal_ppn_2_lpn(ppn_index);
//             ppn_state[lpn_ppn[src_lpn]] = F;
//             lpn_ppn[src_lpn] = -1;
//             // ppn_state[out_ppn] = V;
//         }

//         GC_remove_victim_block(ch_no, block_no);
// //        GC_erase_block(ch_no, block_no);
//         FTL_syn_erase_block(GC_cal_ppn_index(ch_no,block_no,0));
//         GC_add_free_block(ch_no, block_no);
//     }

//     // xil_printf("gc finish 3\n");

//     for (u32 i = 0; i < valid_page_num; i++) {
// //    	xil_printf("finish 3~4 : %d/%d\n",i,valid_page_num);
//         ppn_index = gcmd_entry->valid_page[i];
//         L2P_calc_ppa(ppn_index, &ppa);

//         u32 src_lpn = GC_cal_ppn_2_lpn(ppn_index);
//         u32 out_ppn = gcmd_entry->target_page[i];
//         u32 block_no = ppn_index / PAGE_PER_BLOCK % BLOCK_PER_CH;

//         // *lpn_ppn_scheme[src_lpn] = 0;
//         ppn_state[lpn_ppn[src_lpn]] = F;
//         lpn_ppn[src_lpn] = out_ppn;
//         ppn_state[out_ppn] = V;
//     }
//     flag = 0;
//     gc_counter = 0;
//     used_page = 0;
//     memset(gcmd_entry, 0, sizeof(gc_command_entry));
//     // xil_printf("gc finish 4\n");
// }


// /**********************************************************************************
// Func    Name: GC_cal_ppn_index
// Descriptions: cal ppn index
// Input   para: u32 ch_no, u32 block_no, u32 page_no
// In&Out  Para:
// Output  para:
// Return value: u32 ppn_index
// ***********************************************************************************/
// u32 GC_cal_ppn_index(u32 ch_no, u32 block_no, u32 page_no)
// {
//     return ch_no * PAGE_PER_CH + block_no * PAGE_PER_BLOCK + page_no;
// }


// /*
// functions to be pending, these may be used

// u32 GC_move_data_2_newblock(u32 old_block_addr, u32 data_length);
// void GC_get_gc_sq_4_cq(u32 sq_index);
// */
