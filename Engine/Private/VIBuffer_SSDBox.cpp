#include "..\Public\VIBuffer_SSDBox.h"

CVIBuffer_SSDBox::CVIBuffer_SSDBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_SSDBox::CVIBuffer_SSDBox(const CVIBuffer_SSDBox& rhs)
	: CVIBuffer(rhs)
	, m_pVertexPosition(rhs.m_pVertexPosition)
{
}

HRESULT CVIBuffer_SSDBox::Initialize_Prototype()
{
	m_iStride = sizeof(VTXCUBE);
	m_iVertexCount = 8;
	m_iFaceIndexSize = sizeof(FACEINDICES16);
	m_iFaceCount = 12;
	m_iFaceIndexCount = 3;
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region ���ؽ� ���� ����
	VTXCUBE* pVertices = new VTXCUBE[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iVertexCount);

	m_pVertexPosition = new _float3[m_iVertexCount];

	pVertices[0].vPosition = { -0.5f, 0.5f, -0.5f };
	m_pVertexPosition[0] = pVertices[0].vPosition;

	pVertices[1].vPosition = { 0.5f, 0.5f,-0.5f };
	m_pVertexPosition[1] = pVertices[1].vPosition;

	pVertices[2].vPosition = { 0.5f, -0.5f, -0.5f };
	m_pVertexPosition[2] = pVertices[2].vPosition;

	pVertices[3].vPosition = { -0.5f, -0.5f, -0.5f };
	m_pVertexPosition[3] = pVertices[3].vPosition;

	pVertices[4].vPosition = { -0.5f, 0.5f, 0.5f };
	m_pVertexPosition[4] = pVertices[4].vPosition;

	pVertices[5].vPosition = { 0.5f, 0.5f, 0.5f };
	m_pVertexPosition[5] = pVertices[5].vPosition;

	pVertices[6].vPosition = { 0.5f, -0.5f, 0.5f };
	m_pVertexPosition[6] = pVertices[6].vPosition;

	pVertices[7].vPosition = { -0.5f, -0.5f, 0.5f };
	m_pVertexPosition[7] = pVertices[7].vPosition;
#pragma endregion


#pragma region �ε��� ���� ����

	FACEINDICES16* pIndices = new FACEINDICES16[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iFaceCount);

	pIndices[0]._0 = 1;
	pIndices[0]._1 = 5;
	pIndices[0]._2 = 6;

	pVertices[pIndices[0]._0].vNormal;

	pIndices[1]._0 = 1;
	pIndices[1]._1 = 6;
	pIndices[1]._2 = 2;

	// x-
	pIndices[2]._0 = 4;
	pIndices[2]._1 = 0;
	pIndices[2]._2 = 3;

	pIndices[3]._0 = 4;
	pIndices[3]._1 = 3;
	pIndices[3]._2 = 7;

	// y+
	pIndices[4]._0 = 4;
	pIndices[4]._1 = 5;
	pIndices[4]._2 = 1;

	pIndices[5]._0 = 4;
	pIndices[5]._1 = 1;
	pIndices[5]._2 = 0;

	// y-
	pIndices[6]._0 = 3;
	pIndices[6]._1 = 2;
	pIndices[6]._2 = 6;

	pIndices[7]._0 = 3;
	pIndices[7]._1 = 6;
	pIndices[7]._2 = 7;

	// z+
	pIndices[8]._0 = 7;
	pIndices[8]._1 = 6;
	pIndices[8]._2 = 5;

	pIndices[9]._0 = 7;
	pIndices[9]._1 = 5;
	pIndices[9]._2 = 4;

	// z-
	pIndices[10]._0 = 0;
	pIndices[10]._1 = 1;
	pIndices[10]._2 = 2;

	pIndices[11]._0 = 0;
	pIndices[11]._1 = 2;
	pIndices[11]._2 = 3;

	for (_uint i = 0; i < m_iFaceCount; ++i)
	{
		_vector vSource, vTarget, vNormal;

		vSource = XMLoadFloat3(&pVertices[pIndices[i]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[i]._0].vPosition);
		vTarget = XMLoadFloat3(&pVertices[pIndices[i]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[i]._1].vPosition);
		vNormal = XMVector3Normalize(XMVector3Cross(vSource, vTarget));

		XMStoreFloat3(&pVertices[pIndices[i]._0].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[i]._0].vNormal) + vNormal);
		XMStoreFloat3(&pVertices[pIndices[i]._1].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[i]._1].vNormal) + vNormal);
		XMStoreFloat3(&pVertices[pIndices[i]._2].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[i]._2].vNormal) + vNormal);
	}

	/* ���ؽ� */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
	{
		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
	{
		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_SSDBox::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_SSDBox* CVIBuffer_SSDBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_SSDBox*	pInstance = new CVIBuffer_SSDBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CVIBuffer_SSDBox";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_SSDBox::Clone(void* pArg)
{
	CVIBuffer_SSDBox*	pInstance = new CVIBuffer_SSDBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CVIBuffer_SSDBox";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_SSDBox::Free()
{
	__super::Free();

	if (false == m_isClone)
		Safe_Delete_Array(m_pVertexPosition);
}
