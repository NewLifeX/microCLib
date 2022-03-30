
#pragma once

#include "Type.h"

// cJSON Types:
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6
// 自定义字符串，不需要添加双引号
#define cJSON_BareString 7
#define cJSON_IsReference 256

// The cJSON structure
typedef struct cJSON
{
	// 双向链表
	// next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem.
	struct cJSON* next, * prev;
	// 节点项
	// An array or object item will have a child pointer pointing to a chain of the items in the array/object. 
	struct cJSON* child;

	// 类型
	// The type of the item, as above.
	int type;

	// 8字节的Value，可以承载所有类型了。
	byte Value[8];


	// 字符串值
	// The item's string, if type==cJSON_String
	char* valuestring;
	// 数值类型值
	// The item's number, if type==cJSON_Number 
	int valueint;
	// 浮点数值类型
	// The item's number, if type==cJSON_Number 
	double valuedouble;
	// 名称
	// The item's name string, if this item is the child of, or is in the list of subitems of an object.
	char* string;
} cJSON;

typedef struct cJSON_Hooks
{
	void* (*malloc_fn)(size_t sz);
	void (*free_fn)(void* ptr);
} cJSON_Hooks;

extern void (*cJSON_free)(void* ptr);

/* Supply malloc, realloc and free functions to cJSON */
void cJSON_InitHooks(cJSON_Hooks* hooks);

/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
cJSON* cJSON_Parse(const char* value);
/* Render a cJSON entity to text for transfer/storage. Free the char* when finished. */
char* cJSON_Print(cJSON* item);
/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
// 获得指针必须使用  cJSON_free 释放。否则内存溢出
char* cJSON_PrintUnformatted(cJSON* item);
/* Delete a cJSON entity and all subentities. */
void   cJSON_Delete(cJSON* c);

/* Returns the number of items in an array (or object). */
int	  cJSON_GetArraySize(cJSON* array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
cJSON* cJSON_GetArrayItem(cJSON* array, int item);
/* Get item "string" from object. Case insensitive. */
cJSON* cJSON_GetObjectItem(cJSON* object, const char* string);

// For analysing failed parses. This returns a pointer to the parse error.
// You'll probably need to look a few chars back to make sense of it. 
// Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. 
const char* cJSON_GetErrorPtr(void);

/* These calls create a cJSON item of the appropriate type. */
cJSON* cJSON_CreateNull(void);
cJSON* cJSON_CreateTrue(void);
cJSON* cJSON_CreateFalse(void);
cJSON* cJSON_CreateBool(int b);
cJSON* cJSON_CreateNumber(double num);
cJSON* cJSON_CreateString(const char* string);
cJSON* cJSON_CreateBareString(const char* string);
cJSON* cJSON_CreateArray(void);
cJSON* cJSON_CreateObject(void);

/* These utilities create an Array of count items. */
cJSON* cJSON_CreateIntArray(const int* numbers, int count);
cJSON* cJSON_CreateFloatArray(const float* numbers, int count);
cJSON* cJSON_CreateDoubleArray(const double* numbers, int count);
cJSON* cJSON_CreateStringArray(const char** strings, int count);

/* Append item to the specified array/object. */
void cJSON_AddItemToArray(cJSON* array, cJSON* item);
void	cJSON_AddItemToObject(cJSON* object, const char* string, cJSON* item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
void cJSON_AddItemReferenceToArray(cJSON* array, cJSON* item);
void	cJSON_AddItemReferenceToObject(cJSON* object, const char* string, cJSON* item);

/* Remove/Detatch items from Arrays/Objects. */
cJSON* cJSON_DetachItemFromArray(cJSON* array, int which);
void   cJSON_DeleteItemFromArray(cJSON* array, int which);
cJSON* cJSON_DetachItemFromObject(cJSON* object, const char* string);
void   cJSON_DeleteItemFromObject(cJSON* object, const char* string);

/* Update array items. */
void cJSON_ReplaceItemInArray(cJSON* array, int which, cJSON* newitem);
void cJSON_ReplaceItemInObject(cJSON* object, const char* string, cJSON* newitem);

/* Duplicate a cJSON item */
cJSON* cJSON_Duplicate(cJSON* item, int recurse);
/* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
need to be released. With recurse!=0, it will duplicate any children connected to the item.
The item->next and ->prev pointers are always zero on return from Duplicate. */

/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
cJSON* cJSON_ParseWithOpts(const char* value, const char** return_parse_end, int require_null_terminated);

// 缩小
void cJSON_Minify(char* json);

// Macros for creating things quickly.
#define cJSON_AddNullToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddBoolToObject(object,name,b)	cJSON_AddItemToObject(object, name, cJSON_CreateBool(b))
#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))
#define cJSON_AddBareStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateBareString(s))

// 设置值
// When assigning an integer value, it needs to be propagated to valuedouble too.
#define cJSON_SetIntValue(object,val)			((object)?(object)->valueint=(object)->valuedouble=(val):(val))

