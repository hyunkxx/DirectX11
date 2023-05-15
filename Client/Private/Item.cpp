#include "stdafx.h"
#include "..\Public\Item.h"

#include "GameInstance.h"

CItem::CItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CItem::CItem(const CItem & rhs)
	: CGameObject(rhs)
	, m_ItemDesc(rhs.m_ItemDesc)
{
	//lstrcatW(m_ItemDesc.szTag, rhs.m_ItemDesc.szTag);
}

HRESULT CItem::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Initialize(void * pArg)
{
	assert(pArg);

	m_ItemDesc = *(ITEM_DESC*)pArg;

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	return S_OK;
}

void CItem::Start()
{
}

void CItem::Tick(_double TimeDelta)
{
}

void CItem::LateTick(_double TimeDelta)
{
}

HRESULT CItem::Render()
{
	return S_OK;
}

void CItem::RenderGUI()
{
	ImGui::Begin("Item Test");
	ImGui::Text("ITEMITEM");
	ImGui::End();
}

HRESULT CItem::addComponents()
{
	// Component Clone
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI_SUB,
		TEXT("Com_Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	return S_OK;
}

CItem * CItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CItem*	pInstance = new CItem(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItem::Clone(void * pArg)
{
	CItem*	pInstance = new CItem(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

}
