#include "stdafx.h"
#include "..\Public\P_Chixia.h"

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

const _int CP_Chixia::iStateLimit = CP_Chixia::IS_END;

CCharacter::MULTISTATE CP_Chixia::m_tStates[CP_Chixia::IS_END];

CP_Chixia::CP_Chixia(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CP_Chixia::CP_Chixia(const CP_Chixia & rhs)
	: CCharacter(rhs)
{
}

HRESULT CP_Chixia::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CP_Chixia::Initialize(void * pArg)
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

	// 상하체 본 분리 밑작업
	m_pAnimSetCom[ANIMSET_BASE]->Init_SubAnimController();
	m_pAnimSetCom[ANIMSET_BASE]->Set_SplitBone(TEXT("Bip001Spine"));
	m_pAnimSetCom[ANIMSET_BASE]->Split_UpperLower();

	m_pAnimSetCom[ANIMSET_RIBBON]->Init_SubAnimController();
	m_pAnimSetCom[ANIMSET_RIBBON]->Set_SplitBone(TEXT("Bip001Spine"));
	m_pAnimSetCom[ANIMSET_RIBBON]->Split_UpperLower();

	m_pModelCom->Set_SplitBone(TEXT("Bip001Spine"));

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

	//m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(39.125f, 2.290f, 30.776f, 1.f));
	//m_pNaviCom->Set_CurrentIndex(0);

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
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(70.f, 15.50f, 118.f, 1.f));
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
	//m_pAttackCollider->SetActive(false);

	_uint iLevel = pGameMode->GetCurrentLevel();

	if (iLevel == LEVEL_GAMEPLAY)
		m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(55.f));


	CGameInstance* pGame = CGameInstance::GetInstance();

	m_pEchoSystem = static_cast<CEchoSystem*>(pGame->Find_GameObject(LEVEL_STATIC, L"Echo"));

	m_pPlayerStateClass = static_cast<CPlayerState*>(pGame->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pPlayerStateClass->Register_Character(CPlayerState::CHARACTER_CHIXIA, this, &m_bOnControl);

	m_pCharacterState = m_pPlayerStateClass->Get_CharState_byChar(CPlayerState::CHARACTER_CHIXIA);

	m_pCharacterState->fMaxCooltime[CPlayerState::COOL_SKILL] = 5.f;
	m_pCharacterState->fMaxCooltime[CPlayerState::COOL_BURST] = 10.f;

	m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL] = 60.f;
	m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST] = 100.f;

	m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] = 60.f;
	m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] = 100.f;


	// TODO: 에코 쿨타임, 에코 착용 시 변경하도록 바꿔야 함
	m_pCharacterState->fMaxCooltime[CPlayerState::COOL_ECHO] = 5.f;
	//

	return S_OK;
}

void CP_Chixia::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

#ifdef _DEBUG
	m_pRendererCom->DebugBundleRender_Control(true);
#endif
	
	m_pInven = static_cast<CInventory*>(pGame->Find_GameObject(LEVEL_STATIC, L"Inventory"));
	m_pCamMovement = static_cast<CCameraMovement*>(pGame->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	m_pCamMovement->UseCamera(CCameraMovement::CAM_MAINPLAYER);
	m_pCamMovement->SetupBone(CCameraMovement::CAM_CHIXIA, m_pModelCom->Get_BonePtr(L"Bip001Spine1"));
	m_pUIMain = static_cast<CUI_MainScreen*>(pGame->Find_GameObject(LEVEL_ANYWHERE, L"UI_MainScreen"));

	if (false == m_bOnControl)
		SetState(DISABLE);
}

void CP_Chixia::PreTick(_double TimeDelta)
{
	// 타겟이 잡혀있다면 사용할 수 있는지, 거리가 너무 멀어지지는 않았는지 체크 및 타겟 해제
	if (nullptr != m_pFixedTarget)
	{
		if (false == m_pFixedTarget->IsActive() ||
			true == m_pFixedTarget->Get_Dying() ||
			25.f <  XMVectorGetX(XMVector3Length(m_pFixedTarget->Get_Position() - Get_Position())))
		{
			m_pFixedTarget = nullptr;
			m_pPlayerStateClass->Set_LockOn(false, nullptr);
		}
	}
}

void CP_Chixia::Tick(_double TimeDelta)
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


	if (true == m_bOnControl)
		Key_Input(TimeDelta * m_TimeDelay); // 입력 > 다음 상태 확인 > 갱신될 경우 Setup_state, setup_animation

	if (m_bHolding)
	{
		if (m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] == 0.f &&
			m_Scon.iCurState != IS_HOLDSHOT_UPPER_END)
		{
			m_Scon.iNextState = IS_HOLDSHOT_UPPER_END;
			SetUp_State();
			SetUp_Animations(false);
			
			m_Scon.iNextState = IS_HOLDSHOT_LOWER_END;
			SetUp_SubState();
			SetUp_SubAnimations(false);
		}
	}

	Apply_CoolTime(TimeDelta * m_TimeDelay);


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

	pGameInstance->AddCollider(m_pHitCollider, COLL_MONSTERATTACK);
	m_pHitCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_MOVE);
	m_pMoveCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_NPC);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
}

void CP_Chixia::LateTick(_double TimeDelta)
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

HRESULT CP_Chixia::Render()
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


	for (_uint i = 0; i < 6; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_VertexTexture(m_pShaderCom, "g_VertexTexture", i)))
			return E_FAIL;

		if (i == 5)
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
					_float3 vColor = { 0.8f, 0.2f, 0.2f }; 
					if (FAILED(m_pShaderCom->SetRawValue("g_vColor", &vColor, sizeof(_float3))))
						return E_FAIL;

					m_pShaderCom->Begin(17);		//Burst
				}
				else
					m_pShaderCom->Begin(7);
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
		if (m_bRimToggle && m_Scon.iCurState != IS_BURST && i != 5)
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
		if (i != 5 && m_fBurstRim > 0.f)
		{
			_float vRimPower = 10.f;
			_float3 vColor = { 0.8f, 0.2f, 0.2f };
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

HRESULT CP_Chixia::RenderShadow()
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

void CP_Chixia::RenderGUI()
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

void CP_Chixia::SetUp_Animations(_bool bContinue)
{
	m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_BASE], m_tCurState.bLerp, bContinue);
	m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_RIBBON], m_tCurState.bLerp, bContinue);
}

HRESULT CP_Chixia::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_CHIXIA_MODEL,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AnimSet_Base */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_CHIXIA_ANIMSET_BASE,
		TEXT("Com_AnimSet_Base"), (CComponent**)&m_pAnimSetCom[ANIMSET_BASE])))
		return E_FAIL;

	/* For.Com_AnimSet_Ribbon */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_CHIXIA_ANIMSET_RIBBON,
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

HRESULT CP_Chixia::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(MULTISTATE) * iStateLimit);

	// 로드하는 코드
	for (_int i = 0; i < iStateLimit; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, TEXT("../../Data/CharState/P_ChiXia/ChiXia_%d.state"), i);
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

void CP_Chixia::Release_States()
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

void CP_Chixia::Check_Nearst(CCharacter * pChar, _float fDist)
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

void CP_Chixia::Check_TimeDelay(_double TimeDelta)
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

void CP_Chixia::Appear(CTransform * pTransform, CCharacter * pTarget, _uint iNaviCellID, _float fDissolveSpeed)
{
	SetState(ACTIVE);

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
	Shot_DissolveKey(true, fDissolveSpeed);
	for (auto& pParts : m_Parts)
		pParts->Start_Dissolve(true, fDissolveSpeed, true);
}

