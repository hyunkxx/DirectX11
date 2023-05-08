#pragma once

#include "Camera.h"

BEGIN(Client)

class CIntroCamera final : public CCamera
{
public:
	enum INTRO_STATE { ORIGIN, FIRST, SECOND };

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
	_float GetCameraSpeed();

public:
	static CIntroCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	INTRO_STATE m_eState = INTRO_STATE::ORIGIN;
	_float3 m_vOriginPos = { 0.f, 3.f, -10.f };
	_float3 m_vFirstPos, m_vSecondPos;

};

END