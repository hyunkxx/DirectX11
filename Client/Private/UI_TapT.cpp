#include "stdafx.h"
#include "..\Public\UI_TapT.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "UI_MainScreen.h"
#include "UI_Mouse.h"

CUI_TapT::CUI_TapT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_TapT::CUI_TapT(const CUI_TapT& rhs)
	: CGameObject(rhs)
{
	for (auto& Desc : rhs.m_DescList)
	{
		m_DescList.push_back(Desc);
	}

}

HRESULT CUI_TapT::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	Load();

	return S_OK;
}

HRESULT CUI_TapT::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CUI_TapT::Start()
{
	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	m_pUIMain = static_cast<CUI_MainScreen*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_MainScreen"));
	m_pUIMouse = static_cast<CUI_Mouse*>(pGameInstance->Find_GameObject(LEVEL_ANYWHERE, L"UI_Mouse"));
}

void CUI_TapT::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	
	if (pGameInstance->InputKey(DIK_TAB) == KEY_STATE::HOLD)
	{
		Add_ShaderY(TimeDelta);
		Add_Alpha(3, 4, TimeDelta*4.f);
		if (0.5f < m_ShaderY)
		{
			Add_Alpha(0, 16, TimeDelta);
			Start_Move(4, 10, TimeDelta);




			//마우스커서
			SerectUI();
			for (_uint i = 4; i < 10; ++i)
			{
				if (true == m_DescList[i]->OnRect)
				{
					m_DescList[1]->iTexNum = m_DescList[i]->iTexNum;
				}


				_float Dist = Distance(vMousePos, XMLoadFloat3(&_float3{ 0.f, 0.f, 0.f }));
				if ((Dist < 110.f) && (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::HOLD))
				{
					m_bHolding = true;
				}

				if ((true == m_bHolding) && (pGameInstance->InputMouse(DIMK_LB) == KEY_STATE::AWAY))
				{
					m_bHolding = false;
					_int Texindex = m_DescList[1]->iTexNum;
					// 포인터 저장해놓고 쓰기
					m_pUIMain->Set_Texchange(Texindex);
					m_DescList[i + 6]->OnRect = true;
					break;
				}
			}
		}

	}
	else
	{
		Minus_Alpha(0, 16, TimeDelta);
		Minus_ShaderY(TimeDelta);
		if (0.9f < m_ShaderY)
		{
			Middle_Move(4, 10, TimeDelta);
		}
		else
		{
			
			End_Move(4, 10, TimeDelta);
		}
	}
	

	m_pUIMouse->Set_Texchange(false);
	if (pGameInstance->InputKey(DIK_TAB) == KEY_STATE::TAP)
	{
		m_pUIMouse->Set_RenderMouse(true);
	}
	if (pGameInstance->InputKey(DIK_TAB) == KEY_STATE::AWAY)
	{
		m_pUIMouse->Set_RenderMouse(false);
	}
}

void CUI_TapT::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_TapT::Render()
{
	
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (0.5f < m_ShaderY)
	{
		_uint Descindex = 0;
		for (auto& Desc : m_DescList)
		{
			if (nullptr != Desc)
			{
				if (FAILED(Setup_ShaderResources(Descindex)))
					return E_FAIL;
				m_pShader->Begin(m_iPass);

				if ((9 < Descindex) && (Descindex < 16))
				{
					if (true == m_DescList[Descindex]->OnRect)
					{
						m_pVIBuffer->Render();
					}
				}
				else
					m_pVIBuffer->Render();
			}
			++Descindex;
		}
	}
	else
	{
		if (FAILED(Setup_ShaderResources(3)))
			return E_FAIL;
		m_pShader->Begin(m_iPass);
		m_pVIBuffer->Render();
	}
	return S_OK;
}

void CUI_TapT::RenderGUI()
{
}

