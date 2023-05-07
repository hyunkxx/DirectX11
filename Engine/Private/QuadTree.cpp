#include "..\Public\QuadTree.h"

#include "Frustum.h"
#include "GameInstance.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	/* 한쪽 끝에서 끝을 뺀 껼과가 1이면 더이상 분열할수 없음. -> 함수를 탈출한다 - 자식이 생성되기 전에 (이 시점에 자식이 생성되면 에러발생) */
	if (1 == iRT - iLT)
		return S_OK;

	/* (왼쪽 상단)처음 정점 - (오른쪽 하단)마지막 정점 / 2 = 중앙 정점 */
	m_iCenter = (iLT + iRB) >> 1;

	/* 각 선분의 중점 */
	_uint		iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	/* 자식들을 계속 재귀 생성하는 형태 */

	m_pChilds[CHILD_LT] = CQuadTree::Create(iLT, iTC, m_iCenter, iLC);
	m_pChilds[CHILD_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenter);
	m_pChilds[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, iRB, iBC);
	m_pChilds[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, iLB);

	return S_OK;
}

void CQuadTree::Culling(CFrustum * pFrustum, const _float3 * pVerticesPos, FACEINDICES32* pIndices, _uint * pNumFaces)
{
	// VIBufferTerrain 에게서 정점 정보를 받아온다 -> 버퍼 단계임으로 로컬 스페이스 상태이다 -> CFrustum 를 가져와 로컬 충돌 비교를 한다

	if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]
		|| true == Is_Draw(pVerticesPos))
	{
		/* Is_Draw 를 true 로 초기화 시켜줘야 함 -> false 면 쿼드 트리를 쪼개서 그림 */
		_bool		Is_Draw[NEIGHBOR_END] = { true, true, true, true };

		/* 각 이웃에 대해 Is_Draw 검사 결과를 담는다 */
		if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
			Is_Draw[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->Is_Draw(pVerticesPos);

		if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
			Is_Draw[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->Is_Draw(pVerticesPos);

		if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
			Is_Draw[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->Is_Draw(pVerticesPos);

		if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
			Is_Draw[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->Is_Draw(pVerticesPos);


		// 쿼드 트리 로 분할한 지역중 하나의 4 꼭지점이 충돌하는지 판단

		// 여부분에 Ration 를 줘서 거리 관리?
		_bool		isIn[] = {
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]), 0.0f),
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]), 0.0f),
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RB]]), 0.0f),
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LB]]), 0.0f),
		};

		/* 이웃이 모두 쪼개지지 않는경우 */
		if (true == Is_Draw[NEIGHBOR_LEFT]
			&& true == Is_Draw[NEIGHBOR_TOP]
			&& true == Is_Draw[NEIGHBOR_RIGHT]
			&& true == Is_Draw[NEIGHBOR_BOTTOM])
		{

			// 충돌에 해당한 삼각형을 그려준다 -> 그리고 난뒤 인덱스버퍼에 값을채워주고 삼각형 갯수 증가

			/* 오른쪽 위 삼각형 */
			if (true == isIn[0]
				|| true == isIn[1]
				|| true == isIn[2])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}

			/* 왼쪽 아래 삼각형 */
			if (true == isIn[0]
				|| true == isIn[2]
				|| true == isIn[3])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}
			// 어떠한 삼각형에도 충돌하지 않음 -> 탈출 (쿼드트리로 분할한 지역의 지형중 충돌하지 않은 지형은 제외시킨다)
			return;
		}

		/* 이웃이 쪼개지는 경우 */
		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		/* 자식의 삼각형을 다시 그릴때도 꼭 시계방향으로 그려줘야 함 */
		/* 현재 쿼드 트리의 왼쪽 아래 삼각형이 그려지기 전 좌 , 하 이웃이 분열을 했는지 판단 */
		if (true == isIn[0]
			|| true == isIn[2]
			|| true == isIn[3])
		{
			/* 현재 쿼드 트리의 ( 4개로 나눈 자식들 중 ) 왼쪽 이웃이 쪼개진 자식이 있다면 */
			if (false == Is_Draw[NEIGHBOR_LEFT])
			{
				/* 해당 자식의 삼각형을 다시 그린다. -> 이웃이 아닌 자신의 왼쪽 이웃과 선분을 공유하는 삼각형을 2개 그린다 */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = iLC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iLC;
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}

			/* 왼쪽 이웃이 쪼개지지 않았다면 */
			else
			{
				/* 해당 자식의 삼각형을 다시 그린다. -> 해당 자식의 왼쪽 이웃과 선분을 공유하는 삼각형을 1개 그린다 */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}

			/* 현재 쿼드 트리의 ( 4개로 나눈 자식들 중 ) 아래 이웃이 쪼개진 자식이 있다면 */
			if (false == Is_Draw[NEIGHBOR_BOTTOM])
			{
				/* 이웃이 아닌 (해당 자식)자신의 아래 이웃과 선분을 공유하는 삼각형을 2개 그린다 */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = iBC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iBC;
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}

			/* 아래 이웃이 쪼개지지 않았다면 */
			else
			{
				/* 이웃이 아닌 (해당 자식)자신의 아래 이웃과 선분을 공유하는 삼각형을 1개 그린다  */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
		}

		/* 현재 쿼드 트리의 오른쪽 위 삼각형 이 그려지기 전 우 , 상 이웃이 분열하는지 판단 */
		if (true == isIn[0]
			|| true == isIn[1]
			|| true == isIn[2])
		{
			/* 현재 쿼드 트리의 ( 4개로 나눈 자식들 중 ) 위쪽 이웃이 쪼개진 자식이 있다면 */
			if (false == Is_Draw[NEIGHBOR_TOP])
			{
				/* 이웃이 아닌 (해당 자식)자신의 위쪽 이웃과 선분을 공유하는 삼각형을 2개 그린다 */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenter;
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RT];
				++*pNumFaces;
			}

			/* 위쪽 이웃이 쪼개지지 않았다면 */
			else
			{
				/* 이웃이 아닌 (해당 자식)자신의 위쪽 이웃과 선분을 공유하는 삼각형을 1개 그린다 */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;
			}

			/* 현재 쿼드 트리의 ( 4개로 나눈 자식들 중 ) 오른쪽 이웃이 쪼개진 자식이 있다면 */
			if (false == Is_Draw[NEIGHBOR_RIGHT])
			{
				/* 이웃이 아닌 (해당 자식)자신의 오른쪽 이웃과 선분을 공유하는 삼각형을 2개 그린다 */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenter;
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}

			/* 오른쪽 이웃이 쪼개지지 않았다면 */
			else
			{
				/* 이웃이 아닌 (해당 자식)자신의 오른쪽 이웃과 선분을 공유하는 삼각형을 1개 그린다  */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;
			}
		}

		/* 이웃에 맞춰 삼각형들을 다 그렸으면 더이상 자식에대해 그릴필요 없음으로 탈출 */
		return;
	}

	// 중심으로 부터 왼쪽 상단의 꼭지점 까지의 거리 (임시 원을친 반지름의 길이) 
	_float		fRadius = XMVectorGetX(
		XMVector3Length(
			XMLoadFloat3(&pVerticesPos[m_iCenter]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]))
	);

	// 여부분에는 추가 Ration 를 줘서 거리 관리?

	// 절두체가 (지형) 임시로 만든 원과 충돌 하였으면 -> 자식들의 충돌을 비교해야함 -> 자식의 Culling() 함수를 돌려준다
	// (이때 자식은 해당 범위의 지형을 4등분 한 것) -> 범위 안에 지형을 4등분하여 다시 충돌 비교 ( 함수 재귀 호출 )
	if (true == pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		for (_uint i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChilds[i])
				m_pChilds[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumFaces);
		}
	}

}

