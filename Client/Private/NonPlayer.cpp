#include "stdafx.h"
#include "..\Public\NonPlayer.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"

CNonPlayer::CNonPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CNonPlayer::CNonPlayer(const CNonPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNonPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODELANIM,
		TEXT("Com_Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

void CNonPlayer::Free()
{
	__super::Free();
	
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
}
