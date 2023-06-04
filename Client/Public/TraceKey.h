#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CTraceKey final : public CStateKey
{
private:
	CTraceKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTraceKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pCharacter) override;

private:
	_bool	m_bTraceOn = 0; // iInt0 : Trace On / Off
	_double m_Duration = 0.0; // fFloat0

public:
	static CTraceKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END