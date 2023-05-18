#pragma once

#include "GameObject.h"

BEGIN(Client)

class CItem
{
public:
	enum ITEM_TYPE { ITEM_MATERIAL, ITEM_COOK, ITEM_EQUITMENT, ITEM_MONEY };
	enum ITEM_GRADE { NORMAL, ADVANCED, RARE, UNIQUE };

	typedef struct tagItemDesc
	{
		ITEM_TYPE			eItemType;
		ITEM_GRADE			eItemGrade;
		_uint				iImageIndex;
		_uint				iImageTextIndex;
		_uint				iAmount;
		_tchar				szTag[MAX_TAG];

		//tagItemDesc() {};
		//tagItemDesc(ITEM_TYPE ItemType, _uint Index, _uint textIndex, _tchar* Tag)
		//	: eItemType(ItemType), iImageIndex(Index), iImageTextIndex(textIndex)
		//{
		//	lstrcpy(szTag, Tag);
		//}

	}ITEM_DESC;

public:
	explicit CItem(ITEM_DESC tagItemDesc);
	explicit CItem(const CItem& rhs);
	~CItem() = default;

private:
	ITEM_DESC m_ItemDesc;
	
};

END