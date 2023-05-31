#include "stdafx.h"
#include "..\Public\TerminalUI.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

#include "CameraMovement.h"
#include "PlayerState.h"

CTerminalUI::CTerminalUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerminalUI::CTerminalUI(const CTerminalUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerminalUI::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerminalUI::Initialize(void * pArg)
{
	CAppManager* pApp = CAppManager::GetInstance();

	// MainFrame
	ZeroMemory(m_bActive, sizeof(_bool) * TERMINAL_END);
	ZeroMemory(m_fAlpha, sizeof(_float) * TERMINAL_END);
	ZeroMemory(m_fScaleAcc, sizeof(_float) * TERMINAL_END);

	// Slot
	for (int i = 0; i < SLOT_MAX; ++i)
		m_fSlotAlpha[i] = 1.f;
	
	ZeroMemory(m_bSlotOnMouse, sizeof(_bool) * SLOT_MAX);

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	if (FAILED(addGameObjects()))
		return E_FAIL;

	// Ortho Settings
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

#pragma region TERMINAL_ORTHO
	// Terminal 
	m_OrthoDesc[TERMINAL_MAINFRAME].fWidth = g_iWinSizeX;
	m_OrthoDesc[TERMINAL_MAINFRAME].fHeight = g_iWinSizeY;
	m_OrthoDesc[TERMINAL_MAINFRAME].fX = g_iWinSizeX >> 1;
	m_OrthoDesc[TERMINAL_MAINFRAME].fY = g_iWinSizeY >> 1;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_MAINFRAME]);

	// Left Start 300x300
	m_OrthoDesc[TERMINAL_LEFTSTAR].fWidth = 150.f;
	m_OrthoDesc[TERMINAL_LEFTSTAR].fHeight = 150.f;
	m_OrthoDesc[TERMINAL_LEFTSTAR].fX = (g_iWinSizeX >> 1) >> 1;
	m_OrthoDesc[TERMINAL_LEFTSTAR].fY = g_iWinSizeY >> 1;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEFTSTAR]);

	// Left Center 100x100
	m_OrthoDesc[TERMINAL_LEFTCENTER].fWidth = 50.f;
	m_OrthoDesc[TERMINAL_LEFTCENTER].fHeight = 50.f;
	m_OrthoDesc[TERMINAL_LEFTCENTER].fX = (g_iWinSizeX >> 1) >> 1;
	m_OrthoDesc[TERMINAL_LEFTCENTER].fY = (g_iWinSizeY >> 1) + (m_OrthoDesc[TERMINAL_LEFTCENTER].fHeight * 0.375f);
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEFTCENTER]);

	// Left HalfCircle Out
	m_OrthoDesc[TERMINAL_LEFTHALF_OUT].fWidth = 208.f * 0.525f;
	m_OrthoDesc[TERMINAL_LEFTHALF_OUT].fHeight = 276.f * 0.55f;
	m_OrthoDesc[TERMINAL_LEFTHALF_OUT].fX = ((g_iWinSizeX >> 1) >> 1) + m_OrthoDesc[TERMINAL_LEFTHALF_OUT].fWidth * 0.52f;
	m_OrthoDesc[TERMINAL_LEFTHALF_OUT].fY = (g_iWinSizeY >> 1) + (m_OrthoDesc[TERMINAL_LEFTHALF_OUT].fHeight * 0.42f);
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEFTHALF_OUT]);

	// Left HalfCircle In
	m_OrthoDesc[TERMINAL_LEFTHALF_IN].fWidth = 152.f * 0.525f;
	m_OrthoDesc[TERMINAL_LEFTHALF_IN].fHeight = 200.f * 0.55f;
	m_OrthoDesc[TERMINAL_LEFTHALF_IN].fX = ((g_iWinSizeX >> 1) >> 1) + m_OrthoDesc[TERMINAL_LEFTHALF_IN].fWidth * 0.52f;
	m_OrthoDesc[TERMINAL_LEFTHALF_IN].fY = (g_iWinSizeY >> 1) + (m_OrthoDesc[TERMINAL_LEFTHALF_IN].fHeight * 0.48f);
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEFTHALF_IN]);

	// Player Image
	m_OrthoDesc[TERMINAL_PLAYERIMAGE].fWidth = 250.f;
	m_OrthoDesc[TERMINAL_PLAYERIMAGE].fHeight = 250.f;
	m_OrthoDesc[TERMINAL_PLAYERIMAGE].fX = ((g_iWinSizeX >> 1) >> 1);
	m_OrthoDesc[TERMINAL_PLAYERIMAGE].fY = (g_iWinSizeY >> 1) - 35.f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_PLAYERIMAGE]);

