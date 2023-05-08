#include "..\Public\Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel* pCurrentLevel)
{
	m_bTickStart = false;

	CObject_Manager* pObject_Manager = CObject_Manager::GetInstance();
	pObject_Manager->Clear(m_iLevelIndex);

	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	pComponent_Manager->Clear(m_iLevelIndex);

	Safe_Release(m_pCurrentLevel);
	m_pCurrentLevel = pCurrentLevel;
	m_iLevelIndex = iLevelIndex;

	pObject_Manager->Start(iLevelIndex);

	return S_OK;
}

void CLevel_Manager::Tick_Level(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	//if (!m_bTickStart)
	//{
	//	m_bTickStart = true;
	//	CObject_Manager* pObject_Manager = CObject_Manager::GetInstance();
	//	pObject_Manager->Start(m_iLevelIndex);
	//}

	m_pCurrentLevel->Tick(TimeDelta);
}

void CLevel_Manager::RenderLevelUI()
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->RenderLevelUI();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
