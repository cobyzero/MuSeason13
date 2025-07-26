#include "stdafx.h"
#include "Util.h"
#include "ItemManager.h"
#include "SocketChaos.h"
#include "SocketItemOption.h"
#include "SocketItemType.h"
#include "ChaosBox.h"
#include "ServerInfo.h"
#include "MemScript.h"
#include "RheaGold.h"
CSocketChaos gSocketChaos;
CSocketChaos::CSocketChaos() // OK
{

}

CSocketChaos::~CSocketChaos() // OK
{

}
void CSocketChaos::Load(char* path) // OK
{

	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_ItemUpInfo.clear();
	this->m_SeedSphereUp.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_UP_INFO info;

					info.ItemUpBefore = lpMemScript->GetNumber();

					info.ItemUpAfter = lpMemScript->GetAsNumber();

					info.SaveLevel = lpMemScript->GetAsNumber();

					info.SaveOption = lpMemScript->GetAsNumber();

					this->m_ItemUpInfo.insert(std::pair<int, ITEM_UP_INFO>(info.ItemUpBefore, info));
				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
					SEED_SPHERE_UP info;

					info.level = lpMemScript->GetNumber();

					info.number = lpMemScript->GetAsNumber();

					info.money = lpMemScript->GetAsNumber();

					info.rate = lpMemScript->GetAsNumber();
					this->m_SeedSphereUp.insert(std::pair<int, SEED_SPHERE_UP>(info.level, info));
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;

}
bool CSocketChaos::GetSeedSphereUp(int index, SEED_SPHERE_UP* lpInfo)
{
	std::map<int, SEED_SPHERE_UP>::iterator it = this->m_SeedSphereUp.find(index);

	if (it == this->m_SeedSphereUp.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}
bool CSocketChaos::GetItemUpType(int index, ITEM_UP_INFO* lpInfo)
{
	std::map<int, ITEM_UP_INFO>::iterator it = this->m_ItemUpInfo.find(index);

	if (it == this->m_ItemUpInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}

void CSocketChaos::CGSocketChaosMix(PMSG_SOCKET_MIX_RECV* lpMsg, int aIndex)
{
	switch (lpMsg->info)
	{
	case SOCKET_MIX_ITEM_UP:
		this->UpgradeTheSlot((PMSG_UPGRADE_SLOT*)lpMsg, aIndex);
		break;
	case SOCKET_MIX_ITEM_CREATE_SEED:
		this->ExtractMix((PMSG_EXTRACT_RECV*)lpMsg, aIndex);
		break;
	case SOCKET_MIX_SOCKET_ITEM_CREATE_SEED_SPHERE:
		this->SocketItemCreateSeedSphereMix((PMSG_SEED_SPHERE_MIX_RECV*)lpMsg, aIndex);
		break;
	case SOCKET_MIX_SOCKET_ITEM_MOUNT_SEED_SPHERE:
		this->SocketItemMountSeedSphereMix((PMSG_MOUNT_SEND_SPHERE_MIX_RECV*)lpMsg, aIndex);
		break;
	case SOCKET_MIX_ITEM_EXTRACT_SEED_SPHERE:
		this->SocketItemExtractSeedSphereMix((PMSG_EXTRACT_SEED_SPHERE_MIX_RECV*)lpMsg, aIndex);
		break;
	case SOCKET_MIX_SEED_SPHERE_LEVEL_UP:
		this->SocketSeedSphereLevelUpMix((PMSG_SEED_SPHERE_LEVLE_PRCV*)lpMsg, aIndex);
		break;
	case SOCKET_MIX_SOCKET_DEL:
		this->SocketItemDelSocket((PMSG_SOCKET_DEL_RECV*)lpMsg, aIndex);
		break;
	}

	return;
}

void CSocketChaos::SocketSeedSphereLevelUpMix(PMSG_SEED_SPHERE_LEVLE_PRCV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int SeedSphereCount1 = 0;
	int SeedSphereCount2 = 0;
	int SeedSphereLevelCount = 0;
	int RuneCount = 0;
	int level = 0;
	SEED_SPHERE_UP UpSeedSphere;
	ITEM_INFO ItemInfo;

	gItemManager.GetInfo(lpObj->Inventory[lpMsg->SeedSphere1].m_Index, &ItemInfo);

	GetSeedSphereUp(ItemInfo.Level + 1, &UpSeedSphere);
	if ((lpObj->Inventory[lpMsg->SeedSphere1].m_Index >= GET_ITEM(12, 100) && lpObj->Inventory[lpMsg->SeedSphere1].m_Index <= GET_ITEM(12, 129)) ||
		(lpObj->Inventory[lpMsg->SeedSphere1].m_Index >= GET_ITEM(12, 311) && lpObj->Inventory[lpMsg->SeedSphere1].m_Index <= GET_ITEM(12, 334)))
	{
		SeedSphereCount1++;
	}
	if ((lpObj->Inventory[lpMsg->SeedSphere2].m_Index >= GET_ITEM(12, 100) && lpObj->Inventory[lpMsg->SeedSphere2].m_Index <= GET_ITEM(12, 129)) ||
		(lpObj->Inventory[lpMsg->SeedSphere2].m_Index >= GET_ITEM(12, 311) && lpObj->Inventory[lpMsg->SeedSphere2].m_Index <= GET_ITEM(12, 334)))
	{
		SeedSphereCount2++;
	}
	//if(lpObj->Inventory[lpMsg->SeedSphere1].m_Level == lpObj->Inventory[lpMsg->SeedSphere2].m_Level)
	//{
	//	SeedSphereLevelCount++;
	//}
	if (lpObj->Inventory[lpMsg->Rune].m_Durability >= UpSeedSphere.number)
	{
		RuneCount++;
	}
	if (SeedSphereCount1 != 1 || SeedSphereCount2 != 1 || RuneCount != 1)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = UpSeedSphere.money;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->ChaosSuccessRate = UpSeedSphere.rate;

	lpObj->Inventory[lpMsg->Rune].m_Durability -= UpSeedSphere.number;

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->Inventory[lpMsg->SeedSphere1];

		item.m_Index += (ItemInfo.Level == 5) ? 187 : 6;


		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->SeedSphere1);

		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->SeedSphere1, 1);

		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->SeedSphere2);

		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->SeedSphere2, 1);

		gItemManager.GCItemDurSend(lpObj->Index, lpMsg->Rune, (BYTE)lpObj->Inventory[lpMsg->Rune].m_Durability, 0);

		BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

		PMSG_RHEA_ITEM_BUY_SEND pMsg;

		pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

		pMsg.result = Result;

		gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
	else
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 226, 0);

		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->SeedSphere2);

		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->SeedSphere2, 1);

		gItemManager.GCItemDurSend(lpObj->Index, lpMsg->Rune, (BYTE)lpObj->Inventory[lpMsg->Rune].m_Durability, 0);
	}

}

