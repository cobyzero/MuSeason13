#include "stdafx.h"
#include "BonusManager.h"
#include "CashShop.h"
#include "CastleSiege.h"
#include "CommandManager.h"
#include "CustomAttack.h"
#include "CustomStore.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "GameMaster.h"
#include "Guild.h"
#include "GuildClass.h"
#include "ItemManager.h"
#include "Log.h"
#include "MapServerManager.h"
#include "MasterSkillTree.h"
#include "MasterSkillTree_4th.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "Move.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Protocol.h"
#include "Quest.h"
#include "QuestReward.h"
#include "RheaGold.h"
#include "ResetTable.h"
#include "ServerInfo.h"
#include "SXIDecryp.h"
#include "Util.h"
#include "EventInventory.h"
#include "ItemBagManager.h"
#include "FilaHit.h"
#include "SocketManager.h"
#include "MUFC.h"

CCommandManager gCommandManager;

CCommandManager::CCommandManager() // OK
{
}

CCommandManager::~CCommandManager() // OK
{
}

void CCommandManager::LoadCommands(char* path) // OK
{
	CMemScript* lpScript = new(std::nothrow) CMemScript;

	if (lpScript == NULL)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpScript->GetLastError());
		delete lpScript;
		return;
	}

	m_CommandInfo.clear();

	try
	{
		while (true)
		{
			if (lpScript->GetToken() == 2)
			{
				break;
			}

			int CommandIndex;
			char szCommandName[30];
			int CommandRights;

			CommandIndex = lpScript->GetNumber();
			if (CommandIndex < 1)
			{
				ErrorMessageBox("Error loading file Commands.txt");
				break;
			}
			memcpy(szCommandName, lpScript->GetAsString(), sizeof(szCommandName));
			CommandRights = lpScript->GetAsNumber();

			this->Add(szCommandName, CommandIndex, CommandRights);
		}
	}
	catch (...)
	{
		delete lpScript;
		ErrorMessageBox(0, path, "Error", 0);
		return;
	}

	delete lpScript;

	LogAdd(LOG_GREEN, "[Commands] %s is loaded", path);
}

void CCommandManager::MainProc() // OK
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) == 0)
			continue;

		if (gObj[n].AutoResetEnable != 0)
			this->CommandResetAutoProc(&gObj[n]);
	}

	//MUFC COMMAND
	if (gMUFC.ReSetTimer > 0)
	{
		gMUFC.ReSetTimer--;

		if (gMUFC.ReSetTimer == 0)
		{
			gMUFC.SetNewRound();
		}
	}

	if (gMUFC.Status == MUFC_PREPARED)
	{
		if (gMUFC.StartCounter > 0)
		{
			gMUFC.SendNotice("[MUFC]   Prepare-se! Iniciando luta em (%d)   [MUFC]", gMUFC.StartCounter);
			gMUFC.StartCounter--;
		}
		else if (gMUFC.StartCounter == 0)
		{
			gMUFC.FightRun();
		}
	}
	else if (gMUFC.Status == MUFC_STARTED)
	{
		gMUFC.FightTimer++;

		for (int n = 0; n < 32; n++)
		{
			if (gMUFC.FightDamageRate[n].Timer < 0)
				break;

			if (gMUFC.FightTimer == gMUFC.FightDamageRate[n].Timer)
			{
				gMUFC.FightDamage = gMUFC.FightDamageRate[n].DamageRate;
				gMUFC.SendNotice("[MUFC]  Dano aumentado em %d porcento  [MUFC]", gMUFC.FightDamage);
				break;
			}
		}
	}
}

void CCommandManager::Add(char* label, int code, int level) // OK
{
	m_CommandInfo.add(COMMAND_INFO(code, label, level));
}

long CCommandManager::GetNumber(char* arg, int pos, int defaultNumber) // OK
{
	int count = 0, p = 0;

	char buffer[60] = { 0 };

	int len = strlen(arg);

	len = ((len >= sizeof(buffer)) ? (sizeof(buffer) - 1) : len);

	for (int n = 0; n < len; n++)
	{
		if (arg[n] == 0x20)
		{
			count++;
		}
		else if (count == pos)
		{
			buffer[p] = arg[n];
			p++;
		}
	}

	if (p > 0)
	{
		return atoi(buffer);
	}


	return defaultNumber;
}

void CCommandManager::GetString(char* arg, char* out, int size, int pos) // OK
{
	int count = 0, p = 0;

	char buffer[60] = { 0 };

	int len = strlen(arg);

	len = ((len >= sizeof(buffer)) ? (sizeof(buffer) - 1) : len);

	for (int n = 0; n < len; n++)
	{
		if (arg[n] == 0x20)
		{
			count++;
		}
		else if (count == pos)
		{
			buffer[p] = arg[n];
			p++;
		}
	}

	memcpy(out, buffer, (size - 1));
}

long CCommandManager::GetCommandCode(char* label) // OK
{
	for (int n = 0; n < m_CommandInfo.size(); n++)
	{
		if (_stricmp(label, this->m_CommandInfo[n].label) == 0)
		{
			return this->m_CommandInfo[n].code;
		}
	}

	return -1;
}

long CCommandManager::GetCommandLevel(char* label) // OK
{
	for (int n = 0; n < m_CommandInfo.size(); n++)
	{
		if (_stricmp(label, this->m_CommandInfo[n].label) == 0)
		{
			return this->m_CommandInfo[n].level;
		}
	}

	return -1;
}

