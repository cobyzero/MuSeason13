#include "stdafx.h"
#include "MUFC.h"
#include "MemScript.h"
#include "ServerInfo.h"
#include "Notice.h"
#include "Util.h"
#include "EffectManager.h"

CMUFC gMUFC;

CMUFC::CMUFC()
{
	gMUFC.Status = MUFC_IDLE;
}

CMUFC::~CMUFC()
{
	ZeroMemory(gMUFC.FighterName,sizeof(gMUFC.FighterName));
	gMUFC.FighterID[0] = 0;
	gMUFC.FighterID[1] = 0;
	gMUFC.FighterVictories[0] = 0;
	gMUFC.FighterVictories[1] = 0;
	gMUFC.Rounds = 1;
}

void CMUFC::FightStart(LPOBJ lpObj, int rounds)
{
	if(gMUFC.FighterName[0] != NULL && gMUFC.FighterName[1] != NULL)
	{
		if(strlen(gMUFC.FighterName[0]) > 3 && strlen(gMUFC.FighterName[1]) > 3)
		{
			gMUFC.SendNotice("[MUFC]  INICIANDO LUTA  [MUFC]");
			gMUFC.SendNotice("[MUFC]  %s  vs  %s  [MUFC]", gMUFC.FighterName[0], gMUFC.FighterName[1]);
			
			gMUFC.Status = MUFC_PREPARED;
			gMUFC.StartCounter	= gServerInfo.MUFC_TimerToStart;

			int wtf = 0;

			gMUFC.Rounds = rounds;

			wtf = 1;

			gMUFC.FighterVictories[0] = 0;
			gMUFC.FighterVictories[1] = 0;

			wtf = 2;

			gMUFC.ReSetTimer = 0;

			return;
		}
	}

	GCMessagePopupSend(lpObj, "Falha ao iniciar o MUFC.\nCertifique-se de ter selecionado os dois lutadores.");
}

void CMUFC::ResetFight(LPOBJ lpObj)
{
	if(gMUFC.Status != MUFC_STARTED)
	{
		GCMessagePopupSend(lpObj, "Luta ainda iniciada não pode reiniciar.");
		return;
	}
	
	if ( &gObj[gMUFC.FighterID[0]] == NULL || &gObj[gMUFC.FighterID[1]] == NULL )
	{
		GCMessagePopupSend(lpObj, "Personagem(ns) não encontrado(s). Inicie a luta novamente");
		return;
	}

	gMUFC.Status = MUFC_PREPARED;

	LPOBJ Fighter1Obj = &gObj[gMUFC.FighterID[0]];
	LPOBJ Fighter2Obj = &gObj[gMUFC.FighterID[1]];

	gMUFC.PrepareFighter(1,Fighter1Obj,lpObj);
	gMUFC.PrepareFighter(2,Fighter2Obj,lpObj);
}

void CMUFC::FightRun()
{
	LPOBJ Fighter1Obj = &gObj[gMUFC.FighterID[0]];
	LPOBJ Fighter2Obj = &gObj[gMUFC.FighterID[1]];

	gMUFC.Status = MUFC_STARTED;

	gMUFC.SendNotice("[MUFC] !!!    L U T E M    !!! [MUFC]");

	gEffectManager.DelEffect(Fighter1Obj,EFFECT_ICE_ARROW);
	gEffectManager.DelEffect(Fighter2Obj, EFFECT_ICE_ARROW);

	gMUFC.FightTimer = 0;
	gMUFC.FightDamage = 0;
}

void CMUFC::FighterKilled(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	gMUFC.SendNotice("%s bateu %d com %d hits", gMUFC.FighterName[0], gMUFC.FighterDamage[0], gMUFC.FighterHits[0]);
	gMUFC.SendNotice("%s bateu %d com %d hits", gMUFC.FighterName[1], gMUFC.FighterDamage[1], gMUFC.FighterHits[1]);
	gMUFC.SendNotice("[MUFC]    Vitória de %s    [MUFC]",lpObj->Name);

	int fighterId = lpObj->MUFCFighter - 1;

	gMUFC.FighterVictories[fighterId]++;

	if (gMUFC.Rounds > 1)
	{
		gMUFC.SendNotice("[%s] %d x %d [%s]", gMUFC.FighterName[0], gMUFC.FighterVictories[0], gMUFC.FighterVictories[1], gMUFC.FighterName[1]);
	}		

	if(gMUFC.FighterVictories[fighterId] < gMUFC.Rounds)
	{
		gMUFC.Status = MUFC_MOVED;
		gMUFC.SendNotice("[MUFC] Próximo round em %d segundos. [MUFC]",gServerInfo.MUFC_TimerRound);
		gMUFC.ReSetTimer = gServerInfo.MUFC_TimerRound;
	}
	else
	{
		GCFireworksSend(lpObj, lpObj->X, lpObj->Y);

		lpTargetObj->MUFCFighter = 0;
		lpObj->MUFCFighter = 0;
		LogAdd(LOG_DEBUG, "Zerou saporra");

		lpTargetObj->PKLevel = 3;
		lpObj->PKLevel = 3;
		GCPKLevelSend(lpTargetObj->Index, 3);
		GCPKLevelSend(lpObj->Index, 3);

		ZeroMemory(gMUFC.FighterName,sizeof(gMUFC.FighterName));

		gMUFC.FighterID[0] = 0;
		gMUFC.FighterID[1] = 0;

		gMUFC.Status = MUFC_FINISHED;
		gMUFC.FightTimer = 0;
		gMUFC.FightDamage = 0;

		gObjTeleport(lpTargetObj->Index, 0, 125, 125);
		gObjTeleport(lpObj->Index, 0, 126, 126);
	}
}

