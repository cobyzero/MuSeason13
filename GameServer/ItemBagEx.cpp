#include "stdafx.h"
#include "ItemBagEx.h"
#include "RheaGold.h"
#include "DSProtocol.h"
#include "ItemOptionRate.h"
#include "Map.h"
#include "MemScript.h"
#include "Party.h"
#include "RandomManager.h"
#include "Notice.h"
#include "Message.h"
#include "Util.h"
#include "MuunSystem.h"
#include "Log.h"

CItemBagEx::CItemBagEx() // OK
{
	this->m_ItemBagInfo.clear();

	this->m_ItemBagItemInfo.clear();
}

CItemBagEx::~CItemBagEx() // OK
{

}

void CItemBagEx::Load(char* path) // OK
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

	this->m_ItemBagInfo.clear();

	this->m_ItemBagItemInfo.clear();

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
				if (section == 3)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_EX_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.DropRate = lpMemScript->GetAsNumber();

					this->m_ItemBagInfo.insert(std::pair<int, ITEM_BAG_EX_INFO>(info.Index, info));
				}
				else if (section == 4)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_EX_DROP_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.Section = lpMemScript->GetAsNumber();

					info.SectionRate = lpMemScript->GetAsNumber();

					info.MoneyAmount = lpMemScript->GetAsNumber();

					info.OptionValue = lpMemScript->GetAsNumber();

					for (int n = 0; n < MAX_CLASS; n++) { info.RequireClass[n] = lpMemScript->GetAsNumber(); }

					std::map<int, ITEM_BAG_EX_INFO>::iterator it = this->m_ItemBagInfo.find(info.Index);

					if (it != this->m_ItemBagInfo.end())
					{
						it->second.DropInfo.push_back(info);
					}
				}
				else if ((section >= 5 && section <= 9) || (section >= 12 && section <= 19))
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_EX_ITEM_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.Level = lpMemScript->GetAsNumber();

					info.Grade = lpMemScript->GetAsNumber();

					info.Option0 = lpMemScript->GetAsNumber();

					info.Option1 = lpMemScript->GetAsNumber();

					info.Option2 = lpMemScript->GetAsNumber();

					info.Option3 = lpMemScript->GetAsNumber();

					info.Option4 = lpMemScript->GetAsNumber();

					info.Option5 = lpMemScript->GetAsNumber();

					info.Option6 = lpMemScript->GetAsNumber();

					info.Duration = lpMemScript->GetAsNumber();

					std::map<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>::iterator it = this->m_ItemBagItemInfo.find(section);

					if (it == this->m_ItemBagItemInfo.end())
					{
						this->m_ItemBagItemInfo.insert(std::pair<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>(section, std::vector<ITEM_BAG_EX_ITEM_INFO>(1, info)));
					}
					else
					{
						it->second.push_back(info);
					}

				}
				else if (section >= 10 && section <= 11)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_BAG_ELEMENT_ITEM_INFO info;

					info.Index = lpMemScript->GetNumber();

					info.Level = lpMemScript->GetAsNumber();

					info.Element = lpMemScript->GetAsNumber();

					info.Dur = lpMemScript->GetAsNumber();

					info.Socket[0] = lpMemScript->GetAsNumber();
					info.Socket[1] = lpMemScript->GetAsNumber();
					info.Socket[2] = lpMemScript->GetAsNumber();
					info.Socket[3] = lpMemScript->GetAsNumber();
					info.Socket[4] = lpMemScript->GetAsNumber();

					std::map<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>::iterator it = this->m_ItemElementBagItemInfo.find(section);

					if (it == this->m_ItemElementBagItemInfo.end())
					{
						this->m_ItemElementBagItemInfo.insert(std::pair<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>(section, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>(1, info)));
					}
					else
					{
						it->second.push_back(info);
					}
				}
				else
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}
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

