#include "stdafx.h"
#include "..\Public\TestPlayer.h"

#include "GameInstance.h"

int CTestPlayer::s_SpawnCount = 0;
CTestPlayer::CTestPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTestPlayer::CTestPlayer(const CTestPlayer & rhs)
	: CGameObject(rhs)
{
	m_iSpawnedID = s_SpawnCount++;
}

HRESULT CTestPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	// Model Init
	Init_Models();

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	SetUp_SkelAnimation();
	SetUp_HairAnimation();

	return S_OK;
}

void CTestPlayer::Start()
{
}

void CTestPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGame = CGameInstance::GetInstance();
	if (pGame->InputKey(DIK_PGUP) == KEY_STATE::TAP)
	{
		if (++m_iSkelAnimID > 34)
			m_iSkelAnimID = 34;
		SetUp_SkelAnimation();
	}
	if (pGame->InputKey(DIK_PGDN) == KEY_STATE::TAP)
	{
		if (--m_iSkelAnimID < 0)
			m_iSkelAnimID = 0;
		SetUp_SkelAnimation();
	}

	if (m_iSpawnedID == 0)
	{
		if (pGame->InputKey(DIK_UPARROW) == KEY_STATE::HOLD)
		{
			m_pMainTransform->MoveForward(TimeDelta);
		}
		if (pGame->InputKey(DIK_DOWNARROW) == KEY_STATE::HOLD)
		{
			m_pMainTransform->MoveBackward(TimeDelta);
		}

		if (pGame->InputKey(DIK_LEFTARROW) == KEY_STATE::HOLD)
		{
			m_pMainTransform->MoveLeft(TimeDelta);
		}
		if (pGame->InputKey(DIK_RIGHTARROW) == KEY_STATE::HOLD)
		{
			m_pMainTransform->MoveRight(TimeDelta);
		}
	}

	Tick_State(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

void CTestPlayer::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CTestPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// SkelParts
	for (_uint j = 0; j < SKEL_END; ++j)
	{
		_uint iNumMeshes = m_pSkelParts[j]->Get_NumMeshes();

		if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
			return E_FAIL;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pSkelParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_DiffuseTexture", i, MyTextureType_DIFFUSE);
			m_pSkelParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_NormalTexture", i, MyTextureType_NORMALS);

			m_pSkelParts[j]->SetUp_BoneMatrices(m_pShaderCom[SHADER_MODELANIM], "g_BoneMatrix", i);
			
			m_pShaderCom[SHADER_MODELANIM]->Begin(0);
			m_pSkelParts[j]->Render(i);
		}
	}

	// HairParts
	for (_uint j = 0; j < HAIR_END; ++j)
	{
		if (nullptr == m_pHairParts[j])
			continue;
		
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMMatrixRotationX(XMConvertToRadians(-90.f))
			* XMLoadFloat4x4(&m_pHairParts[j]->Get_HangBone()->Get_CombinedTransfromationMatrix())
			* XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

			if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_WorldMatrix", &WorldMatrix)))
				return E_FAIL;

		_uint iNumMeshes = m_pHairParts[j]->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pHairParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_DiffuseTexture", i, MyTextureType_DIFFUSE);
			m_pHairParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_NormalTexture", i, MyTextureType_NORMALS);

			m_pHairParts[j]->SetUp_BoneMatrices(m_pShaderCom[SHADER_MODELANIM], "g_BoneMatrix", i);

			m_pShaderCom[SHADER_MODELANIM]->Begin(0);

			m_pHairParts[j]->Render(i);
		}
	}

	// StaticParts
	for (_uint j = 0; j < STATIC_END; ++j)
	{
		if (nullptr == m_pStaticParts[j])
			continue;

		if (STATIC_HEADWEAR == j)
		{
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(XMConvertToRadians(180.f))
				* XMMatrixRotationX(XMConvertToRadians(-90.f))
				* XMLoadFloat4x4(&m_pStaticParts[j]->Get_HangBone()->Get_CombinedTransfromationMatrix())
				* XMMatrixRotationY(XMConvertToRadians(180.f))
				* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

			if (FAILED(m_pShaderCom[SHADER_MODEL]->SetMatrix("g_WorldMatrix", &WorldMatrix)))
				return E_FAIL;
		}

		_uint iNumMeshes = m_pStaticParts[j]->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pStaticParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODEL], "g_DiffuseTexture", i, MyTextureType_DIFFUSE);
			m_pStaticParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODEL], "g_NormalTexture", i, MyTextureType_NORMALS);

			m_pShaderCom[SHADER_MODEL]->Begin(0);

			m_pStaticParts[i]->Render(i);
		}
	}

	return S_OK;
}

