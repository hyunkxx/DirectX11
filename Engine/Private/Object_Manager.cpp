#include "..\Public\Object_Manager.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

_uint CObject_Manager::s_iObjectCurrentID = 0;

HRESULT CObject_Manager::Reserve_Manager(_uint iLevelMaxCount)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iLevelMaxCount];
	m_iLevelMaxCount = iLevelMaxCount;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(_int iObjectEnum, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(iObjectEnum))
		return E_FAIL;

	s_iObjectCurrentID++;
	m_Prototypes.emplace(iObjectEnum, pPrototype);
	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, _int iObjectEnum, const _tchar* pLayerTag, _tchar* strObjectTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(iObjectEnum);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(strObjectTag, pGameObject);
		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		pLayer->Add_GameObject(strObjectTag, pGameObject);
	}

	return S_OK;
}

CGameObject * CObject_Manager::Clone_GameObject(_int iObjectEnum, void * pArg)
{
	CGameObject* pPrototype = Find_Prototype(iObjectEnum);

	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

CGameObject * CObject_Manager::Find_GameObject(_uint iLevelIndex, const _tchar * pObjectTag)
{
	CGameObject* pGameObject = nullptr;

	for (auto& pair : m_pLayers[iLevelIndex])
	{
		pGameObject = pair.second->Find_GameObject(pObjectTag);

		if(nullptr != pGameObject)
			return pGameObject;
	}

	return nullptr;
}

void CObject_Manager::Start(_uint iLevelIndex)
{
	for (auto& pair : m_pLayers[iLevelIndex])
	{
		pair.second->Start();
	}
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->Tick(TimeDelta);
		}
	}
}

void CObject_Manager::LateTick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->LateTick(TimeDelta);
		}
	}
}

void CObject_Manager::RenderGUI()
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->RenderGUI();
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}

CGameObject* CObject_Manager::Find_Prototype(_int iObjectEnum)
{
	for (auto iter = m_Prototypes.begin(); iter != m_Prototypes.end(); ++iter)
	{
		if (iter->first == iObjectEnum)
			return iter->second;
	}

	return nullptr;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= m_iLevelMaxCount)
		return nullptr;

	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			Safe_Release(pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& pair : m_Prototypes)
	{
		Safe_Release(pair.second);
	}

	m_Prototypes.clear();
}
