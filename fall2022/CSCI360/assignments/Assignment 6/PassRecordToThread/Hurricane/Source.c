#define WIN32_LEAN_AND_MEAN 


#pragma warning(disable : 4996)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif


typedef struct _RECORD
{
	int AccountNumber;
	int Transaction;
} RECORD, *info_REC;

HANDLE STDInput, STDOutput, Cmapview, copymap;
LARGE_INTEGER FileSize;
DWORD BIn;
HANDLE* InTempFile;
HANDLE *thandle;
RECORD* Vmapview;
RECORD* copymapview;
SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

WORD WINAPI DoTrans(HANDLE*);
CRITICAL_SECTION* fthread;


int _tmain(int argc, LPTSTR argv[])
{
	STDInput = GetStdHandle(STD_INPUT_HANDLE);
	STDOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	GetFileSizeEx(STDInput, &FileSize);	

	int numofbanks = FileSize.QuadPart / 8;
	int numoftrans = argc - 1;

	Cmapview = CreateFileMapping(STDInput, &stdOutSA, PAGE_READONLY, 0, 0, NULL);
	copymap = CreateFileMapping(STDOutput, &stdOutSA, PAGE_READWRITE, FileSize.HighPart, FileSize.LowPart, NULL);
	Vmapview = MapViewOfFile(Cmapview, FILE_MAP_READ , 0, 0, FileSize.QuadPart);
	copymapview = MapViewOfFile(copymap, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);
	for (int i = 0; i < numofbanks; i++)
	{
		copymapview[i] = Vmapview[i];
	}


	InTempFile = malloc(numoftrans * sizeof(HANDLE));
	thandle = malloc(numoftrans * sizeof(HANDLE));
	fthread = malloc(numofbanks * sizeof(CRITICAL_SECTION));
	
	for (int i = 0; i < argc; i++)
	{
		InTempFile[i] =
			CreateFile(argv[i+1],
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	for (int i = 0; i < numofbanks; i++)
	{
		InitializeCriticalSection(&fthread[i]);
	}
	for (int i = 0; i < numoftrans; i++)
	{
		thandle[i] = (HANDLE)_beginthreadex(NULL, 0, DoTrans, InTempFile[i], 0, NULL);
		if (thandle[i] == NULL)
			printf("Could not start up thread# %d !", i);
	}
	for (int i = 0; i < numoftrans; i++)
	{
		WaitForSingleObject(thandle[i], INFINITE);
	}

	UnmapViewOfFile(Vmapview);
	UnmapViewOfFile(copymapview);
	CloseHandle(Cmapview);
	CloseHandle(copymap);


	//system("pause");
	return 0;
}

WORD WINAPI DoTrans(HANDLE who)
{
	LARGE_INTEGER tfilesize;
	GetFileSizeEx(who, &tfilesize);
	int numtrans = tfilesize.QuadPart / 8;
	/*HANDLE chandlemap = CreateFileMapping(who, &stdOutSA, PAGE_READWRITE, tfilesize.HighPart, tfilesize.LowPart, NULL);
	RECORD* vhandlemap = MapViewOfFile(chandlemap, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);*/

	for (int i = 0; i < numtrans; i++)
	{
		RECORD arroftrans;
		ReadFile(who, &arroftrans, sizeof(RECORD), &BIn, NULL);
		EnterCriticalSection(&fthread[arroftrans.AccountNumber]);
		__try {
			int newtranstrans = copymapview[arroftrans.AccountNumber].Transaction + arroftrans.Transaction;
			//printf("%d", newtranstrans);
			copymapview[arroftrans.AccountNumber].Transaction = newtranstrans;
			//printf("%d", copymapview[arroftrans.AccountNumber].Transaction);

		}
		__finally {
			LeaveCriticalSection(&fthread[arroftrans.AccountNumber]);
		}
		
	}



	return 0;
}