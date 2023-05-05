#include "stdafx.h"
#include "..\Public\PartsKey.h"

#include "Character.h"


CPartsKey::CPartsKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CPartsKey::Initialize(BaseData * pData)
{
	__super::Initialize(pData);

	m_iPartsID = pData->iInt0;
	m_iPartsState = pData->iInt1;
	m_iDissolveType = pData->iInt2;
	m_DissolveDuration = (_double)pData->fFloat;

	return S_OK;
}

void CPartsKey::Shot(CCharacter * pMyCharacter)
{
	pMyCharacter->Shot_PartsKey(m_iPartsID, m_iPartsState, m_iDissolveType, m_DissolveDuration);
}

CPartsKey * CPartsKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CPartsKey* pInstance = new CPartsKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CPartsKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPartsKey::Free()
{
	__super::Free();
}
