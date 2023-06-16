#include "stdafx.h"
#include "..\Public\M_Qunjing.h"

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

#define DIST_MELEE 5.f
#define DIST_RANGE 12.f

CCharacter::SINGLESTATE CM_Qunjing::m_tStates[IS_END];

CM_Qunjing::CM_Qunjing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CM_Qunjing::CM_Qunjing(const CM_Qunjing & rhs)
	: CCharacter(rhs)
{
}

HRESULT CM_Qunjing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Qunjing::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//if (FAILED(Init_States()))
	//	return E_FAIL;

	if (FAILED(Init_EffectBones()))
		return E_FAIL;

	Init_AttackInfos();
	Init_Missiles();

	m_iStartAttackArray[MA_ATTACK01] = IS_ATTACK01;
	m_iStartAttackArray[MA_ATTACK02] = IS_ATTACK02;
	m_iStartAttackArray[MA_ATTACK03_1] = IS_ATTACK03_1;
	m_iStartAttackArray[MA_ATTACK08] = IS_ATTACK08;
	m_iStartAttackID = MA_ATTACK01;

	for (_uint i = 0; i < IS_END; ++i)
		m_pModelCom->Get_Animation(m_tStates[i].iAnimID)->Set_TicksPerSecond(m_tStates[i].FramePerSec);


	// ��Ʈ��ǿ� ��ã��
	m_pModelCom->Set_RootBone(TEXT("Root"));

	// �ʱ���ġ ����
	_float fRand = _float(rand() % 10);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f + fRand, 0.f, 45.f - fRand, 1.f));
	m_pNaviCom->Set_CurrentIndex(0);

	// StateController �ʱ�ȭ
	m_Scon.iCurState = 0;
	m_Scon.iNextState = 0;
	m_Scon.ePositionState = PS_GROUND;
	m_Scon.iPrevCellState = 0;
	m_tCurState = m_tStates[0];
	SetUp_State();
	m_pModelCom->SetUp_Animation(0, true, false);

	// ���� ���� �ʱ�ȭ
	m_fAlertRange = 15.f;
	m_fAttackRange = 8.f;
	m_fCloseRange = 4.f;
	m_bAttackReady = true;
	m_fHitPoint = 200.f;

	// CharInfo �ʱ�ȭ
	lstrcpy(m_tMonsterInfo.szName, TEXT("Huojin"));
	m_tMonsterInfo.eElement = ELMT_FUSION;
	m_tMonsterInfo.iLevel = 1;
	m_tMonsterInfo.iExp = 0;
	m_tMonsterInfo.fMaxHP = 10000.f;
	m_tMonsterInfo.fCurHP = m_tMonsterInfo.fMaxHP;
	//m_tMonsterInfo.fMaxSP = 100.f;
	//m_tMonsterInfo.fCurSP = 0.f;
	//m_tMonsterInfo.fMaxTP = 100.f;
	//m_tMonsterInfo.fCurTP = 0.f;
	m_tMonsterInfo.fAttack = 50.f;
	m_tMonsterInfo.fDefense = 50.f;
	m_tMonsterInfo.fCriticalRate = 0.1f;

	// �浹 Ÿ�� ó��
	m_eCollisionType = CT_MONSTER;
	m_fPushWeight = 2000.f;

	//m_pAttackCollider->SetActive(false);

	//UI�߰�
	CGameInstance* pGame = CGameInstance::GetInstance();
	_tchar szIndex[MAX_PATH];
	wsprintf(szIndex, TEXT("UI_Monster%d"), Monindex);
	CUI_Monster::MONINFO MonInfo;
	MonInfo.Level = 3;
	MonInfo.Type = CUI_Monster::MONSTERTYPE::TYPE0;
	CGameObject * pUIMon = nullptr;
	if (pGame->Add_GameObjectEx(&pUIMon, LEVEL_ANYWHERE, OBJECT::UIMONSTER, TEXT("layer_UI"), szIndex, &MonInfo))
		return E_FAIL;
	m_pUIMon = static_cast<CUI_Monster*>(pUIMon);
	m_pUIMon->Set_MonHP(m_tMonsterInfo.fMaxHP);
	++Monindex;
	return S_OK;
}

void CM_Qunjing::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

#ifdef _DEBUG
	m_pRendererCom->DebugBundleRender_Control(true);
