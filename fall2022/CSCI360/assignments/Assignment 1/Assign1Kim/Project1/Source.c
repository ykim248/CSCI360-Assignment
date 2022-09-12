#define WIN32_LEAN_AND_MEAN 

#pragma warning(disable : 4996)

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>
#include <string.h>

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif
int _tmain(int argc, LPTSTR argv[])
{
	//printf("argc %d\n", argc);
	STARTUPINFO si;
	char str[100];

	DWORD  pidarr[11];
	HANDLE hidarr[11];
	HANDLE HTemp;

	GetStartupInfo(&si);

	PROCESS_INFORMATION pi;

	if (argc > 11) {

		printf("error too many arguements\n");
		system("pause");
		return 1;

	}
	else {
		if (argc == 1) {

			for (int i = 0; i < 2; i++) {
				if (!CreateProcess(NULL,   // No module name (use command line)
					argv[0],        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,             // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&si,            // Pointer to STARTUPINFO structure
					&pi)           // Pointer to PROCESS_INFORMATION structure
					)
				{
					printf("error");
					system("pause");
					//return 1;
				}
				pidarr[i] = pi.dwProcessId;
				hidarr[i] = pi.hProcess;
			}
			WaitForSingleObject(hidarr[0], 1);
			WaitForSingleObject(hidarr[1], 1);

		}
		else {
			for (int i = 1; i < argc; i++) { 
				if (!CreateProcess(NULL,   // No module name (use command line)
					argv[i],        // Command line
					NULL,           // Process handle not inheritable
					NULL,           // Thread handle not inheritable
					FALSE,          // Set handle inheritance to FALSE
					0,             // No creation flags
					NULL,           // Use parent's environment block
					NULL,           // Use parent's starting directory 
					&si,            // Pointer to STARTUPINFO structure
					&pi)           // Pointer to PROCESS_INFORMATION structure
					)
				{
					printf("error");
					system("pause");
					//return 1;
				}
				pidarr[i] = pi.dwProcessId;
				hidarr[i] = pi.hProcess;
			}
		}		
	}

	int input;
	int counter = argc;

	do
	{
		for (int i = 1; i < counter; i++) {
			if (pidarr[i] != 0) {
				printf("%d) Terminate %s\n", i, argv[i]);
			}
		}
		printf("Q) Quit and Terminate all children\n");
		gets(str);
		
		input = atoi(str);
		if (input >= 1 && input < 11) {
			DWORD ExitCode;

			HTemp = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pidarr[input]);
			if (HTemp)
			{
				GetExitCodeProcess(HTemp, &ExitCode);
				if (ExitCode == STILL_ACTIVE)
				{
					TerminateProcess(HTemp, 1);
					pidarr[input] = 0;
				}
			}
		}		
	} while (strcmp(str, "Q") != 0);

	for (int i = 0; i < counter; i++) {

		TerminateProcess(hidarr[i], 1);
		
	}
	//TerminateProcess(pi.hProcess, 1);
	system("pause");
	return 0;
}
