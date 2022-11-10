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

int Consume_count;
ConsumeRec* inforrecarr;

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

	inforrecarr = malloc(numofcons * sizeof(ConsumeRec));

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
		inforrecarr[i].who = i;
		inforrecarr[i].consumecount = 0;
		inforrecarr[i].hconsumer = (HANDLE)_beginthreadex(NULL, 0, Consumer, &(inforrecarr[i]), NULL, NULL);
		if(inforrecarr[i].hconsumer == NULL)
		{
			printf("Could not start up thread# %d !", i);
		}
	}
	
	WaitForSingleObject(HProducer, INFINITE);

	for (int i = 0; i < numofcons; i++)
	{
		WaitForSingleObject(inforrecarr[i].hconsumer, INFINITE);

	}
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
	char locdata[3];
	int tempCC;
	while (TRUE)
	{
		if (!WaitForSingleObject(SemFilledSpots, 100))
		{
			WaitForSingleObject(HMutex, INFINITE);
			locdata[0] = buffer[buffer_front].data[0];
			locdata[1] = buffer[buffer_front].data[1];
			buffer_front = (buffer_front + 1) % buffer_size;
			
			ReleaseSemaphore(SemEmptySpots, 1, &PrevCount);
			ReleaseMutex(HMutex);


			if (*encoder == 'E')
			{
				Encrypt(locdata);			
			}
			else if(*encoder == 'D')
			{
				Decrypt(locdata);
			}


			WaitForSingleObject(HMutex2, INFINITE);
			LARGE_INTEGER spot;
			DWORD Bout;
			spot.QuadPart = buffer[buffer_front].location;

			SetFilePointerEx(outfile, spot, NULL, FILE_BEGIN);

			WriteFile(outfile, locdata, sizeof(locdata), &Bout, NULL);
			who->consumecount++;


			ReleaseMutex(HMutex2);			
			
		}

		tempCC = 0;
		for (int i = 0; i < numofcons; i++)
		{
			tempCC += inforrecarr[i].consumecount;
		}

		if (!ProducerAlive && (produce_count - tempCC == 0))
		{
			break;
		}

	}

}
