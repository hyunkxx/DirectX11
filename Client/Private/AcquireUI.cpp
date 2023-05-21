#include "stdafx.h"
#include "..\Public\AcquireUI.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "ItemDB.h"
#include "AppManager.h"

#define FONT_WITDH 10.f

CAcquireUI::CAcquireUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAcquireUI::CAcquireUI(const CAcquireUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAcquireUI::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAcquireUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	m_ItemDescs.reserve(ACQUIRE_MAX);
	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	for (size_t i = 0; i < ACQUIRE_MAX; ++i)
	{
		m_AcquireOrthoDesc[i].fX = 30.f;
		m_AcquireOrthoDesc[i].fY = 460.f + (i * 39.f);
		m_AcquireOrthoDesc[i].fWidth = 260.f;
		m_AcquireOrthoDesc[i].fHeight = 36.f;

		m_ItemIconDesc[i].fX = 40.f;
		m_ItemIconDesc[i].fY = 460.f + (i * 39.f);
		m_ItemIconDesc[i].fWidth = 45.f;
		m_ItemIconDesc[i].fHeight = 45.f;

		m_ItemTextDesc[i].fX = 65.f;
		m_ItemTextDesc[i].fY = 463.f + (i * 39.f);
		m_ItemTextDesc[i].fWidth = 275.f;
		m_ItemTextDesc[i].fHeight = 35.f;

		m_ItemAmountBegin[i].fX = 65.f;
		m_ItemAmountBegin[i].fY = 460.f + (i * 39.f);
		m_ItemAmountBegin[i].fWidth = FONT_WITDH;
		m_ItemAmountBegin[i].fHeight = 18.f;

		for (int iRow = 0; iRow < 5; ++iRow)
		{
			m_ItemAmountDesc[iRow][i].fX = 65.f;
			m_ItemAmountDesc[iRow][i].fY = 460.f + (i * 39.f);
			m_ItemAmountDesc[iRow][i].fWidth = FONT_WITDH;
			m_ItemAmountDesc[iRow][i].fHeight = 18.f;
		}
	}

	m_AcquireTextDesc.fX = 100.f;
	m_AcquireTextDesc.fY = 430.f;
	m_AcquireTextDesc.fWidth = 350.f;
	m_AcquireTextDesc.fHeight = 70.f;
	XMStoreFloat4x4(&m_AcquireTextDesc.WorldMatrix, XMMatrixScaling(m_AcquireTextDesc.fWidth, m_AcquireTextDesc.fHeight, 1.f) * XMMatrixTranslation(m_AcquireTextDesc.fX - g_iWinSizeX * 0.5f, -m_AcquireTextDesc.fY + g_iWinSizeY * 0.5f, 0.f));

	// 획득한 아이템중 최고등급 아이템 표시
	m_HighestItemTextOrtho.fX = _float(g_iWinSizeX >> 1);
	m_HighestItemTextOrtho.fY = _float(200.f);
	m_HighestItemTextOrtho.fWidth = 320.f;
	m_HighestItemTextOrtho.fHeight = 60.f;
	CAppManager::ComputeOrtho(&m_HighestItemTextOrtho);

	m_HighestAcquireOrtho.fX = _float(g_iWinSizeX >> 1);
	m_HighestAcquireOrtho.fY = _float(250.f);
	m_HighestAcquireOrtho.fWidth = 150.f;
	m_HighestAcquireOrtho.fHeight = 80.f;
	CAppManager::ComputeOrtho(&m_HighestAcquireOrtho);

	m_HighestItemSlotOrtho.fX = _float(g_iWinSizeX >> 1);
	m_HighestItemSlotOrtho.fY = _float(250.f);
	m_HighestItemSlotOrtho.fWidth = 80.f;
	m_HighestItemSlotOrtho.fHeight = 80.f;
	CAppManager::ComputeOrtho(&m_HighestItemSlotOrtho);

	m_HighestItemIconOrtho.fX = _float(g_iWinSizeX >> 1);
	m_HighestItemIconOrtho.fY = _float(250.f);
	m_HighestItemIconOrtho.fWidth = 60.f;
	m_HighestItemIconOrtho.fHeight = 60.f;
	CAppManager::ComputeOrtho(&m_HighestItemIconOrtho);

	return S_OK;
}

void CAcquireUI::Start()
{
}

