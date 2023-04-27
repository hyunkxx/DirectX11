#include "stdafx.h"
#include "..\Public\Character.h"

#include "GameInstance.h"

CCharacter::CCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Initialize(void * pArg)
{
	if(pArg)
		m_iObjectCount = *(int*)pArg;

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pMainTransform->Set_State(CTransform::STATE_POSITION, POSITION_ZERO);

	return S_OK;
}

void CCharacter::Start()
{
	_vector vPos = XMVectorSet((_float)m_iObjectCount, 0.f, 0.f, 1.f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

void CCharacter::Tick(_double TimeDelta)
{
}

void CCharacter::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pMainCollider, COLLISION_LAYER::COLL_BASE);
	m_pMainCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	if (m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CCharacter::Render()
{
	if (FAILED(Setup_ShaderResource()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

void CCharacter::RenderGUI()
{
	ImGui::Begin("Character ID");

	ImGui::DragInt("ID", &m_iObjectID);
	ImGui::End();
}

HRESULT CCharacter::Setup_ShaderResource()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pMainTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Add_Components()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER, L"com_renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT, L"com_vibuffer_rect", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM, L"com_transform", (CComponent**)&m_pMainTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, SHADER::VTXTEX, L"com_shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = _float3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _float3(1.f, 1.f, 1.f);
	CollDesc.vRotation = _float3(0.f, 0.f, 0.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE, L"com_collider", (CComponent**)&m_pMainCollider, &CollDesc)))
		return E_FAIL;
	m_pMainCollider->SetSameObjectDetection(false);

	return S_OK;
}

CCharacter * CCharacter::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCharacter* pInstance = new CCharacter(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CCharacter";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCharacter::Clone(void * pArg)
{
	CCharacter* pInstance = new CCharacter(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CCharacter";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacter::Free()
{
	__super::Free();
	
	if (!IsClone())
		return;

	Safe_Release(m_pRenderer);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pMainCollider);
}

void CCharacter::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}

void CCharacter::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CCharacter::OnCollisionExit(CCollider * src, CCollider * dest)
{
}