#include "stdafx.h"
#include "..\Public\UI_Minimap.h"
#include "GameMode.h"
#include "GameInstance.h"
#include "UI_Mouse.h"
#include "P_PlayerGirl.h"
#include "P_Yangyang.h"
#include "P_Chixia.h"
#include "PlayerState.h"
#include "Terrain.h"
#include "TerminalUI.h"
#include "UI_Tip.h"
#include "UI_MerchantMen.h"
#include "UI_Souvenir.h"
#include "UI_Panhua.h"
#include "UI_Cooking.h"

CUI_Minimap::CUI_Minimap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Minimap::CUI_Minimap(const CUI_Minimap& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Minimap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Minimap::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	for (_uint i = 0; i < 2; ++i)
	{
		XMStoreFloat4x4(&(m_WorldMatrixDefaultIcon[i]), XMMatrixScaling(m_fWidthDefaultIcon[i], m_fHeightDefaultIcon[i], 1.f)
			* XMMatrixTranslation(m_fXDefaultIcon[i], m_fYDefaultIcon[i], m_fZDefaultIcon[i]));
	}
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	return S_OK;
}

void CUI_Minimap::Start()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	m_pPlayerStateClass = static_cast<CPlayerState*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"CharacterState"));
	m_pTerminalUI = static_cast<CTerminalUI*>(pGameInstance->Find_GameObject(LEVEL_STATIC, L"Terminal"));
}

