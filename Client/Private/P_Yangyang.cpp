#include "stdafx.h"
#include "..\Public\P_Yangyang.h"

#include "ItemDB.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "Parts.h"
#include "Effect.h"

//State
#include "PlayerState.h"

//StateKeys
#include "EffectKey.h"
#include "PartsKey.h"
#include "PriorityKey.h"
#include "OBBKey.h"
#include "MissileKey.h"
#include "DissolveKey.h"
#include "SlowKey.h"
#include "SoundKey.h"

#include "Missile_Constant.h"
#include "Missile_RotAround.h"

#include "Chest.h"
#include "Inventory.h"
#include "UI_MainScreen.h"
//CamMovement
#include "CameraMovement.h"

#include "Rader.h"

const _int CP_Yangyang::iStateLimit = CP_Yangyang::IS_END;

const char CP_Yangyang::szIndividualStateTag[CP_Yangyang::IS_END - CP_Yangyang::IS_START][MAX_PATH] =
{
	"IS_ATTACK_01",
	"IS_ATTACK_02",
	"IS_ATTACK_03",
	"IS_ATTACK_04",
	"IS_ATTACK_05",
	"IS_ATTACK_09",
	"IS_ATTACK_PO_2",
	"IS_ATTACK_PO_3",
	"IS_AIRATTACK_START",
	"IS_AIRATTACK_LOOP",
	"IS_AIRATTACK_END",
	"IS_SKILL_01",
	"IS_SKILL_02",
	"IS_SKILL_03",
	"IS_BURST"
};

CCharacter::MULTISTATE CP_Yangyang::m_tStates[CP_Yangyang::IS_END];

CP_Yangyang::CP_Yangyang(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CP_Yangyang::CP_Yangyang(const CP_Yangyang & rhs)
	: CCharacter(rhs)
{
}

HRESULT CP_Yangyang::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CP_Yangyang::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//if (FAILED(Init_States()))
	//	return E_FAIL;

	if (FAILED(Init_Parts()))
		return E_FAIL;

	if (FAILED(Init_EffectBones()))
		return E_FAIL;

	// 공격 정보 세팅
	Init_AttackInfos();
	m_iCurAttackID = ATK_NONE;

	Init_Missiles();

	// 애니메이션 연결
	Init_AnimSystem();

	// 루트모션용 본찾기
	m_pAnimSetCom[ANIMSET_BASE]->Set_RootBone(TEXT("Root"));

	// 벽타기용 모델 중심뼈 잡아주기
	m_pClimbBones[CBONE_HEAD] = m_pModelCom->Get_BonePtr(TEXT("Bip001Head"));
	m_pClimbBones[CBONE_SPINE] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine"));
	m_pClimbBones[CBONE_ROOT] = m_pModelCom->Get_BonePtr(TEXT("Root"));

	m_pModelCom->Set_TopBones(TEXT("Bip001Head"), TEXT("Bip001LHand"), TEXT("Bip001RHand"));

	// 초기위치 설정
	//m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(105.f, 30.f, 92.f, 1.f));
	//m_pMainTransform->Set_Scale(_float3(10.f, 10.f, 10.f));
	//m_pNaviCom->Set_CurrentIndex(90);

	CGameMode* pGameMode = CGameMode::GetInstance();

	switch (pGameMode->GetCurrentLevel())
	{
	case LEVEL_TEST:
	case LEVEL_GAMEPLAY:
		m_pNaviCom->Set_CurrentIndex(0);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(39.125f, 2.290f, 30.776f, 1.f));
		break;
	case LEVEL_CITY:
		m_pNaviCom->Set_CurrentIndex(0);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(73.f, 15.50f, 118.f, 1.f));
		m_pMainTransform->Set_LookDir(XMVectorSet(1.f, 0.f, 0.f, 0.f));
		break;

		// 추가됨
	case LEVEL_FOREST:
		m_pNaviCom->Set_CurrentIndex(0);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(271.8380f, 5.9810f, 201.0880f, 1.f));
		break;

		// 추가됨
	case LEVEL_CROWN:
		m_pNaviCom->Set_CurrentIndex(0);
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(33.0f, 17.6140f, 30.0f, 1.f));
		break;
	}

	//m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// StateController 초기화
	m_Scon.iCurState = 0;
	m_Scon.iNextState = 0;
	m_Scon.ePositionState = PS_GROUND;
	m_Scon.iPrevCellState = 0;
	m_tCurState = m_tStates[0];
	SetUp_State();
	SetUp_Animations(false);

	// CharInfo 초기화 >> 플레이어는 CharInfo 안씀
	/*lstrcpy(m_tMonsterInfo.szName, TEXT("Rover"));
	m_tMonsterInfo.eElement = ELMT_SPECTRA;
	m_tMonsterInfo.iLevel = 10;
	m_tMonsterInfo.iExp = 0;
	m_tMonsterInfo.fMaxHP = 100.f;
	m_tMonsterInfo.fCurHP = m_tMonsterInfo.fMaxHP;
	m_tMonsterInfo.fMaxSP = 100.f;
	m_tMonsterInfo.fCurSP = 0.f;
	m_tMonsterInfo.fMaxTP = 100.f;
	m_tMonsterInfo.fCurTP = 0.f;
	m_tMonsterInfo.fAttack = 50.f;
	m_tMonsterInfo.fDefense = 50.f;
	m_tMonsterInfo.fCriticalRate = 0.1f; */

	// 충돌 타입 처리
	m_eCollisionType = CT_PLAYER;
	m_fPushWeight = 50.f;

	// 초기 상태 꺼놓음
	m_pAttackCollider->SetActive(false);

	_uint iLevel = pGameMode->GetCurrentLevel();

	if (iLevel == LEVEL_GAMEPLAY)
		m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(55.f));

	CGameInstance* pGame = CGameInstance::GetInstance();

	m_pEchoSystem = static_cast<CEchoSystem*>(pGame->Find_GameObject(LEVEL_STATIC, L"Echo"));

	m_pPlayerStateClass = static_cast<CPlayerState*>(pGame->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pPlayerStateClass->Register_Character(CPlayerState::CHARACTER_YANGYANG, this, &m_bOnControl);
	m_pCharacterState = m_pPlayerStateClass->Get_CharState_byChar(CPlayerState::CHARACTER_YANGYANG);

	m_pCharacterState->fMaxCooltime[CPlayerState::COOL_SKILL] = 7.f;
	m_pCharacterState->fMaxCooltime[CPlayerState::COOL_BURST] = 9.f;
	m_pCharacterState->fMaxCooltime[CPlayerState::COOL_ECHO] = 5.f;
	
	m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL] = 3.f;
	m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST] = 100.f;

	m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] = 0.f;
	m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] = 0.f;




	return S_OK;
}

void CP_Yangyang::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

#ifdef _DEBUG
	m_pRendererCom->DebugBundleRender_Control(true);
#endif

	m_pInven = static_cast<CInventory*>(pGame->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGame->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pCamMovement->UseCamera(CCameraMovement::CAM_MAINPLAYER);
	m_pCamMovement->SetupBone(CCameraMovement::CAM_YANGYANG, m_pModelCom->Get_BonePtr(L"WeaponProp02"));
	m_pUIMain = static_cast<CUI_MainScreen*>(pGame->Find_GameObject(LEVEL_ANYWHERE, L"UI_MainScreen"));

	if (false == m_bOnControl)
		SetState(DISABLE);

	m_MissilePools[MISS_SKILL_01_1]->Set_SoundTag(this, L"Play_role_yangyang_fx_skill01_longjuan.wem.wav");
	m_MissilePools[MISS_SKILL_01_3]->Set_SoundTag(this, L"Play_role_yangyang_fx_skill01_xiaoshi.wem.wav");
	m_MissilePools[MISS_SKILL_01_3]->Set_CamMovement(m_pCamMovement, 3);

	m_MissilePools[MISS_BURST_1]->Set_SoundTag(this, L"Play_role_yangyang_fx_burst_longjuan.wem.wav");
	m_MissilePools[MISS_BURST_2]->Set_SoundTag(this, L"Play_role_yangyang_fx_burst_end.wem.wav");

	m_MissilePools[MISS_AIRATTACK_2_3]->Set_SoundTag(this, L"Play_role_yangyang_fx_skill02_xuli.wem.wav");

}

void CP_Yangyang::PreTick(_double TimeDelta)
{
	// 타겟이 잡혀있다면 사용할 수 있는지, 거리가 너무 멀어지지는 않았는지 체크 및 타겟 해제
	if (nullptr != m_pFixedTarget)
	{
		if (false == m_pFixedTarget->IsActive() ||
			true == m_pFixedTarget->Get_Dying() ||
			25.f <  XMVectorGetX(XMVector3Length(m_pFixedTarget->Get_Position() - Get_Position())))
		{
			if (nullptr != m_pNearst)
			{
				m_pFixedTarget = m_pNearst;
				m_pPlayerStateClass->Set_LockOn(true, m_pFixedTarget);
			}
			else
			{
				m_pFixedTarget = nullptr;
				m_pPlayerStateClass->Set_LockOn(false, nullptr);
			}
		}
	}
}

void CP_Yangyang::Tick(_double TimeDelta)
{
	Check_TimeDelay(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->ShadowUpdate(80.f, m_pMainTransform->Get_State(CTransform::STATE_POSITION));

	updateAttackDesc();

	__super::Tick(TimeDelta * m_TimeDelay);
	m_pRader->Tick(TimeDelta);

	if (pGameInstance->InputKey(DIK_NUMPAD1) == KEY_STATE::TAP)
	{
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(39.125f, 0.f, 30.776f, 1.f));
		m_pNaviCom->Set_CurrentIndex(0);
		m_Scon.ePositionState = PS_GROUND;
	}

	/*if (pGameInstance->InputKey(DIK_NUMPAD0) == KEY_STATE::HOLD)
	{
	m_TimeDelay = 0.1;
	}
	else
	{
	m_TimeDelay = 1.0;
	}*/

	if (pGameInstance->InputMouse(DIMK_WHEEL) == KEY_STATE::AWAY)
	{
		if (m_ReleaseTargetTimeAcc < 0.3)
		{
			if (nullptr != m_pNearst)
				m_pFixedTarget = m_pNearst;

			if (nullptr != m_pFixedTarget)
			{
				m_pPlayerStateClass->Set_LockOn(true, m_pFixedTarget);
			}
		}
		else if (nullptr != m_pFixedTarget && 0.3 < m_ReleaseTargetTimeAcc)
		{
			m_pFixedTarget = nullptr;
			m_pPlayerStateClass->Set_LockOn(false, nullptr);
		}
	}

	if (pGameInstance->InputMouse(DIMK_WHEEL) == KEY_STATE::HOLD)
	{
		m_ReleaseTargetTimeAcc += TimeDelta;
	}
	else
		m_ReleaseTargetTimeAcc = 0.0;


	Apply_CoolTime(TimeDelta); 

	if (true == m_bOnControl)
		Key_Input(TimeDelta * m_TimeDelay); // 입력 > 다음 상태 확인 > 갱신될 경우 Setup_state, setup_animation

	Tick_State(TimeDelta * m_TimeDelay); // PlayAnimation, 애니메이션에 따른 이동, 애니메이션 종료 시 처리

	if (true == m_bOnControl)
		On_Cell(); // 자발적인 움직임 후처리 >> 주로 내비 메쉬

			   // Parts 처리
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
			m_Parts[i]->Tick(TimeDelta * m_TimeDelay);
	}

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pAttackCollider, COLL_PLAYERATTACK);
	m_pAttackCollider->Update(XMLoadFloat4x4(m_Parts[PARTS_WEAPON_MAIN]->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pHitCollider, COLL_MONSTERATTACK);
	m_pHitCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_MOVE);
	m_pMoveCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_NPC);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
}

void CP_Yangyang::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta * m_TimeDelay);
	m_pRader->LateTick(TimeDelta);

	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//림라이트 임시테스트
	if (pGameInstance->InputKey(DIK_H) == KEY_STATE::TAP)
		m_bRimToggle = !m_bRimToggle;

	if (m_bRimToggle)
	{
		m_fRimAlpha += (_float)TimeDelta;
		if (m_fRimAlpha >= 1.f)
			m_fRimAlpha = 0.f;
	}

	// BurstRim
	if (m_Scon.iCurState == IS_BURST)
		m_fBurstRim = 0.3f;
	else
	{
		m_fBurstRim -= (_float)TimeDelta * 0.5f;
		if (m_fBurstRim <= 0.f)
			m_fBurstRim = 0.f;
	}



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	if (true == m_bRender)
	{
		for (_uint i = 0; i < PARTS_END; ++i)
		{
			if (nullptr != m_Parts[i])
			{
				m_Parts[i]->LateTick(TimeDelta * m_TimeDelay);
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, m_Parts[i]);
				m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, m_Parts[i]);
			}
		}
	}

	static _bool bCamLock = false;
	if (pGameInstance->InputKey(DIK_SCROLL) == KEY_STATE::TAP)
	{
		bCamLock = !bCamLock;

		if (bCamLock)
			m_pCamMovement->UseCamera(CCameraMovement::CAM_MAINPLAYER);
		else
			m_pCamMovement->UseCamera(CCameraMovement::CAM_DYNAMIC);
	}

	//Effect Bones 처리
	Update_EffectBones();

	// 돌진기 스킬 예외처리 // 거리비교해서
	//if (nullptr == m_pFixedTarget)
	//{
	//	if (m_pNearst && m_fNearstDist <= 3.f)
	//	{
	//		static _bool bOverlapedCheck = false;

	//		if (0 == m_Scon.iCurState)
	//		{
	//			static _vector vTargetPos;

	//			CTransform* pTargetTransform = m_pNearst->GetTransform();
	//			_float fExtents = m_pNearst->GetMoveCollider()->GetExtents().x;

	//			if (!bOverlapedCheck)
	//			{
	//				bOverlapedCheck = true;
	//				vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	//			}

	//			_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	//			_vector vOffSetPos = vTargetPos + XMVector3Normalize(vCurPos - vTargetPos) * fExtents;

	//			vCurPos = XMVectorLerp(vCurPos, vOffSetPos, (_float)TimeDelta * 10.f);

	//			m_tCurState.bRootMotion = false;
	//			m_Scon.vMovement = _float3(0.f, 0.f, 0.f);

	//			m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);
	//		}
	//		else
	//		{
	//			bOverlapedCheck = false;
	//		}
	//	}
	//}
	//else
	//{
	//	if (m_pFixedTarget && m_fTargetDist <= 3.f)
	//	{
	//		static _bool bOverlapedCheck = false;

	//		if (0 == m_Scon.iCurState)
	//		{
	//			static _vector vTargetPos;

	//			CTransform* pTargetTransform = m_pFixedTarget->GetTransform();
	//			_float fExtents = m_pFixedTarget->GetMoveCollider()->GetExtents().x;

	//			if (!bOverlapedCheck)
	//			{
	//				bOverlapedCheck = true;
	//				vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	//			}

	//			_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	//			_vector vOffSetPos = vTargetPos + XMVector3Normalize(vCurPos - vTargetPos) * fExtents;

	//			vCurPos = XMVectorLerp(vCurPos, vOffSetPos, (_float)TimeDelta * 10.f);

	//			m_tCurState.bRootMotion = false;
	//			m_Scon.vMovement = _float3(0.f, 0.f, 0.f);

	//			m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);
	//		}
	//		else
	//		{
	//			bOverlapedCheck = false;
	//		}
	//	}
	//}


	// 다음프레임을 위해 초기화
	m_pNearst = nullptr;
	m_fNearstDist = 20.f;
	m_bHit = false;
}

