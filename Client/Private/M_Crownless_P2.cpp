#include "stdafx.h"
#include "..\Public\M_Crownless_P2.h"

#include "GameMode.h"
#include "GameInstance.h"
#include "Parts.h"
#include "Effect.h"
#include "EffectKey.h"
#include "PartsKey.h"
#include "PriorityKey.h"
#include "SlowKey.h"
#include "Missile.h"
#include "MissileKey.h"
#include "Missile_Constant.h"
#include "OBBKey.h"
#include "DissolveKey.h"
#include "TraceKey.h"

#include "CameraMovement.h"
#include "Chest.h"
//UI추가
#include "UI_Monster.h"
#include "UI_Minimap.h"

#define DIST_MELEE 3.5f
#define DIST_MIDRANGE 15.f

const _double CM_Crownless_P2::m_TraceInterval = 0.09;

CCharacter::SINGLESTATE CM_Crownless_P2::m_tStates[IS_END];
CM_Crownless_P2::CM_Crownless_P2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CM_Crownless_P2::CM_Crownless_P2(const CM_Crownless_P2 & rhs)
	: CCharacter(rhs)
{
}

HRESULT CM_Crownless_P2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Crownless_P2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//if (FAILED(Init_States()))
	//	return E_FAIL;

	if (FAILED(Init_EffectBones()))
		return E_FAIL;

	Init_AnimSystem();

	Init_AttackInfos();
	Init_Missiles();

	Init_Traces();

	m_bTraceOn = false;

	m_iMeleeStartAttackArray[MA_ATTACK03] = IS_ATTACK03;
	m_iMeleeStartAttackArray[MA_ATTACK05] = IS_ATTACK05;
	m_iMeleeStartAttackArray[MA_ATTACK08] = IS_ATTACK08;
	m_iMeleeStartAttackArray[MA_ATTACK10] = IS_ATTACK10;
	m_iMeleeStartAttackArray[MA_ATTACK12] = IS_ATTACK12;
	m_iMeleeStartAttackArray[MA_ATTACK18] = IS_ATTACK18;
	m_iMeleeStartAttackArray[MA_ATTACK20] = IS_ATTACK20;

	for (_uint i = 0; i < IS_END; ++i)
		m_pModelCom->Get_Animation(m_tStates[i].iAnimID)->Set_TicksPerSecond(m_tStates[i].FramePerSec);

	// 루트모션용 본찾기
	m_pModelCom->Set_RootBone(TEXT("Root"));

	// 초기위치 설정
	_float fRand = _float(rand() % 10);
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(45.f + fRand, 0.f, 45.f - fRand, 1.f));
	m_pNaviCom->Set_CurrentIndex(0);

	// StateController 초기화
	m_Scon.iCurState = 0;
	m_Scon.iNextState = 0;
	m_Scon.ePositionState = PS_GROUND;
	m_Scon.iPrevCellState = 0;
	m_tCurState = m_tStates[0];
	SetUp_State();
	m_pModelCom->SetUp_Animation(0, true, false);

	// 고유 변수 초기화
	m_fAlertRange = 15.f;
	m_bAttackReady = true;

	// CharInfo 초기화
	lstrcpy(m_tMonsterInfo.szName, TEXT("Crownless"));
	m_tMonsterInfo.eElement = ELMT_HAVOC;
	m_tMonsterInfo.iLevel = 45;
	m_tMonsterInfo.iExp = 0;
	m_tMonsterInfo.fMaxHP = 200000.f;
	m_tMonsterInfo.fCurHP = m_tMonsterInfo.fMaxHP;
	m_tMonsterInfo.fAttack = 50.f;
	m_tMonsterInfo.fDefense = 50.f;
	m_tMonsterInfo.fCriticalRate = 0.1f;

	// 충돌 타입 처리
	m_eCollisionType = CT_MONSTER;
	m_fPushWeight = 150.f;


	//m_pAttackCollider->SetActive(false);



	//UI추가
	CGameInstance* pGame = CGameInstance::GetInstance();
	_tchar szIndex[MAX_PATH];
	wsprintf(szIndex, TEXT("UI_Monster%d"), Monindex);
	CUI_Monster::MONINFO MonInfo;
	MonInfo.Level = 99;
	MonInfo.Type = CUI_Monster::MONSTERTYPE::TYPE1;
	CGameObject * pUIMon = nullptr;
	if (pGame->Add_GameObjectEx(&pUIMon, LEVEL_ANYWHERE, OBJECT::UIMONSTER, TEXT("layer_UI"), szIndex, &MonInfo))
		return E_FAIL;
	m_pUIMon = static_cast<CUI_Monster*>(pUIMon);
	m_pUIMon->Set_MonHP(m_tMonsterInfo.fMaxHP);
	++Monindex;


	return S_OK;
}

void CM_Crownless_P2::Start()
{
	CGameInstance* pGame = CGameInstance::GetInstance();

#ifdef _DEBUG
	m_pRendererCom->DebugBundleRender_Control(true);
#endif

	m_pCamMovement = static_cast<CCameraMovement*>(pGame->Find_GameObject(LEVEL_STATIC, L"CameraMovement"));

	// Find ActivePlayer
	m_pTarget = static_cast<CCharacter*>(pGame->Find_GameObject(LEVEL_ANYWHERE, TEXT("Player")));
	m_pTargetTransform = static_cast<CTransform*>(m_pTarget->Find_Component(TEXT("Com_Transform")));

	//UI추가
	/*m_pUIIcon = static_cast<CUI_Minimap*>(pGame->Find_GameObject(LEVEL_ANYWHERE, TEXT("UI_Minimap")));
	m_UIIndex = m_pUIIcon->Add_Icon(m_pMainTransform->Get_State(CTransform::STATE_POSITION), 44);*/
}

void CM_Crownless_P2::PreTick(_double TimeDelta)
{
	// 플레이어 교체시 현재 선택된 캐릭터로 타겟을 변경하는 함수
	//if(false == m_pTarget->IsActive())
	//	 Find ActivePlayer

	

	// 매 프레임 타겟까지의 거리를 계산하는 함수, 위에서 컬링해야 함
	Find_Target();

	// 공격이 히트했을 경우에 대한 조건처리
	Check_AttackHit();
}