bool CCommandManager::ManagementCore(LPOBJ lpObj, char* message) // OK
{
	char command[32] = { 0 };

	memset(command, 0, sizeof(command));

	this->GetString(message, command, sizeof(command), 0);

	int code = this->GetCommandCode(command);
	int level = this->GetCommandLevel(command);

	char* argument = &message[strlen(command)];

	if (argument[0] == 0x20)
	{
		argument++;
	}

	if (level > 0 && gGameMaster.CheckGameMasterLevel(lpObj, level) == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return false;
	}

	switch (code)
	{
	case COMMAND_MOVE:
		this->CommandMove(lpObj, argument);
		break;
	case COMMAND_POST:
		this->CommandPost(lpObj, argument);
		break;
	case COMMAND_ADD_POINT_STRENGTH:
		this->CommandAddPoint(lpObj, argument, 0);
		break;
	case COMMAND_ADD_POINT_AGILITY:
		this->CommandAddPoint(lpObj, argument, 1);
		break;
	case COMMAND_ADD_POINT_VITALITY:
		this->CommandAddPoint(lpObj, argument, 2);
		break;
	case COMMAND_ADD_POINT_ENERGY:
		this->CommandAddPoint(lpObj, argument, 3);
		break;
	case COMMAND_ADD_POINT_LEADERSHIP:
		this->CommandAddPoint(lpObj, argument, 4);
		break;
	case COMMAND_PK_CLEAR:
		this->CommandPKClear(lpObj, argument);
		break;
	case COMMAND_CHANGE:
		this->CommandEvo(lpObj, argument);
		break;
	case COMMAND_WARE:
		this->CommandWare(lpObj, argument);
		break;
	case COMMAND_RESET:
		this->CommandReset(lpObj, argument);
		break;
	case COMMAND_GM_MOVE:
		this->CommandGMMove(lpObj, argument);
		break;
	case COMMAND_MOVE_GUILD:
		this->CommandMoveGuild(lpObj, argument);
		break;
	case COMMAND_GUILD_DISCONNECT:
		this->CommandGuildDisconnect(lpObj, argument);
		break;
	case COMMAND_TRACK:
		this->CommandTrack(lpObj, argument);
		break;
	case COMMAND_TRACE:
		this->CommandTrace(lpObj, argument);
		break;
	case COMMAND_DISCONNECT:
		this->CommandDisconnect(lpObj, argument);
		break;
	case COMMAND_FIREWORKS:
		this->CommandFireworks(lpObj, argument);
		break;
	case COMMAND_MAKE:
		this->CommandMake(lpObj, argument);
		break;
	case COMMAND_SKIN:
		this->CommandSkin(lpObj, argument);
		break;
	case COMMAND_GM_MONEY:
		this->CommandSetMoney(lpObj, argument);
		break;
	case COMMAND_MASTER_RESET:
		this->CommandMasterReset(lpObj, argument);
		break;
	case COMMAND_GUILD_WAR:
		this->CommandGuildWar(lpObj, argument);
		break;
	case COMMAND_BATTLE_SOCCER:
		this->CommandBattleSoccer(lpObj, argument);
		break;
	case COMMAND_REQUEST:
		this->CommandRequest(lpObj, argument);
		break;
	case COMMAND_CUSTOM_STORE_OFFLINE:
		gCustomStore.CommandCustomStoreOffline(lpObj, argument);
		break;
	case COMMAND_HIDE:
		this->CommandHide(lpObj, argument);
		break;
	case COMMAND_CUSTOM_ATTACK:
		gCustomAttack.CommandCustomAttack(lpObj, argument);
		break;
	case COMMAND_CUSTOM_ATTACK_OFFLINE:
		gCustomAttack.CommandCustomAttackOffline(lpObj, argument);
		break;
	case COMMAND_CLEAR_INVENTORY:
		this->CommandClearInventory(lpObj, argument);
		break;
	case COMMAND_SPAWN:
		this->CommandSpawn(lpObj, argument);
		break;
	case COMMAND_REBUILD:
		this->CommandRebuild(lpObj, argument);
		break;
	case COMMAND_PVP_TEST:
		this->CommandPvpTest(lpObj, argument);
		break;
	case COMMAND_SET_BUFFS:
		this->CommandSetBuffs(lpObj, argument);
		break;
	case COMMAND_SET_TARGET:
		this->CommandSetTarget(lpObj, argument);
		break;
	case COMMAND_SHOW_STATS:
		this->CommandShowStats(lpObj, argument);
		break;
	case COMMAND_SET_STATS:
		this->CommandSetStats(lpObj, argument);
		break;
	case COMMAND_SET_AUTOPOTION:
		this->CommandSetAutoPotion(lpObj, argument);
		break;
	case COMMAND_WATCH:
		this->CommandWatch(lpObj, argument);
		break;
	case COMMAND_CHAT_BLOCK:
		this->CommandChatBlock(lpObj, argument);
		break;
	case COMMAND_GHOST:
		this->CommandGhost(lpObj, argument);
		break;
	case COMMAND_TRACE_PARTY:
		this->CommandBringParty(lpObj, argument);
		break;
	case COMMAND_AUTODT:
		this->CommandAutoDt(lpObj, argument);
		break;
	case COMMAND_SETPK:
		this->CommandSetPk(lpObj, argument);
		break;
	case COMMAND_MUFC:
		this->CommandMUFC(lpObj, argument);
		break;
	case COMMAND_SIEGE:
		this->CommandSiege(lpObj, argument);
		break;
	case COMMAND_FILL_POTION:
		this->CommandFillPotion(lpObj);
		break;
	case COMMAND_SEND_POPUP:
		this->CommandSendPopup(lpObj, argument);
		break;
	case COMMAND_PIN:
		this->CommandPin(lpObj, argument);
		break;
	default:
		return 0;
	}

	return 1;
}

void CCommandManager::CommandMove(LPOBJ lpObj, char* arg) // OK
{
	MOVE_INFO MoveInfo;

	if (gMove.GetInfoByName(arg, &MoveInfo) != 0)
	{
		gMove.Move(lpObj, MoveInfo.Index);
		return;
	}
}

void CCommandManager::CommandPost(LPOBJ lpObj, char* arg) // OK
{
	if (lpObj->ChatLimitTime > 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(269), lpObj->ChatLimitTime);
		return;
	}

	if (gServerInfo.m_CommandPostEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Level < gServerInfo.m_CommandPostLevel[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(65), gServerInfo.m_CommandPostLevel[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Reset < gServerInfo.m_CommandPostReset[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(66), gServerInfo.m_CommandPostReset[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandPostMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(67), gServerInfo.m_CommandPostMoney[lpObj->AccountLevel]);
		return;
	}

	DWORD tick = (GetTickCount() - lpObj->PostTime) / 1000;

	if (tick < ((DWORD)gServerInfo.m_CommandPostDelay[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(68), (gServerInfo.m_CommandPostDelay[lpObj->AccountLevel] - tick));
		return;
	}

	lpObj->PostTime = GetTickCount();

	lpObj->Money -= gServerInfo.m_CommandPostMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	if (gServerInfo.m_CommandPostGlobal[lpObj->AccountLevel] == 0)
	{
		PostMessage(lpObj->Name, gMessage.GetMessage(69), arg, gServerInfo.m_CommandPostType[lpObj->AccountLevel]);
	}
	else
	{
		GDGlobalPostSend(gMapServerManager.GetMapServerGroup(), gServerInfo.m_CommandPostType[lpObj->AccountLevel], lpObj->Name, arg);
	}

	gLog.Output(LOG_COMMAND, "[CommandPost][%s][%s] - (Message: %s)", lpObj->Account, lpObj->Name, arg);
}

void CCommandManager::CommandAddPoint(LPOBJ lpObj, char* arg, int type) // OK
{
	if (gServerInfo.m_CommandAddPointEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Class != CLASS_DL && type == 4)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(71));
		return;
	}

	int amount = this->GetNumber(arg, 0);

	if (amount <= 0 || lpObj->LevelUpPoint < amount)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(72));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandAddPointMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(73), gServerInfo.m_CommandAddPointMoney[lpObj->AccountLevel]);
		return;
	}

	if (gObjectManager.CharacterLevelUpPointAdd(lpObj, type, amount) == 0)
	{
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandAddPointMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	GCNewCharacterInfoSend(lpObj);

	SXGetCharacterInfo(lpObj->Index);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(74), amount, lpObj->LevelUpPoint);

	gLog.Output(LOG_COMMAND, "[CommandAddPoint][%s][%s] - (Type: %d, Amount: %d)", lpObj->Account, lpObj->Name, type, amount);
}

void CCommandManager::CommandPKClear(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandPKClearEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->PKLevel <= 3)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(76));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandPKClearMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(77), gServerInfo.m_CommandPKClearMoney[lpObj->AccountLevel]);
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandPKClearMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->PKLevel = 3;

	GCPKLevelSend(lpObj->Index, lpObj->PKLevel);

	GCMessagePopupSend(lpObj, gMessage.GetMessage(78));

	gLog.Output(LOG_COMMAND, "[CommandPKClear][%s][%s] - (PKLevel: %d)", lpObj->Account, lpObj->Name, lpObj->PKLevel);
}

void CCommandManager::CommandChange(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandChangeEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(82), gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]);
		return;
	}

	int ChangeUp = lpObj->ChangeUp;

	if (lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		ChangeUp++;
	}

	if (ChangeUp >= gServerInfo.m_CommandChangeLimit[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(83));
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	for (int n = 0; n <= ((ChangeUp == 0) ? 3 : 6); n++)
	{
		if (gQuest.CheckQuestListState(lpObj, n, QUEST_FINISH) == 0)
		{
			if (n != 3 || lpObj->Class == CLASS_DK)
			{
				gQuest.AddQuestList(lpObj, n, QUEST_ACCEPT);
				gQuestReward.InsertQuestReward(lpObj, n);
				gQuest.AddQuestList(lpObj, n, QUEST_FINISH);
			}
		}
	}

	lpObj->SendQuestInfo = 0;

	gQuest.GCQuestInfoSend(lpObj->Index);

	GCMessagePopupSend(lpObj, gMessage.GetMessage(84));

	gLog.Output(LOG_COMMAND, "[CommandChange][%s][%s] - (ChangeUp: %d)", lpObj->Account, lpObj->Name, ChangeUp);
}

