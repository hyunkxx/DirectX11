#pragma once
#include "Base.h"

BEGIN(Engine)

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

public:
	HRESULT Push_Effect(const _tchar* pEffectTag, class CEffect* pEffect);
	class CEffect*	Get_Effect(const _tchar* EffectTag);

private:
	list <class CEffect*>*	Find_EffectList(const _tchar* pEffecttag);

private:
	unordered_map < const _tchar*, list <class CEffect*> > m_Effectmap_Update;
	unordered_map < const _tchar*, list <class CEffect*> > m_Effectmap_NonUpdate;

public:
	virtual void Free()override;
};
END
