#include "stdafx.h"
#include "..\Public\BackpackUI.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "Inventory.h"
#include "PlayerState.h"
#include "EchoSystem.h"
#include "CameraMovement.h"

#include "ItemDB.h"
#include "TerminalUI.h"

CBackpackUI::CBackpackUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBackpackUI::CBackpackUI(const CBackpackUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackpackUI::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackpackUI::Initialize(void * pArg)
{
	CAppManager* pApp = CAppManager::GetInstance();

	assert(pArg);
	m_pTerminal = static_cast<CTerminalUI*>(pArg);

	ZeroMemory(m_fElemAlpha, sizeof(m_fElemAlpha));
	ZeroMemory(m_bElemAlphaStart, sizeof(m_bElemAlphaStart));

	ZeroMemory(m_fCurFlashIndex, sizeof(m_fCurFlashIndex));
	ZeroMemory(m_bFlashBegin, sizeof(m_bFlashBegin));
	ZeroMemory(m_bOneFlash, sizeof(m_bOneFlash));

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	// Ortho Settings
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

#pragma region MAIN_FRAME
	m_OrthoBackpackSlot.fWidth = 90.f;
	m_OrthoBackpackSlot.fHeight = 90.f;
	m_OrthoBackpackSlot.fX = 100.f;
	m_OrthoBackpackSlot.fY = 180.f;
	CAppManager::ComputeOrtho(&m_OrthoBackpackSlot);

	m_OrthoBackpackIcon.fWidth = 60.f;
	m_OrthoBackpackIcon.fHeight = 60.f;
	m_OrthoBackpackIcon.fX = 100.f;
	m_OrthoBackpackIcon.fY = 180.f;
	CAppManager::ComputeOrtho(&m_OrthoBackpackIcon);
	
	m_OrthoBackground.fWidth = g_iWinSizeX;
	m_OrthoBackground.fHeight = g_iWinSizeY;
	m_OrthoBackground.fX = g_iWinSizeX >> 1;
	m_OrthoBackground.fY = g_iWinSizeY >> 1;
	CAppManager::ComputeOrtho(&m_OrthoBackground);

	//m_OrthoWeaponSlot[i].fX = (g_iWinSizeX >> 1) - (m_OrthoWeaponInvenBack.fWidth * 0.5f) + 100.f + ((i % 5) * 100.f);
	//m_OrthoWeaponSlot[i].fY = 250 + ((i / 5) * 100.f);

	for (_uint i = 0; i < ITEM_SLOT_MAX; ++i)
	{
		m_OrthoSlot[i].fWidth = 130.f;
		m_OrthoSlot[i].fHeight = 130.f;
		m_OrthoSlot[i].fX = 250.f + ((i % 5) * 135.f);
		m_OrthoSlot[i].fY = 150.f + ((i / 5) * 135.f);
		CAppManager::ComputeOrtho(&m_OrthoSlot[i]);

		m_OrthoItemTextBack[i].fWidth = 130.f;
		m_OrthoItemTextBack[i].fHeight = 20.f;
		m_OrthoItemTextBack[i].fX = m_OrthoSlot[i].fX;
		m_OrthoItemTextBack[i].fY = m_OrthoSlot[i].fY - 50.f;
		CAppManager::ComputeOrtho(&m_OrthoItemTextBack[i]);

		m_OrthoItemText[i].fWidth = 512.f * 0.35f;
		m_OrthoItemText[i].fHeight = 64.f * 0.35f;
		m_OrthoItemText[i].fX = m_OrthoSlot[i].fX;
		m_OrthoItemText[i].fY = m_OrthoSlot[i].fY - 48.f;
		CAppManager::ComputeOrtho(&m_OrthoItemText[i]);

		m_OrthoItemAmountText[i].fWidth = 512.f * 0.4f;
		m_OrthoItemAmountText[i].fHeight = 64.f * 0.4f;
		m_OrthoItemAmountText[i].fX = m_OrthoSlot[i].fX + 50.f;
		m_OrthoItemAmountText[i].fY = m_OrthoSlot[i].fY - 28.f;
		CAppManager::ComputeOrtho(&m_OrthoItemAmountText[i]);

		for (_uint iDigit = 0; iDigit < 6; ++iDigit)
		{
			m_OrthoItemAmount[i][iDigit].fWidth = 16.f * 0.5f;
			m_OrthoItemAmount[i][iDigit].fHeight = 25.f * 0.5f;
			m_OrthoItemAmount[i][iDigit].fX = m_OrthoSlot[i].fX + 5.f + (iDigit * (14.f * 0.5f));
			m_OrthoItemAmount[i][iDigit].fY = m_OrthoSlot[i].fY - 28.f;
			CAppManager::ComputeOrtho(&m_OrthoItemAmount[i][iDigit]);
		}

		m_OrthoItemIcon[i].fWidth  = 80.f;
		m_OrthoItemIcon[i].fHeight = 80.f;
		m_OrthoItemIcon[i].fX = m_OrthoSlot[i].fX + 20.f;
		m_OrthoItemIcon[i].fY = m_OrthoSlot[i].fY + 20.f;
		CAppManager::ComputeOrtho(&m_OrthoItemIcon[i]);
	}

	for (_uint i = 0; i < CInventory::INVEN_MAX; ++i)
	{
		m_OrthoTapText[i].fWidth = 512.f * 0.6f;
		m_OrthoTapText[i].fHeight = 64.f * 0.6f;
		m_OrthoTapText[i].fX = 250.f + (i * 130.f);
		m_OrthoTapText[i].fY = 50.f;
		CAppManager::ComputeOrtho(&m_OrthoTapText[i]);

		m_OrthoTapBack[i].fWidth = 200.f;
		m_OrthoTapBack[i].fHeight = 64.f;
		m_OrthoTapBack[i].fX = 250.f + (i * 130.f);
		m_OrthoTapBack[i].fY = 50.f;
		CAppManager::ComputeOrtho(&m_OrthoTapBack[i]);
	}

	// 메인재화
	m_OrthoMainCoinBack.fWidth = 600.f;
	m_OrthoMainCoinBack.fHeight = 64.f * 0.6f;
	m_OrthoMainCoinBack.fX = g_iWinSizeX - 300.f;
	m_OrthoMainCoinBack.fY = 50.f;
	CAppManager::ComputeOrtho(&m_OrthoMainCoinBack);

	m_OrthoMainCoinIcon.fWidth = 30.f;
	m_OrthoMainCoinIcon.fHeight = 30.f;
	m_OrthoMainCoinIcon.fX = m_OrthoMainCoinBack.fX - (600.f * 0.5f) + 200.f;
	m_OrthoMainCoinIcon.fY = 50.f;
	CAppManager::ComputeOrtho(&m_OrthoMainCoinIcon);

	m_OrthoSubCoinIcon.fWidth = 30.f;
	m_OrthoSubCoinIcon.fHeight = 30.f;
	m_OrthoSubCoinIcon.fX = m_OrthoMainCoinBack.fX - (600.f * 0.5f) + 400.f;
	m_OrthoSubCoinIcon.fY = 50.f;
	CAppManager::ComputeOrtho(&m_OrthoSubCoinIcon);

	for (_uint i = 0; i < 7; ++i)
	{
		m_OrthoMainCoinAmount[i].fWidth = 16.f * 0.7f;
		m_OrthoMainCoinAmount[i].fHeight = 25.f * 0.7f;
		m_OrthoMainCoinAmount[i].fX = m_OrthoMainCoinBack.fX - (600.f * 0.5f) + 220.f + ( i * (15.f * 0.7f));
		m_OrthoMainCoinAmount[i].fY = 50.f;
		CAppManager::ComputeOrtho(&m_OrthoMainCoinAmount[i]);


		m_OrthoSubCoinAmount[i].fWidth = 16.f * 0.7f;
		m_OrthoSubCoinAmount[i].fHeight = 25.f * 0.7f;
		m_OrthoSubCoinAmount[i].fX = m_OrthoMainCoinBack.fX - (600.f * 0.5f) + 420.f + ( i * (15.f * 0.7f));
		m_OrthoSubCoinAmount[i].fY = 50.f;
		CAppManager::ComputeOrtho(&m_OrthoSubCoinAmount[i]);
	}
	
	m_OrthoDetailBack.fWidth = 160.f;
	m_OrthoDetailBack.fHeight = 160.f;
	m_OrthoDetailBack.fX = g_iWinSizeX - 200.f;
	m_OrthoDetailBack.fY = 250.f;
	CAppManager::ComputeOrtho(&m_OrthoDetailBack);

	m_OrthoDetailIcon.fWidth = 150.f;
	m_OrthoDetailIcon.fHeight = 150.f;
	m_OrthoDetailIcon.fX = g_iWinSizeX - 200.f;
	m_OrthoDetailIcon.fY = 250.f;
	CAppManager::ComputeOrtho(&m_OrthoDetailIcon);

	m_OrthoDetailName.fWidth = 512.f * 0.5f;
	m_OrthoDetailName.fHeight = 64.f * 0.5f;
	m_OrthoDetailName.fX = g_iWinSizeX - 200.f;
	m_OrthoDetailName.fY = 350.f;
	CAppManager::ComputeOrtho(&m_OrthoDetailName);

	m_OrthoDetailDesc.fWidth = 720.f * 0.4f;
	m_OrthoDetailDesc.fHeight = 360.f * 0.4f;
	m_OrthoDetailDesc.fX = g_iWinSizeX - 200.f;
	m_OrthoDetailDesc.fY = 470.f;
	CAppManager::ComputeOrtho(&m_OrthoDetailDesc);

	m_OrthoDetailDescBack.fWidth = 720.f * 0.6f;
	m_OrthoDetailDescBack.fHeight = 360.f * 0.6f;
	m_OrthoDetailDescBack.fX = g_iWinSizeX - 200.f;
	m_OrthoDetailDescBack.fY = 470.f;
	CAppManager::ComputeOrtho(&m_OrthoDetailDescBack);

	m_OrthoFoodUse.fWidth = 80.f;
	m_OrthoFoodUse.fHeight = 80.f;
	m_OrthoFoodUse.fX = g_iWinSizeX - 250.f;
	m_OrthoFoodUse.fY = 640.f;
	CAppManager::ComputeOrtho(&m_OrthoFoodUse);

	m_OrthoItemDiscard.fWidth = 80.f;
	m_OrthoItemDiscard.fHeight = 80.f;
	m_OrthoItemDiscard.fX = g_iWinSizeX - 140.f;
	m_OrthoItemDiscard.fY = 640.f;
	CAppManager::ComputeOrtho(&m_OrthoItemDiscard);

	m_OrthoDiscardBack.fWidth = 500.f;
	m_OrthoDiscardBack.fHeight = 400.f;
	m_OrthoDiscardBack.fX = g_iWinSizeX >> 1;
	m_OrthoDiscardBack.fY = g_iWinSizeY >> 1;
	CAppManager::ComputeOrtho(&m_OrthoDiscardBack);

	m_OrthoDiscardExit.fWidth = 50.f;
	m_OrthoDiscardExit.fHeight = 50.f;
	m_OrthoDiscardExit.fX = (g_iWinSizeX >> 1) + 205.f;
	m_OrthoDiscardExit.fY = (g_iWinSizeY >> 1) - 150.f;
	CAppManager::ComputeOrtho(&m_OrthoDiscardExit);

	m_OrthoDiscardSlot.fWidth = 120.f;
	m_OrthoDiscardSlot.fHeight = 120.f;
	m_OrthoDiscardSlot.fX = g_iWinSizeX >> 1;
	m_OrthoDiscardSlot.fY = (g_iWinSizeY >> 1) - 80.f;
	CAppManager::ComputeOrtho(&m_OrthoDiscardSlot);


	m_OrthoDiscardIcon.fWidth = 110.f;
	m_OrthoDiscardIcon.fHeight = 110.f;
	m_OrthoDiscardIcon.fX = g_iWinSizeX >> 1;
	m_OrthoDiscardIcon.fY = (g_iWinSizeY >> 1) - 80.f;
	CAppManager::ComputeOrtho(&m_OrthoDiscardIcon);

	for (_uint i = 0; i < 12; ++i)
	{
		m_OrthoCalcKey[i].fWidth = 50.f;
		m_OrthoCalcKey[i].fHeight = 50.f;
		m_OrthoCalcKey[i].fX = (g_iWinSizeX >> 1) - 150.f + ((i % 3) * 50.f);
		m_OrthoCalcKey[i].fY = (g_iWinSizeY >> 1) + 30.f  + ((i / 3) * 50.f);
		CAppManager::ComputeOrtho(&m_OrthoCalcKey[i]);
	}

	m_OrthoOwnText.fWidth = 512.f * 0.5f;
	m_OrthoOwnText.fHeight = 64.f * 0.5f;
	m_OrthoOwnText.fX = (g_iWinSizeX >> 1) + 150.f;
	m_OrthoOwnText.fY = (g_iWinSizeY >> 1) + 30.f;
	CAppManager::ComputeOrtho(&m_OrthoOwnText);

	m_OrthoDiscardText.fWidth = 512.f * 0.5f;
	m_OrthoDiscardText.fHeight = 64.f * 0.5f;
	m_OrthoDiscardText.fX = (g_iWinSizeX >> 1) + 150.f;
	m_OrthoDiscardText.fY = (g_iWinSizeY >> 1) + 80.f;
	CAppManager::ComputeOrtho(&m_OrthoDiscardText);

	for (_uint i = 0; i < 6; ++i)
	{
		m_OrthoOwnAmount[i].fWidth = 16.f * 0.6f;
		m_OrthoOwnAmount[i].fHeight = 25.f * 0.6f;
		m_OrthoOwnAmount[i].fX = ((g_iWinSizeX >> 1) + 100.f) + (i * (15.f * 0.5f));
		m_OrthoOwnAmount[i].fY = (g_iWinSizeY >> 1) + 30.f;
		CAppManager::ComputeOrtho(&m_OrthoOwnAmount[i]);

		m_OrthoDiscardAmount[i].fWidth = 16.f * 0.6f;
		m_OrthoDiscardAmount[i].fHeight = 25.f * 0.6f;
		m_OrthoDiscardAmount[i].fX = ((g_iWinSizeX >> 1) + 100.f) + (i * (15.f * 0.5f));;
		m_OrthoDiscardAmount[i].fY = (g_iWinSizeY >> 1) + 80.f;
		CAppManager::ComputeOrtho(&m_OrthoDiscardAmount[i]);
	}


#pragma endregion 
	// 직교
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pInven = static_cast<CInventory*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pEchoSystem = static_cast<CEchoSystem*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Echo"));

	return S_OK;
}

void CBackpackUI::Start()
{
}

void CBackpackUI::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);

	if (m_bActive)
	{
		keyInput(TimeDelta);
		elemAlphaUpdate(TimeDelta);
	}
}

