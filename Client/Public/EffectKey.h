#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CEffectKey final : public CStateKey
{
private:
	CEffectKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEffectKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_tchar m_szEffectTag[MAX_PATH] = TEXT(""); // szTag : ������ ����Ʈ �±�
	_uint m_iEffectBoneID = { 0 };	// iInt0 : ���° EffectBone ����� ������
	_uint m_iTypeID = { 0 };		// iInt1 : EffectTypeID
	_bool m_bTracking = { false }; // iInt2 : Ʈ��ŷ ����
	

public:
	static CEffectKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END