void CP_Chixia::Disappear(CTransform ** ppTransform, CCharacter ** ppTarget, _uint* pNaviCellID)
{
	if(nullptr != ppTarget)
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

void CP_Chixia::Appear_QTE(CTransform * pTransform, CCharacter * pTarget, _uint iNaviCellID)
{
	SetState(ACTIVE);

	m_pFixedTarget = pTarget;

	m_pMainTransform->Set_WorldMatrix(pTransform->Get_WorldMatrix());

	CTransform* pTargetTransform = pTarget->GetTransform();
	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_POSITION) - vTargetPos);
	_vector vFinalPos;

	// 1번 슬롯일 경우 왼쪽
	if (CPlayerState::SLOT_SUB1 == m_pPlayerStateClass->Get_Slot(CPlayerState::CHARACTER_CHIXIA))
		vFinalPos = vTargetPos + XMVector3TransformNormal(vDir, XMMatrixRotationY(XMConvertToRadians(120.f)));
	// 2번 슬롯일 경우 오른쪽
	else
		vFinalPos = vTargetPos + XMVector3TransformNormal(vDir, XMMatrixRotationY(XMConvertToRadians(-120.f)));

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vFinalPos, XMVectorGetY(m_pMainTransform->Get_State(CTransform::STATE_POSITION))));
	m_pMainTransform->Set_LookDir(XMVectorSetY(vTargetPos - vFinalPos, 0.f));
	m_pNaviCom->Set_CurrentIndex(iNaviCellID);

	m_Scon.iNextState = IS_SKILL_QTE;
	SetUp_State();
	SetUp_Animations(false);

	m_bInputLock = false;
	m_bOnControl = true;
	m_pMoveCollider->SetActive(true);
	m_pHitCollider->SetActive(true);
	Shot_DissolveKey(true, 5.f);
	for (auto& pParts : m_Parts)
		pParts->Start_Dissolve(true, 5.f, true);
}

void CP_Chixia::Disappear_QTE(CTransform ** ppTransform, CCharacter ** ppTarget, _uint* pNaviCellID)
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

void CP_Chixia::Shot_PartsKey(_uint iParts/*int0*/, _uint iState/*int1*/, _uint iDissolve/*int2*/, _double Duration/*float*/)
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

void CP_Chixia::Shot_PriorityKey(_uint iLeavePriority)
{
	m_tCurState.iLeavePriority = iLeavePriority;
}

void CP_Chixia::Shot_EffectKey(_tchar * szEffectTag/* szTag1*/, _uint EffectBoneID /* iInt0 */, _uint iEffectTypeID, _bool bTracking/*iInt1*/)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag, Engine::EFFECT_ID(iEffectTypeID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CP_Chixia::Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID)
{
	if (MISS_END <= iMissilePoolID || EBONE_END <= iEffectBoneID)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (MISS_SKILL_03 == iMissilePoolID ||
		MISS_ATTACK_03 == iMissilePoolID)
		m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_1);
	else if (MISS_ATTACK_01 == iMissilePoolID ||
		MISS_ATTACK_02 == iMissilePoolID ||
		MISS_SKILL_01 == iMissilePoolID ||
		MISS_AIRATTACK == iMissilePoolID)
		m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_2);
	else if (MISS_AIMATTACK_S == iMissilePoolID ||
		MISS_SKILL_QTE == iMissilePoolID)
		m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_3);
	else if (MISS_BURST_1 == iMissilePoolID ||
		MISS_AIMATTACK_B == iMissilePoolID ||
		MISS_ATTACK_04 == iMissilePoolID)
		m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_4);
	else if (MISS_BURST_2 == iMissilePoolID)
		m_pCamMovement->StartVibration((_uint)CCameraMovement::SHAKE_5);


	if (iMissilePoolID == MISS_SKILL_03)
	{
		m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] -= 1.f;
		if (m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] < 0.f)
		{
			m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] = 0.f;
			return;
		}
			
	}

	m_bAttack = true;

	

	if (0/*iMissilePoolID == MISS_BURST_1*/)
	{
		pGameInstance->StartBlackWhite(0.25f);

		CRenderSetting::RGB_SPLIT_DESC Split;
		Split.m_fDistortion = 7.2f;
		Split.m_fStrength = 1.f;
		Split.m_fSeparation = 0.1f;
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
	
	_vector vTargetPos;
	if (true == m_bAiming)
	{
		_matrix matCam = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);

		vTargetPos = (matCam.r[3] + XMVector3Normalize(matCam.r[2]) * 15.f);
	}
	else
	{
		if (nullptr != m_pFixedTarget)
			vTargetPos = m_pFixedTarget->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
		else if (nullptr != m_pNearst && false == m_bHolding)
			vTargetPos = m_pNearst->Get_Position() + XMVectorSet(0.f, 1.f, 0.f, 0.f);
		else
			vTargetPos = Get_Position() + XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_LOOK)) * 10.f + XMVectorSet(0.f, 1.f, 0.f, 0.f);
	}

	m_MissilePools[iMissilePoolID]->Shot(vInitPos, m_pMainTransform->Get_State(CTransform::STATE_LOOK), matRot, vTargetPos);
}

void CP_Chixia::updateAttackDesc()
{
	const CPlayerState::ATTACK_DESC* AttackDesc = m_pPlayerStateClass->GetAttackDesc(CPlayerState::CHARACTER_CHIXIA);

	static float fOriginDamageFactor[4];
	fOriginDamageFactor[0] = m_AttackInfos[ATK_ATTACK_01].fDamageFactor;
	fOriginDamageFactor[1] = m_AttackInfos[ATK_ATTACK_02].fDamageFactor;
	fOriginDamageFactor[2] = m_AttackInfos[ATK_ATTACK_03].fDamageFactor;
	fOriginDamageFactor[3] = m_AttackInfos[ATK_ATTACK_04].fDamageFactor;

	static float fOriginBurstFactor = m_AttackInfos[MISS_BURST_2].fDamageFactor;

	m_AttackInfos[ATK_ATTACK_01].fDamageFactor = fOriginDamageFactor[0] * AttackDesc->fDamageFactor[0];
	m_AttackInfos[ATK_ATTACK_02].fDamageFactor = fOriginDamageFactor[1] * AttackDesc->fDamageFactor[1];
	m_AttackInfos[ATK_ATTACK_03].fDamageFactor = fOriginDamageFactor[2] * AttackDesc->fDamageFactor[2];
	m_AttackInfos[ATK_ATTACK_04].fDamageFactor = fOriginDamageFactor[3] * AttackDesc->fDamageFactor[3];
	m_AttackInfos[MISS_BURST_2].fDamageFactor = fOriginBurstFactor * AttackDesc->fBurstFactor;

	if (AttackDesc->bAirboneQTE)
		m_AttackInfos[ATK_SKILL_QTE].eHitIntensity = HIT_FLY;
	else
		m_AttackInfos[ATK_SKILL_QTE].eHitIntensity = HIT_BIG;
}

