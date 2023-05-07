#include "..\Public\VIBuffer_Rect_Nor.h"

CVIBuffer_Rect_Nor::CVIBuffer_Rect_Nor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect_Nor::CVIBuffer_Rect_Nor(const CVIBuffer_Rect_Nor& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect_Nor::Initialize_Prototype()
{
	m_iStride = sizeof(VTXSMODEL);
	m_iVertexCount = 4;
	m_iFaceCount = 2;
	m_iFaceIndexCount = 3;
	m_iFaceIndexSize = sizeof(FACEINDICES32);
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region 버텍스 버퍼 세팅
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXSMODEL* pVertices = new VTXSMODEL[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXSMODEL) * m_iVertexCount);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _float2(0.0f, 0.f);
	pVertices[0].vNormal = _float3(0.f, 0.f, -1.f);
	

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.f, 0.f);
	pVertices[1].vNormal = _float3(0.f, 0.f, -1.f);
	

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.f, 1.f);
	pVertices[2].vNormal = _float3(0.f, 0.f, -1.f);


	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.0f, 1.f);
	pVertices[3].vNormal = _float3(0.f, 0.f, -1.f);

	XMStoreFloat3(&pVertices[0].vTangent ,  XMVector3Normalize(XMLoadFloat3(&pVertices[0].vPosition) - XMLoadFloat3(&pVertices[1].vPosition)));
	XMStoreFloat3(&pVertices[1].vTangent, XMVector3Normalize(XMLoadFloat3(&pVertices[1].vPosition) - XMLoadFloat3(&pVertices[2].vPosition)));
	XMStoreFloat3(&pVertices[2].vTangent, XMVector3Normalize(XMLoadFloat3(&pVertices[2].vPosition) - XMLoadFloat3(&pVertices[3].vPosition)));
	XMStoreFloat3(&pVertices[3].vTangent, XMVector3Normalize(XMLoadFloat3(&pVertices[3].vPosition) - XMLoadFloat3(&pVertices[0].vPosition)));

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
	{
		Safe_Delete_Array(pVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region 인덱스 버퍼 세팅
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

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;


	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
	{
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect_Nor::Initialize(void* pArg)
{

	return S_OK;
}

CVIBuffer_Rect_Nor* CVIBuffer_Rect_Nor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect_Nor*	pInstance = new CVIBuffer_Rect_Nor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CVIBuffer_Rect_Nor";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect_Nor::Clone(void * pArg)
{
	CVIBuffer_Rect_Nor*	pInstance = new CVIBuffer_Rect_Nor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CVIBuffer_Rect_Nor";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Nor::Free()
{
	__super::Free();
}