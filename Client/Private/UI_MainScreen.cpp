#include "stdafx.h"
#include "..\Public\UI_MainScreen.h"
#include "GameInstance.h"
#include "RenderTarget.h"
#include "UI_Mouse.h"
#include "Character.h"
#include "Terrain.h"
#include "UI_Terminal.h"

CUI_MainScreen::CUI_MainScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
}

CUI_MainScreen::CUI_MainScreen(const CUI_MainScreen& rhs)
	: CGameObject(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
{
}

HRESULT CUI_MainScreen::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_MainScreen::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Load();
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

 	if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UIMINIMAP, TEXT("layer_UI"), TEXT("UI_Minimap"))))
		return E_FAIL;
	Safe_Release(pGameInstance);
	return S_OK;
}

void CUI_MainScreen::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pGameObject = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("layer_UI"), TEXT("UI_Terminal"));
	
		if (pGameInstance->InputKey(DIK_ESCAPE) == KEY_STATE::TAP) //&& (false == pGameObject->IsActive()))
		{
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, OBJECT::UITERMINAL, TEXT("layer_UI"), TEXT("UI_Terminal"))))
				return;
		}

	if (0 != m_CutDescList.size())
	{
		list<CUTRECT*>::iterator Desciter = m_CutDescList.begin();
		advance(Desciter, 14);
		if (false == (*Desciter)->bCoolTime)
		{
			if (pGameInstance->InputKey(DIK_1) == KEY_STATE::TAP)
			{
				m_iPrePlayer = m_iCurrentPlayer;
				m_iCurrentPlayer = 1;

				if (2 == m_iPrePlayer) // 2번에서 1번 플레이어 선택
				{
					(*Desciter)->bCoolTime = true; //14 쿨타임 시작

					Desciter = m_CutDescList.begin();
					advance(Desciter, 10);
					(*Desciter)->fColorACut = -255.f; // 번호사라짐

					++Desciter; //11
					(*Desciter)->fColorACut = 0.f; //번호나타남

					Desciter = m_CutDescList.begin();
					advance(Desciter, 16);
					(*Desciter)->fColorACut = 0.f; //연기 나타남

					++Desciter; //17
					(*Desciter)->fColorACut = -255.f; //연기 사라짐
				}
				if (3 == m_iPrePlayer) // 3번에서 1번 플레이어 선택
				{
					Desciter = m_CutDescList.begin();
					advance(Desciter, 15);
					(*Desciter)->bCoolTime = true; //15 쿨타임 시작 

					Desciter = m_CutDescList.begin();
					advance(Desciter, 10);
					(*Desciter)->fColorACut = -255.f; // 번호 사라짐

					Desciter = m_CutDescList.begin();
					advance(Desciter, 12);
					(*Desciter)->fColorACut = 0.f; // 번호 나타남

					Desciter = m_CutDescList.begin();
					advance(Desciter, 16);
					(*Desciter)->fColorACut = 0.f; // 연기 나타남

					Desciter = m_CutDescList.begin();
					advance(Desciter, 18);
					(*Desciter)->fColorACut = -255.f; // 연기 사라짐
				}
				// 스킬 3개 그림 달라짐
				Desciter = m_CutDescList.begin();
				advance(Desciter, 5);
				(*Desciter)->iTexNum = 72;
				++Desciter; //6
				(*Desciter)->iTexNum = 75;
				++Desciter; //7
				(*Desciter)->iTexNum = 76;
			}
		}
		Desciter = m_CutDescList.begin();
		advance(Desciter, 13);
		if (false == (*Desciter)->bCoolTime)
		{
			if (pGameInstance->InputKey(DIK_2) == KEY_STATE::TAP)
			{
				m_iPrePlayer = m_iCurrentPlayer;
				m_iCurrentPlayer = 2;

				if (1 == m_iPrePlayer) // 1번에서 2번 플레이어 선택
				{
					(*Desciter)->bCoolTime = true; // 13 쿨타임 시작

					Desciter = m_CutDescList.begin();
					advance(Desciter, 11);
					(*Desciter)->fColorACut = -255.f; //번호 사라짐

					Desciter = m_CutDescList.begin();
					advance(Desciter, 10);
					(*Desciter)->fColorACut = 0.f; // 번호 나타남
				
					Desciter = m_CutDescList.begin();
					advance(Desciter, 16);
					(*Desciter)->fColorACut = -255.f; //연기 사라짐

					Desciter = m_CutDescList.begin();
					advance(Desciter, 17);
					(*Desciter)->fColorACut = 0.f; // 연기 나타남

				}
				if (3 == m_iPrePlayer) // 3번에서 2번 플레이어 선택
				{
					Desciter = m_CutDescList.begin();
					advance(Desciter, 15);
					(*Desciter)->bCoolTime = true;

					Desciter = m_CutDescList.begin();
					advance(Desciter, 11);
					(*Desciter)->fColorACut = -255.f; // 번호 사라짐

					++Desciter;
					(*Desciter)->fColorACut = 0.f; // 번호 나타남

					Desciter = m_CutDescList.begin();
					advance(Desciter, 17);
					(*Desciter)->fColorACut = 0.f; // 연기 나타남
					++Desciter; //18
					(*Desciter)->fColorACut = -255.f; // 연기 사라짐
				}

				// 스킬 4개 그림 달라짐
				Desciter = m_CutDescList.begin();
				advance(Desciter, 5);
				(*Desciter)->iTexNum = 78;
				++Desciter; //6
				(*Desciter)->iTexNum = 79;
				++Desciter; //7
				(*Desciter)->iTexNum = 80;

			}
		}

		Desciter = m_CutDescList.begin();
		advance(Desciter, 13);
		if (false == (*Desciter)->bCoolTime)
		{
			if (pGameInstance->InputKey(DIK_3) == KEY_STATE::TAP)
			{
				m_iPrePlayer = m_iCurrentPlayer;
				m_iCurrentPlayer = 3;

				if (1 == m_iPrePlayer) // 1번에서 3번 플레이어 선택
				{
					(*Desciter)->bCoolTime = true;

					Desciter = m_CutDescList.begin();
					advance(Desciter, 10);
					(*Desciter)->fColorACut = 0.f; // 번호 나타남

					Desciter = m_CutDescList.begin();
					advance(Desciter, 12);
					(*Desciter)->fColorACut = -255.f; // 번호 사라짐

					Desciter = m_CutDescList.begin();
					advance(Desciter, 16);
					(*Desciter)->fColorACut = -255.f; //연기 사라짐

					Desciter = m_CutDescList.begin();
					advance(Desciter, 18);
					(*Desciter)->fColorACut = 0.f; // 연기 나타남
				}

				if (2 == m_iPrePlayer) // 2번에서 3번 플레이어 선택
				{
					Desciter = m_CutDescList.begin();
					advance(Desciter, 14);
					(*Desciter)->bCoolTime = true;

					Desciter = m_CutDescList.begin();
					advance(Desciter, 11);
					(*Desciter)->fColorACut = 0.f; //번호 나타남
					++Desciter;
					(*Desciter)->fColorACut = -255.f; // 번호 사라짐

					Desciter = m_CutDescList.begin();
					advance(Desciter, 17);
					(*Desciter)->fColorACut = -255.f; // 연기 사라짐

					++Desciter; //18
					(*Desciter)->fColorACut = 0.f; // 연기 나타남
					
				}
				// 스킬 4개 그림 달라짐
				Desciter = m_CutDescList.begin();
				advance(Desciter, 5);
				(*Desciter)->iTexNum = 82;
				++Desciter; //6
				(*Desciter)->iTexNum = 83;
				++Desciter; //7
				(*Desciter)->iTexNum = 84;
			}
		}
	}

	for (auto& Desc : m_CutDescList)
	{
		//마우스커서
		SerectUI();

		if (true == Desc->bSizeXAnimM)
			SizeXAnimM(Desc, TimeDelta);
		if (true == Desc->bSizeXAnimP)
			SizeXAnimP(Desc, TimeDelta);
		if (true == Desc->bSizeYAnimM)
			SizeYAnimM(Desc, TimeDelta);
		if (true == Desc->bSizeYAnimP)
			SizeYAnimP(Desc, TimeDelta);
		if (true == Desc->bAlphaM)
			AlphaM(Desc, TimeDelta);
		if (true == Desc->bAlphaP)
			AlphaP(Desc, TimeDelta);
		if (true == Desc->bCoolTime)
			CoolTime(Desc, TimeDelta);
		if (true == Desc->bRot)
			Rot(Desc, TimeDelta);
	}

	UVWave(TimeDelta);
	if (pGameInstance->InputKey(DIK_SPACE) == KEY_STATE::HOLD)
	{
		UVCircle(TimeDelta);
	}

	if (true == m_bHit)
	{
		HPBar(TimeDelta);
	}
	if (true == m_bRedStart)
	{
		HPRedBar(TimeDelta);
	}

	Safe_Release(pGameInstance);
}