void CM_Crownless_P2::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	static _double TimeDelay2 = 1.0;

	if (pGameInstance->InputKey(DIK_NUMPAD8) == KEY_STATE::TAP)
	{
		if (TimeDelay2 == 1.0)
			TimeDelay2 = 0.0;
		else 
			TimeDelay2 = 1.0;
	}

	// 테스트
	/*if (pGameInstance->InputKey(DIK_NUMPAD8) == KEY_STATE::TAP)
	{
		SetState(DISABLE);
	}*/
	//

	__super::Tick(TimeDelta);

	Apply_CoolTime(TimeDelta * TimeDelay2); // 쿨타임 갱신

	if (0.f < m_fHitPoint ||
		150.f > m_fHitPoint)
		m_fHitPoint += (_float)TimeDelta * 2.f;




	Select_State(TimeDelta * TimeDelay2); // 상태 확인


	Tick_State(TimeDelta * TimeDelay2); // PlayAnimation, 애니메이션에 따른 이동, 애니메이션 종료 시 처리

	On_Cell(); // 자발적인 움직임 후처리 >> 주로 내비 메쉬

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pAttackCollider, COLL_MONSTERATTACK);
	m_pAttackCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pHitCollider, COLL_PLAYERATTACK);
	m_pHitCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	pGameInstance->AddCollider(m_pMoveCollider, COLL_MOVE);
	m_pMoveCollider->Update(XMLoadFloat4x4(&m_pMainTransform->Get_WorldMatrix()));

	//UI추가
	if (false == this->IsDisable())
	{
		_float4 Head;
		XMStoreFloat4(&Head, XMLoadFloat4x4(&m_EffectBoneMatrices[EBONE_HEAD]).r[3]);
		Head.y += 0.5f;
		m_pUIMon->Set_CharacterPos(XMLoadFloat4(&Head));
		/*m_pUIIcon->Set_ObjectPos(m_UIIndex, m_pMainTransform->Get_State(CTransform::STATE_POSITION));
		m_pUIIcon->SetRender(m_UIIndex, true);*/
	}
}

void CM_Crownless_P2::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC, this);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DYNAMIC_SHADOW, this);



	// 돌진기 스킬 예외처리 // 거리비교해서
	if (m_fTargetDistance <= 1.8f)
	{
		static _bool bOverlapedCheck = false;

		if ((IS_ATTACK19 == m_Scon.iCurState)
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


	//Effect Bones 처리
	Update_EffectBones();
}

HRESULT CM_Crownless_P2::Render()
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
		if (2 == i || 4 == i || 5 == i || 6 == i)
			continue;

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_DiffuseTexture", i, MyTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_NormalTexture", i, MyTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;

		if (m_bDissolve)
		{
			CGameInstance* pGame = CGameInstance::GetInstance();
			// Dissolve 변수
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

	// 여기부터 잔상 그리기
	for (_uint i = 0; i < m_iTraceCount; ++i)
	{
		if (true == m_TraceArray[i].bRendering)
		{
			if (FAILED(m_pShaderCom->SetMatrix("g_WorldMatrix", &m_TraceArray[i].TraceWorldMatrix)))
				return E_FAIL;

			_float fFadeRatio = 1.f;
			if (m_TraceArray[i].TraceTimeAcc != 0.0)
				int a = 1; 
			if (m_TraceArray[i].TraceTimeAcc < m_TraceArray[i].FadeInRate * m_TraceArray[i].TraceDuration)
				fFadeRatio = _float(m_TraceArray[i].TraceTimeAcc / (m_TraceArray[i].FadeInRate * m_TraceArray[i].TraceDuration));
			else if (m_TraceArray[i].TraceTimeAcc >	m_TraceArray[i].FadeOutRate * m_TraceArray[i].TraceDuration)
				fFadeRatio = _float((m_TraceArray[i].TraceDuration - m_TraceArray[i].TraceTimeAcc) / (m_TraceArray[i].TraceDuration - m_TraceArray[i].FadeOutRate * m_TraceArray[i].TraceDuration));

			if (FAILED(m_pShaderCom->SetRawValue("g_fFadeRatio", &fFadeRatio, sizeof(_float))))
				return E_FAIL;

 			for (_uint j = 0; j < iNumMeshes; ++j)
			{
				if (2 == j || 4 == j || 5 == j || 6 == j)
					continue;

				m_pShaderCom->Set_Matrices("g_BoneMatrix", m_TraceArray[i].ppTraceBoneMatrices[j], 256);


				m_pShaderCom->Begin(14);
				m_pModelCom->Render(j);
			}
		}
	}

	return S_OK;
}

HRESULT CM_Crownless_P2::RenderShadow()
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
		if (2 == i || 4 == i || 5 == i || 6 == i)
			continue;

		if (FAILED(m_pModelCom->SetUp_BoneMatrices(m_pShaderCom, "g_BoneMatrix", i)))
			return E_FAIL;

		m_pShaderCom->Begin(2);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CM_Crownless_P2::RenderGUI()
{
}

HRESULT CM_Crownless_P2::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, DMODEL::DMD_MONSTER_CROWNLESS_P2,
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::OBB,
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 1.2f, 0.9f };
	CollDesc.vExtents = { 1.2f, 1.2f, 1.2f };
	CollDesc.vRotation = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::SPHERE,
		TEXT("Com_AttackCollider"), (CComponent**)&m_pAttackCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.owner = this;
	CollDesc.vCenter = { 0.f, 1.2f, 0.f };
	CollDesc.vExtents = { 1.f, 1.f, 1.f };
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

