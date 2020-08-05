////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"

#include "Places.h"
#include "utils.h"

#define HUNTER_NUMBER 4

void decideDraculaMove(DraculaView dv)
{	
	Round cur_round = DvGetRound(dv);

	PlaceId loopingggg[16] = {KLAUSENBURG, GALATZ, DOUBLE_BACK_3, 
		HIDE, TELEPORT, TELEPORT, CASTLE_DRACULA, GALATZ, BUCHAREST, SOFIA, 
		VALONA, IONIAN_SEA, ATHENS, DOUBLE_BACK_1, HIDE, TELEPORT};

	PlaceId loopingg[9] = {GALATZ, BUCHAREST, SOFIA, 
		VALONA, IONIAN_SEA, ATHENS, DOUBLE_BACK_1, HIDE, TELEPORT};


	// case 1
	if (cur_round == 0) {
		char *xx = placeIdToAbbrev(CASTLE_DRACULA);
		registerBestPlay(xx, "A BA A BA!");
	}
	if (cur_round > 0 && cur_round <= 16) {
		int i = cur_round - 1;
		PlaceId next_move = loopingggg[i];
		char *xx = placeIdToAbbrev(next_move);
		registerBestPlay(xx, "A BA A BA!");
	} 
	if (cur_round > 0 && cur_round > 16) {
		int i = (cur_round - 17) % 9;
		PlaceId next_move = loopingg[i];
		char *xx = placeIdToAbbrev(next_move);
		registerBestPlay(xx, "A BA A BA!");
	}

/*
	if (cur_round == 0) {
		registerBestPlay("HA", "A BA A BA!");
	}
	if (cur_round > 0) {
		int numLocs = 0;
		PlaceId *moves = DvGetValidMoves(dv, &numLocs);
		int numVaild = 0;
		PlaceId vaildMoves[MAX_REAL_PLACE];
		for (int i = 0; i < numLocs; i++) {
			vaildMoves[numVaild] = moves[i];
			numVaild++;
		}
		
		int numLocs_hunters = 0;
		int numHazardLocs = 0;
		PlaceId hazard_palces[MAX_REAL_PLACE];
		PlaceId *locs = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numLocs_hunters);
		for (int j = 0; j < numLocs_hunters; j++) {
			hazard_palces[numHazardLocs] = locs[j];
			numHazardLocs++;
		}
		locs = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &numLocs_hunters);
		for (int j = 0; j < numLocs_hunters; j++) {
			hazard_palces[numHazardLocs] = locs[j];
			numHazardLocs++;
		}				
		locs = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &numLocs_hunters);
		for (int j = 0; j < numLocs_hunters; j++) {
			hazard_palces[numHazardLocs] = locs[j];
			numHazardLocs++;
		}
		locs = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, &numLocs_hunters);
		for (int j = 0; j < numLocs_hunters; j++) {
			hazard_palces[numHazardLocs] = locs[j];
			numHazardLocs++;
		}		
		
		for (int n = 0; n < numHazardLocs; n++) {
			if (placesContains(vaildMoves, numVaild, hazard_palces[n])) {
				placesDelete(vaildMoves, numVaild, hazard_palces[n]);				
			}
		}
		
		if (numVaild == 0) {
			registerBestPlay("TP", "A BA A BA!");		
		} else {
			PlaceId next_move = vaildMoves[(cur_round - 1) % numVaild];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "A BA A BA!");	
		}
	}   
*/
}
