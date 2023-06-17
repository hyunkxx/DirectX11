#include "stdafx.h"
#include "..\Public\SoundKey.h"

#include "Sound_Manager.h"


CSoundKey::CSoundKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CSoundKey::Initialize(BaseData* pData)
{
	__super::Initialize(pData);

	//lstrcpy(m_szSoundKey, szSoundKey);
	//m_iChannelID = iChannelID;
	//m_fVolume = fVolume;

	return S_OK;
}

void CSoundKey::Shot(CCharacter * pMyCharacter)
{
	//CSoundMgr::GetInstance()->StopSound(m_eID);
	//CSoundMgr::GetInstance()->SoundPlay(m_pSoundKey, m_eID, m_fVolume);
}

CSoundKey * CSoundKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, BaseData* pData)
{
	CSoundKey* pInstance = new CSoundKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pData)))
	{
		MSG_BOX("Failed to Create : CSoundKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundKey::Free()
{
	__super::Free();
}
