////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

#include "Places.h"
#include "utils.h"

void decideHunterMove(HunterView hv)
{
	Round cur_round = HvGetRound(hv);
	Player cur_player = HvGetPlayer(hv);
	PlaceId cur_place = HvGetPlayerLocation(hv, cur_player);
	int hp = HvGetHealth(hv, cur_player);

	if (cur_player == PLAYER_LORD_GODALMING) {				
		if (cur_round == 0) {
			char *xx = "GW";
			registerBestPlay(xx, "Wo si yi ge BAOAN!");		
		} else {			
			// search the last plays
			int drac_round = 0;
			PlaceId drac_find = HvGetLastKnownDraculaLocation(hv, &drac_round);

			if (hp < 5 || drac_find == cur_place) {
				char *xx = placeIdToAbbrev(cur_place);
				registerBestPlay(xx, "LEI TING GA BA!");	
			} else if (cur_round > 6 && drac_find == NOWHERE) {
				char *xx = placeIdToAbbrev(cur_place);
				registerBestPlay(xx, "LEI TING GA BA!");				
			} else {
				// patrol area
				PlaceId patrol[14] = {GALWAY, DUBLIN, IRISH_SEA, LIVERPOOL, 
					SWANSEA, LONDON, MANCHESTER, ENGLISH_CHANNEL, EDINBURGH,
					PLYMOUTH, AMSTERDAM, NORTH_SEA, LE_HAVRE, BRUSSELS}; 
				
				// an array to collect all best moves for hunter
				PlaceId best_moves[MAX_REAL_PLACE];
				int count = 0;
				
				// get the hunter reachable places
				int numcanGo = 0; 
				PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
				// remove the current palce
				placesDelete(canGo, numcanGo, cur_place);
				for (int i = 0; i < numcanGo; i++) {
					if (placesContains(patrol, 14, canGo[i])) {
						best_moves[count] = canGo[i];
						count++;
					}
				}
				// randomly pick a best move frome array
				PlaceId next_move = best_moves[(cur_round - 1) % count];
				char *xx = placeIdToAbbrev(next_move);
				registerBestPlay(xx, "Xun Luo zhong...");					
			}
		}
	} else {
		// search the last plays
		int drac_round = 0;
		PlaceId drac_find = HvGetLastKnownDraculaLocation(hv, &drac_round);
		if (cur_round == 0) {
			if (cur_player == PLAYER_DR_SEWARD) {
				registerBestPlay("SZ", "GIAO GIAO!");					
			} else if (cur_player == PLAYER_VAN_HELSING) {
				registerBestPlay("MA", "GIAO GIAO!");	
			} else {
				registerBestPlay("FR", "GIAO GIAO!");	
			}
		} else if (hp < 5 || drac_find == cur_place) {			
			// if hunter hp is less than 5
			// stay here
				char *xx = placeIdToAbbrev(cur_place);
				registerBestPlay(xx, "LEI TING GA BA!");
		} else if (drac_find != NOWHERE) {
			// if find drac's trail is known 
			// find the shorest path to this place
			int length = 0;
			PlaceId *path = HvGetShortestPathTo(hv, cur_player, drac_find, &length);
			PlaceId next_move = path[0];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Bie pao, MEI MEI!");

		} else if (cur_round > 6 && drac_find == NOWHERE) {
				char *xx = placeIdToAbbrev(cur_place);
				registerBestPlay(xx, "Yan jiu Yan jiu!");				
		} else {				
			// normal move
			int numcanGo = 0; 
			PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
			// remove the current paces 
			if (placesContains(canGo, numcanGo, cur_place)) {
				placesDelete(canGo, numcanGo, cur_place);				
			}			
			PlaceId next_move = canGo[(cur_round - 1) % numcanGo];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "HUO HUO!");		
		}
				
	}

}