void CUI_MainScreen::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_MainScreen::Render()
{
	
	if (FAILED(__super::Render()))
		return E_FAIL;

	_uint Bufferindex = 0;
	for (auto& pBuffer : m_BufferCutList)
	{
		if (nullptr != pBuffer)
		{
			if (FAILED(Setup_ShaderResourcesCut(Bufferindex)))
				return E_FAIL;

			m_pShader->Begin(m_iPass);

			list<CUTRECT*>::iterator Desciter = m_CutDescList.begin();
			advance(Desciter, Bufferindex);

			if(4 == (*Desciter)->iPass)
			{
				if(true == (*Desciter)->bCoolTime)
				pBuffer->Render();
			}
			else 
			{
					pBuffer->Render();
			}
		}

		++Bufferindex;
	}


	return S_OK;
}

void CUI_MainScreen::RenderGUI()
{
	ImGui::Begin("MainScreen ID");
	ImGui::InputInt("ID", &m_iObjectID);

ImGui::End();
		
}

void CUI_MainScreen::SerectUI()
{
	POINT		MousePos{};

	GetCursorPos(&MousePos);
	ScreenToClient(g_hWnd, &MousePos);

	UINT           pNumViewports;
	m_pContext->RSGetViewports(&pNumViewports, NULL);
	D3D11_VIEWPORT*		pViewPort = new D3D11_VIEWPORT[pNumViewports];
	m_pContext->RSGetViewports(&pNumViewports, pViewPort);

	float Viewport_Width = (float)pViewPort[0].Width;
	float Viewport_Height = (float)pViewPort[0].Height;

	Safe_Delete_Array(pViewPort);

	_float3	fMousePos;
	fMousePos.x = MousePos.x - (Viewport_Width * 0.5f);
	fMousePos.y = -MousePos.y + (Viewport_Height * 0.5f);
	fMousePos.z = 0.f;

	_vector vMousePos = XMLoadFloat3(&fMousePos);

	if (0 != m_CutDescList.size())
	{
		for (auto& pCutDesc : m_CutDescList)
		{
			_float Dist = 1.f;
			// 버퍼의 각 꼭지점
			_vector P0 = XMVectorSet(pCutDesc->fXCut - pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut + pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);
			_vector P1 = XMVectorSet(pCutDesc->fXCut + pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut + pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);
			_vector P2 = XMVectorSet(pCutDesc->fXCut + pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut - pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);
			_vector P3 = XMVectorSet(pCutDesc->fXCut - pCutDesc->fWidthCut * 0.5f, pCutDesc->fYCut - pCutDesc->fHeightCut * 0.5f, pCutDesc->fZCut, 1.f);

			// UI크기에 맞춰서 범위체크
			if ((XMVectorGetX(P0) < XMVectorGetX(vMousePos)) && (XMVectorGetX(P1) > XMVectorGetX(vMousePos)))
			{
				if ((XMVectorGetY(P0) > XMVectorGetY(vMousePos)) && (XMVectorGetY(P2) < XMVectorGetY(vMousePos)))
				{
					CGameInstance*		pGameInstance = CGameInstance::GetInstance();
					Safe_AddRef(pGameInstance);

					if (pGameInstance->InputMouse(Engine::MOUSE_KEYSTATE::DIMK_LB)) // 범위 안에서 클릭하면
					{
						//마우스가 올려져 있는 텍스처의 패스가 4(쿨)이라면
						if ((4 == pCutDesc->iPass) && (false == pCutDesc->bCoolTime))
						{
							pCutDesc->bCoolTime = true;
						}
					}
					Safe_Release(pGameInstance);
				}

			}
		}
	}
}