void CBackpackUI::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBackpackUI::Render()
{
	// 렌더
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	mainRender();


	if (FAILED(m_pShader->SetRawValue("g_vColor", &_float3(1.f, 1.f, 1.f), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

void CBackpackUI::RenderGUI()
{
}

void CBackpackUI::OnClick()
{
	m_pTerminal->PushActiveUI(this);
}

void CBackpackUI::SetActive(_bool bValue)
{
	m_bActive = bValue;

	if (m_bActive)
		stateActive();
	else
		stateDisable();

}

void CBackpackUI::SetRender(_bool bValue)
{
	m_bRender = bValue;
}

HRESULT CBackpackUI::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI_SUB,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

void CBackpackUI::stateActive()
{
	//초기상태 세팅
	m_bRender = true;

	elemAlphaReset(0);
	m_bElemAlphaStart[0] = true;
	m_fCurDescAlpha = 0.f;

	ZeroMemory(m_bOneFlash, sizeof(m_bOneFlash));
}

void CBackpackUI::stateDisable()
{
	m_bRender = false;
}

void CBackpackUI::elemAlphaUpdate(_double TimeDelta)
{
	_uint iTapInvenCount = m_pInven->GetInvenTotalCount(eInvenType);

	for (_uint i = 0; i < SMOOTH_MAX; ++i)
	{
		if (m_bElemAlphaStart[i])
		{
			m_fElemAlpha[i] += (_float)TimeDelta * 2.f;

			//if (!m_bOneFlash[i] && m_fElemAlpha[i + 2] >= 1.0f)
			/*if (m_fElemAlpha[iTapInvenCount - 1] >= 1.f)
			{
				m_bOneFlash[i] = true;
				m_bFlashBegin[i] = true;
			}*/

			if (m_fElemAlpha[i] >= 1.f)
			{
				m_fElemAlpha[i] = 1.f;
			}
		}

		if (m_fElemAlpha[i] >= 0.3f)
		{
			if (i < SMOOTH_MAX - 1)
				m_bElemAlphaStart[i + 1] = true;

		}
	}

	if (m_fElemAlpha[5] >= 1.f)
	{
		m_fCurDescAlpha += (_float)TimeDelta;
		if (m_fCurDescAlpha >= 1.f)
			m_fCurDescAlpha = 1.f;
	}

	if (!m_bOneFlash[0])
	{
		if (m_fElemAlpha[iTapInvenCount] >= 1.f)
		{
			for (_uint i = 0; i < iTapInvenCount; ++i)
			{
				m_bOneFlash[i] = true;
				m_bFlashBegin[i] = true;
			}
		}
	}


	// 무기, 음식, 재료, 코인 탭 GlowLine 알파
	for (_uint i = 0; i < CInventory::INVEN_MAX; ++i)
	{
		if (eInvenType == i)
		{
			m_fTapAlpha[i] += (_float)TimeDelta;
			if (m_fTapAlpha[i] > 1.f)
				m_fTapAlpha[i] = 1.f;
		}
		else
		{
			m_fTapAlpha[i] -= (_float)TimeDelta;
			if (m_fTapAlpha[i] < 0.5f)
				m_fTapAlpha[i] = 0.5f;
		}
	}

	for (_uint i = 0; i < iTapInvenCount; ++i)
	{
		//Flash Reset
		if (m_bFlashBegin[i])
		{
			m_fCurFlashAcc[i] += (_float)TimeDelta;
			if (m_fCurFlashAcc[i] > 0.01f)
			{
				m_fCurFlashAcc[i] = 0.f;
				m_fCurFlashIndex[i] += 1.f;
				if (m_fCurFlashIndex[i] >= 29.f)
				{
					m_fCurFlashIndex[i] = 0.f;
					m_bFlashBegin[i] = false;
				}
			}
		}
	}

}

void CBackpackUI::elemAlphaReset(_uint iStartIndex)
{
	for (_uint i = iStartIndex; i < SMOOTH_MAX; ++i)
	{
		m_fElemAlpha[i] = 0.f;
		m_bElemAlphaStart[i] = false;
	}

	m_fCurDescAlpha = 0.f;
	ZeroMemory(m_fCurFlashIndex, sizeof(m_fCurFlashIndex));
	ZeroMemory(m_fCurFlashAcc, sizeof(m_fCurFlashAcc));
	ZeroMemory(m_bFlashBegin, sizeof(m_bFlashBegin));
	ZeroMemory(m_bOneFlash, sizeof(m_bOneFlash));
}

void CBackpackUI::keyInput(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();

	_uint iTotalCount = m_pInven->GetInvenTotalCount(eInvenType);
	_uint iCurSlotAmount = m_pInven->GetSlotData(eInvenType, m_iCurSlot).iAmount;
	_uint iCurSlotDigit = (_uint)to_string(iCurSlotAmount).size();

	string strCurSlotAmount = to_string(iCurSlotAmount);

	if (!m_bDiscardPanel)
	{
		// 인벤토리 탭
		for (_uint i = 0; i < CInventory::INVEN_MAX; ++i)
		{
			if (pGM->OnMouse(m_OrthoTapBack[i]))
			{
				if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					if (eInvenType != i)
						elemAlphaReset(2);

					eInvenType = (CInventory::INVEN_TYPE)i;
				}
			}
		}

		// 슬롯 선택
		for (_uint i = 0; i < iTotalCount; ++i)
		{
			if (pGM->OnMouse(m_OrthoSlot[i]))
			{
				if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					if (m_iCurSlot != i)
					{
						m_fCurDescAlpha = 0.f;
						m_iDiscardCurDigit = 0;
						m_iCalcResultAmount = 0;
						ZeroMemory(m_iDiscardAmount, sizeof m_iDiscardAmount);
					}

					m_iCurSlot = i;
					m_bFlashBegin[i] = true;
				}
			}
		}

		if (iTotalCount > m_iCurSlot)
		{
			if (eInvenType == CInventory::INVEN_COOK)
			{
				if (pGM->OnMouse(m_OrthoFoodUse))
				{

				}
				else if (pGM->OnMouse(m_OrthoItemDiscard))
				{
					if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_bDiscardPanel = true;
					}
				}
			}
			else
			{
				if (pGM->OnMouse(m_OrthoItemDiscard))
				{
					if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_bDiscardPanel = true;
					}
				}
			}
		}
	}
	else
	{
		if (pGM->OnMouse(m_OrthoDiscardExit))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_bDiscardPanel = false;
			}
		}
		
		// 수량 입력
		else if (pGM->OnMouse(m_OrthoCalcKey[0]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 7)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 7;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[1]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 8)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 8;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[2]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 9)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 9;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[3]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 4)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 4;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[4]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 5)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 5;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[5]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 6)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 6;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[6]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 1)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 1;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[7]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 2)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 2;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[8]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 3)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 3;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[9]))
		{
			if (m_iDiscardCurDigit > 0 && pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				m_iDiscardAmount[m_iDiscardCurDigit] = 0;
				m_iDiscardCurDigit--;
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[10]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				if (strCurSlotAmount[m_iDiscardCurDigit] - '0' >= 0)
				{
					m_iDiscardAmount[m_iDiscardCurDigit] = 0;
					m_iDiscardCurDigit++;
				}
			}
		}

		else if (pGM->OnMouse(m_OrthoCalcKey[11]))
		{
			if (pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
			{
				for (_uint i = 0; i < iCurSlotDigit; ++i)
				{
					m_iDiscardAmount[i] = strCurSlotAmount[i] - '0';
				}

				m_iDiscardCurDigit = iCurSlotDigit;
			}
		}

		_uint iResult = 1;
		m_iCalcResultAmount = 0;
		for (_uint i = 0; i < iCurSlotDigit; ++i)
		{
			if (i > 0)
				iResult *= 10;
			else
				iResult = 1;
		}

		for (_uint i = 0; i < m_iDiscardCurDigit; ++i)
		{
			m_iCalcResultAmount += m_iDiscardAmount[i] * (_uint)iResult;
			iResult /= 10;
		}

	}
}

