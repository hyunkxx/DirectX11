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

}

void CIntroCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CIntroCamera::Render()
{
	return S_OK;
}

void CIntroCamera::RenderGUI()
{
}

HRESULT CIntroCamera::Add_Components()
{
	return S_OK;
}

HRESULT CIntroCamera::Setup_ShaderResources()
{
	return S_OK;
}

void CIntroCamera::StateCheck(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if ((m_eCurrentState == CAM_LEFT || m_eCurrentState == CAM_RIGHT) && !m_bMoveLock)
	{
		if (pGameInstance->InputKey(DIK_RETURN) == KEY_STATE::TAP)
		{
			m_bSelected = true;
			pLobbyCharacter[m_eCurrentState]->PlaySelectedAnimation();
		}
	}

	if (m_bSelected)
	{
		m_fEndAcc += (_float)TimeDelta;

		if (m_fEndAcc >= 0.8f)
		{
			m_bZoomIn = true;
		}

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
		m_eCurrentState = CAM_ORIGIN;
		m_bMoveLock = true;
	}

	if (!m_bMoveLock)
	{
		if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			if (m_bOnMouse[CHAR_LEFT])
			{
				m_bMoveLock = true;
				m_eCurrentState = CAM_LEFT;
				pLobbyCharacter[CHAR_LEFT]->SetMouseInRect(false);
			}
			else if (m_bOnMouse[CHAR_RIGHT])
			{
				m_bMoveLock = true;
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
		}
	}

	_vector vCurrentPos;
	vCurrentPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);

	switch (m_eCurrentState)
	{
	case CIntroCamera::CAM_LEFT:
		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(-2.0f, 1.5f, -0.5f, 1.f), (_float)TimeDelta * 3.5f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(1.f, 0.9f, 1.5f, 1.f), (_float)TimeDelta * 8.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(-2.0f, 1.5f, -0.5f, 1.f))) < 0.08f)
			m_bMoveLock = false;

		break;
	case CIntroCamera::CAM_RIGHT:
		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(2.0f, 1.5f, -0.5f, 1.f), (_float)TimeDelta * 3.5f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(-1.f, 0.9f, 1.5f, 1.f), (_float)TimeDelta * 8.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(2.0f, 1.5f, -0.5f, 1.f))) < 0.08f)
			m_bMoveLock = false;

		break;
	case CIntroCamera::CAM_ORIGIN:
		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(0.f, 1.f, -4.f, 1.f), (_float)TimeDelta * 4.f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(0.f, 1.f, 0.f, 1.f), (_float)TimeDelta * 8.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(0.f, 1.f, -4.f, 1.f))) < 0.08f)
			m_bMoveLock = false;

		break;
	case CIntroCamera::CAM_START:
	{
		static _float ChangeAngle = 0.0;
		static _float RandomX = 0.f;
		static _float RandomY = 1.f;

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
			if (m_fStartTimeAcc >= 8.f)
				m_bStartMove = true;
		}
		else
		{
			CRenderSetting::RGB_SPLIT_DESC SplitDesc;
			SplitDesc.m_fDistortion = 3.f;
			SplitDesc.m_fSeparation = 0.1f;
			SplitDesc.m_fStrength = 0.2f;

			pGameInstance->SetSplitDesc(SplitDesc);
			pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_REVERSE, 0.7f);
			m_eCurrentState = CAM_END;
			m_bMoveLock = true;
		}

		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(RandomX, RandomY, 25.f, 1.f), (_float)TimeDelta * 0.3f));
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		break;
	}
	case CIntroCamera::CAM_END:
		vCurrentPos = XMVectorLerp(vCurrentPos, XMVectorSet(0.f, 1.f, -10.f, 1.f), (_float)TimeDelta * 3.f);
		XMStoreFloat4(&vLookAtPos, XMVectorLerp(XMLoadFloat4(&vLookAtPos), XMVectorSet(0.f, 1.f, -9.f, 1.f), (_float)TimeDelta * 3.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurrentPos);
		m_pMainTransform->LookAt(XMLoadFloat4(&vLookAtPos));

		if (XMVectorGetX(XMVector3Length(vCurrentPos - XMVectorSet(0.f, 1.f, -9.f, 1.f))) < 0.1f)
			m_bMoveLock = false;

		break;
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
}
