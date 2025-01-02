#include <queue>                // for 'Q' as a min-heap
#include <vector>               // for 'Q'
#include "algorithm.h"
#include <unordered_set>        // for the expanded list
using namespace std;


// expanded list done using a unordered_set to prevent duplicate states.
unordered_set<string> expandedList;     


///////////////////////////////////////////////////////////////////////////////////////////
//
//  Custom comparators.
//  Used "https://www.geeksforgeeks.org/comparator-in-cpp/" for how to do comparator
//      
//      - Uniform Cost will compare using the g-cost
//      - A* will compare using the f-cost (= gcost + hcost)
//
////////////////////////////////////////////////////////////////////////////////////////////

// Custom comparator for the Uniform Cost 'Q' to maintain a min-heap. Uses gCost
struct gCostComparator {
    bool operator() (Puzzle &p1, Puzzle &p2) const {
        return p1.getGCost() > p2.getGCost();
    }
};

// Custom comparator for the A* 'Q' to maintain a min-heap. uses fCost.
struct fCostComparator {
    bool operator() (Puzzle &p1, Puzzle &p2) const {
        return p1.getFCost() > p2.getFCost();
    }
};



///////////////////////////////////////////////////////////////////////////////////////////
//
//  Functions 
//
///////////////////////////////////////////////////////////////////////////////////////////

// Function to update the max length of 'Q'. Take in current Q length, and reference to maxQLength if need to update it
void findMaxQLength(int qLength, int &maxQLength) {
    if(qLength > maxQLength) {
        maxQLength = qLength;
    }
}



// Uniform Cost function to check 'Q' and 'expanded list' before adding an updated state. Uses a reference to puzzle object
void updateUniformCostQ(Puzzle &updatedState, vector<Puzzle> &Q, int &numDeletions, int &numReexpand) {
    
    // Check if this new state is in the expanded list (step 6)
    if(expandedList.find(updatedState.getString()) == expandedList.end()) {
            
        /* Check if the new state is already in 'Q' (step 7)
        Have adapted syntax from:   https://stackoverflow.com/questions/42933943/how-to-use-lambda-for-stdfind-if
        'find_if()' returns an iterator to the first element within 'Q' based on a given condition, or if not found returns 'end' iterator
        [&updatedState] allows the lambda function to use this variable by reference.
        the 'iterator' variable will be a pointer to the returned iterator - either Q.end() or a matching state   */
        auto iterator = find_if(Q.begin(), Q.end(), [&updatedState](Puzzle& p) {
            return p.getString() == updatedState.getString();
        });
        

        // if 'iterator' points to Q.end() then this state is NOT already in 'Q' so can add it
        if (iterator == Q.end()) { 
            Q.push_back(updatedState);
            push_heap(Q.begin(), Q.end(), gCostComparator());
        } 

        // Otherwise if the state is already in 'Q' then need to check the lowest cost version.
        else {            
            // check the gCost to see which version of the state should be in 'Q'
            if (iterator->getGCost() > updatedState.getGCost()) {
                
                // Dereference 'iterator' to replace with the lower-cost state
                *iterator = updatedState;
                
                // Reorganise the heap after the update to rebalance the heap
                make_heap(Q.begin(), Q.end(), gCostComparator());
                
                // haven't delted from heap to avoid extra cost, have instead replaced directly
                numDeletions++;
            }
        }
    }
    
    else {
        // if IS in the expanded list then have tried to re-exapnd
        numReexpand++;
    }
}



