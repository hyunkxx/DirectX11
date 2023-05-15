#include "stdafx.h"
#include "..\Public\PlayerGirl.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Parts.h"
#include "Effect.h"
#include "EffectKey.h"
#include "PartsKey.h"
#include "PriorityKey.h"

#include "Chest.h"

const _int CPlayerGirl::iState_End = CCharacter::SS_END + (CPlayerGirl::IS_END - CPlayerGirl::IS_START);

const char CPlayerGirl::szIndividualStateTag[CPlayerGirl::IS_END - CPlayerGirl::IS_START][MAX_PATH] =
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
	"IS_SKILL_QTE",
	"IS_BURST"
};

CCharacter::MULTISTATE CPlayerGirl::m_tStates[SS_END + (IS_END - IS_START)];

CPlayerGirl::CPlayerGirl(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CPlayerGirl::CPlayerGirl(const CPlayerGirl & rhs)
	: CCharacter(rhs)
{
}

HRESULT CPlayerGirl::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerGirl::Initialize(void * pArg)
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
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(6.f, 30.f, 5.f, 1.f));
	m_pNaviCom->Set_CurrentIndex(0);
	
	//m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	XMStoreFloat4x4(&m_WorldMatrix , XMMatrixIdentity());

	// StateController 초기화
	m_Scon.iCurState = 0;
	m_Scon.iNextState = 0;
	m_Scon.ePositionState = PS_GROUND;
	m_Scon.iPrevCellState = 0;
	m_tCurState = m_tStates[0];
	SetUp_State();
	SetUp_Animations(false);
	return S_OK;
}

void CPlayerGirl::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

#ifdef _DEBUG
	m_pRendererCom->DebugBundleRender_Control(true);
#endif

	//pStaticObject = pGame->Find_GameObject(LEVEL_GAMEPLAY, L"StaticTest");
}

void CPlayerGirl::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_double TimeDelay = 1.0;

	__super::Tick(TimeDelta);

	if (pGameInstance->InputKey(DIK_NUMPAD0) == KEY_STATE::HOLD)
	{
		TimeDelay = 0.1;
	}
	else
	{
		TimeDelay = 1.0;
	}

	Key_Input(TimeDelta * TimeDelay); // 입력 > 다음 상태 확인 > 갱신될 경우 Setup_state, setup_animation

	Tick_State(TimeDelta * TimeDelay); // PlayAnimation, 애니메이션에 따른 이동, 애니메이션 종료 시 처리

	On_Cell(); // 자발적인 움직임 후처리 >> 주로 내비 메쉬
	
	// Parts 처리
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
			m_Parts[i]->Tick(TimeDelta * TimeDelay);
	}

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
}

void CPlayerGirl::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	// On_Hit(TimeDelta) 피격 처리

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
		{
			m_Parts[i]->LateTick(TimeDelta);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, m_Parts[i]);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, m_Parts[i]);
		}
	}

	CGameMode* pGameMode = CGameMode::GetInstance();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	static _bool bCamLock = false;
	if (pGameInstance->InputKey(DIK_SCROLL) == KEY_STATE::TAP)
	{
		bCamLock = !bCamLock;

		if (bCamLock)
			pGameMode->UseCamera(CGameMode::CAM_DYNAMIC);
		else
			pGameMode->UseCamera(CGameMode::CAM_PLAYER);
	}


	//Effect Bones 처리
	Update_EffectBones();
}

HRESULT CPlayerGirl::Render()
{
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

			m_pShaderCom->Begin(7); // Eye
		}
		else
			m_pShaderCom->Begin(iPass);

		m_pModelCom->Render(i);

		////Rim Light
		//if (i != 5 && 10.f > ComputeCameraLength())
		//{
		//	m_pShaderCom->Begin(7);
		//	m_pModelCom->Render(i);
		//}
	}

	m_pNaviCom->Render();

	return S_OK;
}

