#include "stdafx.h"
#include "..\Public\ModelTest_Dynamic.h"

#include "GameInstance.h"

CModelTest_Dynamic::CModelTest_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CModelTest_Dynamic::CModelTest_Dynamic(const CModelTest_Dynamic & rhs)
	: CGameObject(rhs)
{
}

HRESULT CModelTest_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelTest_Dynamic::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pMainTransform->Set_Scale(_float3(0.01f, 0.01f, 0.01f));
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(3.f, 0.f, 3.f, 1.f));

	SetUp_Animation();

	return S_OK;
}

void CModelTest_Dynamic::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	pStaticObject = pGame->Find_GameObject(LEVEL_GAMEPLAY, L"StaticTest");
}

void CModelTest_Dynamic::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_bool bAnimFinished = false;
	m_pModelCom->Play_Animation(TimeDelta, nullptr, &bAnimFinished);
	
	CGameInstance* pGame = CGameInstance::GetInstance();
	if (pGame->InputKey(DIK_UP) == KEY_STATE::TAP)
	{
		if (++m_iAnimID > 2)
			m_iAnimID = 2;
		SetUp_Animation();
	}
	if (pGame->InputKey(DIK_DOWN) == KEY_STATE::TAP)
	{
		if (--m_iAnimID < 0)
			m_iAnimID = 0;
		SetUp_Animation();
	}

	if (pGame->InputKey(DIK_DOWN) == KEY_STATE::TAP)
	{
		pStaticObject->SetState(STATE::DESTROY);
	}

	if (true == bAnimFinished)
	{
		SetUp_Animation();
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

}

void CModelTest_Dynamic::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CModelTest_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE);

		m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CModelTest_Dynamic::Add_Components()
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, DMODEL::DMD_TEST,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, SHADER::MODELANIM,
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelTest_Dynamic::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//if (FAILED(m_pTransformCom->SetUp_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CModelTest_Dynamic::SetUp_Animation()
{
	m_pModelCom->SetUp_Animation(m_iAnimID, true, false);
}

CModelTest_Dynamic * CModelTest_Dynamic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CModelTest_Dynamic* pInstance = new CModelTest_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CModelTest_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CModelTest_Dynamic::Clone(void * pArg)
{
	CModelTest_Dynamic* pInstance = new CModelTest_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModelTest_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModelTest_Dynamic::Free()
{
	__super::Free();

	Safe_Release(m_pMainTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