void CCommandManager::CommandWare(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandWareEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->LoadWarehouse != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(86));
		return;
	}

	int number = this->GetNumber(arg, 0);

	if (number < 0 || number >= gServerInfo.m_CommandWareNumber[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(87));
		return;
	}

	lpObj->WarehouseMoney = 0;

	memset(lpObj->WarehouseMap, 0xFF, WAREHOUSE_SIZE);

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		lpObj->Warehouse[n].Clear();
	}

	lpObj->Interface.use = 0;

	lpObj->Interface.type = INTERFACE_NONE;

	lpObj->Interface.state = 0;

	lpObj->LoadWarehouse = 0;

	lpObj->WarehouseNumber = number;

	GCMessagePopupSend(lpObj, gMessage.GetMessage(88), number);

	gLog.Output(LOG_COMMAND, "[CommandWare][%s][%s] - (Number: %d)", lpObj->Account, lpObj->Name, number);
}

void CCommandManager::CommandReset(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandResetEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	char mode[5] = { 0 };

	this->GetString(arg, mode, sizeof(mode), 0);

	if (strcmp(mode, "auto") == 0)
	{
		this->CommandResetAuto(lpObj, arg);
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(90));
		return;
	}

	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gResetTable.GetResetLevel(lpObj))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(91), gResetTable.GetResetLevel(lpObj));
		return;
	}

	if (lpObj->Money < ((DWORD)gResetTable.GetResetMoney(lpObj)))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(92), gResetTable.GetResetMoney(lpObj));
		return;
	}

	if (lpObj->Reset >= gServerInfo.m_CommandResetLimit[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(93), gServerInfo.m_CommandResetLimit[lpObj->AccountLevel]);
		return;
	}

	if (gServerInfo.m_CommandResetCheckItem[lpObj->AccountLevel] != 0)
	{
		for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				GCMessagePopupSend(lpObj, gMessage.GetMessage(94));
				return;
			}
		}
	}

	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return;
	}

	SDHP_COMMAND_RESET_SEND pMsg;

	pMsg.header.set(0x0F, 0x00, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CCommandManager::CommandResetAuto(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandResetAutoEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->AutoResetEnable == 0)
	{
		lpObj->AutoResetEnable = 1;

		lpObj->AutoResetStats[0] = this->GetNumber(arg, 1);
		lpObj->AutoResetStats[1] = this->GetNumber(arg, 2);
		lpObj->AutoResetStats[2] = this->GetNumber(arg, 3);
		lpObj->AutoResetStats[3] = this->GetNumber(arg, 4);
		lpObj->AutoResetStats[4] = ((lpObj->Class == CLASS_DL) ? this->GetNumber(arg, 5) : 0);

		GCMessagePopupSend(lpObj, gMessage.GetMessage(116));
	}
	else
	{
		lpObj->AutoResetEnable = 0;

		lpObj->AutoResetStats[0] = 0;
		lpObj->AutoResetStats[1] = 0;
		lpObj->AutoResetStats[2] = 0;
		lpObj->AutoResetStats[3] = 0;
		lpObj->AutoResetStats[4] = 0;

		GCMessagePopupSend(lpObj, gMessage.GetMessage(117));
	}
}

void CCommandManager::CommandGMMove(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	int map = this->GetNumber(arg, 1);

	int x = this->GetNumber(arg, 2);

	int y = this->GetNumber(arg, 3);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gObjTeleport(lpTarget->Index, map, x, y);

	gLog.Output(LOG_COMMAND, "[CommandGMMove][%s][%s] - (Name: %s, Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, name, map, x, y);
}

void CCommandManager::CommandGMPost(LPOBJ lpObj, int type, char* arg) // OK
{
	if (type == 0)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[%s] %s", lpObj->Name, arg);
	}
	else if (type == 1)
	{
		GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, arg);
	}
	else if (type == 2)
	{
		gNotice.GCPopupSendToAll(arg);
	}
	else
	{
		GDGlobalPopupSend(gMapServerManager.GetMapServerGroup(), arg);
	}

	gLog.Output(LOG_COMMAND, "[CommandGMPost][%s][%s] - (Type: %d, Message: %s)", lpObj->Account, lpObj->Name, type, arg);
}

void CCommandManager::CommandSendPopup(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	GCMessagePopupSend(lpTarget, &arg[strlen(name)]);
}

void CCommandManager::CommandTrack(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gObjTeleport(lpObj->Index, lpTarget->Map, lpTarget->X, lpTarget->Y);

	gLog.Output(LOG_COMMAND, "[CommandTrack][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandTrace(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gObjTeleport(lpTarget->Index, lpObj->Map, lpObj->X, lpObj->Y);

	gLog.Output(LOG_COMMAND, "[CommandTrace][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandDisconnect(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gObjUserKill(lpTarget->Index);

	gLog.Output(LOG_COMMAND, "[CommandDisconnect][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
}

void CCommandManager::CommandFireworks(LPOBJ lpObj, char* arg) // OK
{
	for (int n = 0; n < 15; n++)
	{
		GCFireworksSend(lpObj, (lpObj->X + (((GetLargeRand() % 5) * 2) - 4)), (lpObj->Y + (((GetLargeRand() % 5) * 2) - 4)));
	}

	gLog.Output(LOG_COMMAND, "[CommandFireworks][%s][%s] - (Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, lpObj->Map, lpObj->X, lpObj->Y);
}

void CCommandManager::CommandMake(LPOBJ lpObj, char* arg) // OK
{
	int section = this->GetNumber(arg, 0);
	int type = this->GetNumber(arg, 1);
	int level = this->GetNumber(arg, 2, 0);
	int skill = this->GetNumber(arg, 3, 0);
	int luck = this->GetNumber(arg, 4, 0);
	int option = this->GetNumber(arg, 5, 0);
	int exc = this->GetNumber(arg, 6, 0);
	int set = this->GetNumber(arg, 7, 0);
	int count = this->GetNumber(arg, 8, 1);

	for (int i = 0; i < count; i++)
	{
		GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, GET_ITEM(section, type), level, 0, skill, luck, option, -1, exc, set, 0, 0, 0, 0xFF, 0);
	}

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(107));

	gLog.Output(LOG_COMMAND, "[CommandMake][%s][%s] - (Section: %d, Type: %d, Level: %d, Skill: %d, Luck: %d, Option: %d, Exc: %d, Set: %d)", lpObj->Account, lpObj->Name, section, type, level, skill, luck, option, exc, set);
}

void CCommandManager::CommandSkin(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpTarget->Change = this->GetNumber(arg, 1);

	gObjViewportListProtocolCreate(lpTarget);

	gObjectManager.CharacterUpdateMapEffect(lpTarget);

	gLog.Output(LOG_COMMAND, "[CommandSkin][%s][%s] - (Name: %s, Change: %d)", lpObj->Account, lpObj->Name, name, lpTarget->Change);
}

void CCommandManager::CommandSetMoney(LPOBJ lpObj, char* arg) // OK
{
	char name[11] = { 0 };
	char type[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);
	this->GetString(arg, type, sizeof(name), 1);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int amount = this->GetNumber(arg, 2);

	if (amount <= 0)
	{
		return;
	}

	if (strcmp(type, gServerInfo.m_CommandCoinSyntaxZen) == 0)
	{
		if (gObjCheckMaxMoney(lpTarget->Index, amount) == 0)
		{
			lpTarget->Money = MAX_MONEY;
		}
		else
		{
			lpTarget->Money += amount;
		}

		GCMoneySend(lpTarget->Index, lpTarget->Money);

		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(32));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxRuud) == 0)
	{
		lpTarget->Ruud += amount;
		gRheaGold.GC_Notify_RuudUpdate(lpTarget->Index, lpTarget->Ruud, amount, 1);

		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(33));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxGP) == 0)
	{
		gCashShop.GDCashShopAddPointSaveSend(lpTarget->Index, 0, 0, 0, amount);
		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(29));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxWP) == 0)
	{
		gCashShop.GDCashShopAddPointSaveSend(lpTarget->Index, 0, 0, amount, 0);
		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(30));
	}
	else if (strcmp(type, gServerInfo.m_CommandCoinSyntaxWC) == 0)
	{
		gCashShop.GDCashShopAddPointSaveSend(lpTarget->Index, 0, amount, 0, 0);
		GCMessagePopupSend(lpTarget, "%s %s %d %s", lpObj->Name, gMessage.GetMessage(34), amount, gMessage.GetMessage(31));
	}

	gLog.Output(LOG_COMMAND, "[CommandSetMoney][%s][%s] - (Name: %s, Type: %s, Amount: %d)", lpObj->Account, lpObj->Name, name, type, amount);
}

