#include "stdafx.h"
#include "..\Public\M_Anjin.h"

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
#include "SoundKey.h"

#include "CameraMovement.h"
#include "Chest.h"
//UI�߰�
#include "UI_Monster.h"
#include "UI_Minimap.h"

CCharacter::SINGLESTATE CM_Anjin::m_tStates[IS_END];

CM_Anjin::CM_Anjin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CM_Anjin::CM_Anjin(const CM_Anjin & rhs)
	: CCharacter(rhs)
{
}

HRESULT CM_Anjin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Anjin::Initialize(void * pArg)
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

	for (_uint i = 0; i < IS_END; ++i)
		m_pModelCom->Get_Animation(m_tStates[i].iAnimID)->Set_TicksPerSecond(m_tStates[i].FramePerSec);

	// ��Ʈ��ǿ� ��ã��
	m_pModelCom->Set_RootBone(TEXT("Root"));

	// �ʱ���ġ ����
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(186.066f, 32.837f, 242.489f, 1.f));
	m_pNaviCom->Set_CurrentIndex(2200);


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
	m_fAttackRange = 7.f;
	m_fCloseRange = 4.f;
	m_bAttackReady = true;

	// CharInfo �ʱ�ȭ
	lstrcpy(m_tMonsterInfo.szName, TEXT("Anjin"));
	m_tMonsterInfo.eElement = ELMT_HAVOC;
	m_tMonsterInfo.iLevel = 1;
	m_tMonsterInfo.iExp = 0;
	m_tMonsterInfo.fMaxHP = 2000.f;
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
	m_fPushWeight = 75.f;

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

void CM_Anjin::Start()
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

	// �ѹ� �ʱ�ȭ �س���
	Update_EffectBones();
}

void CM_Anjin::PreTick(_double TimeDelta)
{
	// �÷��̾� ��ü�� ���� ���õ� ĳ���ͷ� Ÿ���� �����ϴ� �Լ�
	//if(false == m_pTarget->IsActive())
	//	 Find ActivePlayer

	// �� ������ Ÿ�ٱ����� �Ÿ��� ����ϴ� �Լ�, ������ �ø��ؾ� ��
	Find_Target();
}

void CM_Anjin::Tick(_double TimeDelta)
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
	m_pAttackCollider->Update(XMLoadFloat4x4(&m_EffectBoneMatrices[EBONE_RHAND]));

	pGameInstance->AddCollider(m_pHitCollider, COLL_PLAYERATTACK);
	m_pHitCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_MOVE);
	m_pMoveCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	if (true == this->IsActive())
	{
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

void CM_Anjin::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);

	//Effect Bones ó��
	Update_EffectBones();
}

HRESULT CM_Anjin::Render()
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

HRESULT CM_Anjin::RenderShadow()
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

void CM_Anjin::RenderGUI()
{
}

HRESULT CM_Anjin::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_MONSTER_ANJIN,
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
	CollDesc.vCenter = { 0.5f, 0.f, -0.5f };
	CollDesc.vExtents = { 0.5f, 0.1f, 1.f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,

		TEXT("Com_AttackCollider"), (CComponent**)&m_pAttackCollider, &CollDesc)))
		return E_FAIL;


	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 1.f, 0.f };
	CollDesc.vExtents = { 1.f, 1.f, 1.f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_HitCollider"), (CComponent**)&m_pHitCollider, &CollDesc)))
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

HRESULT CM_Anjin::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(SINGLESTATE) * IS_END);

	// �ε��ϴ� �ڵ�
	for (_int i = 0; i < IS_END; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, TEXT("../../Data/CharState/M_Anjin/Anjin_%d.state"), i);
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

void CM_Anjin::Release_States()
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

void CM_Anjin::Shot_EffectKey(_tchar * szEffectTag/* szTag1*/, _uint EffectBoneID /* iInt0 */, _uint iEffectTypeID, _bool bTracking/*iInt1*/)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag, Engine::EFFECT_ID(iEffectTypeID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CM_Anjin::Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID)
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

void CM_Anjin::Shot_OBBKey(_bool bOBB, _uint iAttackInfoID)
{
	m_pAttackCollider->SetActive(bOBB);
	m_iCurAttackID = iAttackInfoID;
}


