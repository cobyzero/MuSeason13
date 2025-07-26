// InventoryEquipment.cpp: implementation of the CInventoryEquipment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InventoryEquipment.h"
#include "EffectManager.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "Util.h"
#include "SkillManager.h"

CInventoryEquipment gInventoryEquipment;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInventoryEquipment::CInventoryEquipment() // OK
{

}

CInventoryEquipment::~CInventoryEquipment() // OK
{

}

void CInventoryEquipment::MainProc() // OK
{
#if(GAMESERVER_UPDATE>=501)

	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) == 0)
		{
			continue;
		}

		LPOBJ lpObj = &gObj[n];

		for (int i = 0; i < MAX_EFFECT_LIST; i++)
		{
			if (lpObj->Effect[i].IsEffect() != 0)
			{
				if (lpObj->Effect[i].m_index == EFFECT_HAWK_FIGURINE || lpObj->Effect[i].m_index == EFFECT_GOAT_FIGURINE || lpObj->Effect[i].m_index == EFFECT_OAK_CHARM || lpObj->Effect[i].m_index == EFFECT_MAPLE_CHARM || lpObj->Effect[i].m_index == EFFECT_GOLDEN_OAK_CHARM || lpObj->Effect[i].m_index == EFFECT_GOLDEN_MAPLE_CHARM || lpObj->Effect[i].m_index == EFFECT_WORN_HORSESHOE)
				{
					EFFECT_INFO* lpInfo = gEffectManager.GetInfo(lpObj->Effect[i].m_index);

					if (lpInfo != 0)
					{
						if (this->CheckInventoryEquipment(lpObj, lpInfo->ItemIndex) == 0)
						{
							gEffectManager.DelEffect(lpObj, lpInfo->Index);
						}
					}
				}
			}
		}
	}

#endif
}

bool CInventoryEquipment::CheckInventoryEquipment(LPOBJ lpObj, int ItemIndex) // OK
{
#if(GAMESERVER_UPDATE>=501)

	for (int n = INVENTORY_WEAR_SIZE; n < INVENTORY_EXT4_SIZE; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			if (lpObj->Inventory[n].m_Index == ItemIndex)
			{
				if (lpObj->Inventory[n].m_Slot == 254)

				{
					return 1;
				}
			}
		}
	}

	return 0;

#else

	return 0;

#endif
}

void CInventoryEquipment::InsertInventoryEquipment(LPOBJ lpObj) // OK
{
	for (int n = INVENTORY_WEAR_SIZE; n < INVENTORY_EXT4_SIZE; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			if (lpObj->Inventory[n].m_Index >= GET_ITEM(13, 128) && lpObj->Inventory[n].m_Index <= GET_ITEM(13, 134))
			{
				if (lpObj->Inventory[n].m_Durability == 254)
				{
					EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[n].m_Index);

					if (lpInfo != 0)
					{
						if (gEffectManager.CheckEffectByGroup(lpObj, lpInfo->Group) == 0)
						{
							gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, 0, 0, 0, 0, 0);
						}
					}
				}
			}
		}
	}

	if (lpObj->m_btInvenPetPos >= INVENTORY_WEAR_SIZE && lpObj->m_btInvenPetPos < INVENTORY_SIZE)
	{
		EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index);

		if (lpInfo != 0)
		{
			if (lpObj->Inventory[lpObj->m_btInvenPetPos].m_JewelOfHarmonyOption == 1)
			{
				if (lpObj->m_wInvenPet != (WORD)-1)
				{
					return;
				}
				gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, 0, 0, 0, 0, 0);
				CItem* lpItem = &lpObj->Inventory[lpObj->m_btInvenPetPos];
				if (lpItem->m_Option1)
				{
					gSkillManager.AddSkillWeapon(lpObj, lpItem->m_SpecialIndex[0], lpItem->m_Level);
				}
				lpObj->m_wInvenPet = lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index;
			}
			
		}
		else
		{
			//gEffectManager.DelEffect(lpObj,lpInfo->Index);

			gSkillManager.GCSkillDelSend(lpObj->Index, gSkillManager.DelSkill(lpObj, lpObj->Inventory[lpObj->m_btInvenPetPos].m_SpecialIndex[0]), lpObj->Inventory[lpObj->m_btInvenPetPos].m_SpecialIndex[0], 0, 0);

			lpObj->m_btInvenPetPos = 0;
		}
	}
}

