#include "stdafx.h"
#include "..\Public\ItemDB.h"

IMPLEMENT_SINGLETON(CItemDB)

CItemDB::CItemDB()
{
	m_DataBase =
	{
		{ "item_cook", { CItem::ITEM_COOK, STATIC_IMAGE::ITEM_TACTREITE_VOUCHER, L"cook"}}
	};
}

CItem::ITEM_DESC CItemDB::GetItemData(string pItemTag)
{
	CItem::ITEM_DESC itemDesc;
	ZeroMemory(&itemDesc, sizeof(itemDesc));

	auto iter = m_DataBase.find(pItemTag);
	if (iter == m_DataBase.end())
	{
		MSG_BOX("CItemDB : nout found pItemTag");
		return itemDesc;
	}

	itemDesc = iter->second;
	return itemDesc;
}

void CItemDB::Free()
{
}