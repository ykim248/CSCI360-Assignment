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
//#include <cmath>

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


int partition(int Low, int High, HANDLE scottdout)
{
	int mid = (Low + High) / 2;
	
	RECORD lowkey, highkey, Pivot;
	Pivot = ReadRecord(scottdout, mid);
	lowkey = ReadRecord(scottdout, Low);
	
	WriteRecord(scottdout, mid, lowkey);

	// array[mid] = array[Low];
	while (Low != High)
	{
		
		while (Low != High)
		{	
			highkey = ReadRecord(scottdout, High);
			if (strcmp(highkey.key, Pivot.key) < 0) //the non-matching character in str1 is lower (in ASCII) than that of str2.
			{
				WriteRecord(scottdout, Low, highkey);
				Low++;
				break;
			}
			else
			{
				High--;
			}
		}
		while (Low != High)
		{
			lowkey = ReadRecord(scottdout, Low);
			if (strcmp(lowkey.key, Pivot.key) > 0) //the non-matching character in str1 is greater (in ASCII) than that of str2.
			{
				WriteRecord(scottdout, High, lowkey);
				High--;
				break;
			}
			else
			{
				Low++;
			}
		}

	}
	WriteRecord(scottdout, Low, Pivot);
	return Low;
}

void quicksort(int low, int high, HANDLE scottdout) {

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

void CooperRush(HANDLE left, HANDLE right, HANDLE scottdout)
{
	int leftidx = 0, //list1
		rightidx = 0,//list2
		dest = 0, //dest
		litems, ritems;
	RECORD lrecord, rrecord;

	GetFileSizeEx(left, &litems);
	GetFileSizeEx(right, &ritems);

	litems /= 64;
	ritems /= 64;

	while (leftidx <= litems && rightidx <= ritems)
	{
		lrecord = ReadRecord(left, leftidx);
		rrecord = ReadRecord(right, rightidx);

		//if (strcmp(highkey.key, Pivot.key) < 0) //the non-matching character in str1 is lower (in ASCII) than that of str2.
		//if (strcmp(lowkey.key, Pivot.key) > 0) //the non-matching character in str1 is greater (in ASCII) than that of str2.

		if (strcmp(lrecord.key, rrecord.key) < 0)
		{
			WriteRecord(scottdout, dest, lrecord);
			leftidx++;
			dest++;
		}
		else
		{
			WriteRecord(scottdout, dest, rrecord);
			rightidx++;
			dest++;
		}
	}
	if (leftidx == litems) {
		while (rightidx <= ritems) 
		{
			WriteRecord(scottdout, dest, rrecord);
			rightidx++;
			dest++;
		}
	}
	else 
	{
		while (leftidx <= litems)
		{
			WriteRecord(scottdout, dest, lrecord);
			leftidx++;
			dest++;
		}
	}
}
	/*void Combine(int* Source, int* Dest, int left, int mid, int right)
{
    int list1 = left;
    int list2 = mid + 1;
    int DestList = left;

    while (list1 <= mid && list2 <= right)
    {
        if (Source[list1] < Source[list2])
        {
            Dest[DestList] = Source[list1];
            list1++;
        }
        else
        {
            Dest[DestList] = Source[list2];
            list2++;
        }
        DestList++;
    }
    if (list1 > mid)
    {
        while (list2 <= right)
        {
            Dest[DestList] = Source[list2];
            DestList++;
            list2++;
        }
    }
    else
    {
        while (list1 <= mid)
        {
            Dest[DestList] = Source[list1];
            DestList++;
            list1++;
        }
    }
}*/



int _tmain(int argc, LPTSTR argv[])
{

	SECURITY_ATTRIBUTES stdOutSA = /* SA for inheritable handle. */
	{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	
	RECORD data;
	HANDLE STDInput, STDOutput;
	HANDLE hLInTempFile, hLOutTempFile, hRInTempFile, hROutTempFile;
	HANDLE leftproc, rightproc;
	LARGE_INTEGER FileSize;
	DWORD BIn, Bout;
	PROCESS_INFORMATION processInfo;
	STARTUPINFO startUpSearch, startUp, LstartUpSearch, LstartUp, RstartUpSearch, RstartUp;
	TCHAR commandLine[200];
	STDInput = GetStdHandle(STD_INPUT_HANDLE);
	STDOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	int processes = atoi(argv[1]);

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numCPU = sysinfo.dwNumberOfProcessors;

	GetFileSizeEx(STDInput, &FileSize);

	if (processes == 1)
	{

		//void insertionSort(int arr[], int n)
		//{
		//	int i, key, j;
		//	for (i = 1; i < n; i++)
		//	{
		//		key = arr[i];
		//		j = i - 1;

		//		/* Move elements of arr[0..i-1], that are
		//		greater than key, to one position ahead
		//		of their current position */
		//		while (j >= 0 && arr[j] > key)
		//		{
		//			arr[j + 1] = arr[j];
		//			j = j - 1;
		//		}
		//		arr[j + 1] = key;
		//	}
		//}


		LARGE_INTEGER spot;
		int size = FileSize.QuadPart / 64;
		int i, j;
		RECORD key, J_Item;


		for (int x = 0; x < size; x++)  // Fill the output file (You can't sort the input file!!!)
		{
			ReadFile(STDInput, &data, RECSIZE, &BIn, NULL);
			WriteFile(STDOutput, &data, RECSIZE, &Bout, NULL);
		}


		/*RECORD* keyarr = malloc(size);
		for (int x = 0; x < size; x++) {
			keyarr[x] = ReadRecord(STDOutput, x);
		}*/

		quicksort(0, size - 1, STDOutput); // pass in handle


		//for (i = 1; i < size; i++)
		//{
		//	//key = arr[i];
		//	key = ReadRecord(STDOutput, i);

		//	j = i - 1;


		//	while (j >= 0) // && arr[j] > key)
		//	{
		//	
		//		J_Item = ReadRecord(STDOutput, j);
		//		if (strcmp(J_Item.key, key.key) <= 0)
		//			break;

		//		//arr[j + 1] = arr[j];
		//		WriteRecord(STDOutput, j + 1, J_Item);
		//		
		//		j = j - 1;
		//	}

		//	//arr[j + 1] = key;
		//	WriteRecord(STDOutput, j + 1, key);
		//}



	}
	else
	{
//		if (ceil(log2(processes)) == floor(log2(processes)))
//		{

			GetStartupInfo(&startUpSearch);
			GetStartupInfo(&startUp);
			TCHAR randname[500];

			processes /= 2;

			sprintf(commandLine, "FileInsertionSort %d", processes);

			if (GetTempFileName(_T("."), _T("LIn"), 0, randname) != 0)
			{
				hLInTempFile = /* This handle is inheritable */
					CreateFile(randname,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				
			}
			else {
				printf("Error creating file name!");
				return 1;
			}

			if (GetTempFileName(_T("."), _T("LOut"), 0, randname) != 0)
			{
				hLOutTempFile = /* This handle is inheritable */
					CreateFile(randname,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			else {
				printf("Error creating file name!");
				return 1;
			}

			if (GetTempFileName(_T("."), _T("RIn"), 0, randname) != 0)
			{
				hRInTempFile = /* This handle is inheritable */
					CreateFile(randname,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


			}
			else {
				printf("Error creating file name!");
				return 1;
			}

			if (GetTempFileName(_T("."), _T("ROut"), 0, randname) != 0)
			{
				hROutTempFile = /* This handle is inheritable */
					CreateFile(randname,
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			else {
				printf("Error creating file name!");
				return 1;
			}

			int firsthalf = (FileSize.QuadPart) / 64 / 2; //gets first half
			int secondhalf = (FileSize.QuadPart) / 64;
			for (int x = 0; x < firsthalf; x++)
			{
				ReadFile(STDInput, &data, RECSIZE, &BIn, NULL);
				WriteFile(hLInTempFile, &data, RECSIZE, &Bout, NULL);
			}

			if ((FileSize.QuadPart / 64) % 2 != 0)
			{
				for (int x = 0; x <= firsthalf; x++)
				{
					ReadFile(STDInput, &data, RECSIZE, &BIn, NULL);
					WriteFile(hRInTempFile, &data, RECSIZE, &Bout, NULL);
				}
			}
			else {
				for (int x = 0; x < firsthalf; x++)
				{
					ReadFile(STDInput, &data, RECSIZE, &BIn, NULL);
					WriteFile(hRInTempFile, &data, RECSIZE, &Bout, NULL);
				}
			}

			LstartUpSearch.dwFlags = STARTF_USESTDHANDLES;
			LstartUpSearch.hStdOutput = hLOutTempFile;
			LstartUpSearch.hStdError = hLOutTempFile;
			LstartUpSearch.hStdInput = hLInTempFile;

			RstartUpSearch.dwFlags = STARTF_USESTDHANDLES;
			RstartUpSearch.hStdOutput = hROutTempFile;
			RstartUpSearch.hStdError = hROutTempFile;
			RstartUpSearch.hStdInput = hRInTempFile;

		
			LARGE_INTEGER spot;
			spot.QuadPart = 0;
			SetFilePointerEx(hLInTempFile, spot, NULL, FILE_BEGIN);
			SetFilePointerEx(hRInTempFile, spot, NULL, FILE_BEGIN);
			

			if (!CreateProcess(NULL, commandLine, NULL, NULL,
				TRUE, 0, NULL, NULL, &LstartUpSearch, &processInfo))
				printf("ProcCreate failed.");

			leftproc = processInfo.hProcess;
			
			if (!CreateProcess(NULL, commandLine, NULL, NULL,
				TRUE, 0, NULL, NULL, &RstartUpSearch, &processInfo))
				printf("ProcCreate failed.");
			
			rightproc = processInfo.hProcess;

			WaitForSingleObject(leftproc, INFINITE);
			WaitForSingleObject(rightproc, INFINITE);
			CloseHandle(hRInTempFile);
			CloseHandle(hLInTempFile);
			
			
//		}
		


	}


	return 0;
}