#pragma once

#include "Base.h"

BEGIN(Engine)

/* ���� ū ������ ǥ���ϴ� �ϳ��� ��� */
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

	/* �ø��� �ϴ� ������ �����迭 �ּҸ� �޾ƿ´� (�ε����� �����س���) */
	void Culling(
		class CFrustum* pFrustum,
		const _float3* pVerticesPos,
		FACEINDICES32* pIndices,
		_uint* pNumFaces
	);

	void SetUp_Neighbors();

private:
	/* ������ 4���� ���� Index */
	_uint			m_iCorners[CORNER_END] = { 0 };
	_uint			m_iCenter = { 0 };

	/* (���� ���) �θ��� �ڽ� ��� (Initialize ���� �ڽĵ��� ��ͻ����ϴ� ���·� ������̴�) */
	/* ������ 4�����Ͽ� ���¸� 1���� �ڽ� 1�� (�»�, ���, ����, ����) */
	CQuadTree*		m_pChilds[CHILD_END] = { nullptr };

	/* LOD ����� ũ�������� ������� �̿��� �����Ѵ� */
	/* �� ���� Ʈ���� ���� ũ���� ���� Ʈ���� �̿����� ����� ���� */
	CQuadTree*		m_pNeighbors[NEIGHBOR_END] = { nullptr };

private:
	_bool			Is_Draw(const _float3* pVerticesPos);

public:
	/* ���� �� ������ 4���� ������ �����´� (1 , 4 �� ���� ������ ����, �� ������ ������ 2���� ���� ������ �ڽ��� ������ ����) */
	/* ������ ��ġ vector �� �ƴ� Index �� �޾ƿ´� -> ���� �� �۾��� �� ��, ������ �������� �����ϴ� �迭�� �ּҸ� �������� Index �� ��ġ������ ������ �� �ִ� */
	static CQuadTree* Create(
		_uint iLT,
		_uint iRT,
		_uint iRB,
		_uint iLB
	);
	virtual void Free() override;
};

END