HRESULT CUI_MainScreen::Add_Components()
{
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/*버퍼 하나당  세이브파일 한개 , 로드할때 파일  하나당 할당 하나. 리스트 원소개수로*/
		
	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UI,
		TEXT("com_texFunc"), (CComponent**)&m_pTexFunc)))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_MainScreen::Setup_ShaderResourcesCut(_uint Bufferindex)
{
	
	list<CUTRECT*>::iterator Desciter = m_CutDescList.begin();
	advance(Desciter, Bufferindex);
	// iter를 Bufferindex만큼 ++연산


	// 쿨타임시 보낼 텍스처
	if ((13 == Bufferindex)&&(*Desciter)->bCoolTime)
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture2", 3))) // 3->캐릭터텍스처의 main%d
			return E_FAIL;
	}
	if ((14 == Bufferindex) && (*Desciter)->bCoolTime)
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture2", 4)))
			return E_FAIL;
	}
	if ((15 == Bufferindex) && (*Desciter)->bCoolTime)
	{
		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture2", 5)))
			return E_FAIL;
	}
	

	Desciter = m_CutDescList.begin();
	advance(Desciter, Bufferindex);

	if (nullptr != m_pTexFunc)
	{

		if (FAILED(m_pTexFunc->Setup_ShaderResource(m_pShader, "g_MyTexture", (*Desciter)->iTexNum)))
			return E_FAIL;
	}

	XMStoreFloat4x4(&((*Desciter)->WorldMatrixCut), XMMatrixScaling((*Desciter)->fWidthCut, (*Desciter)->fHeightCut, 1.f) *
		XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians((*Desciter)->fDegree))	
		* XMMatrixTranslation((*Desciter)->fXCut, (*Desciter)->fYCut, (*Desciter)->fZCut));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	if (nullptr != (*Desciter))
	{

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &((*Desciter)->WorldMatrixCut))))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fLU", &((*Desciter)->LUCutUV), sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fRB", &((*Desciter)->RBCutUV), sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_UV", &m_fUV, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fColorR", &((*Desciter)->fColorRCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &((*Desciter)->fColorGCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &((*Desciter)->fColorBCut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &((*Desciter)->fColorACut), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fCoolTime", &((*Desciter)->Cool), sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_RadianAcc", &m_RadianAcc, sizeof(_float))))
			return E_FAIL;

		if (true == m_bRedStart)
		{
			if (FAILED(m_pShader->SetRawValue("g_fRedBar", &m_fRedBar, sizeof(_float))))
				return E_FAIL;
		}
		if (FAILED(m_pShader->SetRawValue("g_fLoading", &m_fWhiteBar, sizeof(_float))))
			return E_FAIL;

		m_iPass = (*Desciter)->iPass;
	}
	return S_OK;
}


