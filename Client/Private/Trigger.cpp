#include "stdafx.h"
#include "..\Public\Trigger.h"

#include "GameInstance.h"
#include "GameMode.h"

#include "Character.h"
#include "Effect.h"

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

	switch (m_TriggerDesc.iTriggerType)
	{
	case Client::CTrigger::TYPE_PORTAL:
		m_pPotal = CGameInstance::GetInstance()->Get_Effect(L"Potal_Effect_01", EFFECT_ID::COMON);
		m_pPotalEffect = CGameInstance::GetInstance()->Get_Effect(L"Potal_Effect_02", EFFECT_ID::COMON);

		if (nullptr == m_pPotal || nullptr == m_pPotalEffect)
			return E_FAIL;

		ZeroMemory(&m_WorldMatrix_Origin, sizeof(_float4x4));
		ZeroMemory(&m_WorldMatrix_Potal, sizeof(_float4x4));
		m_WorldMatrix_Potal = m_WorldMatrix_Origin = m_pMainTransform->Get_WorldMatrix();
		XMStoreFloat3(&m_vOrigin_Pos, m_pMainTransform->Get_State(CTransform::STATE::STATE_POSITION));
		memcpy(&m_WorldMatrix_Potal.m[3], &m_vOrigin_Pos, sizeof(_float3));

		m_pPotal->Play_Effect(&m_WorldMatrix_Potal, true);

		break;
	case Client::CTrigger::TYPE_SPAWN:
		if (nullptr != m_TriggerDesc.pEditionFilePath)
			Load_SpawnPoint();
		break;
	case Client::CTrigger::TYPE_INTERACT:
		break;
	case Client::CTrigger::TYPE_END:
		break;
	default:
		break;
	}

	m_EffectTime = { 5.0 };

	m_fFloating_Power = { 0.10f };

	m_fFloating_Speed = { 2.0f };

	m_FadeStartTime = { 3.0 };
	m_FadeTime = { 2.0 };

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
}

void CTrigger::PreTick(_double TimeDelta)
{
}

void CTrigger::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Distance_Check();

	if (true == m_IsInTrigger)
		Trigger_Condition();

	if (CTrigger::TRIGGER_TYPE::TYPE_PORTAL ==  m_TriggerDesc.iTriggerType)
		Floating(TimeDelta);
}

void CTrigger::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (true == m_IsTriggerEffect)
		ShowEffect(TimeDelta);

	if (true == m_IsTrigger && false == m_IsOnlyOneTrigger)
		Trigger_Process();

	if (TRIGGER_TYPE::TYPE_SPAWN == m_TriggerDesc.iTriggerType)
	{
		if (true == m_IsSpawnMonster)
			Monster_Spawn();
	}
}

HRESULT CTrigger::Render()
{
	return S_OK;
}

void CTrigger::SetUp_State()
{
	_float4		vPos = { m_TriggerDesc.vP.x, m_TriggerDesc.vP.y, m_TriggerDesc.vP.z, 1.0f };
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pMainTransform->Set_Scale(m_TriggerDesc.vS);
	m_pMainTransform->SetRotationXYZ(m_TriggerDesc.vA);
}

void CTrigger::Distance_Check()
{
	if (nullptr != m_pMainTransform)
		m_vTriggerPos = m_pMainTransform->Get_State(CTransform::STATE::STATE_POSITION);
	if (nullptr != m_pPlayerState)
		m_vPlayerPos = m_pPlayerState->Get_ActiveCharacter()->Get_Position();

	m_fDistance = XMVectorGetX(XMVector3Length(m_vPlayerPos - m_vTriggerPos));

	if (m_TriggerDesc.fRange >= m_fDistance)
		m_IsInTrigger = true;
	else
		m_IsInTrigger = false;
}

void CTrigger::Floating(_double TimeDelta)
{
	m_fFloatingTimeAcc += (_float)TimeDelta;

	_float fHeight = sinf(m_fFloatingTimeAcc) * m_fFloating_Power;

	_vector		vP = m_pMainTransform->Get_State(CTransform::STATE::STATE_POSITION);

	vP = XMLoadFloat3(&m_vOrigin_Pos) + VECTOR_UP * fHeight * (_float)TimeDelta * m_fFloating_Speed;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vP);

	memcpy(&m_WorldMatrix_Potal.m[3], &vP, sizeof(_float3));
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
			m_IsTriggerEffect = true;
		break;

	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_0:
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_1:
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_2:
	case CTrigger::TRIGGER_ID::ID_SPAWN_FOREST_3:
	case CTrigger::TRIGGER_ID::ID_SPAWN_CROWN:
	case CTrigger::TRIGGER_ID::ID_INTERACT_COOK:
	case CTrigger::TRIGGER_ID::ID_INTERACT_COOKNPC:
	case CTrigger::TRIGGER_ID::ID_INTERACT_SHOP_1:
	case CTrigger::TRIGGER_ID::ID_INTERACT_SHOP_2:
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
	case CTrigger::TRIGGER_ID::ID_INTERACT_COOKNPC:
		Trigger_Interact_CookNpc();
		break;
	case CTrigger::TRIGGER_ID::ID_INTERACT_SHOP_1:
		Trigger_Interact_Shop_1();
		break;
	case CTrigger::TRIGGER_ID::ID_INTERACT_SHOP_2:
		Trigger_Interact_Shop_2();
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
}

void CTrigger::Trigger_Spawn_Forest_1()
{
	m_IsSpawnMonster = true;
	m_IsOnlyOneTrigger = true;
}

