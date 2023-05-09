#pragma once

#include "Base.h"

/* �׺���̼��� �����ϴ� �ϳ��� �ﰢ�� */

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT {
		POINT_A,
		POINT_B,
		POINT_C,
		POINT_END
	};

	enum LINE {
		LINE_AB,
		LINE_BC,
		LINE_CA,
		LINE_END
	};

	enum CELL_STATE { NORMAL_CELL, FALL_CELL, CAN_UP_CELL, CAN_DOWN_CELL, END_CELL };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) { return XMLoadFloat3(&m_vPoints[ePoint]); }

	_float3* Get_vPoints() { return m_vPoints; }
	_int Get_State() { return m_iState; }
	
	void Set_State(_int iState) {
		if (0 > iState || END_CELL < iState)
			iState = 0;
		m_iState = iState;
	};

public:
	virtual HRESULT Initialize(_int iIndex, const _float3 * pPoints, _int iState = 0);

public:
	_bool Compare_Points(_fvector vSour, _fvector vDest);
	void SetUp_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_Neighbors[eLine] = pNeighbor->m_iIndex;
	}

	/* ��ü�� ������ ��� ��ġ , ���� ������ �����ϴ� �̿�Cell�� Index */
	/* -> �� �ȿ��� NeighborIndex �� ������ ���̱� ������ �����ͷ� ���ڸ� �޴´�. */
	//_bool Is_In(_fvector vPosition, _int* pNeigborIndex);

	_bool IsIn(_fvector vPosition, _int* pNeighborIndex, _bool bClimbing, _int* pOutLine = nullptr);
	_bool CrossMultiEndLine(_fvector vPosition);

#pragma region FOR_TOOL
public:
	// ����Ҽ��� ���� ������ �; ����
	_bool Is_CurrentIn(_fvector vPosition);
#pragma endregion FOR_TOOL
public:
#ifdef _DEBUG
	void Render(class CShader* pShader, _float4 vColor);
#endif // _DEBUG


private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

private: /* �ڽ� Cell */
	_float3							m_vPoints[POINT_END];
	_float3							m_vNormals[LINE_END];

	// Cell �ɼ�
	_int							m_iState = { 0 };
	_float3							m_vPlaneNormal;

private: /* �̿� Cell */
		 /* �̿��� ������ ���� Cell Ŭ�����̴� -> �ش� Ŭ�������� �ڽ��� Ŭ����Ÿ���� ������ �����ϸ� �����Ҷ� ����� ��찡 ���� -> �׷��� Index �� �̿��ؼ� �����Ѵ� */
		 /* ���� 3���� �����ϰ� ���� 2���� ���� �ﰢ���� �ִٸ� �̿����� ����Ѵ�. - -1 �� �ʱ�ȭ�� �Ͽ� -1 �̸� �̿��� ���°ɷ� �����Ѵ� */
	_int							m_iIndex = { 0 };
	_int							m_Neighbors[3] = { -1, -1, -1 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = { nullptr };
#endif // _DEBUG

public:
	static CCell* Create(
		ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext,
		_int iIndex,
		const _float3* pPoints,
		_int iState = 0
	);
	virtual void Free() override;

};

END