CUI_MainScreen* CUI_MainScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MainScreen* pInstance = new CUI_MainScreen(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_MainScreen";
		MESSAGE(message);
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject* CUI_MainScreen::Clone(void* pArg)
{
	CUI_MainScreen* pInstance = new CUI_MainScreen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_MainScreen";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MainScreen::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexFunc);
	Safe_Release(m_pVIBufferCut);

	for (auto& Buffer : m_BufferCutList)
	{
		Safe_Release(Buffer);
		Buffer = nullptr;
	}
	m_BufferCutList.clear();

	for (auto& Desc : m_CutDescList)
	{
		delete Desc;
		Desc = nullptr;
	}
	m_CutDescList.clear();

	delete m_CutDesc;
	m_CutDesc = nullptr;

	delete CurrentCutDesc;
	CurrentCutDesc = nullptr;

}

void CUI_MainScreen::Load()
{
	_uint index = 35;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/MainScreen%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		CUTRECT*  Desc = new CUTRECT;
		ZeroMemory(Desc, sizeof(CUTRECT));
		XMStoreFloat4x4(&Desc->WorldMatrixCut, XMMatrixIdentity());

		_ulong dwByte = 0;
		while (true)
		{
			ReadFile(hFile, &(Desc->fXCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fYCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fZCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fWidthCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fHeightCut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorACut), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Duration), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->TimeAcc), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->Cool), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfY), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedfZ), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedWidth), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedHeight), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fSpeedAlpha), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeXAnimP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeXAnimM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeYAnimP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bSizeYAnimM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaM), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bAlphaP), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bCoolTime), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bRot), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
			if (0 >= dwByte)
				break;
		}

		CloseHandle(hFile);
		if (109 == Desc->iTexNum)
		{
			Desc->fColorRCut = 255.f;
			Desc->fColorGCut = 255.f;
			Desc->fColorBCut = 255.f;
		}
		if (32 == i)
		{
			Desc->fColorRCut = 255.f;
			Desc->fColorGCut = -255.f;
			Desc->fColorBCut = -255.f;
		}

		if (nullptr != Desc)
		{
			CurrentCutDesc = Desc;
			m_CutDesc = Desc;
			m_CutDescList.push_back(Desc);

			m_pVIBufferCut = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
			if (nullptr == m_pVIBufferCut)
				return;

			m_BufferCutList.push_back(m_pVIBufferCut);
		}
	}
	
}

void CUI_MainScreen::SizeXAnimP(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bSizeXAnimP = false;
	}


	_float fWidth = pDesc->fWidthCut + pDesc->fSpeedWidth * (_float)TimeDelta;
	fWidth = pDesc->WorldMatrixCut._11 = pDesc->fWidthCut;
	pDesc->WorldMatrixCut._22 = pDesc->fHeightCut;
}

