#include "stdafx.h"
#include "..\Public\EffectKey.h"

CEffectKey::CEffectKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CEffectKey::Initialize(BaseData * pData)
{
	__super::Initialize(pData);

	MultiByteToWideChar(CP_ACP, 0,
		pData->szTag,
		int(strlen(pData->szTag)), m_szEffectTag,
		sizeof(_tchar) * MAX_PATH);
	
	m_iEffectBoneID = pData->iInt0;

	m_iTypeID = pData->iInt1;

	if (0 == pData->iInt2)
		m_bTracking = false;
	else
		m_bTracking = true;


	return S_OK;
}

void CEffectKey::Shot(CCharacter * pMyCharacter)
{
	pMyCharacter->Shot_EffectKey(m_szEffectTag, m_iEffectBoneID, m_iTypeID, m_bTracking);
}

CEffectKey * CEffectKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CEffectKey* pInstance = new CEffectKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CEffectKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectKey::Free()
{
	__super::Free();
}