#endif

	m_pCamMovement = static_cast<CCameraMovement*>(pGame->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));

	// Find ActivePlayer
	m_pTarget = static_cast<CPlayerState*>(pGame->Find_GameObject(LEVEL_STATIC, L"CharacterState"))->Get_ActiveCharacter();
	m_pTargetTransform = m_pTarget->GetTransform();

	//UI�߰�
	m_pUIIcon = static_cast<CUI_Minimap*>(pGame->Find_GameObject(LEVEL_ANYWHERE, TEXT("UI_Minimap")));
	m_UIIndex = m_pUIIcon->Add_Icon(m_pMainTransform->Get_State(CTransform::STATE_POSITION), CUI_Minimap::MOSTER);
	m_pUIIcon->SetRender(m_UIIndex, false);
}

void CM_Qunjing::PreTick(_double TimeDelta)
{
	// �÷��̾� ��ü�� ���� ���õ� ĳ���ͷ� Ÿ���� �����ϴ� �Լ�
	//if(false == m_pTarget->IsActive())
	//	 Find ActivePlayer

	

	// �� ������ Ÿ�ٱ����� �Ÿ��� ����ϴ� �Լ�, ������ �ø��ؾ� ��
	Find_Target();

	Check_Derive();
}

void CM_Qunjing::Tick(_double TimeDelta)
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

	Apply_CoolTime(TimeDelta * TimeDelay); // ��Ÿ�� ����

	Select_State(TimeDelta * TimeDelay); // ���� Ȯ��

	Tick_State(TimeDelta * TimeDelay); // PlayAnimation, �ִϸ��̼ǿ� ���� �̵�, �ִϸ��̼� ���� �� ó��

	On_Cell(); // �ڹ����� ������ ��ó�� >> �ַ� ���� �޽�

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pAttackCollider, COLL_MONSTERATTACK);
	if (true == m_bOBBOnLeft)
		m_pAttackCollider->Update(XMLoadFloat4x4(&m_EffectBoneMatrices[EBONE_LHAND]));
	else
		m_pAttackCollider->Update(XMLoadFloat4x4(&m_EffectBoneMatrices[EBONE_RHAND]));

	pGameInstance->AddCollider(m_pHitCollider, COLL_PLAYERATTACK);
	m_pHitCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_MOVE);
	m_pMoveCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	if (true == this->IsActive())
	{// ���Ͱ� ��Ƽ��->ui����on
		if (false == m_pUIMon->IsActive())
			m_pUIMon->SetState(ACTIVE);

		_float4 Head;
		XMStoreFloat4(&Head, XMLoadFloat4x4(&m_EffectBoneMatrices[EBONE_HEAD]).r[3]);
		Head.y += 0.5f;
		m_pUIMon->Set_CharacterPos(XMLoadFloat4(&Head));

		if (false == m_pUIIcon->GetRenderState(m_UIIndex)) 
			m_pUIIcon->SetRender(m_UIIndex, true);
		m_pUIIcon->Set_ObjectPos(m_UIIndex, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
	}
}

void CM_Qunjing::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	//Effect Bones ó��
	Update_EffectBones();


	// ������ ��ų ����ó�� // �Ÿ����ؼ�
	if (m_fTargetDistance <= 1.5f)
	{
		static _bool bOverlapedCheck = false;

		if ((IS_ATTACK02 == m_Scon.iCurState)
			&& true == m_tCurState.bRootMotion)
		{
			static _vector vTargetPos;

			_float fExtents = m_pTarget->GetMoveCollider()->GetExtents().x;

			if (!bOverlapedCheck)
			{
				bOverlapedCheck = true;
				vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
			}

			_vector vCurPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
			_vector vOffSetPos = vTargetPos + XMVector3Normalize(vCurPos - vTargetPos) * fExtents;

			vCurPos = XMVectorSetY(XMVectorLerp(vCurPos, vOffSetPos, (_float)TimeDelta * 10.f), XMVectorGetY(vCurPos));

			m_tCurState.bRootMotion = false;
			m_Scon.vMovement = _float3(0.f, 0.f, 0.f);

			m_pMainTransform->Set_State(CTransform::STATE_POSITION, vCurPos);
		}
		else
		{
			bOverlapedCheck = false;
		}
	}
}

HRESULT CM_Qunjing::Render()
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

HRESULT CM_Qunjing::RenderShadow()
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

void CM_Qunjing::RenderGUI()
{
}