HRESULT CP_Yangyang::Render()
{
	if (false == m_bRender)
		return S_OK;

	CGameInstance* pGame = CGameInstance::GetInstance();

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	int iPass;
	if (10.f <= ComputeCameraLength())
	{
		iPass = 3;
		for (_uint i = 0; i < PARTS_END; ++i)
		{
			if (nullptr != m_Parts[i])
				m_Parts[i]->Set_Outline(false);
		}
	}
	else
	{
		iPass = 4;
		for (_uint i = 0; i < PARTS_END; ++i)
		{
			if (nullptr != m_Parts[i])
				m_Parts[i]->Set_Outline(true);
		}
	}


	for (_uint i = 0; i < 7; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_VertexTexture(m_pShaderCom, "g_VertexTexture", i)))
			return E_FAIL;

		if (i == 6)
		{
			if (FAILED(m_pEyeBurstTexture->Setup_ShaderResource(m_pShaderCom, "g_EyeBurstTexture")))
				return E_FAIL;
			if (FAILED(m_pEyeMaskTexture->Setup_ShaderResource(m_pShaderCom, "g_EyeMaskTexture")))
				return E_FAIL;

			// Eye
			if (m_bDissolve)
			{
				// Dissolve 변수
				if (FAILED(pGame->SetupSRV(STATIC_IMAGE::MASK_DESSOLVE, m_pShaderCom, "g_DissolveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->SetRawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->SetRawValue("g_vDessolveColor", &m_vDissolveColor, sizeof(_float3))))
					return E_FAIL;

				m_pShaderCom->Begin(13);
			}
			else
			{
				if (IS_BURST == m_Scon.iCurState)
				{
					_float3 vColor = { 40.f / 255.f, 120.f / 255.f, 255.f / 255.f };
					if (FAILED(m_pShaderCom->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
						return E_FAIL;

					m_pShaderCom->Begin(18);		//Burst
				}
				else
					m_pShaderCom->Begin(16);
			}
		}
		else
		{
			if (m_bDissolve)
				m_pShaderCom->Begin(13);
			else
				m_pShaderCom->Begin(iPass);
		}

		m_pModelCom->Render(i);

		//Rim Light
		if (m_bRimToggle && m_Scon.iCurState != IS_BURST && i != 6)
		{
			/* H키 누르면 활성화/비활성화 토글
			* g_RimPower : 일반적으로 5정도 주고 값이 높을수록 Rim의 라인 굵기가 얇아짐
			* g_RimColor : 림라이트의 색상값 기본색(흰), 초록(고급), 파랑(레어), 보라(고급), 노랑(신화) 등급 색상은 매크로로 지정 되있음 직접 세팅해도됨
			* g_fTimeAcc : VTXMODELANIM 쉐이더에서는 림라이트에서만 TimeAcc 적용하고있고 림라이트의 알파값을 의미함
			*/
			_float vRimPower = 5.f;
			_float3 vColor = UNIQUE_COLOR;
			_float4 vRimColor = _float4(vColor.x, vColor.y, vColor.z, 1.f);

			if (FAILED(m_pShaderCom->SetRawValue("g_RimPower", &vRimPower, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->SetRawValue("g_RimColor", &vRimColor, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->SetRawValue("g_fTimeAcc", &m_fRimAlpha, sizeof(_float))))
				return E_FAIL;

			m_pShaderCom->Begin(8);
			m_pModelCom->Render(i);
		}

		// Burst Rim
		if (i != 6 && m_fBurstRim > 0.f)
		{
			_float vRimPower = 10.f;
			_float3 vColor = { 40.f / 255.f, 120.f / 255.f, 255.f / 255.f };
			_float4 vRimColor = _float4(vColor.x, vColor.y, vColor.z, 1.f);
			if (FAILED(m_pShaderCom->SetRawValue("g_RimPower", &vRimPower, sizeof(_float))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->SetRawValue("g_RimColor", &vRimColor, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(m_pShaderCom->SetRawValue("g_fTimeAcc", &m_fBurstRim, sizeof(_float))))
				return E_FAIL;

			m_pShaderCom->Begin(8);
			m_pModelCom->Render(i);
		}
	}

#ifdef _DEBUG
	m_pNaviCom->Render();
#endif

	return S_OK;
}

HRESULT CP_Yangyang::RenderShadow()
{
	if (false == m_bRender)
		return S_OK;

	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(Setup_ShadowShaderResource()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_VertexTexture(m_pShaderCom, "g_VertexTexture", i)))
			return E_FAIL;

		m_pShaderCom->Begin(5);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CP_Yangyang::RenderGUI()
{
	ImGui::Begin("Position Helper");

	// 지형 위치 체크용
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
	string strPos = to_string(vPos.x) + " " + to_string(vPos.y) + " " + to_string(vPos.z);
	ImGui::Text(strPos.c_str());

	// 카메라 위치 체크용
	CGameInstance* pGame = CGameInstance::GetInstance();
	_float4x4 vCamMatrix = pGame->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW);

	_float3 vCamPosFromTransform;

	_vector vTransPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pGame->Get_CamPosition()) - vTransPos;

	XMStoreFloat3(&vCamPosFromTransform, vCamPos);
	ImGui::DragFloat3("Cam from transform", (_float*)&vCamPosFromTransform, 0.1f, 50.f);
	ImGui::DragFloat3("Cam Right", (_float*)&vCamMatrix._11, 0.1f, 50.f);
	ImGui::DragFloat3("Cam Up", (_float*)&vCamMatrix._21, 0.1f, 50.f);
	ImGui::DragFloat3("Cam Look", (_float*)&vCamMatrix._31, 0.1f, 50.f);

	ImGui::End();
}

void CP_Yangyang::SetUp_Animations(_bool bContinue)
{
	m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_BASE], m_tCurState.bLerp, bContinue);
	m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_RIBBON], m_tCurState.bLerp, bContinue);
}

HRESULT CP_Yangyang::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(540.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
	_uint nCurrentLevel = pGM->GetCurrentLevel();

	// For.Com_Shader_ModelAnim
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_YANGYANG_MODEL,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AnimSet_Base */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_YANGYANG_ANIMSET_BASE,
		TEXT("Com_AnimSet_Base"), (CComponent**)&m_pAnimSetCom[ANIMSET_BASE])))
		return E_FAIL;

	/* For.Com_AnimSet_Ribbon */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_YANGYANG_ANIMSET_RIBBON,
		TEXT("Com_AnimSet_Ribbon"), (CComponent**)&m_pAnimSetCom[ANIMSET_RIBBON])))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATION_DESC));
	NavigationDesc.iCurrentIndex = 0;

	/* Navigation */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, COMPONENT::NAVIGATION,
		TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NavigationDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::EYE_BURST,
		TEXT("Com_Eye_Burst_Texture"), (CComponent**)&m_pEyeBurstTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::EYE_MASK,
		TEXT("Com_Eye_Mask_Texture"), (CComponent**)&m_pEyeMaskTexture)))
		return E_FAIL;

	m_pRader = CRader::Create(m_pDevice, m_pContext);
	if (nullptr == m_pRader)
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.5f, 0.f };
	CollDesc.vExtents = { 0.5f, 0.5f, 0.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.owner = this;
	CollDesc.vCenter = { -0.4f, 0.f, 0.f };
	CollDesc.vExtents = { 0.75f, 0.15f, 0.15f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_AttackCollider"), (CComponent**)&m_pAttackCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 1.f, 0.f };
	CollDesc.vExtents = { 0.8f, 0.8f, 0.8f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_HitCollider"), (CComponent**)&m_pHitCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.4f, 0.f, 0.4f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_MoveCollider"), (CComponent**)&m_pMoveCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CP_Yangyang::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(MULTISTATE) * iStateLimit);

	// 로드하는 코드
	for (_int i = 0; i < iStateLimit; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, TEXT("../../Data/CharState/P_YangYang/YangYang_%d.state"), i);
		HANDLE hFile = CreateFile(szBuffer, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			continue;


		DWORD dwByte = 0;

		ReadFile(hFile, &m_tStates[i], sizeof(CCharacter::MULTISTATE) - sizeof(CStateKey**), &dwByte, nullptr);

		if (0 != m_tStates[i].iKeyCount)
		{
			m_tStates[i].ppStateKeys = new CStateKey*[m_tStates[i].iKeyCount];
			ZeroMemory(m_tStates[i].ppStateKeys, sizeof(CStateKey*) * m_tStates[i].iKeyCount);

			for (_uint j = 0; j < m_tStates[i].iKeyCount; ++j)
			{
				CStateKey::BaseData tBaseData;
				ReadFile(hFile, &tBaseData, sizeof(CStateKey::BaseData), &dwByte, nullptr);

				switch (tBaseData.iType)
				{
				case CStateKey::TYPE_EFFECT:
					m_tStates[i].ppStateKeys[j] = CEffectKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PARTS:
					m_tStates[i].ppStateKeys[j] = CPartsKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PRIORITY:
					m_tStates[i].ppStateKeys[j] = CPriorityKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_DISSOLVE:
					m_tStates[i].ppStateKeys[j] = CDissolveKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_OBB:
					m_tStates[i].ppStateKeys[j] = COBBKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_MISSILE:
					m_tStates[i].ppStateKeys[j] = CMissileKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_SLOW:
					m_tStates[i].ppStateKeys[j] = CSlowKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_SOUND:
					m_tStates[i].ppStateKeys[j] = CSoundKey::Create(pDevice, pContext, &tBaseData);
					break;
				default:
					break;
				}
			}
		}
		else
			m_tStates[i].ppStateKeys = nullptr;

		CloseHandle(hFile);
	}


	return S_OK;
}

void CP_Yangyang::Release_States()
{
	for (_int i = 0; i < iStateLimit; ++i)
	{
		if (0 == m_tStates[i].iKeyCount)
			continue;
		for (_uint j = 0; j < m_tStates[i].iKeyCount; ++j)
		{
			if (nullptr != m_tStates[i].ppStateKeys[j])
				Safe_Release(m_tStates[i].ppStateKeys[j]);
		}
		Safe_Delete_Array(m_tStates[i].ppStateKeys);
	}
}

void CP_Yangyang::Check_Nearst(CCharacter * pChar, _float fDist)
{

	if (fDist < m_fNearstDist)
	{
		if (pChar != m_pFixedTarget)
		{
			m_pNearst = pChar;
			m_fNearstDist = fDist;
		}
		else
		{
			m_pFixedTarget = pChar;
			m_fTargetDist = fDist;
		}

	}
}

void CP_Yangyang::Check_TimeDelay(_double TimeDelta)
{
	if (m_DelayDuration > 0.0)
	{
		m_DelayDuration -= TimeDelta;
		if (m_DelayDuration < 0.0)
		{
			m_DelayDuration = 0;
			m_TimeDelay = 1.0;
		}
	}
}

void CP_Yangyang::Appear(CTransform * pTransform, CCharacter * pTarget, _uint iNaviCellID, _float fDissolveSpeed)
{
	CGameInstance* pGame = CGameInstance::GetInstance();
	pGame->StopSound(SOUND_PLAYER_VOICE);
	pGame->PlaySoundEx(L"Playback_vo_ba_yangyang_hx_trans.wem.wav", SOUND_PLAYER_VOICE, 0.5f);
	Shot_SoundKey(L"DA_Au_Role_Common_Char_Change.wem.wav", 0, 0.5);


	SetState(ACTIVE);


	Shot_SoundKey(L"", SOUND_PLAYER_VOICE, 0.5);
	

	m_pFixedTarget = pTarget;
	m_pMainTransform->Set_WorldMatrix(pTransform->Get_WorldMatrix());
	m_pNaviCom->Set_CurrentIndex(iNaviCellID);

	m_Scon.iNextState = SS_STAND1;
	SetUp_State();
	SetUp_Animations(false);

	m_bInputLock = false;
	m_bOnControl = true;
	m_pMoveCollider->SetActive(true);
	m_pHitCollider->SetActive(true);
	m_bDisableAfterDissolve = false;
	Shot_DissolveKey(true, fDissolveSpeed);
	for (auto& pParts : m_Parts)
		pParts->Start_Dissolve(true, fDissolveSpeed, true);
}

void CP_Yangyang::Disappear(CTransform ** ppTransform, CCharacter ** ppTarget, _uint* pNaviCellID)
{
	if (nullptr != ppTarget)
		*ppTarget = m_pFixedTarget;
	if (nullptr != ppTransform)
		*ppTransform = m_pMainTransform;
	if (nullptr != pNaviCellID)
		*pNaviCellID = m_pNaviCom->Get_CurrentIndex();

	m_pFixedTarget = nullptr;

	m_bOnControl = false;
	m_pMoveCollider->SetActive(false);
	m_pHitCollider->SetActive(false);
	m_bDisableAfterDissolve = true;
	Shot_DissolveKey(false, 144.f);
	for (auto& pParts : m_Parts)
		pParts->Start_Dissolve(false, 144.f, true);
}

