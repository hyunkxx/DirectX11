#pragma once

#include "Base.h"
#include "Item.h"

BEGIN(Client)

class CItemDB final : public CBase
{
public:
	DECLARE_SINGLETON(CItemDB)

public:
	CItemDB();
	virtual ~CItemDB() = default;
	virtual void Free() override;

public:
	static _float3 GetItemColor(CItem::ITEM_GRADE eItemGrade);

public:
	CItem::ITEM_DESC GetItemData(_uint iKey);

private:
	unordered_map<_uint, CItem::ITEM_DESC> m_DataBase;

};

namespace ITEM
{
	enum
	{
		TACTREITE_VOUCHER = 0,
		COMMEMORATIVE_COIN,
		TACTITE_COIN,
		
	};
}

struct CompareItemGrade
{
	bool operator()(CItem::ITEM_DESC src, CItem::ITEM_DESC dest)
	{
		return src.eItemGrade > dest.eItemGrade;
	}
};

END