void CP_Chixia::SetUp_State()
{
	_uint iPrevState = m_Scon.iCurState;
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

	// PositionState 처리
	if ((SS_JUMP_WALK == m_Scon.iCurState ||
		SS_JUMP_RUN == m_Scon.iCurState ||
		SS_FIXHOOK_END_UP == m_Scon.iCurState ||
		SS_CLIMB_MOVE == m_Scon.iCurState ||
		SS_BEHIT_FLY_START == m_Scon.iCurState ||
		SS_BEHIT_PUSH == m_Scon.iCurState ||
		SS_FALL == m_Scon.iCurState) &&
		PS_AIR != m_Scon.ePositionState)
	{
		m_Scon.ePositionState = PS_AIR;
		m_Scon.bFalling = false;
		XMStoreFloat3(&m_vJumpPos, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
		m_iAirJumpCount = 1; 
	}

	if (SS_LAND_LIGHT == m_Scon.iCurState ||
		SS_LAND_HEAVY == m_Scon.iCurState ||
		SS_LAND_ROLL == m_Scon.iCurState ||
		SS_CLIMB_ONTOP == m_Scon.iCurState ||
		SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState ||
		SS_BEHIT_FLY_FALL == m_Scon.iCurState)
	{
		m_Scon.ePositionState = PS_GROUND;
		m_bAirRecentFireL = false;
	}

	if ((SS_CLIMB_START_UP == m_Scon.iCurState ||
		SS_CLIMB_START_DOWN == m_Scon.iCurState ||
		SS_CLIMB_STAND == m_Scon.iCurState ||
		SS_CLIMB_BOOST_U_START == m_Scon.iCurState) &&
		PS_CLIMB != m_Scon.ePositionState)
	{
		m_Scon.ePositionState = PS_CLIMB;
	}

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
	if (IS_SKILL_01_B == m_Scon.iCurState ||
		IS_SKILL_01_F == m_Scon.iCurState ||
		IS_SKILL_03_B == m_Scon.iCurState ||
		IS_SKILL_03_F == m_Scon.iCurState)
		m_pCharacterState->fCurCooltime[CPlayerState::COOL_SKILL] = m_pCharacterState->fMaxCooltime[CPlayerState::COOL_SKILL];
	else if (IS_BURST == m_Scon.iCurState)
		m_pCharacterState->fCurCooltime[CPlayerState::COOL_BURST] = m_pCharacterState->fMaxCooltime[CPlayerState::COOL_BURST];
	else if (SS_SUMMON == m_Scon.iCurState)
		m_pCharacterState->fCurCooltime[CPlayerState::COOL_ECHO] = m_pCharacterState->fMaxCooltime[CPlayerState::COOL_ECHO];
	else if (SS_FIXHOOK_END_UP == m_Scon.iCurState)
		m_pPlayerStateClass->Set_ToolUsed();

	// 게이지 차감
	if (IS_BURST == m_Scon.iCurState)
		m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] -= 10.f;

	if (IS_SKILL_01_B == m_Scon.iCurState ||
		IS_SKILL_01_F == m_Scon.iCurState)
		m_pMainTransform->Set_RotationSpeed(XMConvertToRadians(120.f));
	else
		m_pMainTransform->Set_RotationSpeed(XMConvertToRadians(540.f));



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

	//
	if (IS_AIMATTACK_START == m_Scon.iCurState ||
		IS_AIMATTACK_STAND == m_Scon.iCurState ||
		IS_AIMATTACK_FIRE_S == m_Scon.iCurState ||
		IS_AIMATTACK_FIRE_B == m_Scon.iCurState ||
		IS_AIMATTACK_END == m_Scon.iCurState)
	{
		m_bAiming = true;
		if(nullptr != m_pPlayerStateClass)
			m_pPlayerStateClass->Set_Aiming(m_bAiming);
		m_Scon.iNextState = SS_STAND1;
		SetUp_SubState();
		SetUp_SubAnimations(false);
	}
	else
	{
		m_bAiming = false;
		if (nullptr != m_pPlayerStateClass)
			m_pPlayerStateClass->Set_Aiming(m_bAiming);
		m_Scon.bWalk = false;
	}

	if (IS_AIRATTACK_FIRE_L == m_Scon.iCurState)
		m_bAirRecentFireL = true;
	else
		m_bAirRecentFireL = false;

	if (IS_HOLDSHOT_UPPER_LOOP_F <= m_Scon.iCurState)
	{
		m_bHolding = true;

		m_Scon.iNextState = SS_STAND1;
		SetUp_SubState();
		SetUp_SubAnimations(false);
	}
	else
	{
		m_bHolding = false;
	}

	if (IS_HOLDSHOT_UPPER_END == iPrevState)
		m_tCurState.bLerp = false;

}

