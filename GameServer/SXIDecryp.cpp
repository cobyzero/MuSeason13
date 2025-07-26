#include "stdafx.h"
#include "SXIDecryp.h"
#include "DefaultClassInfo.h"
#include "Util.h"
#include "MasterSkillTree.h"

void C3TwoDecryp(BYTE* m_byBuffer)//C3
{
	if (m_byBuffer[2] == 0x24)//移动装备2次解密
	{
		unsigned char tmp, r1, r4;
		tmp = m_byBuffer[4];
		m_byBuffer[4] = m_byBuffer[9];
		m_byBuffer[9] = tmp;
		tmp = m_byBuffer[0xD];
		m_byBuffer[0xD] = m_byBuffer[7];
		m_byBuffer[7] = tmp;
		tmp = m_byBuffer[0xA];
		m_byBuffer[0xA] = m_byBuffer[0xB];
		m_byBuffer[0xB] = tmp;
		r1 = m_byBuffer[0x12] >> 1;
		r4 = m_byBuffer[0x12] >> 4;
		r1 &= 1;
		r4 &= 1;
		if (r1 == 0 && r4 == 0) {}
		if (r1 == 0 && r4 == 1)
		{
			m_byBuffer[0x12] += 2;
			m_byBuffer[0x12] -= 0x10;
		}
		if (r1 == 1 && r4 == 0)
		{
			m_byBuffer[0x12] -= 2;
			m_byBuffer[0x12] += 0x10;
		}

		if (r1 == 1 && r4 == 1) {}
		m_byBuffer[0xF] ^= 0x9F;


	}

	if (m_byBuffer[2] == 0x0E && m_byBuffer[1] == 0x0D)//C3 13
	{
		unsigned char tmp, p1, r3, r5;
		m_byBuffer[5] ^= 0xC5;
		p1 = m_byBuffer[7];
		__asm Rol p1, 3
		m_byBuffer[7] = p1;
		tmp = m_byBuffer[3];
		m_byBuffer[3] = m_byBuffer[5];
		m_byBuffer[5] = tmp;
		r3 = m_byBuffer[5] >> 3;
		r5 = m_byBuffer[5] >> 5;
		r3 &= 1;
		r5 &= 1;
		if (r3 == 0 && r5 == 0) {}
		if (r3 == 0 && r5 == 1)
		{
			m_byBuffer[5] += 0x8;
			m_byBuffer[5] -= 0x20;
		}
		if (r3 == 1 && r5 == 0)
		{
			m_byBuffer[5] -= 0x8;
			m_byBuffer[5] += 0x20;
		}
		if (r3 == 1 && r5 == 1) {}
		tmp = m_byBuffer[0xA];
		m_byBuffer[0xA] = m_byBuffer[8];
		m_byBuffer[8] = tmp;

		p1 = m_byBuffer[6];
		__asm Rol p1, 2
		m_byBuffer[6] = p1;

	}

	if (m_byBuffer[2] == 0x19)//C1 07 19 释放技能
	{
		unsigned char tmp;
		m_byBuffer[3] ^= 0x5F;
		m_byBuffer[6] ^= 0x67;
		tmp = m_byBuffer[3];
		m_byBuffer[3] = m_byBuffer[6];
		m_byBuffer[6] = tmp;
		tmp = m_byBuffer[4];
		m_byBuffer[4] = m_byBuffer[5];
		m_byBuffer[5] = tmp;
		m_byBuffer[5] ^= 0xCB;
		m_byBuffer[4] ^= 0x74;
		tmp = m_byBuffer[3];
		__asm ROL tmp, 1;
		m_byBuffer[3] = tmp;

	}
}
void TwoDecryp(BYTE* m_byBuffer)//C1
{
	unsigned char tmp;
	unsigned char p1, p2, r2, r5, r6, r1, r4;
	if (*m_byBuffer != 0xC1)
	{
		return;
	}
	if (m_byBuffer[2] == 0xF3)
	{

		if (m_byBuffer[3] == 0x0E || m_byBuffer[3] == 0x18 || m_byBuffer[3] == 0x0C || m_byBuffer[3] == 0x01)
		{
			tmp = m_byBuffer[4];
			__asm
			{
				Rol tmp, 2
			}
			m_byBuffer[4] = m_byBuffer[9];
			m_byBuffer[9] = tmp;
			m_byBuffer[10] ^= 0xAE;
			tmp = m_byBuffer[4];
			m_byBuffer[4] = m_byBuffer[6];
			m_byBuffer[6] = tmp;
			tmp = m_byBuffer[8];
			m_byBuffer[8] = m_byBuffer[10];
			m_byBuffer[10] = tmp;
			m_byBuffer[4] ^= 0x1E;
		}
	}
	if (m_byBuffer[2] == 0xD7 && m_byBuffer[1] != 0x06)
	{
		tmp = m_byBuffer[6];
		m_byBuffer[6] = m_byBuffer[4];
		m_byBuffer[4] = tmp;

		p1 = m_byBuffer[3];
		p2 = m_byBuffer[6];
		__asm
		{
			Rol p1, 4
			Rol p2, 3
		}
		m_byBuffer[3] = p1;
		m_byBuffer[6] = p2;
		tmp = m_byBuffer[3];
		m_byBuffer[3] = m_byBuffer[4];
		m_byBuffer[4] = tmp;

	}
	if (m_byBuffer[2] == 0xF1 && m_byBuffer[1] == 0x32)
	{
		m_byBuffer[0x1F] ^= 0xD;
		p1 = m_byBuffer[0x13];
		__asm ROL p1, 1
		m_byBuffer[0x13] = p1;
		r2 = m_byBuffer[0x5] >> 2;
		r5 = m_byBuffer[0x5] >> 5;
		r2 &= 1;
		r5 &= 1;
		if (r2 == 0 && r5 == 0) {}
		if (r2 == 0 && r5 == 1)
		{
			m_byBuffer[0x5] += 0x20;
			m_byBuffer[0x5] -= 0x4;
		}
		if (r2 == 1 && r5 == 0)
		{
			m_byBuffer[0x5] -= 0x20;
			m_byBuffer[0x5] += 0x4;
		}
		if (r2 == 1 && r5 == 1) {}

		r2 = m_byBuffer[0xC] >> 2;
		r6 = m_byBuffer[0xC] >> 6;
		r2 &= 1;
		r6 &= 1;
		if (r2 == 0 && r6 == 0) {}
		if (r2 == 1 && r6 == 0)
		{
			m_byBuffer[0xC] += 0x4;
			m_byBuffer[0xC] -= 0x40;
		}
		if (r2 == 0 && r6 == 1)
		{
			m_byBuffer[0xC] -= 0x4;
			m_byBuffer[0xC] += 0x40;
		}
		if (r2 == 1 && r6 == 1) {}
		p1 = m_byBuffer[0xA];
		__asm ROL p1, 3
		m_byBuffer[0xA] = p1;
		m_byBuffer[0x1F] ^= 0xB2;
		p1 = m_byBuffer[0x14];
		__asm ROL p1, 3
		m_byBuffer[0x14] = p1;
	}
	if (m_byBuffer[2] == 0xD9)
	{
		tmp = m_byBuffer[5];
		__asm  ROL tmp, 5
		m_byBuffer[5] = tmp;
		m_byBuffer[5] ^= 0x0B;
		tmp = m_byBuffer[3];
		__asm  ROL tmp, 7
		m_byBuffer[3] = tmp;
		r1 = m_byBuffer[3] >> 1;
		r4 = m_byBuffer[3] >> 4;
		r1 &= 1;
		r4 &= 1;
		if (r1 == 0 && r4 == 0) {}
		if (r1 == 1 && r4 == 0)
		{
			m_byBuffer[3] -= 0x02;
			m_byBuffer[3] += 0x10;
		}
		if (r1 == 0 && r4 == 1)
		{

			m_byBuffer[3] += 0x02;
			m_byBuffer[3] -= 0x10;
		}
		if (r1 == 1 && r4 == 1) {}
	}
}

