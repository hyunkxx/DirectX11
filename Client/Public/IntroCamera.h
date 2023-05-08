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
	enum INTRO_STATE { CAM_ORIGIN, CAM_LEFT, CAM_RIGHT };
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
	void TargetChange(_double TimeDelta);

public:
	_float GetCameraSpeed();

public:
	static CIntroCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool m_bOnMouse[CHAR_END];
	RECT tagRectSize[CHAR_END];

	_uint m_eCurrentState = INTRO_STATE::CAM_ORIGIN;
	_float4 vLookAtPos = { 0.f, 1.f, 0.f, 1.f };

	CLobbyCharacter* pLobbyCharacter[CHAR_END];

};

END