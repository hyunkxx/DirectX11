#include "stdafx.h"
#include "..\Public\Character.h"

#include "GameMode.h"

_uint CCharacter::iPlayerSoundChannel = SOUND_PLAYER_SWING;
_int CCharacter::Monindex = 0;
CCharacter::PHYSICMOVE CCharacter::StatePhysics[SP_END]
{
	// (x, -z, y) 
	// 안 움직임
	{ true, true, _float3(0.f, -1.f, 0.f),	0.f, 0.f, 0.f, 0.f, 0.f },
	// 걷기 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	1.f, 0.f, 0.f, 0.f, 0.f },
	// 달리기 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	3.3f, 0.f, 0.f, 0.f, 0.f },
	// 질주 _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	7.5f, 0.f, 0.f, 0.f, 0.f },
	// 질주+@ _F, 등속
	{ true, true, _float3(0.f, -1.f, 0.f),	10.0f, 0.f, 0.f, 0.f, 0.f },
	// 낙하, 직전 운동 상태 적용, xz감쇄율, y가속도 적용
	{ false, false, _float3(0.f, 0.f, 0.f), 0.f, 0.5f, 20.f, 20.f, -20.f},
	// PLAYERGIRL_AIRATTACK, 등속
	{ true, true, _float3(0.f, 0.f, -1.f), 35.f, 0.f, 0.f, 0.f, 0.f  },
	// BEHIT_FLY_START, 
	{ true, false, _float3(0.f, 0.2f, 1.f), 10.f, 0.5f, 20.f, 20.f, -20.f},
	// BEHIT_HOVER, 
	{ true, false, _float3(0.f, 0.1f, 1.f), 4.f, 0.5f, 20.f, 20.f, -20.f},
	// BEHIT_PUSH, 
	{ true, false, _float3(0.f, 1.f, 0.25f), 20.f, 0.5f, 20.f, 20.f, -20.f},
	// BEHIT_FLY_PLAYER
	{ true, false, _float3(0.f, 0.45f, 0.55f), 13.5f, 0.5f, 20.f, 20.f, -20.f },
	// CHIIXA_AIRATTACK_FIRE 낙하, 직전 운동 상태 적용 안함(정지 상태로 초기화), xz감쇄율, y가속도 적용
	{ true, false, _float3(0.f, -1.f, 0.f),	0.f, 0.5f, 20.f, 20.f, -20.f },

};

void CCharacter::Shot_SoundKey(_tchar * szSoundTag, _uint iSoundChannelm, _float fVolume)
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	if (CT_PLAYER == m_eCollisionType)
	{
		pGame->StopSound(iPlayerSoundChannel);
		pGame->PlaySoundEx(szSoundTag, iPlayerSoundChannel++, fVolume);
		if (SOUND_PLAYER_TEMP_03 < iPlayerSoundChannel)
			iPlayerSoundChannel = SOUND_PLAYER_SWING;
	}
	else
	{
		pGame->StopSound(iSoundChannelm);
		pGame->PlaySoundEx(szSoundTag, iSoundChannelm, fVolume);
	}
}

void CCharacter::Shot_SlowKey(_float fTargetTime, _float fLerpSpeed)
{
	CGameInstance* pGame = CGameInstance::GetInstance();
	pGame->TimeSlowDown(fTargetTime, fLerpSpeed);
}

void CCharacter::Shot_DissolveKey(_bool bDissolveType, _float fDissolveSpeed)
{
	m_bRender = true;
	m_bDissolve = true;
	m_fDissolveTimeAcc = 0.f;

	m_bDissolveType = bDissolveType;
	m_fDissolveSpeed = fDissolveSpeed;
}

void CCharacter::Set_InitPos(_fvector vPos, _uint iNaviID)
{
	m_pMainTransform->Set_State(CTransform::STATE_POSITION, vPos);

	CNavigation* pNavi = static_cast<CNavigation*>(Find_Component(TEXT("Com_Navigation")));
	
	if(nullptr != pNavi)
		pNavi->Set_CurrentIndex(iNaviID);
}