void S12Decryp(BYTE* m_byBuffer)
{
	unsigned char tmp, R2, R5, R1, R4, R7;
	if (m_byBuffer[2] == 0xF1 && m_byBuffer[1] == 0x3C)//帐号登陆包C3
	{
		m_byBuffer[7] ^= 0xFD;
		R2 = m_byBuffer[0xF] >> 2;
		R5 = m_byBuffer[0xF] >> 5;
		R2 &= 1;
		R5 &= 1;
		if (R2 == 0 && R5 == 0) {}
		if (R2 == 0 && R5 == 1)
		{
			m_byBuffer[0xF] += 4;
			m_byBuffer[0xF] -= 0x20;
		}
		if (R2 == 1 && R5 == 0)
		{
			m_byBuffer[0xF] -= 4;
			m_byBuffer[0xF] += 0x20;
		}
		if (R2 == 1 && R5 == 1) {}

		R5 = m_byBuffer[0x8] >> 5;
		R1 = m_byBuffer[0x8] >> 1;
		R5 &= 1;
		R1 &= 1;
		if (R1 == 0 && R5 == 0) {}
		if (R1 == 0 && R5 == 1)
		{
			m_byBuffer[0x8] -= 0x20;
			m_byBuffer[0x8] += 2;
		}
		if (R1 == 1 && R5 == 0)
		{
			m_byBuffer[0x8] += 0x20;
			m_byBuffer[0x8] -= 2;
		}
		if (R1 == 1 && R5 == 1) {}
		tmp = m_byBuffer[0xD];
		__asm ROL tmp, 3
		m_byBuffer[0xD] = tmp;

		tmp = m_byBuffer[0x5];
		m_byBuffer[0x5] = m_byBuffer[0xD];
		m_byBuffer[0xD] = tmp;
		m_byBuffer[0x3] ^= 0x86;
		m_byBuffer[0x12] ^= 0xF7;
	}
	if (m_byBuffer[2] == 0xF3)//创建角色
	{
		if (m_byBuffer[1] == 0x0F || m_byBuffer[1] == 0x0E)
		{
			tmp = m_byBuffer[3];
			__asm ROL tmp, 3
			m_byBuffer[3] = tmp;
			m_byBuffer[6] ^= 0x41;
			m_byBuffer[6] ^= 0xCC;
			R4 = m_byBuffer[5] >> 4;
			R7 = m_byBuffer[5] >> 7;
			R4 &= 1;
			R7 &= 1;
			if (R4 == 0 && R7 == 0) {}
			if (R4 == 0 && R7 == 1)
			{
				m_byBuffer[5] += 0x10;
				m_byBuffer[5] -= 0x80;
			}
			if (R4 == 1 && R7 == 0)
			{
				m_byBuffer[5] -= 0x10;
				m_byBuffer[5] += 0x80;
			}
			if (R4 == 1 && R7 == 1) {}
			tmp = m_byBuffer[0xA];
			__asm ROL tmp, 4
			m_byBuffer[0xA] = tmp;
			m_byBuffer[6] ^= 0x98;
			m_byBuffer[9] ^= 0x21;
			tmp = m_byBuffer[9];
			m_byBuffer[9] = m_byBuffer[3];
			m_byBuffer[3] = tmp;
			tmp = m_byBuffer[5];
			__asm ROL tmp, 2
			m_byBuffer[5] = tmp;
			m_byBuffer[9] ^= 0xAF;
			tmp = m_byBuffer[4];
			__asm ROL tmp, 3
			m_byBuffer[4] = tmp;
			tmp = m_byBuffer[8];
			m_byBuffer[8] = m_byBuffer[9];
			m_byBuffer[9] = tmp;
		}
	}

}

