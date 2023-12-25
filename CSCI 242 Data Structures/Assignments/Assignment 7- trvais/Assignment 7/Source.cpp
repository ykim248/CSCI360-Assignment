#include "AVLTree.h"

void PrintMenu()
{
	SetCursor(40, 0); cout << "Welcome to Travis' AVL Tree!";
	SetCursor(40, 2); cout << "1) Add Node";
	SetCursor(40, 3); cout << "2) Draw Tree"; 
	SetCursor(40, 4); cout << "3) Quit";
}

int main()
{
	AVLTree Tree;
	bool Running = true;

	do
	{
		system("cls");
		PrintMenu();
		int Y = 5;
		int Input;
	
		Y++;
		SetCursor(40, Y++); cout << "Make a selection -> ";
		cin >> Input;

		switch (Input)
		{
		case 1:
			SetCursor(40, Y++); cout << "Enter a value to add -> ";
			
			int Value;
			cin >> Value;
			Tree.Add(Value);
			
			SetCursor(40, Y++); cout << Value << " has been added to the tree.";
			Y++;

			SetCursor(40, Y++); system("pause");

			break;
		case 2:
			system("cls");
			Tree.DrawTree();

			SetCursor(0, 20);
			system("pause");
			break;
		case 3:
			system("cls");
			SetCursor(40, 0); cout << "Thank you for using Travis' AVL Tree!";
			Running = false;
			SetCursor(40, 2); system("pause");
			break;
		}

	} while (Running);
}