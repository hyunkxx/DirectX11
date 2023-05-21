#include "stdafx.h"
#include "..\Public\ItemDB.h"

IMPLEMENT_SINGLETON(CItemDB)

CItemDB::CItemDB()
{
	m_DataBase =
	{
				            /*--- ������ ����-----������ ��� -----������ ������ �ؽ���(�ε���)-----������ �̸� �ؽ���(�ε���)-----����-----������ �±�-----�ΰ� ����-----��밡�� ����(�ܵ�������)-----*/
		{ ITEM::TACTREITE_VOUCHER, { CItem::ITEM_COIN, CItem::LEGEND, STATIC_IMAGE::ITEM_TACTREITE_VOUCHER, STATIC_IMAGE::TEXT_TACTREITE_VOUCHER, 1, L"Tactreite Voucher", { 0, 0 }, false }},
		{ ITEM::COMMEMORATIVE_COIN, { CItem::ITEM_COIN, CItem::UNIQUE, STATIC_IMAGE::ITEM_COMMEMORATIVE_COIN, STATIC_IMAGE::TEXT_COMMEMORATIVE_COIN, 1, L"Commemorative Coin", { 0, 0 }, false }},
		{ ITEM::TACTITE_COIN, { CItem::ITEM_COIN, CItem::NORMAL, STATIC_IMAGE::ITEM_TACTITE_COIN, STATIC_IMAGE::TEXT_TACTITE_COIN, 1, L"Tactite Coin", { 0, 0 }, false }}
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
	// �� �� ��ο�
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
		vColor = { 255.f / 255.f, 255.f / 255.f, 255.f / 255.f }; //ȸ��
		break;
	case Client::CItem::ADVANCED:
		vColor = { 50.f / 255.f, 230.f / 255.f, 80.f / 255.f }; //�ʷ�
		break;
	case Client::CItem::RARE:
		vColor = { 50.f / 255.f, 150.f / 255.f, 255.f / 255.f }; //�Ķ�
		break;
	case Client::CItem::UNIQUE:
		vColor = { 210.f / 255.f, 110.f / 255.f, 255.f / 255.f }; //����
		break;
	case Client::CItem::LEGEND:
		vColor = { 250.f / 255.f, 255.f / 255.f, 110.f / 255.f }; //���
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