void CUI_TapT::SerectUI()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	if (nullptr != m_pUIMouse)
	{
		_float3	fMousePos = m_pUIMouse->Get_MousePos();
		vMousePos = XMLoadFloat3(&fMousePos);

		if (0 != m_DescList.size())
		{
			for (auto& pCutDesc : m_DescList)
			{
				_float Dist = 1.f;
				// 버퍼의 각 꼭지점
				_vector P0 = XMVectorSet(pCutDesc->fX - pCutDesc->fWidth * 0.5f, pCutDesc->fY + pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);
				_vector P1 = XMVectorSet(pCutDesc->fX + pCutDesc->fWidth * 0.5f, pCutDesc->fY + pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);
				_vector P2 = XMVectorSet(pCutDesc->fX + pCutDesc->fWidth * 0.5f, pCutDesc->fY - pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);
				_vector P3 = XMVectorSet(pCutDesc->fX - pCutDesc->fWidth * 0.5f, pCutDesc->fY - pCutDesc->fHeight * 0.5f, pCutDesc->fZ, 1.f);

				// UI크기에 맞춰서 범위체크
				if ((XMVectorGetX(P0) < XMVectorGetX(vMousePos)) && (XMVectorGetX(P1) > XMVectorGetX(vMousePos)))
				{
					if ((XMVectorGetY(P0) > XMVectorGetY(vMousePos)) && (XMVectorGetY(P2) < XMVectorGetY(vMousePos)))
					{
						pCutDesc->OnRect = true;
						m_pUIMouse->Set_Texchange(true);
					}
					else
					{
						pCutDesc->OnRect = false;

					}
				}
				else
				{
					pCutDesc->OnRect = false;

				}
			}
		}
	}
}





HRESULT CUI_TapT::Add_Components()
{
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/*버퍼 하나당  세이브파일 한개 , 로드할때 파일  하나당 할당 하나. 리스트 원소개수로*/
		
	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UI,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibuffer", (CComponent**)&m_pVIBuffer)))
		return E_FAIL;
	return S_OK;

}


HRESULT CUI_TapT::Setup_ShaderResources(_uint Bufferindex)
{
	
	vector<TAPDESC*>::iterator Desciter = m_DescList.begin();
	Desciter = m_DescList.begin();
	advance(Desciter, Bufferindex);

	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", (*Desciter)->iTexNum)))
			return E_FAIL;
	}

	if (nullptr != (*Desciter))
	{
		XMStoreFloat4x4(&((*Desciter)->WorldMatrix), XMMatrixScaling((*Desciter)->fWidth, (*Desciter)->fHeight, 1.f) *
			XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians((*Desciter)->fDegree))
			* XMMatrixTranslation((*Desciter)->fX, (*Desciter)->fY, (*Desciter)->fZ));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &((*Desciter)->WorldMatrix))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &((*Desciter)->fColorR), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &((*Desciter)->fColorG), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &((*Desciter)->fColorB), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &((*Desciter)->fColorA), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fUV_Y", &(m_ShaderY), sizeof(_float))))
			return E_FAIL;
		
		m_iPass = (*Desciter)->iPass;
	}
	return S_OK;
}


CUI_TapT* CUI_TapT::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TapT* pInstance = new CUI_TapT(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_TapT";
		MESSAGE(message);
		Safe_Release(pInstance);
	}
			
	return pInstance;
}

CGameObject* CUI_TapT::Clone(void* pArg)
{
	CUI_TapT* pInstance = new CUI_TapT(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_TapT";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_TapT::Free()
{
	__super::Free();
	m_pUIMouse = nullptr;
	m_pUIMain = nullptr;
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBuffer);
	
if (!m_bClone)
{
	for (auto& Desc : m_DescList)
	{
		delete Desc;
		Desc = nullptr;
	}
	m_DescList.clear();
}
}

void CUI_TapT::Load()
{
	_uint index = 16;
	for(_uint i =0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/TapT%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		TAPDESC*  Desc = new TAPDESC;
		ZeroMemory(Desc, sizeof(TAPDESC));
		XMStoreFloat4x4(&Desc->WorldMatrix, XMMatrixIdentity());

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc->fX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorA), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bRot), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->PrePos), sizeof(_float2), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->OriPos), sizeof(_float2), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}

		CloseHandle(hFile);

		if (nullptr != Desc)
		{
			m_DescList.push_back(Desc);
		}
	}
}

void CUI_TapT::Rot(TAPDESC* pDesc, _double TimeDelta)
{
_vector		vRight, vUp, vLook;
_matrix WorldMat;
WorldMat = XMLoadFloat4x4(&(pDesc->WorldMatrix));
vRight = WorldMat.r[0];
vUp = WorldMat.r[1];
vLook = WorldMat.r[2];

_matrix		RotationMatrix;

RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(pDesc->fDegree));

vRight = XMVector3TransformNormal(vRight, RotationMatrix);
vUp = XMVector3TransformNormal(vUp, RotationMatrix);
vLook = XMVector3TransformNormal(vLook, RotationMatrix);

