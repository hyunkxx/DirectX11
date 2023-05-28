#include "stdafx.h"
#include "..\Public\DissolveKey.h"

#include "Character.h"


CDissolveKey::CDissolveKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CDissolveKey::Initialize(BaseData* pData)
{
	__super::Initialize(pData);

	if (pData->iInt0 != 0)
		m_bDissolveType = true;
	else
		m_bDissolveType = false;

	m_fDissolveSpeed = pData->fFloat0;

	return S_OK;
}

void CDissolveKey::Shot(CCharacter * pCharacter)
{
	pCharacter->Shot_DissolveKey(m_bDissolveType, m_fDissolveSpeed);
}

CDissolveKey * CDissolveKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CDissolveKey* pInstance = new CDissolveKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CDissolveKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDissolveKey::Free()
{
	__super::Free();
}
