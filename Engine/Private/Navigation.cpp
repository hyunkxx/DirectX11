#include "..\Public\Navigation.h"

#include "GameInstance.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)

#ifdef _DEBUG
	, m_vState_1_CellColor{ rhs.m_vState_1_CellColor }
	, m_vState_2_CellColor{ rhs.m_vState_2_CellColor }
	, m_vState_3_CellColor{ rhs.m_vState_3_CellColor }

	, m_vCellColor{ rhs.m_vCellColor }
	, m_vColor{ rhs.m_vColor }
	, m_fIntervalY{ rhs.m_fIntervalY }
#endif // _DEBUG
	

#ifdef _DEBUG
	, m_pShader{ rhs.m_pShader }
#endif // _DEBUG

{
	/* �������� �׺���̼� �޽�(Cell)�� �����ϰ� �纻���� ����ó�� -> Cell�� �ּ� ������ */
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG
}

_int CNavigation::Get_CellState(_uint iIndex)
{
	if (m_Cells.empty() || 0 > iIndex || m_Cells.size() <= iIndex)
		return 0;

	return m_Cells[iIndex]->Get_State();
}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationDataFilePath)
{
	HANDLE			hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
	{
		MSG_BOX("Failed To Open File : Navigation");
		return E_FAIL;
	}
	
	_ulong			dwByte = 0;

	NAVIPOINT_DESC		NaviPoint_Desc = {};

	ReadFile(hFile, &m_iTotalLoadNum, sizeof(_uint), &dwByte, nullptr);

	/*while (true)
	{
		ZeroMemory(&NaviPoint_Desc, sizeof(NAVIPOINT_DESC));

		ReadFile(hFile, NaviPoint_Desc.vP, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &NaviPoint_Desc.iCell_State, sizeof(int), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*			pCell = CCell::Create(m_pDevice, m_pContext, (int)m_Cells.size(), NaviPoint_Desc.vP, NaviPoint_Desc.iCell_State);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}*/

	for (_uint i = 0; i < m_iTotalLoadNum; ++i)
	{
		ZeroMemory(&NaviPoint_Desc, sizeof(NAVIPOINT_DESC));

		ReadFile(hFile, NaviPoint_Desc.vP, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &NaviPoint_Desc.iCell_State, sizeof(int), &dwByte, nullptr);

		CCell*			pCell = CCell::Create(m_pDevice, m_pContext, (int)m_Cells.size(), NaviPoint_Desc.vP, NaviPoint_Desc.iCell_State);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(
		m_pDevice,
		m_pContext,
		TEXT("../../Shader/SHADER_CELL.hlsl"),
		VTXPOS_DECLARATION::Elements,
		VTXPOS_DECLARATION::iNumElements
	);
#endif // _DEBUG

#ifdef _DEBUG
	m_vState_1_CellColor = { 1.0f, 1.0f, 0.0f, 1.0f };
	m_vState_2_CellColor = { 0.0f, 1.0f, 1.0f, 1.0f };
	m_vState_3_CellColor = { 1.0f, 0.0f, 1.0f, 1.0f };

	m_vCellColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	m_vColor = m_vCellColor;
#endif // _DEBUG

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_NavigationDesc, pArg, sizeof(NAVIGATION_DESC));

	return S_OK;
}