HRESULT CPlayerGirl::RenderShadow()
{
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

void CPlayerGirl::RenderGUI()
{
}

void CPlayerGirl::SetUp_Animations(_bool bContinue)
{
	m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_BASE], m_tCurState.bLerp, bContinue);
	m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_RIBBON], m_tCurState.bLerp, bContinue);
}

HRESULT CPlayerGirl::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(270.f);

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
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_PLAYERGIRL_MODEL,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AnimSet_Base */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE,
		TEXT("Com_AnimSet_Base"), (CComponent**)&m_pAnimSetCom[ANIMSET_BASE])))
		return E_FAIL;

	/* For.Com_AnimSet_Ribbon */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON,
		TEXT("Com_AnimSet_Ribbon"), (CComponent**)&m_pAnimSetCom[ANIMSET_RIBBON])))
		return E_FAIL;


	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATION_DESC));
	NavigationDesc.iCurrentIndex = 0;

	/* Navigation */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::NAVIGATION,
		TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NavigationDesc)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::EYE_BURST,
		TEXT("Com_Eye_Burst_Texture"), (CComponent**)&m_pEyeBurstTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::EYE_MASK,
		TEXT("Com_Eye_Mask_Texture"), (CComponent**)&m_pEyeMaskTexture)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.8f, 0.8f, 0.8f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerGirl::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(MULTISTATE) * iState_End);

	// 로드하는 코드
	for (_int i = 0; i < iState_End; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, TEXT("../../Data/CharState/PlayerGirl/PlayerGirl_%d.state"), i);
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
					//m_tStates[i].ppStateKeys[j] = CEffectKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PARTS:
					m_tStates[i].ppStateKeys[j] = CPartsKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PRIORITY:
					m_tStates[i].ppStateKeys[j] = CPriorityKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_DISSOLVE:

					break;
				case CStateKey::TYPE_OBB:

					break;
				case CStateKey::TYPE_MISSILE:

					break;
				case CStateKey::TYPE_SOUND:

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

void CPlayerGirl::Release_States()
{
	for (_int i = 0; i < iState_End; ++i)
	{
		if (0 == m_tStates[i].iKeyCount)
			continue;
		for (_uint j = 0; j < m_tStates[i].iKeyCount; ++j)
		{
			if(nullptr != m_tStates[i].ppStateKeys[j])
				Safe_Release(m_tStates[i].ppStateKeys[j]);
		}
		Safe_Delete_Array(m_tStates[i].ppStateKeys);
	}
}

void CPlayerGirl::Shot_PartsKey(_uint iParts/*int0*/, _uint iState/*int1*/, _uint iDissolve/*int2*/, _double Duration/*float*/)
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

void CPlayerGirl::Shot_PriorityKey(_uint iLeavePriority)
{
	m_tCurState.iLeavePriority = iLeavePriority;
}

