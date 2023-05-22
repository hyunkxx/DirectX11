#include "stdafx.h"
#include "..\Public\TestGeneric.h"

#include "GameInstance.h"
#include "Effect.h"
#include "EffectKey.h"
#include "Parts.h"
#include "PartsKey.h"
#include "PriorityKey.h"
#include "OBBKey.h"
#include "MissileKey.h"


CTestGeneric::CTestGeneric(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CTestChar(pDevice, pContext)
{
}

CTestGeneric::CTestGeneric(const CTestGeneric & rhs)
	: CTestChar(rhs)
	, m_tDesc(rhs.m_tDesc)
{
}

HRESULT CTestGeneric::Initialize_Prototype(TESTGENERIC_DESC* pDesc)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (pDesc != nullptr)
		memcpy(&m_tDesc, pDesc, sizeof m_tDesc);

	return S_OK;
}

HRESULT CTestGeneric::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Init_States()))
		return E_FAIL;

	//if (FAILED(Init_Parts()))
	//	return E_FAIL;

	//Init_AnimSystem();

	m_pModelCom->Set_RootBone(TEXT("Root"));

	if (FAILED(Init_EffectBones()))
		return E_FAIL;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	SetUp_State();

	return S_OK;
}

void CTestGeneric::Start()
{
}

void CTestGeneric::Tick(_double TimeDelta)
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
			++m_tStates[m_iStateID].iAnimID;
			SetUp_State();
		}
		if (pGame->InputKey(DIK_LEFT) == KEY_STATE::TAP)
		{
			--m_tStates[m_iStateID].iAnimID;
			SetUp_State();
		}

		if (pGame->InputKey(DIK_UP) == KEY_STATE::TAP)
		{
			/*m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON2);
			m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON1);*/
		}

		if (pGame->InputKey(DIK_DOWN) == KEY_STATE::TAP)
		{
		/*	m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON4);
			m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON3);*/
		}

		if (pGame->InputKey(DIK_F) == KEY_STATE::TAP)
		{
			m_tStates[m_iStateID].bLoop = !m_tStates[m_iStateID].bLoop;
		}

		if (pGame->InputKey(DIK_R) == KEY_STATE::TAP)
		{
			m_TrackPos = 0.f;
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

void CTestGeneric::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	//// Parts 처리
	//for (_uint i = 0; i < PARTS_END; ++i)
	//{
	//	if (nullptr != m_Parts[i])
	//	{
	//		m_Parts[i]->LateTick(TimeDelta);
	//		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, m_Parts[i]);
	//	}
	//}

	//Effect Bones 처리
	Update_EffectBones();
}

HRESULT CTestGeneric::Render()
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

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;

		m_pShaderCom->Begin(1);
		m_pModelCom->Render(i);

	}

	return S_OK;
}

HRESULT CTestGeneric::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(Setup_ShadowShaderResource()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;

		m_pShaderCom->Begin(2);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CTestGeneric::RenderGUI()
{
}

void CTestGeneric::SetUp_Animation(_uint iType)
{
	Safe_AnimID();
	m_pModelCom->SetUp_Animation(m_tStates[m_iStateID].iAnimID, m_tStates[m_iStateID].bLerp);
}

void CTestGeneric::Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _double Duration)
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

