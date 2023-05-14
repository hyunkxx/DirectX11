#include "stdafx.h"
#include "..\Public\InteractionObject.h"


CInteractionObject::CInteractionObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CInteractionObject::CInteractionObject(const CInteractionObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CInteractionObject::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractionObject::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInteractionObject::Start()
{
}

void CInteractionObject::Tick(_double TimeDelta)
{
}

void CInteractionObject::LateTick(_double TimeDelta)
{
}

HRESULT CInteractionObject::Render()
{
	return S_OK;
}

HRESULT CInteractionObject::RenderShadow()
{
	return S_OK;
}

void CInteractionObject::RenderGUI()
{
}

void CInteractionObject::Free()
{
	__super::Free();
}