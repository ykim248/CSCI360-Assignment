#include <iostream>
#include "CursorControl.h"
#include "AVLTree.h"
using namespace std;

int main()
{
	AVLtree Tree;

	bool Running = true;
	int selection;

	while (Running)
	{
		cout << "1) Add item" << endl;
		cout << "2) Print in Order" << endl;
		cout << "3) Show Tree" << endl;
		cout << "4) Clear" << endl;
		cout << "5) Exit" << endl;
		cout << "Make a Selection -> ";

		cin >> selection;

		int input;

		switch (selection)
		{
		case 1:

			system("cls");
			cout << "Enter a number to add -> ";
			cin >> input;

			Tree.Add(input);
			cout << input << " has been added" << endl;
			system("pause");
			system("cls");
			break;

		case 2:

			system("cls");
			Tree.InOrder();
			system("pause");
			system("cls");
			break;

		case 3:

			system("cls");
			Tree.ShowTree();
			system("pause");
			system("cls");
			break;

		case 4:

			system("cls");
			Tree.Clear();
			system("pause");
			system("cls");
			break;

		case 5:

			system("cls");
			cout << "Thank you Professor for everything this semester, Sorry if we were too much at times" << endl;
			Running = false;
			system("pause");
			break;
		}
	}


}

