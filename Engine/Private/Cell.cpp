#include "..\Public\Cell.h"

#include "VIBuffer_Cell.h"
#include "Shader.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_float CCell::DistanceToWall(_fvector vPosition)
{
	_vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&m_vPoints[POINT_C]));

	return XMVectorGetX(XMPlaneDotCoord(vPlane,	vPosition));
}

HRESULT CCell::Initialize(_int iIndex, const _float3 * pPoints, _int iState)
{
	m_iIndex = iIndex;

	if (0 > iState || END_CELL <= iState)
		iState = 0;
	m_iState = iState;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);



	// ����� �븻 ���س���
	XMStoreFloat3(&m_vPlaneNormal,
		XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]),
			XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_A]))));

	/* ������ �Ǵ� ���⺤�� ���� ������ �ϰ� �տ� -1 �� �����ָ� �ش� ���⺤�Ϳ� ������ ���͸� ���Ҽ� ���� */
	/* �� ������ ���� �÷��̾� ������ ���⺤�͸� �̷��� ���� Normal Vector �� �����Ͽ� �� ������ ���� 90���� ���������� �ﰢ�� �� , 90���� ������ �ﰢ�� �ȿ� �ִ� */
	/* cos �׷��� : 0 ~ 90 ��� , 90 ~ 270 ���� , 270 ~ 360 ��� -> ������ �ﰢ�� �ȿ��ִ� , ����� �ۿ� �ִ� */
	/* �ﰢ�� (�׺�޽�) �� ������ �ȳ����� �� �Ǵ��ϱ� ������ y�� ������ �ʾƵ� ������ */

	


	/* B - A / A->B */
	_float A_B_LineX = pPoints[POINT_B].x - pPoints[POINT_A].x;
	_float A_B_LineZ = pPoints[POINT_B].z - pPoints[POINT_A].z;

	/* C - B / B->C */
	_float B_C_LineX = pPoints[POINT_C].x - pPoints[POINT_B].x;
	_float B_C_LineZ = pPoints[POINT_C].z - pPoints[POINT_B].z;

	/* A - C / C->A */
	_float C_A_LineX = pPoints[POINT_A].x - pPoints[POINT_C].x;
	_float C_A_LineZ = pPoints[POINT_A].z - pPoints[POINT_C].z;
	m_vNormals[LINE_AB] = _float3(A_B_LineZ * -1.f, 0.f, A_B_LineX);
	m_vNormals[LINE_BC] = _float3(B_C_LineZ * -1.f, 0.f, B_C_LineX);
	m_vNormals[LINE_CA] = _float3(C_A_LineZ * -1.f, 0.f, C_A_LineX);

	for (_uint i = 0; i < LINE_END; ++i)
		XMStoreFloat3(&m_vNormals[i], XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])));

	_vector vLineAB, vLineBC, vLineCA;
	vLineAB = XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]);
	vLineBC = XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]);
	vLineCA = XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]);

	XMStoreFloat3(&m_vNormals3D[LINE_AB], XMVector3Normalize(XMVector3Cross(vLineAB, XMLoadFloat3(&m_vPlaneNormal))));
	XMStoreFloat3(&m_vNormals3D[LINE_BC], XMVector3Normalize(XMVector3Cross(vLineBC, XMLoadFloat3(&m_vPlaneNormal))));
	XMStoreFloat3(&m_vNormals3D[LINE_CA], XMVector3Normalize(XMVector3Cross(vLineCA, XMLoadFloat3(&m_vPlaneNormal))));





