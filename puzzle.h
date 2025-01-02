#include <string>
#include <iostream>

using namespace std;


enum heuristicFunction{misplacedTiles, manhattanDistance};


class Puzzle{
        
private:
    string path;            // current path taken
    int pathLength;         // length of the path taken so far, also used as gCost as every move cost = 1.
    int hCost;              // used by A*
    int fCost;              // used by A*
        
    int goalBoard[3][3];    // GOAL board configuration
    int x0, y0;             //coordinates of the blank/0 tile
    int board[3][3];        // CURRENT board config


public:
    
    static pair<int, int> goalStateCoords[8];  

    string strBoard;
     
    Puzzle(const Puzzle &p);    //Constructor
    Puzzle(string const elements, string const goal);
     
    void printBoard();
    
    int h(heuristicFunction hFunction);   
    void updateFCost(); 
    void updateHCost(heuristicFunction hFunction);  
     
    bool goalMatch();
    string toString();
    
    string getString(){
        return strBoard;
    }
    
    // Check if an action can be taken
    bool canMoveLeft();
    bool canMoveRight();
    bool canMoveUp();
    bool canMoveDown();  
    
    // Performs the action and updates the state of the puzzle
    Puzzle * moveUp();
    Puzzle * moveRight();
    Puzzle * moveDown();
    Puzzle * moveLeft();
    
    const string getPath();

    int getPathLength();
    int getFCost();
    int getHCost();
    int getGCost();    
};
