#include "stdafx.h"
#include "..\Public\SoundKey.h"

#include "GameInstance.h"
#include "Sound_Manager.h"


CSoundKey::CSoundKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CStateKey(pDevice, pContext)
{
}

HRESULT CSoundKey::Initialize(BaseData* pData)
{
	__super::Initialize(pData);

	MultiByteToWideChar(CP_ACP, 0,
		pData->szTag,
		int(strlen(pData->szTag)) + 1, m_szSoundKey,
		sizeof(_tchar) * MAX_PATH);

	m_iChannelID = pData->iInt0;

	if (0.0 == pData->fFloat0)
	{
		if (SOUND_PLAYER_SWING <= pData->iInt0 && SOUND_PLAYER_VOICE > pData->iInt0)
			m_fVolume = VOLUME_VFX;
		else if (SOUND_PLAYER_VOICE <= pData->iInt0 && SOUND_NPC_VOICE >= pData->iInt0)
			m_fVolume = VOLUME_VOICE;
		else
			m_fVolume = 0.5f;
	}
	else
		m_fVolume = pData->fFloat0;
		
	return S_OK;
}

void CSoundKey::Shot(CCharacter * pMyCharacter)
{
	pMyCharacter->Shot_SoundKey(m_szSoundKey, m_iChannelID, m_fVolume);
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
