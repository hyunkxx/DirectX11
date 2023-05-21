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
		/* Navi Com�� ������ ��ü�� ���� ��ġ�� ������� �Ǵ��� CurrentIndex */
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
		// �׺� Ȯ�ο� ����ó��
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
	/* ���� ��θ� �޾ƿ;� ������ override �� ����� �� ���� -> �θ�� ���°� �޶����� */
	virtual	HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual	HRESULT Initialize(void* pArg) override;

public:
	/* �Ǵ��� Ÿ���� ��Ȳ�� ���� �����ָ� �ȴ� -> �Ǵ� ��Ȳ�� ����������� ����ü�� ��ȯŸ���� ���� */
	/* ������ �� �ִ�/���� , �����̵��� �� �� �ִ�/���� */
	/* vPosition : ��ü�� �����̰� �� ���� ��� ��ġ */

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
	/* �׺���̼��� �̿��ϴ� ��ü�� �����ϴ� Cell �� ��ġ(�ε���) */
	NAVIGATION_DESC					m_NavigationDesc;
	_int							m_iNeighborIndex = { -1 };

private:
	// Climb ���¸� �������� Ȯ���ϴ� �Լ�
	// 0 == �̵� ����, 1 == ���� ����, 2 == �Ʒ��� ����, 3 == �Ѵ� ����
	_uint	m_iExitClimb = { EXIT_NO };

#ifdef _DEBUG
	class CShader*					m_pShader = { nullptr };
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbor();

public:
	/* CNavigation �� ������ �� ������ ���� �ﰢ���� ���� ���� ���� ������ ������ �ε� �Ͽ� �������� �����Ų�� */
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END