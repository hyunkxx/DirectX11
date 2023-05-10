#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

// 플레이어가 보유한 각 파츠의 상태를 변경하기 위한 키
class CPartsKey final : public CStateKey
{
private:
	CPartsKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPartsKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	_uint m_iPartsID = { 0 };	// iInt0 : 몇 번파츠를?
	_uint m_iPartsState = { 0 }; // iInt1 : 어떤 상태로(수납/사용)
	_uint m_iDissolveType = { 0}; // iInt2 : 디졸브 인/아웃
	_double m_DissolveDuration = { 0.0 }; // fFloat : 디졸브 Duration

public:
	static CPartsKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END