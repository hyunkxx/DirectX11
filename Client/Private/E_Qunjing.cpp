#include "stdafx.h"
#include "..\Public\E_Qunjing.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Parts.h"
#include "Effect.h"
#include "EffectKey.h"
#include "PartsKey.h"
#include "PriorityKey.h"
#include "Missile.h"
#include "MissileKey.h"
#include "Missile_Constant.h"
#include "OBBKey.h"
#include "DissolveKey.h"
#include "PlayerState.h"

#include "CameraMovement.h"
#include "Chest.h"
//UI�߰�
#include "UI_Monster.h"
#include "UI_Minimap.h"
#include "..\Public\E_Leilie.h"

CE_Qunjing::CE_Qunjing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CE_Qunjing::CE_Qunjing(const CE_Qunjing & rhs)
	: CCharacter(rhs)
{
}

HRESULT CE_Qunjing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Qunjing::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Init_State(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Init_EffectBones()))
		return E_FAIL;

	Init_AttackInfos();


	m_pModelCom->Get_Animation(0)->Set_TicksPerSecond(m_tCurState.FramePerSec);

	// ��Ʈ��ǿ� ��ã��
	m_pModelCom->Set_RootBone(TEXT("Root"));

	// �ʱ���ġ ����
	//m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(186.066f, 32.837f, 242.489f, 1.f));
	//m_pNaviCom->Set_CurrentIndex(2200);



	// StateController �ʱ�ȭ
	m_Scon.iCurState = 0;
	m_Scon.iNextState = 0;

	m_Scon.ePositionState = PS_GROUND;
	m_Scon.iPrevCellState = 0;
	SetUp_State();
	m_pModelCom->SetUp_Animation(0, true, false);

	// �浹 Ÿ�� ó��
	m_eCollisionType = CT_PLAYER;
	m_fPushWeight = 75.f;

	CGameInstance* pGame = CGameInstance::GetInstance();

	m_pEchoSystem = static_cast<CEchoSystem*>(pGame->Find_GameObject(LEVEL_STATIC, L"Echo"));
	m_pEchoSystem->Register_EchoPtr(CEchoSystem::EC_ELITE_STONE, this);


#ifdef _DEBUG
	m_pRendererCom->DebugBundleRender_Control(true);
#endif

	m_pCamMovement = static_cast<CCameraMovement*>(pGame->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));

	SetState(DISABLE);



	return S_OK;
}

void CE_Qunjing::Start()
{
	//CGameInstance* pGame = CGameInstance::GetInstance();
	// Find ActivePlayer
	//m_pTarget = static_cast<CPlayerState*>(pGame->Find_GameObject(LEVEL_STATIC, L"CharacterState"))->Get_ActiveCharacter();
	//m_pTargetTransform = m_pTarget->GetTransform();


}

void CE_Qunjing::PreTick(_double TimeDelta)
{
	if (nullptr != m_pTarget)
	{
		if (m_pTarget->Get_Dying())
		{
			m_pTarget = nullptr;
			m_pTargetTransform = nullptr;
		}
	}
}

void CE_Qunjing::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_double TimeDelay = 1.0;

	if (pGameInstance->InputKey(DIK_NUMPAD0) == KEY_STATE::HOLD)
	{
		TimeDelay = 0.0;
	}
	else
	{
		TimeDelay = 1.0;
	}

	__super::Tick(TimeDelta);


	// �����Ӹ��� ȸ��
	if (nullptr != m_pTarget)
	{
		if (CCharacter::ROT_TURN == m_tCurState.iRotationType)
		{
			_vector vTargetDir = XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f);
			_vector vAxis = XMVector3Cross(m_pMainTransform->Get_State(CTransform::STATE_LOOK), vTargetDir);
			_float fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_LOOK)), XMVector3Normalize(vTargetDir))));

			if (!XMVector3Equal(vAxis, XMVectorZero()))
			{
				if (fAngle > m_pMainTransform->Get_RotationSpeed() * TimeDelta)
					m_pMainTransform->Rotate(vAxis, TimeDelta);
				else
					m_pMainTransform->Set_LookDir(vTargetDir);
			}
		}
	}

	Tick_State(TimeDelta * TimeDelay); // PlayAnimation, �ִϸ��̼ǿ� ���� �̵�, �ִϸ��̼� ���� �� ó��

	On_Cell(); // �ڹ����� ������ ��ó�� >> �ַ� ���� �޽�

	pGameInstance->AddCollider(m_pAttackCollider, COLL_PLAYERATTACK);
	m_pAttackCollider->Update(XMLoadFloat4x4(&m_EffectBoneMatrices[EBONE_RHAND]));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_MOVE);
	m_pMoveCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

}