void CAcquireUI::Tick(_double TimeDelta)
{
	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::Tick(TimeDelta);

	if (m_ItemDescQueue.size() > 0 && !m_bHighestCheck)
	{
		ZeroMemory(&m_HighestItem, sizeof m_HighestItem);

		m_Highest = m_Highest = m_ItemDescQueue[0].eItemGrade;
		m_HighestItem = m_ItemDescQueue[0];

		m_bHighestCheck = true;
		m_bHighestRenderBegin = true;

		for (int i = 0; i < m_ItemDescQueue.size(); ++i)
		{
			if (m_Highest < m_ItemDescQueue[i].eItemGrade)
			{
				m_Highest = m_ItemDescQueue[i].eItemGrade;
				m_HighestItem = m_ItemDescQueue[i];
			}
		}
	}
	
	if (m_bHighestRenderBegin && m_bHighestCheck)
	{
		m_fHighestAlpha = m_fHighestTimeAcc += (_float)TimeDelta;
		if (m_fHighestAlpha > 1.f)
			m_fHighestAlpha = 1.f;

		if (m_fHighestTimeAcc >= 3.f)
			m_bHighestRenderBegin = false;
	}
	else
	{
		m_fHighestAlpha -= (_float)TimeDelta;
		if (m_fHighestAlpha <= 0.f)
			m_fHighestAlpha = 0.f;

		m_fHighestTimeAcc -= (_float)TimeDelta;
		if (m_fHighestTimeAcc <= 0.f)
		{
			m_fHighestTimeAcc = 0.f;

			if(m_ItemDescQueue.empty())
				m_bHighestCheck = false;
		}
	}

	// 활성화 시키기
	if (m_iCurrentActiveCount < ACQUIRE_MAX)
	{
		m_fAcquireCheckAcc += (_float)TimeDelta;
		if (m_fAcquireCheckAcc >= m_fAcquireCheckSec)
		{
			m_fAcquireCheckAcc = 0.f;
			if (!m_ItemDescQueue.empty())
			{
				m_ItemDescs.emplace_back(m_ItemDescQueue.front());
				m_ItemDescQueue.pop_front();
				//sort(m_ItemDescs.begin(), m_ItemDescs.end(), CompareItemGrade());

				m_iCurrentActiveCount++;
				m_fActiveTimeAcc = 0.f;

			}
		}
	}

	// 푸쉬 완료 시점부터 활성화 시간 계산
	if (m_iCurrentActiveCount > 0)
	{
		m_fAcquireTextAlpha += (_float)TimeDelta;
		if (m_fAcquireTextAlpha >= 1.f)
			m_fAcquireTextAlpha = 1.f;

		m_fActiveTimeAcc += (_float)TimeDelta;
		if (m_fActiveTimeAcc >= 4.f)
		{
			m_fActiveTimeAcc = 0.f;
			m_bAquireDisable = true;
		}
	}
	else
	{
		m_fAcquireTextAlpha -= (_float)TimeDelta;
		if (m_fAcquireTextAlpha <= 0.f)
			m_fAcquireTextAlpha = 0.f;
	}

	// UI Movement or Alpha
	if (m_bAquireDisable)
	{
		for (int i = 0; i < m_iCurrentActiveCount; ++i)
			m_fAcquireAlpha[i] -= (_float)TimeDelta;

		if (m_fAcquireAlpha[0] <= 0.f)
			resetAcquire();
	}
	else
	{
		for (int i = 0; i < m_iCurrentActiveCount; ++i)
		{
			if (m_fAcquireAlpha[i] < 1.f)
				m_fAcquireAlpha[i] += (_float)TimeDelta;
			else
				m_fAcquireAlpha[i] = 1.f;

			if (m_AcquireOrthoDesc[i].fX <= 180.f)
			{
				_vector vCurPos = XMVectorSet(m_AcquireOrthoDesc[i].fX, 0.f, 0.f, 1.f);
				_vector vGoalPos = XMVectorSet(180.f, 0.f, 0.f, 1.f);

				// UI Back
				m_AcquireOrthoDesc[i].fX = XMVectorGetX(XMVectorLerp(vCurPos, vGoalPos, (_float)TimeDelta * 5.f));
				XMStoreFloat4x4(&m_AcquireOrthoDesc[i].WorldMatrix, XMMatrixScaling(m_AcquireOrthoDesc[i].fWidth, m_AcquireOrthoDesc[i].fHeight, 1.f) * XMMatrixTranslation(m_AcquireOrthoDesc[i].fX - g_iWinSizeX * 0.5f, -m_AcquireOrthoDesc[i].fY + g_iWinSizeY * 0.5f, 0.f));

				// UI ItemIcon
				m_ItemIconDesc[i].fX = m_AcquireOrthoDesc[i].fX - 105.f;
				XMStoreFloat4x4(&m_ItemIconDesc[i].WorldMatrix, XMMatrixScaling(m_ItemIconDesc[i].fWidth, m_ItemIconDesc[i].fHeight, 1.f) * XMMatrixTranslation(m_ItemIconDesc[i].fX - g_iWinSizeX * 0.5f, -m_ItemIconDesc[i].fY + g_iWinSizeY * 0.5f, 0.f));

				// UI ItemText
				m_ItemTextDesc[i].fX = m_AcquireOrthoDesc[i].fX + 15.f;
				XMStoreFloat4x4(&m_ItemTextDesc[i].WorldMatrix, XMMatrixScaling(m_ItemTextDesc[i].fWidth, m_ItemTextDesc[i].fHeight, 1.f) * XMMatrixTranslation(m_ItemTextDesc[i].fX - g_iWinSizeX * 0.5f, -m_ItemTextDesc[i].fY + g_iWinSizeY * 0.5f, 0.f));

				// UI ItemAmount x
				m_ItemAmountBegin[i].fX = m_AcquireOrthoDesc[i].fX + 120.f;
				XMStoreFloat4x4(&m_ItemAmountBegin[i].WorldMatrix, XMMatrixScaling(m_ItemAmountBegin[i].fWidth, m_ItemAmountBegin[i].fHeight, 1.f) * XMMatrixTranslation(m_ItemAmountBegin[i].fX - g_iWinSizeX * 0.5f, -m_ItemAmountBegin[i].fY + g_iWinSizeY * 0.5f, 0.f));

				// UI ItemAmount
				for (int iDigit = 0; iDigit < 5; ++iDigit)
				{
					m_ItemAmountDesc[iDigit][i].fX = m_AcquireOrthoDesc[i].fX + (120.f + FONT_WITDH) + (iDigit * FONT_WITDH);
					XMStoreFloat4x4(&m_ItemAmountDesc[iDigit][i].WorldMatrix, XMMatrixScaling(m_ItemAmountDesc[iDigit][i].fWidth, m_ItemAmountDesc[iDigit][i].fHeight, 1.f) * XMMatrixTranslation(m_ItemAmountDesc[iDigit][i].fX - g_iWinSizeX * 0.5f, -m_ItemAmountDesc[iDigit][i].fY + g_iWinSizeY * 0.5f, 0.f));
				}
			}
		}
	}
}

