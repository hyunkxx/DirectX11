#include "stdafx.h"
#include "..\Public\UICharacter.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "ResonatorUI.h"
#include "UICam.h"
#include "UIWeapon.h"

UICharacter::UICharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

UICharacter::UICharacter(const UICharacter & rhs)
	: CGameObject(rhs)
{
}

HRESULT UICharacter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT UICharacter::Initialize(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(addComponents()))
		return E_FAIL;

	initAnimation();

	for (_uint i = 0; i < MODEL_END; ++i)
	{
		m_pAnimSetBase[i]->Set_RootBone(TEXT("Root"));
		m_iAnimID[i] = UICHAR_IDLE;
	}

	for (_uint i = 0; i < MODEL_END; ++i)
	{
		m_pAnimSetBase[i]->SetUp_Animation(m_iAnimID[i], true);
		m_pAnimSetRibbon[i]->SetUp_Animation(m_iAnimID[i], true);
	}

	m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(270.f));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-999.f, 0.f, -1000.f, 1.f));

	m_pWeapons = static_cast<CUIWeapon*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UIWeapon"));
	m_pWeapons->SetupParentTransform(m_pMainTransform);

	m_pWeapons->SetupWeaponProp(0, 0, m_pModel[0]->Get_BonePtr(TEXT("WeaponProp01")));
	m_pWeapons->SetupWeaponProp(0, 1, m_pModel[0]->Get_BonePtr(TEXT("WeaponProp02")));
	m_pWeapons->SetupWeaponProp(0, 2, m_pModel[0]->Get_BonePtr(TEXT("WeaponProp03")));
	m_pWeapons->SetupWeaponProp(0, 3, m_pModel[0]->Get_BonePtr(TEXT("WeaponProp04")));

	return S_OK;
}

void UICharacter::Start()
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	m_pCamMovement = static_cast<CCameraMovement*>(pGI->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));
	static_cast<CResonatorUI*>(pGI->Find_GameObject(LEVEL_STATIC, L"Resonator"))->BindUIChracter(this);
	m_pUICam = static_cast<CUICam*>(m_pCamMovement->GetCamera(CCameraMovement::CAM_UI));
}

void UICharacter::PreTick(_double TimeDelta)
{
}

void UICharacter::Tick(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);

	CCameraMovement::CAM_TYPE curCamType = m_pCamMovement->GetCurrentCamType();
	if (curCamType != CCameraMovement::CAM_UI)
	{
		for (_uint i = 0; i < 3; ++i)
		{
			m_iAnimID[i] = UICHAR_IDLE;
			m_iRibbonID[i] = UICHAR_RIB_IDLE;
			m_eUIState = UI_STATE;
			m_pAnimSetBase[i]->SetUp_Animation(m_iAnimID[i], true);
			m_pAnimSetRibbon[i]->SetUp_Animation(m_iRibbonID[i], true);
		}

		return;
	}
	
	updateAnimationState(TimeDelta);
}

void UICharacter::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	CCameraMovement::CAM_TYPE curCamType = m_pCamMovement->GetCurrentCamType();
	if (curCamType != CCameraMovement::CAM_UI)
		return;

	CGameInstance* pGI = CGameInstance::GetInstance();
	_float4x4 vViewInv = pGI->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW);
	m_pStudioTransform->Set_WorldMatrix(vViewInv);
	_vector vStudioPos = m_pStudioTransform->Get_State(CTransform::STATE_POSITION) + m_pStudioTransform->Get_State(CTransform::STATE_LOOK) * 2.5f;

	m_pStudioTransform->Set_Scale(_float3(2.1f, 2.1f, 3.8f));
	m_pStudioTransform->Set_State(CTransform::STATE_POSITION, vStudioPos);

	if (m_iAnimID[m_iCurModel] == UICHAR_RESONANT_START)
		m_pUICam->SetMove(true);
	else
	{
		if (m_iAnimID[m_iCurModel] != UICHAR_RESONANT_LOOP)
			m_pUICam->SetMove(false);
	}

	if (m_iAnimID[m_iCurModel] == UICHAR_WEAPON_START ||
		m_iAnimID[m_iCurModel] == UICHAR_WEAPON_LOOP ||
		m_iAnimID[m_iCurModel] == UICHAR_WEAPON_END)
	{
		m_pWeapons->SetWeaponState(true);
	}
	else
	{
		m_pWeapons->SetWeaponState(false);
	}

	if (m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
}

