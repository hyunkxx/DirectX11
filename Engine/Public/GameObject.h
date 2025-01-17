#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum STATE { ACTIVE, DISABLE, DESTROY, NONTICK };

protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();

	//AlphaSort
	_float ComputeCameraLength();
	class CTransform* GetTransform()
	{
		return m_pMainTransform != nullptr ? m_pMainTransform : nullptr;
	}

	class CComponent* Find_Component(const _tchar* pComponentTag);
	unordered_map<_tchar*, class CComponent*> m_Components;

public:

protected:
	_bool						m_IsNormalTex = { false };

protected:
	HRESULT Add_Component(_uint iLevelIndex, _int iComponent, _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	void Erase_Component(_tchar* pComponentTag);

public:
	_bool IsClone() const { return m_bClone; }
	void SetState(STATE eState) { m_eState = eState; }
	void Destroy() { m_eState = STATE::DESTROY; }
	_bool IsActive() const { return m_eState == STATE::ACTIVE; }
	_bool IsDisable() const { return m_eState == STATE::DISABLE; }
	_bool IsDestroy() const { return m_eState == STATE::DESTROY; }
	_bool IsNoneTick() const { return m_eState == STATE::NONTICK; }

	_int GetID() const { return m_iObjectID; }
	_tchar* GetTag() { return m_ObjectTag; }
	_bool Compare(CGameObject* pGameObject) const { return this == pGameObject; }

private:
	friend class CLayer;
	void SetTag(const _tchar* pTag)
	{
		lstrcpy(m_ObjectTag, pTag);
	}

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pContext;

protected:
	_bool m_bClone = false;
	_int m_iObjectID = -1;
	_tchar m_ObjectTag[MAX_TAG] = L"";

	STATE m_eState = { ACTIVE };

protected:
	class CTransform* m_pMainTransform = nullptr;

};

END