void CCommandManager::CommandMasterReset(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandMasterResetEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(119));
		return;
	}

	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gServerInfo.m_CommandMasterResetLevel[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(120), gServerInfo.m_CommandMasterResetLevel[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Reset < gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(121), gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(122), gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->MasterReset >= gServerInfo.m_CommandMasterResetLimit[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(123), gServerInfo.m_CommandMasterResetLimit[lpObj->AccountLevel]);
		return;
	}

	if (gServerInfo.m_CommandMasterResetCheckItem[lpObj->AccountLevel] != 0)
	{
		for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				GCMessagePopupSend(lpObj, gMessage.GetMessage(124));
				return;
			}
		}
	}

	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return;
	}

	SDHP_COMMAND_MASTER_RESET_SEND pMsg;

	pMsg.header.set(0x0F, 0x01, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CCommandManager::CommandGuildWar(LPOBJ lpObj, char* arg) // OK
{
	char GuildName[9] = { 0 };

	this->GetString(arg, GuildName, sizeof(GuildName), 0);

	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 0);
	}
}

void CCommandManager::CommandBattleSoccer(LPOBJ lpObj, char* arg) // OK
{
	char GuildName[9] = { 0 };

	this->GetString(arg, GuildName, sizeof(GuildName), 0);

	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 1);
	}
}

void CCommandManager::CommandRequest(LPOBJ lpObj, char* arg) // OK
{
	char mode[5] = { 0 };

	this->GetString(arg, mode, sizeof(mode), 0);

	if (strcmp(mode, "on") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option &= ~4;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		GCMessagePopupSend(lpObj, gMessage.GetMessage(126));
	}
	else if (strcmp(mode, "off") == 0)
	{
		lpObj->Option &= ~1;
		lpObj->Option &= ~2;
		lpObj->Option &= ~4;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		GCMessagePopupSend(lpObj, gMessage.GetMessage(127));
	}
	else if (strcmp(mode, "auto") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option |= 4;

		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));

		this->GetString(arg, lpObj->AutoPartyPassword, sizeof(lpObj->AutoPartyPassword), 1);

		GCMessagePopupSend(lpObj, gMessage.GetMessage(128));
	}
}

void CCommandManager::CommandHide(LPOBJ lpObj, char* arg) // OK
{
	if (gEffectManager.CheckEffect(lpObj, EFFECT_INVISIBILITY) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, EFFECT_INVISIBILITY, 0, 0, 0, 0, 0);

		gObjViewportListProtocolDestroy(lpObj);

		gLog.Output(LOG_COMMAND, "[CommandHide][%s][%s] - (State: %d)", lpObj->Account, lpObj->Name, 1);
	}
	else
	{
		gEffectManager.DelEffect(lpObj, EFFECT_INVISIBILITY);

		gObjViewportListProtocolCreate(lpObj);

		gLog.Output(LOG_COMMAND, "[CommandHide][%s][%s] - (State: %d)", lpObj->Account, lpObj->Name, 0);
	}
}

