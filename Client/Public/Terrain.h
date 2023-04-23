#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTransform;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
protected:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public: 
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader*			m_pShader = { nullptr };
	CTexture*			m_pTexture = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CVIBuffer_Terrain*	m_pVIBuffer = { nullptr };

};

END