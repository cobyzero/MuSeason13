#include "stdafx.h"
#include "CustomMonster.h"
#include "DSProtocol.h"
#include "MemScript.h"
#include "Message.h"
#include "Monster.h"
#include "Notice.h"
#include "Util.h"
#include "ServerInfo.h"
#include "ObjectManager.h"

CCustomMonster gCustomMonster;

CCustomMonster::CCustomMonster() // OK
{
	this->m_CustomMonsterInfo.clear();
}

CCustomMonster::~CCustomMonster() // OK
{

}

void CCustomMonster::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}

	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_CustomMonsterInfo.clear();

	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}

			CUSTOM_MONSTER_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.MapNumber = lpMemScript->GetAsNumber();

			info.MaxLife	= lpMemScript->GetAsFloatNumber();
			info.Damage		= lpMemScript->GetAsFloatNumber();
			info.Defense	= lpMemScript->GetAsFloatNumber();
			info.AttackRate	= lpMemScript->GetAsFloatNumber();
			info.DefenseRate= lpMemScript->GetAsFloatNumber();

			info.MinDmgMakePoints	= lpMemScript->GetAsNumber();
			info.MinDmgMakePerc		= lpMemScript->GetAsNumber();
			info.MaxDmgMakePerc		= lpMemScript->GetAsNumber();
			info.MinDmgGetPoints	= lpMemScript->GetAsNumber();
			info.MaxDmgGetPoints	= lpMemScript->GetAsNumber();

			info.hpRegenTime	= lpMemScript->GetAsNumber();
			info.hpRegenAmount	= lpMemScript->GetAsNumber();

			info.ExperienceRate = lpMemScript->GetAsNumber();

			info.KillMessage = lpMemScript->GetAsNumber();

			info.PlayerNumber = lpMemScript->GetAsNumber();

			info.Level			= lpMemScript->GetAsNumber();
			info.Points			= lpMemScript->GetAsNumber();
			info.Resets			= lpMemScript->GetAsNumber();
			info.MinVipResets	= lpMemScript->GetAsNumber();
			info.GResets		= lpMemScript->GetAsNumber();
			info.MinVipGResets	= lpMemScript->GetAsNumber();
			info.GoblinPoints	= lpMemScript->GetAsNumber();
			info.WcoinP			= lpMemScript->GetAsNumber();
			info.WcoinC			= lpMemScript->GetAsNumber();
			info.MinVIPCoin		= lpMemScript->GetAsNumber();

			strcpy_s(info.Rank, lpMemScript->GetAsString());

			info.RankColor = lpMemScript->GetAsNumber();

			this->m_CustomMonsterInfo.push_back(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CCustomMonster::SetCustomMonsterInfo(LPOBJ lpObj) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(lpObj->Class,lpObj->Map,&CustomMonsterInfo) == 0)
	{
		return;
	}

	//lpObj->Life = (float)((__int64)lpObj->Life*((CustomMonsterInfo.MaxLife==-1)?100:CustomMonsterInfo.MaxLife))/100;
	//lpObj->MaxLife = (float)((__int64)lpObj->MaxLife * ((CustomMonsterInfo.MaxLife == -1) ? 100 : CustomMonsterInfo.MaxLife)) / 100;
	//lpObj->ScriptMaxLife = (float)((__int64)lpObj->ScriptMaxLife * ((CustomMonsterInfo.MaxLife == -1) ? 100 : CustomMonsterInfo.MaxLife)) / 100;

	lpObj->Life			 *= CustomMonsterInfo.MaxLife;
	lpObj->MaxLife		 *= CustomMonsterInfo.MaxLife;
	lpObj->ScriptMaxLife *= CustomMonsterInfo.MaxLife;	

	//lpObj->PhysiDamageMin = ((__int64)lpObj->PhysiDamageMin*((CustomMonsterInfo.Damage==-1)?100:CustomMonsterInfo.Damage))/100;
	//lpObj->PhysiDamageMax = ((__int64)lpObj->PhysiDamageMax*((CustomMonsterInfo.Damage==-1)?100:CustomMonsterInfo.Damage))/100;

	lpObj->PhysiDamageMin = (__int64)((float)lpObj->PhysiDamageMin * CustomMonsterInfo.Damage);
	lpObj->PhysiDamageMax = (__int64)((float)lpObj->PhysiDamageMax * CustomMonsterInfo.Damage);

	//lpObj->Defense = ((__int64)lpObj->Defense*((CustomMonsterInfo.Defense==-1)?100:CustomMonsterInfo.Defense))/100;
	lpObj->Defense = ((float)lpObj->Defense * CustomMonsterInfo.Defense);

	//lpObj->m_MPSkillOpt.AttackSuccessRate = ((__int64)lpObj->m_MPSkillOpt.AttackSuccessRate*((CustomMonsterInfo.AttackRate==-1)?100:CustomMonsterInfo.AttackRate))/100;
	//lpObj->m_MPSkillOpt.DefenseSuccessRate = ((__int64)lpObj->m_MPSkillOpt.DefenseSuccessRate*((CustomMonsterInfo.DefenseRate==-1)?100:CustomMonsterInfo.DefenseRate))/100;

	lpObj->m_MPSkillOpt.AttackSuccessRate = (__int64)((float)lpObj->m_MPSkillOpt.AttackSuccessRate * CustomMonsterInfo.AttackRate);
	lpObj->m_MPSkillOpt.DefenseSuccessRate = (__int64)((float)lpObj->m_MPSkillOpt.DefenseSuccessRate * CustomMonsterInfo.DefenseRate);

	//lpObj->ElementalDefense = ((__int64)lpObj->ElementalDefense*((CustomMonsterInfo.Defense==-1)?100:CustomMonsterInfo.Defense))/100;
	//lpObj->ElementalDamageMin = ((__int64)lpObj->ElementalDamageMin*((CustomMonsterInfo.Damage==-1)?100:CustomMonsterInfo.Damage))/100;
	//lpObj->ElementalDamageMax = ((__int64)lpObj->ElementalDamageMax*((CustomMonsterInfo.Damage==-1)?100:CustomMonsterInfo.Damage))/100;
	//lpObj->ElementalAttackSuccessRate = ((__int64)lpObj->ElementalAttackSuccessRate*((CustomMonsterInfo.AttackRate==-1)?100:CustomMonsterInfo.AttackRate))/100;
	//lpObj->ElementalDefenseSuccessRate = ((__int64)lpObj->ElementalDefenseSuccessRate*((CustomMonsterInfo.DefenseRate==-1)?100:CustomMonsterInfo.DefenseRate))/100;

	lpObj->ElementalDefense				= (__int64)((float)lpObj->ElementalDefense * CustomMonsterInfo.Defense);

	lpObj->ElementalDamageMin			= (__int64)((float)lpObj->ElementalDamageMin * CustomMonsterInfo.Damage);
	lpObj->ElementalDamageMax			= (__int64)((float)lpObj->ElementalDamageMax * CustomMonsterInfo.Damage);

	lpObj->ElementalAttackSuccessRate	= (__int64)((float)lpObj->ElementalAttackSuccessRate * CustomMonsterInfo.AttackRate);
	lpObj->ElementalDefenseSuccessRate	= (__int64)((float)lpObj->ElementalDefenseSuccessRate * CustomMonsterInfo.DefenseRate);

}

