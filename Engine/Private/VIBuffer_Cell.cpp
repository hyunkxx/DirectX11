#include "..\Public\VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3 * pPoints)
{
	m_iStride = sizeof(VTXPOS); /* ���� �ϳ��� ����Ʈ ũ�� */
	m_iVertexCount = 3; /* ������ ���� */

	m_iFaceCount = 1;

	m_iFaceIndexCount = 4;
	m_iFaceIndexSize = sizeof(_ushort);

	m_iVertexBuffersCount = 1;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;

	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; /* �������� �׸��ڴ� -> ���� 1���� �����̴� */

#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXPOS*		pVertices = new VTXPOS[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iVertexCount);

	memcpy(pVertices, pPoints, sizeof(_float3) * m_iVertexCount);

	m_pVerticesPos = new _float3[m_iVertexCount];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iVertexCount);

	memcpy(m_pVerticesPos, pPoints, sizeof(_float3) * m_iVertexCount);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion VERTEXBUFFER

#pragma region INDEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = sizeof(_ushort) * m_iFaceIndexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	m_pIndices = new _ushort[m_iFaceIndexCount];
	ZeroMemory(m_pIndices, sizeof(_ushort) * m_iFaceIndexCount);

	/* �ε��� ���ۿ� ������ �׸��� ���� ���� */
	/* Strip ���� �ﰢ���� �׸� �� (0 , 1 , 2)������ 3���� ������ �׸��� �ﰢ���� �ϼ����� ����. 4���� �����ؼ� �������� �������� �������� ��������� �ﰢ���� �ϼ��� */
	m_pIndices[0] = 0;
	m_pIndices[1] = 1;
	m_pIndices[2] = 2;
	m_pIndices[3] = 0;


	/* �ε����� �������� ������ �� �Ѱ��ش� */
	m_SubResourceData.pSysMem = m_pIndices;

	/* ���� �θ��� �ε��� ���� �Լ��� �ҷ��� */
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	/* �� ����� ���� �迭 ���� */
	//Safe_Delete_Array(m_pIndices);

#pragma endregion INDEXBUFFER

	return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void * pArg)
{
	return S_OK;
}

_float CVIBuffer_Cell::Compute_Height(_fvector vPosition)
{
	_vector		vPlane = XMVectorZero();

	vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[m_pIndices[0]]),
		XMLoadFloat3(&m_pVerticesPos[m_pIndices[1]]),
		XMLoadFloat3(&m_pVerticesPos[m_pIndices[2]]));

	//ax + by + cz + d = 0;

	//y = (-ax - cz - d) / b;

	return (-XMVectorGetX(vPlane) * XMVectorGetX(vPosition) - XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}

CVIBuffer_Cell * CVIBuffer_Cell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints)
{
	CVIBuffer_Cell*		pInstance = new CVIBuffer_Cell(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pPoints)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Cell::Clone(void * pArg)
{

	CVIBuffer_Cell*		pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pIndices);
	Safe_Delete_Array(m_pVerticesPos);
}