void CMUFC::PrepareFighter(int number, LPOBJ lpTargetObj, LPOBJ lpObj)
{
	int x = gServerInfo.MUFC_1_X;
	int y = gServerInfo.MUFC_1_Y;

	if(number == 2)
	{
		x = gServerInfo.MUFC_2_X;
		y = gServerInfo.MUFC_2_Y;
	}
	
	gObjTeleport(lpTargetObj->Index, gServerInfo.MUFC_Map, x, y);

	int wtf = 0;
	wtf = 9999;

	LogAdd(LOG_BLUE, "MUFC %d %s", number, lpTargetObj->Name);
	lpTargetObj->MUFCFighter = number;
	LogAdd(LOG_BLUE, "MUFC %d %s", lpTargetObj->MUFCFighter, lpTargetObj->Name);

	wtf = 1;

	lpTargetObj->PKLevel = 6;
	GCPKLevelSend(lpTargetObj->Index, 6);

	gEffectManager.ClearAllEffect(lpTargetObj);
	gEffectManager.AddEffect(lpTargetObj, 0, EFFECT_ICE_ARROW, 9999, 0, 0, 0, 0);

	lpTargetObj->PathCount = 0;
	lpTargetObj->PathStartEnd = 0;
	gObjSetPosition(lpTargetObj->Index, x, y);

	strcpy(gMUFC.FighterName[number-1],lpTargetObj->Name);
	gMUFC.FighterName[number-1][10] = '\0';

	gMUFC.FighterID[number-1] = lpTargetObj->Index;

	gMUFC.FighterDamage[number-1] = 0;
	gMUFC.FighterHits[number-1] = 0;
	
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, "Lutador %s está com %d em Vitalidade",lpTargetObj->Name,lpTargetObj->Vitality);

	gMUFC.Status = MUFC_MOVED;
}

void CMUFC::SetNewRound()
{
	LPOBJ lpObj = &gObj[gMUFC.FighterID[0]];
	LPOBJ lpTargetObj = &gObj[gMUFC.FighterID[1]];
	
	gObjClearViewport(lpObj);
	gObjClearViewport(lpTargetObj);

	gObjTeleport(lpObj->Index, gServerInfo.MUFC_Map, gServerInfo.MUFC_1_X, gServerInfo.MUFC_1_Y);
	gObjTeleport(lpTargetObj->Index, gServerInfo.MUFC_Map, gServerInfo.MUFC_2_X, gServerInfo.MUFC_2_Y);

	gEffectManager.ClearAllEffect(lpTargetObj);
	gEffectManager.ClearAllEffect(lpObj);

	gEffectManager.AddEffect(lpObj, 0, EFFECT_ICE_ARROW, 9999, 0, 0, 0, 0);
	lpObj->PathCount = 0;
	lpObj->PathStartEnd = 0;
	gObjSetPosition(lpObj->Index, gServerInfo.MUFC_1_X, gServerInfo.MUFC_1_Y);

	gEffectManager.AddEffect(lpTargetObj, 0, EFFECT_ICE_ARROW, 9999, 0, 0, 0, 0);
	lpTargetObj->PathCount = 0;
	lpTargetObj->PathStartEnd = 0;
	gObjSetPosition(lpTargetObj->Index, gServerInfo.MUFC_2_X, gServerInfo.MUFC_2_Y);

	gObjViewportListProtocolCreate(lpObj);
	gObjViewportListProtocolCreate(lpTargetObj);		

	gMUFC.FighterDamage[0] = 0;
	gMUFC.FighterDamage[1] = 0;

	gMUFC.FighterHits[0] = 0;
	gMUFC.FighterHits[1] = 0;

	gMUFC.Status = MUFC_PREPARED;
	gMUFC.StartCounter	= gServerInfo.MUFC_TimerToStart + 5;
}

void CMUFC::Load(char* filename)
{
	for( int n = 0; n < 32; n++ )
	{
		gMUFC.FightDamageRate[n].Timer		= -1;
		gMUFC.FightDamageRate[n].DamageRate	= -1;
	}

	CMemScript* lpScript = new(std::nothrow) CMemScript;

	if(lpScript == NULL)
	{
		return;
	}

	if(lpScript->SetBuffer(filename) == 0)
	{
		delete lpScript;
		return;
	}

	int count = 0;

	try
	{
		while(true)
		{
			if( lpScript->GetToken() == 2 )
			{
				break;
			}

			gMUFC.FightDamageRate[count].Timer		= lpScript->GetNumber();
			gMUFC.FightDamageRate[count].DamageRate = lpScript->GetAsNumber();

			if(count >= 31)
				break;

			count++;
		}
	}
	catch(...)
	{
		delete lpScript;
		return;
	}

	LogAdd(LOG_BLACK, "[MUFC] MUFCDamageIncrease is loaded");

	delete lpScript;
}

void CMUFC::SendNotice(char* message, ...)
{
	char buff[256] = { 0 };

	va_list arg;
	va_start(arg, message);
	vsprintf_s(buff, message, arg);
	va_end(arg);

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
		if (gObjIsConnectedGP(n) != 0)
			if(gObj[n].Authority == 32 || gObj[n].MUFCFighter > 0)
				gNotice.GCNoticeSend(n, 0, 0, 0, 0, 0, 0, buff);
}
