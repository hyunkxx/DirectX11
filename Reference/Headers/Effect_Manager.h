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
	HRESULT Push_Effect(const _tchar* pEffectTag, class CEffect* pEffect , EFFECT_ID eEffectID);
	class CEffect*	Get_Effect(const _tchar* EffectTag , EFFECT_ID eEffectID);

	void	Stop_All_Effect();
private:
	list <class CEffect*>*	Find_EffectList(const _tchar* pEffecttag, EFFECT_ID eEffectID);

private:
	unordered_map <_tchar*, list <class CEffect*>* > m_Effectmap_Update[EFFECT_ID_END];
	unordered_map <_tchar*, list <class CEffect*>* > m_Effectmap_NonUpdate[EFFECT_ID_END];

public:
	virtual void Free()override;
};
END
