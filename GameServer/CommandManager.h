#pragma once

#include "Protocol.h"
#include "User.h"
#include "array.h"


enum eCommandNumber
{

//GAME MASTER COMMANDS
	COMMAND_GM_MOVE = 1,
	COMMAND_TRACK,
	COMMAND_TRACE,
	COMMAND_DISCONNECT,
	COMMAND_GUILD_DISCONNECT,
	COMMAND_RELOAD,
	COMMAND_SKIN,
	COMMAND_MOVE_GUILD,
	COMMAND_HIDE,
	COMMAND_GM_MONEY,
	COMMAND_MAKE,
	COMMAND_WATCH,
	COMMAND_SIEGE,
	COMMAND_SHOW_STATS,
	COMMAND_FIREWORKS,
	COMMAND_CHAT_BLOCK,
	COMMAND_TRACE_PARTY,
	COMMAND_SET_STATS,
	COMMAND_SET_AUTOPOTION,
	COMMAND_SET_TARGET,
	COMMAND_PVP_TEST,
	COMMAND_SET_BUFFS,
	COMMAND_SETPK,
	COMMAND_MUFC,
	COMMAND_SEND_POPUP,
	
//PLAYER COMMANDS
	COMMAND_RESET = 200,
	COMMAND_ADD_POINT_STRENGTH,
	COMMAND_ADD_POINT_AGILITY,
	COMMAND_ADD_POINT_VITALITY,
	COMMAND_ADD_POINT_ENERGY,
	COMMAND_ADD_POINT_LEADERSHIP,
	COMMAND_POST,
	COMMAND_PK_CLEAR,
	COMMAND_MOVE,
	COMMAND_REQUEST,
	COMMAND_BATTLE_SOCCER,
	COMMAND_GUILD_WAR,
	COMMAND_CHANGE,
	COMMAND_CLEAR_INVENTORY,
	COMMAND_WARE,
	COMMAND_MASTER_RESET,
	COMMAND_REBUILD,
	COMMAND_FILL_POTION,
	COMMAND_COLLECT, //TODO
	COMMAND_CUSTOM_ATTACK,
	COMMAND_CUSTOM_ATTACK_OFFLINE,
	COMMAND_CUSTOM_STORE_OFFLINE,	
	COMMAND_SPAWN,
	COMMAND_GHOST,	
	COMMAND_AUTODT,
	COMMAND_AUTO_POST, //TODO
	COMMAND_PIN
};

//**********************************************//
//********** DataServer -> GameServer **********//
//**********************************************//

struct SDHP_COMMAND_RESET_RECV
{
	PSBMSG_HEAD header; // C1:0F:00
	WORD index;
	char account[11];
	char name[11];
	UINT ResetDay;
	UINT ResetWek;
	UINT ResetMon;
};

struct SDHP_COMMAND_MASTER_RESET_RECV
{
	PSBMSG_HEAD header; // C1:0F:01
	WORD index;
	char account[11];
	char name[11];
	UINT MasterResetDay;
	UINT MasterResetWek;
	UINT MasterResetMon;
};

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_COMMAND_RESET_SEND
{
	PSBMSG_HEAD header; // C1:0F:00
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_COMMAND_MASTER_RESET_SEND
{
	PSBMSG_HEAD header; // C1:0F:01
	WORD index;
	char account[11];
	char name[11];
};

//**********************************************//
//**********************************************//
//**********************************************//

struct COMMAND_INFO
{
	COMMAND_INFO() {}
	COMMAND_INFO(int _code, char* _label, int _level)
	{
		code = _code;
		strcpy_s(label, _label);
		level = _level;
	}
	
	int code;
	char label[32];
	int level;
};

class CCommandManager
{
public:
	CCommandManager();
	virtual ~CCommandManager();
	void LoadCommands(char* path);
	void MainProc();
	void Add(char* label, int code, int level);
	long GetNumber(char* arg,int pos, int defaultNumber = 0);
	void GetString(char* arg,char* out,int size,int pos);
	long GetCommandCode(char* label);
	long GetCommandLevel(char* label);
	void CommandSpawn(LPOBJ lpObj, char* arg);
	bool ManagementCore(LPOBJ lpObj,char* message);
	void CommandMove(LPOBJ lpObj,char* arg);
	void CommandPost(LPOBJ lpObj,char* arg);
	void CommandAddPoint(LPOBJ lpObj,char* arg,int type);
	void CommandPKClear(LPOBJ lpObj,char* arg);
	void CommandChange(LPOBJ lpObj,char* arg);
	void CommandWare(LPOBJ lpObj,char* arg);
	void CommandReset(LPOBJ lpObj,char* arg);
	void CommandResetAuto(LPOBJ lpObj,char* arg);
	void CommandGMMove(LPOBJ lpObj,char* arg);
	void CommandGMPost(LPOBJ lpObj,int type, char* arg);
	void CommandSendPopup(LPOBJ lpObj, char* arg);
	void CommandTrack(LPOBJ lpObj,char* arg);
	void CommandTrace(LPOBJ lpObj,char* arg);
	void CommandDisconnect(LPOBJ lpObj,char* arg);
	void CommandFireworks(LPOBJ lpObj,char* arg);
	void CommandMake(LPOBJ lpObj,char* arg);
	void CommandSkin(LPOBJ lpObj,char* arg);
	void CommandSetMoney(LPOBJ lpObj,char* arg);
	void CommandMasterReset(LPOBJ lpObj,char* arg);
	void CommandGuildWar(LPOBJ lpObj,char* arg);
	void CommandBattleSoccer(LPOBJ lpObj,char* arg);
	void CommandRequest(LPOBJ lpObj,char* arg);
	void CommandHide(LPOBJ lpObj,char* arg);
	void CommandResetAutoProc(LPOBJ lpObj);
	void DGCommandResetRecv(SDHP_COMMAND_RESET_RECV* lpMsg);
	void DGCommandMasterResetRecv(SDHP_COMMAND_MASTER_RESET_RECV* lpMsg);
	void CommandClearInventory(LPOBJ lpObj, char* arg);
	void CommandEvo(LPOBJ lpObj, char* arg);
	void CommandRebuild(LPOBJ lpObj, char* arg);
	void CommandPvpTest(LPOBJ lpObj, char* arg);
	void CommandSetBuffs(LPOBJ lpObj, char* arg);
	void CommandSetTarget(LPOBJ lpObj, char* arg);
	void CommandShowStats(LPOBJ lpObj, char* arg);
	void CommandSetStats(LPOBJ lpObj, char* arg);
	void CommandSetAutoPotion(LPOBJ lpObj, char* arg);
	void CommandWatch(LPOBJ lpObj, char* arg);
	void CommandChatBlock(LPOBJ lpObj, char* arg);
	void CommandGhost(LPOBJ lpObj, char* arg);
	void CommandBringParty(LPOBJ lpObj, char* arg);
	void CommandAutoDt(LPOBJ lpObj, char* arg);
	void CommandSetPk(LPOBJ lpObj, char* arg);
	void CommandMUFC(LPOBJ lpObj, char* arg);
	void CommandSiege(LPOBJ lpObj, char* arg);
	void CommandFillPotion(LPOBJ lpObj);
	void CommandMoveGuild(LPOBJ lpObj, char* arg);
	void CommandGuildDisconnect(LPOBJ lpObj, char* arg);
	void CommandPin(LPOBJ lpObj, char* arg);
private:
	array<COMMAND_INFO> m_CommandInfo;
};

extern CCommandManager gCommandManager;
