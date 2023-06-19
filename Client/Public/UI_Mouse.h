	#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)

class CUI_Mouse final : public CGameObject
{
protected:
	explicit CUI_Mouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Mouse(const CUI_Mouse& rhs);
	virtual ~CUI_Mouse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

	

	
public:
	_float3 Get_MousePos();
	void Set_Texchange(_bool change);
	void Set_RenderMouse(_bool change) { m_bRender = change; }
private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();



private:
	_float		m_fX, m_fY, m_fZ = { 0.f }, m_fWidth = { 25.f }, m_fHeight = { 30.f };
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_uint		m_iTexNum = { 0 };
	_bool		m_bTexchange = { false };

	_float m_fColorR = { 0.f };
	_float m_fColorG = { 0.f };
	_float m_fColorB = { 0.f };
	_float m_fColorA = { -30.f };
	
	_bool m_bRender = { false };


public:
	static CUI_Mouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	CTexture*		m_pTexture = { nullptr };
};

END