void WebDecryp(BYTE* m_byBuffer)
{

}

void XorData(int iStart, int iEnd, BYTE* m_byBuffer)	// Good
{
	//if ( iStart < iEnd  )
	//{
	//	return;
	//}

	unsigned char byXorFilter[32];

	byXorFilter[0] = 0xAB;
	byXorFilter[1] = 0x11;
	byXorFilter[2] = 0xCD;
	byXorFilter[3] = 0xFE;
	byXorFilter[4] = 0x18;
	byXorFilter[5] = 0x23;
	byXorFilter[6] = 0xC5;
	byXorFilter[7] = 0xA3;
	byXorFilter[8] = 0xCA;
	byXorFilter[9] = 0x33;
	byXorFilter[10] = 0xC1;
	byXorFilter[11] = 0xCC;
	byXorFilter[12] = 0x66;
	byXorFilter[13] = 0x67;
	byXorFilter[14] = 0x21;
	byXorFilter[15] = 0xF3;
	byXorFilter[16] = 0x32;
	byXorFilter[17] = 0x12;
	byXorFilter[18] = 0x15;
	byXorFilter[19] = 0x35;
	byXorFilter[20] = 0x29;
	byXorFilter[21] = 0xFF;
	byXorFilter[22] = 0xFE;
	byXorFilter[23] = 0x1D;
	byXorFilter[24] = 0x44;
	byXorFilter[25] = 0xEF;
	byXorFilter[26] = 0xCD;
	byXorFilter[27] = 0x41;
	byXorFilter[28] = 0x26;
	byXorFilter[29] = 0x3C;
	byXorFilter[30] = 0x4E;
	byXorFilter[31] = 0x4D;

	for (int i = iStart; i != iEnd; i--)
	{
		m_byBuffer[i] ^= m_byBuffer[i - 1] ^ byXorFilter[i % 32];

	}
}