void CCharacter::Regen()
{
	Shot_EffectKey(L"M_Appear_Effect", 0, 2, true);
}

void CCharacter::Set_WorldMatrix(_matrix matWorld)
{
	m_pMainTransform->Set_WorldMatrix(matWorld);
}

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
	return S_OK;
}

HRESULT CCharacter::Initialize(void * pArg)
{
	ZeroMemory(&m_Scon, sizeof StateController);

	m_vDissolveColor = { 1.f, 0.7f, 0.4f };

	return S_OK;
}

void CCharacter::Start()
{
}

void CCharacter::Tick(_double TimeDelta)
{
}

void CCharacter::LateTick(_double TimeDelta)
{
	// 디졸브 프레임 단위 처리
	if (m_bDissolve)
	{
		m_fDissolveTimeAcc += (_float)TimeDelta * m_fDissolveSpeed;
		
		if (m_bDissolveType)
			m_fDissolveAmount = 1.f - m_fDissolveTimeAcc;
		else
			m_fDissolveAmount = m_fDissolveTimeAcc;


		if (m_fDissolveAmount > 1.3f)
		{
			// Dissolve Out이 끝나면 Render를 끈다. 
			if (false == m_bDissolveType)
			{
				if (false == m_bDisableAfterDissolve)
					m_bRender = false;
				else
					SetState(DISABLE);
			}
		}
		else
			m_bRender = true;

		if (2.f <= m_fDissolveTimeAcc)
		{
			m_bDissolve = false;
			m_fDissolveTimeAcc = 0.f;
		}

	}
}

HRESULT CCharacter::Render()
{
	return S_OK;
}

HRESULT CCharacter::RenderShadow()
{
	return S_OK;
}

void CCharacter::RenderGUI()
{
}

void CCharacter::Reset_For_KeepState()
{
	if (true == IsDisable())
	{
		SetState(STATE::ACTIVE);
		Set_InitPos(XMLoadFloat3(&m_SpawnPoint.vP), m_SpawnPoint.iCellIndex);
	}
}

void CCharacter::Set_State(STATE eState)
{
	SetState(eState);
}

void CCharacter::SetUp_Activate(SPAWN_POINT SpawnPoint)
{
	m_SpawnPoint = SpawnPoint;

	Set_InitPos(XMLoadFloat3(&SpawnPoint.vP), SpawnPoint.iCellIndex);

	Regen();
}

void CCharacter::Free()
{
	__super::Free();
}

_vector CCharacter::Get_Position()
{
	return m_pMainTransform->Get_State(CTransform::STATE_POSITION);
}

_vector CCharacter::Get_CellHeight_Position()
{
	_vector vPos = m_pMainTransform->Get_State(CTransform::STATE_POSITION);


	CNavigation* pNavi = static_cast<CNavigation*>(Find_Component(TEXT("Com_Navigation")));
	if (nullptr != pNavi)
		vPos = XMVectorSetY(vPos, pNavi->Compute_Height(vPos));
	
	return vPos;
}

void CCharacter::Push_OnNavi(_fvector PushVector)
{
	m_pMainTransform->Push_OnNavi(PushVector, static_cast<CNavigation*>(Find_Component(TEXT("Com_Navigation"))));
}

void CCharacter::Set_LookAt(_vector vPos)
{
	m_pMainTransform->LookAt(vPos);
}

void CCharacter::Play_HitSound(TAGATTACK * pAttackInfo)
{
	CGameInstance* pGame = CGameInstance::GetInstance();

	if (CT_PLAYER == m_eCollisionType)
	{
		pGame->PlaySoundEx(pAttackInfo->szHitSoundTag, SOUND_PLAYER_HIT, VOLUME_VFX);
	}
	else if (CT_MONSTER == m_eCollisionType)
	{
		if (1 == m_iSoundChannel)
			pGame->PlaySoundEx(pAttackInfo->szHitSoundTag, SOUND_MONSTER1_HIT, VOLUME_VFX);
		else
			pGame->PlaySoundEx(pAttackInfo->szHitSoundTag, SOUND_MONSTER2_HIT, VOLUME_VFX);
	}
}