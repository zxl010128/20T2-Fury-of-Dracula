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
#include <string.h> //memset

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here
#define MAXIMUM_TRAP 3
#define NOT_FIND	 -100
#define MAXIMUM_CITY 128	//最大城市数量

int GvGetTotalLocations(GameView gv, Player player);
int GvGetTotalMoves(GameView gv, Player player);
int addRailPlaces(int start, int count_add, int *array, GameView gv);

// TODO: ADD YOUR OWN STRUCTS HERE

typedef struct amateurVamp { 
	int born;						//成长值0幼年 1成年
	PlaceId bornCity;				//出生城市 初始化NOWHERE 代表没有吸血鬼
}*AmateurVamp;

//玩家信息
typedef struct playerInfo {
	Player ID;
	int blood;
	PlaceId current;				//当前城市
	PlaceId locHistory[TRAIL_SIZE];	//位置历史 初始化NOWHERE 位置信息必须是确定的城市
	PlaceId move;					//最后一次移动
	PlaceId history[TRAIL_SIZE];	//移动历史 初始化NOWHERE 移动可以是城市也可以是other places的代码
}*PlayerInfo;

//遭遇类型
typedef enum encounterType
{
	EncounterNull,		//无遭遇
	EncounterTrap,		//陷阱
	EncounterVampire,	//吸血鬼
}*EncounterType;

//遭遇结构体
typedef struct encounterInfo
{
	enum encounterType type;	//遭遇类型
	int value;					//数值，陷阱（耐久值）
	int city;					//事发城市
}*EncounterInfo;

typedef struct gameView {
	int score;
	struct encounterInfo encounters[MAXIMUM_CITY][MAXIMUM_TRAP];	//遭遇，每个城市最多3个
	struct amateurVamp vamp;		//吸血鬼，只有一个

	Round round;	//游戏回合数
	Map gameMap;
	struct playerInfo players[NUM_PLAYERS];	//玩家列表
	Player currentPlayer;	//当前玩家
}*GameView;

//
// 全局函数
//

//这个没写，你们可以完善，事件输出用的都是printf
void GvEvent(Player id, const char* event, PlaceId city)
{
	//print(PlayerName(id), event, CityName(city))....
}

//获得区域类型的名字
const char* GetPlaceTypeName(PlaceType type)
{
	switch(type){
	case SEA:
		return "海域";
	case LAND:
		return "陆地";
	default:
		return "未知地面类型";
	}
}

//遭遇事件结构体初始化
void EcInit(EncounterInfo ev)
{
	ev->type = EncounterNull;
	ev->value = -1;
	ev->city = NOWHERE;
}

//
// Player操作函数
//

//初始化玩家
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

//玩家名字
const char* PlayerName(Player id)
{
	switch(id){
	case PLAYER_LORD_GODALMING:
		return "高达明爵士";//"Lord Godalming";
	case PLAYER_DR_SEWARD:
		return "苏厄德医生";//"Dr. Seward";
	case PLAYER_VAN_HELSING:
		return "范海辛";//"Van Helsing";
	case PLAYER_MINA_HARKER:
		return "米娜·哈克";//"Mina Harker";
	case PLAYER_DRACULA:
		return "德古拉";//"Dracula";
	default:
		return "未知玩家";
	}
}

//解析玩家类型
Player NameToPlayer(char ch)
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

//解析城市，2个字符
void GetCity(char *buf, char *msg)
{
	buf[0] = msg[1];
	buf[1] = msg[2];
	buf[2] = '\0';
}

//
// GameView操作函数
//

//三个判断游戏输赢的函数
void GvOnHitHunter(GameView gv, Player id);
void GvOnHitDracula(GameView gv);
void GvChangeScore(GameView gv, int n);

//设置当前player
void GvSetPlayer(GameView gv, Player id)
{
	gv->currentPlayer = id;
}

//添加一个移动指令历史记录，6个缓存+当前=7个
void GvAddHistory(PlayerInfo player, PlaceId city)
{
	for(int i=TRAIL_SIZE-1; i > 0; --i){
		player->history[i] = player->history[i-1];
	}
	player->history[0] = city;
}

