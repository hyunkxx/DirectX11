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
	for (auto& Buffer : rhs.m_BufferList)
	{
		m_BufferList.push_back(Buffer);
		Safe_AddRef(Buffer);
	}
	for (auto& Desc : rhs.m_DescList)
	{
		m_DescList.push_back(Desc);
	}
}

HRESULT CUI_Monster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Load();
	return S_OK;
}

HRESULT CUI_Monster::Initialize(void * pArg)
{
 	if (FAILED(__super::Initialize(pArg))) // 타입이랑 레벨 인자로 받아서 해당 버퍼만 랜더on 시키기
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	
	return S_OK;
}

void CUI_Monster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CommonHP();
	CommonLevel();

	switch (m_MonsterType)
	{
	case Client::CUI_Monster::MONSTERTYPE::TYPE0: //레벨,hp
	{
		m_DescList[7]->bRender = false;
		m_DescList[8]->bRender = false;
	}
	break;
	case Client::CUI_Monster::MONSTERTYPE::TYPE1: // 레벨, hp, 아이콘, 추가방어바
	{

		m_MonsterUV.x += (_float)TimeDelta*0.3f;
		//m_MonsterGauge += (_float)TimeDelta*0.2f;
		m_DescList[7]->bRender = true;
		m_DescList[8]->bRender = true;
	}
	break;
	case Client::CUI_Monster::MONSTERTYPE::BOSS:
	{
		m_MonsterUV.x += (_float)TimeDelta*0.3f;
		//m_MonsterGauge += (_float)TimeDelta*0.2f;

		_int LevelTen = m_MonsterLevel / 10;
		_int LevelOne = m_MonsterLevel % 10; // 0 = 33 1 = 34

		if (1> LevelTen)
		{
			//11,12,13
			m_DescList[12]->bRender = false;
			m_DescList[13]->iTexNum = LevelOne + 33;
		}
		//만약 두자리일때 좌측 위치
		else
		{
			//6번 그대로 5번 b랜더 true
			m_DescList[12]->bRender = true;
			m_DescList[12]->iTexNum = LevelTen + 33;
			m_DescList[13]->iTexNum = LevelOne + 33;
		}


		if (8 < m_DescList.size())
		{
			for (_uint i = 9; i < (_uint)m_DescList.size(); ++i)
			{
				XMStoreFloat4x4(&(m_DescList[i]->WorldMatrix), XMMatrixScaling(m_DescList[i]->fWidth, m_DescList[i]->fHeight, 1.f) *
					XMMatrixTranslation(m_DescList[i]->fX, m_DescList[i]->fY, m_DescList[i]->fZ));
				XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
				XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
			}
		}

	}
	break;
	default:
		break;
	}


	if (true == m_bHit)
	{
		HPBar(TimeDelta);
	}
	if (true == m_bRedStart)
	{
		HPRedBar(TimeDelta);
	}
}

void CUI_Monster::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	
	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Monster::Render()
{
	
	for (_uint i = 0; i < 4; ++i)
	{
		if (true == m_DescList[i]->bRender)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;
			if (FAILED(Setup_ShaderResourcesHP(i)))
				return E_FAIL;
			m_pShader->Begin(m_DescList[i]->iPass);
			m_BufferList[i]->Render();
		}
	}

	for (_uint i = 4; i < 8; ++i)
	{
		if (true == m_DescList[i]->bRender)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;
			if (FAILED(Setup_ShaderResources(i)))
				return E_FAIL;
			m_pShader->Begin(m_DescList[i]->iPass);
			m_BufferList[i]->Render();
		}
	}


	for (_uint i = 8; i < 9; ++i)
	{
		if (true == m_DescList[i]->bRender)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;
			if (FAILED(Setup_ShaderResourcesMask(i)))
				return E_FAIL;
			m_pShader->Begin(m_DescList[i]->iPass);
			m_BufferList[i]->Render();
		}
	}



	for (_uint i = 9; i < (_uint)m_DescList.size(); ++i)
	{
		if (true == m_DescList[i]->bRender)
		{
			if (FAILED(__super::Render()))
				return E_FAIL;
			if (FAILED(Setup_ShaderResourcesBoss(i)))
				return E_FAIL;
			m_pShader->Begin(m_DescList[i]->iPass);
			m_BufferList[i]->Render();
		}
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


HRESULT CUI_Monster::Setup_ShaderResourcesHP(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index]->iTexNum)))
			return E_FAIL;
	}

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index]->WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
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

	return S_OK;
}

HRESULT CUI_Monster::Setup_ShaderResources(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index]->iTexNum)))
			return E_FAIL;
	}

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index]->WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index]->fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index]->fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index]->fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index]->fColorA), sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Monster::Setup_ShaderResourcesMask(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index]->iTexNum)))
			return E_FAIL;
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Mask", m_MaskTextureNum)))
			return E_FAIL;
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Mask2", m_MaskTextureNum2)))
			return E_FAIL;
	}

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index]->WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index]->fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index]->fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index]->fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index]->fColorA), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_MonUV", &(m_MonsterUV), sizeof(_float2))))
		return E_FAIL;
	//if (FAILED(m_pShader->SetRawValue("g_MonsterGauge", &(m_MonsterGauge), sizeof(_float)))) 
	//	return E_FAIL;


	return S_OK;
}