void CAcquireUI::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::LateTick(TimeDelta);

	if (ComputeCameraLength() > 50.f)
		return;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CAcquireUI::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Acquire Text Render
	if (m_fAcquireTextAlpha > 0.f)
	{
		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_AcquireTextDesc.WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ACQUIRE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAcquireTextAlpha, sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(5);
		m_pVIBuffer->Render();
	}

	// Acquire Element Render
	if (m_iCurrentActiveCount > 0)
	{
		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		for (int iCurCount = 0; iCurCount < m_iCurrentActiveCount; ++iCurCount)
		{
			// Back
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAcquireAlpha[iCurCount], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_AcquireOrthoDesc[iCurCount].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_LISTBACK, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(5);
			m_pVIBuffer->Render();

			// Icon
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ItemIconDesc[iCurCount].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(m_ItemDescs[iCurCount].iImageIndex, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(5);
			m_pVIBuffer->Render();

			// Text
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ItemTextDesc[iCurCount].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(m_ItemDescs[iCurCount].iImageTextIndex, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_vColor", &CItemDB::GetItemColor(m_ItemDescs[iCurCount].eItemGrade), sizeof(_float3))))
				return E_FAIL;

			m_pShader->Begin(7);
			m_pVIBuffer->Render();
			
			// Amount : x
			_uint iNumLength = getConvertNumberToLength(m_ItemDescs[iCurCount].iAmount);
			if (iNumLength > 1)
			{
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ItemAmountBegin[iCurCount].WorldMatrix)))
					return E_FAIL;
				if (FAILED(m_pShader->SetRawValue("g_vColor", &CItemDB::GetItemColor(m_ItemDescs[iCurCount].eItemGrade), sizeof(_float3))))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_X, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;

				m_pShader->Begin(7);
				m_pVIBuffer->Render();

				for (_uint iDigit = 0; iDigit < iNumLength; ++iDigit)
				{
					if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ItemAmountDesc[iDigit][iCurCount].WorldMatrix)))
						return E_FAIL;
					if (FAILED(m_pShader->SetRawValue("g_vColor", &CItemDB::GetItemColor(m_ItemDescs[iCurCount].eItemGrade), sizeof(_float3))))
						return E_FAIL;

					_uint iNumTexture = getConvertAmountToTexture(m_ItemDescs[iCurCount], iDigit);
					if (FAILED(pGameInstance->SetupSRV(iNumTexture, m_pShader, "g_DiffuseTexture")))
						return E_FAIL;

					m_pShader->Begin(7);
					m_pVIBuffer->Render();
				}
			}
		}
	}

	if (m_fHighestAlpha > 0.f)
	{
		// 최고등급 표시
		if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fHighestAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_HighestAcquireOrtho.WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(9);
		m_pVIBuffer->Render();


		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_HighestItemSlotOrtho.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_vColor", &CItemDB::GetItemColor(m_HighestItem.eItemGrade), sizeof(_float3))))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_GLOWGARD, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_HighestItemIconOrtho.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(m_HighestItem.iImageIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(5);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_HighestItemTextOrtho.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_ACQUIRE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(5);
		m_pVIBuffer->Render();
	}

	return S_OK;
}