void CInventoryEquipment::CGInventoryEquipmentRecv(PMSG_INVENTORY_EQUIPMENT_RECV* lpMsg, int aIndex) // OK
{
#if(GAMESERVER_UPDATE>=501)

	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	PMSG_INVENTORY_EQUIPMENT_SEND pMsg;

	pMsg.header.set(0xBF, 0x20, sizeof(pMsg));

	pMsg.slot = lpMsg->slot;

	pMsg.result = -1;

	if (INVENTORY_FULL_RANGE(lpMsg->slot) == 0 || INVENTORY_WEAR_RANGE(lpMsg->slot) != 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}
#if(GAMESERVER_UPDATE>=902)//2017/02/28
	switch (lpObj->Inventory[lpMsg->slot].m_Index)
	{
	case GET_ITEM(13, 2):
	case GET_ITEM(13, 3):
	case GET_ITEM(13, 4):
	case GET_ITEM(13, 5):
	case GET_ITEM(13, 37):
		if (lpMsg->result == 254)
		{
			if (lpObj->Inventory[lpMsg->slot].m_Index == GET_ITEM(13, 4) && lpObj->Class != CLASS_DL)
			{
				goto DK_HORSE;
			}

			if (lpObj->Inventory[lpMsg->slot].m_Index == GET_ITEM(13, 5) && lpObj->Inventory[lpObj->m_btInvenPetDkSpiritPos].m_JewelOfHarmonyOption == 0)
			{
				lpObj->m_btInvenPetDkSpiritPos = lpMsg->slot;

				EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpMsg->slot].m_Index);

				if (lpInfo != 0)
				{
					lpObj->Inventory[lpMsg->slot].m_JewelOfHarmonyOption = 1;
					gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, 0, 0, 0, 0, 0);
					CItem* lpItem = &lpObj->Inventory[lpObj->m_btInvenPetDkSpiritPos];

					if (lpItem->m_Option1)
					{
						gSkillManager.AddSkillWeapon(lpObj, lpItem->m_SpecialIndex[0], lpItem->m_Level);
					}
					pMsg.result = -2;
				}
			}

			if (lpObj->Inventory[lpMsg->slot].m_Index != GET_ITEM(13, 5) && lpObj->Inventory[lpObj->m_btInvenPetPos].m_JewelOfHarmonyOption == 0)
			{
				lpObj->m_btInvenPetPos = lpMsg->slot;

				EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index);

				if (lpInfo != 0)
				{
					lpObj->Inventory[lpMsg->slot].m_JewelOfHarmonyOption = 1;
					gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, 0, 0, 0, 0, 0);
					CItem* lpItem = &lpObj->Inventory[lpObj->m_btInvenPetPos];
					if (lpItem->m_Option1)
					{
						gSkillManager.AddSkillWeapon(lpObj, lpItem->m_SpecialIndex[0], lpItem->m_Level);
					}
					lpObj->m_wInvenPet = lpObj->Inventory[lpObj->m_btInvenPetPos].m_Index;
					pMsg.result = -2;
				}
			}
		}

		if (lpMsg->result == 255)
		{
			if (lpObj->Inventory[lpMsg->slot].m_JewelOfHarmonyOption == 1)
			{
				if (lpObj->Inventory[lpMsg->slot].m_Index == GET_ITEM(13, 5))
				{
					lpObj->m_btInvenPetDkSpiritPos = lpMsg->slot;
				}
				else
				{
					lpObj->m_btInvenPetPos = lpMsg->slot;
				}
				EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpMsg->slot].m_Index);

				if (lpInfo != 0)
				{
					gEffectManager.DelEffect(lpObj, lpInfo->Index);
					gSkillManager.GCSkillDelSend(lpObj->Index, gSkillManager.DelSkill(lpObj, lpObj->Inventory[lpMsg->slot].m_SpecialIndex[0]), lpObj->Inventory[lpMsg->slot].m_SpecialIndex[0], 0, 0);
					lpObj->Inventory[lpMsg->slot].m_JewelOfHarmonyOption = 0;
					lpObj->m_wInvenPet = -1;
					pMsg.result = -1;
				}
			}
		}
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		gItemManager.GCItemChangeSend(lpObj->Index, lpMsg->slot);
		break;
	case GET_ITEM(13, 128):
	case GET_ITEM(13, 129):
	case GET_ITEM(13, 130):
	case GET_ITEM(13, 131):
	case GET_ITEM(13, 132):
	case GET_ITEM(13, 133):
	case GET_ITEM(13, 134):
	case GET_ITEM(13, 177):
	case GET_ITEM(13, 178):
	case GET_ITEM(13, 179):
		if (lpMsg->result == 254)
		{
			if (lpObj->Inventory[lpMsg->slot].m_Slot == 255)
			{
				EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpMsg->slot].m_Index);

				if (lpInfo != 0)
				{
					lpObj->Inventory[lpMsg->slot].m_Slot = 254;
					gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, 0, 0, 0, 0, 0);
					pMsg.result = -2;
				}
			}
		}

		if (lpMsg->result == 255)
		{
			if (lpObj->Inventory[lpMsg->slot].m_Slot == 254)
			{
				EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpMsg->slot].m_Index);
				if (lpInfo != 0)
				{
					lpObj->Inventory[lpMsg->slot].m_Slot = 255;

					gEffectManager.DelEffect(lpObj, lpInfo->Index);
					pMsg.result = -1;
				}
			}
		}
		break;
	}
#else
	if ((lpObj->Inventory[lpMsg->slot].m_Index < GET_ITEM(13, 128) || lpObj->Inventory[lpMsg->slot].m_Index > GET_ITEM(13, 134)) || lpObj->Inventory[lpMsg->slot].m_Durability == 0)
	{
		DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
		return;
	}

	if (lpMsg->result == 254)
	{
		if (lpObj->Inventory[lpMsg->slot].m_Durability == 255)
		{
			EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpMsg->slot].m_Index);

			if (lpInfo != 0)
			{
				lpObj->Inventory[lpMsg->slot].m_Durability = 254;
				gEffectManager.AddEffect(lpObj, 1, lpInfo->Index, 0, 0, 0, 0, 0);
				pMsg.result = -2;
			}
		}
	}

	if (lpMsg->result == 255)
	{
		if (lpObj->Inventory[lpMsg->slot].m_Durability == 254)
		{
			EFFECT_INFO* lpInfo = gEffectManager.GetInfoByItem(lpObj->Inventory[lpMsg->slot].m_Index);

			if (lpInfo != 0)
			{
				lpObj->Inventory[lpMsg->slot].m_Durability = 255;
				gEffectManager.DelEffect(lpObj, lpInfo->Index);
				pMsg.result = -1;
			}
		}
	}
#endif
DK_HORSE:
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

#endif

}