void CSocketChaos::SocketItemExtractSeedSphereMix(PMSG_EXTRACT_SEED_SPHERE_MIX_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int ChaosCount = 0;
	int HarmonyCount = 0;
	int GuardianCount = 0;
	int ItemCount = 0;
	if (lpMsg->SocketNum >= MAX_SOCKET_ITEM_OPTION_TABLE)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}
	for (int n = 0; n < 5; n++)
	{
		if (lpObj->Inventory[lpMsg->Chaos[n]].m_Index == GET_ITEM(12, 15))
		{
			ChaosCount++;
		}
	}
	for (int n = 0; n < 5; n++)
	{
		if (lpObj->Inventory[lpMsg->Harmony[n]].m_Index == GET_ITEM(14, 42))
		{
			HarmonyCount++;
		}
	}
	if (lpObj->Inventory[lpMsg->Guardian].m_Index == GET_ITEM(14, 31))
	{
		GuardianCount++;
	}
	if (lpObj->Inventory[lpMsg->Item].IsSocketItem() != 0 && lpObj->Inventory[lpMsg->Item].m_SocketOption[lpMsg->SocketNum] != SOCKET_ITEM_OPTION_EMPTY)
	{
		ItemCount++;
	}
	if (ChaosCount != 5 || HarmonyCount != 5 || GuardianCount != 1 || ItemCount != 1)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->ChaosSuccessRate = gServerInfo.m_SocketItemExtractSeedSphereRate;

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem iSeedSphere = gSocketItemOption.GetSocketExtractSeedSphere(&lpObj->Inventory[lpMsg->Item], lpMsg->SocketNum);

		CItem item = lpObj->Inventory[lpMsg->Item];

		item.m_SocketOption[lpMsg->SocketNum] = SOCKET_ITEM_OPTION_EMPTY;
		int iSet = 0;
		for (int s = 0; s < 5; s++)
		{
			if (item.m_SocketOption[s] != 255 && item.m_SocketOption[s] != 254)
			{
				if (s == (MAX_SOCKET_ITEM_OPTION_TABLE - 1))
				{
					item.m_NewOption = 0;
				}
				else
				{
					iSet |= (((item.m_SocketOption[s] + 0x100) >> 2) & 0xC0) >> 2 * s;
				}
			}
			item.m_SetOption = iSet;
		}

		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Guardian);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Guardian, 1);
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Item);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Item, 1);

		for (int n = 0; n < 5; n++)
		{
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos[n], 1);
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Harmony[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Harmony[n], 1);
		}

		BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

		PMSG_RHEA_ITEM_BUY_SEND pMsg;

		pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

		pMsg.result = Result;

		gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

		BYTE Result1 = gItemManager.InventoryInsertItem(lpObj->Index, iSeedSphere);

		pMsg.result = Result1;

		gItemManager.ItemByteConvert(pMsg.ItemInfo, iSeedSphere);

		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
	else
	{
		gSocketItemOption.GetSocketExtractSeedSphere(&lpObj->Inventory[lpMsg->Item], lpMsg->SocketNum);

		CItem item = lpObj->Inventory[lpMsg->Item];

		item.m_SocketOption[lpMsg->SocketNum] = SOCKET_ITEM_OPTION_EMPTY;

		int iSet = 0;
		for (int s = 0; s < 5; s++)
		{
			if (item.m_SocketOption[s] != 255 && item.m_SocketOption[s] != 254)
			{
				if (s == (MAX_SOCKET_ITEM_OPTION_TABLE - 1))
				{
					item.m_NewOption = 0;
				}
				else
				{
					iSet |= (((item.m_SocketOption[s] + 0x100) >> 2) & 0xC0) >> 2 * s;
				}
			}
			item.m_SetOption = iSet;
		}
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Guardian);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Guardian, 1);
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Item);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Item, 1);

		for (int n = 0; n < 5; n++)
		{
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos[n], 1);
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Harmony[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Harmony[n], 1);
		}

		BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

		PMSG_RHEA_ITEM_BUY_SEND pMsg;

		pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

		pMsg.result = Result;

		gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

		gChaosBox.GCChaosMixSend(lpObj->Index, 226, 0);
	}
}

