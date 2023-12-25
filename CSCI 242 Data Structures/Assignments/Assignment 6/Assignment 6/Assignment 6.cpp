#include <iostream>
using namespace std;

struct Node
{
    Node* left = NULL;
    Node* right = NULL;
    int value = 0;
};

class Btree
{
private:
    Node* root = NULL;

    void DeleteRec(Node** travel, int num)
    {
        if ((*travel)->value == num)
        {
            Node* Save;

            if ((*travel)->left && (*travel)->right)
            {
                Node** temp = &((*travel)->left);
                Node** Parent = travel;
                int count = 0;

                while (&(*temp)->right)
                {
                    if (count == 0)
                    {
                        Parent = &(*Parent)->left;
                    }
                    else if (count > 0)
                    {
                        Parent = &(*Parent)->right;
                    }
                    temp = &(*temp)->right;
                    count++;
                }

                (*travel)->value = (*temp)->value;

                Save = (*temp)->left;

                if (Save)
                {
                    if (count > 1)
                        (*Parent)->right = Save;
                    else
                        (*Parent)->left = Save;
                }
                delete* temp;
                *temp = Save;
            }
            else if ((*travel)->left && !(*travel)->right)
            {
                Save = (*travel)->left;

                delete* travel;
                *travel = Save;
            }
            else if (!(*travel)->left && (*travel)->right)
            {
                Save = (*travel)->right;

                delete* travel;
                *travel = Save;
            }
            else
            {
                delete* travel;
                *travel = NULL;
            }
        }

        else if ((*travel)->left && (*travel)->value > num)
        {
            DeleteRec(&((*travel)->left), num);
        }

        else if ((*travel)->right && (*travel)->value < num)
        {
            DeleteRec(&((*travel)->right), num);
        }

        else
        {
            cout << "Number not found" << endl;
        }
    }

    bool recSearch(Node* temp, int num)
    {
        if (temp->value == num)
        {
            return true;
        }
        else if (temp->left && num <= temp->value)
        {
            return recSearch(temp->left, num);

        }
        else if (temp->right && num > temp->value)
        {
            return recSearch(temp->right, num);
        }
        else
        {
            return false;
        }
    }

    void recinorder(Node* temp)
    {
        if (temp->left) 
        {
            recinorder(temp->left);
        }

        cout << temp->value << " ";
        

        if (temp->right)
        {
            recinorder(temp->right);
        }
    }

    void recClear(Node* travel)
    {
        if (travel->left)
        {
            recClear(travel->left);            
        }       

        if (travel->right)
        {
            recClear(travel->right);            
        }

        delete travel;
        travel = NULL;      
    }
    
public:
    void Add(int val)
    {
        if (!(root))
        {
            root = new Node;
            root->value = val;
        }
        else
        {
            Node* Travel = root;
            while (true)
            {
                if (Travel->value > val)
                {
                    if (!(Travel->left))
                    {
                        Travel->left = new Node;
                        Travel->left->value = val;
                        break;
                    }
                    else
                    {
                        Travel = Travel->left;
                    }
                }
                else
                {
                    if (!(Travel->right))
                    {
                        Travel->right = new Node;
                        Travel->right->value = val;
                        break;
                    }
                    else
                    {
                        Travel = Travel->right;
                    }
                }
            }
        }
    }

    void Delete(int num)
    {
        DeleteRec(&root, num);
    }

    void InOrder()
    {
        recinorder(root);
    }

    bool Search(int num)
    {
        bool Found = recSearch(root, num);

        if (Found)
        {
            cout << "The number was found.";
        }
        else
        {
            cout << "The number was not found.";
        }

        return Found;
    }

    void Clear()
    {
        recClear(root);
        cout << "tree has been Cleared";
    }
};

int main()
{
    Btree Tree;
    Tree.Add(50);
    Tree.Add(25);
    Tree.Add(10);
    Tree.Add(60);
    Tree.Add(70);
    Tree.InOrder();
    cout << endl;
    Tree.Search(60);

    Tree.Delete(60);
    cout << endl;

    Tree.InOrder();
    cout << endl;

    Tree.Clear();
}

