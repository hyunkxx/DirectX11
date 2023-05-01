#include "..\Public\Mesh.h"

#include "Bone.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(void* pMeshInfo, CModel_Anim* pModel)
{
	// 타입별 함수 안에서 정의
	// m_iMaterialIndex = pAIMesh->mMaterialIndex;
	// m_iVerticesCount = pAIMesh->mNumVertices;
	// m_iPrimitiveCount = pAIMesh->mNumFaces;
	// m_iStride = sizeof(VTXMODEL);

	m_iFaceIndexSize = sizeof(FACEINDICES32);
	m_iFaceIndexCount = 3;
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr;

	if (nullptr == pModel)
		hr = Ready_VIBuffer_For_NonAnim(reinterpret_cast<SMESHINFO*>(pMeshInfo));
	else
		hr = Ready_VIBuffer_For_Anim(reinterpret_cast<DMESHINFO*>(pMeshInfo), pModel);

	if (FAILED(hr))
		return E_FAIL;


	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

void CMesh::Get_BoneMatrices(_float4x4 * pMeshBoneMatrices, CModel_Anim* Model)
{
	_uint iIndex = 0;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CBone* pBone = Model->Get_BonePtr(m_pBoneIndices[i]);

		XMStoreFloat4x4(&pMeshBoneMatrices[iIndex++], XMLoadFloat4x4(&pBone->Get_OffsetMatrix()) *
			XMLoadFloat4x4(&pBone->Get_CombinedTransfromationMatrix()) *
			Model->Get_LocalMatrix());
	}
}

void CMesh::Get_BoneMatrices_VTF(_float4x4 * pMeshBoneMatrices, CModel_Anim * Model)
{
	_uint iIndex = 0;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		CBone* pBone = Model->Get_BonePtr(m_pBoneIndices[i]);

		XMStoreFloat4x4(&pMeshBoneMatrices[iIndex++], XMLoadFloat4x4(&pBone->Get_OffsetMatrix()) *
			XMLoadFloat4x4(&pBone->Get_CombinedTransfromationMatrix()) *
			Model->Get_LocalMatrix());
	}
}

