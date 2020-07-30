////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
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
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

#define ItemCopy(i)     (i)
#define ItemEQ(i1,i2)   ((i1) == (i2))
#define ItemShow(i)     printf("%d",(i))

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	// TODO: ADD FIELDS HERE
    GameView gv;
};

typedef PlaceId Item;

typedef struct QueueNode {
	Item value;
	struct QueueNode *next;
} QueueNode;

typedef struct QueueRep {
	QueueNode *head;  // ptr to first node
	QueueNode *tail;  // ptr to last node
} QueueRep;

typedef struct QueueRep *Queue;

Queue newQueue(); 				// create new empty queue
void dropQueue(Queue); 			// free memory used by queue
void showQueue(Queue); 			// display 
void QueueJoin(Queue,Item); 	// add item on queue
Item QueueLeave(Queue); 		// remove item from queue
int QueueIsEmpty(Queue); 		// check for no items
bool placeRealCheck(PlaceId pid);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}
    
	new->gv = GvNew(pastPlays, messages);
	return new;
}

void HvFree(HunterView hv)
{
    GvFree(hv->gv);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

// Get current round
Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv->gv);
}

// Get current player
Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv->gv);
}

// Get current score
int HvGetScore(HunterView hv)
{
	return GvGetScore(hv->gv);
}

// Get current health
int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv->gv, player);
}

// Get current player location
PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv->gv, player);
}

// Get current vampire location
PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

// Gets  Dracula's  last  known  real  location  as revealed
PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// get current round
	Round current_round = GvGetRound(hv->gv);

	// get All the location History
	int history_count = 0;
	bool canFree = false;
	PlaceId *locations_his = GvGetLocationHistory(hv->gv, PLAYER_DRACULA, &history_count, &canFree);

	bool is_found = false;

	PlaceId lastKnown;

	// loop to find the first known city in the array
	for (int i = 0; i < history_count; i++) {
		if (placeRealCheck(locations_his[history_count - 1 - i]) == false) {
			current_round--;
		} else {
			is_found = true;
			lastKnown = locations_his[history_count - 1 - i];
			current_round--;
			break;
		}
	}

	// all the city unknown
	if (is_found == false) {
		*round = 0;
		return NOWHERE;
	}

	*round = current_round;
	return lastKnown;

}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// get player current place
	PlaceId current = HvGetPlayerLocation(hv, hunter);
	
	// situation when current = dest
	if (current == dest) {
		PlaceId *path = malloc(sizeof(PlaceId));
		path[0] = dest;
		(*pathLength) = 1;
		return path;
	}

	PlaceId temp_city;

	// if visited, it will show 1 of this place ID
	int visited[NUM_REAL_PLACES] = {0};

	// this array will record the connected place ID
	PlaceId last_position[NUM_REAL_PLACES] = {[0 ... (NUM_REAL_PLACES-1)] = -1};

	// Queue created
	Queue queue_location = newQueue();
	QueueJoin(queue_location, current);

	// this array will record the round number of the Place ID
	Round round[NUM_REAL_PLACES];
	
	// Set the src round
	round[current] = HvGetRound(hv);
	
	// check whether the dest is founded
	bool is_Found = false;

	while (!QueueIsEmpty(queue_location) && !is_Found) {
		
		temp_city = QueueLeave(queue_location);

		// if we have visited this city, we need to skip it
		// there should be the situation the place connect more than once
		if (visited[temp_city] == 1) {
			continue;
		}

		// set visited of this city
		visited[temp_city] = 1;

		// get all the reachable locations
		int num_locs = 0;
		PlaceId *locs = GvGetReachable(hv->gv, hunter, round[temp_city], temp_city, &num_locs);

		// join the reachable place to Queue
		for (int i = 0; i < num_locs; i++) {
			
			// the place must be non-visted
			if (!visited[locs[i]] && last_position[locs[i]] == -1) {
				
				last_position[locs[i]] = temp_city;
				// if this location is the dest, break the loop
				if (locs[i] == dest) {
					is_Found = true;
					break;
				}

				// Join this place to the Queue
				QueueJoin(queue_location, locs[i]);

				// set this place round as temp_round + 1
				round[locs[i]] = round[temp_city] + 1;
			}

		}

	}

	dropQueue(queue_location);

	// situation when there is no dest in the map
	if (is_Found == false) {
		return NULL;
	}
	
	// count the length of the path
	int Path_length = 0;
	for (PlaceId position = dest; position != current; position = last_position[position]) Path_length++;
	
	// allocate memory for the return array
	PlaceId *ReturnedLocs = malloc(sizeof(PlaceId) * Path_length);

	// form the path
	int array_index = Path_length - 1;
	ReturnedLocs[array_index] = dest;
	array_index--;

	PlaceId temp = last_position[dest];
	while (array_index >= 0) {
		ReturnedLocs[array_index] = temp;
		temp = last_position[temp];
		array_index--;
	}
	
	(*pathLength) = Path_length;

	return ReturnedLocs;

}

////////////////////////////////////////////////////////////////////////
// Making a Move

// Gets all the locations that the current hunter player can move to this round
PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{

	Player player = HvGetPlayer(hv);
	Round round = HvGetRound(hv);

	// unknown place of the player
	if (round == 0) {
		*numReturnedLocs = 0;
		return NULL;
	}

	// get all the reachableLocations
	PlaceId current = HvGetPlayerLocation(hv, player);

	*numReturnedLocs = 0;
	PlaceId *reachableLocations = GvGetReachable(hv->gv, player, round, current, numReturnedLocs);

	if (reachableLocations == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}

	return reachableLocations;
}

