#include "..\Public\Light.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "RenderSetting.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	_uint iPassIndex = 0;
	
	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eLightType)
	{
		iPassIndex = 1;
		if (FAILED(pShader->SetRawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eLightType)
	{
		iPassIndex = 2;
		if (FAILED(pShader->SetRawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->SetRawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
	}

	if (pShader->SetRawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4)))
		return E_FAIL;
	if (pShader->SetRawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4)))
		return E_FAIL;
	if (pShader->SetRawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4)))
		return E_FAIL;

	CRenderSetting* pRenderSetting = CRenderSetting::GetInstance();
	if (pRenderSetting->IsActiveSSAO())
	{
		if (iPassIndex == 1)
		{
			// Directinal Light
			if (FAILED(pShader->Begin(9)))
				return E_FAIL;
		}
		else if(iPassIndex == 2)
		{
			// Point Light
			if (FAILED(pShader->Begin(2)))
				return E_FAIL;
		}
	}
	else
	{
		if (FAILED(pShader->Begin(iPassIndex)))
			return E_FAIL;
	}


	return pVIBuffer->Render();
}

CLight* CLight::Create(ID3D11Device* Device, ID3D11DeviceContext* Context, const LIGHT_DESC& LightDesc)
{
	CLight*	pInstance = new CLight(Device, Context);
	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}


