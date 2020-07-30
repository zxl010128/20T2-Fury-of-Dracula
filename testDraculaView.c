////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testDraculaView.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DraculaView.h"
#include "Game.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for basic functions, "
			   "just before Dracula's first move\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "..."
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == ZURICH);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 0);
		free(traps);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD.. SAO.... HCD.... MAO....";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahah",
			"Aha!", "", "", ""
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		assert(DvGetScore(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 5);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 30);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == GENEVA);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula leaving minions 1\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST.... DLOT... "
			"GST.... SST.... HST.... MST.... DHIT... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetVampireLocation(dv) == EDINBURGH);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == LONDON);
		assert(traps[1] == LONDON);
		assert(traps[2] == MANCHESTER);
		free(traps);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
		printf("Test for Dracula's valid moves 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);

		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == GALATZ);
		assert(moves[1] == KLAUSENBURG);
		assert(moves[2] == HIDE);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for Dracula's valid moves 2\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBS.... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);

		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == CONSTANTA);
		assert(moves[1] == IONIAN_SEA);
		assert(moves[2] == VARNA);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for Dracula's valid moves 3\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DMU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DZAT... "
			"GGE.... SGE.... HGE.... MGE.... DSZT... "
			"GGE.... SGE.... HGE.... MGE.... DBET... "
			"GGE.... SGE.... HGE.... MGE.... DSOT... "
			"GGE.... SGE.... HGE.... MGE.... DVRT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DGAT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[44] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);

		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BUCHAREST);
		assert(moves[1] == CASTLE_DRACULA);
		assert(moves[2] == KLAUSENBURG);
		assert(moves[3] == HIDE);
		assert(moves[4] == DOUBLE_BACK_1);
		assert(moves[5] == DOUBLE_BACK_2);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for Dracula's valid moves 4\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DMU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DZAT... "
			"GGE.... SGE.... HGE.... MGE.... DSZT... "
			"GGE.... SGE.... HGE.... MGE.... DBET... "
			"GGE.... SGE.... HGE.... MGE.... DSOT... "
			"GGE.... SGE.... HGE.... MGE.... DVRT... "		
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[44] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);

		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BLACK_SEA);
		assert(moves[1] == BUCHAREST);
		assert(moves[2] == GALATZ);
		assert(moves[3] == HIDE);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for Dracula's valid moves 5\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DVR.V.. "		
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[14] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);

		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BLACK_SEA);
		assert(moves[1] == BUCHAREST);
		assert(moves[2] == GALATZ);
		assert(moves[3] == HIDE);
		assert(moves[4] == DOUBLE_BACK_1);
		assert(moves[5] == DOUBLE_BACK_2);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for Dracula's valid moves 6\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBE.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSOT... "
			"GGE.... SGE.... HGE.... MGE.... DVRT... "		
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);

		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BLACK_SEA);
		assert(moves[1] == BUCHAREST);
		assert(moves[2] == GALATZ);
		assert(moves[3] == HIDE);
		assert(moves[4] == DOUBLE_BACK_1);
		assert(moves[5] == DOUBLE_BACK_2);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for Dracula's valid moves 7\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGE..V. "
			"GGE.... SGE.... HGE.... MGE.... DZUT... "		
			"GGE.... SGE.... HGE.... MGE.... DMUT... "
			"GGE.... SGE.... HGE.... MGE.... DNUT... "
			"GGE.... SGE.... HGE.... MGE.... DFRT... "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);

		assert(numMoves == 10);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BRUSSELS);
		assert(moves[1] == COLOGNE);
		assert(moves[2] == GENEVA);		
		assert(moves[3] == PARIS);
		assert(moves[4] == HIDE);
		assert(moves[5] == DOUBLE_BACK_1);
		assert(moves[6] == DOUBLE_BACK_2);
		assert(moves[7] == DOUBLE_BACK_3);
		assert(moves[8] == DOUBLE_BACK_4);
		assert(moves[9] == DOUBLE_BACK_5);
		free(moves);
		
		printf("Test passed!\n");
		DvFree(dv);
	}	

	{///////////////////////////////////////////////////////////////////	
		printf("Test for DvWhereCanIGo 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DKL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);

		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGo 2\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DMU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DZAT... "
			"GGE.... SGE.... HGE.... MGE.... DSZT... "
			"GGE.... SGE.... HGE.... MGE.... DBET... "
			"GGE.... SGE.... HGE.... MGE.... DSOT... "
			"GGE.... SGE.... HGE.... MGE.... DVRT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DGAT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[44] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);

		assert(numLocs == 5);		
		sortPlaces(locs, numLocs);
		assert(locs[0] == BUCHAREST);
		assert(locs[1] == CASTLE_DRACULA);
		assert(locs[2] == CONSTANTA);
		assert(locs[3] == GALATZ);
		assert(locs[4] == KLAUSENBURG);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGo 3\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGE..V. "
			"GGE.... SGE.... HGE.... MGE.... DZUT... "		
			"GGE.... SGE.... HGE.... MGE.... DMUT... "
			"GGE.... SGE.... HGE.... MGE.... DNUT... "
			"GGE.... SGE.... HGE.... MGE.... DFRT... "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE....";

		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);

		assert(numLocs == 9);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BRUSSELS);
		assert(locs[1] == COLOGNE);
		assert(locs[2] == FRANKFURT);
		assert(locs[3] == GENEVA);		
		assert(locs[4] == MUNICH);
		assert(locs[5] == NUREMBURG);
		assert(locs[6] == PARIS);
		assert(locs[7] == STRASBOURG);
		assert(locs[8] == ZURICH);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}



	{///////////////////////////////////////////////////////////////////	
		printf("Test for DvWhereCanIGoByType 1\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DKL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, true, &numLocs);		

		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGoByType 2\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DMU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DZAT... "
			"GGE.... SGE.... HGE.... MGE.... DSZT... "
			"GGE.... SGE.... HGE.... MGE.... DBET... "
			"GGE.... SGE.... HGE.... MGE.... DSOT... "
			"GGE.... SGE.... HGE.... MGE.... DVRT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DGAT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[44] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, false, &numLocs);

		assert(numLocs == 5);		
		sortPlaces(locs, numLocs);
		assert(locs[0] == BUCHAREST);
		assert(locs[1] == CASTLE_DRACULA);
		assert(locs[2] == CONSTANTA);
		assert(locs[3] == GALATZ);
		assert(locs[4] == KLAUSENBURG);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGoByType 3\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGE..V. "
			"GGE.... SGE.... HGE.... MGE.... DZUT... "		
			"GGE.... SGE.... HGE.... MGE.... DMUT... "
			"GGE.... SGE.... HGE.... MGE.... DNUT... "
			"GGE.... SGE.... HGE.... MGE.... DFRT... "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE....";

		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, false, &numLocs);

		assert(numLocs == 9);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BRUSSELS);
		assert(locs[1] == COLOGNE);
		assert(locs[2] == FRANKFURT);
		assert(locs[3] == GENEVA);		
		assert(locs[4] == MUNICH);
		assert(locs[5] == NUREMBURG);
		assert(locs[6] == PARIS);
		assert(locs[7] == STRASBOURG);
		assert(locs[8] == ZURICH);
		free(locs);
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{	
		printf("Test for DvWhereCanIGoByType 4\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSZ.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
		    	"GGE.... SGE.... HGE.... MGE....";
		    
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, true, &numLocs);                             
		
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);		
		assert(locs[0] == BLACK_SEA);		
		assert(locs[1] == BUCHAREST);
		assert(locs[2] == CONSTANTA);
		assert(locs[3] == GALATZ);
		assert(locs[4] == VARNA);						
		free(locs);		
		
		printf("Test passed!\n");
		DvFree(dv);
	}
	
	{	
		printf("Test for DvWhereCanIGoByType 5\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSZ.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
		    	"GGE.... SGE.... HGE.... MGE....";
		    
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, false, &numLocs);                             

		assert(numLocs == 4);
		sortPlaces(locs, numLocs);				
		assert(locs[0] == BUCHAREST);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == VARNA);						
		free(locs);		
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGoByType 6\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSZ.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
		    	"GGE.... SGE.... HGE.... MGE....";
		    
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, false, true, &numLocs);                             
		
		assert(numLocs == 3);
		sortPlaces(locs, numLocs);		
		assert(locs[0] == BLACK_SEA);		
		assert(locs[1] == BUCHAREST);
		assert(locs[2] == CONSTANTA);					
		free(locs);		
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGoByType 7\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSZ.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "			
		   	"GGE.... SGE.... HGE.... MGE....";
		    
		Message messages[29] = {};
		DraculaView dv = DvNew(trail, messages);		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, true, &numLocs);                             
		
		assert(numLocs == 3);
		sortPlaces(locs, numLocs);		
		assert(locs[0] == BLACK_SEA);		
		assert(locs[1] == GALATZ);
		assert(locs[2] == VARNA);						
		free(locs);		
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGoByType 8\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSZ.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "			
			"GGE.... SGE.... HGE.... MGE....";
		    
		Message messages[29] = {};
		DraculaView dv = DvNew(trail, messages);		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, false, &numLocs);                             
		
		assert(numLocs == 2);
		sortPlaces(locs, numLocs);				
		assert(locs[0] == GALATZ);
		assert(locs[1] == VARNA);						
		free(locs);		
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{	
		printf("Test for DvWhereCanIGoByType 9\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSZ.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "			
		    "GGE.... SGE.... HGE.... MGE....";
		    
		Message messages[29] = {};
		DraculaView dv = DvNew(trail, messages);		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, false, true, &numLocs);                             
		
		assert(numLocs == 1);
		sortPlaces(locs, numLocs);		
		assert(locs[0] == BLACK_SEA);							
		free(locs);		
		
		printf("Test passed!\n");
		DvFree(dv);
	}

	{
		printf("Test for DvWhereCanIGoByType 10\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSZ.V.. "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DCNT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "			
		    "GGE.... SGE.... HGE.... MGE....";
		    
		Message messages[29] = {};
		DraculaView dv = DvNew(trail, messages);		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, false, false, &numLocs);                             
		
		assert(numLocs == 0);		
		assert(locs == NULL);							
		free(locs);		
		
		printf("Test passed!\n");
		DvFree(dv);
	}
	
	{///////////////////////////////////////////////////////////////////
		printf("Test for DvWhereCanTheyGo 1\n");

		char *trail =
		    "GGE.... SGE.... HGE.... MGE.... DCD.V.. "
		    "GGE.... SGE.... HGE.... MGE....";

		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);

		int numMoves = -1;
		PlaceId *moves = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numMoves);
		assert(numMoves == 3);
		sortPlaces(moves, numMoves);
		assert(moves[0] == CASTLE_DRACULA);
		assert(moves[1] == GALATZ);
		assert(moves[2] == KLAUSENBURG);
		free(moves);

		printf("Test passed!\n");
		DvFree(dv);
    	}

    	{///////////////////////////////////////////////////////////////////   
		printf("Test for DvWhereCanTheyGoByType 1\n");

		char *trail =
		    "GGE.... SGE.... HGE.... MBA.... DCD.V.. "
		    "GGE.... SGE.... HGE.... MBA....";

		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		int numMoves = -1;
		PlaceId *moves = DvWhereCanTheyGoByType(dv, PLAYER_MINA_HARKER,
		    					false, false, true, &numMoves);
		assert(numMoves == 1);
		sortPlaces(moves, numMoves);
		assert(moves[0] == CASTLE_DRACULA);
		free(moves);

		printf("Test passed!");
		DvFree(dv);
    	}

    	{  
		printf("Test for DvWhereCanTheyGoByType 2\n");

		char *trail =
		    "GGE.... SGE.... HGE.... MBA.... DKL.V.. "
		    "GZU.... SPA.... HGE.... MBA.... DBDT... "
		    "GZU.... SLE.... HST.... MBA.... DZAT... "
		    "GMR.... SNA.... HST.... MBA.... DBDT... "
		    "GGO.... SCF.... HST.... MBA....";

		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		int numMoves = -1;
		PlaceId* moves = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING,
		    					false, true, true, &numMoves);
		assert(numMoves == 3);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BUDAPEST);
		assert(moves[1] == SZEGED);
		assert(moves[2] == VIENNA);
		free(moves);

		printf("Test passed!\n");
		DvFree(dv);
	}
		return EXIT_SUCCESS;
}
