#include "stdafx.h"
#include "User.h"
#include "MonsterSkillManager.h"
#include "CNixiesLakeBoss.h"
#include "CNixiesLakeBossZone.h"
#include "Util.h"
#include "SkillManager.h"
#include "Viewport.h"
#include "Attack.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "Message.h"
#include "Map.h"
CNixiesLakeBoss g_NixiesLakeBoss;

CNixiesLakeBoss::CNixiesLakeBoss()
{
	this->m_dwNixAttackDelay = 0;
	this->m_bNixFirstBelowLife90Per = 0;
	this->m_bNixFirstBelowLife30Per = 0;
	this->m_bNixFirstBelowLife10Per = 0;
	this->m_bUsedDevilIceLife80Per = 0;
	this->m_bUsedDevilIceLife50Per = 0;
	this->m_bUsedDevilIceLife5Per = 0;
	this->m_bNixImmune = 0;
	this->m_dwIceRainDelay = 0;
	this->m_dwRefillLifeDelay = 0;
	this->m_stIceBreathInfo.Clear();
	this->m_stIceBoomInfo.Clear();
	this->m_stKnowAbsorbInfo.Clear();
	this->m_stDevilIceInfo.Clear();

	return;
}


CNixiesLakeBoss::~CNixiesLakeBoss()
{
	return;
}


//----- (00F3F600) --------------------------------------------------------
void  CNixiesLakeBoss::InitNixStatus()
{

	this->m_bNixFirstBelowLife90Per = 0;
	this->m_bNixFirstBelowLife30Per = 0;
	this->m_bNixFirstBelowLife10Per = 0;
	this->m_bUsedDevilIceLife80Per = 0;
	this->m_bUsedDevilIceLife50Per = 0;
	this->m_bUsedDevilIceLife5Per = 0;
	this->m_bNixImmune = 0;
	this->m_dwIceRainDelay = 0;
	this->m_dwNixAttackDelay = 0;
	this->m_dwRefillLifeDelay = 0;
	this->m_stIceBreathInfo.Clear();
	this->m_stIceBoomInfo.Clear();
	this->m_stKnowAbsorbInfo.Clear();
	this->m_stDevilIceInfo.Clear();

}

//----- (00F3F6C0) --------------------------------------------------------
//尼克斯使用技能
BYTE __thiscall CNixiesLakeBoss::NixiesBossSkillRun( LPOBJ lpObj)
{
	char result; // al@8

	if (this->m_bNixImmune != 1 || GetTickCount() < this->m_stKnowAbsorbInfo.m_dwKnowledgeAbsorbTimer)
	{
		if (this->m_stKnowAbsorbInfo.m_dwKnowledgeAbsorbTimer && this->m_stKnowAbsorbInfo.m_nRestrictStoneCount <= 0)
			this->SuccessRemoveRestrictStone(lpObj);
	}
	else
	{
		this->FailRemoveRestrictStone(lpObj);
	}
	if (this->m_stIceBreathInfo.m_bAttack == 1)
	{
		this->IceBreathAttack(lpObj);
		result = 1;
	}
	else if (this->m_stIceBoomInfo.m_bAttack == 1)
	{
		if (this->m_stIceBoomInfo.m_bAttacking == 1)
		{
			result = 1;
		}
		else
		{
			this->IceBoomAttackReady(lpObj);
			result = 1;
		}
	}
	else if (this->m_bNixImmune == 1)
	{
		if (this->m_dwIceRainDelay < GetTickCount())
		{
			CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, 2, 0);
			this->m_dwIceRainDelay = GetTickCount() + 2000;
		}
		result = 1;
	}
	else
	{
		result = 0;
	}
	return result;
}

