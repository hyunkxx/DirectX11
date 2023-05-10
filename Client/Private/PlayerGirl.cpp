#include "stdafx.h"
#include "..\Public\PlayerGirl.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Parts.h"
#include "Effect.h"
#include "PartsKey.h"
#include "PriorityKey.h"

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

	// �ִϸ��̼� ����
	Init_AnimSystem();

	// ��Ʈ��ǿ� ��ã��
	m_pAnimSetCom[ANIMSET_BASE]->Set_RootBone(TEXT("Root"));

	// �ʱ���ġ ����
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(6.f, 30.f, 5.f, 1.f));
	m_pNaviCom->Set_CurrentIndex(0);
	
	//m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	XMStoreFloat4x4(&m_WorldMatrix , XMMatrixIdentity());

	// StateController �ʱ�ȭ
	m_Scon.iCurState = 0;
	m_Scon.iNextState = 0;
	m_Scon.ePositionState = PS_GROUND;
	m_tCurState = m_tStates[0];
	SetUp_State();
	SetUp_Animations(false);
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

	On_Cell();

	// Parts ó��
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
			m_Parts[i]->Tick(TimeDelta);
	}
}

void CPlayerGirl::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	// Parts ó��
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
		{
			m_Parts[i]->LateTick(TimeDelta);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, m_Parts[i]);
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
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

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

	return S_OK;
}

HRESULT CPlayerGirl::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(MULTISTATE) * iState_End);

	// �ε��ϴ� �ڵ�
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
		// ������
		if (0 == iState)
		{
			Set_WeaponUse(false);
		}
		// ������
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
	//	// �Ⱥ��̰� �Ѵ�.
	//	if (0 == iState)
	//	{

	//	}
	//	//���̰� �Ѵ�.
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

void CPlayerGirl::SetUp_State()
{
	// Ű ����
	for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
	{
		if (nullptr != m_tCurState.ppStateKeys[i])
			m_tCurState.ppStateKeys[i]->Reset();
	}

	// ���Է� ����ó��
	if (m_Scon.iNextState >= iState_End)
		m_Scon.iNextState = iState_End - 1;
	else if (m_Scon.iNextState <= 0)
		m_Scon.iNextState = 0;

	// ���� �ִϸ��̼����� ����
	m_Scon.iCurState = m_Scon.iNextState;
	m_tCurState = m_tStates[m_Scon.iCurState];

	m_Scon.TrackPos = 0.0;
	m_Scon.bAnimFinished = false;

	

	if ((SS_JUMP_WALK == m_Scon.iCurState ||
		SS_JUMP_RUN == m_Scon.iCurState || 
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
		IS_AIRATTACK_END == m_Scon.iCurState)
	{
		m_Scon.ePositionState = PS_GROUND;
	}
		


	// ���� ��ġ ���
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

	// �̵� ��� ������ �ӽ� �ڵ�
	_matrix matCam = pGame->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_vector vCamLook = XMVector3Normalize(XMVectorSetY(matCam.r[2], 0.f));
	_vector vCamRight = XMVector3Normalize(XMVectorSetY(matCam.r[0], 0.f));
	_vector vInputDir = XMVectorZero();
	

	// �Է� ���� �ɱ�
	if (pGame->InputKey(DIK_I) == KEY_STATE::TAP)
	{
		m_bInputLock = !m_bInputLock;
	}

	if (m_bInputLock)
	{
		// �ִϸ��̼� �׽�Ʈ�� �ӽ� �ڵ�
		if (pGame->InputKey(DIK_1) == KEY_STATE::TAP)
		{
			CEffect* pEffect = pGame->Get_Effect(L"YangYang_Jump_Attack_01");
			pEffect->Play_Effect(&m_WorldMatrix);
		}
		if (pGame->InputKey(DIK_2) == KEY_STATE::TAP)
		{
			CEffect* pEffect = pGame->Get_Effect(L"Test_SSD");
			pEffect->Play_Effect(&m_WorldMatrix);
		}
		if (pGame->InputKey(DIK_3) == KEY_STATE::TAP)
		{
			CEffect* pEffect = pGame->Get_Effect(L"Test_GLOW_SSD");
			pEffect->Play_Effect(&m_WorldMatrix);
		}
		
		if (pGame->InputKey(DIK_C) == KEY_STATE::TAP)
		{
			m_Scon.bWalk = !m_Scon.bWalk;
		}

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
			// �ʱ���ġ ����
			m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(6.f, 30.f, 5.f, 1.f));
			m_pNaviCom->Set_CurrentIndex(0);
			m_iAirJumpCount += 10;
			m_fSkillGauge += 50.f;
		}



		// �̵� ���� üũ

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

		// Shift �Է� �� ȸ��
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

		//
		if (pGame->InputKey(DIK_SPACE) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_SPACE;
		}

		if (pGame->InputMouse(DIMK_LB) == KEY_STATE::TAP)
		{
			eCurFrameInput = INPUT_ATTACK;
		}
	}

	if (0.f != XMVectorGetX(XMVector3Length(vInputDir)))
		XMVector3Normalize(vInputDir);
	
	

	// �Է¿� ���� �����ϴ� ���� ���� ã��
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
				break;
			case IS_ATTACK_03:
				m_Scon.iNextState = IS_ATTACK_04;
				break;
			case IS_SKILL_02:
				m_Scon.iNextState = IS_ATTACK_05;
				break;
			default:
				m_Scon.iNextState = IS_ATTACK_01;
				break;
			}
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
				if (m_Scon.iCurState != SS_JUMP_SECOND_B)
				{
					m_Scon.iNextState = SS_JUMP_SECOND_B;
					CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Double_Jump_B");
					_float4x4 ParentMatrix = m_pMainTransform->Get_WorldMatrix();
					pEffect->Play_Effect(&ParentMatrix);
				}
					
				else
				{
					m_Scon.iNextState = SS_JUMP_SECOND_F;
					CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Double_Jump_F");
					_float4x4 ParentMatrix = m_pMainTransform->Get_WorldMatrix();
					pEffect->Play_Effect(&ParentMatrix);
				}
				--m_iAirJumpCount;

				//if (!bInputDir[0] && !bInputDir[1] && !bInputDir[2] && !bInputDir[3])
				//{
				//	m_Scon.iNextState = SS_JUMP_SECOND_B;
				//	CEffect* pEffect =CGameInstance::GetInstance()->Get_Effect(L"Double_Jump_B");
				//	_float4x4 ParentMatrix = m_pMainTransform->Get_WorldMatrix();
				//	pEffect->Play_Effect(&ParentMatrix);
				//}
				//else
				//{
				//	m_Scon.iNextState = SS_JUMP_SECOND_F;
				//	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Double_Jump_F");
				//	_float4x4 ParentMatrix = m_pMainTransform->Get_WorldMatrix();
				//	pEffect->Play_Effect(&ParentMatrix);
				//}
				
			}
			break;
			//switch (m_Scon.iCurState)
			//{
			//case SS_JUMP_RUN:
			//case SS_JUMP_WALK:
			//	
			//default:
			//	
			//	break;
			//}
			//break;
		case Client::CPlayerGirl::INPUT_ATTACK:
			m_Scon.iNextState = IS_AIRATTACK_START;
			break;
		default:
			break;
		}
	}

	// ���� ���¸� ���� ���� ���·� ���� ���� ����
	if (m_Scon.iCurState != m_Scon.iNextState)
	{
		if (m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority)
		{
			SetUp_State();
			SetUp_Animations(false);
			// ���� ���� �� 1����
			if (CCharacter::ROT_ONSTART == m_tCurState.iRotationType)
			{
				if(0.f != XMVectorGetX(XMVector3Length(vInputDir)))
					m_pMainTransform->Set_LookDir(vInputDir);
			}
		}
	}

	// �� ������
	if (CCharacter::ROT_LOOKAT == m_tCurState.iRotationType)
	{
		if(0.f != XMVectorGetX(XMVector3Length(vInputDir)))
			m_pMainTransform->Set_LookDir(vInputDir);
	}

}

