#include "..\Public\VIBuffer_Terrain.h"

#include "Frustum.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
	, m_pFrustum(CFrustum::GetInstance())
{
	Safe_AddRef(m_pFrustum);
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_isHeightMap(rhs.m_isHeightMap)
	, m_dwVertexCountX(rhs.m_dwVertexCountX)
	, m_dwVertexCountZ(rhs.m_dwVertexCountZ)
	, m_VertexPos(rhs.m_VertexPos)
	, m_pVertices{ rhs.m_pVertices }
	, m_pIndices{ rhs.m_pIndices }
	, m_pFrustum{ rhs.m_pFrustum }
	, m_pQuadTree{ rhs.m_pQuadTree }
{
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pQuadTree);
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

	m_pVertices = new VTXNORTEX[m_iVertexCount];
	ZeroMemory(m_pVertices, sizeof(VTXNORTEX) * m_iVertexCount);

	if (FAILED(Load_Vertices(TEXT("../../Data/GamePlay/Terrain/Height_Map/Vertices.data"))))
	{
		for (_uint i = 0; i < m_dwVertexCountZ; ++i)
		{
			for (_uint j = 0; j < m_dwVertexCountX; ++j)
			{
				_uint iIndex = i * m_dwVertexCountX + j;

				m_pVertices[iIndex].vPosition = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 10.0f, (_float)i);
				m_pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
				m_pVertices[iIndex].vTexUV = _float2((_float)j / (m_dwVertexCountX - 1.0f), i / (m_dwVertexCountZ - 1.0f));

				memcpy(&m_VertexPos[iIndex], &m_pVertices[iIndex].vPosition, sizeof(_float3));
			}
		}
	}

#pragma endregion

#pragma region INDEXBUFFER

	m_pIndices = new FACEINDICES32[m_iFaceCount];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iFaceCount);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_dwVertexCountZ - 1; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX - 1; ++j)
		{
			/* 삼각형 두개를 정의하기위한 루프. */
			_uint		iIndex = i * m_dwVertexCountX + j;

			/* 사각형의 좌상, 우상, 우하, 좌하 인덱스를 의미한다. */
			_uint		iIndices[] = {
				iIndex + m_dwVertexCountX,
				iIndex + m_dwVertexCountX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vFirst, vSecond, vNormal;

			/* 우상단 삼각형의 인덱스 세개를 구성하자. */
			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[1];
			m_pIndices[iNumFaces]._2 = iIndices[2];

			vFirst = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vSecond = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vFirst, vSecond));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal, vNormal + XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal, vNormal + XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal, vNormal + XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[2];
			m_pIndices[iNumFaces]._2 = iIndices[3];

			vFirst = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vSecond = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vFirst, vSecond));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal, vNormal + XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal, vNormal + XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal);
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal, vNormal + XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal);

			++iNumFaces;
		}
	}

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pPixel);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/*내가 그릴려고하는 인덱스하나의 크기 * 인덱스의 갯수.
	== 삼각형을 그리기위한 인덱스 세개의 크기 * 삼각형의갯수 */;
	m_BufferDesc.ByteWidth = m_iFaceIndexSize * m_iFaceCount;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

#pragma endregion

	m_pQuadTree = CQuadTree::Create(
		m_dwVertexCountX * m_dwVertexCountZ - m_dwVertexCountX,
		m_dwVertexCountX * m_dwVertexCountZ - 1,
		m_dwVertexCountX - 1,
		0
	);
	m_pQuadTree->SetUp_Neighbors();

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

	/*내가 그릴려고하는 인덱스하나의 크기 * 인덱스의 갯수.
	== 삼각형을 그리기위한 인덱스 세개의 크기 * 삼각형의갯수 */;
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

_float CVIBuffer_Terrain::Compute_Height(_fvector vPosition)
{
	if ((_uint)m_dwVertexCountZ < (_uint)XMVectorGetZ(vPosition) || (_uint)m_dwVertexCountX < (_uint)XMVectorGetX(vPosition))
		return 0;

	_uint		iIndex = _uint(XMVectorGetZ(vPosition)) * m_dwVertexCountX + _uint(XMVectorGetX(vPosition));

	if (m_iVertexCount < iIndex + m_dwVertexCountX + 1)
		return 0;

	_uint		iIndices[] = {
		iIndex + m_dwVertexCountX,
		iIndex + m_dwVertexCountX + 1,
		iIndex + 1,
		iIndex
	};

	_float		fWidth = XMVectorGetX(vPosition) - m_VertexPos[iIndices[0]].x;
	_float		fDepth = m_VertexPos[iIndices[0]].z - XMVectorGetZ(vPosition);

	_vector		vPlane = XMVectorZero();

	/* 온르쪽 위 */
	if (fWidth > fDepth)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_VertexPos[iIndices[0]]),
			XMLoadFloat3(&m_VertexPos[iIndices[1]]),
			XMLoadFloat3(&m_VertexPos[iIndices[2]]));
	}
	/* 왼쪽하단 */
	else
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_VertexPos[iIndices[0]]),
			XMLoadFloat3(&m_VertexPos[iIndices[2]]),
			XMLoadFloat3(&m_VertexPos[iIndices[3]]));
	}

	//ax + by + cz + d = 0;
	//y = (-ax - cz - d) / b;
	return (-XMVectorGetX(vPlane) * XMVectorGetX(vPosition) - XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}

void CVIBuffer_Terrain::Culling(_fmatrix Inverse_WorldMatrix)
{
	m_pFrustum->Transform_ToLocalSpace(Inverse_WorldMatrix);

	_uint		iNumFace = { 0 };

	m_pQuadTree->Culling(m_pFrustum, m_VertexPos, m_pIndices, &iNumFace);

	/* 컬링을 한 이후 정점 정보를 컬링을 한뒤의 정보로 바꿔줘야함 */
	D3D11_MAPPED_SUBRESOURCE		Subresource;
	ZeroMemory(&Subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource);

	memcpy(Subresource.pData, m_pIndices, sizeof(FACEINDICES32) * iNumFace);

	m_pContext->Unmap(m_pIB, 0);

	m_iFaceCount = iNumFace;
}

HRESULT CVIBuffer_Terrain::Load_Vertices(const _tchar * pFilePath)
{
	HANDLE		hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Load Data in VIBufferTerrain : Vertice");
		return E_FAIL;
	}

	DWORD		dwByte = 0;

	for (_uint i = 0; i < m_dwVertexCountZ; ++i)
	{
		for (_uint j = 0; j < m_dwVertexCountX; ++j)
		{
			_uint iIndex = i * m_dwVertexCountX + j;

			ReadFile(hFile, &m_pVertices[iIndex].vPosition, sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &m_pVertices[iIndex].vNormal, sizeof(_float3), &dwByte, nullptr);

			m_pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);

			m_pVertices[iIndex].vTexUV = _float2((_float)j / (m_dwVertexCountX - 1.0f), i / (m_dwVertexCountZ - 1.0f));

			memcpy(&m_VertexPos[iIndex], &m_pVertices[iIndex].vPosition, sizeof(_float3));
		}
	}

	CloseHandle(hFile);

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

	Safe_Release(m_pQuadTree);
	Safe_Release(m_pFrustum);

	if (false == m_isClone)
	{
		Safe_Delete_Array(m_VertexPos);

		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}
}
