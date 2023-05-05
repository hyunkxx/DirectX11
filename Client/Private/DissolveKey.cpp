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

	/*m_eDissolveType = eType;
	m_DissolveDuration = DissolveDuration;*/

	return S_OK;
}

void CDissolveKey::Shot(CCharacter * pMyCharacter)
{
	//pMyCharacter->Set_Dissolve(m_eDissolveType, m_DissolveDuration);
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