void CCommandManager::CommandResetAutoProc(LPOBJ lpObj) // OK
{
	if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		return;
	}

	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gResetTable.GetResetLevel(lpObj))
	{
		return;
	}

	if (lpObj->Money < ((DWORD)gResetTable.GetResetMoney(lpObj)))
	{
		return;
	}

	if (lpObj->Reset >= gServerInfo.m_CommandResetLimit[lpObj->AccountLevel])
	{
		return;
	}

	if (gServerInfo.m_CommandResetCheckItem[lpObj->AccountLevel] != 0)
	{
		for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				return;
			}
		}
	}

	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return;
	}

	SDHP_COMMAND_RESET_SEND pMsg;

	pMsg.header.set(0x0F, 0x00, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CCommandManager::DGCommandResetRecv(SDHP_COMMAND_RESET_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandResetRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CommandManagerTransaction[0] = 0;

	if (lpMsg->ResetDay >= ((DWORD)gServerInfo.m_CommandResetLimitDay[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(129), gServerInfo.m_CommandResetLimitDay[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->ResetWek >= ((DWORD)gServerInfo.m_CommandResetLimitWek[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(130), gServerInfo.m_CommandResetLimitWek[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->ResetMon >= ((DWORD)gServerInfo.m_CommandResetLimitMon[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(131), gServerInfo.m_CommandResetLimitMon[lpObj->AccountLevel]);
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandResetMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->Level = ((gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel] == -1) ? (lpObj->Level - gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel]) : gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel]);

	lpObj->Experience = gLevelExperience[lpObj->Level - 1];

	lpObj->Reset += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->ResetDay += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->ResetWek += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->ResetMon += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	if (gServerInfo.m_CommandResetQuest[lpObj->AccountLevel] != 0)
	{
		lpObj->DBClass = lpObj->DBClass >> 4;
		lpObj->ChangeUp = CS_GET_CHANGEUP_SXII(lpObj->DBClass);

		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE Class = lpObj->DBClass;

		gQuest.GCQuestRewardSend(lpObj->Index, 201, Class);

		for (int i = 0; i < 200; i++)
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}

		gQuest.GCQuestInfoSend(lpObj->Index);
	}

	if (gServerInfo.m_CommandResetStartMasterLevel[lpObj->AccountLevel] > -1)
	{
		if (lpObj->MasterLevel > gServerInfo.m_CommandResetStartMasterLevel[lpObj->AccountLevel])
		{
			lpObj->MasterLevel = gServerInfo.m_CommandResetStartMasterLevel[lpObj->AccountLevel];
		}
	}

	if (gServerInfo.m_CommandResetSkill[lpObj->AccountLevel] != 0)
	{
		lpObj->MasterLevel = 0;
		lpObj->MasterPoint = 0;

		for (int n = 0; n < MAX_SKILL_LIST; n++)
		{
			lpObj->Skill[n].Clear();
		}

		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			lpObj->MasterSkill[n].Clear();
		}

		gMasterSkillTree.GCMasterSkillListSend(lpObj->Index);

		gSkillManager.GCSkillListSend(lpObj, 0);

		gObjectManager.CharacterCalcAttribute(lpObj->Index);

		gMasterSkillTree.GCMasterInfoSend(lpObj);
	}

	if (gServerInfo.m_CommandResetType == 1)
	{
		int point = gResetTable.GetResetPoint(lpObj);

		point = (point * gServerInfo.m_CommandResetPointRate[lpObj->Class]) / 100;

		point += (lpObj->Level - 1) * gServerInfo.m_LevelUpPoint[lpObj->Class];

		point += ((gQuest.CheckQuestListState(lpObj, 2, QUEST_FINISH) == 0) ? 0 : ((lpObj->Level > 220) ? ((lpObj->Level - 220) * gServerInfo.m_PlusStatPoint) : 0));

		point += gQuest.GetQuestRewardLevelUpPoint(lpObj);

		point += lpObj->FruitAddPoint;

		lpObj->LevelUpPoint = point;

		lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
		lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
		lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
		lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
		lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
	}

	if (gServerInfo.m_CommandMasterResetType == 1)
	{
		int point = 0;

		point = gServerInfo.m_CommandMasterResetPoint[lpObj->AccountLevel] * lpObj->MasterReset;

		point = (point * gServerInfo.m_CommandMasterResetPointRate[lpObj->Class]) / 100;

		lpObj->LevelUpPoint += point;
	}

	//Auto stats add after reset
	for (int i = 0; i < 5; i++)
	{
		if (lpObj->Class != CLASS_DL && i == 4)
			break;

		if (lpObj->AutoDt[i] > 0)
			gObjectManager.CharacterLevelUpPointAdd(lpObj, i, lpObj->AutoDt[i]);
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);

	GDCharacterInfoSaveSend(lpObj->Index);

	GDResetInfoSaveSend(lpObj->Index, lpMsg->ResetDay, lpMsg->ResetWek, lpMsg->ResetMon);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(95), lpObj->Reset);

	if (gServerInfo.m_CommandResetMove[lpObj->AccountLevel] != 0)
	{
		switch (lpObj->Class)
		{
		case CLASS_GL:
		case CLASS_DW:
		case CLASS_DK:
		case CLASS_MG:
		case CLASS_DL:
		case CLASS_RF:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_FE:
			gObjMoveGate(lpObj->Index, 27);
			break;
		case CLASS_SU:
			gObjMoveGate(lpObj->Index, 267);
			break;
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandReset][%s][%s] - (Reset: %d)", lpObj->Account, lpObj->Name, lpObj->Reset);
}

void CCommandManager::CommandSpawn(LPOBJ lpObj, char* arg)
{
	auto monsterClass = GetNumber(arg, 0, 0);
	auto monsterCount = GetNumber(arg, 1, 1);
	auto monsterX = GetNumber(arg, 2, lpObj->X);
	auto monsterY = GetNumber(arg, 3, lpObj->Y);
	auto monsterMapNumber = GetNumber(arg, 4, lpObj->Map);

	for (int i = 0; i < monsterCount; i++)
	{
		int index = gObjAddMonster(lpObj->Map);

		if (OBJECT_RANGE(index) == 0)
		{
			continue;
		}

		LPOBJ MonsterlpObj = &gObj[index];

		MonsterlpObj->X = monsterX;
		MonsterlpObj->Y = monsterY;
		MonsterlpObj->TX = monsterX;
		MonsterlpObj->TY = monsterY;
		MonsterlpObj->OldX = monsterX;
		MonsterlpObj->OldY = monsterY;
		MonsterlpObj->StartX = monsterX;
		MonsterlpObj->StartY = monsterY;
		MonsterlpObj->Dir = (lpObj->Dir + i) % 8;
		MonsterlpObj->Map = monsterMapNumber;

		if (gObjSetMonster(index, monsterClass, true) == 0)
		{
			gObjDel(index);
			continue;
		}
	}
}

void CCommandManager::DGCommandMasterResetRecv(SDHP_COMMAND_MASTER_RESET_RECV* lpMsg) // OK
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandMasterResetRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	lpObj->CommandManagerTransaction[0] = 0;

	if (lpMsg->MasterResetDay >= ((DWORD)gServerInfo.m_CommandMasterResetLimitDay[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(132), gServerInfo.m_CommandMasterResetLimitDay[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->MasterResetWek >= ((DWORD)gServerInfo.m_CommandMasterResetLimitWek[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(133), gServerInfo.m_CommandMasterResetLimitWek[lpObj->AccountLevel]);
		return;
	}

	if (lpMsg->MasterResetMon >= ((DWORD)gServerInfo.m_CommandMasterResetLimitMon[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(134), gServerInfo.m_CommandMasterResetLimitMon[lpObj->AccountLevel]);
		return;
	}

	lpObj->Money -= gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel];

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->Level = ((gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel] == -1) ? (lpObj->Level - gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel]) : gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel]);

	lpObj->Experience = gLevelExperience[lpObj->Level - 1];

	lpObj->Reset = ((gServerInfo.m_CommandMasterResetStartReset[lpObj->AccountLevel] == -1) ? (lpObj->Reset - gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel]) : gServerInfo.m_CommandMasterResetStartReset[lpObj->AccountLevel]);

	lpObj->MasterReset += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->MasterResetDay += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->MasterResetWek += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	lpMsg->MasterResetMon += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);

	if (gServerInfo.m_CommandMasterResetQuest[lpObj->AccountLevel] != 0)
	{
		lpObj->DBClass = lpObj->DBClass >> 4;

		lpObj->ChangeUp = CS_GET_CHANGEUP_SXII(lpObj->DBClass);	// 

		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE Class = lpObj->DBClass;

		gQuest.GCQuestRewardSend(lpObj->Index, 201, Class);

		for (int i = 0; i < 200; i++)
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}

		gQuest.GCQuestInfoSend(lpObj->Index);
	}

	if (gServerInfo.m_CommandMasterResetSkill[lpObj->AccountLevel] != 0)
	{
		lpObj->MasterLevel = 0;

		lpObj->MasterPoint = 0;

		for (int n = 0; n < MAX_SKILL_LIST; n++)
		{
			lpObj->Skill[n].Clear();
		}

		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			lpObj->MasterSkill[n].Clear();
		}

		gMasterSkillTree.GCMasterSkillListSend(lpObj->Index);

		gSkillManager.GCSkillListSend(lpObj, 0);

		gObjectManager.CharacterCalcAttribute(lpObj->Index);

		gMasterSkillTree.GCMasterInfoSend(lpObj);
	}

	if (gServerInfo.m_CommandMasterResetType == 1)
	{
		int point = 0;

		point = gServerInfo.m_CommandMasterResetPoint[lpObj->AccountLevel] * lpObj->MasterReset;

		point = (point * gServerInfo.m_CommandMasterResetPointRate[lpObj->Class]) / 100;

		point += (lpObj->Level - 1) * gServerInfo.m_LevelUpPoint[lpObj->Class];

		point += ((gQuest.CheckQuestListState(lpObj, 2, QUEST_FINISH) == 0) ? 0 : ((lpObj->Level > 220) ? ((lpObj->Level - 220) * gServerInfo.m_PlusStatPoint) : 0));

		point += gQuest.GetQuestRewardLevelUpPoint(lpObj);

		point += lpObj->FruitAddPoint;

		lpObj->LevelUpPoint = point;

		lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
		lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
		lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
		lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
		lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
	}

	if (gServerInfo.m_CommandResetType == 1)
	{
		int point = gResetTable.GetResetPoint(lpObj);

		point = (point * gServerInfo.m_CommandResetPointRate[lpObj->Class]) / 100;

		lpObj->LevelUpPoint += point;
	}

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);

	GDCharacterInfoSaveSend(lpObj->Index);

	GDMasterResetInfoSaveSend(lpObj->Index, lpMsg->MasterResetDay, lpMsg->MasterResetWek, lpMsg->MasterResetMon);

	GCMessagePopupSend(lpObj, gMessage.GetMessage(125), lpObj->MasterReset);

	if (gServerInfo.m_CommandMasterResetMove[lpObj->AccountLevel] != 0)
	{
		switch (lpObj->Class)
		{
		case CLASS_GL:
		case CLASS_DW:
		case CLASS_DK:
		case CLASS_MG:
		case CLASS_DL:
		case CLASS_RF:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_FE:
			gObjMoveGate(lpObj->Index, 27);
			break;
		case CLASS_SU:
			gObjMoveGate(lpObj->Index, 267);
			break;
		}
	}

	gLog.Output(LOG_COMMAND, "[CommandMasterReset][%s][%s] - (MasterReset: %d)", lpObj->Account, lpObj->Name, lpObj->MasterReset);
}

