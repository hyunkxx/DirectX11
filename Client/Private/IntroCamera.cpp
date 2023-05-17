#include "stdafx.h"
#include "..\Public\IntroCamera.h"

#include "GameInstance.h"
#include "Lobby_Character.h"
#include "Level_Loading.h"

CIntroCamera::CIntroCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CIntroCamera::CIntroCamera(const CIntroCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CIntroCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIntroCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	pLobbyCharacter[0] = nullptr;
	pLobbyCharacter[1] = nullptr;

	m_bUse = true;

	tagRectSize[CHAR_LEFT].left = 0;
	tagRectSize[CHAR_LEFT].right = 390;
	tagRectSize[CHAR_LEFT].top = 0;
	tagRectSize[CHAR_LEFT].bottom = g_iWinSizeY;

	tagRectSize[CHAR_RIGHT].left = 900;
	tagRectSize[CHAR_RIGHT].right = g_iWinSizeX;
	tagRectSize[CHAR_RIGHT].top = 0;
	tagRectSize[CHAR_RIGHT].bottom = g_iWinSizeY;


	m_fWidth = g_iWinSizeX * 0.55f;
	m_fHeight = 67.f;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY - 100.f;

	m_fChooseWidth = 220.f;
	m_fChooseHeight = 220.f;
	m_fChooseTextWidth = g_iWinSizeX * 0.6f;
	m_fChooseTextHeight = 70.f;

	m_fChooseX = g_iWinSizeX >> 1;
	m_fChooseY = 110.f;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fWidth, m_fHeight, 1.f) * XMMatrixTranslation(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ChooseWorldMatrix, XMMatrixScaling(m_fChooseWidth, m_fChooseHeight, 1.f) * XMMatrixTranslation(m_fChooseX - g_iWinSizeX * 0.5f, -m_fChooseY + g_iWinSizeY * 0.5f, 0.f));
	XMStoreFloat4x4(&m_ChooseTextWorldMatrix, XMMatrixScaling(m_fChooseTextWidth, m_fChooseTextHeight, 1.f) * XMMatrixTranslation(m_fChooseX - g_iWinSizeX * 0.5f, -(m_fChooseY + 50.f) + g_iWinSizeY * 0.5f, 0.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CIntroCamera::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

 	pLobbyCharacter[CHAR_LEFT] = static_cast<CLobbyCharacter*>(pGameInstance->Find_GameObject(LEVEL_LOGO, L"LobbyCharacter_Left"));
	pLobbyCharacter[CHAR_RIGHT] = static_cast<CLobbyCharacter*>(pGameInstance->Find_GameObject(LEVEL_LOGO, L"LobbyCharacter_Right"));

	assert(pLobbyCharacter[CHAR_LEFT]);
	assert(pLobbyCharacter[CHAR_RIGHT]);
}

void CIntroCamera::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	__super::Tick(TimeDelta);

	if (m_bZoomIn)
		ZoomIn(TimeDelta);
	else
		CameraMovement(TimeDelta);

	StateCheck(TimeDelta);

	if (m_bTapToStartRenderStart)
	{
		if (m_fTwinklTimeAcc < 1.f)
			m_fTwinklTimeAcc += (_float)TimeDelta;
		else
			m_fTwinklTimeAcc = 1.f;
	}

	if (m_bChooseRender)
	{
		m_fAlpha += (_float)TimeDelta;
		if (m_fAlpha > 1.f)
			m_fAlpha = 1.f;
	}
	else
	{
		m_fAlpha -= (_float)TimeDelta * 2.f;
		if (m_fAlpha < 0.f)
			m_fAlpha = 0.f;
	}

}

void CIntroCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CIntroCamera::Render()
{
	renderTapToStartTexture();

	renderApplyTexture();

	renderChooseRoverTexture();

	return S_OK;
}

void CIntroCamera::RenderGUI()
{
}