HRESULT CM_Crownless_P2::Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ZeroMemory(m_tStates, sizeof(SINGLESTATE) * IS_END);

	// 로드하는 코드
	for (_int i = 0; i < IS_END; ++i)
	{
		_tchar szBuffer[MAX_PATH];
		wsprintf(szBuffer, TEXT("../../Data/CharState/M_Crownless_P2/M_Crownless_P2_%d.state"), i);
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
				case CStateKey::TYPE_SLOW:
					m_tStates[i].ppStateKeys[j] = CSlowKey::Create(pDevice, pContext, &tBaseData);
					break;
				case CStateKey::TYPE_TRACE:
					m_tStates[i].ppStateKeys[j] = CTraceKey::Create(pDevice, pContext, &tBaseData);
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

void CM_Crownless_P2::Release_States()
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

void CM_Crownless_P2::Shot_EffectKey(_tchar * szEffectTag/* szTag1*/, _uint EffectBoneID /* iInt0 */, _uint iEffectTypeID, _bool bTracking/*iInt1*/)
{
	CEffect* pEffect = CGameInstance::GetInstance()->Get_Effect(szEffectTag, Engine::EFFECT_ID(iEffectTypeID));
	if (nullptr == pEffect || EBONE_END <= EffectBoneID)
		return;

	pEffect->Play_Effect(&m_EffectBoneMatrices[EffectBoneID], bTracking);
}

void CM_Crownless_P2::Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID)
{
	if (MISS_END <= iMissilePoolID || EBONE_END <= iEffectBoneID)
		return;

	_vector vInitPos;
	if (0 != iEffectBoneID)
	{
		vInitPos = XMVector3TransformCoord(
			XMVector3TransformCoord(m_EffectBones[iEffectBoneID]->Get_CombinedPosition(), XMMatrixRotationY(180.f)),
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



void CM_Crownless_P2::Shot_PriorityKey(_uint iLeavePriority)
{
	m_tCurState.iLeavePriority = iLeavePriority;
}

void CM_Crownless_P2::Shot_OBBKey(_bool bOBB, _uint iAttackInfoID)
{
	m_pAttackCollider->SetActive(bOBB);
	m_iCurAttackID = iAttackInfoID;
}

void CM_Crownless_P2::Shot_Trace(_double Duration, _double FInRate, _double FOutRate)
{
	_bool bOK = false;
	for (_uint i = 0; i < m_iTraceCount; ++i)
	{
		if (false == m_TraceArray[i].bRendering)
		{
			m_TraceArray[i].TraceWorldMatrix = m_pMainTransform->Get_WorldMatrix();

			for (_uint j = 0; j < m_pModelCom->Get_NumMeshes(); ++j)
			{
				m_pModelCom->Get_BoneMatrices(m_TraceArray[i].ppTraceBoneMatrices[j], j);
			}

			m_TraceArray[i].TraceTimeAcc = 0.0;
			m_TraceArray[i].TraceDuration = Duration;
			m_TraceArray[i].FadeInRate = FInRate;
			m_TraceArray[i].FadeOutRate = FOutRate;
			
			bOK = m_TraceArray[i].bRendering = true;
		}
		if (bOK)
			break;
	}

	// 모든 잔상이 사용 중이면 여기 떨어짐
	if (false == bOK)
	{
		int a = 1;
	}
}

void CM_Crownless_P2::Shot_Trace_Pose(_double Duration, _double FadeInRate, _double FadeOutRate)
{
	_bool bOK = false;
	for (_uint i = 0; i < m_iTraceCount; ++i)
	{
		if (false == m_TraceArray[i].bRendering)
		{
			m_TraceArray[i].TraceWorldMatrix = m_pMainTransform->Get_WorldMatrix();

			for (_uint j = 0; j < m_pModelCom->Get_NumMeshes(); ++j)
			{
				m_pModelCom->Get_BoneMatrices(m_TraceArray[i].ppTraceBoneMatrices[j], j);
			}

			m_TraceArray[i].TraceTimeAcc = 0.0;
			m_TraceArray[i].TraceDuration = Duration;
			m_TraceArray[i].FadeInRate = FadeInRate;
			m_TraceArray[i].FadeOutRate = FadeOutRate;

			bOK = m_TraceArray[i].bRendering = true;
		}
		if (bOK)
			break;
	}

	// 모든 잔상이 사용 중이면 여기 떨어짐
	if (false == bOK)
	{
		int a = 1;
	}
}


void CM_Crownless_P2::SetUp_State()
{
	_uint iPrevState = m_Scon.iCurState;

	// 키 리셋
	for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
	{
		if (nullptr != m_tCurState.ppStateKeys[i])
			m_tCurState.ppStateKeys[i]->Reset();
	}

	//// 애니메이션 캔슬 시 잔상 남김
	//if(false == m_Scon.bAnimFinished && IS_IDLE != m_Scon.iCurState)
	//{
	//	Shot_Trace(2.0, 0.5, 0.5);
	//}

	// 오입력 예외처리
	if (m_Scon.iNextState >= IS_END)
		m_Scon.iNextState = IS_END - 1;
	else if (m_Scon.iNextState <= 0)
		m_Scon.iNextState = 0;

	// 다음 애니메이션으로 갱신
	m_Scon.iCurState = m_Scon.iNextState;
	m_tCurState = m_tStates[m_Scon.iCurState];

	m_Scon.TrackPos = 0.0;
	m_Scon.bAnimFinished = false;

	m_bAttackHit = false;

	// 근접 시동기 사용 시 다음 근접 시동기로 인덱스 넘김z
	for (_uint i = 0; i < MA_END; ++i)
	{
		if (m_Scon.iCurState == m_iMeleeStartAttackArray[i])
		{
			++m_iMeleeAttackID;
			if (MA_END <= m_iMeleeAttackID)
				m_iMeleeAttackID = MA_ATTACK03;
		}
			
	}

	// 애니메이션이 강제로 끊긴 경우 대비 애니메이션 갱신 시 OBB 콜라이더 무조건 끄기
	m_pAttackCollider->SetActive(false);

	//Position State 반영
	if ((IS_BEHIT_FLY_START == m_Scon.iCurState ||
		IS_ATTACK18 == m_Scon.iCurState) &&
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

	if (true == m_tCurState.bApplyCoolTime)
		m_StateCoolTimes[m_Scon.iCurState] = m_tCurState.CoolTime;
	else if (IS_SIDESTEP_L == m_Scon.iCurState ||
		0 == m_Scon.iCurState)
		m_StateCoolTimes[0] = m_tStates[0].CoolTime;

	//if (IS_ATTACK11 == m_Scon.iCurState)
	//	m_pMoveCollider->SetActive(false);
	//else
	//	m_pMoveCollider->SetActive(true);


	//if (IS_ATTACK03 == iPrevState)
	//{
	//	//m_pMainTransform->Set_LookDir(-m_pMainTransform->Get_State(CTransform::STATE_LOOK));
	//	//m_tCurState.bLerp = false;
	//}
}

void CM_Crownless_P2::Find_Target()
{
	// 매프레임 타겟까지의 거리를 계산해놓는다.
	m_fTargetDistance = XMVectorGetX(XMVector3Length(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION)));
	// 타겟으로 잡은 친구는 플레이어 캐릭터여야 함
	m_pTarget->Check_Nearst(this, m_fTargetDistance);


	if (false == m_bAlert)
	{
		if (m_fAlertRange > m_fTargetDistance)
		{
			m_bAlert = true;
			m_Scon.iNextState = 1;
			SetUp_State();
			m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true, false);
			m_pMainTransform->Set_LookDir(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f));

			m_fHitPoint = 150;
			m_iDodgeCount = 2;
			m_DodgeTimeAcc = 15.0;
		}
	}
}

void CM_Crownless_P2::Check_AttackHit()
{
	if (true == m_bAttackHit)
	{
		if (IS_ATTACK03 == m_Scon.iCurState)
		{
			if (32.0 <= m_Scon.TrackPos)
			{
				m_Scon.iNextState = IS_ATTACK06;
				SetUp_State();
				m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
			}
		}

		else if (IS_ATTACK08 == m_Scon.iCurState)
		{
			if (35.0 <= m_Scon.TrackPos)
			{
				m_Scon.iNextState = IS_ATTACK07;
				SetUp_State();
				m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
			}
		}

		else if (IS_ATTACK07 == m_Scon.iCurState)
		{
			if (42.0 <= m_Scon.TrackPos)
			{
				m_Scon.iNextState = IS_ATTACK06_NOSLOW;
				SetUp_State();
				m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
			}
		}
		else if (IS_ATTACK10 == m_Scon.iCurState)
		{
			if (16.0 <= m_Scon.TrackPos)
			{
				m_Scon.iNextState = IS_ATTACK11;
				SetUp_State();
				m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
			}
		}
		
	}
	else
	{
		if (IS_ATTACK18 == m_Scon.iCurState)
		{
			if (39.0 <= m_Scon.TrackPos)
			{
				m_Scon.iNextState = IS_ATTACK21;
				SetUp_State();
				m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
			}
		}
	}
}

void CM_Crownless_P2::Init_AttackInfos()
{
	for (_uint i = 0; i < ATK_END; ++i)
		ZeroMemory(&m_AttackInfos[i], sizeof TAGATTACK);

	m_AttackInfos[ATK_ATTACK03].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK03].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK03].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK03].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK03].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK05_1].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK05_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK05_1].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK05_1].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK05_1].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK05_2].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK05_2].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK05_2].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK05_2].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK05_2].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK05_3].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK05_3].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK05_3].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK05_3].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK05_3].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK06].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK06].eHitIntensity = HIT_PUSH;
	m_AttackInfos[ATK_ATTACK06].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK06].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK06].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK07].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK07].eHitIntensity = HIT_PUSH;
	m_AttackInfos[ATK_ATTACK07].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK07].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK07].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK08_1].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK08_1].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK08_1].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK08_1].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK08_1].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK08_2].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK08_2].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK08_2].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK08_2].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK08_2].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK10].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK10].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK10].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK10].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK10].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK11].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK11].eHitIntensity = HIT_PUSH;
	m_AttackInfos[ATK_ATTACK11].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK11].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK11].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK17].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK17].eHitIntensity = HIT_PUSH;
	m_AttackInfos[ATK_ATTACK17].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK17].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK17].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK18].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK18].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK18].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK18].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK18].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK19].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK19].eHitIntensity = HIT_PUSH;
	m_AttackInfos[ATK_ATTACK19].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK19].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK19].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK20_1].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK20_1].eHitIntensity = HIT_NONE;
	m_AttackInfos[ATK_ATTACK20_1].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK20_1].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK20_1].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK20_2].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK20_2].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK20_2].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK20_2].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK20_2].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK20_3].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK20_3].eHitIntensity = HIT_SMALL;
	m_AttackInfos[ATK_ATTACK20_3].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK20_3].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK20_3].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK20_4].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK20_4].eHitIntensity = HIT_BIG;
	m_AttackInfos[ATK_ATTACK20_4].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK20_4].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK20_4].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK20_5].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK20_5].eHitIntensity = HIT_PUSH;
	m_AttackInfos[ATK_ATTACK20_5].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK20_5].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK20_5].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK21].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK21].eHitIntensity = HIT_FLY;
	m_AttackInfos[ATK_ATTACK21].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK21].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK21].szHitEffectTag, TEXT("Anjin_Hit"));

	m_AttackInfos[ATK_ATTACK22].fDamageFactor = 2.f;
	m_AttackInfos[ATK_ATTACK22].eHitIntensity = HIT_PUSH;
	m_AttackInfos[ATK_ATTACK22].eElementType = ELMT_HAVOC;
	m_AttackInfos[ATK_ATTACK22].iHitEffectID = 2;
	lstrcpy(m_AttackInfos[ATK_ATTACK22].szHitEffectTag, TEXT("Anjin_Hit"));

}

