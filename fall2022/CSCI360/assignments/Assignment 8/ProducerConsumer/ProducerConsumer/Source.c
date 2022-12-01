#define WIN32_LEAN_AND_MEAN 

#pragma warning(disable : 4996)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>
#include <time.h>

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif

#define buffer_size 6

typedef struct _BakerRec
{
	HANDLE toBaker;
	int dbaked[4];
	int bakerid;
}BakerRec;

typedef struct _DonutBinRec
{
	HANDLE SemEmptySpots;
	HANDLE SemFilledSpots;
	HANDLE DMutex;
	int maxDcount, currDcount, soldDcount;

}DonutBinRec;

typedef struct _LineManagerRec
{
	BOOL isalive;
	int linemanagerid;
	HANDLE donutavaliable;
	HANDLE workdone;
	HANDLE hmanager;
}LineManagerRec;

typedef struct _DonutBin
{
	int darr[6];
	int front;
	int back;
	int count;
	HANDLE DLMutex;
}DonutLineRec;

typedef struct _WorkerRec
{
	HANDLE hworker;
	HANDLE workergo;
	int donuttaken[4];
	int workerid;
	int totalsold;
}WorkerRec;

SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
DonutBinRec* Dbin;
BakerRec bakingpeople[2];
DonutLineRec dline[4];
LineManagerRec lmanager[4];
WorkerRec workingpeople[6];
HANDLE GloMutex;

int bakeralivecount = 2;
int linemanageralivecount = 4;
void WINAPI baker(BakerRec*);
void WINAPI linemanager(LineManagerRec*);
void WINAPI worker(WorkerRec*);

HANDLE HMutex;
int baker1cap;
int baker2cap;
int bincap;
int duration;