void CCommandManager::CommandClearInventory(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandClearInventoryEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	int MaxValue = gItemManager.GetInventoryMaxValue(lpObj);

	for (int i = INVENTORY_WEAR_SIZE; i < MaxValue; i++)
	{
		gItemManager.InventoryDelItem(lpObj->Index, i);
		gItemManager.GCItemDeleteSend(lpObj->Index, i, 1);
	}

	GCMessagePopupSend(lpObj, gMessage.GetMessage(13));

	gLog.Output(LOG_COMMAND, "[CommandClearIventory] Used for: [%s] [%s]", lpObj->Account, lpObj->Name);
}

void CCommandManager::CommandEvo(LPOBJ lpObj, char* arg) // OK
{
	if (gServerInfo.m_CommandChangeEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(82), gServerInfo.m_CommandChangeMoney[lpObj->AccountLevel]);
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(81));
		return;
	}

	int profession = GetNumber(arg, 0, 0);

	if (profession == 0) // no input
	{
		switch (lpObj->DBClass)
		{
		case DB_CLASS_DW:	case DB_CLASS_DK:	case DB_CLASS_FE:	case DB_CLASS_SU:
			profession = 2;
			break;
		case DB_CLASS_SM:	case DB_CLASS_BK:	case DB_CLASS_ME:	case DB_CLASS_MG:
		case DB_CLASS_DL:	case DB_CLASS_BS:	case DB_CLASS_RF:	case DB_CLASS_FK:
			profession = 3;
			break;
		case DB_CLASS_GM:	case DB_CLASS_BM:	case DB_CLASS_HE:	case DB_CLASS_DM:
		case DB_CLASS_LE:	case DB_CLASS_DS:	case DB_CLASS_FM:	case DB_CLASS_PK:
		{
			profession = 4;
		}
		break;
		}
	}

	if (lpObj->DBClass == DB_CLASS_MG || lpObj->DBClass == DB_CLASS_DL && profession == 2)
	{
		profession = 3;
	}

	bool changed = false;

	switch (profession)
	{
	case 1:
	{
		lpObj->DBClass &= ~14;
		lpObj->ChangeUp = 0;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		for (int i = 0; i <= 9; i++)
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}
		lpObj->SendQuestInfo = 0;

		gQuest.GCQuestRewardSend(lpObj->Index, 200, lpObj->DBClass);
		gQuest.GCQuestInfoSend(lpObj->Index);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		GCMessagePopupSend(lpObj, gMessage.GetMessage(80));
		changed = true;
	}
	break;
	case 2:
	{
		lpObj->ChangeUp = 1;
		lpObj->DBClass |= CS_SET_CHANGEUP_SXII;

		lpObj->LevelUpPoint += 220;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE btClass = lpObj->Class << 4;
		btClass |= 0x08;

		for (int i = 0; i < 4; i++) // complete 4 first quest states = first prof
		{
			lpObj->Quest[i].questState = QUEST_FINISH;
		}
		for (int i = 4; i <= 9; i++) // uncomplete 5-9 quests 
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}
		lpObj->SendQuestInfo = 0;

		gQuest.GCQuestRewardSend(lpObj->Index, 201, btClass);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		changed = true;
	}
	break;
	case 3:
	{
		lpObj->ChangeUp = 2;
		lpObj->LevelUpPoint += 250;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);

		BYTE btClass = (lpObj->Class << 4);
		btClass |= 0x08;
		btClass |= 0x04;

		lpObj->DBClass |= CS_SET_CHANGEUP_SXII;
		lpObj->DBClass |= CS_SET_CHANGEUP2_SXII;

		for (int i = 0; i < 7; i++) // complete 7 first quest states = first prof
		{
			lpObj->Quest[i].questState = QUEST_FINISH;
		}
		for (int i = 7; i <= 9; i++) // uncomplete other quests 
		{
			lpObj->Quest[i].questState = QUEST_CANCEL;
		}
		lpObj->SendQuestInfo = 0;

		GCLevelUpSend(lpObj);
		gQuest.GCQuestRewardSend(lpObj->Index, 204, btClass);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		changed = true;
	}
	break;
	case 4:
	{
		lpObj->ChangeUp = 3;
		lpObj->LevelUpPoint += 250;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		BYTE btClass = (lpObj->DBClass);
		btClass |= 0x08;
		btClass |= 0x04;
		btClass |= 0x02;

		lpObj->DBClass |= CS_SET_CHANGEUP_SXII;
		lpObj->DBClass |= CS_SET_CHANGEUP2_SXII;
		lpObj->DBClass |= CS_SET_CHANGEUP4_SXII;

		for (int i = 0; i <= 9; i++) // complete all prof quests
		{
			lpObj->Quest[i].questState = QUEST_FINISH;
		}
		lpObj->SendQuestInfo = 0;

		gQuest.GCQuestRewardSend(lpObj->Index, 206, btClass);
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		GCLevelUpSend(lpObj);
		changed = true;
	}
	break;
	}

	if (changed)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(84));
	}
	else
	{
		GCMessagePopupSend(lpObj, "%s\n%s", gMessage.GetMessage(85), gMessage.GetMessage(86));
	}
}

void CCommandManager::CommandRebuild(LPOBJ lpObj, char* arg)
{
	if (gServerInfo.m_CommandRebuildEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(14));
		return;
	}

	int TotalPoints = 0;
	TotalPoints = lpObj->LevelUpPoint + lpObj->Strength + lpObj->Dexterity + lpObj->Energy + lpObj->Vitality + lpObj->Leadership;

	lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
	lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
	lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
	lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
	lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;

	lpObj->LevelUpPoint = TotalPoints - lpObj->Strength - lpObj->Dexterity - lpObj->Vitality - lpObj->Energy - lpObj->Leadership;

	gEffectManager.ClearAllEffect(lpObj);

	lpObj->MaxLife = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].MaxLife + ((lpObj->Level - 1) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].LevelLife) + ((lpObj->Vitality - gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].VitalityToLife);
	lpObj->MaxMana = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].MaxMana + ((lpObj->Level - 1) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].LevelMana) + ((lpObj->Energy - gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy) * gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].EnergyToMana);

	gObjectManager.CharacterCalcAttribute(lpObj->Index);

	GCNewCharacterInfoSend(lpObj);

	SXGetCharacterInfo(lpObj->Index);

	GDCharacterInfoSaveSend(lpObj->Index);

	GCMessagePopupSend(lpObj, gMessage.GetMessage(15));
}

void CCommandManager::CommandPvpTest(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	if (strlen(name) > 3)
	{
		LPOBJ lpTarget = gObjFind(name);

		if (lpTarget == 0)
		{
			GCMessagePopupSend(lpObj, "Defensive Player not found (%s).", name);
			return;
		}
		else
		{
			lpTarget->TargetPvPIndex = 99999;
		}
	}

	if (!IsPvPTestOn)
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "PvP test started");
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "PvP test stopped");
		g_FilaHit.LimparFila();
	}

	PvPTimer = GetTickCount();
	IsPvPTestOn = !IsPvPTestOn;
}

