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
	HANDLE hconsumer;
	int consumecount;
	int who;
}ConsumeRec;

typedef struct _storageunit
{
	char data[2];
	int location;
}storageunit;

typedef struct INFO_BLOCK_TAG {
	int who;
	int state;
	char* Mode;
	char word[2];
	int location;
	BOOL done;
	LARGE_INTEGER bitch;
}  INFO_BLOCK, * PINFO_BLOCK;


INFO_BLOCK* pThreadArg;

int Consume_count;
ConsumeRec* inforrecarr;

storageunit buffer[buffer_size];
int buffer_front = 0;
int buffer_back = 0;
int buffer_count = 0;
int produce_count = 0;
int taskcount = 0;
BOOL ProducerAlive = TRUE;
SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

void WINAPI Producer(HANDLE);
void WINAPI Consumer(ConsumeRec *);
VOID CALLBACK task1(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK);

LARGE_INTEGER FileSize;
HANDLE SemFilledSpots;
HANDLE SemEmptySpots;
HANDLE HMutex, HMutex2;
HANDLE readtofile, outfile;
HANDLE HProducer;


int numofcons;
char* encoder;

int _tmain(int argc, LPTSTR argv[])
{
	SemFilledSpots = CreateSemaphore(NULL, 0, buffer_size, NULL); 
	SemEmptySpots = CreateSemaphore(NULL, buffer_size, buffer_size, NULL);
	InitializeEncoderDecoder();
	numofcons = atoi(argv[4]);
	encoder = argv[3];

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numCPU = sysinfo.dwNumberOfProcessors;

	TP_CALLBACK_ENVIRON cbe;  // Callback environment
	PTP_POOL pool = NULL;

	//inforrecarr = malloc(numofcons * sizeof(ConsumeRec));
	HANDLE* htask1 = malloc(sizeof(HANDLE) * numofcons);
	pThreadArg = malloc(sizeof(INFO_BLOCK) * numofcons);

	InitializeThreadpoolEnvironment(&cbe);
	pool = CreateThreadpool(NULL);
	if (NULL == pool)
		printf("CreateThreadpool failed. LastError: %u\n", GetLastError());

	SetThreadpoolThreadMaximum(pool, 2);  // switch to numCPU later
	SetThreadpoolCallbackPool(&cbe, pool);

	HMutex = CreateMutex(NULL, FALSE, NULL);
	HMutex2 = CreateMutex(NULL, FALSE, NULL);


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
		pThreadArg[i].who = i;
		pThreadArg[i].Mode = encoder;
		pThreadArg[i].state = 1;
		pThreadArg[i].bitch.QuadPart = 0;
		pThreadArg[i].done = FALSE;
	}	

	for (int i = 0; i < numofcons; i++)
	{
		htask1[i] = CreateThreadpoolWork(task1, &(pThreadArg[i]), &cbe);
	}
	for (int i = 0; i < numofcons; i++)
	{
		SubmitThreadpoolWork(htask1[i]);
	}

	while (ProducerAlive || produce_count > taskcount)
	{
		for (int i = 0; i < numofcons; i++)
		{
			if (htask1[i])
			{
				WaitForThreadpoolWorkCallbacks(htask1[i], FALSE);
				if (ProducerAlive || pThreadArg[i].state!=1)
				{
					SubmitThreadpoolWork(htask1[i]);
				}
				else
				{
					htask1[i] = NULL;
					taskcount++;
				}		
			}
		}
	} 
	WaitForSingleObject(HProducer, INFINITE);// this is the problem
 	
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
VOID CALLBACK task1(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)
{
	INFO_BLOCK* data;

	data = (INFO_BLOCK*)Context;

	int PrevCount;
	char locdata[2];
	int tempCC;
	DWORD Bout;
	LARGE_INTEGER spot;

	if (data->state == 1)
	{
		
		//spot.QuadPart = 0;
		if (!WaitForSingleObject(SemFilledSpots, 100))
		{
			WaitForSingleObject(HMutex, 1);
			data->bitch.QuadPart = buffer[buffer_front].location;

			data->word[0] = buffer[buffer_front].data[0];
			data->word[1] = buffer[buffer_front].data[1];
			buffer_front = (buffer_front + 1) % buffer_size;
			data->state++;


			ReleaseSemaphore(SemEmptySpots, 1, &PrevCount);
			ReleaseMutex(HMutex);
		}
	}
	else if (data->state == 2)
	{
		if (*encoder == 'E')
		{
			Encrypt(data->word);
		}
		else if (*encoder == 'D')
		{
			Decrypt(data->word);
		}
		data->state++;
	}
	else if (data->state == 3)
	{
		WaitForSingleObject(HMutex2, 1);


		SetFilePointerEx(outfile, data->bitch, NULL, FILE_BEGIN);

		WriteFile(outfile, data->word, sizeof(data->word), &Bout, NULL);
		taskcount++;
		//data->done = TRUE;

		ReleaseMutex(HMutex2);
		
		data->state = 1;

	}
	
}