#pragma endregion

#pragma region RANK_ORTHO
	// Rank Text
	m_OrthoDesc[TERMINAL_RANKTEXT].fWidth = 512.f * 0.8f;
	m_OrthoDesc[TERMINAL_RANKTEXT].fHeight = 64.f * 0.8f;
	m_OrthoDesc[TERMINAL_RANKTEXT].fX = (((g_iWinSizeX >> 1) >> 1) >> 1);
	m_OrthoDesc[TERMINAL_RANKTEXT].fY = ((g_iWinSizeY >> 1) >> 1) - 10.f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_RANKTEXT]);
	// Rank SubText
	m_OrthoDesc[TERMINAL_RANKSUBTEXT].fWidth = 512.f * 0.5f;
	m_OrthoDesc[TERMINAL_RANKSUBTEXT].fHeight = 64.f * 0.5f;
	m_OrthoDesc[TERMINAL_RANKSUBTEXT].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) + 10.f;
	m_OrthoDesc[TERMINAL_RANKSUBTEXT].fY = ((g_iWinSizeY >> 1) >> 1) - 10.f + (64.f * 0.5f) * 0.5f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_RANKSUBTEXT]);

	// ID Text
	m_OrthoDesc[TERMINAL_ID].fWidth = 512.f * 0.8f;
	m_OrthoDesc[TERMINAL_ID].fHeight = 64.f * 0.8f;
	m_OrthoDesc[TERMINAL_ID].fX = (((g_iWinSizeX >> 1) >> 1) >> 1);
	m_OrthoDesc[TERMINAL_ID].fY = (g_iWinSizeY >> 1)  + 150.f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_ID]);

#pragma endregion

#pragma region LEVEL_ORTHO
	// Level Text
	m_OrthoDesc[TERMINAL_LEVELTEXT].fWidth = 512.f * 0.8f;
	m_OrthoDesc[TERMINAL_LEVELTEXT].fHeight = 64.f * 0.8f;
	m_OrthoDesc[TERMINAL_LEVELTEXT].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) - 50.f;
	m_OrthoDesc[TERMINAL_LEVELTEXT].fY = (g_iWinSizeY >> 1) - 120.f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEVELTEXT]);

	// Level Number0
	m_OrthoLevel[0].fWidth = 18.f;
	m_OrthoLevel[0].fHeight = 28.f;
	m_OrthoLevel[0].fX = (((g_iWinSizeX >> 1) >> 1) >> 1);
	m_OrthoLevel[0].fY = (g_iWinSizeY >> 1) - 120.f;
	pApp->ComputeOrtho(&m_OrthoLevel[0]);
	// Level Number1
	m_OrthoLevel[1].fWidth = 18.f;
	m_OrthoLevel[1].fHeight = 28.f;
	m_OrthoLevel[1].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) + 15.f;
	m_OrthoLevel[1].fY = (g_iWinSizeY >> 1) - 120.f;
	pApp->ComputeOrtho(&m_OrthoLevel[1]);

	// Level SubText
	m_OrthoDesc[TERMINAL_LEVELSUBTEXT].fWidth = 512.f * 0.5f;
	m_OrthoDesc[TERMINAL_LEVELSUBTEXT].fHeight = 64.f * 0.5f;
	m_OrthoDesc[TERMINAL_LEVELSUBTEXT].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) - 37.f;
	m_OrthoDesc[TERMINAL_LEVELSUBTEXT].fY = (g_iWinSizeY >> 1) - 120.f + (64.f * 0.5f) * 0.55f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEVELSUBTEXT]);

	// Level ExpGage
	m_OrthoDesc[TERMINAL_LEVELEXPGAGE].fWidth = 140.f;
	m_OrthoDesc[TERMINAL_LEVELEXPGAGE].fHeight = 5.f;
	m_OrthoDesc[TERMINAL_LEVELEXPGAGE].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) - 12;
	m_OrthoDesc[TERMINAL_LEVELEXPGAGE].fY = (g_iWinSizeY >> 1) - 120.f + (64.f * 0.4f) * 0.5f + 17.f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEVELEXPGAGE]);

	// Level ExpText
	m_OrthoDesc[TERMINAL_LEVELEXPTEXT].fWidth = 512.f * 0.4f;
	m_OrthoDesc[TERMINAL_LEVELEXPTEXT].fHeight = 64.f * 0.4f;
	m_OrthoDesc[TERMINAL_LEVELEXPTEXT].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) - 47.f;
	m_OrthoDesc[TERMINAL_LEVELEXPTEXT].fY = (g_iWinSizeY >> 1) - 120.f + (64.f * 0.4f) * 0.5f + 32.f;
	pApp->ComputeOrtho(&m_OrthoDesc[TERMINAL_LEVELEXPTEXT]);

	// UnionExp
	m_OrthoUnionExp[0].fWidth = 10.f;
	m_OrthoUnionExp[0].fHeight = 15.f;
	m_OrthoUnionExp[0].fX = (((g_iWinSizeX >> 1) >> 1) >> 1);
	m_OrthoUnionExp[0].fY = (g_iWinSizeY >> 1) - 120.f + (64.f * 0.4f) * 0.5f + 32.f;
	pApp->ComputeOrtho(&m_OrthoUnionExp[0]);

	m_OrthoUnionExp[1].fWidth = 10.f;
	m_OrthoUnionExp[1].fHeight = 15.f;
	m_OrthoUnionExp[1].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) + 9.f;
	m_OrthoUnionExp[1].fY = (g_iWinSizeY >> 1) - 120.f + (64.f * 0.4f) * 0.5f + 32.f;
	pApp->ComputeOrtho(&m_OrthoUnionExp[1]);

	m_OrthoUnionExp[2].fWidth = 10.f;
	m_OrthoUnionExp[2].fHeight = 15.f;
	m_OrthoUnionExp[2].fX = (((g_iWinSizeX >> 1) >> 1) >> 1) + 18.f;
	m_OrthoUnionExp[2].fY = (g_iWinSizeY >> 1) - 120.f + (64.f * 0.4f) * 0.5f + 32.f;
	pApp->ComputeOrtho(&m_OrthoUnionExp[2]);

