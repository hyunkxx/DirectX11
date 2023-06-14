#include "stdafx.h"
#include "..\Public\Inventory.h"

#include "Item.h"
#include "GameInstance.h"
#include "PlayerState.h"

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

	CGameInstance* pGI = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGI->Find_GameObject(LEVEL_STATIC, L"CharacterState"));

	// 임시 채우기
	AddItem(ITEM::FLATBREAD, 5);
	AddItem(ITEM::SALAD, 5);
	AddItem(ITEM::FRIEDTOFU, 5);
	AddItem(ITEM::SALTEDTEA, 5);
	AddItem(ITEM::HERBTEA, 5);
	AddItem(ITEM::DRAGONNOODLE, 5);
	AddItem(ITEM::OMURICE, 5);
	AddItem(ITEM::FRIEDRICE, 5);
	AddItem(ITEM::FRIEDCHICKEN, 5);
	AddItem(ITEM::RABBITBREAD, 5);
	AddItem(ITEM::FRIEDMUSHROOM, 5);
	AddItem(ITEM::MAPOTOFU, 5);
	AddItem(ITEM::PORKBELLY, 5);
	AddItem(ITEM::DUCKNOODLES, 5);

	AddItem(ITEM::GEM, 100);
	AddItem(ITEM::SWORD0, 1);
	AddItem(ITEM::SWORD1, 1);
	AddItem(ITEM::SEQUENCE_GEM, 4);

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
		PushItemDesc(itemDesc);
	}
	ImGui::SameLine();
	if(ImGui::Button("Add Rare Item"))
	{
		CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::COMMEMORATIVE_COIN);
		PushItemDesc(itemDesc);
	}

	ImGui::SameLine();
	if (ImGui::Button("Add Coin"))
	{
		CItem::ITEM_DESC itemDesc = pDB->GetItemData(ITEM::TACTITE_COIN);
		PushItemDesc(itemDesc);
	}

	static int iCurItem = 0;
	const char* InvenTypes[INVEN_MAX] =
	{
		"INVEN_WEAPON",
		"INVEN_COOK",
		"INVEN_MATERIAL",
		"INVEN_COIN",
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

_bool CInventory::DeleteCoin(_uint iPayment)
{
	if (iPayment > m_iCoin)
		return false;
	else
	{
		m_iCoin -= iPayment;
		return true;
	}
}

void CInventory::PushItemDesc(CItem::ITEM_DESC tagItemDesc)
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

void CInventory::AddItem(_uint iItemID, _uint iAmount)
{
	CItemDB* pDB = CItemDB::GetInstance();
	CItem::ITEM_DESC pItemDesc = pDB->GetItemData(iItemID);

	pItemDesc.iAmount = iAmount;

	_bool bResult = false;
	switch (pItemDesc.eItemType)
	{
	case CItem::ITEN_WEAPON:
		bResult = addWeapon(pItemDesc);
		break;
	case CItem::ITEM_COOK:
		FALL_THROUGH
	case CItem::ITEM_MATERIAL:
		bResult = addItem((INVEN_TYPE)pItemDesc.eItemType, pItemDesc);
		break;
	case CItem::ITEM_COIN:
		bResult = addCoin(pItemDesc);
		break;
	}

	if (bResult)
		sort(m_Items[pItemDesc.eItemType].begin(), m_Items[pItemDesc.eItemType].end(), CompareItemGrade());
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

_bool CInventory::EraseItem(INVEN_TYPE eInvenType, _uint iItemID, _uint iAmount)
{
	_uint iEraseCount = 0;
	for (auto iter = m_Items[eInvenType].begin(); iter != m_Items[eInvenType].end(); )
	{
		if (iter->iItemID == iItemID)
		{
			if (iter->iAmount <= iAmount)
			{
				iEraseCount += iter->iAmount;
				iter = m_Items[eInvenType].erase(iter);
				sort(m_Items[eInvenType].begin(), m_Items[eInvenType].end(), CompareItemGrade());
			}
			else
			{
				iEraseCount += iter->iAmount;
				iter->iAmount -= iAmount;
			}
		}
		else
		{
			iter++;
		}
		if (iEraseCount >= iAmount)
		{
			sort(m_Items[eInvenType].begin(), m_Items[eInvenType].end(), CompareItemGrade());
			break;
		}
	}
	return true;
}

_uint CInventory::GetTotalAmount(INVEN_TYPE eInvenType, _uint iItemID)
{
	//모든 슬롯을 순회하면서 해당 아이템의 수량을 더함
	_uint iAmount = 0;
	for (auto iter = m_Items[eInvenType].begin(); iter != m_Items[eInvenType].end(); ++iter)
	{
		if (iter->iItemID == iItemID)
			iAmount += iter->iAmount;
	}

	return iAmount;
}

_uint CInventory::GetInvenTotalCount(INVEN_TYPE eInvenType) const
{
	return (_uint)m_Items[eInvenType].size();
}

void CInventory::SwapWeapon(_uint iCharacter, _uint iSlotIndex)
{
	CItem::ITEM_DESC* pPlayerWeapon = m_pPlayerState->GetCurWeaponDesc((CPlayerState::CHARACTERS)iCharacter);

	//슬롯의 아이템 데이터 임시저장
	CItem::ITEM_DESC pSlotItem = m_Items[INVEN_WEAPON][iSlotIndex];
	DiscardItem(INVEN_WEAPON, iSlotIndex, 1);

	m_Items[INVEN_WEAPON].push_back(*pPlayerWeapon);
	m_pPlayerState->SetEquitWeapon((CPlayerState::CHARACTERS)iCharacter, pSlotItem);

	sort(m_Items[INVEN_WEAPON].begin(), m_Items[INVEN_WEAPON].end(), CompareItemGrade());

	//강화순으로도 정렬 하고싶은데
	//_uint iTarget = pSlotItem.eItemGrade;
	//_bool m_bFindTarget = false;
	//vector<CItem::ITEM_DESC>::iterator iTargetGradeBegin;
	//vector<CItem::ITEM_DESC>::iterator iTargetGradeEnd;

	//for (auto iter = m_Items[INVEN_WEAPON].begin(); iter != m_Items[INVEN_WEAPON].end(); ++iter)
	//{
	//	if (!m_bFindTarget)
	//	{
	//		if (iTarget == iter->eItemGrade)
	//		{
	//			m_bFindTarget = true;
	//			iTargetGradeBegin = iter;
	//		}
	//	}
	//	else
	//	{
	//		if (iTarget != iter->eItemGrade)
	//		{
	//			m_bFindTarget = false;
	//			iTargetGradeEnd = iter;
	//		}
	//	}
	//}

	//sort(iTargetGradeBegin, iTargetGradeEnd, CompareWeaponUpgradeCount());
}

_bool CInventory::addWeapon(CItem::ITEM_DESC tagItemDesc)
{
	_uint iItemAmount = tagItemDesc.iAmount;
	tagItemDesc.iAmount = 1;

	if (m_Items[INVEN_WEAPON].size() >= m_iSlotLimit[INVEN_WEAPON])
		return false;

	_uint iPushCount = 0;

	while (iPushCount < iItemAmount)
	{
		if (m_Items[INVEN_WEAPON].size() < m_iSlotLimit[INVEN_WEAPON])
		{
			iPushCount++;
			m_Items[INVEN_WEAPON].push_back(tagItemDesc);
		}
	}

	return true;
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
	__super::Free();

}