bool CItemBagEx::GetItem(LPOBJ lpObj, CItem* lpItem) // OK
{
	for (std::map<int, ITEM_BAG_EX_INFO>::iterator it = this->m_ItemBagInfo.begin(); it != this->m_ItemBagInfo.end(); it++)
	{
		if ((GetLargeRand() % 10000) < it->second.DropRate)
		{
			CRandomManager RandomManager;

			for (std::vector<ITEM_BAG_EX_DROP_INFO>::iterator DropInfo = it->second.DropInfo.begin(); DropInfo != it->second.DropInfo.end(); DropInfo++)
			{
				if (this->CheckDropClass(lpObj, (DropInfo->OptionValue & 2), DropInfo->RequireClass) != 0)
				{
					RandomManager.AddElement((int)(&(*DropInfo)), DropInfo->SectionRate);
				}
			}

			ITEM_BAG_EX_DROP_INFO* lpItemBagDropInfo = 0;

			if (RandomManager.GetRandomElement((int*)&lpItemBagDropInfo) == 0)
			{
				return 0;
			}

			std::map<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>::iterator ItemInfo = this->m_ItemBagItemInfo.find(lpItemBagDropInfo->Section);

			if (ItemInfo == this->m_ItemBagItemInfo.end())
			{
				return 0;
			}

			if (ItemInfo->second.empty() != 0)
			{
				return 0;
			}

			ITEM_BAG_EX_ITEM_INFO* lpItemBagItemInfo = &ItemInfo->second[GetLargeRand() % ItemInfo->second.size()];

			WORD ItemIndex = lpItemBagItemInfo->Index;
			BYTE ItemLevel = lpItemBagItemInfo->Level;
			BYTE ItemOption1 = 0;
			BYTE ItemOption2 = 0;
			BYTE ItemOption3 = 0;
			BYTE ItemNewOption = 0;
			BYTE ItemSetOption = 0;
			BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

			gItemOptionRate.GetItemOption0(lpItemBagItemInfo->Option0, &ItemLevel);

			gItemOptionRate.GetItemOption1(lpItemBagItemInfo->Option1, &ItemOption1);

			gItemOptionRate.GetItemOption2(lpItemBagItemInfo->Option2, &ItemOption2);

			gItemOptionRate.GetItemOption3(lpItemBagItemInfo->Option3, &ItemOption3);

			gItemOptionRate.GetItemOption4(lpItemBagItemInfo->Option4, &ItemNewOption);

			gItemOptionRate.GetItemOption5(lpItemBagItemInfo->Option5, &ItemSetOption);

			gItemOptionRate.GetItemOption6(lpItemBagItemInfo->Option6, &ItemSocketOption[0]);

			gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);

			gItemOptionRate.MakeSetOption(ItemIndex, ItemSetOption, &ItemSetOption);

			gItemOptionRate.MakeSocketOption(ItemIndex, ItemSocketOption[0], &ItemSocketOption[0]);

			lpItem->m_Level = ItemLevel;

			lpItem->Convert(ItemIndex, ItemOption1, ItemOption2, ItemOption3, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF);

			return 1;
		}
	}

	return 0;
}

