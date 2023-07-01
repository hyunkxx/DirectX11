#include "..\Public\Layer.h"

HRESULT CLayer::Add_GameObject(_tchar* szObjectTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;
	
	pGameObject->SetTag(szObjectTag);
	m_GameObjects.emplace(pGameObject->GetTag(), pGameObject);
	
	return S_OK;
}

void CLayer::Start()
{
	for (auto& pGameObject : m_GameObjects)
	{
		pGameObject.second->Start();
	}
}

void CLayer::PreTick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject.second)
		{
			if (pGameObject.second->IsActive())
				pGameObject.second->PreTick(TimeDelta);
		}
	}
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject.second)
		{
			if(pGameObject.second->IsActive())
				pGameObject.second->Tick(TimeDelta);
		}
	}
}

void CLayer::LateTick(_double TimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != iter->second)
		{
			if (iter->second->IsDestroy())
			{
				Safe_Release(iter->second);
				iter = m_GameObjects.erase(iter);
				continue;
			}

			if (iter->second->IsActive() || iter->second->IsNoneTick())
				iter->second->LateTick(TimeDelta);

			iter++;
		}
	}
}

void CLayer::RenderGUI()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject.second)
			pGameObject.second->RenderGUI();
	}
}

CGameObject* CLayer::Find_GameObject(const _tchar* szObjectTag)
{
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), CTagFinder(szObjectTag));
	if (iter == m_GameObjects.end())
		return nullptr;

	return iter->second;
}

CLayer* CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject.second);

	m_GameObjects.clear();
}
