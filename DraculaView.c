////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE

struct draculaView {
	// TODO: ADD FIELDS HERE
	GameView gv;

};

// helper function
int isCotainDBMove(PlaceId *moves, int numMoves);
int isCotainHIMove(PlaceId *moves, int numMoves);
void array_Delete(PlaceId *array, int size, int id);
int array_Find(PlaceId *array, int size, PlaceId city);


#define MAXIMUM_CITY 128	


////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	DraculaView dv = malloc(sizeof(*dv));
	if (dv == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}

	dv->gv = GvNew(pastPlays, messages);

	return dv;
}

void DvFree(DraculaView dv)
{
	GvFree(dv->gv);

	free(dv);
}
////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	return GvGetTrapLocations(dv->gv, numTraps);
}


////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// get current round
	Round cur_round = DvGetRound(dv);
	// an array to store all vail moves
	PlaceId vaildMoves[NUM_REAL_PLACES];
	int count = 0;
	// get current palce
	PlaceId from = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// using GvGetReachable fun to get the adjacent palces with 'from'
	int numLocs = -1;
	PlaceId *locs = GvGetReachable(dv->gv, PLAYER_DRACULA, cur_round, from, &numLocs);
	// an array to store adjacent locations
	PlaceId adjacentLocs[NUM_REAL_PLACES];
	int num_adjacent = 0;
	// store the locs places in vaidMoves
	for (count = 0; count < numLocs; count++) {
		vaildMoves[count] = locs[count];
		adjacentLocs[num_adjacent] = locs[count];
		num_adjacent++;
	}
	// using GvGetLastLocation to get the drac location history within last 5 rounds
	int num_LastLocs = -1; 
	bool canFree = false;
    PlaceId *lastLocs = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 5, &num_LastLocs, &canFree);
	// remove the intersecting places between vaildMoves and moves within last 5 rounds
	int in_ckeck = 0;
	while (in_ckeck < num_LastLocs) {
		int in_find = array_Find(vaildMoves, count, lastLocs[in_ckeck]);
		if (in_find != -1) {
			array_Delete(vaildMoves, count, in_find);
			count--;			
		}
		in_ckeck++;
	}
	// using GvGetMoveHistory to get the drac move history
	int numMoves = -1; 
	bool canFree_2 = false;
    PlaceId *moves = GvGetMoveHistory(dv->gv, PLAYER_DRACULA, &numMoves, &canFree_2);
	// check whether drac have special moves before 5 rounds
	int contain_DB = isCotainDBMove(moves, numMoves);
	int contain_HI = isCotainHIMove(moves, numMoves);
	// if drac did not have double back move
	// add the suited double back moves in vail moves
	if (!contain_DB) {
		for (int check_contain = 0; check_contain < num_adjacent; check_contain++) {
			int find = array_Find(lastLocs, num_LastLocs, adjacentLocs[check_contain]);
			if (find != -1) {
				int shift = num_LastLocs - find;
				vaildMoves[count] = DOUBLE_BACK_1 + shift - 1;
				count++;					
			}
		}

	}
	// if drac did not have hide move
	// current palce is not SEA
	// add hide move to vail moves	
	if (!contain_HI && !placeIsSea(from)) {
		vaildMoves[count] = HIDE;
		count++;
	} 
	// assign the numReturnedLocs value
	*numReturnedMoves = count;
	// If  Dracula  has  no valid moves	
	if (count == 0) {
		return NULL;
	} else { 
		// make all target moves in a dynamically allocated array
		PlaceId *ReturnedVailMoves = malloc(sizeof(PlaceId) * count);
		for (int j = 0; j < count; j++) {
			ReturnedVailMoves[j] = vaildMoves[j];
		}
		// return this dynamically allocated array
		return ReturnedVailMoves;
	}
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// get current round
	Round cur_round = DvGetRound(dv);
	// an array to store all can reachable locations
	PlaceId canGoLocs[NUM_REAL_PLACES];
	int count = 0;
	// get current palce
	PlaceId from = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// using GvGetReachable fun to get the adjacent palces with 'from'
	int numLocs = -1;
	PlaceId *locs = GvGetReachable(dv->gv, PLAYER_DRACULA, cur_round, from, &numLocs);
	// an array to store adjacent locations
	PlaceId adjacentLocs[NUM_REAL_PLACES];
	int num_adjacent = 0;
	// store the locs places in canGoLocs
	for (count = 0; count < numLocs; count++) {
		canGoLocs[count] = locs[count];
		adjacentLocs[num_adjacent] = locs[count];
		num_adjacent++;
	}
	// using GvGetLastLocation to get the drac location history within last 5 rounds
	int num_LastLocs = -1; 
	bool canFree = false;
    PlaceId *lastLocs = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 5, &num_LastLocs, &canFree);
	// remove the intersecting places between canGoLocs and moves within last 5 rounds
	int in_ckeck = 0;
	while (in_ckeck < num_LastLocs) {
		int in_find = array_Find(canGoLocs, count, lastLocs[in_ckeck]);
		if (in_find != -1) {
			array_Delete(canGoLocs, count, in_find);
			count--;			
		}
		in_ckeck++;
	}
	// using GvGetMoveHistory to get the drac move history
	int numMoves = -1; 
	bool canFree_2 = false;
    PlaceId *moves = GvGetMoveHistory(dv->gv, PLAYER_DRACULA, &numMoves, &canFree_2);
	// check whether drac have special moves before 5 rounds
	int contain_DB = isCotainDBMove(moves, numMoves);
	int contain_HI = isCotainHIMove(moves, numMoves);
	// if drac did not have double back move
	// add the suited places in canGoLocs
	if (!contain_DB) {
		for (int check_contain = 0; check_contain < num_adjacent; check_contain++) {
			int find = array_Find(lastLocs, num_LastLocs, adjacentLocs[check_contain]);
			if (find != -1) {
				canGoLocs[count] = lastLocs[find];
				count++;					
			}
		}

	}
	// if drac did not have hide move
	// current palce is not SEA
	// add current palce to canGoLocs	
	if (!contain_HI && !placeIsSea(from)) {
		int hi_find = array_Find(canGoLocs, count, from);
		if (hi_find == -1) {
			canGoLocs[count] = from;
			count++;
		}
	} 
	// assign the numReturnedLocs value
	*numReturnedLocs = count;
	// If  Dracula  has no reachable locations	
	if (count == 0) {
		return NULL;
	} else { 
		// make all target locs in a dynamically allocated array
		PlaceId *ReturnedLocs = malloc(sizeof(PlaceId) * count);
		for (int j = 0; j < count; j++) {
			ReturnedLocs[j] = canGoLocs[j];
		}
		// return this dynamically allocated array
		return ReturnedLocs;
	}
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// get current round
	Round cur_round = DvGetRound(dv);
	// an array to store all can reachable locations
	PlaceId canGoLocs[NUM_REAL_PLACES];
	int count = 0;
	// get current palce
	PlaceId from = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// using GvGetReachableByType fun to get the adjacent palces with 'from'
	int numLocs = -1;
	PlaceId *locs = GvGetReachableByType(dv->gv, PLAYER_DRACULA, cur_round, from, road, false, boat, &numLocs);
	// store the locs places in canGoLocs
	for (count = 0; count < numLocs; count++) {
		canGoLocs[count] = locs[count];
	}
	//
	numLocs = -1;
	PlaceId *locs_alltype = GvGetReachable(dv->gv, PLAYER_DRACULA, cur_round, from, &numLocs);
	// an array to store adjacent locations
	PlaceId adjacentLocs[NUM_REAL_PLACES];
	int num_adjacent = 0;
	// store the locs places in adjacentLocs
	for (num_adjacent = 0; num_adjacent < numLocs; num_adjacent++) {
		adjacentLocs[num_adjacent] = locs_alltype[num_adjacent];
	}
	// using GvGetLastLocation to get the drac location history within last 5 rounds
	int num_LastLocs = -1; 
	bool canFree = false;
    PlaceId *lastLocs = GvGetLastLocations(dv->gv, PLAYER_DRACULA, 5, &num_LastLocs, &canFree);
	// remove the intersecting places between canGoLocs and moves within last 5 rounds
	int in_ckeck = 0;
	while (in_ckeck < num_LastLocs) {
		int in_find = array_Find(canGoLocs, count, lastLocs[in_ckeck]);
		if (in_find != -1) {
			array_Delete(canGoLocs, count, in_find);
			count--;			
		}
		in_ckeck++;
	}
	// using GvGetMoveHistory to get the drac move history
	int numMoves = -1; 
	bool canFree_2 = false;
    PlaceId *moves = GvGetMoveHistory(dv->gv, PLAYER_DRACULA, &numMoves, &canFree_2);
	// check whether drac have special moves before 5 rounds
	int contain_DB = isCotainDBMove(moves, numMoves);
	int contain_HI = isCotainHIMove(moves, numMoves);
	// if drac did not have double back move
	// add the suited places in canGoLocs
	if (!contain_DB) {
		for (int check_contain = 0; check_contain < num_adjacent; check_contain++) {
			int find = array_Find(lastLocs, num_LastLocs, adjacentLocs[check_contain]);
			if (find != -1) {
				canGoLocs[count] = lastLocs[find];
				count++;					
			}
		}
	}
	// if drac did not have hide move
	// current palce is not SEA
	// add current palce to canGoMove	
	if (!contain_HI && !placeIsSea(from)) {
		int hi_find = array_Find(canGoLocs, count, from);
		if (hi_find == -1) {
			canGoLocs[count] = from;
			count++;
		}
	} 
	// assign the numReturnedLocs value
	*numReturnedLocs = count;
	// If  Dracula  has no reachable locations	
	if (count == 0) {
		return NULL;
	} else { 
		// make all target locs in a dynamically allocated array
		PlaceId *ReturnedLocs = malloc(sizeof(PlaceId) * count);
		for (int j = 0; j < count; j++) {
			ReturnedLocs[j] = canGoLocs[j];
		}
		// return this dynamically allocated array
		return ReturnedLocs;
	}
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	// get current round
	Round cur_round = DvGetRound(dv);
	// get current palce
	PlaceId from = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// check current player
	if (GvGetPlayer(dv->gv) != PLAYER_LORD_GODALMING) {
		cur_round++;	
	}
	int numLocs = -1;
	PlaceId *locs = GvGetReachable(dv->gv, player, cur_round, from, &numLocs);
	*numReturnedLocs = numLocs;	
	return locs;
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	// get current round
	Round cur_round = DvGetRound(dv);
	// get current palce
	PlaceId from = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// check current player
	if (GvGetPlayer(dv->gv) != PLAYER_LORD_GODALMING) {
		cur_round++;	
	}
	int numLocs = -1;
	PlaceId *locs = GvGetReachableByType(dv->gv, player, cur_round, from, road, rail, boat, &numLocs);
	
	*numReturnedLocs = numLocs;	
	return locs;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// A function to check whether a given array contains Double Back Moves
