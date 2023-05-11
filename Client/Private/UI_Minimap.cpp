#include "stdafx.h"
#include "..\Public\UI_Minimap.h"
#include "GameInstance.h"
#include "UI_Mouse.h"
#include "PlayerGirl.h"
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
		CComponent* pComponent = dynamic_cast<CPlayerGirl*>(pPlayer)->Find_Component(TEXT("Com_Transform"));
		XMStoreFloat4(&fPlayerPos, dynamic_cast<CTransform*>(pComponent)->Get_State(CTransform::STATE_POSITION));

		//미니맵에 출력될 텍스처 uv의 전체 지형에서의 pos X,Z값
		// 플레이어 pos 기준으로 정해줌
		m_fPointLU.x = fPlayerPos.x - 50.f;
		m_fPointLU.y = fPlayerPos.z + 50.f;
		m_fPointRB.x = fPlayerPos.x + 50.f;
		m_fPointRB.y = fPlayerPos.z - 50.f;

		// 스크린좌표상의 uv구하기, 자른 사진과 지형이 1:1이라는 상황
		// dx 좌표계와 uv 좌표계의 y+ 방향이 반대라서 y값은 -곱해줌
		m_TerrainLU.x = m_fPointLU.x / m_fWidthMiniMap + 0.5f;
		m_TerrainLU.y = -m_fPointLU.y / m_fHeightMiniMap + 0.5f;
		m_TerrainRB.x = m_fPointRB.x / m_fWidthMiniMap + 0.5f;
		m_TerrainRB.y = -m_fPointRB.y / m_fHeightMiniMap + 0.5f;

		XMStoreFloat4x4(&m_WorldMatrixMiniMap, XMMatrixScaling(m_fWidthMiniMap, m_fHeightMiniMap, 1.f) * XMMatrixTranslation(m_fXMiniMap, m_fYMiniMap, m_fZMiniMap));
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
			if (nullptr != pDesc)
			{
				_vector Move;
				Move = XMLoadFloat4(&fPlayerPos) - XMLoadFloat4(&fPrePlayerPos);
				
				_float X = 50.f / 300.f + XMVectorGetX(Move);
				_float Y = 50.f / 300.f + XMVectorGetZ(Move);

				pDesc->fX = 50.f / 300.f - 540.f + OriPos.x - X;
				pDesc->fY = 50.f / 300.f + 260.f + OriPos.z - Y;
				
			}
		}
	


		// 플레이어 아이콘 돌리는 기능
		{
			_float4x4 PlayerWorldMat = dynamic_cast<CTransform*>(pComponent)->Get_WorldMatrix();
			_vector vPlayerLook = XMLoadFloat4x4(&PlayerWorldMat).r[2];
			XMVectorSetY(vPlayerLook,0.f);
			vPlayerLook = XMVector3Normalize(vPlayerLook); //플레이어 룩 노멀라이즈

			 _float Degree =  XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), vPlayerLook)); // 회전각도

			 _float fRadian = acosf(Degree);
			 if (0.f < XMVectorGetX(vPlayerLook))
			 {
				 fRadian *= -1.f;
			 }

			 if (0.98 > Degree)
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
			if (0.98 > Degree)
			{
				_matrix ScaleMat = XMMatrixScaling(m_fWidthDefaultIcon[1], m_fHeightDefaultIcon[1], 1.f);

				_matrix RotaMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), fRadian);

				_matrix TransMat = XMMatrixTranslation(m_fXDefaultIcon[1], m_fYDefaultIcon[1], m_fZDefaultIcon[1]);


				XMStoreFloat4x4(&m_WorldMatrixDefaultIcon[1], ScaleMat * RotaMat * TransMat);
			}
		}
	}

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
	}

	//디폴트아이콘
	for (_uint i = 0; i < 2; ++i)
	{
		if ((false == m_bNull) && (nullptr != m_pVIBufferDefaultIcon[i]))
		{
			if (FAILED(Setup_ShaderResourcesDefaultIcon(i)))
				return E_FAIL;
			m_pShader->Begin(1);
			m_pVIBufferDefaultIcon[i]->Render();
		}
	}

	_uint Bufferindex = 0;
	for (auto& pBuffer : m_BufferList)
	{
		if (nullptr != pBuffer)
		{
			if (FAILED(Setup_ShaderResourcesIcon(Bufferindex)))
				return E_FAIL;
			m_pShader->Begin(1);
			pBuffer->Render();
		}
		++Bufferindex;
	}



	return S_OK;
}

void CUI_Minimap::RenderGUI()
{
	ImGui::Begin("MainScreen ID");
	ImGui::InputInt("ID", &m_iObjectID);

	ImGui::End();

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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibufferDefaultIcon0", (CComponent**)&m_pVIBufferDefaultIcon[0])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::VIBUFFER_RECT,
		L"com_vibufferDefaultIcon1", (CComponent**)&m_pVIBufferDefaultIcon[1])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UIMAPICON,
		TEXT("com_texIcon"), (CComponent**)&m_pTexIcon)))
		return E_FAIL;

	return S_OK;

}

HRESULT CUI_Minimap::Setup_ShaderResourcesMiniMap()
{
	if (nullptr != m_pTexMiniMap)
	{
		if (FAILED(m_pTexMiniMap->Setup_ShaderResource(m_pShader, "g_MyTexture")))
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
		if (FAILED(m_pTexIcon->Setup_ShaderResource(m_pShader, "g_MyTexture", m_IconDescList[Bufferindex]->iTexNum)))
			return E_FAIL;
	}
	

	XMStoreFloat4x4(&(m_IconDescList[Bufferindex]->WorldMatrix), XMMatrixScaling(m_IconDescList[Bufferindex]->fWidth, m_IconDescList[Bufferindex]->fHeight, 1.f)
		* XMMatrixTranslation(m_IconDescList[Bufferindex]->fX, m_IconDescList[Bufferindex]->fY, m_IconDescList[Bufferindex]->fZ));

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_IconDescList[Bufferindex]->WorldMatrix))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_IconDescList[Bufferindex]->fColorA), sizeof(_float))))
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
	Safe_Release(m_pVIBufferMiniMap);
	Safe_Release(m_pTexDefaultIcon);
	Safe_Release(m_pVIBufferIcon);

	for (_uint i = 0; i < 2; ++i)
	{
		Safe_Release(m_pVIBufferDefaultIcon[i]);
	}


	for (auto& Buffer : m_BufferList)
	{
		Safe_Release(Buffer);
		Buffer = nullptr;
	}
	m_BufferList.clear();

	for (auto& Desc : m_IconDescList)
	{
		delete Desc;
		Desc = nullptr;
	}
	m_IconDescList.clear();

	delete m_IconDesc;
	m_IconDesc = nullptr;


}




