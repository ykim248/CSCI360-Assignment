#define WIN32_LEAN_AND_MEAN 

#pragma warning(disable : 4996)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>
#include "EncoderDecoder.h"

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif


#define buffer_size  10

typedef struct _ConsumeRec
{
	int* ConsumeCount;
	int who;
}ConsumeRec;

typedef struct _storageunit
{
	char data[2];
	int location;
}storageunit;

int Consume_count[3];
ConsumeRec* InfoRecArr;

storageunit buffer[buffer_size];
int buffer_front = 0;
int buffer_back = 0;
int buffer_count = 0;
int produce_count = 0;
BOOL ProducerAlive;
SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

void WINAPI Producer(HANDLE);
void WINAPI Consumer(ConsumeRec *);

LARGE_INTEGER FileSize;
HANDLE SemFilledSpots;
HANDLE SemEmptySpots;
HANDLE HMutex;
HANDLE readtofile, outfile;

BOOL encode = TRUE;

int _tmain(int argc, LPTSTR argv[])
{
	SemFilledSpots = CreateSemaphore(NULL, 0, buffer_size, NULL); 
	SemEmptySpots = CreateSemaphore(NULL, buffer_size, buffer_size, NULL);
	InitializeEncoderDecoder();
	HANDLE HProducer;
	int numofcons = argv[4];
	if (argv[3] == "D") 
	{
		encode = FALSE;
	}


	HMutex = CreateMutex(NULL, FALSE, NULL);

	readtofile = CreateFile(argv[1],
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	 outfile = CreateFile(argv[2],
		 GENERIC_WRITE,
		 FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
		 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	HProducer = (HANDLE)_beginthreadex(NULL, 0, Producer, readtofile, NULL, NULL);

	for (int i = 0; i < numofcons; i++)
	{

	}
	
	WaitForSingleObject(HProducer, INFINITE);
	//WaitForSingleObject(HConsumer, INFINITE);
	//WaitForSingleObject(HConsumer2, INFINITE);
	//WaitForSingleObject(HConsumer3, INFINITE);

	system("pause");
	return 0;
}

void WINAPI Producer(HANDLE rfile)
{
	int PrevCount;
	GetFileSizeEx(rfile, &FileSize);
	HANDLE cmapview = CreateFileMapping(rfile, &stdOutSA, PAGE_READONLY, 0, 0, NULL);
	char* vmapview = MapViewOfFile(cmapview, FILE_MAP_READ, 0, 0, FileSize.QuadPart);


	for (int x = 0; x < FileSize.QuadPart; x+=2)
	{
		WaitForSingleObject(SemEmptySpots, INFINITE);
		buffer[buffer_back].data[0] = vmapview[x];
		buffer[buffer_back].data[1] = vmapview[x+1];
		buffer[buffer_back].location = x;
		produce_count++;
		buffer_back = (buffer_back + 1) % buffer_size;
		Sleep(5);
		ReleaseSemaphore(SemFilledSpots, 1, &PrevCount);
	}
	ProducerAlive = FALSE;
}
void WINAPI Consumer(ConsumeRec* who)
{
	int PrevCount;
	while (TRUE)
	{
		if (!WaitForSingleObject(SemFilledSpots, 100))
		{
			if (encode)
			{
				who->ConsumeCount[who->who]++;
				WaitForSingleObject(HMutex, INFINITE);
				printf("consume who %d count %d %d\n", who->who, who->ConsumeCount[who->who], buffer[buffer_front]);
				buffer_front = (buffer_front + 1) % buffer_size;
				ReleaseMutex(HMutex);
				//buffer_count--;
				Sleep(2);
				ReleaseSemaphore(SemEmptySpots, 1, &PrevCount);
			}
			
		}

		if (!ProducerAlive && (produce_count - (who->ConsumeCount[0]+ who->ConsumeCount[1]+ who->ConsumeCount[2]) ==0))
		{
			break;
		}

	}

}