#pragma endregion

#pragma region SLOT_ORTHO
	for (int i = 0; i < SLOT_MAX; ++i)
	{
		// Slot
		m_OrthoSlot[i].fWidth = 90.f;
		m_OrthoSlot[i].fHeight = 100.f;
		m_OrthoSlot[i].fX = (g_iWinSizeX >> 1) - 50.f + ((i % 4) * 150.f);

		if(i < 4)
			m_OrthoSlot[i].fY = (g_iWinSizeY >> 1) - 80.f;
		else
			m_OrthoSlot[i].fY = (g_iWinSizeY >> 1) + 80.f;

		pApp->ComputeOrtho(&m_OrthoSlot[i]);

		// Icon
		if (i == SLOT_SETTING) // 세팅만 크기좀 키움
		{
			m_OrthoSlotIcon[i].fWidth = 55.f;
			m_OrthoSlotIcon[i].fHeight = 55.f;
		}
		else
		{
			m_OrthoSlotIcon[i].fWidth = 45.f;
			m_OrthoSlotIcon[i].fHeight = 45.f;
		}
		m_OrthoSlotIcon[i].fX = (g_iWinSizeX >> 1) - 50.f + ((i % 4) * 150.f);

		if (i < 4)
			m_OrthoSlotIcon[i].fY = (g_iWinSizeY >> 1) - 80.f;
		else
			m_OrthoSlotIcon[i].fY = (g_iWinSizeY >> 1) + 80.f;

		pApp->ComputeOrtho(&m_OrthoSlotIcon[i]);

		// Text
		m_OrthoSlotText[i].fWidth = 250.f;
		m_OrthoSlotText[i].fHeight = 30.f;
		m_OrthoSlotText[i].fX = (g_iWinSizeX >> 1) - 50.f + ((i % 4) * 150.f);

		if (i < 4)
			m_OrthoSlotText[i].fY = (g_iWinSizeY >> 1) - 10.f;
		else
			m_OrthoSlotText[i].fY = (g_iWinSizeY >> 1) + 150.f;

		pApp->ComputeOrtho(&m_OrthoSlotText[i]);
	}

#pragma endregion

	CGameInstance* pGame = CGameInstance::GetInstance();

	m_pCharacterState = static_cast<CPlayerState*>(pGame->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGame->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));

	return S_OK;
}

void CTerminalUI::Start()
{
}

void CTerminalUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	inputKey(TimeDelta);

	activeCheck(TimeDelta);
	activeSlot(TimeDelta);

	m_fTestTimer += (_float)TimeDelta;
	if (m_fTestTimer >= 1.f)
	{
		m_iCount++;
		if (m_iCount > 50)
			m_iCount = 1;

		m_fTestTimer = 0.f;
	}
}

void CTerminalUI::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (!m_pActivateList.empty() && m_pActivateList.top() == this)
		m_bRender = true;

	if (m_fAlpha[TERMINAL_MAINFRAME] > 0.f && m_bRender)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTerminalUI::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	renderMainFrame();
	renderLeftPivot();
	renderSlots();

	return S_OK;
}

void CTerminalUI::RenderGUI()
{
}

void CTerminalUI::inputKey(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_bool isMainCam = CCameraMovement::CAM_MAINPLAYER == m_pCamMovement->GetCurrentCamType();
	if (pGameInstance->InputKey(DIK_ESCAPE) == KEY_STATE::TAP && isMainCam)
	{
		// 최상위 UI가 터미널이 아닐경우 터미널 On 아니면 최상위 UI만 끈다.
		if (!m_pActivateList.empty())
		{
			if (m_pActivateList.top() != this)
			{
				m_pActivateList.top()->SetActive(false);
				m_pActivateList.pop();
				m_pActivateList.top()->SetRender(true);
				m_pActivateList.top()->SetActive(true);
			}
			else
			{
				m_pActivateList.pop();
				m_bSlotActive[0] = m_bActive[TERMINAL_MAINFRAME] = m_bMainActive = !m_bMainActive;
				ShowCursor(m_bMainActive);
			}
		}
		else
		{
			if (m_fAlpha[TERMINAL_MAINFRAME] == 0.f || m_fAlpha[TERMINAL_MAINFRAME] == 1.f)
			{
				if (!m_bActive[TERMINAL_MAINFRAME])
					m_pActivateList.push(this);

				m_bRender = m_bSlotActive[0] = m_bActive[TERMINAL_MAINFRAME] = m_bMainActive = !m_bMainActive;
				ShowCursor(m_bMainActive);
			}
		}

		if (m_bMainActive)
			m_fActiveTimeAcc = 0.f;
	}
		

	if (m_bMainActive)
	{
		m_fActiveTimeAcc += (_float)TimeDelta;
		if (m_fActiveTimeAcc >= 1.f)
			m_fActiveTimeAcc = 1.f;

	}
	else
		shutdownUI(TimeDelta);

	pGM->SetActiveUI(m_bMainActive);
}

void CTerminalUI::shutdownUI(_double TimeDelta)
{
	if (!m_bMainActive)
	{
		for (int i = 0; i < TERMINAL_END; ++i)
		{
			m_bActive[i] = false;
			m_fScaleAcc[i] = 0.f;
		}
	}
}

void CTerminalUI::activeCheck(_double TimeDelta)
{
	for (_uint i = 0; i < TERMINAL_END; ++i)
	{
		_bool bRet = activeDraw((TERMINAL_UI)i, TimeDelta);

		if (m_bActive[i] && !bRet)
			smoothScaleup((TERMINAL_UI)i, TimeDelta);
	}

}

void CTerminalUI::smoothScaleup(TERMINAL_UI eTerminal, _double TimeDelta)
{
	if (eTerminal == TERMINAL_MAINFRAME || eTerminal >= TERMINAL_LEFTHALF_OUT)
		return;

	m_fScaleAcc[eTerminal] += (_float)TimeDelta * 1.5f;
	if (m_fScaleAcc[eTerminal] > 1.f)
		m_fScaleAcc[eTerminal] = 1.f;

	_float fFixedWidth = m_OrthoDesc[eTerminal].fWidth + (m_OrthoDesc[eTerminal].fWidth * m_fScaleAcc[eTerminal]);
	_float fFixedHeight = m_OrthoDesc[eTerminal].fHeight + (m_OrthoDesc[eTerminal].fHeight * m_fScaleAcc[eTerminal]);

	XMStoreFloat4x4(&m_OrthoDesc[eTerminal].WorldMatrix
		, XMMatrixScaling(fFixedWidth, fFixedHeight, 1.f)
		* XMMatrixTranslation(m_OrthoDesc[eTerminal].fX - g_iWinSizeX * 0.5f, -m_OrthoDesc[eTerminal].fY + g_iWinSizeY * 0.5f, 0.f));
}

