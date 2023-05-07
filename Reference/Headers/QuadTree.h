#pragma once

#include "Base.h"

BEGIN(Engine)

/* 가장 큰 지형을 표현하는 하나의 노드 */
class CQuadTree final : public CBase
{
public:
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB, CORNER_END };
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_RB, CHILD_LB, CHILD_END };

	enum NEIGHBOR {
		NEIGHBOR_LEFT,
		NEIGHBOR_TOP,
		NEIGHBOR_RIGHT,
		NEIGHBOR_BOTTOM,
		NEIGHBOR_END
	};

private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT Initialize(
		_uint iLT,
		_uint iRT,
		_uint iRB,
		_uint iLB
	);

	/* 컬링을 하는 시점에 정점배열 주소를 받아온다 (인덱스만 저장해놨음) */
	void Culling(
		class CFrustum* pFrustum,
		const _float3* pVerticesPos,
		FACEINDICES32* pIndices,
		_uint* pNumFaces
	);

	void SetUp_Neighbors();

private:
	/* 꼭지점 4개의 정보 Index */
	_uint			m_iCorners[CORNER_END] = { 0 };
	_uint			m_iCenter = { 0 };

	/* (현재 노드) 부모의 자식 노드 (Initialize 에서 자식들을 재귀생성하는 형태로 만들것이다) */
	/* 지형을 4분할하여 나온면 1개당 자식 1개 (좌상, 우상, 우하, 좌하) */
	CQuadTree*		m_pChilds[CHILD_END] = { nullptr };

	/* LOD 기법의 크렉현상을 잡기위해 이웃을 설정한다 */
	/* 내 쿼드 트리와 같은 크기의 쿼드 트리를 이웃으로 등록할 것임 */
	CQuadTree*		m_pNeighbors[NEIGHBOR_END] = { nullptr };

private:
	_bool			Is_Draw(const _float3* pVerticesPos);

public:
	/* 생성 시 꼭지점 4개의 정보를 가져온다 (1 , 4 를 더해 나누면 중점, 각 선분의 꼭지점 2개를 더해 나누면 자식의 꼭지점 정보) */
	/* 정점의 위치 vector 가 아닌 Index 를 받아온다 -> 실제 비교 작업읋 할 때, 정점의 정보들을 보관하는 배열의 주소를 던짐으로 Index 로 위치정보를 가져올 수 있다 */
	static CQuadTree* Create(
		_uint iLT,
		_uint iRT,
		_uint iRB,
		_uint iLB
	);
	virtual void Free() override;
};

END