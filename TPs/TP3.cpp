// ConsoleApplication2.cpp : Defines the entry point for the console application.
//TP3

#include "stdafx.h"
#include <Windows.h>
#include <winternl.h>

int _tmain(int argc, _TCHAR* argv[])
{
	printf("%d", IsDebuggerPresent());
	if (IsDebuggerPresent()) {
		printf("Debug mod ON\n");
	}
	else {
		printf("Debug mod OFF\n");
	}

	int hum;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &hum);
	printf("debug : %d\n", hum);

	void *pointeur_peb;
	PPEB peb;

	//Note sur les registres (pour l'assembleur):
	//	Il y a eax, ebx, ecx....
	//	ainsi que rax, rbx, rcx...


	//Comment is�rer de l'assembleur dans du code C ?
	//H� bin juste comme �a :
	_asm {
		//on charge le registre eax avec le contenu de la 30eme (avec 'h' pour 'hexa') case m�moire apr�s fs (apr�s c'est o� fs ? myst�re)
		mov eax, fs:[30h]
		//puis onle met dans peb
		mov peb, eax
	}
	//Et on a trouv� la place de la table PEB
	printf("peb : %p\n", peb);


	//Pour rappel, la structure PEB est ainsi faite :
	//typedef struct _PEB {
	//	BYTE                          Reserved1[2];
	//	BYTE                          BeingDebugged;
	//	BYTE                          Reserved2[1];
	//	PVOID                         Reserved3[2];
	//	PPEB_LDR_DATA                 Ldr;
	//	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
	//	BYTE                          Reserved4[104];
	//	PVOID                         Reserved5[52];
	//	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	//	BYTE                          Reserved6[128];
	//	PVOID                         Reserved7[1];
	//	ULONG                         SessionId;
	//} PEB, *PPEB;

	//On va modifier le second byte pour tromper le syst�me

	//D'abord on le lit : 
	char dbg = ((char*)peb)[2];
	int ddbg = ((char *)peb)[0x68] & 0x70;
	printf("peb %p , dbg %d, ddbg %d \n\n", peb, dbg, ddbg);//Bon et bien on n'arrive pas � changer ddbg, �a marche pas �a rend 0 -_-
	
	PPEB_LDR_DATA pld = peb->Ldr;
	PLIST_ENTRY module = &pld->InMemoryOrderModuleList;

	//On peut maintenant se balader dans la listedes moduelsavec les atributs Flink et Blink
	//module = module->Flink;

	char *pointeur = (char *)module;
	//On enl�ve deux fois un PVOID pour trouver le d�but de _LDR_DATA_TABLE_ENTRY (voir structur :)
	//typedef struct _LDR_DATA_TABLE_ENTRY { //<= on veut aller l�
	//	PVOID Reserved1[2];
	//	LIST_ENTRY InMemoryOrderLinks; //<= on est l�
	//	PVOID Reserved2[2];
	//	PVOID DllBase;
	//	PVOID EntryPoint;
	//	PVOID Reserved3;
	//	UNICODE_STRING FullDllName;
	//	BYTE Reserved4[8];
	//	PVOID Reserved5[3];
	//	union {
	//		ULONG CheckSum;
	//		PVOID Reserved6;
	//	};
	module = module->Flink;
	pointeur = (char *)module - 2 * sizeof(PVOID);
	PLDR_DATA_TABLE_ENTRY pdte = (PLDR_DATA_TABLE_ENTRY)pointeur;
	printf("DLL Flink = %S \n\n", pdte->FullDllName.Buffer); //On peut lire que dans la liste doublement chain�e des modules (Flink et Blink), le premier des module est l'�x�cutable lui m�me.

	//si on remonte encore :

	module = module->Flink;
	pointeur = (char *)module - 2 * sizeof(PVOID);
	pdte = (PLDR_DATA_TABLE_ENTRY)pointeur;
	printf("DLL Flink*2 = %S\n\n", pdte->FullDllName.Buffer);

	int cpt = 2;
	while (pdte->FullDllName.Buffer != NULL) {
		module = module->Flink;
		pointeur = (char *)module - 2 * sizeof(PVOID);
		pdte = (PLDR_DATA_TABLE_ENTRY)pointeur;
		cpt++;
		printf("DLL Flink*%d = %S\n\n",cpt, pdte->FullDllName.Buffer);
	}

	///Deuxi�me partie : PE et modificationdu code d'internet explorer
	//Intro: Quelque part dans la m�moire existe une zone "ie.exe", ce ne sont pas tout � fait les octets de l'�x�cutable
	//Avec la table PEB + cette zone ie.exe, si on peut modifier les deux, c'est gagn�.

	//Lors d'une ex�cution, la premi�re chose que l'OS fait est de v�rifier si le programme commence par les octetes "4d a5", nom� DOS MZ header
	//Pour v�rifier qu'il sagirt bien de code ex�cutable. Ceci s'adresse au DOS et pas � Windows � la base(une couche au dessus), DOS n'�xiste plus vraiments
	//mais Windows se sert quand m�me de cette partie, avec la partie suivante, nomm�e DOS stub. On peut planqu� du code dedans, c'est vraiment un truc qui
	//est l� pour des raison historiques.

	//Ensuite il y � le PE header, et �a devient int�ressant. Ca commence par 50 45 00 00 (qui sont les char P et E). On peut y trouver le WORD (qui
	//est en fait simplement un octet) nomm� AddressOfEntryPoint qui va nous donn� l'adresse o� trouver le code du programme qu'on pourra r��crir octets par octets.
	//Ensuite il y a la table des sections, qui rassemble les emplacements de certains �l�ments indispensable pour l'execution du code (comme les dll). Il y a 
	//les tables import�es et export�es.
	while (1);
    return 0;
}

