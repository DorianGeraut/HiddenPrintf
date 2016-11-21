// ConsoleApplication4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



int main()
{
	char * msg = "Hello World";
	printf(" len : %d", sizeof(msg));

	char	fmt[] = "eax=%d, ebx=%d\n";
	_asm
	{
		mov ecx, 8
		mov eax, 1
		mov ebx, 10
		back1:
		inc eax
			dec ebx
			push	eax
			push	ebx
			push	ecx
			push	ebx
			push	eax
			lea	eax, fmt
			push	eax
			call dword ptr[printf]
			pop	eax
			pop	eax
			pop	eax
			pop	ecx
			pop	ebx
			pop	eax
			loop back1
			nop
	}

	return 0;
}