bool CItemBagEx::DropItem(LPOBJ lpObj, int map, int x, int y) // OK
{
	int ElementTypeForMultiple = -1;

	for (std::map<int, ITEM_BAG_EX_INFO>::iterator it = this->m_ItemBagInfo.begin(); it != this->m_ItemBagInfo.end(); it++)
	{
		if ((GetLargeRand() % 10000) < it->second.DropRate)
		{
			CRandomManager RandomManager;

			for (std::vector<ITEM_BAG_EX_DROP_INFO>::iterator DropInfo = it->second.DropInfo.begin(); DropInfo != it->second.DropInfo.end(); DropInfo++)
			{
				if (this->CheckDropClass(lpObj, (DropInfo->OptionValue & 2), DropInfo->RequireClass) != 0)
				{
					RandomManager.AddElement((int)(&(*DropInfo)), DropInfo->SectionRate);
				}
			}

			ITEM_BAG_EX_DROP_INFO* lpItemBagDropInfo = 0;

			if (it->second.DropInfo[0].Section >= 10 && it->second.DropInfo[0].Section <= 11)
			{
				if (RandomManager.GetRandomElement((int*)&lpItemBagDropInfo) == 0)
				{
					continue;
				}

				int px = x;
				int py = y;

				if (it != this->m_ItemBagInfo.begin() || (gMap[map].CheckAttr(px, py, 4) != 0 || gMap[map].CheckAttr(px, py, 8) != 0))
				{
					if (this->GetRandomItemDropLocation(map, &px, &py, 2, 2, 10) == 0)
					{
						px = lpObj->X;
						py = lpObj->Y;
					}
				}

				std::map<int, std::vector<ITEM_BAG_ELEMENT_ITEM_INFO>>::iterator ItemInfo = this->m_ItemElementBagItemInfo.find(lpItemBagDropInfo->Section);

				if (ItemInfo == this->m_ItemElementBagItemInfo.end())
				{
					gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
					continue;
				}

				if (ItemInfo->second.empty() != 0)
				{
					gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
					continue;
				}

				ITEM_BAG_ELEMENT_ITEM_INFO* lpItemElementBagItemInfo = &ItemInfo->second[GetLargeRand() % ItemInfo->second.size()];

				WORD ItemIndex = lpItemElementBagItemInfo->Index;
				BYTE ItemLevel = lpItemElementBagItemInfo->Level;
				BYTE ItemElement;

				if (ElementTypeForMultiple == -1 && it->second.DropInfo[0].Section == 10)
					ItemElement = lpItemElementBagItemInfo->Element | 16;
				else
					ItemElement = ElementTypeForMultiple;

				if (it->second.DropInfo[0].Section == 10)
					ElementTypeForMultiple = ItemElement;

				BYTE ItemDur = lpItemElementBagItemInfo->Dur;
				BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };				

				memcpy(ItemSocketOption, lpItemElementBagItemInfo->Socket, sizeof(lpItemElementBagItemInfo->Socket));

				gPentagramSystem.GetPentagramRandomJewelOption(ItemIndex, 1, &ItemSocketOption[0]);

				if (lpObj->MouseItemUp == 1)
				{
					ITEM_INFO ItemInfo;

					if (gItemManager.GetInfo(ItemIndex, &ItemInfo) == 0)
					{
						return 0;
					}

					CItem item;
					item.Clear();
					item.m_Index = ItemIndex;
					item.m_Level = ItemLevel;
					item.m_SocketOptionBonus = ItemElement;
					memcpy(item.m_SocketOption, ItemSocketOption, sizeof(item.m_SocketOption));
					item.m_Slot = ItemInfo.Slot;
					item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());
					
					gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [Section: %d] [Item: %s +%d] [Element: %d]", lpObj->Account, lpObj->Name, it->second.DropInfo[0].Section, gItemManager.GetName(ItemIndex), ItemLevel, ItemElement);
					
					gRheaGold.RheaItemSend(lpObj->Index, item, gItemManager.InventoryInsertItem(lpObj->Index, item));
				}
				else
				{
					gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [Section: %d] [Item: %s +%d]", lpObj->Account, lpObj->Name, it->second.DropInfo[0].Section, gItemManager.GetName(ItemIndex), ItemLevel);
					GDCreateItemSend(lpObj->Index, map, px, py, ItemIndex, ItemLevel, ItemDur, 0, 0, 0, lpObj->Index, 0, 0, 0, 0, ItemSocketOption, ItemElement, 0);
				}

				if ((lpItemBagDropInfo->OptionValue & 1) != 0)
				{
					GCFireworksSend(lpObj, lpObj->X, lpObj->Y);
				}
			}
			else
			{
				if (RandomManager.GetRandomElement((int*)&lpItemBagDropInfo) == 0)
				{
					continue;
				}

				int px = x;
				int py = y;

				if (it != this->m_ItemBagInfo.begin() || (gMap[map].CheckAttr(px, py, 4) != 0 || gMap[map].CheckAttr(px, py, 8) != 0))
				{
					if (this->GetRandomItemDropLocation(map, &px, &py, 2, 2, 10) == 0)
					{
						px = lpObj->X;
						py = lpObj->Y;
					}
				}

				std::map<int, std::vector<ITEM_BAG_EX_ITEM_INFO>>::iterator ItemInfo = this->m_ItemBagItemInfo.find(lpItemBagDropInfo->Section);

				if (ItemInfo == this->m_ItemBagItemInfo.end())
				{
					gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
					continue;
				}

				if (ItemInfo->second.empty() != 0)
				{
					gMap[map].MoneyItemDrop(lpItemBagDropInfo->MoneyAmount, px, py);
					continue;
				}

				ITEM_BAG_EX_ITEM_INFO* lpItemBagItemInfo = &ItemInfo->second[GetLargeRand() % ItemInfo->second.size()];

				WORD ItemIndex = lpItemBagItemInfo->Index;
				BYTE ItemLevel = lpItemBagItemInfo->Level;
				BYTE ItemOption1 = 0;
				BYTE ItemOption2 = 0;
				BYTE ItemOption3 = 0;
				BYTE ItemNewOption = 0;
				BYTE ItemSetOption = 0;
				BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };

				gItemOptionRate.GetItemOption0(lpItemBagItemInfo->Option0, &ItemLevel);
				gItemOptionRate.GetItemOption1(lpItemBagItemInfo->Option1, &ItemOption1);
				gItemOptionRate.GetItemOption2(lpItemBagItemInfo->Option2, &ItemOption2);
				gItemOptionRate.GetItemOption3(lpItemBagItemInfo->Option3, &ItemOption3);
				gItemOptionRate.GetItemOption4(lpItemBagItemInfo->Option4, &ItemNewOption);
				gItemOptionRate.GetItemOption5(lpItemBagItemInfo->Option5, &ItemSetOption);
				gItemOptionRate.GetItemOption6(lpItemBagItemInfo->Option6, &ItemSocketOption[0]);
				gItemOptionRate.MakeNewOption(ItemIndex, ItemNewOption, &ItemNewOption);
				gItemOptionRate.MakeSetOption(ItemIndex, ItemSetOption, &ItemSetOption);
				gItemOptionRate.MakeSocketOption(ItemIndex, ItemSocketOption[0], &ItemSocketOption[0]);

				if (lpObj->MouseItemUp == 1)
				{
					if (ItemIndex == GET_ITEM(14, 285))
					{
						int RuudAmount = 0;

						if (lpItemBagItemInfo->Option1 > lpItemBagItemInfo->Option0)
							RuudAmount = lpItemBagItemInfo->Option0 + (GetLargeRand() % (lpItemBagItemInfo->Option1 - lpItemBagItemInfo->Option0));
						else
							RuudAmount = lpItemBagItemInfo->Option0;

						lpObj->Ruud += RuudAmount;
						gRheaGold.GC_Notify_RuudUpdate(lpObj->Index, lpObj->Ruud, RuudAmount, 1);
						GCFireworksSend(lpObj, lpObj->X, lpObj->Y);
						return 1;
					}

					CItem item;
					item.Clear();
					item.m_Index = ItemIndex;

					if (gMuunSystem.IsMuunItem(&item) == 0)
					{
						item.m_Level = lpItemBagItemInfo->Option0;
						item.m_Option1 = lpItemBagItemInfo->Option1;
						item.m_Option2 = lpItemBagItemInfo->Option2;
						item.m_Option3 = lpItemBagItemInfo->Option3;
						item.m_NewOption = lpItemBagItemInfo->Option4;
						item.m_SetOption = lpItemBagItemInfo->Option5;
						memcpy(item.m_SocketOption, ItemSocketOption, sizeof(item.m_SocketOption));
					}

					item.m_Durability = (float)gItemManager.GetItemDurability(item.m_Index, item.m_Level, item.IsExcItem(), item.IsSetItem());

					gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [Section: %d] [Item: %s +%d +%d]", lpObj->Account, lpObj->Name, it->second.DropInfo[0].Section, gItemManager.GetName(ItemIndex), ItemLevel, ItemOption3);
					GDCreateItemSend(lpObj->Index, 0xEB, px, py, ItemIndex, ItemLevel, 0, ItemOption1, ItemOption2, ItemOption3, lpObj->Index, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF, ((lpItemBagItemInfo->Duration > 0) ? ((DWORD)time(0) + lpItemBagItemInfo->Duration) : 0));
					
					gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(502), gItemManager.GetName(ItemIndex));
				}
				else
				{
					if (ItemIndex == GET_ITEM(14, 285))
					{
						int RuudAmount = 0;

						if (lpItemBagItemInfo->Option1 > lpItemBagItemInfo->Option0)
							RuudAmount = lpItemBagItemInfo->Option0 + (GetLargeRand() % (lpItemBagItemInfo->Option1 - lpItemBagItemInfo->Option0));
						else
							RuudAmount = lpItemBagItemInfo->Option0;

						lpObj->Ruud += RuudAmount;
						gRheaGold.GC_Notify_RuudUpdate(lpObj->Index, lpObj->Ruud, RuudAmount, 1);
						GCFireworksSend(lpObj, lpObj->X, lpObj->Y);
						return 1;
					}

					gLog.Output(ITEM_BAG_LOG, "[ItemBagEx][%s][%s] [Section: %d] [Item: %s +%d +%d]", lpObj->Account, lpObj->Name, it->second.DropInfo[0].Section, gItemManager.GetName(ItemIndex), ItemLevel, ItemOption3);
					GDCreateItemSend(lpObj->Index, map, px, py, ItemIndex, ItemLevel, 0, ItemOption1, ItemOption2, ItemOption3, lpObj->Index, ((ItemNewOption == 0) ? lpItemBagItemInfo->Grade : ItemNewOption), ItemSetOption, 0, 0, ItemSocketOption, 0xFF, ((lpItemBagItemInfo->Duration > 0) ? ((DWORD)time(0) + lpItemBagItemInfo->Duration) : 0));
				}

				if ((lpItemBagDropInfo->OptionValue & 1) != 0)
				{ 
					GCFireworksSend(lpObj, lpObj->X, lpObj->Y); 
				}
			}
		}
	}

	return 1;
}

