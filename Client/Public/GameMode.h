#pragma once

#include "Base.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "InteractionUI.h"
#include "AcquireUI.h"
#include "Camera.h"

#define LEVEL_ANYWHERE CGameMode::GetInstance()->GetCurrentLevel()

BEGIN(Engine)
class CRenderer;

END

BEGIN(Client)

class CGameMode final : public CBase
{
public:
	DECLARE_SINGLETON(CGameMode)

public:
	explicit CGameMode() = default;
	virtual ~CGameMode() = default;
	virtual void Free() override;

public:
	__forceinline POINT GetScreenMousePos() {
		GetCursorPos(&m_MousePosition);
		ScreenToClient(g_hWnd, &m_MousePosition);
		return m_MousePosition;
	};

	__forceinline _bool InRect(RECT rect) {
		POINT point = GetScreenMousePos();
		if (PtInRect(&rect, point))
			return true;
		else
			return false;
	};

	__forceinline _bool OnMouse(ORTHO_DESC tagDesc) {
		RECT rect;
		rect.left = LONG(tagDesc.fX - tagDesc.fWidth * 0.5f);
		rect.right = LONG(tagDesc.fX + tagDesc.fWidth * 0.5f);
		rect.top = LONG(tagDesc.fY - tagDesc.fHeight * 0.5f);
		rect.bottom = LONG(tagDesc.fY + tagDesc.fHeight * 0.5f);

		POINT point = GetScreenMousePos();
		if (PtInRect(&rect, point))
			return true;
		else
			return false;
	};

	void ResetStaticShadowBake() { m_pRenderer->ResetStaticShadow(); }
	void SetupRenderer(CRenderer* pRenderer) { m_pRenderer = pRenderer; };

	void SetupAcquireSystem(class CAcquireSystem* pAcquireSystem) {
		m_pAcquireSystem = pAcquireSystem;
	}

	void SetActiveUI(_bool bValue) { m_bActiveUI = bValue; }
	_bool IsActiveUI() const { return m_bActiveUI; }

public:
	void SetCurrentLevel(_uint nCurrentLevel) { m_nCurrentLevel = nCurrentLevel; }
	_uint GetCurrentLevel() const { return m_nCurrentLevel; };
	
	//Effect
	HRESULT Add_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , HWND _hWnd, EFFECT_ID eEffectID , const _tchar* EffectTag, const char* TextureTag);

private: // Utility
	_uint m_nCurrentLevel = 0;
	POINT m_MousePosition;

	_bool m_bActiveUI = false;

public: // UI Handler
	void SetInteractionActive(CInteractionUI::INTERACT_TYPE eInteractType, _bool bValue);
	void EnqueueItemDesc(CItem::ITEM_DESC ItemDesc);
	void SetGagebar(_float fValue);

public:
	void ReserveLevel(LEVEL_ID eLevelID) {
		if (LEVEL_ID::LEVEL_END <= eLevelID || 0 > eLevelID)
			return;
		m_IsReserveLevel = true;
		m_eReserveLevel_ID = eLevelID;
	}
	void Reset_ReserveLevel() {
		m_IsReserveLevel = false; 
		m_eReserveLevel_ID = LEVEL_END;
	}

	_bool Is_ReserveLevel() { return m_IsReserveLevel; }
	LEVEL_ID Get_ReserveLevel() { return m_eReserveLevel_ID; }
	
public:
	void  SetMouseActive(_bool bMouse) { m_bMouseActive = bMouse; }
	_bool GetMouseActive() { return m_bMouseActive; }
private:
	_bool				m_IsReserveLevel = { false };
	LEVEL_ID			m_eReserveLevel_ID = { LEVEL_END };
	_bool				m_bMouseActive = { false };

private: 
	class CAcquireSystem* m_pAcquireSystem = nullptr;
	CRenderer* m_pRenderer = nullptr;

};

END