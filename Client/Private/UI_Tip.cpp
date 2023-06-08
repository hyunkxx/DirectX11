#include "stdafx.h"
#include "..\Public\UI_Tip.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "UI_Mouse.h"
#include "UI_MainScreen.h"
#include "UI_Minimap.h"


CUI_Tip::CUI_Tip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Tip::CUI_Tip(const CUI_Tip& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Tip::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Tip::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	Load();
	return S_OK;
}

void CUI_Tip::Start()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pUIMouse = static_cast<CUI_Mouse*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Mouse"));
	SetState(DISABLE);
}

void CUI_Tip::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//if (pGameInstance->InputKey(DIK_NUMPAD1) == KEY_STATE::TAP) // 임시
	//{
	//	SituIndex = CUI_Tip::AREA_OPENING;
	//}
	//if (pGameInstance->InputKey(DIK_NUMPAD2) == KEY_STATE::TAP) // 임시
	//{
	//	m_bUIRender = true;
	//	SituIndex = CUI_Tip::CHAR_YANGYANG;
	//	//디버깅용
	//	for (auto& Chixia : m_ChixiaDescList)
	//	{
	//		Chixia.Color.w = Chixia.fColorA;
	//	}
	//}
	//if (pGameInstance->InputKey(DIK_NUMPAD3) == KEY_STATE::TAP) // 임시
	//{
	//	m_bUIRender = true;
	//	SituIndex = CUI_Tip::CHAR_CHIXIA;
	//	// 디버깅용
	//	for (auto& Yang : m_YangyanDescList)
	//	{
	//		Yang.Color.w = Yang.fColorA;
	//	}
	//}

		switch (SituIndex)
		{
		case Client::CUI_Tip::AREA_OPENING:
		{
			m_bMouseActive = false;
			Area_Opening(TimeDelta);
		}
		break;
		case Client::CUI_Tip::CHAR_YANGYANG:
		{
			if ((true == m_bUIRender) && (AddAlphaW(&m_YangyanDescList, TimeDelta)))
			{
				
				m_pUIMouse->Set_RenderMouse(true);
				m_bMouseActive = true;
				m_bUIRender = false;
				for (auto& Yang : m_YangyanDescList)
				{
					if (Yang.fColorA > Yang.Color.w)
					{
						Yang.fColorA = Yang.Color.w;
					}
				}
			}

			if (false == m_bUIRender)
			{
				if(SelectUI(&m_YangyanDescList[9]))
				{
					// 반짝반짝 추가
					ColorP(&m_YangyanDescList[9], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
					ColorP(&m_YangyanDescList[10], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_YangyanDescList[9].OnRect = true;
					}
				}
				else
				{
					ColorM(&m_YangyanDescList[9], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
					ColorM(&m_YangyanDescList[10], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
				}

				if (true == m_YangyanDescList[9].OnRect)
				{
					m_TimeAcc += (_float)TimeDelta;
					if (MinusAlphaW(&m_YangyanDescList, TimeDelta))
					{
							SetState(DISABLE);
							m_YangyanDescList[9].OnRect = false;
							m_pUIMouse->Set_RenderMouse(false);
							m_TimeAcc = 0.f;
							m_bMouseActive = false;
					}
				}
			}
		}
		break;
		case Client::CUI_Tip::CHAR_CHIXIA:
		{
			if ((true == m_bUIRender) && (AddAlphaW(&m_ChixiaDescList, TimeDelta)))
			{
				m_pUIMouse->Set_RenderMouse(true);
				m_bMouseActive = true;
				m_bUIRender = false;
				for (auto& Chixia : m_ChixiaDescList)
				{
					if (Chixia.fColorA > Chixia.Color.w)
					{
						Chixia.fColorA = Chixia.Color.w;
					}
				}
			}

			if (false == m_bUIRender)
			{
				if (SelectUI(&m_ChixiaDescList[9]))
				{
					// 반짝반짝 추가
					ColorP(&m_ChixiaDescList[9], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
					ColorP(&m_ChixiaDescList[10], _float4(150.f, -30.f, -140.f, 0.f), TimeDelta);
					if (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::TAP)
					{
						m_ChixiaDescList[9].OnRect = true;
					}
				}
				else
				{
					ColorM(&m_ChixiaDescList[9], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
					ColorM(&m_ChixiaDescList[10], _float4(0.f, 0.f, 0.f, 0.f), TimeDelta);
				}

				if (true == m_ChixiaDescList[9].OnRect)
				{
					m_TimeAcc += (_float)TimeDelta;
					if (MinusAlphaW(&m_ChixiaDescList, TimeDelta))
					{
							SetState(DISABLE);
							m_ChixiaDescList[9].OnRect = false;
							m_pUIMouse->Set_RenderMouse(false);
							m_TimeAcc = 0.f;
							m_bMouseActive = false;
					}
				}
			}

		}
		break;
		default:
			break;
		}
	

	UVWave(TimeDelta);
}

void CUI_Tip::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI_POST, this);
}

HRESULT CUI_Tip::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

		switch (SituIndex)
		{
		case Client::CUI_Tip::AREA_OPENING:
		{
			for (_uint i = 0; i <(_uint)m_OpeningDescList.size(); ++i)
			{
				if (FAILED(Setup_ShaderResources(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}

		}
		break;
		case Client::CUI_Tip::CHAR_YANGYANG:
		{
			for (_uint i = 0; i <(_uint)m_YangyanDescList.size(); ++i)
			{
				if (FAILED(Setup_ShaderResourcesYang(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		break;
		case Client::CUI_Tip::CHAR_CHIXIA:
		{
			for (_uint i = 0; i <(_uint)m_ChixiaDescList.size(); ++i)
			{
				if (FAILED(Setup_ShaderResourcesChixia(i)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);
				m_pVIBuffer->Render();
			}
		}
		break;
		default:
			break;
		}


	return S_OK;
}

void CUI_Tip::RenderGUI()
{
}

HRESULT CUI_Tip::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UITIP,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		TEXT("com_vibuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_Tip::Setup_ShaderResources(_uint index)
{

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_OpeningDescList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Title", m_MainMaskNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_TitleUV", &m_fUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_OpeningDescList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_OpeningDescList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_OpeningDescList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_OpeningDescList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_OpeningDescList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_OpeningDescList[index].iPass;

	return S_OK;
}

HRESULT CUI_Tip::Setup_ShaderResourcesYang(_uint index)
{

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_YangyanDescList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Title", m_MainMaskNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_TitleUV", &m_fUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_YangyanDescList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_YangyanDescList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_YangyanDescList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_YangyanDescList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_YangyanDescList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_YangyanDescList[index].iPass;

	return S_OK;
}

HRESULT CUI_Tip::Setup_ShaderResourcesChixia(_uint index)
{

	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_ChixiaDescList[index].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Title", m_MainMaskNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_TitleUV", &m_fUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_ChixiaDescList[index].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_ChixiaDescList[index].fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_ChixiaDescList[index].fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_ChixiaDescList[index].fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_ChixiaDescList[index].fColorA), sizeof(_float))))
		return E_FAIL;

	m_iPass = m_YangyanDescList[index].iPass;

	return S_OK;
}

_bool CUI_Tip::AddAlpha(vector<TIPDESC>* pDesc, _double TimeDelta)
{
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (0.f >(*pDesc)[i].fColorA)
		{
			(*pDesc)[i].fColorA += (_float)TimeDelta * 100.f;
		}
		else
		{
			return true;
		}
	}
	return false;
}

_bool CUI_Tip::MinusAlpha(vector<TIPDESC>* pDesc, _double TimeDelta)
{
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (-255.f < (*pDesc)[i].fColorA)
		{
			(*pDesc)[i].fColorA -= (_float)TimeDelta * 100.f;
		}
		else
		{
			return true;
		}
	}
	return false;
}

_bool CUI_Tip::AddAlphaW(vector<TIPDESC>* pDesc, _double TimeDelta)
{
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if ((*pDesc)[i].Color.w >(*pDesc)[i].fColorA)
		{
			(*pDesc)[i].fColorA += (_float)TimeDelta * 200.f;
		}
		else
		{
			(*pDesc)[i].fColorA = (*pDesc)[i].Color.w;
		}
	}
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (0.1f > abs((*pDesc)[i].Color.w - (*pDesc)[i].fColorA))
		{
			++m_Count;
		}
		if (m_Count == (_int)(*pDesc).size())
		{
			m_Count = 0;
			return true;
		}
	}
	m_Count = 0;
	return false;
}

_bool CUI_Tip::MinusAlphaW(vector<TIPDESC>* pDesc, _double TimeDelta)
{
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (-300.f < (*pDesc)[i].fColorA)
		{
			(*pDesc)[i].fColorA -= (_float)TimeDelta * 200.f;
		}
		else
		{
			(*pDesc)[i].fColorA = -300.f;
		}
	}
	for (_uint i = 0; i < (_int)(*pDesc).size(); ++i)
	{
		if (0.01f > abs(-300.f - (*pDesc)[i].fColorA))
		{
			++m_Count;
		}
		if (m_Count == (_int)(*pDesc).size())
		{
			m_Count = 0;
			return true;
		}
	}
	m_Count = 0;
	return false;
}

void CUI_Tip::ColorP(TIPDESC* pDesc, _float4 fcolor, _double TimeDelta)
{
	if (0.1f < fcolor.x - pDesc->fColorR)
	{
		pDesc->fColorR += fcolor.x * (_float)TimeDelta;
	}
	else
		pDesc->fColorR = fcolor.x;
	if (0.1f < fcolor.y - pDesc->fColorG)
	{
		pDesc->fColorG += fcolor.y * (_float)TimeDelta;
	}
	else
		pDesc->fColorG = fcolor.y;
	if (0.1f < fcolor.z - pDesc->fColorB)
	{
		pDesc->fColorB += fcolor.z * (_float)TimeDelta;
	}
	else
		pDesc->fColorB = fcolor.z;
}

void CUI_Tip::ColorM(TIPDESC* pDesc, _float4 fcolor, _double TimeDelta)
{
	if (0.1f < abs(fcolor.x - pDesc->fColorR))
	{
		pDesc->fColorR -= pDesc->fColorR * (_float)TimeDelta;
	}
	else
		pDesc->fColorR = fcolor.x;
	if (0.1f < abs(fcolor.y - pDesc->fColorG))
	{
		pDesc->fColorG -= pDesc->fColorG * (_float)TimeDelta;
	}
	else
		pDesc->fColorG = fcolor.y;
	if (0.1f < abs(fcolor.z - pDesc->fColorB))
	{
		pDesc->fColorB -= pDesc->fColorB * (_float)TimeDelta;
	}
	else
		pDesc->fColorB = fcolor.z;
}

_bool CUI_Tip::SelectUI(TIPDESC* pDesc)
{
	_float3	fMousePos = m_pUIMouse->Get_MousePos();
	_vector vMouse = XMLoadFloat3(&fMousePos);
	_float Dist = 1.f;
	// 버퍼의 각 꼭지점
	_vector P0 = XMVectorSet(pDesc->fX - pDesc->fWidth * 0.5f, pDesc->fY + pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P1 = XMVectorSet(pDesc->fX + pDesc->fWidth * 0.5f, pDesc->fY + pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P2 = XMVectorSet(pDesc->fX + pDesc->fWidth * 0.5f, pDesc->fY - pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);
	_vector P3 = XMVectorSet(pDesc->fX - pDesc->fWidth * 0.5f, pDesc->fY - pDesc->fHeight * 0.5f, pDesc->fZ, 1.f);

	// UI크기에 맞춰서 범위체크
	if ((XMVectorGetX(P0) < XMVectorGetX(vMouse)) && (XMVectorGetX(P1) > XMVectorGetX(vMouse)))
	{
		if ((XMVectorGetY(P0) > XMVectorGetY(vMouse)) && (XMVectorGetY(P2) < XMVectorGetY(vMouse)))
		{
			m_pUIMouse->Set_Texchange(true);
			return true;
		}
		else
		{
			return false;

		}
	}
	else
	{
		return false;
	}

	return false;
}

void CUI_Tip::UVWave(_double TimeDelta)
{
	m_fUV.x -= (_float)TimeDelta;

}


void CUI_Tip::Load()
{
	_uint index = 5;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/AREA_OPENING%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		TIPDESC Desc;
		ZeroMemory(&Desc, sizeof(TIPDESC));

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);

			if (0 == dwByte)
				break;
		}
		Desc.Color.w = Desc.fColorA;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_OpeningDescList.push_back(Desc);
		CloseHandle(hFile);

	}
	index = 12;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CHAR_YANGYANG%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		TIPDESC Desc;
		ZeroMemory(&Desc, sizeof(TIPDESC));

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);

			if (0 == dwByte)
				break;
		}
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.fZ = 0.01f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_YangyanDescList.push_back(Desc);
		CloseHandle(hFile);

	}

	index = 12;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/CHAR_CHIXIA%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		TIPDESC Desc;
		ZeroMemory(&Desc, sizeof(TIPDESC));

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc.fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorR), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorG), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.fColorB), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc.iTexNum), sizeof(_int), &dwByte, nullptr);

			if (0 == dwByte)
				break;
		}
		Desc.Color.w = Desc.fColorA;
		Desc.fColorA = -255.f;
		Desc.fZ = 0.01f;
		XMStoreFloat4x4(&(Desc.WorldMatrix), XMMatrixScaling(Desc.fWidth, Desc.fHeight, 1.f)
			* XMMatrixTranslation(Desc.fX, Desc.fY, Desc.fZ));
		m_ChixiaDescList.push_back(Desc);
		CloseHandle(hFile);

	}


}

void CUI_Tip::Area_Opening(_double TimeDelta)
{
	if ((true == m_bUIRender) && (AddAlpha(&m_OpeningDescList, TimeDelta)))
	{
		m_bUIRender = false;
	}

	if (false == m_bUIRender)
	{
		m_TimeAcc += (_float)TimeDelta;
	}

	if (3.f < m_TimeAcc)
	{
		if (MinusAlpha(&m_OpeningDescList, TimeDelta))
		{
			m_TimeAcc = 0.f;
			SetState(DISABLE);
		}
	}

}

CUI_Tip* CUI_Tip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Tip* pInstance = new CUI_Tip(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Tip";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Tip::Clone(void* pArg)
{
	CUI_Tip* pInstance = new CUI_Tip(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Tip";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Tip::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);


}


