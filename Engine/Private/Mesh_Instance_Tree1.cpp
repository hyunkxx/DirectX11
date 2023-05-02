#include "..\Public\Mesh_Instance_Tree1.h"

#include "GameInstance.h"

CMesh_Instance_Tree1::CMesh_Instance_Tree1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh_Instance_Tree1::CMesh_Instance_Tree1(const CMesh_Instance_Tree1& rhs)
	: CVIBuffer(rhs)
	, m_iStrideInstance{ rhs.m_iStrideInstance }
	, m_pVBInstance{ rhs.m_pVBInstance }
	, m_iNumInstance{ rhs.m_iNumInstance }
	, m_pInstanceMatrix{ rhs.m_pInstanceMatrix }
	, m_pIndices{ rhs.m_pIndices }
	, m_iTotalNumInstance{ rhs.m_iTotalNumInstance }
	, m_pTotalInstanceMatrix{ rhs.m_pTotalInstanceMatrix }
{

	Safe_AddRef(m_pVBInstance);
}

HRESULT CMesh_Instance_Tree1::Initialize_Prototype(void* pMeshInfo, _uint iNumInstance, _float4x4* pInstanceMatrix)
{
	m_iTotalNumInstance = iNumInstance;
	m_iNumInstance = m_iTotalNumInstance;

	m_iFaceIndexSize = sizeof(FACEINDICES32);
	m_iFaceIndexCount = 3;
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr;

	hr = Ready_VIBuffer_For_NonAnim_Instance(reinterpret_cast<SMESHINFO*>(pMeshInfo), pInstanceMatrix);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh_Instance_Tree1::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh_Instance_Tree1::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*			pVertexBuffer[] = { m_pVB, m_pVBInstance };
	_uint					iStrides[] = { m_iStride, m_iStrideInstance };
	_uint					iOffsets[] = { 0, 0 };

	m_pContext->IASetVertexBuffers(0, m_iVertexBuffersCount, pVertexBuffer, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB,	m_eIndexFormat,	0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iFaceIndexCount * m_iFaceCount, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

#pragma region MODEL_INSTANCE
HRESULT CMesh_Instance_Tree1::Ready_VIBuffer_For_NonAnim_Instance(SMESHINFO * pMeshInfo, _float4x4* pInstanceMatrix)
{
	m_iStrideInstance = sizeof(VTXMATRIX);

	m_iStride = sizeof(VTXSMODEL);
	m_iMaterialIndex = pMeshInfo->s_iMaterialIndex;
	m_iVertexCount = pMeshInfo->s_iNumVertices;
	m_iFaceCount = pMeshInfo->s_iNumFaces;

	lstrcpy(m_szName, pMeshInfo->s_szName);

	m_iVertexBuffersCount = 2;

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

	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount * m_iTotalNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iFaceCount * m_iTotalNumInstance];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iFaceCount * m_iTotalNumInstance);

	for (_uint i = 0; i < m_iTotalNumInstance; ++i)
	{
		for (_uint j = 0; j < m_iFaceCount; ++j)
		{
			_uint		iIndex = i * m_iFaceCount + j;
			memcpy(&pIndices[iIndex], &pMeshInfo->s_pFaces[j], sizeof(FACEINDICES32));
		}
	}

	m_pIndices = new FACEINDICES32[m_iFaceCount];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iFaceCount);

	for (_uint i = 0; i < m_iFaceCount; ++i)
		memcpy(&m_pIndices[i], &pMeshInfo->s_pFaces[i], sizeof(FACEINDICES32));

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
	{
		MSG_BOX("CVIBuffer_Rect / Initialize_Prototype / 01");
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCEBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStrideInstance * m_iTotalNumInstance;

	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStrideInstance;

	m_pInstanceMatrix = new VTXMATRIX[m_iTotalNumInstance];
	ZeroMemory(m_pInstanceMatrix, sizeof(VTXMATRIX) * m_iTotalNumInstance);

	m_pTotalInstanceMatrix = new VTXMATRIX[m_iTotalNumInstance];
	ZeroMemory(m_pTotalInstanceMatrix, sizeof(VTXMATRIX) * m_iTotalNumInstance);

	for (_uint i = 0; i < m_iTotalNumInstance; ++i)
	{
		memcpy(&m_pInstanceMatrix[i], &pInstanceMatrix[i], sizeof(_float4x4));
		memcpy(&m_pTotalInstanceMatrix[i], &m_pInstanceMatrix[i], sizeof(_float4x4));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pTotalInstanceMatrix;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

#pragma endregion INSTANCEBUFFER

	return S_OK;
}

void CMesh_Instance_Tree1::Update(_uint iNumInstance, _float4x4 * pInstanceMatrix)
{
}

void CMesh_Instance_Tree1::Get_PSA_To_InstanceMatrix(_uint iGetNum, SOBJECT_DESC* pOut)
{
	if (m_iTotalNumInstance <= iGetNum || 0 > iGetNum)
		return;

	pOut->vS = GetScale_To_InstanceMatrix(iGetNum);
	pOut->vA = GetAngle_To_InstanceMatrix(iGetNum);
	pOut->vP = GetPosition_To_InstanceMatrix(iGetNum);
}

_float3 CMesh_Instance_Tree1::GetScale_To_InstanceMatrix(_uint iGetNum)
{
	if (m_iTotalNumInstance <= iGetNum || 0 > iGetNum)
		return _float3{ 0.0f, 0.0f, 0.0f };

	_float4x4	fM;
	ZeroMemory(&fM, sizeof(_float4x4));

	memcpy(&fM, &m_pTotalInstanceMatrix[iGetNum], sizeof(_float4x4));

	_vector vR = { 0.0f, 0.0f, 0.0f, 0.0f };
	_vector vU = { 0.0f, 0.0f, 0.0f, 0.0f };
	_vector vL = { 0.0f, 0.0f, 0.0f, 0.0f };

	vR = { fM._11, fM._12, fM._13, fM._14 };
	vU = { fM._21, fM._22, fM._23, fM._24 };
	vL = { fM._31, fM._32, fM._33, fM._34 };

	return _float3{ XMVectorGetX(XMVector3Length(vR)), XMVectorGetX(XMVector3Length(vU)), XMVectorGetX(XMVector3Length(vL)) };
}

_float3 CMesh_Instance_Tree1::GetAngle_To_InstanceMatrix(_uint iGetNum)
{
	if (m_iTotalNumInstance <= iGetNum || 0 > iGetNum)
		return _float3{ 0.0f, 0.0f, 0.0f };

	_float4x4 fM = {};
	ZeroMemory(&fM, sizeof(_float4x4));

	memcpy(&fM, &m_pTotalInstanceMatrix[iGetNum], sizeof(_float4x4));

	_vector		vR = {}, vU = {}, vL = {};

	vR = XMVector3Normalize(XMLoadFloat4x4(&fM).r[0]);
	vU = XMVector3Normalize(XMLoadFloat4x4(&fM).r[1]);
	vL = XMVector3Normalize(XMLoadFloat4x4(&fM).r[2]);

	_matrix SM = XMMatrixIdentity();
	SM = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	vR = XMVector3TransformNormal(vR, SM);
	vU = XMVector3TransformNormal(vU, SM);
	vL = XMVector3TransformNormal(vL, SM);

	_float4		fR = {}, fU = {}, fL = {};

	XMStoreFloat4(&fR, vR);
	memcpy(((_float4*)&fM + 0), &fR, sizeof(_float3));
	XMStoreFloat4(&fU, vU);
	memcpy(((_float4*)&fM + 1), &fU, sizeof(_float3));
	XMStoreFloat4(&fL, vL);
	memcpy(((_float4*)&fM + 2), &fL, sizeof(_float3));


	float sinTheta = -fM._13;
	float Theta = asinf(sinTheta);

	float cosTheta = cosf(Theta);
	float sinRho = fM._12 / cosf(Theta);
	float Rho = asinf(sinRho);

	float sinPsi = fM._23 / cosTheta;
	float Psi = asinf(sinPsi);

	_float3		vAngle = { 0.0f, 0.0f, 0.0f };
	vAngle = { XMConvertToDegrees(Psi),XMConvertToDegrees(Theta),XMConvertToDegrees(Rho) };

	Safe_Angle(vAngle);
	return vAngle;
}

_float3 CMesh_Instance_Tree1::GetPosition_To_InstanceMatrix(_uint iGetNum)
{
	if (m_iTotalNumInstance <= iGetNum || 0 > iGetNum)
		return _float3{ 0.0f, 0.0f, 0.0f };

	_float4x4	fM;
	ZeroMemory(&fM, sizeof(_float4x4));

	memcpy(&fM, &m_pTotalInstanceMatrix[iGetNum], sizeof(_float4x4));

	return _float3{ fM._41, fM._42, fM._43 };
}

void CMesh_Instance_Tree1::Safe_Angle(_float3 vAngle)
{
	if (true == isnan(vAngle.x))
		vAngle.x = 0.0f;
	if (true == isnan(vAngle.y))
		vAngle.y = 0.0f;
	if (true == isnan(vAngle.z))
		vAngle.z = 0.0f;

	if (0.10f >= vAngle.x && -0.10f <= vAngle.x)
		vAngle.x = 0.10f;
	if (0.10f >= vAngle.y && -0.10f <= vAngle.y)
		vAngle.y = 0.10f;
	if (0.10f >= vAngle.z && -0.10f <= vAngle.z)
		vAngle.z = 0.10f;

	if (90.10f >= vAngle.x && 89.90f <= vAngle.x)
		vAngle.x = 90.10f;
	if (90.10f >= vAngle.y && 89.90f <= vAngle.y)
		vAngle.y = 90.10f;
	if (90.10f >= vAngle.z && 89.90f <= vAngle.y)
		vAngle.z = 90.10f;

	if (-90.10f <= vAngle.x && -89.90f >= vAngle.x)
		vAngle.x = -90.10f;
	if (-90.10f <= vAngle.y && -89.90f >= vAngle.y)
		vAngle.y = -90.10f;
	if (-90.10f <= vAngle.z && -89.90f >= vAngle.y)
		vAngle.z = -90.10f;

	if (179.90f <= vAngle.x)
		vAngle.x = 179.90f;
	if (179.90f <= vAngle.y)
		vAngle.y = 179.90f;
	if (179.90f <= vAngle.y)
		vAngle.z = 179.90f;

	if (-179.90f >= vAngle.x)
		vAngle.x = -179.90f;
	if (-179.90f >= vAngle.y)
		vAngle.y = -179.90f;
	if (-179.90f >= vAngle.y)
		vAngle.z = -179.90f;
}

void CMesh_Instance_Tree1::GetOn_Terrain()
{
}

void CMesh_Instance_Tree1::Culling(_fmatrix Inverse_WorldMatrix, _float fRadius)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	pGameInstance->Transform_ToLocalSpace(Inverse_WorldMatrix);

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return;

	m_fRadius = fRadius;

	ZeroMemory(m_pInstanceMatrix, sizeof(VTXMATRIX) * m_iTotalNumInstance);

	_uint		iNumInstance = { 0 };

	for (_uint i = 0; i < m_iTotalNumInstance; i++)
	{
		if (true == pGameInstance->InLocalSpace(XMLoadFloat4(&m_pTotalInstanceMatrix[i].vPosition), m_fRadius))
		{
			memcpy(&m_pInstanceMatrix[iNumInstance++], &m_pTotalInstanceMatrix[i], sizeof(_float4x4));
		}
	}

	memcpy(SubResource.pData, m_pInstanceMatrix, sizeof(_float4x4) * iNumInstance);
	m_iNumInstance = iNumInstance;

	m_pContext->Unmap(m_pVBInstance, 0);
}

#pragma endregion MODEL_INSTANCE

CMesh_Instance_Tree1 * CMesh_Instance_Tree1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void* pMeshInfo, _uint iNumInstance, _float4x4* pInstanceMatrix)
{
	CMesh_Instance_Tree1* pInstance = new CMesh_Instance_Tree1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pMeshInfo, iNumInstance, pInstanceMatrix)))
	{
		MSG_BOX("Failed to Create : CMesh_Instance_Tree1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh_Instance_Tree1::Clone(void * pArg)
{
	CMesh_Instance_Tree1* pInstance = new CMesh_Instance_Tree1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh_Instance_Tree1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Instance_Tree1::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);

	if (false == m_isClone)
	{
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pInstanceMatrix);

		Safe_Delete_Array(m_pTotalInstanceMatrix);
	}

}