void CTestGeneric::Shot_EffectKey(_tchar * szEffectTag, _uint EffectBoneID, _uint iEffectID, _bool bTracking)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag, EFFECT_ID(iEffectID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CTestGeneric::Safe_AnimID()
{
	if ((_int)m_pModelCom->Get_AnimCount() <= m_tStates[m_iStateID].iAnimID)
		m_tStates[m_iStateID].iAnimID = m_pModelCom->Get_AnimCount() - 1;
	else if (0 > m_tStates[m_iStateID].iAnimID)
		m_tStates[m_iStateID].iAnimID = 0;
}

void CTestGeneric::SetUp_State()
{
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

HRESULT CTestGeneric::Add_Components()
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

	return S_OK;
}

HRESULT CTestGeneric::Init_States()
{
	ZeroMemory(m_tStates, sizeof(SINGLESTATE_TOOL) * 200);

	// 로드하는 코드
	for (_int i = 0; i < 200; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, m_tDesc.szFilePath, i);
		HANDLE hFile = CreateFile(szBuffer, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			continue;


		DWORD dwByte = 0;

		SINGLESTATE tSingleState;
		ZeroMemory(&tSingleState, sizeof(SINGLESTATE));

		ReadFile(hFile, &tSingleState, sizeof(CCharacter::SINGLESTATE) - sizeof(CStateKey**), &dwByte, nullptr);

		if (0 != tSingleState.iKeyCount)
		{
			tSingleState.ppStateKeys = new CStateKey*[tSingleState.iKeyCount];
			ZeroMemory(tSingleState.ppStateKeys, sizeof(CStateKey*) * tSingleState.iKeyCount);

			for (_uint j = 0; j < tSingleState.iKeyCount; ++j)
			{
				CStateKey::BaseData tBaseData;
				ReadFile(hFile, &tBaseData, sizeof(CStateKey::BaseData), &dwByte, nullptr);

				switch (tBaseData.iType)
				{
				case CStateKey::TYPE_EFFECT:
					tSingleState.ppStateKeys[j] = CEffectKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PARTS:
					tSingleState.ppStateKeys[j] = CPartsKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PRIORITY:
					tSingleState.ppStateKeys[j] = CPriorityKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_DISSOLVE:

					break;
				case CStateKey::TYPE_OBB:
					tSingleState.ppStateKeys[j] = COBBKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_MISSILE:
					tSingleState.ppStateKeys[j] = CMissileKey::Create(m_pDevice, m_pContext, &tBaseData);
					break;
				case CStateKey::TYPE_SOUND:

					break;
				default:
					break;
				}
			}
		}
		else
			tSingleState.ppStateKeys = nullptr;

		// 읽어온 정보들을 멤버 변수에 저장
		m_tStates[i].iAnimID = (_int)tSingleState.iAnimID;
		m_tStates[i].iNextState = (_int)tSingleState.iNextState;
		m_tStates[i].iRotationType = (_int)tSingleState.iRotationType;
		m_tStates[i].FramePerSec = (_float)tSingleState.FramePerSec;
		m_tStates[i].bLoop = tSingleState.bLoop;
		m_tStates[i].bLerp = tSingleState.bLerp;
		m_tStates[i].bRootMotion = tSingleState.bRootMotion;
		m_tStates[i].bApplyCoolTime = tSingleState.bApplyCoolTime;
		m_tStates[i].bWeaponState = tSingleState.bWeaponState;
		m_tStates[i].CoolTime = (_float)tSingleState.CoolTime;
		m_tStates[i].iPhysicMoveID = (_int)tSingleState.iPhysicMoveID;
		m_tStates[i].iEnterPriority = (_int)tSingleState.iEnterPriority;
		m_tStates[i].iLeavePriority = (_int)tSingleState.iLeavePriority;
		m_tStates[i].iKeyCount = (_int)tSingleState.iKeyCount;

		m_tStates[i].ppStateKeys = tSingleState.ppStateKeys;

		CloseHandle(hFile);
	}


	return S_OK;
}

//void CTestGeneric::Init_AnimSystem()
//{
//	auto& vecAnims = m_pModelCom->Get_Animations();
//	for (auto& pAnim : vecAnims)
//	{
//		auto& vecChannels = pAnim->Get_Channels();
//		for (auto& pChannel : vecChannels)
//		{
//			pChannel->Set_Apply(true);
//		}
//	}
//
//}


HRESULT CTestGeneric::SetUp_ShaderResources()
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

HRESULT CTestGeneric::Setup_ShadowShaderResource()
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

void CTestGeneric::Set_WeaponUse(_bool bBool)
{
	//if (true == bBool)
	//{
	//	m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON1);
	//	m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON2);
	//}
	//else
	//{
	//	m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON3);
	//	m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON4);
	//}
}

void CTestGeneric::Set_TrackPos()
{
	m_pModelCom->Set_TrackPos(m_TrackPos);

	m_pModelCom->Play_Animation(0.0);
	m_pModelCom->Update_TargetBones();
	m_pModelCom->Invalidate_CombinedMatrices();
}

void CTestGeneric::Safe_StateParams()
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

void CTestGeneric::SetUp_Animation()
{
	Safe_AnimID();
	m_pModelCom->SetUp_Animation(m_tStates[m_iStateID].iAnimID, m_tStates[m_iStateID].bLerp);
}

void CTestGeneric::Tick_State(_double TimeDelta)
{
	_double TrackPos = 0.0;

	//
	if (true == m_bPlay)
	{
		_float4 vRotation;
		_float3 vMovement;

		m_pModelCom->Play_Animation(TimeDelta, &vRotation, &vMovement, &TrackPos, &m_Scon.bAnimFinished);

		m_TrackPos = (_float)TrackPos;

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
				m_tStates[m_iStateID].ppStateKeys[i]->Check(TrackPos, this);
		}

	}

	if (true == m_Scon.bAnimFinished)
	{
		for (_int i = 0; i < m_tStates[m_iStateID].iKeyCount; ++i)
		{
			if (nullptr != m_tStates[m_iStateID].ppStateKeys[i])
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

HRESULT CTestGeneric::Init_Parts()
{
	//CGameInstance* pGame = CGameInstance::GetInstance();

	//m_PartsBone[PBONE_WEAPON1] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp01"));
	//m_PartsBone[PBONE_WEAPON2] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp02"));
	//m_PartsBone[PBONE_WEAPON3] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp03"));
	//m_PartsBone[PBONE_WEAPON4] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp04"));
	//m_PartsBone[PBONE_WEAPON5] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp05"));
	//m_PartsBone[PBONE_WEAPON6] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp06"));
	//m_PartsBone[PBONE_WEAPON7] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp07"));
	//m_PartsBone[PBONE_HULU] = m_pModelCom->Get_BonePtr(TEXT("HuluProp01"));

	//// 파츠는 캐릭터에 종속적, 레퍼런스 관리X, 삭제 순서 고려O
	//CParts::PARTSDESC PartsDesc;
	//ZeroMemory(&PartsDesc, sizeof PartsDesc);
	//PartsDesc.ppParentBone = m_PartsBone;
	//PartsDesc.pParentTransform = m_pMainTransform;

	//m_Parts[PARTS_WEAPON_MAIN] = static_cast<CParts*>(pGame->Clone_GameObject(m_tDesc.iPartsID[PARTS_WEAPON_MAIN], &PartsDesc));
	//m_Parts[PARTS_WEAPON_MAIN]->Set_Parent(PBONE_WEAPON7);
	//m_Parts[PARTS_WEAPON_SUB] = static_cast<CParts*>(pGame->Clone_GameObject(m_tDesc.iPartsID[PARTS_WEAPON_SUB], &PartsDesc));
	//m_Parts[PARTS_WEAPON_SUB]->Set_Parent(PBONE_WEAPON7);
	//m_Parts[PARTS_HULU] = static_cast<CParts*>(pGame->Clone_GameObject(OBJECT::PARTS_HULU_0, &PartsDesc));
	//m_Parts[PARTS_HULU]->Set_Parent(PBONE_HULU);


	return S_OK;
}

HRESULT CTestGeneric::Init_EffectBones()
{
	//NONE은 걍 월드 매트릭스 던짐
	//m_EffectBones[EBONE_SPINE2] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine2"));
	//m_EffectBones[EBONE_WEAPON01] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp01"));
	//m_EffectBones[EBONE_WEAPON02] = m_pModelCom->Get_BonePtr(TEXT("WeaponProp02"));
	//m_EffectBones[EBONE_LHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001LHand"));
	//m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));
	return S_OK;
}

void CTestGeneric::Update_EffectBones()
{
	memcpy(&m_EffectBoneMatrices[EBONE_NONE], &m_pMainTransform->Get_WorldMatrix(), sizeof(_float4x4));


	for (_uint i = 1; i < EBONE_END; ++i)
	{
		XMStoreFloat4x4(&m_EffectBoneMatrices[i], XMLoadFloat4x4(&m_EffectBones[i]->Get_CombinedTransfromationMatrix())
			* XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
	}
}

CTestGeneric * CTestGeneric::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TESTGENERIC_DESC* pDesc)
{
	CTestGeneric* pInstance = new CTestGeneric(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX("Failed to Create : CTestGeneric");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CTestGeneric::Clone(void * pArg)
{
	CTestGeneric* pInstance = new CTestGeneric(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTestGeneric");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestGeneric::Free()
{
	__super::Free();

	if (true == m_bClone)
	{
		for (_int i = 0; i < 200; ++i)
		{
			if (nullptr != m_tStates[i].ppStateKeys)
			{
				for (_int j = 0; j < m_tStates[i].iKeyCount; ++j)
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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

void CTestGeneric::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}

void CTestGeneric::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CTestGeneric::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
