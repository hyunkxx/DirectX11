#include "..\Public\Effect.h"
#include "PipeLine.h"

CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	for (int j = 0; BUFFER_END > j; j++)
	{
		for (int i = 0; 100 > i; i++)
		{
			_itoa_s(i, m_FileName[j][i], 10);
		}
	}
	
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CEffect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CEffect::Render()
{
	return S_OK;
}

void CEffect::RenderGUI()
{
	__super::RenderGUI();
}

void CEffect::Set_Edit()
{
	if (m_bEDit)
		m_bEDit = false;
	else
		m_bEDit = true;

}

void CEffect::Compute_CamDistance(_fvector vWorldPos)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	Safe_AddRef(pPipeLine);
	m_fCameraDistance = XMVectorGetX(XMVector3Length(vWorldPos - XMLoadFloat4(&pPipeLine->Get_CamPosition())));

	Safe_Release(pPipeLine);

}
void CEffect::Free()
{
	__super::Free();
}
