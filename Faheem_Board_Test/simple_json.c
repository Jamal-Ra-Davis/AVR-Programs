#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
//#include <stdio.h>
#include <ctype.h>
#include "simple_json.h"


int simpleJsonParse(const char* json_str, int N, JSON_Data *json)
{
	int braces = 0;
	uint8_t open_quotes = 0;
	char key[64];
	int key_idx = 0;
	char value[128];
	int value_idx = 0;

	uint8_t found_colon = 0;
	uint8_t key_complete = 0;
	uint8_t value_complete = 0;

	for (int i=0; i<N; i++)
	{
		char c = json_str[i];
		if (isspace(c) && !open_quotes)
		{
			continue;
		}


		if (c == '\"')
		{
			if (open_quotes)
			{
                open_quotes = 0;
				if (!key_complete)
				{
					key_complete = 1;
					key[key_idx] = '\0';
				}
				//else if (!value_complete)
				//{
				//	value_compete = 1;
				//	value[value_idx] = '\0';
				//}
			}
            else
                open_quotes = 1;
		}
		else if (c == '{' && !open_quotes)
			braces++;
		else if (c == '}' && !open_quotes)
		{
			braces--;
			if (!value_complete)
            {
                value_complete = 1;
                value[value_idx] = '\0';
            }
		}
		else if (c == ':')
		{
			found_colon = 1;
		}
		else
		{
			if (open_quotes && !key_complete)
			{
				key[key_idx] = c;
				key_idx++;	
			}
			else if (found_colon && !value_complete)
			{
				value[value_idx] = c;
				value_idx++;
			}
		}


	}
	//printf("Key: %s\nValue: %s\n", key, value);
	if (braces || open_quotes || !found_colon || !key_complete || !value_complete)
	{
		//printf("Invalid JSON\n");
		return -1;
	}

	int i = 0;
	for (; key[i] != '\0'; i++)
	{
		json->key[i] = key[i];
	}
	json->key[i] = key[i];

	i = 0;
    for (; value[i] != '\0'; i++)
    {
        json->value[i] = value[i];
    }
    json->value[i] = value[i];

	return 0;
}