//----- (00F3F7F0) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::NixAttack(LPOBJ lpObj)
{
	bool bImmune; // [sp+4Eh] [bp-Ah]@16
	bool bDevilice; // [sp+4Fh] [bp-9h]@4
	int nLifePercentageValue; // [sp+50h] [bp-8h]@4



	if (gObjIsConnected(lpObj->TargetNumber) && lpObj->TargetNumber >= 0)
	{
		nLifePercentageValue = (int)this->CheckNixLifeStatus(lpObj);
		bDevilice = 0;
		if (!this->m_stDevilIceInfo.m_bAttacking)
		{
			if (this->m_bUsedDevilIceLife80Per || nLifePercentageValue > 80)
			{
				if (this->m_bUsedDevilIceLife50Per || nLifePercentageValue > 50)
				{
					if (!this->m_bUsedDevilIceLife5Per && nLifePercentageValue <= 5)
					{
						bDevilice = 1;
						this->m_bUsedDevilIceLife5Per = 1;
					}
				}
				else
				{
					bDevilice = 1;
					this->m_bUsedDevilIceLife50Per = 1;
				}
			}
			else
			{
				bDevilice = 1;
				this->m_bUsedDevilIceLife80Per = 1;
			}
		}
		if (bDevilice)
		{
			this->SetDevilIceMagicSquare();
			this->NotifyDevilIceMagicSquarePos();
			CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, 5, 0);
		}
		else
		{
			bImmune = 0;
			if (this->m_bNixFirstBelowLife90Per || nLifePercentageValue > 90)
			{
				if (this->m_bNixFirstBelowLife30Per || nLifePercentageValue > 30)
				{
					if (!this->m_bNixFirstBelowLife10Per && nLifePercentageValue <= 10)
					{
						bImmune = 1;
						this->m_bNixFirstBelowLife10Per = 1;
					}
				}
				else
				{
					bImmune = 1;
					this->m_bNixFirstBelowLife30Per = 1;
				}
			}
			else
			{
				bImmune = 1;
				this->m_bNixFirstBelowLife90Per = 1;
			}
			if ((!bImmune || this->SetKnowledgeAbsorb( lpObj) != 1)
				&& this->m_dwNixAttackDelay <= GetTickCount())
			{
				this->m_dwNixAttackDelay = GetTickCount() + 2000;
				CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, -1, 0);
			}
		}
	}
}

//----- (00F3FA10) --------------------------------------------------------
//检查尼克斯状态
double  CNixiesLakeBoss::CheckNixLifeStatus(LPOBJ lpObj)
{
	//double v2; // st7@1
	//int v3; // [sp+0h] [bp-54h]@0
	//int v4; // [sp+4h] [bp-50h]@0
	//int v5; // [sp+8h] [bp-4Ch]@0

	//v2 = lpObj->Life / lpObj->MaxLife * 100.0;
	//j___ftol2_sse(v3, v4, v5);
	return lpObj->Life / lpObj->MaxLife * 100.0;
}

//----- (00F3FA60) --------------------------------------------------------
BYTE  CNixiesLakeBoss::SummonRestrictStone(LPOBJ lpObj)
{
	char result; // al@3
	int v3; // eax@7
	int i; // [sp+50h] [bp-68h]@4
	int ty2; // [sp+54h] [bp-64h]@7
	int tx2; // [sp+58h] [bp-60h]@7
	bool bIsSuccess; // [sp+67h] [bp-51h]@4
	//int StonePos[4][4] = { -2,0,2,0,-1,1,1,-1,0,2,0,-2,1,1,-1,-1 }; // [sp+68h] [bp-50h]@4
	int StonePos[4][4] = { -2,0,2,0,-1,1,1,-1,0,2,0,-2,1,1,-1,-1 }; // [sp+68h] [bp-50h]@4
	int nRestrictStone2; // [sp+A8h] [bp-10h]@1
	int nRestrictStone1; // [sp+ACh] [bp-Ch]@1
	int aIndex; // [sp+B0h] [bp-8h]@1



	aIndex = lpObj->Index;
	nRestrictStone1 = -1;
	nRestrictStone2 = -1;
	if (aIndex < 0 || lpObj->Class != 746)
	{
		LogAdd(LOG_BLACK, gMessage.GetMessage(506), aIndex);
		return 0;
	}

	bIsSuccess = 0;
	for (i = 0; i < 4; ++i)
	{
		v3 = rand() % 4;
		tx2 = StonePos[v3][2];
		ty2 = StonePos[v3][3];
		nRestrictStone1 = g_NixiesLakeBossZone.AddMonster(
			747,
			StonePos[v3][0] + gObj[aIndex].X,
			StonePos[v3][1] + gObj[aIndex].Y,
			StonePos[v3][0] + gObj[aIndex].X,
			StonePos[v3][1] + gObj[aIndex].Y,
			0);
		if (nRestrictStone1 >= 0)
		{
			nRestrictStone2 = g_NixiesLakeBossZone.AddMonster(
				747,
				tx2 + gObj[aIndex].X,
				ty2 + gObj[aIndex].Y,
				tx2 + gObj[aIndex].X,
				ty2 + gObj[aIndex].Y,
				0);
			if (nRestrictStone2 >= 0)
			{
				bIsSuccess = 1;
				break;
			}
			gObjDel(nRestrictStone1);
		}
	}
	if (!bIsSuccess)
	{
		nRestrictStone1 = g_NixiesLakeBossZone.AddMonster(
			747,
			gObj[aIndex].X - 3,
			gObj[aIndex].Y - 3,
			gObj[aIndex].X + 3,
			gObj[aIndex].Y + 3,
			0);
		if (nRestrictStone1 < 0)
		{
			//LogAdd(LOG_BLACK,
			//	"[NixiesBoss][SummonRestrictStone] error : %s %d",
			//	"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\nixieslake.cpp",
			//	this->SummonRestrictStone(lpObj));
			return 0;
		}
		nRestrictStone2 = g_NixiesLakeBossZone.AddMonster(
			747,
			gObj[aIndex].X - 3,
			gObj[aIndex].Y - 3,
			gObj[aIndex].X + 3,
			gObj[aIndex].Y + 3,
			0);
		if (nRestrictStone2 < 0)
		{
			gObjDel(nRestrictStone1);
			//LogAdd(LOG_BLACK,
			//	"[NixiesBoss][SummonRestrictStone] error : %s %d",
			//	"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\nixieslake.cpp",
			//	this->SummonRestrictStone(lpObj));
			return 0;
		}
		bIsSuccess = 1;
	}
	if (bIsSuccess)
	{
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 = nRestrictStone1;
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 = nRestrictStone2;
		this->m_stKnowAbsorbInfo.m_nRestrictStoneCount = 2;
		GCMonsterSkillSend(lpObj, lpObj, 123);
		LogAdd(LOG_BLACK, "[NixiesBoss][SummonRestrictStone] Create. Index1 : %d, Index2  : %d)", nRestrictStone1, nRestrictStone2);
		result = 1;
	}
	else
	{
		result = 0;
	}
	return result;
}
// 13ADBF8: using guessed type int `CNixiesLakeBoss::SummonRestrictStone'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);
// A72693C: using guessed type void (*LogAdd)(char *, ...);

