// Timing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <time.h> //ctime
#include <sys/timeb.h> //_timeb _ftime_s

using namespace std;

int main()
{
	struct _timeb timebuffer;
	char timeline[26];

	_ftime_s(&timebuffer);
	ctime_s(timeline,sizeof(timeline), &(timebuffer.time));

	printf("The time is %.19s.%hu %s", timeline, timebuffer.millitm, &timeline[20]);

	system("pause");
	return 0;
}

