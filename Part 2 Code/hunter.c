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
	PlaceId cur_palce = HvGetPlayerLocation(hv, cur_player);
	int hp = HvGetHealth(hv, cur_player);

	if (cur_player == PLAYER_LORD_GODALMING) {				
		if (cur_round == 0) {
			char *xx = "GR";
			registerBestPlay(xx, "Have we nothing Toulouse?");		
		} else {
			// if hunter hp is less than 5
			// stay here
			if (hp < 5) {
				char *xx = placeIdToAbbrev(cur_palce);
				registerBestPlay(xx, "Have we nothing Toulouse?");	
			}
			// patrol area
			PlaceId patrol[15] = {CADIZ, LISBON, MADRID, GRANADA, ALICANTE,
				SARAGOSSA, SANTANDER, BARCELONA, BORDEAUX, TOULOUSE, CAGLIARI,
				MARSEILLES, NANTES, MEDITERRANEAN_SEA, CLERMONT_FERRAND}; 
			
			// an array to collect all best moves for hunter
			PlaceId best_moves[MAX_REAL_PLACE];
			int count = 0;
			
			// get the hunter reachable places
			int numcanGo = 0; 
			PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
			// remove the current palce
			placesDelete(canGo, numcanGo, cur_palce);
			for (int i = 0; i < 15; i++) {
				if (placesContains(canGo, numcanGo, patrol[i])) {
					best_moves[count] = patrol[i];
					count++;
				}
			}
			// randomly pick a best move frome array
			PlaceId next_move = best_moves[cur_round % count];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Have we nothing Toulouse?");		
		}

	}
	
	if (cur_player == PLAYER_DR_SEWARD) {
		if (cur_round == 0) {
			char *xx = "SZ";
			registerBestPlay(xx, "Have we nothing Toulouse?");		
		} else {
			// if hunter hp is less than 5
			// stay here
			if (hp < 5) {
				char *xx = placeIdToAbbrev(cur_palce);
				registerBestPlay(xx, "Have we nothing Toulouse?");	
			}
			// an array to collect all best moves for hunter
			PlaceId best_moves[MAX_REAL_PLACE];
			int count = 0;

			// find drac reachable places
			int numLocs = 0;
			PlaceId *locs = HvWhereCanTheyGo(hv, PLAYER_DRACULA, &numLocs);
			// if find
			if (locs != NULL) {
				// get the hunter reachable places
				int numcanGo = 0; 
				PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
				for (int i = 0; i < numcanGo; i++) {
					if (placesContains(locs, numLocs, canGo[i])) {
						best_moves[count] = canGo[i];
						count++;
					}
				}
				// randomly pick a best move frome array
				PlaceId next_move = best_moves[cur_round % count];
				char *xx = placeIdToAbbrev(next_move);
				registerBestPlay(xx, "Have we nothing Toulouse?");				
			}

			// search the last plays
			int drac_round = 0;
			PlaceId drac_find = HvGetLastKnownDraculaLocation(hv, &drac_round);
			// if find drac's trail is known 
			// find the shorest path to this place
			if (drac_find != NOWHERE) {
				int length = 0;
				PlaceId *path = HvGetShortestPathTo(hv, cur_player, drac_find, &length);
				PlaceId next_move = path[0];
				char *xx = placeIdToAbbrev(next_move);
				registerBestPlay(xx, "Have we nothing Toulouse?");					
			}
			
			// normal move
			int numcanGo = 0; 
			PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
			// remove the current paces 
			if (placesContains(canGo, numcanGo, cur_palce)) {
				placesDelete(canGo, numcanGo, cur_palce);				
			}			
			PlaceId next_move = canGo[cur_round % numcanGo];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Have we nothing Toulouse?");	
		}
	}	
	
	if (cur_player == PLAYER_VAN_HELSING) {
		if (cur_round == 0) {
			char *xx = "GW";
			registerBestPlay(xx, "Have we nothing Toulouse?");		
		} else {
			if (hp < 5) {
				char *xx = placeIdToAbbrev(cur_palce);
				registerBestPlay(xx, "Have we nothing Toulouse?");	
			}
			// patrol area
			PlaceId patrol[14] = {GALWAY, DUBLIN, IRISH_SEA, LIVERPOOL, 
				SWANSEA, LONDON, MANCHESTER, ENGLISH_CHANNEL, EDINBURGH,
				PLYMOUTH, AMSTERDAM, NORTH_SEA, LE_HAVRE, BRUSSELS}; 
			
			// an array to collect all best moves for hunter
			PlaceId best_moves[1000];
			int count = 0;
			
			// get the hunter reachable places
			int numcanGo = 0; 
			PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
			// remove the current palce
			placesDelete(canGo, numcanGo, cur_palce);
			for (int i = 0; i < 14; i++) {
				if (placesContains(canGo, numcanGo, patrol[i])) {
					best_moves[count] = patrol[i];
					count++;
				}
			}
			// randomly pick a best move frome array
			PlaceId next_move = best_moves[cur_round % count];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Have we nothing Toulouse?");					
		}

	}	
	
	if (cur_player == PLAYER_MINA_HARKER) {
		if (cur_round == 0) {
			char *xx = "FR";
			registerBestPlay(xx, "Have we nothing Toulouse?");		
		} else {
			// if hunter hp is less than 5
			// stay here
			if (hp < 5) {
				char *xx = placeIdToAbbrev(cur_palce);
				registerBestPlay(xx, "Have we nothing Toulouse?");	
			}
			// an array to collect all best moves for hunter
			PlaceId best_moves[1000];
			int count = 0;

			// find drac reachable places
			int numLocs = 0;
			PlaceId *locs = HvWhereCanTheyGo(hv, PLAYER_DRACULA, &numLocs);
			// if find
			if (locs != NULL) {
				// get the hunter reachable places
				int numcanGo = 0; 
				PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
				for (int i = 0; i < numcanGo; i++) {
					if (placesContains(locs, numLocs, canGo[i])) {
						best_moves[count] = canGo[i];
						count++;
					}
				}
				// randomly pick a best move frome array
				PlaceId next_move = best_moves[cur_round % count];
				char *xx = placeIdToAbbrev(next_move);
				registerBestPlay(xx, "Have we nothing Toulouse?");				
			}

			// search the last plays
			int drac_round = 0;
			PlaceId drac_find = HvGetLastKnownDraculaLocation(hv, &drac_round);
			// if find drac's trail is known 
			// find the shorest path to this place
			if (drac_find != NOWHERE) {
				int length = 0;
				PlaceId *path = HvGetShortestPathTo(hv, cur_player, drac_find, &length);
				PlaceId next_move = path[0];
				char *xx = placeIdToAbbrev(next_move);
				registerBestPlay(xx, "Have we nothing Toulouse?");					
			}
			
			// normal move
			int numcanGo = 0; 
			PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
			// remove the current paces 
			if (placesContains(canGo, numcanGo, cur_palce)) {
				placesDelete(canGo, numcanGo, cur_palce);				
			}			
			PlaceId next_move = canGo[cur_round % numcanGo];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Have we nothing Toulouse?");	
		}	
	}	


}
