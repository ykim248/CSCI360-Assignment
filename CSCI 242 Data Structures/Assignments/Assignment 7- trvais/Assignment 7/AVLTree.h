#pragma once
#include "CursorControl.h"
#include <iostream>

using namespace std;

struct Node
{
	int Value;
	int BF = 0;

	Node* Left = NULL;
	Node* Right = NULL;

	Node(int Val)
	{
		Value = Val;
	}
};


struct AVLTree
{
private:
	Node* Root = NULL;
	bool Done = false;

public:
	void Add(int Val)
	{
		Done = false;
		addRec(Root, Val);
	}

	void DrawTree()
	{
		int X = 39;		// Console is 80 columns wide. 79 (0 based) is 49).
		RecDrawTree(Root, 0, X, 79, 0);
	}

private:
	void RecDrawTree(Node* Trav, int LeftEdge, int X, int RightEdge, int Y)
	{
		if (Trav)
		{
			// Value
			SetCursor(X, Y);
			cout << Trav->Value;

			// Balance Factor
			SetCursor(X, Y + 1);
			cout << Trav->BF;

			int LeftX = (X + LeftEdge) / 2;
			int RightX = (X + RightEdge) / 2;

			RecDrawTree(Trav->Left, LeftEdge, LeftX,          X, Y + 2);
			RecDrawTree(Trav->Right,        X, RightX, RightEdge, Y + 2);
		}
	}

	void addRec(Node*& Trav, int Val)
	{
		if (!Trav)
		{
			Trav = new Node(Val);
		}
		else if (Trav->Value < Val) // Right
		{
			addRec(Trav->Right, Val);

			if (!Done)
			{
				Trav->BF++;

				if (Trav->BF == 0)
				{
					Done = true;
				}
				else if (Trav->BF == 2)
				{
					if (Trav->Right->BF == 1)
					{
						RotLeft(Trav);
						Done = true;
					}
					else if (Trav->Right->BF == -1)
					{
						DoubleRotateLeft(Trav);
						Done = true;
					}
				}
			}
		}
		else if (Trav->Value > Val) // Left
		{
			addRec(Trav->Left, Val);

			if (!Done)
			{
				Trav->BF--;

				if (Trav->BF == 0)
				{
					Done = true;
				}
				else if (Trav->BF == -2)
				{
					if (Trav->Left->BF == -1)
					{
						RotRight(Trav);
						Done = true;
					}
					else if (Trav->Left->BF == 1)
					{
						DoubleRotateRight(Trav);
						Done = true;
					}
				}
				else if (Trav->BF == 2)
				{
					if (Trav->Right->BF == 1)
					{
						RotLeft(Trav);
					}
					else if (Trav->Right->BF == -1)
					{
						DoubleRotateLeft(Trav);
					}
				}
			}
		}
	}

private:
	void RotLeft(Node*& Trav)
	{
		Node* Temp = Trav->Right->Left;
		Trav->Right->Left = Trav;
		Trav = Trav->Right;
		Trav->Left->Right = Temp;

		Trav->BF = 0;
		Trav->Left->BF = 0;
	}

	void RotRight(Node*& Trav)
	{
		Node* Temp = Trav->Left->Right;
		Trav->Left->Right= Trav;
		Trav = Trav->Left;
		Trav->Right->Left = Temp;

		Trav->BF = 0;
		Trav->Right->BF = 0;
	}

	void DoubleRotateRight(Node*& Trav)
	{
		Node* Temp = Trav;

		Trav = Temp->Left->Right;
		Temp->Left->Right = Trav->Left;
		Trav->Left = Temp->Left;
		Temp->Left = Trav->Right;
		Trav->Right = Temp;

		Trav->BF = 0;
		Trav->Right->BF = 0;
		Trav->Left->BF = 0;
	}

	void DoubleRotateLeft(Node*& Trav)
	{
		Node* Temp = Trav;

		Trav = Temp->Right->Left;
		Temp->Right->Left = Trav->Right;
		Trav->Right = Temp->Right;
		Temp->Right= Trav->Left;
		Trav->Left = Temp;

		Trav->BF = 0;
		Trav->Right->BF = 0;
		Trav->Left->BF = 0;
	}
};