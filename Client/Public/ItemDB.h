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

	CItem::ITEM_DESC GetItemData(string pItemTag);
private:
	unordered_map<string , CItem::ITEM_DESC> m_DataBase;

};

END