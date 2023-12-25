// Draw Tree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>

using namespace std;

void goTo(int x, int y)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = x; //Column
	pos.Y = y; //Row
	SetConsoleCursorPosition(hConsole, pos);
}

int main()
{
	goTo(0, 0);
	cout << "Hello";
	goTo(5, 1);
	cout << "World" << endl;

	//Write a preorder traversal and use it to print nodes starting in the middle of the screen
	//The windows console is 80 columns wide, so put the first node at row 0 and col 38 or 39.

	system("pause");
	return 0;
}

