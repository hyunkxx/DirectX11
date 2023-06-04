#include "stdafx.h"
#include "..\Public\UICharacter.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"


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

	return S_OK;
}

void UICharacter::Start()
{
}

void UICharacter::PreTick(_double TimeDelta)
{
}

void UICharacter::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	updateAnimationState(TimeDelta);
}

void UICharacter::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
}

HRESULT UICharacter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

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
			m_pShader->Begin(3);
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

	if (bFinished)
	{
		// 현재 UI 상태에 따라 다음 애니메이션 결정,
		switch (m_iUIState)
		{
		// ex) Weapon 탭이 선택된 상태라면 다음 애니메이션을 WEAPON LOOP로 함?
		case 2:
			m_iAnimID = UICHAR_WEAPON_LOOP;
			break;
		default:
			break;
		}


		m_pAnimSetBase->SetUp_Animation(m_iAnimID, true);
		m_pAnimSetRibbon->SetUp_Animation(m_iAnimID, true);
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


}
