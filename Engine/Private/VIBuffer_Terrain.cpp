#include "..\Public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_isHeightMap(rhs.m_isHeightMap)
	, m_dwVertexCountX(rhs.m_dwVertexCountX)
	, m_dwVertexCountZ(rhs.m_dwVertexCountZ)
	, m_VertexPos(rhs.m_VertexPos)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	m_isHeightMap = true;

	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
	{
		wstring message = pHeightMapFilePath;
		MESSAGE(message);
		return E_FAIL;
	}

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	_ulong* pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);
	CloseHandle(hFile);

	m_VertexPos = new _float3[ih.biWidth * ih.biHeight];

	m_iStride = sizeof(VTXNORTEX);
	m_dwVertexCountX = ih.biWidth;
	m_dwVertexCountZ = ih.biHeight;
	m_iVertexCount = m_dwVertexCountX * m_dwVertexCountZ;

	m_iFaceIndexSize = sizeof(FACEINDICES32);
	m_iFaceCount = (m_dwVertexCountX - 1) * (m_dwVertexCountZ - 1) * 2;
	m_iFaceIndexCount = 3;
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEXBUFFER

	VTXNORTEX* pVertices = new VTXNORTEX[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iVertexCount);

	for (_uint i = 0; i < m_dwVertexCountZ; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX; ++j)
		{
			_uint iIndex = i * m_dwVertexCountX + j;

			pVertices[iIndex].vPosition = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 10.0f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (m_dwVertexCountX - 1.0f), i / (m_dwVertexCountZ - 1.0f));

			m_VertexPos[iIndex] = pVertices[iIndex].vPosition;
		}
	}

#pragma endregion

#pragma region INDEXBUFFER

	FACEINDICES32* pIndices = new FACEINDICES32[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iFaceCount);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_dwVertexCountZ - 1; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX - 1; ++j)
		{
			/* �ﰢ�� �ΰ��� �����ϱ����� ����. */
			_uint		iIndex = i * m_dwVertexCountX + j;

			/* �簢���� �»�, ���, ����, ���� �ε����� �ǹ��Ѵ�. */
			_uint		iIndices[] = {
				iIndex + m_dwVertexCountX,
				iIndex + m_dwVertexCountX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vFirst, vSecond, vNormal;

			/* ���� �ﰢ���� �ε��� ������ ��������. */
			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			vFirst = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vSecond = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vFirst, vSecond));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			vFirst = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vSecond = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vFirst, vSecond));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, vNormal + XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;
		}
	}

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
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pPixel);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/*���� �׸������ϴ� �ε����ϳ��� ũ�� * �ε����� ����.
	== �ﰢ���� �׸������� �ε��� ������ ũ�� * �ﰢ���ǰ��� */;
	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const TERRAIN_SIZE& iSize)
{
	m_isHeightMap = false;

	m_VertexPos = new _float3[iSize.mX * iSize.mZ];

	m_iStride = sizeof(VTXNORTEX);
	m_dwVertexCountX = iSize.mX;
	m_dwVertexCountZ = iSize.mZ;
	m_iVertexCount = m_dwVertexCountX * m_dwVertexCountZ;

	m_iFaceIndexSize = sizeof(FACEINDICES32);
	m_iFaceCount = (m_dwVertexCountX - 1) * (m_dwVertexCountZ - 1) * 2;
	m_iFaceIndexCount = 3;
	m_iVertexBuffersCount = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEXBUFFER

	VTXNORTEX* pVertices = new VTXNORTEX[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iVertexCount);

	for (_uint i = 0; i < m_dwVertexCountZ; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX; ++j)
		{
			_uint iIndex = i * m_dwVertexCountX + j;

			pVertices[iIndex].vPosition = _float3((_float)j, 0.f / 10.0f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (m_dwVertexCountX - 1.0f), i / (m_dwVertexCountZ - 1.0f));

			m_VertexPos[iIndex] = pVertices[iIndex].vPosition;
		}
	}

#pragma endregion

#pragma region INDEXBUFFER

	FACEINDICES32* pIndices = new FACEINDICES32[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iFaceCount);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_dwVertexCountZ - 1; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX - 1; ++j)
		{
			_uint iIndex = i * m_dwVertexCountX + j;

			_uint iIndices[] = {
				iIndex + m_dwVertexCountX,
				iIndex + m_dwVertexCountX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			++iNumFaces;
		}
	}

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
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/*���� �׸������ϴ� �ε����ϳ��� ũ�� * �ε����� ����.
	== �ﰢ���� �׸������� �ε��� ������ ũ�� * �ﰢ���ǰ��� */;
	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TERRAIN_SIZE& iSize)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iSize)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (false == m_isClone)
		Safe_Delete_Array(m_VertexPos);
}
