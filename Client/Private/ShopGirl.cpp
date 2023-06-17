#include "stdafx.h"
#include "..\Public\ShopGirl.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "PlayerState.h"
#include "UI_Souvenir.h"

CShopGirl::CShopGirl(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNonPlayer(pDevice, pContext)
{
}

CShopGirl::CShopGirl(const CShopGirl & rhs)
	: CNonPlayer(rhs)
{
}

HRESULT CShopGirl::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopGirl::Initialize(void * pArg)
{
	if (FAILED(CNonPlayer::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	m_pModel->SetUp_Animation(CLIP_IDLE, false, false);
	_vector vPos = XMVectorSet(70.3f, 16.650f, 141.213f, 1.f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pMainTransform->SetRotation(VECTOR_UP, XMConvertToRadians(140.f));

	return S_OK;
}

void CShopGirl::Start()
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	m_pPlayerState = static_cast<CPlayerState*>(pGI->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	//m_pTargetUI = static_cast<CUI_Souvenir*>(pGI->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
}

void CShopGirl::PreTick(_double TimeDelta)
{

}

void CShopGirl::Tick(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	//테스트용코드
	//if (pGI->InputKey(DIK_I) == KEY_STATE::TAP)
	//	PushAnimation(ANIM_STATE::ANIM_SCARE);

	//if (pGI->InputKey(DIK_P) == KEY_STATE::TAP)
	//	PushAnimation(ANIM_STATE::ANIM_TALK);

	//if (pGI->InputKey(DIK_O) == KEY_STATE::TAP)
	//	PushAnimation(ANIM_STATE::ANIM_IDLE);

	animationUpdate(TimeDelta);

	pGI->AddCollider(m_pCollider, COLL_NPC);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
}

void CShopGirl::LateTick(_double TimeDelta)
{

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
}

HRESULT CShopGirl::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(setupShader()))
		return E_FAIL;

	_uint iMeshCount = m_pModel->Get_NumMeshes();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if (FAILED(m_pModel->SetUp_BoneMatrices(m_pShader, "g_BoneMatrix", i)))
			return E_FAIL;
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		m_pShader->Begin(1);
		m_pModel->Render(i);
	}

	return S_OK;
}

HRESULT CShopGirl::RenderShadow()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pMainTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->GetLightFloat4x4(LIGHT_MATRIX::LIGHT_PROJ))))
		return E_FAIL;

	_uint iMeshCount = m_pModel->Get_NumMeshes();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		m_pShader->Begin(2);
		m_pModel->Render(i);
	}

	return S_OK;
}

void CShopGirl::RenderGUI()
{
}

HRESULT CShopGirl::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_CITY, DMODEL::DMD_SHOP_GIRL,
		TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 1.f };
	CollDesc.vExtents = { 2.f, 2.f, 2.f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("com_collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopGirl::setupShader()
{
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

void CShopGirl::animationUpdate(_double TimeDelta)
{
	_bool bFinished = false;
	m_pModel->Play_Animation(TimeDelta, nullptr, nullptr, nullptr, &bFinished);
	m_pModel->Invalidate_CombinedMatrices();
	
	if (!m_AnimQueue.empty())
	{
		if (m_AnimQueue.front() == CLIP_SCARE_START && m_iCurClip == CLIP_IDLE)
		{
			m_iCurClip = m_AnimQueue.front();
			m_AnimQueue.pop();
			m_pModel->SetUp_Animation(m_iCurClip, false, false);
		}
		
		if (bFinished)
		{
			m_iCurClip = m_AnimQueue.front();
			m_AnimQueue.pop();
			m_pModel->SetUp_Animation(m_iCurClip, false, false);
		}
	}
	else
	{
		if (bFinished)
			m_pModel->SetUp_Animation(m_iCurClip, false, false);
	}

}

void CShopGirl::Interaction()
{
}

void CShopGirl::PushAnimation(ANIM_STATE eAnimState)
{
	switch (eAnimState)
	{
	case ANIM_IDLE:
		if ((CLIP)m_iCurClip != CLIP_IDLE)
		{
			//CLIP eNext = CLIP(m_iCurClip - 1);
			//m_AnimQueue.push(eNext);
			m_AnimQueue.push(CLIP_IDLE);
		}

		break;
	case ANIM_TALK:
		m_AnimQueue.push(CLIP_TALK_START);
		m_AnimQueue.push(CLIP_TALK_LOOP);
		m_AnimQueue.push(CLIP_TALK_END);
		m_AnimQueue.push(CLIP_IDLE);

		break;
	case ANIM_SCARE:
		m_AnimQueue.push(CLIP_SCARE_START);
		m_AnimQueue.push(CLIP_SCARE_LOOP);
		m_AnimQueue.push(CLIP_SCARE_END);
		m_AnimQueue.push(CLIP_IDLE);

		break;
	default:
		break;
	}

}

CShopGirl * CShopGirl::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopGirl* pInstance = new CShopGirl(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShopGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopGirl::Clone(void * pArg)
{
	CShopGirl* pInstance = new CShopGirl(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShopGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShopGirl::Free()
{
	CNonPlayer::Free();

	Safe_Release(m_pModel);
}

void CShopGirl::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	CCharacter* pActiveCharacter = m_pPlayerState->Get_ActiveCharacter();
	if (pActiveCharacter == dest->GetOwner())
	{
		//UI Active
	}
}

void CShopGirl::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CShopGirl::OnCollisionExit(CCollider * src, CCollider * dest)
{
	CCharacter* pActiveCharacter = m_pPlayerState->Get_ActiveCharacter();
	if (pActiveCharacter == dest->GetOwner())
	{
		//UI Active
	}
}