bool CItemBagEx::CheckDropClass(LPOBJ lpObj, int type, int RequireClass[MAX_CLASS]) // OK
{
	int aIndex = -1;

	if (type == 0 || OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		if (RequireClass[lpObj->Class] != 0 && RequireClass[lpObj->Class] <= (lpObj->ChangeUp + 1))
		{
			return 1;
		}
	}
	else
	{
		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			if (OBJECT_RANGE((aIndex = gParty.GetMemberIndex(lpObj->PartyNumber, n))) != 0)
			{
				if (RequireClass[gObj[aIndex].Class] != 0 && RequireClass[gObj[aIndex].Class] <= (gObj[aIndex].ChangeUp + 1))
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

bool::CItemBagEx::GetRandomItemDropLocation(int map, int* ox, int* oy, int tx, int ty, int count) // OK
{
	int x = (*ox);
	int y = (*oy);

	tx = ((tx < 1) ? 1 : tx);
	ty = ((ty < 1) ? 1 : ty);

	for (int n = 0; n < count; n++)
	{
		(*ox) = ((GetLargeRand() % (tx + 1)) * ((GetLargeRand() % 2 == 0) ? -1 : 1)) + x;
		(*oy) = ((GetLargeRand() % (ty + 1)) * ((GetLargeRand() % 2 == 0) ? -1 : 1)) + y;

		if (gMap[map].CheckAttr((*ox), (*oy), 4) == 0 && gMap[map].CheckAttr((*ox), (*oy), 8) == 0)
		{
			return 1;
		}
	}

	return 0;
}