#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	/* �������� �Ǵ����ִ� �Լ� -> �� �Լ����� �������� ������ true�� ��ȯ�Ǹ� ���������� �̿����� ����� �ش� */

	/* _float3 �� XMVector �� == ���� �����ε��� �Ǿ� ���� ���� ��� XMVector �� �Լ��� ���� XMVector3Equal() - ��ȯ�� bool */

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDest))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSour))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDest))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDest))
			return true;
	}

	return false;
}
//
//_bool CCell::Is_In(_fvector vPosition, _int * pNeigborIndex)
//{
//	for (_uint i = 0; i < LINE_END; ++i)
//	{
//		/* Normalize �� �� �� w�� ���� ��Ȯ���� ������ vector3 �� �Ѵ� */
//		/* Cell �� (3��) �� ������ ���� ���� ��ü�� ���ϴ� ���⺤�͸� ���Ѵ� */
//		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
//
//		/* ���������� ���� ��ü�� ���� ���⺤�� , �̸� ���س��� ������ Normal���� �� �����Ѵ� */
//		/* -> ��� ����� ������ Cell ������ ���� �� ,������� Cell �ȿ� �ִ� �� -> Cos(��Ÿ) ��� �� */
//		/* �����Լ� XMVector3Dot()�� vector�� ���������� XMVectorGetX() X���� �����´� */
//		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i]))))
//		{
//			/* Cell�� ������ �����ٸ� �̿��� �����ϰ� false ���� */
//			*pNeigborIndex = m_Neighbors[i];
//			return false;
//		}
//	}
//
//	return true;
//}

_bool CCell::IsIn(_fvector vPosition, _int * pNeighborIndex, _bool bClimbing, _int * pOutLine)
{
	if (bClimbing)
	{
		// ���� Ÿ�� ���� �� ���� ��Ȯ��
		_vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2]));

		_float fDistance = XMVectorGetX(XMPlaneDotCoord(vPlane, vPosition));

		_vector vPosOnPlane = vPosition - XMLoadFloat3(&m_vPlaneNormal) * fDistance;
		
		for (_uint i = 0; i < LINE_END; ++i)
		{
			_vector		vDir = XMVector3Normalize(vPosOnPlane - XMVectorSetW(XMLoadFloat3(&m_vPoints[i]), 1.f));

			if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals3D[i]))))
			{
				*pNeighborIndex = m_Neighbors[i];
				if (nullptr != pOutLine)
					*pOutLine = i;
				return false;
			}
		}
	}
	else
	{
		for (_uint i = 0; i < LINE_END; ++i)
		{
			_vector		vDir = XMVector3Normalize(vPosition - XMVectorSetY(XMLoadFloat3(&m_vPoints[i]), 0.f));

			if (0 < XMVectorGetX(XMVector3Dot(vDir, XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])))))
			{
				*pNeighborIndex = m_Neighbors[i];
				if (nullptr != pOutLine)
					*pOutLine = i;
				return false;
			}
		}
	}

	return true;
}

_bool CCell::CrossMultiEndLine(_fvector vPosition)
{
	_uint iCrossCount = 0;
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i]))))
		{
			if (-1 == m_Neighbors[i])
				++iCrossCount;

			if (iCrossCount >= 2)
				return true;
		}
	}
	return false;
}

_bool CCell::Is_CurrentIn(_fvector vPosition)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		/* Normalize �� �� �� w�� ���� ��Ȯ���� ������ vector3 �� �Ѵ� */
		/* Cell �� (3��) �� ������ ���� ���� ��ü�� ���ϴ� ���⺤�͸� ���Ѵ� */
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		/* ���������� ���� ��ü�� ���� ���⺤�� , �̸� ���س��� ������ Normal���� �� �����Ѵ� */
		/* -> ��� ����� ������ Cell ������ ���� �� ,������� Cell �ȿ� �ִ� �� -> Cos(��Ÿ) ��� �� */
		/* �����Լ� XMVector3Dot()�� vector�� ���������� XMVectorGetX() X���� �����´� */
		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i]))))
		{
			/* Cell�� ������ �����ٸ� �̿��� �����ϰ� false ���� */
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
void CCell::Render(CShader * pShader, _float4 vColor)
{
	/* ���̴��������� ���� ������ */
	if (FAILED(pShader->SetRawValue("g_vColor", &vColor, sizeof(_float4))))
		return;

	pShader->Begin(0);

	m_pVIBuffer->Render();
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _int iIndex, const _float3 * pPoints, _int iState)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iIndex, pPoints, iState)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG
}