//添加一个位置历史记录，这个代码上没有表现最大多少个，6个够用
void GvAddLocationHistory(GameView gv, PlayerInfo player, PlaceId city)
{
	#if 0
	//德古拉移动6步，6步之外的陷阱全部移除
	if(player->ID == PLAYER_DRACULA){
		PlaceId trapCity = player->locHistory[TRAIL_SIZE - 1];//最后一步，要移除的城市
		bool canRemove = true;
		//如果有相同的城市，不能移除陷阱
		if(trapCity == city)canRemove = false;
		for(int i=0; i<TRAIL_SIZE-1; ++i){
			if(trapCity == player->locHistory[i]){
				canRemove = false;
			}
		}

		if(canRemove){//判断需要移除陷阱
			for(int i=0; i<MAXIMUM_TRAP; ++i){//查看城市是否有需要移除的陷阱，移除
				if(gv->encounters[trapCity][i].type == EncounterTrap){
					EcInit(&gv->encounters[trapCity][i]);//移除
				}
			}
		}
	}
	#endif

	for(int i=TRAIL_SIZE-1; i > 0; --i){
		player->locHistory[i] = player->locHistory[i-1];
	}
	player->locHistory[0] = player->current;
}

//统计移动指令历史记录大小
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

//统计位置历史记录大小
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

//玩家修养事件
void GvPlayerRest(GameView gv, PlayerInfo player)
{
	if(player->ID == PLAYER_DRACULA){//德古拉修养
		if(placeIdToType(player->current) == SEA){//test6泡在水里
			player->blood -= LIFE_LOSS_SEA;
			printf("    %-10s  受到海水的侵蚀，生命值减少了 %d。\n", PlayerName(player->ID), LIFE_LOSS_SEA);
			GvOnHitDracula(gv);//德古拉受伤事件
		}
		else if(player->current == CASTLE_DRACULA){//城堡中修养
			player->blood += LIFE_GAIN_CASTLE_DRACULA;
			printf("    %-10s  在舒适的城堡里得到了修养，生命值增加了 %d，当前生命 %d。\n", 
				PlayerName(player->ID), LIFE_GAIN_CASTLE_DRACULA, player->blood);
		}
	}
	else{//猎人修养
		player->blood += LIFE_GAIN_REST;
		//最大生命封顶
		if(player->blood > GAME_START_HUNTER_LIFE_POINTS)player->blood = GAME_START_HUNTER_LIFE_POINTS;
		printf("    %-10s  停留在 %s 得到了修养，生命值增加了 %d，当前生命 %d/%d。\n",
			PlayerName(player->ID), placeIdToName(player->current),
			LIFE_GAIN_REST, player->blood, GAME_START_HUNTER_LIFE_POINTS);
	}
}

//传送玩家，不做任何判断的移动。传送。
void GvMovePlayer(GameView gv, Player id, PlaceId city)
{
	PlayerInfo player = &gv->players[id];
	//GvAddHistory(player->current);
	player->current = city;
	//printf("    %-10s  来到了  %s\n", PlayerName(player->ID), placeIdToName(city));
}

