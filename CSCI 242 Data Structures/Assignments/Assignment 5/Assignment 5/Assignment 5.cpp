#include <iostream>
#include <fstream>

using namespace std;

struct sBoard 
{
    signed short board[6][6];
    sBoard()
    {
        //loop(0 to 5) and assign 0s everywhere
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                board[i][j] = 0;
            }
        }
    }
};

const unsigned short LEFT = 1;
const unsigned short RIGHT = 2;
const unsigned short UP = 4;
const unsigned short DOWN = 8;

bool operator ==(sBoard ls, sBoard rs)
{
    bool SameBoard = true;

    for (int Row = 0; ((Row < 6) && SameBoard); Row++)
    {
        for (int Col = 0; ((Col < 6) && SameBoard); Col++)
        {
            if (ls.board[Row][Col] != rs.board[Row][Col])
            {
                SameBoard = false;
            }
        }
    }

    return SameBoard;
}

class MarbleGame
{
private:

    unsigned short walls[6][6];
    sBoard boardArray[100];
    int size, balls, wallcount;
    int best = INT_MAX;

    bool rollRight(sBoard& tempBoard) 
    {
        int col, row;
        for (col = (size - 2); col >= 0; col--)
        {
            for (row = 0; row < size; row++)
            {
                if (tempBoard.board[row][col] > 0)
                {
                    for (int tempcol = col; tempcol < size - 1; tempcol++)
                    {
                        if (!(walls[row][tempcol] & RIGHT))
                        {
                            if (tempBoard.board[row][tempcol + 1] < 0)
                            {
                                if ((tempBoard.board[row][tempcol] + tempBoard.board[row][tempcol + 1]) == 0)
                                {
                                    tempBoard.board[row][tempcol] = 0;
                                    tempBoard.board[row][tempcol + 1] = 0;
                                    break;
                                }
                                else
                                {
                                    return false;
                                }                            
                            }
                            else if (tempBoard.board[row][tempcol + 1] == 0)
                            {                                
                                tempBoard.board[row][tempcol + 1] = tempBoard.board[row][tempcol];
                                tempBoard.board[row][tempcol] = 0;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }


    bool rollLeft(sBoard& tempBoard)
    {
        int col, row;
        for (col = 1; col < size; col++)
        {
            for (row = 0; row < size; row++)
            {
                if (tempBoard.board[row][col] > 0)
                {
                    for (int tempcol = col; tempcol > 0; tempcol--)
                    {
                        if (!(walls[row][tempcol] & LEFT))
                        {
                            if (tempBoard.board[row][tempcol - 1] < 0)
                            {
                                if ((tempBoard.board[row][tempcol] + tempBoard.board[row][tempcol - 1]) == 0)
                                {
                                    tempBoard.board[row][tempcol] = 0;
                                    tempBoard.board[row][tempcol - 1] = 0;
                                    break;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else if (tempBoard.board[row][tempcol - 1] == 0)
                            {
                                tempBoard.board[row][tempcol - 1] = tempBoard.board[row][tempcol];
                                tempBoard.board[row][tempcol] = 0;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }
    
    bool rollUP(sBoard& tempBoard) 
    {
        int col, row;
        for (col = 0; col < size; col++)
        {
            for (row = 1; row < size; row++)
            {
                if (tempBoard.board[row][col] > 0)
                {
                    for (int temprow = row; row > 0; temprow--)
                    {
                        if (!(walls[temprow][col] & UP))
                        {
                            if (tempBoard.board[temprow-1][col] < 0)
                            {
                                if ((tempBoard.board[temprow][col] + tempBoard.board[temprow - 1][col]) == 0)
                                {
                                    tempBoard.board[temprow][col] = 0;
                                    tempBoard.board[temprow-1][col] = 0;
                                    break;
                                }
                                else
                                {
                                    return false;
                                }                            
                            }
                            else if (tempBoard.board[temprow - 1][col] == 0)
                            {                                
                                tempBoard.board[temprow - 1][col] = tempBoard.board[temprow][col];
                                tempBoard.board[temprow][col] = 0;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool rollDown(sBoard& tempBoard)
    {
        int col, row;
        for (col = 0; col < size; col++)
        {
            for (row = (size - 2); row >= 0; row--)
            {
                if (tempBoard.board[row][col] > 0)
                {
                    for (int temprow = row; row <size; temprow++)
                    {
                        if (!(walls[temprow][col] & DOWN))
                        {
                            if (tempBoard.board[temprow + 1][col] < 0)
                            {
                                if ((tempBoard.board[temprow][col] + tempBoard.board[temprow + 1][col]) == 0)
                                {
                                    tempBoard.board[temprow][col] = 0;
                                    tempBoard.board[temprow + 1][col] = 0;
                                    break;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else if (tempBoard.board[temprow + 1][col] == 0)
                            {
                                tempBoard.board[temprow + 1][col] = tempBoard.board[temprow][col];
                                tempBoard.board[temprow][col] = 0;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool isSolution(sBoard tempBoard)
    {
        for (int row = 0; row < size; row++)
        {
            for (int col = 0; col < size; col++)
            {
                if (tempBoard.board[row][col] != 0)
                {
                    return false;
                }
            }
        }

        return true;
    }
    bool boardSeen(sBoard tempBoard, int Level)
    {
        for (int i = 0; i <= Level; i++)
        {
            if (tempBoard == boardArray[i])
                return true;
        }

        return false;
    }


public:

    void readFromFile() {
        ifstream infile("test5 .txt");
        infile >> size >> balls >> wallcount;

        for (int num = 0; num < balls; num++)
        {
            int row, col;
            infile >> row >> col;
            boardArray[0].board[row][col] = num + 1;
        }

        for (int num = 0; num > -balls; num--)
        {
            int row, col;
            infile >> row >> col;
            boardArray[0].board[row][col] = num - 1;
        }

        for (int X = 0; X < 6; X++)
        {
            for (int Y = 0; Y < 6; Y++)
            {
                walls[X][Y] = 0;
            }
        }

        for (int i = 0; i < size; i++)
        {
            walls[0][i] |= UP;
            walls[i][0] |= LEFT;
            walls[size - 1][i] |= DOWN;
            walls[i][size - 1] |= RIGHT; 
        }

        for (int num = 0; num < wallcount; num++)
        {
            int r1, r2, c1, c2;
            infile >> r1 >> c1 >> r2 >> c2;
            if (r1 == r2)
            {
                if (c1 > c2)
                {
                    walls[r1][c1] |= LEFT;
                    walls[r2][c2] |= RIGHT;
                }
                else
                {
                    walls[r1][c1] |= RIGHT;
                    walls[r2][c2] |= LEFT;
                }
            }
            else if (c1 == c2)
            {
                if (r1 > r2)
                {
                    walls[r1][c1] |= UP;
                    walls[r2][c2] |= DOWN;
                }
                else
                {
                    walls[r1][c1] |= DOWN;
                    walls[r2][c2] |= UP;
                }
            }
        }
    }

    void Roll(int level)
    {
        if (level == best)
        {
            return;
        }
        else if (isSolution(boardArray[level]))
        {
            if (level < best)
            {
                best = level;
            }
        }

        sBoard tempBoard = boardArray[level];

        if (rollRight(tempBoard))
        {
            if (!boardSeen(tempBoard, level))
            {
                boardArray[level + 1] = tempBoard;
                Roll(level + 1);
            }
        }

        tempBoard = boardArray[level];

        if (rollLeft(tempBoard))
        {
            if (!boardSeen(tempBoard, level))
            {
                boardArray[level + 1] = tempBoard;
                Roll(level + 1);
            }
        }

        tempBoard = boardArray[level];

        if (rollUP(tempBoard))
        {
            if (!boardSeen(tempBoard, level))
            {
                boardArray[level + 1] = tempBoard;
                Roll(level + 1);
            }
        }

        tempBoard = boardArray[level];

        if (rollDown(tempBoard))
        {
            if (!boardSeen(tempBoard, level))
            {
                boardArray[level + 1] = tempBoard;
                Roll(level + 1);
            }
        }
    }


    void Solve()
    {
        Roll(0);

        if (best < INT_MAX)
            cout << "least amount of moves is " << best << endl;
        else
            cout << "No solution" << endl;
    }
};


int main()
{
    MarbleGame Game;

    Game.readFromFile();
    Game.Solve();

    return 0;

}


