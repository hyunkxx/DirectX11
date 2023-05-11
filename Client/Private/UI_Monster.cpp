#include "stdafx.h"
#include "..\Public\UI_Monster.h"
#include "GameInstance.h"
#include "UI_MainScreen.h"
#include "UI_Mouse.h"
#include "PlayerGirl.h"
#include "DynamicCamera.h"
CUI_Monster::CUI_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Monster::CUI_Monster(const CUI_Monster& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Monster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Monster::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	Load();
	
	return S_OK;
}

void CUI_Monster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (auto& Desc : m_DescList)
	{
		

		CGameObject* pPlayer = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Player"));
		// 가져온 오브젝트들이 준비가 되면 랜더 돌릴 수 있음-> 랜더조건
		if (nullptr != pPlayer)
		{
			_float4 fPlayerPos;
			CComponent* pComponent = dynamic_cast<CPlayerGirl*>(pPlayer)->Find_Component(TEXT("Com_Transform"));
			XMStoreFloat4(&fPlayerPos, dynamic_cast<CTransform*>(pComponent)->Get_State(CTransform::STATE_POSITION));


			
				_float4 fCamPos = pGameInstance->Get_CamPosition();
				_vector vCamPos = XMLoadFloat4(&fCamPos);
				_vector vUIPos = XMVectorSet(fPlayerPos.x + Desc->fX, fPlayerPos.y + Desc->fY, fPlayerPos.z + Desc->fZ, 1.f);
				_vector vLook = vCamPos - vUIPos;
				vLook = XMVector3Normalize(vLook);
				_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
				_vector vUp = XMVector3Cross(vLook, vRight);
				_float3 vScale = _float3(Desc->fWidth, Desc->fHeight, 1.f);

				vRight = (XMVector3Normalize(vRight) * vScale.x);
				vUp = (XMVector3Normalize(vUp) * vScale.y);
				vLook = (XMVector3Normalize(vLook) * vScale.z);

				_matrix DescMat = XMMatrixIdentity();
				DescMat.r[0] = vRight;
				DescMat.r[1] = vUp;
				DescMat.r[2] = vLook;
				DescMat.r[3] = XMVectorSet(fPlayerPos.x + Desc->fX, fPlayerPos.y + Desc->fY, fPlayerPos.z + Desc->fZ, fPlayerPos.w);

				XMStoreFloat4x4(&Desc->WorldMatrix, DescMat);

		}

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

void CUI_Monster::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Monster::Render()
{
	
	if (FAILED(__super::Render()))
		return E_FAIL;
	_int index = 0;
	for (auto& Desc : m_DescList)
	{
		if (FAILED(Setup_ShaderResources(index)))
			return E_FAIL;
		m_pShader->Begin(Desc->iPass);
		m_pVIBuffer->Render();
		++index;
	}
	return S_OK;
}

void CUI_Monster::RenderGUI()
{
	ImGui::Begin("Monster ID");
	ImGui::InputInt("ID", &m_iObjectID);

	ImGui::InputFloat("Damage", &m_Damage);
	if (ImGui::Button("Hit")) { m_bHit = true; }
	ImGui::Separator();

ImGui::End();
		
}



HRESULT CUI_Monster::Add_Components()
{
 	if (FAILED(__super::Add_Component(LEVEL_STATIC, COMPONENT::RENDERER,
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/*버퍼 하나당  세이브파일 한개 , 로드할때 파일  하나당 할당 하나. 리스트 원소개수로*/
		
	if (FAILED(__super::Add_Component(LEVEL_STATIC, SHADER::UI,
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXTURE::UIFIGHT,
		TEXT("com_texture"), (CComponent**)&m_pTexture)))
		return E_FAIL;


	return S_OK;

}


HRESULT CUI_Monster::Setup_ShaderResources(_int index)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr == m_pShader)
		return E_FAIL;

	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index]->iTexNum)))
			return E_FAIL;
	}

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index]->WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index]->fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index]->fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index]->fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index]->fColorA), sizeof(_float))))
		return E_FAIL;

	if (true == m_bRedStart)
	{
		if (FAILED(m_pShader->SetRawValue("g_fRedBar", &m_fRedBar, sizeof(_float))))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetRawValue("g_fwhiteBar", &m_fWhiteBar, sizeof(_float))))
		return E_FAIL;



	Safe_Release(pGameInstance);
	return S_OK;
}


CUI_Monster* CUI_Monster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Monster* pInstance = new CUI_Monster(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CUI_Monster";
		MESSAGE(message);
		Safe_Release(pInstance);
	}
			
	return pInstance;
}

CGameObject* CUI_Monster::Clone(void* pArg)
{
	CUI_Monster* pInstance = new CUI_Monster(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CUI_Monster";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Monster::Free()
{
	__super::Free();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);

	for (auto& Buffer : m_BufferList)
	{
		Safe_Release(Buffer);
		Buffer = nullptr;
	}
	m_BufferList.clear();

	for (auto& Desc : m_DescList)
	{
		delete Desc;
		Desc = nullptr;
	}
	m_DescList.clear();

	CurrentDesc = nullptr;

}

void CUI_Monster::HPBar(_double TimeDelta)
{
	if (0.f < m_fWhiteBar)
	{
		if (0 < m_Damage) // 힐
		{
			if (m_PreHp + m_Damage < m_CurrentHp) // 힐 받은만큼만 참
			{
				m_bHit = false;
				m_PreHp = m_RedDamageACC = m_CurrentHp;
			}
			m_CurrentHp += m_Damage * (_float)TimeDelta;
			m_fWhiteBar = m_CurrentHp / m_HP;
		}
		else
		{
			m_CurrentHp += m_Damage;
			m_fWhiteBar = m_CurrentHp / m_HP;  // 현재체력/전체체력 
			m_PreHp = m_CurrentHp;
			m_bHit = false;
			m_bRedStart = true;
		}
	}
}

void CUI_Monster::HPRedBar(_double TimeDelta)
{
	if (m_CurrentHp < m_RedDamageACC)
	{
		m_RedDamageACC += (m_CurrentHp - m_RedDamageACC) * (_float)TimeDelta;
		m_fRedBar = m_RedDamageACC / m_HP;
	}
	else
	{
		// 피가 다 닳았으면
		m_bRedStart = false;

	}
}


void CUI_Monster::Load()
{
	_uint index = 4;
	for (_uint i = 0; i<index; ++i)
	{
		TCHAR	szFileName[128] = L"";
		wsprintf(szFileName, L"../../Data/UI/UIMonster%d.dat", i);
		HANDLE hFile = CreateFile(
			szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		MONSTERDESC*  Desc = new MONSTERDESC;
		ZeroMemory(Desc, sizeof(MONSTERDESC));
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
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bRot), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}

		CloseHandle(hFile);

		if (nullptr != Desc)
		{
			if (2 == i)
			{
				Desc->fColorR = 255.f;
				Desc->fColorG = -255.f;
				Desc->fColorB = -255.f;
			}
			CurrentDesc = Desc;
			m_DescList.push_back(Desc);

			m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
			if (nullptr == m_pVIBuffer)
				return;

			m_BufferList.push_back(m_pVIBuffer);
		}
	}

}