void CP_Yangyang::Appear_QTE(CTransform * pTransform, CCharacter * pTarget, _uint iNaviCellID)
{
	CGameInstance* pGame = CGameInstance::GetInstance();
	pGame->StopSound(SOUND_PLAYER_VOICE);
	pGame->PlaySoundEx(L"Playback_vo_yangyang_dash.wem.wav", SOUND_PLAYER_VOICE, 0.5f);
	Shot_SoundKey(L"DA_Au_Role_Common_Char_Change.wem.wav", 0, 0.5);

	SetState(ACTIVE);

	m_pFixedTarget = pTarget;

	m_pMainTransform->Set_WorldMatrix(pTransform->Get_WorldMatrix());
	CTransform* pTargetTransform = pTarget->GetTransform();
	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_POSITION) - vTargetPos);
	_vector vFinalPos;

	// 1번 슬롯일 경우 왼쪽
	if (CPlayerState::SLOT_SUB1 == m_pPlayerStateClass->Get_Slot(CPlayerState::CHARACTER_YANGYANG))
		vFinalPos = vTargetPos + 2.f * XMVector3TransformNormal(vDir, XMMatrixRotationY(XMConvertToRadians(120.f)));
	// 2번 슬롯일 경우 오른쪽
	else
		vFinalPos = vTargetPos + 2.f * XMVector3TransformNormal(vDir, XMMatrixRotationY(XMConvertToRadians(-120.f)));

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vFinalPos, XMVectorGetY(m_pMainTransform->Get_State(CTransform::STATE_POSITION)) + 1.5f));
	m_pMainTransform->Set_LookDir(XMVectorSetY(vTargetPos - vFinalPos, 0.f));
	m_pNaviCom->Set_CurrentIndex(iNaviCellID);

	m_Scon.iNextState = IS_SKILL_QTE;
	SetUp_State();
	SetUp_Animations(false);

	m_bInputLock = false;
	m_bOnControl = true;
	m_pMoveCollider->SetActive(true);
	m_pHitCollider->SetActive(true);
	m_bDisableAfterDissolve = false;
	Shot_DissolveKey(true, 5.f);
	for (auto& pParts : m_Parts)
		pParts->Start_Dissolve(true, 5.f, true);
}

void CP_Yangyang::Disappear_QTE(CTransform ** ppTransform, CCharacter ** ppTarget, _uint* pNaviCellID)
{
	*ppTarget = m_pFixedTarget;
	*ppTransform = m_pMainTransform;
	*pNaviCellID = m_pNaviCom->Get_CurrentIndex();
	m_pFixedTarget = nullptr;

	m_Scon.iNextState = SS_SPRINT_IMPULSE_F;
	SetUp_State();
	SetUp_Animations(false);

	m_bOnControl = false;
	m_pMoveCollider->SetActive(false);
	m_pHitCollider->SetActive(false);
	m_bDisableAfterDissolve = true;
	Shot_DissolveKey(false, 10.f);
	for (auto& pParts : m_Parts)
		pParts->Start_Dissolve(false, 10.f, true);
}

void CP_Yangyang::Shot_PartsKey(_uint iParts/*int0*/, _uint iState/*int1*/, _uint iDissolve/*int2*/, _double Duration/*float*/)
{
	// Weapon Main / Sub
	if (0 == iParts)
	{
		// 등으로
		if (0 == iState)
		{
			Set_WeaponUse(false);
		}
		// 손으로
		else if (1 == iState)
		{
			Set_WeaponUse(true);
		}

		// Dissolve In
		if (1 == iDissolve)
		{

		}
		// Dissolve Out
		else if (2 == iDissolve)
		{

		}
	}
	//// Hulu
	//else if (1 == iParts)
	//{
	//	// 안보이게 한다.
	//	if (0 == iState)
	//	{

	//	}
	//	//보이게 한다.
	//	else if (1 == iState)
	//	{
	//		
	//	}
	//}
}

void CP_Yangyang::Shot_PriorityKey(_uint iLeavePriority)
{
	m_tCurState.iLeavePriority = iLeavePriority;

	if (IS_SKILL_02 == m_Scon.iCurState)
		m_pMoveCollider->SetActive(true);
}

void CP_Yangyang::Shot_EffectKey(_tchar * szEffectTag/* szTag1*/, _uint EffectBoneID /* iInt0 */, _uint iEffectTypeID, _bool bTracking/*iInt1*/)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag, Engine::EFFECT_ID(iEffectTypeID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CP_Yangyang::Shot_OBBKey(_bool bOBB, _uint iAttackInfoID)
{
	m_bAttack = true;
	m_pAttackCollider->SetActive(bOBB);
	m_iCurAttackID = iAttackInfoID;

	if (IS_ATTACK_05 == m_Scon.iCurState)
	{
		m_pAttackCollider->SetCenter_(_float3(-0.6f, 0.f, 0.f));
		m_pAttackCollider->SetExtents_(_float3(0.75f, 0.15f, 0.15f));
	}
	else if(IS_ATTACK_06 == m_Scon.iCurState)
	{
		m_pAttackCollider->SetCenter_(_float3(-1.2f, 0.f, 0.f));
		m_pAttackCollider->SetExtents_(_float3(1.5f, 0.15f, 0.15f));
	}

	else if (IS_SKILL_02 == m_Scon.iCurState)
	{
		m_pAttackCollider->SetCenter_(_float3(0.f, 0.f, 0.f));
		m_pAttackCollider->SetExtents_(_float3(1.5f, 0.5f, 0.5f));
	}
	else
	{
		m_pAttackCollider->SetCenter_(_float3(-0.4f, 0.f, 0.f));
		m_pAttackCollider->SetExtents_(_float3(0.5f, 0.15f, 0.15f));
	}
	
}

void CP_Yangyang::Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID)
{
	if (MISS_END <= iMissilePoolID || EBONE_END <= iEffectBoneID)
		return;

	m_bAttack = true;

	if (MISS_AIRATTACK_2_1 == iMissilePoolID || 
		MISS_SKILL_QTE == iMissilePoolID)
		m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_3);
	else if (MISS_SKILL_01_1 == iMissilePoolID)
		m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_4);
	else if (MISS_AIRATTACK_1 == iMissilePoolID ||
		MISS_AIRATTACK_2_3 == iMissilePoolID ||
		MISS_AIRATTACK_2_2 == iMissilePoolID)
		m_pCamMovement->StartWave((_uint)CCameraMovement::SHAKE_5);



	
	if (iMissilePoolID == MISS_BURST_1)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		CRenderSetting::RGB_SPLIT_DESC Split;
		Split.m_fDistortion = 4.0f;
		Split.m_fStrength = 0.5f;
		Split.m_fSeparation = 0.5f;
		pGameInstance->SetSplitDesc(Split);
		pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DIR::SPLIT_DEFAULT, 1.f);
	}

	_vector vInitPos;
	if (0 != iEffectBoneID)
	{
		vInitPos = XMVector3TransformCoord(
			XMVector3TransformCoord(m_EffectBones[iEffectBoneID]->Get_CombinedPosition(), XMMatrixRotationY(XMConvertToRadians(180.f))),
			XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));
	}
	else
		vInitPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);


	_matrix matRot = XMMatrixRotationAxis(m_pMainTransform->Get_State(CTransform::STATE_RIGHT), m_MissileRotAngles[iMissilePoolID].x)
		* XMMatrixRotationAxis(m_pMainTransform->Get_State(CTransform::STATE_UP), m_MissileRotAngles[iMissilePoolID].y)
		* XMMatrixRotationAxis(m_pMainTransform->Get_State(CTransform::STATE_LOOK), m_MissileRotAngles[iMissilePoolID].z);

	m_MissilePools[iMissilePoolID]->Shot(vInitPos, m_pMainTransform->Get_State(CTransform::STATE_LOOK), matRot);
}

void CP_Yangyang::updateAttackDesc()
{
	const CPlayerState::ATTACK_DESC* AttackDesc = m_pPlayerStateClass->GetAttackDesc(CPlayerState::CHARACTER_YANGYANG);

	static float fOriginDamageFactor[4];
	static float fOriginBurstFactor;
	static _bool bSetup = false;

	if (!bSetup)
	{
		bSetup = true;
		fOriginDamageFactor[0] = m_AttackInfos[ATK_ATTACK_01].fDamageFactor;
		fOriginDamageFactor[1] = m_AttackInfos[ATK_ATTACK_02].fDamageFactor;
		fOriginDamageFactor[2] = m_AttackInfos[ATK_ATTACK_03_2].fDamageFactor;
		fOriginDamageFactor[3] = m_AttackInfos[ATK_ATTACK_04_3].fDamageFactor;
		fOriginBurstFactor = m_AttackInfos[ATK_BURST_2].fDamageFactor;
	}

	m_AttackInfos[ATK_ATTACK_01].fDamageFactor = fOriginDamageFactor[0] * AttackDesc->fDamageFactor[0];
	m_AttackInfos[ATK_ATTACK_02].fDamageFactor = fOriginDamageFactor[1] * AttackDesc->fDamageFactor[1];
	m_AttackInfos[ATK_ATTACK_03_2].fDamageFactor = fOriginDamageFactor[2] * AttackDesc->fDamageFactor[2];
	m_AttackInfos[ATK_ATTACK_04_3].fDamageFactor = fOriginDamageFactor[3] * AttackDesc->fDamageFactor[3];
	m_AttackInfos[ATK_BURST_2].fDamageFactor = fOriginBurstFactor * AttackDesc->fBurstFactor;

	if (AttackDesc->bAirboneQTE)
		m_AttackInfos[ATK_SKILL_QTE].eHitIntensity = HIT_FLY;
	else
		m_AttackInfos[ATK_SKILL_QTE].eHitIntensity = HIT_BIG;
}

void CP_Yangyang::SetUp_State()
{
	// 키 리셋
	for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
	{
		if (nullptr != m_tCurState.ppStateKeys[i])
			m_tCurState.ppStateKeys[i]->Reset();
	}

	// 오입력 예외처리
	if (m_Scon.iNextState >= iStateLimit)
		m_Scon.iNextState = iStateLimit - 1;
	else if (m_Scon.iNextState <= 0)
		m_Scon.iNextState = 0;

	// 다음 애니메이션으로 갱신
	m_Scon.iCurState = m_Scon.iNextState;
	m_tCurState = m_tStates[m_Scon.iCurState];

	m_Scon.TrackPos = 0.0;
	m_Scon.bAnimFinished = false;

	// 애니메이션이 강제로 끊긴 경우 대비 애니메이션 갱신 시 OBB 콜라이더 무조건 끄기
	m_pAttackCollider->SetActive(false);

	// PositionState 처리
	if ((SS_JUMP_WALK == m_Scon.iCurState ||
		SS_JUMP_RUN == m_Scon.iCurState ||
		SS_FIXHOOK_END_UP == m_Scon.iCurState ||
		SS_CLIMB_MOVE == m_Scon.iCurState ||
		SS_BEHIT_FLY_START == m_Scon.iCurState ||
		SS_BEHIT_PUSH == m_Scon.iCurState ||
		IS_SKILL_QTE == m_Scon.iCurState ||
		IS_SKILL_03 == m_Scon.iCurState ||
		SS_FALL == m_Scon.iCurState) &&
		PS_AIR != m_Scon.ePositionState)
	{
		m_Scon.ePositionState = PS_AIR;
		m_Scon.bFalling = false;
		m_iAirJumpCount = 1;

		XMStoreFloat3(&m_vJumpPos, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
	}

	if (SS_LAND_LIGHT == m_Scon.iCurState ||
		SS_LAND_HEAVY == m_Scon.iCurState ||
		SS_LAND_ROLL == m_Scon.iCurState ||
		SS_CLIMB_ONTOP == m_Scon.iCurState ||
		SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState ||
		SS_BEHIT_FLY_FALL == m_Scon.iCurState ||
		IS_AIRATTACK_1_END == m_Scon.iCurState ||
		IS_AIRATTACK_2_END == m_Scon.iCurState ||
		IS_AIRATTACK_2_QTE_END == m_Scon.iCurState)
	{
		m_Scon.ePositionState = PS_GROUND;
	}

	if ((SS_CLIMB_START_UP == m_Scon.iCurState ||
		SS_CLIMB_START_DOWN == m_Scon.iCurState ||
		SS_CLIMB_STAND == m_Scon.iCurState ||
		SS_CLIMB_BOOST_U_START == m_Scon.iCurState) &&
		PS_CLIMB != m_Scon.ePositionState)
	{
		m_Scon.ePositionState = PS_CLIMB;
	}

	// Move 끄기
	if (IS_SKILL_02 == m_Scon.iCurState)
		m_pMoveCollider->SetActive(false);
	else
		m_pMoveCollider->SetActive(true);

	// Climb 종료 시 Look 방향 되돌리기
	if (SS_CLIMB_ONTOP == m_Scon.iCurState ||
		SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState ||
		SS_CLIMB_MOVE == m_Scon.iCurState ||
		SS_LAND_LIGHT == m_Scon.iCurState)
	{
		m_pMainTransform->Set_LookDir(XMVectorSetY(m_pMainTransform->Get_State(CTransform::STATE_LOOK), 0.f));
	}

	// 2단 점프 횟수 차감
	if (SS_JUMP_SECOND_B == m_Scon.iCurState ||
		SS_JUMP_SECOND_F == m_Scon.iCurState)
		--m_iAirJumpCount;

	// 무기 위치 잡기
	if (false == m_tCurState.bWeaponState)
		Set_WeaponUse(false);
	else
		Set_WeaponUse(true);

	// 쿨타임, 게이지 적용
		if (IS_SKILL_01 == m_Scon.iCurState)
			m_pCharacterState->fCurCooltime[CPlayerState::COOL_SKILL] = m_pCharacterState->fMaxCooltime[CPlayerState::COOL_SKILL];
		else if (IS_BURST == m_Scon.iCurState)
			m_pCharacterState->fCurCooltime[CPlayerState::COOL_BURST] = m_pCharacterState->fMaxCooltime[CPlayerState::COOL_BURST];
		else if (SS_SUMMON == m_Scon.iCurState)
			m_pCharacterState->fCurCooltime[CPlayerState::COOL_ECHO] = m_pCharacterState->fMaxCooltime[CPlayerState::COOL_ECHO];
		else if (SS_FIXHOOK_END_UP == m_Scon.iCurState)
			m_pPlayerStateClass->Set_ToolUsed();
	
	if (false == m_bRender)
		if (IS_AIRATTACK_2_END == m_Scon.iCurState ||
			IS_AIRATTACK_2_QTE_END == m_Scon.iCurState)
			m_bRender = true;


	// 게이지 차감
	if (IS_AIRATTACK_2_START == m_Scon.iCurState)
		m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] = 0.f;
	
	if (IS_BURST == m_Scon.iCurState)
	{
		m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] = 0.f;
		m_pHitCollider->SetActive(false);
	}
	else
		m_pHitCollider->SetActive(true);

	//PhysicMove
	if (false == m_tCurState.bRootMotion)
	{
		PHYSICMOVE tPhysicMove = StatePhysics[m_tCurState.iPhysicMoveID];

		if (true == tPhysicMove.bInitMovement)
		{
			XMStoreFloat3(&m_Scon.vMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
			XMStoreFloat3(&m_Scon.vPrevMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
		}
	}
}

HRESULT CP_Yangyang::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CP_Yangyang::Setup_ShadowShaderResource()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CP_Yangyang::Set_WeaponUse(_bool bBool)
{
	if (true == bBool)
	{
		if (m_bWeaponUsing != bBool)
		{
			m_bWeaponUsing = bBool;
			m_Parts[PARTS_WEAPON_SUB]->Start_Dissolve(true, 5.f, true);
			m_Parts[PARTS_WEAPON_MAIN]->Start_Dissolve(true, 5.f, true);

			m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON1);
			m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON2);
		}
	}
	else
	{
		if (m_bWeaponUsing != bBool)
		{
			m_bWeaponUsing = bBool;
			m_Parts[PARTS_WEAPON_SUB]->Start_Dissolve(true, 1.5f, false);
			m_Parts[PARTS_WEAPON_MAIN]->Start_Dissolve(true, 1.5f, false);

			m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON3);
			m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON4);
		}
	}
}

