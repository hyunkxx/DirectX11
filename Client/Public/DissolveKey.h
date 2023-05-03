#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CDissolveKey final : public CStateKey
{
public:
	typedef enum DissolveType
	{
		TYPE_DISSOLVE_IN,
		TYPE_DISSOLVE_OUT,
		TYPE_END
	}TYPE;

private:
	CDissolveKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDissolveKey() = default;

public:
	virtual HRESULT Initialize(_double ShotFrame, TYPE eType, _double DissolveDuration);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	TYPE m_eDissolveType = { TYPE_END };
	_double m_DissolveDuration = { 0.0 };

public:
	static CDissolveKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _double ShotFrame, TYPE eType, _double DissolveDuration);
	virtual void Free() override;
};

END