void CTerminalUI::activeSlot(_double TimeDelta)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CGameInstance* pGI = CGameInstance::GetInstance();
	
	if (m_bSlotActive[0])
	{
		for (int i = 0; i < SLOT_MAX; ++i)
		{
			// 렌더중일 경우
			if (m_bRender)
			{
				if (pGM->OnMouse(m_OrthoSlot[i]))
					m_bSlotOnMouse[i] = true;
				else
					m_bSlotOnMouse[i] = false;

				if (m_bSlotOnMouse[i] && pGI->InputMouse(DIMK_LB) == KEY_STATE::TAP)
				{
					IOnClick* pClickedUI = dynamic_cast<IOnClick*>(m_pSlotUI[i]);

					if (pClickedUI)
						pClickedUI->OnClick();
				}

			}

			if (m_bSlotActive[i])
			{
				m_fSlotAlpha[i] += (_float)TimeDelta;

				if (m_fSlotAlpha[i] > 0.125f && i < SLOT_MAX - 1)
					m_bSlotActive[i + 1] = true;

				if (m_fSlotAlpha[i] > 1.f)
					m_fSlotAlpha[i] = 1.f;
			}
		}
	}
	else
	{
		for (int i = 0 ; i < SLOT_MAX ; ++i)
		{
			m_fSlotAlpha[i] -= (_float)TimeDelta;
			if (m_fSlotAlpha[i] < 0.f)
			{
				m_fSlotAlpha[i] = 0.f;
				m_bSlotActive[i] = false;
			}
		}
	}

}

_int CTerminalUI::getSlotTexture(SLOT_TEXTURE eType, _int iIndex)
{
	_int iTextureID = 0;

	if (eType == SLOT_TEXTURE::SLOT_TEXT)
	{
		switch (iIndex)
		{
		case SLOT_RESONATORS:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_RESONATORS;
			break;
		case SLOT_BACKPACK:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_BACKPACK;
			break;
		case SLOT_TERMINAL:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_TERMINAL;
			break;
		case SLOT_MISSION:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_MISSION;
			break;

		case SLOT_TEAM:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_TEAM;
			break;
		case SLOT_MERGE:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_MERGE;
			break;
		case SLOT_MAP:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_MAP;
			break;
		case SLOT_SETTING:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_TEXT_SETTING;
			break;
		default:
			iTextureID = 0;
			break;
		}
	}
	else if (eType == SLOT_TEXTURE::SLOT_ICON)
	{
		switch (iIndex)
		{
		case SLOT_RESONATORS:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_RESONATORS;
			break;
		case SLOT_BACKPACK:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_BACKPACK;
			break;
		case SLOT_TERMINAL:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_TERMINAL;
			break;
		case SLOT_MISSION:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_MISSION;
			break;

		case SLOT_TEAM:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_TEAM;
			break;
		case SLOT_MERGE:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_MERGE;
			break;
		case SLOT_MAP:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_MAP;
			break;
		case SLOT_SETTING:
			iTextureID = STATIC_IMAGE::UI_TERMINAL_ICON_SETTING;
			break;
		default:
			iTextureID = 0;
			break;
		}
	}

	return iTextureID;
}

_bool CTerminalUI::activeDraw(TERMINAL_UI eTerminal, _double TimeDelta)
{
	if (m_bActive[eTerminal])
	{
		m_fAlpha[eTerminal] += (_float)TimeDelta;
		
		if (m_fAlpha[eTerminal] >= 0.5f && eTerminal < TERMINAL_END - 1)
		{
			m_bActive[eTerminal + 1] = true;
		}

		if (m_fAlpha[eTerminal] >= 1.f)
		{
			m_fAlpha[eTerminal] = 1.f;
			return true;
		}

		return false;
	}
	else
	{
		m_fAlpha[eTerminal] -= (_float)TimeDelta;
		if (m_fAlpha[eTerminal] <= 0.f)
		{
			m_fAlpha[eTerminal] = 0.f;

			//scaleReset(eTerminal);
		}

		return false;
	}
}

void CTerminalUI::PushActiveUI(IActivate * pActivateUI)
{
	if (m_pActivateList.empty())
		return;

	m_pActivateList.top()->SetRender(false);
	m_pActivateList.push(pActivateUI);
	m_pActivateList.top()->SetActive(true);
	m_pActivateList.top()->SetRender(true);
}

HRESULT CTerminalUI::addComponents()
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