void CP_Chixia::SetUp_SubState()
{
	m_iSubState = m_Scon.iNextState;

	if (true == m_bHolding)
	{
		if (IS_HOLDSHOT_LOWER_LOOP == m_iSubState)
		{
			if (m_Scon.iCurState != IS_HOLDSHOT_UPPER_LOOP_B)
			{
				m_Scon.iNextState = IS_HOLDSHOT_UPPER_LOOP_B;
				SetUp_State();
				SetUp_Animations(false);
			}
		}
		else if (SS_RUN_B == m_iSubState ||
			SS_RUN_LB == m_iSubState ||
			SS_RUN_RB == m_iSubState ||
			SS_WALK_LB == m_iSubState ||
			SS_WALK_RB == m_iSubState ||
			SS_WALK_B == m_iSubState)
		{
			if (m_Scon.iCurState != IS_HOLDSHOT_UPPER_LOOP_B)
			{
				m_Scon.iNextState = IS_HOLDSHOT_UPPER_LOOP_B;
				SetUp_State();
				SetUp_Animations(false);
			}
		}
		else if (SS_RUN_F == m_iSubState ||
			SS_RUN_LF == m_iSubState ||
			SS_RUN_RF == m_iSubState ||
			SS_WALK_LF == m_iSubState ||
			SS_WALK_RF == m_iSubState ||
			SS_WALK_F == m_iSubState)
		{
			if (m_Scon.iCurState != IS_HOLDSHOT_UPPER_LOOP_F)
			{
				m_Scon.iNextState = IS_HOLDSHOT_UPPER_LOOP_F;
				SetUp_State();
				SetUp_Animations(false);
			}
		}
	
	}

	m_tSubState = m_tStates[m_iSubState];

	m_SubTrackPos = 0.0;
	m_SubAnimFinished = false;

	if (true == m_bAiming)
	{
		m_Scon.bWalk = true;
		m_bInputDirMove = true;

		if (SS_STAND1 == m_iSubState)
		{
			m_tSubState.iAnimID[ANIMSET_BASE] = 173;
			m_tSubState.iAnimID[ANIMSET_RIBBON] = 175;
			m_tSubState.iPhysicMoveID = 0;
			m_Scon.vMovement = _float3(0.f, 0.f, 0.f);
			m_Scon.vPrevMovement = _float3(0.f, 0.f, 0.f);
		}
		else
			m_tSubState.iPhysicMoveID = 1;
	}

	//PhysicMove
	if (false == m_tSubState.bRootMotion)
	{
		PHYSICMOVE tPhysicMove = StatePhysics[m_tSubState.iPhysicMoveID];

		if (true == tPhysicMove.bInitMovement)
		{
			XMStoreFloat3(&m_Scon.vMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
			XMStoreFloat3(&m_Scon.vPrevMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
		}
	}
}

void CP_Chixia::SetUp_SubAnimations(_bool bContinue)
{
	m_pAnimSetCom[ANIMSET_BASE]->SetUp_SubAnimation(m_tSubState.iAnimID[ANIMSET_BASE], m_tSubState.bLerp, bContinue);
	m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_SubAnimation(m_tSubState.iAnimID[ANIMSET_RIBBON], m_tSubState.bLerp, bContinue);
}

HRESULT CP_Chixia::SetUp_ShaderResources()
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

HRESULT CP_Chixia::Setup_ShadowShaderResource()
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

void CP_Chixia::Set_WeaponUse(_bool bBool)
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

void CP_Chixia::Apply_CoolTime(_double TimeDelta)
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

	if (true == m_bAiming)
		m_AimChargeAcc += TimeDelta;
	else
		m_AimChargeAcc = 0.0;
}

void CP_Chixia::Key_Input(_double TimeDelta)
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

		if (pGame->InputKey(DIK_E) == KEY_STATE::AWAY)
		{
			if(m_SkillChargeAcc < 0.5)
				eCurFrameInput = INPUT_SKILL;
		}

		if (pGame->InputKey(DIK_E) == KEY_STATE::HOLD)
		{
			m_SkillChargeAcc += TimeDelta;
			if (m_SkillChargeAcc > 0.5)
				eCurFrameInput = INPUT_SKILL_CHARGE;
		}
		else
			m_SkillChargeAcc = 0.0;

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


		if (pGame->InputMouse(DIMK_RB) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_AIM;
		}

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

			m_pEchoSystem->Shot_Echo(CEchoSystem::CHIXIA, m_pMainTransform, pTarget, m_pNaviCom->Get_CurrentIndex());
		}

		if (pGame->InputKey(DIK_1) == KEY_STATE::TAP)
		{
			if (PS_GROUND == m_Scon.ePositionState &&
				7 > m_tCurState.iLeavePriority)
				m_pPlayerStateClass->Change_ActiveCharacter(CPlayerState::SLOT_SUB1);
		}

		if (pGame->InputKey(DIK_2) == KEY_STATE::TAP)
		{
			if (PS_GROUND == m_Scon.ePositionState &&
				7 > m_tCurState.iLeavePriority)
				m_pPlayerStateClass->Change_ActiveCharacter(CPlayerState::SLOT_SUB2);
		}
	}
	// Echo Summon
	if (pGame->InputKey(DIK_NUMPAD6) == KEY_STATE::TAP)
	{
		eCurFrameInput = INPUT_AIM;
	}

	// 타겟 방향
	_vector vTargetDir;
	if (true == m_bAiming)
	{
		vTargetDir = XMVector3TransformNormal(vCamLook, XMMatrixRotationY(XMConvertToRadians(22.5f)));
	}
	else if (nullptr != m_pFixedTarget)
		vTargetDir = XMVector3Normalize(XMVectorSetY(m_pFixedTarget->Get_Position() - this->Get_Position(), 0.f));
	else if (nullptr != m_pNearst)
		vTargetDir = XMVector3Normalize(XMVectorSetY(m_pNearst->Get_Position() - this->Get_Position(), 0.f));
	else
		vTargetDir = XMVectorZero();

	// 입력 방향
	if (0.f != XMVectorGetX(XMVector3Length(vInputDir)))
		vInputDir = XMVector3Normalize(vInputDir);

	// 임시 이펙트 재생용
	CEffect* pEffect = nullptr;
	_float4x4 ParentMatrix;

	// 입력에 따라 대응하는 다음 상태 찾기
	if (PS_GROUND == m_Scon.ePositionState)
	{
		switch (eCurFrameInput)
		{
		case Client::CP_Chixia::INPUT_NONE:
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

			default:
				m_Scon.iNextState = SS_STAND1;
				break;
			}
			break;

		case Client::CP_Chixia::INPUT_MOVE:

			if (nullptr != m_pFixedTarget || true == m_bAiming)
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
						if (fScalar >= -0.1f)
							m_Scon.iNextState = SS_WALK_LF;
						else
							m_Scon.iNextState = SS_WALK_LB;
					}
					else if (bLeft == false)
					{
						if (fScalar >= -0.1f)
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
						if (fScalar >= -0.1f)
							m_Scon.iNextState = SS_RUN_LF;
						else
							m_Scon.iNextState = SS_RUN_LB;
					}
					else if (bLeft == false)
					{
						if (fScalar >= -0.1f)
							m_Scon.iNextState = SS_RUN_RF;
						else
							m_Scon.iNextState = SS_RUN_RB;
					}
				}
				if (m_tCurState.iLeavePriority == 0 || true == m_bAiming || true == m_bHolding)
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

		case Client::CP_Chixia::INPUT_DASH:
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
		case Client::CP_Chixia::INPUT_FRONTSTEP:
			if (m_Scon.iCurState != SS_MOVE_B)
				m_Scon.iNextState = SS_MOVE_F;
			break;

		case Client::CP_Chixia::INPUT_BACKSTEP:
			if (m_Scon.iCurState != SS_MOVE_F)
				m_Scon.iNextState = SS_MOVE_B;
			break;
		case Client::CP_Chixia::INPUT_SPACE:
			if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
				m_Scon.iNextState = SS_JUMP_WALK;
			else
				m_Scon.iNextState = SS_JUMP_RUN;
			break;

		case Client::CP_Chixia::INPUT_ATTACK:
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
				m_Scon.iNextState = IS_ATTACK_04;
				break;
			case IS_AIMATTACK_START:
			case IS_AIMATTACK_STAND:
				if (m_AimChargeAcc > 1.25)
					m_Scon.iNextState = IS_AIMATTACK_FIRE_B;
				else
					m_Scon.iNextState = IS_AIMATTACK_FIRE_S;
				break;
			default:
				m_Scon.iNextState = IS_ATTACK_01;
				break;
			}
			break;

		case Client::CP_Chixia::INPUT_AIM:
			if(false == m_bAiming)
				m_Scon.iNextState = IS_AIMATTACK_START;
			break;

		case Client::CP_Chixia::INPUT_SKILL:
			if (IS_HOLDSHOT_UPPER_LOOP_F <= m_Scon.iCurState)
			{
				m_Scon.iNextState = IS_SKILL_03_Deprive;
			}
			else
			{
				if (0.f == m_pCharacterState->fCurCooltime[CPlayerState::COOL_SKILL])
				{
					if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
						m_Scon.iNextState = IS_SKILL_01_B;
					else
						m_Scon.iNextState = IS_SKILL_01_F;
				}
			}
			break;

		case Client::CP_Chixia::INPUT_SKILL_CHARGE:
			if (0.f == m_pCharacterState->fCurCooltime[CPlayerState::COOL_SKILL])
			{
				if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
					m_Scon.iNextState = IS_SKILL_03_B;
				else
					m_Scon.iNextState = IS_SKILL_03_F;
			}
			break;

		case Client::CP_Chixia::INPUT_BURST:
			/*if (0.0 == m_pCharacterState->fCurCooltime[CPlayerState::COOL_BURST] &&
				m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST] == m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST])*/
				m_Scon.iNextState = IS_BURST;
			break;

		case Client::CP_Chixia::INPUT_TOOL:
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
		case Client::CP_Chixia::INPUT_SPACE:
			if (m_iAirJumpCount > 0)
			{
				if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
					m_Scon.iNextState = SS_JUMP_SECOND_B;
				else
					m_Scon.iNextState = SS_JUMP_SECOND_F;
			}
			break;
		case Client::CP_Chixia::INPUT_ATTACK:
			if(false == m_bAirRecentFireL)
				m_Scon.iNextState = IS_AIRATTACK_FIRE_L;
			else
				m_Scon.iNextState = IS_AIRATTACK_FIRE_R;
			break;
		case Client::CP_Chixia::INPUT_TOOL:
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
		case Client::CP_Chixia::INPUT_NONE:
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

		case Client::CP_Chixia::INPUT_MOVE:
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
		case Client::CP_Chixia::INPUT_DASH:
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
		case Client::CP_Chixia::INPUT_SPACE:
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

	// 조준 공격 중일 경우
	if (true == m_bAiming)
	{
		// 45 도 기준 블렌딩
		if (0.f <= XMVectorGetY(matCam.r[2]))
			m_bAimingUp = true;
		else
			m_bAimingUp = false;

		if (0.03f > fabs(XMVectorGetY(XMVector3Normalize(matCam.r[2]))))
			m_fYAimingAngle = 0.f;
		else
			m_fYAimingAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCamLook), XMVector3Normalize(matCam.r[2]))));

		vFinalDir = vTargetDir;
	}
	// 공격 행동일 경우
	else if (eCurFrameInput >= INPUT_ATTACK)
	{
		// 고정 타겟이 있거나 가장 가까운 몬스터와의 거리가 10 미만일 때 타겟 방향
		if ((nullptr != m_pFixedTarget) ||
			nullptr != m_pNearst)
			vFinalDir = vTargetDir;
		// 그 외 입력 방향
		else
			vFinalDir = vInputDir;
	}
	else if (m_bInputDirMove || IS_SKILL_01_B == m_Scon.iCurState || IS_SKILL_01_F == m_Scon.iCurState)
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

			if (true == m_bAiming)
			{
				m_pMainTransform->Set_LookDir(vCamLook);
			}

			//궁극기 카메라 모션 실행
			if (IS_BURST == m_Scon.iCurState)
				m_pCamMovement->UseCamera(CCameraMovement::CAM_CHIXIA);
		}
		else if ((true == m_bAiming || true == m_bHolding) && INPUT_MOVE >= eCurFrameInput)
		{
			if (INPUT_NONE == eCurFrameInput && true == m_bHolding)
			{
				m_Scon.iNextState = IS_HOLDSHOT_LOWER_LOOP;
			}

			if (m_iSubState != m_Scon.iNextState)
			{
				if (IS_HOLDSHOT_LOWER_END != m_iSubState)
				{
					SetUp_SubState();
					SetUp_SubAnimations(false);
				}
			}
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

void CP_Chixia::Tick_State(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (false == m_Scon.bAnimFinished)
	{
		_float4 vRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float3 vMovement;
		_double ProgressRatio;
		if (true == m_bAiming)
		{
			_double SubProgressRatio;
			// 포즈 잡기
			// Pose 상태를 Animset-DefaultMatrix에 세팅
			// D : 172, F : 173, U : 178
			if (XMConvertToRadians(45.f) <= fabs(m_fYAimingAngle))
			{
				if (true == m_bAimingUp)
					m_pAnimSetCom[ANIMSET_BASE]->Set_PoseAnim(178);
				else
					m_pAnimSetCom[ANIMSET_BASE]->Set_PoseAnim(172);
			}
			else
			{
				m_pAnimSetCom[ANIMSET_BASE]->Set_PoseAnim(173);

				if (true == m_bAimingUp)
					m_pAnimSetCom[ANIMSET_BASE]->Blend_PoseAnim(178, fabs(m_fYAimingAngle) / XMConvertToRadians(45.f));
				else
					m_pAnimSetCom[ANIMSET_BASE]->Blend_PoseAnim(172, fabs(m_fYAimingAngle) / XMConvertToRadians(45.f));
			}

			if (m_Scon.iCurState != IS_AIMATTACK_STAND)
			{
				// BASE - 상체
				m_pAnimSetCom[ANIMSET_BASE]->Play_Animation_Blending(TimeDelta, nullptr, nullptr, &m_Scon.TrackPos, &m_Scon.bAnimFinished, &ProgressRatio);
				m_pAnimSetCom[ANIMSET_BASE]->Update_TargetUpperBones();

				// RIB - 상체
				m_pAnimSetCom[ANIMSET_RIBBON]->Update_RibbonAnimation(ProgressRatio);
				m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetUpperBones();
			}
			else 
				m_pAnimSetCom[ANIMSET_BASE]->Update_TargetUpperBones();
			
			if (m_tSubState.iAnimID[ANIMSET_BASE] != 173)
			{
				// BASE - 하체
				m_pAnimSetCom[ANIMSET_BASE]->Play_SubAnimation(TimeDelta, &vRotation, &vMovement, &m_SubTrackPos, &m_SubAnimFinished, &SubProgressRatio);
				m_pAnimSetCom[ANIMSET_BASE]->Update_TargetLowerBones();

				// RIB - 하체
				m_pAnimSetCom[ANIMSET_RIBBON]->Update_SubRibbonAnimation(SubProgressRatio);
				m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetLowerBones();
			}
			else
				m_pAnimSetCom[ANIMSET_BASE]->Update_TargetLowerBones();

			m_pModelCom->Invalidate_CombinedMatrices_Split();
		}
		else if (true == m_bHolding)
		{
			_double SubProgressRatio;
			// 포즈 잡기
			m_pAnimSetCom[ANIMSET_BASE]->Set_PoseAnim(201);
		
			// BASE - 상체
			m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, nullptr, nullptr, &m_Scon.TrackPos, &m_Scon.bAnimFinished, &ProgressRatio);
			m_pAnimSetCom[ANIMSET_BASE]->Update_TargetUpperBones();

			// RIB - 상체
			m_pAnimSetCom[ANIMSET_RIBBON]->Update_RibbonAnimation(ProgressRatio);
			m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetUpperBones();
		
			// BASE - 하체
			if (IS_HOLDSHOT_LOWER_LOOP == m_iSubState ||
				IS_HOLDSHOT_LOWER_END == m_iSubState)
			{
				m_pAnimSetCom[ANIMSET_BASE]->Play_SubAnimation_Blending(TimeDelta, &vRotation, &vMovement, &m_SubTrackPos, &m_SubAnimFinished, &SubProgressRatio);
			}
			else
				m_pAnimSetCom[ANIMSET_BASE]->Play_SubAnimation(TimeDelta, &vRotation, &vMovement, &m_SubTrackPos, &m_SubAnimFinished, &SubProgressRatio);
			m_pAnimSetCom[ANIMSET_BASE]->Update_TargetLowerBones();

			// RIB - 하체
			m_pAnimSetCom[ANIMSET_RIBBON]->Update_SubRibbonAnimation(SubProgressRatio);
			m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetLowerBones();
		
				

			m_pModelCom->Invalidate_CombinedMatrices_Split();
		}
		else if(IS_AIRATTACK_FIRE_L == m_Scon.iCurState ||
			IS_AIRATTACK_FIRE_R == m_Scon.iCurState)
		{
			m_pAnimSetCom[ANIMSET_BASE]->Set_PoseAnim(189);

			m_pAnimSetCom[ANIMSET_BASE]->Play_Animation_Blending(TimeDelta, nullptr, nullptr, &m_Scon.TrackPos, &m_Scon.bAnimFinished, &ProgressRatio);
			m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();

			m_pAnimSetCom[ANIMSET_RIBBON]->Update_RibbonAnimation(ProgressRatio);
			m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();

			m_pModelCom->Invalidate_CombinedMatrices_Split();
		}
		else
		{
			// 애니메이션 갱신
			m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_Scon.TrackPos, &m_Scon.bAnimFinished, &ProgressRatio);
			m_pAnimSetCom[ANIMSET_RIBBON]->Update_RibbonAnimation(ProgressRatio);

			m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();

			m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();
			// 여기까지 Playanimation

			m_pModelCom->Invalidate_CombinedMatrices();
		}
		

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
				PHYSICMOVE PhysicMove;

				if(true == m_bAiming || true == m_bHolding)
					PhysicMove = StatePhysics[m_tSubState.iPhysicMoveID];
				else
					PhysicMove = StatePhysics[m_tCurState.iPhysicMoveID];


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

	if (true == m_SubAnimFinished)
	{
		if (true == m_tSubState.bLoop)
		{
			m_Scon.iNextState = m_Scon.iCurState;
			SetUp_SubState();
			m_tSubState.bLerp = false;
			SetUp_SubAnimations(true);

		}
		else
		{
			if (NO_ANIM != m_tSubState.iNextState)
			{
				m_Scon.iNextState = m_tSubState.iNextState;
				SetUp_SubState();
				SetUp_SubAnimations(true);
			}
		}
	}
}

