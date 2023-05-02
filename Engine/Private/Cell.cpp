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

HRESULT CCell::Initialize(_int iIndex, const _float3 * pPoints, _int iState)
{
	m_iIndex = iIndex;

	if (0 > iState || END_CELL <= iState)
		iState = 0;
	m_iState = iState;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	/* B - A / A->B */
	_float A_B_LineX = pPoints[POINT_B].x - pPoints[POINT_A].x;
	_float A_B_LineZ = pPoints[POINT_B].z - pPoints[POINT_A].z;

	/* C - B / B->C */
	_float B_C_LineX = pPoints[POINT_C].x - pPoints[POINT_B].x;
	_float B_C_LineZ = pPoints[POINT_C].z - pPoints[POINT_B].z;

	/* A - C / C->A */
	_float C_A_LineX = pPoints[POINT_A].x - pPoints[POINT_C].x;
	_float C_A_LineZ = pPoints[POINT_A].z - pPoints[POINT_C].z;

	/* 기준이 되는 방향벡터 에서 스왑을 하고 앞에 -1 을 곱해주면 해당 방향벡터와 수직인 벡터를 구할수 있음 */
	/* 각 점으로 부터 플레이어 까지의 방향벡터를 이렇게 구한 Normal Vector 와 내적하여 각 축으로 부터 90도를 넘지않으면 삼각형 밖 , 90도를 넘으면 삼각형 안에 있다 */
	/* cos 그래프 : 0 ~ 90 양수 , 90 ~ 270 음수 , 270 ~ 360 양수 -> 음수면 삼각형 안에있다 , 양수면 밖에 있다 */
	/* 삼각형 (네비메쉬) 을 나갔냐 안나갔냐 를 판단하기 때문에 y를 구하지 않아도 괜찮다 */
	m_vNormals[LINE_AB] = _float3(A_B_LineZ * -1.f, 0.f, A_B_LineX);
	m_vNormals[LINE_BC] = _float3(B_C_LineZ * -1.f, 0.f, B_C_LineX);
	m_vNormals[LINE_CA] = _float3(C_A_LineZ * -1.f, 0.f, C_A_LineX);

	for (_uint i = 0; i < LINE_END; ++i)
		XMStoreFloat3(&m_vNormals[i], XMVector3Normalize(XMLoadFloat3(&m_vNormals[i])));

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
	/* 같은점을 판단해주는 함수 -> 이 함수에서 같은점인 조건이 true로 반환되면 같은점들을 이웃으로 등록해 준다 */

	/* _float3 나 XMVector 는 == 연산 오버로딩이 되어 있지 않음 대신 XMVector 는 함수를 제공 XMVector3Equal() - 반환이 bool */

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

_bool CCell::Is_In(_fvector vPosition, _int * pNeigborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		/* Normalize 를 할 때 w에 대해 명확하지 않으면 vector3 로 한다 */
		/* Cell 의 (3개) 각 꼭짓점 으로 부터 객체로 향하는 방향벡터를 구한다 */
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		/* 꼭짓점으로 부터 객체를 향한 방향벡터 , 미리 구해놓은 선분의 Normal벡터 를 내적한다 */
		/* -> 모든 결과가 양수라면 Cell 밖으로 나간 것 ,음수라면 Cell 안에 있는 것 -> Cos(세타) 결과 값 */
		/* 내적함수 XMVector3Dot()도 vector를 리턴함으로 XMVectorGetX() X값만 가져온다 */
		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i]))))
		{
			/* Cell의 밖으로 나갔다면 이웃을 갱신하고 false 리턴 */
			*pNeigborIndex = m_Neighbors[i];
			return false;
		}
	}

	return true;
}

_bool CCell::Is_CurrentIn(_fvector vPosition)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		/* Normalize 를 할 때 w에 대해 명확하지 않으면 vector3 로 한다 */
		/* Cell 의 (3개) 각 꼭짓점 으로 부터 객체로 향하는 방향벡터를 구한다 */
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

		/* 꼭짓점으로 부터 객체를 향한 방향벡터 , 미리 구해놓은 선분의 Normal벡터 를 내적한다 */
		/* -> 모든 결과가 양수라면 Cell 밖으로 나간 것 ,음수라면 Cell 안에 있는 것 -> Cos(세타) 결과 값 */
		/* 내적함수 XMVector3Dot()도 vector를 리턴함으로 XMVectorGetX() X값만 가져온다 */
		if (0 < XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(&m_vNormals[i]))))
		{
			/* Cell의 밖으로 나갔다면 이웃을 갱신하고 false 리턴 */
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
void CCell::Render(CShader * pShader, _float4 vColor)
{
	/* 쉐이더전역으로 색을 던진다 */
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
