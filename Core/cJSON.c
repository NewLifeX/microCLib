
#include "Type.h"
#include "cJSON.h"

#include <math.h>
#include <limits.h>
#include <float.h>


static const char* ep;

const char* cJSON_GetErrorPtr(void) { return ep; }

static int cJSON_strcasecmp(const char* s1, const char* s2)
{
	if (!s1) return (s1 == s2) ? 0 : 1;
	if (!s2) return 1;

	for (; tolower(*s1) == tolower(*s2); ++s1, ++s2)
	{
		if (*s1 == 0)	return 0;
	}

	return tolower(*(const unsigned char*)s1) - tolower(*(const unsigned char*)s2);
}

static void* (*cJSON_malloc)(size_t sz);
void (*cJSON_free)(void* ptr);

// 复制值。
static char* cJSON_strdup(const char* str)
{
	// 计算原str 长度。
	size_t len = strlen(str) + 1;

	// 创建缓冲区
	char* rs = (char*)cJSON_malloc(len);
	if (rs == NULL)return NULL;

	// 复制
	memcpy(rs, str, len);
	return rs;
}

// 设置 malloc free
void cJSON_InitHooks(cJSON_Hooks* hooks)
{
	if (!hooks)
	{
		cJSON_malloc = GlobleMalloc;
		cJSON_free = GlobleFree;
		return;
	}

	cJSON_malloc = (hooks->malloc_fn) ? hooks->malloc_fn : GlobleMalloc;
	cJSON_free = (hooks->free_fn) ? hooks->free_fn : GlobleFree;
}

// 创建一个名值对
static cJSON* cJSON_New_Item(void)
{
	cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
	if (node != NULL) memset(node, 0, sizeof(cJSON));

	return node;
}

// 删除名值对
void cJSON_Delete(cJSON* c)
{
	cJSON* next;
	while (c)
	{
		next = c->next;
		// 递归释放节点
		if (!(c->type & cJSON_IsReference) && c->child) cJSON_Delete(c->child);

		// 释放本节点 名值 对
		if (!(c->type & cJSON_IsReference) && c->valuestring) cJSON_free(c->valuestring);
		if (c->string) cJSON_free(c->string);

		// 释放节点结构体
		cJSON_free(c);
		c = next;
	}
}

// 解析数字值。
static const char* parse_number(cJSON* item, const char* num)
{
	char* numbak = (char*)num;

	// sign 正负数符号
	// scale 小数点位置
	// subscale 科学计数法 a x 10^n 的 n 值。
	// signsubscale	科学计数法 a x 10^n 的 n 符号。
	double n = 0, sign = 1, scale = 0; int subscale = 0, signsubscale = 1;

	// 负值
	if (*num == '-') sign = -1, num++;	/* Has sign? */

	// 0处理
	if (*num == '0') num++;			/* is zero */

	// 整数
	if (*num >= '1' && *num <= '9')
	{
		do
		{
			n = (n * 10.0) + (*num++ - '0');
		} while (*num >= '0' && *num <= '9');	/* Number? */
	}

	// 处理小数。
	if (*num == '.' && num[1] >= '0' && num[1] <= '9')
	{
		num++;
		do
		{
			n = (n * 10.0) + (*num++ - '0'), scale--;
		} while (*num >= '0' && *num <= '9');
	}

	// 科学计数法的标志
	if (*num == 'e' || *num == 'E')		/* Exponent? */
	{
		num++; if (*num == '+') num++;	else if (*num == '-') signsubscale = -1, num++;
		while (*num >= '0' && *num <= '9') subscale = (subscale * 10) + (*num++ - '0');
	}

	// number = +/- number.fraction * 10^+/- exponent
	n = sign * n * pow(10.0, (scale + subscale * signsubscale));

	int numlen = num - numbak;
	char* numstr = (char*)cJSON_malloc(numlen + 1);
	if (numstr != NULL)
	{
		item->valuestring = numstr;
		memcpy(numstr, numbak, numlen);
		numstr[numlen] = 0x00;
	}

	item->valuedouble = n;
	item->valueint = (int)n;
	item->type = cJSON_Number;
	return num;
}

