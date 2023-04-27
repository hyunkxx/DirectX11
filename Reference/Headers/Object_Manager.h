#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
public:
	typedef unordered_map<const _tchar*, class CLayer*> LAYERS;

private:
	explicit CObject_Manager() = default;
	virtual ~CObject_Manager() = default;

public:
	static _uint GetCurrentID() { return s_iObjectCurrentID; };

	HRESULT Reserve_Manager(_uint iLevelMaxCount);
	HRESULT Add_Prototype(_int iObjectEnum, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, _int iObjectEnum, const _tchar* pLayerTag, _tchar* pObjectTag, void* pArg = nullptr);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _tchar* pObjectTag);

	void Start(_uint iLevelIndex);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);
	void RenderGUI();
	void Clear(_uint iLevelIndex);

	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	unordered_map<const _tchar*, class CLayer*>* m_pLayers = { nullptr };
private:
	class CGameObject* Find_Prototype(_int iObjectEnum);

public:
	virtual void Free() override;

private: // 원형 객체 보관
	unordered_map<_int, class CGameObject*> m_Prototypes;

private:
	_uint m_iLevelMaxCount;

	static _uint s_iObjectCurrentID;

};

END