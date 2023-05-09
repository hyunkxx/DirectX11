#pragma once

#include "Base.h"

/* 네비게이션을 구성하는 하나의 삼각형 */

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

	/* 객체의 움직임 결과 위치 , 나간 선분을 공유하는 이웃Cell의 Index */
	/* -> 이 안에서 NeighborIndex 를 가져올 것이기 때문에 포인터로 인자를 받는다. */
	//_bool Is_In(_fvector vPosition, _int* pNeigborIndex);

	_bool IsIn(_fvector vPosition, _int* pNeighborIndex, _bool bClimbing, _int* pOutLine = nullptr);
	_bool CrossMultiEndLine(_fvector vPosition);

#pragma region FOR_TOOL
public:
	// 사용할수도 있지 않을까 싶어서 냅둠
	_bool Is_CurrentIn(_fvector vPosition);
#pragma endregion FOR_TOOL
public:
#ifdef _DEBUG
	void Render(class CShader* pShader, _float4 vColor);
#endif // _DEBUG


private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

private: /* 자신 Cell */
	_float3							m_vPoints[POINT_END];
	_float3							m_vNormals[LINE_END];

	// Cell 옵션
	_int							m_iState = { 0 };
	_float3							m_vPlaneNormal;

private: /* 이웃 Cell */
		 /* 이웃의 정보는 같은 Cell 클래스이다 -> 해당 클래스에서 자신의 클래스타입의 정보를 저장하면 삭제할때 곤란한 경우가 있음 -> 그래서 Index 를 이용해서 저장한다 */
		 /* 정점 3개를 저장하고 정점 2개가 같은 삼각형이 있다면 이웃으로 등록한다. - -1 로 초기화를 하여 -1 이면 이웃이 없는걸로 판정한다 */
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