//移动玩家，处理移动事件
void GvMove(GameView gv, PlaceId city)
{
	PlayerInfo player = &gv->players[gv->currentPlayer];
	PlaceId last;
	int n;
	switch(city){
	//case CITY_UNKNOWN:
	//case SEA_UNKNOWN:
	case HIDE:	//隐藏行踪，首次出现test9
		GvAddHistory(player, player->move);
		GvAddLocationHistory(gv, player, player->current);//这可能是在地图上显示的虚假信息
		//player->current = UNKNOWN_PLACE;
		player->move = city;
		
		printf("    %-10s  隐藏了自己的行踪。\n", PlayerName(player->ID));

		break;
	case DOUBLE_BACK_1:	//后退
	case DOUBLE_BACK_2:
	case DOUBLE_BACK_3:
	case DOUBLE_BACK_4:
	case DOUBLE_BACK_5://test6?
		n = city - DOUBLE_BACK_1;
		if(n > 0){//后退
			GvAddHistory(player, player->move);	//记录历史信息
			player->current = player->history[n - 0];
			player->move = city;
			GvAddLocationHistory(gv, player, player->current);
		}
		else{//n==0，后退一步，原地不动，这个testGameView也没用提现
			GvAddLocationHistory(gv, player, player->current);//所以这个历史记录也不确定是否需要
		}

		//事件描述
		printf("    %-10s  后退%d步，来到了  %s\n", PlayerName(player->ID), n+1, placeIdToName(player->current));
		//printf("%s\n", GetPlaceTypeName(placeIdToType(player->current)));
		if(player->ID == PLAYER_DRACULA){
			//水域检测
			GvPlayerRest(gv, player);
		}
	
		break;
	case TELEPORT://瞬移，testGaveView里面没用到，正确性不确定
		GvAddHistory(player, city);
		player->current = city;
		printf("    %-10s  施展时空法术，传送到了  %s\n", PlayerName(player->ID), placeIdToName(city));
		break;
	case UNKNOWN_PLACE:
	case NOWHERE:
		break;
	default://普通移动
		GvAddHistory(player, player->move);	//先记录上步移动方式
		last = player->current;	//记录上次移动城市
		player->current = city;	//设置属性
		player->move = city;
		GvAddLocationHistory(gv, player, city);	//记录本次移动的位置

		//if(player->ID == PLAYER_DRACULA){	//猎人移动事件
			printf("    %-10s  来到了  %s\n", PlayerName(player->ID), placeIdToName(city));
		//}
		if(player->ID == PLAYER_DRACULA){//test10 德古拉移动事件
			GvPlayerRest(gv, player);
		}
		else{
			if(last == city){//和上次的城市相同，修养恢复生命
				GvPlayerRest(gv, player);
			}
		}
		break;
	}
}

//
// 吸血鬼函数
//

//添加吸血鬼，现在占据城市的一个事件位
bool GvAddVampire(GameView gv, PlaceId city)
{
	EncounterInfo ev = gv->encounters[(int)city];
	for(int i=0; i<MAXIMUM_TRAP; ++i){
		if(ev[i].type == EncounterNull){//空事件位
			ev[i].type = EncounterVampire;
			gv->vamp.born = 0;
			gv->vamp.bornCity = city;
			return true;
		}
	}
	return false;//放置失败
}

