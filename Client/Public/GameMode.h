#pragma once

#include "Base.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Renderer.h"

#define LEVEL_ANYWHERE CGameMode::GetInstance()->GetCurrentLevel()

BEGIN(Engine)
class CCamera;
END

class CGameMode final : public CBase
{
public:
	enum GAMEPLAY_CAM { CAM_DYNAMIC, CAM_PLAYER };

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

public:
	void SetCurrentLevel(_uint nCurrentLevel) { m_nCurrentLevel = nCurrentLevel; }
	_uint GetCurrentLevel() const { return m_nCurrentLevel; };
	
	//Effect
	HRESULT Add_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , HWND _hWnd, const _tchar* EffectTag, const char* TextureTag);

public: // Cams
	void ResetCameraList();
	void PushCamera(CCamera* pCamera);
	void UseCamera(int iCameraIndex);

private:
	_uint m_nCurrentLevel = 0;

	vector<CCamera*> m_pCams;

private: // Utility
	POINT m_MousePosition;

};