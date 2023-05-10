#include "stdafx.h"
#include "..\Public\PriorityKey.h"

CPriorityKey::CPriorityKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CPriorityKey::Initialize(BaseData* pData)
{
	__super::Initialize(pData);

	m_iLeavePriority = pData->iInt0;

	return S_OK;
}

void CPriorityKey::Shot(CCharacter * pMyCharacter)
{
	pMyCharacter->Shot_PriorityKey(m_iLeavePriority);
}

CPriorityKey * CPriorityKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CPriorityKey* pInstance = new CPriorityKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CPriorityKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPriorityKey::Free()
{
	__super::Free();
}