void CE_Qunjing::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	//Effect Bones ó��
	Update_EffectBones();
}

HRESULT CE_Qunjing::Render()
{
	if (false == m_bRender)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;


		if (m_bDissolve)
		{
			CGameInstance* pGame = CGameInstance::GetInstance();
			// Dissolve ����
			if (FAILED(pGame->SetupSRV(STATIC_IMAGE::MASK_DESSOLVE, m_pShaderCom, "g_DissolveTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->SetRawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->SetRawValue("g_vDessolveColor", &m_vDissolveColor, sizeof(_float3))))
				return E_FAIL;

			m_pShaderCom->Begin(12);
		}
		else
			m_pShaderCom->Begin(10);

		m_pModelCom->Render(i);

	}

	return S_OK;
}

HRESULT CE_Qunjing::RenderShadow()
{
	if (false == m_bRender)
		return S_OK;

	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(Setup_ShadowShaderResource()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;

		m_pShaderCom->Begin(2);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CE_Qunjing::RenderGUI()
{
}

HRESULT CE_Qunjing::Add_Components()
{
	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC	TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.fMoveSpeed = 15.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(180.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::TRANSFORM,
		TEXT("Com_Transform"), (CComponent**)&m_pMainTransform, &TransformDesc)))
		return E_FAIL;

	CGameMode* pGM = CGameMode::GetInstance();
	_uint nCurrentLevel = pGM->GetCurrentLevel();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, DMODEL::DMD_ECHO_QUNJING,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDER_DESC CollDesc;
	// attack Hit / Move
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.8f, 0.5f, 0.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,

		TEXT("Com_AttackCollider"), (CComponent**)&m_pAttackCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.4f, 0.4f, 0.4f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_MoveCollider"), (CComponent**)&m_pMoveCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Qunjing::Init_State(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(&m_tCurState, sizeof(SINGLESTATE));

	// �ε��ϴ� �ڵ�
	_tchar szBuffer[MAX_PATH];
	wsprintf(szBuffer, TEXT("../../Data/CharState/Echos/Qunjing_16.state"));
	HANDLE hFile = CreateFile(szBuffer, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	DWORD dwByte = 0;

	ReadFile(hFile, &m_tCurState, sizeof(CCharacter::SINGLESTATE) - sizeof(CStateKey**), &dwByte, nullptr);

	if (0 != m_tCurState.iKeyCount)
	{
		m_tCurState.ppStateKeys = new CStateKey*[m_tCurState.iKeyCount];
		ZeroMemory(m_tCurState.ppStateKeys, sizeof(CStateKey*) * m_tCurState.iKeyCount);

		for (_uint j = 0; j < m_tCurState.iKeyCount; ++j)
		{
			CStateKey::BaseData tBaseData;
			ReadFile(hFile, &tBaseData, sizeof(CStateKey::BaseData), &dwByte, nullptr);

			switch (tBaseData.iType)
			{
			case CStateKey::TYPE_EFFECT:
				m_tCurState.ppStateKeys[j] = CEffectKey::Create(pDevice, pContext, &tBaseData);
				break;
			case CStateKey::TYPE_PARTS:
				//m_tStates[i].ppStateKeys[j] = CPartsKey::Create(pDevice, pContext, &tBaseData);
				break;
			case CStateKey::TYPE_PRIORITY:
				m_tCurState.ppStateKeys[j] = CPriorityKey::Create(pDevice, pContext, &tBaseData);
				break;
			case CStateKey::TYPE_DISSOLVE:
				m_tCurState.ppStateKeys[j] = CDissolveKey::Create(pDevice, pContext, &tBaseData);
				break;
			case CStateKey::TYPE_OBB:
				m_tCurState.ppStateKeys[j] = COBBKey::Create(pDevice, pContext, &tBaseData);
				break;
			case CStateKey::TYPE_MISSILE:
				m_tCurState.ppStateKeys[j] = CMissileKey::Create(pDevice, pContext, &tBaseData);
				break;
			case CStateKey::TYPE_SOUND:

				break;
			default:
				break;
			}
		}
	}
	else
		m_tCurState.ppStateKeys = nullptr;

	CloseHandle(hFile);



	return S_OK;
}

void CE_Qunjing::Release_State()
{
	if (0 == m_tCurState.iKeyCount)
		return;

	for (_uint j = 0; j < m_tCurState.iKeyCount; ++j)
	{
		if (nullptr != m_tCurState.ppStateKeys[j])
			Safe_Release(m_tCurState.ppStateKeys[j]);
	}
	Safe_Delete_Array(m_tCurState.ppStateKeys);

}

HRESULT CE_Qunjing::Reload_Components()
{
	// For.Com_Shader_ModelAnim
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
	{
		m_pShaderCom = nullptr;
	}

	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATION_DESC));
	NavigationDesc.iCurrentIndex = 0;

	/* Navigation */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, COMPONENT::NAVIGATION,
		TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NavigationDesc)))
	{
		m_pNaviCom = nullptr;
	}

	Init_Missiles();

	return S_OK;
}

