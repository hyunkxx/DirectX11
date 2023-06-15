#include "stdafx.h"
#include "..\Public\Trigger.h"

#include "GameInstance.h"
#include "GameMode.h"

#include "Character.h"

CTrigger::CTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_TriggerDesc, sizeof(TRIGGER_DESC));
	m_TriggerDesc.fRange = 1.0f;
}

CTrigger::CTrigger(const CTrigger & rhs)
	: CGameObject(rhs)
	, m_TriggerDesc(rhs.m_TriggerDesc)
{
}

HRESULT CTrigger::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_TriggerDesc, pArg, sizeof(TRIGGER_DESC));
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eTriggerType = (CTrigger::TRIGGER_TYPE)m_TriggerDesc.iTriggerType;
	m_eTriggerID = (CTrigger::TRIGGER_ID)m_TriggerDesc.iTriggerID;

	SetUp_State();

	if (TRIGGER_TYPE::TYPE_SPAWN == m_TriggerDesc.iTriggerType && nullptr != m_TriggerDesc.pEditionFilePath)
		Load_SpawnPoint();

	return S_OK;
}

void CTrigger::Start()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	m_pPlayerState = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, TEXT("CharacterState")));
	if (nullptr == m_pPlayerState)
	{
		MSG_BOX("Failed to Find GameObject In Trigger : CharacterState");
		return;
	}

	if (nullptr != m_TriggerDesc.pMonsterLayerTag)
	{
		m_pLayer = pGameInstance->Find_Layer(LEVEL_ANYWHERE, m_TriggerDesc.pMonsterLayerTag);
		if (nullptr == m_pLayer)
		{
			MSG_BOX("Failed to Find Layer In Trigger : MonterLayer");
			return;
		}
	}
}

void CTrigger::PreTick(_double TimeDelta)
{
}

void CTrigger::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (nullptr != m_pMainTransform)
		m_vTriggerPos = m_pMainTransform->Get_State(CTransform::STATE::STATE_POSITION);
	if (nullptr != m_pPlayerState)
		m_vPlayerPos = m_pPlayerState->Get_ActiveCharacter()->Get_Position();
	
	m_fDistance = XMVectorGetX(XMVector3Length(m_vPlayerPos - m_vTriggerPos));

	if (m_TriggerDesc.fRange >= m_fDistance)
		m_IsInTrigger = true;
	else
		m_IsInTrigger = false;

	if (true == m_IsInTrigger)
		Trigger_Condition();
}

void CTrigger::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (true == m_IsTrigger && false == m_IsOnlyOneTrigger)
		Trigger_Process();

	if (true == m_IsSpawnMonster)
		Monster_Spawn();


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
}

HRESULT CTrigger::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		// Model 중 NormalTextrue 가 없는 Model 은 m_IsNormalTex 이 false 가 되어서 노말맵 을 적용시키지 않음.
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource_Distinction(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS, &m_IsNormalTex)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->SetRawValue("g_IsUseNormalTex", &m_IsNormalTex, sizeof(_bool))))
			return E_FAIL;

		m_IsNormalTex = false;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CTrigger::SetUp_State()
{
	_float4		vPos = { m_TriggerDesc.vP.x, m_TriggerDesc.vP.y, m_TriggerDesc.vP.z, 1.0f };
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pMainTransform->Set_Scale(m_TriggerDesc.vS);
	m_pMainTransform->SetRotationXYZ(m_TriggerDesc.vA);
}

void CTrigger::Trigger_Condition()
{
	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	switch (m_eTriggerID)
	{
	case CTrigger::TRIGGER_ID::ID_POTAL_CITY:
	case CTrigger::TRIGGER_ID::ID_POTAL_FOREST:
	case CTrigger::TRIGGER_ID::ID_POTAL_CROWN:
		if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F))
			m_IsTrigger = true;
		break;

	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_0:
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_1:
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_2:
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_3:
	case CTrigger::TRIGGER_ID::ID_SPAWN_CROWN:
	case CTrigger::TRIGGER_ID::ID_INTERACT_COOK:
		if (KEY_STATE::TAP == pGameInstance->InputKey(DIK_F))
			m_IsTrigger = true;
		break;

	default:
		break;
	}
}

