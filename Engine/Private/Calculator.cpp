#include "..\Public\Calculator.h"

#include "GameInstance.h"

CCalculator::CCalculator(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CCalculator::CCalculator(const CCalculator & rhs)
	: CComponent(rhs)
	, m_vMousePicking_Positon(rhs.m_vMousePicking_Positon)
{
}

HRESULT CCalculator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCalculator::Initialize(void * pArg)
{
	return S_OK;
}

_float3 CCalculator::Picking_OnTerrain(
	HWND hWnd,
	CVIBuffer_Terrain* pVerticesTerrainCom,
	CTransform* pTransformCom
)
{
	/* 1. ���콺 ��ǥ�� ������. */
	/* 2. ��ŷ�� ���� ������Ʈ */
	/* 3. ������Ʈ�� ���콺�� ��ŷ �Ǵ� */

#pragma region MOUSEPOSITION_INVERSE

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		Vp_MousePosition;

	_uint		iViewPortNum;

	/* RS : ������ ������ */
	/* nullptr �� ������ ����Ʈ�� ������ ���� */
	m_pContext->RSGetViewports(&iViewPortNum, nullptr);

	D3D11_VIEWPORT*		pViewPort;
	/* ���� ����Ʈ ���� ��ŭ ���� �Ҵ� */
	pViewPort = new D3D11_VIEWPORT[iViewPortNum];

	m_pContext->RSGetViewports(&iViewPortNum, pViewPort);

	/* ���� ����Ʈ�� �ϳ����� ó������ ������ �� */
	_float fvpWidth = pViewPort[0].Width;
	_float fvpHeight = pViewPort[0].Height;

	/* �� ��Ʈ -> ���� */
	Vp_MousePosition.x = ptMouse.x / (fvpWidth * 0.5f) - 1.f;
	Vp_MousePosition.y = ptMouse.y / -(fvpHeight * 0.5f) + 1.f;
	Vp_MousePosition.z = 0.f;
	Vp_MousePosition.w = 1.f;

	Safe_Delete_Array(pViewPort);

	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* ���� -> �� */
	_matrix			Inverse_ProjMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_PROJ);
	/* XMVECTOR �ڷ����� ����ϸ� ������ ���Ұ��� -> Coord , Normal �Լ� ���� ��ȯ���� �޾���� �� or XMFLOAT4 �ڷ��� ����� ���� �ÿ��� XMVECTOR �� ���� */
	_vector View_MousePosition = XMVector3TransformCoord(XMLoadFloat4(&Vp_MousePosition), Inverse_ProjMatrix);

	/* ���콺 ���� */
	_vector		View_RayDir, View_RayPos;
	View_RayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f); /* �� �����̽� ���� ���� */
	View_RayDir = View_MousePosition - View_RayPos; /* ���� -> ���콺 ��ġ �� ��� ���� */

	/* �� -> ���� */
	_matrix			Inverse_ViewMatrix = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE::TS_VIEW);

	_vector World_RayPos, World_RayDir;
	World_RayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	/* ��ġ ���� */
	World_RayPos = XMVector3TransformCoord(View_RayPos, Inverse_ViewMatrix);
	/* ���� ���� */
	World_RayDir = XMVector3TransformNormal(View_RayDir, Inverse_ViewMatrix);

	/* ���� -> ���� */
	/*_matrix			Inverse_WorldMatrix = pTransformCom->Get_Inverse_WorldMatrix_XM();
	XMVector3TransformCoord(vRayPos, Inverse_WorldMatrix);
	XMVector3TransformNormal(vRayDir, Inverse_WorldMatrix);*/

	/* ���尡 �׵� �϶� ���ñ��� ������ ������ ���� ���� ������ ����. */

	/* ���� ���� ����ȭ -> ���� ������ ���콺 ������ �ϼ��� */
	_vector World_RayDir_Noramlize = XMVector3Normalize(World_RayDir);

	Safe_Release(pGameInstance);

#pragma endregion MOUSEPOSITION_INVERSE

#pragma region INTERSECTS_TERRAIN_MOUSE
	/* CVIBuffer_Terrain ���� ���� X , Z �� ���� ������ ���´� */
	_ulong dwVerticesNumX = pVerticesTerrainCom->GetTerrainSize().mX;
	_ulong dwVerticesNumZ = pVerticesTerrainCom->GetTerrainSize().mZ;
	/* CVIBuffer_Terrain �� ������ ��ġ ������ ������ �����迭�� ���´� */
	_float3* pVerticesPosition = pVerticesTerrainCom->GetVertexPosition();

	/* �׋� �׶� �ﰢ���� �׸� ������ ������ ����� �ſ� */
	_long dwVerticesIndex[3]{};

	_float fDist = 0.f;

	/* ������ ������ŭ */
	for (_ulong i = 0; i < dwVerticesNumZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwVerticesNumX - 1; ++j)
		{
			/* �ﰢ���� ���� ���� */
			_ulong dwIndex = i * dwVerticesNumX + j;

			/* ������ �� �ﰢ�� */
			dwVerticesIndex[0] = dwIndex + dwVerticesNumX; /* �� �� */
			dwVerticesIndex[1] = dwIndex + dwVerticesNumX + 1; /* �� �� */
			dwVerticesIndex[2] = dwIndex + 1; /* �� �� */

											  /* ���콺 ������ �ﰢ���� �浹 �� */
			if (TriangleTests::Intersects(
				World_RayPos, /* ���� ���� */
				World_RayDir_Noramlize, /* ���� ���� */
										/* �ﰢ���� �׸��� �̿�� ������ ��ġ -> ���ڰ� XMVECTOR �� XMVectorSEt() �� ���� �� ���� w���� 1 */
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[1]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[2]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[0]]),
				fDist /* ���콺 ������ ��ġ�� ���� ��ŷ�� �� ������ �Ÿ� */
			))
			{
				/* ���� ��ŷ�� ������ ��ġ */
				XMStoreFloat3(&m_vMousePicking_Positon, World_RayPos + World_RayDir_Noramlize * fDist);
				return m_vMousePicking_Positon;
			}

			/* ���� �Ʒ� �ﰢ�� */
			dwVerticesIndex[0] = dwIndex + dwVerticesNumX; /* �� �� */
			dwVerticesIndex[1] = dwIndex + 1; /* �� �� */
			dwVerticesIndex[2] = dwIndex; /* �� �� */

										  /* ���콺 ������ �ﰢ���� �浹 �� */
			if (TriangleTests::Intersects(
				World_RayPos, /* ���� ���� */
				World_RayDir_Noramlize, /* ���� ���� */
										/* �ﰢ���� �׸��� �̿�� ������ ��ġ -> ���ڰ� XMVECTOR �� XMVectorSEt() �� ���� �� ���� w���� 1 */
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[2]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[0]]),
				XMLoadFloat3(&pVerticesPosition[dwVerticesIndex[1]]),
				fDist /* ���콺 ������ ��ġ�� ���� ��ŷ�� �� ������ �Ÿ� */
			))
			{
				/* ���� ��ŷ�� ������ ��ġ */
				XMStoreFloat3(&m_vMousePicking_Positon, World_RayPos + World_RayDir_Noramlize * fDist);
				return m_vMousePicking_Positon;
			}
		}
	}

#pragma endregion INTERSECTS_TERRAIN_MOUSE

	return m_vMousePicking_Positon;
}

CCalculator * CCalculator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCalculator*			pInstance = new CCalculator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCalculator::Clone(void * pArg)
{
	CCalculator*			pInstance = new CCalculator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCalculator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCalculator::Free()
{
	__super::Free();
}