void CE_Qunjing::Release_Reloadable()
{
	if (nullptr != m_pShaderCom)
	{
		Safe_Release(m_pShaderCom);
		Erase_Component(TEXT("Com_Shader_ModelAnim"));
	}


	if (nullptr != m_pNaviCom)
	{
		Safe_Release(m_pNaviCom);
		Erase_Component(TEXT("Com_Navigation"));
	}

	if (nullptr != m_MissilePool)
		Safe_Release(m_MissilePool);
}

void CE_Qunjing::Shot_EffectKey(_tchar * szEffectTag/* szTag1*/, _uint EffectBoneID /* iInt0 */, _uint iEffectTypeID, _bool bTracking/*iInt1*/)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag, Engine::EFFECT_ID(iEffectTypeID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CE_Qunjing::Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID)
{
	//if (MISS_END <= iMissilePoolID || EBONE_END <= iEffectBoneID)
	//	return;

	_vector vInitPos;

	if (0 != iEffectBoneID)
	{
		vInitPos = XMVector3TransformCoord(
		XMVector3TransformCoord(m_EffectBones[iEffectBoneID]->Get_CombinedPosition(), XMMatrixRotationY(XMConvertToRadians(180.f))),
		XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));
	}
	else
	vInitPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);

	_matrix matRot = XMMatrixIdentity();

	_vector vTargetPos = vInitPos + XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_MissilePool->Shot(vInitPos, m_pMainTransform->Get_State(CTransform::STATE_LOOK), matRot, vTargetPos);
}

void CE_Qunjing::Shot_OBBKey(_bool bOBB, _uint iAttackInfoID)
{
	m_pAttackCollider->SetActive(bOBB);
	//m_iCurAttackID = iAttackInfoID;
}

void CE_Qunjing::Shot_DissolveKey(_bool bDissolveType, _float fDissolveSpeed)
{
	__super::Shot_DissolveKey(bDissolveType, fDissolveSpeed);

	if (bDissolveType == 0)
		m_bDisableAfterDissolve = true;

}

void CE_Qunjing::Shot_Echo(CTransform * pTransform, CCharacter * pTarget, _uint iNaviCellID)
{
	SetState(ACTIVE);

	m_pMainTransform->Set_WorldMatrix(pTransform->Get_WorldMatrix());
	m_pTarget = pTarget;
	if (nullptr != m_pTarget)
		m_pTargetTransform = m_pTarget->GetTransform();
	m_pNaviCom->Set_CurrentIndex(iNaviCellID);

	m_bDisableAfterDissolve = false;

	SetUp_State();
	m_pModelCom->SetUp_Animation(0, false, false);

	if (nullptr != m_pTarget)
	{
		if (CCharacter::ROT_ONSTART == m_tCurState.iRotationType)
		{
			m_pMainTransform->Set_LookDir(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f));
		}
	}

}

