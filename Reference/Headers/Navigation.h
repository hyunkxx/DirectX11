#pragma once

#include "Component.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	enum NAVISTATE {
		NAVI_OK,
		NAVI_NO,
		NAVI_SLIDE,
		NAVI_END
	};
	enum EXIT
	{
		EXIT_NO,
		EXIT_UP,
		EXIT_DOWN,
		EXIT_ALL,
		EXIT_END
	};

public:
	typedef struct tagNavigation_Desc
	{
		/* Navi Com을 가지는 객체의 생성 위치가 어디인지 판단할 CurrentIndex */
		_int		iCurrentIndex = -1;
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	vector<class CCell*> Get_Cells() { return m_Cells; }
	NAVIGATION_DESC Get_NavigationDesc() { return m_NavigationDesc; }
	_int Get_NeighborIndex() { return m_iNeighborIndex; }
	_int Get_CellState(_uint iIndex);

	_uint Get_CurCellState();

	void Set_CurrentIndex(_uint iIndex)
	{
		// 네비 확인용 예외처리
		if (m_Cells.size() < iIndex)
			iIndex = 0;

		m_NavigationDesc.iCurrentIndex = iIndex;
	}
	CCell* Get_CurCell()
	{
		return  m_Cells[m_NavigationDesc.iCurrentIndex];
	}

	_uint Get_ExitClimb()
	{
		return m_iExitClimb;
	}

	void Set_ExitClimb(_uint iExitClimb)
	{
		m_iExitClimb = iExitClimb;
	}

public:
	/* 파일 경로를 받아와야 됨으로 override 를 사용할 수 없음 -> 부모와 형태가 달라져서 */
	virtual	HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual	HRESULT Initialize(void* pArg) override;

public:
	/* 판단할 타입은 상황에 따라 정해주면 된다 -> 판단 상황이 여러가지라면 열거체로 반환타입을 변경 */
	/* 움직일 수 있다/없다 , 슬라이딩을 할 수 있다/없다 */
	/* vPosition : 객체의 움직이고 난 뒤의 결과 위치 */

	CNavigation::NAVISTATE Move_OnNavigation(_fvector vPosition, _fvector vMove, _float3* vSlideOut);
	CNavigation::NAVISTATE Move_OnNavigation_NoClimb(_fvector vPosition, _fvector vMove, _float3* vSlideOut);
	CNavigation::NAVISTATE Climb_OnNavigation(_fvector vPosition, _fvector vTopPosition, _fvector vSpinePosition, _gvector vMove, _float3* vSlideOut);

public:
	_float Compute_Height(_fvector vPosition);

public:
	_int Is_CurrentIn(_fvector vPosition);
	_int Get_CurrentIndex()
	{
		return m_NavigationDesc.iCurrentIndex;
	}

private:
	_int							m_iCell_In_CheckIndex = { 0 };

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;

	void Set_IntervalY(_float fIntervalY) { m_fIntervalY = fIntervalY; };

private:
	_float4							m_vState_1_CellColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	_float4							m_vState_2_CellColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	_float4							m_vState_3_CellColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	_float4							m_vCellColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	_float4							m_vColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	_float							m_fIntervalY = { 0.0f };

#endif // _DEBUG

private:
	typedef vector<class CCell*>	CELLS;
	CELLS							m_Cells;

	_uint							m_iTotalLoadNum = { 0 };

private:
	/* 네비게이션을 이용하는 객체가 존재하는 Cell 의 위치(인덱스) */
	NAVIGATION_DESC					m_NavigationDesc;
	_int							m_iNeighborIndex = { -1 };

private:
	// Climb 상태를 끝낼건지 확인하는 함수
	// 0 == 이동 안함, 1 == 위로 나감, 2 == 아래로 나감, 3 == 둘다 나감
	_uint	m_iExitClimb = { EXIT_NO };

#ifdef _DEBUG
	class CShader*					m_pShader = { nullptr };
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbor();

public:
	/* CNavigation 을 생성할 때 툴에서 만든 삼각형의 정점 구성 정보 파일을 가져와 로드 하여 이정보를 적용시킨다 */
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END