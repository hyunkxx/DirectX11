#include "..\Public\Component_Manager.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

HRESULT CComponent_Manager::Reserve_Manager(_uint iLevelMaxCount)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iLevelMaxCount];
	m_iLevelMaxCount = iLevelMaxCount;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, _int iComponent, CComponent* pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, iComponent))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(iComponent, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, _int iComponent, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, iComponent);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CComponent_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex == 0)
		return;

	for (auto& pair : m_pPrototypes[iLevelIndex])
	{
		Safe_Release(pair.second);
	}
	m_pPrototypes[iLevelIndex].clear();
}

CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, _int iComponent)
{
	if (nullptr == m_pPrototypes || iLevelIndex >= m_iLevelMaxCount)
		return nullptr;

	for (auto iter = m_pPrototypes[iLevelIndex].begin(); iter != m_pPrototypes[iLevelIndex].end() ; ++iter)
	{
		if (iter->first == iComponent)
			return iter->second;
	}

	return nullptr;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pPrototypes[i])
		{
			Safe_Release(pair.second);
		}
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