HRESULT UICharacter::Render()
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (FAILED(__super::Render()))
		return E_FAIL;

	// Studio
	if (FAILED(m_pUIShader->SetMatrix("g_WorldMatrix", &m_pStudioTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pUIShader->SetMatrix("g_ViewMatrix", &pGI->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pUIShader->SetMatrix("g_ProjMatrix", &pGI->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;
	
	_float fValue = 1.f;
	if (FAILED(m_pUIShader->SetRawValue("g_fTimeAcc", &fValue, sizeof(_float))))
		return E_FAIL;

	//if (FAILED(pGI->SetupSRV(STATIC_IMAGE::UI_DETAILPANEL, m_pUIShader, "g_DiffuseTexture")))
	//	return E_FAIL;
	if (FAILED(pGI->SetupSRV(STATIC_IMAGE::UI_DRAGON_BACK, m_pUIShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pUIShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(setupShaderResource()))
		return E_FAIL;

	_uint iMeshCount;
	if (m_iCurModel == YANGYANG)
		iMeshCount = m_pModel[YANGYANG]->Get_NumMeshes() - 2;
	else
		iMeshCount = 6;

	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if (FAILED(m_pModel[m_iCurModel]->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModel[m_iCurModel]->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;
		if (FAILED(m_pModel[m_iCurModel]->SetUp_VertexTexture(m_pShader, "g_VertexTexture", i)))
			return E_FAIL;

		if (i == iMeshCount - 1)
		{
			if (FAILED(m_pModel[m_iCurModel]->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i + 1, MyTextureType_DIFFUSE)))
				return E_FAIL;

			if (m_iCurModel == YANGYANG)
				m_pShader->Begin(16);
			else
				m_pShader->Begin(7);		//Burst
		}
		else
		{
			m_pShader->Begin(4);
		}

		m_pModel[m_iCurModel]->Render(i);

	}

	return S_OK;
}

HRESULT UICharacter::RenderShadow()
{
	return S_OK;
}

void UICharacter::RenderGUI()
{
}

void UICharacter::SetAnimation(UIANIMATION eCurUI)
{
	m_eUIState = m_eUIState;

	if (m_iAnimID[m_iCurModel] == UICHAR_IDLE)
	{
		m_bStateBase = true;
		switch (eCurUI)
		{
		case UI_STATE:
			m_iAnimID[m_iCurModel] = UICHAR_IDLE;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_IDLE;
			m_eUIState = UI_STATE;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_WEAPON:
			m_iAnimID[m_iCurModel] = UICHAR_WEAPON_START;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_WEAPON_START;
			m_eUIState = UI_WEAPON;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_ECHO:
			m_iAnimID[m_iCurModel] = UICHAR_CHIP_START;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_CHIP_START;
			m_eUIState = UI_ECHO;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_RESONANCE:
			m_iAnimID[m_iCurModel] = UICHAR_RESONANT_START;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_RESONANT_START;
			m_eUIState = UI_RESONANCE;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_WUTHERIDE:
			m_iAnimID[m_iCurModel] = UICHAR_INTO1_START;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_INTO1_START;
			m_eUIState = UI_WUTHERIDE;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		}
	}
	else
	{
		m_bStateBase = false;
		switch (m_eUIState)
		{
		case UI_STATE:
			m_iAnimID[m_iCurModel] = UICHAR_IDLE;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_IDLE;
			m_eUIState = eCurUI;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_WEAPON:
			m_iAnimID[m_iCurModel] = UICHAR_WEAPON_END;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_WEAPON_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_ECHO:
			m_iAnimID[m_iCurModel] = UICHAR_CHIP_END;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_CHIP_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_RESONANCE:
			m_iAnimID[m_iCurModel] = UICHAR_RESONANT_END;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_RESONANT_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		case UI_WUTHERIDE:
			m_iAnimID[m_iCurModel] = UICHAR_INTO1_END;
			m_iRibbonID[m_iCurModel] = UICHAR_RIB_INTO1_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
			break;
		}
	}
	
}

void UICharacter::ChangeCharacter(MODEL eModelType)
{
	m_iCurModel = eModelType;

	if(m_eUIState == UI_WEAPON)
		m_pWeapons->SetDissolveAmount(1.f);

	m_pWeapons->SetupWeaponProp(m_iCurModel, 0, m_pModel[m_iCurModel]->Get_BonePtr(TEXT("WeaponProp01")));
	m_pWeapons->SetupWeaponProp(m_iCurModel, 1, m_pModel[m_iCurModel]->Get_BonePtr(TEXT("WeaponProp02")));
	m_pWeapons->SetupWeaponProp(m_iCurModel, 2, m_pModel[m_iCurModel]->Get_BonePtr(TEXT("WeaponProp03")));
	m_pWeapons->SetupWeaponProp(m_iCurModel, 3, m_pModel[m_iCurModel]->Get_BonePtr(TEXT("WeaponProp04")));
	
	switch (m_eUIState)
	{
	case UI_STATE:
		m_iAnimID[m_iCurModel] = UICHAR_IDLE;
		m_iRibbonID[m_iCurModel] = UICHAR_RIB_IDLE;
		break;
	case UI_WEAPON:
		m_iAnimID[m_iCurModel] = UICHAR_WEAPON_START;
		m_iRibbonID[m_iCurModel] = UICHAR_RIB_WEAPON_START;
		break;
	case UI_ECHO:
		m_iAnimID[m_iCurModel] = UICHAR_CHIP_START;
		m_iRibbonID[m_iCurModel] = UICHAR_RIB_CHIP_START;
		break;
	case UI_RESONANCE:
		m_iAnimID[m_iCurModel] = UICHAR_RESONANT_START;
		m_iRibbonID[m_iCurModel] = UICHAR_RIB_RESONANT_START;
		break;
	case UI_WUTHERIDE:
		m_iAnimID[m_iCurModel] = UICHAR_INTO1_START;
		m_iRibbonID[m_iCurModel] = UICHAR_RIB_INTO1_START;
		break;
	}

	m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
	m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
}

void UICharacter::initAnimation()
{
	initRoverAnimation();
	initYangyangAnimation();
	initChixiagAnimation();
}

void UICharacter::initRoverAnimation()
{
	for (auto& pBone : m_pAnimSetBase[ROVER]->Get_Bones())
		pBone->Set_TargetBone(m_pModel[ROVER]->Get_BonePtr(pBone->Get_Name()));

	for (auto& pBone : m_pAnimSetRibbon[ROVER]->Get_Bones())
		pBone->Set_TargetBone(m_pModel[ROVER]->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetBase[ROVER]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetBase[ROVER]->Get_BonePtr(pChannel->Get_TargetBoneID());

			if ((wcsncmp(szChannelName, TEXT("Bip001"), 6) &&
				lstrcmp(szChannelName, TEXT("WeaponProp01")) &&
				lstrcmp(szChannelName, TEXT("WeaponProp02")) &&
				wcsncmp(szChannelName, TEXT("Root"), 4)) ||
				pBone->Is_ChildOf(TEXT("Bip001Head")))
				pChannel->Set_Apply(false);
			else
				pChannel->Set_Apply(true);
		}
	}

	for (auto& pAnim : m_pAnimSetRibbon[ROVER]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetRibbon[ROVER]->Get_BonePtr(pChannel->Get_TargetBoneID());


			if (!wcsncmp(szChannelName, TEXT("Root"), 4) ||
				!wcsncmp(szChannelName, TEXT("Bip001"), 6) ||
				!wcsncmp(szChannelName, TEXT("Weapon"), 6) ||
				!wcsncmp(szChannelName, TEXT("R2"), 2) ||
				!wcsncmp(szChannelName, TEXT("Hulu"), 4))
				pChannel->Set_Apply(false);
			else
				pChannel->Set_Apply(true);
		}
	}
}

void UICharacter::initYangyangAnimation()
{
	for (auto& pBone : m_pAnimSetBase[YANGYANG]->Get_Bones())
		pBone->Set_TargetBone(m_pModel[YANGYANG]->Get_BonePtr(pBone->Get_Name()));
	
	for (auto& pAnim : m_pAnimSetBase[YANGYANG]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		if (!lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_Start")))
			pAnim->Set_Duration(10.0);

		if (!lstrcmp(szAnimName, TEXT("R2T1YangyangMd10011.ao|AirAttack_Start_2")))
			pAnim->Set_Duration(30.0);


		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetBase[YANGYANG]->Get_BonePtr(pChannel->Get_TargetBoneID());

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
	for (auto& pBone : m_pAnimSetRibbon[YANGYANG]->Get_Bones())
		pBone->Set_TargetBone(m_pModel[YANGYANG]->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetRibbon[YANGYANG]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetRibbon[YANGYANG]->Get_BonePtr(pChannel->Get_TargetBoneID());

			/*if (pChannel->Get_NumKeyFrames() <= 2)
			continue;*/

			// spine ÀÚ½Ä Bone
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
}

void UICharacter::initChixiagAnimation()
{
	for (auto& pBone : m_pAnimSetBase[CHIXIA]->Get_Bones())
		pBone->Set_TargetBone(m_pModel[CHIXIA]->Get_BonePtr(pBone->Get_Name()));

	for (auto& pBone : m_pAnimSetRibbon[CHIXIA]->Get_Bones())
		pBone->Set_TargetBone(m_pModel[CHIXIA]->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetBase[CHIXIA]->Get_Animations())
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
			CBone* pBone = m_pAnimSetBase[CHIXIA]->Get_BonePtr(pChannel->Get_TargetBoneID());

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
	for (auto& pBone : m_pAnimSetBase[CHIXIA]->Get_Bones())
	{
		const _tchar* szBonename = pBone->Get_Name();
		CBone* pTargetBone = m_pModel[CHIXIA]->Get_BonePtr(szBonename);
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


	for (auto& pAnim : m_pAnimSetRibbon[CHIXIA]->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetRibbon[CHIXIA]->Get_BonePtr(pChannel->Get_TargetBoneID());

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
}

void UICharacter::updateAnimationState(_double TimeDelta)
{
	_bool bFinished = false;

	m_pAnimSetBase[m_iCurModel]->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, &bFinished);
	m_pAnimSetBase[m_iCurModel]->Update_TargetBones();

	m_pAnimSetRibbon[m_iCurModel]->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, nullptr);
	m_pAnimSetRibbon[m_iCurModel]->Ribbon_TargetBones();

	m_pModel[m_iCurModel]->Invalidate_CombinedMatrices();

	if (m_bStateBase)
	{
		if (bFinished)
		{
			switch (m_eUIState)
			{
			case UI_STATE:
				m_iAnimID[m_iCurModel] = UICHAR_IDLE;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_IDLE;
				break;
			case UI_WEAPON:
				m_iAnimID[m_iCurModel] = UICHAR_WEAPON_LOOP;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_WEAPON_LOOP;
				break;
			case UI_ECHO:
				m_iAnimID[m_iCurModel] = UICHAR_CHIP_LOOP;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_CHIP_LOOP;
				break;
			case UI_RESONANCE:
				m_iAnimID[m_iCurModel] = UICHAR_RESONANT_LOOP;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_RESONANT_LOOP;
				break;
			case UI_WUTHERIDE:
				m_iAnimID[m_iCurModel] = UICHAR_INTO1_LOOP;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_INTO1_LOOP;
				break;
			}

			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], false);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], false);
		}
	}
	else
	{
		if (bFinished)
		{
			m_bStateBase = true;
			switch (m_eUIState)
			{
			case UI_STATE:
				m_iAnimID[m_iCurModel] = UICHAR_IDLE;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_IDLE;
				break;
			case UI_WEAPON:
				m_iAnimID[m_iCurModel] = UICHAR_WEAPON_START;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_WEAPON_START;
				break;
			case UI_ECHO:
				m_iAnimID[m_iCurModel] = UICHAR_CHIP_START;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_CHIP_START;
				break;
			case UI_RESONANCE:
				m_iAnimID[m_iCurModel] = UICHAR_RESONANT_START;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_RESONANT_START;
				break;
			case UI_WUTHERIDE:
				m_iAnimID[m_iCurModel] = UICHAR_INTO1_START;
				m_iRibbonID[m_iCurModel] = UICHAR_RIB_INTO1_START;
				break;
			}

			m_pAnimSetBase[m_iCurModel]->SetUp_Animation(m_iAnimID[m_iCurModel], true);
			m_pAnimSetRibbon[m_iCurModel]->SetUp_Animation(m_iRibbonID[m_iCurModel], true);
		}
	}
}

HRESULT UICharacter::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(540.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
	_uint nCurrentLevel = pGM->GetCurrentLevel();

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_PLAYERGIRL_MODEL,
		TEXT("model_rover"), (CComponent**)&m_pModel[ROVER])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_UI_ROVER,
		TEXT("baseanim_rover"), (CComponent**)&m_pAnimSetBase[ROVER])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_UI_ROVER_RIBBON,
		TEXT("robbone_rover"), (CComponent**)&m_pAnimSetRibbon[ROVER])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_YANGYANG_MODEL,
		TEXT("model_yangyang"), (CComponent**)&m_pModel[YANGYANG])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_UI_YANGYANG,
		TEXT("baseanim_yang"), (CComponent**)&m_pAnimSetBase[YANGYANG])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_UI_YANGYANG_RIBBON,
		TEXT("robbone_yang"), (CComponent**)&m_pAnimSetRibbon[YANGYANG])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_CHIXIA_MODEL,
		TEXT("model_chixia"), (CComponent**)&m_pModel[CHIXIA])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_UI_CHIXIA,
		TEXT("baseanim_chixia"), (CComponent**)&m_pAnimSetBase[CHIXIA])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_UI_CHIXIA_RIBBON,
		TEXT("robbone_chixia"), (CComponent**)&m_pAnimSetRibbon[CHIXIA])))
		return E_FAIL;

	// Studio
	ZeroMemory(&TransformDesc, sizeof TransformDesc);
	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(540.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("com_buffer_transform"), (CComponent**)&m_pStudioTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI_SUB,
		TEXT("com_shader"), (CComponent**)&m_pUIShader)))
		return E_FAIL;

	return S_OK;
}

HRESULT UICharacter::setupShaderResource()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;

}

UICharacter * UICharacter::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	UICharacter* pInstance = new UICharacter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : UICharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * UICharacter::Clone(void * pArg)
{
	UICharacter* pInstance = new UICharacter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : UICharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void UICharacter::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMainTransform);

	Safe_Release(m_pUIShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pStudioTransform);

	if (IsClone())
	{
		Safe_Release(m_pModel[0]);
		Safe_Release(m_pAnimSetBase[0]);
		Safe_Release(m_pAnimSetRibbon[0]);

		Safe_Release(m_pModel[1]);
		Safe_Release(m_pAnimSetBase[1]);
		Safe_Release(m_pAnimSetRibbon[1]);

		Safe_Release(m_pModel[2]);
		Safe_Release(m_pAnimSetBase[2]);
		Safe_Release(m_pAnimSetRibbon[2]);
	}

}