void CE_Qunjing::SetUp_State()
{
	// Ű ����
	for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
	{
		if (nullptr != m_tCurState.ppStateKeys[i])
			m_tCurState.ppStateKeys[i]->Reset();
	}

	// ���� �ִϸ��̼����� ����
	m_Scon.TrackPos = 0.0;
	m_Scon.bAnimFinished = false;

	// �ִϸ��̼��� ������ ���� ��� ��� �ִϸ��̼� ���� �� OBB �ݶ��̴� ������ ����
	m_pAttackCollider->SetActive(false);

	//PhysicMove
	if (false == m_tCurState.bRootMotion)
	{
		PHYSICMOVE tPhysicMove = StatePhysics[m_tCurState.iPhysicMoveID];

		if (true == tPhysicMove.bInitMovement)
		{
			XMStoreFloat3(&m_Scon.vMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
			XMStoreFloat3(&m_Scon.vPrevMovement, XMVector3Normalize(XMLoadFloat3(&tPhysicMove.vInitDir)) * tPhysicMove.fInitForce);
		}
	}
}

void CE_Qunjing::Change_Target(CCharacter * pActiveCharacter)
{
	m_pTarget = pActiveCharacter;
	m_pTargetTransform = m_pTarget->GetTransform();
}

void CE_Qunjing::Init_AttackInfos()
{
	ZeroMemory(&m_AttackInfos, sizeof TAGATTACK * ATK_END);

	m_AttackInfos[ATK_ATTACK08_1].fDamageFactor = 1.f;
	m_AttackInfos[ATK_ATTACK08_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK08_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK08_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK08_1].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK08_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK08_1].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK08_2].fDamageFactor = 3.f;
	m_AttackInfos[ATK_ATTACK08_2].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK08_2].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK08_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK08_2].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK08_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK08_2].szHitEffectTag, TEXT("M_Yellow_Hit"));

}
void CE_Qunjing::Init_Missiles()
{
	CMissilePool::MISSILEPOOLDESC tMissilePoolDesc;
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack08_2_Echo_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK08_2;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.5f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePool = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
}

void CE_Qunjing::Release_Missiles()
{
	if (nullptr != m_MissilePool)
		Safe_Release(m_MissilePool);
}

HRESULT CE_Qunjing::SetUp_ShaderResources()
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

HRESULT CE_Qunjing::Setup_ShadowShaderResource()
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

void CE_Qunjing::Tick_State(_double TimeDelta)
{
	//
	if (false == m_Scon.bAnimFinished)
	{
		_float4 vRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float3 vMovement;

		// �ִϸ��̼� ����
		m_pModelCom->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_Scon.TrackPos, &m_Scon.bAnimFinished);
		m_pModelCom->Invalidate_CombinedMatrices();

		// �������� �ʴ� �ִϸ��̼��� �̹� �����ӿ� �������� �������� �������� ����
		// �ִϸ��̼� �������̶� FrameRate�� �޶� �߻��ϴ� ���������� ���� ���� ����
		if (true == m_Scon.bAnimFinished && false == m_tCurState.bLoop)
			XMStoreFloat3(&vMovement, XMLoadFloat3(&m_Scon.vPrevMovement) * (_float)TimeDelta);
		else
		{
			// ��Ʈ ��� ó��(ȸ��, �̵�)
			if (true == m_tCurState.bRootMotion)
			{
				if (!XMVector4Equal(XMQuaternionIdentity(), XMLoadFloat4(&vRotation)))
					m_pMainTransform->Rotate_Quaternion(XMLoadFloat4(&vRotation));
			}
			else
			{

				const PHYSICMOVE& PhysicMove = StatePhysics[m_tCurState.iPhysicMoveID];
				if (false == PhysicMove.bConstant)
				{
					_vector vMove = XMLoadFloat3(&m_Scon.vPrevMovement);

					_float fXZSpeed = XMVectorGetX(XMVector2Length(vMove)) * (1.f - (_float)TimeDelta * PhysicMove.fHorizontalAtten);
					if (fabs(fXZSpeed) < 0.01f)
						fXZSpeed = 0.f;
					_float fYSpeed = (XMVectorGetZ(vMove) - PhysicMove.fVerticalAccel * (_float)TimeDelta);

					fXZSpeed = min(fXZSpeed, PhysicMove.fHorizontalMaxSpeed);
					fYSpeed = max(fYSpeed, PhysicMove.fVerticalMaxSpeed);

					_vector vFinalMove = XMVectorSetZ(XMVector2Normalize(vMove) * fXZSpeed, fYSpeed) * (_float)TimeDelta;

					XMStoreFloat3(&vMovement, vFinalMove);
				}
				else
					XMStoreFloat3(&vMovement, XMLoadFloat3(&m_Scon.vMovement) * (_float)TimeDelta);
			}
		}
		// ������ �̵�����ŭ �����̰� ���� ������ ������ ����, + TimeDelta ����
		m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, m_pNaviCom);

		XMStoreFloat3(&m_Scon.vPrevMovement, XMLoadFloat3(&vMovement) / (_float)TimeDelta);

		// StateKey ó��
		for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
		{
			if (nullptr != m_tCurState.ppStateKeys[i])
				m_tCurState.ppStateKeys[i]->Check(m_Scon.TrackPos, this);
		}

		// �������� ���� ��� 
		if (vMovement.z < 0.f)
		{
			m_Scon.bFalling = true;
		}
	}

	if (true == m_Scon.bAnimFinished)
	{
		SetState(DISABLE);
	}
}

