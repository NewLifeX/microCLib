
#include "cJsonProcess.h"


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

// 处理 Json
bool JsonProcessMsg(Buffer2_t* buff, cJsonProcess_t* ProcessArray)
{
	if (buff == NULL)return false;
	if (buff->data == NULL)return false;

	if (ProcessArray == NULL)
	{
		DebugPrintf("ProcessArray = NULL \r\n");
		return false;
	}

	cJSON* root = cJSON_Parse((char*)buff->data);
	if (root == NULL) return false;
	
	cJSON* action = cJSON_GetObjectItem(root, "action");
	if (action == NULL)
	{
		DebugPrintf("JsonProcessMsg haven't \"action\"\r\n");

		cJSON_Delete(root);
		return false;
	}

	// 处理
	for (int i = 0;; i++)
	{
		if (ProcessArray[i].ActionName == NULL)break;
		if (ProcessArray[i].JsonProcess == NULL)continue;

		if (StringIndexOf(action->valuestring, ProcessArray[i].ActionName) == 0)
		{
			DebugPrintf("JsonProcessMsg %s\r\n", ProcessArray[i].ActionName);
			ProcessArray[i].JsonProcess(root, ProcessArray[i].Process);
			break;
		}
	}

	cJSON_Delete(root);
	return true;
}

