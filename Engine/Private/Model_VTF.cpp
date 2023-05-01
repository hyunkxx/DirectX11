#include "..\Public\Model_VTF.h"

#include "Bone.h"
#include "Mesh.h"
#include "Animation.h"
#include "AnimController.h"

#include "Texture.h"
#include "Shader.h"


CModel_VTF::CModel_VTF(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CModel_Anim(pDevice, pContext)
{
}

CModel_VTF::CModel_VTF(const CModel_VTF & rhs)
	: CModel_Anim(rhs)
{

}

HRESULT CModel_VTF::Initialize_Prototype(const _tchar * pModelFilePath)
{
	__super::Initialize_Prototype(pModelFilePath);

	return S_OK;
}

HRESULT CModel_VTF::Initialize(void * pArg)
{
	__super::Initialize();

	// 본 한바퀴 돌면서 Combined 갱신해놓는다.
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedMatrix();
	}

	// 텍스처 만들어야 함
	if (FAILED(Init_VertexTexture()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel_VTF::SetUp_VertexTexture(CShader * pShaderCom, const char * pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;


	_float4x4 MeshBoneMatrices[1024];

	m_Meshes[iMeshIndex]->Get_BoneMatrices_VTF(MeshBoneMatrices, this);

	// 읽어온 매트릭스를 텍스처에 그린다.
	D3D11_MAPPED_SUBRESOURCE MappedSubResource;

	m_pContext->Map(m_ppVertexTexture[iMeshIndex], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
	memcpy(MappedSubResource.pData, MeshBoneMatrices, sizeof(_float4x4) * 1024);
	m_pContext->Unmap(m_ppVertexTexture[iMeshIndex], 0);

	if (FAILED(pShaderCom->SetShaderResourceView(pConstantName, m_ppVertexTextureSRV[iMeshIndex])))
		return E_FAIL;

	return  S_OK;
}

HRESULT CModel_VTF::Init_VertexTexture()
{

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 64;
	TextureDesc.Height = 64;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	m_ppVertexTexture = new ID3D11Texture2D*[m_iNumMeshes];
	m_ppVertexTextureSRV = new ID3D11ShaderResourceView*[m_iNumMeshes];

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_ppVertexTexture[i]);
		m_pDevice->CreateShaderResourceView(m_ppVertexTexture[i], nullptr, &m_ppVertexTextureSRV[i]);

		D3D11_MAPPED_SUBRESOURCE MappedSubResource;

		_float4x4 MeshBoneMatrices[1024];

		for (_uint i = 0; i < m_iNumBones; ++i)
		{
			XMStoreFloat4x4(&MeshBoneMatrices[i], XMLoadFloat4x4(&m_Bones[i]->Get_OffsetMatrix()) *
				XMLoadFloat4x4(&m_Bones[i]->Get_CombinedTransfromationMatrix()) *
				Get_LocalMatrix());
		}

		m_pContext->Map(m_ppVertexTexture[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);
		memcpy(MappedSubResource.pData, MeshBoneMatrices, sizeof(_float4x4) * 1024);
		m_pContext->Unmap(m_ppVertexTexture[i], 0);
	}

	return S_OK;
}

CModel_VTF* CModel_VTF::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pModelFilePath)
{
	CModel_VTF* pInstance = new CModel_VTF(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Create : CModel_VTF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel_VTF::Clone(void * pArg)
{
	CModel_VTF* pInstance = new CModel_VTF(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel_VTF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_VTF::Free()
{
	__super::Free();

	if (true == m_isClone)
	{
		for (_uint i = 0; i < m_iNumMeshes; ++i)
		{
			Safe_Release(m_ppVertexTextureSRV[i]);
			Safe_Release(m_ppVertexTexture[i]);
		}

		Safe_Delete_Array(m_ppVertexTextureSRV);
		Safe_Delete_Array(m_ppVertexTexture);
	}
}
