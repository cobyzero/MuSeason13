// SetItemType.h: interface for the CSetItemType class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_SET_ITEM_TYPE 1000
#if(GAMESERVER_UPDATE>=1100)//2017/04/22
#define MAX_SET_ITEM_OPTION_INDEX 3
#define MAX_SET_ITEM_OPTION_I 2
#else
#define MAX_SET_ITEM_OPTION_INDEX 2
#endif
struct SET_ITEM_TYPE_INFO
{
	int Index;
	int StatType;
	int OptionIndex[MAX_SET_ITEM_OPTION_INDEX];
};

class CSetItemType
{
public:
	CSetItemType();
	virtual ~CSetItemType();
	void Init();
	void Load(char* path);
	void SetInfo(SET_ITEM_TYPE_INFO info);
	SET_ITEM_TYPE_INFO* GetInfo(int index);
	bool CheckSetItemType(int index);
	int GetSetItemOptionIndex(int index,int number);
	int GetSetItemStatType(int index);
	int GetRandomSetItem();
	void MakeRandomSetItem(int aIndex,int map,int x,int y);
	void GremoryCaseMakeRandomSetItem(int aIndex,int map,int x,int y,BYTE btStorageType,BYTE btRewardSource,int iDaysToExpire);
private:
	SET_ITEM_TYPE_INFO m_SetItemTypeInfo[MAX_SET_ITEM_TYPE];
	int m_count;
};

extern CSetItemType gSetItemType;
