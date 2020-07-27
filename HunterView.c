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

bool placeRealCheck(PlaceId pid);
// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	// TODO: ADD FIELDS HERE
    GameView gv;
};

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

Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv->gv);
}

Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv->gv);
}

int HvGetScore(HunterView hv)
{
	return GvGetScore(hv->gv);
}

int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv->gv, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv->gv, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv->gv);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	Round current_round = GvGetRound(hv->gv);

	int history_count = 0;
	bool canFree = false;
	PlaceId *locations_his = GvGetLocationHistory(hv->gv, PLAYER_DRACULA, &history_count, &canFree);

	bool is_found = false;

	PlaceId lastKnown;

	for (int i = 0; i <= history_count; i++) {
		if (placeRealCheck(locations_his[history_count - 1 - i]) == false) {
			current_round--;
		} else {
			is_found = true;
			lastKnown = locations_his[history_count - 1 - i];
			current_round--;
			break;
		}
	}

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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*pathLength = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{

	Player player = HvGetPlayer(hv);
	Round round = HvGetRound(hv);
	PlaceId current = HvGetPlayerLocation(hv, player);

	*numReturnedLocs = 0;

	PlaceId *reachableLocations = GvGetReachable(hv->gv, player, round, current, numReturnedLocs);

	if (reachableLocations == NULL) {
		*numReturnedLocs = 0;
		return NULL;
	}

	return reachableLocations;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	Round round = HvGetRound(hv);
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

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

bool placeRealCheck(PlaceId pid)
{
	return pid >= MIN_REAL_PLACE && pid <= MAX_REAL_PLACE;
}