void CPlayerGirl::Shot_EffectKey(_tchar * szEffectTag/* szTag1*/, _uint EffectBoneID /* iInt0 */, EFFECT_ID eEffectID , _bool bTracking/*iInt1*/)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag , eEffectID);
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	if (0 == EffectBoneID)
		pEffect->Play_Effect(&m_pMainTransform->Get_WorldMatrix(), bTracking);
	else
		pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CPlayerGirl::SetUp_State()
{
	// 키 리셋
	for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
	{
		if (nullptr != m_tCurState.ppStateKeys[i])
			m_tCurState.ppStateKeys[i]->Reset();
	}

	// 오입력 예외처리
	if (m_Scon.iNextState >= iState_End)
		m_Scon.iNextState = iState_End - 1;
	else if (m_Scon.iNextState <= 0)
		m_Scon.iNextState = 0;

	// 다음 애니메이션으로 갱신
	m_Scon.iCurState = m_Scon.iNextState;
	m_tCurState = m_tStates[m_Scon.iCurState];

	m_Scon.TrackPos = 0.0;
	m_Scon.bAnimFinished = false;

	

	if ((SS_JUMP_WALK == m_Scon.iCurState ||
		SS_JUMP_RUN == m_Scon.iCurState || 
		SS_FIXHOOK_END_UP == m_Scon.iCurState ||
		SS_CLIMB_MOVE == m_Scon.iCurState ||
		SS_FALL == m_Scon.iCurState) &&
		PS_AIR != m_Scon.ePositionState)
	{
		m_Scon.ePositionState = PS_AIR;
		m_Scon.bFalling = false;
		XMStoreFloat3(&m_fJumpPos, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
	}

	if (SS_LAND_LIGHT == m_Scon.iCurState ||
		SS_LAND_HEAVY == m_Scon.iCurState ||
		SS_LAND_ROLL == m_Scon.iCurState ||
		SS_CLIMB_ONTOP == m_Scon.iCurState ||
		SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState ||
		IS_AIRATTACK_END == m_Scon.iCurState)
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

	// Climb 종료 시 Look 방향 되돌리기
	if(SS_CLIMB_ONTOP == m_Scon.iCurState ||
		SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState ||
		SS_CLIMB_MOVE == m_Scon.iCurState ||
		SS_LAND_LIGHT == m_Scon.iCurState)
	{
		m_pMainTransform->Set_LookDir(XMVectorSetY(m_pMainTransform->Get_State(CTransform::STATE_LOOK), 0.f));
	}

	// 2단 점프 횟수 차감
	if(SS_JUMP_SECOND_B == m_Scon.iCurState ||
		SS_JUMP_SECOND_F == m_Scon.iCurState)
		--m_iAirJumpCount;

	// 무기 위치 잡기
	if (0 == m_tCurState.bWeaponState)
		Set_WeaponUse(false);
	else
		Set_WeaponUse(true);

	//PhysicMove
	if (false == m_tCurState.bRootMotion)
	{
		PHYSICMOVE tPhysicMove = PlayerStatePhysics[m_tCurState.iPhysicMoveID];

		if (true == tPhysicMove.bInitMovement)
			XMStoreFloat3(&m_Scon.vMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
	}
}

HRESULT CPlayerGirl::SetUp_ShaderResources()
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

HRESULT CPlayerGirl::Setup_ShadowShaderResource()
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

void CPlayerGirl::Set_WeaponUse(_bool bBool)
{
	if (true == bBool)
	{
		m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON1);
		m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON2);
	}
	else
	{
		m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON3);
		m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON4);
	}
}

