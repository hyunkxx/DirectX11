#include "stdafx.h"
#include "..\Public\SoundKey.h"

#include "Character.h"

CSoundKey::CSoundKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CSoundKey::Initialize(_double ShotFrame, _tchar* szSoundKey, _uint iChannelID, _float fVolume)
{
	__super::Initialize(ShotFrame);

	lstrcpy(m_szSoundKey, szSoundKey);
	m_iChannelID = iChannelID;
	m_fVolume = fVolume;

	return S_OK;
}

void CSoundKey::Shot(CCharacter * pMyCharacter)
{
	//CSoundMgr::GetInstance()->StopSound(m_eID);
	//CSoundMgr::GetInstance()->SoundPlay(m_pSoundKey, m_eID, m_fVolume);
}

CSoundKey * CSoundKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _double ShotFrame, _tchar* szSoundKey, _uint iChannelID, _float fVolume)
{
	CSoundKey* pInstance = new CSoundKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize(ShotFrame, szSoundKey, iChannelID, fVolume)))
	{
		MSG_BOX("Failed to Create : CSoundKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundKey::Free()
{
}