HRESULT CM_Qunjing::Add_Components()
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

	// For.Com_Shader_ModelAnim
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, SHADER::MODELANIM,
		TEXT("Com_Shader_ModelAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_MONSTER_QUNJING,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATION_DESC));
	NavigationDesc.iCurrentIndex = 0;

	/* Navigation */
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, COMPONENT::NAVIGATION,
		TEXT("Com_Navigation"), (CComponent**)&m_pNaviCom, &NavigationDesc)))
		return E_FAIL;

	CCollider::COLLIDER_DESC CollDesc;
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.8f, 0.8f, 0.8f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	// attack Hit / Move
	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.8f, 0.5f, 0.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,

		TEXT("Com_AttackCollider"), (CComponent**)&m_pAttackCollider, &CollDesc)))
		return E_FAIL;


	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 1.5f, 0.f };
	CollDesc.vExtents = { 1.5f, 1.5f, 1.5f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_HitCollider"), (CComponent**)&m_pHitCollider, &CollDesc)))
		return E_FAIL;


	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 0.f, 0.f };
	CollDesc.vExtents = { 0.8f, 0.8f, 0.8f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_MoveCollider"), (CComponent**)&m_pMoveCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Qunjing::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(SINGLESTATE) * IS_END);

	// �ε��ϴ� �ڵ�
	for (_int i = 0; i < IS_END; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, TEXT("../../Data/CharState/M_Qunjing/Qunjing_%d.state"), i);
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
				case CStateKey::TYPE_PARTS:
					//m_tStates[i].ppStateKeys[j] = CPartsKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_PRIORITY:
					m_tStates[i].ppStateKeys[j] = CPriorityKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_DISSOLVE:
					m_tStates[i].ppStateKeys[j] = CDissolveKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_OBB:
					m_tStates[i].ppStateKeys[j] = COBBKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_MISSILE:
					m_tStates[i].ppStateKeys[j] = CMissileKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_SOUND:

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

void CM_Qunjing::Release_States()
{
	for (_int i = 0; i < IS_END; ++i)
	{
		if (0 == m_tStates[i].iKeyCount)
			continue;
		for (_uint j = 0; j < m_tStates[i].iKeyCount; ++j)
		{
			if (nullptr != m_tStates[i].ppStateKeys[j])
				Safe_Release(m_tStates[i].ppStateKeys[j]);
		}
		Safe_Delete_Array(m_tStates[i].ppStateKeys);
	}
}

void CM_Qunjing::Shot_EffectKey(_tchar * szEffectTag/* szTag1*/, _uint EffectBoneID /* iInt0 */, _uint iEffectTypeID, _bool bTracking/*iInt1*/)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag, Engine::EFFECT_ID(iEffectTypeID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CM_Qunjing::Shot_OBBKey(_bool bOBB, _uint iAttackInfoID)
{
	m_pAttackCollider->SetActive(bOBB);
	m_iCurAttackID = iAttackInfoID;

	if (ATK_ATTACK01_2 == iAttackInfoID ||
		ATK_ATTACK02_3 == iAttackInfoID)
	{
		m_bOBBOnLeft = true;
	}
	else if (ATK_ATTACK03_1 == iAttackInfoID)
	{
		m_bOBBOnLeft = true;
		m_pAttackCollider->SetCenter_(_float3(0.f, 0.f, -0.7f));
	}
	
	else
	{
		m_pAttackCollider->SetCenter_(_float3(0.6f, 0.f, 0.f));
		m_bOBBOnLeft = false;
	}
		

}

void CM_Qunjing::Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID)
{
	if (MISS_END <= iMissilePoolID || EBONE_END <= iEffectBoneID)
		return;

	_vector vInitPos;
	if (0 != iEffectBoneID)
	{
		vInitPos = XMVector3TransformCoord(
			XMVector3TransformCoord(m_EffectBones[iEffectBoneID]->Get_CombinedPosition(), XMMatrixRotationY(XMConvertToRadians(180.f))),
			XMLoadFloat4x4(m_pMainTransform->Get_WorldMatrixPtr()));
	}
	else
		vInitPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);



	_matrix matRot = XMMatrixRotationAxis(m_pMainTransform->Get_State(CTransform::STATE_RIGHT), m_MissileRotAngles[iMissilePoolID].x)
		* XMMatrixRotationAxis(m_pMainTransform->Get_State(CTransform::STATE_UP), m_MissileRotAngles[iMissilePoolID].y)
		* XMMatrixRotationAxis(m_pMainTransform->Get_State(CTransform::STATE_LOOK), m_MissileRotAngles[iMissilePoolID].z);

	_vector vTargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_MissilePools[iMissilePoolID]->Shot(vInitPos, m_pMainTransform->Get_State(CTransform::STATE_LOOK), matRot, vTargetPos);
}