void CPlayerGirl::Key_Input(_double TimeDelta)
{
	m_WorldMatrix = m_pMainTransform->Get_WorldMatrix();

	CGameInstance* pGame = CGameInstance::GetInstance();

	//
	INPUT eCurFrameInput = INPUT_NONE;
	_bool bInputDir[4] = { false, false, false, false };

	// 이동 기능 구현용 임시 코드
	_matrix matCam = pGame->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_vector vCamLook = XMVector3Normalize(XMVectorSetY(matCam.r[2], 0.f));
	_vector vCamRight = XMVector3Normalize(XMVectorSetY(matCam.r[0], 0.f));
	_vector vInputDir = XMVectorZero();
	

	// 입력 제한 걸기
	if (pGame->InputKey(DIK_I) == KEY_STATE::TAP)
	{
		m_bInputLock = !m_bInputLock;
	}

	if (m_bInputLock)
	{
		if (pGame->InputKey(DIK_C) == KEY_STATE::TAP)
			m_Scon.bWalk = !m_Scon.bWalk;

		/*if (pGame->InputKey(DIK_UP) == KEY_STATE::TAP)
		{
		m_Scon.iNextState = m_Scon.iCurState + 1;

		if (m_Scon.iNextState >= iState_End)
		m_Scon.iNextState = iState_End - 1;

		SetUp_State();
		}

		if (pGame->InputKey(DIK_DOWN) == KEY_STATE::TAP)
		{
		m_Scon.iNextState = m_Scon.iCurState - 1;

		if (m_Scon.iNextState <= 0)
		m_Scon.iNextState = 0;

		SetUp_State();
		}*/

		if (pGame->InputKey(DIK_TAB) == KEY_STATE::TAP)
		{
			m_Scon.iNextState = 145;
			SetUp_State();
		}

		if (pGame->InputKey(DIK_R) == KEY_STATE::TAP)
		{
			// 초기위치 설정
			m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(6.f, 30.f, 5.f, 1.f));
			m_pNaviCom->Set_CurrentIndex(0);
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
		if (pGame->InputKey(DIK_Q) == KEY_STATE::TAP)
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
			if (m_ChargeAcc > 0.5)
			{
				eCurFrameInput = INPUT_ATTACK_RELEASE;
			}
		}

		if (pGame->InputMouse(DIMK_LB) == KEY_STATE::HOLD)
		{
			m_ChargeAcc += TimeDelta;
			if (m_ChargeAcc > 0.5)
				eCurFrameInput = INPUT_ATTACK_CHARGE;
		}
		else
			m_ChargeAcc = 0.0;

		

		// Tool
		if (pGame->InputKey(DIK_T) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_TOOL;
		}
	}

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
		case Client::CPlayerGirl::INPUT_NONE:
			switch (m_Scon.iCurState)
			{
			case SS_WALK_F:
				m_Scon.iNextState = SS_WALK_STOP_L;
				break;

			case SS_RUN_F:
				m_Scon.iNextState = SS_RUN_STOP_L;
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

		case Client::CPlayerGirl::INPUT_MOVE:
			// WALK
			if (m_Scon.bWalk)
			{
				m_Scon.iNextState = SS_WALK_F;
				/*if (true == bInputDir[0])
					if (true == bInputDir[2])
						m_Scon.iNextState = SS_WALK_LF;
					else if (true == bInputDir[3])
						m_Scon.iNextState = SS_WALK_RF;
					else
						m_Scon.iNextState = SS_WALK_F;
				else if (true == bInputDir[1])
					if (true == bInputDir[2])
						m_Scon.iNextState = SS_WALK_LB;
					else if (true == bInputDir[3])
						m_Scon.iNextState = SS_WALK_RB;
					else
						m_Scon.iNextState = SS_WALK_B;
				else if (true == bInputDir[2])
					m_Scon.iNextState = SS_WALK_LF;
				else 
					m_Scon.iNextState = SS_WALK_RF;*/
			}
			// RUN
			else
			{
				m_Scon.iNextState = SS_RUN_F;
				/*if (true == bInputDir[0])
					if (true == bInputDir[2])
						m_Scon.iNextState = SS_RUN_LF;
					else if (true == bInputDir[3])
						m_Scon.iNextState = SS_RUN_RF;
					else
						m_Scon.iNextState = SS_RUN_F;
				else if (true == bInputDir[1])
					if (true == bInputDir[2])
						m_Scon.iNextState = SS_RUN_LB;
					else if (true == bInputDir[3])
						m_Scon.iNextState = SS_RUN_RB;
					else
						m_Scon.iNextState = SS_RUN_B;
				else if (true == bInputDir[2])
					m_Scon.iNextState = SS_RUN_LF;
				else
					m_Scon.iNextState = SS_RUN_RF;*/
			}
			break;

		case Client::CPlayerGirl::INPUT_DASH:
			if (SS_SPRINT != m_Scon.iCurState &&
				SS_SPRINT_IMPULSE_F != m_Scon.iCurState)
				m_Scon.iNextState = SS_SPRINT_IMPULSE_F;
			else if (SS_CLIMB_BOOST_ONTOPTOSTOP == m_Scon.iCurState)
				m_Scon.iNextState = SS_CLIMB_BOOST_ONTOPTOSPRINT;
			/*if(!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
				m_Scon.iNextState = SS_MOVE_B;
			else
				m_Scon.iNextState = SS_MOVE_F;*/
			break;

		case Client::CPlayerGirl::INPUT_SPACE:
			switch (m_Scon.iCurState)
			{
			case SS_STAND1:
			case SS_STAND1_ACTION01:
			case SS_STAND2:
			case SS_STANDCHANGE:
			case SS_STANDUP:
				m_Scon.iNextState = SS_JUMP_WALK;
				break;
			default:
				m_Scon.iNextState = SS_JUMP_RUN;
				break;
			}
			break;


		case Client::CPlayerGirl::INPUT_ATTACK:
			switch (m_Scon.iCurState)
			{
			case IS_ATTACK_01:
				m_Scon.iNextState = IS_ATTACK_02;
				break;
			case IS_ATTACK_02:
				m_Scon.iNextState = IS_ATTACK_03;

				pEffect = CGameInstance::GetInstance()->Get_Effect(L"Nvzhu_Attack_03", EFFECT_ID::PLAYER_NVZHU);
				ParentMatrix = m_pMainTransform->Get_WorldMatrix();
				pEffect->Play_Effect(&ParentMatrix);

				pEffect = CGameInstance::GetInstance()->Get_Effect(L"Link_Effect", EFFECT_ID::PLAYER_NVZHU);
				ParentMatrix._42 += 1.f;
				ParentMatrix = m_pMainTransform->Get_WorldMatrix();
				pEffect->Play_Effect(&ParentMatrix);

				break; 
			case IS_ATTACK_03:
				if (m_Scon.TrackPos > 20.0)
					m_Scon.iNextState = IS_ATTACK_PO_2;
				else
					m_Scon.iNextState = IS_ATTACK_04;
				break;
			case IS_ATTACK_09:
				if (m_Scon.TrackPos > 20.0)
					m_Scon.iNextState = IS_ATTACK_PO_2;
				else
					m_Scon.iNextState = IS_ATTACK_01;
				break;
			case IS_SKILL_02:
				m_Scon.iNextState = IS_ATTACK_05;
				break;
			case IS_ATTACK_PO_2:
				m_Scon.iNextState = IS_ATTACK_PO_3;
				break;
			default:
				m_Scon.iNextState = IS_ATTACK_01;
				break;
			}
			break;

		case Client::CPlayerGirl::INPUT_ATTACK_CHARGE:
			m_Scon.iNextState = IS_ATTACK_09;
			break;

		case Client::CPlayerGirl::INPUT_SKILL:
			if (m_fSkillGauge >= 50.f)
			{
				m_fSkillGauge -= 50.f;
				m_Scon.iNextState = IS_SKILL_02;
			}
			else
				m_Scon.iNextState = IS_SKILL_01;
			break;

		case Client::CPlayerGirl::INPUT_BURST:
			m_Scon.iNextState = IS_BURST;
			break;

		case Client::CPlayerGirl::INPUT_TOOL:
			m_Scon.iNextState = SS_FIXHOOK_END_UP;
			break;
		default:
			break;
		}
	}
	else if (PS_AIR == m_Scon.ePositionState)
	{
		switch (eCurFrameInput)
		{
		case Client::CPlayerGirl::INPUT_SPACE:
			if (m_iAirJumpCount > 0)
			{
				if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
					m_Scon.iNextState = SS_JUMP_SECOND_B;
				else
					m_Scon.iNextState = SS_JUMP_SECOND_F;
			}
			break;
		case Client::CPlayerGirl::INPUT_ATTACK:
			m_Scon.iNextState = IS_AIRATTACK_START;
			break;
		case Client::CPlayerGirl::INPUT_TOOL:
			m_Scon.iNextState = SS_FIXHOOK_END_UP;
			break;
		default:
			break;
		}
	}
	else if (PS_CLIMB == m_Scon.ePositionState)
	{
		switch (eCurFrameInput)
		{
		case Client::CPlayerGirl::INPUT_NONE:
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

		case Client::CPlayerGirl::INPUT_MOVE:
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
			break;
		case Client::CPlayerGirl::INPUT_DASH:
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
			else if(bInputDir[3])
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
		case Client::CPlayerGirl::INPUT_SPACE:
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

	// 지금 상태를 끊고 다음 상태로 갱신 할지 여부
	if (m_Scon.iCurState != m_Scon.iNextState)
	{
		if (m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority)
		{
			SetUp_State();
			SetUp_Animations(false);
			// 상태 갱신 시 1번만
			if (CCharacter::ROT_ONSTART == m_tCurState.iRotationType)
			{
				if(0.f != XMVectorGetX(XMVector3Length(vInputDir)))
					m_pMainTransform->Set_LookDir(vInputDir);
			}
		}
	}

	// 매 프레임
	if (CCharacter::ROT_LOOKAT == m_tCurState.iRotationType)
	{
		if (PS_CLIMB != m_Scon.ePositionState)
		{
			_vector vAxis = XMVector3Cross(m_pMainTransform->Get_State(CTransform::STATE_LOOK), vInputDir);
			_float fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_LOOK)), XMVector3Normalize(vInputDir))));

			if (!XMVector3Equal(vAxis, XMVectorZero()))
			{
				if (fAngle > m_pMainTransform->Get_RotationSpeed() * TimeDelta)
					m_pMainTransform->Rotate(vAxis, TimeDelta);
				else
					m_pMainTransform->Set_LookDir(vInputDir);
			}
		}
	}

}

