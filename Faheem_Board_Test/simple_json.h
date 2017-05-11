#pragma once

typedef struct
{
	char key[64];
	char value[128];
} JSON_Data;


int simpleJsonParse(const char* json_str, int N, JSON_Data *json);
