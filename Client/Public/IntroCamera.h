#pragma once

#include "Camera.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CLobbyCharacter;

class CIntroCamera final : public CCamera
{
public:
	enum INTRO_STATE { CAM_LEFT, CAM_RIGHT, CAM_ORIGIN, CAM_START, CAM_END };
	enum { CHAR_LEFT, CHAR_RIGHT, CHAR_END };

private:
	explicit CIntroCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CIntroCamera(const CIntroCamera& rhs);
	virtual ~CIntroCamera() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	_bool IsLobbyOut() const { return m_bLobbyOut; }

private:
	void StateCheck(_double TimeDelta);
	void CameraMovement(_double TimeDelta);
	void ZoomIn(_double TimeDelta);

public:
	_float GetCameraSpeed();

public:
	static CIntroCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool m_bMoveLock = true;
	_bool m_bZoomIn = false;
	_bool m_bLobbyOut = false;

	_bool m_bSelected = false;
	_float m_fEndAcc = 0.f;
	const _float m_fEndLimit = 3.8f;

	_bool m_bOnMouse[CHAR_END];
	RECT tagRectSize[CHAR_END];

	_uint m_eCurrentState = INTRO_STATE::CAM_START;
	_float4 vLookAtPos = { 123.f, 1.f, 112.f, 1.f };

	CLobbyCharacter* pLobbyCharacter[CHAR_END];

	_bool m_bStartMove = false;
	_float m_fStartTimeAcc = 0.f;

};

END