HRESULT CIntroCamera::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXTURE::TAPSTART_TEXT,
		TEXT("com_texture_tap"), (CComponent**)&m_pTapStartTexture)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXTURE::LOGO_APLLY,
		TEXT("com_texture_apply"), (CComponent**)&m_pApplyTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXTURE::LOGO_TEXT_BACKGROUND,
		TEXT("com_texture_textback"), (CComponent**)&m_pTextBackground)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXTURE::CHOOSE_ROVER_BACK,
		TEXT("com_texture_choose_back"), (CComponent**)&m_pChooseRoverBack)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXTURE::CHOOSE_ROVER_TEXT,
		TEXT("com_texture_choose_text"), (CComponent**)&m_pChooseRoverText)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXTURE::TWINKL_ON_MASK,
		TEXT("com_twinkl_on_mask"), (CComponent**)&m_pTwinklOnMask)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXTURE::TWINKL_OFF_MASK,
		TEXT("com_twinkl_off_mask"), (CComponent**)&m_pTwinklOffMask)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIntroCamera::setupTaptoStartResources()
{
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIntroCamera::setupApplyResources()
{
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CIntroCamera::StateCheck(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if ((m_eCurrentState == CAM_LEFT || m_eCurrentState == CAM_RIGHT) && !m_bMoveLock && !m_bSelected)
	{
		if (pGameInstance->InputKey(DIK_RETURN) == KEY_STATE::TAP)
		{
			m_bSelected = true;
			m_bChooseRender = false;
			m_bApplyRenderStart = false;
			pLobbyCharacter[m_eCurrentState]->PlaySelectedAnimation();
			pGameInstance->PlaySoundEx(L"Intro_Select_Enter.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
		}
	}

	if (m_bSelected)
	{
		pGameInstance->BGMSmoothOff(TimeDelta);

		m_fEndAcc += (_float)TimeDelta;

		if (m_fEndAcc >= 0.5f)
			m_bZoomIn = true;

		if (m_fEndAcc >= m_fEndLimit)
			m_bLobbyOut = true;

	}
}

void CIntroCamera::CameraMovement(_double TimeDelta)
{
	if (m_bSelected)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	POINT pPoint;
	GetCursorPos(&pPoint);
	ScreenToClient(g_hWnd, &pPoint);

	if (m_eCurrentState != CAM_START && m_eCurrentState != CAM_END)
	{
		if (PtInRect(&tagRectSize[CHAR_LEFT], pPoint))
		{
			m_bOnMouse[CHAR_LEFT] = true;
			m_bOnMouse[CHAR_RIGHT] = false;

			if (m_eCurrentState != CAM_LEFT)
				pLobbyCharacter[CHAR_LEFT]->SetMouseInRect(true);

			pLobbyCharacter[CHAR_RIGHT]->SetMouseInRect(false);
		}
		else
		{
			pLobbyCharacter[CHAR_LEFT]->SetMouseInRect(false);
		}

		if (PtInRect(&tagRectSize[CHAR_RIGHT], pPoint))
		{
			m_bOnMouse[CHAR_LEFT] = false;
			m_bOnMouse[CHAR_RIGHT] = true;

			pLobbyCharacter[CHAR_LEFT]->SetMouseInRect(false);

			if (m_eCurrentState != CAM_RIGHT)
				pLobbyCharacter[CHAR_RIGHT]->SetMouseInRect(true);
		}
		else
			static_cast<CLobbyCharacter*>(pLobbyCharacter[CHAR_RIGHT])->SetMouseInRect(false);
	}

	if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP && m_eCurrentState == CAM_END)
	{
		CRenderSetting::RGB_SPLIT_DESC SplitDesc;
		SplitDesc.m_fDistortion = 3.5f;
		SplitDesc.m_fSeparation = 0.1f;
		SplitDesc.m_fStrength = 0.2f;

		pGameInstance->SetSplitDesc(SplitDesc);
		pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DEFAULT, 0.7f);
		pGameInstance->PlaySoundEx(L"Intro_CameraMove.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
		m_eCurrentState = CAM_ORIGIN;
		m_bMoveLock = true;
	}

	if (!m_bMoveLock)
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			if (m_bOnMouse[CHAR_LEFT])
			{
				if (!m_bMoveLock)
				{
					CRenderSetting::RGB_SPLIT_DESC SplitDesc;
					SplitDesc.m_fDistortion = 3.5f;
					SplitDesc.m_fSeparation = 0.1f;
					SplitDesc.m_fStrength = 0.2f;

					pGameInstance->SetSplitDesc(SplitDesc);
					pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DEFAULT, 1.f);
				}

				m_bMoveLock = true;
				pGameInstance->PlaySoundEx(L"Intro_Selet.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
				pGameInstance->PlaySoundEx(L"Intro_CameraMove.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
				m_eCurrentState = CAM_LEFT;
				pLobbyCharacter[CHAR_LEFT]->SetMouseInRect(false);
			}
			else if (m_bOnMouse[CHAR_RIGHT])
			{
				if (!m_bMoveLock)
				{
					CRenderSetting::RGB_SPLIT_DESC SplitDesc;
					SplitDesc.m_fDistortion = 3.5f;
					SplitDesc.m_fSeparation = 0.1f;
					SplitDesc.m_fStrength = 0.2f;

					pGameInstance->SetSplitDesc(SplitDesc);
					pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DEFAULT, 1.f);
				}

				m_bMoveLock = true;
				pGameInstance->PlaySoundEx(L"Intro_Selet.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
				pGameInstance->PlaySoundEx(L"Intro_CameraMove.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
				m_eCurrentState = CAM_RIGHT;
				pLobbyCharacter[CHAR_RIGHT]->SetMouseInRect(false);
			}
		}
		else if (pGameInstance->InputKey(DIK_ESCAPE) == KEY_STATE::TAP)
		{
			m_bMoveLock = true;
			m_eCurrentState = CAM_ORIGIN;
			m_bOnMouse[CHAR_LEFT] = false;
			m_bOnMouse[CHAR_RIGHT] = false;
			pGameInstance->PlaySoundEx(L"Intro_CameraMove.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
		}
	}

	_vector vCurrentPos;
	vCurrentPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);

	switch (m_eCurrentState)
	{
	case CIntroCamera::CAM_LEFT:
		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(121.f, 3.f, 86.5f, 1.f), (_float)TimeDelta * 3.5f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(124.f, 2.4f, 88.5f, 1.f), (_float)TimeDelta * 8.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(121.f, 3.f, 86.5f, 1.f))) < 0.08f)
		{
			m_bChooseRender = true;
			m_bApplyRenderStart = true;
			m_bApplyRender = true;
			m_bMoveLock = false;
		}

		break;
	case CIntroCamera::CAM_RIGHT:
		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(125.f, 2.2f, 86.5f, 1.f), (_float)TimeDelta * 3.5f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(122.f, 2.4f, 88.5f, 1.f), (_float)TimeDelta * 8.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(125.f, 2.2f, 86.5f, 1.f))) < 0.08f)
		{
			m_bChooseRender = true;
			m_bApplyRenderStart = true;
			m_bApplyRender = true;
			m_bMoveLock = false;
		}

		break;
	case CIntroCamera::CAM_ORIGIN:
		if (m_bTapToStartRender)
		{
			m_fTwinklTimeAcc = 0.f;
			m_bTapToStartRender = false;
		}

		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(123.f, 2.5f, 83.f, 1.f), (_float)TimeDelta * 4.f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(123.f, 2.5f, 87.f, 1.f), (_float)TimeDelta * 8.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(123.f, 2.5f, 83.f, 1.f))) < 0.08f)
		{
			m_bChooseRender = true;
			m_bApplyRenderStart = true;
			m_bMoveLock = false;
		}

		break;
	case CIntroCamera::CAM_START:
	{
		static _float ChangeAngle = 0.0;
		static _float RandomX = 0.f;
		static _float RandomY = 2.5f;

		ChangeAngle += (_float)TimeDelta;

		if (ChangeAngle > 1.f)
		{
			ChangeAngle = 0.0;
			RandomX = _float(rand() % 3 - 1);
			RandomY = _float(rand() % 2 + 1);
		}

		if (!m_bStartMove)
		{
			m_fStartTimeAcc += (_float)TimeDelta;
			if (m_fStartTimeAcc >= 6.f)
				m_bStartMove = true;
		}
		else
		{
			CRenderSetting::RGB_SPLIT_DESC SplitDesc;
			SplitDesc.m_fDistortion = 2.5f;
			SplitDesc.m_fSeparation = 0.1f;
			SplitDesc.m_fStrength = 0.2f;

			pGameInstance->SetSplitDesc(SplitDesc);
			pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_REVERSE, 0.5f);
			m_eCurrentState = CAM_END;
			m_bMoveLock = true;
		}

		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(RandomX + 123.f, RandomY, 112.f, 1.f), (_float)TimeDelta * 0.3f));
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		break;
	}
	case CIntroCamera::CAM_END:
		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(123.f, 2.5f, -10.f + 87.f, 1.f), (_float)TimeDelta * 3.f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(123.f, 2.5f, -9.f + 87.f, 1.f), (_float)TimeDelta * 3.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(123.f, 2.5f, -9.f + 87.f, 1.f))) < 0.1f)
		{
			m_bTapToStartRenderStart = true;
			m_bTapToStartRender = true;
			m_bMoveLock = false;
		}
		break;
	}

	if (m_bApplyRender && m_bMoveLock)
	{
		m_fApplyTimeAcc = 0.f;
		m_bApplyRender = false;
	}


	if (m_bMoveLock)
		m_bChooseRender = false;

	m_fApplyTimeAcc += (_float)TimeDelta;
	if (m_fApplyTimeAcc >= 1.f)
	{
		m_fApplyTimeAcc = 1.f;
	}
}