void CP_Chixia::On_Cell()
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
					if (SS_BEHIT_FLY_START == m_Scon.iCurState ||
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
						{
							m_pCamMovement->StartWave(2);
							m_Scon.iNextState = SS_LAND_ROLL;
						}
						else if (fGap > 2.f)
						{
							m_pCamMovement->StartWave(1);
							m_Scon.iNextState = SS_LAND_HEAVY;
						}
						else
						{
							m_pCamMovement->StartWave(0);
							m_Scon.iNextState = SS_LAND_LIGHT;
						}
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

void CP_Chixia::On_Hit(CCharacter * pGameObject, TAGATTACK * pAttackInfo, _float fAttackPoint, _float3 * pEffPos, _float fCritRate, _float fCritDMG)
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


	CGameInstance* pGI = CGameInstance::GetInstance();

	static_cast<CCharacter*>(pGameObject)->Set_AttackHit(true);
	Play_HitSound(pAttackInfo);

	// 그 외 피격 처리
	// 피격 이펙트 출력
	if (lstrcmp(pAttackInfo->szHitEffectTag, TEXT("")))
	{
		
		_float4x4 EffectMatrix = m_pMainTransform->Get_WorldMatrix();
		memcpy(EffectMatrix.m[3], pEffPos, sizeof(_float3));
		pGI->Get_Effect(pAttackInfo->szHitEffectTag, (EFFECT_ID)pAttackInfo->iHitEffectID)->Play_Effect(&EffectMatrix);
	}

	// 대미지 계산 공식 : 모션 계수 * 공격력 * ((공격력 * 2 - 방어력) / 공격력) * (속성 보너스)
	// 공격력과 방어력이 같을 때 1배 대미지
	_float fFinalDamage = pAttackInfo->fDamageFactor * fAttackPoint *
		((fAttackPoint * 2 - m_pCharacterState->fDefense[CPlayerState::STAT_TOTAL]) / fAttackPoint);
	
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

void CP_Chixia::On_Dodge()
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

void CP_Chixia::Init_AnimSystem()
{
	for (auto& pBone : m_pAnimSetCom[ANIMSET_BASE]->Get_Bones())
		pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));


	for (auto& pAnim : m_pAnimSetCom[ANIMSET_BASE]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		if (!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Z_HoldShot_Upper_Loop_F) HoldShot_Upper_Loop_F")) ||
			!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Z_HoldShot_Upper_Loop_B) HoldShot_Upper_Loop_B")))
		{
			pAnim->Set_Duration(20.0);
		}

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetCom[ANIMSET_BASE]->Get_BonePtr(pChannel->Get_TargetBoneID());

			if (wcsncmp(szChannelName, TEXT("Bip001"), 6) &&
				lstrcmp(szChannelName, TEXT("WeaponProp01")) &&
				lstrcmp(szChannelName, TEXT("WeaponProp02")) &&
				wcsncmp(szChannelName, TEXT("Root"), 4))
			{
				pChannel->Set_Apply(false);
			}
				


			if (!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(X_AimShot_Attack) AimShot_Attack")) ||
				!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(X_AimPose_F_Start) AimPose_F_Start")) || 
				!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Y_AirAttack01_Fl) AirAttack01_Fl")) || 
				!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Y_AirAttack01_FR) AirAttack01_FR")))
			{
				if (!lstrcmp(szChannelName, TEXT("Bip001LForearm")) ||
					pBone->Is_ChildOf(TEXT("Bip001LForearm")))
				{
					pChannel->Set_Apply(false);
				}
			}

		
			if (!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Z_HoldShot_Lower_End) HoldShot_Lower_End")) ||
				!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Z_HoldShot_Lower_Stand) HoldShot_Lower_Stand")))
			{
				if (!lstrcmp(szChannelName, TEXT("Bip001_L_CalfTwist")) ||
					//pBone->Is_ChildOf(TEXT("Bip001_L_Calf")) ||
					!lstrcmp(szChannelName, TEXT("Bip001_R_CalfTwist")))
					//pBone->Is_ChildOf(TEXT("Bip001_R_Calf")))
				{
					for (auto& KeyFrame : pChannel->Get_KeyFrames())
					{
						XMStoreFloat4(&KeyFrame.vRotation, XMQuaternionInverse(XMLoadFloat4(&KeyFrame.vRotation)));
					}
					//pChannel->Set_Apply(false);
				}
			}

			if (!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Z_HoldShot_Upper_Loop_F) HoldShot_Upper_Loop_F")) || 
				!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Z_HoldShot_Upper_Loop_B) HoldShot_Upper_Loop_B")) || 
				!lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Skill03_Derive) Skill03_Derive")))
			{
				if (!lstrcmp(szChannelName, TEXT("WeaponProp01")) ||
					!lstrcmp(szChannelName, TEXT("WeaponProp02")))
				{
					pChannel->Set_Apply(false);
				}
			}
				
			if (true == pBone->Is_ChildOf(TEXT("Bip001Head")))
			{
				if (lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Attack01) Attack01")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Attack02) Attack02")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Attack03) Attack03")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Attack04) Attack04")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Burst01) Burst01")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Skill01) Skill01")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Skill01_B) Skill01_B")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Skill03_B) Skill03_B")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Skill03_Derive) Skill03_Derive")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(SkillQTE) SkillQTE")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Stand1_Action01) Stand1_Action01")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(Stand2) Stand2")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(StandChange) StandChange")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(AimPose_D) AimPose_D")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(AimPose_F) AimPose_F")) &&
					lstrcmp(szAnimName, TEXT("R2T1MaxiaofangMd10011.ao|(AimPose_U) AimPose_U")))
				{
					pChannel->Set_Apply(false);
				}
			}
		}
	}

	// Ribbon
	for (auto& pBone : m_pAnimSetCom[ANIMSET_RIBBON]->Get_Bones())
	{
		const _tchar* szBonename = pBone->Get_Name();
		CBone* pTargetBone = m_pModelCom->Get_BonePtr(szBonename);
		pBone->Set_TargetBone(pTargetBone);


		if (!lstrcmp(szBonename, TEXT("Bone_Skirt001_L")) ||
			!lstrcmp(szBonename, TEXT("Bone_Skirt007_R")) ||
			!lstrcmp(szBonename, TEXT("Bone_Skirt011_L")) ||
			!lstrcmp(szBonename, TEXT("Bone_Skirt017_R")) ||
			!lstrcmp(szBonename, TEXT("Bone_Skirt022_M")) ||
			!lstrcmp(szBonename, TEXT("Bone_Piao001_M")) ||
			!lstrcmp(szBonename, TEXT("Bone_Piao011_R")))
		{
			_matrix matTemp = XMLoadFloat4x4(&pTargetBone->Get_DefaultTransformationMatrix());
			_vector vAxis = matTemp.r[1];
			matTemp = matTemp * XMMatrixRotationAxis(vAxis, XMConvertToRadians(-7.f));
			matTemp.r[3] *= 1.15f;
			matTemp.r[3] = XMVectorSetW(matTemp.r[3], 1.f);
			pTargetBone->Set_DefaultMatrix(matTemp);
		}
	}


	for (auto& pAnim : m_pAnimSetCom[ANIMSET_RIBBON]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetCom[ANIMSET_RIBBON]->Get_BonePtr(pChannel->Get_TargetBoneID());

			if (pChannel->Get_NumKeyFrames() <= 2)
			{
				pChannel->Set_Apply(false);
				continue;
			}


			if (!(true == pBone->Is_ChildOf(TEXT("Bone_Skirt001_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Skirt007_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Skirt011_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Skirt017_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Skirt022_M")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Piao001_M")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Piao011_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair001_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair005_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair008_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair011_M")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair014_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair018_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair028_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair032_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair036_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair040_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Hair044_R")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Piao025_L")) ||
				true == pBone->Is_ChildOf(TEXT("Bone_Piao032_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Skirt001_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Skirt007_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Skirt011_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Skirt017_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Skirt022_M")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Piao001_M")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Piao011_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair001_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair005_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair008_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair011_M")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair014_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair018_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair028_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair032_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair036_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair040_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Hair044_R")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Piao025_L")) ||
				!lstrcmp(szChannelName, TEXT("Bone_Piao032_R"))))
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

