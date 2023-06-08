#include "stdafx.h"
#include "..\Public\UICharacter.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "ResonatorUI.h"
#include "UICam.h"

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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(addComponents()))
		return E_FAIL;

	initAnimation();

	m_pAnimSetBase->Set_RootBone(TEXT("Root"));

	m_iAnimID = UICHAR_IDLE;

	m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
	m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);

	m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(270.f));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(-999.f, 0.f, -1000.f, 1.f));

	m_pStudioTransform->Set_WorldMatrix(m_pMainTransform->Get_WorldMatrix());
	_vector vStudioPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_LOOK) * 1.5f;
	m_pStudioTransform->Set_State(CTransform::STATE_POSITION, vStudioPos);
	m_pStudioTransform->Set_Scale(_float3(20.f, 10.f, 20.f));

	return S_OK;
}

void UICharacter::Start()
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	m_pCamMovement = static_cast<CCameraMovement*>(pGI->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));

	static_cast<CResonatorUI*>(pGI->Find_GameObject(LEVEL_STATIC, L"Resonator"))->BindUIChracter(this);
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
		return;

	updateAnimationState(TimeDelta);
}

void UICharacter::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	CCameraMovement::CAM_TYPE curCamType = m_pCamMovement->GetCurrentCamType();
	if (curCamType != CCameraMovement::CAM_UI)
		return;

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

	if (FAILED(pGI->SetupSRV(STATIC_IMAGE::UI_DETAILPANEL, m_pUIShader, "g_DiffuseTexture")))
		return E_FAIL;
	m_pUIShader->Begin(10);
	m_pVIBuffer->Render();

	if (FAILED(setupShaderResource()))
		return E_FAIL;

	for (_uint i = 0; i < 6; ++i)
	{
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModel->SetUp_VertexTexture(m_pShader, "g_VertexTexture", i)))
			return E_FAIL;

		if (i == 5)
		{
			//if (FAILED(m_pEyeBurstTexture->Setup_ShaderResource(m_pShader, "g_EyeBurstTexture")))
			//	return E_FAIL;
			//if (FAILED(m_pEyeMaskTexture->Setup_ShaderResource(m_pShader, "g_EyeMaskTexture")))
			//	return E_FAIL;

			m_pShader->Begin(7);		//Burst
		}
		else
		{
			m_pShader->Begin(4);
		}

		m_pModel->Render(i);

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
	if (m_iAnimID == UICHAR_IDLE)
	{
		m_bStateBase = true;
		switch (eCurUI)
		{
		case UI_STATE:
			m_iAnimID = UICHAR_IDLE;
			m_eUIState = UI_STATE;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_WEAPON:
			m_iAnimID = UICHAR_WEAPON_START;
			m_eUIState = UI_WEAPON;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_ECHO:
			m_iAnimID = UICHAR_CHIP_START;
			m_eUIState = UI_ECHO;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_RESONANCE:
			m_iAnimID = UICHAR_RESONANT_START;
			m_eUIState = UI_RESONANCE;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_WUTHERIDE:
			m_iAnimID = UICHAR_INTEN_START;
			m_eUIState = UI_WUTHERIDE;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		}
	}
	else
	{
		m_bStateBase = false;
		switch (m_eUIState)
		{
		case UI_STATE:
			m_iAnimID = UICHAR_IDLE;
			m_eUIState = eCurUI;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_WEAPON:
			m_iAnimID = UICHAR_WEAPON_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_ECHO:
			m_iAnimID = UICHAR_CHIP_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_RESONANCE:
			m_iAnimID = UICHAR_RESONANT_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		case UI_WUTHERIDE:
			m_iAnimID = UICHAR_INTEN_END;
			m_eUIState = eCurUI;
			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
			break;
		}

		//ExitAnimation(eCurUI);
	}
}

void UICharacter::ExitAnimation(UIANIMATION eCurUI)
{
	if (m_iAnimID == UICHAR_CHIP_LOOP ||
		m_iAnimID == UICHAR_INTEN_LOOP ||
		m_iAnimID == UICHAR_INTO1_LOOP ||
		m_iAnimID == UICHAR_RESONANT_LOOP ||
		m_iAnimID == UICHAR_WEAPON_LOOP)
	{
		m_iAnimID--;
		m_eUIState = eCurUI;
		m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
		m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
	}
	else if (
		m_iAnimID == UICHAR_CHIP_START ||
		m_iAnimID == UICHAR_INTEN_START ||
		m_iAnimID == UICHAR_INTO1_START ||
		m_iAnimID == UICHAR_RESONANT_START ||
		m_iAnimID == UICHAR_WEAPON_START)
	{
		m_iAnimID -= 2; // >> 재생중에 연속으로 클릭하니까 밀림 일단 보류
		m_eUIState = eCurUI;
		m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
		m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
	}
}

void UICharacter::initAnimation()
{
	for (auto& pBone : m_pAnimSetBase->Get_Bones())
		pBone->Set_TargetBone(m_pModel->Get_BonePtr(pBone->Get_Name()));

	for (auto& pBone : m_pAnimSetRibbon->Get_Bones())
		pBone->Set_TargetBone(m_pModel->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetBase->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetBase->Get_BonePtr(pChannel->Get_TargetBoneID());

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

	for (auto& pAnim : m_pAnimSetRibbon->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetRibbon->Get_BonePtr(pChannel->Get_TargetBoneID());


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

void UICharacter::updateAnimationState(_double TimeDelta)
{
	_bool bFinished = false;

	m_pAnimSetBase->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, &bFinished);
	m_pAnimSetBase->Update_TargetBones();

	m_pAnimSetRibbon->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, &bFinished);
	m_pAnimSetRibbon->Ribbon_TargetBones();

	m_pModel->Invalidate_CombinedMatrices();

	if (m_bStateBase)
	{
		if (bFinished)
		{
			switch (m_eUIState)
			{
			case UI_STATE:
				m_iAnimID = UICHAR_IDLE;
				break;
			case UI_WEAPON:
				m_iAnimID = UICHAR_WEAPON_LOOP;
				break;
			case UI_ECHO:
				m_iAnimID = UICHAR_CHIP_LOOP;
				break;
			case UI_RESONANCE:
				m_iAnimID = UICHAR_RESONANT_LOOP;
				break;
			case UI_WUTHERIDE:
				m_iAnimID = UICHAR_INTEN_LOOP;
				break;
			}

			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
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
				m_iAnimID = UICHAR_IDLE;
				break;
			case UI_WEAPON:
				m_iAnimID = UICHAR_WEAPON_START;
				break;
			case UI_ECHO:
				m_iAnimID = UICHAR_CHIP_START;
				break;
			case UI_RESONANCE:
				m_iAnimID = UICHAR_RESONANT_START;
				break;
			case UI_WUTHERIDE:
				m_iAnimID = UICHAR_INTEN_START;
				break;
			}

			m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
			m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
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
		TEXT("Com_Shader_ModelVTF"), (CComponent**)&m_pModel)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_UI_ROVER,
		TEXT("Com_Shader_ModelAnimBase"), (CComponent**)&m_pAnimSetBase)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_UI_ROVER_RIBBON,
		TEXT("Com_Shader_ModelAnimRibbon"), (CComponent**)&m_pAnimSetRibbon)))
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
	Safe_Release(m_pModel);
	Safe_Release(m_pAnimSetBase);
	Safe_Release(m_pAnimSetRibbon);
	Safe_Release(m_pMainTransform);

	Safe_Release(m_pUIShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pStudioTransform);

}
