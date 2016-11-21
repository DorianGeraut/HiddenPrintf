// ConsoleApplication1.cpp : Defines the entry point for the console application.
//TP2

#include "stdafx.h"

typedef int(*type_printf)(const char*, ...);
char ref[] = { '\x55','\x8b','\xec','\x56','\x8b','\x75','\x08','\x6a','\x01','\xff' };

int main()
{
	//type_printf f = (type_printf)((char*)getchar + 0x86D0); //0x86D0 est une différence qui dépend beaucoup du système

	//printf("printf: %p, scanf_s: %p\n", printf, scanf_s);

	//printf("printf - scanf_s = %x", (char*)printf-(char*)scanf_s );
	
	char* p = (char*)scanf_s;
	int i = 0;


	//search forward scanf_s
	for (i = 0; i < 1000; i++) {
		if (memcmp(p + i, ref, sizeof(ref)) == 0) {
			break;
		}
	}

	//if not find yet
	if (memcmp(p + i, ref, sizeof(ref))) {
		//search backward scanf_s
		for (i = 0; i < 1000; i--) {
			if (memcmp(p + i, ref, sizeof(ref)) == 0) {
				break;
			}
		}
	}

	if (memcmp(p + i, ref, sizeof(ref)) == 0) {
		printf("%p = %p  ???\n", p + i, printf);
	}
	else {
		printf("printf not found :(\n");
	}
	return 0;


}

