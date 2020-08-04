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


void decideDraculaMove(DraculaView dv)
{	
	Round cur_round = DvGetRound(dv);
	
	PlaceId within_20_round[21] = {GALWAY, ATLANTIC_OCEAN, IRISH_SEA, DUBLIN, 
	 DOUBLE_BACK_4, HIDE, TELEPORT, GALATZ, CONSTANTA, VARNA, BLACK_SEA, 
	 IONIAN_SEA, TYRRHENIAN_SEA, CAGLIARI, MEDITERRANEAN_SEA, ATLANTIC_OCEAN, 
	 GALWAY, DUBLIN, DOUBLE_BACK_2, HIDE, TELEPORT};
	
	PlaceId play_with_you[12] = {GALATZ, CONSTANTA, BLACK_SEA, IONIAN_SEA, 
	 TYRRHENIAN_SEA, MEDITERRANEAN_SEA, ADRIATIC_SEA, GALWAY, DUBLIN, DOUBLE_BACK_2,
	 HIDE, TELEPORT};

	if (cur_round < 21) {
		PlaceId next_move = within_20_round[cur_round];
		char *xx = placeIdToAbbrev(next_move);
		registerBestPlay(xx, "Mwahahahaha");
	} else {
		cur_round = (cur_round - 21) % 12;
		PlaceId next_move = play_with_you[cur_round];
		char *xx = placeIdToAbbrev(next_move);
		registerBestPlay(xx, "Mwahahahaha");
	}
}
