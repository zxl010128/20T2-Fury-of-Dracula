////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here
// add your own #defines here
#define MAXIMUM_TRAP 3
#define MAXIMUM_CITY 128

int addRailPlaces(int start, int count_add, int *rail_places, GameView gv);

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct amateurVamp {
	int born;						
	PlaceId bornCity;					//Initialize the born city by no vamp
}*AmateurVamp;

//Information of the players
typedef struct playerInfo {
	Player ID;
	int blood;
	PlaceId current;					//current city
	PlaceId move;						//The last move

	PlaceId locHistory[TRAIL_SIZE];				//Initialize the location history with NOWHERE
								//Must be a city
	
	PlaceId history[TRAIL_SIZE];				//Initialize the move history with NOWHERE 
								//Can  be a city or other places
}*PlayerInfo;

//Type of the trap
typedef enum trapType
{
	TRAP_NULL,						//0 forno trap
	TRAP_SET,						//1 for has trap
}*TrapType;

typedef struct gameView {
	int score;
	enum trapType traps[MAXIMUM_CITY][MAXIMUM_TRAP];	//At most 3 trap each city
	struct amateurVamp vamp;				//Ther will be only one amateur vamp

	Round round;						//round number of the game
	Map m;
	struct playerInfo players[NUM_PLAYERS];			// Player list
	Player currentPlayer;					// The current player
}*GameView;

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return PLAYER_LORD_GODALMING;
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{	
	// get a list which contains the adjacent places of 'from' 
	ConnList adjacent = MapGetConnections(gv -> gameMap, from);
	// count is the sum number of reachable places by any transport type
	int count = 0;
	// curr_one: using to loop count number
	// curr_two: using to loop logging data
	ConnList cur_one = adjacent;
	ConnList cur_two = adjacent;
	// road and boat part
	int road_boat_num = 0;
	bool isContainHospital = false;
	// count the number of places reachable by road  and boat that drac and hunt can get
	while (cur_one != NULL && cur_one -> type != RAIL) {
		if (cur_one -> p == ST_JOSEPH_AND_ST_MARY) {
			isContainHospital = true;
		}
		road_boat_num++;
		cur_one = cur_one -> next;	
	}
	// logging the drac's reachable places
	if (player == PLAYER_DRACULA) {
		if (isContainHospital) {
			numReturnedLocs = malloc((road_boat_num - 1) * sizeof(int));
		} else {
			numReturnedLocs = malloc(road_boat_num * sizeof(int));
		}
		while (cur_two != NULL && cur_two -> type != RAIL) {
			// expect the two hospitals
			if (cur_two -> p == ST_JOSEPH_AND_ST_MARY) {
				break;
			}
			numReturnedLocs[count] = cur_two -> p;
			count++;
			cur_two = cur_two -> next;
		}
	} 
	// logging the hunt's reachable places 
	else {
		numReturnedLocs = malloc(road_boat_num * sizeof(int));
		while (cur_two != NULL && cur_two -> type != RAIL) {
			numReturnedLocs[count] = cur_two -> p;
			count++;
			cur_two = cur_two -> next;
		}
		// rail part (hunters only)
		// using current round to calculate how far the player can move by rail
		int rail_times = (round + player) % 4;
		// an array to collect the palces which can move by rail
		int rail_places[NUM_REAL_PLACES];
		// curr: using to loop count number
		ConnList rail_cur = adjacent;
		int rail_num = 0;
		// collect the from's adjacent places(can move by rail) to rail_places array
		while (rail_cur != NULL && rail_cur -> type == RAIL) {
			rail_places[rail_num] = rail_cur -> p;
			rail_num++;		
			rail_cur = rail_cur -> next;	
		}
		// the hunter may not move by rail this turn
		if (rail_times == 0) {
			return numReturnedLocs;
		}
		// the maximum allowed distance via rail is 1
		if (rail_times == 1) {
			// add the from's adjacent places(can move by rail)
			numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
			for (int i = 0; i <= rail_num; i++) {
				numReturnedLocs[count] = rail_places[i];
				count++;					
			}
		} 
		// the maximum allowed distance via rail is 2
		if (rail_times == 2) {
			// using fun to get new number of palces which distance via rail is 2
			// fun will modify the rail_places array
			rail_num = addRailPlaces(0, rail_num, rail_places, gv);		
			// add the places(can move by rail) which distance via rail is 2
			numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
			for (int i = 0; i <= rail_num; i++) {
				numReturnedLocs[count] = rail_places[i];
				count++;					
			}
		}
		// the maximum allowed distance via rail is 3
		if (rail_times == 3) {
			// using fun to get new number of palces which distance via rail is 2
			// fun will modify the rail_places array		 	
			int result = addRailPlaces(0, rail_num, rail_places, gv);
			int orig = rail_num;
			rail_num = result;
			// using fun to get new number of palces which distance via rail is 3	
			// fun will modify the rail_places array			
			rail_num = addRailPlaces(orig, rail_num, rail_places, gv);
			// add the places(can move by rail) which distance via rail is 3
			numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
			for (int i = 0; i <= rail_num; i++) {
				numReturnedLocs[count] = rail_places[i];
				count++;					
			}	
		}	
	}
	return numReturnedLocs;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// get a list which contains the adjacent places of 'from' 
	ConnList adjacent = MapGetConnections(gv -> gameMap, from);
	// count is the sum number of reachable places by any transport type
	int count = 0;
	// transportType is road
	if (road) {
		// curr_one: using to loop count number
		// curr_two: using to loop logging data
		ConnList road_cur_one = adjacent;
		ConnList road_cur_two = adjacent;
		bool isContainHospital = false;
		int road_num = 0;
		// count the number of places reachable by road that drac and hunt can get
		while (road_cur_one != NULL && road_cur_one -> type == ROAD) {
			if (road_cur_one -> p == ST_JOSEPH_AND_ST_MARY) {
				isContainHospital = true;
			} 
			road_num++;	
		 	road_cur_one = road_cur_one -> next;	
		}
		// logging the drac's reachable places
		if (player == PLAYER_DRACULA) {
			if (isContainHospital) {
				numReturnedLocs = malloc((road_num - 1) * sizeof(int));
			} else {
				numReturnedLocs = malloc(road_num * sizeof(int));
			}
			while (road_cur_two != NULL && road_cur_two -> type == ROAD) {
				// expect the two hospitals
				if (road_cur_two -> p == ST_JOSEPH_AND_ST_MARY) {
					break;
				}
				numReturnedLocs[count] = road_cur_two -> p;
				count++;
				road_cur_two = road_cur_two -> next;
			}
		}
		// logging the hunt's reachable places 
		else {
			numReturnedLocs = malloc(road_num * sizeof(int));
			while (road_cur_two != NULL && road_cur_two -> type == ROAD) {
				numReturnedLocs[count] = road_cur_two -> p;
				count++;
				road_cur_two = road_cur_two -> next;
			}
		}
	}
	// transportType is boat
	if (boat) {
		// curr_one: using to loop count number
		// curr_two: using to loop logging data
		ConnList boat_cur_one = adjacent;
		ConnList boat_cur_two = adjacent;
		int boat_num = 0;
		// count the number of places reachable by road that drac and hunt can get
		while (boat_cur_one != NULL && boat_cur_one -> type == BOAT) {
			boat_num++;		
		 	boat_cur_one = boat_cur_one -> next;	
		}
		// logging the drac's reachable places
		if (player == PLAYER_DRACULA) {
			numReturnedLocs = realloc(numReturnedLocs, (count + boat_num) * sizeof(int));
			while (boat_cur_two != NULL && boat_cur_two -> type == BOAT) {
				numReturnedLocs[count] = boat_cur_two -> p;
				count++;
				boat_cur_two = boat_cur_two -> next;
			}
		}
		// logging the hunt's reachable places 
		else {
			numReturnedLocs = realloc(numReturnedLocs, (count + boat_num) * sizeof(int));
			while (boat_cur_two != NULL && boat_cur_two -> type == BOAT) {
				numReturnedLocs[count] = boat_cur_two -> p;
				count++;
				boat_cur_two = boat_cur_two -> next;
			}
		}
	}
	// transportType is rail
	if (rail) {
		// only consider the player is hunters
		// drac can not have rail move
		if (player != PLAYER_DRACULA) {
			// using current round to calculate how far the player can move by rail
			int rail_times = (round + player) % 4;
			// an array to collect the palces which can move by rail
			int rail_places[NUM_REAL_PLACES];
			// curr: using to loop count number
			ConnList rail_cur = adjacent;
			int rail_num = 0;
			// collect the from's adjacent places(can move by rail) to rail_places array
			while (rail_cur != NULL && rail_cur -> type == RAIL) {
				rail_places[rail_num] = rail_cur -> p;
				rail_num++;		
				rail_cur = rail_cur -> next;	
			}
			// the hunter may not move by rail this turn
			if (rail_times == 0) {
				return numReturnedLocs;
			}
			// the maximum allowed distance via rail is 1
			if (rail_times == 1) {
				// add the from's adjacent places(can move by rail)
				numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
				for (int i = 0; i <= rail_num; i++) {
					numReturnedLocs[count] = rail_places[i];
					count++;					
				}
			} 
			// the maximum allowed distance via rail is 2
			if (rail_times == 2) {
				// using fun to get new number of palces which distance via rail is 2
				// fun will modify the rail_places array
				rail_num = addRailPlaces(0, rail_num, rail_places, gv);		
				// add the places(can move by rail) which distance via rail is 2
				numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
				for (int i = 0; i <= rail_num; i++) {
					numReturnedLocs[count] = rail_places[i];
					count++;					
				}
			}
			// the maximum allowed distance via rail is 3
			if (rail_times == 3) {
				// using fun to get new number of palces which distance via rail is 2
				// fun will modify the rail_places array		 	
				int result = addRailPlaces(0, rail_num, rail_places, gv);
				int orig = rail_num;
				rail_num = result;
				// using fun to get new number of palces which distance via rail is 3	
				// fun will modify the rail_places array			
				rail_num = addRailPlaces(orig, rail_num, rail_places, gv);
				// add the places(can move by rail) which distance via rail is 3
				numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
				for (int i = 0; i <= rail_num; i++) {
					numReturnedLocs[count] = rail_places[i];
					count++;					
				}	
			}
		} 
		// drac is just return
		else {
			return numReturnedLocs;	
		}
	}
	//	
	return numReturnedLocs;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
// A function to add places(can move by rail) to a given array
// this function will return a modify array and a count
// will loop all list of places can move from the each places in given array
// find the unique places(can move by rail) and add it.
int addRailPlaces(int start, int count_add, int *array, GameView gv) {
	int i = count_add;
	while (i > start) {
		ConnList current = MapGetConnections(gv -> gameMap, array[i]);
		while (current != NULL && current -> type == RAIL) {
			// check contain unique places
			int check = 0;
			bool iscontain = false;
			while (check < count_add) {
				if (current -> p == array[check]) {
					iscontain = true;
				}
				check++;
			}
			// if didn't found, adding it
			if (!iscontain)	{
				count_add++;
				array[count_add] = current -> p;
			}	
			current = current -> next;
		}
		i--;
	}
	return count_add;
} 
