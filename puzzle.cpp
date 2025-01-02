#include "puzzle.h"
#include  <cmath>
#include  <assert.h>

using namespace std;

// Have added. Array of coordinates for the GOAL board. Used for Manhattan.
pair<int, int> Puzzle::goalStateCoords[8];

//////////////////////////////////////////////////////////////
//	
//	Constructor - Creates a copy of a given puzzle
//		
//////////////////////////////////////////////////////////////
Puzzle::Puzzle(const Puzzle &p) : path(p.path){
	
	for(int i=0; i < 3; i++) {
		for(int j=0; j < 3; j++) {	
		    board[i][j] = p.board[i][j];
		    goalBoard[i][j] = p.goalBoard[i][j];
		}
	}	
	
	x0 = p.x0;
	y0 = p.y0;
	pathLength = p.pathLength;
	hCost = p.hCost;
	fCost = p.fCost;	
	strBoard = toString(); 	//uses the board contents to generate the string equivalent	
}

// ---------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////
//	Constructor
//		Generates 2D array versions corresponding to an initial board config and a goal config
//		inputs:  initial state, goal state.
//		Have added to the 'goal board part' to use an array of coordinates for each of the numbers
//
////////////////////////////////////////////////////////////////////////////////////////////////////
Puzzle::Puzzle(string const elements, string const goal){
	
	int n;
	n = 0;

	for(int i=0; i < 3; i++) {
		for(int j=0; j < 3; j++) {	
		    board[i][j] = elements[n] - '0';
		    if(board[i][j] == 0) {
			    x0 = j;
			    y0 = i;
			}
		    n++;
		} 
	}
		
	///////////////////////
	n = 0;
	for(int i=0; i < 3; i++) {
		for(int j=0; j < 3; j++) {	
		    goalBoard[i][j] = goal[n] - '0';

			/* Have added.
			store coords for numbers 1 - 8 in order (not storing 0, so use -1 to 0 index). 
			This will help when doing the Manhattan distance as these coords will never change. 
			Can now easily index into them. 
			inner loop ('j') gives the x axis values, while outer loop ('i') gives the y axis */
			if(goalBoard[i][j] != 0) {
				Puzzle::goalStateCoords[goalBoard[i][j] - 1] = {j, i};
			}
			
		    n++;
		} 
	}	

	///////////////////////	
	path = "";
	pathLength=0;
	hCost = 0;
	fCost = 0;
	strBoard = toString();	
}


// ---------------------------------------------------------------------------------------------------------------

// Calls the 'h' function passing in an Enum. The returned value will be the current heuristic cost
void Puzzle::updateHCost(heuristicFunction hFunction) {
	hCost = h(hFunction);
}

// ---------------------------------------------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////
//		
//	Computes for the f-cost of a search node
//		f = gCost + hCost
//			where:
//				gCost is cost of path so far
//				hCost is heuristic for current state.
//
///////////////////////////////////////////////////////////////////
void Puzzle::updateFCost() {
	fCost = getGCost() + getHCost();
}


// ---------------------------------------------------------------------------------------------------------------

int Puzzle::getFCost() {
	return fCost;
}

int Puzzle::getHCost() {
	return hCost;
}

int Puzzle::getGCost() {
	return pathLength;
}


// ---------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
//		
//	Computes for the heuristic value of a state.
//		The parameter is an enum used to determine what algorithm to
//		run - misplaced or mahattan.
//		Returns the respective 'h cost' value back to the 'h' function
//
////////////////////////////////////////////////////////////////////////
int Puzzle::h(heuristicFunction hFunction) {
	
	int sum=0;
	int h=0;
	int numOfMisplacedTiles=0;
	
	switch(hFunction){
		case misplacedTiles:			      	
			
			// Count the number of tiles in the wrong place - avoiding the '0' tile 
			for(int i = 0; i < 3; i++) {
				for(int j = 0; j < 3; j++) {
					if((board[i][j] != goalBoard[i][j]) && (board[i][j] != 0)) {
						numOfMisplacedTiles++;
					}
				}
			}
			h = numOfMisplacedTiles; 					
			break;
		         
		case manhattanDistance:
			
			// loop through the current state board. 'x' and 'y' are respective axis coords
			for(int y = 0; y < 3; y++) {
				for(int x = 0; x < 3; x++) {

					// store the number that are currently looking at, will be used to index into goalStateCoords array.
					int currNum = board[y][x];

					// for numbers 1 - 8, find the coord values on the goal state board. 'goalStateCoords' is 0 indexed so need -1.
					// goalStateCoords is initialised in the constructor
					if(currNum != 0) {
						int x2 = Puzzle::goalStateCoords[currNum - 1].first;		// retrieves x value of goal state
						int y2 = Puzzle::goalStateCoords[currNum - 1].second;		// retrieves y value of goal state

						// use absoulte values to find current cost, add to 'sum'. The loop values act as the x1 and y1 values.
						sum += abs(x - x2) + abs(y - y2);
					}	
				}
			}

			h = sum; 					
			break;               
	};
	
	return h;
}


