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

	/* ���� ������ ���� �� ������ 1�̸� ���̻� �п��Ҽ� ����. -> �Լ��� Ż���Ѵ� - �ڽ��� �����Ǳ� ���� (�� ������ �ڽ��� �����Ǹ� �����߻�) */
	if (1 == iRT - iLT)
		return S_OK;

	/* (���� ���)ó�� ���� - (������ �ϴ�)������ ���� / 2 = �߾� ���� */
	m_iCenter = (iLT + iRB) >> 1;

	/* �� ������ ���� */
	_uint		iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	/* �ڽĵ��� ��� ��� �����ϴ� ���� */

	m_pChilds[CHILD_LT] = CQuadTree::Create(iLT, iTC, m_iCenter, iLC);
	m_pChilds[CHILD_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenter);
	m_pChilds[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, iRB, iBC);
	m_pChilds[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, iLB);

	return S_OK;
}

void CQuadTree::Culling(CFrustum * pFrustum, const _float3 * pVerticesPos, FACEINDICES32* pIndices, _uint * pNumFaces)
{
	// VIBufferTerrain ���Լ� ���� ������ �޾ƿ´� -> ���� �ܰ������� ���� �����̽� �����̴� -> CFrustum �� ������ ���� �浹 �񱳸� �Ѵ�

	if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]
		|| true == Is_Draw(pVerticesPos))
	{
		/* Is_Draw �� true �� �ʱ�ȭ ������� �� -> false �� ���� Ʈ���� �ɰ��� �׸� */
		_bool		Is_Draw[NEIGHBOR_END] = { true, true, true, true };

		/* �� �̿��� ���� Is_Draw �˻� ����� ��´� */
		if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
			Is_Draw[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->Is_Draw(pVerticesPos);

		if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
			Is_Draw[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->Is_Draw(pVerticesPos);

		if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
			Is_Draw[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->Is_Draw(pVerticesPos);

		if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
			Is_Draw[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->Is_Draw(pVerticesPos);


		// ���� Ʈ�� �� ������ ������ �ϳ��� 4 �������� �浹�ϴ��� �Ǵ�

		// ���κп� Ration �� �༭ �Ÿ� ����?
		_bool		isIn[] = {
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]), 0.0f),
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]), 0.0f),
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RB]]), 0.0f),
			pFrustum->InLocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LB]]), 0.0f),
		};

		/* �̿��� ��� �ɰ����� �ʴ°�� */
		if (true == Is_Draw[NEIGHBOR_LEFT]
			&& true == Is_Draw[NEIGHBOR_TOP]
			&& true == Is_Draw[NEIGHBOR_RIGHT]
			&& true == Is_Draw[NEIGHBOR_BOTTOM])
		{

			// �浹�� �ش��� �ﰢ���� �׷��ش� -> �׸��� ���� �ε������ۿ� ����ä���ְ� �ﰢ�� ���� ����

			/* ������ �� �ﰢ�� */
			if (true == isIn[0]
				|| true == isIn[1]
				|| true == isIn[2])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}

			/* ���� �Ʒ� �ﰢ�� */
			if (true == isIn[0]
				|| true == isIn[2]
				|| true == isIn[3])
			{
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}
			// ��� �ﰢ������ �浹���� ���� -> Ż�� (����Ʈ���� ������ ������ ������ �浹���� ���� ������ ���ܽ�Ų��)
			return;
		}

		/* �̿��� �ɰ����� ��� */
		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		/* �ڽ��� �ﰢ���� �ٽ� �׸����� �� �ð�������� �׷���� �� */
		/* ���� ���� Ʈ���� ���� �Ʒ� �ﰢ���� �׷����� �� �� , �� �̿��� �п��� �ߴ��� �Ǵ� */
		if (true == isIn[0]
			|| true == isIn[2]
			|| true == isIn[3])
		{
			/* ���� ���� Ʈ���� ( 4���� ���� �ڽĵ� �� ) ���� �̿��� �ɰ��� �ڽ��� �ִٸ� */
			if (false == Is_Draw[NEIGHBOR_LEFT])
			{
				/* �ش� �ڽ��� �ﰢ���� �ٽ� �׸���. -> �̿��� �ƴ� �ڽ��� ���� �̿��� ������ �����ϴ� �ﰢ���� 2�� �׸��� */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = iLC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iLC;
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}

			/* ���� �̿��� �ɰ����� �ʾҴٸ� */
			else
			{
				/* �ش� �ڽ��� �ﰢ���� �ٽ� �׸���. -> �ش� �ڽ��� ���� �̿��� ������ �����ϴ� �ﰢ���� 1�� �׸��� */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_LB];
				++*pNumFaces;
			}

			/* ���� ���� Ʈ���� ( 4���� ���� �ڽĵ� �� ) �Ʒ� �̿��� �ɰ��� �ڽ��� �ִٸ� */
			if (false == Is_Draw[NEIGHBOR_BOTTOM])
			{
				/* �̿��� �ƴ� (�ش� �ڽ�)�ڽ��� �Ʒ� �̿��� ������ �����ϴ� �ﰢ���� 2�� �׸��� */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = iBC;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = iBC;
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}

			/* �Ʒ� �̿��� �ɰ����� �ʾҴٸ� */
			else
			{
				/* �̿��� �ƴ� (�ش� �ڽ�)�ڽ��� �Ʒ� �̿��� ������ �����ϴ� �ﰢ���� 1�� �׸���  */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LB];
				pIndices[*pNumFaces]._1 = m_iCenter;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}
		}

		/* ���� ���� Ʈ���� ������ �� �ﰢ�� �� �׷����� �� �� , �� �̿��� �п��ϴ��� �Ǵ� */
		if (true == isIn[0]
			|| true == isIn[1]
			|| true == isIn[2])
		{
			/* ���� ���� Ʈ���� ( 4���� ���� �ڽĵ� �� ) ���� �̿��� �ɰ��� �ڽ��� �ִٸ� */
			if (false == Is_Draw[NEIGHBOR_TOP])
			{
				/* �̿��� �ƴ� (�ش� �ڽ�)�ڽ��� ���� �̿��� ������ �����ϴ� �ﰢ���� 2�� �׸��� */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenter;
				pIndices[*pNumFaces]._1 = iTC;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RT];
				++*pNumFaces;
			}

			/* ���� �̿��� �ɰ����� �ʾҴٸ� */
			else
			{
				/* �̿��� �ƴ� (�ش� �ڽ�)�ڽ��� ���� �̿��� ������ �����ϴ� �ﰢ���� 1�� �׸��� */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_LT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;
			}

			/* ���� ���� Ʈ���� ( 4���� ���� �ڽĵ� �� ) ������ �̿��� �ɰ��� �ڽ��� �ִٸ� */
			if (false == Is_Draw[NEIGHBOR_RIGHT])
			{
				/* �̿��� �ƴ� (�ش� �ڽ�)�ڽ��� ������ �̿��� ������ �����ϴ� �ﰢ���� 2�� �׸��� */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;

				pIndices[*pNumFaces]._0 = m_iCenter;
				pIndices[*pNumFaces]._1 = iRC;
				pIndices[*pNumFaces]._2 = m_iCorners[CORNER_RB];
				++*pNumFaces;
			}

			/* ������ �̿��� �ɰ����� �ʾҴٸ� */
			else
			{
				/* �̿��� �ƴ� (�ش� �ڽ�)�ڽ��� ������ �̿��� ������ �����ϴ� �ﰢ���� 1�� �׸���  */
				pIndices[*pNumFaces]._0 = m_iCorners[CORNER_RT];
				pIndices[*pNumFaces]._1 = m_iCorners[CORNER_RB];
				pIndices[*pNumFaces]._2 = m_iCenter;
				++*pNumFaces;
			}
		}

		/* �̿��� ���� �ﰢ������ �� �׷����� ���̻� �ڽĿ����� �׸��ʿ� �������� Ż�� */
		return;
	}

	// �߽����� ���� ���� ����� ������ ������ �Ÿ� (�ӽ� ����ģ �������� ����) 
	_float		fRadius = XMVectorGetX(
		XMVector3Length(
			XMLoadFloat3(&pVerticesPos[m_iCenter]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]))
	);

	// ���κп��� �߰� Ration �� �༭ �Ÿ� ����?

	// ����ü�� (����) �ӽ÷� ���� ���� �浹 �Ͽ����� -> �ڽĵ��� �浹�� ���ؾ��� -> �ڽ��� Culling() �Լ��� �����ش�
	// (�̶� �ڽ��� �ش� ������ ������ 4��� �� ��) -> ���� �ȿ� ������ 4����Ͽ� �ٽ� �浹 �� ( �Լ� ��� ȣ�� )
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
	/* �� ����Ʈ���� �̿��� �ƴ� �� �ڽ��� �̿��� �����Ѵ� -> �� ���� Ʈ���� �� �ɰ��� �׷��� �ϴ��� �Ǵ��ϱ� ���� */

	/* LT �� �ִ� �ڽ��� �ڽ��� �������� �ʾ����� Ż�� -> �ڽ��� nullptr == �ڽ��� ���� -> �ɰ����� �ʴ´� */
	if (nullptr == m_pChilds[CHILD_LT]->m_pChilds[CHILD_LT])
		return;

	/* �� �ڽĵ��� ���⿡ �ִ� �ڽ��� �̿����� ���� */

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


	/* �θ�� ���� �ڽ��� �ɰ����� �� , �ڽĵ��� �̿��� ��� �ֵ��� ����� */
	/* -> �θ𳢸��� �̿� ���谡 �����Ǿ�����, �ش� �θ��� �ڽ��� �̿��� ������ �ȵ� ������ ���� */
	/* -> �ڽ��� ������ �ȵ� �̿��� �������ش� */

	/* ������ �̿��� �ִ� �θ��� �ڽ��̶�� */
	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		/* �ڽ� RT �� ��� �ִ� ������ �̿��� , �θ��� ������ �̿��� �ڽ� LT �� �������ش� */
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LT];
		/* �ڽ� RB �� ��� �ִ� ������ �̿��� , �θ��� ������ �̿��� �ڽ� LB �� �������ش� */
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LB];
	}

	/* �Ʒ��� �̿��� �ִ� �θ��� �ڽ��̶�� */
	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		/* �ڽ� LB �� ��� �ִ� �Ʒ��� �̿��� , �θ��� �Ʒ��� �̿��� �ڽ� LT �� �������ش� */
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_LT];
		/* �ڽ� RB �� ��� �ִ� �Ʒ����̿��� , �θ��� �Ʒ��� �̿��� �ڽ� RT �� �������ش� */
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_RT];
	}

	/* ���� �̿��� �ִ� �θ��� �ڽ��̶�� */
	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		/* �ڽ� LT �� ��� �ִ� ���� �̿��� , �θ��� ���� �̿��� �ڽ� RT �� �������ش� */
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RT];
		/* �ڽ� LB �� ��� �ִ� ���� �̿��� , �θ��� ���� �̿��� �ڽ� RB �� �������ش� */
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RB];
	}

	/* ���� �̿��� �ִ� �θ��� �ڽ��̶�� */
	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		/* �ڽ� LT �� ��� �ִ� ���� �̿��� , �θ��� ���� �̿��� �ڽ� LB �� �������ش� */
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_LB];
		/* �ڽ� RT �� ��� �ִ� ���� �̿��� , �θ��� ���� �̿��� �ڽ� RB �� �������ش� */
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_RB];
	}

	/* ������ �ڽ��� �ڽ��� �������ش� (���ȣ��) -> ������ �Լ� ù���� return ���� Ż�� */
	for (_uint i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChilds[i])
			m_pChilds[i]->SetUp_Neighbors();
	}
}

_bool CQuadTree::Is_Draw(const _float3 * pVerticesPos)
{
	/* LOD ����� ���� IsDraw */
	/* LEVEL OF DETAIL : LEVLE = �Ÿ� */

	/* ������ ���̰� �������� �������� �ʴٸ� ũ���� �����. */
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return false;

	/* ī�޶�� ������ �Ÿ��� ���� �������� �����Ѵ� -> ī�޶� ��ġ������ ������ */
	_float4 vCamPosition = pGameInstance->Get_CamPosition();


	/* ī�޶�� ������ ������ �Ÿ� */
	_float	fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCenter]) - XMLoadFloat4(&vCamPosition)));

	/* ���� ū ��ƮƮ���� ���γ��� */
	_float	fWidth = XMVectorGetX(
		XMVector3Length(
			XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])
		));

	/* LOD �� ������ ������ ��� �Ѵ� -> �ƴϸ� ����ϴ� ��Ȳ�� ���ֻ��ܼ� ������ ������ �̷�� ���� ���� */
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
