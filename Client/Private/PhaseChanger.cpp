#include "stdafx.h"
#include "..\Public\PhaseChanger.h"

#include "GameInstance.h"
#include "GameMode.h"
#include "EffectKey.h"
#include "SoundKey.h"

CPhaseChanger::CPhaseChanger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPhaseChanger::CPhaseChanger(const CPhaseChanger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPhaseChanger::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhaseChanger::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 
	for (auto& pAnimation : m_pRoverModel->Get_Animations())
		pAnimation->Set_TicksPerSecond(30.0);

	for (auto& pAnimation : m_pCrownlessModel->Get_Animations())
		pAnimation->Set_TicksPerSecond(30.0);

	

	
	// 위치
	// 104.558f, 20.010f, 91.400f
	// 바라볼 방향
	// 103.343f, 20.010f, 104.625f
	_vector vPos = XMVectorSet(104.558f, 20.010f, 91.400f, 1.f);
	_vector vLookDir = XMVector3Normalize(vPos - XMVectorSet(104.343f, 20.010f, 104.625f, 1.f));

	m_pRoverTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pRoverTransform->Set_LookDir(vLookDir);

	_vector vCrownlessPos = vPos - vLookDir * 6.f;
	m_pCrownlessTransform->Set_State(CTransform::STATE_POSITION, vCrownlessPos);
	m_pCrownlessTransform->Set_LookDir(vLookDir);
	
	CGameInstance* pGame = CGameInstance::GetInstance();

	m_pPlayerStateClass = static_cast<CPlayerState*>(pGame->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	
	//SetState(DISABLE);
	m_bRender = false;

	return S_OK;
}

void CPhaseChanger::Start()
{
}

void CPhaseChanger::Tick(_double TimeDelta)
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	if (pGame->InputKey(DIK_NUMPAD1) == KEY_STATE::TAP)
	{
		CutScene1_Ready();
	}
	if (pGame->InputKey(DIK_NUMPAD2) == KEY_STATE::TAP)
	{
		CutScene2_Ready();
	}


	//
	if (0.0 < m_Timer)
	{
		m_Timer -= TimeDelta;
		if (m_Timer < 0.0)
		{
			m_Timer = 0.0;
			if (STATE_1_READY == m_eState)
			{
				m_eState = STATE_1_ONGOING;
				CutScene1_Start();
			}
			else if (STATE_2_READY == m_eState)
			{
				m_eState = STATE_2_ONGOING;
				CutScene2_Start();
			}
		}
	}

	if (STATE_1_ONGOING == m_eState)
	{
		m_pRoverModel->Play_Animation(TimeDelta, nullptr, nullptr, &m_TrackPos, &m_bAnimFinished, nullptr);
		m_pCrownlessModel->Play_Animation(TimeDelta);

		m_pRoverModel->Invalidate_CombinedMatrices();
		m_pCrownlessModel->Invalidate_CombinedMatrices();

		if (true == m_bAnimFinished)
			CutScene1_End();
	}
	else if (STATE_2_ONGOING == m_eState)
	{
		_float4 vRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float3 vMovement;

		m_pCrownlessModel->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_TrackPos, &m_bAnimFinished, nullptr);

		m_pCrownlessModel->Invalidate_CombinedMatrices();

		if (!XMVector4Equal(XMQuaternionIdentity(), XMLoadFloat4(&vRotation)))
			m_pCrownlessTransform->Rotate_Quaternion(XMLoadFloat4(&vRotation));

		m_pCrownlessTransform->Move_Anim(&vMovement, 0);

		if (true == m_bAnimFinished)
			CutScene2_End();
	}

	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);

}

void CPhaseChanger::LateTick(_double TimeDelta)
{

}