// if true retrn 1, else return 0
int isCotainDBMove(PlaceId *moves, int numMoves) {
	int check = numMoves - 1;
    while (check >= 0) {
        if (moves[check] == DOUBLE_BACK_1 || moves[check] == DOUBLE_BACK_2 
			|| moves[check] == DOUBLE_BACK_3 || moves[check] == DOUBLE_BACK_4
			|| moves[check] == DOUBLE_BACK_5) {
            return 1;
        } 
        check--;
    }
	return 0;

}

// A function to check whether a given array contains HIDE Moves
// if true retrn 1, else return 0
int isCotainHIMove(PlaceId *moves, int numMoves) {
	int check = 0;
    while (check < numMoves) {
		if (moves[check] == HIDE) {
            return 1;			
		} 
		check++;
    }
	return 0;
}

// A function to delete id index place in an array
void array_Delete(PlaceId *array, int size, int id) {
	if (size <= 0 || id < 0 || id >= size) {
		return;
	}
	array[id] = array[size - 1];
	array[size - 1] = NOWHERE;
}

// A function to check whether a given array contain the city
// if ture return the index in array, else return -1
int array_Find(PlaceId *array, int size, PlaceId city) {
	for (int i = 0; i < size; i++) {
		if (array[i] == city) {
			return i;
		}
	}
	return -1;
}