// A* Cost function to check 'Q' and 'expanded list' before adding an updated state. Uses a reference to puzzle object 
void updateAStarQ(Puzzle &updatedState, vector<Puzzle> &Q, int &numDeletions, int &numReexpand) {
    
    // Check if this new state is in the expanded list (step 6)
    if(expandedList.find(updatedState.getString()) == expandedList.end()) {
            
        
        /* Check if the new state is already in 'Q' (step 7)
        Have adapted syntax from:   https://stackoverflow.com/questions/42933943/how-to-use-lambda-for-stdfind-if
        'find_if()' returns an iterator to the first element within 'Q' based on a given condition, or if not found returns 'end' iterator
        [&updatedState] allows the lambda function to use this variable by reference.
        the 'iterator' variable will be a pointer to the returned iterator - either Q.end() or a matching state   */
        auto iterator = find_if(Q.begin(), Q.end(), [&updatedState](Puzzle& p) {
            return p.getString() == updatedState.getString();
        });
        

        // if 'iterator' points to Q.end() then this state is NOT already in 'Q' so can add it
        if (iterator == Q.end()) { 
            Q.push_back(updatedState);
            push_heap(Q.begin(), Q.end(), fCostComparator()); // Add to heap
        } 

        // Otherwise if the state is already in 'Q' then need to check the lowest cost version.
        else {            
            // check the fCost to see which version of the state should be in 'Q'
            if (iterator->getFCost() > updatedState.getFCost()) {
                
                // Dereference 'iterator' to replace with the lower-cost state
                *iterator = updatedState;
                
                // Reorganise the heap after the update
                make_heap(Q.begin(), Q.end(), fCostComparator());
                
                // haven't delted from heap to avoid extra cost, have instead replaced directly
                numDeletions++;
            }
        }
    }
    
    else {
        // if IS in the expanded list then have tried to re-exapnd
        numReexpand++;
    }
}



///////////////////////////////////////////////////////////////////////////////////////////
//
//  Search Algorithm:  UC with Strict Expanded List
//
////////////////////////////////////////////////////////////////////////////////////////////
string uc_explist(string const initialState, string const goalState, int& pathLength, int &numOfStateExpansions, int& maxQLength,
                    float &actualRunningTime, int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, 
                    int &numOfAttemptedNodeReExpansions) {
											 
   
    //cout << "------------------------------" << endl;
    //cout << "<<uc_explist>>" << endl;
    //cout << "------------------------------" << endl;
   
    // initialising values
    numOfDeletionsFromMiddleOfHeap = 0;          
    numOfLocalLoopsAvoided = 0;                  
    numOfAttemptedNodeReExpansions = 0;          
    actualRunningTime = 0.0;
    clock_t startTime;
    string path = "";

    // Clear the global expanded list variable
    expandedList.clear();
    
    // Create 'Q' as a vector, then convert to a min-heap using custom comparator of 'g-cost' value
    vector<Puzzle> Q; 
    make_heap(Q.begin(), Q.end(), gCostComparator());

    // Create new Puzzle object with initial and goal states, add it to 'Q' vector, then update the heap
    Puzzle puzzle(initialState, goalState);
    Q.push_back(puzzle);         
    push_heap(Q.begin(), Q.end(), gCostComparator());   
 	
	
    startTime = clock();
    
    // Start of the Uniform Cost algorithm loop
    while(true) {
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 
        //  STEP 1:  Check if 'Q' is empty. If not, then continue algorithm, else exit loop/algorithm
        //
        //////////////////////////////////////////////////////////////////////////////////////////////
        if(!Q.empty()) {
            

            /////////////////////////////////////////////////////////////////////////
            // 
            //  STEP 2:  Extract the lowest path length from 'Q'
            //           pop_heap() swaps lowest and highest cost objects in heap
            //
            ////////////////////////////////////////////////////////////////////////
            pop_heap(Q.begin(), Q.end(), gCostComparator());
            Puzzle currPuzzle = Q.back();                       
            Q.pop_back();                                       
            

            //////////////////////////////////////////////////////////////////////////////////
            // 
            //  STEP 3:  If the current state matches the goal state then exit loop/algorithm
            //
            //////////////////////////////////////////////////////////////////////////////////
            if(currPuzzle.goalMatch()) {
                path = currPuzzle.getPath();
                break; 
            } 
        

            /////////////////////////////////////////////////////////////////////////////
            // 
            //  STEP 4:  If the current state is NOT the goal state, then proceed
            //
            //////////////////////////////////////////////////////////////////////////////
            else {
                
                 numOfStateExpansions++;        // put after checking the goal state, as was getting 1 more expansion than needed    !!!!!!!!!!!!!!!!!!!!!

                /////////////////////////////////////////////////////////////////////////////
                // 
                //  STEP 5:  Check if the current state is in the expanded list, if it IS
                //           then go back to start of loop, else add the current state to 
                //           the expanded list and continue.
                //
                //////////////////////////////////////////////////////////////////////////////
                if(expandedList.find(currPuzzle.getString()) == expandedList.end()) {
                   
                    expandedList.insert(currPuzzle.getString());     // add state to the expanded list


                    ///////////////////////////////////////////////////////////////////////////////
                    // 
                    //  STEP 6:  From this state, find all valid children states. Valid means
                    //              1) a move can be made (performed in order U, R, D, L)
                    //              2) the child is NOT already in the expanded list
                    //
                    //
                    //  STEP 7:  Add all valid children to front of queue in order U, R, D, L
                    //           If child is already in 'Q' only keep the shortest path
                    //
                    ////////////////////////////////////////////////////////////////////////////////
                    
                    // Move blank tile up
                    if(currPuzzle.canMoveUp()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveUp();

                        // Dereference the puzzle object and pass to function to check if can be added to 'Q'
                        updateUniformCostQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }


                    // Move blank tile right
                    if(currPuzzle.canMoveRight()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveRight();
                        
                        // Dereference the puzzle object, and pass to function to check if can be added to 'Q'
                        updateUniformCostQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }


                    // Move blank tile down
                    if(currPuzzle.canMoveDown()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveDown();

                        // Dereference the puzzle object, and pass to function to check if can be added to 'Q'
                        updateUniformCostQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }
                    

                    // Move blank tile left
                    if(currPuzzle.canMoveLeft()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveLeft();

                        // Dereference the puzzle object, and pass to function to check if can be added to 'Q'
                        updateUniformCostQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }


                    // After any new nodes added to the 'Q', check if the maxsize has changed.
                    findMaxQLength(Q.size(), maxQLength);
                
                } // end of currPuzzle NOT being in expanded list  

            }  // end of NOT goal state

        } // end of "if(!Q.empty())"
        
        // If the 'Q' is empty then exit loop to update time
        else {
            break;    
        }

    } // end of loop/algorithm
    
    
    // Final values to calculate.
	actualRunningTime = ((float)(clock() - startTime)/CLOCKS_PER_SEC);
    pathLength = path.length();
	return path;	
}