void CQuadTree::SetUp_Neighbors()
{
	/* 내 쿼드트리의 이웃이 아닌 내 자식의 이웃을 세팅한다 -> 내 쿼드 트리를 더 쪼개서 그려야 하는지 판단하기 위해 */

	/* LT 에 있는 자식의 자식이 생성되지 않았으면 탈출 -> 자식이 nullptr == 자식이 없다 -> 쪼개지지 않는다 */
	if (nullptr == m_pChilds[CHILD_LT]->m_pChilds[CHILD_LT])
		return;

	/* 각 자식들의 방향에 있는 자식을 이웃으로 설정 */

	/* LT */
	m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RT];
	m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_LB];

	/* RT */
	m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CHILD_LT];
	m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_RB];

	/* RB */
	m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CHILD_LB];
	m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CHILD_RT];

	/* LB */
	m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RB];
	m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CHILD_LT];


	/* 부모로 부터 자식이 쪼개졌을 때 , 자식들중 이웃이 비는 애들이 생긴다 */
	/* -> 부모끼리는 이웃 관계가 형성되었지만, 해당 부모의 자식의 이웃중 세팅이 안된 곳들이 생김 */
	/* -> 자식의 세팅이 안된 이웃을 세팅해준다 */

	/* 오른쪽 이웃이 있는 부모의 자식이라면 */
	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		/* 자식 RT 의 비어 있는 오른쪽 이웃을 , 부모의 오른쪽 이웃의 자식 LT 로 설정해준다 */
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LT];
		/* 자식 RB 의 비어 있는 오른쪽 이웃을 , 부모의 오른쪽 이웃의 자식 LB 로 설정해준다 */
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LB];
	}

	/* 아래쪽 이웃이 있는 부모의 자식이라면 */
	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		/* 자식 LB 의 비어 있는 아래쪽 이웃을 , 부모의 아래쪽 이웃의 자식 LT 로 설정해준다 */
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_LT];
		/* 자식 RB 의 비어 있는 아래쪽이웃을 , 부모의 아래쪽 이웃의 자식 RT 로 설정해준다 */
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_RT];
	}

	/* 왼쪽 이웃이 있는 부모의 자식이라면 */
	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		/* 자식 LT 의 비어 있는 왼쪽 이웃을 , 부모의 왼쪽 이웃의 자식 RT 로 설정해준다 */
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RT];
		/* 자식 LB 의 비어 있는 왼쪽 이웃을 , 부모의 왼쪽 이웃의 자식 RB 로 설정해준다 */
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RB];
	}

	/* 위쪽 이웃이 있는 부모의 자식이라면 */
	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		/* 자식 LT 의 비어 있는 위쪽 이웃을 , 부모의 위쪽 이웃의 자식 LB 로 설정해준다 */
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_LB];
		/* 자식 RT 의 비어 있는 위쪽 이웃을 , 부모의 위쪽 이웃의 자식 RB 로 설정해준다 */
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_RB];
	}

	/* 세팅한 자식의 자식을 세팅해준다 (재귀호출) -> 없으면 함수 첫줄의 return 으로 탈출 */
	for (_uint i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChilds[i])
			m_pChilds[i]->SetUp_Neighbors();
	}
}

_bool CQuadTree::Is_Draw(const _float3 * pVerticesPos)
{
	/* LOD 기법을 위한 IsDraw */
	/* LEVEL OF DETAIL : LEVLE = 거리 */

	/* 정점의 높이가 선형으로 존재하지 않다면 크렉이 생긴다. */
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return false;

	/* 카메라와 지형의 거리에 따라 디테일을 수정한다 -> 카메라 위치정보를 가져옴 */
	_float4 vCamPosition = pGameInstance->Get_CamPosition();


	/* 카메라와 지형의 중점의 거리 */
	_float	fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCenter]) - XMLoadFloat4(&vCamPosition)));

	/* 가장 큰 쿼트트리의 가로넓이 */
	_float	fWidth = XMVectorGetX(
		XMVector3Length(
			XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])
		));

	/* LOD 의 조건을 빡세게 줘야 한다 -> 아니면 통과하는 상황이 자주생겨서 세세한 분할이 이루어 지지 않음 */
	if (fDistance * 0.30f > fWidth)
		return true;

	return false;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Create : CQuadTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	for (_uint i = 0; i < CHILD_END; ++i)
		Safe_Release(m_pChilds[i]);
}
