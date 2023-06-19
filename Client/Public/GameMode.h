#pragma once

#include "Base.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "InteractionUI.h"
#include "AcquireUI.h"
#include "Camera.h"
#include "PhaseChanger.h"

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

	CPhaseChanger* Get_PhaseChanger() { return m_pPhaseChanger; }
	void Set_PhaseChanger(CPhaseChanger* pPhaseChanger) 
	{
		m_pPhaseChanger = pPhaseChanger;
	}

	_bool Get_Chixia() { return m_bChixia; }
	void Set_Chixia(_bool bChixia) { m_bChixia = bChixia; }
	
public:
	void  SetMouseActive(_bool bMouse) { m_bMouseActive = bMouse; }
	_bool GetMouseActive() { return m_bMouseActive; }
private:
	_bool				m_IsReserveLevel = { false };
	LEVEL_ID			m_eReserveLevel_ID = { LEVEL_END };
	_bool				m_bMouseActive = { false };

	CPhaseChanger*		m_pPhaseChanger = { nullptr };

	_bool				m_bChixia = { false };

public:
	void Distance_BattleRange(_float fTargetDistance);

	// 전투 비전투 판단
	_bool Is_Battle() const { return m_IsBattle; }
	void Reset_Battle();

	_bool IsIn_BattleRange() const { return m_IsIn_BattleRange; }
	void Reset_BattleRange();

	// 사운드 변경 딜레이 -> SMoothOff
	_bool Is_ChangeDelay() const { return m_IsChangeDelay; }
	void SetUp_ChangeDelay();
	void Reset_ChangeDelay();

private:
	_bool				m_IsChangeDelay = { false };
	_bool				m_IsBattle = { false };
	_bool				m_IsIn_BattleRange = { false };

	const _float&		m_fBattle_Range = { 20.0f };
	_double				m_Battle_Time = { 0.0 };
	
public:
	_bool Is_Combat() const { return m_IsCombat; }
	void SetUp_Combat();
	void Reset_Combat();

	_bool Is_RenewalCombat() const { return m_RenewalCombat; }
	void SetUp_RenewalCombat();
	void Reset_RenewalCombat();

private:
	// 전투 중 임을 체크함.
	_bool				m_IsCombat = { false };

	_bool				m_RenewalCombat = { false };

public: /* Crown */
	_bool Is_BattleBgm() const { return m_IsBattleBgm; }
	void SetUp_BattleBgm();
	void Reset_BattleBgm();

	_bool Is_ChangeBgm() const { return m_IsChangeBgm; }
	void SetUp_ChangeBgm();
	void Reset_ChangeBgm();

private:
	_bool				m_IsBattleBgm = { false };
	_bool				m_IsChangeBgm = { false };

private: 
	class CAcquireSystem* m_pAcquireSystem = nullptr;
	CRenderer* m_pRenderer = nullptr;

};

END