int _tmain(int argc, LPTSTR argv[])
{
	//srand(time(NULL));
	printf("how many donuts can baker 1 bake? ->");
	scanf("%d", &baker1cap);
	printf("how many donuts can baker 2 bake? ->");
	scanf("%d", &baker2cap);
	printf("bin capacity? ->");
	scanf("%d", &bincap);
	printf("how long should you wait? ->");
	scanf("%d", &duration);


	HMutex = CreateMutex(NULL, FALSE, NULL);
	Dbin = malloc(4 * sizeof(DonutBinRec));

	GloMutex = CreateMutex(NULL, FALSE, NULL);

	// create threads in suspend mode
	for (int i = 0; i < 2; i++)//baker
	{
		memset(bakingpeople[i].dbaked, 0, sizeof bakingpeople[i].dbaked);//if this doesnt work then just do it staticly
		bakingpeople[i].bakerid = i;
		bakingpeople[i].toBaker = (HANDLE)_beginthreadex(NULL, 0, baker, &bakingpeople[i], CREATE_SUSPENDED, NULL);
		if (bakingpeople[i].toBaker == NULL)
		{
			printf("Could not start up thread# %d !", i);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		//dountbin
		Dbin[i].currDcount = 0;
		Dbin[i].soldDcount = 0;
		Dbin[i].maxDcount = bincap;
		Dbin[i].SemFilledSpots = CreateSemaphore(NULL, 0, Dbin[i].maxDcount, NULL);
		Dbin[i].SemEmptySpots = CreateSemaphore(NULL, Dbin[i].maxDcount, Dbin[i].maxDcount, NULL);
		Dbin[i].DMutex = CreateMutex(NULL, FALSE, NULL);

		//donutline
		dline[i].front = 0;
		dline[i].back = 0;
		dline[i].count = 0;
		dline[i].DLMutex = CreateMutex(NULL, FALSE, NULL);

		//line manager
		lmanager[i].linemanagerid = i;
		lmanager[i].isalive = TRUE;
		lmanager[i].donutavaliable = CreateEvent(NULL, FALSE, FALSE, NULL);
		lmanager[i].workdone = CreateEvent(NULL, FALSE, FALSE, NULL);
		lmanager[i].hmanager = (HANDLE)_beginthreadex(NULL, 0, linemanager, &lmanager[i], CREATE_SUSPENDED, NULL);
		if (lmanager[i].hmanager == NULL)
		{
			printf("Could not start up thread# %d !", i);
		}
	}

	
	for (int i = 0; i < 6; i++)//worker
	{
		memset(workingpeople[i].donuttaken, 0, sizeof workingpeople[i].donuttaken);//if this doesnt work then just do it staticly
		workingpeople[i].workerid = i;
		workingpeople[i].totalsold = 0;
		workingpeople[i].workergo = CreateEvent(NULL, FALSE, FALSE, NULL);
		workingpeople[i].hworker = (HANDLE)_beginthreadex(NULL, 0, worker, &workingpeople[i], CREATE_SUSPENDED, NULL);
		if (workingpeople[i].hworker == NULL)
		{
			printf("Could not start up thread# %d !", i);
		}
	}
	ResumeThread(bakingpeople[0].toBaker);
	ResumeThread(bakingpeople[1].toBaker);

	for (int i = 0; i < 4; i++)
	{
		ResumeThread(lmanager[i].hmanager);
	}

	for (int i = 0; i < 6; i++)
	{
		ResumeThread(workingpeople[i].hworker);
	}

	WaitForSingleObject(bakingpeople[0].toBaker, INFINITE);
	WaitForSingleObject(bakingpeople[1].toBaker, INFINITE);

	for (int i = 0; i < 4; i++)
	{
		WaitForSingleObject(lmanager[i].hmanager, INFINITE);
	}
	
	for (int i = 0; i < 6; i++)
	{
		WaitForSingleObject(workingpeople[i].hworker, INFINITE);
	}

	/*ResumeThread(bakingpeople[0].toBaker);
	ResumeThread(lmanager[0].hmanager);
	ResumeThread(workingpeople[0].hworker);
	ResumeThread(workingpeople[1].hworker);
	WaitForSingleObject(bakingpeople[0].toBaker, INFINITE);
	WaitForSingleObject(lmanager[0].hmanager, INFINITE);
	WaitForSingleObject(workingpeople[0].hworker, INFINITE);
	WaitForSingleObject(workingpeople[1].hworker, INFINITE);*/

	printf("Bakers\n");
	printf("Baker 1 \n");
	printf("made %d donuts \n", baker1cap);
	for (int i = 0; i < 4; i++)
	{
		printf("%d type %d \n", bakingpeople[0].dbaked[i], i);
	}
	printf("\n");

	printf("Baker 2 \n");
	printf("made %d donuts \n", baker2cap);
	for (int i = 0; i < 4; i++)
	{
		printf("%d type %d \n", bakingpeople[1].dbaked[i], i);
	}
	printf("\n");

	printf("Bin\n");
	for (int i = 0; i < 4; i++)
	{
		printf("Bin %d \n", i + 1);
		printf("%d donuts \n", Dbin[i].soldDcount);
		printf("\n");
	}

	printf("workers\n");
	for (int i = 0; i < 6; i++)
	{
		printf("Worker %d \n", i + 1);
		printf("Sold %d donuts \n", workingpeople[i].totalsold);
		for (int j = 0; j < 4; j++)
		{
			printf("%d type %d \n", workingpeople[i].donuttaken[j], j);
		}
		printf("\n");
	}

	system("pause");
}

void WINAPI baker(BakerRec* who)
{
	int totaldonuts = 0;
	int donutcap = 0;
	int PrevCount;
	if (who->bakerid == 0) {
		totaldonuts = who->dbaked[0] + who->dbaked[1] + who->dbaked[2] + who->dbaked[3];
		donutcap = baker1cap;
	}
	else
	{
		totaldonuts = who->dbaked[0] + who->dbaked[1] + who->dbaked[2] + who->dbaked[3];
		donutcap = baker2cap;
	}
	srand(time(NULL));
	while (totaldonuts < donutcap)
	{
		int numDtomake = rand() % 4;//thisd should 4
		if (!WaitForSingleObject(Dbin[numDtomake].SemEmptySpots, duration))
		{
			Sleep(5);
			WaitForSingleObject(Dbin[numDtomake].DMutex, INFINITE);
			Dbin[numDtomake].currDcount++;
			ReleaseMutex((Dbin[numDtomake].DMutex));
			ReleaseSemaphore(Dbin[numDtomake].SemFilledSpots, 1, &PrevCount);
			who->dbaked[numDtomake]++;
			//insert event
			SetEvent(lmanager[numDtomake].donutavaliable);
			totaldonuts++;
		}		
	}
	WaitForSingleObject(HMutex, INFINITE);
	bakeralivecount--;
	if (bakeralivecount == 0) {
		//SetEvent(lmanager[0].donutavaliable);
		for (int i = 0; i < 4; i++)
		{
			SetEvent(lmanager[i].donutavaliable);
		}
	}
	
	ReleaseMutex(HMutex);
	//printf("baker %d went home \n", who->bakerid);
	
}
void WINAPI worker(WorkerRec* workerwho)
{
	srand(time(NULL));
	int PrevCount;
	int temp;
	int randD;
	while (linemanageralivecount > 0)
	{
		randD = rand() % 4;//this should be 4
		
		if (lmanager[randD].isalive)
		{
			WaitForSingleObject(dline[randD].DLMutex, INFINITE);			
			dline[randD].darr[dline[randD].back] = workerwho->workerid;
			
			dline[randD].count++;
			dline[randD].back++;
			if (dline[randD].back == 6) {
				dline[randD].back = 0;
			}
			ReleaseMutex(dline[randD].DLMutex);
			WaitForSingleObject(workingpeople[workerwho->workerid].workergo, INFINITE);
			if (!WaitForSingleObject(Dbin[randD].SemFilledSpots, duration))
			{
				WaitForSingleObject(Dbin[randD].DMutex, INFINITE);
				Dbin[randD].currDcount--;
				Dbin[randD].soldDcount++;
				ReleaseMutex(Dbin[randD].DMutex);
				workingpeople[workerwho->workerid].donuttaken[randD]++;
				workingpeople[workerwho->workerid].totalsold++;
				ReleaseSemaphore(Dbin[randD].SemEmptySpots, 1, &PrevCount);
				SetEvent(lmanager[randD].workdone);
			}
			else
			{
				SetEvent(lmanager[randD].workdone);
			}
		}
	}
	//lmanager[randD].isalive = FALSE;
	//printf("worker %d went home \n", workerwho->workerid);
}
void WINAPI linemanager(LineManagerRec* linewho)
{
	while (bakeralivecount > 0 || Dbin[linewho->linemanagerid].currDcount > 0)// or donut in manager bin
	{
		WaitForSingleObject(lmanager[linewho->linemanagerid].donutavaliable, INFINITE);
		//lock down if statement
		if (dline[linewho->linemanagerid].count > 0)
		{
			
			int temp = dline[linewho->linemanagerid].darr[dline[linewho->linemanagerid].front];
			dline[linewho->linemanagerid].front++;	
			if (dline[linewho->linemanagerid].front == 6)
			{
				dline[linewho->linemanagerid].front = 0;
			}
			//dline[linewho->linemanagerid].count--;
			//lock line start here.
			WaitForSingleObject(dline[linewho->linemanagerid].DLMutex, INFINITE);//you need to fix this not global mutex
			dline[linewho->linemanagerid].count--;
			ReleaseMutex(dline[linewho->linemanagerid].DLMutex);
			SetEvent(workingpeople[temp].workergo);
			WaitForSingleObject(lmanager[linewho->linemanagerid].workdone, INFINITE);
			if (Dbin[linewho->linemanagerid].currDcount > 0)
			{
				SetEvent(lmanager[linewho->linemanagerid].donutavaliable);
			}
		}
		else
		{
			SetEvent(lmanager[linewho->linemanagerid].donutavaliable);
		}
	}

	WaitForSingleObject(GloMutex, INFINITE);
	linemanageralivecount--;
	lmanager[linewho->linemanagerid].isalive = FALSE;
	ReleaseMutex(GloMutex);
	while (dline[linewho->linemanagerid].count > 0)
	{
		int temp = dline[linewho->linemanagerid].darr[dline[linewho->linemanagerid].front];
		dline[linewho->linemanagerid].front++;
		dline[linewho->linemanagerid].count--;
		if (dline[linewho->linemanagerid].front == 6)
		{
			dline[linewho->linemanagerid].front = 0;
		}
		SetEvent(workingpeople[temp].workergo);
	}
	//printf("line %d went home \n", linewho->linemanagerid);
}
