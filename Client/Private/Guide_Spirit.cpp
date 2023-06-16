#include "stdafx.h"
#include "..\Public\Guide_Spirit.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Character.h"
#include "Effect.h"
#include "PlayerState.h"

#include "Invisible_Chest.h"

CGuide_Spirit::CGuide_Spirit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice , pContext)
{
}

CGuide_Spirit::CGuide_Spirit(const CGuide_Spirit & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGuide_Spirit::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGuide_Spirit::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pRouteList = new vector<_float4*>;

	for (auto& iter : *((vector<_float4>*)pArg))
	{
		_float4* pPos = new _float4;
		*(pPos) = iter;
		m_pRouteList->push_back(pPos);
	}

	CGameInstance * pGameInstance = CGameInstance::GetInstance();
	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_fStopCount = 0.f;

	m_eSpiritState = STATE_WAIT;
	m_vPosition = _float3((**(m_pRouteList->begin())).x, (**(m_pRouteList->begin())).y, (**(m_pRouteList->begin())).z);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPosition));

	m_pEffect = CGameInstance::GetInstance()->Get_Effect(L"Guide_Spirit_R",EFFECT_ID::COMON);
	m_pEffect->Play_Effect(m_pMainTransform->Get_WorldMatrixPtr(), true);

	_int iSize = (_int)m_pRouteList->size();
	auto& iter = m_pRouteList->begin();
	iter += (iSize - 1);
	
	iSize = ((_int)(*iter)->x + (_int)(*iter)->y + (_int)(*iter)->z);

	wsprintf(m_BoxPath, L"%d", iSize);

	_float4 vPos = *(*iter);
	vPos.y -= 1.5f;
	_tchar ObjectPath[MAX_PATH];
	lstrcpy(ObjectPath, m_BoxPath);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANYWHERE, OBJECT::INVISIBLE_CHEST_EXPANDED, L"layer_Interaction_Object", ObjectPath, &vPos)))
		return E_FAIL;
	m_pMyBox = pGameInstance->Find_GameObject(LEVEL_ANYWHERE, m_BoxPath);
	static_cast<CInvisible_Chest*>(m_pMyBox)->Off_Detection();

	return S_OK;
}

void CGuide_Spirit::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	XMStoreFloat3(&m_vPosition, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
	
	Particle(TimeDelta);

	switch (m_eSpiritState)
	{
	case Client::CGuide_Spirit::STATE_WAIT:
		Wait_Tick(TimeDelta);
		break;
	case Client::CGuide_Spirit::STATE_GUIDE:
		Guide_Tick(TimeDelta);
		break;
	case Client::CGuide_Spirit::STATE_RECALL:
		ReCall_Tick(TimeDelta);
		break;
	case Client::CGuide_Spirit::STATE_STOP:
		Stop_Tick(TimeDelta);
		break;
	case Client::CGuide_Spirit::STATE_END:
		break;
	default:
		break;
	}

}

void CGuide_Spirit::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	switch (m_eSpiritState)
	{
	case Client::CGuide_Spirit::STATE_WAIT:
		break;
	case Client::CGuide_Spirit::STATE_GUIDE:
		break;
	case Client::CGuide_Spirit::STATE_RECALL:
		break;
	case Client::CGuide_Spirit::STATE_STOP:
		break;
	case Client::CGuide_Spirit::STATE_END:
		break;
	default:
		break;
	}
}

HRESULT CGuide_Spirit::Render()
{
	return S_OK;
}

void CGuide_Spirit::Wait_Tick(_double TimeDelta)
{
	_float3 vPlayerPos;
	_float fDist;

	m_pTarget = m_pPlayerState->Get_ActiveCharacter();
	XMStoreFloat3(&vPlayerPos, m_pTarget->GetTransform()->Get_State(CTransform::STATE::STATE_POSITION));
	fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vPosition) - XMLoadFloat3(&vPlayerPos)));

	if (fDist < 2.f)
	{
		// 이동 한다는 알림 에펙트 띄우기
		++m_iDestinationCount;

		m_pEffect = CGameInstance::GetInstance()->Get_Effect(L"Spilit_Flare", EFFECT_ID::COMON);
		m_pEffect->Play_Effect(m_pMainTransform->Get_WorldMatrixPtr(), true);

		if (m_pRouteList->size() <= m_iDestinationCount)
			m_eSpiritState = STATE_RECALL;
		else
		{
			m_eSpiritState = STATE_GUIDE;
			auto iter = m_pRouteList->begin();
			iter += m_iDestinationCount;
			m_vDestination = _float3((*(*iter)).x, (*(*iter)).y, (*(*iter)).z);
			
			if (1.f == (*(*iter)).w)
				m_bStop = true;
			else
				m_bStop = false;

		}
	}

}

