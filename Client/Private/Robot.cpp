#include "stdafx.h"
#include "..\Public\Robot.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Effect.h"

#include "Invisible_Chest.h"
#include "CameraMovement.h"

CRobot::CRobot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CRobot::CRobot(const CRobot & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRobot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	for (_int i = 0; 4 > i; i++)
	{
		m_bHit[i] = false;
	}
	_int iCount = 0;
	for (auto& iter : *((vector<_float4x4>*)pArg))
	{
		m_RobotMatrix[iCount++] = iter;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_BoxMatrix = m_RobotMatrix[3];
	m_BoxMatrix._41 += 2.f;
	_int iNameCount = (_int)(m_BoxMatrix._41 + m_BoxMatrix._42 + m_BoxMatrix._43);

	wsprintf(m_BoxPath, L"%d", iNameCount);

	_tchar ObjectPath[MAX_PATH];
	lstrcpy(ObjectPath, m_BoxPath);

	_float3 vPos = _float3(m_BoxMatrix._41, m_BoxMatrix._42, m_BoxMatrix._43);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANYWHERE, OBJECT::INVISIBLE_CHEST_EXPANDED, L"Layer_Interaction_Object_Forest", ObjectPath, &vPos)))
		return E_FAIL;

	m_pMyBox = pGameInstance->Find_GameObject(LEVEL_ANYWHERE, m_BoxPath);
	static_cast<CInvisible_Chest*>(m_pMyBox)->Off_Detection();

	m_pCamera = pGameInstance->Find_GameObject(LEVEL_STATIC, L"CameraMovement");

	return S_OK;
}

void CRobot::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (_int i = 0; 4 > i; i++)
	{
		if (!m_bHit[i])
		{
			pGameInstance->AddCollider(m_pCollider[i], COLL_PLAYERATTACK);
			m_pCollider[i]->Update(XMLoadFloat4x4(&m_RobotMatrix[i]));
		}
	}
}

void CRobot::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CRobot::Render()
{
	return S_OK;
}

_bool CRobot::All_Hit_Check()
{
	for (_int i = 0; 4 > i; i++)
	{
		if (!m_bHit[i])
			return false;
	}
	return true;
}

void CRobot::Give_Compensation(_int iLast_Hit_Count)
{
	_float3 vPos = _float3(m_RobotMatrix[iLast_Hit_Count]._41, m_RobotMatrix[iLast_Hit_Count]._42, m_RobotMatrix[iLast_Hit_Count]._43);
	vPos.x += 2.f;
	static_cast<CInvisible_Chest*>(m_pMyBox)->Set_Position(vPos);
	static_cast<CInvisible_Chest*>(m_pMyBox)->Appear_Box();

	CGameInstance::GetInstance()->PlaySoundEx(L"DA_Au_Role_Common_Char_ExFlash.wem.wav",
		SOUND_UI_BUTTON, 1.f);

	for (_int i = 0; 4 > i; i++)
	{
		_float4x4 WorldMatrix = m_RobotMatrix[i];

		CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Robot_Clear_Mark", EFFECT_ID::COMON);
		if (nullptr == pEffect)
			return;

		pEffect->Play_Effect(&WorldMatrix, false);

		pEffect = CGameInstance::GetInstance()->Get_Effect(L"Robot_Good_Mark", EFFECT_ID::COMON);
		if (nullptr == pEffect)
			return;

		pEffect->Play_Effect(&WorldMatrix, false);
	}
}

HRESULT CRobot::Add_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (_int i = 0; 4 > i; i++)
	{
		m_pEffect[i] = pGameInstance->Get_Effect(L"Box_Robot",EFFECT_ID::COMON);
		if(nullptr != m_pEffect[i])
			m_pEffect[i]->Play_Effect(&m_RobotMatrix[i],false);
	}

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.5f , 1.f , 0.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider_0"), (CComponent**)&m_pCollider[0], &CollDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider_1"), (CComponent**)&m_pCollider[1], &CollDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider_2"), (CComponent**)&m_pCollider[2], &CollDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider_3"), (CComponent**)&m_pCollider[3], &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CRobot * CRobot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRobot* pInstance = new CRobot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRobot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRobot::Clone(void * pArg)
{
	CRobot* pInstance = new CRobot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CRobot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobot::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);

	for(_int i = 0 ; 4 > i ; i++)
		Safe_Release(m_pCollider[i]);
}

void CRobot::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	for (_int i = 0; 4 > i; i++)
	{
		if (m_pCollider[i] == src && false == m_bHit[i])
		{


			_tchar EffectPath[MAX_PATH];
			_int iEffectNum = i + 1;
			wsprintf(EffectPath, L"Box_Robot_Mark_0%d", iEffectNum);

			m_pHitEffect[i] = CGameInstance::GetInstance()->Get_Effect(EffectPath, EFFECT_ID::COMON);
			
			if (nullptr == m_pHitEffect[i])
				return;
			_float4x4 WorldMatrix = m_RobotMatrix[i];
			WorldMatrix._42 += 1.f;
			m_pHitEffect[i]->Play_Effect(&WorldMatrix, false);
			m_bHit[i] = true;

			static_cast<CCameraMovement*>(m_pCamera)->StartVibration(15.f, 0.5f);

			CGameInstance::GetInstance()->PlaySoundEx(L"Play_Role_Common_Weapon_Crash_2.wem.wav",
				SOUND_UI_TEMP, 1.f);

			if (All_Hit_Check())
				Give_Compensation(i);
		}
	}
}
