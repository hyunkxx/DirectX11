#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

// �÷��̾ ������ �� ������ ���¸� �����ϱ� ���� Ű
class CPartsKey final : public CStateKey
{
private:
	CPartsKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPartsKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_uint m_iPartsID = { 0 };	// iInt0 : �� ��������?
	_uint m_iPartsState = { 0 }; // iInt1 : � ���·�(����/���)
	_uint m_iDissolveType = { 0}; // iInt2 : ������ ��/�ƿ�
	_double m_DissolveDuration = { 0.0 }; // fFloat : ������ Duration

public:
	static CPartsKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END