void CGuide_Spirit::Guide_Tick(_double TimeDelta)
{
	_vector vLook;

	vLook = XMVector3Normalize(XMLoadFloat3(&m_vDestination) - XMLoadFloat3(&m_vPosition));

	XMStoreFloat3(&m_vPosition, XMLoadFloat3(&m_vPosition) + (vLook * m_fSpeed * (_float)TimeDelta));

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPosition));
	
	_float fDist = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vDestination) - XMLoadFloat3(&m_vPosition)));

	if (fDist <= 0.1f)
	{
		if (m_bStop)
		{
			m_fStopCount = 0.f;
			m_eSpiritState = STATE_WAIT;
		}
		else
		{
			++m_iDestinationCount;
			auto iter = m_pRouteList->begin();
			iter += m_iDestinationCount;
			m_vDestination = _float3((*(*iter)).x, (*(*iter)).y, (*(*iter)).z);
			if (1.f == (*(*iter)).w)
				m_bStop = true;
			else
				m_bStop = false;
		}
	}
	
}

void CGuide_Spirit::ReCall_Tick(_double TimeDelta)
{
	// 여기서 소환 하는 이펙트 재생
	m_fStopCount += (_float)TimeDelta;


	if (m_fStopCount < 2.f)
	{
		m_vPosition.y -= (_float)TimeDelta;
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vPosition));
	}

	if (!m_bBoxAppear && 2.f < m_fStopCount)
	{
		m_bBoxAppear = true;
		CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Spilit_Mark", EFFECT_ID::COMON);
		pEffect->Play_Effect(m_pMainTransform->Get_WorldMatrixPtr(), false);

		static_cast<CInvisible_Chest*>(m_pMyBox)->Appear_Box();
	}

	if (m_fStopCount > 10.f)
	{
		m_bBoxAppear = false;
		m_eSpiritState = STATE_STOP;
	}
}

void CGuide_Spirit::Stop_Tick(_double TimeDelta)
{
	m_iDestinationCount = 0;
	m_eSpiritState = STATE_WAIT;

	auto iter = m_pRouteList->begin();
	iter += m_iDestinationCount;
	m_vDestination = _float3((*(*iter)).x, (*(*iter)).y, (*(*iter)).z);

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_vDestination));

}

void CGuide_Spirit::Particle(double TimeDelta)
{
	m_fParticle_Count += (_float)TimeDelta;

	if (2.f <= m_fParticle_Count)
	{
		m_fParticle_Count = 0.f;
		CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Guide_Spirit_Particle", EFFECT_ID::COMON);

		if (nullptr == pEffect)
			return;

		pEffect->Play_Effect(m_pMainTransform->Get_WorldMatrixPtr(), true);

	}
}

HRESULT CGuide_Spirit::Add_Components()
{
	//여기서 박스 들고 있을듯

	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	m_pEffect = CGameInstance::GetInstance()->Get_Effect(L"Guide_Spirit_R",EFFECT_ID::COMON);

	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->Play_Effect(m_pMainTransform->Get_WorldMatrixPtr(), true);

	return S_OK;
}

CGuide_Spirit * CGuide_Spirit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGuide_Spirit* pInstance = new CGuide_Spirit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CGuide_Spirit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGuide_Spirit::Clone(void * pArg)
{
	CGuide_Spirit* pInstance = new CGuide_Spirit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CGuide_Spirit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGuide_Spirit::Free()
{
	__super::Free();

	if (m_bClone)
	{
		for (auto& iter : *m_pRouteList)
		{
			Safe_Delete(iter);
		}
		m_pRouteList->clear();
		Safe_Delete(m_pRouteList);
	}
	Safe_Release(m_pMainTransform);
}