HRESULT CUI_Monster::Setup_ShaderResourcesBoss(_int index)
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (nullptr != m_pTexture)
	{

		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_MyTexture", m_DescList[index]->iTexNum)))
			return E_FAIL;
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Mask", m_MaskTextureNum)))
			return E_FAIL;
		if (FAILED(m_pTexture->Setup_ShaderResource(m_pShader, "g_Mask2", m_MaskTextureNum2)))
			return E_FAIL;
	}

	if (FAILED(m_pShader->SetMatrix("g_MyWorldMatrix", &(m_DescList[index]->WorldMatrix))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_MyViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_MyProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_fColorR", &(m_DescList[index]->fColorR), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorG", &(m_DescList[index]->fColorG), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorB", &(m_DescList[index]->fColorB), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_fColorA", &(m_DescList[index]->fColorA), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_MonUV", &(m_MonsterUV), sizeof(_float2))))
		return E_FAIL;
	//if (FAILED(m_pShader->SetRawValue("g_MonsterGauge", &(m_MonsterGauge), sizeof(_float)))) 
	//	return E_FAIL;

	if (true == m_bRedStart)
	{
		if (FAILED(m_pShader->SetRawValue("g_fRedBar", &m_fRedBar, sizeof(_float))))
			return E_FAIL;
	}
	if (FAILED(m_pShader->SetRawValue("g_fwhiteBar", &m_fWhiteBar, sizeof(_float))))
		return E_FAIL;

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
	Safe_Release(m_pTexture);
	m_pVIBuffer = nullptr;
	Safe_Release(m_pVIBuffer);

	
		for (auto& Buffer : m_BufferList)
		{
			Safe_Release(Buffer);
		}
		m_BufferList.clear();
	

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


void CUI_Monster::CommonHP()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < 4; ++i)
	{
		m_DescList[i]->bRender = true;
		CGameObject* pPlayer = pGameInstance->Find_GameObject(LEVEL_GAMEPLAY, TEXT("Player"));
		// 가져온 오브젝트들이 준비가 되면 랜더 돌릴 수 있음-> 랜더조건
		if (nullptr != dynamic_cast<CPlayerGirl*>(pPlayer)) // 다이나믹캐스트 조건 검사완료했으므로 아래에서는 스테틱캐스트로 변경
		{
			CTransform* pComponent = static_cast<CTransform*>(static_cast<CPlayerGirl*>(pPlayer)->Find_Component(TEXT("Com_Transform")));	 // 가져오는건 무조건 트랜스폼->스테틱캐스트 후 애초에 트랜스폼*으로 받음
			_float4 fPlayerPos;
			_vector vPlayerPos;
			vPlayerPos = pComponent->Get_State(CTransform::STATE_POSITION); // 바꾸려면 여기서 위치조정
			XMStoreFloat4(&fPlayerPos, vPlayerPos);
			fPlayerPos.y = fPlayerPos.y + 1.8f;
			vPlayerPos = XMLoadFloat4(&fPlayerPos);
			vPlayerPos = XMVector3TransformCoord(vPlayerPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_VIEW));
			vPlayerPos = XMVector3TransformCoord(vPlayerPos, pGameInstance->Get_Transform_Matrix(CPipeLine::TS_PROJ));
			XMStoreFloat4(&fPlayerPos, vPlayerPos);
			m_DescList[i]->fX = (fPlayerPos.x) * (0.5f * g_iWinSizeX);
			m_DescList[i]->fY = (fPlayerPos.y) * (0.5f * g_iWinSizeY);

			XMStoreFloat4x4(&(m_DescList[i]->WorldMatrix), XMMatrixScaling((m_DescList[i])->fWidth, (m_DescList[i])->fHeight, 1.f)
				* XMMatrixTranslation((m_DescList[i])->fX, (m_DescList[i])->fY, (m_DescList[i])->fZ));
			XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
		}
	}

	Safe_Release(pGameInstance);
}

void CUI_Monster::CommonLevel()
{
	_int LevelTen = m_MonsterLevel / 10;
	_int LevelOne = m_MonsterLevel % 10; // 0 = 33 1 = 34

	if (1> LevelTen)
	{
		//6번 그대로 5번 b랜더false 
		m_DescList[5]->bRender = false;
		m_DescList[6]->iTexNum = LevelOne + 33;
	}
	//만약 두자리일때 좌측 위치
	else
	{
		//6번 그대로 5번 b랜더 true
		m_DescList[5]->bRender = true;
		m_DescList[5]->iTexNum = LevelTen + 33;
		m_DescList[6]->iTexNum = LevelOne + 33;
	}

	if (3 < (_uint)m_DescList.size())
	{
		for (_uint i = 4; i < 9; ++i)
		{
			m_DescList[i]->fX = m_DescList[0]->fX + m_DescList[i]->PosSet.x;
			m_DescList[i]->fY = m_DescList[0]->fY + m_DescList[i]->PosSet.y;

			XMStoreFloat4x4(&(m_DescList[i]->WorldMatrix), XMMatrixScaling((m_DescList[i])->fWidth, (m_DescList[i])->fHeight, 1.f)
				* XMMatrixTranslation((m_DescList[i])->fX, (m_DescList[i])->fY, (m_DescList[i])->fZ));
			XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
			XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
		}
	}
}

void CUI_Monster::Load()
{
	_uint index = 19;
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
			ReadFile(hFile, &(Desc->fColorR), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorG), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fColorB), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iTexNum), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->iPass), sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->bRot), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->fDegree), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(Desc->PosSet), sizeof(_float2), &dwByte, nullptr);
			if (0 == dwByte)
				break;
		}

		CloseHandle(hFile);

		if (nullptr != Desc)
		{
			Desc->bRender = true;

			m_DescList.push_back(Desc);

			m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
			if (nullptr == m_pVIBuffer)
				return;

			m_BufferList.push_back(m_pVIBuffer);
		}
	}

}