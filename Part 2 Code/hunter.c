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

	// some current info
	Round cur_round = HvGetRound(hv);
	Player cur_player = HvGetPlayer(hv);
	PlaceId cur_place = HvGetPlayerLocation(hv, cur_player);
	int hp = HvGetHealth(hv, cur_player);
	if (cur_round == 0) {
		// hunters' born places
		if (cur_player == PLAYER_LORD_GODALMING) {
			registerBestPlay("GW", "GIAO GIAO!");	
		} else if (cur_player == PLAYER_DR_SEWARD) {
			registerBestPlay("CN", "GIAO GIAO!");					
		} else if (cur_player == PLAYER_VAN_HELSING) {
			registerBestPlay("MA", "GIAO GIAO!");	
		} else {
			registerBestPlay("FR", "GIAO GIAO!");	
		}
	} else {
		// search the last plays
		int drac_round = 0;
		PlaceId drac_find = HvGetLastKnownDraculaLocation(hv, &drac_round);
		int diff_round = cur_round - drac_round;
		if (hp < 5) {			
			// if hunter hp is less than 5
			// stay here
			char *xx = placeIdToAbbrev(cur_place);
			registerBestPlay(xx, "HP+++++++");
		} else if (cur_round > 6 && drac_find == NOWHERE) {
			// if current round biger than 6
			// hunters can 'research' to find drac last known city
			char *xx = placeIdToAbbrev(cur_place);
			registerBestPlay(xx, "Research!");		
		} else if (diff_round > 6 && drac_find != NOWHERE) {
			// if diff is biger than 6
			// hunters neend 'research' a new drac last known city
			char *xx = placeIdToAbbrev(cur_place);
			registerBestPlay(xx, "Research!");
		} else if (drac_find != NOWHERE && drac_find != cur_place) {
			// if find drac's trail is known 
			// find the shorest path to this place
			int length = 0;
			PlaceId *path = HvGetShortestPathTo(hv, cur_player, drac_find, &length);
			PlaceId next_move = path[0];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "I'll catch you!");
		} else if (drac_find == cur_place && diff_round == 1) {
			// if drac find is same as current city and diff is 1
			// i.e. current hunter and drac in same city
			char *xx = placeIdToAbbrev(cur_place);
			registerBestPlay(xx, "Fight!");		
		} else if (drac_find == cur_place && diff_round < 3 && diff_round > 1)  {
			// if arrive at drac lase known city
			// diff is between 1 and 3
			// search road and boat
			int numcanGo = 0; 		
			PlaceId *canGo = HvWhereCanIGoByType(hv, true, false, true, &numcanGo);
			// remove the current places 
			if (placesContains(canGo, numcanGo, cur_place)) {
				placesDelete(canGo, numcanGo, cur_place);				
			}
			// random pick one move
			PlaceId next_move = canGo[(cur_round - 1) % numcanGo];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Searching...(mode 1)");		
		} else if (drac_find == cur_place && diff_round >= 3) {
			// if arrive at drac lase known city
			// diff is biger than 3
			// search road and boat and rail
			// an array contaion all city have rail
			PlaceId rail_cities[44] = {EDINBURGH, MANCHESTER, LIVERPOOL, LONDON, SWANSEA,
				LISBON, MADRID, ALICANTE, SANTANDER, SARAGOSSA, BARCELONA, BORDEAUX,
				LE_HAVRE, PARIS, MARSEILLES, BRUSSELS, COLOGNE, FRANKFURT, STRASBOURG,
				ZURICH, GENEVA, MILAN, GENOA, FLORENCE, ROME, NAPLES, BARI, VARNA,
				LEIPZIG, BERLIN, HAMBURG, NUREMBURG, MUNICH, PRAGUE, VIENNA, VENICE,
				BUDAPEST, SZEGED, BUCHAREST, GALATZ, CONSTANTA, BELGRADE, SOFIA, SALONICA
			};			
			int numcanGo = 0;
			PlaceId *canGo = NULL;
			if (placesContains(rail_cities, 44, cur_place)) {
				canGo = HvWhereCanIGoByType(hv, false, true, true, &numcanGo);
			} else {
				canGo = HvWhereCanIGoByType(hv, true, true, true, &numcanGo);
			}		
			// remove the current places 
			if (placesContains(canGo, numcanGo, cur_place)) {
				placesDelete(canGo, numcanGo, cur_place);				
			}
			// random pick one move		
			PlaceId next_move = canGo[(cur_round - 1) % numcanGo];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Searching...(mode 2)");									
		} else {				
			// random move
			int numcanGo = 0; 
			PlaceId *canGo = HvWhereCanIGo(hv, &numcanGo);
			// remove the current places 
			if (placesContains(canGo, numcanGo, cur_place)) {
				placesDelete(canGo, numcanGo, cur_place);				
			}
			// random pick one move				
			PlaceId next_move = canGo[(cur_round - 1) % numcanGo];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Random move");			
		}
	}
}