void CM_Qunjing::Shot_PriorityKey(_uint iLeavePriority)
{
	m_tCurState.iLeavePriority = iLeavePriority;
}

void CM_Qunjing::SetUp_State()
{
	// Ű ����
	for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
	{
		if (nullptr != m_tCurState.ppStateKeys[i])
			m_tCurState.ppStateKeys[i]->Reset();
	}

	// ���Է� ����ó��
	if (m_Scon.iNextState >= IS_END)
		m_Scon.iNextState = IS_END - 1;
	else if (m_Scon.iNextState <= 0)
		m_Scon.iNextState = 0;

	// ���� �ִϸ��̼����� ����
	m_Scon.iCurState = m_Scon.iNextState;
	m_tCurState = m_tStates[m_Scon.iCurState];

	m_Scon.TrackPos = 0.0;
	m_Scon.bAnimFinished = false;

	// 
	m_bAttackHit = false;
	for (_uint i = 0; i < MA_END; ++i)
	{
		if (m_Scon.iCurState == m_iStartAttackArray[i])
		{
			++m_iStartAttackID;
			if (MA_END <= m_iStartAttackID)
				m_iStartAttackID = MA_ATTACK01;
		}

	}

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

	// ��Ÿ�� ����
	if (true == m_tCurState.bApplyCoolTime)
		m_StateCoolTimes[m_Scon.iCurState] = m_tCurState.CoolTime;
}

void CM_Qunjing::Change_Target(CCharacter * pActiveCharacter)
{
	m_pTarget = pActiveCharacter;
	m_pTargetTransform = m_pTarget->GetTransform();
}

void CM_Qunjing::Find_Target()
{
	// �������� Ÿ�ٱ����� �Ÿ��� ����س��´�.
	m_fTargetDistance = XMVectorGetX(XMVector3Length(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION)));
	// Ÿ������ ���� ģ���� �÷��̾� ĳ���Ϳ��� ��
	m_pTarget->Check_Nearst(this, m_fTargetDistance);

	if (false == m_bAlert)
	{
		if (m_fAlertRange > m_fTargetDistance)
		{
			m_bAlert = true;
			m_Scon.iNextState = IS_GREETING;
			SetUp_State();
			m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true, false);

			m_fHitPoint = 200.f;
		}
	}
}

void CM_Qunjing::Check_Derive()
{
	if (IS_ATTACK03_1 == m_Scon.iCurState)
	{
		if (IS_ATTACK03_2 != m_tCurState.iNextState)
		{
			if (true == m_bAttackHit)
			{
				m_tCurState.iNextState = IS_ATTACK03_2;
			}
		}
	}
	else if (IS_ATTACK03_3 == m_Scon.iCurState && 90.0 <= m_Scon.TrackPos && m_Scon.TrackPos < 96.0)
	{
		if (m_fTargetDistance < DIST_MELEE)
			m_Scon.iNextState = IS_ATTACK03_4;
		SetUp_State();
		m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
	}

}

