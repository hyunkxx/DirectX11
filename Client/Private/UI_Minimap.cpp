#include "stdafx.h"
#include "..\Public\UI_Minimap.h"
#include "GameInstance.h"
#include "UI_Mouse.h"
#include "P_PlayerGirl.h"
#include "Terrain.h"

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

	//Load();
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
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	}
	return S_OK;
}

void CUI_Minimap::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 미니맵 
	_float3 fTerrainScale;
	CGameObject* pPlayer = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Player"));
	// 가져온 오브젝트들이 준비가 되면 랜더 돌릴 수 있음-> 랜더조건
	if(nullptr != pPlayer)
	{
 		m_bNull = false;
		CTransform* pComponent = static_cast<CTransform*>(dynamic_cast<CP_PlayerGirl*>(pPlayer)->Find_Component(TEXT("Com_Transform")));
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
		XMStoreFloat4x4(&m_WorldMatrixFrame, XMMatrixScaling(m_fWidthFrame, m_fHeightFrame, 1.f) * XMMatrixTranslation(m_fXFrame, m_fYFrame, m_fZFrame));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


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

		for (auto& pDesc : m_IconDescList)
		{
			_vector vWorldPos, vObjectPos;
			_float4 fObjectPos;
			_float2 IconPos;
			vWorldPos = XMVectorSet(pDesc.fX, pDesc.fY, pDesc.fZ, 1.f);
			vObjectPos = XMVector3TransformCoord(vWorldPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_VIEW));
			vObjectPos = XMVector3TransformCoord(vObjectPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_PROJ));
			XMStoreFloat4(&fObjectPos, vObjectPos);
			IconPos.x = (fObjectPos.x) * (0.5f * g_iWinSizeX);
			IconPos.y = (fObjectPos.y) * (0.5f * g_iWinSizeY);

			IconPos.x = IconPos.x * 0.2f * (m_fWidthMiniMap / g_iWinSizeX);
			IconPos.y = IconPos.y * 0.2f * (m_fHeightMiniMap / g_iWinSizeY);

			_vector vec1 = XMVectorSet(m_fXMiniMap, m_fYMiniMap, 0.f , 1.f);
			_vector vec2 = XMVectorSet(IconPos.x + m_fXMiniMap, IconPos.y + m_fYMiniMap, 0.f, 1.f);
			_float Dist = XMVectorGetX(XMVector4Length(vec2 - vec1));

			if (m_fWidthMiniMap / 2.f < Dist)
			{
				pDesc.bRender = true;
			}
			else
			{
				pDesc.bRender = true;
			}



			XMStoreFloat4x4(&(pDesc.WorldMatrix), XMMatrixScaling(pDesc.fWidth, pDesc.fHeight, 1.f)
				* XMMatrixTranslation(IconPos.x + m_fXMiniMap, IconPos.y + m_fYMiniMap, 0.f));
			XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
		}
	


		// 플레이어 아이콘 돌리는 기능
		{
			_float4x4 PlayerWorldMat = dynamic_cast<CTransform*>(pComponent)->Get_WorldMatrix();
			_vector vPlayerLook = XMLoadFloat4x4(&PlayerWorldMat).r[2];
			XMVectorSetY(vPlayerLook,0.f);
			vPlayerLook = XMVector3Normalize(vPlayerLook); //플레이어 룩 노멀라이즈
			 _float Get =  XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), vPlayerLook)); // 회전각도

			 _float fRadian = acosf(Get);
			 _float Angle = (fRadian*180.f) / 3.141592f;
			 if (0.f < XMVectorGetX(vPlayerLook))
			 {
				 fRadian *= -1.f;
			 }

			 if (0.98 > Get)
			 {
				
				 _matrix ScaleMat = XMMatrixScaling(m_fWidthDefaultIcon[0], m_fHeightDefaultIcon[0], 1.f);

				 _matrix RotaMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), fRadian);

				 _matrix TransMat= XMMatrixTranslation(m_fXDefaultIcon[0], m_fYDefaultIcon[0], m_fZDefaultIcon[0]);


				 XMStoreFloat4x4(&m_WorldMatrixDefaultIcon[0], ScaleMat * RotaMat * TransMat);
			 }
		}

		// 시야범위 돌리는 기능
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
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
	}


	// 메인화면시야아이콘


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

		if (-400.f > IconPos.x)
		{
			_float Dist = -400.f - IconPos.x;
			IconPos.x = IconPos.x + Dist;
		}
		if (400.f <= IconPos.x)
		{
			_float Dist = 400.f - IconPos.x;
			IconPos.x = IconPos.x + Dist;
		}

		if (-180.f >= IconPos.y)
		{
			_float Dist = -180.f - IconPos.y;
			IconPos.y = IconPos.y + Dist;
		}
		if (270.f <= IconPos.y)
		{
			_float Dist = 270.f - IconPos.y;
			IconPos.y = IconPos.y + Dist;
		}
		CPipeLine* pPipe = CPipeLine::GetInstance();
		_matrix CamMat = pPipe->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
		_vector CamLook = CamMat.r[2];
		_vector CamPos = CamMat.r[3];
		_vector vNormailzeDir = XMVector4Normalize(Desc.vObjectPos - CamPos);
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
			IconPos.y = -180.f;
		}


		XMStoreFloat4x4(&(Desc.IconWorldMatrix), XMMatrixScaling(IconSize.x, IconSize.y, 1.f)
			* XMMatrixTranslation(IconPos.x, IconPos.y, 0.f));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
	}

	DecideRender();







	Safe_Release(pGameInstance);
}

