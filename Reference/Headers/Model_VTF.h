#pragma once
#include "Model_Anim.h"
#include "Bone.h"

BEGIN(Engine)

class ENGINE_DLL CModel_VTF final : public CModel_Anim
{
private:
	CModel_VTF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_VTF(const CModel_VTF& rhs);
	virtual ~CModel_VTF() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pModelFilePath);
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	HRESULT	SetUp_VertexTexture(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex);
	HRESULT Init_VertexTexture();



private:
	ID3D11Texture2D** m_ppVertexTexture = { nullptr };
	ID3D11ShaderResourceView** m_ppVertexTextureSRV = { nullptr };

public:
	static CModel_VTF* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pModelFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free();
};

END