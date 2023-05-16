#pragma once

#include "GameObject.h"

BEGIN(Client)

class CItem
{
public:
	enum ITEM_TYPE { ITEM_MATERIAL, ITEM_COOK, ITEM_EQUITMENT };

	typedef struct tagItemDesc
	{
		ITEM_TYPE			eItemType;
		_uint				iImageIndex;
		_tchar				szTag[MAX_TAG];

		tagItemDesc() {};
		tagItemDesc(ITEM_TYPE ItemType, _uint Index, _tchar* Tag)
			: eItemType(ItemType), iImageIndex(Index)
		{
			lstrcpy(szTag, Tag);
		}

	}ITEM_DESC;

public:
	explicit CItem(ITEM_DESC tagItemDesc);
	explicit CItem(const CItem& rhs);
	~CItem() = default;

private:
	ITEM_DESC m_ItemDesc;
	
};

END