void CUI_Minimap::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	CCharacter* pActiveCharacter = m_pPlayerStateClass->Get_ActiveCharacter();
	CTransform* pComponent = static_cast<CTransform*>(pActiveCharacter->Find_Component(TEXT("Com_Transform")));
	OtherobjIsActive();

	// 미니맵 
	// 가져온 오브젝트들이 준비가 되면 랜더 돌릴 수 있음-> 랜더조건
		if (nullptr != pComponent)
		{
			m_bNull = false;
			//CTransform* pComponent = static_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")));
			XMStoreFloat4(&fPlayerPos, pComponent->Get_State(CTransform::STATE_POSITION));
			//미니맵에 출력될 텍스처 uv의 전체 지형에서의 pos X,Z값
			// 플레이어 pos 기준으로 정해줌
			m_fPointLU.x = fPlayerPos.x - 200.f;
			m_fPointLU.y = fPlayerPos.z + 200.f;
			m_fPointRB.x = fPlayerPos.x + 200.f;
			m_fPointRB.y = fPlayerPos.z - 200.f;

			// 스크린좌표상의 uv구하기, 자른 사진과 지형이 1:1이라는 상황
			// dx 좌표계와 uv 좌표계의 y+ 방향이 반대라서 y값은 -곱해줌
			m_TerrainLU.x = m_fPointLU.x / m_fWidthMiniMap + 0.5f;
			m_TerrainLU.y = -m_fPointLU.y / m_fHeightMiniMap + 0.5f;
			m_TerrainRB.x = m_fPointRB.x / m_fWidthMiniMap + 0.5f;
			m_TerrainRB.y = -m_fPointRB.y / m_fHeightMiniMap + 0.5f;

			XMStoreFloat4x4(&m_WorldMatrixMiniMap, XMMatrixScaling(m_fWidthMiniMap, m_fHeightMiniMap, 1.f) * XMMatrixTranslation(m_fXMiniMap, m_fYMiniMap, m_fZMiniMap));



			for (_uint i = 0; i < 2; ++i)
			{
				m_fDefaultIconPointLU[i].x = m_fPointLU.x;
				m_fDefaultIconPointLU[i].y = m_fPointLU.y;
				m_fDefaultIconPointRB[i].x = m_fPointRB.x;
				m_fDefaultIconPointRB[i].y = m_fPointRB.y;

				m_DefaultIconLU[i].x = m_TerrainLU.x;
				m_DefaultIconLU[i].y = m_TerrainLU.y;
				m_DefaultIconRB[i].x = m_TerrainRB.x;
				m_DefaultIconRB[i].y = m_TerrainRB.y;
			}

			//미니맵아이콘
			for (auto& pDesc : m_IconDescList)
			{
				_vector vPlayerPos, vWorldPos;
				vWorldPos = XMVectorSet(pDesc.fX, pDesc.fY, pDesc.fZ, 1.f);
				vPlayerPos = XMLoadFloat4(&fPlayerPos);
				_float X = XMVectorGetX(vPlayerPos - vWorldPos);
				_float Y = XMVectorGetZ(vPlayerPos - vWorldPos);
				pDesc.Dist = XMVectorGetX(XMVector4Length(vPlayerPos - vWorldPos));

				if ((190.f <= pDesc.Dist) || ((_int)pDesc.IconLU.x == -1))
					pDesc.bRender = false;
				if ((190.f > pDesc.Dist) && ((_int)pDesc.IconLU.x != -1))
					pDesc.bRender = true;

				XMStoreFloat4x4(&(pDesc.WorldMatrix), XMMatrixScaling(pDesc.fWidth, pDesc.fHeight, 1.f)
					* XMMatrixTranslation(-540.f - X * (m_fWidthMiniMap / g_iWinSizeX) * 1.5f
						, 260.f - Y  * (m_fHeightMiniMap / g_iWinSizeY) * 1.5f, 0.01f));
			}



			// 플레이어 아이콘 돌리는 기능
			{
				_float4x4 PlayerWorldMat = pComponent->Get_WorldMatrix();
				_vector vPlayerLook = XMLoadFloat4x4(&PlayerWorldMat).r[2];
				XMVectorSetY(vPlayerLook, 0.f);
				vPlayerLook = XMVector3Normalize(vPlayerLook); //플레이어 룩 노멀라이즈
				_float Get = XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), vPlayerLook)); // 회전각도

				_float fRadian = acosf(Get);
				_float Angle = (fRadian*180.f) / 3.141592f;
				if (0.f < XMVectorGetX(vPlayerLook))
				{
					fRadian *= -1.f;
				}

				if (0.99 > Get)
				{

					_matrix ScaleMat = XMMatrixScaling(m_fWidthDefaultIcon[0], m_fHeightDefaultIcon[0], 1.f);

					_matrix RotaMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), fRadian);

					_matrix TransMat = XMMatrixTranslation(m_fXDefaultIcon[0], m_fYDefaultIcon[0], m_fZDefaultIcon[0]);


					XMStoreFloat4x4(&m_WorldMatrixDefaultIcon[0], ScaleMat * RotaMat * TransMat);
				}
			}
		}
	
	
		// 시야범위 돌리는 기능
		{
			_float4x4 CamMat = pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW);
			_vector vCamLook = XMLoadFloat4x4(&CamMat).r[2];
			XMVectorSetY(vCamLook, 0.f);
			vCamLook = XMVector3Normalize(vCamLook); //플레이어 룩 노멀라이즈

			_float Degree = XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), vCamLook)); // 회전각도

			_float fRadian = acosf(Degree);
			if (0.f < XMVectorGetX(vCamLook))
			{
				fRadian *= -1.f;
			}
			if (0.99 > Degree)
			{
				_matrix ScaleMat = XMMatrixScaling(m_fWidthDefaultIcon[1], m_fHeightDefaultIcon[1], 1.f);

				_matrix RotaMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), fRadian);

				_matrix TransMat = XMMatrixTranslation(m_fXDefaultIcon[1], m_fYDefaultIcon[1], m_fZDefaultIcon[1]);


				XMStoreFloat4x4(&m_WorldMatrixDefaultIcon[1], ScaleMat * RotaMat * TransMat);
			}
		}
	


	// 메인화면아이콘

	for (auto& Desc : m_DescList)
	{
		_vector vObjectPos;
		_float4 fObjectPos;
		_float2 IconPos;
		vObjectPos = XMVector3TransformCoord(Desc.vObjectPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_VIEW));
		vObjectPos = XMVector3TransformCoord(vObjectPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_PROJ));
		XMStoreFloat4(&fObjectPos, vObjectPos);
		IconPos.x = (fObjectPos.x) * (0.5f * g_iWinSizeX);
		IconPos.y = (fObjectPos.y) * (0.5f * g_iWinSizeY);

		if (-620.f > IconPos.x)
		{
			_float Dist = -620.f - IconPos.x;
			IconPos.x = IconPos.x + Dist;
		}
		if (620.f <= IconPos.x)
		{
			_float Dist = 620.f - IconPos.x;
			IconPos.x = IconPos.x + Dist;
		}

		if (-250.f >= IconPos.y)
		{
			_float Dist = -250.f - IconPos.y;
			IconPos.y = IconPos.y + Dist;
		}
		if (320.f <= IconPos.y)
		{
			_float Dist = 320.f - IconPos.y;
			IconPos.y = IconPos.y + Dist;
		}
		_matrix CamMat = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
		_vector CamLook = CamMat.r[2];
		_vector CamPos = CamMat.r[3];
		_float Dist = XMVectorGetX(XMVector4Length(Desc.vObjectPos - CamPos));
		_vector vNormailzeDir = XMVector4Normalize(XMVectorSetY(Desc.vObjectPos - CamPos, 0.f));
		XMVectorSetY(CamLook, 0.f);
		_vector vNormailzeLook = XMVector4Normalize(CamLook);


		// 내적결과는 -1~1사이 
		_float Get = XMVectorGetX(XMVector4Dot(vNormailzeLook, vNormailzeDir));
		_float fRadian = acosf(Get);
		//각도 = (라디안 * 180) / π
		_float Angle = (fRadian*180.f) / 3.141592f;
		if ((90.f <= Angle) && (270.f >= Angle))       //1이면 같은 방향 0보다 작으면 반대방향
		{
			IconPos.x *= -1.f;
			IconPos.y = -210.f;
		}
		Desc.Dist = Dist;
		XMStoreFloat4x4(&(Desc.IconWorldMatrix), XMMatrixScaling(30.f, 30.f, 1.f)
			* XMMatrixTranslation(IconPos.x, IconPos.y, 0.f));
	}

}

