
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

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif

struct ProcessData
{
	HANDLE HChildProc;
	HANDLE HChildThread;
	BOOL Alive;
	int Done;
	int processID;
	char name[20];
	FILETIME lpCreationTime;
	FILETIME lpExitTime;
	FILETIME lpKernelTime;
	FILETIME lpUserTime;
};

void GetCommandArg(char s[], char c[], char a[])
{
	int x = 0;
	int y = 0;
	while (s[x] != ' ' && s[x] != 0)
	{
		c[x] = s[x];
		x++;
	}
	c[x] = 0;
	if (s[x] == 0) 
		return;
	x++;
	
	while (s[x] != ' ' && s[x] != 0)
	{
		a[y] = s[x];
		x++;
		y++;
	}
	a[y] = 0;
}

int _tmain(int argc, LPTSTR argv[])
{

	STARTUPINFO si;
	char str[100];
	char command[100];
	char argument[100];
	char msg[100];
	struct ProcessData ProcessArray[20];
	GetStartupInfo(&si);
	int option;
	HANDLE temp;
	DWORD Exitcode = 0;

	PROCESS_INFORMATION pi;

	FILE *ptr_myfile;
	//*** setup file ****

	/*ptr_myfile = fopen("test.bin", "wb");
	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		return 1;
	}
	for (int x = 0; x < 20; x++)
	{
		ProcessArray[x].Alive = FALSE;
		ProcessArray[x].HChildProc = NULL;
		ProcessArray[x].HChildThread = NULL;
		ProcessArray[x].processID = 0;
		ProcessArray[x].Done = 0;
		strcpy(ProcessArray[x].name, "");
	}

	for (int x = 0; x < 20; x++)
	{
		temp = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcessArray[x].processID);
		if (temp)
		{
			GetExitCodeProcess(temp, &Exitcode);
			if (Exitcode != STILL_ACTIVE && ProcessArray[x].Done == 1)
			{
				ProcessArray[x].Alive = FALSE;
				ProcessArray[x].Done = 0;
			}
		}
		fwrite(&ProcessArray[x], sizeof(struct ProcessData), 1, ptr_myfile);
	}

	fclose(ptr_myfile);

	return;*/
	

	////*** setup file ****



	ptr_myfile = fopen("test.bin", "rb");
	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		//return 1;
	}
	for (int x = 0; x < 20; x++)
	{
		fread(&ProcessArray[x], sizeof(struct ProcessData), 1, ptr_myfile);
	}

	fclose(ptr_myfile);

	

	int y = 0;
	for (int x = 0; y < argc-1; x++)
	{
		if (ProcessArray[x].Done == 0)
		{
			ProcessArray[x].Alive = FALSE;
			ProcessArray[x].HChildProc = NULL;
			ProcessArray[x].HChildThread = NULL;
			ProcessArray[x].processID = 0;
			strcpy(ProcessArray[x].name, argv[y + 1]);
			


			if (!CreateProcess(NULL,   // No module name (use command line)
				argv[y + 1],        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				CREATE_SUSPENDED,         // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi)           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("Could not start process!\n");
			}
			else
			{
				ProcessArray[x].Alive = TRUE;
				ProcessArray[x].Done = 1;
				ProcessArray[x].HChildProc = pi.hProcess;
				ProcessArray[x].HChildThread = pi.hThread;
				ProcessArray[x].processID = pi.dwProcessId;
				GetProcessTimes(ProcessArray[x].HChildProc, 
					&(ProcessArray[x].lpCreationTime),
					&(ProcessArray[x].lpExitTime), 
					&ProcessArray[x].lpKernelTime,
					&ProcessArray[x].lpUserTime
				);
				ResumeThread(ProcessArray[x].HChildThread);
				y++;
			}
		}
	}


	do
	{
		printf(">");
		gets(str);
		GetCommandArg(str, command, argument);
		if (strcmp(command, "ps") == 0)
		{
			for (int x = 0; x < 20; x++)
			{
				temp = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcessArray[x].processID);
				if (temp)
				{
					GetExitCodeProcess(temp, &Exitcode);
					if (Exitcode != STILL_ACTIVE && ProcessArray[x].Done==1)
					{
						ProcessArray[x].Alive = FALSE;
						ProcessArray[x].Done = 2;
					}
					else {
						FILETIME TempCreation;
						FILETIME TempExit;
						FILETIME TempKernal;
						FILETIME TempUser;
						if (GetProcessTimes(temp, &TempCreation, &TempExit, &TempKernal, &TempUser)) {
							if (CompareFileTime(&(ProcessArray[x].lpCreationTime), &TempCreation) != 0 && ProcessArray[x].Done != 0) {
								ProcessArray[x].Done = 2;
								ProcessArray[x].Alive = FALSE;
							}
						}
					}
				}
				else
				{
					if (ProcessArray[x].Alive)
					{
						ProcessArray[x].Alive = FALSE;
						ProcessArray[x].Done = 2;
					}
				}
				if (ProcessArray[x].Alive || ProcessArray[x].Done==1 || ProcessArray[x].Done == 2)
				{
					if (ProcessArray[x].Done == 2)
					{
						ProcessArray[x].Done = 0;
						strcpy(msg, "Done");
					}
					else {
						strcpy(msg, "");
					}						
					printf("#%d(%s) (%d) %s\n", x + 1, ProcessArray[x].name, ProcessArray[x].processID, msg);

					if (ProcessArray[x].Done == 0) {
						ProcessArray[x].Alive = FALSE;
						ProcessArray[x].HChildProc = NULL;
						ProcessArray[x].HChildThread = NULL;
						ProcessArray[x].processID = 0;
						ProcessArray[x].Done = 0;
						strcpy(ProcessArray[x].name, "");
					}
					
				}
			}
			 
		}
		else if (strcmp(command, "kill") == 0)
		{
			int processid = atoi(argument)-1;
			temp = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcessArray[processid].processID);
			if (temp)
			{
				FILETIME TempCreation;
				FILETIME TempExit;
				FILETIME TempKernal;
				FILETIME TempUser;
				if (GetProcessTimes(temp, &TempCreation, &TempExit, &TempKernal, &TempUser)) {
					if (CompareFileTime(&(ProcessArray[processid].lpCreationTime), &TempCreation) == 0) {
						if (TerminateProcess(temp, 1))
						{
							ProcessArray[processid].Done = 2;
							ProcessArray[processid].Alive = FALSE;
						}
					}
					else {
						ProcessArray[processid].Done = 2;
						printf("not running\n");
					}
				}
				
			}
		}
		else if (strcmp(command, "Quit") == 0)
		{

		}
		else
		{
			if (!CreateProcess(NULL,   // No module name (use command line)
				command,        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi)           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("Could not start process (%s)!\n",command);
			}
			else
			{
				for (int x = 0; x < 20; x++)
				{
					if (ProcessArray[x].Done == 0)
					{
						ProcessArray[x].Alive = TRUE;
						ProcessArray[x].Done = 1;
						ProcessArray[x].HChildProc = pi.hProcess;
						ProcessArray[x].HChildThread = pi.hThread;
						ProcessArray[x].processID = pi.dwProcessId;
						GetProcessTimes(ProcessArray[x].HChildProc,
							&(ProcessArray[x].lpCreationTime),
							&(ProcessArray[x].lpExitTime),
							&ProcessArray[x].lpKernelTime,
							&ProcessArray[x].lpUserTime
						);
						strcpy(ProcessArray[x].name, command);
						break;

					}
				}
			}

		}
	} while (strcmp(command, "Quit") != 0);

	//FILE *ptr_myfile;
	ptr_myfile = fopen("test.bin", "wb");
	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		return 1;
	}
	for (int x = 0; x < 20; x++)
	{
		temp = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ProcessArray[x].processID);
		if (temp)
		{
			GetExitCodeProcess(temp, &Exitcode);
			if (Exitcode != STILL_ACTIVE && ProcessArray[x].Done == 1)
			{
				ProcessArray[x].Alive = FALSE;
				ProcessArray[x].Done = 0;
			}
		}
		fwrite(&ProcessArray[x], sizeof(struct ProcessData), 1, ptr_myfile);
	}

	fclose(ptr_myfile);



	return 0;
}