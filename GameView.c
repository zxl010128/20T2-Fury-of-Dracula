////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

// add your own #includes here
#define MAXIMUM_TRAP 3
#define NOT_FIND    -100
#define MAXIMUM_CITY 128	
#define MOVE_SIZE    7	

// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct amateurVamp {
	int born; 
	PlaceId bornCity;				//Initialize the born city by no vamp
}*AmateurVamp;

//Information of the players
typedef struct playerInfo {
	Player ID;
	int blood;
	PlaceId current;				//Current city
	PlaceId move;					//The last move
	PlaceId locHistory[TRAIL_SIZE];			//Initialize the location history with NOWHERE
							//Must be a city
	PlaceId history[TRAIL_SIZE];			//Initialize the move history with NOWHERE
							//Can be a city or other places
}*PlayerInfo;

//Encounter type
typedef enum encounterType
{
	EncounterNull,					//nothing
	EncounterTrap,					//trap
	EncounterVampire,				//vamp
}*EncounterType;

//Encounter struct
typedef struct encounterInfo
{
	enum encounterType type;			//Encounter type
	int value;					//Durability
	int city;					//Incident City
}*EncounterInfo;

typedef struct gameView {
	int score;
	struct encounterInfo encounters[MAXIMUM_CITY][MAXIMUM_TRAP];	//Encounters up to 3 per city
	struct amateurVamp vamp;			//There will be only one amateur vamp(Sorry for the chinglish)

	Round round;					//round number of the game
	Map gameMap;
	struct playerInfo players[NUM_PLAYERS];		//Player list
	Player currentPlayer;				//Current player
}*GameView;


//---------------------------------------------------------------------------
// Move part
//---------------------------------------------------------------------------
// Count the number of locations of the given player
int GvGetTotalLocations(GameView gv, Player player);

// Count the number of moves of the given player
int GvGetTotalMoves(GameView gv, Player player);
//---------------------------------------------------------------------------
//
// Global function
//
//---------------------------------------------------------------------------



//Encountered event initialization
void EcInit(EncounterInfo ev)
{
	ev->type = EncounterNull;
	ev->value = -1;
	ev->city = NOWHERE;
}

//Analyze player types
Player GvParsePlayer(char ch);

//Parse city into 2 characters
void GvParseCity(char *buf, char *msg);

//---------------------------------------------------------------------------
//
// Player helper function
//
//---------------------------------------------------------------------------

//Get player name
const char* GvPlayerName(Player id);

//Initialize the player
void GvInitPlayer(GameView gv, Player ID);

//Add a move order history
void GvAddHistory(PlayerInfo player, PlaceId city);

//Count the size of the movement order history
int GvTotalHistorys(GameView gv, Player player);

//Add a location history
void GvAddLocationHistory(GameView gv, PlayerInfo player, PlaceId city);

//Count the size of location history
int GvTotalLocationHistorys(GameView gv, Player player);

//Player recuperation event
void GvPlayerRest(GameView gv, PlayerInfo player);

//---------------------------------------------------------------------------
//
// vamp helper function
//
//---------------------------------------------------------------------------

//Add vampire, now occupying an event slot in the city
bool GvAddVampire(GameView gv, PlaceId city);

//Remove Vampire
bool GvRemoveVampire(GameView gv);

//Vampire growth event, traverse all vampires, increase growth value
void GvVampireProc(GameView gv);

//---------------------------------------------------------------------------
//
// Trap helper functions, the maximum setting is 3
//
//---------------------------------------------------------------------------

//Place the trap
bool GvPushTrap(GameView gv, PlaceId city);

//Remove the trap
bool GvRemoveTrap(GameView gv, PlaceId city);

//Trap round processing, this is based on durability and can be deleted
void GvTrapProc(GameView gv);

//---------------------------------------------------------------------------
//
// GameView hlper function
//
//---------------------------------------------------------------------------

//Three functions for judging game winning or losing
void GvOnHitHunter(GameView gv, Player id);
void GvOnHitDracula(GameView gv);
void GvChangeScore(GameView gv, int n);

//Set current player
void GvSetPlayer(GameView gv, Player id)
{
	gv->currentPlayer = id;
}

//Teleport the player without making any judgmental moves.
void GvMovePlayer(GameView gv, Player id, PlaceId city)
{
	PlayerInfo player = &gv->players[id];

	player->current = city;
}

