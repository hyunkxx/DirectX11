#include "stdafx.h"
#include "..\Public\PlayerGirl.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Parts.h"
#include "Effect.h"
#include "PartsKey.h"

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

	Init_AnimSystem();

	m_pAnimSetCom[ANIMSET_BASE]->Set_RootBone(TEXT("Root"));

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 0.f, 0.f, 1.f));
	//m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	XMStoreFloat4x4(&m_WorldMatrix , XMMatrixIdentity());

	//
	m_tStateController.iCurState = 0;
	m_tStateController.iNextState = 0;
	m_tCurState = m_tStates[0];
	SetUp_State();

	return S_OK;
}

void CPlayerGirl::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	//pStaticObject = pGame->Find_GameObject(LEVEL_GAMEPLAY, L"StaticTest");
}

void CPlayerGirl::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	Key_Input(TimeDelta);

	

	Tick_State(TimeDelta);

	// Parts 처리
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
			m_Parts[i]->Tick(TimeDelta);
	}
}

void CPlayerGirl::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	// Parts 처리
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
		{
			m_Parts[i]->LateTick(TimeDelta);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, m_Parts[i]);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, m_Parts[i]);
		}

	}
}

HRESULT CPlayerGirl::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	int iPass = 4;
	if (10.f <= ComputeCameraLength())
		iPass = 3;

	for (_uint i = 0; i < 6; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_VertexTexture(m_pShaderCom, "g_VertexTexture", i)))
			return E_FAIL;

		if (i == 5)
			m_pShaderCom->Begin(6); // Eye
		else
			m_pShaderCom->Begin(iPass);

		m_pModelCom->Render(i);
	}

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

void CPlayerGirl::SetUp_Animations()
{
	m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_BASE], m_tCurState.bLerp);
	m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tCurState.iAnimID[ANIMSET_RIBBON], m_tCurState.bLerp);
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
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
  	_uint nCurrentLevel = pGM->GetCurrentLevel();

	// For.Com_Shader_ModelAnim
	if (FAILED(__super::Add_Component(nCurrentLevel, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Component(nCurrentLevel, DMODEL::DMD_PLAYERGIRL_MODEL,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AnimSet_Base */
	if (FAILED(__super::Add_Component(nCurrentLevel, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE,
		TEXT("Com_AnimSet_Base"), (CComponent**)&m_pAnimSetCom[ANIMSET_BASE])))
		return E_FAIL;

	/* For.Com_AnimSet_Ribbon */
	if (FAILED(__super::Add_Component(nCurrentLevel, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON,
		TEXT("Com_AnimSet_Ribbon"), (CComponent**)&m_pAnimSetCom[ANIMSET_RIBBON])))
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

					break;
				case CStateKey::TYPE_PARTS:
					m_tStates[i].ppStateKeys[j] = CPartsKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PRIORITY:

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

void CPlayerGirl::Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration)
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

void CPlayerGirl::SetUp_State()
{
	// 키 리셋
	for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
	{
		if (nullptr != m_tCurState.ppStateKeys[i])
			m_tCurState.ppStateKeys[i]->Reset();
	}

	// 오입력 예외처리
	if (m_tStateController.iNextState >= iState_End)
		m_tStateController.iNextState = iState_End - 1;
	else if (m_tStateController.iNextState <= 0)
		m_tStateController.iNextState = 0;

	// 다음 애니메이션으로 갱신
	m_tStateController.iCurState = m_tStateController.iNextState;
	m_tCurState = m_tStates[m_tStateController.iCurState];

	m_tStateController.TrackPos = 0.0;
	m_tStateController.bAnimFinished = false;

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
			XMStoreFloat3(&m_tStateController.vMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
		else
			m_tStateController.vMovement = m_tStateController.vPrevMovement;
	}

	SetUp_Animations();
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
	CGameInstance* pGame = CGameInstance::GetInstance();

	INPUT eCurFrameInput = INPUT_NONE;
	_vector	vMoveDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vLook = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_LOOK));
	_vector vRight = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_RIGHT));
	_bool bMoved = false;

	if (pGame->InputKey(DIK_1) == KEY_STATE::TAP)
	{
		CEffect* pEffect = pGame->Get_Effect(L"YangYang_Jump_Attack_01");
		pEffect->Play_Effect(&m_WorldMatrix);
	}


	if (pGame->InputKey(DIK_UP) == KEY_STATE::TAP)
	{
		m_tStateController.iNextState = m_tStateController.iCurState + 1;

		if (m_tStateController.iNextState >= iState_End)
			m_tStateController.iNextState = iState_End - 1;

		SetUp_State();
	}

	if (pGame->InputKey(DIK_DOWN) == KEY_STATE::TAP)
	{
		m_tStateController.iNextState = m_tStateController.iCurState - 1;

		if (m_tStateController.iNextState <= 0)
			m_tStateController.iNextState = 0;

		SetUp_State();
	}

	if (pGame->InputKey(DIK_TAB) == KEY_STATE::TAP)
	{
		m_tStateController.iNextState = 145;
		SetUp_State();
	}


	if (pGame->InputKey(DIK_W) == KEY_STATE::HOLD)
	{
		vMoveDir += vLook;
		eCurFrameInput = INPUT_MOVE;
		bMoved = true;
	}
	else if (pGame->InputKey(DIK_S) == KEY_STATE::HOLD)
	{
		vMoveDir -= vLook;
		eCurFrameInput = INPUT_MOVE;
		bMoved = true;
	}

	if (pGame->InputKey(DIK_A) == KEY_STATE::HOLD)
	{
		vMoveDir -= vRight;
		eCurFrameInput = INPUT_MOVE;
		bMoved = true;
	}
	else if (pGame->InputKey(DIK_D) == KEY_STATE::HOLD)
	{
		vMoveDir += vRight;
		eCurFrameInput = INPUT_MOVE;
		bMoved = true;
	}

	if (pGame->InputKey(DIK_LSHIFT) == KEY_STATE::HOLD)
	{
	}

	// 입력에 따라 대응하는 다음 상태 찾기
	switch (eCurFrameInput)
	{
	case Client::CPlayerGirl::INPUT_NONE:
		//m_tStateController.iNextState = SS_STAND2;
		break;
	case Client::CPlayerGirl::INPUT_MOVE:
		
		break;
	default:
		break;
	}

	// 지금 상태를 끊고 다음 상태로 갱신 할지 여부
	if (m_tStateController.iCurState != m_tStateController.iNextState)
	{
		if (m_tCurState.iPriority < m_tStates[m_tStateController.iNextState].iPriority)
		{
			SetUp_State();
		}
	}

}