void CP_Yangyang::Apply_CoolTime(_double TimeDelta)
{
	for (_uint i = 0; i < IS_END; ++i)
	{
		if (0.0 < m_StateCoolTimes[i])
		{
			m_StateCoolTimes[i] -= TimeDelta;
			if (0.0 > m_StateCoolTimes[i])
				m_StateCoolTimes[i] = 0.0;
		}
	}

	if (SS_STAND1 == m_Scon.iCurState && true == m_bShowIdleAction)
		m_IdleTimeAcc += TimeDelta;
	else
		m_IdleTimeAcc = 0.0;

}

void CP_Yangyang::Key_Input(_double TimeDelta)
{
	m_WorldMatrix = m_pMainTransform->Get_WorldMatrix();

	CGameInstance* pGame = CGameInstance::GetInstance();
	CGameMode* pGM = CGameMode::GetInstance();

	//
	INPUT eCurFrameInput = INPUT_NONE;
	_bool bInputDir[4] = { false, false, false, false };
	m_bInputDirMove = false;

	// 이동 기능 구현용 임시 코드
	_matrix matCam = pGame->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_vector vCamLook = XMVector3Normalize(XMVectorSetY(matCam.r[2], 0.f));
	_vector vCamRight = XMVector3Normalize(XMVectorSetY(matCam.r[0], 0.f));
	_vector vInputDir = XMVectorZero();

	// 입력 제한 걸기
	m_bInputLock = pGM->IsActiveUI() ? false : true;

	if (m_bInputLock)
	{
		if (pGame->InputKey(DIK_C) == KEY_STATE::TAP)
			m_Scon.bWalk = !m_Scon.bWalk;

		if (pGame->InputKey(DIK_P) == KEY_STATE::TAP)
		{
			// 초기위치 설정
			m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(145.974f, 25.767f, 183.063f, 1.f));
			m_pNaviCom->Set_CurrentIndex(1340);
			m_iAirJumpCount += 10;
			m_fSkillGauge += 50.f;

			m_Scon.ePositionState = PS_AIR;
		}

		// 이동 방향 체크

		if (pGame->InputKey(DIK_W) == KEY_STATE::HOLD)
		{
			bInputDir[0] = true;
			vInputDir += vCamLook;
		}
		else if (pGame->InputKey(DIK_S) == KEY_STATE::HOLD)
		{
			bInputDir[1] = true;
			vInputDir -= vCamLook;
		}


		if (pGame->InputKey(DIK_A) == KEY_STATE::HOLD)
		{
			bInputDir[2] = true;
			vInputDir -= vCamRight;
		}

		else if (pGame->InputKey(DIK_D) == KEY_STATE::HOLD)
		{
			bInputDir[3] = true;
			vInputDir += vCamRight;
		}


		if (bInputDir[0] ||
			bInputDir[1] ||
			bInputDir[2] ||
			bInputDir[3])
			eCurFrameInput = INPUT_MOVE;
		else
			eCurFrameInput = INPUT_NONE;

		// Shift 입력 시 회피
		if (pGame->InputKey(DIK_LSHIFT) == KEY_STATE::TAP)
		{
			if (INPUT_NONE == eCurFrameInput)
				eCurFrameInput = INPUT_BACKSTEP;
			else if (INPUT_MOVE == eCurFrameInput)
				eCurFrameInput = INPUT_FRONTSTEP;
		}

		if (pGame->InputKey(DIK_LSHIFT) == KEY_STATE::HOLD && INPUT_MOVE == eCurFrameInput)
		{
			eCurFrameInput = INPUT_DASH;
		}

		// Skill
		if (pGame->InputKey(DIK_E) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_SKILL;
		}


		// Burst
		if (pGame->InputKey(DIK_R) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_BURST;
		}

		// Jump
		if (pGame->InputKey(DIK_SPACE) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_SPACE;
		}

		// Attack
		if (pGame->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_ATTACK;
		}

		if (pGame->InputMouse(DIMK_LB) == KEY_STATE::AWAY)
		{
			if (m_AimChargeAcc > 0.5)
			{
				eCurFrameInput = INPUT_ATTACK_RELEASE;
			}
		}

		if (pGame->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
		{
			m_AimChargeAcc += TimeDelta;
			if (m_AimChargeAcc > 0.5)
				eCurFrameInput = INPUT_ATTACK_CHARGE;
		}
		else
			m_AimChargeAcc = 0.0;

		// Tool
		if (pGame->InputKey(DIK_T) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_TOOL;
		}

		// Echo Summon
		if (pGame->InputKey(DIK_Q) == KEY_STATE::TAP)
		{
			CCharacter* pTarget = nullptr;

			if (nullptr != m_pFixedTarget)
				pTarget = m_pFixedTarget;
			else if (nullptr != m_pNearst)
				pTarget = m_pNearst;

			m_pEchoSystem->Shot_Echo(CEchoSystem::YANGYANG, m_pMainTransform, pTarget, m_pNaviCom->Get_CurrentIndex());
		}

		if (pGame->InputKey(DIK_1) == KEY_STATE::TAP)
		{
			if (PS_GROUND == m_Scon.ePositionState &&
				7 > m_tCurState.iLeavePriority)
				m_pPlayerStateClass->Change_ActiveCharacter(CPlayerState::SLOT_SUB1);
		}

		if (m_pPlayerStateClass->Get_PlayerState()->iCharCount == 3)
		{
			if (pGame->InputKey(DIK_2) == KEY_STATE::TAP)
			{
				if (PS_GROUND == m_Scon.ePositionState &&
					7 > m_tCurState.iLeavePriority)
					m_pPlayerStateClass->Change_ActiveCharacter(CPlayerState::SLOT_SUB2);
			}
		}

		if (pGame->InputKey(DIK_UP) == KEY_STATE::TAP)
		{
			m_bShowIdleAction = false;
		}

		if (pGame->InputKey(DIK_DOWN) == KEY_STATE::TAP)
		{
			m_bShowIdleAction = true;
		}
	}


	// 타겟 방향
	_vector vTargetDir;
	if (nullptr != m_pFixedTarget)
		vTargetDir = XMVector3Normalize(XMVectorSetY(m_pFixedTarget->Get_Position() - this->Get_Position(), 0.f));
	else if (nullptr != m_pNearst)
		vTargetDir = XMVector3Normalize(XMVectorSetY(m_pNearst->Get_Position() - this->Get_Position(), 0.f));
	else
		vTargetDir = XMVectorZero();

	// 입력 방향
	if (0.f != XMVectorGetX(XMVector3Length(vInputDir)))
		XMVector3Normalize(vInputDir);

	// 임시 이펙트 재생용
	CEffect* pEffect = nullptr;
	_float4x4 ParentMatrix;



	// 입력에 따라 대응하는 다음 상태 찾기
	if (PS_GROUND == m_Scon.ePositionState)
	{
		switch (eCurFrameInput)
		{
		case Client::CP_Yangyang::INPUT_NONE:
			switch (m_Scon.iCurState)
			{
			case SS_WALK_F:
				m_Scon.iNextState = SS_WALK_STOP_L;
				break;
			case SS_RUN_F:
				m_Scon.iNextState = SS_RUN_STOP_L;
				break;
			case SS_WALK_B:
			case SS_WALK_LF:
			case SS_WALK_LB:
			case SS_WALK_RF:
			case SS_WALK_RB:
			case SS_RUN_B:
			case SS_RUN_LF:
			case SS_RUN_LB:
			case SS_RUN_RF:
			case SS_RUN_RB:
				// 강제 정지용 애니메이션, 진입 우선순위 빼고는 Idle이랑 같음
				m_Scon.iNextState = SS_STAND1_ACTION02;
				break;

			case SS_SPRINT:
			case SS_SPRINT_IMPULSE_F:
				m_Scon.iNextState = SS_SPRINT_STOP_L;
				break;

			case SS_STAND1:
				if (8.0 < m_IdleTimeAcc)
					m_Scon.iNextState = SS_STAND1_ACTION01;
				else
					m_Scon.iNextState = SS_STAND1;
				break;

			default:
				m_Scon.iNextState = SS_STAND1;
				break;
			}
			break;

		case Client::CP_Yangyang::INPUT_MOVE:

			if (nullptr != m_pFixedTarget)
			{
				if (m_Scon.bWalk)
				{
					_bool	bLeft = (0.f > XMVectorGetY(XMVector3Cross(vTargetDir, vInputDir)));
					_float fScalar = XMVectorGetX(XMVector3Dot(vTargetDir, vInputDir));
					if (fScalar > 0.85f)
						m_Scon.iNextState = SS_WALK_F;
					else if (fScalar < -0.85f)
						m_Scon.iNextState = SS_WALK_B;
					else if (bLeft == true)
					{
						if (fScalar >= 0.f)
							m_Scon.iNextState = SS_WALK_LF;
						else
							m_Scon.iNextState = SS_WALK_LB;
					}
					else if (bLeft == false)
					{
						if (fScalar >= 0.f)
							m_Scon.iNextState = SS_WALK_RF;
						else
							m_Scon.iNextState = SS_WALK_RB;
					}
				}
				else
				{
					_bool	bLeft = (0.f > XMVectorGetY(XMVector3Cross(vTargetDir, vInputDir)));
					_float fScalar = XMVectorGetX(XMVector3Dot(vTargetDir, vInputDir));
					if (fScalar > 0.85f)
						m_Scon.iNextState = SS_RUN_F;
					else if (fScalar < -0.85f)
						m_Scon.iNextState = SS_RUN_B;
					else if (bLeft == true)
					{
						if (fScalar >= 0.f)
							m_Scon.iNextState = SS_RUN_LF;
						else
							m_Scon.iNextState = SS_RUN_LB;
					}
					else if (bLeft == false)
					{
						if (fScalar >= 0.f)
							m_Scon.iNextState = SS_RUN_RF;
						else
							m_Scon.iNextState = SS_RUN_RB;
					}
				}
				if (m_tCurState.iLeavePriority == 0)
				{
					m_bInputDirMove = true;
					XMStoreFloat3(&m_vInputDir, vInputDir);
				}
			}
			else
			{
				if (m_Scon.bWalk)
					m_Scon.iNextState = SS_WALK_F;
				else
					m_Scon.iNextState = SS_RUN_F;
			}
			break;

		case Client::CP_Yangyang::INPUT_DASH:
			if (SS_CLIMB_BOOST_ONTOPTOSTOP == m_Scon.iCurState)
				m_Scon.iNextState = SS_CLIMB_BOOST_ONTOPTOSPRINT;
			else
			{
				if (nullptr == m_pFixedTarget)
				{
					if (SS_SPRINT != m_Scon.iCurState)
						m_Scon.iNextState = SS_SPRINT_IMPULSE_F;
					else
						m_Scon.iNextState = SS_SPRINT;
				}
				else
				{
					if ((m_Scon.iCurState == SS_MOVE_B && m_Scon.TrackPos >= 12.0) ||
						(m_Scon.iCurState == SS_MOVE_F && m_Scon.TrackPos >= 12.0) ||
						m_Scon.iCurState == SS_SPRINT)
						m_Scon.iNextState = SS_SPRINT;
				}
			}
			break;
		case Client::CP_Yangyang::INPUT_FRONTSTEP:
			if (m_Scon.iCurState != SS_MOVE_B)
				m_Scon.iNextState = SS_MOVE_F;
			break;

		case Client::CP_Yangyang::INPUT_BACKSTEP:
			if (m_Scon.iCurState != SS_MOVE_F)
				m_Scon.iNextState = SS_MOVE_B;
			break;
		case Client::CP_Yangyang::INPUT_SPACE:
			if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
				m_Scon.iNextState = SS_JUMP_WALK;
			else
				m_Scon.iNextState = SS_JUMP_RUN;
			break;

		case Client::CP_Yangyang::INPUT_ATTACK:
			switch (m_Scon.iCurState)
			{
			case IS_ATTACK_01:
				m_Scon.iNextState = IS_ATTACK_02;
				break;
			case IS_ATTACK_02:
				m_Scon.iNextState = IS_ATTACK_03;
				break;
			case IS_ATTACK_03:
				m_Scon.iNextState = IS_ATTACK_04;
				break;

			case SS_MOVE_LIMIT_B:
			case SS_MOVE_LIMIT_F:
				m_Scon.iNextState = IS_SKILL_02;
				break;

			case IS_ATTACK_05:
				m_Scon.iNextState = IS_ATTACK_06;
				break;

			case IS_SKILL_02:
				if (m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL] == m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL])
					m_Scon.iNextState = IS_SKILL_03;
				else
					m_Scon.iNextState = IS_ATTACK_06;
				break;

			default:
				m_Scon.iNextState = IS_ATTACK_01;
				break;
			}
			break;

		case Client::CP_Yangyang::INPUT_ATTACK_CHARGE:
			if (m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL] == m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL])
				m_Scon.iNextState = IS_SKILL_02;
			else 
				m_Scon.iNextState = IS_ATTACK_05;
			break;

		case Client::CP_Yangyang::INPUT_SKILL:
			if (0.0 == m_pCharacterState->fCurCooltime[CPlayerState::COOL_SKILL])
				m_Scon.iNextState = IS_SKILL_01;
			break;

		case Client::CP_Yangyang::INPUT_BURST:
			if (0.0 == m_pCharacterState->fCurCooltime[CPlayerState::COOL_BURST] &&
				m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST] == m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST])
				m_Scon.iNextState = IS_BURST;
			break;

		case Client::CP_Yangyang::INPUT_TOOL:
			if (0.0 == m_pPlayerStateClass->Get_CurToolCoolTime())
			{
				if (CPlayerState::TOOL_FIXHOOK == m_pPlayerStateClass->Get_CurToolID())
					m_Scon.iNextState = SS_FIXHOOK_END_UP;
				else if (CPlayerState::TOOL_SCANNER == m_pPlayerStateClass->Get_CurToolID())
				{
					m_pRader->Play_Rader(m_pMainTransform);
					m_pPlayerStateClass->Set_ToolUsed();
				}
			}
			break;
		default:
			break;
		}
	}
	else if (PS_AIR == m_Scon.ePositionState)
	{
		switch (eCurFrameInput)
		{
		case Client::CP_Yangyang::INPUT_SPACE:
			if (m_iAirJumpCount > 0)
			{
				if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
					m_Scon.iNextState = SS_JUMP_SECOND_B;
				else
					m_Scon.iNextState = SS_JUMP_SECOND_F;
			}
			break;
		case Client::CP_Yangyang::INPUT_ATTACK:
			if (IS_SKILL_QTE == m_Scon.iCurState)
				m_Scon.iNextState = IS_AIRATTACK_2_QTE_START;
			else if (m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL] <= m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] || IS_SKILL_QTE == m_Scon.iCurState)
				m_Scon.iNextState = IS_AIRATTACK_2_START;
			else
				m_Scon.iNextState = IS_AIRATTACK_1_START;
			break;
		case Client::CP_Yangyang::INPUT_TOOL:
			if (0.0 == m_pPlayerStateClass->Get_CurToolCoolTime())
			{
				if (CPlayerState::TOOL_FIXHOOK == m_pPlayerStateClass->Get_CurToolID())
					m_Scon.iNextState = SS_FIXHOOK_END_UP;
				else if (CPlayerState::TOOL_SCANNER == m_pPlayerStateClass->Get_CurToolID())
					int a = 1; // 즉발 후 쿨타임 세팅을 할 수도 있음 

			}
			break;
		default:
			break;
		}
	}
	else if (PS_CLIMB == m_Scon.ePositionState)
	{
		switch (eCurFrameInput)
		{
		case Client::CP_Yangyang::INPUT_NONE:
			switch (m_Scon.iCurState)
			{
			case SS_CLIMB_D1:
				m_Scon.iNextState = SS_CLIMB_D1_STOP;
				break;
			case SS_CLIMB_D2:
				m_Scon.iNextState = SS_CLIMB_D2_STOP;
				break;
			case SS_CLIMB_DL1:
				m_Scon.iNextState = SS_CLIMB_DL1_STOP;
				break;
			case SS_CLIMB_DL2:
				m_Scon.iNextState = SS_CLIMB_DL2_STOP;
				break;
			case SS_CLIMB_DR1:
				m_Scon.iNextState = SS_CLIMB_DR1_STOP;
				break;
			case SS_CLIMB_DR2:
				m_Scon.iNextState = SS_CLIMB_DR2_STOP;
				break;
			case SS_CLIMB_L1:
				m_Scon.iNextState = SS_CLIMB_L1_STOP;
				break;
			case SS_CLIMB_L2:
				m_Scon.iNextState = SS_CLIMB_L2_STOP;
				break;
			case SS_CLIMB_R1:
				m_Scon.iNextState = SS_CLIMB_R1_STOP;
				break;
			case SS_CLIMB_R2:
				m_Scon.iNextState = SS_CLIMB_R2_STOP;
				break;
			case SS_CLIMB_U1:
				m_Scon.iNextState = SS_CLIMB_U1_STOP;
				break;
			case SS_CLIMB_U2:
				m_Scon.iNextState = SS_CLIMB_U2_STOP;
				break;
			case SS_CLIMB_UL1:
				m_Scon.iNextState = SS_CLIMB_UL1_STOP;
				break;
			case SS_CLIMB_UL2:
				m_Scon.iNextState = SS_CLIMB_UL2_STOP;
				break;
			case SS_CLIMB_UR1:
				m_Scon.iNextState = SS_CLIMB_UR1_STOP;
				break;
			case SS_CLIMB_UR2:
				m_Scon.iNextState = SS_CLIMB_UR2_STOP;
				break;
			case SS_CLIMB_BOOST_U:
			case SS_CLIMB_BOOST_U_START:
				m_Scon.iNextState = SS_CLIMB_BOOST_U_STOP;
				break;
			case SS_CLIMB_BOOST_L:
			case SS_CLIMB_BOOST_L_START:
				m_Scon.iNextState = SS_CLIMB_BOOST_L_STOP;
				break;
			case SS_CLIMB_BOOST_R:
			case SS_CLIMB_BOOST_R_START:
				m_Scon.iNextState = SS_CLIMB_BOOST_R_STOP;
				break;
			default:
				m_Scon.iNextState = SS_CLIMB_STAND;
				break;
			}
			break;

		case Client::CP_Yangyang::INPUT_MOVE:
			if (SS_CLIMB_BOOST_U_START == m_Scon.iCurState ||
				SS_CLIMB_BOOST_U == m_Scon.iCurState)
			{
				m_Scon.iNextState = SS_CLIMB_BOOST_U_STOP;
			}
			else if (SS_CLIMB_BOOST_L_START == m_Scon.iCurState ||
				SS_CLIMB_BOOST_L == m_Scon.iCurState)
			{
				m_Scon.iNextState = SS_CLIMB_BOOST_L_STOP;
			}
			else if (SS_CLIMB_BOOST_R_START == m_Scon.iCurState ||
				SS_CLIMB_BOOST_R == m_Scon.iCurState)
			{
				m_Scon.iNextState = SS_CLIMB_BOOST_R_STOP;
			}
			else
			{
				if (bInputDir[0])
				{
					if (bInputDir[2])
					{
						if (SS_CLIMB_UL1 == m_Scon.iCurState ||
							SS_CLIMB_UL1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_UL2;
						else
							m_Scon.iNextState = SS_CLIMB_UL1;
					}
					else if (bInputDir[3])
					{
						if (SS_CLIMB_UR1 == m_Scon.iCurState ||
							SS_CLIMB_UR1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_UR2;
						else
							m_Scon.iNextState = SS_CLIMB_UR1;
					}
					else
					{
						if (SS_CLIMB_U1 == m_Scon.iCurState ||
							SS_CLIMB_U1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_U2;
						else
							m_Scon.iNextState = SS_CLIMB_U1;
					}
				}
				else if (bInputDir[1])
				{
					if (bInputDir[2])
					{
						if (SS_CLIMB_DL1 == m_Scon.iCurState ||
							SS_CLIMB_DL1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_DL2;
						else
							m_Scon.iNextState = SS_CLIMB_DL1;
					}
					else if (bInputDir[3])
					{
						if (SS_CLIMB_DR1 == m_Scon.iCurState ||
							SS_CLIMB_DR1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_DR2;
						else
							m_Scon.iNextState = SS_CLIMB_DR1;
					}
					else
					{
						if (SS_CLIMB_D1 == m_Scon.iCurState ||
							SS_CLIMB_D1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_D2;
						else
							m_Scon.iNextState = SS_CLIMB_D1;
					}
				}
				else
				{
					if (bInputDir[2])
					{
						if (SS_CLIMB_L1 == m_Scon.iCurState ||
							SS_CLIMB_L1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_L2;
						else
							m_Scon.iNextState = SS_CLIMB_L1;
					}
					else if (bInputDir[3])
					{
						if (SS_CLIMB_R1 == m_Scon.iCurState ||
							SS_CLIMB_R1_STOP == m_Scon.iCurState)
							m_Scon.iNextState = SS_CLIMB_R2;
						else
							m_Scon.iNextState = SS_CLIMB_R1;
					}
				}
			}

			break;
		case Client::CP_Yangyang::INPUT_DASH:
			if (bInputDir[0])
			{
				switch (m_Scon.iCurState)
				{
				case SS_CLIMB_BOOST_U_START:
				case SS_CLIMB_BOOST_U:
				case SS_CLIMB_BOOST_L_START:
				case SS_CLIMB_BOOST_L:
				case SS_CLIMB_BOOST_R_START:
				case SS_CLIMB_BOOST_R:
					m_Scon.iNextState = SS_CLIMB_BOOST_U;
					break;
				default:
					m_Scon.iNextState = SS_CLIMB_BOOST_U_START;
					break;
				}
			}
			else if (bInputDir[2])
			{
				switch (m_Scon.iCurState)
				{
				case SS_CLIMB_BOOST_U_START:
				case SS_CLIMB_BOOST_U:
				case SS_CLIMB_BOOST_L_START:
				case SS_CLIMB_BOOST_L:
				case SS_CLIMB_BOOST_R_START:
				case SS_CLIMB_BOOST_R:
					m_Scon.iNextState = SS_CLIMB_BOOST_L;
					break;
				default:
					m_Scon.iNextState = SS_CLIMB_BOOST_L_START;
					break;
				}
			}
			else if (bInputDir[3])
			{
				switch (m_Scon.iCurState)
				{
				case SS_CLIMB_BOOST_U_START:
				case SS_CLIMB_BOOST_U:
				case SS_CLIMB_BOOST_L_START:
				case SS_CLIMB_BOOST_L:
				case SS_CLIMB_BOOST_R_START:
				case SS_CLIMB_BOOST_R:
					m_Scon.iNextState = SS_CLIMB_BOOST_R;
					break;
				default:
					m_Scon.iNextState = SS_CLIMB_BOOST_R_START;
					break;
				}
			}
			break;
		case Client::CP_Yangyang::INPUT_SPACE:
			if (bInputDir[0])
			{
				if (bInputDir[2])
					m_Scon.iNextState = SS_CLIMB_DASH_UL;
				else if (bInputDir[3])
					m_Scon.iNextState = SS_CLIMB_DASH_UR;
				else
					m_Scon.iNextState = SS_CLIMB_DASH_U;
			}
			else if (bInputDir[1])
			{
				if (bInputDir[2])
					m_Scon.iNextState = SS_CLIMB_DASH_DL;
				else if (bInputDir[3])
					m_Scon.iNextState = SS_CLIMB_DASH_DR;
				else
					m_Scon.iNextState = SS_CLIMB_DASH_D;
			}
			else
			{
				if (bInputDir[2])
					m_Scon.iNextState = SS_CLIMB_DASH_L;
				else if (bInputDir[3])
					m_Scon.iNextState = SS_CLIMB_DASH_R;
				else
					m_Scon.iNextState = SS_CLIMB_MOVE;
			}
			break;
		default:
			break;
		}
	}




	// 그래서 어느 방향을 바라보는가
	_vector vFinalDir;

	// 공격 행동일 경우
	if (eCurFrameInput >= INPUT_ATTACK)
	{
		// 고정 타겟이 있거나 가장 가까운 몬스터와의 거리가 10 미만일 때 타겟 방향
		if ((nullptr != m_pFixedTarget) ||
			(nullptr != m_pNearst && 10.f > m_fNearstDist))
			vFinalDir = vTargetDir;
		// 그 외 입력 방향
		else
			vFinalDir = vInputDir;
	}
	else if (m_bInputDirMove)
		vFinalDir = vTargetDir;
	else
		vFinalDir = vInputDir;

	// 지금 상태를 끊고 다음 상태로 갱신 할지 여부
	if (m_Scon.iCurState != m_Scon.iNextState)
	{
		if (m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority)
		{
			SetUp_State();
			SetUp_Animations(false);

			if (CCharacter::ROT_ONSTART == m_tCurState.iRotationType)
			{
				// 제로 벡터 뜨는 경우는 갱신X
				if (!XMVector3Equal(vFinalDir, XMVectorZero()))
					m_pMainTransform->Set_LookDir(vFinalDir);
			}

			//궁극기 카메라 모션 실행
			if (IS_BURST == m_Scon.iCurState)
				m_pCamMovement->UseCamera(CCameraMovement::CAM_YANGYANG);

		}
	}

	// 매 프레임
	if (PS_CLIMB != m_Scon.ePositionState)
	{
		if (CCharacter::ROT_LOOKAT == m_tCurState.iRotationType)
		{
			if (!XMVector3Equal(vFinalDir, XMVectorZero()))
				m_pMainTransform->Set_LookDir(vFinalDir);
		}
		else if (CCharacter::ROT_TURN == m_tCurState.iRotationType)
		{
			_vector vAxis = XMVector3Cross(m_pMainTransform->Get_State(CTransform::STATE_LOOK), vFinalDir);
			_float fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_LOOK)), XMVector3Normalize(vFinalDir))));

			if (!XMVector3Equal(vAxis, XMVectorZero()))
			{
				if (fAngle > m_pMainTransform->Get_RotationSpeed() * TimeDelta)
					m_pMainTransform->Rotate(vAxis, TimeDelta);
				else
					m_pMainTransform->Set_LookDir(vFinalDir);
			}
		}
	}

}

void CP_Yangyang::Tick_State(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();



	if (false == m_Scon.bAnimFinished)
	{
		_float4 vRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float3 vMovement;
		_double ProgressRatio;

		// 애니메이션 갱신
		m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_Scon.TrackPos, &m_Scon.bAnimFinished, &ProgressRatio);
		m_pAnimSetCom[ANIMSET_RIBBON]->Update_RibbonAnimation(ProgressRatio);

		m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();

		m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();
		// 여기까지 Playanimation

		m_pModelCom->Invalidate_CombinedMatrices();

		// 루프하지 않는 애니메이션이 이번 프레임에 끝났으면 전프레임 움직임을 적용
		// 애니메이션 프레임이랑 FrameRate가 달라서 발생하는 오차값으로 인한 버그 방지
		if (true == m_Scon.bAnimFinished && false == m_tCurState.bLoop)
			XMStoreFloat3(&vMovement, XMLoadFloat3(&m_Scon.vPrevMovement) * (_float)TimeDelta);
		else
		{
			// 루트 모션 처리(회전, 이동)
			if (true == m_tCurState.bRootMotion)
			{
				if (!XMVector4Equal(XMQuaternionIdentity(), XMLoadFloat4(&vRotation)))
					m_pMainTransform->Rotate_Quaternion(XMLoadFloat4(&vRotation));
			}
			else
			{
				const PHYSICMOVE& PhysicMove = StatePhysics[m_tCurState.iPhysicMoveID];
				if (false == PhysicMove.bConstant)
				{
					_vector vMove = XMLoadFloat3(&m_Scon.vPrevMovement);

					_float fXZSpeed = XMVectorGetX(XMVector2Length(vMove)) * (1.f - (_float)TimeDelta * PhysicMove.fHorizontalAtten);
					if (fabs(fXZSpeed) < 0.01f)
						fXZSpeed = 0.f;
					_float fYSpeed = (XMVectorGetZ(vMove) - PhysicMove.fVerticalAccel * (_float)TimeDelta);

					fXZSpeed = min(fXZSpeed, PhysicMove.fHorizontalMaxSpeed);
					fYSpeed = max(fYSpeed, PhysicMove.fVerticalMaxSpeed);

					_vector vFinalMove = XMVectorSetZ(XMVector2Normalize(vMove) * fXZSpeed, fYSpeed) * (_float)TimeDelta;

					XMStoreFloat3(&vMovement, vFinalMove);
				}
				else
				{
					if (true == m_bInputDirMove)
					{
						XMStoreFloat3(&vMovement, XMLoadFloat3(&m_vInputDir) * PhysicMove.fInitForce * (_float)TimeDelta);
					}
					else
						XMStoreFloat3(&vMovement, XMLoadFloat3(&m_Scon.vMovement) * (_float)TimeDelta);
				}

			}

		}
		// 구해진 이동값만큼 움직이고 이전 프레임 정보를 저장, + TimeDelta 대응
		if (SS_CLIMB_ONTOP == m_Scon.iCurState ||
			SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState)
			m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, nullptr, m_pModelCom->Get_TopBoneCombinedPos(), m_pClimbBones[CBONE_SPINE]->Get_CombinedPosition_Float3());
		else
		{
			if (true == m_bInputDirMove)
				m_pMainTransform->Push_OnNavi(XMLoadFloat3(&vMovement), m_pNaviCom);
			else
				m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, m_pNaviCom, m_pModelCom->Get_TopBoneCombinedPos(), m_pClimbBones[CBONE_SPINE]->Get_CombinedPosition_Float3());
		}


		XMStoreFloat3(&m_Scon.vPrevMovement, XMLoadFloat3(&vMovement) / (_float)TimeDelta);


		// 이번 프레임에 공격이 발동했는지 체크하는 변수
		m_bAttack = false;

		// StateKey 처리
		for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
		{
			if (nullptr != m_tCurState.ppStateKeys[i])
				m_tCurState.ppStateKeys[i]->Check(m_Scon.TrackPos, this);
		}

		// 떨어지고 있을 경우 
		if (vMovement.z < 0.f)
		{
			if (!(SS_FIXHOOK_END_UP == m_Scon.iCurState && 10.f > m_Scon.TrackPos))
				m_Scon.bFalling = true;
		}
	}

	if (true == m_Scon.bAnimFinished)
	{
		if (true == m_tCurState.bLoop)
		{
			m_Scon.iNextState = m_Scon.iCurState;
			SetUp_State();
			m_tCurState.bLerp = false;
			SetUp_Animations(true);

		}
		else
		{
			if (NO_ANIM != m_tCurState.iNextState)
			{
				m_Scon.iNextState = m_tCurState.iNextState;
				SetUp_State();
				SetUp_Animations(true);
			}
		}
	}
}

