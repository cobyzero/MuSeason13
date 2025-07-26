#include "stdafx.h"
#include "MemScript.h"
#include "Util.h"
#include "MasteryItemExtOptionSet.h"
#include "ItemManager.h"

MasteryItemExtOptionSet gMasteryItemExtOptionSet;

MasteryItemExtOptionSet::MasteryItemExtOptionSet()
{
	
}


MasteryItemExtOptionSet::~MasteryItemExtOptionSet()
{
	
}

void MasteryItemExtOptionSet::Load(char* path)
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

	memset(this->m_MasteryItemExtOptionSet, -1, sizeof(this->m_MasteryItemExtOptionSet));

	int count = 0;

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			m_MasteryItemExtOptionSet[count].ItemType = lpMemScript->GetNumber();

			m_MasteryItemExtOptionSet[count].ItemIndex = lpMemScript->GetAsNumber();

			for (int i = 0; i < 6; i++)
			{
				m_MasteryItemExtOptionSet[count].ExtOption[i]= lpMemScript->GetAsNumber();
			}

			m_MasteryItemExtOptionSet[count].Rate = lpMemScript->GetAsNumber();

			count++;
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

BOOL  MasteryItemExtOptionSet::RandMasteryItemExtOption(int iType, int iIndex, BYTE *ExtOp)
{
	int iAddRate = 0;

	for (int i = 0; i < 100; i++)
	{
		if (this->m_MasteryItemExtOptionSet[i].ItemType == iType && this->m_MasteryItemExtOptionSet[i].ItemIndex == iIndex)
		{
			if (GetLargeRand() % 1000000 < this->m_MasteryItemExtOptionSet[i].Rate + iAddRate)
			{
				for (int j = 0; j < 6; j++)
				{
					ExtOp[j] = this->m_MasteryItemExtOptionSet[i].ExtOption[j];
				}

				return 1;
			}

			iAddRate += this->m_MasteryItemExtOptionSet[i].Rate;
		}
	}

	return 0;
}

BOOL  MasteryItemExtOptionSet::IsMasteryItem(int Index)
{
	for (int i = 0; i < 100; i++)
	{
		if (GET_ITEM(this->m_MasteryItemExtOptionSet[i].ItemType,this->m_MasteryItemExtOptionSet[i].ItemIndex) == Index)
		{
			return 1;
		}
	}

	return 0;
}