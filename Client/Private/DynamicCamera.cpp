#include "stdafx.h"
#include "..\Public\DynamicCamera.h"

#include "GameInstance.h"

CDynamicCamera::CDynamicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CDynamicCamera::CDynamicCamera(const CDynamicCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CDynamicCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamicCamera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDynamicCamera::Start()
{
}

void CDynamicCamera::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_CameraDesc;
	if (pGameInstance->InputKey(DIK_W) == KEY_STATE::HOLD)
		m_pMainTransform->MoveForward(TimeDelta);

	if (pGameInstance->InputKey(DIK_S) == KEY_STATE::HOLD)
		m_pMainTransform->MoveBackward(TimeDelta);

	if (pGameInstance->InputKey(DIK_A) == KEY_STATE::HOLD)
		m_pMainTransform->MoveLeft(TimeDelta);

	if (pGameInstance->InputKey(DIK_D) == KEY_STATE::HOLD)
		m_pMainTransform->MoveRight(TimeDelta);

	_long MouseMove = 0;

	if (pGameInstance->InputMouse(DIMK_RB) == KEY_STATE::HOLD)
	{
		if (MouseMove = pGameInstance->InputMouseMove(DIMM_X))
			m_pMainTransform->Rotate(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * TimeDelta * 0.1f);

		if (MouseMove = pGameInstance->InputMouseMove(DIMM_Y))
			m_pMainTransform->Rotate(m_pMainTransform->Get_State(CTransform::STATE_RIGHT), MouseMove * TimeDelta * 0.1f);
	}	

	if (pGameInstance->InputKey(DIK_LCONTROL) == KEY_STATE::HOLD &&
		pGameInstance->InputKey(DIK_PGUP) == KEY_STATE::TAP)
	{
		_float fSpeed = floor(m_pMainTransform->Get_Speed() * 2.f);
		if (fSpeed <= 1280.f)
			m_pMainTransform->Set_Speed(fSpeed);
	}

	if(pGameInstance->InputKey(DIK_LCONTROL) == KEY_STATE::HOLD &&
		pGameInstance->InputKey(DIK_PGDN) == KEY_STATE::TAP)
	{
		_float fSpeed = floor(m_pMainTransform->Get_Speed() * 0.5f);
		if (fSpeed >= 5.f)
			m_pMainTransform->Set_Speed(fSpeed);
	}
	
	__super::Tick(TimeDelta);
}

void CDynamicCamera::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CDynamicCamera::Render()
{
	return S_OK;
}

void CDynamicCamera::RenderGUI()
{
}

HRESULT CDynamicCamera::Add_Components()
{
	return S_OK;
}

HRESULT CDynamicCamera::Setup_ShaderResources()
{
	return S_OK;
}

_float CDynamicCamera::GetCameraSpeed()
{
	if (m_pMainTransform == nullptr)
		return 0.f;
	return m_pMainTransform->Get_Speed();
}

CDynamicCamera* CDynamicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDynamicCamera*	pInstance = new CDynamicCamera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDynamicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDynamicCamera::Clone(void* pArg)
{
	CDynamicCamera*	pInstance = new CDynamicCamera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDynamicCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicCamera::Free()
{
	__super::Free();
}
