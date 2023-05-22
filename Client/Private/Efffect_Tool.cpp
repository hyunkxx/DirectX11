#include "stdafx.h"
#include "..\Public\Efffect_Tool.h"

#include "GameInstance.h"
#include "Effect.h"

#include "Particle_Effect.h"
#include "Mesh_Effect.h"
#include "Effect_Player.h"
#include "Rect_Effect.h"

#include "Particle_Effect_P.h"
#include "Mesh_Effect_P.h"
#include "Rect_Effect_P.h"

IMPLEMENT_SINGLETON(CEfffect_Tool)

CEfffect_Tool::CEfffect_Tool()
{
}

HRESULT CEfffect_Tool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	for (int i = 0; 100 > i; i++)
	{
		_itoa_s(i, m_Effect_ListName[i], 10);
	}


	return S_OK;
}

void CEfffect_Tool::Tick(_double TimeDelta)
{

	if (CGameInstance::GetInstance()->InputKey(DIK_DELETE) == KEY_STATE::TAP)
	{
		if (m_bRender)
			m_bRender = false;
		else
			m_bRender = true;
	}

	if (!m_bRender)
		return;

	m_PushID = 1000;

	if (m_bLoadEffect)
	{
		if (!m_bPlay)
			TimeDelta = 0.f;

		m_fTimeAcc += TimeDelta;

		if (nullptr != m_pEffects)
		{
			for (auto& iter : *m_pEffects)
			{
				iter.second->Tick(TimeDelta);
			}
	
		}
	}
	else if (m_bLoadEffect2)
	{
		if (!m_bPlay)
			TimeDelta = 0.f;

		m_fTimeAcc += TimeDelta;

		if (nullptr != m_pEffectPlayer)
		{
			m_pEffectPlayer->Tick(TimeDelta);
		}
	}
	else
	{
		if (nullptr != m_pEffect)
		{
			m_pEffect->Tick(TimeDelta);
		}
	}



}

void CEfffect_Tool::LateTick(_double TimeDelta)
{
	if (!m_bRender)
		return;

	if (m_bLoadEffect)
	{
		_bool bFinish = true;
		if (nullptr != m_pEffects)
		{
			for (auto& iter : *m_pEffects)
			{
				iter.second->LateTick(TimeDelta);
				if (!iter.second->Get_Finish())
					bFinish = false;
			}
		}

		if (bFinish)
			m_bLoadEffect = false;

	}
	else if (m_bLoadEffect2)
	{
		m_pEffectPlayer->LateTick(TimeDelta);
		if (m_pEffectPlayer->Get_Finish())
			m_bLoadEffect2 = false;
	}
	else
	{
		if (nullptr != m_pEffect)
		{
			m_pEffect->LateTick(TimeDelta);
		}
	}
}

HRESULT CEfffect_Tool::Render()
{
	return S_OK;
}

void CEfffect_Tool::RenderGUI()
{
	if (!m_bRender)
		return;

	Effect_List();
	Play_Effect();
	Create_Effect();
	Parts_List();

	if (m_bLoadEffect)
	{
		if (nullptr != m_pEffects)
		{
			for (auto& iter : *m_pEffects)
			{
				iter.second->RenderGUI();
			}
		}
	}
	else
	{
		if (nullptr != m_pEffect)
		{
			m_pEffect->RenderGUI();
		}
	}
}