void CTrigger::Trigger_Process()
{
	switch (m_eTriggerID)
	{
	case CTrigger::TRIGGER_ID::ID_POTAL_CITY:
		Trigger_Potal_City();
		break;
	case CTrigger::TRIGGER_ID::ID_POTAL_FOREST:
		Trigger_Potal_Forest();
		break;
	case CTrigger::TRIGGER_ID::ID_POTAL_CROWN:
		Trigger_Potal_Crown();
		break;
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_0:
		Trigger_Spawn_Forest_0();
		break;
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_1:
		Trigger_Spawn_Forest_1();
		break;
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_2:
		Trigger_Spawn_Forest_2();
		break;
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_3:
		Trigger_Spawn_Forest_3();
		break;
	case CTrigger::TRIGGER_ID::ID_SPAWN_CROWN:
		Trigger_Spawn_Crown();
		break;
	case CTrigger::TRIGGER_ID::ID_INTERACT_COOK:
		Trigger_Interact_Cook();
		break;

	default:
		break;
	}

	m_IsTrigger = false;
}

void CTrigger::Trigger_Potal_City()
{
	CGameMode*			pGameMode = CGameMode::GetInstance();
	if (nullptr == pGameMode)
		return;

	if (false == pGameMode->Is_ReserveLevel())
		pGameMode->ReserveLevel(LEVEL_ID::LEVEL_CITY);
}

void CTrigger::Trigger_Potal_Forest()
{
	CGameMode*			pGameMode = CGameMode::GetInstance();
	if (nullptr == pGameMode)
		return;

	if (false == pGameMode->Is_ReserveLevel())
		pGameMode->ReserveLevel(LEVEL_ID::LEVEL_FOREST);
}

void CTrigger::Trigger_Potal_Crown()
{
	CGameMode*			pGameMode = CGameMode::GetInstance();
	if (nullptr == pGameMode)
		return;

	if (false == pGameMode->Is_ReserveLevel())
		pGameMode->ReserveLevel(LEVEL_ID::LEVEL_CROWN);
}

void CTrigger::Trigger_Spawn_Forest_0()
{
	m_IsSpawnMonster = true;
	m_IsOnlyOneTrigger = true;

	// 트리거 발동 시 4마리 활성화
	for (auto iter = m_pLayer->m_GameObjects.begin(); iter != m_pLayer->m_GameObjects.end(); ++iter)
	{
		if (m_iMonsterSpawnLimitCount > m_iSpawnMonsterIndex )
		{
			if (nullptr != iter->second)
			{
				if (iter->second->IsDisable())
				{
					static_cast<CCharacter*>(iter->second)->SetUp_Activate(m_SpawnPoints[m_iSpawnPointIndex]);

					Add_SpawnPoint();

					m_iSpawnMonsterIndex++;
					m_iCurrentSpawnMonsterCount++;
				}
			}
		}
	}
}

void CTrigger::Trigger_Spawn_Forest_1()
{
	m_IsSpawnMonster = true;
	m_IsOnlyOneTrigger = true;

	// 트리거 발동 시 4마리 활성화
	for (auto iter = m_pLayer->m_GameObjects.begin(); iter != m_pLayer->m_GameObjects.end(); ++iter)
	{
		if (m_iMonsterSpawnLimitCount > m_iSpawnMonsterIndex)
		{
			if (nullptr != iter->second)
			{
				if (iter->second->IsDisable())
				{
					static_cast<CCharacter*>(iter->second)->SetUp_Activate(m_SpawnPoints[m_iSpawnPointIndex]);

					Add_SpawnPoint();

					m_iSpawnMonsterIndex++;
					m_iCurrentSpawnMonsterCount++;
				}
			}
		}
	}
}

void CTrigger::Trigger_Spawn_Forest_2()
{
	m_IsSpawnMonster = true;
	m_IsOnlyOneTrigger = true;

	// 트리거 발동 시 4마리 활성화
	for (auto iter = m_pLayer->m_GameObjects.begin(); iter != m_pLayer->m_GameObjects.end(); ++iter)
	{
		if (m_iMonsterSpawnLimitCount > m_iSpawnMonsterIndex)
		{
			if (nullptr != iter->second)
			{
				if (iter->second->IsDisable())
				{
					static_cast<CCharacter*>(iter->second)->SetUp_Activate(m_SpawnPoints[m_iSpawnPointIndex]);

					Add_SpawnPoint();

					m_iSpawnMonsterIndex++;
					m_iCurrentSpawnMonsterCount++;
				}
			}
		}
	}
}

