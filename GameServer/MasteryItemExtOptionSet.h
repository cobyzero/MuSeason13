#pragma once
struct MASTERY_ITEM_EXT_OPTION_SET
{
	int ItemType;
	int ItemIndex;
	BYTE ExtOption[6];
	int Rate;
};
class MasteryItemExtOptionSet
{
public:
	MasteryItemExtOptionSet();
	~MasteryItemExtOptionSet();
	void Load(char* path);
	BOOL  RandMasteryItemExtOption(int iType, int iIndex, BYTE *ExtOp);
	BOOL IsMasteryItem(int Index);
private:
	MASTERY_ITEM_EXT_OPTION_SET m_MasteryItemExtOptionSet[100];
};
extern MasteryItemExtOptionSet gMasteryItemExtOptionSet;

