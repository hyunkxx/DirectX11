#include "stdafx.h"
#include "..\Public\CookGirl.h"

#include "GameMode.h"
#include "AppManager.h"
#include "GameInstance.h"
#include "PlayerState.h"
#include "UI_Panhua.h"
#include "UI_Minimap.h"
#include "UI_Cooking.h"

CCookGirl::CCookGirl(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNonPlayer(pDevice, pContext)
{
}

CCookGirl::CCookGirl(const CCookGirl & rhs)
	: CNonPlayer(rhs)
{
}

HRESULT CCookGirl::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCookGirl::Initialize(void * pArg)
{
	if (FAILED(CNonPlayer::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(addComponents()))
		return E_FAIL;

	m_pModel->SetUp_Animation(CLIP_IDLE, false, false);
	_vector vPos = XMVectorSet(96.f, 16.650f, 98.5f, 1.f);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

void CCookGirl::Start()
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	m_pPlayerState = static_cast<CPlayerState*>(pGI->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pTargetUI = static_cast<CUI_Panhua*>(pGI->Find_GameObject(LEVEL_CITY, L"UI_Panhua"));
	m_pTargetUI2 = static_cast<CUI_Cooking*>(pGI->Find_GameObject(LEVEL_CITY, L"UI_Cooking"));
	m_pUIIcon = static_cast<CUI_Minimap*>(pGI->Find_GameObject(LEVEL_ANYWHERE, TEXT("UI_Minimap")));
	m_UIIndex = m_pUIIcon->Add_Icon(m_pMainTransform->Get_State(CTransform::STATE_POSITION), CUI_Minimap::PANHUA);
	m_pUIIcon->SetRender(m_UIIndex, false);
}

void CCookGirl::PreTick(_double TimeDelta)
{

}

void CCookGirl::Tick(_double TimeDelta)
{
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (m_bOverlaped)
	{
		if (m_iCurClip == CLIP_IDLE)
		{
			m_fTimeAcc += (_float)TimeDelta;
			if (m_fTimeAcc > 2.f)
			{
				m_fTimeAcc = 0.f;
				PushAnimation(ANIM_STATE::ANIM_TALK);
			}
		}
	}
	else
	{
		m_fTimeAcc = 0.f;
	}

	animationUpdate(TimeDelta);

	pGI->AddCollider(m_pCollider, COLL_NPC);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGI->AddCollider(m_pCook, COLL_NPC);
	m_pCook->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	if (true == this->IsActive())
	{
		if (false == m_pUIIcon->GetRenderState(m_UIIndex))
			m_pUIIcon->SetRender(m_UIIndex, true);
		m_pUIIcon->Set_ObjectPos(m_UIIndex, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
	}
}

void CCookGirl::LateTick(_double TimeDelta)
{

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);
}

HRESULT CCookGirl::Render()
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

HRESULT CCookGirl::RenderShadow()
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
		if (FAILED(m_pModel->SetUp_BoneMatrices(m_pShader, "g_BoneMatrix", i)))
			return E_FAIL;
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;
		if (FAILED(m_pModel->SetUp_ShaderMaterialResource(m_pShader, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		m_pShader->Begin(2);
		m_pModel->Render(i);
	}

	return S_OK;
}

void CCookGirl::RenderGUI()
{
}

HRESULT CCookGirl::addComponents()
{
	if (FAILED(__super::Add_Component(LEVEL_CITY, DMODEL::DMD_COOK_GIRL,
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

	CollDesc.owner = this;
	CollDesc.vCenter = { 5.f, 0.f, -4.f };
	CollDesc.vExtents = { 2.f, 2.f, 2.f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("com_collider_cook"), (CComponent**)&m_pCook, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCookGirl::setupShader()
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

void CCookGirl::animationUpdate(_double TimeDelta)
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

void CCookGirl::Interaction()
{
}

void CCookGirl::PushAnimation(ANIM_STATE eAnimState)
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

CCookGirl * CCookGirl::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCookGirl* pInstance = new CCookGirl(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCookGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCookGirl::Clone(void * pArg)
{
	CCookGirl* pInstance = new CCookGirl(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCookGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCookGirl::Free()
{
	CNonPlayer::Free();

	Safe_Release(m_pModel);
}

void CCookGirl::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	// 상인
	if (src == m_pCollider)
	{
		CCharacter* pActiveCharacter = m_pPlayerState->Get_ActiveCharacter();
		if (pActiveCharacter == dest->GetOwner())
		{
			PushAnimation(ANIM_STATE::ANIM_TALK);
			m_pTargetUI->Set_SituMeet();
		}
	}
	
	// 조리대
	if (src == m_pCook)
	{
		CCharacter* pActiveCharacter = m_pPlayerState->Get_ActiveCharacter();
		if (pActiveCharacter == dest->GetOwner())
		{
			m_pTargetUI2->Set_SituMeet();
		}
	}
}

void CCookGirl::OnCollisionStay(CCollider * src, CCollider * dest)
{
	// 요리상인
	if (src == m_pCollider)
	{
		CCharacter* pActiveCharacter = m_pPlayerState->Get_ActiveCharacter();
		if (pActiveCharacter == dest->GetOwner())
		{
			if (m_iCurClip == CLIP_IDLE)
				m_bOverlaped = true;
		}
	}


	// 조리대
	if (src == m_pCook)
	{
		CCharacter* pActiveCharacter = m_pPlayerState->Get_ActiveCharacter();
		if (pActiveCharacter == dest->GetOwner())
		{

		}
	}
}

void CCookGirl::OnCollisionExit(CCollider * src, CCollider * dest)
{
	//상인
	if (src == m_pCollider)
	{
		m_pTargetUI->Call_END();
	}
	//조리대
	if (src == m_pCook)
	{
		m_pTargetUI2->Call_END();
	}
}