void CM_Crownless_P2::Init_Missiles()
{
	CMissilePool::MISSILEPOOLDESC tMissilePoolDesc;

	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Crownless_P1_Attack05_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_CONSTANT;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("GenkiDama_Shoot"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 3.0;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK17;
	tMissilePoolDesc.tMissileDesc.fExtents = 0.8f;

	tMissilePoolDesc.bTargetDir = true;
	tMissilePoolDesc.vFixMoveDir = _float3(0.f, 0.f, 1.f);
	tMissilePoolDesc.fVelocity = 50.f;
	tMissilePoolDesc.StopTime = 3.0;
	tMissilePoolDesc.iStopCondition = CMissile_Constant::STOP_NONE;

	m_MissilePools[MISS_ATTACK_17] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 1.f, 1.5f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK_17] = _float3(0.f, 0.f, 0.f);


	ZeroMemory(&tMissilePoolDesc, sizeof(tMissilePoolDesc));

	tMissilePoolDesc.pMissilePoolTag = TEXT("Crownless_P1_Attack03_%d");
	tMissilePoolDesc.iMissileType = CMissilePool::MISS_NOMOVE;
	tMissilePoolDesc.iNumMissiles = 2;

	lstrcpy(tMissilePoolDesc.tMissileDesc.szLoopEffectTag, TEXT("M_Boom"));
	tMissilePoolDesc.tMissileDesc.iLoopEffectLayer = 2;
	tMissilePoolDesc.tMissileDesc.pOwner = this;
	tMissilePoolDesc.tMissileDesc.HitInterval = 0.0;
	tMissilePoolDesc.tMissileDesc.LifeTime = 0.15;
	tMissilePoolDesc.tMissileDesc.iAttackInfoID = ATK_ATTACK21;
	tMissilePoolDesc.tMissileDesc.fExtents = 2.f;

	m_MissilePools[MISS_ATTACK_21] = CMissilePool::Create(m_pDevice, m_pContext, XMVectorSet(0.f, 0.f, 0.f, 0.f), &tMissilePoolDesc);
	m_MissileRotAngles[MISS_ATTACK_21] = _float3(0.f, 0.f, 0.f);


}