void CM_Qunjing::Init_AttackInfos()
{
	for (_uint i = 0; i < ATK_END; ++i)
	{
		ZeroMemory(&m_AttackInfos[i], sizeof TAGATTACK);
	}

	m_AttackInfos[ATK_ATTACK01_1].fDamageFactor = 0.2f;
	m_AttackInfos[ATK_ATTACK01_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK01_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK01_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_1].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK01_1].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK01_2].fDamageFactor = 1.1f;
	m_AttackInfos[ATK_ATTACK01_2].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK01_2].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK01_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_2].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK01_2].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK01_3].fDamageFactor = 1.2f;
	m_AttackInfos[ATK_ATTACK01_3].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK01_3].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK01_3].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_3].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_3].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK01_3].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK01_4].fDamageFactor = 1.8f;
	m_AttackInfos[ATK_ATTACK01_4].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK01_4].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK01_4].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_4].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK01_4].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK01_4].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK02_1].fDamageFactor = 1.f;
	m_AttackInfos[ATK_ATTACK02_1].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK02_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK02_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_1].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK02_1].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK02_2].fDamageFactor = 0.3f;
	m_AttackInfos[ATK_ATTACK02_2].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK02_2].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK02_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_2].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK02_2].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK02_3].fDamageFactor = 1.5f;
	m_AttackInfos[ATK_ATTACK02_3].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK02_3].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK02_3].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_3].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_3].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK02_3].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK02_4].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK02_4].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK02_4].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK02_4].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_4].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK02_4].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK02_4].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK03_1].fDamageFactor = 1.f;
	m_AttackInfos[ATK_ATTACK03_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK03_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK03_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK03_1].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK03_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK03_1].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK03_2].fDamageFactor = 2.4f;
	m_AttackInfos[ATK_ATTACK03_2].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK03_2].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK03_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK03_2].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK03_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK03_2].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK03_4].fDamageFactor = 2.8f;
	m_AttackInfos[ATK_ATTACK03_4].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK03_4].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK03_4].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK03_4].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK03_4].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK03_4].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK04].fDamageFactor = 2.1f;
	m_AttackInfos[ATK_ATTACK04].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK04].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK04].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK04].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK04].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK04].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK06].fDamageFactor = 1.8f;
	m_AttackInfos[ATK_ATTACK06].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK06].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK06].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK06].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK06].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK06].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK07_1].fDamageFactor = 0.7f;
	m_AttackInfos[ATK_ATTACK07_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK07_1].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK07_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK07_1].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK07_1].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK07_1].szHitEffectTag, TEXT("M_Yellow_Hit"));

	m_AttackInfos[ATK_ATTACK07_2].fDamageFactor = 1.9f;
	m_AttackInfos[ATK_ATTACK07_2].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK07_2].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK07_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK07_2].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK07_2].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK07_2].szHitEffectTag, TEXT("M_Yellow_Hit"));

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

	m_AttackInfos[ATK_ATTACK08_3].fDamageFactor = 1.6f;
	m_AttackInfos[ATK_ATTACK08_3].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK08_3].eElementType = ELMT_NONE;
	m_AttackInfos[ATK_ATTACK08_3].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK08_3].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK08_3].iHitEffectID = 0;
	lstrcpy(m_AttackInfos[ATK_ATTACK08_3].szHitEffectTag, TEXT("M_Yellow_Hit"));

}

void CM_Qunjing::Init_Missiles()
{
	CMissilePool::MISSILEPOOLDESC tMissilePoolDesc;
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	// Attack01_1
	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack01_1_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK01_1;
	tMissilePoolDesc.tMissileDesc.fExtents = 2.5f;

	m_MissilePools[MISS_ATTACK01_1] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK01_1] = _float3(0.f, 0.f, 0.f);

	// Attack01_4
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack01_4_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK01_4;
	tMissilePoolDesc.tMissileDesc.fExtents = 2.5f;

	tMissilePoolDesc.bTargetDir = false;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 15.f;
	tMissilePoolDesc.StopTime = 3.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK01_4] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK01_4] = _float3(0.f, 0.f, 0.f);

	// ATK_ATTACK02_4
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack02_2_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK02_4;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.5f;

	m_MissilePools[MISS_ATTACK02_4] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK02_4] = _float3(0.f, 0.f, 0.f);

	// Attack03_2
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack03_2_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK03_2;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	tMissilePoolDesc.bTargetDir = false;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 15.f;
	tMissilePoolDesc.StopTime = 3.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK03_2] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK03_2] = _float3(0.f, 0.f, 0.f);

	// Attack03_4
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack03_4_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK03_4;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.5f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_ATTACK03_4] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK03_4] = _float3(0.f, 0.f, 0.f);

	// Attack04
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack04_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK04;
	tMissilePoolDesc.tMissileDesc.fExtents = 3.5f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_ATTACK04] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK04] = _float3(0.f, 0.f, 0.f);

	// Attack07_1
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack07_1_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK07_1;
	tMissilePoolDesc.tMissileDesc.fExtents = 2.5f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_ATTACK07_1] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK07_1] = _float3(0.f, 0.f, 0.f);

	// Attack07_2
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack07_2_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("M_Qunjing_Attack_Bullet"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 7; //Qunjing
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 3.0;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK07_2;
	tMissilePoolDesc.tMissileDesc.fExtents = 2.f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 45.f;
	tMissilePoolDesc.StopTime = 3.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK07_2] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK07_2] = _float3(0.f, 0.f, 0.f);

	// Attack08_2
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack08_2_%d");
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

	m_MissilePools[MISS_ATTACK08_2] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK08_2] = _float3(0.f, 0.f, 0.f);

	// Attack08_3
	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Attack08_3_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT(""));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial 
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.3;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK08_3;
	tMissilePoolDesc.tMissileDesc.fExtents = 2.5f;
	tMissilePoolDesc.tMissileDesc.bNoShutDownEffect = true;

	m_MissilePools[MISS_ATTACK08_3] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK08_3] = _float3(0.f, 0.f, 0.f);
}

