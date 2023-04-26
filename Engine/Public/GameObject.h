#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum STATE { ACTIVE, DISABLE, DESTROY };

protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

	//AlphaSort
	_float ComputeCameraLength();

	class CComponent* Find_Component(const _tchar* pComponentTag);
	unordered_map<const _tchar*, class CComponent*> m_Components;

public:

protected:
	HRESULT Add_Component(_uint iLevelIndex, _int iComponent, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

public:
	void SetState(STATE eState) { m_eState = eState; }
	void Destroy() { m_eState = STATE::DESTROY; }
	_bool IsActive() const { return m_eState == STATE::ACTIVE; }
	_bool IsDisable() const { return m_eState == STATE::DISABLE; }
	_bool IsDestroy() const { return m_eState == STATE::DESTROY; }

	_int GetID() const { return m_iObjectID; }
	_bool Compare(CGameObject* pGameObject) const { return this == pGameObject; }

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;

protected:
	_int m_iObjectID = -1;
	STATE m_eState = { ACTIVE };

protected:
	class CTransform* m_pMainTransform = nullptr;

};

END