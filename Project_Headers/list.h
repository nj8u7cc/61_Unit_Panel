#ifndef __H_LISTHEAD__
  #define __H_LISTHEAD__

#include	"typedefine.h"

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
 
// ��ͷ
typedef volatile struct list_head 
{
	volatile struct list_head *next, *prev;
}list_head;												// ����Ľṹ�����2�� ָ��ͬ���ͽṹ��� ָ��

	
/*
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define  list_entry(ptr, type, member) \
	( (type *)( (char *)(ptr) - (unsigned long)(&((type *)0)->member) ) )

// ���� ͷ ��ʼ��
#define LIST_HEAD_INIT(name)  { &(name), &(name) }

// define name and then initial 
#define LIST_HEAD(name) \
	volatile struct list_head name = LIST_HEAD_INIT(name)	
// ���� list_head���͵Ľṹ�� name ֵ���� &(name), &(name)  ������ṹ�����2��Ԫ�ض�ָ���Լ�����


// ��ʼ������ͷ  �������Ĳ����� list_head���͵Ľṹ��ָ�� �����ָ��ָ��Ľṹ���2��Ԫ�ظ�ֵΪ�Լ��Ľṹ���׵�ַ
void INIT_LIST_HEAD(volatile struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

// ����һ���ڵ�
void __list_add(volatile struct list_head *new, volatile struct list_head *prev,volatile struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/*
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
void list_add(volatile struct list_head *new, volatile struct list_head *head)
{
	__list_add(new, head, head->next);
}

// �� prev��next2���ṹ��������  ȥ�������м������
void __list_del(volatile struct list_head * prev, volatile struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}



/*
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
void list_del(volatile struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}


/*
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
 
// ����һ�������Ƿ�Ϊ�� �����������һ��ָ��ָ������Լ�����ʾ����
_UBYTE list_empty(volatile struct list_head *head)
{
	return head->next == head;						// ˫�Ⱥ�  �����
}


/*
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for( pos=(head)->next; pos!=(head); pos=pos->next )

/*
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)


#endif
