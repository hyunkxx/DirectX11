#pragma once

#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	typedef unordered_map<_tchar*, class CGameObject*> GAMEOBJECTS;
	unordered_map<_tchar*, class CGameObject*> m_GameObjects;

public:
	explicit CLayer() = default;
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(_tchar* szObjectTag, CGameObject* pGameObject);
	void Start();
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);
	void RenderGUI();

	CGameObject* Find_GameObject(const _tchar* szObjectTag);
public:
	static CLayer* Create();
	virtual void Free() override;
	
};

END