void CPlayerGirl::Tick_State(_double TimeDelta)
{
	//
	if (false == m_Scon.bAnimFinished)
	{
		_float4 vRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float3 vMovement;

		// 애니메이션 갱신
		m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_Scon.TrackPos, &m_Scon.bAnimFinished);
		m_pAnimSetCom[ANIMSET_RIBBON]->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, nullptr);

		m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();
		m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();
		// 여기까지 Playanimation

		m_pModelCom->Invalidate_CombinedMatrices();

		// 루프하지 않는 애니메이션이 이번 프레임에 끝났으면 전프레임 움직임을 적용
		// 애니메이션 프레임이랑 FrameRate가 달라서 발생하는 오차값으로 인한 버그 방지
		if(true == m_Scon.bAnimFinished && false == m_tCurState.bLoop)
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
				const PHYSICMOVE& PhysicMove = PlayerStatePhysics[m_tCurState.iPhysicMoveID];
				if (false == PhysicMove.bConstant)
				{
					_vector vMove = XMLoadFloat3(&m_Scon.vPrevMovement) * (_float)TimeDelta;

					_float fXZSpeed = XMVectorGetX(XMVector2Length(vMove)) * (1.f - (_float)TimeDelta * PhysicMove.fHorizontalAtten);
					if (fabs(fXZSpeed) < 0.01f)
						fXZSpeed = 0.f;
					_float fYSpeed = XMVectorGetZ(vMove) - PhysicMove.fVerticalAccel * (_float)TimeDelta;

					fXZSpeed = min(fXZSpeed, PhysicMove.fHorizontalMaxSpeed);
					fYSpeed = max(fYSpeed, PhysicMove.fVerticalMaxSpeed);

					_vector vFinalMove = XMVectorSetZ(XMVector2Normalize(vMove) * fXZSpeed, fYSpeed);

					XMStoreFloat3(&vMovement, vFinalMove);
				}
				else
					XMStoreFloat3(&vMovement, XMLoadFloat3(&m_Scon.vMovement) * (_float)TimeDelta);
			}
		}
		// 구해진 이동값만큼 움직이고 이전 프레임 정보를 저장, + TimeDelta 대응
		if(SS_CLIMB_ONTOP == m_Scon.iCurState || 
			SS_CLIMB_BOOST_ONTOP == m_Scon.iCurState)
			m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, nullptr, m_pModelCom->Get_TopBoneCombinedPos());
		else
			m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, m_pNaviCom, m_pModelCom->Get_TopBoneCombinedPos());
		
		XMStoreFloat3(&m_Scon.vPrevMovement, XMLoadFloat3(&vMovement) / (_float)TimeDelta);
		
		// StateKey 처리
		for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
		{
			if(nullptr != m_tCurState.ppStateKeys[i])
				m_tCurState.ppStateKeys[i]->Check(m_Scon.TrackPos, this);
		}

		// 떨어지고 있을 경우 
		if (vMovement.z < 0.f)
		{
			if(!(SS_FIXHOOK_END_UP == m_Scon.iCurState && 10.f > m_Scon.TrackPos))
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

void CPlayerGirl::On_Cell()
{
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

				vExitPos += XMVector3Normalize(vExitLook) * 1.3f * (_float)m_Scon.TrackPos / 14.0;

				vExitPos = XMVectorSetY(vExitPos, XMVectorGetY(m_pMainTransform->Get_State(CTransform::STATE_POSITION)));

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

			vExitPos = XMVectorSetY(vExitPos, XMVectorGetY(m_pMainTransform->Get_State(CTransform::STATE_POSITION)));

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
					if (IS_AIRATTACK_START == m_Scon.iCurState ||
						IS_AIRATTACK_LOOP == m_Scon.iCurState)
					{
						m_Scon.iNextState = IS_AIRATTACK_END;
					}
					// 일반적인 경우
					else
					{
						_float fGap = m_fJumpPos.y - fCellHeight;
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
	
		// 벽에 딱 붙임

		if (CELL_WALL == iCurCellState)
		{
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

void CPlayerGirl::Init_AnimSystem()
{
	// Base
	for (auto& pBone : m_pAnimSetCom[ANIMSET_BASE]->Get_Bones())
		pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetCom[ANIMSET_BASE]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		if (!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|AirAttack_Start")))
			pAnim->Set_Duration(10.0);

		if (!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_D_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_D_2")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_DL_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_DL_2")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_DR_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_DR_2")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_L_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_L_2")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_R_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_R_2")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_U_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_U_2")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_UL_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_UL_2")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_UR_1")) ||
			!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_UR_2")))
			pAnim->Set_Duration(15.0);


		if (!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Climb_OnTop")))
			pAnim->Set_Duration(48.0);


		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetCom[ANIMSET_BASE]->Get_BonePtr(pChannel->Get_TargetBoneID());

			if (wcsncmp(szChannelName, TEXT("Bip001"), 6) &&
				lstrcmp(szChannelName, TEXT("WeaponProp01")) &&
				lstrcmp(szChannelName, TEXT("WeaponProp02")) &&
				wcsncmp(szChannelName, TEXT("Root"), 4))
				continue;

			if (true == pBone->Is_ChildOf(TEXT("Bip001Head")))
			{
				if (lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|AirAttack_End")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|AirAttack_Loop")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|AirAttack_Start")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Attack01")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Attack02")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Attack03")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Attack04")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Attack09")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Attack_po2_Temp")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Attack_po3_Temp")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Burst01")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Skill01")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Skill02")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|SkillQte")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Stand1_Action01")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Stand1_Action02")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Stand1_Action03")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Stand2")) &&
					lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|StandChange")))
				{
					continue;
				}
			}

			pChannel->Set_Apply(true);

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

			// 필살기 
			if (!lstrcmp(szAnimName, TEXT("R2T1PlayerFemaleMd10011.ao|Rib_Burst01")))
			{
				if (!wcsncmp(szChannelName, TEXT("Root"), 4) ||
					!wcsncmp(szChannelName, TEXT("Bip001"), 6) ||
					!wcsncmp(szChannelName, TEXT("Weapon"), 6) ||
					!wcsncmp(szChannelName, TEXT("Hulu"), 4) ||
					!lstrcmp(szChannelName, TEXT("R2T1PlayerFemaleMd10011.ao")) ||
					!lstrcmp(szChannelName, TEXT("Bone_Hair001_L")) ||
					!lstrcmp(szChannelName, TEXT("Bone_Hair001_R")) ||
					!lstrcmp(szChannelName, TEXT("Hiar_L_F01")) ||
					!lstrcmp(szChannelName, TEXT("Hiar_R_F01")) ||
					!lstrcmp(szChannelName, TEXT("Hair_liuhai01")))
					continue;

				/*if (pChannel->Get_NumKeyFrames() <= 2)
					continue;*/

				//if(wcsncmp(szChannelName, TEXT("Bip001"), 6))
				pChannel->Set_Apply(true);
			}
			else
			{
				// 이전 세팅
				if (true == pBone->Is_ChildOf(TEXT("Hair_M_B00")) ||
					true == pBone->Is_ChildOf(TEXT("Piao_L_lingjie01")) ||
					true == pBone->Is_ChildOf(TEXT("Piao_R_lingjie01")) ||
					true == pBone->Is_ChildOf(TEXT("skrit_L_F02")) ||
					true == pBone->Is_ChildOf(TEXT("skirt_M_B02")) ||
					true == pBone->Is_ChildOf(TEXT("Piao_F01")) ||
					true == pBone->Is_ChildOf(TEXT("Bone_Piao011_L")))
				{
					pChannel->Set_Apply(true);
				}
				else if (!lstrcmp(szChannelName, TEXT("Hair_M_B00")) ||
					!lstrcmp(szChannelName, TEXT("Piao_L_lingjie01")) ||
					!lstrcmp(szChannelName, TEXT("Piao_R_lingjie01")) ||
					!lstrcmp(szChannelName, TEXT("Piao_L_pidai01")) ||
					!lstrcmp(szChannelName, TEXT("Piao_R_pidai01")) ||
					!lstrcmp(szChannelName, TEXT("skrit_L_F01")) ||
					!lstrcmp(szChannelName, TEXT("skirt_M_B01")) ||
					!lstrcmp(szChannelName, TEXT("Bone_Piao011_L"))
					/*!lstrcmp(szChannelName, TEXT("Piao_F01"))*/)
				{
					pChannel->Set_Apply(true);
				}
			}
		}
	}


	// FramePerSec 세팅
	for(_uint i= 0; i < iState_End; ++i)
	{ 
		m_pAnimSetCom[ANIMSET_BASE]->Get_Animation(m_tStates[i].iAnimID[ANIMSET_BASE])->Set_TicksPerSecond(m_tStates[i].FramePerSec);
		m_pAnimSetCom[ANIMSET_RIBBON]->Get_Animation(m_tStates[i].iAnimID[ANIMSET_RIBBON])->Set_TicksPerSecond(m_tStates[i].FramePerSec);
	}
}

