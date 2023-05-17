#include "stdafx.h"
#include "..\Public\ItemDB.h"

IMPLEMENT_SINGLETON(CItemDB)

CItemDB::CItemDB()
{
	m_DataBase =
	{
		{ ITEM::TACTREITE_VOUCHER, { CItem::ITEM_MATERIAL, CItem::MYTH, STATIC_IMAGE::ITEM_TACTREITE_VOUCHER, STATIC_IMAGE::TEXT_TACTREITE_VOUCHER, 1, L"Tactreite Voucher"}},
		{ ITEM::COMMEMORATIVE_COIN, { CItem::ITEM_MATERIAL, CItem::LEGEND, STATIC_IMAGE::ITEM_COMMEMORATIVE_COIN, STATIC_IMAGE::TEXT_COMMEMORATIVE_COIN, 1, L"Commemorative Coin" }},
		{ ITEM::TACTITE_COIN, { CItem::ITEM_MATERIAL, CItem::NORMAL, STATIC_IMAGE::ITEM_TACTITE_COIN, STATIC_IMAGE::TEXT_TACTITE_COIN, 1, L"Tactite Coin" }}
	};
}

CItem::ITEM_DESC CItemDB::GetItemData(_uint iKey)
{
	CItem::ITEM_DESC itemDesc;
	ZeroMemory(&itemDesc, sizeof(itemDesc));

	auto iter = m_DataBase.find(iKey);
	if (iter == m_DataBase.end())
	{
		MSG_BOX("CItemDB : nout found pItemTag");
		return itemDesc;
	}

	itemDesc = iter->second;
	return itemDesc;
}

_float3 CItemDB::GetItemColor(CItem::ITEM_GRADE eItemGrade)
{
	_float3 vColor = { 0.f, 0.f, 0.f };
	switch (eItemGrade)
	{
	case Client::CItem::NORMAL:
		vColor = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f };
		break;
	case Client::CItem::ADVANCED:
		vColor = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f };
		break;
	case Client::CItem::RARE:
		vColor = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f };
		break;
	case Client::CItem::HERO:
		vColor = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f };
		break;
	case Client::CItem::LEGEND:
		vColor = { 210.f / 255.f, 110.f / 255.f, 255.f / 255.f };
		break;
	case Client::CItem::MYTH:
		vColor = { 250.f / 255.f, 255.f / 255.f, 110.f / 255.f };
		break;
	default:
		vColor = { 0.3f, 0.3f, 0.3f };
		break;
	}

	return vColor;
}

void CItemDB::Free()
{
}