// Gets all the locations that the current hunter player can move to this round, consider the rail and boat.
PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	Round round = HvGetRound(hv);

	// unknown place of the player
	if (round == 0) {
		*numReturnedLocs = 0;
		return NULL;
	}

	// get all the reachableLocations
	PlaceId current = HvGetPlayerLocation(hv, player);

	*numReturnedLocs = 0;
	PlaceId *reachableByType = GvGetReachableByType(hv->gv, player, round, current, 
								road, rail, boat, numReturnedLocs);
	
	if (reachableByType == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}
	return reachableByType;
}

// Gets all the locations that the given player can move to in their next move
PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// if the given player is hunter
	if (player != PLAYER_DRACULA) {
		
		// get the next round
		Round round = HvGetRound(hv);
		if (HvGetPlayer(hv) != PLAYER_LORD_GODALMING) {
			round++;
		}
		
		// get all the reachableLocations
		PlaceId current = HvGetPlayerLocation(hv, player);
		*numReturnedLocs = 0;
		PlaceId *reachableLocations = GvGetReachable(hv->gv, player, round, current, numReturnedLocs);

		if (reachableLocations == NULL) {
			*numReturnedLocs = 0;
			return NULL;
		}

		return reachableLocations;

	// if the given player is Darcula
	} else {
		
		// if Darcula's current location is unknown
		PlaceId check = HvGetPlayerLocation(hv, player);
		if (check == CITY_UNKNOWN || check == SEA_UNKNOWN) {
			*numReturnedLocs = 0;
			return NULL;

		} else {
			
			// get all the reachableLocations
			Round round = HvGetRound(hv);
			*numReturnedLocs = 0;
			PlaceId *reachableLocations = GvGetReachable(hv->gv, player, round, check, numReturnedLocs);

			if (reachableLocations == NULL) {
				*numReturnedLocs = 0;
				return NULL;
			}

			return reachableLocations;
		}
	}
}

// Gets all the locations that the given player can move to in their next move, consider the rail and boat.
PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// if the given player is hunter
	if (player != PLAYER_DRACULA) {
		
		// get the next round
		Round round = HvGetRound(hv);
		if (HvGetPlayer(hv) != PLAYER_LORD_GODALMING) {
			round++;
		}
		
		// get all the reachableLocations
		PlaceId current = HvGetPlayerLocation(hv, player);
		*numReturnedLocs = 0;
		PlaceId *reachableByType = GvGetReachableByType(hv->gv, player, round, current, 
								road, rail, boat, numReturnedLocs);

		if (reachableByType == NULL) {
			*numReturnedLocs = 0;
			return NULL;
		}

		return reachableByType;

	// if the given player is Darcula
	} else {
		
		// if Darcula's current location is unknown
		PlaceId check = HvGetPlayerLocation(hv, player);
		if (check == CITY_UNKNOWN || check == SEA_UNKNOWN) {
			*numReturnedLocs = 0;
			return NULL;

		} else {
			
			// get all the reachableLocations
			Round round = HvGetRound(hv);
			*numReturnedLocs = 0;
			PlaceId *reachableByType = GvGetReachableByType(hv->gv, player, round, check, 
								road, rail, boat, numReturnedLocs);

			if (reachableByType == NULL) {
				*numReturnedLocs = 0;
				return NULL;
			}

			return reachableByType;

		}
	}
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// check whether it is a Real Place
bool placeRealCheck(PlaceId pid)
{
	return pid >= MIN_REAL_PLACE && pid <= MAX_REAL_PLACE;
}

////////////////////////////////////////////////////////////////////////
// ADT Queue is below 

// create new empty Queue
Queue newQueue()
{
	Queue q;
	q = malloc(sizeof(QueueRep));
	assert(q != NULL);
	q->head = NULL;
	q->tail = NULL;
	return q;
}

// free memory used by Queue
void dropQueue(Queue Q)
{
	QueueNode *curr, *next;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	// free queue rep
	free(Q);
}

// display as 3 > 5 > 4 > ...
void showQueue(Queue Q)
{
	QueueNode *curr;
	assert(Q != NULL);
	// free list nodes
	curr = Q->head;
	while (curr != NULL) {
		ItemShow(curr->value);
		if (curr->next != NULL)
			printf(">");
		curr = curr->next;
	}
	printf("\n");
}

// add item at end of Queue
void QueueJoin(Queue Q, Item it)
{
	assert(Q != NULL);
	QueueNode *new = malloc(sizeof(QueueNode));
	assert(new != NULL);
	new->value = ItemCopy(it);
	new->next = NULL;
	if (Q->head == NULL)
		Q->head = new;
	if (Q->tail != NULL)
		Q->tail->next = new;
	Q->tail = new;
}

// remove item from front of Queue
Item QueueLeave(Queue Q)
{
	assert(Q != NULL);
	assert(Q->head != NULL);
	Item it = ItemCopy(Q->head->value);
	QueueNode *old = Q->head;
	Q->head = old->next;
	if (Q->head == NULL)
		Q->tail = NULL;
	free(old);
	return it;
}

// check for no items
int QueueIsEmpty(Queue Q)
{
	return (Q->head == NULL);
}