void CSocketChaos::SocketItemMountSeedSphereMix(PMSG_MOUNT_SEND_SPHERE_MIX_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpMsg->SocketNum >= MAX_SOCKET_ITEM_OPTION_TABLE)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}
	int ChaosCount = 0;
	int CreationCount = 0;
	int SeedSphereCount = 0;
	int SeedSphereType = 0;
	int SeedSphereLevel = 0;
	int SeedSphereOption = 0;
	int ItemCount = 0;
	if (lpObj->Inventory[lpMsg->Chaos].m_Index == GET_ITEM(12, 15))
	{
		ChaosCount++;
	}
	if (lpObj->Inventory[lpMsg->Creation].m_Index == GET_ITEM(14, 22))
	{
		CreationCount++;
	}
	if (lpObj->Inventory[lpMsg->SeedSphere].m_Index >= GET_ITEM(12, 100) && lpObj->Inventory[lpMsg->SeedSphere].m_Index <= GET_ITEM(12, 129))
	{
		SeedSphereCount++;
		SeedSphereType = (lpObj->Inventory[lpMsg->SeedSphere].m_Index - GET_ITEM(12, 100)) % MAX_SOCKET_ITEM_OPTION_TYPE;
		SeedSphereLevel = (lpObj->Inventory[lpMsg->SeedSphere].m_Index - GET_ITEM(12, 100)) / MAX_SOCKET_ITEM_OPTION_TYPE;
		SeedSphereOption = lpObj->Inventory[lpMsg->SeedSphere].m_Level;
	}
	if (lpObj->Inventory[lpMsg->SeedSphere].m_Index >= GET_ITEM(12, 311) && lpObj->Inventory[lpMsg->SeedSphere].m_Index <= GET_ITEM(12, 340))
	{
		SeedSphereCount++;
		SeedSphereType = (lpObj->Inventory[lpMsg->SeedSphere].m_Index - GET_ITEM(12, 311)) % MAX_SOCKET_ITEM_OPTION_TYPE;
		SeedSphereLevel = ((lpObj->Inventory[lpMsg->SeedSphere].m_Index - GET_ITEM(12, 311)) / MAX_SOCKET_ITEM_OPTION_TYPE) + 5;
		SeedSphereOption = lpObj->Inventory[lpMsg->SeedSphere].m_Level;
	}
	if (gSocketItemType.CheckSocketItemType(lpObj->Inventory[lpMsg->Item].m_Index) != 0 && lpObj->Inventory[lpMsg->Item].m_SocketOption[lpMsg->SocketNum] == SOCKET_ITEM_OPTION_EMPTY)
	{
		ItemCount++;
	}

	if (ChaosCount != 1 || CreationCount != 1 || SeedSphereCount != 1 || ItemCount != 1)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);
	int SocketOption;
	if (lpObj->Inventory[lpMsg->SeedSphere].m_Index >= GET_ITEM(12, 311) && lpObj->Inventory[lpMsg->SeedSphere].m_Index <= GET_ITEM(12, 340))
	{
		//for ( int i = 0; i < MAX_SOCKET_ITEM_OPTION_TABLE; ++i )
  //    {
		//  if (lpObj->Inventory[lpMsg->Item].m_SocketOption[i] != 255 )
  //      {
  //        int v25 = lpObj->Inventory[lpMsg->Item].m_SocketOption[i]+ 50 * SeedSphereLevel;
  //        if ( v25 >= 254 && (SeedSphereLevel+1) > 5 )
  //          v25 += 2;
  //        if ( i == 4 )
  //           lpObj->Inventory[lpMsg->Item].m_NewOption |= (v25 >> 4) & 0x30;
  //        else
  //         lpObj->Inventory[lpMsg->Item].m_SetOption |= ((v25 >> 2) & 0xC0) >> 2 * i;
  //      }
  //    }
		int SetOption = 0;
		SocketOption = gSocketItemOption.GetSocketItemOptionSXI(&lpObj->Inventory[lpMsg->Item], (SeedSphereType + 1), SeedSphereOption, SeedSphereLevel);

		if ((SeedSphereLevel + 1) > 5 && SocketOption >= 254)
		{
			SocketOption += 2;
		}
		//if(SeedSphereLevel<=4)
		if (lpMsg->SocketNum == (MAX_SOCKET_ITEM_OPTION_TABLE - 1))
		{
			//SetOption = (SocketOption>>4) &0x30;
			lpObj->Inventory[lpMsg->Item].m_NewOption |= (SocketOption >> 4) & 0x30;
		}
		else
		{
			//SetOption = ((SocketOption>>2) &0xC0) >> (lpMsg->SocketNum << 1);
			//lpObj->Inventory[lpMsg->Item].m_SetOption |= SetOption;
			lpObj->Inventory[lpMsg->Item].m_SetOption |= ((SocketOption >> 2) & 0xC0) >> 2 * lpMsg->SocketNum;
		}
	}
	else
	{
		SocketOption = gSocketItemOption.GetSocketItemOption(&lpObj->Inventory[lpMsg->Item], (SeedSphereType + 1), SeedSphereOption, SeedSphereLevel);
	}

	if (SocketOption == SOCKET_ITEM_OPTION_NONE)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 0, 0);
		return;
	}

	CItem item = lpObj->Inventory[lpMsg->Item];

	item.m_NewOption = lpObj->Inventory[lpMsg->Item].m_NewOption;

	item.m_SetOption = lpObj->Inventory[lpMsg->Item].m_SetOption;

	item.m_SocketOption[lpMsg->SocketNum] = SocketOption;

	item.m_SocketOptionBonus = gSocketItemOption.GetSocketItemBonusOption(&item);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Creation);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Creation, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Item);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Item, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->SeedSphere);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->SeedSphere, 1);


	BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

	PMSG_RHEA_ITEM_BUY_SEND pMsg;

	pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

	pMsg.result = Result;

	gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

}
void CSocketChaos::SocketItemCreateSeedSphereMix(PMSG_SEED_SPHERE_MIX_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int ChaosCount = 0;
	int CreationCount = 0;
	int SeedCount = 0;
	int SeedType = 0;
	int SeedLevel = 0;
	int SphereCount = 0;
	int SphereType = 0;
	int ItemMoney = 0;

	if (lpObj->Inventory[lpMsg->Chaos].m_Index == GET_ITEM(12, 15))
	{
		ChaosCount++;
	}

	if (lpObj->Inventory[lpMsg->Creation].m_Index == GET_ITEM(14, 22))
	{
		CreationCount++;
	}

	if (lpObj->Inventory[lpMsg->Seed].m_Index >= GET_ITEM(12, 60) && lpObj->Inventory[lpMsg->Seed].m_Index <= GET_ITEM(12, 66))
	{
		SeedCount++;
		SeedType = lpObj->Inventory[lpMsg->Seed].m_Index - GET_ITEM(12, 60);
		SeedLevel = lpObj->Inventory[lpMsg->Seed].m_Level;
		ItemMoney += lpObj->Inventory[lpMsg->Seed].m_BuyMoney;
	}

	if (lpObj->Inventory[lpMsg->Sphere].m_Index >= GET_ITEM(12, 70) && lpObj->Inventory[lpMsg->Sphere].m_Index <= GET_ITEM(12, 74))
	{
		SphereCount++;
		SphereType = lpObj->Inventory[lpMsg->Sphere].m_Index - GET_ITEM(12, 70);
		ItemMoney += lpObj->Inventory[lpMsg->Sphere].m_BuyMoney;
	}

	if (ChaosCount != 1 || CreationCount != 1 || SeedCount != 1 || SphereCount != 1)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	lpObj->ChaosMoney = 1000000;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Creation);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Creation, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Seed);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Seed, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Sphere);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Sphere, 1);

	CItem item;
	item.m_Index = (GET_ITEM(12, 100) + (SphereType * 6) + SeedType);
	item.m_Level = SeedLevel;

	BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

	PMSG_RHEA_ITEM_BUY_SEND pMsg;

	pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

	pMsg.result = Result;

	gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

}
void CSocketChaos::ExtractMix(PMSG_EXTRACT_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	int ChaosCount = 0;
	int CreationCount = 0;
	int HarmonyCount = 0;
	int ExcItemCount = 0;
	int SetItemCount = 0;
	int ItemMoney = 0;
	if (lpObj->Inventory[lpMsg->Chaos].m_Index == GET_ITEM(12, 15))
	{
		ChaosCount++;
	}
	if (lpObj->Inventory[lpMsg->Creation].m_Index == GET_ITEM(14, 22))
	{
		CreationCount++;
	}
	if (lpObj->Inventory[lpMsg->ExcItem].IsExcItem() != 0 && lpObj->Inventory[lpMsg->ExcItem].m_Level >= 4)
	{
		ExcItemCount++;
	}
	if (lpObj->Inventory[lpMsg->SetItem].IsSetItem() != 0 && lpObj->Inventory[lpMsg->SetItem].m_Level >= 4)
	{
		SetItemCount++;
	}
	if (lpObj->Inventory[lpMsg->Harmony].m_Index == GET_ITEM(14, 42))
	{
		HarmonyCount++;
	}
	if (ChaosCount != 1 || CreationCount != 1 || HarmonyCount != 1 || ExcItemCount != 1 || SetItemCount != 1)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}
	lpObj->ChaosMoney = 1000000;

	if (lpObj->Money < ((DWORD)lpObj->ChaosMoney))
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= lpObj->ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	int SocketOptionType;

	int SocketOptionIndex;

	int SocketOptionValue;
	for (;;)
	{
		SocketOptionType = 1 + (GetLargeRand() % MAX_SOCKET_ITEM_OPTION_TYPE);

		SocketOptionIndex = gSocketItemOption.GetSocketItemRandomOption(SocketOptionType);

		SocketOptionValue = gSocketItemOption.GetSocketItemSeedOption(SocketOptionIndex, SocketOptionType);

		if (SocketOptionIndex != -1 || SocketOptionValue != -1)
		{
			break;
		}
	}
	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Creation);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Creation, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->ExcItem);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->ExcItem, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Harmony);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Harmony, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->SetItem);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->SetItem, 1);

	CItem item;

	item.m_Index = (GET_ITEM(12, 60) + (SocketOptionType - 1));

	item.m_Level = SocketOptionValue;

	BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

	PMSG_RHEA_ITEM_BUY_SEND pMsg;

	pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

	pMsg.result = Result;

	gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);

}
void CSocketChaos::UpgradeTheSlot(PMSG_UPGRADE_SLOT* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	ITEM_UP_INFO ItemType;
	int ChaosCount = 0;
	int ItemCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int Paper = 0;

	if (lpObj->Inventory[lpMsg->Chaos].m_Index == GET_ITEM(12, 15))
	{
		ChaosCount++;
	}

	if (this->GetItemUpType(lpObj->Inventory[lpMsg->ItemIndex].m_Index, &ItemType) != 0)
	{
		ItemCount++;
	}
	for (int i = 0; i < 5; i++)
	{
		if (lpObj->Inventory[lpMsg->Bless[i]].m_Index == GET_ITEM(14, 13))
		{
			BlessCount++;
		}
	}
	for (int n = 0; n < 5; n++)
	{
		if (lpObj->Inventory[lpMsg->Soul[n]].m_Index == GET_ITEM(14, 14))
		{
			SoulCount++;
		}
	}

	if (lpObj->Inventory[lpMsg->Paper].m_Index == GET_ITEM(14, 364))
	{
		Paper++;
	}

	if (ChaosCount != 1 || ItemCount != 1 || BlessCount != 5 || SoulCount != 5 || Paper != 1)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	int ChaosMoney = 10000000;

	if (lpObj->Money < ((DWORD)ChaosMoney))
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 2, 0);
		return;
	}

	lpObj->Money -= ChaosMoney;

	GCMoneySend(lpObj->Index, lpObj->Money);

	lpObj->ChaosSuccessRate = gServerInfo.m_SocketItemUpRate;

	if ((GetLargeRand() % 100) < lpObj->ChaosSuccessRate)
	{
		CItem item = lpObj->Inventory[lpMsg->ItemIndex];

		item.m_Index = ItemType.ItemUpAfter;

		if (ItemType.SaveLevel == 0)
		{
			item.m_Level = 0;
		}

		if (ItemType.SaveOption == 0)
		{
			item.m_Option3 = 0;
		}
		for (int n = 0; n < BlessCount; n++)
		{
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Bless[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Bless[n], 1);
		}
		for (int n = 0; n < SoulCount; n++)
		{
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Soul[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Soul[n], 1);
		}

		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->ItemIndex);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->ItemIndex, 1);
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Paper);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Paper, 1);
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos, 1);

		BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

		PMSG_RHEA_ITEM_BUY_SEND pMsg;

		pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

		pMsg.result = Result;

		gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
	else
	{
		for (int n = 0; n < BlessCount; n++)
		{
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Bless[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Bless[n], 1);
		}
		for (int n = 0; n < SoulCount; n++)
		{
			gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Soul[n]);
			gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Soul[n], 1);
		}
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->ItemIndex);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->ItemIndex, 1);
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Paper);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Paper, 1);
		gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos);
		gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos, 1);

		gChaosBox.GCChaosMixSend(lpObj->Index, 226, 0);
	}
}

