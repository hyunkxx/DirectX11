#include "..\Public\VIBuffer_Particle.h"
#include "Shader.h"

CVIBuffer_Particle::CVIBuffer_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Particle::CVIBuffer_Particle(const CVIBuffer_Particle & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_iStrideInstance(rhs.m_iStrideInstance)
	, m_pSpeed(rhs.m_pSpeed)
	, m_pRotationMatrix(rhs.m_pRotationMatrix)
{
	m_ParticleDesc.iNumInstance = rhs.m_ParticleDesc.iNumInstance;
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Particle::Initialize_Prototype(const ParticleDesc& _ParticleDesc)
{
	memcpy(&m_ParticleDesc, &_ParticleDesc, sizeof(ParticleDesc));

	srand(unsigned(time(nullptr)));
	//Imgui 설정용
	m_iSettimeMaxNumInst = m_ParticleDesc.iNumInstance;

	m_iMaxNumInstance = m_ParticleDesc.iNumInstance;
	m_iOriMaxNumInstance = m_ParticleDesc.iNumInstance;
	m_pStartScale = new _float3[m_ParticleDesc.iNumInstance];
	m_pCurScale = new _float3[m_ParticleDesc.iNumInstance];
	m_pEndScale = new _float3[m_ParticleDesc.iNumInstance];
	m_pOriPos = new _float3[m_ParticleDesc.iNumInstance];
	m_pAngle = new _float3[m_ParticleDesc.iNumInstance];

	m_pDist = new _float[m_ParticleDesc.iNumInstance];
	m_pMaxDist = new _float[m_ParticleDesc.iNumInstance];
	m_pSpeed = new _float[m_ParticleDesc.iNumInstance];
	m_pRotationMatrix = new _float4x4[m_ParticleDesc.iNumInstance];

	m_vCurColor = _ParticleDesc.vStartColor;	

	for (_int i = 0; i < m_ParticleDesc.iNumInstance; ++i)
	{
		m_pMaxDist[i] = (rand() % ((_int)(m_ParticleDesc.fMaxDist * 100.f - m_ParticleDesc.fMinDist * 100.f) + 1) + m_ParticleDesc.fMinDist * 100.f) / 100.f;
		m_pDist[i] = 0.f;

		m_pSpeed[i] = rand() % (_int)(m_ParticleDesc.fMaxSpeed * 100 - m_ParticleDesc.fMinSpeed * 100 + 1) + m_ParticleDesc.fMinSpeed * 100;
		m_pSpeed[i] /= 100.f;

		m_pStartScale[i].x = (rand() % ((_int)(m_ParticleDesc.vMaxScale.x * 100.f - m_ParticleDesc.vMinScale.x * 100.f) + 1) + m_ParticleDesc.vMinScale.x * 100.f) / 100.f;
		m_pStartScale[i].y = (rand() % ((_int)(m_ParticleDesc.vMaxScale.y * 100.f - m_ParticleDesc.vMinScale.y * 100.f) + 1) + m_ParticleDesc.vMinScale.y * 100.f) / 100.f;
		m_pStartScale[i].z = (rand() % ((_int)(m_ParticleDesc.vMaxScale.z * 100.f - m_ParticleDesc.vMinScale.z * 100.f) + 1) + m_ParticleDesc.vMinScale.z * 100.f) / 100.f;
		m_pStartScale[i] = m_pStartScale[i];
		m_pCurScale[i] = m_pStartScale[i];
		m_pEndScale[i] = m_pStartScale[i];

		m_pOriPos[i].x = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.x * 100.f - m_ParticleDesc.vMinPosition.x * 100.f) + 1) + m_ParticleDesc.vMinPosition.x * 100.f) / 100.f;
		m_pOriPos[i].y = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.y * 100.f - m_ParticleDesc.vMinPosition.y * 100.f) + 1) + m_ParticleDesc.vMinPosition.y * 100.f) / 100.f;
		m_pOriPos[i].z = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.z * 100.f - m_ParticleDesc.vMinPosition.z * 100.f) + 1) + m_ParticleDesc.vMinPosition.z * 100.f) / 100.f;
		m_pOriPos[i] = _float3(m_pOriPos[i].x / 100.f, m_pOriPos[i].y / 100.f, m_pOriPos[i].z / 100.f);

		_matrix Matrix = XMMatrixIdentity();
		_float AngleX = rand() % ((_int)(m_ParticleDesc.vMaxAngle.x - m_ParticleDesc.vMinAngle.x) + 1) + m_ParticleDesc.vMinAngle.x;
		_float AngleY = rand() % ((_int)(m_ParticleDesc.vMaxAngle.y - m_ParticleDesc.vMinAngle.y) + 1) + m_ParticleDesc.vMinAngle.y;

		Matrix *= XMMatrixRotationX(XMConvertToRadians(AngleX));
		Matrix *= XMMatrixRotationY(XMConvertToRadians(AngleY));
		XMStoreFloat4x4(&m_pRotationMatrix[i], Matrix);
	}

	m_iStrideInstance = sizeof(VTXMATRIX);
	m_iStride = sizeof(VTXPOINT);
	m_iVertexCount = 1;
	m_iFaceIndexSize = sizeof(_ushort);
	m_iFaceCount = m_ParticleDesc.iNumInstance;
	m_iFaceIndexCount = 1;
	m_iVertexBuffersCount = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iVertexCount;

	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXPOINT*		pVertices = new VTXPOINT[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iVertexCount);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	pVertices[0].vPSize = _float2(1.0f, 1.f);

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


