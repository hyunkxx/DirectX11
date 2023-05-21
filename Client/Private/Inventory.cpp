#include "stdafx.h"
#include "..\Public\Inventory.h"

#include "Item.h"
#include "GameInstance.h"

CInventory::CInventory(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CInventory::CInventory(const CInventory & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	for (_uint i = 0; i < INVEN_MAX; ++i)
	{
		m_iSlotLimit[i] = 20;
		m_Items[i].reserve(100);
	}

	return S_OK;
}

void CInventory::Start()
{
}

void CInventory::Tick(_double TimeDelta)
{
}

void CInventory::LateTick(_double TimeDelta)
{
}

void CInventory::RenderGUI()
{
	// 테스트용 완성되면 삭제
	ImGui::Begin("Inventory Handler");
	CGameInstance* pGI = CGameInstance::GetInstance();
	CItemDB* pDB = CItemDB::GetInstance();

	if (ImGui::Button("Add Unique Item"))
	{
		CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::TACTREITE_VOUCHER);
		PushItem(itemDesc);
	}
	ImGui::SameLine();
	if(ImGui::Button("Add Rare Item"))
	{
		CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::COMMEMORATIVE_COIN);
		PushItem(itemDesc);
	}

	ImGui::SameLine();
	if (ImGui::Button("Add Coin"))
	{
		CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::TACTITE_COIN);
		PushItem(itemDesc);
	}

	static int iCurItem = 0;
	const char* InvenTypes[INVEN_MAX] =
	{
		"INVEN_WEAPON",
		"INVEN_COOK",
		"INVEN_MATERIAL",
		"INVEN_COIN"
	};

	ImGui::Combo("Erase InvenType", (int*)&iCurItem, InvenTypes, 4);

	static int iEraseIndex = 0;
	ImGui::InputInt("Erase Index ", (int*)&iEraseIndex);
	ImGui::SameLine();
	if (ImGui::Button("Erase Item"))
	{
		DiscardItem((INVEN_TYPE)iCurItem, iEraseIndex);
	}

	ImGui::End();

	for (int j = 0; j < INVEN_MAX; j++)
	{
		string strInven = "Items[" + to_string(j) + "]";
		ImGui::Begin(strInven.c_str());
		for (int i = 0; i < MAX_AMOUNT; ++i)
		{
			if (i < m_Items[j].size())
			{
				string iType = "Slot  " + to_string(i) + " , ItemType " + to_string(m_Items[j][i].eItemType) + " , Amount 50" + " / "  + to_string(m_Items[j][i].iAmount);
				ImGui::Text(iType.c_str());
			}
		}
		ImGui::End();
	}
}

void CInventory::PushItem(CItem::ITEM_DESC tagItemDesc)
{
	_bool bResult = false;
	switch (tagItemDesc.eItemType)
	{
	case CItem::ITEN_WEAPON:
		bResult = addWeapon(tagItemDesc);
		break;
	case CItem::ITEM_COOK:
		FALL_THROUGH
	case CItem::ITEM_MATERIAL:
		bResult = addItem((INVEN_TYPE)tagItemDesc.eItemType, tagItemDesc);
		break;
	case CItem::ITEM_COIN:
		bResult = addCoin(tagItemDesc);
		break;
	}

	if (bResult)
		sort(m_Items[tagItemDesc.eItemType].begin(), m_Items[tagItemDesc.eItemType].end(), CompareItemGrade());
}

void CInventory::DiscardItem(INVEN_TYPE eInvenType, _uint iSlotIndex, _uint iAmount)
{
	int i = 0;
	for (auto iter = m_Items[eInvenType].begin(); iter != m_Items[eInvenType].end();)
	{
		if (i == iSlotIndex)
		{
			if (iter->iAmount <= iAmount)
			{
				iter = m_Items[eInvenType].erase(iter);
				sort(m_Items[eInvenType].begin(), m_Items[eInvenType].end(), CompareItemGrade());
			}
			else
				iter->iAmount -= iAmount;

			return;
		}
		else
		{
			++iter;
			++i;
		}
	}
}

_bool CInventory::addWeapon(CItem::ITEM_DESC tagItemDesc)
{
	if (m_Items[INVEN_WEAPON].size() < m_iSlotLimit[INVEN_WEAPON])
	{
		m_Items[INVEN_WEAPON].push_back(tagItemDesc);
		return true;
	}

	return false;
}