void CE_Qunjing::On_Cell()
{
	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_float fPosY = XMVectorGetY(vPos);
	_float fCellHeight = m_pNaviCom->Compute_Height(vPos);

	if (PS_GROUND == m_Scon.ePositionState)
	{
		m_pMainTransform->Set_PosY(fCellHeight);
	}
}

HRESULT CE_Qunjing::Init_EffectBones()
{
	//NONE�� �� ���� ��Ʈ������ �����س��ٰ� ����
	m_EffectBones[EBONE_SPINE] = nullptr;
	m_EffectBones[EBONE_LHAND] = nullptr;
	m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));
	m_EffectBones[EBONE_HEAD] = m_pModelCom->Get_BonePtr(TEXT("Bip001Head"));

	return S_OK;
}

void CE_Qunjing::Update_EffectBones()
{
	memcpy(&m_EffectBoneMatrices[EBONE_NONE], &m_pMainTransform->Get_WorldMatrix(), sizeof(_float4x4));

	for (_uint i = 1; i < EBONE_END; ++i)
	{
		if (nullptr != m_EffectBones[i])
		{
			XMStoreFloat4x4(&m_EffectBoneMatrices[i], XMLoadFloat4x4(&m_EffectBones[i]->Get_CombinedTransfromationMatrix())
				* XMMatrixRotationY(XMConvertToRadians(180.f))
				* XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));
		}
	}
}

