#include "..\Public\Calculator.h"

#include "GameInstance.h"

CCalculator::CCalculator(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCalculator::CCalculator(const CCalculator & rhs)
	: CComponent(rhs)
	, m_vMousePicking_Positon(rhs.m_vMousePicking_Positon)
{
}

HRESULT CCalculator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCalculator::Initialize(void * pArg)
{
	return S_OK;
}

_float3 CCalculator::Picking_OnTerrain(
	HWND hWnd,
	CVIBuffer_Terrain* pVerticesTerrainCom,
	CTransform* pTransformCom
)
{
	/* 1. 마우스 좌표를 내린다. */
	/* 2. 피킹을 비교할 오브젝트 */
	/* 3. 오브젝트와 마우스의 피킹 판단 */

#pragma region MOUSEPOSITION_INVERSE

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		Vp_MousePosition;

	_uint		iViewPortNum;

	/* RS : 레스터 라이저 */
	/* nullptr 을 넣으면 뷰포트의 갯수를 얻어옴 */
	m_pContext->RSGetViewports(&iViewPortNum, nullptr);

	D3D11_VIEWPORT*		pViewPort;
	/* 얻어온 뷰포트 갯수 만큼 동적 할당 */
	pViewPort = new D3D11_VIEWPORT[iViewPortNum];

	m_pContext->RSGetViewports(&iViewPortNum, pViewPort);

	/* 보통 뷰포트는 하나여서 처음것을 가져와 줌 */
	_float fvpWidth = pViewPort[0].Width;
	_float fvpHeight = pViewPort[0].Height;

	/* 뷰 포트 -> 투영 */
	Vp_MousePosition.x = ptMouse.x / (fvpWidth * 0.5f) - 1.f;
	Vp_MousePosition.y = ptMouse.y / -(fvpHeight * 0.5f) + 1.f;
	Vp_MousePosition.z = 0.f;
	Vp_MousePosition.w = 1.f;

	Safe_Delete_Array(pViewPort);

	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 투영 -> 뷰 */
	_matrix			Inverse_ProjMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_PROJ);
	/* XMVECTOR 자료형을 사용하면 포인터 사용불가함 -> Coord , Normal 함수 사용시 반환값을 받아줘야 함 or XMFLOAT4 자료형 사용후 연산 시에만 XMVECTOR 로 변경 */
	_vector View_MousePosition = XMVector3TransformCoord(XMLoadFloat4(&Vp_MousePosition), Inverse_ProjMatrix);

	/* 마우스 광선 */
	_vector		View_RayDir, View_RayPos;
	View_RayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f); /* 뷰 스페이스 공간 원점 */
	View_RayDir = View_MousePosition - View_RayPos; /* 원점 -> 마우스 위치 로 쏘는 광선 */

	/* 뷰 -> 월드 */
	_matrix			Inverse_ViewMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);

	_vector World_RayPos, World_RayDir;
	World_RayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	/* 위치 벡터 */
	World_RayPos = XMVector3TransformCoord(View_RayPos, Inverse_ViewMatrix);
	/* 방향 벡터 */
	World_RayDir = XMVector3TransformNormal(View_RayDir, Inverse_ViewMatrix);

	/* 월드 -> 로컬 */
	/*_matrix			Inverse_WorldMatrix = pTransformCom->Get_Inverse_WorldMatrix_XM();
	XMVector3TransformCoord(vRayPos, Inverse_WorldMatrix);
	XMVector3TransformNormal(vRayDir, Inverse_WorldMatrix);*/

	/* 월드가 항등 일때 로컬까지 내려도 되지만 굳이 내릴 이유가 없다. */

	/* 방향 벡터 정규화 -> 로컬 상태의 마우스 광선이 완성됨 */
	_vector World_RayDir_Noramlize = XMVector3Normalize(World_RayDir);

	Safe_Release(pGameInstance);

#pragma endregion MOUSEPOSITION_INVERSE

#pragma region INTERSECTS_TERRAIN_MOUSE
	/* CVIBuffer_Terrain 으로 부터 X , Z 의 정점 개수르 얻어온다 */
	_ulong dwVerticesNumX = pVerticesTerrainCom->GetTerrainSize().mX;
	_ulong dwVerticesNumZ = pVerticesTerrainCom->GetTerrainSize().mZ;
	/* CVIBuffer_Terrain 각 정점의 위치 정보를 저장한 동적배열을 얻어온다 */
	_float3* pVerticesPosition = pVerticesTerrainCom->GetVertexPosition();

	/* 그떄 그때 삼각형을 그린 정점의 순서를 담아줄 매열 */
	_long dwVerticesIndex[3]{};

	_float fDist = 0.f;

	/* 정점의 개수만큼 */
	for (_ulong i = 0; i < dwVerticesNumZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVerticesNumX - 1; ++j)
		{
			/* 삼각형의 정점 순서 */
			_ulong dwIndex = i * dwVerticesNumX + j;

			/* 오른쪽 위 삼각형 */
			dwVerticesIndex[0] = dwIndex + dwVerticesNumX; /* 좌 상 */
			dwVerticesIndex[1] = dwIndex + dwVerticesNumX + 1; /* 우 상 */
			dwVerticesIndex[2] = dwIndex + 1; /* 우 하 */

											  /* 마우스 광선과 삼각형의 충돌 비교 */
			if (TriangleTests::Intersects(
				World_RayPos, /* 광선 원점 */
				World_RayDir_Noramlize, /* 광선 방향 */
										/* 삼각형을 그릴때 이용된 정점의 위치 -> 인자가 XMVECTOR 라 XMVectorSEt() 을 통해 값 전달 w값은 1 */
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[1]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[2]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[0]]),
				fDist /* 마우스 광선의 위치로 부터 피킹된 곳 까지의 거리 */
			))
			{
				/* 최종 피킹된 지점의 위치 */
				XMStoreFloat3(&m_vMousePicking_Positon, World_RayPos + World_RayDir_Noramlize * fDist);
				return m_vMousePicking_Positon;
			}

			/* 왼쪽 아래 삼각형 */
			dwVerticesIndex[0] = dwIndex + dwVerticesNumX; /* 좌 상 */
			dwVerticesIndex[1] = dwIndex + 1; /* 우 하 */
			dwVerticesIndex[2] = dwIndex; /* 좌 하 */

										  /* 마우스 광선과 삼각형의 충돌 비교 */
			if (TriangleTests::Intersects(
				World_RayPos, /* 광선 원점 */
				World_RayDir_Noramlize, /* 광선 방향 */
										/* 삼각형을 그릴때 이용된 정점의 위치 -> 인자가 XMVECTOR 라 XMVectorSEt() 을 통해 값 전달 w값은 1 */
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[2]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[0]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[1]]),
				fDist /* 마우스 광선의 위치로 부터 피킹된 곳 까지의 거리 */
			))
			{
				/* 최종 피킹된 지점의 위치 */
				XMStoreFloat3(&m_vMousePicking_Positon, World_RayPos + World_RayDir_Noramlize * fDist);
				return m_vMousePicking_Positon;
			}
		}
	}

#pragma endregion INTERSECTS_TERRAIN_MOUSE

	return m_vMousePicking_Positon;
}

CCalculator * CCalculator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCalculator*			pInstance = new CCalculator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCalculator::Clone(void * pArg)
{
	CCalculator*			pInstance = new CCalculator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalculator::Free()
{
	__super::Free();
}