_bool CInventory::addItem(INVEN_TYPE eInvenType, CItem::ITEM_DESC tagItemDesc)
{
	// 슬롯의 수량 체크
	if (m_Items[eInvenType].empty())
	{
		m_Items[eInvenType].push_back(tagItemDesc);
		return true;
	}

	_uint iAmount = tagItemDesc.iAmount;
	for (_uint i = 0; i < m_Items[eInvenType].size(); ++i)
	{
		if (lstrcmp(m_Items[eInvenType][i].szTag, tagItemDesc.szTag))
			continue;

		if (m_Items[eInvenType][i].iAmount < MAX_AMOUNT)
		{
			if (m_Items[eInvenType][i].iAmount > MAX_AMOUNT - iAmount)
			{
				// 일부 추가될 수 있음
				_uint iOffsetAmount = MAX_AMOUNT - m_Items[eInvenType][i].iAmount;
				iAmount -= iOffsetAmount;
				m_Items[eInvenType][i].iAmount = MAX_AMOUNT;
				tagItemDesc.iAmount = iAmount;

				if (m_Items[eInvenType].size() < m_iSlotLimit[eInvenType])
				{
					m_Items[eInvenType].push_back(tagItemDesc);
					return true;
				}
				else
					return false;
			}
			else
			{
				// 해당 슬롯에 모두 들어갈 수 있음
				m_Items[eInvenType][i].iAmount += iAmount;
				return true;
			}

		}
		else
			continue;
	}

	//모두 들어갈 수 없을때 마지막 슬롯에 추가
	if (m_Items[eInvenType].size() < m_iSlotLimit[eInvenType])
	{
		m_Items[eInvenType].push_back(tagItemDesc);
		return true;
	}
	else
		return false;

	return true;
}

_bool CInventory::addCoin(CItem::ITEM_DESC tagItemDesc)
{
	//기본 재화일 경우
	if (!lstrcmp(L"Tactite Coin", tagItemDesc.szTag))
	{
		if (UINT_MAX < m_iCoin + tagItemDesc.iAmount)
			m_iCoin = UINT_MAX;
		else
			m_iCoin += tagItemDesc.iAmount;

		return true;
	}


	// 슬롯의 수량 체크
	if (m_Items[INVEN_COIN].empty())
	{
		m_Items[INVEN_COIN].push_back(tagItemDesc);
		return true;
	}

	_uint iAmount = tagItemDesc.iAmount;
	for (_uint i = 0; i < m_Items[INVEN_COIN].size(); ++i)
	{
		if (lstrcmp(m_Items[INVEN_COIN][i].szTag, tagItemDesc.szTag))
			continue;

		if (m_Items[INVEN_COIN][i].iAmount < MAX_AMOUNT)
		{
			if (m_Items[INVEN_COIN][i].iAmount > MAX_AMOUNT - iAmount)
			{
				// 일부 추가될 수 있음
				_uint iOffsetAmount = MAX_AMOUNT - m_Items[INVEN_COIN][i].iAmount;
				iAmount -= iOffsetAmount;
				m_Items[INVEN_COIN][i].iAmount = MAX_AMOUNT;
				tagItemDesc.iAmount = iAmount;

				if (m_Items[INVEN_COIN].size() < m_iSlotLimit[INVEN_COIN])
				{
					m_Items[INVEN_COIN].push_back(tagItemDesc);
					return true;
				}
				else
					return false;
			}
			else
			{
				// 해당 슬롯에 모두 들어갈 수 있음
				m_Items[INVEN_COIN][i].iAmount += iAmount;
				return true;
			}

		}
		else
			continue;
	}

	//모두 들어갈 수 없을때 마지막 슬롯에 추가
	if (m_Items[INVEN_COIN].size() < m_iSlotLimit[INVEN_COIN])
	{
		m_Items[INVEN_COIN].push_back(tagItemDesc);
		return true;
	}
	else
		return false;

	return true;
}

CInventory* CInventory::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInventory*	pInstance = new CInventory(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventory::Clone(void * pArg)
{
	CInventory*	pInstance = new CInventory(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory::Free()
{
}
