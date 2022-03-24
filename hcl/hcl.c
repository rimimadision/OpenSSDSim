#include"hcl.h"

#include"../config/mem.h"
#include"../lib/type.h"
#include"../lib/dprint.h"
#include "../emu/emu_config.h"
#ifdef EMU
#include "../emu/emu_log.h"
#include<string.h>
#else
#include"xil_printf.h"
#endif

#define DEBUG_TEST
static host_cmd_entry *hcmd_queue = NULL;
static u32 *hcmd_map = NULL;
/**********************************************************************************
Func    Name: HCL_init_host_cmd_entry_queue
Descriptions: 缂佹獘ost cmd entry queue閸掑棝鍘ょ粚娲？楠炶泛鍨垫慨瀣host cmd entry queue 256娑撶尃ntry
Input   para: 
In&Out  Para: 
Output  para: 
Return value: 鏉╂柨娲朣UCCESS鐞涖劎銇氶幋鎰閿涘瓗AIL鐞涖劎銇氭径杈Еsucce
***********************************************************************************/
u32 HCL_init_host_cmd_entry_queue()
{
    hcmd_queue = (host_cmd_entry *)HCMD_BASE_ADDR;
    hcmd_map = (u32 *)(HCMD_BASE_ADDR + sizeof(host_cmd_entry) * HCMD_NUM);
    memset(hcmd_queue, 0, sizeof(host_cmd_entry) * HCMD_NUM);
    memset(hcmd_map, 0, HCMD_NUM / sizeof(u8));
    return SUCCESS;
}

u32 HCL_hcmd_queue_empty()
{
    for (u32 i = 0; i < HCMD_NUM / 32; i++)
    {
        for (u32 j = 0; j < 32; j++)
        {
            if (((hcmd_map[i] & (1 << j)) >> j) == 0)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/**********************************************************************************
Func    Name: HCL_get_host_cmd_entry
Descriptions: 鏉╂柨娲栨稉锟芥稉鐚ee 閻樿埖锟戒胶娈慼ost cmd entry
Input   para: 
In&Out  Para: 
Output  para: 
Return value: 鏉╂柨娲杊cmd閹稿洭鎷＄悰銊с仛閹存劕濮涢敍瀛║LL鐞涖劎銇氭径杈Е
***********************************************************************************/
host_cmd_entry *HCL_get_host_cmd_entry()
{
	// static u32 hcmd_index = 0;
	// hcmd_index++;
	// hcmd_index = hcmd_index%HCMD_NUM;

    u32 i, j, index;
    index = -1;
    for (i = 0; i < HCMD_NUM / 32; i++)
    {
        for (j = 0; j < 32; j++)
        {
            if (((hcmd_map[i] & (1 << j)) >> j) == 0)
            {
                index = i * 32 + j;
                hcmd_map[i] |= (1 << j);
                goto find_index;
            }
        }
    }
find_index:
    if ((index != -1) || (index < HCMD_NUM))
    {
        DEBUG_PRINTF("[HCL_get_host_cmd_entry]index %d\n", index);
        return &hcmd_queue[index];
    }
    else
    {
        //#ifdef DEBUG_TEST
        DEBUG_PRINTF("[HCL_get_host_cmd_entry] Do not find index\n");
        //#endif
        return NULL;
    }
}

/**********************************************************************************
Func    Name: HCL_set_host_cmd_entry
Descriptions: 鐠佸墽鐤唄ost cmd entry
Input   para: 
In&Out  Para: 
Output  para: 
Return value: 鏉╂柨娲朣UCCESS鐞涖劎銇氶幋鎰閿涘瓗AIL鐞涖劎銇氭径杈Е
***********************************************************************************/
u32 HCL_set_host_cmd_entry(host_cmd_entry **new_host_cmd_entry, u32 opcode, u32 nvme_startLA, u32 nvme_num, u32 cmdSlotTag)
{
#if (FTL_MODE == OBJECT_MODE)
#else
    return SUCCESS;
#endif
}

/**********************************************************************************
Func    Name: HCL_get_hcmd_entry_index
Descriptions: 鏉╂柨娲杊ost cmd entry 閻ㄥ埇ndex
Input   para: 
In&Out  Para: 
Output  para: 
Return value: 鏉╂柨娲杋ndex
***********************************************************************************/
u32 HCL_get_hcmd_entry_index(host_cmd_entry *hcmd_entry)
{

    u32 index = ((u32)hcmd_entry - HCMD_BASE_ADDR) / sizeof(host_cmd_entry);
    return index;
}

/**********************************************************************************
Func    Name: HCL_reclaim_hcmd_entry
Descriptions: 閸ョ偞鏁筯ost cmd entry
Input   para: 
In&Out  Para: 
Output  para: 
Return value: 鏉╂柨娲朣UCCESS鐞涖劎銇氶幋鎰閿涘瓗AIL鐞涖劎銇氭径杈Е
***********************************************************************************/
u32 HCL_reclaim_hcmd_entry(host_cmd_entry *hcmd_entry)
{
    u32 i, j, bit;
    u32 index = ((u32)hcmd_entry - HCMD_BASE_ADDR) / sizeof(host_cmd_entry);
    i = index / 32;
    j = index % 32;
    bit = 1 << j;

    hcmd_map[i] = hcmd_map[i] & (~bit);
    memset(hcmd_entry, 0, sizeof(host_cmd_entry));
    return SUCCESS;
}

/**********************************************************************************
Func    Name: HCL_get_hcmd_entry_addr
Descriptions: 鏉╂柨娲杊ost  index閻ㄥ嫬婀撮崸锟�
Input   para: 
In&Out  Para: 
Output  para: 
Return value: 鏉╂柨娲杊ost  index閻ㄥ嫬婀撮崸锟�
***********************************************************************************/
host_cmd_entry *HCL_get_hcmd_entry_addr(u32 hcmd_id)
{

    return &hcmd_queue[hcmd_id];
}
