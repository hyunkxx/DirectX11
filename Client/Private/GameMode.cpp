#include "stdafx.h"
#include "..\Public\GameMode.h"

#include "Camera.h"
#include "PlayerCamera.h"
#include "Effect_Player.h"
#include "AcquireSystem.h"

IMPLEMENT_SINGLETON(CGameMode)

_bool CGameMode::s_bGameEnd;

void CGameMode::Free()
{
}

HRESULT CGameMode::Add_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND _hWnd, EFFECT_ID eEffectID , const _tchar * EffectTag, const char * TextureTag)
{
	list<EFFECT_DESC*> EffectDescList;
	CGameInstance::GetInstance()->Load_Effect(g_hWnd, EffectTag, &EffectDescList);
	CEffect* pEffect = CEffect_Player::Create(pDevice, pContext, TextureTag, EffectDescList);

	if (nullptr == pEffect)
		return E_FAIL;

	for (auto& iter : EffectDescList)
		Safe_Delete(iter);
	EffectDescList.clear();

	_tchar FileName[MAX_PATH] = TEXT("");
	_wsplitpath_s(EffectTag, nullptr, 0, nullptr, 0, FileName, MAX_PATH, nullptr, 0);
	if (FAILED(CGameInstance::GetInstance()->Push_Effect(FileName, pEffect , eEffectID)))
		return E_FAIL;

	return S_OK;
}

void CGameMode::SetInteractionActive(CInteractionUI::INTERACT_TYPE eInteractType, _bool bValue)
{
	m_pAcquireSystem->SetInteractionActive(eInteractType, bValue);
}

void CGameMode::EnqueueItemDesc(CItem::ITEM_DESC ItemDesc)
{
	m_pAcquireSystem->EnqueueItemDesc(ItemDesc);
}

void CGameMode::SetGagebar(_float fValue)
{
	m_pAcquireSystem->SetGageBar(fValue);
}

void CGameMode::Distance_BattleRange(_float fTargetDistance)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	if (0.0f >= fTargetDistance)
	{
		m_IsIn_BattleRange = false;
		return;
	}

	// ���� �Ÿ��� ������ ��. m_IsBattle ���� ���·� ���� , m_IsIn_BattleRange true 
	// �Ѹ����� ��� ���� ��� m_IsIn_BattleRange �� true
	// + �÷��̾ ���� �� �̶�� ( ���Ͱ� �¾��� ��� )
	if (m_fBattle_Range >= fTargetDistance || true == m_IsCombat)
	{
		if (false == m_IsBattle)
			m_IsChangeDelay = true;

		m_IsBattle = true;
		m_IsIn_BattleRange = true;
	}
	else
		m_IsIn_BattleRange = false;
}

void CGameMode::Reset_Battle()
{
	m_IsBattle = false;
}

void CGameMode::Reset_BattleRange()
{
	m_IsIn_BattleRange = false;
}

void CGameMode::SetUp_ChangeDelay()
{
	m_IsChangeDelay = true;
}

void CGameMode::Reset_ChangeDelay()
{
	m_IsChangeDelay = false;
}

void CGameMode::SetUp_Combat()
{
	m_IsCombat = true;
	m_RenewalCombat = true;
}

void CGameMode::Reset_Combat()
{
	m_IsCombat = false;
}

void CGameMode::SetUp_RenewalCombat()
{
	m_RenewalCombat = true;
}

void CGameMode::Reset_RenewalCombat()
{
	m_RenewalCombat = false;
}

void CGameMode::SetUp_BattleBgm()
{
	m_IsBattleBgm = true;
}

void CGameMode::Reset_BattleBgm()
{
	m_IsBattleBgm = false;
}

void CGameMode::SetUp_ChangeBgm()
{
	m_IsChangeBgm = true;
}

void CGameMode::Reset_ChangeBgm()
{
	m_IsChangeBgm = false;
}