HRESULT CPhaseChanger::Render()
{
	if (m_bRender == false)
		return S_OK;

	CGameInstance* pGame = CGameInstance::GetInstance();

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	// Crownless
	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pCrownlessTransform->Get_WorldMatrix())))
		return E_FAIL;

	_uint iNumMeshes = m_pCrownlessModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (STATE_1_ONGOING == m_eState)
		{
			if ( 2 == i || 4== i || 5 == i || 6 == i)
				continue;
		}
		else if (STATE_2_ONGOING == m_eState)
		{
			if (5 == i || 6 == i)
				continue;
		}
		

		if (FAILED(m_pCrownlessModel->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pCrownlessModel->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pCrownlessModel->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;

		m_pShaderCom->Begin(10);

		m_pCrownlessModel->Render(i);
	}

	if (STATE_1_ONGOING != m_eState)
		return S_OK;

	// Rover
	if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_pRoverTransform->Get_WorldMatrix())))
		return E_FAIL;

	for (_uint i = 0; i < 6; ++i)
	{
		if (FAILED(m_pRoverModel->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pRoverModel->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pRoverModel->SetUp_VertexTexture(m_pShaderCom, "g_VertexTexture", i)))
			return E_FAIL;

		if (i == 5)
		{
			// Eye
			m_pShaderCom->Begin(7);
		}
		else
		{
			m_pShaderCom->Begin(4);
		}

		m_pRoverModel->Render(i);
	}

	return S_OK;
}

void CPhaseChanger::CutScene1_Ready()
{
	m_Timer = 1.0;
	m_eState = STATE_1_READY;

	m_pPlayerStateClass->Change_ActiveCharacter(m_pPlayerStateClass->Get_Slot(CPlayerState::CHARACTER_ROVER));
	CCharacter* pChar = m_pPlayerStateClass->Get_ActiveCharacter();
	pChar->Set_OnControl(false);
	pChar->Set_ForceIdle();

	// UI 끄기, 기존 모델 숨기기 등?
}

void CPhaseChanger::CutScene1_Start()
{
	m_pCrownless_P1->SetState(DISABLE);

	m_pPlayerStateClass->Get_ActiveCharacter()->Set_WorldMatrix(XMLoadFloat4x4(&m_pRoverTransform->Get_WorldMatrix()));
	m_pPlayerStateClass->Get_ActiveCharacter()->Disappear(nullptr, nullptr, nullptr);

	m_bRender = true;

	m_TrackPos = 0.0;
	m_bAnimFinished = false;
	
	m_pRoverModel->SetUp_Animation(0, false);
	m_pCrownlessModel->SetUp_Animation(0, false);

}

void CPhaseChanger::CutScene1_End()
{
	m_bRender = false;
	m_eState = STATE_WAIT;

	m_pCrownless_P2->SetState(ACTIVE);
	m_pCrownless_P2->Set_WorldMatrix(XMLoadFloat4x4(&m_pCrownlessTransform->Get_WorldMatrix()));

	//m_pCrownless_P2->Set_ForceIdle();


	m_pRoverTransform->LookAt(m_pCrownlessTransform->Get_State(CTransform::STATE_POSITION));

	m_pPlayerStateClass->Get_ActiveCharacter()->Appear(m_pRoverTransform, nullptr, 196, 200.f);

	m_pPlayerStateClass->Get_ActiveCharacter()->Set_ForceIdle();

}


void CPhaseChanger::CutScene2_Ready()
{
	m_Timer = 1.0;
	m_eState = STATE_2_READY;

	m_pPlayerStateClass->Change_ActiveCharacter(m_pPlayerStateClass->Get_Slot(CPlayerState::CHARACTER_ROVER));
	CCharacter* pChar = m_pPlayerStateClass->Get_ActiveCharacter();
	pChar->Set_OnControl(false);
	pChar->Set_ForceIdle();
}

void CPhaseChanger::CutScene2_Start()
{
	m_pCrownless_P2->SetState(DISABLE);

	m_pPlayerStateClass->Get_ActiveCharacter()->Set_WorldMatrix(XMLoadFloat4x4(&m_pRoverTransform->Get_WorldMatrix()));
	m_pPlayerStateClass->Get_ActiveCharacter()->Disappear(nullptr, nullptr, nullptr);

	m_bRender = true;

	m_pCrownlessModel->Set_RootBone(TEXT("Root"));
	
	m_TrackPos = 1200.0;
	m_bAnimFinished = false;

	m_pCrownlessModel->SetUp_Animation(1, false);
	m_pCrownlessModel->Set_TrackPos(1200.f);
}