void CAcquireUI::RenderGUI()
{
}

void CAcquireUI::EnqueueItemDesc(CItem::ITEM_DESC ItemDesc)
{
	if (m_bHighestCheck)
	{
		m_bHighestRenderBegin = false;
		m_bHighestCheck = false;
		m_fHighestAlpha = 0.f;
		m_fHighestTimeAcc = 0.f;
	}

	sort(m_ItemDescQueue.begin(), m_ItemDescQueue.end(), CompareItemGrade());
	
	m_ItemDescQueue.push_back(ItemDesc);

}

HRESULT CAcquireUI::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI_SUB,
		TEXT("Com_Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

void CAcquireUI::resetAcquire()
{
	m_bAquireDisable = false;
	for (size_t i = 0; i < m_iCurrentActiveCount; ++i)
	{
		m_fAcquireAlpha[i] = 0.f;
		m_AcquireOrthoDesc[i].fX = 30.f;
		XMStoreFloat4x4(&m_AcquireOrthoDesc[i].WorldMatrix, XMMatrixScaling(m_AcquireOrthoDesc[i].fWidth, m_AcquireOrthoDesc[i].fHeight, 1.f) * XMMatrixTranslation(m_AcquireOrthoDesc[i].fX - g_iWinSizeX * 0.5f, -m_AcquireOrthoDesc[i].fY + g_iWinSizeY * 0.5f, 0.f));

		m_ItemIconDesc[i].fX = m_AcquireOrthoDesc[i].fX - 105.f;
		XMStoreFloat4x4(&m_ItemIconDesc[i].WorldMatrix, XMMatrixScaling(m_ItemIconDesc[i].fWidth, m_ItemIconDesc[i].fHeight, 1.f) * XMMatrixTranslation(m_ItemIconDesc[i].fX - g_iWinSizeX * 0.5f, -m_ItemIconDesc[i].fY + g_iWinSizeY * 0.5f, 0.f));

		m_ItemTextDesc[i].fX = m_AcquireOrthoDesc[i].fX + 15.f;
		XMStoreFloat4x4(&m_ItemTextDesc[i].WorldMatrix, XMMatrixScaling(m_ItemTextDesc[i].fWidth, m_ItemTextDesc[i].fHeight, 1.f) * XMMatrixTranslation(m_ItemTextDesc[i].fX - g_iWinSizeX * 0.5f, -m_ItemTextDesc[i].fY + g_iWinSizeY * 0.5f, 0.f));

		m_ItemAmountBegin[i].fX = m_AcquireOrthoDesc[i].fX + 120.f;
		XMStoreFloat4x4(&m_ItemAmountBegin[i].WorldMatrix, XMMatrixScaling(m_ItemAmountBegin[i].fWidth, m_ItemAmountBegin[i].fHeight, 1.f) * XMMatrixTranslation(m_ItemAmountBegin[i].fX - g_iWinSizeX * 0.5f, -m_ItemAmountBegin[i].fY + g_iWinSizeY * 0.5f, 0.f));

		for (int iDigit = 0; iDigit < 5; ++iDigit)
		{
			m_ItemAmountDesc[iDigit][i].fX = m_AcquireOrthoDesc[i].fX + 133.f + (iDigit * FONT_WITDH);
			XMStoreFloat4x4(&m_ItemAmountDesc[iDigit][i].WorldMatrix, XMMatrixScaling(m_ItemAmountDesc[iDigit][i].fWidth, m_ItemAmountDesc[iDigit][i].fHeight, 1.f) * XMMatrixTranslation(m_ItemAmountDesc[iDigit][i].fX - g_iWinSizeX * 0.5f, -m_ItemAmountDesc[iDigit][i].fY + g_iWinSizeY * 0.5f, 0.f));
		}

	}

	m_iCurrentActiveCount = 0;
	m_ItemDescs.clear();
}

_uint CAcquireUI::getConvertAmountToTexture(CItem::ITEM_DESC ItemDesc, _uint iDigit)
{
	string strAmount = to_string(ItemDesc.iAmount);
	size_t iLength = strAmount.size();

	assert(iDigit < iLength);

	_int iNum = strAmount[iDigit] - '0';

	return iNum;
}

_uint CAcquireUI::getConvertNumberToLength(_uint iAmount)
{
	string strAmount = to_string(iAmount);
	_uint iLength = (_uint)strAmount.size();

	return iLength;
}

CAcquireUI * CAcquireUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAcquireUI*	pInstance = new CAcquireUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAcquireUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAcquireUI::Clone(void * pArg)
{
	CAcquireUI*	pInstance = new CAcquireUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAcquireUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAcquireUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

}
