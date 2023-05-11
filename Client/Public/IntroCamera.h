#pragma once

#include "Camera.h"

BEGIN(Engine)
class CGameObject;
class CVIBuffer_Rect;
class CShader;
class CTexture;
class CRenderer;
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

	HRESULT setupTaptoStartResources();
	HRESULT setupApplyResources();

	void renderTapToStartTexture();
	void renderApplyTexture();
	void renderChooseRoverTexture();

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
	CRenderer* m_pRenderer = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CShader* m_pShader = nullptr;
	CTexture* m_pTapStartTexture = nullptr;
	CTexture* m_pApplyTexture = nullptr;
	CTexture* m_pTwinklOnMask = nullptr;
	CTexture* m_pTwinklOffMask = nullptr;
	CTexture* m_pTextBackground = nullptr;
	CTexture* m_pChooseRoverBack = nullptr;
	CTexture* m_pChooseRoverText = nullptr;

	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	_float		m_fChooseX, m_fChooseY, m_fChooseWidth, m_fChooseHeight, m_fChooseTextWidth, m_fChooseTextHeight;
	_float4x4	m_ChooseWorldMatrix, m_ChooseTextWorldMatrix;

	_bool m_bTapToStartRenderStart = false;
	_bool m_bTapToStartRender = false;

	_bool m_bApplyRenderStart = false;
	_bool m_bApplyRender = false;

	_float m_fTwinklTimeAcc = 0.f;
	_float m_fApplyTimeAcc = 0.f;

	_bool m_bChooseRender = false;
	_float m_fAlpha = 0.f;

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
	_float4 vLookAtPos = { 123.f, 2.5f, 112.f, 1.f };

	CLobbyCharacter* pLobbyCharacter[CHAR_END];

	_bool m_bStartMove = false;
	_float m_fStartTimeAcc = 0.f;

};

END