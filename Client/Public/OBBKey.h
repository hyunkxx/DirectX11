#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

// 무기 OBB 상태 변경용 키
class COBBKey final : public CStateKey
{
private:
	COBBKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COBBKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_bool m_bTargetState = false;

public:
	static COBBKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END