//----- (00F3FF90) --------------------------------------------------------
BYTE  CNixiesLakeBoss::SetDevilIceMagicSquare()
{
	int nRangeX; // ST50_4@3
	int nRangeY; // ST4C_4@3
	int i; // [sp+54h] [bp-8h]@1


	this->m_stDevilIceInfo.m_bAttacking = 1;
	
	for (i = 0; i < 3; ++i)
	{
		nRangeX = g_btBossZoneDevilIceSquareMapXY[4 * i + 2]-g_btBossZoneDevilIceSquareMapXY[4 * i];
		
		nRangeY = g_btBossZoneDevilIceSquareMapXY[4 * i + 3]-g_btBossZoneDevilIceSquareMapXY[4 * i + 1];
		
		this->m_stDevilIceInfo.m_DevilIceMagicSquare[i].m_btPosX = rand() % nRangeX + g_btBossZoneDevilIceSquareMapXY[4 * i];
		
		this->m_stDevilIceInfo.m_DevilIceMagicSquare[i].m_btPosY = rand() % nRangeY + g_btBossZoneDevilIceSquareMapXY[4 * i + 1];
	}
	return 1;
}

//----- (00F40080) --------------------------------------------------------
//获得可用的攻击技能计数
int CNixiesLakeBoss::GetAvailableAttackSkillCount (LPOBJ lpObj)
{
	int nCountOfAvailableAttackSkill; // [sp+50h] [bp-8h]@3

	if (this->CheckNixLifeStatus(lpObj) > 90 || this->m_stDevilIceInfo.m_bAttacking)
		nCountOfAvailableAttackSkill = 3;
	else
		nCountOfAvailableAttackSkill = 4;
	return nCountOfAvailableAttackSkill;
}

//----- (00F400F0) --------------------------------------------------------
void CNixiesLakeBoss::InitIceBreathAttack(LPOBJ lpObj, LPOBJ lpTargetObj)
{

	int v4; // eax@1

	v4 = gSkillManager.GetSkillAngle( lpTargetObj->X, lpTargetObj->Y, lpObj->X, lpObj->Y);
	this->m_stIceBreathInfo.m_bAttack = 1;
	this->m_stIceBreathInfo.m_nAttackStartAngle = (v4 + 90) % 360;
	this->m_stIceBreathInfo.m_nAttackCount = 0;
	this->NotifyIceBreathDir( 1, lpTargetObj);
	this->IceBreathAttack( lpObj);
	LogAdd(LOG_BLACK, gMessage.GetMessage(507), lpTargetObj->Index, lpTargetObj->Name);
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F401D0) --------------------------------------------------------
void  CNixiesLakeBoss::IceBreathAttack( LPOBJ lpObj)
{

	if (++this->m_stIceBreathInfo.m_nAttackCount >= 12)
	{
		this->m_stIceBreathInfo.Clear();

		this->NotifyIceBreathDir( 0, 0);
		this->InitIceBoom();
	}
}

//----- (00F40230) --------------------------------------------------------
bool  CNixiesLakeBoss::IsIceBreathAttacking()
{
	return this->m_stIceBreathInfo.m_bAttack;
}

//----- (00F40250) --------------------------------------------------------
void  CNixiesLakeBoss::InitIceBoom()
{
	this->m_stIceBoomInfo.m_bAttack = 1;
	this->m_stIceBreathInfo.m_nAttackCount = 0;
}