// 打印数字
static char* print_number(cJSON* item)
{
	char* str;
	double d = item->valuedouble;
	if (fabs(((double)item->valueint) - d) <= DBL_EPSILON && d <= INT_MAX && d >= INT_MIN)
	{
		// 2^64+1 can be represented in 21 chars.
		str = (char*)cJSON_malloc(21);
		if (str) sprintf(str, "%d", item->valueint);
	}
	else
	{
		/* This is a nice tradeoff. */
		str = (char*)cJSON_malloc(64);
		if (str)
		{
			if (fabs(floor(d) - d) <= DBL_EPSILON && fabs(d) < 1.0e60)sprintf(str, "%.0f", d);

			else if (fabs(d) < 1.0e-6 || fabs(d) > 1.0e9)
				sprintf(str, "%e", d);
			else
				sprintf(str, "%f", d);
		}
	}

	return str;
}

// BCD 编码？
// 16进制 4 字节字符串，变成 ushort 二进制。
// "89AB" => 0x89ab => byte[]{ 0xab, 0xab }
static unsigned parse_hex4(const char* str)
{
	unsigned h = 0;
	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return 0;

	h = h << 4; str++;

	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return 0;

	h = h << 4; str++;

	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return 0;

	h = h << 4; str++;

	if (*str >= '0' && *str <= '9') h += (*str) - '0';
	else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
	else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
	else return 0;

	return h;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
// 解析字符串
static const char* parse_string(cJSON* item, const char* str)
{
	const char* ptr = str + 1;
	char* ptr2;
	char* out;
	int len = 0;
	unsigned uc, uc2;

	// 判误
	if (*str != '\"') { ep = str; return 0; }
	// 跳过转义符号
	while (*ptr != '\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;

	// 创建输出字符串。
	out = (char*)cJSON_malloc(len + 1);
	if (!out) return 0;

	ptr = str + 1; ptr2 = out;
	while (*ptr != '\"' && *ptr)
	{
		if (*ptr != '\\') *ptr2++ = *ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
			case 'b': *ptr2++ = '\b';	break;
			case 'f': *ptr2++ = '\f';	break;
			case 'n': *ptr2++ = '\n';	break;
			case 'r': *ptr2++ = '\r';	break;
			case 't': *ptr2++ = '\t';	break;
			case 'u':
			{
				/* transcode utf16 to utf8. */
				/* get the unicode char. */
				uc = parse_hex4(ptr + 1); ptr += 4;
				// 判断是否合法
				if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0)	break;	/* check for invalid.	*/

				// UTF16 范围
				if (uc >= 0xD800 && uc <= 0xDBFF)	/* UTF16 surrogate pairs.	*/
				{
					if (ptr[1] != '\\' || ptr[2] != 'u')	break;	/* missing second-half of surrogate.	*/
					uc2 = parse_hex4(ptr + 3); ptr += 6;
					if (uc2 < 0xDC00 || uc2>0xDFFF)		break;	/* invalid second-half of surrogate.	*/
					uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
				}

				len = 4; if (uc < 0x80) len = 1; else if (uc < 0x800) len = 2; else if (uc < 0x10000) len = 3; ptr2 += len;

				switch (len) {
				case 4: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
				case 3: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
				case 2: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
				case 1: *--ptr2 = (uc | firstByteMark[len]);
				}
				ptr2 += len;
			}

			break;

			default:  *ptr2++ = *ptr; break;
			}
			ptr++;
		}
	}

	*ptr2 = 0;
	if (*ptr == '\"') ptr++;
	item->valuestring = out;
	item->type = cJSON_String;
	return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char* print_string_ptr(const char* str)
{
	const char* ptr;
	char* ptr2, * out;
	int len = 0;
	unsigned char token;

	if (!str) return cJSON_strdup("");
	ptr = str;

	while (true)
	{
		token = *ptr;
		++len;

		if (token == '\0')break;

		if (strchr("\"\\\b\f\n\r\t", token))
			len++;
		else if (token < 32)
			len += 5;

		ptr++;
	}

	out = (char*)cJSON_malloc(len + 3);
	if (!out) return 0;

	ptr2 = out; ptr = str;
	*ptr2++ = '\"';

	while (*ptr)
	{
		if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\')
		{
			*ptr2++ = *ptr++;
		}
		else
		{
			*ptr2++ = '\\';
			switch (token = *ptr++)
			{
			case '\\':	*ptr2++ = '\\';	break;
			case '\"':	*ptr2++ = '\"';	break;
			case '\b':	*ptr2++ = 'b';	break;
			case '\f':	*ptr2++ = 'f';	break;
			case '\n':	*ptr2++ = 'n';	break;
			case '\r':	*ptr2++ = 'r';	break;
			case '\t':	*ptr2++ = 't';	break;
			default:
				sprintf(ptr2, "u%04x", token);
				ptr2 += 5;
				break;
			}
		}
	}

	*ptr2++ = '\"'; *ptr2++ = 0;
	return out;
}

