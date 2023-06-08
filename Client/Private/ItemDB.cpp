#include "stdafx.h"
#include "..\Public\ItemDB.h"

IMPLEMENT_SINGLETON(CItemDB)

CItemDB::CItemDB()
{
	m_DataBase =
	{
	/*--- ������ ����-----������ ��� -----������ ������ �ؽ���(�ε���)-----������ �̸� �ؽ���(�ε���)-----����-----������ �±�-----�ΰ� ����-----��밡�� ����(�ܵ�������)-----*/
	{ ITEM::TACTREITE_VOUCHER, { ITEM::TACTREITE_VOUCHER, CItem::ITEM_COIN, CItem::LEGEND, STATIC_IMAGE::ITEM_TACTREITE_VOUCHER, STATIC_IMAGE::TEXT_TACTREITE_VOUCHER, 1, L"Tactreite Voucher", { 0, 0, 0, 0 }, false }},
	{ ITEM::COMMEMORATIVE_COIN, { ITEM::COMMEMORATIVE_COIN, CItem::ITEM_COIN, CItem::UNIQUE, STATIC_IMAGE::ITEM_COMMEMORATIVE_COIN, STATIC_IMAGE::TEXT_COMMEMORATIVE_COIN, 1, L"Commemorative Coin", { 0, 0, 0, 0 }, false }},
	{ ITEM::TACTITE_COIN, { ITEM::TACTITE_COIN, CItem::ITEM_COIN, CItem::NORMAL, STATIC_IMAGE::ITEM_TACTITE_COIN, STATIC_IMAGE::TEXT_TACTITE_COIN, 1, L"Tactite Coin", { 0, 0, 0, 0 }, false }},
	{ ITEM::CASKET,{ ITEM::CASKET, CItem::ITEM_MATERIAL, CItem::NORMAL, STATIC_IMAGE::ITEM_CASKET, STATIC_IMAGE::TEXT_CASKET, 1, L"Casket",{ 0, 0, 0, 0 }, false } },
	{ ITEM::CASKETPIECE,{ ITEM::CASKETPIECE, CItem::ITEM_MATERIAL, CItem::NORMAL, STATIC_IMAGE::ITEM_CASKETPIECE, STATIC_IMAGE::TEXT_CASKETPIECE, 1, L"Casket Piece",{ 0, 0, 0, 0 }, false } },

	//����ġ ĸ�� ( ���淮, ���� �����)
	{ ITEM::EXP0, { ITEM::EXP0, CItem::ITEM_MATERIAL, CItem::NORMAL, STATIC_IMAGE::ITEM_EXP0, STATIC_IMAGE::TEXT_EXP0, 1, L"Standard Capsule",{ 150, 750, 0, 0 }, true }},
	{ ITEM::EXP1, { ITEM::EXP1, CItem::ITEM_MATERIAL, CItem::ADVANCED, STATIC_IMAGE::ITEM_EXP1, STATIC_IMAGE::TEXT_EXP1, 1, L"Extanded Capsule",{ 300, 1500, 0, 0 }, true }},
	{ ITEM::EXP2, { ITEM::EXP2, CItem::ITEM_MATERIAL, CItem::RARE, STATIC_IMAGE::ITEM_EXP2, STATIC_IMAGE::TEXT_EXP2, 1, L"Highend Capsule",{ 600, 3000, 0, 0 }, true } },
	{ ITEM::EXP3, { ITEM::EXP3, CItem::ITEM_MATERIAL, CItem::UNIQUE, STATIC_IMAGE::ITEM_EXP3, STATIC_IMAGE::TEXT_EXP3, 1, L"Unique Capsule",{ 1000, 6000, 0, 0 }, true } },

	//���� (���ݷ�, ġ��Ÿ Ȯ��, ��ȭȽ��, �ŷ����)
	{ ITEM::SWORD0, { ITEM::SWORD0, CItem::ITEN_WEAPON, CItem::NORMAL, STATIC_IMAGE::ICON_SWORD_0, STATIC_IMAGE::TEXT_SWORD_0, 1, L"Training Sword",{ 26, 5, 0, 0 }, true } },
	{ ITEM::SWORD1, { ITEM::SWORD1, CItem::ITEN_WEAPON, CItem::ADVANCED, STATIC_IMAGE::ICON_SWORD_1, STATIC_IMAGE::TEXT_SWORD_1, 1, L"Beginner`s Sword",{ 48, 10, 0, 0 }, true } },
	{ ITEM::SWORD2, { ITEM::SWORD2, CItem::ITEN_WEAPON, CItem::RARE, STATIC_IMAGE::ICON_SWORD_2, STATIC_IMAGE::TEXT_SWORD_2, 1, L"Intermediate Sword",{ 96, 15, 0, 0 }, true } },
	{ ITEM::SWORD3, { ITEM::SWORD3, CItem::ITEN_WEAPON, CItem::UNIQUE, STATIC_IMAGE::ICON_SWORD_3, STATIC_IMAGE::TEXT_SWORD_3, 1, L"Advanced Sword",{ 124, 20, 0, 0 }, true } },
	{ ITEM::SWORD4, { ITEM::SWORD4, CItem::ITEN_WEAPON, CItem::LEGEND, STATIC_IMAGE::ICON_SWORD_4, STATIC_IMAGE::TEXT_SWORD_4, 1, L"Special Sword",{ 253, 25, 0, 0 }, true } },

	//���� (���ݷ�, ġ��Ÿ Ȯ��, ��ȭȽ��, �ŷ����)
	{ ITEM::GUN0,{ ITEM::GUN0, CItem::ITEN_WEAPON, CItem::NORMAL, STATIC_IMAGE::ICON_GUN_0, STATIC_IMAGE::TEXT_GUN_0, 1, L"Training Gun",{ 26, 5, 0, 0 }, true } },
	{ ITEM::GUN1,{ ITEM::GUN1, CItem::ITEN_WEAPON, CItem::ADVANCED, STATIC_IMAGE::ICON_GUN_1, STATIC_IMAGE::TEXT_GUN_1, 1, L"Beginner`s Gun",{ 48, 10, 0, 0 }, true } },
	{ ITEM::GUN2,{ ITEM::GUN2, CItem::ITEN_WEAPON, CItem::RARE, STATIC_IMAGE::ICON_GUN_2, STATIC_IMAGE::TEXT_GUN_2, 1, L"Intermediate Gun",{ 96, 15, 0, 0 }, true } },
	{ ITEM::GUN3,{ ITEM::GUN3, CItem::ITEN_WEAPON, CItem::UNIQUE, STATIC_IMAGE::ICON_GUN_3, STATIC_IMAGE::TEXT_GUN_3, 1, L"Advanced Gun",{ 124, 20, 0, 0 }, true } },
	{ ITEM::GUN4,{ ITEM::GUN4, CItem::ITEN_WEAPON, CItem::LEGEND, STATIC_IMAGE::ICON_GUN_4, STATIC_IMAGE::TEXT_GUN_4, 1, L"Special Gun",{ 253, 25, 0, 0 }, true } },

	// ����
	{ ITEM::GEM,{ ITEM::GEM, CItem::ITEM_MATERIAL, CItem::LEGEND, STATIC_IMAGE::ICON_GEM, STATIC_IMAGE::TEXT_GEM, 1, L"Gem",{ 20000, 0, 0, 0 }, false } }
	

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

_float3 CItemDB::GetItemSlotColor(CItem::ITEM_GRADE eItemGrade)
{
	_float3 vColor = { 0.f, 0.f, 0.f };
	switch (eItemGrade)
	{
	case Client::CItem::NORMAL:
		vColor = SLOT_COLOR; //ȸ��
		break;
	case Client::CItem::ADVANCED:
		vColor = SLOT_ADVANCED_COLOR; //�ʷ�
		break;
	case Client::CItem::RARE:
		vColor = SLOT_RARE_COLOR; //�Ķ�
		break;
	case Client::CItem::UNIQUE:
		vColor = SLOT_UNIQUE_COLOR; //����
		break;
	case Client::CItem::LEGEND:
		vColor = SLOT_LEGEND_COLOR; //���
		break;
	default:
		vColor = { 1.f, 1.f, 1.f };
		break;
	}

	return vColor;
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

_uint CItemDB::ComputeWeaponDamage(CItem::ITEM_DESC tagItemDesc)
{
	// ��ȭ�� ���ݷ� 10%�� ����
	assert(tagItemDesc.eItemType == CItem::ITEN_WEAPON);
	assert(tagItemDesc.iData[2] <= 5);

	return tagItemDesc.iData[0] + _uint((tagItemDesc.iData[0] * 0.1f) * tagItemDesc.iData[2]);
}

_uint CItemDB::ComputeWeaponCriticalRate(CItem::ITEM_DESC tagItemDesc)
{
	// ��ȭ�� ġ��Ÿ Ȯ�� 1%�� ����
	assert(tagItemDesc.eItemType == CItem::ITEN_WEAPON);
	assert(tagItemDesc.iData[2] <= 5);

	return tagItemDesc.iData[1] + tagItemDesc.iData[2];
}

void CItemDB::Free()
{
}