//----- (00F40280) --------------------------------------------------------
void  CNixiesLakeBoss::IceBoomAttackReady( LPOBJ lpObj)
{

	if (lpObj->TargetNumber >= 0)
	{
		this->m_stIceBoomInfo.m_bAttacking = 1;
		this->SetIceBoomMagicSquare( lpObj);
		this->NotifyIceBoomMagicSquarePos();
		CMonsterSkillManager::UseMonsterSkill(lpObj->Index, lpObj->TargetNumber, 0, 4, 0);
	}
	else
	{
		this->m_stIceBoomInfo.m_bAttack = 0;
	}
}

//----- (00F40300) --------------------------------------------------------
void  CNixiesLakeBoss::SetIceBoomMagicSquare(LPOBJ lpObj)
{

	int i; // [sp+50h] [bp-34h]@14
	int nTarUserIndex; // [sp+54h] [bp-30h]@17
	__int16 nPosY; // [sp+58h] [bp-2Ch]@11
	__int16 nPosX; // [sp+5Ch] [bp-28h]@11
	int nTarUserCnt; // [sp+60h] [bp-24h]@1
	int nTarIndex[5]; // [sp+64h] [bp-20h]@1
	int count; // [sp+78h] [bp-Ch]@1
	int tObjNum; // [sp+7Ch] [bp-8h]@3

	count = 0;
	nTarIndex[0] = 0;
	nTarIndex[1] = 0;
	nTarIndex[2] = 0;
	nTarIndex[3] = 0;
	nTarIndex[4] = 0;
	nTarUserCnt = 0;
	do
	{
		if (lpObj->VpPlayer[count].state)
		{
			tObjNum = lpObj->VpPlayer[count].index;
			if (gObjIsConnected(tObjNum) != 0)
			{
				if (lpObj->VpPlayer[count].type == 1 && gObjCalcDistance(lpObj, &gObj[tObjNum]) <= 9)
				{
					nTarIndex[nTarUserCnt++] = tObjNum;
					if (nTarUserCnt >= 5)
						break;
				}
			}

		}
		++count;
	} while (count <= 74);
	nPosX = 0;
	nPosY = 0;
	if (nTarUserCnt >= 5)
		nTarUserCnt = 5;
	if (nTarUserCnt > 0)
	{
		for (i = 0; i < nTarUserCnt; ++i)
		{
			nTarUserIndex = nTarIndex[i];
			if (gObjIsConnected(nTarUserIndex))
			{
				nPosX = gObj[nTarUserIndex].X;
				nPosY = gObj[nTarUserIndex].Y;
				if (!g_NixiesLakeBossZone.IsBossZone( nPosX, nPosY))
				{
					if (!g_NixiesLakeBossZone.GetBoxPosition(
						MAP_KNICKS,
						lpObj->X - 4,
						lpObj->Y - 4,
						lpObj->X + 4,
						lpObj->Y + 4,
						&nPosX,
						&nPosY))
					{
						nPosX = lpObj->X;
						nPosY = lpObj->Y;
						//LogAdd(LOG_BLACK,
						//	"[NixiesBoss][SetIceBoomSquare] GetBoxPosition #1 FAIL!!! %s %d",
						//	"c:\\muserversvn\\job_branch\\kor\\job_season12_part1\\gameserver\\nixieslake.cpp",
						//	this->SummonRestrictStone(lpObj));
					}
					LogAdd(LOG_BLACK,
						gMessage.GetMessage(508),
						gObj[nTarUserIndex].Name,
						nTarUserIndex,
						nPosX,
						nPosY);
				}
				this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btIndex = i;
				this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX = (BYTE)nPosX;
				this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY = (BYTE)nPosY;
				this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_wTarget = nTarUserIndex;
				++this->m_stIceBoomInfo.m_nMagicSquareCnt;
			}
		}
	}
}
// 13ADBFC: using guessed type int `CNixiesLakeBoss::SetIceBoomMagicSquare'::`2'::__LINE__Var;
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F40680) --------------------------------------------------------
void CNixiesLakeBoss::IceBoomAttackDamage( LPOBJ lpObj)
{
	double v2; // st7@13


	signed int j; // [sp+4Ch] [bp-24h]@16
	int i; // [sp+54h] [bp-1Ch]@8
	LPOBJ lpTargetObj; // [sp+58h] [bp-18h]@5
	int PosY; // [sp+60h] [bp-10h]@10
	int PosX; // [sp+64h] [bp-Ch]@10
	char Index; // [sp+68h] [bp-8h]@10

	if (this->m_stIceBoomInfo.m_bAttacking)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];
			if (gObjIsConnected(n)
				&& lpTargetObj->Map == MAP_KNICKS
				&& g_NixiesLakeBossZone.IsBossZone( lpTargetObj->X, lpTargetObj->Y))
			{
				for (i = 0; i < this->m_stIceBoomInfo.m_nMagicSquareCnt; ++i)
				{
					Index = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btIndex;
					PosX = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX;
					PosY = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY;
					if (gViewport.CheckViewportObjectPosition(n, lpTargetObj->Map,PosX, PosY,15) == 1)
						this->SendIceBoomMagicSquarePos( n, 1, Index, PosX, PosY);
					if (gObjCalDistance(lpTargetObj->X, lpTargetObj->Y, PosX, PosY) <= 2)
					{
						v2 = 80.0 / 100.0 * lpTargetObj->Life;
						gAttack.Attack(lpObj, lpTargetObj, 0, 0, 0, (int)sqrt(v2), 0, 0);
					}
				}
			}
		}
		++this->m_stIceBoomInfo.m_nAttackCount;
		this->m_stIceBoomInfo.m_bAttacking = 0;
		this->m_stIceBoomInfo.m_nMagicSquareCnt = 0;
		for (j = 0; j < 5; ++j)
			this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[j];

		if (this->m_stIceBoomInfo.m_nAttackCount >= 3)
			this->m_stIceBoomInfo.Clear();
	}
}

