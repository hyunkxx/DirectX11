#include "stdafx.h"
#include "..\Public\OBBKey.h"


COBBKey::COBBKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT COBBKey::Initialize(BaseData* pData)
{
	__super::Initialize(pData);

	//_int0
	if (pData->iInt0 != 0)
		m_bTargetState = true;
	else
		m_bTargetState = false; 

	m_iAttackInfoID = pData->iInt1;

	return S_OK;
}

void COBBKey::Shot(CCharacter * pCharacter)
{
	pCharacter->Shot_OBBKey(m_bTargetState, m_iAttackInfoID);
}

COBBKey * COBBKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	COBBKey* pInstance = new COBBKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : COBBKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COBBKey::Free()
{
	__super::Free();
}
