/* 
   Code by: Or Yamin    
   Project: Knight's Tour
   Date: 08/07/24
   Review by: Amit
   Review Date: 08/07/24
   Approved by: Amit
   Approval Date: 08/07/24
*/

#include <stddef.h> /* size_t */
#include <time.h> /* clock CLOCKS_PER_SEC */

#include "knight_tour.h"
#include "bitarray.h"

#define NUM_OF_MOVES 8
#define NIBBLE 4
#define ROW 0
#define COL 1
#define DIRECTIONS 2
#define COL_MASK 0x0F

/* LUT for knight moves */
static const char knight_moves[NUM_OF_MOVES][DIRECTIONS] = 
{
	{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
};

static bitarray_t board = 0;
static clock_t start_time = 0;

static int IsMoveValid(char row, char col);
static knights_tour_status_e BackTrackingSolution(position_t path[PATH_LENGTH], 
				  int path_index, char row, char col, unsigned int time_limit);
static char GetRow(position_t pos);
static char GetCol(position_t pos);
static size_t BuildIndex(position_t position);

static knights_tour_status_e WarnsdorfSolution(position_t path[PATH_LENGTH], 
										int path_index, char row, char col);
static int NextMoveWarnsdorf(char *row, char *col);
static int CountPossibleMoves(char row, char col);


/******************************* API functions *******************************/

knights_tour_status_e RunKnightsTour(position_t path[PATH_LENGTH], 
									 position_t starting_position, 
									 int use_heuristic, unsigned int time_limit)
{
	char start_row = GetRow(starting_position);
	char start_col = GetCol(starting_position);

	/* Initialize the board and starting time */
	board = ResetAll(board);
	start_time = clock();

	/* Start the tour */
	path[0] = starting_position;
	board = SetBitOn(board, BuildIndex(starting_position));
	
	return use_heuristic ? 
		   WarnsdorfSolution(path, 1, start_row, start_col) :
		   BackTrackingSolution(path, 1, start_row, start_col, time_limit);
}

position_t BuildPosition(unsigned char row, unsigned char col)
{
	return (position_t)((row << NIBBLE) | col);
}


/*************************** Warnsdorf functions ******************************/

static knights_tour_status_e WarnsdorfSolution(position_t path[PATH_LENGTH], 
										int path_index, char row, char col)
{
	position_t next_pos = 0;
	
	if (path_index == PATH_LENGTH)
	{
	    return SUCCESS;
	}

	/* Get the next move with the minimum possible moves */
	if(0 != NextMoveWarnsdorf(&row, &col))
	{
		return BAD_PATH;
	}
	
	/* Set the next position and continue */
	next_pos = BuildPosition(row, col);
	path[path_index] = next_pos;
	board = SetBitOn(board, BuildIndex(next_pos));
	
	return WarnsdorfSolution(path, path_index + 1, row, col);
}

static int NextMoveWarnsdorf(char *row, char *col)
{
	int min_moves = NUM_OF_MOVES + 1;
	int min_index = -1;
	size_t i = 0;

	for (; i < NUM_OF_MOVES; ++i)
	{
		char next_row = *row + knight_moves[i][ROW];
		char next_col = *col + knight_moves[i][COL];
		int onward_moves = CountPossibleMoves(next_row, next_col);

		/* Find the move with the minimum moves afterwards */
		if (IsMoveValid(next_row, next_col) && 
		!GetVal(board, BuildIndex(BuildPosition(next_row, next_col))) && 
		onward_moves < min_moves)
		{
			min_moves = onward_moves;
			min_index = i;
		}
	}

	/* Set the next position and continue */
	if (min_index != -1)
	{
		*row += knight_moves[min_index][ROW];
		*col += knight_moves[min_index][COL];
		
		return 0;
	}
	
	return 1;
}

static int CountPossibleMoves(char row, char col)
{
	int count = 0;
	size_t i = 0;
    
	for (i = 0; i < NUM_OF_MOVES; ++i)
	{
		char next_row = row + knight_moves[i][ROW];
		char next_col = col + knight_moves[i][COL];
		
		/* Add 1 for each valid move */
		if (IsMoveValid(next_row, next_col) && !GetVal(board, 
			BuildIndex(BuildPosition(next_row, next_col))))
		{
			++count;
		}
	}
	
	return count;
}
							
							
/****************************** BackTracking Solution *************************/

static knights_tour_status_e BackTrackingSolution(position_t path[PATH_LENGTH], 
								int path_index, char row, char col, 
								unsigned int time_limit)
{
	char next_row = 0;
	char next_col = 0;
	position_t next_pos = 0;
	int i = 0;

	/* If all squares are board */
	if (path_index == PATH_LENGTH)
	{
		return SUCCESS;
	}

	for ( i = 0; i < NUM_OF_MOVES; ++i)
	{
		next_row = row + knight_moves[i][ROW];
		next_col = col + knight_moves[i][COL];
		next_pos = BuildPosition(next_row, next_col);

		if (IsMoveValid(next_row, next_col) && !GetVal(board, 
														BuildIndex(next_pos)))
		{
			path[path_index] = next_pos;
			board = SetBitOn(board, BuildIndex(next_pos));

			if (BackTrackingSolution(path, path_index + 1, next_row, next_col, 
															time_limit) == 0)
			{
				return SUCCESS;
			}

			/*	 Check if the time limit is exceeded */
			if (((clock() - start_time) / CLOCKS_PER_SEC) > time_limit)
			{
				return TIME_EXCEEDED;
			}
			
            /* Backtracking */
			board = SetBitOff(board, BuildIndex(next_pos));
		}
	}

	return BAD_PATH;
}


/****************************** Helpers **************************************/

static int IsMoveValid(char row, char col)
{
	return (row >= 0 && row < NUM_OF_ROWS && col >= 0 && col < NUM_OF_ROWS);
}

static char GetRow(position_t pos)
{
	return (pos >> NIBBLE);
}

static char GetCol(position_t pos)
{
	return (pos & COL_MASK);
}

static size_t BuildIndex(position_t position)
{
	char start_row = GetRow(position);
	char start_col = GetCol(position);
	
	return (start_row * NUM_OF_ROWS + start_col );
}