HRESULT CPlayerGirl::Init_Parts()
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

	m_Parts[PARTS_WEAPON_MAIN] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_SWORD_0_SWORD, &PartsDesc));
	m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON7);
	m_Parts[PARTS_WEAPON_SUB] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_SWORD_0_SCABBARD, &PartsDesc));
	m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON7);
	m_Parts[PARTS_HULU] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_HULU_0, &PartsDesc));
	m_Parts[PARTS_HULU]->Set_Parent(PBONE_HULU);

	return S_OK;
}

HRESULT CPlayerGirl::Init_EffectBones()
{
	//NONE은 걍 월드 매트릭스 던짐
	m_EffectBones[EBONE_SPINE] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine"));
	m_EffectBones[EBONE_WEAPON] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp03"));

	return S_OK;
}

void CPlayerGirl::Update_EffectBones()
{
	// TODO : 살아있는 이펙트가 걸려있는

	for (_uint i = 1; i < EBONE_END; ++i)
	{
		XMStoreFloat4x4(&m_EffectBoneMatrices[i], XMLoadFloat4x4(&m_EffectBones[i]->Get_CombinedTransfromationMatrix())
			* XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
	}
}

CPlayerGirl * CPlayerGirl::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerGirl* pInstance = new CPlayerGirl(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayerGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerGirl::Clone(void * pArg)
{
	CPlayerGirl* pInstance = new CPlayerGirl(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayerGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerGirl::Free()
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

	Safe_Release(m_pNaviCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pEyeBurstTexture);
	Safe_Release(m_pEyeMaskTexture);

	Safe_Release(m_pCollider);
}

void CPlayerGirl::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}

void CPlayerGirl::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CPlayerGirl::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