_float3 CBackpackUI::getGradeColor(_uint iGrade)
{
	_float3 vColor;
	switch (iGrade)
	{
	case 0:
		vColor = SLOT_COLOR;
		break;
	case 1:
		vColor = SLOT_ADVANCED_COLOR;
		break;
	case 2:
		vColor = SLOT_RARE_COLOR;
		break;
	case 3:
		vColor = SLOT_UNIQUE_COLOR;
		break;
	case 4:
		vColor = SLOT_LEGEND_COLOR;
		break;
	case 5:
		vColor = SLOT_RED_COLOR;
		break;
	default:
		vColor = SLOT_COLOR;
		break;
	}

	return vColor;
}

_float3 CBackpackUI::getGradeToneDownColor(_uint iGrade)
{
	_float3 vColor;
	switch (iGrade)
	{
	case 0:
		vColor = DEFAULT_COLOR;
		break;
	case 1:
		vColor = ADVANCED_COLOR;
		break;
	case 2:
		vColor = RARE_COLOR;
		break;
	case 3:
		vColor = UNIQUE_COLOR;
		break;
	case 4:
		vColor = LEGEND_COLOR;
		break;
	default:
		vColor = DEFAULT_COLOR;
		break;
	}

	return vColor;
}

HRESULT CBackpackUI::mainRender()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_uint iTotalInvenCount = m_pInven->GetInvenTotalCount(eInvenType);

	_float fAlpha = 1.f;
	_float3 vColor3 = { 1.f, 1.f, 1.f };
	_float4 vColor4 = { 1.f, 1.f, 1.f, 1.f };
	
	_float fMainAlpha = 1.f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fMainAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackground.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_DRAGON_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackpackSlot.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOWBUTTON, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 가방 아이콘
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[0], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoBackpackIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_ICON_BACKPACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// 메인 재화
	fAlpha = m_fElemAlpha[1] * 0.2f;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMainCoinBack.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(13);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMainCoinIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_TACTITE_COIN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	_uint iCoin = m_pInven->GetCoin();
	string strCoin = to_string(iCoin);
	_uint iDigit = (_uint)strCoin.size();

	for (_uint i = 0; i < iDigit; ++i)
	{
		fAlpha = m_fElemAlpha[1] * 0.9f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoMainCoinAmount[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCoin[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// 보조 재화
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[1], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSubCoinIcon.WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_COMMEMORATIVE_COIN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	iCoin = m_pInven->GetTotalAmount(CInventory::INVEN_COIN, ITEM::COMMEMORATIVE_COIN);
	strCoin = to_string(iCoin);
	iDigit = (_uint)strCoin.size();

	for (_uint i = 0; i < iDigit; ++i)
	{
		fAlpha = m_fElemAlpha[1] * 0.9f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSubCoinAmount[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(strCoin[i] - '0', m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	// 아이템 텝
	for (_uint i = 0; i < CInventory::INVEN_MAX; ++i)
	{
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fTapAlpha[i], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTapBack[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::GLOW_GREEN_LINE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoTapText[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_INVEN_WEAPON + i, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	_uint iItemCount = m_pInven->GetInvenTotalCount(eInvenType);

	// 아이템 슬롯
	for (_uint i = 0; i < iItemCount/*ITEM_SLOT_MAX*/; ++i)
	{
		if (m_iCurSlot == i)
			vColor3 = DEFAULT_COLOR;
		else
			vColor3 = { 0.5f, 0.5f, 0.5f };

		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSlot[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();
		
		// ItemName Back
		vColor3 = getGradeToneDownColor((_uint)m_pInven->GetSlotData(eInvenType, i).eItemGrade);
		vColor4 = { vColor3.x, vColor3.y, vColor3.z, (m_fElemAlpha[i + 1] / 1.f) * 0.5f };
		if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[i + 2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemTextBack[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_SIDEALPHA_WHITE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(11);
		m_pVIBuffer->Render();

		// ItemName
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[i + 2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemText[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(m_pInven->GetSlotData(eInvenType, i).iImageTextIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (eInvenType != CInventory::INVEN_WEAPON)
		{
			// Amount Text
			vColor4 = { 0.7f, 0.7f, 0.7f, (m_fElemAlpha[i + 1] / 1.f) * 0.5f };
			if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[i + 2], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemAmountText[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_AMOUNT_LEFT, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(11);
			m_pVIBuffer->Render();

			// Amount
			string strItemAmount = to_string(m_pInven->GetSlotData(eInvenType, i).iAmount);
			_uint iDigit = (_uint)strItemAmount.size();

			if (iDigit > 5)
				iDigit = 5;

			for (_uint iIndex = 0; iIndex < iDigit; ++iIndex)
			{
				vColor4 = { 0.7f, 0.7f, 0.7f, (m_fElemAlpha[i + 1] / 1.f) * 0.5f };
				if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
					return E_FAIL;
				if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[i + 2], sizeof(_float))))
					return E_FAIL;
				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemAmount[i][iIndex].WorldMatrix)))
					return E_FAIL;
				if (FAILED(pGameInstance->SetupSRV(strItemAmount[iIndex] - '0', m_pShader, "g_DiffuseTexture")))
					return E_FAIL;

				m_pShader->Begin(11);
				m_pVIBuffer->Render();
			}
		}

		// Item Icon
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemIcon[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(m_pInven->GetSlotData(eInvenType, i).iImageIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// Sprite Flash
		if (m_bFlashBegin[i])
		{
			//OnMouse일떄 클릭할떄 동작하도록 수정할것
			_float2 fXY = { 5.f, 6.f };
			_float4 vColor4 = { 1.f, 1.f, 1.f, 0.3f };
			if (FAILED(m_pShader->SetRawValue("g_CurrentCount", &m_fCurFlashIndex[i], sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_SpriteXY", &fXY, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(m_pShader->SetRawValue("g_vColor4", &vColor4, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSlot[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::SPRITE_UI_FLASH2, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(6);
			m_pVIBuffer->Render();
		}
	}

	// Detail Panel
	if (iTotalInvenCount > 0 && m_iCurSlot < iTotalInvenCount)
	{	
		// GlowSlot
		vColor3 = DEFAULT_COLOR;
		if (FAILED(m_pShader->SetRawValue("g_vColor", &vColor3, sizeof(_float3))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fElemAlpha[2], sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDetailBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();

		//Back
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// Icon
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDetailIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(m_pInven->GetSlotData(eInvenType, m_iCurSlot).iImageIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// Name
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDetailName.WorldMatrix)))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::GLOW_PUPLE_LINE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(pGameInstance->SetupSRV(m_pInven->GetSlotData(eInvenType, m_iCurSlot).iImageTextIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		_float fAlpha = m_fElemAlpha[3] * 0.3f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDetailDescBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShader->Begin(13);
		m_pVIBuffer->Render();

		// 아이템 설명
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDetailDesc.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fCurDescAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fCurDescAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(m_pInven->GetSlotData(eInvenType, m_iCurSlot).iDescTextureID, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(14);
		m_pVIBuffer->Render();

		// 아이템 사용
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoFoodUse.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fCurDescAlpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOW_SOLID_BUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_USE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// 아이템 버리기
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoItemDiscard.WorldMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fCurDescAlpha, sizeof(_float))))
			return E_FAIL;


		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_GLOW_SOLID_BUTTON, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_DISCARD, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	if (m_bDiscardPanel)
	{
		fAlpha = 1.f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDiscardBack.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_PANEL, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDiscardExit.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ICON_BACKPAGE, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDiscardSlot.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_BACK, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDiscardSlot.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::ITEM_SLOT_GLOW, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDiscardIcon.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(m_pInven->GetSlotData(eInvenType, m_iCurSlot).iImageIndex, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		for (_uint i = 0; i < 12; ++i)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoCalcKey[i].WorldMatrix)))
				return E_FAIL;

			switch (i)
			{
			case 0:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_7, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 1:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_8, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 2:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_9, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 3:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_4, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 4:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_5, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 5:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_6, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 6:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_1, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 7:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_2, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 8:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_3, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 9:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_BACK, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 10:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_0, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			case 11:
				if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::KEY_A, m_pShader, "g_DiffuseTexture")))
					return E_FAIL;
				break;
			default:
				break;
			}

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoOwnText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_OWN, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		string strOwn = to_string(m_pInven->GetSlotData(eInvenType, m_iCurSlot).iAmount);
		_uint iDigit = (_uint)strOwn.size();
		if (iDigit >= 6)
			iDigit = 5;

		for (_uint i = 0; i < iDigit; ++i)
		{
			fAlpha *= 0.7f;
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoOwnAmount[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strOwn[i] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

		fAlpha *= 0.7f;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDiscardText.WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_AMOUNT_LEFT, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		strOwn = to_string(m_iCalcResultAmount);
		iDigit = (_uint)strOwn.size();
		if (iDigit > 5)
			iDigit = 5;

		for (_uint i = 0; i < iDigit; ++i)
		{
			if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &fAlpha, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDiscardAmount[i].WorldMatrix)))
				return E_FAIL;
			if (FAILED(pGameInstance->SetupSRV(strOwn[i] - '0', m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			m_pShader->Begin(10);
			m_pVIBuffer->Render();
		}

	}

	return S_OK;
}

CBackpackUI * CBackpackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBackpackUI* pInstance = new CBackpackUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackpackUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBackpackUI::Clone(void * pArg)
{
	CBackpackUI* pInstance = new CBackpackUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackpackUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackpackUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

}
