#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CIntro final : public CGameObject
{
protected:
	explicit CIntro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CIntro(const CIntro& rhs);
	virtual ~CIntro() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CIntro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	CRenderer*		m_pRenderer = nullptr;
	CShader*		m_pShader = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CTexture*		m_pTexture = nullptr;

};

END