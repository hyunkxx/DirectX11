#include "stdafx.h"
#include "..\Public\GameMode.h"

#include "Camera.h"
#include "PlayerCamera.h"
#include "Effect_Player.h"
#include "AcquireSystem.h"

IMPLEMENT_SINGLETON(CGameMode)

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