void CP_Yangyang::On_Cell()
{
	CGameMode* pGM = CGameMode::GetInstance();

	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_float fPosY = XMVectorGetY(vPos);
	_float fCellHeight = m_pNaviCom->Compute_Height(vPos);
	CCell* pCell = m_pNaviCom->Get_CurCell();
	_uint iCurCellState = m_pNaviCom->Get_CurCellState();

	if (PS_GROUND == m_Scon.ePositionState)
	{
		if (m_Scon.iPrevCellState != iCurCellState && CELL_WALL == iCurCellState &&
			pCell->DistanceToWall(vPos) < 0.f)
		{
			if (SS_SPRINT == m_Scon.iCurState ||
				SS_SPRINT_IMPULSE_F == m_Scon.iCurState)
			{
				m_Scon.iNextState = SS_CLIMB_BOOST_U_START;
				SetUp_State();
				SetUp_Animations(false);
			}
			else
			{
				m_Scon.iNextState = SS_CLIMB_START_UP;
				SetUp_State();
				SetUp_Animations(false);
			}
		}
		else if (SS_CLIMB_ONTOP == m_Scon.iCurState)
		{
			if (m_Scon.TrackPos < 14.0)
			{
				_vector vExitPos = XMLoadFloat3(&m_vClimbExitPos);
				_vector vExitLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);

				vExitPos += XMVector3Normalize(vExitLook) * 1.3f * (_float)m_Scon.TrackPos / 14.0f;
				_float fYTemp = XMVectorGetY(vExitPos) + m_fClimbExitYGap * (_float)m_Scon.TrackPos / 14.0f * 0.8f;

				vExitPos = XMVectorSetY(vExitPos, fYTemp);

				m_pMainTransform->Set_State(CTransform::STATE_POSITION, vExitPos);
			}
			else
			{
				if (CELL_GROUND == iCurCellState)
				{
					m_pMainTransform->Set_LookDir(XMVectorSetY(m_pMainTransform->Get_State(CTransform::STATE_LOOK), 0.f));
					m_pMainTransform->Set_PosY(fCellHeight);
				}
			}
		}
		else if (SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState)
		{
			_vector vExitPos = XMLoadFloat3(&m_vClimbExitPos);
			_vector vExitLook = m_pMainTransform->Get_State(CTransform::STATE_LOOK);

			vExitPos += XMVector3Normalize(vExitLook) * 1.3f * (_float)m_Scon.TrackPos / 9.0;
			_float fYTemp = XMVectorGetY(vExitPos) + m_fClimbExitYGap * (_float)m_Scon.TrackPos / 9.0f * 0.8f;

			vExitPos = XMVectorSetY(vExitPos, fYTemp);

			m_pMainTransform->Set_State(CTransform::STATE_POSITION, vExitPos);

			if (CELL_GROUND == iCurCellState)
			{
				m_pMainTransform->Set_LookDir(XMVectorSetY(m_pMainTransform->Get_State(CTransform::STATE_LOOK), 0.f));
				m_pMainTransform->Set_PosY(fCellHeight);
			}
		}
		else
		{
			if (fPosY - fCellHeight > 0.1f)
			{
				m_Scon.iNextState = SS_FALL;
				SetUp_State();
				SetUp_Animations(false);
			}
			else
				m_pMainTransform->Set_PosY(fCellHeight);
		}
	}
	else if (PS_AIR == m_Scon.ePositionState)
	{
		if (CELL_GROUND == iCurCellState)
		{
			if (fPosY - fCellHeight < 0.f)
			{
				if (false == m_Scon.bFalling)
				{
					// 오르막 방향으로 점프 시 예외처리
					m_pMainTransform->Set_PosY(fCellHeight);
				}
				else
				{
					// 공중 공격으로 착지하는 경우
					if (IS_AIRATTACK_1_START == m_Scon.iCurState ||
						IS_AIRATTACK_1_LOOP == m_Scon.iCurState)
					{
						m_Scon.iNextState = IS_AIRATTACK_1_END;
					}
					else if (IS_AIRATTACK_2_START == m_Scon.iCurState ||
						IS_AIRATTACK_2_LOOP == m_Scon.iCurState)
					{
						m_Scon.iNextState = IS_AIRATTACK_2_END;
					}
					else if (IS_AIRATTACK_2_QTE_START == m_Scon.iCurState ||
						IS_AIRATTACK_2_QTE_LOOP == m_Scon.iCurState)
					{
						m_Scon.iNextState = IS_AIRATTACK_2_QTE_END;
					}
					else if (SS_BEHIT_FLY_START == m_Scon.iCurState ||
						SS_BEHIT_PUSH == m_Scon.iCurState ||
						SS_BEHIT_FLY_LOOP == m_Scon.iCurState)
					{
						m_pCamMovement->StartWave(2);
						m_Scon.iNextState = SS_BEHIT_FLY_FALL;
					}
					// 일반적인 경우
					else
					{
						_float fGap = m_vJumpPos.y - fCellHeight;
						if (fGap > 4.f)
							m_Scon.iNextState = SS_LAND_ROLL;
						else if (fGap > 2.f)
							m_Scon.iNextState = SS_LAND_HEAVY;
						else
							m_Scon.iNextState = SS_LAND_LIGHT;
					}

					SetUp_State();
					SetUp_Animations(false);
				}
			}
		}
		else if (CELL_WALL == iCurCellState)
		{
			if (pCell->DistanceToWall(vPos) < 0.f)
			{
				m_Scon.iNextState = SS_CLIMB_STAND;

				SetUp_State();
				SetUp_Animations(false);
			}
		}
	}
	else if (PS_CLIMB == m_Scon.ePositionState)
	{
		_uint iExit = m_pNaviCom->Get_ExitClimb();

		if (CELL_WALL == iCurCellState)
		{
			// 명치를 기준으로 벽에 딱 붙임
			_vector vRootPos = XMVector3TransformCoord(m_pClimbBones[CBONE_SPINE]->Get_CombinedPosition(), XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
			_float fDistToWall = pCell->DistanceToWall(vRootPos);
			m_pMainTransform->Push_Position(pCell->Get_Normal() * (0.3f - fDistToWall));

			// 벽을 바라보게 함
			m_pMainTransform->Set_LookDir(pCell->Get_Normal() * -1.f);
		}

		if (CNavigation::EXIT_UP == iExit)
		{
			//m_pMainTransform->Push_Position(pCell->Get_Normal() * (-0.3f));
			XMStoreFloat3(&m_vClimbExitPos, m_pMainTransform->Get_State(CTransform::STATE_POSITION));

			m_fClimbExitYGap = m_pModelCom->Get_TopBoneCombinedPos()->y;

			if (SS_CLIMB_BOOST_U == m_Scon.iCurState ||
				SS_CLIMB_BOOST_U_START == m_Scon.iCurState ||
				SS_CLIMB_BOOST_L == m_Scon.iCurState ||
				SS_CLIMB_BOOST_L_START == m_Scon.iCurState ||
				SS_CLIMB_BOOST_R == m_Scon.iCurState ||
				SS_CLIMB_BOOST_R_START == m_Scon.iCurState)
			{
				m_Scon.iNextState = SS_CLIMB_BOOST_ONTOP;
			}
			else
			{
				m_Scon.iNextState = SS_CLIMB_ONTOP;
			}


			SetUp_State();
			SetUp_Animations(false);
			m_pNaviCom->Set_ExitClimb(CNavigation::EXIT_NO);

		}
		else if (CNavigation::EXIT_DOWN == iExit)
		{
			m_Scon.iNextState = SS_LAND_LIGHT;
			SetUp_State();
			SetUp_Animations(false);
			m_pNaviCom->Set_ExitClimb(CNavigation::EXIT_NO);
		}
		else if (CNavigation::EXIT_ALL == iExit)
		{
			m_pNaviCom->Set_ExitClimb(CNavigation::EXIT_NO);
		}
	}
}