/* Invote print_string_ptr (which is useful) on an item. */
static char* print_string(cJSON* item) { return print_string_ptr(item->valuestring); }

/* Predeclare these prototypes. */
static const char* parse_value(cJSON* item, const char* value);
static char* print_value(cJSON* item, int depth, int fmt);
static const char* parse_array(cJSON* item, const char* value);
static char* print_array(cJSON* item, int depth, int fmt);
static const char* parse_object(cJSON* item, const char* value);
static char* print_object(cJSON* item, int depth, int fmt);

/* Utility to jump whitespace and cr/lf */
static const char* skip(const char* in)
{
	while (in && *in && (unsigned char)*in <= 32)
	{
		in++;
	}

	return in;
}

/* Parse an object - create a new root, and populate. */
cJSON* cJSON_ParseWithOpts(const char* value, const char** return_parse_end, int require_null_terminated)
{
	const char* end = 0;
	cJSON* c = cJSON_New_Item();
	ep = 0;
	if (!c) return 0;       /* memory fail */

	end = parse_value(c, skip(value));
	if (!end) { cJSON_Delete(c); return 0; }	/* parse failure. ep is set. */

	/* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
	if (require_null_terminated)
	{
		end = skip(end);
		if (*end)
		{
			cJSON_Delete(c);
			ep = end; return 0;
		}
	}

	if (return_parse_end) *return_parse_end = end;

	return c;
}

/* Default options for cJSON_Parse */
cJSON* cJSON_Parse(const char* value) { return cJSON_ParseWithOpts(value, 0, 0); }

/* Render a cJSON item/entity/structure to text. */
char* cJSON_Print(cJSON* item) { return print_value(item, 0, 1); }
char* cJSON_PrintUnformatted(cJSON* item) { return print_value(item, 0, 0); }

/* Parser core - when encountering text, process appropriately. */
static const char* parse_value(cJSON* item, const char* value)
{
	if (!value)	 return 0;

	if (!strncmp(value, "null", 4)) { item->type = cJSON_NULL;  return value + 4; }
	if (!strncmp(value, "Null", 4)) { item->type = cJSON_NULL;  return value + 4; }
	if (!strncmp(value, "NULL", 4)) { item->type = cJSON_NULL;  return value + 4; }

	if (!strncmp(value, "false", 5)) { item->type = cJSON_False; return value + 5; }
	if (!strncmp(value, "False", 5)) { item->type = cJSON_False; return value + 5; }

	if (!strncmp(value, "true", 4)) { item->type = cJSON_True; item->valueint = 1;	return value + 4; }
	if (!strncmp(value, "True", 4)) { item->type = cJSON_True; item->valueint = 1;	return value + 4; }

	if (*value == '\"') { return parse_string(item, value); }
	if (*value == '-' || (*value >= '0' && *value <= '9')) { return parse_number(item, value); }
	if (*value == '[') { return parse_array(item, value); }
	if (*value == '{') { return parse_object(item, value); }

	// 错误定位
	ep = value;
	return NULL;
}

/* Render a value to text. */
static char* print_value(cJSON* item, int depth, int fmt)
{
	char* out = 0;
	if (!item) return NULL;

	switch ((item->type) & 255)
	{
	case cJSON_NULL:	out = cJSON_strdup("null");		break;
	case cJSON_False:	out = cJSON_strdup("false");	break;
	case cJSON_True:	out = cJSON_strdup("true");		break;
	case cJSON_Number:	out = print_number(item);		break;
	case cJSON_String:	out = print_string(item);		break;
	case cJSON_BareString:  out = cJSON_strdup(item->valuestring); break;
	case cJSON_Array:	out = print_array(item, depth, fmt); break;
	case cJSON_Object:	out = print_object(item, depth, fmt); break;
	}

	return out;
}

/* Build an array from input text. */
static const char* parse_array(cJSON* item, const char* value)
{
	cJSON* child;
	/* not an array! */
	if (*value != '[') { ep = value; return 0; }

	item->type = cJSON_Array;
	value = skip(value + 1);

	/* empty array. */
	if (*value == ']') return value + 1;

	item->child = child = cJSON_New_Item();
	if (!item->child) return NULL;

	/* skip any spacing, get the value. */
	value = skip(parse_value(child, skip(value)));
	if (!value) return NULL;

	while (*value == ',')
	{
		cJSON* new_item;
		/* memory fail */
		new_item = cJSON_New_Item();
		if (new_item == NULL) return NULL;

		child->next = new_item; new_item->prev = child; child = new_item;
		value = skip(parse_value(child, skip(value + 1)));

		/* memory fail */
		if (!value) return NULL;
	}

	/* end of array */
	if (*value == ']') return value + 1;

	ep = value;
	return NULL;
}

/* Render an array to text */
static char* print_array(cJSON* item, int depth, int fmt)
{
	char** entries;
	char* out = 0, * ptr, * ret; int len = 5;
	cJSON* child = item->child;
	int numentries = 0, i = 0, fail = 0;

	/* How many entries in the array? */
	while (child) numentries++, child = child->next;
	/* Explicitly handle numentries==0 */
	if (!numentries)
	{
		out = (char*)cJSON_malloc(3);
		if (out) strcpy(out, "[]");
		return out;
	}

	/* Allocate an array to hold the values for each */
	entries = (char**)cJSON_malloc(numentries * sizeof(char*));
	if (!entries) return NULL;

	memset(entries, 0, numentries * sizeof(char*));
	/* Retrieve all the results: */
	child = item->child;
	while (child && !fail)
	{
		ret = print_value(child, depth + 1, fmt);
		entries[i++] = ret;
		if (ret) len += strlen(ret) + 2 + (fmt ? 1 : 0); else fail = 1;
		child = child->next;
	}

	/* If we didn't fail, try to malloc the output string */
	if (!fail) out = (char*)cJSON_malloc(len);
	/* If that fails, we fail. */
	if (!out) fail = 1;

	/* Handle failure. */
	if (fail)
	{
		for (i = 0; i < numentries; i++) if (entries[i]) cJSON_free(entries[i]);
		cJSON_free(entries);
		return NULL;
	}

	/* Compose the output array. */
	*out = '[';
	ptr = out + 1; *ptr = 0;
	for (i = 0; i < numentries; i++)
	{
		strcpy(ptr, entries[i]); ptr += strlen(entries[i]);

		if (i != numentries - 1)
		{
			*ptr++ = ',';
			if (fmt)*ptr++ = ' ';
			*ptr = 0;
		}

		cJSON_free(entries[i]);
	}
	cJSON_free(entries);

	*ptr++ = ']';
	*ptr++ = 0;
	return out;
}

/* Build an object from the text. */
static const char* parse_object(cJSON* item, const char* value)
{
	cJSON* child;
	/* not an object! */
	if (*value != '{')
	{
		ep = value;
		return NULL;
	}

	item->type = cJSON_Object;
	value = skip(value + 1);
	/* empty array. */
	if (*value == '}') return value + 1;

	item->child = child = cJSON_New_Item();
	if (!item->child) return NULL;

	value = skip(parse_string(child, skip(value)));
	if (!value) return NULL;

	child->string = child->valuestring; child->valuestring = 0;
	if (*value != ':')
	{
		ep = value;
		return NULL;
	}

	/* skip any spacing, get the value. */
	value = skip(parse_value(child, skip(value + 1)));
	if (!value) return NULL;

	while (*value == ',')
	{
		cJSON* new_item = cJSON_New_Item();
		/* memory fail */
		if (new_item == NULL)	return NULL;

		child->next = new_item; new_item->prev = child; child = new_item;
		value = skip(parse_string(child, skip(value + 1)));
		if (!value) return NULL;

		child->string = child->valuestring; child->valuestring = 0;
		if (*value != ':')
		{
			ep = value;
			return NULL;
		}

		/* skip any spacing, get the value. */
		value = skip(parse_value(child, skip(value + 1)));
		if (!value) return NULL;
	}

	if (*value == '}') return value + 1;	/* end of array */

	ep = value;
	return NULL;
}

/* Render an object to text. */
static char* print_object(cJSON* item, int depth, int fmt)
{
	char** entries = 0, ** names = 0;
	char* out = 0, * ptr, * ret, * str;
	int len = 7, i = 0, j;
	cJSON* child = item->child;
	int numentries = 0, fail = 0;

	/* Count the number of entries. */
	while (child) numentries++, child = child->next;

	/* Explicitly handle empty object case */
	if (!numentries)
	{
		out = (char*)cJSON_malloc(fmt ? depth + 4 : 3);
		if (!out)	return NULL;

		ptr = out; *ptr++ = '{';
		if (fmt)
		{
			*ptr++ = '\n';
			for (i = 0; i < depth - 1; i++) *ptr++ = '\t';
		}

		*ptr++ = '}';
		*ptr++ = 0;
		return out;
	}

	/* Allocate space for the names and the objects */
	entries = (char**)cJSON_malloc(numentries * sizeof(char*));
	if (!entries) return NULL;

	names = (char**)cJSON_malloc(numentries * sizeof(char*));
	if (!names)
	{
		cJSON_free(entries);
		return NULL;
	}

	memset(entries, 0, sizeof(char*) * numentries);
	memset(names, 0, sizeof(char*) * numentries);

	/* Collect all the results into our arrays: */
	child = item->child; depth++; if (fmt) len += depth;
	while (child)
	{
		names[i] = str = print_string_ptr(child->string);
		entries[i++] = ret = print_value(child, depth, fmt);

		if (str && ret)
			len += strlen(ret) + strlen(str) + 2 + (fmt ? 2 + depth : 0);
		else
			fail = 1;

		child = child->next;
	}

	/* Try to allocate the output string */
	if (!fail) out = (char*)cJSON_malloc(len);
	if (!out) fail = 1;

	/* Handle failure */
	if (fail)
	{
		for (i = 0; i < numentries; i++)
		{
			if (names[i]) cJSON_free(names[i]);
			if (entries[i]) cJSON_free(entries[i]);
		}

		cJSON_free(names); cJSON_free(entries);
		return 0;
	}

	/* Compose the output: */
	*out = '{'; ptr = out + 1; if (fmt)*ptr++ = '\n'; *ptr = 0;
	for (i = 0; i < numentries; i++)
	{
		if (fmt)
		{
			for (j = 0; j < depth; j++) *ptr++ = '\t';
		}
		// key
		strcpy(ptr, names[i]);
		ptr += strlen(names[i]);
		// :
		*ptr++ = ':';
		// value
		if (fmt) *ptr++ = '\t';
		strcpy(ptr, entries[i]);
		ptr += strlen(entries[i]);

		// 分割
		if (i != numentries - 1) *ptr++ = ',';
		if (fmt) *ptr++ = '\n'; *ptr = 0;
		cJSON_free(names[i]); cJSON_free(entries[i]);
	}

	cJSON_free(names); cJSON_free(entries);
	if (fmt)
	{
		for (i = 0; i < depth - 1; i++) *ptr++ = '\t';
	}

	*ptr++ = '}';
	*ptr++ = 0;
	return out;
}

/* Get Array size/item / object item. */
int    cJSON_GetArraySize(cJSON* array) { cJSON* c = array->child; int i = 0; while (c)i++, c = c->next; return i; }
cJSON* cJSON_GetArrayItem(cJSON* array, int item) { cJSON* c = array->child;  while (c && item > 0) item--, c = c->next; return c; }
cJSON* cJSON_GetObjectItem(cJSON* object, const char* string) { cJSON* c = object->child; while (c && cJSON_strcasecmp(c->string, string)) c = c->next; return c; }

/* Utility for array list handling. */
static void suffix_object(cJSON* prev, cJSON* item) { prev->next = item; item->prev = prev; }
/* Utility for handling references. */
static cJSON* create_reference(cJSON* item)
{
	cJSON* ref = cJSON_New_Item();
	if (!ref) return NULL;

	memcpy(ref, item, sizeof(cJSON));
	ref->string = 0;
	ref->type |= cJSON_IsReference;
	ref->next = ref->prev = 0;
	return ref;
}

/* Add item to array/object. */
void   cJSON_AddItemToArray(cJSON* array, cJSON* item)
{
	cJSON* c = array->child;
	if (!item) return;

	if (!c)
	{
		array->child = item;
	}
	else
	{
		while (c && c->next) c = c->next;
		suffix_object(c, item);
	}
}

void   cJSON_AddItemToObject(cJSON* object, const char* string, cJSON* item)
{
	if (!item) return;
	if (item->string)
		cJSON_free(item->string);
	item->string = cJSON_strdup(string);
	cJSON_AddItemToArray(object, item);
}

void	cJSON_AddItemReferenceToArray(cJSON* array, cJSON* item) { cJSON_AddItemToArray(array, create_reference(item)); }
void	cJSON_AddItemReferenceToObject(cJSON* object, const char* string, cJSON* item) { cJSON_AddItemToObject(object, string, create_reference(item)); }

cJSON* cJSON_DetachItemFromArray(cJSON* array, int which)
{
	cJSON* c = array->child;
	while (c && which > 0) c = c->next, which--;

	if (!c) return NULL;
	if (c->prev) c->prev->next = c->next;
	if (c->next) c->next->prev = c->prev;

	if (c == array->child) array->child = c->next;

	c->prev = c->next = 0;
	return c;
}

void   cJSON_DeleteItemFromArray(cJSON* array, int which) { cJSON_Delete(cJSON_DetachItemFromArray(array, which)); }
cJSON* cJSON_DetachItemFromObject(cJSON* object, const char* string)
{
	int i = 0;
	cJSON* c = object->child;
	while (c && cJSON_strcasecmp(c->string, string)) i++, c = c->next;
	if (c) return cJSON_DetachItemFromArray(object, i);

	return NULL;
}

void   cJSON_DeleteItemFromObject(cJSON* object, const char* string) { cJSON_Delete(cJSON_DetachItemFromObject(object, string)); }

/* Replace array/object items with new ones. */
void   cJSON_ReplaceItemInArray(cJSON* array, int which, cJSON* newitem)
{
	cJSON* c = array->child;
	while (c && which > 0) c = c->next, which--;

	if (!c) return;
	newitem->next = c->next;
	newitem->prev = c->prev;
	if (newitem->next) newitem->next->prev = newitem;
	if (c == array->child)
		array->child = newitem;
	else
		newitem->prev->next = newitem;

	c->next = c->prev = 0;
	cJSON_Delete(c);
}

void   cJSON_ReplaceItemInObject(cJSON* object, const char* string, cJSON* newitem)
{
	int i = 0;
	cJSON* c = object->child;
	while (c && cJSON_strcasecmp(c->string, string))i++, c = c->next;
	if (c)
	{
		newitem->string = cJSON_strdup(string);
		cJSON_ReplaceItemInArray(object, i, newitem);
	}
}

/* Create basic types: */
cJSON* cJSON_CreateNull(void) { cJSON* item = cJSON_New_Item(); if (item)item->type = cJSON_NULL; return item; }
cJSON* cJSON_CreateTrue(void) { cJSON* item = cJSON_New_Item(); if (item)item->type = cJSON_True; return item; }
cJSON* cJSON_CreateFalse(void) { cJSON* item = cJSON_New_Item(); if (item)item->type = cJSON_False; return item; }
cJSON* cJSON_CreateBool(int b) { cJSON* item = cJSON_New_Item(); if (item)item->type = b ? cJSON_True : cJSON_False; return item; }
cJSON* cJSON_CreateNumber(double num) { cJSON* item = cJSON_New_Item(); if (item) { item->type = cJSON_Number; item->valuedouble = num; item->valueint = (int)num; }return item; }
cJSON* cJSON_CreateString(const char* string) { cJSON* item = cJSON_New_Item(); if (item) { item->type = cJSON_String; item->valuestring = cJSON_strdup(string); }return item; }
cJSON* cJSON_CreateBareString(const char* string) { cJSON* item = cJSON_New_Item(); if (item) { item->type = cJSON_BareString; item->valuestring = cJSON_strdup(string); }return item; }
cJSON* cJSON_CreateArray(void) { cJSON* item = cJSON_New_Item(); if (item)item->type = cJSON_Array; return item; }
cJSON* cJSON_CreateObject(void) { cJSON* item = cJSON_New_Item(); if (item)item->type = cJSON_Object; return item; }

/* Create Arrays: */
cJSON* cJSON_CreateIntArray(const int* numbers, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateNumber(numbers[i]);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}

cJSON* cJSON_CreateFloatArray(const float* numbers, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateNumber(numbers[i]);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}

cJSON* cJSON_CreateDoubleArray(const double* numbers, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateNumber(numbers[i]);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}
	return a;
}