CNavigation::NAVISTATE CNavigation::Move_OnNavigation(_fvector vPosition, _fvector vMove, _float3 * vSlideOut, _bool bClimbing)
{
	if (-1 == m_NavigationDesc.iCurrentIndex)
		return  CNavigation::NAVI_NO;
	

	_int		iStartIndex = m_NavigationDesc.iCurrentIndex;
	_int		iNeighborIndex = -1;
	_int		iOutLine = -1;

	_fvector PredictedPosition = vPosition + vMove;

	if (true == m_Cells[m_NavigationDesc.iCurrentIndex]->IsIn(PredictedPosition, &iNeighborIndex, bClimbing, &iOutLine))
		return  CNavigation::NAVI_OK;
	else
	{
		// �ش� ���⿡ �̿��� �ִٸ� �̵�
		if (-1 != iNeighborIndex)
		{
			// �̵� ���� �̿��� Type
			// CCell::STATE > 0 == GROUND, 1 == WALL
			_int iNeighborState = m_Cells[iNeighborIndex]->Get_State();

			// ��Ÿ�� ���� �ƴ϶��
			if (!bClimbing)
			{
				// �̵� ���� �̿��� Wall�� �ƴ϶��
				if (1/*iNeighborState != 1*/)
				{
					// �׳� �̵�
					while (true)
					{
						if (-1 == iNeighborIndex)
						{
							XMStoreFloat3(vSlideOut, XMVectorSet(0.f, 0.f, 0.f, 0.f));
							return  CNavigation::NAVI_NO;
						}

						if (true == m_Cells[iNeighborIndex]->IsIn(PredictedPosition, &iNeighborIndex, bClimbing))
							break;
					}
					m_NavigationDesc.iCurrentIndex = iNeighborIndex;
					return CNavigation::NAVI_OK;
				}
				// �̵� ���� �̿��� Wall�̶��
				else
				{

				}
			}

			// ��Ÿ�� ���̶��
			else
			{
				// �̵� ���� �̿��� Wall�̶��
				if (1/*iNeighborState != 1*/)
				{
					// �׳� �̵�
					while (true)
					{
						if (-1 == iNeighborIndex)
						{
							XMStoreFloat3(vSlideOut, XMVectorSet(0.f, 0.f, 0.f, 0.f));
							return  CNavigation::NAVI_NO;
						}
						if (true == m_Cells[iNeighborIndex]->IsIn(PredictedPosition, &iNeighborIndex, bClimbing))
							break;

						m_NavigationDesc.iCurrentIndex = iNeighborIndex;
						return CNavigation::NAVI_OK;
					}
				}
				// �̵� ���� �̿��� Wall�� �ƴ϶��
				else
				{

				}
			}
		}

		// �ش� ���⿡ �̿��� ���ٸ� Slide
		else
		{
			if (false == m_Cells[m_NavigationDesc.iCurrentIndex]->CrossMultiEndLine(PredictedPosition))
			{
				_vector vPointA = m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point((CCell::POINT)iOutLine);
				_vector vPointB = m_Cells[m_NavigationDesc.iCurrentIndex]->Get_Point((CCell::POINT)((iOutLine + 1) % 3));

				_vector vOutLine = XMVector3Normalize(vPointB - vPointA);
				_vector vSlided = vOutLine * XMVector3Dot(vMove, vOutLine);


				XMStoreFloat3(vSlideOut, vOutLine * XMVector3Dot(vMove, vOutLine));

				_vector vSlidePos = vPosition + vSlided;

				if (true == m_Cells[m_NavigationDesc.iCurrentIndex]->IsIn(vSlidePos, &iNeighborIndex, bClimbing, &iOutLine))
					return  CNavigation::NAVI_SLIDE;
				else
				{
					if (-1 != iNeighborIndex)
					{
						while (true)
						{
							if (-1 == iNeighborIndex)
							{
								XMStoreFloat3(vSlideOut, XMVectorSet(0.f, 0.f, 0.f, 0.f));
								return CNavigation::NAVI_NO;
							}

							if (true == m_Cells[iNeighborIndex]->IsIn(vSlidePos, &iNeighborIndex, bClimbing))
								break;
						}
						m_NavigationDesc.iCurrentIndex = iNeighborIndex;
						return CNavigation::NAVI_SLIDE;
					}
					else
					{
						XMStoreFloat3(vSlideOut, XMVectorSet(0.f, 0.f, 0.f, 0.f));
						return CNavigation::NAVI_NO;
					}
				}
			}
			else
			{
				XMStoreFloat3(vSlideOut, XMVectorSet(0.f, 0.f, 0.f, 0.f));
				return CNavigation::NAVI_NO;
			}
		}
		return CNavigation::NAVI_SLIDE;
	}
}