#if(GAMESERVER_UPDATE>=1200)//2017/11/11
void CSocketChaos::SocketItemDelSocket(PMSG_SOCKET_DEL_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	int ChaosCount = 0;

	int ItemCount = 0;

	int CreationCount = 0;

	if (lpObj->Inventory[lpMsg->Chaos].m_Index == GET_ITEM(12, 15))
	{
		ChaosCount++;
	}

	if (lpObj->Inventory[lpMsg->Item].IsSocketItem() != 0)
	{
		for (int n = 0; n < 5; n++)
		{
			if (lpObj->Inventory[lpMsg->Item].m_SocketOption[n] != 0xFF)
			{
				ItemCount++;
			}
		}

	}

	if (lpObj->Inventory[lpMsg->Creation].m_Index == GET_ITEM(14, 22))
	{
		CreationCount++;
	}
	if (ChaosCount != 1 || ItemCount < 3 || CreationCount != 1)
	{
		gChaosBox.GCChaosMixSend(lpObj->Index, 7, 0);
		return;
	}

	CItem item = lpObj->Inventory[lpMsg->Item];

	item.m_SocketOption[3] = 0xFF;

	item.m_SocketOption[4] = 0xFF;

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Chaos);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Chaos, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Creation);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Creation, 1);

	gItemManager.InventoryDelItem(lpObj->Index, lpMsg->Item);

	gItemManager.GCItemDeleteSend(lpObj->Index, lpMsg->Item, 1);

	BYTE Result = gItemManager.InventoryInsertItem(lpObj->Index, item);

	PMSG_RHEA_ITEM_BUY_SEND pMsg;

	pMsg.header.setE(0xF3, 0x14, sizeof(pMsg));

	pMsg.result = Result;

	gItemManager.ItemByteConvert(pMsg.ItemInfo, item);

	DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
}
#endif