void CP_Yangyang::On_Hit(CCharacter * pGameObject, TAGATTACK * pAttackInfo, _float fAttackPoint, _float3 * pEffPos, _float fCritRate, _float fCritDMG)
{


	// 저스트 회피 성공
	if ((SS_MOVE_B == m_Scon.iCurState ||
		SS_MOVE_F == m_Scon.iCurState) &&
		12.0 > m_Scon.TrackPos)
	{
		On_Dodge();
		return;
	}
	// 무적 상태
	else if (SS_MOVE_LIMIT_B == m_Scon.iCurState ||
		SS_MOVE_LIMIT_F == m_Scon.iCurState)
	{
		return;
	}

	static_cast<CCharacter*>(pGameObject)->Set_AttackHit(true);
	
	Play_HitSound(pAttackInfo);

	// 그 외 피격 처리
	// 피격 이펙트 출력
	if (lstrcmp(pAttackInfo->szHitEffectTag, TEXT("")))
	{
		CGameInstance* pGI = CGameInstance::GetInstance();
		_float4x4 EffectMatrix = m_pMainTransform->Get_WorldMatrix();
		memcpy(EffectMatrix.m[3], pEffPos, sizeof(_float3));
		CEffect* pEffect = pGI->Get_Effect(pAttackInfo->szHitEffectTag, (EFFECT_ID)pAttackInfo->iHitEffectID);
		if(nullptr != pEffect)
			pEffect->Play_Effect(&EffectMatrix);
	}

	// 대미지 계산 공식 : 모션 계수 * 공격력 * ((공격력 * 2 - 방어력) / 공격력) * (속성 보너스)
	// 공격력과 방어력이 같을 때 1배 대미지
	_float fDefenceRate = fAttackPoint / m_pCharacterState->fDefense[CPlayerState::STAT_TOTAL] * 1.75f;
	fDefenceRate = max(min(fDefenceRate, 1.2f), 0.25f);

	_float fFinalDamage = pAttackInfo->fDamageFactor * fAttackPoint * fDefenceRate;


	_bool bCrit = false;
	if (fCritRate > _float(rand() % 100))
	{
		bCrit = true;
		fFinalDamage *= fCritDMG * 0.01f;
	}

	fFinalDamage *= _float(110 - (rand() % 20)) * 0.01f;

	m_pCharacterState->fCurHP -= fFinalDamage;

	// TODO: 대미지 폰트 UI 출력
	// pEffPos : 두 콜라이더의 중간점, 이펙트 출력 기준위치
	// fFinalDamage : 최종 대미지
	// pAttackInfo->eElementType : 공격 판정의 속성
	m_pUIMain->Set_Damage(fFinalDamage);


	// 사망 시 사망 애니메이션 실행 
	if (false/*0.f >= m_tMonsterInfo.fCurHP*/)
	{
		m_pCharacterState->fCurHP = 0.f;
		m_Scon.iNextState = SS_DEAD;
	}

	else
	{
		if (PS_GROUND == m_Scon.ePositionState)
		{
			if (SS_BEHIT_FLY_FALL != m_Scon.iCurState)
			{
				switch (pAttackInfo->eHitIntensity)
				{
				case HIT_SMALL:
					m_Scon.iNextState = SS_BEHIT_S;
					m_pCamMovement->StartVibration(_uint(1));
					break;
				case HIT_BIG:
					m_Scon.iNextState = SS_BEHIT_B;
					m_pCamMovement->StartVibration(_uint(2));
					break;
				case HIT_FLY:
					m_Scon.iNextState = SS_BEHIT_FLY_START;
					m_pCamMovement->StartVibration(_uint(3));
					break;
				case HIT_PUSH:
					m_Scon.iNextState = SS_BEHIT_PUSH;
					m_pCamMovement->StartVibration(_uint(4));
					break;
				default:
					break;
				}
			}
			else
			{
				switch (pAttackInfo->eHitIntensity)
				{
				case HIT_FLY:
					m_Scon.iNextState = SS_BEHIT_FLY_START;
					m_pCamMovement->StartVibration(_uint(3));
					break;
				case HIT_PUSH:
					m_Scon.iNextState = SS_BEHIT_PUSH;
					m_pCamMovement->StartVibration(_uint(4));
					break;
				default:
					break;
				}
			}
		}
		else if (PS_AIR == m_Scon.ePositionState)
		{
			switch (pAttackInfo->eHitIntensity)
			{
			case HIT_FLY:
				m_Scon.iNextState = SS_BEHIT_FLY_START;
				m_pCamMovement->StartVibration(_uint(3));
				break;
			case HIT_PUSH:
				m_Scon.iNextState = SS_BEHIT_PUSH;
				m_pCamMovement->StartVibration(_uint(4));
				break;
			default:
				break;
			}
		}

		if (m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority)
		{
			m_pMainTransform->Set_LookDir(XMVectorSetY(
				static_cast<CTransform*>(pGameObject->Find_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION)
				- this->Get_Position(), 0.f));
			SetUp_State();
			SetUp_Animations(false);
		}
	}
}

