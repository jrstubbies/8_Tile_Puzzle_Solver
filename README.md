# 8_Tile_Puzzle_Solver

    A C++ program to solve the 8-tile puzzle with A* (using Misplaced Tiles heuristic, and Sum of Manhattan Distance heuristic)and Uniform Cost algorithms.

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                              NOTE:                                                                   //
    //  Code in main.cpp, graphics.cpp, graphics.h, puzzle.cpp were produced and given by Massey teacher Napoleon Reyes.    //
    //  I have done the code in algorithm.cpp, algorithm.h and added some code in puzzle.cpp, puzzle.h.                     //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    The board has each tile labelled 0 - 9, with the 'blank tile' being the '0'. This makes it easier to keep track of board states based on the location of these numbers.


    Uniform Cost Algorithm:
        This is an optimal, uninformed search algorithm that will find the shortest path to the goal using path length as a measurement. In this program moving the blank tile has a path cost of 1 (e.g. try move the blank tile left > up > right would have a path cost of 3). The algorithm follows:
            1:  Initialise the search Queue (heap) with the start board (state) 
            2:  If Q is empty then failed, else pick a node 'N' with the lowest path length/cost from the Q
            3:  If have found the target state, then end
            4:  If not the target state, then remove from Q
            5:  If this node 'N' is in expanded list go to step 2 (effectively ignore this state as alreday looked at it),    otherwise add it to expanded and continue to 6
            6:  Expand this state to find all children states (do in fixed order Up, Right, Down, Left) that are not in the expanded list
            7.  For each valid children state put them into Q.  If any states ALREADY exist in Q then only keep the one with shortest path
            8.  Repeat from step 2


    A* Algorithm:
        An optimal informed search algorithm that uses the path length and a heuristic to find the shortest path. Follows similar algorithm to Uniform Cost, except now need to add the heuristic value to the total path cost. Two heuristics are used.

        Misplaced Tiles heuristic:
            At each state will need to count the number of tiles that are in the INCORRECT position. This compares the current board state against the target board state. This number is added to the path cost when looking at a states children states.
        

        Sum of Manhattan Distance:
            At each state this heuristic looks at each tile and calculates how far away it is from its target position. This is done using the formula    
                                        dist =  |x1 - x2| and |y1 - y2|
            The sum of these differences is then added to the total path cost for each of the current board's children states.


    Both algorithms use a strict expanded list to ensure that a state is never revisted, this prevents looping and reduces the amount of expansions. The strict expanded list has been implemented using an unordered_set to store unique states, and for quick lookup.
