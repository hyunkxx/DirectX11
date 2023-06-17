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
	_tchar		m_szSoundKey[MAX_PATH];		// tag : ���� ���� �±�
	_uint		m_iChannelID;				// iInt0 = ���� ä�� ID
	_float		m_fVolume;					// fFloat = ���� (max = 1.0)

public:
	static CSoundKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END