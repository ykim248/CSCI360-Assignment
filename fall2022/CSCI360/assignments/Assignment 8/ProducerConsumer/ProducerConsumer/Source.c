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
int baker1cap = 10;
int baker2cap = 10;
int duration = 100;

int _tmain(int argc, LPTSTR argv[])
{
	//srand(time(NULL));
	HMutex = CreateMutex(NULL, FALSE, NULL);
	Dbin = malloc(4 * sizeof(DonutBinRec));

	GloMutex = CreateMutex(NULL, FALSE, NULL);

	// create threads in suspend mode
	for (int i = 0; i < 2; i++)//baker
	{
		memset(bakingpeople[i].dbaked, 0, sizeof bakingpeople[i].dbaked);//if this doesnt work then just do it staticly
		bakingpeople[i].bakerid = i;
		bakingpeople[i].toBaker = (HANDLE)_beginthreadex(NULL, 0, baker, &bakingpeople[i], NULL, NULL);
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
		Dbin[i].maxDcount = 10;
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
		lmanager[i].hmanager = (HANDLE)_beginthreadex(NULL, 0, linemanager, &lmanager[i], NULL, NULL);
		if (lmanager[i].hmanager == NULL)
		{
			printf("Could not start up thread# %d !", i);
		}
	}

	
	for (int i = 0; i < 6; i++)//worker
	{
		memset(workingpeople[i].donuttaken, 0, sizeof workingpeople[i].donuttaken);//if this doesnt work then just do it staticly
		workingpeople[i].workerid = i;
		workingpeople[i].workergo = CreateEvent(NULL, FALSE, FALSE, NULL);
		workingpeople[i].hworker = (HANDLE)_beginthreadex(NULL, 0, worker, &workingpeople[i], NULL, NULL);
		if (workingpeople[i].hworker == NULL)
		{
			printf("Could not start up thread# %d !", i);
		}
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
	while (totaldonuts <= donutcap)
	{
		int numDtomake = rand() % 4;
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
		}
	}
	WaitForSingleObject(HMutex, INFINITE);
	bakeralivecount--;
	if (bakeralivecount == 0) {
		for (int i = 0; i < 4; i++)
		{
			SetEvent(lmanager[i].donutavaliable);
		}
	}
	ReleaseMutex(HMutex);
	
}
void WINAPI worker(WorkerRec* workerwho)
{
	srand(time(NULL));
	int PrevCount;
	int temp;
	int randD;
	while (linemanageralivecount > 0)
	{
		randD = rand() % 4;
		
		if (lmanager[randD].isalive)
		{
			WaitForSingleObject(dline[randD].DLMutex, INFINITE);
			dline[randD].darr[dline->back] = workerwho->workerid;
			dline[randD].count++;
			dline[randD].back++;
			ReleaseMutex(dline[randD].DLMutex);
			WaitForSingleObject(workerwho->workergo, INFINITE);
			if (WaitForSingleObject(Dbin[randD].SemFilledSpots, duration))
			{
				WaitForSingleObject(Dbin[randD].DMutex, INFINITE);
				Dbin[randD].currDcount--;
				Dbin[randD].soldDcount++;
				ReleaseMutex(Dbin[randD].DMutex);
				workerwho->donuttaken[randD]++;
				ReleaseSemaphore(Dbin[randD].SemEmptySpots, 1, &PrevCount);
				SetEvent(lmanager[randD].workdone);
			}
		}
	}
	lmanager[randD].isalive = FALSE;
}
void WINAPI linemanager(LineManagerRec* linewho)
{
	while (bakeralivecount > 0 || Dbin[linewho->linemanagerid].currDcount > 0)// or donut in manager bin
	{
		WaitForSingleObject(linewho->donutavaliable, INFINITE);
		//lock down if statement
		if (dline[linewho->linemanagerid].count > 0)
		{
			int temp = dline[linewho->linemanagerid].front;
			dline[linewho->linemanagerid].front++;	
			dline[linewho->linemanagerid].count--;
			//lock line start here.
			WaitForSingleObject(dline[linewho->linemanagerid].DLMutex, INFINITE);//you need to fix this not global mutex
			dline[linewho->linemanagerid].count--;
			ReleaseMutex(dline[linewho->linemanagerid].DLMutex);
			SetEvent(workingpeople[temp].workergo);
			WaitForSingleObject(workingpeople[temp].workergo, INFINITE);
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
		dline[linewho->linemanagerid].front++;
		dline[linewho->linemanagerid].count--;
		SetEvent(workingpeople[linewho->linemanagerid].workergo);
	}
	
}