//----- (00F40900) --------------------------------------------------------
BYTE  CNixiesLakeBoss::SetKnowledgeAbsorb(LPOBJ lpObj)
{
	BYTE result; // al@2



	if (this->SummonRestrictStone(lpObj) == 1)
	{
		LogAdd(LOG_BLACK, gMessage.GetMessage(509));
		this->StartImmune(lpObj);
		this->NotifyKnowledgeAbsorbInfo(1);
		result = 1;
	}
	else
	{
		result = 0;
	}
	return result;
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F40970) --------------------------------------------------------
void  CNixiesLakeBoss::FailRemoveRestrictStone( LPOBJ lpObj)
{



	this->ClearRestrictStone();
	this->EndImmune( lpObj);
	this->SummonMonster();
	this->NotifyKnowledgeAbsorbInfo( 0);
}

//----- (00F409C0) --------------------------------------------------------
void  CNixiesLakeBoss::SuccessRemoveRestrictStone(LPOBJ lpObj)
{

	this->ClearRestrictStone();
	this->EndImmune( lpObj);
	this->NotifyKnowledgeAbsorbInfo( 0);
}

//----- (00F40A00) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::RemoveRestrictStone(LPOBJ lpObj)
{
	--this->m_stKnowAbsorbInfo.m_nRestrictStoneCount;
	if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 == lpObj->Index)
	{
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 = -1;
	}
	else if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 == lpObj->Index)
	{
		this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 = -1;
	}
}

//----- (00F40A70) --------------------------------------------------------
void  CNixiesLakeBoss::ClearRestrictStone()
{



	if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1 != -1)
		gObjDel(this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex1);
	if (this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2 != -1)
		gObjDel(this->m_stKnowAbsorbInfo.m_nRestrictStoneIndex2);
	this->m_stKnowAbsorbInfo.Clear();

}

