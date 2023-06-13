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
	static _float3 GetItemSlotColor(CItem::ITEM_GRADE eItemGrade);
	static _float3 GetItemColor(CItem::ITEM_GRADE eItemGrade);
	static _uint ComputeWeaponDamage(CItem::ITEM_DESC tagItemDesc);
	static _uint ComputeWeaponCriticalRate(CItem::ITEM_DESC tagItemDesc);

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
		CASKET,
		CASKETPIECE,
		
		EXP0,
		EXP1,
		EXP2,
		EXP3,

		SWORD0,
		SWORD1,
		SWORD2,
		SWORD3,
		SWORD4,

		GUN0,
		GUN1,
		GUN2,
		GUN3,
		GUN4,

		GEM,

		DOGTAG0,
		DOGTAG1,
		DOGTAG2,
		DOGTAG3,

		SEQUENCE_GEM,

		RICE,
		TOFU,
		FLOUR,
		MUSHROOM,
		CHICKEN,
		EGG,
		MEAT,
		HERB,
		VIOLET,
		DANDELION,
		PEPPER,
		SUGAR,
		SALT,
		OIL,
		SOYSAUCE,
		RECIPE0,
		RECIPE1,
		RECIPE2,

		FLATBREAD,
		SALAD,
		FRIEDTOFU,
		SALTEDTEA,
		HERBTEA,
		DRAGONNOODLE,
		OMURICE,
		FRIEDRICE,
		FRIEDCHICKEN,
		RABBITBREAD,
		FRIEDMUSHROOM,
		MAPOTOFU,
		PORKBELLY,
		DUCKNOODLES

	};
}

struct CompareItemGrade
{
	bool operator()(CItem::ITEM_DESC src, CItem::ITEM_DESC dest)
	{
		return src.eItemGrade > dest.eItemGrade;
	}
};

struct CompareWeaponUpgradeCount
{
	bool operator()(CItem::ITEM_DESC src, CItem::ITEM_DESC dest)
	{
		return src.iData[2] > dest.iData[2];
	}
};


END

