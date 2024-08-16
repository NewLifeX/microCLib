#pragma once

#ifndef __MyList_H__
#define __MyList_H__

#include "Type.h"
#include "Debug.h"

#include "FreeRTOS.h"


#define MyListMalloc pvPortMalloc
#define MyListFree vPortFree

// 内部使用一些Cache，避免 malloc 太占 cpu 资源
// #define ListItemCacheSize 5

/*
	List结构
	单向链表，一个数据指针，一个结构体next指针。
	对比数据是否相等，采用钩子函数完成。isEquiFunc  多半函数提供此钩子
	删除数据项的时候提供其它动作的ListCallBack钩子，可以做数据项释放等操作。
	强烈建议  定义了List_t 后 ListInit（）初始化一下  避免拿到的内存有脏数据。
*/

struct MyListItem
{
	void*		Data;		// 链表数据项
	struct MyListItem* _next;	// 链表连接项
};
typedef struct MyListItem MyListItem_t;

typedef struct
{
	MyListItem_t* head;	// 头指针
	int count;			// 数据项计数
}MyList_t;

typedef bool(*isEquiFunc)(void* data1, void* data2);
typedef bool(*ListCallBack)(void* data);

void ListInit(MyList_t* list);

bool ListAdd(MyList_t* list, void* data, isEquiFunc isEqui);

void* ListFind(MyList_t* list, void* data, isEquiFunc isEqui);

bool ListRemove(MyList_t* list, void* data, isEquiFunc isEqui);

bool ListRemoveWithCallBack(MyList_t* list, void* data, isEquiFunc isEqui, ListCallBack callBack);

void* ListChange(MyList_t* list, void* src, void* dst, isEquiFunc isEqui);

// 纠结是否放出来使用
MyListItem_t* ListFindItem(MyList_t* list, void* data, isEquiFunc isEqui);


#endif // !__List_H__