void CM_Qunjing::Apply_CoolTime(_double TimeDelta)
{
	if (0.0 < m_GlobalCoolTime)
	{
		m_GlobalCoolTime -= TimeDelta;
		if (0.0 > m_GlobalCoolTime)
		{
			m_GlobalCoolTime = 0.0;
			m_bAttackReady = true;
		}
	}

	for (_uint i = 0; i < IS_END; ++i)
	{
		if (0.0 < m_StateCoolTimes[i])
		{
			m_StateCoolTimes[i] -= TimeDelta;
			if (0.0 > m_StateCoolTimes[i])
				m_StateCoolTimes[i] = 0.0;
		}
	}
}

HRESULT CM_Qunjing::SetUp_ShaderResources()
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

HRESULT CM_Qunjing::Setup_ShadowShaderResource()
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

void CM_Qunjing::Select_State(_double TimeDelta)
{
	AI_STATE iCurFrameAI = AI_NONE;

	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f));
	_vector vLook = XMVector3Normalize(m_pMainTransform->Get_State(CTransform::STATE_LOOK));

	if (false == m_bAlert)
	{
		iCurFrameAI = AI_IDLE;
	}
	else
	{
		if (DIST_RANGE < m_fTargetDistance)
		{
			iCurFrameAI = AI_CHASE;
		}
		else
		{
			if (true == m_bAttackReady)
			{
				if (DIST_MELEE < m_fTargetDistance)
				{
					iCurFrameAI = AI_ATTACK_RANGE;
				}
				else
				{
					iCurFrameAI = AI_ATTACK_MELEE;
				}
			}
			else
			{
				iCurFrameAI = AI_STAY;
			}
		}
	}

	switch (iCurFrameAI)
	{
	case Client::CM_Qunjing::AI_IDLE:
		m_Scon.iNextState = IS_IDLE;
		break;
	case Client::CM_Qunjing::AI_ATTACK_MELEE:
		if (0.0 == m_StateCoolTimes[IS_ATTACK06] && 0.f > XMVectorGetX(XMVector3Dot(vTargetDir, vLook)))
			m_Scon.iNextState = IS_ATTACK06;
		else 
			m_Scon.iNextState = m_iStartAttackArray[ m_iStartAttackID];
		break;
	case Client::CM_Qunjing::AI_ATTACK_RANGE:
		if (0.0 == m_StateCoolTimes[IS_ATTACK07])
			m_Scon.iNextState = IS_ATTACK07;
		else
			m_Scon.iNextState = IS_ATTACK04;
		break;
	case Client::CM_Qunjing::AI_CHASE:
		m_Scon.iNextState = IS_RUN;
		break;
	case Client::CM_Qunjing::AI_STAY:
		if (m_fAttackRange * 0.7f < m_fTargetDistance)
		{
			m_Scon.iNextState = IS_WALK_F;
		}
		else if (m_fAttackRange * 0.3f < m_fTargetDistance)
		{
			_int iRand = rand() % 2;
			if (0 == iRand)
				m_Scon.iNextState = IS_WALK_L;
			else
				m_Scon.iNextState = IS_WALK_B;
		}
		else
			m_Scon.iNextState = IS_WALK_B;
		break;
	default:
		break;
	}


	// ���� ���¸� ���� ���� ���·� ���� ���� ����
	if (m_Scon.iCurState != m_Scon.iNextState)
	{
		if (m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority)
		{
			SetUp_State();
			m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, false);
			// ���� ���� �� 1���� ȸ��
			if (CCharacter::ROT_ONSTART == m_tCurState.iRotationType)
			{
				m_pMainTransform->Set_LookDir(vTargetDir);
			}
		}
	}

	// �� ������ ȸ��
	if (CCharacter::ROT_LOOKAT == m_tCurState.iRotationType)
	{
		m_pMainTransform->Set_LookDir(vTargetDir);
	}
	else if (CCharacter::ROT_TURN == m_tCurState.iRotationType)
	{
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

void CM_Qunjing::Tick_State(_double TimeDelta)
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
		if (IS_DEAD == m_Scon.iCurState)
		{
			SetState(DISABLE);
			m_pUIMon->SetState(DISABLE);
			m_pUIIcon->SetRender(m_UIIndex, false);
		}

		// ���� �ൿ ��
		if (IS_ATTACK01 == m_Scon.iCurState ||
			IS_ATTACK02 == m_Scon.iCurState ||
			IS_ATTACK03_2 == m_Scon.iCurState ||
			IS_ATTACK03_4 == m_Scon.iCurState ||
			IS_ATTACK04 == m_Scon.iCurState ||
			IS_ATTACK06 == m_Scon.iCurState ||
			IS_ATTACK07 == m_Scon.iCurState ||
			IS_ATTACK08 == m_Scon.iCurState)
		{
			m_bAttackReady = false;
			m_GlobalCoolTime += 3.f;
		}

		if (IS_BEHIT_B == m_Scon.iCurState ||
			IS_BEHIT_S == m_Scon.iCurState)
		{
			m_fHitPoint = 200.f;
		}

		if (true == m_tCurState.bLoop)
		{
			m_Scon.iNextState = m_Scon.iCurState;
			SetUp_State();
			m_tCurState.bLerp = false;
			m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, m_tCurState.bLerp, true);

		}
		else
		{
			if (NO_ANIM != m_tCurState.iNextState)
			{
				m_Scon.iNextState = m_tCurState.iNextState;
				SetUp_State();
				m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, m_tCurState.bLerp, true);
			}
		}
	}
}