//The game score changes. 
//If n is a negative value, determine 
//whether the game score is less than 0 and the game is over
void GvChangeScore(GameView gv, int n)
{
	gv->score += n;
	if(n < 0 && gv->score <= 0){
		printf("    The hunter lost all the game points and the game failed.\n");
	}
}

//---------------------------------------------------------------------------
//
// Hunter part
//
//---------------------------------------------------------------------------

//Hunter Move event
void GvHunterMove(GameView gv, PlaceId city)
{
	assert(gv->currentPlayer != PLAYER_DRACULA);
	PlayerInfo player = &gv->players[gv->currentPlayer];
	PlaceId last;
	switch(city){
	case CITY_UNKNOWN:
	case SEA_UNKNOWN:
	case HIDE:
	case DOUBLE_BACK_1:
	case DOUBLE_BACK_2:
	case DOUBLE_BACK_3:
	case DOUBLE_BACK_4:
	case DOUBLE_BACK_5:
	case TELEPORT:
	case UNKNOWN_PLACE:
	case NOWHERE:

		break;
	default:					//Ordinary move
		GvAddHistory(player, player->move);	//Record the previous move method first
		last = player->current;			//Record the last moved city
		player->current = city;	
		player->move = city;
		GvAddLocationHistory(gv, player, city);	//Record the location of this move


		GvPlayerRest(gv, player);
		if(last == city){			//Same as last time, rest and restore life
			GvPlayerRest(gv, player);
		}
		break;
	}
}

//This event is triggered when the player is injured 
// to determine whether the game is won or lost.
void GvOnHitHunter(GameView gv, Player id)
{
	PlayerInfo player = &gv->players[id];
	//The hunter is not dead
	if(gv->players[id].blood > 0){
		return ;
	}

	//The hunter is dead
	player->blood = 0;
	GvChangeScore(gv, -SCORE_LOSS_HUNTER_HOSPITAL);

	GvMovePlayer(gv, id, HOSPITAL_PLACE);



	//Count the dead players
	//if all of them die, judge Dracula to win
	int n = 0;
	for(int i=0; i<NUM_PLAYERS-1; ++i){
		if(gv->players[i].blood <= 0){
			++n;
		}
	}
	if(n == 4){
		printf("    All hunters were defeated, %s won.\n", GvPlayerName(PLAYER_DRACULA));
	}
}

//---------------------------------------------------------------------------
//
// Dracula Part
//
//---------------------------------------------------------------------------

//Move dracula, handle moving events
void GvDraculaMove(GameView gv, PlaceId city)
{
	assert(gv->currentPlayer == PLAYER_DRACULA);
	PlayerInfo player = &gv->players[gv->currentPlayer];
	int n;
	switch(city){
	//case CITY_UNKNOWN:
	//case SEA_UNKNOWN:
	case HIDE:	//hide
		GvAddHistory(player, player->move);
		GvAddLocationHistory(gv, player, player->current);
		player->move = city;
		break;
	case DOUBLE_BACK_1:	
	case DOUBLE_BACK_2:
	case DOUBLE_BACK_3:
	case DOUBLE_BACK_4:
	case DOUBLE_BACK_5:
		n = city - DOUBLE_BACK_1;
		if(n > 0){
			//Record historical information
			GvAddHistory(player, player->move);	
			player->current = player->history[n - 0];
			player->move = city;
			GvAddLocationHistory(gv, player, player->current);
		}
		else{
			GvAddHistory(player, player->move);
			player->move = city;
			GvAddLocationHistory(gv, player, player->current);
		}

		
		//Water detection
		GvPlayerRest(gv, player);
		break;
	case TELEPORT:
		GvAddHistory(player, city);
		//Teleport to the castle
		player->current = CASTLE_DRACULA;
		player->move = city;
		GvAddLocationHistory(gv, player, player->current);

		GvPlayerRest(gv, player);//Rest
		break;
	case UNKNOWN_PLACE:
	case NOWHERE:
		break;
	default://Ordinary move
		GvAddHistory(player, player->move);	//Record the previous move method first
		player->current = city;	
		player->move = city;
		GvAddLocationHistory(gv, player, city);	//Record the location of this move

		//Dracula moving event
		GvPlayerRest(gv, player);
		break;
	}
}

//Dracula under attack
void GvOnHitDracula(GameView gv)
{
	PlayerInfo player = &gv->players[PLAYER_DRACULA];
	if(player->ID == PLAYER_DRACULA){
		if(player->blood <= 0){
			printf("    %-10s was defeated, the hunters won!\n", GvPlayerName(PLAYER_DRACULA));
		}
	}
}

