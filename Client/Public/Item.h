#pragma once

#include "GameObject.h"

BEGIN(Client)

class CItem
{
public:
	enum ITEM_TYPE { ITEN_WEAPON, ITEM_COOK, ITEM_MATERIAL, ITEM_COIN };
	enum ITEM_GRADE { NORMAL, ADVANCED, RARE, UNIQUE, LEGEND };

	typedef struct tagItemDesc
	{
		_uint				iItemID;			// 아이템 ID
		ITEM_TYPE			eItemType;			// 아이템 종류
		ITEM_GRADE			eItemGrade;			// 아이템 등급
		_uint				iImageIndex;		// 아이템 아이콘 텍스쳐(인덱스)
		_uint				iImageTextIndex;	// 아이템 이름 텍스쳐(인덱스)
		_uint				iAmount;			// 수량
		_tchar				szTag[MAX_TAG];		// 아이템 태그
		_int				iData[2];			// 부가 정보
		_bool				bAvailable;			// 사용가능 여부(단독적으로)

	}ITEM_DESC;
	
public:
	explicit CItem(ITEM_DESC tagItemDesc);
	explicit CItem(const CItem& rhs);
	~CItem() = default;

public: 
	//아이템 공용 인터페이스 작성
	ITEM_DESC GetItemDesc() const {	return m_ItemDesc; }


private:
	ITEM_DESC m_ItemDesc;
	
};

END