//移除吸血鬼
bool GvRemoveVampire(GameView gv)
{
	if(gv->vamp.bornCity != NOWHERE){
		EncounterInfo ev = gv->encounters[(int)gv->vamp.bornCity];
		for(int i=0; i<MAXIMUM_TRAP; ++i){//清理事件位
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

//test13 吸血鬼成长事件，遍历所有吸血鬼，增加成长值
void GvVampireProc(GameView gv)
{
	if(gv->vamp.bornCity != NOWHERE){
		gv->vamp.born++;//增加成长值
		//test13 成年，飞走
		if(gv->vamp.born >= 6){//6回合成熟
			printf("    %-10s  在      %10s 成长成了成年吸血鬼，失去分数：%d\n", "吸血鬼",
				placeIdToName(gv->vamp.bornCity), SCORE_LOSS_VAMPIRE_MATURES);
			GvRemoveVampire(gv);	//移除吸血鬼
			GvChangeScore(gv, -SCORE_LOSS_VAMPIRE_MATURES);
		}
	}
}

//
// 陷阱类函数，最大设置的是3个
//

//放置陷阱
bool GvPushTrap(GameView gv, PlaceId city)
{
	EncounterInfo ev = gv->encounters[(int)city];
	for(int i=0; i<MAXIMUM_TRAP; ++i){
		if(ev[i].type == EncounterNull){//空事件位
			ev[i].type = EncounterTrap;
			ev[i].value = 6;	//6回合之后报废
			ev[i].city = city;
			return true;
		}
	}
	return false;//放置失败
}

//拆除陷阱test12
bool GvRemoveTrap(GameView gv, PlaceId city)
{
	EncounterInfo ev = gv->encounters[(int)city];
	for(int i=0; i<MAXIMUM_TRAP; ++i){
		if(ev[i].type == EncounterTrap){//找到第一个陷阱
			EcInit(&ev[i]);
			return true;
		}
	}
	return false;
}

//陷阱的回合处理，这个根据耐久度处理，可以删掉
void GvTrapProc(GameView gv)
{
	//遍历所有城市所有陷阱，耐久值-1
	EncounterInfo ev;
	for(int city = 0; city < MAXIMUM_CITY; ++city){
		ev = gv->encounters[(int)city];
		for(int i=0; i<MAXIMUM_TRAP; ++i){
			if(ev[i].type == EncounterTrap){
				--ev[i].value;
				if(ev[i].value == 0){
					printf("    %s 里的一个陷阱因为长时间没有维护而失去作用。\n", placeIdToName(ev[i].city));
					EcInit(&ev[i]);
				}
			}
		}
	}
}

//
//当玩家受伤时触发这个事件，判断游戏是否胜利或失败
//

//猎人受到攻击
void GvOnHitHunter(GameView gv, Player id)
{
	PlayerInfo player = &gv->players[id];
	//猎人没有挂
	if(gv->players[id].blood > 0){
		return ;
	}

	//猎人挂了
	player->blood = 0;
	GvChangeScore(gv, -SCORE_LOSS_HUNTER_HOSPITAL);
	printf("    经过惨烈的战斗，%-10s 身受重伤。\n", PlayerName(id));
	GvMovePlayer(gv, id, HOSPITAL_PLACE);
	printf("    %-10s 被送往 %s\n", PlayerName(id), placeIdToName(HOSPITAL_PLACE));
	printf("    游戏当前分数：%d\n", gv->score);

	//统计死亡的玩家，如果全部死亡，判断德古拉胜利
	int n = 0;
	for(int i=0; i<NUM_PLAYERS-1; ++i){
		if(gv->players[i].blood <= 0){
			++n;
		}
	}
	if(n == 4){
		printf("    所有猎人都被打败了，%s 获得了胜利。\n", PlayerName(PLAYER_DRACULA));
	}
}

//德古拉受到攻击
void GvOnHitDracula(GameView gv)
{
	PlayerInfo player = &gv->players[PLAYER_DRACULA];
	if(player->ID == PLAYER_DRACULA){//德古拉
		if(player->blood <= 0){
			printf("    %-10s 被打败了，猎人们获得了胜利！\n", PlayerName(PLAYER_DRACULA));
		}
	}
}

//游戏分数改变，如果n是负值，判断游戏分数是否小于0，游戏是否结束
void GvChangeScore(GameView gv, int n)
{
	gv->score += n;
	if(n < 0 && gv->score <= 0){
		printf("    猎人损失了所有的游戏分数，游戏失败。\n");
	}
}

//pastPlays的执行函数
void GvRunGame(GameView gv, char* pastPlays, Message messages[])
{
	size_t size = strlen(pastPlays);
	if(size == 0){//无效表
		return ;
	}

	Player id;
	
	char turn[8];	//一轮游戏
	int turnID = 0;

	char city[3];	//城市信息
	PlaceId cityID;

	int msgID = 0;	//对话索引

	for(int c=0; c<=size; c += 8){
		memcpy(turn, pastPlays + c, 7);	//一轮游戏7个字节
		turn[7] = '\0';					//第八个字节设置0
		id = NameToPlayer(turn[0]);	//玩家ID
		GetCity(city, turn);
		GvSetPlayer(gv, id);	//设置当前玩家
		cityID = placeAbbrevToId(city);	//获取城市id
		GvMove(gv, cityID);
		if(strlen(messages[msgID])){
			printf("    %-10s  说道：   \"%s\"\n", PlayerName(id), (char*)messages[msgID]);
		}
		else{
		}//test7 bug已修复

		for(int i=3; i<8; ++i){
			switch(turn[i]){
			case '\0'://本轮处理结束，切换下一个玩家
				++turnID;
				if(turnID == NUM_PLAYERS){//回合结束
					turnID = 0;
					++gv->round;//游戏回合数+1

					//处理吸血鬼成长事件
					GvVampireProc(gv);

					//处理所有城市陷阱，减少耐久度
					GvTrapProc(gv);
				}

				//德古拉每回合结束，记分减少
				if(id == PLAYER_DRACULA){
					GvChangeScore(gv, -SCORE_LOSS_DRACULA_TURN);
				}

				++msgID;
				++id;
				id %= NUM_PLAYERS;
				GvSetPlayer(gv, id);//轮换到下一个玩家
				//printf("当前玩家：%s\n", PlayerName(player));
				break;
			case '.'://这个点忽略了
				break;
			case 'V':	//吸血鬼事件
				if(id == PLAYER_DRACULA){//德古拉事件
					//德古拉无法在海里触发事件
					if(placeIdToType(cityID) == SEA){
						continue;
					}

					//这个是读取test编码事件，每13回合刷一个吸血鬼
					if((gv->round % 13) == 0){
						GvAddVampire(gv, cityID);
						printf("    %-10s  在      %10s 降生了。\n", "吸血鬼", placeIdToName(cityID));
					}
				}
				else{//猎人事件，遭遇吸血鬼，吸血鬼会被消灭掉，猎人不会受伤
					GvRemoveVampire(gv);
					printf("    %-10s 和吸血鬼在 %10s 进行了激烈的战斗，吸血鬼被消灭了。\n", 
						PlayerName(id), placeIdToName(cityID));
				}
				break;
			case 'T':	//陷阱
				if(id == PLAYER_DRACULA){//德古拉放置陷阱
					//德古拉无法在海里触发事件
					if(placeIdToType(cityID) == SEA){
						continue;
					}

					if(gv->round % 13 != 0){//13回合的倍数，不能放置陷阱
						GvPushTrap(gv, cityID);
						int cid = GvGetPlayerLocation(gv, PLAYER_DRACULA);
						printf("    %-10s  在      %10s 放置了陷阱。\n", PlayerName(id), placeIdToName(cid));
					}
				}
				else{//猎人遭遇陷阱
					GvRemoveTrap(gv, cityID);//test12
					gv->players[id].blood -= LIFE_LOSS_TRAP_ENCOUNTER;
					printf("    %-10s  在 %10s 遭遇了陷阱，损失了生命值：%d\n", PlayerName(id), placeIdToName(cityID), LIFE_LOSS_TRAP_ENCOUNTER);
				}
				break;
			case 'D'://猎人和德古拉遭遇
				//printf("%s---%s 当前位置。", PlayerName(PLAYER_DRACULA), placeIdToName(cityID));
				printf("    %-10s 和 %s 在 %s 遭遇了。\n", PlayerName(id), PlayerName(PLAYER_DRACULA), placeIdToName(cityID));
				//猎人掉血
				gv->players[id].blood -= LIFE_LOSS_DRACULA_ENCOUNTER;
				printf("      %-10s 损失生命：%d，剩余生命值：%d\n",
					PlayerName(id), LIFE_LOSS_DRACULA_ENCOUNTER, gv->players[id].blood);
				GvOnHitHunter(gv, id);//猎人受伤事件

				//德古拉掉血
				gv->players[PLAYER_DRACULA].blood -= LIFE_LOSS_HUNTER_ENCOUNTER;
				printf("      %-10s 损失生命：%d，剩余生命值：%d\n",
					PlayerName(PLAYER_DRACULA), LIFE_LOSS_HUNTER_ENCOUNTER, gv->players[PLAYER_DRACULA].blood);
				GvOnHitDracula(gv);//德古拉受伤事件
				break;
			default:
				printf("未知消息：%c\n", turn[i]);
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

	//初始化所有玩家
	for(int i=0; i<NUM_PLAYERS; ++i){
		GvInitPlayer(gv, i);
	}

	//设置当前player
	gv->currentPlayer = PLAYER_LORD_GODALMING;

	//初始化吸血鬼
	gv->vamp.born = -1;
	gv->vamp.bornCity = NOWHERE;

	//初始化所有遭遇
	EncounterInfo ev;
	for(int city = 0; city < MAXIMUM_CITY; ++city){
		ev = gv->encounters[city];
		for(int i=0; i<MAXIMUM_TRAP; ++i){
			EcInit(&ev[i]);
		}
	}

	//创建地图
	gv->gameMap = MapNew();

	//执行游戏
	GvRunGame(gv, pastPlays, messages);

	return gv;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	//释放地图
	MapFree(gv->gameMap);

	//释放游戏对象
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

	for (int i = 0; i < MAXIMUM_CITY; i++) {
		ev = gv->encounters[i];
		for (int j = 0, j < MAXIMUM_TRAP, j++) {
			if (ev[j].type == EncounterTrap) {
				p[num] = i;
				num++; 
			}
		}
	}

	*numTraps = num;
	return trap;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
    int moveTimes = GvGetTotalMoves(gv, player);
    
    if (moveTimes == 0){
        *numReturnedMoves = 0;
        (*canFree) = false;
        return NULL;
    }
    
    PlaceId *moveHistory = malloc(sizeof(PlaceId) * (moveTimes + 1));
    
    for (int i = 0; i < moveTimes; i++) {
        moveHistory[i] = gv->players[player].history[moveTimes - i - 1];
        (*numReturnedMoves)++;
    }
    
    moveHistory[moveTimes] = gv->players[player].move;
    (*numReturnedMoves)++;
    (*canFree) = true;

	return moveHistory;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
      
    int moveTimes = GvGetTotalMoves(gv, player);
    
    if (moveTimes == 0){
        *numReturnedMoves = 0;
        (*canFree) = false;
        return NULL;
    }
    
    PlaceId *moveHistory = malloc(sizeof(PlaceId) * numMoves);
    
    
    for (int i = 0; i < numMoves - 1; i++) {
        moveHistory[i] = gv->players[player].history[numMoves - i - 2];
        (*numReturnedMoves)++;
    }
    
    moveHistory[*numReturnedMoves] = gv->players[player].move;
    (*numReturnedMoves)++;
    (*canFree) = true;
    
    return moveHistory;

}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{

    int locCount = GvGetTotalLocations(gv, player);
    
    if (locCount == 0){
        *numReturnedLocs = 0;
        (*canFree) = false;
        return NULL;
    }
    
    PlaceId *locationHistory = malloc(sizeof(PlaceId) * (locCount + 1));
    
    for (int i = 0; i < locCount; i++) {
        locationHistory[i] = gv->players[player].locHistory[locCount - i - 1];
        (*numReturnedLocs)++;
    }
    
    locationHistory[locCount] = gv->players[player].current;
    (*canFree) = true;

    return locationHistory;

}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
    int locCount = GvGetTotalLocations(gv, player);
    
    if (locCount == 0){
        *numReturnedLocs = 0;
        (*canFree) = false;
        return NULL;
    }
    
    PlaceId *locationHistory = malloc(sizeof(PlaceId) * numLocs);


    for (int i = 0; i < numLocs - 1; i++) {
        locationHistory[i] = gv->players[player].locHistory[numLocs - i - 2];
        (*numReturnedLocs)++;
    }

    locationHistory[*numReturnedLocs] = gv->players[player].current;
    (*numReturnedLocs)++;
    (*canFree) = true;
	return locationHistory;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{	
	// get a list which contains the adjacent places of 'from' 
	ConnList adjacent = MapGetConnections(gv -> gameMap, from);
	// count is the sum number of reachable places by any transport type
	int count = 0;
	// curr_one: using to loop count number
	// curr_two: using to loop logging data
	ConnList cur_one = adjacent;
	ConnList cur_two = adjacent;
	// road and boat part
	int road_boat_num = 0;
	bool isContainHospital = false;
	// count the number of places reachable by road  and boat that drac and hunt can get
	while (cur_one != NULL && cur_one -> type != RAIL) {
		if (cur_one -> p == ST_JOSEPH_AND_ST_MARY) {
			isContainHospital = true;
		}
		road_boat_num++;
		cur_one = cur_one -> next;	
	}
	// logging the drac's reachable places
	if (player == PLAYER_DRACULA) {
		if (isContainHospital) {
			numReturnedLocs = malloc((road_boat_num - 1) * sizeof(int));
		} else {
			numReturnedLocs = malloc(road_boat_num * sizeof(int));
		}
		while (cur_two != NULL && cur_two -> type != RAIL) {
			// expect the two hospitals
			if (cur_two -> p == ST_JOSEPH_AND_ST_MARY) {
				break;
			}
			numReturnedLocs[count] = cur_two -> p;
			count++;
			cur_two = cur_two -> next;
		}
	} 
	// logging the hunt's reachable places 
	else {
		numReturnedLocs = malloc(road_boat_num * sizeof(int));
		while (cur_two != NULL && cur_two -> type != RAIL) {
			numReturnedLocs[count] = cur_two -> p;
			count++;
			cur_two = cur_two -> next;
		}
		// rail part (hunters only)
		// using current round to calculate how far the player can move by rail
		int rail_times = (round + player) % 4;
		// an array to collect the palces which can move by rail
		int rail_places[NUM_REAL_PLACES];
		// curr: using to loop count number
		ConnList rail_cur = adjacent;
		int rail_num = 0;
		// collect the from's adjacent places(can move by rail) to rail_places array
		while (rail_cur != NULL && rail_cur -> type == RAIL) {
			rail_places[rail_num] = rail_cur -> p;
			rail_num++;		
			rail_cur = rail_cur -> next;	
		}
		// the hunter may not move by rail this turn
		if (rail_times == 0) {
			return numReturnedLocs;
		}
		// the maximum allowed distance via rail is 1
		if (rail_times == 1) {
			// add the from's adjacent places(can move by rail)
			numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
			for (int i = 0; i <= rail_num; i++) {
				numReturnedLocs[count] = rail_places[i];
				count++;					
			}
		} 
		// the maximum allowed distance via rail is 2
		if (rail_times == 2) {
			// using fun to get new number of palces which distance via rail is 2
			// fun will modify the rail_places array
			rail_num = addRailPlaces(0, rail_num, rail_places, gv);		
			// add the places(can move by rail) which distance via rail is 2
			numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
			for (int i = 0; i <= rail_num; i++) {
				numReturnedLocs[count] = rail_places[i];
				count++;					
			}
		}
		// the maximum allowed distance via rail is 3
		if (rail_times == 3) {
			// using fun to get new number of palces which distance via rail is 2
			// fun will modify the rail_places array		 	
			int result = addRailPlaces(0, rail_num, rail_places, gv);
			int orig = rail_num;
			rail_num = result;
			// using fun to get new number of palces which distance via rail is 3	
			// fun will modify the rail_places array			
			rail_num = addRailPlaces(orig, rail_num, rail_places, gv);
			// add the places(can move by rail) which distance via rail is 3
			numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
			for (int i = 0; i <= rail_num; i++) {
				numReturnedLocs[count] = rail_places[i];
				count++;					
			}	
		}	
	}
	return numReturnedLocs;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// get a list which contains the adjacent places of 'from' 
	ConnList adjacent = MapGetConnections(gv -> gameMap, from);
	// count is the sum number of reachable places by any transport type
	int count = 0;
	// transportType is road
	if (road) {
		// curr_one: using to loop count number
		// curr_two: using to loop logging data
		ConnList road_cur_one = adjacent;
		ConnList road_cur_two = adjacent;
		bool isContainHospital = false;
		int road_num = 0;
		// count the number of places reachable by road that drac and hunt can get
		while (road_cur_one != NULL && road_cur_one -> type == ROAD) {
			if (road_cur_one -> p == ST_JOSEPH_AND_ST_MARY) {
				isContainHospital = true;
			} 
			road_num++;	
		 	road_cur_one = road_cur_one -> next;	
		}
		// logging the drac's reachable places
		if (player == PLAYER_DRACULA) {
			if (isContainHospital) {
				numReturnedLocs = malloc((road_num - 1) * sizeof(int));
			} else {
				numReturnedLocs = malloc(road_num * sizeof(int));
			}
			while (road_cur_two != NULL && road_cur_two -> type == ROAD) {
				// expect the two hospitals
				if (road_cur_two -> p == ST_JOSEPH_AND_ST_MARY) {
					break;
				}
				numReturnedLocs[count] = road_cur_two -> p;
				count++;
				road_cur_two = road_cur_two -> next;
			}
		}
		// logging the hunt's reachable places 
		else {
			numReturnedLocs = malloc(road_num * sizeof(int));
			while (road_cur_two != NULL && road_cur_two -> type == ROAD) {
				numReturnedLocs[count] = road_cur_two -> p;
				count++;
				road_cur_two = road_cur_two -> next;
			}
		}
	}
	// transportType is boat
	if (boat) {
		// curr_one: using to loop count number
		// curr_two: using to loop logging data
		ConnList boat_cur_one = adjacent;
		ConnList boat_cur_two = adjacent;
		int boat_num = 0;
		// count the number of places reachable by road that drac and hunt can get
		while (boat_cur_one != NULL && boat_cur_one -> type == BOAT) {
			boat_num++;		
		 	boat_cur_one = boat_cur_one -> next;	
		}
		// logging the drac's reachable places
		if (player == PLAYER_DRACULA) {
			numReturnedLocs = realloc(numReturnedLocs, (count + boat_num) * sizeof(int));
			while (boat_cur_two != NULL && boat_cur_two -> type == BOAT) {
				numReturnedLocs[count] = boat_cur_two -> p;
				count++;
				boat_cur_two = boat_cur_two -> next;
			}
		}
		// logging the hunt's reachable places 
		else {
			numReturnedLocs = realloc(numReturnedLocs, (count + boat_num) * sizeof(int));
			while (boat_cur_two != NULL && boat_cur_two -> type == BOAT) {
				numReturnedLocs[count] = boat_cur_two -> p;
				count++;
				boat_cur_two = boat_cur_two -> next;
			}
		}
	}
	// transportType is rail
	if (rail) {
		// only consider the player is hunters
		// drac can not have rail move
		if (player != PLAYER_DRACULA) {
			// using current round to calculate how far the player can move by rail
			int rail_times = (round + player) % 4;
			// an array to collect the palces which can move by rail
			int rail_places[NUM_REAL_PLACES];
			// curr: using to loop count number
			ConnList rail_cur = adjacent;
			int rail_num = 0;
			// collect the from's adjacent places(can move by rail) to rail_places array
			while (rail_cur != NULL && rail_cur -> type == RAIL) {
				rail_places[rail_num] = rail_cur -> p;
				rail_num++;		
				rail_cur = rail_cur -> next;	
			}
			// the hunter may not move by rail this turn
			if (rail_times == 0) {
				return numReturnedLocs;
			}
			// the maximum allowed distance via rail is 1
			if (rail_times == 1) {
				// add the from's adjacent places(can move by rail)
				numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
				for (int i = 0; i <= rail_num; i++) {
					numReturnedLocs[count] = rail_places[i];
					count++;					
				}
			} 
			// the maximum allowed distance via rail is 2
			if (rail_times == 2) {
				// using fun to get new number of palces which distance via rail is 2
				// fun will modify the rail_places array
				rail_num = addRailPlaces(0, rail_num, rail_places, gv);		
				// add the places(can move by rail) which distance via rail is 2
				numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
				for (int i = 0; i <= rail_num; i++) {
					numReturnedLocs[count] = rail_places[i];
					count++;					
				}
			}
			// the maximum allowed distance via rail is 3
			if (rail_times == 3) {
				// using fun to get new number of palces which distance via rail is 2
				// fun will modify the rail_places array		 	
				int result = addRailPlaces(0, rail_num, rail_places, gv);
				int orig = rail_num;
				rail_num = result;
				// using fun to get new number of palces which distance via rail is 3	
				// fun will modify the rail_places array			
				rail_num = addRailPlaces(orig, rail_num, rail_places, gv);
				// add the places(can move by rail) which distance via rail is 3
				numReturnedLocs = realloc(numReturnedLocs, (count + rail_num) * sizeof(int));
				for (int i = 0; i <= rail_num; i++) {
					numReturnedLocs[count] = rail_places[i];
					count++;					
				}	
			}
		} 
		// drac is just return
		else {
			return numReturnedLocs;	
		}
	}
	//	
	return numReturnedLocs;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
// A function to add places(can move by rail) to a given array
// this function will return a modify array and a count
// will loop all list of places can move from the each places in given array
// find the unique places(can move by rail) and add it.
int addRailPlaces(int start, int count_add, int *array, GameView gv) {
	int i = count_add;
	while (i > start) {
		ConnList current = MapGetConnections(gv -> gameMap, array[i]);
		while (current != NULL && current -> type == RAIL) {
			// check contain unique places
			int check = 0;
			bool iscontain = false;
			while (check < count_add) {
				if (current -> p == array[check]) {
					iscontain = true;
				}
				check++;
			}
			// if didn't found, adding it
			if (!iscontain)	{
				count_add++;
				array[count_add] = current -> p;
			}	
			current = current -> next;
		}
		i--;
	}
	return count_add;
} 

// Count the number of moves of the given player
int GvGetTotalMoves(GameView gv, Player player) {
    
    int num = 0;
    for (int i = 0; i < TRAIL_SIZE; i++) {
        if(gv->players[player].history[i] != NOWHERE){
            num++;
        }
    }
    return num;
}

// Count the number of locations of the given player
int GvGetTotalLocations(GameView gv, Player player) {
    
    int num = 0;
    for (int i = 0; i < TRAIL_SIZE; i++) {
        if(gv->players[player].locHistory[i] != NOWHERE){
            num++;
        }
    }
    return num;
}