void CM_Crownless_P2::Init_AnimSystem()
{
	for (auto& pAnim : m_pModelCom->Get_Animations())
	{
		const _tchar* szAnimName = pAnim->Get_Name();

		for (auto& pChannel : pAnim->Get_Channels())
		{
			const _tchar* szChannelName = pChannel->Get_Name();

			if (!lstrcmp(szChannelName, TEXT("Bone_RA_Chibang01")) ||
				!lstrcmp(szChannelName, TEXT("Bone_LA_Chibang01")))
			{
				for (auto& KeyFrames : pChannel->Get_KeyFrames())
				{
					KeyFrames.vPosition.x = 100000.f;
				}
			}
		}


		//if (!lstrcmp(szAnimName, TEXT("MB1Wuguanzhe00601.ao|Attack_03")))
		//{
		//	CChannel* pBip001Ch = pAnim->Get_Channel(TEXT("Bip001"));
		//	if (nullptr == pBip001Ch)
		//		int a = 1;

		//	auto& Bip001KeyFrames = pBip001Ch->Get_KeyFrames();
		//	_uint iBipNumKeyFrames = pBip001Ch->Get_NumKeyFrames();

		//	for (_uint i = 0; i < iBipNumKeyFrames; ++i)
		//	{
		//		//XMStoreFloat4(&Bip001KeyFrames[i].vRotation, XMQuaternionIdentity());

		//		Bip001KeyFrames[i].vRotation.y = 0.f;
		//		Bip001KeyFrames[i].vRotation.z = 0.f;

		//		XMStoreFloat4(&Bip001KeyFrames[i].vRotation, XMQuaternionNormalize(XMLoadFloat4(&Bip001KeyFrames[i].vRotation)));
		//	}

		//	//CChannel* pRootCh = pAnim->Get_Channel(TEXT("Root"));
		//	//if (nullptr == pRootCh)
		//	//	int a = 1;

		//	//_uint iRootNumKeyFrames = pRootCh->Get_NumKeyFrames();
		//	//_uint iBipNumKeyFrames = pBip001Ch->Get_NumKeyFrames();

		//	//if (iRootNumKeyFrames != iBipNumKeyFrames)
		//	//	int a = 1;
		//	//
		//	//auto& RootKeyFrames = pRootCh->Get_KeyFrames();

		//	//for (_uint i = 0; i < iRootNumKeyFrames; ++i)
		//	//{
		//	//	_float3 LerpRotation = {0.f, 0.f, 0.f} ;

		//	//	_double Time = RootKeyFrames[i].Time;
		//	//	
		//	//	_uint Bip001KeyID = 0;
		//	//	while (Time > Bip001KeyFrames[Bip001KeyID].Time)
		//	//		++Bip001KeyID;

		//	//	if (Bip001KeyID < iBipNumKeyFrames - 1)
		//	//	{
		//	//		_float fRatio = _float((Time - Bip001KeyFrames[Bip001KeyID].Time) /
		//	//			(Bip001KeyFrames[Bip001KeyID + 1].Time - Bip001KeyFrames[Bip001KeyID].Time));


		//	//		RootKeyFrames[i].vRotation.z = XMVectorGetZ(XMQuaternionSlerp(XMLoadFloat4(&Bip001KeyFrames[Bip001KeyID].vRotation),
		//	//			XMLoadFloat4(&Bip001KeyFrames[Bip001KeyID + 1].vRotation), fRatio));

		//	//		//XMStoreFloat4(&RootKeyFrames[i].vRotation,XMQuaternionSlerp(XMLoadFloat4(&Bip001KeyFrames[Bip001KeyID].vRotation),
		//	//		//		XMLoadFloat4(&Bip001KeyFrames[Bip001KeyID + 1].vRotation), fRatio));
		//	//	}
		//	//	else
		//	//		RootKeyFrames[i].vRotation.z = Bip001KeyFrames[Bip001KeyID].vRotation.z;

		//	//	XMStoreFloat4(&RootKeyFrames[i].vRotation, XMQuaternionNormalize(XMLoadFloat4(&RootKeyFrames[i].vRotation)));
		//	//}

		//	
		//}
	}
}

void CM_Crownless_P2::Init_Traces()
{
	ZeroMemory(m_TraceArray, sizeof(TRACE) * m_iTraceCount);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	for (_uint i = 0; i < m_iTraceCount; ++i)
	{
		m_TraceArray[i].ppTraceBoneMatrices = new _float4x4*[iNumMeshes];
		ZeroMemory(m_TraceArray[i].ppTraceBoneMatrices, sizeof(_float4x4*) * iNumMeshes);
		for (_uint j = 0; j < iNumMeshes; ++j)
		{
			m_TraceArray[i].ppTraceBoneMatrices[j] = new _float4x4[256];
			ZeroMemory(m_TraceArray[i].ppTraceBoneMatrices[j], sizeof(_float4x4) * 256);
		}
	}

}

void CM_Crownless_P2::Release_Traces()
{
	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < m_iTraceCount; ++i)
	{
		for (_uint j = 0; j < iNumMeshes; ++j)
		{
			Safe_Delete_Array(m_TraceArray[i].ppTraceBoneMatrices[j]);
		}
		Safe_Delete_Array(m_TraceArray[i].ppTraceBoneMatrices);
	}
}


