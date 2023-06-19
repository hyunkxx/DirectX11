#include "stdafx.h"
#include "..\Public\NPC_Chixia.h"

#include "GameInstance.h"
#include "GameMode.h"
#include "SoundKey.h"
#include "UI_Tip.h"

CNPC_Chixia::CNPC_Chixia(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CNPC_Chixia::CNPC_Chixia(const CNPC_Chixia & rhs)
	: CCharacter(rhs)
{
}

HRESULT CNPC_Chixia::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Chixia::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 애니메이션 연결
	Init_States(m_pDevice, m_pContext);
	Init_AnimSystem();


	// 루트모션용 본찾기
	m_pAnimSetCom[ANIMSET_BASE]->Set_RootBone(TEXT("Root"));

	// 초기위치 설정
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(170.025f, 16.875f, 131.380f, 1.f));
	m_pNaviCom->Set_CurrentIndex(264);

	return S_OK;
}

void CNPC_Chixia::Start()
{
	m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tStates[0].iAnimID[ANIMSET_BASE], false);
	m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tStates[0].iAnimID[ANIMSET_RIBBON], false);

	CGameInstance* pGame = CGameInstance::GetInstance();
	m_UITip = static_cast<CUI_Tip*>(pGame->Find_GameObject(LEVEL_ANYWHERE, L"UI_Tip"));

}

void CNPC_Chixia::PreTick(_double TimeDelta)
{
}

void CNPC_Chixia::Tick(_double TimeDelta)
{
	_bool bAnimFinished;
	_double Progress;
	m_pAnimSetCom[ANIMSET_BASE]->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, &bAnimFinished, &Progress);
	m_pAnimSetCom[ANIMSET_RIBBON]->Update_RibbonAnimation(Progress);

	m_pAnimSetCom[ANIMSET_BASE]->Update_TargetBones();
	m_pAnimSetCom[ANIMSET_RIBBON]->Ribbon_TargetBones();

	m_pModelCom->Invalidate_CombinedMatrices();

	if (true == bAnimFinished)
	{
		if(STATE_STAND1 == m_eState)
		{
			m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tStates[0].iAnimID[ANIMSET_BASE], false);
			m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tStates[0].iAnimID[ANIMSET_RIBBON], false);
		}
		else if (STATE_STAND1_ACTION == m_eState)
		{
			// 캐릭터 획득 처리
			SetState(DISABLE);
		}
	}
}

void CNPC_Chixia::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGame = CGameInstance::GetInstance();

	if (pGame->InputKey(DIK_5) == KEY_STATE::TAP)
	{
		m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tStates[0].iAnimID[ANIMSET_BASE], false);
		m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tStates[0].iAnimID[ANIMSET_RIBBON], false);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
	pGame->AddCollider(m_pCollider);
}

HRESULT CNPC_Chixia::Render()
{
	if (false == m_bRender)
		return S_OK;

	CGameInstance* pGame = CGameInstance::GetInstance();

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


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
		}
		else
		{
			if (m_bDissolve)
				m_pShaderCom->Begin(13);
			else
				m_pShaderCom->Begin(4);
		}

		m_pModelCom->Render(i);
		
	}

	return S_OK;
}

HRESULT CNPC_Chixia::RenderShadow()
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

void CNPC_Chixia::RenderGUI()
{
	
}

HRESULT CNPC_Chixia::Add_Components()
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

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.5f, 0.f };
	CollDesc.vExtents = { 2.5f, 2.5f, 2.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.4f, 0.4f, 0.4f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_MoveCollider"), (CComponent**)&m_pMoveCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CNPC_Chixia::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(MULTISTATE) * 2);

	// 로드하는 코드
	for (_int i = 0; i < 2; ++i)
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


	return;
}

void CNPC_Chixia::Release_States()
{
	for (_int i = 0; i < 2; ++i)
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

HRESULT CNPC_Chixia::SetUp_ShaderResources()
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

HRESULT CNPC_Chixia::Setup_ShadowShaderResource()
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

void CNPC_Chixia::Init_AnimSystem()
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
	for (_uint i = 0; i < 2; ++i)
	{
		m_pAnimSetCom[ANIMSET_BASE]->Get_Animation(m_tStates[i].iAnimID[ANIMSET_BASE])->Set_TicksPerSecond(m_tStates[i].FramePerSec);
		m_pAnimSetCom[ANIMSET_RIBBON]->Get_Animation(m_tStates[i].iAnimID[ANIMSET_RIBBON])->Set_TicksPerSecond(m_tStates[i].FramePerSec);
	}

}

CNPC_Chixia * CNPC_Chixia::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPC_Chixia* pInstance = new CNPC_Chixia(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CNPC_Chixia");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPC_Chixia::Clone(void * pArg)
{
	CNPC_Chixia* pInstance = new CNPC_Chixia(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CNPC_Chixia");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Chixia::Free()
{
	__super::Free();

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
	Safe_Release(m_pMoveCollider);
}

void CNPC_Chixia::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	m_UITip->SetTipIndex(CUI_Tip::CHAR_CHIXIA);
}

void CNPC_Chixia::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());

	if (CCharacter::CT_PLAYER == pOpponent->Get_CollType())
	{
		CGameInstance* pGame = CGameInstance::GetInstance();
		CGameMode* pGM = CGameMode::GetInstance();
		
		// UI 버튼 출력 처리


		if (pGame->InputKey(DIK_F) == KEY_STATE::TAP)
		{
			m_pAnimSetCom[ANIMSET_BASE]->SetUp_Animation(m_tStates[1].iAnimID[ANIMSET_BASE], false);
			m_pAnimSetCom[ANIMSET_RIBBON]->SetUp_Animation(m_tStates[1].iAnimID[ANIMSET_RIBBON], false);
			pGM->Set_Chixia(true);
		}
	}

}

void CNPC_Chixia::OnCollisionExit(CCollider * src, CCollider * dest)
{
}