//---------------------------------------------------------------------------
//
// execution function of pastPlays
//
//---------------------------------------------------------------------------

void GvRunGame(GameView gv, char* pastPlays, Message messages[])
{
	size_t size = strlen(pastPlays);
	if(size == 0){				//invalid situation
		return ;
	}

	Player id;
	
	char turn[8];				//one turn
	int turnID = 0;

	char city[3];				//city information
	PlaceId cityID;

	int msgID = 0;				//Dialogue Index

	for(int i=0; i<size; i += 8){
		memcpy(turn, pastPlays + i, 7);	//7 bytes for one turn
		turn[7] = '\0';			//The eighth byte is set to 0

		if(turnID == 0){
		}

		id = GvParsePlayer(turn[0]);	//Player ID
		GvParseCity(city, turn);		
		GvSetPlayer(gv, id);		//Set current player
		cityID = placeAbbrevToId(city);	//Get city id
		if(id == PLAYER_DRACULA){	//Moves
			GvDraculaMove(gv, cityID);
		}
		else{
			GvHunterMove(gv, cityID);
		}

		//dialogue
		if(strlen(messages[msgID])){
			printf("    %-10s says：   \"%s\"\n", GvPlayerName(id), (char*)messages[msgID]);
		}
		else{
		}

		for(int j=3; j<8; ++j){
			switch(turn[j]){
			case '\0':      	//This round of processing is over
				++turnID;   	//switch to the next player
				if(turnID == NUM_PLAYERS){
					turnID = 0;
					//Game rounds +1
					++gv->round;

					//Dealing with vampire growth
					GvVampireProc(gv);

					//Deal with all traps and reduce durability
					GvTrapProc(gv);
				}

				//Dracula’s score decreases at the end of each round
				if(id == PLAYER_DRACULA){
					GvChangeScore(gv, -SCORE_LOSS_DRACULA_TURN);
				}

				++msgID;
				++id;
				id %= NUM_PLAYERS;
				GvSetPlayer(gv, id);//Rotate to the next player
				break;
			case '.':
				break;
			case 'V':	//vamp event
				if(id == PLAYER_DRACULA){//dracula event
					//Dracula cannot trigger events in the sea
					if(placeIdToType(cityID) == SEA){
						continue;
					}

					//Refresh a vampire every 13 rounds
					if((gv->round % 13) == 0){
						GvAddVampire(gv, cityID);

					}
				}
				else{	//Hunter incident, encounter a vampire
                     			//the vampire will be destroyed, the hunter will not be injured
					GvRemoveVampire(gv);

				}
				break;
			case 'T':	//Trap
				if(id == PLAYER_DRACULA){
					//Dracula places a trap
					//Dracula cannot trigger events in the sea
					if(placeIdToType(cityID) == SEA){
						continue;
					}
					//Multiple of 13 rounds, no traps can be placed
					if(gv->round % 13 != 0){
						int location = GvGetPlayerLocation(gv, PLAYER_DRACULA);
						GvPushTrap(gv, location);

					}
				}
				else{//Hunter encounters a trap
					GvRemoveTrap(gv, cityID);
					gv->players[id].blood -= LIFE_LOSS_TRAP_ENCOUNTER;
					GvOnHitHunter(gv, id);	//Hunter injured
				}
				break;
			case 'D'://Hunter and Dracula encounter
				//hunter loos blood
				gv->players[id].blood -= LIFE_LOSS_DRACULA_ENCOUNTER;
				GvPlayerName(id), LIFE_LOSS_DRACULA_ENCOUNTER, gv->players[id].blood);
				GvOnHitHunter(gv, id);		//Hunter injured

				//Dracula Hurts
				gv->players[PLAYER_DRACULA].blood -= LIFE_LOSS_HUNTER_ENCOUNTER;
				GvOnHitDracula(gv);		//Dracula injured
				break;
			default:
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GameView gv = malloc(sizeof(*gv));
	if (gv == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	memset(gv, 0, sizeof(*gv));
	gv->score = GAME_START_SCORE;

	//Initialize all players
	for(int i=0; i<NUM_PLAYERS; ++i){
		GvInitPlayer(gv, i);
	}

	//Set current player
	gv->currentPlayer = PLAYER_LORD_GODALMING;

	//Initialize the vampire
	gv->vamp.born = -1;
	gv->vamp.bornCity = NOWHERE;

	//Initialize all encounters
	EncounterInfo ev;
	for(int city = 0; city < MAXIMUM_CITY; ++city){
		ev = gv->encounters[city];
		for(int i=0; i<MAXIMUM_TRAP; ++i){
			EcInit(&ev[i]);
		}
	}

	//Create a map
	gv->gameMap = MapNew();

	//Run the game
	GvRunGame(gv, pastPlays, messages);

	return gv;
}

void GvFree(GameView gv)
{

	//Release map
	MapFree(gv->gameMap);

	//Release game objects
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{	
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	return gv->currentPlayer;
}

int GvGetScore(GameView gv)
{
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	return gv->players[player].blood;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	return gv->players[player].current;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	return gv->vamp.bornCity;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	*numTraps = 0;
	PlaceId *trap = malloc(sizeof(PlaceId) * MAXIMUM_CITY * MAXIMUM_TRAP);
	int num = 0;
	EncounterInfo ev;
	
	// check every city for traps
	for (int i = 0; i < MAXIMUM_CITY; i++) {
		ev = gv->encounters[i];
		for (int j = 0; j < MAXIMUM_TRAP; j++) {
			if (ev[j].type == EncounterTrap) {
				
				// if the city has a trap, add the city to the array
				trap[num] = i;
				num++; 
			}
		}
	}

	*numTraps = num;
	return trap;
}

////////////////////////////////////////////////////////////////////////
// Game History

//Gets the complete move history in chronological order.
PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// get the total move numbers
	int moveTimes = GvGetTotalMoves(gv, player);

	// situation when there is no move
	if (moveTimes == 0){
    	*numReturnedMoves = 0;
    	(*canFree) = false;
    	return NULL;
	}
	
	// allocate memory for the return array
	int move_count = 0;
	PlaceId *moveHistory = malloc(sizeof(PlaceId) * (moveTimes + 1));

	// get the moveHistory
	for (int i = 0; i < moveTimes; i++) {
		moveHistory[i] = gv->players[player].history[moveTimes - i - 1];
		move_count++;
	}
    
	// add the last move History
	moveHistory[moveTimes] = gv->players[player].move;
	move_count++;
	(*numReturnedMoves) = move_count;
	(*canFree) = true;

	return moveHistory;
}

// Gets the given player's last `numMoves` moves in chronological order
PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{

   	// get the total move numbers   
	int moveTimes = GvGetTotalMoves(gv, player);

	// situation when there is no move or require 0 move
	if (moveTimes == 0 || numMoves == 0){
		*numReturnedMoves = 0;
		(*canFree) = false;
		return NULL;
	}

	// allocate memory for the return array
	int move_count = 0;
	PlaceId *moveHistory = malloc(sizeof(PlaceId) * numMoves);

	// when numMoves > moveTimes, the result will totally be the same with GvGetMoveHistory
	if (numMoves > moveTimes) {
		moveHistory = GvGetMoveHistory(gv, player, numReturnedMoves, canFree);

	} else {
		// get the last n moveHistory
		for (int i = 0; i < numMoves - 1; i++) {
		moveHistory[i] = gv->players[player].history[numMoves - i - 2];
		move_count++;
		}
		
		// add the last move History
		moveHistory[move_count] = gv->players[player].move;
		move_count++;
		(*numReturnedMoves) = move_count;
		(*canFree) = true;
	}

	return moveHistory;

}

// Gets the given player's locations history in chronological order
PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
    // get total location number
	int locCount = GvGetTotalLocations(gv, player);

	// situation when there is no location
	if (locCount == 0){
		*numReturnedLocs = 0;
		(*canFree) = false;
		return NULL;
	}

	// allocate memory for the return array
	int loccount = 0;
	PlaceId *locationHistory = malloc(sizeof(PlaceId) * (locCount + 1));

	if (player == PLAYER_DRACULA) {

		// get the locsHistory for Dracula
		for (int i = 0; i < locCount; i++) {
			locationHistory[i] = gv->players[player].locHistory[locCount - i - 1];
			loccount++;
		}
		// add the last location History
		locationHistory[locCount] = gv->players[player].current;
		(*numReturnedLocs) = loccount;
		(*canFree) = true;

	} else {
		// If the given player is a hunter, this function should behave exactly the same as GvGetMoveHistory
		locationHistory = GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	}

	return locationHistory;

}

// Gets the given player's last `numLocs` locations in chronological order
PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
    // get total location number
	int locCount = GvGetTotalLocations(gv, player);

	// situation when there is no location or require 0 location
	if (locCount == 0 || numLocs == 0){
		*numReturnedLocs = 0;
		(*canFree) = false;
		return NULL;
	}
	
	// allocate memory for the return array
	PlaceId *locationHistory = malloc(sizeof(PlaceId) * numLocs);
	int loccount = 0;

	if (numLocs > locCount) {
		// when numLocs > locCount, the result will totally be the same with GvGetLocationHistory
		locationHistory = GvGetLocationHistory(gv, player, numReturnedLocs, canFree);

	} else {
		// get the locsHistory
		for (int i = 0; i < numLocs - 1; i++) {
			locationHistory[i] = gv->players[player].locHistory[numLocs - i - 1];
			loccount++;
		}
		// add the last location History
		locationHistory[loccount] = gv->players[player].current;
		loccount++;
		(*numReturnedLocs) = loccount;
		(*canFree) = true;
	}

	return locationHistory;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{	
	// using MapGetConnections fun to get adjacent places
	ConnList adjacent = MapGetConnections(gv->gameMap, from);
	PlaceId reachableLocs[NUM_REAL_PLACES];
	// count is the sum number of reachable places by any transport type
	int count = 0;
	// add the from place in array
	reachableLocs[count] = from;
	count++;
	// transportType is road and boat
	ConnList cur = adjacent;
	while (cur != NULL) {
		// expect the two hospitals
		if (player == PLAYER_DRACULA && cur->p == ST_JOSEPH_AND_ST_MARY) {
			cur = cur->next;
			continue;
		} 
		if (cur->type == ROAD || cur->type == BOAT) {
			reachableLocs[count] = cur->p;
			count++;
		}
		cur = cur->next;
	}
	
	// transportType is rail
	if (player != PLAYER_DRACULA) {
		// only consider the player is hunters
		// drac can not have rail move
		// an array to store target palces		
		PlaceId rail_places[NUM_REAL_PLACES];
		// using current round to calculate how far the player can move by rail
		int rail_times = (round + player) % 4;
		// collect the from's adjacent places(can move by rail) to rail_places array
		ConnList rail_cur = adjacent;
		int rail_num = 0;
		while (rail_cur != NULL) {
			if (rail_cur->type == RAIL) {
				rail_places[rail_num] = rail_cur->p;
				rail_num++;		
			}
			rail_cur = rail_cur->next;	
		}
		// rail_times can be 0, 1, 2, 3
		// but when it equal 0
		// the hunter may not move by rail this turn

		// the maximum allowed distance via rail is 1
		if (rail_times == 1) {
			// add the from's adjacent places(can move by rail)
			for (int i = 0; i < rail_num; i++) {
				reachableLocs[count] = rail_places[i];
				count++;										
			}
		} 
		// the maximum allowed distance via rail is 2
		if (rail_times == 2) {
			// loop to find the target places (the second lap)
			int loop_time = rail_num - 1;
			while (loop_time >= 0) {
				ConnList current = MapGetConnections(gv->gameMap, rail_places[loop_time]);
				while (current != NULL) {
					if (current->type == RAIL) {
						rail_places[rail_num] = current->p;
						rail_num++;
					}		
					current = current->next;
				}
				loop_time--;
			}
			// add the places(can move by rail) which distance via rail is 2
			// only add the same place once
			int a = 0;
			while (a < rail_num) {
				int b = 0;
				bool iscontain = false;
				while (b < count) {
					if (rail_places[a] == reachableLocs[b]) {
						iscontain = true;
					}							
					b++;	
				}
				if (!iscontain) {
					reachableLocs[count] = rail_places[a];
					count++;
				}															
				a++;					
			}
		}
		// the maximum allowed distance via rail is 3		
		if (rail_times == 3) {
			// // loop to find the target places (the second lap)
			int loop_time = rail_num - 1;
			int orig = rail_num;
			while (loop_time >= 0) {
				ConnList current = MapGetConnections(gv->gameMap, rail_places[loop_time]);
				while (current != NULL) {
					if (current->type == RAIL) {
						rail_places[rail_num] = current->p;
						rail_num++;
					}		
					current = current->next;
				}
				loop_time--;
			}
			// loop to find the target places (the third lap)
			loop_time = rail_num - 1;
			while (loop_time >= orig) {
				ConnList current = MapGetConnections(gv->gameMap, rail_places[loop_time]);
				while (current != NULL) {
					if (current->type == RAIL) {
						rail_places[rail_num] = current->p;
						rail_num++;
					}		
					current = current->next;
				}
				loop_time--;
			}
			// add the places(can move by rail) which distance via rail is 3
			// only add the same place once
			int a = 0;
			while (a < rail_num) {
				int b = 0;
				bool iscontain = false;
				while (b < count) {
					if (rail_places[a] == reachableLocs[b]) {
						iscontain = true;
					}							
					b++;	
				}			
				if (!iscontain) {
					reachableLocs[count] = rail_places[a];
					count++;
				}															
				a++;					
			}
		} 
	}
	// assign the numReturnedLocs value
	*numReturnedLocs = count;
	// make all target places locations in a dynamically allocated array
	PlaceId *ReturnedLocs = malloc(sizeof(PlaceId) * count);
	for (int j = 0; j < count; j++) {
		ReturnedLocs[j] = reachableLocs[j];
	}
	// return this dynamically allocated array
	return ReturnedLocs;
}


PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// using MapGetConnections fun to get adjacent places
	ConnList adjacent = MapGetConnections(gv->gameMap, from);
	PlaceId reachableLocs[NUM_REAL_PLACES];
	// count is the sum number of reachable places by any transport type
	int count = 0;
	// add from place in array
	reachableLocs[count] = from;
	count++;
	// transportType is road
	if (road) {
		ConnList road_cur = adjacent;
		while (road_cur != NULL) {
			// expect the two hospitals
			if (player == PLAYER_DRACULA && road_cur->p == ST_JOSEPH_AND_ST_MARY) {
				road_cur = road_cur->next;
				continue;
			} 
			if (road_cur->type == ROAD) {
				reachableLocs[count] = road_cur->p;
				count++;
			}
			road_cur = road_cur->next;
		}
	}
	// transportType is boat
	if (boat) {
		ConnList boat_cur = adjacent;
		while (boat_cur != NULL) {
			if (boat_cur->type == BOAT) {
				reachableLocs[count] = boat_cur->p;
				count++;
			}
			boat_cur = boat_cur->next;
		}
	}	
	// transportType is rail
	if (rail && player != PLAYER_DRACULA) {
		// only consider the player is hunters
		// drac can not have rail move
		// an array to store target palces		
		PlaceId rail_places[NUM_REAL_PLACES];
		// using current round to calculate how far the player can move by rail
		int rail_times = (round + player) % 4;
		// collect the from's adjacent places(can move by rail) to rail_places array
		ConnList rail_cur = adjacent;
		int rail_num = 0;
		while (rail_cur != NULL) {
			if (rail_cur->type == RAIL) {
				rail_places[rail_num] = rail_cur->p;
				rail_num++;		
			}
			rail_cur = rail_cur->next;	
		}
		// rail_times can be 0, 1, 2, 3
		// but when it equal 0
		// the hunter may not move by rail this turn

		// the maximum allowed distance via rail is 1
		if (rail_times == 1) {
			// add the from's adjacent places(can move by rail)
			for (int i = 0; i < rail_num; i++) {
				reachableLocs[count] = rail_places[i];
				count++;										
			}
		} 
		// the maximum allowed distance via rail is 2
		if (rail_times == 2) {
			// loop to find the target places (the second lap)
			int loop_time = rail_num - 1 ;
			while (loop_time >= 0) {
				ConnList current = MapGetConnections(gv->gameMap, rail_places[loop_time]);
				while (current != NULL) {
					if (current->type == RAIL) {
						rail_places[rail_num] = current->p;
						rail_num++;
					}		
					current = current->next;
				}
				loop_time--;
			}
			// add the places(can move by rail) which distance via rail is 2
			// only add the same place once
			int a = 0;
			while (a < rail_num) {
				int b = 0;
				bool iscontain = false;
				while (b < count) {
					if (rail_places[a] == reachableLocs[b]) {
						iscontain = true;
					}							
					b++;	
				}
				if (!iscontain) {
					reachableLocs[count] = rail_places[a];
					count++;
				}															
				a++;					
			}
		}
		// the maximum allowed distance via rail is 3		
		if (rail_times == 3) {
			// // loop to find the target places (the second lap)
			int loop_time = rail_num - 1;
			int orig = rail_num;
			while (loop_time >= 0) {
				ConnList current = MapGetConnections(gv->gameMap, rail_places[loop_time]);
				while (current != NULL) {
					if (current->type == RAIL) {
						rail_places[rail_num] = current->p;
						rail_num++;
					}		
					current = current->next;
				}
				loop_time--;
			}
			// loop to find the target places (the third lap)
			loop_time = rail_num - 1;
			while (loop_time >= orig) {
				ConnList current = MapGetConnections(gv->gameMap, rail_places[loop_time]);
				while (current != NULL) {
					if (current->type == RAIL) {
						rail_places[rail_num] = current->p;
						rail_num++;
					}		
					current = current->next;
				}
				loop_time--;
			}
			// add the places(can move by rail) which distance via rail is 3
			// only add the same place once
			int a = 0;
			while (a < rail_num) {
				int b = 0;
				bool iscontain = false;
				while (b < count) {
					if (rail_places[a] == reachableLocs[b]) {
						iscontain = true;
					}							
					b++;	
				}			
				if (!iscontain) {
					reachableLocs[count] = rail_places[a];
					count++;
				}															
				a++;					
			}
		} 
	}
	// assign the numReturnedLocs value
	*numReturnedLocs = count;
	// make all target places locations in a dynamically allocated array
	PlaceId *ReturnedLocs = malloc(sizeof(PlaceId) * count);
	for (int j = 0; j < count; j++) {
		ReturnedLocs[j] = reachableLocs[j];
	}
	// return this dynamically allocated array
	return ReturnedLocs;	

}
////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//
// Own interface functions
//
//---------------------------------------------------------------------------

//Analyze player types
Player GvParsePlayer(char ch)
{
	switch(ch){
	case 'G':	//Lord Godalming
		return PLAYER_LORD_GODALMING;
	case 'S':	//Dr. Seward
		return PLAYER_DR_SEWARD;
	case 'H':	//Van Helsing
		return PLAYER_VAN_HELSING;
	case 'M':	//Mina Harker
		return PLAYER_MINA_HARKER;
	case 'D':	//Dracula
		return PLAYER_DRACULA;
	default:
		return (Player)-1;
	}
}

//Parse city into 2 characters
void GvParseCity(char *buf, char *msg)
{
	buf[0] = msg[1];
	buf[1] = msg[2];
	buf[2] = '\0';
}

//---------------------------------------------------------------------------
//
// player helper function
//
//---------------------------------------------------------------------------

//Get player name
const char* GvPlayerName(Player id)
{
	switch(id){
	case PLAYER_LORD_GODALMING:
		return "Lord Godalming";//"Lord Godalming";
	case PLAYER_DR_SEWARD:
		return "Dr. Seward";//"Dr. Seward";
	case PLAYER_VAN_HELSING:
		return "Van Helsing";//"Van Helsing";
	case PLAYER_MINA_HARKER:
		return "Mina Harker";//"Mina Harker";
	case PLAYER_DRACULA:
		return "Dracula";//"Dracula";
	default:
		return "unknown player";
	}
}

//Initialize the player
void GvInitPlayer(GameView gv, Player ID)
{
	PlayerInfo player = &gv->players[ID];
	player->ID = ID;
	switch(ID){
	case PLAYER_LORD_GODALMING:
	case PLAYER_DR_SEWARD:
	case PLAYER_VAN_HELSING:
	case PLAYER_MINA_HARKER:
		player->blood = GAME_START_HUNTER_LIFE_POINTS;
		break;
	case PLAYER_DRACULA:
		player->blood = GAME_START_BLOOD_POINTS;
		break;
	}
	for(int i=0; i<TRAIL_SIZE; ++i){
		player->history[i] = NOWHERE;
		player->locHistory[i] = NOWHERE;
	}
	player->current = NOWHERE;
	player->move = NOWHERE;
}

//Add a move order history
void GvAddHistory(PlayerInfo player, PlaceId city)
{
	for(int i=TRAIL_SIZE-1; i > 0; --i){
		player->history[i] = player->history[i-1];
	}
	player->history[0] = city;
}

//Add a location history
void GvAddLocationHistory(GameView gv, PlayerInfo player, PlaceId city)
{
	for(int i=TRAIL_SIZE-1; i > 0; --i){
		player->locHistory[i] = player->locHistory[i-1];
	}
	player->locHistory[0] = player->current;
}

//Count the size of the movement order history
int GvTotalHistorys(GameView gv, Player player)
{
	int n = 0;
	for(int i=0; i<TRAIL_SIZE; ++i){
		if(gv->players[player].history[i] != NOWHERE){
			++n;
		}
	}
	return n;
}

//Count the size of location history
int GvTotalLocationHistorys(GameView gv, Player player)
{
	int n = 0;

	for(int i=0; i<TRAIL_SIZE; ++i){
		if(gv->players[player].locHistory[i] != NOWHERE){
			++n;
		}
	}
	return n;
}

//Player recuperation event
void GvPlayerRest(GameView gv, PlayerInfo player)
{
	//Dracula gets rest
	if(player->ID == PLAYER_DRACULA){	
		//In the water
		if(placeIdToType(player->current) == SEA){
			player->blood -= LIFE_LOSS_SEA;
			GvOnHitDracula(gv);	//Dracula hurts~
		}
		//Rest in castle
		else if(player->current == CASTLE_DRACULA){
			player->blood += LIFE_GAIN_CASTLE_DRACULA;
		}
	}
	//Hunter gets rest
	else{
		player->blood += LIFE_GAIN_REST;
		//Maximum HP cap
		if(player->blood > GAME_START_HUNTER_LIFE_POINTS)player->blood = GAME_START_HUNTER_LIFE_POINTS;
		
	}
}

//---------------------------------------------------------------------------
//
// vamp helper function
//
//---------------------------------------------------------------------------

//Add vampire, now occupying an event slot in the city
bool GvAddVampire(GameView gv, PlaceId city)
{
	EncounterInfo ev = gv->encounters[(int)city];
	for(int i=0; i<MAXIMUM_TRAP; ++i){
		if(ev[i].type == EncounterNull){//Empty event
			ev[i].type = EncounterVampire;
			gv->vamp.born = 0;
			gv->vamp.bornCity = city;
			return true;
		}
	}
	//Fail to place
	return false;
}

//Remove Vampire
bool GvRemoveVampire(GameView gv)
{
	if(gv->vamp.bornCity != NOWHERE){
		EncounterInfo ev = gv->encounters[(int)gv->vamp.bornCity];
		for(int i=0; i<MAXIMUM_TRAP; ++i){//Clean event
			if(ev[i].type == EncounterVampire){
				ev[i].type = EncounterNull;
			}
		}
		gv->vamp.born = 0;
		gv->vamp.bornCity = NOWHERE;
		return true;
	}
	return false;
}

//Vampire growth event, traverse all vampires, increase growth value
void GvVampireProc(GameView gv)
{
	if(gv->vamp.bornCity != NOWHERE){
		//Increase growth value
		gv->vamp.born++;
		//Mateur, fly away
		//6 rounds mature
		if(gv->vamp.born >= 6){
			//Remove Vampire
			GvRemoveVampire(gv);	
			GvChangeScore(gv, -SCORE_LOSS_VAMPIRE_MATURES);
		}
	}
}

//---------------------------------------------------------------------------
//
// Trap helper functions, the maximum setting is 3
//
//---------------------------------------------------------------------------

////Place the trap
bool GvPushTrap(GameView gv, PlaceId city)
{
	EncounterInfo ev = gv->encounters[(int)city];
	for(int i=0; i<MAXIMUM_TRAP; ++i){
		//Empty event
		if(ev[i].type == EncounterNull){
			ev[i].type = EncounterTrap;
			//Scrap after 6 rounds
			ev[i].value = 6;	
			ev[i].city = city;
			return true;
		}
	}
	return false;//Placement failed
}

//Remove the trap
bool GvRemoveTrap(GameView gv, PlaceId city)
{
	EncounterInfo ev = gv->encounters[(int)city];
	for(int i=0; i<MAXIMUM_TRAP; ++i){
		//Find the first trap
		if(ev[i].type == EncounterTrap){
			EcInit(&ev[i]);
			return true;
		}
	}
	return false;
}

//Trap round processing, this is based on durability and can be deleted
void GvTrapProc(GameView gv)
{
	//Traverse all the traps in citys，durability - 1
	EncounterInfo ev;
	for(int city = 0; city < MAXIMUM_CITY; ++city){
		ev = gv->encounters[(int)city];
		for(int i=0; i<MAXIMUM_TRAP; ++i){
			if(ev[i].type == EncounterTrap){
				--ev[i].value;
				if(ev[i].value == 0){
					EcInit(&ev[i]);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
//
// Move part
//
//---------------------------------------------------------------------------

// Count the number of moves of the given player
int GvGetTotalMoves(GameView gv, Player player) {
    
    int num = 0;
	int round_num = GvGetRound(gv);
	int i = 0;
	while (i < round_num) {
        	if(gv->players[player].history[i] != NOWHERE){
            		num++;
        	}
		i++;
	}

    return num;
}

// Count the number of locations of the given player
int GvGetTotalLocations(GameView gv, Player player) {
    
    int num = 0;
	int i = 0;
	int round_num = GvGetRound(gv);
	while (i < round_num) {
		if(gv->players[player].locHistory[i] != NOWHERE){
		    num++;
		}
		i++;
	}

    return num;
}
