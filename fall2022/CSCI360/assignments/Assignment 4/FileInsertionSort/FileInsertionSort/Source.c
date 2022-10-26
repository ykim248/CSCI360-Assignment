#define WIN32_LEAN_AND_MEAN 

#define NOATOM
#define NOCLIPBOARD
#define NOCOMM
#define NOCTLMGR
#define NOCOLOR
#define NODEFERWINDOWPOS
#define NODESKTOP
#define NODRAWTEXT
#define NOEXTAPI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOTIME
#define NOIMM
#define NOKANJI
#define NOKERNEL
#define NOKEYSTATES
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMSG
#define NONCMESSAGES
#define NOPROFILER
#define NORASTEROPS
#define NORESOURCE
#define NOSCROLL
//#define NOSERVICE		/* Windows NT Services */
#define NOSHOWWINDOW
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOSYSPARAMS
#define NOTEXTMETRIC
#define NOVIRTUALKEYCODES
#define NOWH
#define NOWINDOWSTATION
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define OEMRESOURCE
#pragma warning(disable : 4996)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>
#include <math.h>

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif

#define DATALEN 56
#define KEYLEN 8
typedef struct _RECORD {
	TCHAR key[KEYLEN];
	TCHAR data[DATALEN];
} RECORD;

#define RECSIZE sizeof (RECORD)

RECORD ReadRecord(HANDLE STDInput, int spot_o) {
	DWORD BIn;
	RECORD temp;
	LARGE_INTEGER spot;
	spot.QuadPart = spot_o * 64;
	SetFilePointerEx(STDInput, spot, NULL, FILE_BEGIN);
	ReadFile(STDInput, &temp, RECSIZE, &BIn, NULL);
	return temp;
}

void WriteRecord(HANDLE STDOutput, int spot_o, RECORD to_write) {
	DWORD Bout;

	LARGE_INTEGER spot;
	spot.QuadPart = spot_o * 64;
	SetFilePointerEx(STDOutput, spot, NULL, FILE_BEGIN);
	WriteFile(STDOutput, &to_write, RECSIZE, &Bout, NULL);
	//return temp;
}


int partition(int low, int high, RECORD scottdout[])
{
	int mid = (low + high) / 2;
	RECORD pivot = scottdout[mid];
	scottdout[mid] = scottdout[low];
	while (low != high)
	{
		while (low != high)
		{
			if (strcmp(scottdout[high].key, pivot.key) < 0) //the non-matching character in str1 is lower (in ASCII) than that of str2.
			{
				scottdout[low] = scottdout[high];
				low++;
				break;
			}
			else
			{
				high--;
			}
		}
		while (low != high)
		{
			if (strcmp(scottdout[low].key, pivot.key) > 0) //the non-matching character in str1 is greater (in ASCII) than that of str2.
			{
				scottdout[high] = scottdout[low];
				high--;
				break;
			}
			else
			{
				low++;
			}
		}
	}
	scottdout[low] = pivot;
	return low;
}

void quicksort(int low, int high, RECORD scottdout[]) {

	int pi = partition(low, high, scottdout);
	if (low < (pi - 1))
	{
		quicksort(low, (pi - 1), scottdout);
	}
	if (pi + 2 <= high)
	{
		quicksort((pi + 1), high, scottdout);
	}

}

void Merge(RECORD dakprescott[], RECORD cooperrush[], int first, int mid, int end)//dak is old rush is new
{
	int list1 = first;
	int list2 = mid;
	int dest = first;

	while (list1 < mid && list2 < end)
	{
		if (strcmp(dakprescott[list1].key, dakprescott[list2].key) < 0)		//the non-matching character in str1 is lower (in ASCII) than that of str2.
		{
			cooperrush[dest] = dakprescott[list1];
			list1++;
		}
		else
		{
			cooperrush[dest] = dakprescott[list2];
			list2++;
		}
		dest++;
	}
	if (list1 > mid)
	{
		while (list2 < end)
		{
			cooperrush[dest] = dakprescott[list2];
			dest++;
			list2++;
		}
	}
	else
	{
		while (list1 < mid)
		{
			cooperrush[dest] = dakprescott[list1];
			dest++;
			list1++;
		}
	}
}

