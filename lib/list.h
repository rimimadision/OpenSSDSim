#ifndef LIST_H
#define LIST_H

#include"type.h"
#include <stddef.h>
typedef struct _list_node
{
	struct _list_node *prev;
	struct _list_node *next;
}list_node, list_head;

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define list_for_each_entry(pos, head, member) \
    for (pos = container_of((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = container_of(pos->member.next, typeof(*pos), member))


void list_delete(list_node *node);
u32 list_empty(list_head *head);

void list_delete_tail(list_head *head);
void list_delete_head(list_head *head);

void list_move_head(list_node *node, list_head *head);
void list_move_tail_to_head(list_head *head);

void list_add_tail(list_head *head, list_node *new);
void list_add_head(list_head *new, list_node *head);


#endif