void CUI_Minimap::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Minimap::Render()
{

	if (true == m_bRender)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;
		//디폴트아이콘
		for (_uint i = 0; i < 2; ++i)
		{
			if (false == m_bNull)
			{
				if (FAILED(Setup_ShaderResourcesDefaultIcon(i)))
					return E_FAIL;
				m_pShader->Begin(1);
				m_pVIBufferMiniMap->Render();
			}
		}


		//미니맵아이콘
		_uint Descindex = 0;
		for (auto& Desc : m_IconDescList)
		{
			if (true == Desc.bRender && 190.f > Desc.Dist)
			{
				if (FAILED(Setup_ShaderResourcesIcon(Descindex)))
					return E_FAIL;
				m_pShader->Begin(1);
				m_pVIBufferMiniMap->Render();
			}
			++Descindex;
		}

		//메인화면아이콘
		for (auto& Desc : m_DescList)
		{
			if ((true == Desc.bRender) && (Desc.Dist > 50.f) && (Desc.Dist < 100.f))
			{
				if (FAILED(Setup_ShaderResourcesIcons(&Desc)))
					return E_FAIL;
				m_pShader->Begin(1);
				m_pVIBufferMiniMap->Render();
			}
		}

		// 미니맵
		if (false == m_bNull)
		{
			if (FAILED(Setup_ShaderResourcesMiniMap()))
				return E_FAIL;
			m_pShader->Begin(16);
			m_pVIBufferMiniMap->Render();
		}
	}
	return S_OK;
}

void CUI_Minimap::RenderGUI()
{
}


HRESULT CUI_Minimap::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibufferMiniMap", (CComponent**)&m_pVIBufferMiniMap)))
		return E_FAIL;

	//디폴트아이콘
	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UIMAPDEFAULT,
		TEXT("com_texDefaultIcon"), (CComponent**)&m_pTexDefaultIcon)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ANYWHERE, TEXTURE::UIMAPICON,
		TEXT("com_texIcon"), (CComponent**)&m_pTexIcon)))
		return E_FAIL;

	return S_OK;

}

void CUI_Minimap::OtherobjIsActive()
{
	CGameMode* pGM = CGameMode::GetInstance();
	if (pGM->GetMouseActive())
		m_bRender = false;
	else
		m_bRender = true;

	if (m_pTerminalUI != nullptr)
	{
		if (m_pTerminalUI->IsActive())
			m_bRender = false;
	}

}

void CUI_Minimap::Set_ObjectPos(_int Index, _fvector vObjectPos)
{
	m_DescList[Index].vObjectPos = vObjectPos;
}

_bool CUI_Minimap::GetRenderState(_int Index)
{
	return m_IconDescList[Index].bRender;
}

void CUI_Minimap::SetRender(_int index, _bool bRender)
{
	
	if (false == bRender)
	{
		m_IconDescList[index].IconLU.x = -1.1f;
		m_DescList[index].bRender = bRender;
	}
	else
	{
		m_IconDescList[index].IconLU.x = 0.f;
		m_IconDescList[index].bRender = bRender; //미니맵 아이콘
		m_DescList[index].bRender = bRender; // 메인아이콘
	}
}

