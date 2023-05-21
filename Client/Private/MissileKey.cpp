#include "stdafx.h"
#include "..\Public\MissileKey.h"

CMissileKey::CMissileKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CMissileKey::Initialize(BaseData * pData)
{
	__super::Initialize(pData);


	m_iMissilePoolID = pData->iInt0;
	m_iEffectBoneID = pData->iInt1;

	return S_OK;
}

void CMissileKey::Shot(CCharacter * pMyCharacter)
{
	pMyCharacter->Shot_MissileKey(m_iMissilePoolID, m_iEffectBoneID);
}

CMissileKey * CMissileKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CMissileKey* pInstance = new CMissileKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CMissileKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissileKey::Free()
{
	__super::Free();
}