void GCCharacterInfo(LPOBJ lpObj)
{
	PMSG_SXUPPRONT pMsg;
	pMsg.h.set(0xEC, 0x25, sizeof(PMSG_SXUPPRONT));
	pMsg.h.size = sizeof(PMSG_SXUPPRONT);
	pMsg.Str = lpObj->Strength;
	pMsg.AddStr = lpObj->AddStrength;
	pMsg.Dex = lpObj->Dexterity;
	pMsg.AddDex = lpObj->AddDexterity;
	pMsg.Vit = lpObj->Vitality;
	pMsg.AddVit = lpObj->AddVitality;
	pMsg.Ene = lpObj->Energy;
	pMsg.AddEne = lpObj->AddEnergy;
	pMsg.Leadership = lpObj->Leadership;
	pMsg.AddLeadership = lpObj->AddLeadership;
	pMsg.mPrec = lpObj->m_MPSkillOpt.MPRecoveryRate + lpObj->MPRecovery;
	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(PMSG_SXUPPRONT));
}

void SXGetMosaic(int aIndex)//镶嵌
{
	PMSG_SXMOSAIC pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	LPOBJ lpObj = &gObj[aIndex];
	pMsg.h.set(0xEC, 0x26, sizeof(pMsg));
	pMsg.nAttributeAttackPowerMin_info = lpObj->ElementalDamageMin;
	pMsg.nAttributeAttackPowerMax_info = lpObj->ElementalDamageMax;
	pMsg.nAttributeAttackPowerPVP_Min_info = lpObj->ElementalDamageMin;
	pMsg.nAttributeAttackPowerPVP_Max_info = lpObj->ElementalDamageMax;
	pMsg.nAttributeAttackSuccessRatePVE_info = lpObj->ElementalAttackSuccessRate;
	pMsg.nAttributeAttackSuccessRatePVP_info = lpObj->ElementalAttackSuccessRate;
	pMsg.nAttributeDefencePowerPVE_info = lpObj->ElementalDefense;
	pMsg.nAttributeDefencePowerPVP_info = lpObj->ElementalDefense;
	pMsg.nAttributeDefenceSuccessRatePVE_info = lpObj->ElementalDefenseSuccessRate;
	pMsg.nAttributeDefenceSuccessRatePVP_info = lpObj->ElementalDefenseSuccessRate;

	pMsg.nRubyAttackType = lpObj->ElementalAttribute;//火
	pMsg.nRubyAttackValue = 0;

	pMsg.nEmeAttackType = 0;
	pMsg.nEmeAttackValue = 0;	//水
	pMsg.nSapphDefeseType = 0;
	pMsg.nSapphDefeseValue = 0;	//土
	pMsg.nTopaAttackType = 0;
	pMsg.nTopaAttackAttack = 0;	//风

	pMsg.nAttributeDamageInc_PVE_info = 0;
	pMsg.nAttributeDamageInc_PVP_info = 0;
	pMsg.nAttributeDamageAbsorbs_PVE_info = 0;
	pMsg.nAttributeDamageAbsorbs_PVP_info = 0;
	pMsg.nAttributeShieldAbsortion_info = 0;
	pMsg.nAttributeLifeAbsortion_info = 0;
	pMsg.nAttributeBleedingEffectOccur_info = lpObj->PentagramJewelOption.Bleeding;
	pMsg.nAttributeEffectsCausedParalysis_info = lpObj->PentagramJewelOption.Paralyzing;
	pMsg.nAttributeBoundEffectOccur_info = 0;
	pMsg.nAttributeWraithEffectOccur_info = 0;
	pMsg.nAttributeEffectsCausedBlindness_info = lpObj->PentagramJewelOption.Binding;
	pMsg.nAttributeResistantToStrongProperty_info = 0;
	pMsg.nAttributeResistantToAttributeAttacks_info = 0;
	pMsg.nAttributeAddBunNoErtelOptionEffect_info = 0;
	pMsg.nAttributeAddGaHoErtelOptionEffect_info = 0;
	pMsg.nAttributeCriticalDamageRate_info = lpObj->PentagramOption.AddElementalCriticalDamageRate;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(PMSG_SXMOSAIC));
}

