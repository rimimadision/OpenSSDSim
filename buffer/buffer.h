#ifndef BUFFER_H
#define BUFFER_H

#include"../lib/type.h"
#include"../lib/list.h"
#include"../config/config.h"

#define HASH_TABLE_SIZE 1000
#define INVALID_ID (0xffffffff)

typedef struct _hash_table
{
	list_head hash_table[HASH_TABLE_SIZE];
}hash_table;

typedef struct _hcmd_buf_table
{
	list_head table_entry[256];
	u32       cnt[256];
	u32       cur_buf_id[256];
}hcmd_buf_table;



typedef struct _buffer_entry{
    u32 lpn;
    u32 buf_id;
    u32 dirty;
    u32 valid;

    list_node free_list_node;
    list_node lru_list_node;

    list_node hash_table_node;
    list_node hcmd_buf_list_node;

}buffer_entry;

typedef struct _buffer_list{
	buffer_entry buffer_entry[BUFFER_NUMBER];
}buffer_list;

buffer_list *buffer_list_ptr;

#define INVALID_BUFF_ID (0xffffffff)
#define INVALID_BUFF_INDEX (0xffffffff)


typedef struct _hcmd_buffer_entry{
	u32 start_entry;
	u32 end_index;
	u32 length;
	u32 cur_entry;
}hcmd_buffer_entry;



u32 L2P_get_hcmd_buff_cnt(u32 hcmd_id);
u32 L2P_hcmd_buf_get_lpn(u32 hcmd_id, u32 buf_offset);
void dump_free_buf_list();
void dump_lru_buf_list();
u32 L2P_hcmd_get_buffer(u32 hcmd_index, u32 offset);
u32 L2P_get_buffer_addr(u32 buff_id);
u32 L2P_get_hcmd_buff_cnt(u32 hcmd_index);
u32 L2P_allocate_buf(u32 lpn);
u32 L2P_check_buf_hit(u32 lpn);
u32 L2P_hcmd_free_buf(u32 hcmd_index);
void L2P_release_buffer(u32 index);
void L2P_hcmd_add_buffer(u32 hcmd_id, u32 buffer_id, u32 lpn);
void L2P_init_buffer();
void L2P_dump_lru_list();

#endif