void CTrigger::Trigger_Spawn_Forest_3()
{
	m_IsSpawnMonster = true;
	m_IsOnlyOneTrigger = true;

	// 트리거 발동 시 4마리 활성화
	for (auto iter = m_pLayer->m_GameObjects.begin(); iter != m_pLayer->m_GameObjects.end(); ++iter)
	{
		if (m_iMonsterSpawnLimitCount > m_iSpawnMonsterIndex)
		{
			if (nullptr != iter->second)
			{
				if (iter->second->IsDisable())
				{
					static_cast<CCharacter*>(iter->second)->SetUp_Activate(m_SpawnPoints[m_iSpawnPointIndex]);

					Add_SpawnPoint();

					m_iSpawnMonsterIndex++;
					m_iCurrentSpawnMonsterCount++;
				}
			}
		}
	}
}

void CTrigger::Trigger_Spawn_Crown()
{
	//MSG_BOX("Trigger : Trigger_Spawn_Crown");
}

void CTrigger::Trigger_Interact_Cook()
{
	//MSG_BOX("Trigger : Interact_Cook");
}

void CTrigger::Add_SpawnPoint()
{
	m_iSpawnPointIndex++;

	if (m_iSpawnPointCount <= m_iSpawnPointIndex)
		m_iSpawnPointIndex = 0;
}

void CTrigger::Clear_SpawnPoint()
{
	m_iSpawnPointCount = 0;
	m_SpawnPoints.clear();

	m_iSpawnPointIndex = 0;
}

void CTrigger::Clear_MonsterSpawnControl()
{
	Clear_SpawnPoint();

	m_iSpawnMonsterIndex = 0;
	m_IsSpawnMonster = false;

	m_iCurrentSpawnMonsterCount = 0;
}

void CTrigger::Monster_Spawn()
{
	Check_ActiveMonster();

	for (auto iter = m_pLayer->m_GameObjects.begin(); iter != m_pLayer->m_GameObjects.end(); ++iter)
	{
		// 최대 개체 수가 되었을경우 스폰 중단.
		if (m_iMaxSpawnMonsterIndex <= m_iSpawnMonsterIndex)
		{
			//MSG_BOX("Trigger : Spawn_Forest End");
			Clear_MonsterSpawnControl();
			return;
		}

		// 몬스터가 비활성화 -> 죽으면 다시 스폰 후 인덱스 증가.
		if (m_iMonsterSpawnLimitCount > m_iCurrentSpawnMonsterCount)
		{
			if (nullptr != iter->second)
			{
				if (iter->second->IsDisable())
				{
					static_cast<CCharacter*>(iter->second)->SetUp_Activate(m_SpawnPoints[m_iSpawnPointIndex]);

					Add_SpawnPoint();

					m_iSpawnMonsterIndex++;
				}
			}
		}
	}
}

void CTrigger::Check_ActiveMonster()
{
	m_iCurrentSpawnMonsterCount = 0;
	for (auto iter = m_pLayer->m_GameObjects.begin(); iter != m_pLayer->m_GameObjects.end(); ++iter)
	{
		// 몬스터가 비활성화 -> 죽으면 다시 스폰 후 인덱스 증가.

		if (nullptr != iter->second)
		{
			if (iter->second->IsActive())
				m_iCurrentSpawnMonsterCount++;
		}
	}
}

HRESULT CTrigger::Load_SpawnPoint()
{
	HANDLE		hFile = CreateFile(m_TriggerDesc.pEditionFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in Trigger : SpawnPoint");
		return E_FAIL;
	}

	DWORD		dwByte = 0;

	SPAWN_POINT		SpawnPoint = {};
	ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));

	ReadFile(hFile, &m_iSpawnPointCount, sizeof(_uint), &dwByte, nullptr);

	m_SpawnPoints.resize(m_iSpawnPointCount);

	for (_uint i = 0; i < m_iSpawnPointCount; ++i)
	{
		ZeroMemory(&SpawnPoint, sizeof(SPAWN_POINT));

		ReadFile(hFile, &SpawnPoint.vP, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &SpawnPoint.iCellIndex, sizeof(_uint), &dwByte, nullptr);

		m_SpawnPoints[i].vP = SpawnPoint.vP;
		m_SpawnPoints[i].iCellIndex = SpawnPoint.iCellIndex;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTrigger::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;
	m_pMainTransform->Set_Scale(_float3(1.0f, 1.0f, 1.0f));

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_POTAL,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */ // MODEL_INSTANCE MODEL
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


HRESULT CTrigger::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CTrigger * CTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrigger* pInstance = new CTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrigger::Clone(void * pArg)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}