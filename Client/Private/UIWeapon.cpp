#include "stdafx.h"
#include "..\Public\UIWeapon.h"

#include "GameMode.h"
#include "GameInstance.h"

CUIWeapon::CUIWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUIWeapon::CUIWeapon(const CUIWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUIWeapon::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIWeapon::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(m_pModels, sizeof m_pModels);

	if (FAILED(addGameObject()))
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationY(XMConvertToRadians(180.f)));
	m_pMainTransform->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

void CUIWeapon::Start()
{
}

void CUIWeapon::PreTick(_double TimeDelta)
{
	CGameObject::PreTick(TimeDelta);
}

void CUIWeapon::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	__super::Tick(TimeDelta);

}

void CUIWeapon::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);
}

HRESULT CUIWeapon::Render()
{

	return S_OK;
}

void CUIWeapon::SetupWeaponProb(_uint iCharacter, _uint iWeaponProbID, CBone ** ppBone)
{
	m_pWeaponProb[iCharacter][iWeaponProbID] = ppBone;
}

HRESULT CUIWeapon::addGameObject()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODEL,
		TEXT("shader_model"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_1_MAIN,
		TEXT("model_sword1_main"), (CComponent**)&m_pModels[SWORD1])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SMODEL::SMD_SWORD_1_SUB,
		TEXT("model_sword1_sub"), (CComponent**)&m_pModels[SWORD1_SUB])))
		return E_FAIL;

	return S_OK;
}

CUIWeapon * CUIWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUIWeapon*	pInstance = new CUIWeapon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUIWeapon::Clone(void * pArg)
{
	CUIWeapon*	pInstance = new CUIWeapon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIWeapon::Free()
{
	CGameObject::Free();


}
