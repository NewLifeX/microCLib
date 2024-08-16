
#include "MyList.h"

// #define ListPrintf		DebugPrintf
#define ListPrintf(...)

/*
后期优化措施，减少malloc  free   使用C# List策略    批量申请，自动扩容。
另外  如果重要数据，上层做处理，定期写入Flash然后 再读出，避免内存泄漏！！！
*/

void ListInit(MyList_t* list)
{
	list->head = NULL;
	list->count = 0;
}

bool ListAdd(MyList_t* list, void* data, isEquiFunc isEqui)
{
	if ((list->head) == NULL)
	{
		MyListItem_t* item = (MyListItem_t*)MyListMalloc(sizeof(MyListItem_t));
		if (!item)return false;
		ListPrintf("ListAdd Item %08X : %08X\r\n", item, data);
		item->Data = data;
		item->_next = NULL;
		list->count = 1;

		list->head = item;
		return true;
	}
	else
	{
		if (isEqui != NULL)
		{
			if (ListFindItem(list, data, isEqui) != NULL)
			{
				ListPrintf("ListAdd this data exist.\r\n");
				return true;
			}
		}
		MyListItem_t* item = (MyListItem_t*)MyListMalloc(sizeof(MyListItem_t));
		if (!item)return false;
		ListPrintf("ListAdd Item %08X : %08X\r\n", item, data);
		item->Data = data;
		item->_next = list->head;

		list->count++;
		list->head = item;
		return true;
	}
}

void* ListFind(MyList_t* list, void* data, isEquiFunc isEqui)
{
	MyListItem_t* item = list->head;
	while (1)
	{
		if (item == NULL)break;

		if (isEqui(item->Data, data))break;

		item = item->_next;
	}
	if (item != NULL) return item->Data;
	return NULL;
}

MyListItem_t* ListFindItem(MyList_t* list, void* data, isEquiFunc isEqui)
{
	MyListItem_t* item = list->head;
	while (1)
	{
		if (item == NULL)break;

		if (isEqui(item->Data, data))break;

		item = item->_next;
	}
	return item;
}

bool ListRemove(MyList_t* list, void* data, isEquiFunc isEqui)
{
	MyListItem_t* pre = list->head;
	MyListItem_t* item = list->head;
	while (1)
	{
		if (item == NULL)break;

		if (isEqui(item->Data, data))
		{
			if (pre == item)
				list->head = item->_next;
			else
				pre->_next = item->_next;

			list->count--;
			ListPrintf("ListRemove Item %08X : %08X\r\n", item, item->Data);
			MyListFree(item);
			return true;
		}
		pre = item;
		item = item->_next;
	}
	return false;
}

bool ListRemoveWithCallBack(MyList_t* list, void* data, isEquiFunc isEqui, ListCallBack callBack)
{
	MyListItem_t* pre = list->head;
	MyListItem_t* item = list->head;
	while (1)
	{
		if (item == NULL)break;

		if (isEqui(item->Data, data))
		{
			if (pre == item)
				list->head = item->_next;
			else
				pre->_next = item->_next;

			list->count--;
			ListPrintf("ListRemove Item %08X : %08X  &  callback:\r\n", item, item->Data);
			callBack(item->Data);
			MyListFree(item);		// while内有判断  此处item不为NULL
			return true;
		}
		pre = item;
		item = item->_next;
	}

	return false;
}

void* ListChange(MyList_t* list, void* src, void* dst, isEquiFunc isEqui)
{
	// if ((*head) == NULL)return NULL;

	MyListItem_t* item = list->head;
	while (1)
	{
		if (item == NULL)break;;

		if (isEqui(item->Data, src))
		{
			ListPrintf("ListChange Item %08X   %08X -> %08X\r\n", item, src, dst);
			item->Data = dst;
			break;
		}
		item = item->_next;
	}
	if (item == NULL)
	{
		ListPrintf("ListChange Fail,src exist\r\n");
		return NULL;
	}

	return item->Data;
}
