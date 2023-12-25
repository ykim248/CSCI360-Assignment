#include <iostream>
using namespace std;

struct Node
{
    int value;
    Node* prev = NULL;
    Node* next = NULL;
};

class DLL //Double Linked List
{

private:
    Node* front = NULL;
    Node* back = NULL;

public:
    void add(int num)
    {
        Node* temp = front;
        if (front) // requires delete to set front back to Null
        {
            Node* newNode = new Node;
            newNode->value = num;

            if (num > temp->value)
            {
                while (temp->next != NULL)//at least 1 node when we get here
                {
                    if (temp->next->value > num)
                        break;
                    temp = temp->next;
                }
                //add new node after temp
                newNode->next = temp->next;
                newNode->prev = temp;
                if (temp->next)//item isnt the last in the list
                {
                    temp->next->prev = newNode;
                }
                else
                {
                    back = temp;
                }
                temp->next = newNode;

            }
            else//new node is the fornt of the list
            {
                front = newNode;
                newNode->next = temp;
                temp->prev = newNode;
            }
        }
        else 
        {
            back = front = new Node;
            front->value = num;

        }
    }

    void remove(int num)
    {
        Node* temp = front;
        while (temp)
        {
            if (temp->value == num)
            {
                if (front == temp)
                {
                    //item is in the front
                    front = temp->next;
                }
                if (back == temp)
                {
                    //items is in the back
                    back = temp->prev;
                }
                //remove the item

                if (temp->next)
                {
                    temp->next->prev = temp->prev;
                }
                if (temp -> prev)
                {
                    temp->prev->next = temp->next;
                }
                delete temp;
                temp = NULL;
                return;
            }
            temp = temp->next;
        }
    }

    void print()
    {
        Node* temp = back;
        while (temp)
        {
            cout << temp->value << " ";
            temp = temp->prev;
        }
    }

};

int main()
{
    DLL list;

    list.add(5);
    list.add(10);
    list.add(4);
    list.add(7);
    list.remove(10);
    list.print();

    return 0;
}
