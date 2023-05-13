#include "stdafx.h"
#include "..\Public\Lobby_Rock.h"

#include "GameInstance.h"
#include "GameMode.h"

CLobby_Rock::CLobby_Rock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

	ZeroMemory(&m_SMap_Obejct_Desc, sizeof(SMAP_OBJECT_DESC));
}

CLobby_Rock::CLobby_Rock(const CLobby_Rock & rhs)
	: CGameObject(rhs)
{
	memcpy(&m_SMap_Obejct_Desc, &rhs.m_SMap_Obejct_Desc, sizeof(SMAP_OBJECT_DESC));
}

HRESULT CLobby_Rock::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Rock::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//ZeroMemory(&m_SMap_Obejct_Desc, sizeof(SMAP_OBJECT_DESC));

	if (nullptr != pArg)
		memcpy(&m_SMap_Obejct_Desc, pArg, sizeof(SMAP_OBJECT_DESC));

	if (FAILED(Add_Components()))
		return E_FAIL;

	SetUp_State();

	m_iShaderPass_ID = { 6 };

	m_iDiffuseTex_ID = { m_SMap_Obejct_Desc.iDiffuseTexID };

	return S_OK;
}

void CLobby_Rock::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLobby_Rock::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC, this);
}

HRESULT CLobby_Rock::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		//if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			//return E_FAIL;

		m_pDiffuseTexture[m_iDiffuseTex_ID]->Setup_ShaderResource(m_pShaderCom, "g_DiffuseTexture");

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		m_pShaderCom->Begin(m_iShaderPass_ID);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CLobby_Rock::SetUp_State()
{
	if (0.0f >= m_SMap_Obejct_Desc.SObjectDesc.vS.x)
		m_SMap_Obejct_Desc.SObjectDesc.vS.x = 1.0f;
	if (0.0f >= m_SMap_Obejct_Desc.SObjectDesc.vS.y)
		m_SMap_Obejct_Desc.SObjectDesc.vS.y = 1.0f;
	if (0.0f >= m_SMap_Obejct_Desc.SObjectDesc.vS.z)
		m_SMap_Obejct_Desc.SObjectDesc.vS.z = 1.0f;

	m_pMainTransform->Set_Scale(m_SMap_Obejct_Desc.SObjectDesc.vS);
	m_pMainTransform->SetRotationXYZ(m_SMap_Obejct_Desc.SObjectDesc.vA);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_SMap_Obejct_Desc.SObjectDesc.vP));
}

HRESULT CLobby_Rock::ModelType_Detection()
{
	m_eLobbyRock_ModelType = (LOBBYROCK_MODELTYPE)m_SMap_Obejct_Desc.iModelTypeID;

	switch (m_eLobbyRock_ModelType)
	{
	case CLobby_Rock::LOBBYROCK_0:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_LOBBY_ROCK_0,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CLobby_Rock::LOBBYROCK_1:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_LOBBY_ROCK_1,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CLobby_Rock::LOBBYROCK_2:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_LOBBY_ROCK_2,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CLobby_Rock::LOBBYROCK_3:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_LOBBY_ROCK_3,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CLobby_Rock::LOBBYROCK_4:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_LOBBY_ROCK_4,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CLobby_Rock::LOBBYROCK_5:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_LOBBY_ROCK_5,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CLobby_Rock::LOBBYROCK_6:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_LOBBY_ROCK_0,
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CLobby_Rock::Add_Components()
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
	m_pMainTransform->Set_Scale(_float3(1.0f, 1.0f, 1.0f));

	/* For.Com_Shader */ // MODEL_INSTANCE MODEL
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(ModelType_Detection()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_1,
		TEXT("com_texture_d_1"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_2,
		TEXT("com_texture_d_2"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_2])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_3,
		TEXT("com_texture_d_3"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_3])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXTURE::ROCK_D_4,
		TEXT("com_texture_d_4"), (CComponent**)&m_pDiffuseTexture[ROCK_DIFFUSE_KINDS::RD_4])))
		return E_FAIL;

	return S_OK;
}

HRESULT CLobby_Rock::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CLobby_Rock * CLobby_Rock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLobby_Rock*			pInstance = new CLobby_Rock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLobby_Rock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobby_Rock::Clone(void * pArg)
{
	CLobby_Rock*			pInstance = new CLobby_Rock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Fialed to Cloned : CLobby_Rock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby_Rock::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for (_uint i = 0; i < ROCK_DIFFUSE_KINDS::RD_END; ++i)
		Safe_Release(m_pDiffuseTexture[i]);

}
