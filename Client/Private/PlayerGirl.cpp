#include "stdafx.h"
#include "..\Public\PlayerGirl.h"

#include "GameInstance.h"
#include "Parts.h"

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

	if (FAILED(Init_States()))
		return E_FAIL;

	if (FAILED(Init_Parts()))
		return E_FAIL;

	Init_AnimSystem();

	m_pAnimSetCom[ANIMSET_BASE]->Set_RootBone(TEXT("Root"));

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(15.f, 0.f, 0.f, 1.f));
	//m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	SetUp_Animation();

	return S_OK;
}

void CPlayerGirl::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	pStaticObject = pGame->Find_GameObject(LEVEL_GAMEPLAY, L"StaticTest");
}

void CPlayerGirl::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGame = CGameInstance::GetInstance();
	if (pGame->InputKey(DIK_RIGHT) == KEY_STATE::TAP)
	{
		++m_tStates[m_iStateID].iAnimID[ANIMSET_BASE];
		SetUp_Animation();
	}
	if (pGame->InputKey(DIK_LEFT) == KEY_STATE::TAP)
	{
		--m_tStates[m_iStateID].iAnimID[ANIMSET_BASE];
		SetUp_Animation();
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

	if (pGame->InputKey(DIK_T) == KEY_STATE::TAP)
	{
		m_tStates[m_iStateID].iAnimID[ANIMSET_RIBBON] = 132; //Run_F
		SetUp_Animation();
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
		SetUp_Animation();
	}

	if (pGame->InputKey(DIK_SPACE) == KEY_STATE::TAP)
	{
		m_bPlay = !m_bPlay;
	}

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

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_VertexTexture(m_pShaderCom, "g_VertexTexture", i)))
			return E_FAIL;

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
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

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

void CPlayerGirl::SetUp_Animation(_uint iType)
{
	Safe_AnimID();
	m_pAnimSetCom[iType]->SetUp_Animation(m_tStates[m_iStateID].iAnimID[iType], m_tStates[m_iStateID].bLerp, false);
}

void CPlayerGirl::Safe_AnimID()
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

	// For.Com_Shader_ModelAnim
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Model*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_MODEL,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	/* For.Com_AnimSet_Base */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_BASE_ACTION,
		TEXT("Com_AnimSet_Base"), (CComponent**)&m_pAnimSetCom[ANIMSET_BASE])))
		return E_FAIL;

	/* For.Com_AnimSet_Ribbon */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_PLAYERGIRL_ANIMSET_RIBBON_ACTION,
		TEXT("Com_AnimSet_Ribbon"), (CComponent**)&m_pAnimSetCom[ANIMSET_RIBBON])))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPlayerGirl::Init_States()
{
	ZeroMemory(m_tStates, sizeof(MULTISTATE) * iState_End);

	for (_uint i = 0; i < iState_End; ++i)
	{
		m_tStates[i].FramePerSec = (_float)m_pAnimSetCom[ANIMSET_BASE]->Get_Animation(0)->Get_TicksPerSecond();
	}

	return S_OK;
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

void CPlayerGirl::Set_TrackPos(_uint iType)
{
	m_pAnimSetCom[iType]->Set_TrackPos(m_TrackPos[iType]);

	m_pAnimSetCom[iType]->Play_Animation(0.0);
	m_pAnimSetCom[iType]->Update_TargetBones();
	m_pModelCom->Invalidate_CombinedMatrices();
}

void CPlayerGirl::Safe_StateID()
{
	if (m_iStateID < 0)
		m_iStateID = 0;
	else if (m_iStateID >= iState_End)
		m_iStateID = iState_End - 1;

	if (m_tStates[m_iStateID].iNextState < 0)
		m_tStates[m_iStateID].iNextState = 0;
	else if (m_tStates[m_iStateID].iNextState >= iState_End)
		m_tStates[m_iStateID].iNextState = iState_End - 1;
}

void CPlayerGirl::SetUp_Animation()
{
	SetUp_Animation(ANIMSET_BASE);
	SetUp_Animation(ANIMSET_RIBBON);
}

void CPlayerGirl::Tick_State(_double TimeDelta)
{
	_bool bAnimFinished = false;
	_bool bRibbonFinished = false;
	//
	if (true == m_bPlay)
	{
		// ImGui에서 드래그로 TrackPos 가능하도록 처리
		_double TrackPosBase, TrackPosRibbon;

		_float3 vAnimMove = m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, &TrackPosBase, &bAnimFinished);
		m_pAnimSetCom[ANIMSET_RIBBON]->Play_Animation(TimeDelta, &TrackPosRibbon, &bRibbonFinished);

		m_TrackPos[ANIMSET_BASE] = (_float)TrackPosBase;
		m_TrackPos[ANIMSET_RIBBON] = (_float)TrackPosRibbon;
		// 임시 끝

		m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();

		m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();

		m_pModelCom->Invalidate_CombinedMatrices();

		if (true == m_tStates[m_iStateID].bRootMotion)
			m_pMainTransform->Move_Anim(&vAnimMove);
	}

	if (true == bRibbonFinished)
	{
		SetUp_Animation(ANIMSET_RIBBON);
	}

	if (true == bAnimFinished)
	{
		if (true == m_tStates[m_iStateID].bLoop)
			SetUp_Animation();
		else
			m_iStateID = m_tStates[m_iStateID].iNextState;
	}
}

