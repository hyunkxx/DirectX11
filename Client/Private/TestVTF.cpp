#include "stdafx.h"
#include "..\Public\TestVTF.h"

#include "GameInstance.h"
#include "Effect.h"
#include "EffectKey.h"
#include "Parts.h"
#include "PartsKey.h"
#include "PriorityKey.h"
#include "OBBKey.h"

CTestVTF::CTestVTF(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CTestChar(pDevice, pContext)
{
}

CTestVTF::CTestVTF(const CTestVTF & rhs)
	: CTestChar(rhs)
	, m_tDesc(rhs.m_tDesc)
{
}

HRESULT CTestVTF::Initialize_Prototype(TESTVTF_DESC* pDesc)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	if (pDesc != nullptr)
		memcpy(&m_tDesc, pDesc, sizeof m_tDesc);

	return S_OK;
}

HRESULT CTestVTF::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Init_States()))
		return E_FAIL;

	if (FAILED(Init_Parts()))
		return E_FAIL;

	Init_AnimSystem();
	m_pAnimSetCom[ANIMSET_BASE]->Set_RootBone(TEXT("Root"));

	if (FAILED(Init_EffectBones()))
		return E_FAIL;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	SetUp_State();

	return S_OK;
}

void CTestVTF::Start()
{
}

void CTestVTF::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGame = CGameInstance::GetInstance();

	if (pGame->InputKey(DIK_I) == KEY_STATE::TAP)
	{
		m_bInputLock = !m_bInputLock;
	}
	
	if (m_bInputLock)
	{
		if (pGame->InputKey(DIK_RIGHT) == KEY_STATE::TAP)
		{
			++m_tStates[m_iStateID].iAnimID[ANIMSET_BASE];
			SetUp_State();
		}
		if (pGame->InputKey(DIK_LEFT) == KEY_STATE::TAP)
		{
			--m_tStates[m_iStateID].iAnimID[ANIMSET_BASE];
			SetUp_State();
		}

		if (pGame->InputKey(DIK_UP) == KEY_STATE::TAP)
		{
			m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON2);
			m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON1);
		}

		if (pGame->InputKey(DIK_DOWN) == KEY_STATE::TAP)
		{
			m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON4);
			m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON3);
		}

		if (pGame->InputKey(DIK_F) == KEY_STATE::TAP)
		{
			m_tStates[m_iStateID].bLoop = !m_tStates[m_iStateID].bLoop;
		}

		if (pGame->InputKey(DIK_R) == KEY_STATE::TAP)
		{
			m_TrackPos[ANIMSET_BASE] = 0.f;
			m_TrackPos[ANIMSET_RIBBON] = 0.f;
			m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


			// StateKey 처리
			for (_int i = 0; i < m_tStates[m_iStateID].iKeyCount; ++i)
			{
				if (nullptr != m_tStates[m_iStateID].ppStateKeys[i])
					m_tStates[m_iStateID].ppStateKeys[i]->Reset();
			}

			SetUp_State();
		}

		if (pGame->InputKey(DIK_SPACE) == KEY_STATE::TAP)
		{
			m_bPlay = !m_bPlay;
		}
	}

	Tick_State(TimeDelta);

	// Parts 처리
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
			m_Parts[i]->Tick(TimeDelta);
	}
}

void CTestVTF::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	// Parts 처리
	for (_uint i = 0; i < PARTS_END; ++i)
	{
		if (nullptr != m_Parts[i])
		{
			m_Parts[i]->LateTick(TimeDelta);
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, m_Parts[i]);
		}
	}

	//Effect Bones 처리
	Update_EffectBones();
}

HRESULT CTestVTF::Render()
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

HRESULT CTestVTF::RenderShadow()
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

void CTestVTF::RenderGUI()
{
}

void CTestVTF::SetUp_Animation(_uint iType)
{
	Safe_AnimID();
	m_pAnimSetCom[iType]->SetUp_Animation(m_tStates[m_iStateID].iAnimID[iType], m_tStates[m_iStateID].bLerp);
}

void CTestVTF::Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration)
{
	// Weapon Main / Sub
	if(0 == iParts)
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
	// Hulu
	else if (1 == iParts)
	{
		// 안보이게 한다.
		if (0 == iState)
		{

		}
		//보이게 한다.
		else if (1 == iState)
		{
			
		}
	}
}

