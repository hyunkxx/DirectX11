#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CSlowKey final : public CStateKey
{
private:
	CSlowKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSlowKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_float m_fTargetTime = { 0.f }; // fFloat0
	_float m_fLerpSpeed = { 0.f };	// iInt0 * 0.01f;

public:
	static CSlowKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END