void CM_Anjin::Shot_PriorityKey(_uint iLeavePriority)
{
	m_tCurState.iLeavePriority = iLeavePriority;
}

void CM_Anjin::SetUp_State()
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

	// �ִϸ��̼��� ������ ���� ��� ��� �ִϸ��̼� ���� �� OBB �ݶ��̴� ������ ����
	m_pAttackCollider->SetActive(false);

	//Position State �ݿ�
	if ((IS_BEHIT_FLY_START == m_Scon.iCurState) &&
		PS_AIR != m_Scon.ePositionState)
	{
		m_Scon.ePositionState = PS_AIR;
		m_Scon.bFalling = false;
	}

	if (IS_BEHIT_FLY_FALL == m_Scon.iCurState)
	{
		m_Scon.ePositionState = PS_GROUND;
	}

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

void CM_Anjin::Change_Target(CCharacter * pActiveCharacter)
{
	m_pTarget = pActiveCharacter;
	m_pTargetTransform = m_pTarget->GetTransform();
}

void CM_Anjin::Find_Target()
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
			m_pMainTransform->Set_LookDir(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f));
		}
	}
}

void CM_Anjin::Init_AttackInfos()
{
	for (_uint i = 0; i < ATK_END; ++i)
	{
		ZeroMemory(&m_AttackInfos[i], sizeof TAGATTACK);
	}

	m_AttackInfos[ATK_ATTACK_01].fDamageFactor = 1.f;
	m_AttackInfos[ATK_ATTACK_01].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_01].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK_01].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_01].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_01].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK_01].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK_02_1].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK_02_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK_02_1].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK_02_1].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_02_1].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_02_1].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK_02_1].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK_02_2].fDamageFactor = 2.5f;
	m_AttackInfos[ATK_ATTACK_02_2].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK_02_2].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK_02_2].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_02_2].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_02_2].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK_02_2].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK_02_3].fDamageFactor = 3.f;
	m_AttackInfos[ATK_ATTACK_02_3].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK_02_3].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK_02_3].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_02_3].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_02_3].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK_02_3].szHitEffectTag, TEXT("GenkiDama_Boom"));

	m_AttackInfos[ATK_ATTACK_03].fDamageFactor = 3.5f;
	m_AttackInfos[ATK_ATTACK_03].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK_03].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK_03].fSPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_03].fTPGain = 0.f;
	m_AttackInfos[ATK_ATTACK_03].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK_03].szHitEffectTag, TEXT("Anjin_Hit"));
}

void CM_Anjin::Init_Missiles()
{
	// Attack01
	//CMissilePool::MISSILEPOOLDESC tMissilePoolDesc;
	//ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	//tMissilePoolDesc.pMissilePoolTag = TEXT("GenkiDama_Shoot_%d");
	//tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	//tMissilePoolDesc.iNumMissiles = 3;

	//lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("GenkiDama_Shoot"));
	//tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial / GAzizi
	//tMissilePoolDesc.tMissileDesc.pOwner = this;
	//tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	//tMissilePoolDesc.tMissileDesc.LifeTime = 3.0;
	//tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK_01;
	//tMissilePoolDesc.tMissileDesc.fExtents = 0.4f;

	//tMissilePoolDesc.bTargetDir = false;
	//tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	//tMissilePoolDesc.fVelocity = 18.f;
	//tMissilePoolDesc.StopTime = 3.0;
	//tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	//m_MissilePools[MISS_ATTACK_01] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 1.2f, 0.f, 0.f), &tMissilePoolDesc);
	//m_MissileRotAngles[MISS_ATTACK_01] = _float3(0.f, 0.f, 0.f);

	//// Attack03
	//ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	//tMissilePoolDesc.pMissilePoolTag = TEXT("GenkiDama_Shoot2_%d");
	//tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	//tMissilePoolDesc.iNumMissiles = 3;

	//lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("GenkiDama_Shoot_02"));
	//tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2; //Tutorial / GAzizi
	//tMissilePoolDesc.tMissileDesc.pOwner = this;
	//tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	//tMissilePoolDesc.tMissileDesc.LifeTime = 3.0;
	//tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK_03;
	//tMissilePoolDesc.tMissileDesc.fExtents = 0.4f;

	//tMissilePoolDesc.bTargetDir = true;
	//tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	//tMissilePoolDesc.fVelocity = 18.f;
	//tMissilePoolDesc.StopTime = 3.0;
	//tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	//m_MissilePools[MISS_ATTACK_03] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 1.2f, 0.f, 0.f), &tMissilePoolDesc);
	//m_MissileRotAngles[MISS_ATTACK_03] = _float3(0.f, 0.f, 0.f);
}