void CM_Qunjing::On_Cell()
{
	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_float fPosY = XMVectorGetY(vPos);
	_float fCellHeight = m_pNaviCom->Compute_Height(vPos);

	if (PS_GROUND == m_Scon.ePositionState)
	{
		if(m_Scon.iCurState != IS_ATTACK04)
			m_pMainTransform->Set_PosY(fCellHeight);
	}
}

void CM_Qunjing::On_Hit(CCharacter * pChar, TAGATTACK * pAttackInfo, _float fAttackPoint, _float3 * pEffPos, _float fCritRate, _float fCritDMG)
{
	// �ǰ� ����Ʈ ���
	if (lstrcmp(pAttackInfo->szHitEffectTag, TEXT("")))
	{
		CGameInstance* pGI = CGameInstance::GetInstance();
		_float4x4 EffectMatrix = m_pMainTransform->Get_WorldMatrix();
		memcpy(EffectMatrix.m[3], pEffPos, sizeof(_float3));
		pGI->Get_Effect(pAttackInfo->szHitEffectTag, (EFFECT_ID)pAttackInfo->iHitEffectID)->Play_Effect(&EffectMatrix);
	}

	pChar->Recover_Gauge(pAttackInfo->fSPGain, pAttackInfo->fBPGain, pAttackInfo->fTPGain);

	CGameMode* pGM = CGameMode::GetInstance();

	// ����� ��� ���� : ��� ��� * ���ݷ� * ((���ݷ� * 2 - ����) / ���ݷ�) * (�Ӽ� ���ʽ�)
	// ���ݷ°� ������ ���� �� 1�� �����
	_float fFinalDamage = pAttackInfo->fDamageFactor * fAttackPoint * ((fAttackPoint * 2 - m_tMonsterInfo.fDefense) / fAttackPoint) /** �Ӽ� ���ʽ� */;
	_bool bCrit = false;
	if (fCritRate > _float(rand() % 100))
	{
		bCrit = true;
		fFinalDamage *= fCritDMG * 0.01f;
	}
	fFinalDamage *= _float(110 - (rand() % 20)) * 0.01f;
	m_tMonsterInfo.fCurHP -= fFinalDamage;

	// TODO: ���⼭ ����� ��Ʈ ���
	if (true == this->IsActive())
	{
		m_pUIMon->Set_Damage(fFinalDamage, bCrit);
	}

	_bool bHitCheck = false;

	// ��� �� ��� �ִϸ��̼� ���� 
	if (0.f >= m_tMonsterInfo.fCurHP)
	{
		m_tMonsterInfo.fCurHP = 0.f;
		m_Scon.iNextState = IS_DEAD;
		m_bDying = true;
		m_pHitCollider->SetActive(false);
		bHitCheck = true;
	}
	// �ǰ� �ִϸ��̼� ����
	else if (PS_GROUND == m_Scon.ePositionState)
	{
		switch (pAttackInfo->eHitIntensity)
		{
		case HIT_NONE:
			m_fHitPoint -= 3.f;
			break;
		case HIT_SMALL:
			m_fHitPoint -= 10.f;
			if (m_fHitPoint < 0.f)
			{
				m_pCamMovement->StartVibration();
				if (m_fHitPoint < -50.f)
					m_Scon.iNextState = IS_BEHIT_B;
				else
					m_Scon.iNextState = IS_BEHIT_S;
				bHitCheck = true;
			}
			break;
		case HIT_BIG:
			m_fHitPoint -= 25.f;
			if (m_fHitPoint < 0.f)
			{
				m_pCamMovement->StartVibration(10.f, 0.7f);
				bHitCheck = true;
				if (m_fHitPoint < -50.f)
					m_Scon.iNextState = IS_BEHIT_B;
				else
					m_Scon.iNextState = IS_BEHIT_S;
			}
			break;
		case HIT_FLY:
		{
			m_fHitPoint -= 60.f;

			if (m_fHitPoint < 0)
			{
				//���� ġ�� ����̸� ��ġ �����ؼ� �� �־��ֱ� �ϴ� ����Ʈ ���̺� ����
				m_pCamMovement->StartWave();
				bHitCheck = true;
				if (m_fHitPoint < -50.f)
					m_Scon.iNextState = IS_BEHIT_B;
				else
					m_Scon.iNextState = IS_BEHIT_S;
			}
			break;
		}
		default:
			break;
		}

	}

	if (bHitCheck == true && m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority && m_Scon.iCurState != m_Scon.iNextState)
	{
		m_pMainTransform->Set_LookDir(XMVectorSetY(pChar->Get_Position() - this->Get_Position(), 0.f));
		SetUp_State();
		m_pModelCom->SetUp_Animation(m_tStates[m_Scon.iCurState].iAnimID, false, false);
	}
}