void CP_Chixia::Init_AttackInfos()
{
	for (_uint i = 0; i < ATK_END; ++i)
	{
		ZeroMemory(&m_AttackInfos[i], sizeof TAGATTACK);
	}

	m_AttackInfos[ATK_ATTACK_01].fDamageFactor = 0.85f;
	m_AttackInfos[ATK_ATTACK_01].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_01].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK_01].fSPGain = 1.f;
	m_AttackInfos[ATK_ATTACK_01].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_01].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_01].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_ATTACK_01].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_01].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_1.wem.wav"));

	m_AttackInfos[ATK_ATTACK_02].fDamageFactor = 0.6f;
	m_AttackInfos[ATK_ATTACK_02].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_02].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK_02].fSPGain = 1.f;
	m_AttackInfos[ATK_ATTACK_02].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_02].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_02].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_ATTACK_02].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_02].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_2.wem.wav"));

	m_AttackInfos[ATK_ATTACK_03].fDamageFactor = 0.45f;
	m_AttackInfos[ATK_ATTACK_03].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_03].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK_03].fSPGain = 1.f;
	m_AttackInfos[ATK_ATTACK_03].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_03].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_03].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_ATTACK_03].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_03].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_3.wem.wav"));

	m_AttackInfos[ATK_ATTACK_04].fDamageFactor = 2.1f;
	m_AttackInfos[ATK_ATTACK_04].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK_04].eElementType = ELMT_FUSION;
	m_AttackInfos[ATK_ATTACK_04].fSPGain = 5.f;
	m_AttackInfos[ATK_ATTACK_04].fBPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04].fTPGain = 1.5f;
	m_AttackInfos[ATK_ATTACK_04].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_ATTACK_04].szHitEffectTag, TEXT("Chixia_Hit_Effect_B"));
	lstrcpy(m_AttackInfos[ATK_ATTACK_04].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_3.wem.wav"));

	m_AttackInfos[ATK_SKILL_01].fDamageFactor = 0.8f;
	m_AttackInfos[ATK_SKILL_01].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_SKILL_01].eElementType = ELMT_FUSION;
	m_AttackInfos[ATK_SKILL_01].fSPGain = 2.f;
	m_AttackInfos[ATK_SKILL_01].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_01].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_SKILL_01].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_SKILL_01].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_4.wem.wav"));

	m_AttackInfos[ATK_SKILL_QTE].fDamageFactor = 0.8f;
	m_AttackInfos[ATK_SKILL_QTE].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_SKILL_QTE].eElementType = ELMT_FUSION;
	m_AttackInfos[ATK_SKILL_QTE].fSPGain = 2.f;
	m_AttackInfos[ATK_SKILL_QTE].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_QTE].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_QTE].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_SKILL_QTE].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_SKILL_QTE].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_2.wem.wav"));

	m_AttackInfos[ATK_BURST_1].fDamageFactor = 1.f;
	m_AttackInfos[ATK_BURST_1].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_BURST_1].eElementType = ELMT_FUSION;
	m_AttackInfos[ATK_BURST_1].fSPGain = 1.f;
	m_AttackInfos[ATK_BURST_1].fBPGain = 1.5f;
	m_AttackInfos[ATK_BURST_1].fTPGain = 1.5f;
	m_AttackInfos[ATK_BURST_1].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_BURST_1].szHitEffectTag, TEXT("Chixia_Hit_Effect_B"));
	lstrcpy(m_AttackInfos[ATK_BURST_1].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_5.wem.wav"));

	m_AttackInfos[ATK_BURST_2].fDamageFactor = 2.9f;
	m_AttackInfos[ATK_BURST_2].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_BURST_2].eElementType = ELMT_FUSION;
	m_AttackInfos[ATK_BURST_2].fSPGain = 10.f;
	m_AttackInfos[ATK_BURST_2].fBPGain = 1.5f;
	m_AttackInfos[ATK_BURST_2].fTPGain = 1.5f;
	m_AttackInfos[ATK_BURST_2].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_BURST_2].szHitEffectTag, TEXT("Chixia_Hit_Effect_B"));
	lstrcpy(m_AttackInfos[ATK_BURST_2].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_5.wem.wav"));

	m_AttackInfos[ATK_AIMATTACK_S].fDamageFactor = 1.2f;
	m_AttackInfos[ATK_AIMATTACK_S].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_AIMATTACK_S].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_AIMATTACK_S].fSPGain = 1.f;
	m_AttackInfos[ATK_AIMATTACK_S].fBPGain = 1.5f;
	m_AttackInfos[ATK_AIMATTACK_S].fTPGain = 1.5f;
	m_AttackInfos[ATK_AIMATTACK_S].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_AIMATTACK_S].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_AIMATTACK_S].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_2.wem.wav"));


	m_AttackInfos[ATK_AIMATTACK_B].fDamageFactor = 2.5f;
	m_AttackInfos[ATK_AIMATTACK_B].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_AIMATTACK_B].eElementType = ELMT_FUSION;
	m_AttackInfos[ATK_AIMATTACK_B].fSPGain = 5.f;
	m_AttackInfos[ATK_AIMATTACK_B].fBPGain = 1.5f;
	m_AttackInfos[ATK_AIMATTACK_B].fTPGain = 1.5f;
	m_AttackInfos[ATK_AIMATTACK_B].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_AIMATTACK_B].szHitEffectTag, TEXT("Chixia_Hit_Effect_SC"));
	lstrcpy(m_AttackInfos[ATK_AIMATTACK_B].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_3.wem.wav"));

	m_AttackInfos[ATK_AIRATTACK].fDamageFactor = 600.6f;
	m_AttackInfos[ATK_AIRATTACK].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_AIRATTACK].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_AIRATTACK].fSPGain = 1.f;
	m_AttackInfos[ATK_AIRATTACK].fBPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK].fTPGain = 1.5f;
	m_AttackInfos[ATK_AIRATTACK].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_AIRATTACK].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_AIRATTACK].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_1.wem.wav"));

	m_AttackInfos[ATK_SKILL_03].fDamageFactor = 0.33f;
	m_AttackInfos[ATK_SKILL_03].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_SKILL_03].eElementType = ELMT_FUSION;
	m_AttackInfos[ATK_SKILL_03].fSPGain = 0.f;
	m_AttackInfos[ATK_SKILL_03].fBPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_03].fTPGain = 1.5f;
	m_AttackInfos[ATK_SKILL_03].iHitEffectID = EFFECT_ID::PLAYER_CHIXIA;
	lstrcpy(m_AttackInfos[ATK_SKILL_03].szHitEffectTag, TEXT("Chixia_Hit_Effect_S"));
	lstrcpy(m_AttackInfos[ATK_SKILL_03].szHitSoundTag, TEXT("DA_Au_Role_Common_Imp_Bullet_4.wem.wav"));

}

