#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CMissileKey final : public CStateKey
{
private:
	CMissileKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMissileKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_uint m_iMissilePoolID = { 0 };	// iInt0 : � Missile�� �߻��Ұ���
	_uint m_iEffectBoneID = { 0 };	// iInt1 : � Bone��ġ�� �������� ù ��ġ�� �����

public:
	static CMissileKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END