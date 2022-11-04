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

typedef struct info
{
	int num;
	char message[20];
}info_rec,*pinfo_rec;

char data[100];  // Static global data
char* data2; // Dynamic global data
char* data3;
int globsize;
HANDLE* thandle;

WORD WINAPI DoWork(pinfo_rec);



int _tmain(int argc, LPTSTR argv[])
{



	SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
	{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE STDInput, hscource;
	LARGE_INTEGER FileSize;
	STDInput = GetStdHandle(STD_INPUT_HANDLE);
	int numworkers = argc - 1;
	GetFileSizeEx(STDInput, &FileSize);
	hscource = CreateFileMapping(STDInput, &stdOutSA, PAGE_READONLY, 0, 0, NULL);
	char* pindumb = MapViewOfFile(hscource, FILE_MAP_READ , 0, 0, FileSize.QuadPart); //
	int size = FileSize.QuadPart;
	globsize = size;
	data3 = malloc(size);
	for (int x = 0; x < size; x++) {
		data3[x] = pindumb[x];
	}
	

	if (argc > 1)
	{
		thandle = malloc(numworkers * sizeof(HANDLE));
		pinfo_rec rec = malloc(numworkers * sizeof(info_rec));
		for (int x = 0; x < numworkers; x++)
		{
			strcpy(rec[x].message, argv[x + 1]);		
			
		}
		for (int x = 0; x < numworkers; x++)
		{
			thandle[x] = (HANDLE)_beginthreadex(NULL, 0, DoWork, &rec[x], 0, NULL);
			if (thandle[x] == NULL)
				printf("Could not start up thread# %d !", x);
			//WaitForSingleObject(thandle[x], INFINITE);
		}
		for (int x = 0; x < numworkers; x++)
		{
			WaitForSingleObject(thandle[x], INFINITE);
		}
		//WaitForMultipleObjects(numworkers, thandle, TRUE, INFINITE);
	}
	
	free(data3);
	getch();
	return 0;
}

WORD WINAPI DoWork(pinfo_rec who)
{
	//int textlen = sizeof(data3);
	
	int searchlen = strlen(who->message);
	who->num = 0;
	for (int x = 0; x <= (globsize - searchlen); x++)
	{
		if (strncmp(&who->message[0], &data3[x], searchlen) == 0)
		{
			who->num++;
		}
	}
	
	printf("%s appears %d \n", who->message, who->num);

	/*printf("Thread #%d Message ->%s\n", who->num,who->message);
	printf("data ->%s\n", data);
	printf("data ->%s\n", data2);
	if (who->num == 1)
	{
		who->num = 22;
	}
	else
	{
		who->num = 33;
		Sleep(500);
		strcpy(data, "");
	}*/

}