void CPlayerGirl::Tick_State(_double TimeDelta)
{
	//
	if (false == m_Scon.bAnimFinished)
	{
		_float4 vRotation;
		_float3 vMovement;

		// �ִϸ��̼� ����
		m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_Scon.TrackPos, &m_Scon.bAnimFinished);
		m_pAnimSetCom[ANIMSET_RIBBON]->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, nullptr);

		m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();
		m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();

		m_pModelCom->Invalidate_CombinedMatrices();



		// �������� �ʴ� �ִϸ��̼��� �̹� �����ӿ� �������� �������� �������� ����
		// �ִϸ��̼� �������̶� FrameRate�� �޶� �߻��ϴ� ���������� ���� ���� ����
		if(true == m_Scon.bAnimFinished && false == m_tCurState.bLoop)
		{
			XMStoreFloat3(&vMovement, XMLoadFloat3(&m_Scon.vPrevMovement) * (_float)TimeDelta);
			m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, m_pNaviCom);
		}
		else
		{
			// ��Ʈ ��� ó��(ȸ��, �̵�)
			if (true == m_tCurState.bRootMotion)
			{
				// TODO : ȸ�� ����
				//m_pMainTransform->Rotate_Quaternion(XMLoadFloat4(&vRotation));
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
			// ������ �̵�����ŭ �����̰� ���� ������ ������ ����, + TimeDelta ����
			m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, m_pNaviCom);
			XMStoreFloat3(&m_Scon.vPrevMovement, XMLoadFloat3(&vMovement) / (_float)TimeDelta);
		}
		

		// StateKey ó��
		for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
		{
			if(nullptr != m_tCurState.ppStateKeys[i])
				m_tCurState.ppStateKeys[i]->Check(m_Scon.TrackPos, this);
		}

		// �������� ���� ��� 
		if (vMovement.z < 0.f)
			m_Scon.bFalling = true;
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


	if (PS_GROUND == m_Scon.ePositionState)
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
	else if (PS_AIR == m_Scon.ePositionState)
	{
		if (fPosY - fCellHeight < 0.f )
		{
			// ������ �������� ���� �� ����ó��
			if (false == m_Scon.bFalling)
				m_pMainTransform->Set_PosY(fCellHeight);
			else
			{
				m_pMainTransform->Set_PosY(fCellHeight);

				// ���� �������� �����ϴ� ���
				if (IS_AIRATTACK_START == m_Scon.iCurState ||
					IS_AIRATTACK_LOOP == m_Scon.iCurState)
				{
					m_Scon.iNextState = IS_AIRATTACK_END;
				}
				// �Ϲ����� ���
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
	else if (PS_CLIMB == m_Scon.ePositionState)
	{

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

			// �ʻ�� 
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
				// ���� ����
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


	// FramePerSec ����
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

	// ������ ĳ���Ϳ� ������, ���۷��� ����X, ���� ���� ���O
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
}