_float CNavigation::Compute_Height(_fvector vPosition)
{
	if (m_Cells.empty())
		return 0.f;

	if (m_NavigationDesc.iCurrentIndex >= m_Cells.size())
		return 0.f;

	_float3* vPoint = m_Cells[m_NavigationDesc.iCurrentIndex]->Get_vPoints();

	_vector		vPlane = XMVectorZero();

	vPlane = XMPlaneFromPoints(XMLoadFloat3(&vPoint[0]),
		XMLoadFloat3(&vPoint[1]),
		XMLoadFloat3(&vPoint[2]));

	//ax + by + cz + d = 0;

	//y = (-ax - cz - d) / b;

	return (-XMVectorGetX(vPlane) * XMVectorGetX(vPosition) - XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition) - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}

_int CNavigation::Is_CurrentIn(_fvector vPosition)
{
	_int iCellIndex = 0;

	for (auto& pCell : m_Cells)
	{
		if (true == pCell->Is_CurrentIn(vPosition))
			m_iCell_In_CheckIndex = iCellIndex;
		else
			iCellIndex++;
	}

	return m_iCell_In_CheckIndex;
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float4x4		Identity;
	XMStoreFloat4x4(&Identity, XMMatrixIdentity());

	CGameInstance*	pGamInstance = CGameInstance::GetInstance();
	if (nullptr == pGamInstance)
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGamInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGamInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	/* ������ ����ִ� Nevigation Com ���δ� ��ü�� ����ִ��� �˱Ⱑ �������� -> ��ü�� ����ִ� ������Ʈ�� Cell�� �ѹ� �� �׸���  */
	if (-1 == m_NavigationDesc.iCurrentIndex)
	{
		/* ������ WorldMatrix �� �׵���ķ� �־�������, �����̴� ������ �ִٸ� WorldMatrix �� ���� ������ �־���� �Ѵ� -> ������ �����̸� Navigation �� ���� ������ */

		_int	iIndex = { 0 };
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
			{
				if (CCell::CELL_STATE::FALL_CELL == pCell->Get_State())
				{
					Identity._42 = m_fIntervalY + 0.01f;
					m_vColor = m_vState_1_CellColor;
				}
				else if (CCell::CELL_STATE::CAN_UP_CELL == pCell->Get_State())
				{
					Identity._42 = m_fIntervalY + 0.01f;
					m_vColor = m_vState_2_CellColor;
				}
				else if (CCell::CELL_STATE::CAN_DOWN_CELL == pCell->Get_State())
				{
					Identity._42 = m_fIntervalY + 0.01f;
					m_vColor = m_vState_3_CellColor;
				}
				else
				{
					Identity._42 = m_fIntervalY;
					m_vColor = m_vCellColor;
				}

				if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &Identity)))
					return E_FAIL;
				pCell->Render(m_pShader, m_vColor);
			}
			iIndex++;
		}
	}
	else
	{
		/* ��ü�� ������ �׺�޽��� ������ġ�� �־� ������ �ȵ� -> ��ü�� �׺�޽��� y�� �� ����ش� */
		Identity._42 = m_fIntervalY + 0.02f;
		if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &Identity)))
			return E_FAIL;

		m_Cells[m_NavigationDesc.iCurrentIndex]->Render(m_pShader, _float4(1.f, 0.f, 0.f, 1.f));
	}

	return S_OK;
}
#endif // _DEBUG

HRESULT CNavigation::SetUp_Neighbor()
{
	/* ��� �̿� �ﰢ���� �����Ѵ�. - �̿��� ������ ��� Cell �� ����� ���� �� �ڿ� �ؾ��� */
	/* ��� ���� Ž���ϴµ� 2�� ������ ���� ���� �ּ��� Cell�� �ǳʶٰ� Compare_Points() �Լ��� �� 2���� ������ �Ѱ� ���� �� �̶�� �̿����� ����� �ش� */
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}

	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationDataFilePath)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG
}