void CP_Chixia::Init_Missiles()
{
	CMissilePool::MISSILEPOOLDESC tMissilePoolDesc;
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));


	// ATTACK_01
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));
	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Attack_01_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 5;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Bullet"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK_01;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 3.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK_01] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK_01] = _float3(0.f, 0.f, 0.f);

	// MISS_ATTACK_02
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Attack_02_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 5;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Bullet"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK_02;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK_02] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK_02] = _float3(0.f, 0.f, 0.f);

	// MISS_ATTACK_03
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Attack_03_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 5;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Bullet"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK_03;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK_03] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK_03] = _float3(0.f, 0.f, 0.f);

	// MISS_ATTACK_04
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Attack_04_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 5;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Skill_Bullet_B"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK_04;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK_04] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK_04] = _float3(0.f, 0.f, 0.f);

	// MISS_SKILL_01
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Skill_01_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 10;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Skill_Bullet_S"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_SKILL_01;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_SKILL_01] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_SKILL_01] = _float3(0.f, 0.f, 0.f);

	// MISS_SKILL_QTE
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Skill_QTE_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 10;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Bullet"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_SKILL_QTE;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_SKILL_QTE] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_SKILL_QTE] = _float3(0.f, 0.f, 0.f);

	// MISS_BURST_1
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Burst_1_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 20;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.15;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_BURST_1;
	tMissilePoolDesc.tMissileDesc.fExtents = 6.f;

	m_MissilePools[MISS_BURST_1] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_BURST_1] = _float3(0.f, 0.f, 0.f);

	// MISS_BURST_2
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Burst_2_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 1;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.15;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_BURST_2;
	tMissilePoolDesc.tMissileDesc.fExtents = 8.f;

	m_MissilePools[MISS_BURST_2] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_BURST_2] = _float3(0.f, 0.f, 0.f);

	// MISS_AIMATTACK_S
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_AimAttack_S_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 10;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Bullet"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 2.0;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_AIMATTACK_S;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_AIMATTACK_S] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_AIMATTACK_S] = _float3(0.f, 0.f, 0.f);

	// MISS_AIMATTACK_B
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_AimAttack_B_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 10;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Skill_Bullet_B"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 3.0;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_AIMATTACK_B;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.4f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 60.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_AIMATTACK_B] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_AIMATTACK_B] = _float3(0.f, 0.f, 0.f);

	// MISS_AIRATTACK
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_AirAttack_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 10;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Bullet"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_AIRATTACK;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_AIRATTACK] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_AIRATTACK] = _float3(0.f, 0.f, 0.f);

	// MISS_SKILL_03
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Chixia_Skill_03_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 30;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("Skill_Bullet_S"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = EFFECT_ID::PLAYER_CHIXIA;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.5;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_SKILL_03;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.2f;
	tMissilePoolDesc.tMissileDesc.bDeleteOnHit = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 75.f;
	tMissilePoolDesc.StopTime = 1.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_SKILL_03] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_SKILL_03] = _float3(0.f, 0.f, 0.f);


}