HRESULT CTerminalUI::addGameObjects()
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (FAILED(pGI->Add_GameObjectEx(&m_pSlotUI[0], LEVEL_STATIC, OBJECT::UI_TERMINAL_RESONATOR, L"Terminal_UI", L"Resonator", this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerminalUI::renderMainFrame()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Terminal Setup
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_MAINFRAME].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERNINAL_BACK, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_MAINFRAME], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CTerminalUI::renderLeftPivot()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_fAlpha[TERMINAL_LEFTSTAR] <= 0.f)
		return E_FAIL;

	// Left PlayerImage
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_PLAYERIMAGE].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_PLAYER_IMAGE, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTHALF_IN], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Left Star
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEFTSTAR].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEFT0, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTSTAR], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Left Center
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEFTCENTER].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEFT1, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTCENTER], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Left HaflOut
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEFTHALF_OUT].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEFTHALF_OUT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTHALF_OUT], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Left HaflIn
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEFTHALF_IN].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEFTHALF_IN, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTHALF_IN], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// RankText
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_RANKTEXT].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_RANK_TEXT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTHALF_OUT], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// RankSub Text
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_RANKSUBTEXT].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_RANKSUB_TEXT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// LevelText
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEVELTEXT].WorldMatrix)))
		return E_FAIL;

	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEVEL_TEXT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTHALF_IN], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Level Number 0
	string LevelString = to_string(m_iCount);
	int iFirst = LevelString[0] - '0';
	int iSecond = LevelString[1] - '0';

	_float3 fColor = { 1.f, 1.f, 1.f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &fColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(iFirst, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTHALF_IN], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(8);
	m_pVIBuffer->Render();

	if (LevelString.size() > 1)
	{
		// Level Number 1
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoLevel[1].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(iSecond, m_pShader, "g_DiffuseTexture")))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_LEFTHALF_IN], sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(8);
		m_pVIBuffer->Render();
	}

	// LevelSub Text
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEVELSUBTEXT].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEVELSUB_TEXT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Level GageBack
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEVELEXPGAGE].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	fColor = { 0.3f, 0.3f, 0.3f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &fColor, sizeof(_float3))))
		return E_FAIL;

	m_pShader->Begin(7);
	m_pVIBuffer->Render();

	// Level GageFront
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEVELEXPGAGE].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::IMAGE_WHITE_RECT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fFillAmount", &m_fTestTimer, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(2);
	m_pVIBuffer->Render();

	// LevelExp Text
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_LEVELEXPTEXT].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_LEVELEXP_TEXT, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	// Union Exp[3]
	fColor = { 0.8f, 0.8f, 0.8f };
	if (FAILED(m_pShader->SetRawValue("g_vColor", &fColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUnionExp[0].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_1, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(7);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUnionExp[1].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_5, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(7);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoUnionExp[2].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::TEXT_0, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pShader->Begin(7);
	m_pVIBuffer->Render();

	// ID Text
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoDesc[TERMINAL_ID].WorldMatrix)))
		return E_FAIL;
	if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_ID, m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha[TERMINAL_PLAYERIMAGE], sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(m_pShader->SetRawValue("g_vColor", &_float3(1.f, 1.f, 1.f), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerminalUI::renderSlots()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Slot Resource
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	for (int i = 0; i < SLOT_MAX; ++i)
	{
		// Slot
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSlot[i].WorldMatrix)))
			return E_FAIL;

		if (m_bSlotOnMouse[i])
		{
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_SLOT_CLICK, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
		}
		else
		{
			if (FAILED(pGameInstance->SetupSRV(STATIC_IMAGE::UI_TERMINAL_SLOT_NONE, m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fSlotAlpha[i], sizeof(_float))))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// ICON
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSlotIcon[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(getSlotTexture(SLOT_TEXTURE::SLOT_ICON, i), m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();

		// Text
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_OrthoSlotText[i].WorldMatrix)))
			return E_FAIL;
		if (FAILED(pGameInstance->SetupSRV(getSlotTexture(SLOT_TEXTURE::SLOT_TEXT, i), m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		m_pShader->Begin(10);
		m_pVIBuffer->Render();
	}

	return S_OK;
}

CTerminalUI * CTerminalUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerminalUI*	pInstance = new CTerminalUI(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerminalUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerminalUI::Clone(void * pArg)
{
	CTerminalUI*	pInstance = new CTerminalUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerminalUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerminalUI::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);

}