void CP_Yangyang::On_Dodge()
{
	// Move > Move_Limit로 // 방향에 따라 Index가 2씩 차이남
	m_Scon.iNextState = m_Scon.iCurState + 2;

	SetUp_State();
	SetUp_Animations(false);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CRenderSetting::RGB_SPLIT_DESC Split;
	Split.m_fDistortion = 1.5f;
	Split.m_fStrength = 0.2f;
	Split.m_fSeparation = 0.3f;
	pGameInstance->SetSplitDesc(Split);
	pGameInstance->StartRGBSplit(CRenderSetting::SPLIT_DIR::SPLIT_DEFAULT, 0.7f);

}

void CP_Yangyang::Init_AnimSystem()
{
	for (auto& pBone : m_pAnimSetCom[ANIMSET_BASE]->Get_Bones())
		pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetCom[ANIMSET_BASE]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		if (!lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_Start")))
			pAnim->Set_Duration(10.0);

		if (!lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_Start_2")))
			pAnim->Set_Duration(30.0);


		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetCom[ANIMSET_BASE]->Get_BonePtr(pChannel->Get_TargetBoneID());

			if (wcsncmp(szChannelName, TEXT("Bip001"), 6) &&
				lstrcmp(szChannelName, TEXT("WeaponProp01")) &&
				lstrcmp(szChannelName, TEXT("WeaponProp02")) &&
				wcsncmp(szChannelName, TEXT("Root"), 4))
				pChannel->Set_Apply(false);

			if (true == pBone->Is_ChildOf(TEXT("Bip001Head")))
			{
				if (lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_End")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_End_2")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_Loop")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_Start")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_Start_2")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Attack01")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Attack02")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Attack03")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Attack04")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Attack06")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Burst01")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Skill01")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Skill02")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|SkillQte")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Stand1_Action01")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Stand1_Action02")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Stand1_Action03")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|Stand2")) &&
					lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|StandChange")))
				{
					pChannel->Set_Apply(false);
				}
			}
		}
	}

	// Ribbon
	for (auto& pBone : m_pAnimSetCom[ANIMSET_RIBBON]->Get_Bones())
		pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetCom[ANIMSET_RIBBON]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetCom[ANIMSET_RIBBON]->Get_BonePtr(pChannel->Get_TargetBoneID());

			/*if (pChannel->Get_NumKeyFrames() <= 2)
			continue;*/

			// spine 자식 Bone
			if (!(true == pBone->Is_ChildOf(TEXT("R_Skirt_Bone09")) ||
				!lstrcmp(szChannelName, TEXT("R_Skirt_Bone09")) ||
				true == pBone->Is_ChildOf(TEXT("R_Skirt_Bone19")) ||
				!lstrcmp(szChannelName, TEXT("R_Skirt_Bone19")) ||
				true == pBone->Is_ChildOf(TEXT("L_Skirt_Bone09")) ||
				!lstrcmp(szChannelName, TEXT("L_Skirt_Bone09")) ||
				true == pBone->Is_ChildOf(TEXT("L_Skirt_Bone19")) ||
				!lstrcmp(szChannelName, TEXT("L_Skirt_Bone19")) ||
				true == pBone->Is_ChildOf(TEXT("R_Skirt_Bone38")) ||
				!lstrcmp(szChannelName, TEXT("R_Skirt_Bone38")) ||
				true == pBone->Is_ChildOf(TEXT("L_Skirt_Bone38")) ||
				!lstrcmp(szChannelName, TEXT("L_Skirt_Bone38")) ||
				true == pBone->Is_ChildOf(TEXT("HairLong_Bone01")) ||
				!lstrcmp(szChannelName, TEXT("HairLong_Bone01")) ||
				true == pBone->Is_ChildOf(TEXT("HairLong_Bone20")) ||
				!lstrcmp(szChannelName, TEXT("HairLong_Bone20")) ||
				true == pBone->Is_ChildOf(TEXT("HairF_Bone17")) ||
				!lstrcmp(szChannelName, TEXT("HairF_Bone17")) ||
				true == pBone->Is_ChildOf(TEXT("HairF_Bone13")) ||
				!lstrcmp(szChannelName, TEXT("HairF_Bone13")) ||
				true == pBone->Is_ChildOf(TEXT("HairF_Bone09")) ||
				!lstrcmp(szChannelName, TEXT("HairF_Bone09")) ||
				true == pBone->Is_ChildOf(TEXT("HairF_Bone05")) ||
				!lstrcmp(szChannelName, TEXT("HairF_Bone05")) ||
				true == pBone->Is_ChildOf(TEXT("HairF_Bone01")) ||
				!lstrcmp(szChannelName, TEXT("HairF_Bone01")) ||
				true == pBone->Is_ChildOf(TEXT("Fadai_Bone01")) ||
				!lstrcmp(szChannelName, TEXT("Fadai_Bone01")) ||
				true == pBone->Is_ChildOf(TEXT("Fadai_Bone08")) ||
				!lstrcmp(szChannelName, TEXT("Fadai_Bone08")) ||
				true == pBone->Is_ChildOf(TEXT("Fadai_Bone11")) ||
				!lstrcmp(szChannelName, TEXT("Fadai_Bone11")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Piao007_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Piao007_R"))))
			{
				pChannel->Set_Apply(false);
			}
		}
	}

	// FramePerSec 세팅
	for (_uint i = 0; i < iStateLimit; ++i)
	{
		m_pAnimSetCom[ANIMSET_BASE]->Get_Animation(m_tStates[i].iAnimID[ANIMSET_BASE])->Set_TicksPerSecond(m_tStates[i].FramePerSec);
		m_pAnimSetCom[ANIMSET_RIBBON]->Get_Animation(m_tStates[i].iAnimID[ANIMSET_RIBBON])->Set_TicksPerSecond(m_tStates[i].FramePerSec);
	}
}

void CP_Yangyang::Init_AttackInfos()
{
	for (_uint i = 0; i < ATK_END; ++i)
	{
		ZeroMemory(&m_AttackInfos[i], sizeof TAGATTACK);
	}

	m_AttackInfos[ATK_ATTACK_01].fDamageFactor = 0.95f;
	m_AttackInfos[ATK_ATTACK_01].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_01].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK_01].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_01].fBPGain = 3.f;
	m_AttackInfos[ATK_ATTACK_01].fTPGain = 3.f;
	m_AttackInfos[ATK_ATTACK_01].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_01].szHitEffectTag, TEXT("Hit_Effect_Y_01"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_01].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_1.wem.wav"));

	m_AttackInfos[ATK_ATTACK_02].fDamageFactor = 1.1f;
	m_AttackInfos[ATK_ATTACK_02].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_02].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK_02].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_02].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_02].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_02].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_02].szHitEffectTag, TEXT("Hit_Effect_Y_02"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_02].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_2.wem.wav"));

	m_AttackInfos[ATK_ATTACK_03_1].fDamageFactor = 0.75f;
	m_AttackInfos[ATK_ATTACK_03_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_03_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK_03_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_03_1].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_03_1].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_03_1].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_02].szHitEffectTag, TEXT("Hit_Effect_Y_06"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_03_1].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_1.wem.wav"));

	m_AttackInfos[ATK_ATTACK_03_2].fDamageFactor = 1.05f;
	m_AttackInfos[ATK_ATTACK_03_2].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK_03_2].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK_03_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_03_2].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_03_2].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_03_2].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_02].szHitEffectTag, TEXT("Hit_Effect_Y_03"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_03_2].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_3.wem.wav"));

	m_AttackInfos[ATK_ATTACK_04_1].fDamageFactor = 0.9f;
	m_AttackInfos[ATK_ATTACK_04_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_04_1].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_ATTACK_04_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_04_1].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04_1].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04_1].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_04_1].szHitEffectTag, TEXT("Hit_Effect_Y_5"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_04_1].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_2.wem.wav"));

	m_AttackInfos[ATK_ATTACK_04_2].fDamageFactor = 1.3f;
	m_AttackInfos[ATK_ATTACK_04_2].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_04_2].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_ATTACK_04_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_04_2].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04_2].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04_2].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_04_2].szHitEffectTag, TEXT("Hit_Effect_Y_4_1"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_04_2].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_4.wem.wav"));

	m_AttackInfos[ATK_ATTACK_04_3].fDamageFactor = 1.6f;
	m_AttackInfos[ATK_ATTACK_04_3].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK_04_3].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_ATTACK_04_3].fSPGain = 1.f;
	m_AttackInfos[ATK_ATTACK_04_3].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04_3].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04_3].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_04_3].szHitEffectTag, TEXT("Hit_Effect_Y_4_2"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_04_3].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_5.wem.wav"));

	m_AttackInfos[ATK_ATTACK_05].fDamageFactor = 1.25f;
	m_AttackInfos[ATK_ATTACK_05].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_05].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_ATTACK_05].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_05].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_05].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_05].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_05].szHitEffectTag, TEXT("Hit_Effect_Y_5"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_05].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_3.wem.wav"));

	m_AttackInfos[ATK_ATTACK_06].fDamageFactor = 1.f;
	m_AttackInfos[ATK_ATTACK_06].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_06].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_ATTACK_06].fSPGain = 1.f;
	m_AttackInfos[ATK_ATTACK_06].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_06].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_06].iHitEffectID = 5;
	lstrcpy(m_AttackInfos[ATK_ATTACK_06].szHitEffectTag, TEXT("Hit_Effect_Y_6"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_06].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_5.wem.wav"));

	m_AttackInfos[ATK_AIRATTACK_1].fDamageFactor = 1.55f;
	m_AttackInfos[ATK_AIRATTACK_1].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_AIRATTACK_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_AIRATTACK_1].fSPGain = 0.f;
	m_AttackInfos[ATK_AIRATTACK_1].fBPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_1].fTPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_1].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_1].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Stick_Heavy_3.wem.wav"));

	m_AttackInfos[ATK_AIRATTACK_2_1].fDamageFactor = 0.45f;
	m_AttackInfos[ATK_AIRATTACK_2_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_AIRATTACK_2_1].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_AIRATTACK_2_1].fSPGain = 0.f;
	m_AttackInfos[ATK_AIRATTACK_2_1].fBPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_2_1].fTPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_2_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_2_1].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_2_1].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_2.wem.wav"));

	m_AttackInfos[ATK_AIRATTACK_2_2].fDamageFactor = 1.35f;
	m_AttackInfos[ATK_AIRATTACK_2_2].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_AIRATTACK_2_2].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_AIRATTACK_2_2].fSPGain = 0.f;
	m_AttackInfos[ATK_AIRATTACK_2_2].fBPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_2_2].fTPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_2_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_2_2].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_2_2].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Stick_Heavy_3.wem.wav"));

	m_AttackInfos[ATK_AIRATTACK_2_3].fDamageFactor = 2.15f;
	m_AttackInfos[ATK_AIRATTACK_2_3].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_AIRATTACK_2_3].eElementType = ELMT_AERO;
	m_AttackInfos[ATK_AIRATTACK_2_3].fSPGain = 0.f;
	m_AttackInfos[ATK_AIRATTACK_2_3].fBPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_2_3].fTPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK_2_3].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_2_3].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_AIRATTACK_2_3].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Sword_Light_2.wem.wav"));

	m_AttackInfos[ATK_SKILL_01_1].fDamageFactor = 1.2f;
	m_AttackInfos[ATK_SKILL_01_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_SKILL_01_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_SKILL_01_1].fSPGain = 1.f;
	m_AttackInfos[ATK_SKILL_01_1].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01_1].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_SKILL_01_1].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_SKILL_01_1].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Magic_Wind_5.wem.wav"));

	m_AttackInfos[ATK_SKILL_01_2].fDamageFactor = 0.65f;
	m_AttackInfos[ATK_SKILL_01_2].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_SKILL_01_2].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_SKILL_01_2].fSPGain = 0.f;
	m_AttackInfos[ATK_SKILL_01_2].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01_2].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_SKILL_01_2].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_SKILL_01_2].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Magic_Wind_4.wem.wav"));

	m_AttackInfos[ATK_SKILL_01_3].fDamageFactor = 2.f;
	m_AttackInfos[ATK_SKILL_01_3].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_SKILL_01_3].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_SKILL_01_3].fSPGain = 0.f;
	m_AttackInfos[ATK_SKILL_01_3].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01_3].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01_3].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_SKILL_01_3].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_SKILL_01_3].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Magic_Wind_3.wem.wav"));

	m_AttackInfos[ATK_SKILL_02].fDamageFactor = 2.6f;
	m_AttackInfos[ATK_SKILL_02].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_SKILL_02].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_SKILL_02].fSPGain = 0.f;
	m_AttackInfos[ATK_SKILL_02].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_02].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_02].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_SKILL_02].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_SKILL_02].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Magic_Wind_3.wem.wav"));

	m_AttackInfos[ATK_SKILL_QTE].fDamageFactor = 1.6f;
	m_AttackInfos[ATK_SKILL_QTE].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_SKILL_QTE].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_SKILL_QTE].fSPGain = 0.f;
	m_AttackInfos[ATK_SKILL_QTE].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_QTE].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_QTE].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_SKILL_QTE].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_SKILL_QTE].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Magic_Wind_3.wem.wav"));

	m_AttackInfos[ATK_BURST_1].fDamageFactor = 1.2f;
	m_AttackInfos[ATK_BURST_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_BURST_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_BURST_1].fSPGain = 0.f;
	m_AttackInfos[ATK_BURST_1].fBPGain = 1.5f;
	m_AttackInfos[ATK_BURST_1].fTPGain = 1.5f;
	m_AttackInfos[ATK_BURST_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_BURST_1].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_BURST_1].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Magic_Wind_2.wem.wav"));

	m_AttackInfos[ATK_BURST_2].fDamageFactor = 3.6f;
	m_AttackInfos[ATK_BURST_2].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_BURST_2].fSPGain = 0.f;
	m_AttackInfos[ATK_BURST_2].fBPGain = 1.5f;
	m_AttackInfos[ATK_BURST_2].fTPGain = 1.5f;
	m_AttackInfos[ATK_BURST_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_BURST_2].szHitEffectTag, TEXT("M_Blue_Hit"));
	lstrcpy(m_AttackInfos[ATK_BURST_2].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Magic_Wind_1.wem.wav"));

}

