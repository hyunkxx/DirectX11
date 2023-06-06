#include "stdafx.h"
#include "..\Public\TraceKey.h"

CTraceKey::CTraceKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CTraceKey::Initialize(BaseData* pData)
{
	__super::Initialize(pData);

	if (0 != pData->iInt0)
		m_bTraceOn = true;
	else
		m_bTraceOn = false;

	if (0 != pData->iInt1)
		m_bOnce = true;
	else
		m_bOnce = false;

	m_Duration = (_double)pData->fFloat0;

	return S_OK;
}

void CTraceKey::Shot(CCharacter * pMyCharacter)
{
	pMyCharacter->Shot_TraceKey(m_bTraceOn, m_bOnce, m_Duration);
}

CTraceKey * CTraceKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CTraceKey* pInstance = new CTraceKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CTraceKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTraceKey::Free()
{
	__super::Free();
}