int _tmain(int argc, LPTSTR argv[])
{

	SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
	{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	typedef struct { TCHAR tempFile[MAX_PATH]; } PROCFILE;
	PROCFILE* randfilename; /* Pointer to array of temp file names. */
	
	RECORD data;
	HANDLE STDInput, STDOutput, hinstupid;
	HANDLE hLInTempFile, hLOutTempFile, hRInTempFile, hROutTempFile;
	HANDLE leftproc, rightproc;
	HANDLE mmsource = NULL;
	HANDLE mmdest = NULL;
	HANDLE fOut = NULL;
	HANDLE mmtostdout = NULL;
	HANDLE* hProc;
	LARGE_INTEGER FileSize;
	DWORD BIn, Bout;
	PROCESS_INFORMATION processInfo;
	STARTUPINFO startUpSearch, startUp, LstartUpSearch, LstartUp, RstartUpSearch, RstartUp;
	TCHAR commandLine[200];
	STDInput = GetStdHandle(STD_INPUT_HANDLE);
	STDOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numCPU = sysinfo.dwNumberOfProcessors;

	int processes;
	if (argc == 1) {
		processes = numCPU;
		if (ceil(log2(processes)) != floor(log2(processes)))
		{
			processes = pow(2, ceil(log(processes) / log(2)));
		}
	}
	else
	{
		processes = atoi(argv[1]);
		if (ceil(log2(processes)) != floor(log2(processes)))
		{
			processes = pow(2, ceil(log(processes) / log(2)));
		}
	}

	

	

	GetFileSizeEx(STDInput, &FileSize);
	hinstupid = CreateFileMapping(STDInput, &stdOutSA, PAGE_READONLY, 0, 0, NULL);


	if (processes == 1)
	{
		HANDLE hdest, hscource;
		int low, high, mid;
		if (argc == 2)
		{
			low = 0;
			high = FileSize.QuadPart / 64;
			hinstupid = CreateFileMapping(STDInput, &stdOutSA, PAGE_READONLY, 0, 0, NULL);
			mmtostdout = CreateFileMapping(STDOutput, &stdOutSA, PAGE_READWRITE, FileSize.HighPart, FileSize.LowPart, NULL);

			RECORD* pindumb = MapViewOfFile(hinstupid, FILE_MAP_READ, 0, 0, FileSize.QuadPart);
			DWORD e = GetLastError();
			RECORD* mvtostdout = MapViewOfFile(mmtostdout, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);
			e = GetLastError();
			
			for (int x = 0; x < high; x++)
			{
				mvtostdout[x] = pindumb[x];
			}

			//quicksort
			quicksort(low, high - 1, mvtostdout);

			UnmapViewOfFile(pindumb);
			UnmapViewOfFile(mvtostdout);
			CloseHandle(hinstupid);
			CloseHandle(mmtostdout);
		}
		else if (argc == 6)
		{
			//tempfile is hsource, hdest is stdout
			hdest = atoi(argv[2]);
			hscource = atoi(argv[3]);
			low = atoi(argv[4]);
			high = atoi(argv[5]);
			RECORD* rdest = MapViewOfFile(hdest, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);
			quicksort(low, high - 1, rdest);

		}
	}
	else
	{
		if (ceil(log2(processes)) == floor(log2(processes)))
		{
			
			hProc = malloc(processes * sizeof(HANDLE));

			GetStartupInfo(&startUpSearch);
			GetStartupInfo(&startUp);
			
			if (argc == 2)
			{
				fOut = CreateFile("tempfile.txt",
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, &stdOutSA,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				mmsource = CreateFileMapping(fOut, &stdOutSA, PAGE_READWRITE, FileSize.HighPart, FileSize.LowPart, NULL);
				mmdest = CreateFileMapping(STDOutput, &stdOutSA, PAGE_READWRITE, FileSize.HighPart, FileSize.LowPart, NULL);
				//tempfile is mmsource, mmdest is stdout
				//copy record to mmsource and mmdest

				int totalitems = (FileSize.QuadPart) / 64;
				int numitems = (FileSize.QuadPart) / 64 / processes;
				int extra_items = (FileSize.QuadPart) / 64 % processes;

				int low = 0;
				int high = FileSize.QuadPart / 64;
				int mid = (low + high) / 2;

				RECORD* hinsodumb = MapViewOfFile(hinstupid, FILE_MAP_READ, 0, 0, FileSize.QuadPart);
				RECORD* rsource = MapViewOfFile(mmsource, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);
				RECORD* rdest = MapViewOfFile(mmdest, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);

				for (int x = 0; x < totalitems; x++)//copy record to mmsource and mmdest
				{
					rsource[x] = hinsodumb[x];
					rdest[x] = hinsodumb[x];
				}
				// tempfile and two.txt both have data
				processes /= 2;
				int temp = low;
				int quarterback = mid;

				for (int iProc = 0; iProc < 2; iProc++)
				{
					//tempfile is mmsource, mmdest is stdout           old at this point
					//at child level result goes into tempfile         new
					//low is offset
					sprintf(commandLine, _T("FileInsertionSort %d %d %d %d %d"), processes, rsource, rdest, low, mid);

					if (!CreateProcess(NULL, commandLine, NULL, NULL,
						TRUE, 0, NULL, NULL, &startUpSearch, &processInfo))
						printf("ProcCreate failed.");

					hProc[iProc] = processInfo.hProcess;

					low = mid;
					mid = high;
				}
				WaitForMultipleObjects(processes, hProc, TRUE, INFINITE);

				Merge(rsource, rdest, temp, quarterback, high);
				UnmapViewOfFile(rdest);
				UnmapViewOfFile(rsource);
				CloseHandle(mmsource);
				CloseHandle(fOut);
				CloseHandle(mmdest);
				free(hProc);

				if (!DeleteFile("tempfile.txt"))
				{
					int e = GetLastError();
					if (e == ERROR_ACCESS_DENIED)
					{
						printf("Did not delete");
					}
				}
			}
			else
			{
				mmdest = atoi(argv[3]);
				mmsource = atoi(argv[2]);
				RECORD* rsource = MapViewOfFile(mmsource, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);
				RECORD* rdest = MapViewOfFile(mmdest, FILE_MAP_WRITE, 0, 0, FileSize.QuadPart);
				int low = atoi(argv[4]);
				int high = atoi(argv[5]);
				int mid = (low + high) / 2;
				processes /= 2;
				int temp = low;
				int quarterback = mid;

				for (int iProc = 0; iProc < 2; iProc++)
				{
					//tempfile is mmsource, mmdest is stdout           old at this point
					//at child level result goes into tempfile         new
					//low is offset
					sprintf(commandLine, _T("FileInsertionSort %d %d %d %d %d %d"), processes, rdest, rsource, low, mid);

					if (!CreateProcess(NULL, commandLine, NULL, NULL,
						TRUE, 0, NULL, NULL, &startUpSearch, &processInfo))
						printf("ProcCreate failed.");

					hProc[iProc] = processInfo.hProcess;

					low = mid;
					mid = high;
				}
				WaitForMultipleObjects(processes, hProc, TRUE, INFINITE);

				Merge(rsource, rdest, temp, quarterback, high);
				UnmapViewOfFile(rdest);
				UnmapViewOfFile(rsource);
				CloseHandle(mmsource);
				CloseHandle(fOut);
				CloseHandle(mmdest);
				free(hProc);
			}		
		}
	}

	return 0;
}