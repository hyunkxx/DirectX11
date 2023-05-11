#include "stdafx.h"
#include "..\Public\Lobby_Character.h"

#include "GameInstance.h"
#include "GameMode.h"

#include "Effect.h"

CLobbyCharacter::CLobbyCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CLobbyCharacter::CLobbyCharacter(const CLobbyCharacter & rhs)
	: CGameObject(rhs)
	, m_eState(rhs.m_eState)
	, m_iModelID(rhs.m_iModelID)
	, m_iAnimID(rhs.m_iAnimID)
	, m_iModelType(rhs.m_iModelType)
{
}

HRESULT CLobbyCharacter::Initialize_Prototype(_uint iModelID, _uint iAnimID, _uint iModelType)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iModelID = iModelID;
	m_iAnimID = iAnimID;
	m_iModelType = iModelType;

	return S_OK;
}

HRESULT CLobbyCharacter::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Init_AnimSystem();

	m_pAnimSetCom->SetUp_Animation(m_eState, true);

	switch (m_iModelType)
	{
	case LEFT_MODEL:
		m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(-90.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(121.8f, 1.5f, 87.f, 1.f));
		break;
	case RIGHT_MODEL:
		m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(90.f));
		m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(124.2f, 1.5f, 87.f, 1.f));
		break;
	}

	// Lobby_Player_Particle_Effect
	CEffect* pEffect= CGameInstance::GetInstance()->Get_Effect(L"Loby_Particle_Effect_01");
	_float4x4 WorldMatirx = m_pMainTransform->Get_WorldMatrix();
	pEffect->Play_Effect(&WorldMatirx, false);

	m_pEyeBone = m_pModelCom->Get_BonePtr(L"Bip001_Eye_Bone_L");
	
	return S_OK;
}

void CLobbyCharacter::Start()
{
}

void CLobbyCharacter::Tick(_double TimeDelta)
{
	Tick_State(TimeDelta);

}

void CLobbyCharacter::LateTick(_double TimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
}

HRESULT CLobbyCharacter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	int iPass;
	if (10.f <= ComputeCameraLength())
		iPass = 3;
	else
		iPass = 4;

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

		if (m_bOnMoused)
		{
			m_pShaderCom->Begin(9);
			m_pModelCom->Render(i);
		}
	}

	return S_OK;
}

HRESULT CLobbyCharacter::RenderShadow()
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

void CLobbyCharacter::RenderGUI()
{
}

void CLobbyCharacter::PlaySelectedAnimation()
{
	m_eState = STATE_SELECTED;
	m_pAnimSetCom->SetUp_Animation(m_eState, true);

	// ÀÓ½Ã ÀÌÆåÆ® ³¢¿ö³Ö±â
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(L"Loby_Particle_Effect_02");
	_float4x4 WorldMatirx = m_pMainTransform->Get_WorldMatrix();
	pEffect->Play_Effect(&WorldMatirx, false);

	pEffect = CGameInstance::GetInstance()->Get_Effect(L"Test_Loby_Effect");
	pEffect->Play_Effect(&WorldMatirx, false);

}

void CLobbyCharacter::SetMouseInRect(_bool bValue)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (!m_bOnMoused && bValue)
	{
		pGameInstance->PlaySoundEx(L"Intro_Click.wav", SOUND_CHANNEL::VFX, VOLUME_VFX);
	}

	m_bOnMoused = bValue;
}

_vector CLobbyCharacter::GetEyePosition()
{
	return (XMLoadFloat4x4(&m_pEyeBone->Get_CombinedTransfromationMatrix()) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix())).r[3];
}

HRESULT CLobbyCharacter::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::EYE_BURST,
		TEXT("Com_Eye_Burst_Texture"), (CComponent**)&m_pEyeBurstTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::EYE_MASK,
		TEXT("Com_Eye_Mask_Texture"), (CComponent**)&m_pEyeMaskTexture)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Component(LEVEL_LOGO, m_iModelID,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AnimSet_Base */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, m_iAnimID,
		TEXT("Com_AnimSet_Base"), (CComponent**)&m_pAnimSetCom)))
		return E_FAIL;

	return S_OK;

}

void CLobbyCharacter::Init_AnimSystem()
{
	for (auto& pBone : m_pAnimSetCom->Get_Bones())
		pBone->Set_TargetBone(m_pModelCom->Get_BonePtr(pBone->Get_Name()));

	for (auto& pAnim : m_pAnimSetCom->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();
			CBone* pBone = m_pAnimSetCom->Get_BonePtr(pChannel->Get_TargetBoneID());

			pChannel->Set_Apply(true);
		}
	}
}

void CLobbyCharacter::Tick_State(_double TimeDelta)
{
	_bool bFinished = false;
	m_pAnimSetCom->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, &bFinished);
	m_pAnimSetCom->Update_TargetBones();
	m_pModelCom->Invalidate_CombinedMatrices();

	if(bFinished)

		m_pAnimSetCom->SetUp_Animation(m_eState, true);
}

HRESULT CLobbyCharacter::SetUp_ShaderResources()
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

HRESULT CLobbyCharacter::Setup_ShadowShaderResource()
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

CLobbyCharacter * CLobbyCharacter::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iModelID, _uint iAnimID, _uint iModelType)
{
	CLobbyCharacter* pInstance = new CLobbyCharacter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iModelID, iAnimID, iModelType)))
	{
		MSG_BOX("Failed to Create : CLobbyCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLobbyCharacter::Clone(void * pArg)
{
	CLobbyCharacter* pInstance = new CLobbyCharacter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLobbyCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobbyCharacter::Free()
{
	__super::Free();
	Safe_Release(m_pAnimSetCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pEyeBurstTexture);
	Safe_Release(m_pEyeMaskTexture);
}