void CUI_MainScreen::SizeXAnimM(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bSizeXAnimM = false;
	}


	_float fWidth = pDesc->fWidthCut - pDesc->fSpeedWidth * (_float)TimeDelta;
	pDesc->fWidthCut = pDesc->WorldMatrixCut._11 = fWidth;
	pDesc->WorldMatrixCut._22 = pDesc->fHeightCut;
}

void CUI_MainScreen::SizeYAnimP(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bSizeYAnimP = false;
	}

	_float fHeight = pDesc->fHeightCut + pDesc->fSpeedHeight * (_float)TimeDelta;
	pDesc->WorldMatrixCut._11 = pDesc->fWidthCut;
	pDesc->fHeightCut = pDesc->WorldMatrixCut._22 = fHeight;
}

void CUI_MainScreen::SizeYAnimM(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bSizeYAnimM = false;
	}

	_float fHeight = pDesc->fHeightCut - pDesc->fSpeedHeight * (_float)TimeDelta;
	pDesc->WorldMatrixCut._11 = pDesc->fWidthCut;
	pDesc->fHeightCut = pDesc->WorldMatrixCut._22 = fHeight;
}

void CUI_MainScreen::AlphaM(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bAlphaM = false;
	}

	pDesc->fColorACut -= pDesc->fSpeedAlpha * (_float)TimeDelta;
}

void CUI_MainScreen::AlphaP(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bAlphaP = false;
	}

	pDesc->fColorACut += pDesc->fSpeedAlpha * (_float)TimeDelta;
}

void CUI_MainScreen::CoolTime(CUTRECT* pDesc, _double TimeDelta)
{
	pDesc->TimeAcc += (_float)TimeDelta;

	if (pDesc->TimeAcc > pDesc->Duration)
	{
		pDesc->TimeAcc = 0.f;
		pDesc->bCoolTime = false;
		pDesc->Cool = 0.f;
	}

	if (1.f > pDesc->Cool)
	{
		pDesc->Cool += (_float)TimeDelta / pDesc->Duration; /*uv좌표 기준으로 m_Cool를 1로 잡고 원하는 쿨타임 == 매 틱/ 전체시간, 텍스처 크기로 잡을 필요x */
	}
}

void CUI_MainScreen::Rot(CUTRECT* pDesc, _double TimeDelta)
{

_vector		vRight, vUp, vLook;
_matrix WorldMat;
WorldMat = XMLoadFloat4x4(&(pDesc->WorldMatrixCut));
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
XMStoreFloat4x4(&(pDesc->WorldMatrixCut), WorldMat);
}

void CUI_MainScreen::UVWave(_double TimeDelta)
{
	m_fUV.x += (_float)TimeDelta;

}

void CUI_MainScreen::UVCircle(_double TimeDelta)
{
	m_RadianAcc += (_float)TimeDelta;

}

void CUI_MainScreen::HPBar(_double TimeDelta)
{ //83->빨 82->흰
	//흰색 줄어듬
	list<CUTRECT*>  ::iterator iter = m_CutDescList.begin();
	advance(iter, 31);
	if (0.f < m_fWhiteBar)
	{
		if (0 < m_Damage) // 힐
		{
			if(m_PreHp + m_Damage <m_CurrentHp) // 힐 받은만큼만 참
			{
				m_bHit = false;
				m_PreHp = m_RedDamageACC = m_CurrentHp;
			}
			m_CurrentHp += m_Damage * (_float)TimeDelta;
			m_fWhiteBar = m_CurrentHp / (*iter)->fWidthCut;
		}
		else
		{
			m_CurrentHp += m_Damage;
			m_fWhiteBar = m_CurrentHp / (*iter)->fWidthCut;
			m_PreHp = m_CurrentHp;
			m_bHit = false;
			m_bRedStart = true;
		}
	}


}
void CUI_MainScreen::HPRedBar(_double TimeDelta)
{
	list<CUTRECT*>  ::iterator iter = m_CutDescList.begin();
	advance(iter, 32);
	if (m_CurrentHp < m_RedDamageACC)
	{
	
		m_RedDamageACC += (m_CurrentHp - m_RedDamageACC) * (_float)TimeDelta;
		m_fRedBar = m_RedDamageACC / (*iter)->fWidthCut;
	

	}
	else
	{
		// 피가 다 닳았으면
		m_bRedStart = false;

	}
}

void CUI_MainScreen::Set_Texchange(_int Texindex)
{
	list<CUTRECT*>  ::iterator iter = m_CutDescList.begin();
	advance(iter, 4);
	(*iter)->iTexNum = Texindex;

}