#pragma region INDEXBUFFER
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

	_ushort*		pIndices = new _ushort[m_iFaceCount];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iFaceCount);

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCEBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStrideInstance * m_ParticleDesc.iNumInstance;

	/*D3D11_USAGE_DYNAMIC : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 한다. */
	/*D3D11_USAGE_DEAFULT : CPU가 접근하여 이 공간에 값을 수정할 수 있도록 하지 않는다. */
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStrideInstance;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXMATRIX*		pInstanceVertices = new VTXMATRIX[m_ParticleDesc.iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_ParticleDesc.iNumInstance);

	for (_int i = 0; i < m_ParticleDesc.iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}

	m_SubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);

#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Particle::Initialize(void * pArg)
{
	return E_NOTIMPL;
}

HRESULT CVIBuffer_Particle::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint					iStrides[] = {
		m_iStride,
		(_uint)m_iStrideInstance

	};

	_uint					iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iVertexBuffersCount, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);
	m_pContext->DrawIndexedInstanced(m_iVertexBuffersCount,
		m_ParticleDesc.iNumInstance,
		0, 0, 0);

	return S_OK;
}

void CVIBuffer_Particle::Update(_double TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;

	XMStoreFloat3(&m_vCurColor, XMVectorLerp(XMLoadFloat3(&m_vCurColor), XMLoadFloat3(&m_ParticleDesc.vEndColor), 0.5f * (_float)TimeDelta));

	m_fTimeAcc += (_float)TimeDelta / 5.f;
	m_fDelayAcc += (_float)TimeDelta;

	if (m_fDelayAcc > m_ParticleDesc.fDelayTime)
	{
		m_ParticleDesc.iNumInstance += m_ParticleDesc.iDelayParticle;

		if (m_ParticleDesc.iNumInstance > m_iMaxNumInstance)
			m_ParticleDesc.iNumInstance = m_iMaxNumInstance;
		m_fDelayAcc = 0.f;
	}

	for (_int i = 0; i < m_ParticleDesc.iNumInstance; ++i)
	{
		_float4 vOldPos = ((VTXMATRIX*)SubResource.pData)[i].vPosition;
		_float3 vLook = _float3(((VTXMATRIX*)SubResource.pData)[i].vLook.x,
			((VTXMATRIX*)SubResource.pData)[i].vLook.y, ((VTXMATRIX*)SubResource.pData)[i].vLook.z);

		_float3 vPos = _float3(vOldPos.x, vOldPos.y, vOldPos.z);

		if (m_ParticleDesc.bGravuty)
		{
			_float3 vNewPos;

			_float3 vNorLook;
			XMStoreFloat3(&vNorLook, XMVector3Normalize(XMLoadFloat3(&vLook)));
			_float3 vMove = _float3(0.f, 0.f, 0.f);

			XMStoreFloat3(&vMove, XMLoadFloat3(&vNorLook) *  m_pSpeed[i] * (_float)TimeDelta);

			vMove.y -= (0.5f * m_ParticleDesc.fGravityPower * (m_fTimeAcc * m_fTimeAcc));

			XMStoreFloat3(&vNewPos, XMLoadFloat3(&vPos) + XMLoadFloat3(&vMove));

			m_pDist[i] += XMVectorGetX(XMVector3Length(XMLoadFloat3(&vMove)));
			_float3 vNewLook;
			_float3 vUp = { 0.f , 1.f , 0.f };
			_float3 vRight = { 1.f , 0.f , 0.f };

			XMStoreFloat3(&vNewLook, XMVector3Normalize((XMLoadFloat3(&vNewPos) - XMLoadFloat3(&vPos))));
			XMStoreFloat3(&vRight, XMVector3Cross(XMLoadFloat3(&vUp), XMLoadFloat3(&vNewLook)));
			XMStoreFloat3(&vUp, XMVector3Cross(XMLoadFloat3(&vNewLook), XMLoadFloat3(&vRight)));

			XMStoreFloat3(&vRight, XMLoadFloat3(&vRight) * m_pStartScale[i].x);
			XMStoreFloat3(&vUp, XMLoadFloat3(&vUp) * m_pStartScale[i].y);
			XMStoreFloat3(&vNewLook, XMLoadFloat3(&vNewLook) * m_pStartScale[i].z);
			((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(vRight.x, vRight.y, vRight.z, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(vUp.x, vUp.y, vUp.z, 0.f);
			((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(vNewLook.x, vNewLook.y, vNewLook.z, 0.f);

			vPos = vNewPos;
		}
		else
		{
			m_pDist[i] += XMVectorGetX(XMVector3Length(XMVector3Normalize(XMLoadFloat3(&vLook)) * m_pSpeed[i] * (_float)TimeDelta));
			XMStoreFloat3(&vPos, XMLoadFloat3(&vPos) + XMVector3Normalize(XMLoadFloat3(&vLook)) * m_pSpeed[i] * (_float)TimeDelta);

		}

		if (m_pMaxDist[i] < m_pDist[i])
		{
			if (m_ParticleDesc.bLoop)
			{
				m_pDist[i] = 0.f;
				vPos = m_pOriPos[i];
			}
			else
			{
				_float3 vStartScale = m_pStartScale[i];
				_float3 vCurScale = m_pCurScale[i];
				_float3 vEndScale = m_pEndScale[i];
				_float3 vOriPos = m_pOriPos[i];
				_float fDist = m_pDist[i];
				_float fMaxDist = m_pMaxDist[i];
				_float fSpeed = m_pSpeed[i];
				_float4x4 vRotationMatrix = m_pRotationMatrix[i];

				for (_int j = i; m_ParticleDesc.iNumInstance - 1 > j; j++)
				{
					m_pStartScale[i] = m_pStartScale[i + 1];
					m_pCurScale[i] = m_pCurScale[i + 1];
					m_pEndScale[i] = m_pEndScale[i + 1];
					m_pOriPos[i] = m_pOriPos[i + 1];
					m_pDist[i] = m_pDist[i + 1];
					m_pMaxDist[i] = m_pMaxDist[i + 1];
					m_pSpeed[i] = m_pSpeed[i + 1];
					m_pRotationMatrix[i] = m_pRotationMatrix[i + 1];
				}
				m_ParticleDesc.iNumInstance--;
				m_iMaxNumInstance--;
			}
		}

		vOldPos.x = vPos.x;
		vOldPos.y = vPos.y;
		vOldPos.z = vPos.z;

		((VTXMATRIX*)SubResource.pData)[i].vPosition = vOldPos;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Reset_Update(_float3 vDir)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;
	m_ParticleDesc.vDir = vDir;
	m_fTimeAcc = 0.f;

	if (m_ParticleDesc.bDelay)
	{
		m_ParticleDesc.iNumInstance = 0;
		m_iMaxNumInstance = m_iOriMaxNumInstance;
		m_fDelayAcc = 0.f;
	}
	else
	{
		m_ParticleDesc.iNumInstance = m_iOriMaxNumInstance;
		m_iMaxNumInstance = m_iOriMaxNumInstance;
	}

	_float4 vPos = { 0.f,0.f,0.f,1.f };

	m_vCurColor = m_ParticleDesc.vStartColor;

	for (_int i = 0; i < m_iOriMaxNumInstance; ++i)
	{
		m_pMaxDist[i] = (rand() % ((_int)(m_ParticleDesc.fMaxDist * 100.f - m_ParticleDesc.fMinDist * 100.f) + 1) + m_ParticleDesc.fMinDist * 100.f) / 100.f;
		m_pDist[i] = 0.f;

		m_pSpeed[i] = rand() % (_int)(m_ParticleDesc.fMaxSpeed * 100 - m_ParticleDesc.fMinSpeed * 100 + 1) + m_ParticleDesc.fMinSpeed * 100;
		m_pSpeed[i] /= 100.f;

		m_pStartScale[i].x = (rand() % ((_int)(m_ParticleDesc.vMaxScale.x * 100.f - m_ParticleDesc.vMinScale.x * 100.f) + 1) + m_ParticleDesc.vMinScale.x * 100.f) / 100.f;
		m_pStartScale[i].y = (rand() % ((_int)(m_ParticleDesc.vMaxScale.y * 100.f - m_ParticleDesc.vMinScale.y * 100.f) + 1) + m_ParticleDesc.vMinScale.y * 100.f) / 100.f;
		m_pStartScale[i].z = (rand() % ((_int)(m_ParticleDesc.vMaxScale.z * 100.f - m_ParticleDesc.vMinScale.z * 100.f) + 1) + m_ParticleDesc.vMinScale.z * 100.f) / 100.f;
		m_pStartScale[i] = m_pStartScale[i];
		m_pCurScale[i] = m_pStartScale[i];

		_vector	vDefaultRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		_vector	vDefaultUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_matrix Matrix = XMMatrixIdentity();
		_float AngleX = rand() % ((_int)(m_ParticleDesc.vMaxAngle.x - m_ParticleDesc.vMinAngle.x) + 1) + m_ParticleDesc.vMinAngle.x;
		_float AngleY = rand() % ((_int)(m_ParticleDesc.vMaxAngle.y - m_ParticleDesc.vMinAngle.y) + 1) + m_ParticleDesc.vMinAngle.y;
		m_pAngle[i].x = AngleX;
		m_pAngle[i].y = AngleY;
		Matrix *= XMMatrixRotationX(XMConvertToRadians(AngleX));
		Matrix *= XMMatrixRotationY(XMConvertToRadians(AngleY));
		XMStoreFloat4x4(&m_pRotationMatrix[i], Matrix);

		_float3 vRight = _float3(1.f, 0.f, 0.f);
		_float3 vUp = _float3(0.f, 1.f, 0.f);
		_float3 vLook;

		XMStoreFloat3(&vLook, XMVector3Normalize(XMLoadFloat3(&m_ParticleDesc.vDir)));

		XMStoreFloat3(&vLook, XMVector3TransformNormal(XMLoadFloat3(&vLook), XMLoadFloat4x4(&m_pRotationMatrix[i])));
		XMStoreFloat3(&vRight, XMVector3Cross(XMLoadFloat3(&vUp), XMVector3Normalize(XMLoadFloat3(&vLook))));
		XMStoreFloat3(&vUp, XMVector3Cross(XMLoadFloat3(&vLook), XMLoadFloat3(&vRight)));

		XMStoreFloat3(&vRight, XMLoadFloat3(&vRight) * m_pStartScale[i].x);
		XMStoreFloat3(&vUp, XMLoadFloat3(&vUp) * m_pStartScale[i].y);
		XMStoreFloat3(&vLook, XMLoadFloat3(&vLook) * m_pStartScale[i].z);

		((VTXMATRIX*)SubResource.pData)[i].vRight = _float4(vRight.x, vRight.y, vRight.z, 0.f);
		((VTXMATRIX*)SubResource.pData)[i].vUp = _float4(vUp.x, vUp.y, vUp.z, 0.f);
		((VTXMATRIX*)SubResource.pData)[i].vLook = _float4(vLook.x, vLook.y, vLook.z, 0.f);

		if (!m_ParticleDesc.bCircle)
		{
			m_pOriPos[i].x = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.x * 100.f - m_ParticleDesc.vMinPosition.x * 100.f) + 1) + m_ParticleDesc.vMinPosition.x * 100.f) / 100.f;
			m_pOriPos[i].y = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.y * 100.f - m_ParticleDesc.vMinPosition.y * 100.f) + 1) + m_ParticleDesc.vMinPosition.y * 100.f) / 100.f;
			m_pOriPos[i].z = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.z * 100.f - m_ParticleDesc.vMinPosition.z * 100.f) + 1) + m_ParticleDesc.vMinPosition.z * 100.f) / 100.f;
		}
		else
		{
			m_pOriPos[i].x = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.x * 100.f - m_ParticleDesc.vMinPosition.x * 100.f) + 1) + m_ParticleDesc.vMinPosition.x * 100.f) / 100.f;
			m_pOriPos[i].y = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.y * 100.f - m_ParticleDesc.vMinPosition.y * 100.f) + 1) + m_ParticleDesc.vMinPosition.y * 100.f) / 100.f;
			m_pOriPos[i].z = (rand() % ((_int)(m_ParticleDesc.vMaxPosition.z * 100.f - m_ParticleDesc.vMinPosition.z * 100.f) + 1) + m_ParticleDesc.vMinPosition.z * 100.f) / 100.f;

			XMStoreFloat3(&m_pOriPos[i], XMVector3Normalize(XMLoadFloat3(&vLook)) * XMVectorGetX(XMLoadFloat3(&m_pOriPos[i])));
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition = _float4(m_pOriPos[i].x, m_pOriPos[i].y, m_pOriPos[i].z, 1.f);
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Set_Desc(ParticleNum eID, void * pArg)
{
	switch (eID)
	{
	case Engine::CVIBuffer_Particle::ID_CIRCLE:
		memcpy(&m_ParticleDesc.bCircle, pArg, sizeof(_bool));
		break;
	case Engine::CVIBuffer_Particle::ID_DELAY:
		memcpy(&m_ParticleDesc.bDelay, pArg, sizeof(_bool));
		break;
	case Engine::CVIBuffer_Particle::ID_DELAY_TIME:
		memcpy(&m_ParticleDesc.fDelayTime, pArg, sizeof(_float));
		break;
	case Engine::CVIBuffer_Particle::ID_DELAY_PARTICLE:
		memcpy(&m_ParticleDesc.iDelayParticle, pArg, sizeof(_int));
		break;
	case Engine::CVIBuffer_Particle::ID_DIR:
		memcpy(&m_ParticleDesc.vDir, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_MINSPEED:
		memcpy(&m_ParticleDesc.fMinSpeed, pArg, sizeof(_float));
		break;
	case Engine::CVIBuffer_Particle::ID_MAXSPEED:
		memcpy(&m_ParticleDesc.fMaxSpeed, pArg, sizeof(_float));
		break;
	case Engine::CVIBuffer_Particle::ID_MINANGLE:
		memcpy(&m_ParticleDesc.vMinAngle, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_MAXANGLE:
		memcpy(&m_ParticleDesc.vMaxAngle, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_MINDIST:
		memcpy(&m_ParticleDesc.fMinDist, pArg, sizeof(_float));
		break;
	case Engine::CVIBuffer_Particle::ID_MAXDIST:
		memcpy(&m_ParticleDesc.fMaxDist, pArg, sizeof(_float));
		break;
	case Engine::CVIBuffer_Particle::ID_MINSCALE:
		memcpy(&m_ParticleDesc.vMinScale, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_MAXSCALE:
		memcpy(&m_ParticleDesc.vMaxScale, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_ENDSCALE:
		memcpy(&m_ParticleDesc.vEndScale, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_STARTCOLOR:
		memcpy(&m_ParticleDesc.vStartColor, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_ENDCOLOR:
		memcpy(&m_ParticleDesc.vEndColor, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_MINPOSITION:
		memcpy(&m_ParticleDesc.vMinPosition, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_MAXPOSITION:
		memcpy(&m_ParticleDesc.vMaxPosition, pArg, sizeof(_float3));
		break;
	case Engine::CVIBuffer_Particle::ID_NUMINSTANCE:
		memcpy(&m_ParticleDesc.iNumInstance, pArg, sizeof(_int));
		m_iOriMaxNumInstance = m_ParticleDesc.iNumInstance;
		m_iOriMaxNumInstance = m_ParticleDesc.iNumInstance;

		if (m_iSettimeMaxNumInst < (_uint)m_ParticleDesc.iNumInstance)
		{
			m_ParticleDesc.iNumInstance = m_iSettimeMaxNumInst;
			m_iOriMaxNumInstance = m_iSettimeMaxNumInst;
			m_iMaxNumInstance = m_iSettimeMaxNumInst;
		}
		else if (0 > m_ParticleDesc.iNumInstance)
		{
			m_ParticleDesc.iNumInstance = 1;
			m_iOriMaxNumInstance = 1;
			m_iMaxNumInstance = 1;
		}
		break;
	case Engine::CVIBuffer_Particle::ID_PARTICLE_POWER:
		memcpy(&m_ParticleDesc.fParticlePower, pArg, sizeof(_float));
		break;
	case Engine::CVIBuffer_Particle::ID_GRAVITY_POWER:
		memcpy(&m_ParticleDesc.fGravityPower, pArg, sizeof(_float));
		break;
	case Engine::CVIBuffer_Particle::ID_GRAVITY:
		memcpy(&m_ParticleDesc.bGravuty, pArg, sizeof(_bool));
		break;
	case Engine::CVIBuffer_Particle::ID_LOOP:
		memcpy(&m_ParticleDesc.bLoop, pArg, sizeof(_bool));
		break;
	case Engine::CVIBuffer_Particle::ID_END:
		break;
	default:
		break;
	}

}

HRESULT CVIBuffer_Particle::SetUp_Shader_Color(CShader * pShader, const char* ShaderTag)
{
	if (FAILED(pShader->SetRawValue(ShaderTag, &m_vCurColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Particle * CVIBuffer_Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const ParticleDesc& ParticleDesc)
{
	CVIBuffer_Particle*		pInstance = new CVIBuffer_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(ParticleDesc)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Particle::Clone(void * pArg)
{
	CVIBuffer_Particle*		pInstance = new CVIBuffer_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pOriPos);
	Safe_Delete_Array(m_pStartScale);
	Safe_Delete_Array(m_pCurScale);
	Safe_Delete_Array(m_pEndScale);
	Safe_Delete_Array(m_pAngle);

	Safe_Delete_Array(m_pDist);
	Safe_Delete_Array(m_pMaxDist);
	Safe_Delete_Array(m_pSpeed);
	Safe_Delete_Array(m_pRotationMatrix);

	Safe_Release(m_pVBInstance);
}