void CPlayerGirl::Init_AnimSystem()
{
	// Base
	vector<CAnimation*> vAnims = m_pAnimSetCom[ANIMSET_BASE]->Get_Animations();
	_uint iNumAnim = (_uint)vAnims.size();

	for (_uint i = 0; i < iNumAnim; ++i)
	{
		CAnimation* pAnim = vAnims[i];

		vector<CChannel*> vChannels = pAnim->Get_Channels();
		_uint iNumChannel = (_uint)vChannels.size();

		for (_uint j = 0; j < iNumChannel; ++j)
		{
			CBone* pBone = m_pAnimSetCom[ANIMSET_BASE]->Get_BonePtr(vChannels[j]->Get_TargetBoneID());

			const _tchar* szName = vChannels[j]->Get_Name();

			if (wcsncmp(szName, TEXT("Bip001"), 6) &&
				lstrcmp(szName, TEXT("WeaponProp01")) &&
				lstrcmp(szName, TEXT("WeaponProp02")) &&
				wcsncmp(szName, TEXT("Root"), 4))
				continue;

			if (true == pBone->Is_ChildOf(TEXT("Bip001Head"))
				|| !wcsncmp(szName, TEXT("R_Chest"), 7)
				|| !wcsncmp(szName, TEXT("L_Chest"), 7))
				continue;

			pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));

		}
	}

	// Ribbon
	vAnims = m_pAnimSetCom[ANIMSET_RIBBON]->Get_Animations();
	iNumAnim = (_uint)vAnims.size();

	for (_uint i = 0; i < iNumAnim; ++i)
	{
		CAnimation* pAnim = vAnims[i];

		vector<CChannel*> vChannels = pAnim->Get_Channels();
		_uint iNumChannel = (_uint)vChannels.size();

		for (_uint j = 0; j < iNumChannel; ++j)
		{
			const _tchar* szName = vChannels[j]->Get_Name();
			CBone* pBone = m_pAnimSetCom[ANIMSET_RIBBON]->Get_BonePtr(vChannels[j]->Get_TargetBoneID());

			//if (vChannels[j]->Get_NumKeyFrames() <= 2)
			//	continue;

			if (true == pBone->Is_ChildOf(TEXT("Hair_M_B00")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_L_lingjie01")) ||
				true == pBone->Is_ChildOf(TEXT("Piao_R_lingjie01")) ||
				true == pBone->Is_ChildOf(TEXT("skrit_L_F01")) ||
				true == pBone->Is_ChildOf(TEXT("skirt_M_B01")))
				//true == pBone->Is_ChildOf(TEXT("Piao_F01")))
			{
				pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));

			}
			else if (!lstrcmp(szName, TEXT("Hair_M_B00")) ||
				!lstrcmp(szName, TEXT("Piao_L_lingjie01")) ||
				!lstrcmp(szName, TEXT("Piao_R_lingjie01")) ||
				//!lstrcmp(szName, TEXT("Piao_L_pidai01")) ||
				//!lstrcmp(szName, TEXT("Piao_R_pidai01")) ||
				!lstrcmp(szName, TEXT("skrit_L_F01")) ||
				!lstrcmp(szName, TEXT("skirt_M_B01")))
				//!lstrcmp(szName, TEXT("Piao_F01")))
			{
				pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));
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