HRESULT CP_Chixia::Init_Parts()
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

	m_Parts[PARTS_WEAPON_MAIN] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_GUN_5, &PartsDesc));
	m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(EBONE_WEAPON02);
	m_Parts[PARTS_WEAPON_MAIN]->Set_DissolveColor(_float3(0.3f, 0.6f, 1.f));

	m_Parts[PARTS_WEAPON_SUB] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_GUN_5, &PartsDesc));
	m_Parts[PARTS_WEAPON_SUB]->Set_Parent(EBONE_WEAPON01);
	m_Parts[PARTS_WEAPON_SUB]->Set_DissolveColor(_float3(0.3f, 0.6f, 1.f));

	m_Parts[PARTS_HULU] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_HULU_3, &PartsDesc));
	m_Parts[PARTS_HULU]->Set_Parent(PBONE_HULU);
	m_Parts[PARTS_HULU]->Set_DissolveColor(_float3(0.3f, 0.3f, 1.f));

	return S_OK;
}

HRESULT CP_Chixia::Init_EffectBones()
{
	//NONE은 걍 월드 매트릭스 던짐
	m_EffectBones[EBONE_SPINE2] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine2"));
	m_EffectBones[EBONE_WEAPON01] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp01"));
	m_EffectBones[EBONE_WEAPON02] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp02"));
	m_EffectBones[EBONE_LHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001LHand"));
	m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));

	return S_OK;
}

void CP_Chixia::Update_EffectBones()
{
	memcpy(&m_EffectBoneMatrices[EBONE_NONE], &m_pMainTransform->Get_WorldMatrix(), sizeof(_float4x4));

	for (_uint i = 1; i < EBONE_END; ++i)
	{
		XMStoreFloat4x4(&m_EffectBoneMatrices[i], XMLoadFloat4x4(&m_EffectBones[i]->Get_CombinedTransfromationMatrix())
			* XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
	}
}

CP_Chixia * CP_Chixia::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CP_Chixia* pInstance = new CP_Chixia(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CP_Chixia");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CP_Chixia::Clone(void * pArg)
{
	CP_Chixia* pInstance = new CP_Chixia(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CP_Chixia");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CP_Chixia::Free()
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

	Safe_Release(m_pHitCollider);
	Safe_Release(m_pMoveCollider);
	Safe_Release(m_pRader);
}

void CP_Chixia::OnCollisionEnter(CCollider * src, CCollider * dest)
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

				// SP BP TP 수치 회복 처리
				/*m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] += m_AttackInfos[m_iCurAttackID].fSPGain;
				if (m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] > m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL])
				m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] = m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL];

				m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] += m_AttackInfos[m_iCurAttackID].fBPGain;
				if (m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] > m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST])
				m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] = m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST];

				m_pPlayerStateClass->Gain_QTEGauge(m_AttackInfos[m_iCurAttackID].fTPGain);*/

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

void CP_Chixia::OnCollisionStay(CCollider * src, CCollider * dest)
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
					IS_ATTACK_04 == m_Scon.iCurState)
				{
					m_tCurState.bRootMotion = false;
					m_Scon.vMovement = _float3(0.f, 0.f, 0.f);
				}
			}
#pragma endregion 
		}
	}

}

void CP_Chixia::OnCollisionExit(CCollider * src, CCollider * dest)
{
}

