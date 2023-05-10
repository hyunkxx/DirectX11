#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

// �ִϸ��̼� �켱 ���� ����� Ű, attack �� ���ӱ⿡ ���
class CPriorityKey final : public CStateKey
{
private:
	CPriorityKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPriorityKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pCharacter) override;

private:
	_uint m_iLeavePriority = 0; // iInt0 : ���� �� ���� �ִϸ��̼��� ��Ż �켱����

public:
	static CPriorityKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END