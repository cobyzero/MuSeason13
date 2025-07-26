#include "StdAfx.h"
#include "CheatGuard.h"
#include "user.h"
#include "GameMain.h"
#include "MemScript.h"
#include "ServerInfo.h"
#include "SpeedTable.h"
#include "Util.h"
#include "Log.h"
#include "ObjectManager.h"

CheatGuard gCheatGuard;

/*DWORD AccumulatedAttackCount;
DWORD AccumulatedAttackTimer;*/

CheatGuard::CheatGuard()
{
}

CheatGuard::~CheatGuard()
{
}

void CheatGuard::HardIdRecv(int aIndex, HARDWARE_ID_RECV * pResult)
{
	HARDWARE_ID_SEND_DB pMsg;
	pMsg.header.set(0xD7, 10, sizeof(HARDWARE_ID_SEND_DB));
	strcpy(pMsg.Account,gObj[aIndex].Account);
	pMsg.Account[10] = '\0';
	pResult->HarwareId[44] = '\0';
	strcpy(pMsg.HardwareId,pResult->HarwareId);
	pMsg.HardwareId[44] = '\0';
	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

/*bool CheatGuard::CheckSpeedHack(int aIndex, int bIndex, int skill, char* callingFunction)
{
	if (gServerInfo.SpeedGenerateConfig == 1 && strcmp(gServerInfo.CharacterToTest, gObj[aIndex].Name) == 0)
	{
		this->GenerateConfig(aIndex, bIndex, skill);
		return true;
	}

	if (gServerInfo.AntiSpeedEnable == 0 || gObj[aIndex].Type != OBJECT_USER || gObj[aIndex].AttackCustomOffline > 0 || gObj[aIndex].isGhost)
		return true;

	if (!OBJECT_RANGE(aIndex) || gObj[aIndex].Connected == OBJECT_EMPTY)
	{
		LogAdd(LOG_RED, "Speed Hit Hack invalid return. [Name: %s] [Connected: %d] [Skill: %d] [Function: %s]", gObj[aIndex].Name, gObj[aIndex].Connected, skill, callingFunction);
		return false;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->DisconnectedBySpeedHit)
	{
		//disconnect
		return 0;
	}

	LPOBJ lpTarget = 0;

	if (OBJECT_RANGE(bIndex) && gObj[bIndex].Connected > OBJECT_EMPTY)
	{
		lpTarget = &gObj[bIndex];
	}

	int n;

	for (n = 0; n < gSpeedTable.m_count[lpObj->Class]; n++)
	{
		if (gSpeedTable.SpeedTable[lpObj->Class][n].Skill > -1 && skill != gSpeedTable.SpeedTable[lpObj->Class][n].Skill)
		{
			//LogAddC(2,"[DEBUG] continue Skill 1: %d   2: %d",gSpeedTable.m_SpeedTableInfoV2[n].Skill, skill);
			continue;
		}

		if (lpObj->PhysiSpeed < gSpeedTable.SpeedTable[lpObj->Class][n].MinSpeed || lpObj->PhysiSpeed > gSpeedTable.SpeedTable[lpObj->Class][n].MaxSpeed)
		{
			//LogAddC(2,"[DEBUG] continue AttackSpeed");
			continue;
		}

		if (gSpeedTable.SpeedTable[lpObj->Class][n].PvP == 0 && lpTarget > 0 && lpTarget->Type == OBJECT_USER)
		{
			//LogAddC(2,"[DEBUG] continue PvP");
			continue;
		}

		if (gSpeedTable.SpeedTable[lpObj->Class][n].PvM == 0 && lpTarget > 0 && lpTarget->Type == OBJECT_MONSTER)
		{
			//LogAddC(2,"[DEBUG] continue PvM");
			continue;
		}

		//IF - ainda não esgotou o tempo
		if ((GetTickCount() - lpObj->AttackTimer) < gSpeedTable.SpeedTable[lpObj->Class][n].Timer)
		{
			//IF - tem mais hits do que deveria
			if (lpObj->AttackCounter >= gSpeedTable.SpeedTable[lpObj->Class][n].Hits)
			{
				//Incrementa contagem de floods consecutivos
				lpObj->SpeedFloodCounter++;

				//Aplica o CoolDown se configurado
				if (gSpeedTable.SpeedTable[lpObj->Class][n].CoolDownHits > 0)
					lpObj->SpeedBanCounter += gSpeedTable.SpeedTable[lpObj->Class][n].CoolDownHits;

				//Gera logs, se configurado para tal
				if (gSpeedTable.SpeedTable[lpObj->Class][n].Log == 1 && lpObj->SpeedFloodCounter > 1 && lpObj->TargetPvPIndex == -1)
				{
					gLog.Output(SPEED_LOG, "[%s][%s] Accumulated Occurrences: [%d] | Hits: [%d]/[%d] [Class: %d] [Skill: %d] [Speed: %d] [Function: %s]",
						lpObj->Account, lpObj->Name, lpObj->SpeedFloodCounter, lpObj->AttackCounter, gSpeedTable.SpeedTable[lpObj->Class][n].Hits, lpObj->Class, skill, lpObj->PhysiSpeed, callingFunction);
				}

				//Verifica o timer em comparação com o limite
				if ((GetTickCount() - lpObj->SpeedFloodTimer) >= gSpeedTable.SpeedTable[lpObj->Class][n].ThresholdTimer && lpObj->TargetPvPIndex == -1)
				{
					//Verifica se está configurado para dar DC
					if (gSpeedTable.SpeedTable[lpObj->Class][n].DC == 1)
					{
						//Verifica se o numero de ocorrencias passou o limite configurado
						if (gSpeedTable.SpeedTable[lpObj->Class][n].ThresholdOcurrences > 0 && lpObj->SpeedFloodCounter >= gSpeedTable.SpeedTable[lpObj->Class][n].ThresholdOcurrences)
						{
							//Setar o status desconectado para parar as verificações
							lpObj->DisconnectedBySpeedHit = TRUE;

							//Mostrar log de desconexão
							//LogAddC(7,"[%s][%s] Disconnected by Speed Hit Hack - Occurrences: [%d] | Hits: [%d]/[%d] [Class: %d] [Skill: %d] [Speed: %d]",
							//	lpObj->AccountID,lpObj->Name,lpObj->SpeedFloodCounter,lpObj->AttackCounter,gSpeedTable.SpeedTable[lpObj->Class][n].Hits,lpObj->Class,skill,lpObj->m_AttackSpeed);

							gLog.Output(SPEED_LOG, "[%s][%s] Disconnected - Occurrences: [%d] | Hits: [%d]/[%d] [Class: %d] [Skill: %d] [Speed: %d]",
								lpObj->Account, lpObj->Name, lpObj->SpeedFloodCounter, lpObj->AttackCounter, gSpeedTable.SpeedTable[lpObj->Class][n].Hits, lpObj->Class, skill, lpObj->PhysiSpeed);

							//Envia a mensagem de desconexão por speed
							TNotice pNotice0(0);
							TNotice pNotice1(1);
							pNotice0.SendToUser(aIndex,"Desconectado por High Speed Hit"); //Message txt
							pNotice1.SendToUser(aIndex,"Desconectado por High Speed Hit"); //Message txt

							//Desconecta o meliante
							//g_CheatGuard.Disconnect(aIndex);
						}
					}

					//Reseta o timer e contador de ocorrencias
					lpObj->SpeedFloodTimer = GetTickCount();

					//Reset contagem de Floods
					lpObj->SpeedFloodCounter = 0;
				}

				return 0;
			}
			else
			{
				//Decrementa contagem de floods consecutivos
				if (lpObj->SpeedFloodCounter > 0)
					lpObj->SpeedFloodCounter--;
			}
		}
		else //IF - esgotou o tempo da velocidade atual
		{
			lpObj->AttackTimer = GetTickCount();
			lpObj->AttackCounter = 0;
		}

		if (lpObj->AttackCounter >= gSpeedTable.SpeedTable[lpObj->Class][n].Hits)
		{
			lpObj->AttackCounter++;
			return 0;
		}

		break;
	}

	if (lpObj->SpeedBanCounter > 0)
	{
		gLog.Output(SPEED_LOG, "[%s][%s] Hit ignorado por CoolDown [%d]", lpObj->Account, lpObj->Name, lpObj->SpeedBanCounter);
		lpObj->SpeedBanCounter--;
		return 0;
	}

	lpObj->AttackCounter++;

	return 1;
}*/

/*void CheatGuard::GenerateConfig(int aIndex, int bIndex, int skill)
{
	if (!OBJECT_RANGE(aIndex) || gObj[aIndex].Connected == OBJECT_EMPTY)
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJECT_USER)
		return;

	LPOBJ lpTarget = 0;

	if (OBJECT_RANGE(bIndex) && gObj[bIndex].Connected > OBJECT_EMPTY)
		lpTarget = &gObj[bIndex];

	BOOL PvP, PvM;

	if (lpTarget == 0) { PvP = TRUE; PvM = TRUE; }
	else
	{
		if (lpTarget->Type == OBJECT_USER) { PvP = TRUE;  PvM = FALSE; }
		if (lpTarget->Type == OBJECT_MONSTER) { PvP = FALSE; PvM = TRUE; }
	}

	AccumulatedAttackCount++;

	if ((GetTickCount() - AccumulatedAttackTimer) >= gServerInfo.GenerateConfigTimer)
	{
		LogAdd(LOG_BLUE, "Class: %d | Skill: %d | PvP: %d | PvM: %d | Speed: %d | Timer: %dms | Hits: %d | AVG: %dms ", lpObj->Class, skill, PvP, PvM, lpObj->PhysiSpeed, (GetTickCount() - AccumulatedAttackTimer), AccumulatedAttackCount, ((GetTickCount() - AccumulatedAttackTimer) / AccumulatedAttackCount));

		AccumulatedAttackCount = 0;
		AccumulatedAttackTimer = GetTickCount();

		if (lpObj->Dexterity < (65536 - gServerInfo.AgilityAutoIncrement))
		{
			gObjectManager.CharacterLevelUpPointAdd(lpObj, 1, gServerInfo.AgilityAutoIncrement);
		}
	}
}*/

void CheatGuard::PingStack(int aIndex, int ping)
{
	if (ping >= 5 && ping < 1000)
	{
		int pingSum = 0;
		int pingCount = 1;

		for (int i = 0; i < 9; i++)
		{
			if (gObj[aIndex].PingLast[i + 1] > 0)
			{
				gObj[aIndex].PingLast[i] = gObj[aIndex].PingLast[i + 1];
				pingSum += gObj[aIndex].PingLast[i];
				pingCount++;
			}
		}

		gObj[aIndex].PingLast[9] = ping;
		pingSum += ping;

		gObj[aIndex].Ping = (int)(pingSum / pingCount);
	}
}

void CheatGuard::GCSendPing(int aIndex)
{
	PMSG_PING pMsg;
	pMsg.h.set(0x71, sizeof(pMsg));
	gObj[aIndex].PingSentTick = GetTickCount();
	pMsg.ping = gObj[aIndex].Ping;
	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
}

void CheatGuard::GCPingSendRecv(PMSG_PING* aRecv, int aIndex)
{
	int LastPing = (GetTickCount() - gObj[aIndex].PingSentTick);
	this->PingStack(aIndex, LastPing);
}