void CM_Crownless_P2::Apply_CoolTime(_double TimeDelta)
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

	if (0.0 < m_DodgeTimeAcc && m_iDodgeCount < 2)
	{
		m_DodgeTimeAcc -= TimeDelta;
		if (0.0 > m_DodgeTimeAcc)
		{
			m_DodgeTimeAcc = 15.0;
			++m_iDodgeCount;

			if (m_iDodgeCount > 2)
				m_iDodgeCount = 2;
		}
	}

	for (_uint i = 0; i < m_iTraceCount; ++i)
	{
		if (true == m_TraceArray[i].bRendering)
		{
			m_TraceArray[i].TraceTimeAcc += TimeDelta;
			if (m_TraceArray[i].TraceTimeAcc > m_TraceArray[i].TraceDuration)
			{
				m_TraceArray[i].bRendering = false;
			}
		}
	}

	if (true == m_bTraceOn)
	{
		m_TraceTimeAcc -= TimeDelta;
		if (0.0 > m_TraceTimeAcc)
		{
			m_TraceTimeAcc = m_TraceInterval;
			Shot_Trace(m_TraceDuration, 0.05, 0.15);
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

HRESULT CM_Crownless_P2::SetUp_ShaderResources()
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

HRESULT CM_Crownless_P2::Setup_ShadowShaderResource()
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

void CM_Crownless_P2::Select_State(_double TimeDelta)
{
	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE_POSITION) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f));
	_vector vLevelCenterDir = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&m_vLevelCenterPosition) - m_pMainTransform->Get_State(CTransform::STATE_POSITION), 0.f));

	AI_STATE iCurFrameAI = AI_NONE;

	if (false == m_bAlert)
	{
		iCurFrameAI = AI_IDLE;
	}
	else
	{
		if (DIST_MIDRANGE < m_fTargetDistance)
		{
			iCurFrameAI = AI_CHASE;
		}
		else
		{
			if (m_pTarget->Get_Attack() &&
				0.0 == m_StateCoolTimes[0])
			{
				iCurFrameAI = AI_DODGE;
			}
			else if (true == m_bAttackReady)
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

	if (PS_GROUND == m_Scon.ePositionState)
	{
		switch (iCurFrameAI)
		{
		case Client::CM_Crownless_P2::AI_CHASE:
			if (m_iTauntStack >= 2)
			{
				m_Scon.iNextState = IS_TAUNT;
			}
			else
			{
				m_Scon.iNextState = IS_WALK_F;
			}
			break;

		case Client::CM_Crownless_P2::AI_DODGE:
			m_Scon.iNextState = IS_SIDESTEP_L;
			break;

		case Client::CM_Crownless_P2::AI_ATTACK_RANGE:
			if (0.0 == m_StateCoolTimes[IS_ATTACK17])
				m_Scon.iNextState = IS_ATTACK17;
			else
				m_Scon.iNextState = IS_ATTACK19;
			break;

		case Client::CM_Crownless_P2::AI_ATTACK_MELEE:
			m_Scon.iNextState = m_iMeleeStartAttackArray[m_iMeleeAttackID];
			break;

		case Client::CM_Crownless_P2::AI_STAY:
			if (DIST_MELEE > m_fTargetDistance)
			{
				_int iRand = rand() % 2;
				if (0 == iRand)
					m_Scon.iNextState = IS_WALK_L;
			}
			else
			{
				m_Scon.iNextState = IS_WALK_F;
			}
			break;
		default:
			break;
		}
	}


	// 지금 상태를 끊고 다음 상태로 갱신 할지 여부
	if (m_Scon.iCurState != m_Scon.iNextState)
	{
		if (m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority)
		{
			if(AI_CHASE == iCurFrameAI)
				++m_iTauntStack;
			else
				m_iTauntStack = 0;

			SetUp_State();
			m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, false);
			// 상태 갱신 시 1번만 회전
			if (CCharacter::ROT_ONSTART == m_tCurState.iRotationType)
			{
				m_pMainTransform->Set_LookDir(vTargetDir);
			}
		}
	}


	// 매 프레임 회전
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

void CM_Crownless_P2::Tick_State(_double TimeDelta)
{
	// 애니메이션 개별 처리
	/*if (m_Scon.iCurState == IS_ATTACK08)
	{
		if (m_Scon.TrackPos >= 65.0)
		{
			m_pHitCollider->SetActive(true);
			m_pModelCom->Get_Animation(m_tCurState.iAnimID)->Set_TicksPerSecond(36.0);
		}
		else if (m_Scon.TrackPos >= 50)
			m_pModelCom->Get_Animation(m_tCurState.iAnimID)->Set_TicksPerSecond(60.0);
		else if (m_Scon.TrackPos >= 16)
			m_pHitCollider->SetActive(false);

	}*/

	//
	if (false == m_Scon.bAnimFinished)
	{
		_float4 vRotation = _float4(0.f, 0.f, 0.f, 1.f);
		_float3 vMovement;

		// 애니메이션 갱신
		m_pModelCom->Play_Animation(TimeDelta, &vRotation, &vMovement, &m_Scon.TrackPos, &m_Scon.bAnimFinished);
		m_pModelCom->Invalidate_CombinedMatrices();

		// 루프하지 않는 애니메이션이 이번 프레임에 끝났으면 전프레임 움직임을 적용
		// 애니메이션 프레임이랑 FrameRate가 달라서 발생하는 오차값으로 인한 버그 방지
		if (true == m_Scon.bAnimFinished && false == m_tCurState.bLoop)
			XMStoreFloat3(&vMovement, XMLoadFloat3(&m_Scon.vPrevMovement) * (_float)TimeDelta);
		else
		{
			// 루트 모션 처리(회전, 이동)
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

		// 구해진 이동값만큼 움직이고 이전 프레임 정보를 저장, + TimeDelta 대응
		m_pMainTransform->Move_Anim(&vMovement, m_Scon.ePositionState, m_pNaviCom);

		XMStoreFloat3(&m_Scon.vPrevMovement, XMLoadFloat3(&vMovement) / (_float)TimeDelta);

		// StateKey 처리
		for (_uint i = 0; i < m_tCurState.iKeyCount; ++i)
		{
			if (nullptr != m_tCurState.ppStateKeys[i])
				m_tCurState.ppStateKeys[i]->Check(m_Scon.TrackPos, this);
		}

		// 떨어지고 있을 경우 
		if (vMovement.z < 0.f)
		{
			m_Scon.bFalling = true;
		}
	}

	if (true == m_Scon.bAnimFinished)
	{
		if (IS_DEAD == m_Scon.iCurState)
		{
			//SetState(DISABLE);
			/*m_pUIMon->SetState(DISABLE);
			m_pUIMon = nullptr;
			m_pUIIcon->SetRender(m_UIIndex, false);
			m_pUIIcon = nullptr;*/
		}


		// 공격 행동 시
		if (7 <= m_Scon.iCurState &&
			21 > m_Scon.iCurState)
		{
			m_bAttackReady = false;
			m_GlobalCoolTime += _double(rand() % 15) * 0.1 + 0.5;
		}

		// 피격 애니메이션 종료 시
		if (IS_BEHIT_S == m_Scon.iCurState ||
			IS_BEHIT_B == m_Scon.iCurState ||
			IS_BEHIT_FLY_FALL == m_Scon.iCurState)
		{
			m_fHitPoint = 150.f;
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
				if (IS_ATTACK18 == m_Scon.iCurState)
					m_Scon.ePositionState = PS_GROUND;

				m_Scon.iNextState = m_tCurState.iNextState;
				SetUp_State();

				m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, m_tCurState.bLerp, true);
			}
		}
	}
}