void CUI_Minimap::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Minimap::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

	// 미니맵
	if ((false == m_bNull) && (nullptr != m_pVIBufferMiniMap))
	{
		if (FAILED(Setup_ShaderResourcesMiniMap()))
			return E_FAIL;
		m_pShader->Begin(5);
		m_pVIBufferMiniMap->Render();

		if (FAILED(Setup_ShaderResourcesFrame()))
			return E_FAIL;
		m_pShader->Begin(m_FramePass);
		m_pVIBufferMiniMap->Render();

	}

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
		if (true == Desc.bRender)
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
		if (true == Desc.bRender)
		{
			if (FAILED(Setup_ShaderResourcesIcons(&Desc)))
				return E_FAIL;
			m_pShader->Begin(1);
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

	//미니맵
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UIMAP,
		TEXT("com_texMap"), (CComponent**)&m_pTexMiniMap)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibufferMiniMap", (CComponent**)&m_pVIBufferMiniMap)))
		return E_FAIL;

	//디폴트아이콘
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UIMAPDEFAULT,
		TEXT("com_texDefaultIcon"), (CComponent**)&m_pTexDefaultIcon)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UIMAPICON,
		TEXT("com_texIcon"), (CComponent**)&m_pTexIcon)))
		return E_FAIL;

	return S_OK;

}

void CUI_Minimap::Set_ObjectPos(_int Index, _fvector vObjectPos)
{
	for (auto& Desc : m_DescList)
	{
		if (Index == Desc.Index)
		{
			m_DescList[Index].vObjectPos = vObjectPos;
			break;
		}
	}
}

_int CUI_Minimap::Add_Icon(_fvector vObjectPos, _int TextureNum)
{
	_matrix WorldMat = XMMatrixIdentity();
	_float4x4 fWorldMat;

	MAPDESC MiniMapDesc;
	ZeroMemory(&MiniMapDesc,sizeof(MAPDESC));

	MiniMapDesc.fX = XMVectorGetX(vObjectPos);
	MiniMapDesc.fY = XMVectorGetY(vObjectPos);
	MiniMapDesc.fZ = XMVectorGetZ(vObjectPos);
	MiniMapDesc.fHeight = 15.f;
	MiniMapDesc.fWidth = 15.f;
	if (44 == TextureNum)
	{
		MiniMapDesc.iTexNum = 5;
	}
	if (45 == TextureNum)
	{
		MiniMapDesc.iTexNum = 35;
	}
	XMStoreFloat4x4(&MiniMapDesc.WorldMatrix, WorldMat);
	m_IconDescList.push_back(MiniMapDesc);
	
	XMStoreFloat4x4(&fWorldMat, WorldMat);
	ICONDESC Desc;
	Desc.vObjectPos = vObjectPos;
	Desc.TextureNum = TextureNum;
	Desc.IconWorldMatrix = fWorldMat;
	Desc.Index = Index;
	m_DescList.push_back(Desc);
	++Index;
	return Desc.Index;


}

void CUI_Minimap::DecideRender()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CPipeLine* pPipe = CPipeLine::GetInstance();
	_matrix CamMat = pPipe->Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_vector CamPos = CamMat.r[3];
	for (auto& Desc : m_DescList)
	{
		_float Length = XMVectorGetX(XMVector4Length(Desc.vObjectPos - CamPos));

		if (40.f <= Length)
		{
			Desc.bRender = true;
		}
		else
		{
			Desc.bRender = false;
		}
	}

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
	if (nullptr != m_pTexMiniMap)
	{
		if (FAILED(m_pTexMiniMap->Setup_ShaderResource(m_pShader, "g_MyTexture")))
			return E_FAIL;
		if (FAILED(m_pTexMiniMap->Setup_ShaderResource(m_pShader, "g_MyTexture2", 1)))
			return E_FAIL;
	}


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

HRESULT CUI_Minimap::Setup_ShaderResourcesFrame()
{
	if (nullptr != m_pTexMiniMap)
	{
		if (FAILED(m_pTexMiniMap->Setup_ShaderResource(m_pShader, "g_MyTexture", 1)))
			return E_FAIL;
	}


	if (nullptr != m_pVIBufferMiniMap)
	{

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &m_WorldMatrixFrame)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->SetRawValue("g_fLU", &m_TerrainLU, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fRB", &m_TerrainRB, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorR", &m_fColorRFrame, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorG", &m_fColorGFrame, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorB", &m_fColorBFrame, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &m_fColorAFrame, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}



HRESULT CUI_Minimap::Setup_ShaderResourcesDefaultIcon(_uint Bufferindex)
{
	if (nullptr != m_pTexDefaultIcon)
	{
		if (FAILED(m_pTexDefaultIcon->Setup_ShaderResource(m_pShader, "g_MyTexture", Bufferindex)))
			return E_FAIL;
	}

		if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &m_WorldMatrixDefaultIcon[Bufferindex])))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->SetRawValue("g_fColorA", &m_fColorADefaultIcon[Bufferindex], sizeof(_float))))
			return E_FAIL;
	
	return S_OK;
}


HRESULT CUI_Minimap::Setup_ShaderResourcesIcon(_uint Bufferindex)
{
	
	if (nullptr != m_pTexIcon)
	{
		if (FAILED(m_pTexIcon->Setup_ShaderResource(m_pShader, "g_MyTexture", m_IconDescList[Bufferindex].iTexNum)))
			return E_FAIL;
	}
	
	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_IconDescList[Bufferindex].WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
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
	Safe_Release(m_pTexMiniMap);
	Safe_Release(m_pTexDefaultIcon);
	Safe_Release(m_pVIBufferMiniMap);
	

}




