#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

// 애니메이션 우선 순위 변경용 키, attack 등 연속기에 사용
class CPriorityKey final : public CStateKey
{
private:
	CPriorityKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPriorityKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pCharacter) override;

private:
	_uint m_iLeavePriority = 0; // iInt0 : 변경 후 현재 애니메이션의 이탈 우선순위

public:
	static CPriorityKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END