///////////////////////////////////////////////////////////////////////////////////////////
//
//  Search Algorithm:  A* with the Strict Expanded List
//
////////////////////////////////////////////////////////////////////////////////////////////
string aStar_ExpandedList(string const initialState, string const goalState, int& pathLength, int &numOfStateExpansions, int& maxQLength,
                            float &actualRunningTime, int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, 
                            int &numOfAttemptedNodeReExpansions, heuristicFunction heuristic){
											 
    // initialise some values
    numOfDeletionsFromMiddleOfHeap=0;
    numOfLocalLoopsAvoided=0;
    numOfAttemptedNodeReExpansions=0;
    actualRunningTime=0.0;	
    string path = "";
    clock_t startTime;

	
    // Clear the global expanded list variable
    expandedList.clear();

    // Create 'Q' as a vector, then convert to a min-heap using custom comparator of 'g-cost' value
    vector<Puzzle> Q; 
    make_heap(Q.begin(), Q.end(), fCostComparator());

    // Create new Puzzle object with initial and goal states, calculate the heuristic value, then calculate f cost
    Puzzle puzzle(initialState, goalState);
    puzzle.updateHCost(heuristic);
    puzzle.updateFCost();
  
    // Add to 'Q', then update the heap
    Q.push_back(puzzle);         
    push_heap(Q.begin(), Q.end(), fCostComparator());   
	startTime = clock();

    // start the A* Seach algorithm
	while(true) {

        ////////////////////////////////////////////////////////////////////////////////////////
        // 
        //  STEP 1:  Check if 'Q' is empty. If not, continue algorithm, else exit loop
        //
        /////////////////////////////////////////////////////////////////////////////////////////
        if(!Q.empty()) {

            ///////////////////////////////////////////////////////////
            // 
            //  STEP 2:  Extract the lowest f-cost from 'Q'
            //           pop_heap() swaps lowest and highest cost objects in heap
            //
            ////////////////////////////////////////////////////////////
            pop_heap(Q.begin(), Q.end(), fCostComparator());     
            Puzzle currPuzzle = Q.back();                       
            Q.pop_back();                                       


            /////////////////////////////////////////////////////////////////////////////
            // 
            //  STEP 3:  If the current state matches the goal state then exit loop
            //
            //////////////////////////////////////////////////////////////////////////////
            if(currPuzzle.goalMatch()) {
                path = currPuzzle.getPath();
                break;                      
            } 


            /////////////////////////////////////////////////////////////////////////////
            // 
            //  STEP 4:  If the current state is NOT the goal state, then proceed
            //
            //////////////////////////////////////////////////////////////////////////////
            else {
                
                numOfStateExpansions++;        // put after checking the goal state, as was getting 1 more expansion than needed    !!!!??????

                /////////////////////////////////////////////////////////////////////////////
                // 
                //  STEP 5:  Check if the current state is in the expanded list, if it IS
                //           then go back to start of loop, else add the current state to 
                //           the expanded list.
                //
                //////////////////////////////////////////////////////////////////////////////

                if(expandedList.find(currPuzzle.getString()) == expandedList.end()) {

                    expandedList.insert(currPuzzle.getString());     // add state to the expanded list

                    ///////////////////////////////////////////////////////////////////////////////
                    // 
                    //  STEP 6:  From this state, find all valid children states. Valid means
                    //              1) the move can be made (performed in order U, R, D, L)
                    //              2) the child is NOT already in the expanded list
                    //
                    //
                    //  STEP 7:  Add all valid children to front of queue in order U, R, D, L
                    //           If child is already in 'Q' only keep the shortest path
                    //
                    ////////////////////////////////////////////////////////////////////////////////

                    // Move blank tile up
                    if(currPuzzle.canMoveUp()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveUp();
                        tempPuzzle->updateHCost(heuristic);
                        tempPuzzle->updateFCost();

                        // Dereference the puzzle object, and pass to function to check if can be added to 'Q'
                        updateAStarQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }


                    // Move blank tile right
                    if(currPuzzle.canMoveRight()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveRight();
                        tempPuzzle->updateHCost(heuristic);
                        tempPuzzle->updateFCost();
                        
                        // Dereference the puzzle object, and pass to function to check if can be added to 'Q'
                        updateAStarQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }


                    // Move blank tile down
                    if(currPuzzle.canMoveDown()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveDown();
                        tempPuzzle->updateHCost(heuristic);
                        tempPuzzle->updateFCost();

                        // Dereference the puzzle object, and pass to function to check if can be added to 'Q'
                        updateAStarQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }
                    

                    // Move blank tile left
                    if(currPuzzle.canMoveLeft()) {
                        
                        // Get the pointer to the updated Puzzle object 
                        Puzzle *tempPuzzle = currPuzzle.moveLeft();
                        tempPuzzle->updateHCost(heuristic);
                        tempPuzzle->updateFCost();

                        // Dereference the puzzle object, and pass to function to check if can be added to 'Q'
                        updateAStarQ(*tempPuzzle, Q, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
                    }

                    // After any new nodes added to the 'Q', check if the maxsize has changed.
                    findMaxQLength(Q.size(), maxQLength);
                
                }   

            }

        } // end of "if(!Q.empty())"

        // If the 'Q' is empty then exit loop to update time
        else {
            break;    
        }
    }


    // final values to calculate
	actualRunningTime = ((float)(clock() - startTime)/CLOCKS_PER_SEC);
	pathLength = path.length();
	return path;		
}