//----- (00F40AE0) --------------------------------------------------------
void CNixiesLakeBoss::SummonMonster()
{
	int iMonIndex; // ST68_4@3
	signed int v2; // eax@3
	int nIndex; // [sp+4Ch] [bp-18h]@3
	int i; // [sp+54h] [bp-10h]@1

	for (i = 0; i < 20; ++i)
	{
		iMonIndex = rand() % 3 + 743;
		v2 = rand();
		nIndex =g_NixiesLakeBossZone.AddMonster(
			iMonIndex,
			(unsigned __int8)g_btBossZoneMonSummonMapXY[0],
			(unsigned __int8)g_btBossZoneMonSummonMapXY[1],
			(unsigned __int8)g_btBossZoneMonSummonMapXY[2],
			(unsigned __int8)g_btBossZoneMonSummonMapXY[3],
			v2 % 6);
		if (nIndex < 0)
		{
			LogAdd(LOG_BLACK, "[BossZone][SummonMon] AddMonster fail!! [nIndex]:%d", nIndex);
			return;
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F40BD0) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::StartImmune(LPOBJ lpObj)
{


	this->m_stKnowAbsorbInfo.m_dwKnowledgeAbsorbTimer = GetTickCount() + 180000;
	this->m_bNixImmune = 1;

	gEffectManager.AddEffect(lpObj, 0, EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY, 236, 0, 0, 0, 0);
	gEffectManager.AddEffect(lpObj, 0, EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY, 236, 0, 0, 0, 0);

	LogAdd(LOG_BLACK, gMessage.GetMessage(510));
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F40C60) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::EndImmune(LPOBJ lpObj)
{

	this->m_bNixImmune = 0;
	gEffectManager.DelEffect(lpObj, EFFECT_MONSTER_PHYSI_DAMAGE_IMMUNITY);
	gEffectManager.DelEffect(lpObj, EFFECT_MONSTER_MAGIC_DAMAGE_IMMUNITY);
	this->m_stKnowAbsorbInfo.Clear();

	LogAdd(LOG_BLACK, gMessage.GetMessage(511));
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F40CD0) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::NotifyIceBoomMagicSquarePos()
{
	int i; // [sp+4Ch] [bp-1Ch]@8
	LPOBJ lpTargetObj; // [sp+50h] [bp-18h]@5

	char PosY; // [sp+58h] [bp-10h]@10
	char PosX; // [sp+5Ch] [bp-Ch]@10
	char Index; // [sp+60h] [bp-8h]@10

	if (this->m_stIceBoomInfo.m_bAttack)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];
			if (gObjIsConnected(n)
				&& lpTargetObj->Map == MAP_KNICKS
				&& g_NixiesLakeBossZone.IsBossZone(lpTargetObj->X, lpTargetObj->Y))
			{
				for (i = 0; i < this->m_stIceBoomInfo.m_nMagicSquareCnt; ++i)
				{
					Index = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btIndex;
					PosX = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX;
					PosY = this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY;
					if (gViewport.CheckViewportObjectPosition(n, lpTargetObj->Map, this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosX, this->m_stIceBoomInfo.m_stIceBoomMagicSquarePos[i].m_btPosY, 15)== 1)
						this->SendIceBoomMagicSquarePos( n, 0, Index, PosX, PosY);
				}
			}
		}
	}
}

//----- (00F40E50) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::NotifyDevilIceMagicSquarePos()
{
	LPOBJ lpTargetObj; // [sp+4Ch] [bp-Ch]@5

	if (this->m_stDevilIceInfo.m_bAttacking)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];
			if (gObjIsConnected(n)
				&& lpTargetObj->Map == MAP_KNICKS
				&& g_NixiesLakeBossZone.IsBossZone( lpTargetObj->X, lpTargetObj->Y))
			{
				this->SendDevilIceMagicSquarePos(n, 0, this->m_stDevilIceInfo.m_DevilIceMagicSquare);
			}
		}
	}
}

//----- (00F40F40) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::NotifyIceBreathDir(BYTE status, LPOBJ lpTargetObj)
{
	LPOBJ lpTempObj; // [sp+4Ch] [bp-Ch]@5


	if (this->m_stIceBreathInfo.m_bAttack)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTempObj = &gObj[n];
			if (gObjIsConnected(n)
				&& lpTempObj->Map == MAP_KNICKS
				&& g_NixiesLakeBossZone.IsBossZone(lpTempObj->X, lpTempObj->Y))
			{
				if (status)
					this->SendIceBreathDir( n, status, lpTargetObj->Index, (BYTE)lpTargetObj->X, (BYTE)lpTargetObj->Y);
				else
					this->SendIceBreathDir( n, 0, 0, 0, 0);
			}
		}
	}
}