void CM_Crownless_P2::On_Cell()
{
	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);
	_float fPosY = XMVectorGetY(vPos);
	_float fCellHeight = m_pNaviCom->Compute_Height(vPos);

	if (PS_GROUND == m_Scon.ePositionState)
	{
		if (0 != m_Scon.iCurState)
			m_pMainTransform->Set_PosY(fCellHeight);
	}
	else if (PS_AIR == m_Scon.ePositionState)
	{
		if (fPosY - fCellHeight < 0.f)
		{
			if (false == m_Scon.bFalling)
			{
				// 오르막 방향으로 점프 시 예외처리
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
				else if (IS_ATTACK21 == m_Scon.iCurState ||
					IS_ATTACK18 == m_Scon.iCurState)
				{
					m_pMainTransform->Set_PosY(fCellHeight);
					m_Scon.ePositionState = PS_GROUND;
				}
			}
		}
	}
}

void CM_Crownless_P2::On_Hit(CCharacter* pChar, TAGATTACK * pAttackInfo, _float fAttackPoint, _float3 * pEffPos)
{
	// 피격 이펙트 출력
	if (lstrcmp(pAttackInfo->szHitEffectTag, TEXT("")))
	{
		CGameInstance* pGI = CGameInstance::GetInstance();
		_float4x4 EffectMatrix = m_pMainTransform->Get_WorldMatrix();
		memcpy(EffectMatrix.m[3], pEffPos, sizeof(_float3));
		pGI->Get_Effect(pAttackInfo->szHitEffectTag, (EFFECT_ID)pAttackInfo->iHitEffectID)->Play_Effect(&EffectMatrix);
	}

	pChar->Recover_Gauge(pAttackInfo->fSPGain, pAttackInfo->fBPGain, pAttackInfo->fTPGain);

	CGameMode* pGM = CGameMode::GetInstance();

	// 대미지 계산 공식 : 모션 계수 * 공격력 * ((공격력 * 2 - 방어력) / 공격력) * (속성 보너스)
	// 공격력과 방어력이 같을 때 1배 대미지
	_float fFinalDamage = pAttackInfo->fDamageFactor * fAttackPoint * ((fAttackPoint * 2 - m_tMonsterInfo.fDefense) / fAttackPoint) /** 속성 보너스 */;

	m_tMonsterInfo.fCurHP -= fFinalDamage;

	// TODO: 여기서 대미지 폰트 출력
	if (false == m_pUIMon->IsDisable())
	{
		m_pUIMon->Set_Damage(fFinalDamage);
	}

	_bool bHitCheck = false;
	// 사망 시 사망 애니메이션 실행 
	if (0.f >= m_tMonsterInfo.fCurHP)
	{
		m_tMonsterInfo.fCurHP = 0.f;
		m_Scon.iNextState = IS_DEAD;
	}
	// 피격 애니메이션 실행
	else if (PS_GROUND == m_Scon.ePositionState)
	{
		if (IS_BEHIT_FLY_FALL != m_Scon.iCurState)
		{
			switch (pAttackInfo->eHitIntensity)
			{
			case HIT_NONE:
				m_fHitPoint -= 3;
				break;
			case HIT_SMALL:
				m_fHitPoint -= 10;
				if (m_fHitPoint < 0)
				{
					m_pCamMovement->StartVibration();
					m_Scon.iNextState = IS_BEHIT_S;
					bHitCheck = true;
				}
				break;
			case HIT_BIG:
				m_fHitPoint -= 25;
				if (m_fHitPoint < 0)
				{
					m_pCamMovement->StartVibration(10.f, 0.7f);
					m_Scon.iNextState = IS_BEHIT_B;
					bHitCheck = true;
				}
				break;
			case HIT_FLY:
			{
				m_fHitPoint -= 60;

				if (m_fHitPoint < 0)
				{
					//위로 치는 모션이면 수치 조절해서 값 넣어주기 일단 디폴트 웨이브 넣음
					m_pCamMovement->StartWave();
					m_Scon.iNextState = IS_BEHIT_FLY_START;
					bHitCheck = true;
				}
				break;
			}
			default:
				break;
			}
		}
	}
	else if (PS_AIR == m_Scon.ePositionState)
	{
		if (pAttackInfo->eHitIntensity != HIT_NONE)
		{
			if (IS_BEHIT_FLY_LOOP == m_Scon.iCurState ||
				IS_BEHIT_FLY_START == m_Scon.iCurState ||
				IS_BEHIT_HOVER == m_Scon.iCurState)
			{
				m_Scon.iNextState = IS_BEHIT_HOVER;
				bHitCheck = true;
			}
		}
	}


	if (true == bHitCheck || m_tCurState.iLeavePriority < m_tStates[m_Scon.iNextState].iEnterPriority)
	{
		m_pMainTransform->Set_LookDir(XMVectorSetY(pChar->Get_Position() - this->Get_Position(), 0.f));
		SetUp_State();
		m_pModelCom->SetUp_Animation(m_tStates[m_Scon.iCurState].iAnimID, false, false);
	}

}