// ---------------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////
//
//		converts board state into its string representation
//
///////////////////////////////////////////////////////////////////
string Puzzle::toString(){
  	int n;
  	string stringPath;
  
  	n=0;
  	for(int i=0; i < 3; i++) {
		for(int j=0; j < 3; j++) {			    
		    stringPath.insert(stringPath.end(), board[i][j] + '0');
		    n++;
		} 
  	}
  
	// cout << "toString = " << stringPath << endl;
  	return stringPath;
}


// ---------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////
//		
//		Returns true if the current board config matches the goal state - otherwise false
//		
////////////////////////////////////////////////////////////////////////////////////////
bool Puzzle::goalMatch() {
	
	// compare the curr board state and the goal state, if any difference return false
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			if(board[i][j] != goalBoard[i][j]) {
				return false;
			}
		}
	}

	// if no differences were found will reach this point (ie the current board state matches goal state)
	return true;
}


// ---------------------------------------------------------------------------------------------------------------

const string Puzzle::getPath(){
	return path;
}


// ---------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////
//
//		Checks to see if the BLANK/0 tile could be moved in specific direction
//
////////////////////////////////////////////////////////////////////////////////////
bool Puzzle::canMoveLeft() {
   return (x0 > 0);		// if blank tile has x0 = 0 then is at left edge of grid and cant be moved left
}

bool Puzzle::canMoveRight() {
   return (x0 < 2);		// if blank tile has x0 = 2 then is at right edge of grid and cant be moved right
}

bool Puzzle::canMoveUp() {
   return (y0 > 0);		// if blank tile has y0 = 0 then is at top edge of grid and cant be moved up
}

bool Puzzle::canMoveDown() {
   return (y0 < 2); 	// if blank tile has y0 = 2 then is at bot edge of grid and cant be moved down
}


// ---------------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////
//
//		Creates instance of a Puzzle (including memory allocation)
//		representing the result of the move taken. Also tracks the sequence
//		of moves taken (or partial path). The function returns a Puzzle object
//
/////////////////////////////////////////////////////////////////////////////////////
Puzzle *Puzzle::moveLeft(){
	
	Puzzle *p = new Puzzle(*this);
	
	if(x0 > 0){
		p->board[y0][x0] = p->board[y0][x0-1];
		p->board[y0][x0-1] = 0;
		
		p->x0--;
		
		p->path = path + "L";
		p->pathLength = pathLength + 1;   
	}

	p->strBoard = p->toString();
	return p;	
}


Puzzle *Puzzle::moveRight(){
	
   Puzzle *p = new Puzzle(*this);
	
   	if(x0 < 2){
		p->board[y0][x0] = p->board[y0][x0+1];
		p->board[y0][x0+1] = 0;
		
		p->x0++;
		
		p->path = path + "R";
		p->pathLength = pathLength + 1; 
	}
	
	p->strBoard = p->toString();
	return p;
}


Puzzle *Puzzle::moveUp(){
	
   Puzzle *p = new Puzzle(*this);
	
   	if(y0 > 0){
		p->board[y0][x0] = p->board[y0-1][x0];
		p->board[y0-1][x0] = 0;
		
		p->y0--;
		
		p->path = path + "U";
		p->pathLength = pathLength + 1;  
	}
	
	p->strBoard = p->toString();
	return p;
}


Puzzle *Puzzle::moveDown(){
	
   Puzzle *p = new Puzzle(*this);
	
   	if(y0 < 2){
		p->board[y0][x0] = p->board[y0+1][x0];
		p->board[y0+1][x0] = 0;
		
		p->y0++;
		
		p->path = path + "D";
		p->pathLength = pathLength + 1;  
	}

	p->strBoard = p->toString();	
	return p;
}


// ---------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////
//
//		Prints the content of a puzzle
//
////////////////////////////////////////////////////////////
void Puzzle::printBoard() {
	cout << "board: "<< endl;
	for(int i=0; i < 3; i++) {
		for(int j=0; j < 3; j++) {	
		  cout << endl << "board[" << i << "][" << j << "] = " << board[i][j];
		}
	}
	cout << endl;
}


// ---------------------------------------------------------------------------------------------------------------

int Puzzle::getPathLength(){
	return pathLength;
}