void CCustomMonster::MonsterDieProc(LPOBJ lpObj /*monster*/ , LPOBJ lpTarget /*player*/) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(lpObj->Class,lpObj->Map,&CustomMonsterInfo) == 0)
	{
		return;
	}

	int sortedCount = gObjMonsterGetTopHitDamageUser(lpObj, CustomMonsterInfo.PlayerNumber);

	bool announced = false;

	if (sortedCount > 0)
	{
		for (int i = 0; i < sortedCount; i++)
		{
			if (OBJECT_RANGE(lpObj->HitDamage[i].index) <= 0)
			{
				continue;
			}

			lpTarget = &gObj[lpObj->HitDamage[i].index]; //player

			if (gObjCalcDistance(lpObj, lpTarget) > 11)
			{
				LogAdd(LOG_RED, "[MonsterKillBonus] [%s][%s] Lost monster bonus caused by big distance [Monster %d][Map %d]", lpTarget->Account, lpTarget->Name, lpObj->Class, lpObj->Map);
				continue;
			}

			if (announced == false && CustomMonsterInfo.KillMessage != -1)
			{
				gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gMessage.GetMessage(CustomMonsterInfo.KillMessage), lpTarget->Name, lpObj->Name);
				announced = true;
			}

			if (CustomMonsterInfo.GoblinPoints > 0 || CustomMonsterInfo.WcoinP > 0 || CustomMonsterInfo.WcoinC > 0)
			{
				GDCustomMonsterRewardSaveSend(lpTarget->Index, lpObj->Class, lpObj->Map, CustomMonsterInfo.GoblinPoints, CustomMonsterInfo.WcoinP, CustomMonsterInfo.WcoinC, lpObj->HitDamage[i].damage);
				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got |%d|%d|%d| coins for killing %d with %d damage", lpTarget->Name, CustomMonsterInfo.GoblinPoints, CustomMonsterInfo.WcoinP, CustomMonsterInfo.WcoinC, CustomMonsterInfo.Index, lpObj->HitDamage[i].damage);
			}

			if (CustomMonsterInfo.Level > 0)
			{
				lpTarget->Level += CustomMonsterInfo.Level;
				lpTarget->LevelUpPoint += (CustomMonsterInfo.Level * gServerInfo.m_LevelUpPoint[lpTarget->Class]);				

				if (lpTarget->Level > MAX_CHARACTER_LEVEL)
				{
					lpTarget->LevelUpPoint -= (lpTarget->Level - MAX_CHARACTER_LEVEL) * gServerInfo.m_LevelUpPoint[lpTarget->Class];
					lpTarget->Level = MAX_CHARACTER_LEVEL;
				}

				lpTarget->Experience = gLevelExperience[lpTarget->Level - 1];
				gObjCalcExperience(lpTarget);

				gObjectManager.CharacterCalcAttribute(lpTarget->Index);

				GCLevelUpSend(lpTarget);
				GCNewCharacterInfoSend(lpTarget);
				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got %d levels for killing %d", lpTarget->Name, CustomMonsterInfo.Level, CustomMonsterInfo.Index);
			}

			if (CustomMonsterInfo.Points > 0)
			{
				lpTarget->LevelUpPoint += CustomMonsterInfo.Points;
				GCLevelUpSend(lpTarget);
				GCNewCharacterInfoSend(lpTarget);
				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got %d points for killing %d", lpTarget->Name, CustomMonsterInfo.Points, CustomMonsterInfo.Index);
			}

			if (CustomMonsterInfo.Resets > 0 && CustomMonsterInfo.MinVipResets <= lpTarget->AccountLevel)
			{
				lpTarget->Reset += CustomMonsterInfo.Resets;

				if (lpTarget->Reset > gServerInfo.m_CommandResetLimit[lpTarget->AccountLevel])
				{
					lpTarget->Reset = gServerInfo.m_CommandResetLimit[lpTarget->AccountLevel];
				}

				GCNewCharacterInfoSend(lpTarget);

				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got %d resets for killing %d", lpTarget->Name, CustomMonsterInfo.Resets, CustomMonsterInfo.Index);
			}

			if (CustomMonsterInfo.GResets > 0 && CustomMonsterInfo.MinVipGResets <= lpTarget->AccountLevel)
			{
				lpTarget->MasterReset += CustomMonsterInfo.GResets;

				GCNewCharacterInfoSend(lpTarget);

				LogAdd(LOG_GREEN, "[MonsterKillBonus] %s got %d master resets for killing %d", lpTarget->Name, CustomMonsterInfo.GResets, CustomMonsterInfo.Index);
			}	
		}			
	}
}

