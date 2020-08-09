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

	// get hunters locations
	PlaceId G_cur_loc = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
	PlaceId S_cur_loc = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
	PlaceId H_cur_loc = DvGetPlayerLocation(dv, PLAYER_VAN_HELSING);
	PlaceId M_cur_loc = DvGetPlayerLocation(dv, PLAYER_MINA_HARKER);
	PlaceId D_cur_loc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	// 
	PlaceId hunters_locs[HUNTER_NUMBER] = {G_cur_loc, S_cur_loc, H_cur_loc, M_cur_loc};
	//get current round
    Round cur_round = DvGetRound(dv);
	
	// partition palces
	PlaceId region1[12] = {
		ATLANTIC_OCEAN, ENGLISH_CHANNEL, DUBLIN, EDINBURGH, GALWAY, LIVERPOOL,
		LONDON, MANCHESTER, PLYMOUTH, SWANSEA, IRISH_SEA, NORTH_SEA
	};
	PlaceId region2[16] = {
		CADIZ, LISBON, MADRID, GRANADA, ALICANTE, SARAGOSSA, SANTANDER, BARCELONA, 
		BORDEAUX, TOULOUSE, CAGLIARI, MARSEILLES, CLERMONT_FERRAND, NANTES, 
		BAY_OF_BISCAY, MEDITERRANEAN_SEA
	};
	PlaceId region3[24] = {
		HAMBURG, BERLIN, AMSTERDAM, COLOGNE, LEIPZIG, PRAGUE, FRANKFURT, BRUSSELS, 
		LE_HAVRE, PARIS, STRASBOURG, NUREMBURG, MUNICH, MILAN, GENEVA, GENOA, 
		VENICE, ZURICH, ADRIATIC_SEA, BARI, NAPLES, ROME, FLORENCE, TYRRHENIAN_SEA
	};
	PlaceId region4[19] = {
		BUDAPEST, VIENNA, ZAGREB, ST_JOSEPH_AND_ST_MARY, SZEGED, SARAJEVO, BELGRADE, 
		CASTLE_DRACULA, GALATZ, CONSTANTA, KLAUSENBURG, BUCHAREST, 
		VARNA, SOFIA, SALONICA, VALONA, ATHENS, BLACK_SEA, IONIAN_SEA
	};
	// loop region 
	PlaceId lop_region1[18] = {
		ATLANTIC_OCEAN, ENGLISH_CHANNEL, DUBLIN, EDINBURGH, GALWAY, LIVERPOOL,
		LONDON, MANCHESTER, PLYMOUTH, SWANSEA, IRISH_SEA, NORTH_SEA,
		LE_HAVRE, AMSTERDAM, HAMBURG, BAY_OF_BISCAY, LISBON, CADIZ

	};
	PlaceId lop_region2[23] = {
		CADIZ, LISBON, MADRID, GRANADA, ALICANTE, SARAGOSSA, SANTANDER, BARCELONA, 
		BORDEAUX, TOULOUSE, CAGLIARI, MARSEILLES, CLERMONT_FERRAND, NANTES, 
		BAY_OF_BISCAY, MEDITERRANEAN_SEA,
		ATLANTIC_OCEAN, GENOA, TYRRHENIAN_SEA, LE_HAVRE, PARIS, GENEVA, MILAN	
	};
	PlaceId lop_region3[34] = {
		HAMBURG, BERLIN, AMSTERDAM, COLOGNE, LEIPZIG, PRAGUE, FRANKFURT, BRUSSELS, 
		LE_HAVRE, PARIS, STRASBOURG, NUREMBURG, MUNICH, MILAN, GENEVA, GENOA, 
		VENICE, ZURICH, ADRIATIC_SEA, BARI, NAPLES, ROME, FLORENCE, TYRRHENIAN_SEA,
		ENGLISH_CHANNEL, NORTH_SEA, NANTES, CLERMONT_FERRAND, MARSEILLES, CAGLIARI,
		VIENNA, ZAGREB, SARAJEVO, IONIAN_SEA
	};
	PlaceId lop_region4[23] = {
		BUDAPEST, VIENNA, ZAGREB, SZEGED, SARAJEVO, BELGRADE, 
		CASTLE_DRACULA, GALATZ, CONSTANTA, KLAUSENBURG, BUCHAREST, 
		VARNA, SOFIA, SALONICA, VALONA, ATHENS, BLACK_SEA, IONIAN_SEA, 
		PRAGUE, MUNICH, VENICE, ADRIATIC_SEA, TYRRHENIAN_SEA
	};

	// An array contain the best places for born palces
	PlaceId expect_born[14] = {
		GRANADA, EDINBURGH, TOULOUSE, AMSTERDAM, HAMBURG, ATHENS, 
		NANTES, PLYMOUTH, GENOA, NAPLES, BARI, VARNA, CADIZ, SARAJEVO
	};	
	
	// count the number of hunter in each region 
	int num_region_1 = 0;
	int num_region_2 = 0;
	int num_region_3 = 0;
	int num_region_4 = 0;
	for (int i = 0; i < HUNTER_NUMBER; i++) {
		if (placesContains(region1, 12, hunters_locs[i])) {
			num_region_1++;
		}
		if (placesContains(region2, 16, hunters_locs[i])) {
			num_region_2++;
		}
		if (placesContains(region3, 24, hunters_locs[i])) {
			num_region_3++;
		}
		if (placesContains(region4, 19, hunters_locs[i])) {
			num_region_4++;
		}				
	}	
	// fill an array which contaions all hunters reachable places
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


	if (cur_round == 0) {
		// choose the best born city
		if (region1 == 0) {
			registerBestPlay("ED", "A BA A BA!");
		} else if (region2 == 0) {
			registerBestPlay("GR", "A BA A BA!");
		} else if (region4 == 0) {
			registerBestPlay("VR", "A BA A BA!");
		} else if (region3 == 0) {
			registerBestPlay("HA", "A BA A BA!");
		} else {			
			for (int n = 0; n < numHazardLocs; n++) {
				if (placesContains(expect_born, 14, hazard_palces[n])) {
					placesDelete(expect_born, 14, hazard_palces[n]);				
				}
			}
			PlaceId next_move = expect_born[0];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "A BA A BA!");	
		}
	} else {
		// chech current palces locate in whick region
		int drac_region_1 = 0;
		int drac_region_2 = 0;
		int drac_region_3 = 0;
		int drac_region_4 = 0;
		if (placesContains(region1, 12, D_cur_loc)) {
			drac_region_1 = 1;
		}
		if (placesContains(region2, 16, D_cur_loc)) {
			drac_region_2 = 1;
		}
		if (placesContains(region3, 24, D_cur_loc)) {
			drac_region_3 = 1;
		}
		if (placesContains(region4, 19, D_cur_loc)) {
			drac_region_4 = 1;
		}

		// drac vaild moves
		int numLocs = 0;
		PlaceId *moves = DvGetValidMoves(dv, &numLocs);
		int numVaild = 0;
		PlaceId vaildMoves[MAX_REAL_PLACE];
		for (int i = 0; i < numLocs; i++) {
			vaildMoves[numVaild] = moves[i];
			numVaild++;
		}
		// an array contain all best moves
		int num_best_move = 0;
		PlaceId best_move[MAX_REAL_PLACE];
		// if current region do not contain hunters, loopping	
		if (drac_region_1 == 1 && num_region_1 == 0) {
			for (int n = 0; n < numVaild; n++) {
				if (placeIsReal(vaildMoves[n])) {
					if (placesContains(lop_region1, 18, vaildMoves[n])) {
						best_move[num_best_move] = vaildMoves[n];
						num_best_move++;				
					} 
				} else {
					best_move[num_best_move] = vaildMoves[n];
					num_best_move++;
				}
			}
			// pick one in best palces
			PlaceId next_move = best_move[(cur_round - 1) % num_best_move];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Region one lopping...");		
		} else if (drac_region_2 == 1 && num_region_2 == 0) {
			for (int n = 0; n < numVaild; n++) {
				if (placeIsReal(vaildMoves[n])) {
					if (placesContains(lop_region2, 23, vaildMoves[n])) {
						best_move[num_best_move] = vaildMoves[n];
						num_best_move++;				
					}
				} else {
					best_move[num_best_move] = vaildMoves[n];
					num_best_move++;
				}
			}
			// pick one in best palces	
			PlaceId next_move = best_move[(cur_round - 1) % num_best_move];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Region two lopping...");	
		} else if (drac_region_3 == 1 && num_region_3 == 0) {
			for (int n = 0; n < numVaild; n++) {
				if (placeIsReal(vaildMoves[n])) {
					if (placesContains(lop_region3, 34, vaildMoves[n])) {
						best_move[num_best_move] = vaildMoves[n];
						num_best_move++;				
					}
				} else {
					best_move[num_best_move] = vaildMoves[n];
					num_best_move++;
				}
			}
			// pick one in best palces
			PlaceId next_move = best_move[(cur_round - 1) % num_best_move];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Region three lopping...");	
		} else if (drac_region_4 == 1 && num_region_4 == 0) {
			for (int n = 0; n < numVaild; n++) {
				if (placeIsReal(vaildMoves[n])) {
					if (placesContains(lop_region4, 23, vaildMoves[n])) {
						best_move[num_best_move] = vaildMoves[n];
						num_best_move++;				
					}
				} else {
					best_move[num_best_move] = vaildMoves[n];
					num_best_move++;
				}
			}
			// pick one in best palces			
			PlaceId next_move = best_move[(cur_round - 1) % num_best_move];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "Region four lopping...");	
		} else {
			// random move
			// remove the hunters reachable palces			
			for (int n = 0; n < numHazardLocs; n++) {
				if (placesContains(vaildMoves, numVaild, hazard_palces[n])) {
					placesDelete(vaildMoves, numVaild, hazard_palces[n]);				
				}
			}			
			// pick one in vaild moves
			PlaceId next_move = vaildMoves[(cur_round - 1) % numVaild];
			char *xx = placeIdToAbbrev(next_move);
			registerBestPlay(xx, "GO GO GO!");	
		}   
	}
}