void CPlayerGirl::Tick_State(_double TimeDelta)
{
	//
	if (false == m_tStateController.bAnimFinished)
	{
		_float4 vRotation;
		_float3 vMovement;

		// 애니메이션 갱신
		m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_tStateController.TrackPos, &m_tStateController.bAnimFinished);
		m_pAnimSetCom[ANIMSET_RIBBON]->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, nullptr);

		m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();
		m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();

		m_pModelCom->Invalidate_CombinedMatrices();

		// 루트 모션 처리(회전, 이동)
		if (true == m_tCurState.bRootMotion)
		{
			// TODO : 회전 적용
			//m_pMainTransform->Rotate_Quaternion(XMLoadFloat4(&vRotation));
			m_pMainTransform->Move_Anim(&vMovement);
			m_tStateController.vPrevMovement = vMovement;
		}
		else
		{
			// TODO : 등속 운동이 아닐 경우 가속/감속 체크하는 코드

			XMStoreFloat3(&vMovement, XMLoadFloat3(&m_tStateController.vMovement) * (_float)TimeDelta);
			m_pMainTransform->Move_Anim(&vMovement);
		}

		// StateKey 처리
		for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
		{
			if(nullptr != m_tCurState.ppStateKeys[i])
				m_tCurState.ppStateKeys[i]->Check(m_tStateController.TrackPos, this);
		}
			
	}

	if (true == m_tStateController.bAnimFinished)
	{
		if (true == m_tCurState.bLoop)
		{
			m_tStateController.iNextState = m_tStateController.iCurState;
			SetUp_State();
		}
			
		else
		{
			if (NO_ANIM != m_tCurState.iNextState)
			{
				m_tStateController.iNextState = m_tCurState.iNextState;
				SetUp_State();
			}
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

			/*if (pChannel->Get_NumKeyFrames() <= 2)
			continue;*/

			if (true == pBone->Is_ChildOf(TEXT("Hair_M_B00")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_L_lingjie01")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_R_lingjie01")) ||
				true == pBone->Is_ChildOf(TEXT("skrit_L_F02")) ||
				true == pBone->Is_ChildOf(TEXT("skirt_M_B02")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_F01")))
			{
				pChannel->Set_Apply(true);
			}
			else if (!lstrcmp(szChannelName, TEXT("Hair_M_B00")) ||
				!lstrcmp(szChannelName, TEXT("Piao_L_lingjie01")) ||
				!lstrcmp(szChannelName, TEXT("Piao_R_lingjie01")) ||
				!lstrcmp(szChannelName, TEXT("Piao_L_pidai01")) ||
				!lstrcmp(szChannelName, TEXT("Piao_R_pidai01")) ||
				!lstrcmp(szChannelName, TEXT("skrit_L_F01")) ||
				!lstrcmp(szChannelName, TEXT("skirt_M_B01"))
				/*!lstrcmp(szChannelName, TEXT("Piao_F01"))*/)
			{
				pChannel->Set_Apply(true);
			}
		}
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
	//m_Parts[PARTS_HULU] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_HULU, &PartsDesc));


	return S_OK;
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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