HRESULT CMesh::Ready_VIBuffer_For_NonAnim(SMESHINFO* pMeshInfo)
{
	m_iStride = sizeof(VTXSMODEL);
	m_iMaterialIndex = pMeshInfo->s_iMaterialIndex;
	m_iVertexCount = pMeshInfo->s_iNumVertices;
	m_iFaceCount = pMeshInfo->s_iNumFaces;
	lstrcpy(m_szName, pMeshInfo->s_szName);

#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXSMODEL*		pVertices = new VTXSMODEL[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXSMODEL) * m_iVertexCount);

	for (_uint i = 0; i < m_iVertexCount; ++i)
	{
		memcpy(&pVertices[i], &pMeshInfo->s_pVertices[i], sizeof(VTXSMODEL));

		//XMStoreFloat3(&SModel.s_pMeshes[MeshID].s_pVertices[VertexID].vPosition, XMVector3TransformNormal(XMLoadFloat3(&SModel.s_pMeshes[MeshID].s_pVertices[VertexID].vPosition), LocalMatrix));
		//XMStoreFloat3(&SModel.s_pMeshes[MeshID].s_pVertices[VertexID].vNormal, XMVector3TransformNormal(XMLoadFloat3(&SModel.s_pMeshes[MeshID].s_pVertices[VertexID].vNormal), LocalMatrix));
		//XMStoreFloat3(&SModel.s_pMeshes[MeshID].s_pVertices[VertexID].vTangent, XMVector3TransformNormal(XMLoadFloat3(&SModel.s_pMeshes[MeshID].s_pVertices[VertexID].vTangent), LocalMatrix));

		//memcpy(&pVertices[i].vPosition, &pMeshInfo->s_pVertices[i].vPosition, sizeof(_float3));
		//memcpy(&pVertices[i].vNormal, &pMeshInfo->s_pVertices[i].vNormal, sizeof(_float3));
		//memcpy(&pVertices[i].vTexUV, &pMeshInfo->s_pVertices[i].vTexUV, sizeof(_float2));
		//memcpy(&pVertices[i].vTangent, &pMeshInfo->s_pVertices[i].vTangent, sizeof(_float3));
	}

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
	{
		MSG_BOX("CVIBuffer_Rect / Initialize_Prototype / 00");
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEXBUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES32* pIndices = new FACEINDICES32[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iFaceCount);

	for (_uint i = 0; i < m_iFaceCount; ++i)
	{
		memcpy(&pIndices[i], &pMeshInfo->s_pFaces[i], sizeof(FACEINDICES32));

		//pIndices[i]._0 = pMeshInfo->s_pFaces[i]._0;
		//pIndices[i]._1 = pMeshInfo->s_pFaces[i]._1;
		//pIndices[i]._2 = pMeshInfo->s_pFaces[i]._2;
	}

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
	{
		MSG_BOX("CVIBuffer_Rect / Initialize_Prototype / 01");
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Ready_VIBuffer_For_Anim(DMESHINFO * pMeshInfo, CModel_Anim* pModel)
{
	m_iStride = sizeof(VTXDMODEL);
	m_iMaterialIndex = pMeshInfo->s_iMaterialIndex;
	m_iVertexCount = pMeshInfo->s_iNumVertices;
	m_iFaceCount = pMeshInfo->s_iNumFaces;
	lstrcpy(m_szName, pMeshInfo->s_szName);

#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXDMODEL*		pVertices = new VTXDMODEL[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXDMODEL) * m_iVertexCount);

	for (_uint i = 0; i < m_iVertexCount; ++i)
	{
		memcpy(&pVertices[i], &pMeshInfo->s_pVertices[i], sizeof(VTXDMODEL));

		//memcpy(&pVertices[i].vPosition, &pMeshInfo->s_pVertices[i].vPosition, sizeof(_float3));
		//memcpy(&pVertices[i].vNormal, &pMeshInfo->s_pVertices[i].vNormal, sizeof(_float3));
		//memcpy(&pVertices[i].vTexUV, &pMeshInfo->s_pVertices[i].vTexUV, sizeof(_float2));
		//memcpy(&pVertices[i].vTangent, &pMeshInfo->s_pVertices[i].vTangent, sizeof(_float3));
		/*XMUINT4		vBlendIndices;
		XMFLOAT4	vBlendWeights;*/
	}

	m_iNumBones = pMeshInfo->s_iNumBones;



	if (0 < m_iNumBones)
	{
		m_pBoneIndices = new _uint[m_iNumBones];


		for (_uint i = 0; i < m_iNumBones; ++i)
		{
			_uint iBoneIndex = pModel->Get_BoneIndex(pMeshInfo->s_pszBoneNameArray[i]);

			if (UINT_MAX == iBoneIndex)
				return E_FAIL;

			m_pBoneIndices[i] = iBoneIndex;

		}
	}

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
	{
		MSG_BOX("CVIBuffer_Rect / Initialize_Prototype / 00");
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEXBUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES32* pIndices = new FACEINDICES32[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iFaceCount);

	for (_uint i = 0; i < m_iFaceCount; ++i)
	{
		memcpy(&pIndices[i], &pMeshInfo->s_pFaces[i], sizeof(FACEINDICES32));

		//pIndices[i]._0 = pMeshInfo->s_pFaces[i]._0;
		//pIndices[i]._1 = pMeshInfo->s_pFaces[i]._1;
		//pIndices[i]._2 = pMeshInfo->s_pFaces[i]._2;
	}

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
	{
		MSG_BOX("CVIBuffer_Rect / Initialize_Prototype / 01");
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void* pMeshInfo, CModel_Anim* pModel)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pMeshInfo, pModel)))
	{
		MSG_BOX("Failed to Create : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pBoneIndices);
}