void CPhaseChanger::CutScene2_End()
{
	m_bRender = false;
	m_eState = STATE_WAIT;

	m_pCrownless_P3->SetState(ACTIVE);
	m_pCrownless_P3->Set_WorldMatrix(XMLoadFloat4x4(&m_pCrownlessTransform->Get_WorldMatrix()));
	//m_pCrownless_P3->Set_ForceIdle();

	
	m_pRoverTransform->LookAt(m_pCrownlessTransform->Get_State(CTransform::STATE_POSITION));

	m_pPlayerStateClass->Get_ActiveCharacter()->Appear(m_pRoverTransform, nullptr, 196, 200.f);
	m_pPlayerStateClass->Get_ActiveCharacter()->Set_ForceIdle();
}

HRESULT CPhaseChanger::Init_States(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	ZeroMemory(m_tStates, sizeof(CCharacter::SINGLESTATE) * 2);

	// 로드하는 코드
	for (_int i = 0; i < 2; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, TEXT("../../Data/CharState/Act_Crownless/Qunjing_%d.state"), i);
		HANDLE hFile = CreateFile(szBuffer, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			continue;


		DWORD dwByte = 0;

		ReadFile(hFile, &m_tStates[i], sizeof(CCharacter::SINGLESTATE) - sizeof(CStateKey**), &dwByte, nullptr);

		if (0 != m_tStates[i].iKeyCount)
		{
			m_tStates[i].ppStateKeys = new CStateKey*[m_tStates[i].iKeyCount];
			ZeroMemory(m_tStates[i].ppStateKeys, sizeof(CStateKey*) * m_tStates[i].iKeyCount);

			for (_uint j = 0; j < m_tStates[i].iKeyCount; ++j)
			{
				CStateKey::BaseData tBaseData;
				ReadFile(hFile, &tBaseData, sizeof(CStateKey::BaseData), &dwByte, nullptr);

				switch (tBaseData.iType)
				{
				case CStateKey::TYPE_EFFECT:
					m_tStates[i].ppStateKeys[j] = CEffectKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_SOUND:
					m_tStates[i].ppStateKeys[j] = CSoundKey::Create(pDevice, pContext, &tBaseData);
					break;
				default:
					break;
				}
			}
		}
		else
			m_tStates[i].ppStateKeys = nullptr;

		CloseHandle(hFile);
	}


	return S_OK;
}

void CPhaseChanger::Release_States()
{
}

HRESULT CPhaseChanger::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(540.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_RoverTransform"), (CComponent**)&m_pRoverTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_CrownlessTransform"), (CComponent**)&m_pCrownlessTransform, &TransformDesc)))
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
	_uint nCurrentLevel = pGM->GetCurrentLevel();

	// For.Com_Shader_ModelAnim
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// Models
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_PLAYERGIRL_ACT,
		TEXT("Com_PlayerGirl_Act"), (CComponent**)&m_pRoverModel)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_MONSTER_CROWNLESS_ACT,
		TEXT("Com_Crownless_Act"), (CComponent**)&m_pCrownlessModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhaseChanger::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShaderCom->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetRawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	return S_OK;
}

CPhaseChanger * CPhaseChanger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPhaseChanger* pInstance = new CPhaseChanger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CPhaseChanger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPhaseChanger::Clone(void * pArg)
{
	CPhaseChanger* pInstance = new CPhaseChanger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CPhaseChanger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhaseChanger::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	
	Safe_Release(m_pCrownlessModel);
	Safe_Release(m_pCrownlessTransform);
	Safe_Release(m_pRoverModel);
	Safe_Release(m_pRoverTransform);
}
