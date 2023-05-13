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
	/* 원본에서 네비게이션 메쉬(Cell)을 세팅하고 사본으로 복사처리 -> Cell의 주소 공유됨 */
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
		// 해당 방향에 이웃이 있다면 이동
		if (-1 != iNeighborIndex)
		{
			// 이동 방향 이웃의 Type
			// CCell::STATE > 0 == GROUND, 1 == WALL
			_int iNeighborState = m_Cells[iNeighborIndex]->Get_State();

			// 벽타기 중이 아니라면
			if (!bClimbing)
			{
				// 이동 방향 이웃이 Wall이 아니라면
				if (1/*iNeighborState != 1*/)
				{
					// 그냥 이동
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
				// 이동 방향 이웃이 Wall이라면
				else
				{

				}
			}

			// 벽타기 중이라면
			else
			{
				// 이동 방향 이웃이 Wall이라면
				if (1/*iNeighborState != 1*/)
				{
					// 그냥 이동
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
				// 이동 방향 이웃이 Wall이 아니라면
				else
				{

				}
			}
		}

		// 해당 방향에 이웃이 없다면 Slide
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

	/* 지형이 들고있는 Nevigation Com 으로는 객체가 어디있는지 알기가 쉽지않음 -> 객체가 들고있는 컴포넌트로 Cell을 한번 더 그린다  */
	if (-1 == m_NavigationDesc.iCurrentIndex)
	{
		/* 지금은 WorldMatrix 를 항등행렬로 넣어주지만, 움직이는 지형이 있다면 WorldMatrix 를 따로 가져와 넣어줘야 한다 -> 지형이 움직이면 Navigation 도 같이 움직임 */

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
		/* 객체와 지형의 네비메쉬가 같은위치에 있어 구분인 안됨 -> 객체의 네비메쉬의 y를 좀 띄워준다 */
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
	/* 모든 이웃 삼각형을 세팅한다. - 이웃의 세팅은 모든 Cell 이 만들어 지고 난 뒤에 해야함 */
	/* 모든 셀을 탐색하는데 2중 루프를 돌려 같은 주소인 Cell을 건너뛰고 Compare_Points() 함수에 점 2개의 정보를 넘겨 같은 점 이라면 이웃으로 등록해 준다 */
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
