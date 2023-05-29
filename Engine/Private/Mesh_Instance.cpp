#include "..\Public\Mesh_Instance.h"

#include "Bone.h"
#include "GameInstance.h"

CMesh_Instance::CMesh_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh_Instance::CMesh_Instance(const CMesh_Instance& rhs)
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

HRESULT CMesh_Instance::Initialize_Prototype(void* pMeshInfo, _uint iNumInstance, _float4x4* pInstanceMatrix)
{
	// 타입별 함수 안에서 정의
	// m_iMaterialIndex = pAIMesh->mMaterialIndex;
	// m_iVerticesCount = pAIMesh->mNumVertices;
	// m_iPrimitiveCount = pAIMesh->mNumFaces;
	// m_iStride = sizeof(VTXMODEL);

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

HRESULT CMesh_Instance::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CMesh_Instance::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 버택스 버퍼를 담는 배열 */
	ID3D11Buffer*			pVertexBuffer[] = {
		m_pVB,
		m_pVBInstance
	};
	/* 버택스 버퍼에 정점 하나의 사이즈를 담는 배열 */
	_uint					iStrides[] = {
		m_iStride,
		m_iStrideInstance
	};
	/* 오프셋 - 정점 버퍼의 첫 번째 요소와 , 사용되는 첫 번째 요소 사이의 바이트 수 (맨 처음과 시작 까지의 간격) */
	_uint					iOffsets[] = {
		0,
		0
	};

	/* 버택스 버퍼 설정 IASetVertexBuffers() */
	/* 내가 사용하고자하는 버택스 버퍼들을 장치에 바인딩 한다. */
	/* -> 장치는 다수의 버택스 버퍼를 바인딩 해놓을 수 있다. (2,3,4 인자 들은 이중 포인터를 사용하고 있어서 배열을 만들어서 인자 전달) */
	m_pContext->IASetVertexBuffers(
		0, /* 처음 설정할 입력슬롯 (첫 번째 버퍼의 위치) -> 최대 슬롯은 16 / 32 이다. */
		m_iVertexBuffersCount, /* 설정할 버택스 버퍼 개수 */
		pVertexBuffer, /* 설정할 버택스 버퍼의 배열 */
		iStrides, /* 각 버택스 버퍼의 사이즈 */
		iOffsets /* 버텍스 버퍼의 오프셋 */
	);

	/* 인덱스 버퍼 설정 IASetIndexBuffer(IdxBuffer, format, offset); */
	m_pContext->IASetIndexBuffer(
		m_pIB, /* 설정할 인덱스 버퍼 */
		m_eIndexFormat, /* 인덱스 버퍼의 포멧(데이터 형식 지정) [16 / 32 비트 정수] */
		0 /* 인덱스 버퍼의 오프셋 */
	);

	/* 랜더링할 프리미티브의 종류 */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/* 1. 정점과 인덱스를 활용하여 도형(메시)를 그린다 */
	/* 2. 모든 정점은 랜더링 파이프 라인을 거쳐서 그려야 한다. 그래서 셰이더가 필요하다 */
	/* 3. 정점은 반드시 셰이더를 통과해야 한다 -> 정점은 셰이더에 입력 되어야 한다 */

	/* ( 렌더링 할 인덱스 수(버택스 수) , 인덱스 버퍼의 최초 인덱스 위치, 버텍스 버퍼의 최초 버텍스 위치 ) */
	m_pContext->DrawIndexedInstanced(m_iFaceIndexCount * m_iFaceCount, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

#pragma region MODEL_INSTANCE
HRESULT CMesh_Instance::Ready_VIBuffer_For_NonAnim_Instance(SMESHINFO * pMeshInfo, _float4x4* pInstanceMatrix)
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

	/*FACEINDICES32*		pIndices = new FACEINDICES32[m_iFaceCount * m_iNumInstance];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iFaceCount * m_iNumInstance);*/

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

	/* 총 인스턴스 버퍼의 크기 = 인스턴스 버퍼 정점 하나의 크기 * 만들려는 인스턴스의 개수 */
	m_BufferDesc.ByteWidth = m_iStrideInstance * m_iTotalNumInstance;

	/* (동적버퍼)D3D11_USAGE_DYNAMIC : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 한다 */
	/* -> CPU가 접근하여 읽는 속도가 가장 빠름 / CPU 가 접근 함으로 Lock() , unLock() 사용 가능 */
	/* (정적버퍼)D3D11_USAGE_DEFAULT : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 하지 않는다. */
	/* -> CPU가 접근하여 읽는 속도가 느림 / CPU 의 접근을 막아 Lock() , unLock() 사용 불가 */
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;  /* (VTXMATRIX)인스턴스 버퍼에 정의되어 있는 정점행렬은 매 프레임마다 위치가 바뀔 수 있음으로 동적으로 버퍼를 만들어준다 */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	/* D3D11_CPU_ACCESS_WRITE : CPU가 버퍼를 쓸수 있게 설정. (동적버퍼) */
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	/* 5. 기타 플래그 */
	/* 0 : 정점 버퍼에 대해서는 0으로 지정 */
	m_BufferDesc.MiscFlags = 0;

	/* 6. 구조적 버퍼에 저장된 원소 하나의 크기 (단위 : 바이트) */
	/* 정점 하나의 바이트 크기 를 넣어준다. */
	m_BufferDesc.StructureByteStride = m_iStrideInstance;

	m_pInstanceMatrix = new VTXMATRIX[m_iTotalNumInstance];
	ZeroMemory(m_pInstanceMatrix, sizeof(VTXMATRIX) * m_iTotalNumInstance);

	m_pTotalInstanceMatrix = new VTXMATRIX[m_iTotalNumInstance];
	ZeroMemory(m_pTotalInstanceMatrix, sizeof(VTXMATRIX) * m_iTotalNumInstance);

	/* 인스턴싱의 행렬정보 세팅. -> 회전 , 이동 , 크기 등을 여서 조절? */
	for (_uint i = 0; i < m_iTotalNumInstance; ++i)
	{
		memcpy(&m_pInstanceMatrix[i], &pInstanceMatrix[i], sizeof(_float4x4));
		/*m_pInstanceMatrix[i].vPosition.x *= 100.0f;
		m_pInstanceMatrix[i].vPosition.y *= 100.0f;
		m_pInstanceMatrix[i].vPosition.z *= 100.0f;*/

		memcpy(&m_pTotalInstanceMatrix[i], &m_pInstanceMatrix[i], sizeof(_float4x4));
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pTotalInstanceMatrix;

	/* 기존 부모의 CreateBuffer를 사용할수 없음 -> Device 로 부터 직접 호출해 줘야 한다 */
	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

#pragma endregion INSTANCEBUFFER

	return S_OK;
}

void CMesh_Instance::Update(_uint iNumInstance, _float4x4 * pInstanceMatrix)
{
}

void CMesh_Instance::Get_PSA_To_InstanceMatrix(_uint iGetNum, SOBJECT_DESC* pOut)
{
	if (m_iTotalNumInstance <= iGetNum || 0 > iGetNum)
		return;

	pOut->vS = GetScale_To_InstanceMatrix(iGetNum);
	pOut->vA = GetAngle_To_InstanceMatrix(iGetNum);
	pOut->vP = GetPosition_To_InstanceMatrix(iGetNum);
}

_float3 CMesh_Instance::GetScale_To_InstanceMatrix(_uint iGetNum)
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

_float3 CMesh_Instance::GetAngle_To_InstanceMatrix(_uint iGetNum)
{
	if (m_iTotalNumInstance <= iGetNum || 0 > iGetNum)
		return _float3{ 0.0f, 0.0f, 0.0f };

	_float4x4	fM;
	ZeroMemory(&fM, sizeof(_float4x4));

	memcpy(&fM, &m_pTotalInstanceMatrix[iGetNum], sizeof(_float4x4));

	// -sinθ1 -> θ1 : Y
	float sinTheta = { 0.0f }, Theta = { 0.0f };
	sinTheta = -fM._13;
	Theta = asinf(sinTheta);

	// cosθ * sinθ2 -> θ2 : X
	float cosTheta = { 0.0f }, sinRho = { 0.0f }, Rho = { 0.0f };
	cosTheta = cosf(Theta);
	sinRho = fM._12 / cosf(Theta);
	Rho = asinf(sinRho);

	// sinθ3 * cosθ -> θ3 : X
	float sinPsi = { 0.0f }, Psi = { 0.0f };
	sinPsi = fM._23 / cosTheta;
	Psi = asinf(sinPsi);

	// x z 축이 바뀌는거같아서 일단 순서 바꿔봄 
	return _float3{ XMConvertToDegrees(Psi),XMConvertToDegrees(Theta),XMConvertToDegrees(Rho) };
	//return _float3{ XMConvertToDegrees(Rho),XMConvertToDegrees(Theta),XMConvertToDegrees(Psi) };

	/*
	_float4x4 fR;
	memcpy(&fR, &m_pTotalInstanceMatrix[iGetNum], sizeof(_float4x4));

	float sinTheta = -fR._13;
	float Theta = asinf(sinTheta);

	float cosTheta = cosf(Theta);
	float sinRho = fR._12 / cosf(Theta);
	float Rho = asinf(sinRho);

	float sinPsi = fR._23 / cosTheta;
	float Psi = asinf(sinPsi);

	return _float3{ XMConvertToDegrees(Rho),XMConvertToDegrees(Theta),XMConvertToDegrees(Psi) };
	*/
}

_float3 CMesh_Instance::GetPosition_To_InstanceMatrix(_uint iGetNum)
{
	if (m_iTotalNumInstance <= iGetNum || 0 > iGetNum)
		return _float3{ 0.0f, 0.0f, 0.0f };

	_float4x4	fM;
	ZeroMemory(&fM, sizeof(_float4x4));

	memcpy(&fM, &m_pTotalInstanceMatrix[iGetNum], sizeof(_float4x4));

	return _float3{ fM._41, fM._42, fM._43 };
}

void CMesh_Instance::Culling(_fmatrix Inverse_WorldMatrix, _float fRadius)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;

	pGameInstance->Transform_ToLocalSpace(Inverse_WorldMatrix);

	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	if (FAILED(m_pContext->Map(
		m_pVBInstance,
		0,
		D3D11_MAP_WRITE_DISCARD,
		/* D3D11_MAP_WRITE_DISCARD : 모든 정보가 ZeroMemory로 밀어져있음 -> 이전 데이터와 1대1로 교체를 할 경우 사용 */
		/* D3D11_MAP_WRITE_NO_OVERWRITE : 기존 데이터를 살리고 수정을 할때 사용된다 */
		0,
		&SubResource
	)))
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

	return;
}

#pragma endregion MODEL_INSTANCE

CMesh_Instance * CMesh_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void* pMeshInfo, _uint iNumInstance, _float4x4* pInstanceMatrix)
{
	CMesh_Instance* pInstance = new CMesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pMeshInfo, iNumInstance, pInstanceMatrix)))
	{
		MSG_BOX("Failed to Create : CMesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh_Instance::Clone(void * pArg)
{
	CMesh_Instance* pInstance = new CMesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CMesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Instance::Free()
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