void CCommandManager::CommandSetBuffs(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int Type = this->GetNumber(arg, 1, 0);

	if (Type == 0) //self class buff
	{
		switch (lpTarget->Class)
		{
		case CLASS_DW:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_MANA_SHIELD) != 0)
				gSkillManager.SkillManaShield(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_MANA_SHIELD));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Soul Barrier.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_MAGIC_CIRCLE) != 0)
				gSkillManager.SkillMagicCircle(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_MAGIC_CIRCLE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Expansion of Wizardry.", lpTarget->Name);
		}
		break;
		case CLASS_DK:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_LIFE) != 0)
				gSkillManager.SkillGreaterLife(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_LIFE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Greater Fortitude.", lpTarget->Name);
		}
		break;
		case CLASS_DL:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_CRITICAL_DAMAGE) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_CRITICAL_DAMAGE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Critical Damage.", lpTarget->Name);
		}
		break;
		case CLASS_SU:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_SWORD_POWER) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_SWORD_POWER));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Berseker.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_DAMAGE_REFLECT) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_DAMAGE_REFLECT));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Damage Reflection.", lpTarget->Name);
		}
		break;
		case CLASS_FE:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DEFENSE) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DEFENSE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Greater Defense.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DAMAGE) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DAMAGE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Greater Damage.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_INFINITY_ARROW) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_INFINITY_ARROW));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Infinity Arrow.", lpTarget->Name);
		}
		break;
		case CLASS_RF:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_FITNESS) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_FITNESS));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Fitness.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DEFENSE_SUCCESS_RATE) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_DEFENSE_SUCCESS_RATE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Increase Block.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_GREATER_IGNORE_DEFENSE_RATE) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_GREATER_IGNORE_DEFENSE_RATE));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Ignore Defense.", lpTarget->Name);
		}
		break;
		case CLASS_GL:
		{
			if (gSkillManager.GetSkill(lpTarget, SKILL_WRATH) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_WRATH));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Wrath.", lpTarget->Name);

			if (gSkillManager.GetSkill(lpTarget, SKILL_OBSIDIAN) != 0)
				gSkillManager.SkillGreaterCriticalDamage(lpTarget->Index, lpTarget->Index, gSkillManager.GetSkill(lpTarget, SKILL_OBSIDIAN));
			else
				GCMessagePopupSend(lpObj, "Personagem %s nao tem skill Obsidian.", lpTarget->Name);
		}
		break;
		}
	}
	else if (Type == 1) //put defense
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_MANA_SHIELD), 999999999, gServerInfo.m_ManaShieldMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_LIFE), 999999999, gServerInfo.m_GreaterLifeMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_DEFENSE), 999999999, gServerInfo.m_GreaterDefenseMax, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_DAMAGE_REFLECT), 999999999, gServerInfo.m_ReflectDamageMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_FITNESS), 999999999, gServerInfo.m_FitnessMax, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_DEFENSE_SUCCESS_RATE), 999999999, gServerInfo.m_GreaterIgnoreDefenseMax, 0, 0, 0);
	}
	else if (Type == 2) //put attack
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_CRITICAL_DAMAGE), 999999999, gServerInfo.m_GreaterCriticalDamageMax, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_DAMAGE), 999999999, gServerInfo.m_GreaterDamageMax, 0, 0, 0);

		if (lpTarget->Class == CLASS_DW)
		{
			int v1 = (((lpTarget->Energy + lpTarget->AddEnergy) / gServerInfo.m_DWMagicDamageMinConstA) * gServerInfo.m_MagicCircleConstA) / 100.0f;
			int v2 = (((lpTarget->Energy + lpTarget->AddEnergy) / gServerInfo.m_DWMagicDamageMaxConstA) * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_IMPROVED)) / 100.0f;
			int v3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_ENHANCED);
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_MAGIC_CIRCLE), 999999999, gServerInfo.m_MagicCircleConstA, v1, v2, v3);
		}

		if (lpTarget->Class == CLASS_FE)
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_INFINITY_ARROW), 999999999, 0, 0, 0, 0);

		if (lpTarget->Class == CLASS_SU)
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_SWORD_POWER), 999999999, gServerInfo.m_SwordPowerMaxRate, gServerInfo.m_SwordPowerMaxRate, 0, 0);

		if (lpTarget->Class == CLASS_RF)
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_IGNORE_DEFENSE_RATE), 999999999, gServerInfo.m_GreaterIgnoreDefenseMax, 0, 0, 0);

		if (lpTarget->Class == CLASS_GL)
		{
			int value = lpTarget->Defense * gServerInfo.m_WrathCutDefConst / 100;
			int value1 = lpTarget->PhysiDamageMaxRight * gServerInfo.m_WrathConst / 100;
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_WRATH), 999999999, value, value1, 0, 0);
			gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_OBSIDIAN), 999999999, gServerInfo.m_ObsidianMax, 0, 0, 0);
		}
	}
	else if (Type == 3) //guga pediu
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_MANA_SHIELD), 999999999, gServerInfo.m_ManaShieldMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_GREATER_LIFE), 999999999, gServerInfo.m_GreaterLifeMaxRate, 0, 0, 0);
	}
	else if (Type == 4) //debuffs
	{
		//Summoner
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_LESSER_DAMAGE), 999999999, gServerInfo.m_LesserDamageMaxRate, 0, 0, 0);
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(SKILL_LESSER_DEFENSE), 999999999, gServerInfo.m_LesserDefenseMaxRate, 0, 0, 0);
	}
	else
	{
		gEffectManager.ClearAllEffect(lpTarget);
	}

	gObjectManager.CharacterCalcAttribute(lpTarget->Index);
}

void CCommandManager::CommandSetTarget(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpAttacker = gObjFind(name);

	if (lpAttacker == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	this->GetString(arg, name, sizeof(name), 1);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpAttacker->TargetPvPIndex = lpTarget->Index;

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Setado alvo PvP: %s (%d) -> %s (%d)", lpAttacker->Name, lpAttacker->Index, lpTarget->Name, lpTarget->Index);
}

void CCommandManager::CommandShowStats(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "[%s]: [F: %d] [A: %d] [V: %d] [E: %d] [C: %d]", lpTarget->Name, lpTarget->Strength, lpTarget->Dexterity, lpTarget->Vitality, lpTarget->Energy, lpTarget->Leadership);
}

void CCommandManager::CommandSetStats(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int str = this->GetNumber(arg, 1);
	int dex = this->GetNumber(arg, 2);
	int vit = this->GetNumber(arg, 3);
	int eng = this->GetNumber(arg, 4);
	int com = this->GetNumber(arg, 5, 0);

	lpTarget->Strength = str;
	lpTarget->Dexterity = dex;
	lpTarget->Vitality = vit;
	lpTarget->Energy = eng;
	lpTarget->Leadership = com;

	gEffectManager.ClearAllEffect(lpTarget);

	gObjectManager.CharacterCalcAttribute(lpTarget->Index);

	GCNewCharacterInfoSend(lpObj);

	SXGetCharacterInfo(lpObj->Index);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Stats of %s changed to %d %d %d %d %d", lpTarget->Name, str, dex, vit, eng, com);
	gNotice.GCNoticeSend(lpTarget->Index, 1, 0, 0, 0, 0, 0, "%s changed your stats to %d %d %d %d %d", lpObj->Name, str, dex, vit, eng, com);
}

void CCommandManager::CommandSetAutoPotion(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	if (!lpTarget->AutoPotionBot)
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Auto potion started to %s", lpTarget->Name);
	else
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Auto potion stopped to %s", lpTarget->Name);

	lpTarget->AutoPotionBot = !lpTarget->AutoPotionBot;
}

void CCommandManager::CommandWatch(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	if (strcmp(name, "#all") == 0)
	{
		WatchTargetIndex = (WatchTargetIndex == 99999) ? -1 : 99999;
		return;
	}

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	LogAdd(LOG_BLUE, "[GM][%s] is watching user %s", lpObj->Name, lpTarget->Name);

	if (WatchTargetIndex == lpTarget->Index)
	{
		WatchTargetIndex = -1;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "%s is not being watched anymore", lpTarget->Name);
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "%s is being watched", lpTarget->Name);
		WatchTargetIndex = lpTarget->Index;
	}
}