void SXGetCharacterInfo(int aIndex)
{
	PMSG_SXCHARINFO pMsg;
	memset(&pMsg, 0, sizeof(pMsg));

	LPOBJ lpObj = &gObj[aIndex];

	pMsg.h.set(0xEC, 0x29, sizeof(PMSG_SXCHARINFO));

	pMsg.CriticalDamage = lpObj->CriticalDamage + lpObj->EffectOption.AddCriticalDamage;
	pMsg.ExcellentDamage = lpObj->ExcellentDamage + lpObj->EffectOption.AddExcellentDamage;
	pMsg.SkillDamageBonus = lpObj->SkillDamageBonus;
	pMsg.Defense = lpObj->Defense;
	pMsg.Str = lpObj->Strength;
	pMsg.AddStr = lpObj->AddStrength;
	pMsg.Dex = lpObj->Dexterity;
	pMsg.AddDex = lpObj->AddDexterity;
	pMsg.Vit = lpObj->Vitality;
	pMsg.AddVit = lpObj->AddVitality;
	pMsg.Energy = lpObj->Energy;
	pMsg.AddEnergy = lpObj->AddEnergy;
	pMsg.Leadership = lpObj->Leadership;
	pMsg.AddLeadership = lpObj->AddLeadership;
	pMsg.SDAttack = 0;
	pMsg.IgnoreShieldGaugeRate = lpObj->IgnoreShieldGaugeRate;
	pMsg.SDAttack1 = 0;
	pMsg.MoneyAmountDropRate = lpObj->MoneyAmountDropRate - 100;
	pMsg.IgnoreDefenseRate = (float)(lpObj->m_MPSkillOpt.IgnoreDefenseRate + lpObj->EffectOption.AddIgnoreDefenseRate);
	pMsg.HPRecovery = (float)lpObj->HPRecovery + lpObj->EffectOption.AddHPRecovery;
	pMsg.MPRecovery = (float)lpObj->MPRecovery + lpObj->EffectOption.AddMPRecovery;
	pMsg.ResistStunRate = (float)lpObj->ResistStunRate;
	pMsg.ShieldDamageReduction = (float)lpObj->ShieldDamageReduction;
	pMsg.fTripleDamageRationInfo = lpObj->m_MPSkillOpt.TripleDamageRate;
	pMsg.fMonsterDieGetHP_info = lpObj->HuntHP;
	pMsg.fMonsterDieGetMana_info = lpObj->HuntMP;
	pMsg.fMonsterDieGetSD_info = lpObj->HuntSD;
	pMsg.SDRecovery = (float)(lpObj->SDRecovery + lpObj->EffectOption.AddSDRecovery);
	pMsg.DefensiveFullMPRestoreRate = (float)lpObj->m_MPSkillOpt.DefensiveFullMPRestoreRate;
	pMsg.DefensiveFullHPRestoreRate = (float)lpObj->m_MPSkillOpt.DefensiveFullHPRestoreRate;
	pMsg.OffensiveFullSDRestoreRate = (float)lpObj->m_MPSkillOpt.OffensiveFullSDRestoreRate;
	pMsg.BPRecovery = (float)(lpObj->BPRecovery + lpObj->EffectOption.AddBPRecovery);
	pMsg.BlockRate = lpObj->BlockRate;
	pMsg.ParryRate = lpObj->ParryRate;
	pMsg.AbsorbLife = lpObj->AbsorbLife;
	pMsg.AbsorbSD = lpObj->AbsorbSD;
	pMsg.FullDamageReflectRate = (float)lpObj->m_MPSkillOpt.FullDamageReflectRate;
	pMsg.MPConsumptionRate = 100.0f - lpObj->MPConsumptionRate;
	pMsg.CriticalDamageRate = (float)(lpObj->m_MPSkillOpt.CriticalDamageRate + lpObj->EffectOption.AddCriticalDamageRate);
	pMsg.ExcellentDamageRate = (float)(lpObj->m_MPSkillOpt.ExcellentDamageRate + lpObj->EffectOption.AddExcellentDamageRate);
	pMsg.DoubleDamageRate = (float)lpObj->m_MPSkillOpt.DoubleDamageRate;

	for (int n = 0; n < MAX_DAMAGE_REDUCTION; n++)
	{
		pMsg.DamageReduction += lpObj->DamageReduction[n];
	}

	pMsg.DamageReduction += lpObj->EffectOption.AddDamageReduction;
	pMsg.BPConsumptionRate = lpObj->BPConsumptionRate;
	pMsg.DamageReflect = lpObj->DamageReflect + lpObj->EffectOption.AddDamageReflect;
	pMsg.StunRate = 0; /* gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MACE_MASTERY);*/
	pMsg.fWingDamageIncRate_info = lpObj->DamageIncRate;
	pMsg.fWingDamageAbsorb_info = lpObj->DamageDecRate;

	DataSend(lpObj->Index, (LPBYTE)&pMsg, sizeof(PMSG_SXCHARINFO));
}

void EventIcon(BYTE type, BYTE Switch)
{
	PMSG_TIME_ICON_SEND	ICpMsg;
	ICpMsg.header.set(0xF1, sizeof(ICpMsg));
	ICpMsg.sub = 0xFE;
	ICpMsg.type = type;
	ICpMsg.Switch = Switch;
	DataSendAll((BYTE*)&ICpMsg, ICpMsg.header.size);
}