void CTrigger::Trigger_Spawn_Forest_2()
{
	m_IsSpawnMonster = true;
	m_IsOnlyOneTrigger = true;
}

void CTrigger::Trigger_Spawn_Forest_3()
{
	m_IsSpawnMonster = true;
	m_IsOnlyOneTrigger = true;
}

void CTrigger::Trigger_Spawn_Crown()
{
	m_IsOnlyOneTrigger = true;

	// 트리거 발동 시 현재 웨이브로 등록된 몬스터를 활성화.
	for (auto& pMonster : m_pMonsters[m_iCurrent_Wave])
	{
		if (nullptr != pMonster)
		{
			if (pMonster->IsDisable())
			{
				pMonster->SetUp_Activate(m_SpawnPoints[m_iSpawnPointIndex]);
				Add_SpawnPoint();
			}
		}
	}

	Clear_MonsterSpawnControl();
}

void CTrigger::Trigger_Interact_Cook()
{
	//MSG_BOX("Trigger : Interact_Cook");
}

void CTrigger::Trigger_Interact_CookNpc()
{
	//MSG_BOX("Trigger : Interact_CookNpc");
}

void CTrigger::Trigger_Interact_Shop_1()
{
	//MSG_BOX("Trigger : Interact_Shop_1");
}

void CTrigger::Trigger_Interact_Shop_2()
{
	//MSG_BOX("Trigger : Interact_Shop_2");
}

void CTrigger::ShowEffect(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	if (false == m_OnlyOnePlay)
	{
		m_pPotalEffect->Play_Effect(&m_WorldMatrix_Origin);
		m_OnlyOnePlay = true;
	}

	m_EffectTimeAcc += TimeDelta;

	if (m_FadeStartTime <= m_EffectTimeAcc && false == m_IsFade)
	{
		pGameInstance->StartFade(CRenderSetting::FADE_OUT, m_FadeTime);
		m_IsFade = true;
	}

	if (m_EffectTime <= m_EffectTimeAcc)
	{
		m_EffectTimeAcc = 0.0;
		m_IsTriggerEffect = false;
		m_IsTrigger = true;
		m_OnlyOnePlay = false;
		m_IsFade = false;
	}
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

	m_IsSpawnMonster = false;

	ClearLink_Monster();

	m_iCurrent_Wave = 0;
	m_iLimitWave = 0;

	m_IsWave = false;
	m_IsWave_End = false;

	
}

void CTrigger::Wave_Start()
{
	if (true == m_IsWave || m_iLimitWave < m_iCurrent_Wave || SPAWN_WAVE::WAVE_END <= m_iCurrent_Wave)
		return;

	for (auto& pMonster : m_pMonsters[m_iCurrent_Wave])
	{
		if (nullptr != pMonster)
		{
			if (pMonster->IsDisable())
			{
				pMonster->SetUp_Activate(m_SpawnPoints[m_iSpawnPointIndex]);
				Add_SpawnPoint();
			}
		}
	}

	m_IsWave = true;
}

void CTrigger::Monster_Spawn()
{
	// 리미트 웨이브 체크
	if (m_iLimitWave < m_iCurrent_Wave || SPAWN_WAVE::WAVE_END <= m_iCurrent_Wave || m_pMonsters[m_iCurrent_Wave].empty())
		return;

	// 웨이브 발동.
	if (false == m_IsWave)
		Wave_Start();

	// 끝이 났나 판단하기 위해 true
	m_IsWave_End = true;

	for (auto& pMonster : m_pMonsters[m_iCurrent_Wave])
	{
		// 몬스터가 하나라도 활성화 ㅅ항태면 false 세팅.
		if (pMonster->IsActive())
			m_IsWave_End = false;
	}

	// 한 웨이브가 끝나면 다음 웨이브 진행 -> LimitWave 에 도달하면 스폰을 끝낸다.
	if (true == m_IsWave_End)
	{
		// 웨이브 전체가 끝 -> 초기화.
		if (m_iLimitWave <= m_iCurrent_Wave)
			Clear_MonsterSpawnControl();
		else
		{
			// 한웨이브 끝 -> 다음웨이브 진행.
			m_iCurrent_Wave++;
			m_IsWave = false;
		}
	}
}



HRESULT CTrigger::Link_WaveMonster(SPAWN_WAVE eWave, CCharacter * pCharacter)
{
	if (nullptr == pCharacter || TRIGGER_TYPE::TYPE_SPAWN != m_TriggerDesc.iTriggerType || SPAWN_WAVE::WAVE_END <= eWave)
	{
		MSG_BOX("Failed To Link Monster : CTrigger");
		return E_FAIL;
	}

	m_pMonsters[eWave].push_back(pCharacter);
	// 리미트 웨이브 갱신 -> 가장 높은 웨이브가 저장된다.
	if (m_iLimitWave <= (_uint)eWave)
		m_iLimitWave = eWave;

	return S_OK;
}

void CTrigger::ClearLink_Monster()
{
	for (_uint i = 0; i < SPAWN_WAVE::WAVE_END; ++i)
		m_pMonsters[i].clear();
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
	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;
	m_pMainTransform->Set_Scale(_float3(1.0f, 1.0f, 1.0f));

	return S_OK;
}


HRESULT CTrigger::SetUp_ShaderResources()
{
	/*
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;
		*/

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

	if (TRIGGER_TYPE::TYPE_SPAWN == m_TriggerDesc.iTriggerType)
		ClearLink_Monster();
}