CE_Qunjing * CE_Qunjing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CE_Qunjing* pInstance = new CE_Qunjing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CE_Qunjing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CE_Qunjing::Clone(void * pArg)
{
	CE_Qunjing* pInstance = new CE_Qunjing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CE_Qunjing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CE_Qunjing::Free()
{
	__super::Free();

	if (true == m_bClone)
		Release_State();

	Safe_Release(m_pNaviCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pMoveCollider);

}

void CE_Qunjing::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());

	if (pOpponent)
	{
		// ��밡 �÷��̾��� ���
		if (CT_PLAYER == pOpponent->Get_CollType())
		{
			// �� ������ ��뿡�� ������ ���
			if (true == src->Compare(GetAttackCollider()) &&
				true == dest->Compare(pOpponent->GetHitCollider()))
			{
				// Ÿ�� ��ġ�� ã�Ƽ� ��Ʈ ����Ʈ ���
				m_pCamMovement->StartVibration(10.f, 0.5f);
			}

			// ����� ������ ������ ������ ��� 
			if (true == src->Compare(GetHitCollider()) &&
				true == dest->Compare(pOpponent->GetAttackCollider()))
			{
				m_pCamMovement->StartVibration();

				// ��/�� ���� : ����� ó��, ����� ��Ʈ ���, �ǰ� �ִϸ��̼� ����
				TAGATTACK tAttackInfo;
				ZeroMemory(&tAttackInfo, sizeof(tAttackInfo));
				_float fAttackPoint = 0.f;

				pOpponent->Get_AttackInfo(pOpponent->Get_AttackID(), &tAttackInfo, &fAttackPoint);

				_vector destCenter = XMLoadFloat3(dest->GetWorldCenter());
				_vector srcCenter = XMLoadFloat3(src->GetWorldCenter());
				_float3 EffPos = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&EffPos, (destCenter + srcCenter) * 0.5f);

				On_Hit(pOpponent, &tAttackInfo, fAttackPoint, &EffPos, pOpponent->Get_CritRate(), pOpponent->Get_CritDMG());
			}
		}
	}

	CMissile* pMissile = dynamic_cast<CMissile*>(dest->GetOwner());

	if (pMissile)
	{
		CCharacter* pMissileOwner = pMissile->Get_Owner();
		if (CT_PLAYER == pMissileOwner->Get_CollType())
		{
			// ����� ����ü ������ ������ ������ ��� 
			if (true == src->Compare(GetHitCollider()))
			{
				// ��/�� ���� : ����� ó��, ����� ��Ʈ ���, �ǰ� �ִϸ��̼� ����
				TAGATTACK tAttackInfo;
				ZeroMemory(&tAttackInfo, sizeof(tAttackInfo));
				_float fAttackPoint = 0.f;

				pMissileOwner->Get_AttackInfo(pMissile->Get_AttackID(), &tAttackInfo, &fAttackPoint);

				_vector destCenter = XMLoadFloat3(dest->GetWorldCenter());
				_vector srcCenter = XMLoadFloat3(src->GetWorldCenter());
				_float3 EffPos = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&EffPos, (destCenter + srcCenter) * 0.5f);

				On_Hit(pMissileOwner, &tAttackInfo, fAttackPoint, &EffPos, pMissileOwner->Get_CritRate(), pMissileOwner->Get_CritDMG());
			}
		}
	}


}

void CE_Qunjing::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());

	if (pOpponent)
	{
		if (CT_MONSTER == pOpponent->Get_CollType())
		{

#pragma region Move
			// pushWeight ���� ���� ���θ� �о
			// �о�� ������ �÷��̾� �ʿ��� ó��?
			if (true == src->Compare(GetMoveCollider()) &&
				true == dest->Compare(pOpponent->GetMoveCollider()))
			{
				_float fTargetDistance = src->GetExtents().x + dest->GetExtents().x;
				_float fPushDistance = fTargetDistance - XMVectorGetX(XMVector3Length(pOpponent->Get_Position() - this->Get_Position()));
				_float fPushRatio = 1 - m_fPushWeight / (pOpponent->Get_PushWeight() + m_fPushWeight);
				_vector vPushDir = XMVector3Normalize(XMVectorSetY(this->Get_Position() - pOpponent->Get_Position(), 0.f));

				//_vector vTargetDir = XMLoadFloat3(&m_vTargetDir);

				//// �̹� �����ӿ� Ư�� Ÿ���� �������� �̵����� ���
				//if (!XMVector3Equal(XMVectorZero(), vTargetDir) && fTargetDistance * 0.5f < XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Scon.vPrevMovement))))
				//{
				//	// �̵� �� Ÿ�� ����� �о�� ������ ���ΰ��� 45�� �̸��̶��
				//	if (45.f > fabs(acosf(XMVectorGetX(XMVector3Dot(vPushDir, vTargetDir)))))
				//	{
				//		vPushDir *= -1.f;
				//	}
				//}

				// TODO: ����޽� ���� ����ó�� �ؾ� ��
				m_pMainTransform->Push_OnNavi(vPushDir * fPushDistance * fPushRatio, m_pNaviCom);
				pOpponent->Push_OnNavi(vPushDir * -1.f * fPushDistance * (1 - fPushRatio));
			}
#pragma endregion 
		}
	}
}

void CE_Qunjing::OnCollisionExit(CCollider * src, CCollider * dest)
{
}