void CTestVTF::Shot_EffectKey(_tchar * szEffectTag, _uint EffectBoneID , _uint iEffectID , _bool bTracking)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag , EFFECT_ID(iEffectID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

		pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CTestVTF::Safe_AnimID()
{
	if ((_int)m_pAnimSetCom[ANIMSET_BASE]->Get_AnimCount() <= m_tStates[m_iStateID].iAnimID[ANIMSET_BASE])
		m_tStates[m_iStateID].iAnimID[ANIMSET_BASE] = m_pAnimSetCom[ANIMSET_BASE]->Get_AnimCount() - 1;
	else if (0 > m_tStates[m_iStateID].iAnimID[ANIMSET_BASE])
		m_tStates[m_iStateID].iAnimID[ANIMSET_BASE] = 0;

	if ((_int)m_pAnimSetCom[ANIMSET_RIBBON]->Get_AnimCount() <= m_tStates[m_iStateID].iAnimID[ANIMSET_RIBBON])
		m_tStates[m_iStateID].iAnimID[ANIMSET_RIBBON] = m_pAnimSetCom[ANIMSET_RIBBON]->Get_AnimCount() - 1;
	else if (0 > m_tStates[m_iStateID].iAnimID[ANIMSET_RIBBON])
		m_tStates[m_iStateID].iAnimID[ANIMSET_RIBBON] = 0;
}

void CTestVTF::SetUp_State()
{
	// 무기 위치 잡기
	if (0 == m_tStates[m_iStateID].bWeaponState)
		Set_WeaponUse(false);
	else
		Set_WeaponUse(true);

	//PhysicMove

	if (false == m_tStates[m_iStateID].bRootMotion)
	{
		PHYSICMOVE tPhysicMove = StatePhysics[m_tStates[m_iStateID].iPhysicMoveID];

		if (true == tPhysicMove.bInitMovement)
			XMStoreFloat3(&m_vMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
		else
			m_vMovement = m_vPrevMovement;
	}

	SetUp_Animation();

}

HRESULT CTestVTF::Add_Components()
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

	// For.Com_Shader_ModelAnim
	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Model*/
	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, m_tDesc.iModelID,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AnimSet_Base */
	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, m_tDesc.iAnimSetID[ANIMSET_BASE],
		TEXT("Com_AnimSet_Base"), (CComponent**)&m_pAnimSetCom[ANIMSET_BASE])))
		return E_FAIL;

	/* For.Com_AnimSet_Ribbon */
	if (FAILED(__super::Add_Component(LEVEL_ANIMTOOL, m_tDesc.iAnimSetID[ANIMSET_RIBBON],
		TEXT("Com_AnimSet_Ribbon"), (CComponent**)&m_pAnimSetCom[ANIMSET_RIBBON])))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestVTF::Init_States()
{
	ZeroMemory(m_tStates, sizeof(MULTISTATE_TOOL) * 200);

	// 로드하는 코드
	for (_int i = 0; i < 200; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, m_tDesc.szFilePath, i);
		HANDLE hFile = CreateFile(szBuffer, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			continue;


		DWORD dwByte = 0;

		MULTISTATE tMultiState;
		ZeroMemory(&tMultiState, sizeof(MULTISTATE));

		ReadFile(hFile, &tMultiState, sizeof(CCharacter::MULTISTATE) - sizeof(CStateKey**), &dwByte, nullptr);

		if (0 != tMultiState.iKeyCount)
		{
			tMultiState.ppStateKeys = new CStateKey*[tMultiState.iKeyCount];
			ZeroMemory(tMultiState.ppStateKeys, sizeof(CStateKey*) * tMultiState.iKeyCount);

			for (_uint j = 0; j < tMultiState.iKeyCount; ++j)
			{
				CStateKey::BaseData tBaseData;
				ReadFile(hFile, &tBaseData, sizeof(CStateKey::BaseData), &dwByte, nullptr);

				switch (tBaseData.iType)
				{
				case CStateKey::TYPE_EFFECT:
					tMultiState.ppStateKeys[j] = CEffectKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PARTS:
					tMultiState.ppStateKeys[j] = CPartsKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PRIORITY:
					tMultiState.ppStateKeys[j] = CPriorityKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_DISSOLVE:

					break;
				case CStateKey::TYPE_OBB:
					tMultiState.ppStateKeys[j] = COBBKey::Create(m_pDevice, m_pContext, &tBaseData);
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
			tMultiState.ppStateKeys = nullptr;

		// 읽어온 정보들을 멤버 변수에 저장
		m_tStates[i].iAnimID[CCharacter::ANIMSET_BASE] = (_int)tMultiState.iAnimID[CCharacter::ANIMSET_BASE];
		m_tStates[i].iAnimID[CCharacter::ANIMSET_RIBBON] = (_int)tMultiState.iAnimID[CCharacter::ANIMSET_RIBBON];
		m_tStates[i].iNextState = (_int)tMultiState.iNextState;
		m_tStates[i].iRotationType = (_int)tMultiState.iRotationType;
		m_tStates[i].FramePerSec = (_float)tMultiState.FramePerSec;
		m_tStates[i].bLoop = tMultiState.bLoop;
		m_tStates[i].bLerp = tMultiState.bLerp;
		m_tStates[i].bRootMotion = tMultiState.bRootMotion;
		m_tStates[i].bApplyCoolTime = tMultiState.bApplyCoolTime;
		m_tStates[i].bWeaponState = tMultiState.bWeaponState;
		m_tStates[i].CoolTime = (_float)tMultiState.CoolTime;
		m_tStates[i].iPhysicMoveID = (_int)tMultiState.iPhysicMoveID;
		m_tStates[i].iEnterPriority = (_int)tMultiState.iEnterPriority;
		m_tStates[i].iLeavePriority = (_int)tMultiState.iLeavePriority;
		m_tStates[i].iKeyCount = (_int)tMultiState.iKeyCount;

		m_tStates[i].ppStateKeys = tMultiState.ppStateKeys;

		CloseHandle(hFile);
	}


	return S_OK;
}

HRESULT CTestVTF::SetUp_ShaderResources()
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

HRESULT CTestVTF::Setup_ShadowShaderResource()
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

void CTestVTF::Set_WeaponUse(_bool bBool)
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

void CTestVTF::Set_TrackPos(_uint iType)
{
	m_pAnimSetCom[iType]->Set_TrackPos(m_TrackPos[iType]);

	m_pAnimSetCom[iType]->Play_Animation(0.0);
	m_pAnimSetCom[iType]->Update_TargetBones();
	m_pModelCom->Invalidate_CombinedMatrices();
}

void CTestVTF::Safe_StateParams()
{
	if (m_iStateID < 0)
		m_iStateID = 0;
	else if (m_iStateID >= 200)
		m_iStateID = 199;

	if (m_tStates[m_iStateID].iNextState < 0)
		m_tStates[m_iStateID].iNextState = 0;
	else if (m_tStates[m_iStateID].iNextState >= 200)
		m_tStates[m_iStateID].iNextState = 199;

	if (m_tStates[m_iStateID].iRotationType < 0)
		m_tStates[m_iStateID].iRotationType = 0;
	else if (m_tStates[m_iStateID].iRotationType >= CTestChar::ROT_END)
		m_tStates[m_iStateID].iRotationType = CTestChar::ROT_END - 1;

	if (m_tStates[m_iStateID].iPhysicMoveID < 0)
		m_tStates[m_iStateID].iPhysicMoveID = 0;
	else if (m_tStates[m_iStateID].iPhysicMoveID >= CCharacter::SP_END)
		m_tStates[m_iStateID].iPhysicMoveID = CCharacter::SP_END - 1;

}

void CTestVTF::SetUp_Animation()
{
	SetUp_Animation(ANIMSET_BASE);
	SetUp_Animation(ANIMSET_RIBBON);
}

void CTestVTF::Tick_State(_double TimeDelta)
{
	_bool bRibbonFinished = false;
	_double BaseTrackPos = 0.0, RibbonTrackPos = 0.0;

	//
	if (true == m_bPlay)
	{
		_float4 vRotation;
		_float3 vMovement;

		m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, &vRotation, &vMovement, &BaseTrackPos, &m_Scon.bAnimFinished);
		m_pAnimSetCom[ANIMSET_RIBBON]->Play_Animation(TimeDelta, nullptr, nullptr, &RibbonTrackPos, &bRibbonFinished);

		m_TrackPos[ANIMSET_BASE] = (_float)BaseTrackPos;
		m_TrackPos[ANIMSET_RIBBON] = (_float)RibbonTrackPos;

		m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();
		m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();

		m_pModelCom->Invalidate_CombinedMatrices();

		// 루트 모션 or not
		if (true == m_tStates[m_iStateID].bRootMotion)
		{
			m_pMainTransform->Move_Anim(&vMovement);
			//m_pMainTransform->Rotate_Quaternion(XMLoadFloat4(&vRotation));
			m_vPrevMovement = vMovement;
		}
		else
		{
			// 가속/감속 체크하는 코드
			const PHYSICMOVE& PhysicMove = StatePhysics[m_tStates[m_iStateID].iPhysicMoveID];
			if (false == PhysicMove.bConstant)
			{
				_vector vMove = XMLoadFloat3(&m_vPrevMovement);

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
				XMStoreFloat3(&vMovement, XMLoadFloat3(&m_vMovement) * (_float)TimeDelta);
			
			m_pMainTransform->Move_Anim(&vMovement);

			m_vPrevMovement = vMovement;
		}

		// StateKey 처리
		for (_int i = 0; i < m_tStates[m_iStateID].iKeyCount; ++i)
		{
			if (nullptr != m_tStates[m_iStateID].ppStateKeys[i])
				m_tStates[m_iStateID].ppStateKeys[i]->Check(BaseTrackPos, this);
		}

	}

	if (true == bRibbonFinished)
	{
		SetUp_Animation(ANIMSET_RIBBON);
	}

	if (true == m_Scon.bAnimFinished)
	{
		for (_int i = 0; i < m_tStates[m_iStateID].iKeyCount; ++i)
		{
			if(nullptr != m_tStates[m_iStateID].ppStateKeys[i])
				m_tStates[m_iStateID].ppStateKeys[i]->Reset();
		}
			

		if (true == m_tStates[m_iStateID].bLoop)
			SetUp_State();
		else
		{
			if (NO_ANIM != m_tStates[m_iStateID].iNextState)
			{
				m_iStateID = m_tStates[m_iStateID].iNextState;
				SetUp_State();
			}
			else
				m_bPlay = false;
		}
			
	}
}

void CTestVTF::Init_AnimSystem()
{
	// Base
	for (auto& pBone : m_pAnimSetCom[ANIMSET_BASE]->Get_Bones())
		pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));

	for(auto& pAnim : m_pAnimSetCom[ANIMSET_BASE]->Get_Animations())
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
				pChannel->Set_Apply(false);

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

			if (!(true == pBone->Is_ChildOf(TEXT("Hair_M_B00")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_L_lingjie01")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_R_lingjie01")) ||
				true == pBone->Is_ChildOf(TEXT("skrit_L_F02")) ||
				true == pBone->Is_ChildOf(TEXT("skirt_M_B02")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_F01"))))
			{
				pChannel->Set_Apply(false);
			}
			else if (!(!lstrcmp(szChannelName, TEXT("Hair_M_B00")) ||
				!lstrcmp(szChannelName, TEXT("Piao_L_lingjie01")) ||
				!lstrcmp(szChannelName, TEXT("Piao_R_lingjie01")) ||
				!lstrcmp(szChannelName, TEXT("Piao_L_pidai01")) ||
				!lstrcmp(szChannelName, TEXT("Piao_R_pidai01")) ||
				!lstrcmp(szChannelName, TEXT("skrit_L_F01")) ||
				!lstrcmp(szChannelName, TEXT("skirt_M_B01"))) 
				/*!lstrcmp(szChannelName, TEXT("Piao_F01"))*/)
			{
				pChannel->Set_Apply(false);
			}
		}
	}
}

HRESULT CTestVTF::Init_Parts()
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

	m_Parts[PARTS_WEAPON_MAIN] = static_cast<CParts*>(pGame->Clone_GameObject(m_tDesc.iPartsID[PARTS_WEAPON_MAIN], &PartsDesc));
	m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON7);
	m_Parts[PARTS_WEAPON_SUB] = static_cast<CParts*>(pGame->Clone_GameObject(m_tDesc.iPartsID[PARTS_WEAPON_SUB], &PartsDesc));
	m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON7);
	m_Parts[PARTS_HULU] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_HULU_0, &PartsDesc));
	m_Parts[PARTS_HULU]->Set_Parent(PBONE_HULU);


	return S_OK;
}

