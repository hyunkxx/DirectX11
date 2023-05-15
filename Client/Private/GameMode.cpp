#include "stdafx.h"
#include "..\Public\GameMode.h"

#include "Camera.h"
#include "Effect_Player.h"

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

void CGameMode::ResetCameraList()
{
	m_pCams.clear();
}

void CGameMode::PushCamera(CCamera * pCamera)
{
	m_pCams.push_back(pCamera);
}

void CGameMode::UseCamera(int iCameraIndex)
{
	int iCameraSize = (int)m_pCams.size();
	for (int i = 0; i < iCameraSize; ++i)
	{
		if (iCameraIndex == i)
			m_pCams[i]->Set_Use(true);
		else
			m_pCams[i]->Set_Use(false);
	}
}

void CGameMode::SetRenderInteractUI(CInteractionUI::INTERACT_TYPE eInteractType, _bool bValue)
{

}
