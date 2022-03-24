#ifndef CONFIG_H
#define CONFIG_H

#define OBJECT_MODE (1)
#define NORMAL_MODE (2)
#define FTL_MODE    (NORMAL_MODE)

#define NORMAL_CHANNEL {1,1,1,1}
#define TOTAL_CHANNEL  (4)

#define SINGLE_CORE_MODE (1)
#define MULTI_CORE_MODE  (2)
#define CORE_MODE    (MULTI_CORE_MODE)

#define YES   (1)
#define NO    (0)
#define BYPASS_BUFFER  (NO)

#define PAGE_PER_BLOCK_SLC_MODE  (384)

#define PAGE_PER_OBJECT (4*8*2*384)
#define BLOCK_PER_OBJECT (4*8*2)
#define BLOCK_PER_PLANE  (1006)
#define USED     0
#define UNUSED   1

#define HW_QUEUE_ENTRY_NUMBER   (256)
#define INVALID_VALUE        (0xffffffff)
#define INVALID_ID           (0xffffffff)

#define PAGE_SIZE (4 * 4096)
#define TU_SIZE (4096)

#define PAGE_PER_BLOCK (384)

#define BLOCK_PER_CH (8*2*1006)
#define BLOCK_PER_CE (2*1006)
#define BLOCK_PER_LUN (2*1006)
//#define BLOCK_PER_PLANE (1006)

#define PAGE_PER_CH (PAGE_PER_BLOCK * BLOCK_PER_CH)
#define PAGE_PER_CE (PAGE_PER_BLOCK * BLOCK_PER_CE)
#define PAGE_PER_LUN (PAGE_PER_BLOCK * BLOCK_PER_LUN)
#define PAGE_PER_PLANE (PAGE_PER_BLOCK * BLOCK_PER_PLANE)

#define PLANE_PER_LUN (2)
#define PLANE_PER_CE (2)
#define LUN_PER_CE (1)
#define CE_PER_CH (8)
#define CH_PER_BOARD (4)
#define PAGE_PER_CHANNEL (PAGE_PER_BLOCK * BLOCK_PER_CE * CE_PER_CH)

#define TOTAL_BOARD (1)
#define TOTAL_CH (TOTAL_BOARD * CH_PER_BOARD)
#define TOTAL_CE (TOTAL_CH * CE_PER_CH)
#define TOTAL_LUN (TOTAL_CE * LUN_PER_CE)
#define TOTAL_PLANE (TOTAL_LUN * PLANE_PER_LUN)
#define TOTAL_BLOCK (TOTAL_PLANE * BLOCK_PER_PLANE)
#define TOTAL_PAGE (TOTAL_BLOCK * PAGE_PER_BLOCK)

#define F	0
#define V	1
#define I	2

//ROW means each plane select one block
#define PAGE_PER_ROW (PAGE_PER_BLOCK * PLANE_PER_LUN * LUN_PER_CE * CE_PER_CH * TOTAL_BOARD * CH_PER_BOARD)

//column means each plane select 3 page
#define PAGE_PER_COLUMN (3 * PLANE_PER_LUN)
// buffer define
#define BUFFER_NUMBER (1500)

// hardware define
#define HW_QUEUE_DEPTH   (256)
#define INVALID_INDEX   (0xffffffff)


/*
    gc related
*/
//#define TOTAL_PAGE_SLC (4*8*2*1006*384)
//#define TOTAL_PAGE_TLC (4*8*2*1006*1152)
// #define TOTAL_PAGE (TOTAL_SLC)

#define AVA_PAGE (TOTAL_PAGE - PAGE_PER_CHANNEL)
#define PAGES_PER_BLOCK_SLC (384)
#define PAGES_PER_BLOCK_TLC (1152)

#define DIE_NUM (4*8*1)
#define CH_NUM (4)

#define DATA_BUFFER_ADDR_GC_READ (GC_DATA_BUFFER_BASE_ADDR)
// #define DATA_BUFFER_ADDR_GC_WRITE (DATA_BUFFER_ADDR_GC_READ + PAGE_PER_BLOCK * PAGE_SIZE)

//#define BLOCKS_PER_DIE (BLOCK_PER_PLANE*PLANE_PER_LUN)
//#define BLOCKS_PER_CHANNEL (8*BLOCKS_PER_DIE)
//#define PAGES_PER_BLOCK (PAGE_PER_BLOCK)

#define DATA_BUFFER_SIZE_GC_SLC (PAGES_PER_BLOCK_SLC*16*1024)
#define DATA_BUFFER_SIZE_GC_TLC (PAGES_PER_BLOCK_MLC*16*1024)

// block state
#define NORMAL_BLOCK (0)
#define BAD_BLOCK (1)

// block list state
#define NONE_BLOCK (0xffff)
#define BLOCK_PTR_NONE (0xffff)

// GC command
#define MAX_GC_SQ_NUM (1152*10)
#define MAX_GC_COMMAND_NUM (10)
#define GC_CMD_PTR_NONE (0xffff)



#endif
