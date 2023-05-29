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
	// Ÿ�Ժ� �Լ� �ȿ��� ����
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

	/* ���ý� ���۸� ��� �迭 */
	ID3D11Buffer*			pVertexBuffer[] = {
		m_pVB,
		m_pVBInstance
	};
	/* ���ý� ���ۿ� ���� �ϳ��� ����� ��� �迭 */
	_uint					iStrides[] = {
		m_iStride,
		m_iStrideInstance
	};
	/* ������ - ���� ������ ù ��° ��ҿ� , ���Ǵ� ù ��° ��� ������ ����Ʈ �� (�� ó���� ���� ������ ����) */
	_uint					iOffsets[] = {
		0,
		0
	};

	/* ���ý� ���� ���� IASetVertexBuffers() */
	/* ���� ����ϰ����ϴ� ���ý� ���۵��� ��ġ�� ���ε� �Ѵ�. */
	/* -> ��ġ�� �ټ��� ���ý� ���۸� ���ε� �س��� �� �ִ�. (2,3,4 ���� ���� ���� �����͸� ����ϰ� �־ �迭�� ���� ���� ����) */
	m_pContext->IASetVertexBuffers(
		0, /* ó�� ������ �Է½��� (ù ��° ������ ��ġ) -> �ִ� ������ 16 / 32 �̴�. */
		m_iVertexBuffersCount, /* ������ ���ý� ���� ���� */
		pVertexBuffer, /* ������ ���ý� ������ �迭 */
		iStrides, /* �� ���ý� ������ ������ */
		iOffsets /* ���ؽ� ������ ������ */
	);

	/* �ε��� ���� ���� IASetIndexBuffer(IdxBuffer, format, offset); */
	m_pContext->IASetIndexBuffer(
		m_pIB, /* ������ �ε��� ���� */
		m_eIndexFormat, /* �ε��� ������ ����(������ ���� ����) [16 / 32 ��Ʈ ����] */
		0 /* �ε��� ������ ������ */
	);

	/* �������� ������Ƽ���� ���� */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/* 1. ������ �ε����� Ȱ���Ͽ� ����(�޽�)�� �׸��� */
	/* 2. ��� ������ ������ ������ ������ ���ļ� �׷��� �Ѵ�. �׷��� ���̴��� �ʿ��ϴ� */
	/* 3. ������ �ݵ�� ���̴��� ����ؾ� �Ѵ� -> ������ ���̴��� �Է� �Ǿ�� �Ѵ� */

	/* ( ������ �� �ε��� ��(���ý� ��) , �ε��� ������ ���� �ε��� ��ġ, ���ؽ� ������ ���� ���ؽ� ��ġ ) */
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

	/* �� �ν��Ͻ� ������ ũ�� = �ν��Ͻ� ���� ���� �ϳ��� ũ�� * ������� �ν��Ͻ��� ���� */
	m_BufferDesc.ByteWidth = m_iStrideInstance * m_iTotalNumInstance;

	/* (��������)D3D11_USAGE_DYNAMIC : CPU�� �����Ͽ� �� ������ ���� ������ �� �ֵ��� �Ѵ� */
	/* -> CPU�� �����Ͽ� �д� �ӵ��� ���� ���� / CPU �� ���� ������ Lock() , unLock() ��� ���� */
	/* (��������)D3D11_USAGE_DEFAULT : CPU�� �����Ͽ� �� ������ ���� ������ �� �ֵ��� ���� �ʴ´�. */
	/* -> CPU�� �����Ͽ� �д� �ӵ��� ���� / CPU �� ������ ���� Lock() , unLock() ��� �Ұ� */
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;  /* (VTXMATRIX)�ν��Ͻ� ���ۿ� ���ǵǾ� �ִ� ��������� �� �����Ӹ��� ��ġ�� �ٲ� �� �������� �������� ���۸� ������ش� */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	/* D3D11_CPU_ACCESS_WRITE : CPU�� ���۸� ���� �ְ� ����. (��������) */
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	/* 5. ��Ÿ �÷��� */
	/* 0 : ���� ���ۿ� ���ؼ��� 0���� ���� */
	m_BufferDesc.MiscFlags = 0;

	/* 6. ������ ���ۿ� ����� ���� �ϳ��� ũ�� (���� : ����Ʈ) */
	/* ���� �ϳ��� ����Ʈ ũ�� �� �־��ش�. */
	m_BufferDesc.StructureByteStride = m_iStrideInstance;

	m_pInstanceMatrix = new VTXMATRIX[m_iTotalNumInstance];
	ZeroMemory(m_pInstanceMatrix, sizeof(VTXMATRIX) * m_iTotalNumInstance);

	m_pTotalInstanceMatrix = new VTXMATRIX[m_iTotalNumInstance];
	ZeroMemory(m_pTotalInstanceMatrix, sizeof(VTXMATRIX) * m_iTotalNumInstance);

	/* �ν��Ͻ��� ������� ����. -> ȸ�� , �̵� , ũ�� ���� ���� ����? */
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

	/* ���� �θ��� CreateBuffer�� ����Ҽ� ���� -> Device �� ���� ���� ȣ���� ��� �Ѵ� */
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

	// -sin��1 -> ��1 : Y
	float sinTheta = { 0.0f }, Theta = { 0.0f };
	sinTheta = -fM._13;
	Theta = asinf(sinTheta);

	// cos�� * sin��2 -> ��2 : X
	float cosTheta = { 0.0f }, sinRho = { 0.0f }, Rho = { 0.0f };
	cosTheta = cosf(Theta);
	sinRho = fM._12 / cosf(Theta);
	Rho = asinf(sinRho);

	// sin��3 * cos�� -> ��3 : X
	float sinPsi = { 0.0f }, Psi = { 0.0f };
	sinPsi = fM._23 / cosTheta;
	Psi = asinf(sinPsi);

	// x z ���� �ٲ�°Ű��Ƽ� �ϴ� ���� �ٲ㺽 
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
		/* D3D11_MAP_WRITE_DISCARD : ��� ������ ZeroMemory�� �о������� -> ���� �����Ϳ� 1��1�� ��ü�� �� ��� ��� */
		/* D3D11_MAP_WRITE_NO_OVERWRITE : ���� �����͸� �츮�� ������ �Ҷ� ���ȴ� */
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