HRESULT CTestVTF::Init_EffectBones()
{
	//NONE은 걍 월드 매트릭스 던짐
	m_EffectBones[EBONE_SPINE2] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine2"));
	m_EffectBones[EBONE_WEAPON01] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp01"));
	m_EffectBones[EBONE_WEAPON02] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp02"));
	m_EffectBones[EBONE_LHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001LHand"));
	m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));
	return S_OK;
}

void CTestVTF::Update_EffectBones()
{
	memcpy(&m_EffectBoneMatrices[EBONE_NONE], &m_pMainTransform->Get_WorldMatrix(), sizeof(_float4x4));


	for (_uint i = 1; i < EBONE_END; ++i)
	{
		XMStoreFloat4x4(&m_EffectBoneMatrices[i], XMLoadFloat4x4(&m_EffectBones[i]->Get_CombinedTransfromationMatrix())
			* XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
	}
}

CTestVTF * CTestVTF::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TESTVTF_DESC* pDesc)
{
	CTestVTF* pInstance = new CTestVTF(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX("Failed to Create : CTestVTF");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTestVTF::Clone(void * pArg)
{
	CTestVTF* pInstance = new CTestVTF(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTestVTF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestVTF::Free()
{
	__super::Free();

	if(true == m_bClone)
	{ 
		for (_int i = 0; i < 200; ++i)
		{
			if (nullptr != m_tStates[i].ppStateKeys)
			{
				for (_int j = 0; j <  m_tStates[i].iKeyCount; ++j)
				{
					Safe_Release(m_tStates[i].ppStateKeys[j]);
				}
				Safe_Delete_Array(m_tStates[i].ppStateKeys);
			}
		}
	}
	

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

void CTestVTF::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}

void CTestVTF::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CTestVTF::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