void CIntroCamera::ZoomIn(_double TimeDelta)
{
	static _bool bFadeStart = false;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_vector vCurrentPos;
	_vector vEyePos = pLobbyCharacter[m_eCurrentState]->GetEyePosition();

	vCurrentPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	vCurrentPos = XMVectorLerp(vCurrentPos, vEyePos, (_float)TimeDelta * 0.5f);
	XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), vEyePos, (_float)TimeDelta * 0.3f));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
	m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

	if (!bFadeStart)
	{
		if (XMVectorGetX(XMVector3Length(vEyePos - vCurrentPos)) < 0.4f)
		{
			bFadeStart = true;
			pGameInstance->StartFade(CRenderSetting::FADE_OUT, 1.f);
		}
	}
}

void CIntroCamera::renderTapToStartTexture()
{
	if (!m_bTapToStartRenderStart)
		return;

	if (FAILED(setupTaptoStartResources()))
		return;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fTwinklTimeAcc, sizeof(_float))))
		return;

	if(m_bTapToStartRender)
		m_pTwinklOnMask->Setup_ShaderResource(m_pShader, "g_TwinklMask");
	else
		m_pTwinklOffMask->Setup_ShaderResource(m_pShader, "g_TwinklMask");

	if (FAILED(m_pTextBackground->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(4);
	m_pVIBuffer->Render();

	if (FAILED(m_pTapStartTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(4);
	m_pVIBuffer->Render();

}

void CIntroCamera::renderApplyTexture()
{
	if (!m_bApplyRenderStart)
		return;

	if (FAILED(setupApplyResources()))
		return;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fApplyTimeAcc, sizeof(_float))))
		return;

	if (m_bApplyRender)
		m_pTwinklOnMask->Setup_ShaderResource(m_pShader, "g_TwinklMask");
	else
		m_pTwinklOffMask->Setup_ShaderResource(m_pShader, "g_TwinklMask");

	if (FAILED(m_pTextBackground->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(4);
	m_pVIBuffer->Render();

	if (FAILED(m_pApplyTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(4);
	m_pVIBuffer->Render();

}

void CIntroCamera::renderChooseRoverTexture()
{
	if (!m_bChooseRender && m_fAlpha <= 0.f)
		return;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ChooseWorldMatrix)))
		return;

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fAlpha, sizeof(_float))))
		return;

	if (FAILED(m_pChooseRoverBack->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return;

	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	if (FAILED(m_pChooseRoverText->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_ChooseTextWorldMatrix)))
		return;

	m_pShader->Begin(5);
	m_pVIBuffer->Render();
}

_float CIntroCamera::GetCameraSpeed()
{
	if (m_pMainTransform == nullptr)
		return 0.f;
	return m_pMainTransform->Get_Speed();
}

CIntroCamera* CIntroCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIntroCamera*	pInstance = new CIntroCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CIntroCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CIntroCamera::Clone(void* pArg)
{
	CIntroCamera*	pInstance = new CIntroCamera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CIntroCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIntroCamera::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pTextBackground);
	Safe_Release(m_pTapStartTexture);
	Safe_Release(m_pChooseRoverBack);
	Safe_Release(m_pChooseRoverText);
	Safe_Release(m_pApplyTexture);
	Safe_Release(m_pTwinklOnMask);
	Safe_Release(m_pTwinklOffMask);
}
