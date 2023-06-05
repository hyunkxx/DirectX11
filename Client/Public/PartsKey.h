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
	_uint	m_iPartsID = { 0 };	// iInt0 : �� ��������? (0 : ���� / 1 : ȣ����)
	_uint	m_iPartsState = { 0 }; // iInt1 : � ���·� (0 : ���� / 1 : ���)
	_uint	m_iDissolveType = { 0 }; // iInt2 : ������ ��/�ƿ� (0 : In / 1 : Out)
	_float	m_fDissolveSpeed = { 0.f }; // fFloat0 : ������ Speed

public:
	static CPartsKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END