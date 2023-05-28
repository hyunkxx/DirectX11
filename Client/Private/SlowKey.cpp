#include "stdafx.h"
#include "..\Public\SlowKey.h"

#include "Character.h"


CSlowKey::CSlowKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CSlowKey::Initialize(BaseData* pData)
{
	__super::Initialize(pData);

	m_fTargetTime = pData->fFloat0;

	m_fLerpSpeed = _float(pData->iInt0) * 0.01f;

	return S_OK;
}

void CSlowKey::Shot(CCharacter * pCharacter)
{
	pCharacter->Shot_SlowKey(m_fTargetTime, m_fLerpSpeed);
}

CSlowKey * CSlowKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CSlowKey* pInstance = new CSlowKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CSlowKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSlowKey::Free()
{
	__super::Free();
}