void CM_Anjin::Apply_CoolTime(_double TimeDelta)
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

HRESULT CM_Anjin::SetUp_ShaderResources()
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

HRESULT CM_Anjin::Setup_ShadowShaderResource()
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

void CM_Anjin::Select_State(_double TimeDelta)
{
	AI_STATE iCurFrameAI = AI_NONE;

	if (false == m_bAlert)
	{
		iCurFrameAI = AI_IDLE;
	}
	else
	{
		// iEnterPriority�� 1 == �޺� ������ ������ �ִϸ��̼���
		// iLeavePriority�� 0 == ���� �ִϸ��̼����� �Ѿ �������� �Ǿ���
		if (1 == m_tCurState.iEnterPriority && 0 == m_tCurState.iLeavePriority)
		{
			// ���� Ȯ���� �޺� ���� or �׳� ������ ��������
			if (2 >= rand() % 5)
				iCurFrameAI = AI_COMBO;
			else
			{
				m_tCurState.iLeavePriority = 1;
				m_tCurState.iNextState = IS_IDLE;
			}
		}

		if (IS_RUN == m_Scon.iCurState)
		{
			if (m_fTargetDistance < m_fAttackRange && 0.0 == m_StateCoolTimes[IS_ATTACK03_READY])
			{
				iCurFrameAI = AI_RUSH;
			}
			else if (m_fTargetDistance < m_fCloseRange * 0.8f)
				m_Scon.iCurState = IS_IDLE;
		}

		if (IS_IDLE == m_Scon.iCurState)
		{
			if (m_fCloseRange < m_fTargetDistance)
			{
				iCurFrameAI = AI_CHASE;
			}
			else
			{
				if (true == m_bAttackReady)
				{
					iCurFrameAI = AI_ATTACK;
				}
				else
				{
					iCurFrameAI = AI_STAY;
				}
			}
		}
	}

	switch (iCurFrameAI)
	{
	case Client::CM_Anjin::AI_IDLE:
		m_Scon.iNextState = AI_IDLE;
		break;
	case Client::CM_Anjin::AI_ATTACK:
		if (0.0 == m_StateCoolTimes[IS_ATTACK02_1])
			m_Scon.iNextState = IS_ATTACK02_1;
		else
			m_Scon.iNextState = IS_ATTACK01;
		break;
	case Client::CM_Anjin::AI_RUSH:
		m_Scon.iNextState = IS_ATTACK03_READY;
		break;
	case Client::CM_Anjin::AI_CHASE:
		m_Scon.iNextState = IS_RUN;
		break;
	case Client::CM_Anjin::AI_STAY:
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
	case Client::CM_Anjin::AI_COMBO:
		// �޺� �Ǵ��� �� ��� ���� �޺� ��� ����
		m_Scon.iNextState = m_tCurState.iNextState;
		break;
	default:
		break;
	}

	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f));


	// ���� ���¸� ���� ���� ���·� ���� ���� ����
	if (AI_NONE != iCurFrameAI && AI_IDLE != iCurFrameAI)
	{
		SetUp_State();
		m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
		// ���� ���� �� 1���� ȸ��
		if (CCharacter::ROT_ONSTART == m_tCurState.iRotationType)
		{
			m_pMainTransform->Set_LookDir(vTargetDir);
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

void CM_Anjin::Tick_State(_double TimeDelta)
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
			IS_ATTACK02_1 == m_Scon.iCurState ||
			IS_ATTACK02_2 == m_Scon.iCurState ||
			IS_ATTACK02_3 == m_Scon.iCurState ||
			IS_ATTACK03 == m_Scon.iCurState)
		{
			m_bAttackReady = false;
			m_GlobalCoolTime += 3.f;
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

void CM_Anjin::On_Cell()
{
	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_float fPosY = XMVectorGetY(vPos);
	_float fCellHeight = m_pNaviCom->Compute_Height(vPos);

	if (PS_GROUND == m_Scon.ePositionState)
	{
		m_pMainTransform->Set_PosY(fCellHeight);
	}
	else if (PS_AIR == m_Scon.ePositionState)
	{
		if (fPosY - fCellHeight < 0.f)
		{
			if (false == m_Scon.bFalling)
			{
				// ������ �������� ���� �� ����ó��
				m_pMainTransform->Set_PosY(fCellHeight);
			}
			else
			{
				if (IS_BEHIT_FLY_START == m_Scon.iCurState ||
					IS_BEHIT_FLY_LOOP == m_Scon.iCurState ||
					IS_BEHIT_HOVER == m_Scon.iCurState)
				{
					m_Scon.iNextState = IS_BEHIT_FLY_FALL;
					SetUp_State();
					m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, m_tCurState.bLerp, false);
				}
			}
		}
	}

}

void CM_Anjin::On_Hit(CCharacter * pChar, TAGATTACK * pAttackInfo, _float fAttackPoint, _float3 * pEffPos, _float fCritRate, _float fCritDMG)
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

	Play_HitSound(pAttackInfo);

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

	// ��� �� ��� �ִϸ��̼� ���� 
	if (0.f >= m_tMonsterInfo.fCurHP)
	{
		m_tMonsterInfo.fCurHP = 0.f;
		m_Scon.iNextState = IS_DEAD;
		m_pHitCollider->SetActive(false);
		m_bDying = true;
	}
	// �ǰ� �ִϸ��̼� ����
	else if (PS_GROUND == m_Scon.ePositionState)
	{
		if (IS_BEHIT_FLY_FALL != m_Scon.iCurState &&
			IS_BEHIT_PRESS != m_Scon.iCurState)
		{
			switch (pAttackInfo->eHitIntensity)
			{
			case HIT_SMALL:
				m_pCamMovement->StartVibration();
				m_Scon.iNextState = IS_BEHIT_S;
				break;
			case HIT_BIG:
				m_pCamMovement->StartVibration(10.f, 0.7f);
				m_Scon.iNextState = IS_BEHIT_B;
				break;
			case HIT_FLY:
			{
				//���� ġ�� ����̸� ��ġ �����ؼ� �� �־��ֱ� �ϴ� ����Ʈ ���̺� ����
				m_pCamMovement->StartWave();
				m_Scon.iNextState = IS_BEHIT_FLY_START;
				break;
			}
			default:
				break;
			}
		}
		else
		{
			switch (pAttackInfo->eHitIntensity)
			{
			case HIT_SMALL:
			case HIT_BIG:
				m_Scon.iNextState = IS_BEHIT_PRESS;
				break;
			case HIT_FLY:
				m_Scon.iNextState = IS_BEHIT_FLY_START;
				break;
			default:
				break;
			}
		}
	}
	else if (PS_AIR == m_Scon.ePositionState)
		m_Scon.iNextState = IS_BEHIT_HOVER;

	if (1/*m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority*/)
	{
		m_pMainTransform->Set_LookDir(XMVectorSetY(pChar->Get_Position() - this->Get_Position(), 0.f));
		SetUp_State();
		m_pModelCom->SetUp_Animation(m_tStates[m_Scon.iCurState].iAnimID, false, false);
	}
}


HRESULT CM_Anjin::Init_EffectBones()
{
	//NONE�� �� ���� ��Ʈ������ �����س��ٰ� ����
	m_EffectBones[EBONE_SPINE] = nullptr;
	m_EffectBones[EBONE_LHAND] = nullptr;
	m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));
	m_EffectBones[EBONE_HEAD] = m_pModelCom->Get_BonePtr(TEXT("Bip001Head"));

	return S_OK;
}

void CM_Anjin::Update_EffectBones()
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

CM_Anjin * CM_Anjin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Anjin* pInstance = new CM_Anjin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CM_Anjin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CM_Anjin::Clone(void * pArg)
{
	CM_Anjin* pInstance = new CM_Anjin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CM_Anjin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CM_Anjin::Free()
{
	__super::Free();

	//if (m_bClone)
	//{
	//	for (_uint i = 0; i < MISS_END; ++i)
	//		Safe_Release(m_MissilePools[i]);
	//}


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

void CM_Anjin::OnCollisionEnter(CCollider * src, CCollider * dest)
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

void CM_Anjin::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CM_Anjin::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
