#include "..\Public\LightManager.h"
#include "Light.h"
#include "GameInstance.h"
#include "RenderSetting.h"

IMPLEMENT_SINGLETON(CLightManager)

HRESULT CLightManager::AddLight(ID3D11Device* Device, ID3D11DeviceContext* Context, const LIGHT_DESC& LightDesc)
{
	CLight* Light = CLight::Create(Device, Context, LightDesc);
	if (nullptr == Light)
		return E_FAIL;

	m_Lights.push_back(Light);

	return S_OK;
}

HRESULT CLightManager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

const LIGHT_DESC* CLightManager::GetLightDesc(_uint Index)
{
	CPipeLine* pPipeline = CPipeLine::GetInstance();
	CRenderSetting* pRenderSetting = CRenderSetting::GetInstance();

	auto iter = m_Lights.begin();

	for (_uint i = 0; i < Index; ++i)
		++iter;

	if (iter == m_Lights.end())
		return nullptr;

	return (*iter)->GetLightDesc();
}

void CLightManager::ShadowUpdate(_float fLightHight, _vector vOriginPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float fRatio = fLightHight * 0.45f;
	_float fHeight = XMVectorGetY(vOriginPos);
	//카메라->플레이어 위치를 원점으로 하는 좌표로 변경
	//_vector vCamPos = XMLoadFloat4(&pGameInstance->Get_CamPosition());
	_vector vCamPos = vOriginPos;
	_vector vLightEye = XMVectorSet(XMVectorGetX(vCamPos) - 50.f, fLightHight + fHeight, XMVectorGetZ(vCamPos) - 50.f, 1.f);
	_vector vLightAt = XMVectorSet(XMVectorGetX(vCamPos), fHeight, XMVectorGetZ(vCamPos), 1.f);
	_matrix vLightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, VECTOR_UP);
	pGameInstance->SetLightMatrix(vLightViewMatrix, LIGHT_MATRIX::LIGHT_VIEW);
	SetLightDirection(XMVector3Normalize(vLightAt - vLightEye));
}

void CLightManager::BakeShadowLight(_fvector vLightEye, _fvector vLightAt)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_matrix vLightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, VECTOR_UP);
	XMStoreFloat4x4(&m_BakeLightMatrix[LIGHT_VIEW], vLightViewMatrix);
}

_float4x4 CLightManager::GetBakeLightFloat4x4(LIGHT_MATRIX eLightMatrix)
{
	return m_BakeLightMatrix[eLightMatrix];
}

void CLightManager::SetLightMatrix(_fmatrix LightMatrix, LIGHT_MATRIX eLightMatrix)
{
	switch (eLightMatrix)
	{
	case Engine::CLightManager::LIGHT_VIEW:
		XMStoreFloat4x4(&m_LightMatrix[LIGHT_VIEW], LightMatrix);
		break;
	case Engine::CLightManager::LIGHT_PROJ:
		XMStoreFloat4x4(&m_LightMatrix[LIGHT_PROJ], LightMatrix);
		XMStoreFloat4x4(&m_BakeLightMatrix[LIGHT_PROJ], LightMatrix);
		break;
	default:
		break;
	}
}

_float4x4 CLightManager::GetLightFloat4x4(LIGHT_MATRIX eLightMatrix)
{
	return m_LightMatrix[eLightMatrix];
}

_float4x4 CLightManager::GetLightInverseFloat4x4(LIGHT_MATRIX eLightMatrix)
{
	_float4x4 InvMatrix;
	XMStoreFloat4x4(&InvMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LightMatrix[eLightMatrix])));
	return InvMatrix;
}

void CLightManager::SetLightPosition(_fvector vLightPos)
{
	XMStoreFloat4(&m_vLightPos, vLightPos);
}

void CLightManager::SetLightDirection(_fvector vLightDir)
{
	XMStoreFloat4(&m_vLightDir, vLightDir);
}

void CLightManager::Free()
{
	for (auto& Light : m_Lights)
		Safe_Release(Light);

	m_Lights.clear();
}