HRESULT CM_Qunjing::Init_EffectBones()
{
	//NONE�� �� ���� ��Ʈ������ �����س��ٰ� ����
	m_EffectBones[EBONE_SPINE] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine"));
	m_EffectBones[EBONE_LHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001LHand"));
	m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));
	m_EffectBones[EBONE_HEAD] = m_pModelCom->Get_BonePtr(TEXT("Bip001Head"));
	m_EffectBones[EBONE_LFOOT] = m_pModelCom->Get_BonePtr(TEXT("Bip001LFoot"));

	return S_OK;
}

void CM_Qunjing::Update_EffectBones()
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

CM_Qunjing * CM_Qunjing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Qunjing* pInstance = new CM_Qunjing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CM_Qunjing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CM_Qunjing::Clone(void * pArg)
{
	CM_Qunjing* pInstance = new CM_Qunjing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CM_Qunjing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CM_Qunjing::Free()
{
	__super::Free();

	if (m_bClone)
	{
		for (_uint i = 0; i < MISS_END; ++i)
			Safe_Release(m_MissilePools[i]);
	}

	Safe_Release(m_pNaviCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pMainTransform);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pCollider);
	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pHitCollider);
	Safe_Release(m_pMoveCollider);

}

void CM_Qunjing::OnCollisionEnter(CCollider * src, CCollider * dest)
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
				m_pCamMovement->StartVibration(10.f, 0.5f);
			}

			// ����� ������ ������ ������ ��� 
			if (true == src->Compare(GetHitCollider()) &&
				true == dest->Compare(pOpponent->GetAttackCollider()))
			{
				if (IS_ATTACK03_1 == m_Scon.iCurState && m_Scon.TrackPos < 21.0)
				{
					m_Scon.iNextState = IS_ATTACK03_2;
					SetUp_State();
					m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, m_tCurState.bLerp);
				}
				else
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
				if (IS_ATTACK03_1 == m_Scon.iCurState && m_Scon.TrackPos < 21.0)
				{
					m_Scon.iNextState = IS_ATTACK03_2;
					SetUp_State();
					m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, m_tCurState.bLerp);
				}
				else
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


}

void CM_Qunjing::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CM_Qunjing::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