void CCommandManager::CommandChatBlock(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int time = this->GetNumber(arg, 1, 1);

	lpTarget->ChatLimitTime = time;

	GCMessagePopupSend(lpObj, gMessage.GetMessage(27), lpTarget->Name, time);
}

void CCommandManager::CommandGhost(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	if (strlen(name) < 4 || strcmp(name, lpObj->Name) == 0)
	{
		lpObj->Socket = INVALID_SOCKET;
		lpObj->isGhost = true;
		closesocket(lpObj->PerSocketContext->Socket);
		return;
	}

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	lpTarget->Socket = INVALID_SOCKET;
	lpTarget->isGhost = true;
	closesocket(lpTarget->PerSocketContext->Socket);
}

void CCommandManager::CommandBringParty(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	if (lpTarget->PartyNumber < 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(28), name);
		return;
	}

	for (int i = OBJECT_START_USER; i < MAX_OBJECT; i++)
	{
		if (gObj[i].Connected == OBJECT_ONLINE && gObj[i].Type == OBJECT_USER && gObj[i].PartyNumber == lpTarget->PartyNumber)
		{
			int x_inc = (rand() % 3);
			int y_inc = (rand() % 3);

			if (rand() % 2 == 0) { x_inc *= -1; }
			if (rand() % 2 == 0) { y_inc *= -1; }

			LogAdd(LOG_BLUE, "[GM][%s] Brought user %s to him by Party Move [%d %d]", lpObj->Name, lpTarget->Name, x_inc, y_inc);

			gObjTeleport(gObj[i].Index, lpObj->Map, lpObj->X + x_inc, lpObj->Y + y_inc);
		}
	}
}

void CCommandManager::CommandAutoDt(LPOBJ lpObj, char* arg)
{
	if (gServerInfo.m_CommandAddPointAutoEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}

	int str = this->GetNumber(arg, 0, 0);
	int agi = this->GetNumber(arg, 1, 0);
	int vit = this->GetNumber(arg, 2, 0);
	int ene = this->GetNumber(arg, 3, 0);

	lpObj->AutoDt[0] = str;
	lpObj->AutoDt[1] = agi;
	lpObj->AutoDt[2] = vit;
	lpObj->AutoDt[3] = ene;
	lpObj->AutoDt[4] = 0;

	if (lpObj->Class == CLASS_DL)
	{
		int com = this->GetNumber(arg, 4, 0);
		lpObj->AutoDt[4] = com;
		GCMessagePopupSend(lpObj, "%s\n%s: %d\n%s: %d\n%s: %d\n%s: %d\n%s: %d",
			gMessage.GetMessage(26),
			gMessage.GetMessage(21), str,
			gMessage.GetMessage(22), agi,
			gMessage.GetMessage(23), vit,
			gMessage.GetMessage(24), ene,
			gMessage.GetMessage(25), com);
	}
	else
	{
		GCMessagePopupSend(lpObj, "%s\n%s: %d\n%s: %d\n%s: %d\n%s: %d",
			gMessage.GetMessage(26),
			gMessage.GetMessage(21), str,
			gMessage.GetMessage(22), agi,
			gMessage.GetMessage(23), vit,
			gMessage.GetMessage(24), ene);
	}
}

void CCommandManager::CommandSetPk(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	LPOBJ lpTarget = gObjFind(name);

	if (lpTarget == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int pkLevel = (lpTarget->PKLevel >= 5) ? 3 : 6;

	lpTarget->PKLevel = pkLevel;
	GCPKLevelSend(lpTarget->Index, pkLevel);
}

void CCommandManager::CommandMUFC(LPOBJ lpObj, char* arg)
{
	int rounds = 1;
	int type = this->GetNumber(arg, 0, 0);

	switch (type)
	{
	case 0:
	{
		rounds = this->GetNumber(arg, 1, 1);
		gMUFC.FightStart(lpObj, rounds);
	}
	break;

	case 1:
	case 2:
	{
		char name[11] = { 0 };
		this->GetString(arg, name, sizeof(name), 1);

		LPOBJ lpTarget = gObjFind(name);

		if (lpTarget == 0)
		{
			GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
			return;
		}

		gMUFC.PrepareFighter(type, lpTarget, lpObj);
	}
	break;

	case 3:
		gMUFC.ResetFight(lpObj);
		break;
	}
}

void CCommandManager::CommandSiege(LPOBJ lpObj, char* arg)
{
#if(GAMESERVER_TYPE==1)
	char cmd[16] = { 0 };
	this->GetString(arg, cmd, sizeof(cmd), 0);

	if (strcmp(cmd, "start") == 0)
		gCastleSiege.SetState(CASTLESIEGE_STATE_STARTSIEGE, TRUE);
#endif
}

void CCommandManager::CommandFillPotion(LPOBJ lpObj)
{
	if (gServerInfo.m_CommandFillPotionEnable[lpObj->AccountLevel] == 0)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(12));
		return;
	}	

	if (lpObj->Money < gServerInfo.m_CommandFillPotionMoney[lpObj->AccountLevel])
	{
		GCMessagePopupSend(lpObj, "%s %s", gMessage.GetMessage(35), gMessage.GetMessage(32));
		return;
	}

	CItem item;
	BYTE btItemPos;
	int PotionCount = 0;
	int totalCost = 0;
	int item_type = GET_ITEM(14, 3);
	item.m_Level = 0;
	item.m_Durability = 255;

	item.Convert(item_type, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF);

	for (int i = INVENTORY_WEAR_SIZE; i < INVENTORY_MAIN_SIZE; i++)
	{
		if (gServerInfo.m_CommandFillPotionMaxAmount[lpObj->AccountLevel] > 0 && PotionCount > gServerInfo.m_CommandFillPotionMaxAmount[lpObj->AccountLevel])
		{
			break;
		}

		//if (gItemManager.InventoryInsertItemStack(lpObj, &item) == 0)
		//{
			btItemPos = gItemManager.InventoryInsertItem(lpObj->Index, item);

			if (btItemPos == 0xFF)
				continue;

			PotionCount++;
		//}
	}

	gItemManager.GCItemListSend(lpObj->Index);

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(36));
}

void CCommandManager::CommandMoveGuild(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	GUILD_INFO_STRUCT* lpGuildInfo = gGuildClass.SearchGuild(name);

	if (lpGuildInfo == NULL)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	int M = GetNumber(arg, 1, lpObj->Map);
	int X = GetNumber(arg, 2, lpObj->X);
	int Y = GetNumber(arg, 3, lpObj->Y);

	for (int i = 0; i < MAX_GUILD_USER; i++)
	{
		if (lpGuildInfo->Index[i] >= 0)
		{
			gObjTeleport(lpGuildInfo->Index[i], M, X++, Y);
		}
	}
}

void CCommandManager::CommandGuildDisconnect(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };

	this->GetString(arg, name, sizeof(name), 0);

	GUILD_INFO_STRUCT* lpGuildInfo = gGuildClass.SearchGuild(name);

	if (lpGuildInfo == NULL)
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(97), name);
		return;
	}

	for (int i = 0; i < MAX_GUILD_USER; i++)
	{
		if (lpGuildInfo->Index[i] >= 0)
		{
			gObjUserKill(lpGuildInfo->Index[i]);
		}
	}
}

void CCommandManager::CommandPin(LPOBJ lpObj, char* arg)
{
	char pin[11] = { 0 };

	this->GetString(arg, pin, sizeof(pin), 0);

	if (gObjCheckPersonalCode(lpObj->Index, pin))
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(38));
	}
	else
	{
		GCMessagePopupSend(lpObj, gMessage.GetMessage(39));
	}
}