HRESULT CTestPlayer::RenderShadow()
{
	//if (ComputeCameraLength() >= 40.f)
	//	return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShdowShaderResources()))
		return E_FAIL;

	// SkelParts
	for (_uint j = 0; j < SKEL_END; ++j)
	{
		_uint iNumMeshes = m_pSkelParts[j]->Get_NumMeshes();

		if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
			return E_FAIL;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pSkelParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_DiffuseTexture", i, MyTextureType_DIFFUSE);
			m_pSkelParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_NormalTexture", i, MyTextureType_NORMALS);

			m_pSkelParts[j]->SetUp_BoneMatrices(m_pShaderCom[SHADER_MODELANIM], "g_BoneMatrix", i);

			//Shadow
			CGameInstance* pGameInstance = CGameInstance::GetInstance();

			m_pShaderCom[SHADER_MODELANIM]->Begin(1);
			m_pSkelParts[j]->Render(i);
		}
	}

	// HairParts
	for (_uint j = 0; j < HAIR_END; ++j)
	{
		if (nullptr == m_pHairParts[j])
			continue;

		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMMatrixRotationX(XMConvertToRadians(-90.f))
			* XMLoadFloat4x4(&m_pHairParts[j]->Get_HangBone()->Get_CombinedTransfromationMatrix())
			* XMMatrixRotationY(XMConvertToRadians(180.f))
			* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

		if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_WorldMatrix", &WorldMatrix)))
			return E_FAIL;

		_uint iNumMeshes = m_pHairParts[j]->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pHairParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_DiffuseTexture", i, MyTextureType_DIFFUSE);
			m_pHairParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODELANIM], "g_NormalTexture", i, MyTextureType_NORMALS);

			m_pHairParts[j]->SetUp_BoneMatrices(m_pShaderCom[SHADER_MODELANIM], "g_BoneMatrix", i);

			m_pShaderCom[SHADER_MODELANIM]->Begin(1);

			m_pHairParts[j]->Render(i);
		}
	}

	// StaticParts
	for (_uint j = 0; j < STATIC_END; ++j)
	{
		if (nullptr == m_pStaticParts[j])
			continue;

		if (STATIC_HEADWEAR == j)
		{
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(XMConvertToRadians(180.f))
				* XMMatrixRotationX(XMConvertToRadians(-90.f))
				* XMLoadFloat4x4(&m_pStaticParts[j]->Get_HangBone()->Get_CombinedTransfromationMatrix())
				* XMMatrixRotationY(XMConvertToRadians(180.f))
				* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

			if (FAILED(m_pShaderCom[SHADER_MODEL]->SetMatrix("g_WorldMatrix", &WorldMatrix)))
				return E_FAIL;
		}

		_uint iNumMeshes = m_pStaticParts[j]->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pStaticParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODEL], "g_DiffuseTexture", i, MyTextureType_DIFFUSE);
			m_pStaticParts[j]->SetUp_ShaderMaterialResource(m_pShaderCom[SHADER_MODEL], "g_NormalTexture", i, MyTextureType_NORMALS);

			m_pShaderCom[SHADER_MODEL]->Begin(1);

			m_pStaticParts[i]->Render(i);
		}
	}

	return S_OK;
}

void CTestPlayer::RenderGUI()
{
}

