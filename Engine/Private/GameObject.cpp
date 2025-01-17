#include "..\Public\GameObject.h"

#include "GameInstance.h"
#include "Object_Manager.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{	
	m_iObjectID = CObject_Manager::GetCurrentID();
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
	, m_iObjectID{ rhs.m_iObjectID }
	, m_IsNormalTex{ rhs.m_IsNormalTex }
	, m_bClone(true)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

void CGameObject::Start()
{
}

void CGameObject::PreTick(_double TimeDelta)
{
}

void CGameObject::Tick(_double TimeDelta)
{
}

void CGameObject::LateTick(_double TimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::RenderShadow()
{
	return S_OK;
}

void CGameObject::RenderGUI()
{
	return;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, _int iComponent, _tchar* pComponentTag, CComponent** ppOut, void * pArg)
{
	_tchar* pTag = new _tchar[MAX_TAG];
	lstrcpy(pTag, pComponentTag);

	if (nullptr != Find_Component(pTag))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, iComponent, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;
	m_Components.emplace(pTag, pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Erase_Component(_tchar * pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if (nullptr != pComponent)
	{
		Safe_Release(pComponent);


		for (auto& pair : m_Components)
		{
			if (!lstrcmp(pComponentTag, pair.first))
			{
				m_Components.erase(pair.first);
				break;
			}
		}
	}
}

_float CGameObject::ComputeCameraLength()
{
	if (nullptr == m_pMainTransform)
		return 0.f;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	return XMVectorGetX(XMVector3Length(m_pMainTransform->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&pPipeLine->Get_CamPosition())));
}

CComponent* CGameObject::Find_Component(const _tchar* pComponentTag)
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& pair : m_Components)
	{
		Safe_Release(pair.second);
		delete pair.first;
	}
	m_Components.clear();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}