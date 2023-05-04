#include "..\Public\Effect_Manager.h"
#include "Effect.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{

}

void CEffect_Manager::Tick(_double TimeDelta)
{
	for (auto& Pair : m_Effectmap_Update)
	{
		if (Pair.second.empty())
			continue;

		for (auto& pEffect : Pair.second)
		{
			pEffect->Tick(TimeDelta);
		}
	}
}

void CEffect_Manager::Late_Tick(_double TimeDelta)
{
	for (auto& Pair : m_Effectmap_Update)
	{
		if (Pair.second.empty())
			continue;

		for (auto& pEffect = Pair.second.begin(); pEffect != Pair.second.end(); )
		{
			_bool bUpdate = (*pEffect)->Get_UpdateOK();
			if (true == bUpdate)
			{
				(*pEffect)->LateTick(TimeDelta);
				pEffect++;
			}
			else if (false == bUpdate)
			{
				auto&	Dest = find_if(m_Effectmap_NonUpdate.begin(), m_Effectmap_NonUpdate.end(), CTagFinder(Pair.first));
				Dest->second.push_back(*pEffect);
				pEffect = Pair.second.erase(pEffect);
			}
		}
	}
}

HRESULT CEffect_Manager::Push_Effect(const _tchar * pEffectTag, CEffect * pEffect)
{
	if (nullptr != Find_EffectList(pEffectTag))
	{
		Find_EffectList(pEffectTag)->push_back(pEffect);
	}

	list <class CEffect*> EffectList;
	list <class CEffect*> EffectListUpdate;
	EffectList.push_back(pEffect);

	m_Effectmap_NonUpdate.emplace(pEffectTag, EffectList);
	m_Effectmap_Update.emplace(pEffectTag, EffectListUpdate);

	return S_OK;
}

CEffect * CEffect_Manager::Get_Effect(const _tchar * pEffectTag)
{
	if (nullptr != Find_EffectList(pEffectTag))
	{
		list<CEffect*>* pEfectlist = Find_EffectList(pEffectTag);
		auto& iter = ((*pEfectlist).begin());
		CEffect*  pEffect = (*iter);
		(*pEfectlist).erase(iter);

		auto	Dest = find_if(m_Effectmap_Update.begin(), m_Effectmap_Update.end(), CTagFinder(pEffectTag));
		if (Dest == m_Effectmap_Update.end())
			return nullptr;

		pEffect->Set_UpdateOK(true);
		Dest->second.push_back(pEffect);

		return pEffect;
	}

	return nullptr;
}

list <class CEffect*>* CEffect_Manager::Find_EffectList(const _tchar * pEffecttag)
{
	auto	Dest = find_if(m_Effectmap_NonUpdate.begin(), m_Effectmap_NonUpdate.end(), CTagFinder(pEffecttag));


	if (Dest == m_Effectmap_NonUpdate.end())
		return nullptr;

	return &Dest->second;
}

void CEffect_Manager::Free()
{
	for (auto& Pair : m_Effectmap_Update)
	{
		for (auto& pEffect : Pair.second)
			Safe_Release(pEffect);

		Pair.second.clear();
	}
	m_Effectmap_Update.clear();

	for (auto& Pair : m_Effectmap_NonUpdate)
	{
		for (auto& pEffect : Pair.second)
			Safe_Release(pEffect);

		Pair.second.clear();
	}
	m_Effectmap_NonUpdate.clear();

}