_int CUI_Minimap::Add_Icon(_fvector vObjectPos, _int TextureNum)
{
	_matrix WorldMat = XMMatrixIdentity();
	_float4x4 fWorldMat;

	MAPDESC MiniMapDesc;
	ZeroMemory(&MiniMapDesc, sizeof(MAPDESC));
	//미니아이콘
	MiniMapDesc.fX = XMVectorGetX(vObjectPos);
	MiniMapDesc.fY = XMVectorGetY(vObjectPos);
	MiniMapDesc.fZ = XMVectorGetZ(vObjectPos);
	MiniMapDesc.fHeight = 15.f;
	MiniMapDesc.fWidth = 15.f;
	switch (TextureNum)
	{
	case ICONNUM::MOSTER:
	{
		MiniMapDesc.iTexNum = 44;
	}
	break;
	case ICONNUM::BOSS:
	{
		MiniMapDesc.iTexNum = 1;
	}
	break;
	case ICONNUM::BOX:
	{
		MiniMapDesc.iTexNum = 35;
	}
	break;
	
	case ICONNUM::MERCHANTMEN:
	{
		MiniMapDesc.iTexNum = 25;
	}
	break;
	case ICONNUM::SOUVENIR:
	{
		MiniMapDesc.iTexNum = 31;
	}
	break;
	case ICONNUM::PANHUA:
	{
		MiniMapDesc.iTexNum = 32;
	}
	break;
	case ICONNUM::POTAL:
	{
		MiniMapDesc.iTexNum = 9;
	}
	break;
	}

	XMStoreFloat4x4(&MiniMapDesc.WorldMatrix, WorldMat);
	m_IconDescList.push_back(MiniMapDesc);


	//메인아이콘
	XMStoreFloat4x4(&fWorldMat, WorldMat);
	ICONDESC Desc;
	Desc.vObjectPos = vObjectPos;
	Desc.TextureNum = MiniMapDesc.iTexNum;
	Desc.IconWorldMatrix = fWorldMat;
	Desc.Index = Index;
	m_DescList.push_back(Desc);
	++Index;
	return Desc.Index;


}

void CUI_Minimap::DecideRender()
{
	for (auto& Desc : m_DescList)
		Desc.bRender = true;

}

HRESULT CUI_Minimap::Setup_ShaderResourcesIcons(ICONDESC* pDesc)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pTexIcon->Setup_ShaderResource(m_pShader, "g_MyTexture", pDesc->TextureNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(pDesc->IconWorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Minimap::Setup_ShaderResourcesMiniMap()
{
	if (FAILED(m_pTexDefaultIcon->Setup_ShaderResource(m_pShader, "g_MyTexture", 2)))
		return E_FAIL;
	if (nullptr != m_pVIBufferMiniMap)
	{

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &m_WorldMatrixMiniMap)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fLU", &m_TerrainLU, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fRB", &m_TerrainRB, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &m_fColorRMiniMap, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &m_fColorGMiniMap, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &m_fColorBMiniMap, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &m_fColorAMiniMap, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CUI_Minimap::Setup_ShaderResourcesDefaultIcon(_uint Bufferindex)
{
	if (FAILED(m_pTexDefaultIcon->Setup_ShaderResource(m_pShader, "g_MyTexture", Bufferindex)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &m_WorldMatrixDefaultIcon[Bufferindex])))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &m_fColorADefaultIcon[Bufferindex], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &m_fColorADefaultIcon[Bufferindex], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &m_fColorADefaultIcon[Bufferindex], sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &m_fColorADefaultIcon[Bufferindex], sizeof(_float))))
		return E_FAIL;

	return S_OK;
}


HRESULT CUI_Minimap::Setup_ShaderResourcesIcon(_uint Bufferindex)
{

	if (FAILED(m_pTexIcon->Setup_ShaderResource(m_pShader, "g_MyTexture", m_IconDescList[Bufferindex].iTexNum)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_IconDescList[Bufferindex].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_IconDescList[Bufferindex].fColorA), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_IconDescList[Bufferindex].fColorA), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_IconDescList[Bufferindex].fColorA), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_IconDescList[Bufferindex].fColorA), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI_Minimap* CUI_Minimap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Minimap* pInstance = new CUI_Minimap(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Minimap";
		MESSAGE(message);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Minimap::Clone(void* pArg)
{
	CUI_Minimap* pInstance = new CUI_Minimap(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Minimap";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Minimap::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexIcon);
	Safe_Release(m_pTexDefaultIcon);
	Safe_Release(m_pVIBufferMiniMap);


}




