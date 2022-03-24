#include"list.h"

void list_add(list_node *new, list_node *prev, list_node *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

void list_delete(list_node *node)
{
	//xil_printf("list delete 1\n");
	if(node == NULL)
	{
		// xil_printf("list delete 2\n");
		// xil_printf("xxx \n");
		return;
	}

	if((node->next == NULL)||(node->prev == NULL))
	{
		// xil_printf("list delete 3\n");
		// xil_printf("list delete has a bug \n");
		return;
	}
	// xil_printf("list delete 4\n");
	node->prev->next = node->next;
	node->next->prev = node->prev;

	// xil_printf("list delete 5\n");
	node->prev = NULL;
	node->next = NULL;
}


u32 list_empty(list_head *head)
{
	if(head->next == head)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void list_delete_tail(list_head *head)
{
	list_delete(head->prev);
}

void list_delete_head(list_head *head)
{
	list_delete(head->next);
}

void list_move_tail_to_head(list_head *head)
{
	list_move_head(head->prev,head);
}

void list_move_head(list_node *node, list_head *head)
{
	node->next->prev = node->prev;
	node->prev->next = node->next;

	list_add(node, head, head->next);
}

void list_add_tail(list_head *new, list_node *head)
{
	list_add(new, head->prev, head);
}

void list_add_head(list_node *new, list_node *head)
{
	list_add(new, head, head->next);
}

void dd()
{
}