long CCustomMonster::GetCustomMonsterExperienceRate(int index,int map) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(index,map,&CustomMonsterInfo) == 0)
	{
		return 100;
	}
	else
	{
		return ((CustomMonsterInfo.ExperienceRate==-1)?100:CustomMonsterInfo.ExperienceRate);
	}
}

long CCustomMonster::GetCustomMonsterMasterExperienceRate(int index,int map) // OK
{
	CUSTOM_MONSTER_INFO CustomMonsterInfo;

	if(this->GetCustomMonsterInfo(index,map,&CustomMonsterInfo) == 0)
	{
		return 100;
	}
	else
	{
		return ((CustomMonsterInfo.ExperienceRate==-1)?100:CustomMonsterInfo.ExperienceRate);
	}
}

bool CCustomMonster::GetCustomMonsterInfo(int index,int map,CUSTOM_MONSTER_INFO* lpInfo) // OK
{
	for (std::vector<CUSTOM_MONSTER_INFO>::iterator it = this->m_CustomMonsterInfo.begin(); it != this->m_CustomMonsterInfo.end(); it++)
	{
		if ((it->Index == -1 || it->Index == index) && (it->MapNumber == -1 || it->MapNumber == map))
		{
			(*lpInfo) = (*it);
			return 1;
		}
	}

	return 0;
}
