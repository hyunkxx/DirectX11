#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

//LoadingBackground
class CBackGround final : public CGameObject
{
protected:
	explicit CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetLoadRatio(_float fValue) { m_fLoadRatio = fValue; }

private:
	HRESULT Add_Components();
	HRESULT Setup_BackgroundResources();
	HRESULT Setup_TextResources();
	HRESULT Setup_GageResources(_uint nGageType);
	HRESULT Setup_SideResources();

	void ComputeLoadRatioLerp(_double TimeDelta);

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	_float		m_fTextX, m_fTextY, m_fTextWidth, m_fTextHeight;
	_float4x4	m_TextWorldMatrix;

private:
	_float		m_fGageX, m_fGageY, m_fGageWidth, m_fGageHeight;
	_float4x4	m_GageWorldMatrix;
	enum { GAGE_BACK, GAGE_FRONT };

private:
	_float		m_fSideX, m_fSideY, m_fSideWidth, m_fSideHeight;
	_float4x4	m_SideWorldMatrix;

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	CTexture*		m_pTexture = { nullptr };
	CTexture*		m_pTextTexture = { nullptr };
	CTexture*		m_pSideTexture = { nullptr };
	CTexture*		m_pBackTexture = { nullptr };
	CTexture*		m_pFrontTexture = { nullptr };

private:
	_float m_fLoadRatio = 0.0f;
	_float m_fCurLoadAcc = 0.0f;

};

END