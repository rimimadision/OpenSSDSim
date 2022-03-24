#include "buffer.h"

#include "../config/config.h"
#include "../config/mem.h"
#include "../lib/dprint.h"

hash_table *hash_table_ptr;
hcmd_buf_table *hcmd_buf_table_ptr;

u32 get_hash_code(u32 key);
buffer_entry  *search_hash_table_entry(u32 key);
void init_hash_table();
void init_hcmd_buf_table();
void insert_hash_table_entry(u32 key, buffer_entry *be);
void delete_hash_table_entry(u32 key);


list_head free_buf_list_head = {&free_buf_list_head, &free_buf_list_head};
list_head lru_buf_list_head = {&lru_buf_list_head, &lru_buf_list_head};

/**********************************************************************************
Func    Name: L2P_init_buffer
Descriptions: None
Input   para: None
In&Out  Para: None
Output  para: None
Return value: None
***********************************************************************************/
void L2P_init_buffer()
{
#if (BYPASS_BUFFER == YES)

#else

	// init hash table
	init_hash_table();

	// init hcmd buf table
	init_hcmd_buf_table();

	// init buffer space
	buffer_list_ptr = (buffer_list *)(BUFFER_ENTRY_BASE_ADDR);
	for(u32 i = 0; i < BUFFER_NUMBER; i++)
	{
		buffer_list_ptr->buffer_entry[i].lpn = 0xffffffff;
		buffer_list_ptr->buffer_entry[i].buf_id = i;
		buffer_list_ptr->buffer_entry[i].free_list_node.next = NULL;
		buffer_list_ptr->buffer_entry[i].free_list_node.prev = NULL;
		buffer_list_ptr->buffer_entry[i].lru_list_node.next = NULL;
		buffer_list_ptr->buffer_entry[i].lru_list_node.prev = NULL;
		buffer_list_ptr->buffer_entry[i].hash_table_node.next = NULL;
		buffer_list_ptr->buffer_entry[i].hash_table_node.prev = NULL;
	}

	// init free buffer list
	for(u32 i = 0; i < BUFFER_NUMBER; i++)
	{
		list_add_tail(&buffer_list_ptr->buffer_entry[i].free_list_node, &free_buf_list_head);
	}

#endif
}

u32 L2P_check_buf_hit(u32 lpn)
{
#if (BYPASS_BUFFER == YES)

	return INVALID_ID;
#else
	buffer_entry *buf;
	// check hash table, hash table's entries equal lru list's entries
	buf = search_hash_table_entry(lpn);

	//xil_printf("   %d\n", lpn);

	if(buf == NULL)
	{
		//xil_printf("not hello %d \n", lpn);
		return INVALID_ID;
	}
	else
	{
		// change lru list
		//xil_printf("hello %d \n", lpn);
		//list_move_head(&buf->lru_list_node,&lru_buf_list_head);
		return buf->buf_id;
	}
#endif
}


u32 L2P_allocate_buf(u32 lpn)
{
#if (BYPASS_BUFFER == YES)
	static u32 ret_buf_id = 0;
	ret_buf_id++;
	ret_buf_id = ret_buf_id%BUFFER_NUMBER;

	return ret_buf_id;
#else
//	xil_printf("l2p allocate buffer 1\n");
	u32 ret = 0xffffffff;
	buffer_entry *buf;

	if(TRUE == list_empty(&free_buf_list_head))
	{
//		xil_printf("l2p allocate buffer 2\n");
		//xil_printf("[L2P_allocate_buf]: free list is empty %d \n", lpn);
		buf = container_of((&lru_buf_list_head)->prev, typeof(*buf), lru_list_node);
//		xil_printf("l2p allocate buffer 3\n");
		delete_hash_table_entry(buf->lpn);
		//xil_printf("delete_hash_table_entry(buf->lpn) \n");

//		xil_printf("l2p allocate buffer 4\n");
		buf->lpn = lpn;
		insert_hash_table_entry(lpn,buf);

//		xil_printf("l2p allocate buffer 5\n");
		list_delete_tail(&lru_buf_list_head);
//		xil_printf("l2p allocate buffer 6\n");
		list_add_head(&buf->lru_list_node, &lru_buf_list_head);

	}
	else
	{
		//xil_printf("[L2P_allocate_buf]: free list is not empty %d \n", lpn);
		// xil_printf("l2p allocate buffer 7\n");
		buf = container_of((&free_buf_list_head)->next, typeof(*buf), free_list_node);
		// xil_printf("l2p allocate buffer 8\n");
		list_delete_head(&free_buf_list_head);
		//xil_printf("list_delete_head(&free_buf_list_head) \n");

		// xil_printf("l2p allocate buffer 9\n");
		list_add_head(&buf->lru_list_node, &lru_buf_list_head);
		// xil_printf("l2p allocate buffer 10\n");
		insert_hash_table_entry(lpn,buf);
	}

	  buf->lpn = lpn;
	  ret = buf->buf_id;

	  xil_printf("l2p allocate buffer 11\n");
	  return ret;

#endif
}

void dump_free_buf_list()
{
	buffer_entry *buf;

	list_for_each_entry(buf, &free_buf_list_head, free_list_node)
	{
		xil_printf("dump_free_buf_list, buf_id is %d \n", buf->buf_id);
	}
}

void dump_lru_buf_list()
{
	buffer_entry *buf;

	list_for_each_entry(buf, &lru_buf_list_head, lru_list_node)
	{
		xil_printf("dump_lru_buf_list, buf_id is %d \n", buf->buf_id);
	}
}



