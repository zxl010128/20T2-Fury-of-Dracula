////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testGameView.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{
	{///////////////////////////////////////////////////////////////////
	
		printf("Basic initialisation\n");

		char *trail = "";
		Message messages[] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == NOWHERE);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("After Lord Godalming's turn\n");

		char *trail =
			"GST....";
		
		Message messages[1] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DR_SEWARD);
		assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == NOWHERE);

		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("After Mina Harker's turn\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[4] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DRACULA);
		assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == ZURICH);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == NOWHERE);

		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("After Dracula's turn\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?.V..";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha"
		};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 1);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetScore(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == ZURICH);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == CITY_UNKNOWN);

		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD..";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha",
			"Aha!"
		};
		
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) ==
				GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == GENEVA);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("Test passed\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula doubling back at sea, "
		       "and losing blood points (Hunter View)\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		
		Message messages[] = {
			"Party at Geneva", "Okay", "Sure", "Let's go", "Mwahahahaha",
			"", "", "", "", "Back I go"
		};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 2);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetScore(gv) == GAME_START_SCORE - 2 * SCORE_LOSS_DRACULA_TURN);
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == SEA_UNKNOWN);

		GvFree(gv);
		printf("Test passed!\n");
	}


	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula doubling back at sea, "
			   "and losing blood points (Dracula View)\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1.... "
			"GFR.... SFR.... HFR.... MFR....";
		
		Message messages[14] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 2);
		assert(GvGetPlayer(gv) == PLAYER_DRACULA);
		assert(GvGetScore(gv) == GAME_START_SCORE - 2 * SCORE_LOSS_DRACULA_TURN);
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == ENGLISH_CHANNEL);

		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Checking that hunters' health points are capped\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE....";
	
		Message messages[6] = {};
		GameView gv = GvNew(trail, messages);
	
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing a hunter 'dying'\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GSTTTTD";
		
		Message messages[21] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetScore(gv) == GAME_START_SCORE
		                         - 4 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_HUNTER_HOSPITAL);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 0);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == STRASBOURG);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Dracula doubling back to Castle Dracula\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T...";
		
		Message messages[10] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS + (2 * LIFE_GAIN_CASTLE_DRACULA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing vampire/trap locations\n");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DCD.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GSZ.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == GALATZ);
		assert(GvGetVampireLocation(gv) == CASTLE_DRACULA);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 2);
		sortPlaces(traps, numTraps);
		assert(traps[0] == GALATZ && traps[1] == KLAUSENBURG);
		free(traps);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing trap locations after one is destroyed\n");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DBC.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GBE.... SGE.... HGE.... MGE.... DCNT... "
			"GKLT... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) ==
				GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_TRAP_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == KLAUSENBURG);
		assert(GvGetVampireLocation(gv) == BUCHAREST);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 2);
		sortPlaces(traps, numTraps);
		assert(traps[0] == CONSTANTA && traps[1] == GALATZ);
		free(traps);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing a vampire maturing\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T.V.";
		
		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetScore(gv) == GAME_START_SCORE
		                         - 7 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing move/location history\n");
		
		char *trail =
			"GLS.... SGE.... HGE.... MGE.... DST.V.. "
			"GCA.... SGE.... HGE.... MGE.... DC?T... "
			"GGR.... SGE.... HGE.... MGE.... DC?T... "
			"GAL.... SGE.... HGE.... MGE.... DD3T... "
			"GSR.... SGE.... HGE.... MGE.... DHIT... "
			"GSN.... SGE.... HGE.... MGE.... DC?T... "
			"GMA.... SSTTTV.";
		
		Message messages[32] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_DR_SEWARD) ==
				GAME_START_HUNTER_LIFE_POINTS - 2 * LIFE_LOSS_TRAP_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		
		// Lord Godalming's move/location history
		{
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_LORD_GODALMING,
			                                  &numMoves, &canFree);
			assert(numMoves == 7);
			assert(moves[0] == LISBON);
			assert(moves[1] == CADIZ);
			assert(moves[2] == GRANADA);
			assert(moves[3] == ALICANTE);
			assert(moves[4] == SARAGOSSA);
			assert(moves[5] == SANTANDER);
			assert(moves[6] == MADRID);
			if (canFree) free(moves);
		}
		
		// Lord Godalming's location history
		{
			int numLocs = 0; bool canFree = false;
			PlaceId *locs = GvGetLocationHistory(gv, PLAYER_LORD_GODALMING,
			                                  &numLocs, &canFree);
			assert(numLocs == 7);
			assert(locs[0] == LISBON);
			assert(locs[1] == CADIZ);
			assert(locs[2] == GRANADA);
			assert(locs[3] == ALICANTE);
			assert(locs[4] == SARAGOSSA);
			assert(locs[5] == SANTANDER);
			assert(locs[6] == MADRID);
			if (canFree) free(locs);
		}
		
		// Dracula's move/location history
		{
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_DRACULA,
			                                  &numMoves, &canFree);
			assert(numMoves == 6);
			assert(moves[0] == STRASBOURG);
			assert(moves[1] == CITY_UNKNOWN);
			assert(moves[2] == CITY_UNKNOWN);
			assert(moves[3] == DOUBLE_BACK_3);
			assert(moves[4] == HIDE);
			assert(moves[5] == CITY_UNKNOWN);
			if (canFree) free(moves);
		}
		
		{
			int numLocs = 0; bool canFree = false;
			PlaceId *locs = GvGetLocationHistory(gv, PLAYER_DRACULA,
			                                     &numLocs, &canFree);
			assert(numLocs == 6);
			assert(locs[0] == STRASBOURG);
			assert(locs[1] == CITY_UNKNOWN);
			assert(locs[2] == CITY_UNKNOWN);
			assert(locs[3] == STRASBOURG);
			assert(locs[4] == STRASBOURG);
			assert(locs[5] == CITY_UNKNOWN);
			if (canFree) free(locs);
		}
		
		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
		printf("Testing connections (GvGetReachable test)\n");

		char *trail = "";
		Message messages[] = {};
		GameView gv = GvNew(trail, messages);
		
		{
			printf("\tChecking Rome raod, rail and boat connections "
			       "(Dr Seward, Round 1)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_DR_SEWARD,
			                                     1, ROME, &numLocs);
			
			assert(numLocs == 6);
			sortPlaces(locs, numLocs);			
			assert(locs[0] == BARI);
			assert(locs[1] == FLORENCE);
			assert(locs[2] == MILAN);
			assert(locs[3] == NAPLES);
			assert(locs[4] == ROME);
			assert(locs[5] == TYRRHENIAN_SEA);			
			free(locs);
		}
		
		{
			printf("\tChecking Constanta raod, rail and boat connections "
			       "(Mina Harker, Round 8)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_MINA_HARKER,
			                                     8, CONSTANTA, &numLocs);
			
			assert(numLocs == 8);
			sortPlaces(locs, numLocs);			
			assert(locs[0] == BELGRADE);
			assert(locs[1] == BLACK_SEA);
			assert(locs[2] == BUCHAREST);
			assert(locs[3] == BUDAPEST);
			assert(locs[4] == CONSTANTA);
			assert(locs[5] == GALATZ);	
			assert(locs[6] == SZEGED);
			assert(locs[7] == VARNA);				
			free(locs);
		}
		
		{
			printf("\tChecking Constanta raod, rail and boat connections "
			       "(Dracula, Round 8)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_DRACULA,
			                                     8, CONSTANTA, &numLocs);
			
			assert(numLocs == 5);
			sortPlaces(locs, numLocs);			
			assert(locs[0] == BLACK_SEA);
			assert(locs[1] == BUCHAREST);
			assert(locs[2] == CONSTANTA);
			assert(locs[3] == GALATZ);	
			assert(locs[4] == VARNA);				
			free(locs);
		}

		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
		printf("Testing connections (GvGetReachableByType test)\n");
		
		char *trail = "";
		Message messages[] = {};
		GameView gv = GvNew(trail, messages);

		{
			printf("\tChecking Galatz road connections "
			       "(Lord Godalming, Round 1)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     1, GALATZ, true, false,
			                                     false, &numLocs);

			assert(numLocs == 5);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BUCHAREST);
			assert(locs[1] == CASTLE_DRACULA);
			assert(locs[2] == CONSTANTA);
			assert(locs[3] == GALATZ);
			assert(locs[4] == KLAUSENBURG);
			free(locs);
		}

		{
			printf("\tChecking Ionian Sea boat connections "
			       "(Lord Godalming, Round 1)\n");
			
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     1, IONIAN_SEA, false, false,
			                                     true, &numLocs);
			
			assert(numLocs == 7);
			sortPlaces(locs, numLocs);
			assert(locs[0] == ADRIATIC_SEA);
			assert(locs[1] == ATHENS);
			assert(locs[2] == BLACK_SEA);
			assert(locs[3] == IONIAN_SEA);
			assert(locs[4] == SALONICA);
			assert(locs[5] == TYRRHENIAN_SEA);
			assert(locs[6] == VALONA);
			free(locs);
		}

		{
			printf("\tChecking Paris rail connections "
			       "(Lord Godalming, Round 2)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     2, PARIS, false, true,
			                                     false, &numLocs);
			
			assert(numLocs == 7);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BORDEAUX);
			assert(locs[1] == BRUSSELS);
			assert(locs[2] == COLOGNE);
			assert(locs[3] == LE_HAVRE);
			assert(locs[4] == MARSEILLES);
			assert(locs[5] == PARIS);
			assert(locs[6] == SARAGOSSA);
			free(locs);
		}
		
		{
			printf("\tChecking Athens rail connections (none)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     1, ATHENS, false, true,
			                                     false, &numLocs);
			
			assert(numLocs == 1);
			assert(locs[0] == ATHENS);
			free(locs);
		}

		{
			printf("\tChecking Frankfurt rail connections "
			       "(Lord Godalming, Round 3)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     3, FRANKFURT, false, true,
			                                     false, &numLocs);
			
			assert(numLocs == 13);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BERLIN);
			assert(locs[1] == BRUSSELS);
			assert(locs[2] == COLOGNE);
			assert(locs[3] == FRANKFURT);
			assert(locs[4] == HAMBURG);
			assert(locs[5] == LEIPZIG);
			assert(locs[6] == MILAN);
			assert(locs[7] == MUNICH);
			assert(locs[8] == NUREMBURG);
			assert(locs[9] == PARIS);
			assert(locs[10] == PRAGUE);
			assert(locs[11] == STRASBOURG);
			assert(locs[11] == STRASBOURG);
			assert(locs[11] == STRASBOURG);
			assert(locs[12] == ZURICH);
			free(locs);
		}	
		
		{
			printf("\tChecking Rome raod, rail and boat connections "
			       "(Dr Seward, Round 1)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_DR_SEWARD,
			                                     1, ROME, true, true,
			                                     true, &numLocs);
			
			assert(numLocs == 6);
			sortPlaces(locs, numLocs);			
			assert(locs[0] == BARI);
			assert(locs[1] == FLORENCE);
			assert(locs[2] == MILAN);
			assert(locs[3] == NAPLES);
			assert(locs[4] == ROME);
			assert(locs[5] == TYRRHENIAN_SEA);			
			free(locs);
		}
		
		{
			printf("\tChecking Constanta raod, rail and boat connections "
			       "(Mina Harker, Round 8)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_MINA_HARKER,
			                                     8, CONSTANTA, true, true,
			                                     true, &numLocs);
			
			assert(numLocs == 8);
			sortPlaces(locs, numLocs);			
			assert(locs[0] == BELGRADE);
			assert(locs[1] == BLACK_SEA);
			assert(locs[2] == BUCHAREST);
			assert(locs[3] == BUDAPEST);
			assert(locs[4] == CONSTANTA);
			assert(locs[5] == GALATZ);	
			assert(locs[6] == SZEGED);
			assert(locs[7] == VARNA);				
			free(locs);
		}
		
		{
			printf("\tChecking Constanta raod, rail and boat connections "
			       "(Dracula, Round 8)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_DRACULA,
			                                     8, CONSTANTA, true, true,
			                                     true, &numLocs);
			
			assert(numLocs == 5);
			sortPlaces(locs, numLocs);			
			assert(locs[0] == BLACK_SEA);
			assert(locs[1] == BUCHAREST);
			assert(locs[2] == CONSTANTA);
			assert(locs[3] == GALATZ);	
			assert(locs[4] == VARNA);				
			free(locs);
		}			

		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Last moves/locations history\n");
		
		char *trail =
			"GLS.... SGE.... HGE.... MGE.... DST.V.. "
			"GCA.... SGE.... HGE.... MGE.... DC?T... "
			"GGR.... SGE.... HGE.... MGE.... DC?T... "
			"GAL.... SGE.... HGE.... MGE.... DD3T... "
			"GSR.... SGE.... HGE.... MGE.... DHIT... "
			"GSN.... SGE.... HGE.... MGE.... DC?T... "
			"GMA.... SSTTTV.";
		
		Message messages[32] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_DR_SEWARD) ==
				GAME_START_HUNTER_LIFE_POINTS - 2 * LIFE_LOSS_TRAP_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		
		// Lord Godalming's last move
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 4;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 4);
			assert(moves[0] == ALICANTE);
			assert(moves[1] == SARAGOSSA);
			assert(moves[2] == SANTANDER);
			assert(moves[3] == MADRID);
			if (canFree) free(moves);
		}		
		
		// Lord Godalming's last location
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 4;
			PlaceId *moves = GvGetLastLocations(gv, PLAYER_LORD_GODALMING, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 4);
			assert(moves[0] == ALICANTE);
			assert(moves[1] == SARAGOSSA);
			assert(moves[2] == SANTANDER);
			assert(moves[3] == MADRID);
			if (canFree) free(moves);
		}
		// Dracula's Last move
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 3;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == DOUBLE_BACK_3);
			assert(moves[1] == HIDE);
			assert(moves[2] == CITY_UNKNOWN);

			if (canFree) free(moves);
		}

		// Dracula's Last location		
		{
			int numLocs = 0; bool canFree = false;
			int recentNum = 3;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DRACULA, recentNum, 
			                                     &numLocs, &canFree);
			assert(numLocs == 3);
			assert(locs[0] == STRASBOURG);
			assert(locs[1] == STRASBOURG);
			assert(locs[2] == CITY_UNKNOWN);
			if (canFree) free(locs);
		}

		// Lord Godalming's last location when num == 0
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 0;
			PlaceId *moves = GvGetLastLocations(gv, PLAYER_LORD_GODALMING, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 0);
			assert(moves == NULL);
			if (canFree) free(moves);
		}

		// Dracula's Last move when num == 0
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 0;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 0);
			assert(moves == NULL);
			if (canFree) free(moves);
		}
		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Last moves/locations history when just start\n");
		
		char *trail = "";
		
		Message messages[0] = {};
		GameView gv = GvNew(trail, messages);
		
		// Lord Godalming's last move when there is no move
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 4;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 0);
			assert(moves == NULL);
			if (canFree) free(moves);
		}		
		
		// Lord Godalming's last location when there is no location
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 4;
			PlaceId *moves = GvGetLastLocations(gv, PLAYER_LORD_GODALMING, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 0);
			assert(moves == NULL);	
			if (canFree) free(moves);
		}
		// Dracula's Last move when there is no move
		{
			int numMoves = 0; bool canFree = false;
			int recentNum = 3;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA, recentNum,
			                                  &numMoves, &canFree);
			assert(numMoves == 0);
			assert(moves == NULL);
			if (canFree) free(moves);
		}

		// Dracula's Last location when there is no location		
		{
			int numLocs = 0; bool canFree = false;
			int recentNum = 3;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DRACULA, recentNum, 
			                                     &numLocs, &canFree);
			assert(numLocs == 0);
			assert(locs == NULL);
			if (canFree) free(locs);
		}

		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Last move/location Edge Cases\n");
		
		char *trail =
			"GLS.... SGE.... HGE.... MGE.... DST.V.. "
			"GCA.... SGE.... HGE.... MGE.... DC?T... "
			"GGR.... SGE.... HGE.... MGE.... DC?T... "
			"GAL.... SGE.... HGE.... MGE.... DD3T... "
			"GSR.... SGE.... HGE.... MGE.... DHIT... "
			"GSN.... SGE.... HGE.... MGE.... DC?T... "
			"GMA.... SSTTTV.";
		
		Message messages[32] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_DR_SEWARD) ==
				GAME_START_HUNTER_LIFE_POINTS - 2 * LIFE_LOSS_TRAP_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		
		// Lord Godalming's last move when numMoves is greater than player has already moved
		{
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING, 8,
			                                  &numMoves, &canFree);
			assert(numMoves == 7);
			assert(moves[0] == LISBON);
			assert(moves[1] == CADIZ);
			assert(moves[2] == GRANADA);
			assert(moves[3] == ALICANTE);
			assert(moves[4] == SARAGOSSA);
			assert(moves[5] == SANTANDER);
			assert(moves[6] == MADRID);
			if (canFree) free(moves);
		}
		
		// Lord Godalming's last loc when numLocss is greater than player has already moved
		{
			int numLocs = 0; bool canFree = false;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_LORD_GODALMING, 8,
			                                  &numLocs, &canFree);
			assert(numLocs == 7);
			assert(locs[0] == LISBON);
			assert(locs[1] == CADIZ);
			assert(locs[2] == GRANADA);
			assert(locs[3] == ALICANTE);
			assert(locs[4] == SARAGOSSA);
			assert(locs[5] == SANTANDER);
			assert(locs[6] == MADRID);
			if (canFree) free(locs);
		}
		
		// Dracula's last move when numMoves is greater than player has already moved
		{
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA, 7,
			                                  &numMoves, &canFree);
			assert(numMoves == 6);
			assert(moves[0] == STRASBOURG);
			assert(moves[1] == CITY_UNKNOWN);
			assert(moves[2] == CITY_UNKNOWN);
			assert(moves[3] == DOUBLE_BACK_3);
			assert(moves[4] == HIDE);
			assert(moves[5] == CITY_UNKNOWN);
			if (canFree) free(moves);
		}
		
		// Dracula's last loc when numLocss is greater than player has already moved
		{
			int numLocs = 0; bool canFree = false;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DRACULA, 7,
			                                     &numLocs, &canFree);
			assert(numLocs == 6);
			assert(locs[0] == STRASBOURG);
			assert(locs[1] == CITY_UNKNOWN);
			assert(locs[2] == CITY_UNKNOWN);
			assert(locs[3] == STRASBOURG);
			assert(locs[4] == STRASBOURG);
			assert(locs[5] == CITY_UNKNOWN);
			if (canFree) free(locs);
		}
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Dracula Teleport\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DST.V.. "
			"GGE.... SGE.... HGE.... MGE.... DTPT...";
		
		Message messages[10] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS + (1 * LIFE_GAIN_CASTLE_DRACULA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	{///////////////////////////////////////////////////////////////////

		printf("Testing doubleback\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DMR.V.. "
			"GMI.... SZU.... HPA.... MCF.... DC?T... "
			"GGO.... SMU.... HMR.... MTO.... DC?T... "
			"GTS.... SZA.... HMS.... MBO.... DTOT... "
			"GRO.... SSZ.... HCG.... MMS.... DD3T... "
			"GNP.... SBE.... HMS.... MNA.... DC?T... "
			"GNP.... SBE.... HMS.... MNA.... DTPTV..";
		
		Message messages[30] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetScore(gv) == GAME_START_SCORE
		                         - 7 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS + LIFE_GAIN_CASTLE_DRACULA);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		printf("Test passed!\n");
	}	
	{///////////////////////////////////////////////////////////////////

		printf("Testing doubleback\n");
		
		char *trail =
			"GAS.... SVI.... HAL.... MGA.... DLS.V.. "
			"GAS.... SPR.... HBA.... MGA.... DC?T... "
			"GAS.... SPR.... HAL.... MGA.... DC?T... "
			"GAS.... SPR.... HAL.... MBC.... DCAT... " 
			"GVE.... SPR.... HSR.... MBC.... DD1T... "
			"GVE.... SZA.... HMA.... MBE.... DGRT... "
			"GMU.... SZA.... HGR.... MBC.... DCA.V.. "
			"GMU.... SBR.... HGR.... MKL.... DMAT... "
			"GMU.... SBR.... HCA.... MSZ.... DD5T..."
			
			
			;
		
		Message messages[45] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetScore(gv) == GAME_START_SCORE
		                         - 9 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CADIZ);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		printf("Test passed!\n");
	}
	
	return EXIT_SUCCESS;
}