cJSON* cJSON_CreateStringArray(const char** strings, int count)
{
	int i;
	cJSON* n = 0, * p = 0, * a = cJSON_CreateArray();
	for (i = 0; a && i < count; i++)
	{
		n = cJSON_CreateString(strings[i]);
		if (!i)
			a->child = n;
		else
			suffix_object(p, n);
		p = n;
	}

	return a;
}

// 复制
cJSON* cJSON_Duplicate(cJSON* item, int recurse)
{
	if (!item) return 0;

	cJSON* newitem, * cptr, * nptr = 0, * newchild;

	/* Create new item */
	newitem = cJSON_New_Item();

	if (!newitem) return 0;

	/* Copy over all vars */
	newitem->type = item->type & (~cJSON_IsReference), newitem->valueint = item->valueint, newitem->valuedouble = item->valuedouble;

	if (item->valuestring)
	{
		newitem->valuestring = cJSON_strdup(item->valuestring);
		if (!newitem->valuestring)
		{
			cJSON_Delete(newitem);
			return 0;
		}
	}

	if (item->string)
	{
		newitem->string = cJSON_strdup(item->string);
		if (!newitem->string)
		{
			cJSON_Delete(newitem);
			return 0;
		}
	}

	/* If non-recursive, then we're done! */
	if (!recurse) return newitem;

	/* Walk the ->next chain for the child. */
	cptr = item->child;
	while (cptr)
	{
		// Duplicate (with recurse) each item in the ->next chain。 
		// 递归遍历
		newchild = cJSON_Duplicate(cptr, 1);

		if (!newchild)
		{
			cJSON_Delete(newitem);
			return 0;
		}

		// If newitem->child already set, then crosswire ->prev and ->next and move on
		if (nptr)
		{
			nptr->next = newchild, newchild->prev = nptr; nptr = newchild;
		}
		else
		{
			/* Set newitem->child and move to it */
			newitem->child = newchild; nptr = newchild;
		}

		cptr = cptr->next;
	}

	return newitem;
}

