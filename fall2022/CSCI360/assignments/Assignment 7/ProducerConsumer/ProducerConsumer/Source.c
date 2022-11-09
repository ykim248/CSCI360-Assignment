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

int Consume_count[3];
ConsumeRec InfoRecArr[3];
int buffer[buffer_size];
int buffer_front = 0;
int buffer_back = 0;
int buffer_count = 0;
int produce_count = 0;
BOOL ProducerAlive;

void WINAPI Producer(int);
void WINAPI Consumer(ConsumeRec *);

HANDLE SemFilledSpots;
HANDLE SemEmptySpots;
HANDLE HMutex;

int _tmain(int argc, LPTSTR argv[])
{
	SemFilledSpots = CreateSemaphore(NULL, 0, buffer_size, NULL); 
	SemEmptySpots = CreateSemaphore(NULL, buffer_size, buffer_size, NULL);
	InitializeEncoderDecoder();

	HMutex = CreateMutex(NULL, FALSE, NULL);
	
	HANDLE HProducer;
	HANDLE HConsumer;
	HANDLE HConsumer2;
	HANDLE HConsumer3;

	for (int x = 0; x< 3; x++)
	{
		Consume_count[x] = 0;
		InfoRecArr[x].ConsumeCount = Consume_count;
		InfoRecArr[x].who = x;
	}

	ProducerAlive = TRUE;
	HProducer = (HANDLE)_beginthreadex(NULL, 0, Producer, 100, NULL, NULL);

	HConsumer = (HANDLE)_beginthreadex(NULL, 0, Consumer, &InfoRecArr[0], NULL, NULL);
	HConsumer2 = (HANDLE)_beginthreadex(NULL, 0, Consumer, &InfoRecArr[1], NULL, NULL);
	HConsumer3 = (HANDLE)_beginthreadex(NULL, 0, Consumer, &InfoRecArr[2], NULL, NULL);
	
	WaitForSingleObject(HProducer, INFINITE);
	WaitForSingleObject(HConsumer, INFINITE);
	WaitForSingleObject(HConsumer2, INFINITE);
	WaitForSingleObject(HConsumer3, INFINITE);

	system("pause");
	return 0;
}

void WINAPI Producer(int count)
{
	int PrevCount;

	for (int x = 0; x < count; x++)
	{
		WaitForSingleObject(SemEmptySpots, INFINITE);  
		buffer[buffer_back] = rand() % 36578;
		//buffer_count++;
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
			who->ConsumeCount[who->who]++;
			WaitForSingleObject(HMutex, INFINITE);
			printf("consume who %d count %d %d\n", who->who, who->ConsumeCount[who->who], buffer[buffer_front]);
			buffer_front = (buffer_front + 1) % buffer_size;
			ReleaseMutex(HMutex);
			//buffer_count--;
			Sleep(2);
			ReleaseSemaphore(SemEmptySpots, 1, &PrevCount);
		}

		if (!ProducerAlive && (produce_count - (who->ConsumeCount[0]+ who->ConsumeCount[1]+ who->ConsumeCount[2]) ==0))
		{
			break;
		}

	}

}
