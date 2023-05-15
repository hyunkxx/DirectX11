#include "..\Public\Effect_Manager.h"
#include "Effect.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{

}

void CEffect_Manager::Tick(_double TimeDelta)
{

	for (_int i = 0; EFFECT_ID_END > i; i++)
	{
		for (auto& Pair : m_Effectmap_Update[i])
		{
			if (Pair.second->empty())
				continue;

			for (auto& pEffect : (*Pair.second))
			{
				pEffect->Tick(TimeDelta);
			}
		}
	}

}

void CEffect_Manager::Late_Tick(_double TimeDelta)
{
	for (_int i = 0; EFFECT_ID_END > i; i++)
	{
		for (auto& Pair : m_Effectmap_Update[i])
		{
			if (Pair.second->empty())
				continue;

			for (auto& pEffect = Pair.second->begin(); pEffect != Pair.second->end();)
			{
				_bool bUpdate = (*pEffect)->Get_UpdateOK();
				if (true == bUpdate)
				{
					(*pEffect)->LateTick(TimeDelta);
					pEffect++;
				}
				else if (false == bUpdate)
				{
					auto&	Dest = find_if(m_Effectmap_NonUpdate[i].begin(), m_Effectmap_NonUpdate[i].end(), CTagFinder(Pair.first));
					Dest->second->push_back(*pEffect);
					pEffect = Pair.second->erase(pEffect);
				}
			}
		}
	}
	
}

HRESULT CEffect_Manager::Push_Effect(const _tchar * pEffectTag, CEffect * pEffect, EFFECT_ID eEffectID)
{
	if (EFFECT_ID_END <= eEffectID)
		return E_FAIL;

	list <class CEffect*>* pEffectList = nullptr;
	pEffectList = Find_EffectList(pEffectTag, eEffectID);

	if (nullptr != pEffectList)
	{
		pEffectList->push_back(pEffect);
	}
	else
	{
		list <class CEffect*>* EffectList = new list <class CEffect*>;
		list <class CEffect*>* EffectListUpdate = new list <class CEffect*>;

		EffectList->push_back(pEffect);

		_tchar* pFileName = new _tchar[MAX_PATH];
		lstrcpy(pFileName, pEffectTag);

		m_Effectmap_NonUpdate[eEffectID].emplace(pFileName, EffectList);
		m_Effectmap_Update[eEffectID].emplace(pFileName, EffectListUpdate);
	}

	return S_OK;
}

CEffect * CEffect_Manager::Get_Effect(const _tchar * pEffectTag, EFFECT_ID eEffectID)
{
	if (EFFECT_ID_END <= eEffectID)
		return nullptr;

	if (nullptr != Find_EffectList(pEffectTag , eEffectID))
	{
		list<CEffect*>* pEfectlist = Find_EffectList(pEffectTag , eEffectID);
		auto& iter = ((*pEfectlist).begin());
		CEffect*  pEffect = (*iter);
		(*pEfectlist).erase(iter);

		auto	Dest = find_if(m_Effectmap_Update[eEffectID].begin(), m_Effectmap_Update[eEffectID].end(), CTagFinder(pEffectTag));
		if (Dest == m_Effectmap_Update[eEffectID].end())
			return nullptr;

		pEffect->Set_UpdateOK(true);
		Dest->second->push_back(pEffect);

		return pEffect;
	}

	return nullptr;
}

void CEffect_Manager::Stop_All_Effect()
{
	for (_int i = 0; EFFECT_ID_END > i; i++)
	{
		for (auto& Pair : m_Effectmap_Update[i])
		{
			if (Pair.second->empty())
				continue;

			for (auto& pEffect = Pair.second->begin(); pEffect != Pair.second->end(); )
			{
				auto&	Dest = find_if(m_Effectmap_NonUpdate[i].begin(), m_Effectmap_NonUpdate[i].end(), CTagFinder(Pair.first));
				Dest->second->push_back(*pEffect);
				pEffect = Pair.second->erase(pEffect);
			}
		}
	}
}

list <class CEffect*>* CEffect_Manager::Find_EffectList(const _tchar * pEffecttag, EFFECT_ID eEffectID)
{
	if (EFFECT_ID_END <= eEffectID)
		return nullptr;

	auto	Dest = find_if(m_Effectmap_NonUpdate[eEffectID].begin(), m_Effectmap_NonUpdate[eEffectID].end(), CTagFinder(pEffecttag));


	if (Dest == m_Effectmap_NonUpdate[eEffectID].end())
		return nullptr;

	return Dest->second;
}

void CEffect_Manager::Free()
{
	for (_int i = 0; EFFECT_ID_END > i; i++)
	{
		for (auto& Pair : m_Effectmap_Update[i])
		{
			for (auto& pEffect : (*Pair.second))
				Safe_Release(pEffect);

			Pair.second->clear();
			Safe_Delete(Pair.second);
		}
		m_Effectmap_Update[i].clear();

		for (auto& Pair : m_Effectmap_NonUpdate[i])
		{
			for (auto& pEffect : (*Pair.second))
				Safe_Release(pEffect);

			Pair.second->clear();
			Safe_Delete(Pair.second);
			_tchar* pEffectTag = Pair.first;
			Safe_Delete_Array(pEffectTag);
		}
		m_Effectmap_NonUpdate[i].clear();
	}
}