// 缩小， 删除一堆不可见字符，回车换行等。
void cJSON_Minify(char* json)
{
	char* into = json;
	while (*json)
	{
		if (*json == ' ') json++;
		else if (*json == '\t') json++;
		else if (*json == '\r') json++;
		else if (*json == '\n') json++;
		else if (*json == '/' && json[1] == '/')
		{
			// double-slash comments, to end of line.
			while (*json && *json != '\n') json++;
		}

		else if (*json == '/' && json[1] == '*')
		{
			// multiline comments.
			while (*json && !(*json == '*' && json[1] == '/')) json++; json += 2;
		}

		else if (*json == '\"')
		{
			// string literals, which are \" sensitive.
			*into++ = *json++;
			while (*json && *json != '\"')
			{
				if (*json == '\\') *into++ = *json++;
				*into++ = *json++;
			}
			*into++ = *json++;
		}

		// All other characters.
		else *into++ = *json++;
	}

	*into = 0;	// and null-terminate.
}


int JsonTryGetValue_Int(cJSON* json, char* name, int def)
{
	if (json == NULL)return def;
	if (name == NULL)return def;

	cJSON* obj = cJSON_GetObjectItem(json, name);
	if (obj == NULL)return def;

	return obj->valueint;
}

double JsonTryGetValue_Double(cJSON* json, char* name, double def)
{
	if (json == NULL)return def;
	if (name == NULL)return def;

	cJSON* obj = cJSON_GetObjectItem(json, name);
	if (obj == NULL)return def;

	return obj->valuedouble;
}

char* JsonTryGetValue_String(cJSON* json, char* name, char* def)
{
	if (json == NULL)return def;
	if (name == NULL)return def;

	cJSON* obj = cJSON_GetObjectItem(json, name);
	if (obj == NULL)return def;

	return obj->valuestring;
}

