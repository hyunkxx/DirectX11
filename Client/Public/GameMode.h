#pragma once

#include "Base.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Renderer.h"

#define LEVEL_ANYWHERE CGameMode::GetInstance()->GetCurrentLevel()

class CGameMode final : public CBase
{
public:
	DECLARE_SINGLETON(CGameMode)

public:
	explicit CGameMode() = default;
	virtual ~CGameMode() = default;
	virtual void Free() override;


	//ID3D11DeviceContext*
public:
	void SetCurrentLevel(_uint nCurrentLevel) { m_nCurrentLevel = nCurrentLevel; }
	_uint GetCurrentLevel() const { return m_nCurrentLevel; };
	
	//Effect
	HRESULT Add_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , HWND _hWnd, const _tchar* EffectTag, const char* TextureTag);
private:
	_uint m_nCurrentLevel = 0;

};