/**********************************************************************************
Func    Name: L2P_hcmd_get_buffer
Descriptions:
Input   para: buff_id
In&Out  Para: None
Output  para: None
Return value: buffer鐨勭墿鐞嗗湴鍧�
***********************************************************************************/
u32 L2P_get_buffer_addr(u32 buff_id)
{
	u32 buffer_address = buff_id * 16 * 1024 + BUFFER_BASE_ADDR;
	return buffer_address;
}


u32 get_hash_code(u32 key)
{
    return key%HASH_TABLE_SIZE;
}

void init_hash_table()
{
	hash_table_ptr = (hash_table *)(HASH_TABLE_BASE_ADDR);

	for(u32 i = 0; i < HASH_TABLE_SIZE; i++)
	{
		hash_table_ptr->hash_table[i].next = &hash_table_ptr->hash_table[i];
		hash_table_ptr->hash_table[i].prev = &hash_table_ptr->hash_table[i];
	}
}

void insert_hash_table_entry(u32 key, buffer_entry *be)
{
	u32 hash_value = get_hash_code(key);
	list_add_tail(&be->hash_table_node, &hash_table_ptr->hash_table[hash_value]);
}

buffer_entry  *search_hash_table_entry(u32 key)
{
	buffer_entry *buf;

	u32 hash_value = get_hash_code(key);
	list_for_each_entry(buf, &hash_table_ptr->hash_table[hash_value], hash_table_node)
	{
		if(buf->lpn == key)
		{
			return buf;
		}
	}

	return NULL;
}

void delete_hash_table_entry(u32 key)
{
	buffer_entry *buf;
	u32 hash_value = get_hash_code(key);

	list_for_each_entry(buf, &hash_table_ptr->hash_table[hash_value], hash_table_node)
	{
		if(buf->lpn == key)
		{
			list_delete(&buf->hash_table_node);
			//xil_printf("list_delete(&buf->hash_table_node) %d \n", buf->buf_id);
			return;
		}
	}
}


void init_hcmd_buf_table()
{
	hcmd_buf_table_ptr = (hcmd_buf_table *)(HCMD_BUFFER_TABLE_BASE_ADDR);

	for(u32 i = 0; i < 256; i++)
	{
		hcmd_buf_table_ptr->table_entry[i].next = &hcmd_buf_table_ptr->table_entry[i];
		hcmd_buf_table_ptr->table_entry[i].prev = &hcmd_buf_table_ptr->table_entry[i];
		hcmd_buf_table_ptr->cnt[i] = 0 ;
		hcmd_buf_table_ptr->cur_buf_id[i] = 0xffffffff;
	}
}

u32 L2P_get_hcmd_buff_cnt(u32 hcmd_id)
{
#if (BYPASS_BUFFER == YES)

	return 1;
#else
	return hcmd_buf_table_ptr->cnt[hcmd_id];
#endif
}


void L2P_hcmd_add_buffer(u32 hcmd_id, u32 buffer_id, u32 lpn)
{
#if (BYPASS_BUFFER == YES)
	static u32 ret_buf_id = 0;
	ret_buf_id++;
	ret_buf_id = ret_buf_id%BUFFER_NUMBER;

#else
//	xil_printf("l2p hcmd add buffer 1\n");
	buffer_entry *buf = &buffer_list_ptr->buffer_entry[buffer_id];
	buf->lpn = lpn;

//	xil_printf("l2p hcmd add buffer 2\n");
	list_add_tail(&buf->hcmd_buf_list_node, &hcmd_buf_table_ptr->table_entry[hcmd_id]);
	hcmd_buf_table_ptr->cnt[hcmd_id]++;
#endif
}

u32 L2P_hcmd_buf_get_lpn(u32 hcmd_id, u32 buf_offset)
{
#if (BYPASS_BUFFER == YES)
	static u32 ret_lpn = 0;
	ret_lpn++;

	return ret_lpn;

#else
	buffer_entry *buf;
	u32 tmp_cnt = 0;

	list_for_each_entry(buf, &hcmd_buf_table_ptr->table_entry[hcmd_id], hcmd_buf_list_node)
	{
		if(buf_offset <= tmp_cnt)
		{
			return buf->lpn;
		}
		tmp_cnt++;
	}
#endif
}

u32 L2P_hcmd_get_buffer(u32 hcmd_index, u32 offset)
{
#if (BYPASS_BUFFER == YES)
	static u32 ret_buf_id = 0;
	ret_buf_id++;
	ret_buf_id = ret_buf_id%BUFFER_NUMBER;

	return ret_buf_id;

#else
	if(offset >= hcmd_buf_table_ptr->cnt[hcmd_index])
	{
		xil_printf("error!! \n");
	}

	buffer_entry *buf;
	u32 tmp_cnt = 0;

	list_for_each_entry(buf, &hcmd_buf_table_ptr->table_entry[hcmd_index], hcmd_buf_list_node)
	{
		if(offset <= tmp_cnt)
		{
			return buf->buf_id;
		}
		tmp_cnt++;
	}
#endif
}

u32 L2P_hcmd_free_buf(u32 hcmd_index)
{
#if (BYPASS_BUFFER == YES)

#else
	while(FALSE == list_empty(&hcmd_buf_table_ptr->table_entry[hcmd_index]))
	{
		list_delete_tail(&hcmd_buf_table_ptr->table_entry[hcmd_index]);
	}

	hcmd_buf_table_ptr->cnt[hcmd_index] = 0;
#endif
}
