#pragma once

#include "StateKey.h"
#include "Character.h"

BEGIN(Client)

class CPartsKey final : public CStateKey
{
public:
	/*typedef enum PartsState
	{
		STATE_HIDE,
		STATE_SHOW,
		STATE_USE,
		STATE_END
	}STATE;

	typedef enum DissolveType
	{
		TYPE_NONE,
		TYPE_DISSOLVEIN,
		TYPE_DISSOLVEOUT,
		TYPE_END
	}TYPE;*/

private:
	CPartsKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPartsKey() = default;

public:
	virtual HRESULT Initialize(BaseData* pData);
	virtual void Shot(class CCharacter* pMyCharacter) override;

private:
	// 몇번 파츠를 / 어떤 상태로(숨김/수납/사용) / 어떤 방식으로(디졸브 적용 여부)
	_uint m_iPartsID = { 0 };
	_uint m_iPartsState = { 0 };
	_uint m_iDissolveType = { 0};
	_double m_DissolveDuration = { 0.0 };

public:
	static CPartsKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BaseData* pData);
	virtual void Free() override;
};

END