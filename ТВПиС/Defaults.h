#pragma once

#define SEGMENTSIZE 65536
#define LISTSIZE 64
#define LINESIZE 1025
#define MAX_TYPE_NAME_LENGHT 16
#define MAX_OPERATION_NAME_LENGHT 8

#define AUTOMAT "Automat.txt"
#define LOG "Log.txt"

// @ Заменить на стандартную, ибо зачем.
inline void Copy(char* destination, char* source, int lenght)
{
	for (int i = 0; i < lenght; i++)
	{
		destination[i] = source[i];
	}
	destination[lenght] = '\0';
}