WorldMat.r[0] = vRight;
WorldMat.r[1] = vUp;
WorldMat.r[2] = vLook;
XMStoreFloat4x4(&(pDesc->WorldMatrix), WorldMat);
}

void CUI_TapT::Add_Alpha(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i<indexend; ++i)
	{
		if (0.f > m_DescList[i]->fColorA)
		{
			m_DescList[i]->fColorA += (_float)TimeDelta*400.f;
		}
		if (0.0f < m_DescList[i]->fColorA)
			m_DescList[i]->fColorA = 0.0f;
	}
}

void CUI_TapT::Minus_Alpha(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i<indexend; ++i)
	{
		if (-255.0f < m_DescList[i]->fColorA)
		{
			m_DescList[i]->fColorA -= (_float)TimeDelta*400.0f;
		}
		if (-255.0f > m_DescList[i]->fColorA)
			m_DescList[i]->fColorA = -255.0f;
	}
}
void CUI_TapT::Add_ShaderY(_double TimeDelta)
{
	if(1.0f>m_ShaderY)
	m_ShaderY += (_float)TimeDelta;
}

void CUI_TapT::Minus_ShaderY(_double TimeDelta)
{
	if (0.0f < m_ShaderY)
	m_ShaderY -= (_float)TimeDelta;
}
void CUI_TapT::Start_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_DescList[i]->OriPos.x, m_DescList[i]->OriPos.y, 0.01f, 1.f);
		_vector PrePos = XMVectorSet(m_DescList[i]->PrePos.x, m_DescList[i]->PrePos.y, 0.01f, 1.f);
		_vector vDir = OriPos - PrePos;
		_vector CurrentPos = XMVectorSet(m_DescList[i]->fX, m_DescList[i]->fY, 0.01f, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta*30.f;
		_vector pos = CurrentPos;
			pos += (OriPos - CurrentPos) * (_float)TimeDelta * 3.f;
			_float Dist = Distance(XMVectorSet(m_DescList[i+6]->fX, m_DescList[i+6]->fY, 0.01f, 1.f), CurrentPos);
			if (2.f < Dist)
			{
				m_DescList[i]->fX = XMVectorGetX(pos);
				m_DescList[i]->fY = XMVectorGetY(pos);
			}
	}
}
void CUI_TapT::End_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_DescList[i]->OriPos.x, m_DescList[i]->OriPos.y, 0.01f, 1.f);
		_vector PrePos = XMVectorSet(m_DescList[i]->PrePos.x, m_DescList[i]->PrePos.y, 0.01f, 1.f);
		_vector vDir = OriPos - PrePos;
		_vector CurrentPos = XMVectorSet(m_DescList[i]->fX, m_DescList[i]->fY, 0.01f, 1.f);
		CurrentPos -= XMVector4Normalize(vDir) * (_float)TimeDelta*30.f;
		_vector pos = CurrentPos;
		pos += (PrePos - CurrentPos) * (_float)TimeDelta;
		m_DescList[i]->fX = XMVectorGetX(pos);
		m_DescList[i]->fY = XMVectorGetY(pos);
	}
}

_float CUI_TapT::Distance(_fvector TargetPos, _fvector CurrentPos)
{
	return XMVectorGetX(XMVector3Length(TargetPos - CurrentPos));
}

void CUI_TapT::Middle_Move(_uint indexstart, _uint indexend, _double TimeDelta)
{
	for (_uint i = indexstart; i < indexend; ++i)
	{
		_vector OriPos = XMVectorSet(m_DescList[i]->OriPos.x, m_DescList[i]->OriPos.y, 0.01f, 1.f);
		_vector PrePos = XMVectorSet(m_DescList[i]->PrePos.x, m_DescList[i]->PrePos.y, 0.01f, 1.f);
		_vector vDir = OriPos - PrePos;
		_vector CurrentPos = XMVectorSet(m_DescList[i]->fX, m_DescList[i]->fY, 0.01f, 1.f);
		CurrentPos += XMVector4Normalize(vDir) * (_float)TimeDelta*100.f;
		_vector pos = CurrentPos;
		pos += (OriPos - CurrentPos) * (_float)TimeDelta;
		m_DescList[i]->fX = XMVectorGetX(pos);
		m_DescList[i]->fY = XMVectorGetY(pos);
	}

}