//----- (00F41080) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::DevilIceAttackDamage(LPOBJ lpObj)
{
	LPOBJ lpTargetObj; // [sp+4Ch] [bp-Ch]@5


	if (this->m_stDevilIceInfo.m_bAttacking)
	{
		for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
		{
			lpTargetObj = &gObj[n];
			if (gObjIsConnected(n))
			{
				if (lpTargetObj->Map == MAP_KNICKS
					&& g_NixiesLakeBossZone.IsBossZone( lpTargetObj->X, lpTargetObj->Y))
				{
					this->SendDevilIceMagicSquarePos( n, 1, this->m_stDevilIceInfo.m_DevilIceMagicSquare);
					if (gObjCalDistance(
						lpTargetObj->X,
						lpTargetObj->Y,
						this->m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosX,
						this->m_stDevilIceInfo.m_DevilIceMagicSquare[0].m_btPosY) > 2
						&& gObjCalDistance(
							lpTargetObj->X,
							lpTargetObj->Y,
							this->m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosX,
							this->m_stDevilIceInfo.m_DevilIceMagicSquare[1].m_btPosY) > 2
						&& gObjCalDistance(
							lpTargetObj->X,
							lpTargetObj->Y,
							this->m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosX,
							this->m_stDevilIceInfo.m_DevilIceMagicSquare[2].m_btPosY) > 2)
					{
						lpTargetObj->Life = 0.0;
						
						gObjectManager.CharacterLifeCheck(lpTargetObj, lpObj, 100, 0, 0, 0, 0, 0);
						LogAdd(LOG_BLACK,
							"[NixiesBoss][ApplyDebuff] Instance Death. Target : [%d][%s]",
							lpTargetObj->Index,
							lpTargetObj->Name);
					}
				}
			}
		}
		this->m_stDevilIceInfo.m_bAttacking = 0;
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F41290) --------------------------------------------------------
void  CNixiesLakeBoss::RefillLife(LPOBJ lpObj)
{
	double v2; // st7@1

	unsigned __int64 v4; // st7@5

	int n; // [sp+5Ch] [bp-Ch]@6

	v2 = lpObj->Life / 2.0;
	if (lpObj->Life < (double)lpObj->MaxLife)
	{
		lpObj->Life = lpObj->MaxLife >(double)sqrt(v2) + lpObj->Life ? (float)((double)sqrt(v2) + lpObj->Life) : lpObj->MaxLife;
		*(double *)&v4 = lpObj->Life - (double)sqrt(v2);
		LogAdd(LOG_BLACK,
			gMessage.GetMessage(512),
			(DWORD)SET_NUMBERLDW(v4),
			(DWORD)SET_NUMBERHDW(v4), 
			(DWORD)SET_NUMBERLDW(lpObj->Life),
			(DWORD)SET_NUMBERHDW(lpObj->Life));
		if (lpObj->Type == 2)
		{
			for (n = 0; n < MAX_VIEWPORT; ++n)
			{
				if (lpObj->VpPlayer2[n].state)
				{
					if (lpObj->VpPlayer2[n].type == 1)
					{
						//v5 = lpObj->Life;
						//j___ftol2_sse(v13, v15, v17);
						sqrt(lpObj->Life);
						//v6 = (double)lpObj->AddLife + lpObj->MaxLife;
						//j___ftol2_sse(v7, v8, v9);
						sqrt(lpObj->AddLife + lpObj->MaxLife);
						GCLifeUpdateSend(&gObj[lpObj->VpPlayer2[n].index]);
					}
				}
			}
		}
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);

//----- (00F41450) --------------------------------------------------------
void  CNixiesLakeBoss::FailBossBattles(LPOBJ lpObj)
{

	g_NixiesLakeBossZone.Clear();
	this->EndImmune(lpObj);
	lpObj->Life = lpObj->MaxLife;
	g_NixiesLakeBossZone.SetBossZoneState( 0);
}

//----- (00F414B0) --------------------------------------------------------
void CNixiesLakeBoss::DieNixiesBoss()
{
	g_NixiesLakeBossZone.Clear();
	g_NixiesLakeBossZone.SetBossDieTimer();
	g_NixiesLakeBossZone.SetBossNixRegenTimer();
	g_NixiesLakeBossZone.SetBossZoneState( 2);
}

//----- (00F41500) --------------------------------------------------------
void  CNixiesLakeBoss::NotifyKnowledgeAbsorbInfo(BYTE status)
{
	LPOBJ lpTargetObj; // [sp+4Ch] [bp-Ch]@4

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; ++n)
	{
		lpTargetObj = &gObj[n];
		if (gObjIsConnected(n)
			&& lpTargetObj->Map == MAP_KNICKS
			&& g_NixiesLakeBossZone.IsBossZone( lpTargetObj->X, lpTargetObj->Y))
		{
			this->SendKnowledgeAbsorb(lpTargetObj->Index, status);
		}
	}
}

//----- (00F415D0) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::DisableCountSkill()
{
	if (this->m_stIceBreathInfo.m_bAttack == 1)
		this->m_stIceBreathInfo.Clear();

}

//----- (00F41610) --------------------------------------------------------
int  CNixiesLakeBoss::SearchTarget(LPOBJ lpObj, int Distance)
{

	int count; // [sp+50h] [bp-Ch]@1
	int tObjNum; // [sp+54h] [bp-8h]@3

	count = 0;
	do
	{
		if (lpObj->VpPlayer[count].state)
		{
			tObjNum = lpObj->VpPlayer[count].index;
			if (gObjIsConnected(tObjNum) != 0 && lpObj->VpPlayer[count].type == 1 && gObj[tObjNum].Live == 1 && gObjCalcDistance(lpObj, &gObj[tObjNum]) <= Distance)
			{
				return tObjNum;
			}
		}
		++count;
	} while (count <= 74);
	return -1;
}

//----- (00F41740) --------------------------------------------------------
void __thiscall CNixiesLakeBoss::SetRefillLifeDelay()
{
	this->m_dwRefillLifeDelay = GetTickCount() + 4000;
}

//----- (00F41770) --------------------------------------------------------
BOOL __thiscall CNixiesLakeBoss::ChkRefillLifeDelay()
{
	return this->m_dwRefillLifeDelay && this->m_dwRefillLifeDelay <= GetTickCount();
}

//----- (00F417C0) --------------------------------------------------------
void  CNixiesLakeBoss::SendIceBoomMagicSquarePos( int aIndex, BYTE status, BYTE index, BYTE x, BYTE y)
{
	_tagPMSG_SEND_ICE_BOOM_POS pMsg; // [sp+4Ch] [bp-Ch]@1
	pMsg.h.set(0x76, 0x05, sizeof(pMsg));

	pMsg.btStatus = status;
	pMsg.btIndex = index;
	pMsg.btPosX = x;
	pMsg.btPosY = y;
	DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
}

//----- (00F41830) --------------------------------------------------------
void  CNixiesLakeBoss::SendDevilIceMagicSquarePos( int aIndex, BYTE status, _stMagicSquarePos *squarePos)
{
	_tagPMSG_SEND_DEVILICE_MAGIC_SQUARE_POS pMsg; // [sp+4Ch] [bp-10h]@1
	pMsg.h.set(0x76, 0x06, sizeof(pMsg));

	pMsg.btStatus = status;
	pMsg.btPosX1 = squarePos->m_btPosX;
	pMsg.btPosY1 = squarePos->m_btPosY;
	pMsg.btPosX2 = squarePos[1].m_btPosX;
	pMsg.btPosY2 = squarePos[1].m_btPosY;
	pMsg.btPosX3 = squarePos[2].m_btPosX;
	pMsg.btPosY3 = squarePos[2].m_btPosY;
	DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
}

//----- (00F418D0) --------------------------------------------------------
void  CNixiesLakeBoss::SendIceBreathDir( int aIndex, BYTE status, unsigned __int16 TargetIndex, BYTE x, BYTE y)
{
	_tagPMSG_SEND_ICE_BREATH_DIR pMsg; // [sp+4Ch] [bp-10h]@1
	pMsg.h.set(0x76, 0x07, sizeof(pMsg));

	pMsg.btState = status;
	pMsg.wTargetIndex = TargetIndex;
	pMsg.btTargetX = x;
	pMsg.btTargetY = y;
	DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
}

//----- (00F41940) --------------------------------------------------------
void  CNixiesLakeBoss::SendKnowledgeAbsorb( int aIndex, BYTE status)
{
	_tagPMSG_SEND_KNOWLEDGEABSORBINFO pMsg; // [sp+4Ch] [bp-Ch]@1
	pMsg.h.set(0x76, 0x08, sizeof(pMsg));

	pMsg.btStatus = status;
	DataSend(aIndex, (BYTE *)&pMsg, pMsg.h.size);
}

//----- (00F41990) --------------------------------------------------------
void  CNixiesLakeBoss::RecvSendIceBreathAttack( _tagPMSG_ICE_BREATH_ATTACK *lpMsg, int nTargetIndex)
{

	int NewTarget; // [sp+50h] [bp-10h]@17
	LPOBJ lpTargetObj; // [sp+54h] [bp-Ch]@10
	LPOBJ lpBossObj; // [sp+58h] [bp-8h]@10

	if (this->m_stIceBreathInfo.m_bAttack)
	{
		if (gObjIsConnected(nTargetIndex) == 0)
		{
			return;
		}
		if (gObjIsConnected(lpMsg->wAttackerIndex) == 0)
		{
			return;
		}

		lpBossObj = &gObj[lpMsg->wAttackerIndex];
		lpTargetObj = &gObj[nTargetIndex];
		if (lpBossObj->Class == 746
			&& lpTargetObj->Map == MAP_KNICKS
			&& g_NixiesLakeBossZone.IsBossZone( lpTargetObj->X, lpTargetObj->Y))
		{
			if (!gEffectManager.CheckEffect(lpTargetObj, EFFECT_ICE))
			{
				gEffectManager.AddEffect(lpTargetObj, 0, EFFECT_ICE, 20, 0, 0, 0, 0);


				LogAdd(LOG_BLACK,
					gMessage.GetMessage(513),
					lpTargetObj->Index,
					lpTargetObj->Name);
			}
			gAttack.Attack(lpBossObj, lpTargetObj, 0, 0, 0, 0, 0, 0);

			DataSend(nTargetIndex, (BYTE *)&lpMsg, lpMsg->h.size);
			MsgSendV2(lpTargetObj, (BYTE *)&lpMsg, lpMsg->h.size);
			if (lpTargetObj->Life <= 0.0 || !lpTargetObj->Live)
			{
				NewTarget = this->SearchTarget( lpBossObj, 15);
				if (NewTarget <= -1)
				{
					this->NotifyIceBreathDir( 0, 0);
					this->m_stIceBreathInfo.Clear();

				}
				else
				{
					this->NotifyIceBreathDir( 2, &gObj[NewTarget]);
				}
			}
		}
			
		
	}
}
// A7268D8: using guessed type void (*LogAddTD)(char *, ...);