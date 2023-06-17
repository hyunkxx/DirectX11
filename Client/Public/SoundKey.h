#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CSoundKey final : public CStateKey
{
private:
	CSoundKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSoundKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_tchar		m_szSoundKey[MAX_PATH];		// tag : 사운드 파일 태그
	_uint		m_iChannelID;				// iInt0 = 사운드 채널 ID
	_float		m_fVolume;					// fFloat = 볼륨 (max = 1.0)

public:
	static CSoundKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END