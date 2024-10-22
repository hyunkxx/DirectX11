#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CRenderer;
class CTexture;
class CShader;
class CNavigation;
END

BEGIN(Client)

class CCharacterSelect_Terrain final : public CGameObject
{
public:
	enum TERRAIN_KINDS { T_1, T_2, T_3, T_4, T_END };

protected:
	explicit CCharacterSelect_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCharacterSelect_Terrain(const CCharacterSelect_Terrain& rhs);
	virtual ~CCharacterSelect_Terrain() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	_uint				m_iShader_PassID = { 0 };

	_float				m_fUVSampler_Ratio_1 = { 0.0f };
	_float				m_fUVSampler_Ratio_2 = { 0.0f };
	_float				m_fUVSampler_Ratio_3 = { 0.0f };
	_float				m_fUVSampler_Ratio_4 = { 0.0f };

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CCharacterSelect_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader*			m_pShader = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CVIBuffer_Terrain*	m_pVIBuffer = { nullptr };

	// 안쓰는 것 같아 주석 처리
	//CNavigation*		m_pNavigation = { nullptr };

	CTexture*			m_pDiffuseTexture[TERRAIN_KINDS::T_END] = { nullptr };
	CTexture*			m_pNormalTexture[TERRAIN_KINDS::T_END] = { nullptr };

	CTexture*			m_pFilterTexture = { nullptr };

public:
	HRESULT Load_UVSamplerRatio_Data(const _tchar* pSamplerRatioFilePath);

};

END