void CP_Yangyang::Init_Missiles()
{
	CMissilePool::MISSILEPOOLDESC tMissilePoolDesc;
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	// MISS_AIRATTACK
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_AirAttack_1_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 0; 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_AIRATTACK_1;
	tMissilePoolDesc.tMissileDesc.fExtents = 4.f;

	m_MissilePools[MISS_AIRATTACK_1] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_AIRATTACK_1] = _float3(0.f, 0.f, 0.f);

	// MISS_AIRATTACK_2_1
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_AirAttack_2_1_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 8;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 0; 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.15;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_AIRATTACK_2_1;
	tMissilePoolDesc.tMissileDesc.fExtents = 4.f;

	m_MissilePools[MISS_AIRATTACK_2_1] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 1.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_AIRATTACK_2_1] = _float3(0.f, 0.f, 0.f);

	// MISS_AIRATTACK_2_2
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_AirAttack_2_2_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 0; 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_AIRATTACK_2_2;
	tMissilePoolDesc.tMissileDesc.fExtents = 4.f;

	m_MissilePools[MISS_AIRATTACK_2_2] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_AIRATTACK_2_2] = _float3(0.f, 0.f, 0.f);

	// MISS_AIRATTACK_2_3
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_AirAttack_2_3_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 0; 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_AIRATTACK_2_3;
	tMissilePoolDesc.tMissileDesc.fExtents = 4.5f;

	m_MissilePools[MISS_AIRATTACK_2_3] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_AIRATTACK_2_3] = _float3(0.f, 0.f, 0.f);

	// MISS_SKILL_01_1
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_Skill_01_1_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("P_Yangyang_Skill_01_02"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 5;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.15;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_SKILL_01_1;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.5f;
	tMissilePoolDesc.tMissileDesc.ppNextMissilePool = &m_MissilePools[MISS_SKILL_01_2];
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_SKILL_01_1] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 4.5f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_SKILL_01_1] = _float3(0.f, 0.f, 0.f);

	// MISS_SKILL_01_2
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_Skill_01_2_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 0;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.15;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.75;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_SKILL_01_2;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.5f;
	tMissilePoolDesc.tMissileDesc.ppNextMissilePool = &m_MissilePools[MISS_SKILL_01_3];

	m_MissilePools[MISS_SKILL_01_2] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_SKILL_01_2] = _float3(0.f, 0.f, 0.f);

	// MISS_SKILL_01_3
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_Skill_01_3_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 0;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.15;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_SKILL_01_3;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.5f;

	m_MissilePools[MISS_SKILL_01_3] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_SKILL_01_3] = _float3(0.f, 0.f, 0.f);

	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	// MISS_SKILL_QTE
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_Skill_QTE_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 6;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 0; //PlayerGirl
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_SKILL_QTE;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_SKILL_QTE] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 1.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_SKILL_QTE] = _float3(0.f, 0.f, 0.f);

	// MISS_BURST_1
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_Burst_1_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("P_Yangyang_Burst_01"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 5; //PlayerGirl
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.3;
	tMissilePoolDesc.tMissileDesc.LifeTime = 2.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_BURST_1;
	tMissilePoolDesc.tMissileDesc.fExtents = 5.5f;
	tMissilePoolDesc.tMissileDesc.ppNextMissilePool = &m_MissilePools[MISS_BURST_2];
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_BURST_1] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 4.5f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_BURST_1] = _float3(0.f, 0.f, 0.f);

	// MISS_BURST_2
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Yangyang_Burst_2_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 3;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("P_Yangyang_Burst_02"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 5; //PlayerGirl
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_BURST_2;
	tMissilePoolDesc.tMissileDesc.fExtents = 7.f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_BURST_2] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_BURST_2] = _float3(0.f, 0.f, 0.f);
}

HRESULT CP_Yangyang::Init_Parts()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	m_PartsBone[PBONE_WEAPON1] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp01"));
	m_PartsBone[PBONE_WEAPON2] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp02"));
	m_PartsBone[PBONE_WEAPON3] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp03"));
	m_PartsBone[PBONE_WEAPON4] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp04"));
	m_PartsBone[PBONE_WEAPON5] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp05"));
	m_PartsBone[PBONE_WEAPON6] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp06"));
	m_PartsBone[PBONE_WEAPON7] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp07"));
	m_PartsBone[PBONE_HULU] = m_pModelCom->Get_BonePtr(TEXT("HuluProp01"));

	// 파츠는 캐릭터에 종속적, 레퍼런스 관리X, 삭제 순서 고려O
	CParts::PARTSDESC PartsDesc;
	ZeroMemory(&PartsDesc, sizeof PartsDesc);
	PartsDesc.ppParentBone = m_PartsBone;
	PartsDesc.pParentTransform = m_pMainTransform;

	// P_Player : 스태틱 변수로 Clone_GameObject해서 별도의

	// CharState? 지금 사용중인 WEAPONMAIN 

	m_Parts[PARTS_WEAPON_MAIN] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_SWORD_5_MAIN, &PartsDesc));
	m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(EBONE_WEAPON02);
	m_Parts[PARTS_WEAPON_MAIN]->Set_DissolveColor(_float3(0.3f, 0.6f, 1.f));

	m_Parts[PARTS_WEAPON_SUB] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_SWORD_5_SUB, &PartsDesc));
	m_Parts[PARTS_WEAPON_SUB]->Set_Parent(EBONE_WEAPON01);
	m_Parts[PARTS_WEAPON_SUB]->Set_DissolveColor(_float3(0.3f, 0.6f, 1.f));

	m_Parts[PARTS_HULU] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_HULU_2, &PartsDesc));
	m_Parts[PARTS_HULU]->Set_Parent(PBONE_HULU);
	m_Parts[PARTS_HULU]->Set_DissolveColor(_float3(0.3f, 0.3f, 1.f));

	return S_OK;
}

HRESULT CP_Yangyang::Init_EffectBones()
{
	//NONE은 걍 월드 매트릭스 던짐
	m_EffectBones[EBONE_SPINE2] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine2"));
	m_EffectBones[EBONE_WEAPON01] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp01"));
	m_EffectBones[EBONE_WEAPON02] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp02"));
	m_EffectBones[EBONE_LHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001LHand"));
	m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));

	return S_OK;
}

void CP_Yangyang::Update_EffectBones()
{
	memcpy(&m_EffectBoneMatrices[EBONE_NONE], &m_pMainTransform->Get_WorldMatrix(), sizeof(_float4x4));

	for (_uint i = 1; i < EBONE_END; ++i)
	{
		XMStoreFloat4x4(&m_EffectBoneMatrices[i], XMLoadFloat4x4(&m_EffectBones[i]->Get_CombinedTransfromationMatrix())
			* XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
	}
}

CP_Yangyang * CP_Yangyang::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CP_Yangyang* pInstance = new CP_Yangyang(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CP_Yangyang");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CP_Yangyang::Clone(void * pArg)
{
	CP_Yangyang* pInstance = new CP_Yangyang(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CP_Yangyang");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Yangyang::Free()
{
	__super::Free();

	for (_uint i = 0; i < PARTS_END; ++i)
	{
		Safe_Release(m_Parts[i]);
	}

	for (_uint i = 0; i < ANIMSET_END; ++i)
	{
		Safe_Release(m_pAnimSetCom[i]);
	}

	for (_uint i = 0; i < MISS_END; ++i)
	{
		Safe_Release(m_MissilePools[i]);
	}

	Safe_Release(m_pNaviCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pEyeBurstTexture);
	Safe_Release(m_pEyeMaskTexture);

	Safe_Release(m_pCollider);

	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pHitCollider);
	Safe_Release(m_pMoveCollider);
	Safe_Release(m_pRader);
}

void CP_Yangyang::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (pOpponent)
	{
#pragma region Attack & Hit
		// 상대가 몬스터일 경우
		if (CT_MONSTER == pOpponent->Get_CollType())
		{
			// 내 공격이 상대에게 적중한 경우
			if (true == src->Compare(GetAttackCollider()) &&
				true == dest->Compare(pOpponent->GetHitCollider()))
			{
				// 플레이어 전용 : 카메라 쉐이크 / 블러 / 쉐이더 / 히트렉(혼자 1~2프레임 애니메이션 정지 or 느려지기) 등??

				if (ATK_ATTACK_04_1 == m_iCurAttackID ||
					ATK_ATTACK_03_1 == m_iCurAttackID)
					m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_1);
				else if (ATK_ATTACK_01 == m_iCurAttackID ||
					ATK_ATTACK_02 == m_iCurAttackID ||
					ATK_ATTACK_03_2 == m_iCurAttackID ||
					ATK_ATTACK_04_2 == m_iCurAttackID)
					m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_2);
				else if (ATK_ATTACK_04_3 == m_iCurAttackID ||
					ATK_ATTACK_05 == m_iCurAttackID ||
					ATK_ATTACK_06 == m_iCurAttackID)
					m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_3);
				else if (ATK_SKILL_02 == m_iCurAttackID)
					m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_4);

			}

			// 상대의 OBB 공격이 나에게 적중한 경우 
			if (true == src->Compare(GetHitCollider()) &&
				true == dest->Compare(pOpponent->GetAttackCollider()))
			{
				TAGATTACK tAttackInfo;
				ZeroMemory(&tAttackInfo, sizeof(tAttackInfo));
				_float fAttackPoint = 0.f;

				pOpponent->Get_AttackInfo(pOpponent->Get_AttackID(), &tAttackInfo, &fAttackPoint);

				_vector destCenter = XMLoadFloat3(dest->GetWorldCenter());
				_vector srcCenter = XMLoadFloat3(src->GetWorldCenter());
				_float3 EffPos = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&EffPos, (destCenter + srcCenter) * 0.5f);

				//XMStoreFloat3(&EffPos, (XMLoadFloat3(&dest->GetCenter()) + XMLoadFloat3(&src->GetCenter())) * 0.5f);

				// 피격 처리 함수 : 대미지 처리, 대미지 폰트 출력, 피격 애니메이션 이행
				if (SS_DEAD != m_Scon.iCurState)
				{
					m_bHit = true;
					On_Hit(pOpponent, &tAttackInfo, fAttackPoint, &EffPos, pOpponent->Get_CritRate(), pOpponent->Get_CritDMG());
				}

			}
		}
#pragma endregion
	}

	CMissile* pMissile = dynamic_cast<CMissile*>(dest->GetOwner());

	if (pMissile)
	{
		CCharacter* pMissileOwner = pMissile->Get_Owner();
		if (CT_MONSTER == pMissileOwner->Get_CollType())
		{
			// 상대의 투사체 공격이 나에게 적중한 경우 
			if (true == src->Compare(GetHitCollider()))
			{
				// 플/몬 공통 : 대미지 처리, 대미지 폰트 출력, 피격 애니메이션 이행
				TAGATTACK tAttackInfo;
				ZeroMemory(&tAttackInfo, sizeof(tAttackInfo));
				_float fAttackPoint = 0.f;

				pMissileOwner->Get_AttackInfo(pMissile->Get_AttackID(), &tAttackInfo, &fAttackPoint);

				_vector destCenter = XMLoadFloat3(dest->GetWorldCenter());
				_vector srcCenter = XMLoadFloat3(src->GetWorldCenter());
				_float3 EffPos = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&EffPos, (destCenter + srcCenter) * 0.5f);

				/*_float3 EffPos;
				XMStoreFloat3(&EffPos, (XMLoadFloat3(&dest->GetCenter()) + XMLoadFloat3(&src->GetCenter())) * 0.5f);*/
				if (SS_DEAD != m_Scon.iCurState)
					On_Hit(pMissileOwner, &tAttackInfo, fAttackPoint, &EffPos, pMissileOwner->Get_CritRate(), pMissileOwner->Get_CritDMG());
			}
		}
	}
}

void CP_Yangyang::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());

	if (pOpponent)
	{
		if (CT_MONSTER == pOpponent->Get_CollType())
		{

#pragma region Move
			// pushWeight 값에 따라 서로를 밀어냄
			// 밀어내는 로직은 플레이어 쪽에서 처리?
			if (true == src->Compare(GetMoveCollider()) &&
				true == dest->Compare(pOpponent->GetMoveCollider()))
			{
				_float fTargetDistance = src->GetExtents().x + dest->GetExtents().x;
				_float fPushDistance = fTargetDistance - XMVectorGetX(XMVector3Length(pOpponent->Get_Position() - this->Get_Position()));
				_float fPushRatio = 1 - m_fPushWeight / (pOpponent->Get_PushWeight() + m_fPushWeight);
				_vector vPushDir = XMVector3Normalize(XMVectorSetY(this->Get_Position() - pOpponent->Get_Position(), 0.f));

				//_vector vTargetDir = XMLoadFloat3(&m_vTargetDir);

				//// 이번 프레임에 특정 타겟을 기준으로 이동했을 경우
				//if (!XMVector3Equal(XMVectorZero(), vTargetDir) && fTargetDistance * 0.5f < XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Scon.vPrevMovement))))
				//{
				//	// 이동 전 타겟 방향과 밀어내는 방향의 끼인각이 45도 미만이라면
				//	if (45.f > fabs(acosf(XMVectorGetX(XMVector3Dot(vPushDir, vTargetDir)))))
				//	{
				//		vPushDir *= -1.f;
				//	}
				//}

				// TODO: 내비메쉬 관련 예외처리 해야 함
				m_pMainTransform->Push_OnNavi(vPushDir * fPushDistance * fPushRatio, m_pNaviCom);
				pOpponent->Push_OnNavi(vPushDir * -1.f * fPushDistance * (1 - fPushRatio));

				//// 전진형 공격을 정지시킴
				if (IS_ATTACK_01 == m_Scon.iCurState ||
					IS_ATTACK_02 == m_Scon.iCurState ||
					IS_ATTACK_03 == m_Scon.iCurState ||
					IS_ATTACK_04 == m_Scon.iCurState ||
					IS_ATTACK_05 == m_Scon.iCurState)
				{
					m_tCurState.bRootMotion = false;
					m_Scon.vMovement = _float3(0.f, 0.f, 0.f);
				}
			}
#pragma endregion 
		}
	}

}

void CP_Yangyang::OnCollisionExit(CCollider * src, CCollider * dest)
{
}

