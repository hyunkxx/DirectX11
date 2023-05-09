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

	void Set_CurrentIndex(_uint iIndex)
	{
		m_iCurrentIndex = iIndex;
	}

public:
	/* ���� ��θ� �޾ƿ;� ������ override �� ����� �� ���� -> �θ�� ���°� �޶����� */
	virtual	HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual	HRESULT Initialize(void* pArg) override;

public:
	/* �Ǵ��� Ÿ���� ��Ȳ�� ���� �����ָ� �ȴ� -> �Ǵ� ��Ȳ�� ����������� ����ü�� ��ȯŸ���� ���� */
	/* ������ �� �ִ�/���� , �����̵��� �� �� �ִ�/���� */
	/* vPosition : ��ü�� �����̰� �� ���� ��� ��ġ */

	CNavigation::NAVISTATE Move_OnNavigation(_fvector vPosition, _fvector vMove, _float3* vSlideOut, _bool bClimbing = false);

public:
	_float Compute_Height(_fvector vPosition);

public:
	_int Is_CurrentIn(_fvector vPosition);

private:
	_int							m_iCurrentIndex = { 0 };

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