void CEfffect_Tool::Effect_List()
{
	ImGui::Begin("Effect_List");

	if (ImGui::Button("Load_Effect"))
	{
#ifdef _DEBUG
		Load_EffectList(TEXT("../../Resource/Tool_Effect/"));
#endif // _DEBUG

	}

	if (ImGui::BeginCombo("Mesh_List", m_ListCur_Select))
	{
		for (int n = 0; n < IM_ARRAYSIZE(m_Effect_ListName); n++)
		{
			bool is_selected = (m_ListCur_Select == m_Effect_ListName[n]);
			if (ImGui::Selectable(m_Effect_ListName[n], is_selected))
				strcpy_s(m_ListCur_Select, m_Effect_ListName[n]);
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	if (m_ListCur_Select != NULL)
	{
		if (strcmp(m_ListCur_Select, m_ListPrev_Select))
		{
			strcpy_s(m_ListPrev_Select, m_ListCur_Select);
			auto& iter = find_if(m_LoadEffectList.begin(), m_LoadEffectList.end(), CTagFinder2(m_ListCur_Select));

			m_pEffectPlayer = iter->second;
		}
	}

	if (ImGui::Button("Play"))
	{
		if (nullptr == m_pEffectPlayer)
		{
			MSG_BOX("No Effect Selected");
		}
		else
		{
			m_fTimeAcc = 0.f;
			m_bPlay = true;
			m_bLoadEffect2 = true;

			m_WorldMatrix._41 = m_vStartPos.x;
			m_WorldMatrix._42 = m_vStartPos.y;
			m_WorldMatrix._43 = m_vStartPos.z;

			m_pEffectPlayer->Play_Effect(&m_WorldMatrix);
			m_fLifeTime = static_cast<CEffect_Player*>(m_pEffectPlayer)->Get_LifeTime();
		}
	}
	ImGui::SameLine(100);
	if (ImGui::Button("Copy"))
	{
		Copy_Effect();
	}

	ImGui::End();
}

void CEfffect_Tool::Parts_List()
{
	ImGui::Begin("Parts_List");
	
	ImGui::Separator();

	if (!m_bLoadEffect)
	{
		const char* phar = nullptr;
		ImGui::InputText("Effect Tag", m_EffectName,sizeof(char) * MAX_PATH);
		ImGui::Text(m_EffectName);
		if (ImGui::Button("All_Play"))
		{
			if (nullptr != m_pEffects)
			{
				_float fLifeTime = 0.f;
				m_fTimeAcc = 0.f;
				m_bPlay = true;
				m_bLoadEffect = true;

				for (auto&iter : *m_pEffects)
				{
					m_WorldMatrix._41 = m_vStartPos.x;
					m_WorldMatrix._42 = m_vStartPos.y;
					m_WorldMatrix._43 = m_vStartPos.z;

					iter.second->Play_Effect(&m_WorldMatrix);

					if (fLifeTime < iter.second->Get_Effect_Desc().fLifeTime)
					{
						fLifeTime = iter.second->Get_Effect_Desc().fLifeTime;
					}
				}

				m_fLifeTime = fLifeTime;
			}
		}
		ImGui::SameLine(100);
		if (ImGui::Button("Save"))
		{
			list<EFFECT_DESC*> EffectDescVec;
			EFFECT_DESC* pEffecDesc = nullptr;

			for (auto&iter : *m_pEffects)
			{
				pEffecDesc = new EFFECT_DESC;
				memcpy(pEffecDesc, &(iter.second->Get_Effect_Desc()),sizeof(iter.second->Get_Effect_Desc()));
				EffectDescVec.push_back(pEffecDesc);
			}
			CGameInstance::GetInstance()->Save_File(g_hWnd, &EffectDescVec, m_EffectName);

			for (auto& iter : EffectDescVec)
				Safe_Delete(iter);
			EffectDescVec.clear();
		}
	}
	ImGui::Separator();
#pragma region Edit
	ImGui::Text("Part_List");
	ImGui::Separator();
	if (nullptr != m_pEffects)
	{
		for (auto& iter : *m_pEffects)
		{
			if (ImGui::TreeNodeEx(iter.first))
			{
				ImGui::TreePop();
				m_pEffect = iter.second;

				PUSHID
				if (ImGui::Button("Edit"))
				{
					iter.second->Set_Edit();
				}
				POPID
				ImGui::SameLine(50);
				PUSHID
				if (ImGui::Button("Delete"))
				{
					Safe_Release(iter.second);
					char* pChar = iter.first;
					m_pEffects->erase(iter.first);
					delete[](pChar);
					m_pEffect = nullptr;
					ImGui::PopID();
					break;
				}
				POPID
				ImGui::SameLine(110);
#pragma region Copy
				PUSHID
					if (ImGui::Button("Copy"))
					{
						EFFECT_DESC EffectDesc = iter.second->Get_Effect_Desc();
						if (EFFECT_TYPE::ID_MESH == EffectDesc.eEffectType)
						{
							m_pEffect = CMesh_Effect::Create(m_pDevice, m_pContext, EffectDesc);

							char stName[MAX_PATH] = "";

							if (nullptr != m_pEffects)
							{
								sprintf_s(stName, "Mesh_%d", m_iMapCount++);
								char* pChar = new char[MAX_PATH];
								strcpy_s(pChar, sizeof(stName), stName);
								m_pEffects->emplace(pChar, m_pEffect);
							}

							m_bLoadEffect = false;
						}
						else if (EFFECT_TYPE::ID_PARTICLE == EffectDesc.eEffectType)
						{
							m_pEffect = CParticle_Effect::Create(m_pDevice, m_pContext, EffectDesc);

							char stName[MAX_PATH] = "";

							if (nullptr != m_pEffects)
							{
								sprintf_s(stName, "Particle_%d", m_iMapCount++);
								char* pChar = new char[MAX_PATH];
								strcpy_s(pChar, sizeof(stName), stName);
								m_pEffects->emplace(pChar, m_pEffect);
							}

							m_bLoadEffect = false;
						}
						else if (EFFECT_TYPE::TYPE_RECT == EffectDesc.eEffectType)
						{
							m_pEffect = CRect_Effect::Create(m_pDevice, m_pContext, EffectDesc);

							char stName[MAX_PATH] = "";

							if (nullptr != m_pEffects)
							{
								sprintf_s(stName, "Rect_%d", m_iMapCount++);
								char* pChar = new char[MAX_PATH];
								strcpy_s(pChar, sizeof(stName), stName);
								m_pEffects->emplace(pChar, m_pEffect);
							}

							m_bLoadEffect = false;
						}
						ImGui::PopID();
						break;
					}
				POPID
#pragma endregion
				
			}
		}
	}

#pragma endregion
	ImGui::End();
}

void CEfffect_Tool::Create_Effect()
{
	ImGui::Begin("Crate_Effect");

	if (ImGui::TreeNodeEx("Type_Mesh"))
	{
		ImGui::TreePop();
		if (ImGui::Button("Mesh"))
		{
			EFFECT_DESC EffectDesc;
			ZeroMemory(&EffectDesc, sizeof(EFFECT_DESC));
			EffectDesc.fCurAlpha = 1.f;
			m_pEffect = CMesh_Effect::Create(m_pDevice, m_pContext, EffectDesc);

			char stName[MAX_PATH] = "";

			if (nullptr != m_pEffects)
			{
				sprintf_s(stName, "Mesh_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, m_pEffect);
			}
			else
			{
				m_iMapCount = 0;
				m_pEffects = new EFFECTS;
				sprintf_s(stName, "Mesh_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, m_pEffect);
			}
			m_bLoadEffect = false;
		}
	}
	if (ImGui::TreeNodeEx("Type_Rect"))
	{
		ImGui::TreePop();
		if (ImGui::Button("Rect"))
		{
			EFFECT_DESC EffectDesc;
			ZeroMemory(&EffectDesc, sizeof(EFFECT_DESC));
			EffectDesc.fCurAlpha = 1.f;
			m_pEffect = CRect_Effect::Create(m_pDevice, m_pContext, EffectDesc);

			char stName[MAX_PATH] = "";

			if (nullptr != m_pEffects)
			{
				sprintf_s(stName, "Rect_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, m_pEffect);
			}
			else
			{
				m_iMapCount = 0;
				m_pEffects = new EFFECTS;
				sprintf_s(stName, "Rect_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, m_pEffect);
			}
			m_bLoadEffect = false;
		}
	}
	if (ImGui::TreeNodeEx("Type_Particle"))
	{
		ImGui::TreePop();
		ImGui::InputInt("ParticleInstance", &m_iNumInstance);
		if (ImGui::Button("Particle"))
		{
			EFFECT_DESC EffectDesc;
			ZeroMemory(&EffectDesc, sizeof(EFFECT_DESC));
			EffectDesc.iNumInstance = m_iNumInstance;

			m_pEffect = CParticle_Effect::Create(m_pDevice, m_pContext, EffectDesc);

			char stName[MAX_PATH] = "";

			if (nullptr != m_pEffects)
			{
				sprintf_s(stName, "Particle_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, m_pEffect);
			}
			else
			{
				m_iMapCount = 0;
				m_pEffects = new EFFECTS;
				sprintf_s(stName, "Particle_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, m_pEffect);
			}

			m_bLoadEffect = false;
		}
	}

	ImGui::End();
}

void CEfffect_Tool::Play_Effect()
{
	ImGui::Begin("Play_Effect");
	
	char pTimeTag[MAX_PATH] = "LifeTime : ";
	sprintf_s(m_EffectAccTex, "%f", m_fLifeTime);
	strcat_s(pTimeTag, m_EffectAccTex);
	ImGui::Text(pTimeTag);

	char pTimeTag2[MAX_PATH] = "TimeAcc : ";
	sprintf_s(m_LifeTimeTex, "%f", m_fTimeAcc);
	strcat_s(pTimeTag2, m_LifeTimeTex);
	ImGui::Text(pTimeTag2);

	ImGui::Separator();

	if (ImGui::Button("Play"))
		m_bPlay = true;
	ImGui::SameLine(100);
	if (ImGui::Button("Stop"))
		m_bPlay = false;

	ImGui::Separator();
	ImGui::InputFloat3("Start_Pos", (float*)&m_vStartPos);
	ImGui::InputFloat3("End_Pos", (float*)&m_vEndPos);

	_float3 vCurPos = {0.f , 0.f , 0.f};
	_float fLerp = m_fTimeAcc / m_fLifeTime;
	XMStoreFloat3(&vCurPos, XMVectorLerp(XMLoadFloat3(&m_vStartPos), XMLoadFloat3(&m_vEndPos), fLerp));
	ImGui::Text("Current Position");
	char pPosTagX[MAX_PATH] = "";
	char pPosTagY[MAX_PATH] = "";
	char pPosTagZ[MAX_PATH] = "";

	sprintf_s(pPosTagX, "X : %f", vCurPos.x);
	sprintf_s(pPosTagY, " / Y : %f", vCurPos.y);
	sprintf_s(pPosTagZ, " / Z : %f", vCurPos.z);

	strcat_s(pPosTagX, pPosTagY);
	strcat_s(pPosTagX, pPosTagZ);
	ImGui::Text(pPosTagX);
	m_WorldMatrix._41 = vCurPos.x;
	m_WorldMatrix._42 = vCurPos.y;
	m_WorldMatrix._43 = vCurPos.z;

	ImGui::End();

}

void CEfffect_Tool::Copy_Effect()
{
	if (nullptr == m_pEffectPlayer)
	{
		MSG_BOX("No Effect Selected");
		return;
	}
		

	list<EFFECT_DESC*>* pEffectDescList = static_cast<CEffect_Player*>(m_pEffectPlayer)->Get_Effects();

	for (auto& iter : *pEffectDescList)
	{
		if (ID_MESH == iter->eEffectType)
		{
			CEffect* pEffect = CMesh_Effect::Create(m_pDevice, m_pContext, *iter);

			char stName[MAX_PATH] = "";

			if (nullptr != m_pEffects)
			{
				sprintf_s(stName, "Mesh_Copy_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, pEffect);
			}
			else
			{
				m_iMapCount = 0;
				m_pEffects = new EFFECTS;
				sprintf_s(stName, "Mesh_Copy_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, pEffect);
			}

		}
		else if (EFFECT_TYPE::ID_PARTICLE == iter->eEffectType)
		{

			CEffect* pEffect = CParticle_Effect::Create(m_pDevice, m_pContext, *iter);
			char stName[MAX_PATH] = "";

			if (nullptr != m_pEffects)
			{
				sprintf_s(stName, "Particle_Copy_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, pEffect);
			}
			else
			{
				m_iMapCount = 0;
				m_pEffects = new EFFECTS;
				sprintf_s(stName, "Particle_Copy_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, pEffect);
			}
		}
		else if (EFFECT_TYPE::TYPE_RECT == iter->eEffectType)
		{

			CEffect* pEffect = CRect_Effect::Create(m_pDevice, m_pContext, *iter);
			char stName[MAX_PATH] = "";

			if (nullptr != m_pEffects)
			{
				sprintf_s(stName, "Rect_Copy_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, pEffect);
			}
			else
			{
				m_iMapCount = 0;
				m_pEffects = new EFFECTS;
				sprintf_s(stName, "Rect_Copy_%d", m_iMapCount++);
				char* pChar = new char[MAX_PATH];
				strcpy_s(pChar, sizeof(stName), stName);
				m_pEffects->emplace(pChar, pEffect);
			}
		}

		Safe_Delete(iter);
	}
	pEffectDescList->clear();
	Safe_Delete(pEffectDescList);

}

#ifdef _DEBUG

void CEfffect_Tool::Load_EffectList(const _tchar* pPathName)
{
	wstring path = pPathName;
	_tchar ParentFolderName[MAX_PATH] = TEXT("");

	for (const auto & file : recursive_directory_iterator(path))
	{
		if (file.path().extension().string() != ".bin")
			continue;

		lstrcpy(ParentFolderName, file.path().parent_path().filename().c_str());

		_tchar RouteName[MAX_PATH] = TEXT("");
		_tchar FileName[MAX_PATH] = TEXT("");
		_tchar EndName[MAX_PATH] = TEXT(".bin");
		_wsplitpath_s(file.path().c_str(), nullptr, 0, nullptr, 0, FileName, MAX_PATH, nullptr, 0);

		list<EFFECT_DESC*> pEffectDesces;
		lstrcpy(RouteName, pPathName);
		lstrcat(RouteName, FileName);
		lstrcat(RouteName, EndName);
		wstring strFileTag = RouteName;

		CGameInstance::GetInstance()->Load_File(g_hWnd, strFileTag, &pEffectDesces);

		CEffect* pEffect = CEffect_Player::Create(m_pDevice, m_pContext, "../../Resource/Tool_Resource/", pEffectDesces);

		char* pChar = new char[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, FileName, -1, pChar, MAX_PATH, NULL, NULL);
		m_LoadEffectList.emplace(pChar, pEffect);

		strcpy_s(m_Effect_ListName[m_iListCount++], pChar);

		for (auto& iter : pEffectDesces)
			Safe_Delete(iter);
		pEffectDesces.clear();
	}
}

#endif // _DEBUG

void CEfffect_Tool::Free()
{

	if (nullptr != m_pEffects)
	{
		for (auto& iter : *m_pEffects)
		{
			delete[](iter.first);
			Safe_Release(iter.second);
		}
		(*m_pEffects).clear();
	}

	for (auto& iter : m_LoadEffectList)
	{
		delete[](iter.first);
		Safe_Release(iter.second);
	}
	m_LoadEffectList.clear();

	Safe_Delete(m_pEffects);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