HRESULT CTestPlayer::Add_Components()
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

	/* For.Com_AnimSet*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_ANIMSET,
		TEXT("Com_AnimSet"), (CComponent**)&m_pAnimSet)))
		return E_FAIL;

	/* For.Com_SkelPart_Body*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_BODY,
		TEXT("Com_SkelPart_Body"), (CComponent**)&m_pSkelParts[SKEL_BODY])))
		return E_FAIL;

	/* For.Com_SkelPart_Face*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_FACE,
		TEXT("Com_SkelPart_Face"), (CComponent**)&m_pSkelParts[SKEL_FACE])))
		return E_FAIL;

	/* For.Com_SkelPart_Eye*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_EYE,
		TEXT("Com_SkelPart_Eye"), (CComponent**)&m_pSkelParts[SKEL_EYE])))
		return E_FAIL;

	/* For.Com_SkelPart_Eyebrow*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_EYEBROW,
		TEXT("Com_SkelPart_Eyebrow"), (CComponent**)&m_pSkelParts[SKEL_EYEBROW])))
		return E_FAIL;

	/* For.Com_SkelPart_Eyelash*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_EYELASH,
		TEXT("Com_SkelPart_Eyelash"), (CComponent**)&m_pSkelParts[SKEL_EYELASH])))
		return E_FAIL;

	/* For.Com_SkelPart_Suppressor*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_SUPPRESSOR,
		TEXT("Com_SkelPart_Suppressor"), (CComponent**)&m_pSkelParts[SKEL_SUPPRESSOR])))
		return E_FAIL;

	/* For.Com_HairPart_Front*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_HAIR_F,
		TEXT("Com_HairPart_Front"), (CComponent**)&m_pHairParts[HAIR_FRONT])))
		return E_FAIL;

	/* For.Com_HairPart_Back*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_HAIR_B,
		TEXT("Com_HairPart_Back"), (CComponent**)&m_pHairParts[HAIR_BACK])))
		return E_FAIL;

	/* For.Com_StaticPart_Headwear*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SMODEL::SMD_HEADWEAR,
		TEXT("Com_StaticPart_Headwear"), (CComponent**)&m_pStaticParts[STATIC_HEADWEAR])))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SHADER::MODEL,
		TEXT("Com_Shader_Model"), (CComponent**)&m_pShaderCom[SHADER_MODEL])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom[SHADER_MODELANIM])))
		return E_FAIL;

	return S_OK;
}

void CTestPlayer::Init_Models()
{
	for (_uint i = 0; i < SKEL_END; ++i)
	{
		m_pSkelParts[i]->Register_SkelModel(m_pAnimSet->Get_Bones());
	}

	for (_uint i = 0; i < HAIR_END; ++i)
	{
		if (nullptr != m_pHairParts[i])
			m_pHairParts[i]->Set_HangBone(m_pSkelParts[SKEL_FACE]->Get_BonePtr(TEXT("hair_hang")));
	}

	if(nullptr != m_pStaticParts[STATIC_HEADWEAR])
		m_pStaticParts[STATIC_HEADWEAR]->Set_HangBone(m_pSkelParts[SKEL_FACE]->Get_BonePtr(TEXT("hair_hang")));;
}

HRESULT CTestPlayer::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();


	if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom[SHADER_MODEL]->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom[SHADER_MODEL]->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestPlayer::SetUp_ShdowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom[SHADER_MODEL]->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom[SHADER_MODEL]->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetRawValue("g_LightDir", &pGameInstance->GetLightDirection(), sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom[SHADER_MODELANIM]->SetRawValue("g_LightPos", &pGameInstance->GetLightPosition(), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

void CTestPlayer::SetUp_SkelAnimation()
{
	m_pAnimSet->SetUp_Animation(m_iSkelAnimID, true, false);
}

void CTestPlayer::SetUp_HairAnimation()
{
	for (_uint i = 0; i < HAIR_END; ++i)
	{
		if (nullptr != m_pHairParts[i])
			if (m_iHairAnimID >= m_pHairParts[i]->Get_AnimCount())
				m_pHairParts[i]->SetUp_Animation(0, true, false);
			else 
				m_pHairParts[i]->SetUp_Animation(m_iHairAnimID, true, false);
	}
}

void CTestPlayer::Tick_State(_double TimeDelta)
{
	_bool bAnimFinished = false;
	m_pAnimSet->Play_Animation(TimeDelta, nullptr, &bAnimFinished);

	if (true == bAnimFinished)
	{
		SetUp_SkelAnimation();
	}

	for(_uint i = 0; i < SKEL_END; ++i)
	{
		m_pSkelParts[i]->Follow_Animation();
	}

	
	for (_uint i = 0; i < HAIR_END; ++i)
	{
		if (nullptr != m_pHairParts[i])
		{
			m_pHairParts[i]->Play_Animation(TimeDelta, nullptr, nullptr);
		}
	}
}

CTestPlayer * CTestPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestPlayer* pInstance = new CTestPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTestPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTestPlayer::Clone(void * pArg)
{
	CTestPlayer* pInstance = new CTestPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTestPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestPlayer::Free()
{
	__super::Free();

	for (_uint i = 0; i < SKEL_END; ++i)
		Safe_Release(m_pSkelParts[i]);

	for (_uint i = 0; i < HAIR_END; ++i)
	{
		if (nullptr != m_pHairParts[i])
			Safe_Release(m_pHairParts[i]);
	}
		
	for (_uint i = 0; i < STATIC_END; ++i)
	{
		if (nullptr != m_pStaticParts[i])
			Safe_Release(m_pStaticParts[i]);
	}

	Safe_Release(m_pAnimSet);
	Safe_Release(m_pMainTransform);

	for (_uint i = 0; i < SHADER_END; ++i)
	{
		Safe_Release(m_pShaderCom[i]);
	}

	Safe_Release(m_pRendererCom);
}
