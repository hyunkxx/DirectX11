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
	_bool m_bTargetState = false;	// iInt0 : OBB On/Off
	_uint m_iAttackInfoID = 0;		// iInt1 : 대상의 iAttackInfoID를 세팅해주는 변수

public:
	static COBBKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END