HRESULT CM_Crownless_P2::Init_EffectBones()
{
	//NONE은 걍 월드 매트릭스를 저장해놨다가 던짐
	m_EffectBones[EBONE_SPINE] = m_pModelCom->Get_BonePtr(TEXT("Bip001Spine"));
	m_EffectBones[EBONE_LHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001LHand"));
	m_EffectBones[EBONE_RHAND] = m_pModelCom->Get_BonePtr(TEXT("Bip001RHand"));
	m_EffectBones[EBONE_HEAD] = m_pModelCom->Get_BonePtr(TEXT("Bip001Head"));

	m_EffectBones[EBONE_LFOOT] = m_pModelCom->Get_BonePtr(TEXT("Bip001LFoot"));
	m_EffectBones[EBONE_RFOOT] = m_pModelCom->Get_BonePtr(TEXT("Bip001RFoot"));

	return S_OK;
}

void CM_Crownless_P2::Update_EffectBones()
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

CM_Crownless_P2 * CM_Crownless_P2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CM_Crownless_P2* pInstance = new CM_Crownless_P2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CM_Crownless_P2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CM_Crownless_P2::Clone(void * pArg)
{
	CM_Crownless_P2* pInstance = new CM_Crownless_P2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CM_Crownless_P2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CM_Crownless_P2::Free()
{

	__super::Free();

	for (_uint i = 0; i < MISS_END; ++i)
		Safe_Release(m_MissilePools[i]);

	if(m_bClone)
		Release_Traces();


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

void CM_Crownless_P2::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	CGameMode* pGM = CGameMode::GetInstance();
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());

	if (pOpponent)
	{
		// 상대가 플레이어일 경우
		if (CT_PLAYER == pOpponent->Get_CollType())
		{
			// 내 공격이 상대에게 적중한 경우
			if (true == src->Compare(GetAttackCollider()) &&
				true == dest->Compare(pOpponent->GetHitCollider()))
			{
				m_pCamMovement->StartVibration(10.f, 0.5f);

				if (ATK_ATTACK03 == m_iCurAttackID)
				{
					m_bAttackHit = true;
					pOpponent->Set_TimeDelay(1.0, 0.33);
				}
				else if (ATK_ATTACK07 == m_iCurAttackID)
					m_bAttackHit = true;
				else if (ATK_ATTACK08_2 == m_iCurAttackID)
					m_bAttackHit = true;
				else if (ATK_ATTACK10 == m_iCurAttackID)
					m_bAttackHit = true;
				else if (ATK_ATTACK18 == m_iCurAttackID)
					m_bAttackHit = true;

			}

			// 상대의 공격이 나에게 적중한 경우 
			if (true == src->Compare(GetHitCollider()) &&
				true == dest->Compare(pOpponent->GetAttackCollider()))
			{
				m_pCamMovement->StartVibration();

				// 플/몬 공통 : 대미지 처리, 대미지 폰트 출력, 피격 애니메이션 이행
				TAGATTACK tAttackInfo;
				ZeroMemory(&tAttackInfo, sizeof(tAttackInfo));
				_float fAttackPoint = 0.f;

				pOpponent->Get_AttackInfo(pOpponent->Get_AttackID(), &tAttackInfo, &fAttackPoint);

				_vector destCenter = XMLoadFloat3(dest->GetWorldCenter());
				_vector srcCenter = XMLoadFloat3(src->GetWorldCenter());
				_float3 EffPos = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&EffPos, (destCenter + srcCenter) * 0.5f);

				if(IS_ATTACK12 != m_Scon.iCurState)
					On_Hit(pOpponent, &tAttackInfo, fAttackPoint, &EffPos);
				else
				{
					if (5.0 < m_Scon.TrackPos)
					{
						m_Scon.iNextState = IS_ATTACK22;
						Shot_SlowKey(0.5f, 0.1f);
						SetUp_State();
						m_pModelCom->SetUp_Animation(m_tCurState.iAnimID, true);
					}
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
			// 상대의 투사체 공격이 나에게 적중한 경우 
			if (true == src->Compare(GetHitCollider()))
			{
				// 플/몬 공통 : 대미지 처리, 대미지 폰트 출력, 피격 애니메이션 이행
				TAGATTACK tAttackInfo;
				ZeroMemory(&tAttackInfo, sizeof(tAttackInfo));
				_float fAttackPoint = 0.f;

				pMissileOwner->Get_AttackInfo(pMissile->Get_AttackID(), &tAttackInfo, &fAttackPoint);

				_vector destCenter = XMLoadFloat3(dest->GetWorldCenter());
				_vector srcCenter = XMLoadFloat3(src->GetWorldCenter());
				_float3 EffPos = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&EffPos, (destCenter + srcCenter) * 0.5f);

				On_Hit(pMissileOwner, &tAttackInfo, fAttackPoint, &EffPos);
			}
		}
	}


}

void CM_Crownless_P2::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CCharacter* pOpponent = dynamic_cast<CCharacter*>(dest->GetOwner());

	if (pOpponent)
	{
#pragma region Move
		// pushWeight 값에 따라 서로를 밀어냄
		// 밀어내는 로직은 플레이어 쪽에서 처리?
		if (true == src->Compare(GetMoveCollider()) &&
			true == dest->Compare(pOpponent->GetMoveCollider()))
		{
			//// 전진형 공격을 정지시킴
			if (0 == m_Scon.iCurState ||
				IS_ATTACK03 == m_Scon.iCurState ||
				IS_ATTACK05 == m_Scon.iCurState ||
				IS_ATTACK08 == m_Scon.iCurState)
			{
				m_tCurState.bRootMotion = false;
				m_Scon.vMovement = _float3(0.f, 0.f, 0.f);
			}
		}
#pragma endregion 
	}
}

void CM_Crownless_P2::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
