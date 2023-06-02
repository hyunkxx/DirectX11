#include "stdafx.h"
#include "..\Public\ItemDB.h"

IMPLEMENT_SINGLETON(CItemDB)

CItemDB::CItemDB()
{
	m_DataBase =
	{
		/*--- 아이템 종류-----아이템 등급 -----아이템 아이콘 텍스쳐(인덱스)-----아이템 이름 텍스쳐(인덱스)-----수량-----아이템 태그-----부가 정보-----사용가능 여부(단독적으로)-----*/
		{ ITEM::TACTREITE_VOUCHER, { ITEM::TACTREITE_VOUCHER, CItem::ITEM_COIN, CItem::LEGEND, STATIC_IMAGE::ITEM_TACTREITE_VOUCHER, STATIC_IMAGE::TEXT_TACTREITE_VOUCHER, 1, L"Tactreite Voucher", { 0, 0 }, false }},
		{ ITEM::COMMEMORATIVE_COIN, { ITEM::COMMEMORATIVE_COIN, CItem::ITEM_COIN, CItem::UNIQUE, STATIC_IMAGE::ITEM_COMMEMORATIVE_COIN, STATIC_IMAGE::TEXT_COMMEMORATIVE_COIN, 1, L"Commemorative Coin", { 0, 0 }, false }},
		{ ITEM::TACTITE_COIN, { ITEM::TACTITE_COIN, CItem::ITEM_COIN, CItem::NORMAL, STATIC_IMAGE::ITEM_TACTITE_COIN, STATIC_IMAGE::TEXT_TACTITE_COIN, 1, L"Tactite Coin", { 0, 0 }, false }},
		
		{ ITEM::EXP0, { ITEM::EXP0, CItem::ITEM_MATERIAL, CItem::NORMAL, STATIC_IMAGE::ITEM_EXP0, STATIC_IMAGE::TEXT_EXP0, 1, L"Standard Capsule",{ 150, 750 }, true }},
		{ ITEM::EXP1, { ITEM::EXP1, CItem::ITEM_MATERIAL, CItem::ADVANCED, STATIC_IMAGE::ITEM_EXP1, STATIC_IMAGE::TEXT_EXP1, 1, L"Extanded Capsule",{ 300, 1500 }, true }},
		{ ITEM::EXP2, { ITEM::EXP2, CItem::ITEM_MATERIAL, CItem::RARE, STATIC_IMAGE::ITEM_EXP2, STATIC_IMAGE::TEXT_EXP2, 1, L"Highend Capsule",{ 600, 3000 }, true } },
		{ ITEM::EXP3, { ITEM::EXP3, CItem::ITEM_MATERIAL, CItem::UNIQUE, STATIC_IMAGE::ITEM_EXP3, STATIC_IMAGE::TEXT_EXP3, 1, L"Unique Capsule",{ 1000, 6000 }, true } }

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
	// 좀 더 어두움
	//case Client::CItem::NORMAL:
	//	vColor = { 129.f / 255.f, 137.f / 255.f, 143.f / 255.f };
	//	break;
	//case Client::CItem::ADVANCED:
	//	vColor = { 110.f / 255.f, 144.f / 255.f, 80.f / 255.f };
	//	break;
	//case Client::CItem::RARE:
	//	vColor = { 199.f / 255.f, 176.f / 255.f, 80.f / 255.f };
	//	break;
	//case Client::CItem::UNIQUE:
	//	vColor = { 112.f / 255.f, 80.f / 255.f, 144.f / 255.f };

	_float3 vColor = { 0.f, 0.f, 0.f };
	switch (eItemGrade)
	{
	case Client::CItem::NORMAL:
		vColor = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f }; //회색
		break;
	case Client::CItem::ADVANCED:
		vColor = { 50.f / 255.f, 230.f / 255.f, 80.f / 255.f }; //초록
		break;
	case Client::CItem::RARE:
		vColor = { 50.f / 255.f, 150.f / 255.f, 255.f / 255.f }; //파랑
		break;
	case Client::CItem::UNIQUE:
		vColor = { 210.f / 255.f, 110.f / 255.f, 255.f / 255.f }; //보라
		break;
	case Client::CItem::LEGEND:
		vColor = { 250.f / 255.f, 255.f / 255.f, 110.f / 255.f }; //노랑
		break;
	default:
		vColor = { 1.f, 1.f, 1.f };
		break;
	}

	return vColor;
}

void CItemDB::Free()
{
}