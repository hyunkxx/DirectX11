#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CDissolveKey final : public CStateKey
{
private:
	CDissolveKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDissolveKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_bool m_bDissolveType = { false }; // iInt0 : true == dissoveIn
	_float m_fDissolveSpeed = { 0.f };	// fFloat0

public:
	static CDissolveKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END