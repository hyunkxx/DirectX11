#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pPasses(rhs.m_pPasses)
	, m_iPassCount(rhs.m_iPassCount)
{
	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElement, _uint iElementCount)
{
	_uint			iHlslFlag = 0;
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL0;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);
	pTechnique->GetDesc(&TechniqueDesc);

	m_pPasses = new PASS[TechniqueDesc.Passes];
	m_iPassCount = TechniqueDesc.Passes;

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		m_pPasses[i].pPass = pTechnique->GetPassByIndex(i);
		D3DX11_PASS_DESC PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);

		m_pPasses[i].pPass->GetDesc(&PassDesc);
		if (FAILED(m_pDevice->CreateInputLayout(
			pElement, //���� �׸������ϴ� ������ ��������
			iElementCount, //������ ��� ����
			PassDesc.pIAInputSignature, //���̴����� �޾��ִ� ������ ����
			PassDesc.IAInputSignatureSize, //���̴����� �޾��ִ� ������ ������ ũ��
			&m_pPasses[i].pInputLayout)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iPassCount)
		return E_FAIL;

	m_pContext->IASetInputLayout(m_pPasses[iPassIndex].pInputLayout);
	m_pPasses[iPassIndex].pPass->Apply(0, m_pContext);

	return S_OK;
}

HRESULT CShader::SetMatrix(const char* pName, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	//�������� ���ڿ��� ���� �̸��� ���� ���̴� ���������� �ڵ��� ����
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	//pVariable->SetRawValue(); ������ Ÿ���Ǳ��о��� ���̴� ���������� ���� ���� �� �ִ� �Լ�.
	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Set_Matrices(const char * pConstantName, const _float4x4 * pMatrix, _uint iNumMatrices)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);

	// �ش� �̸��� ������ ��ã�� ��� ���⼭ Ż��
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();

	// �ش� �̸��� ������ ����� �ƴ� ��� ���⼭ Ż��
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::SetRawValue(const char* pConstantName, const void* pData, _uint iSize)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

HRESULT CShader::SetShaderResourceView(const char* pName, ID3D11ShaderResourceView* pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable = pVariable->AsShaderResource();
	if (nullptr == pShaderResourceVariable)
		return E_FAIL;

	return pShaderResourceVariable->SetResource(pSRV);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElement, _uint iElementCount)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElement, iElementCount)))
	{
		wstring message = L"Failed to Create : CShader";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Cloned : CShader";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	if (false == m_isClone)
	{
		for (_uint i = 0; i < m_iPassCount; ++i)
		{
			Safe_Release(m_pPasses[i].pInputLayout);
		}
		Safe_Delete_Array(m_pPasses);
	}

	Safe_Release(m_pEffect);
}
