#include "stdafx.h"
#include "..\Public\AnimToolManager.h"

#include "GameInstance.h"

#include "StateKey.h"
#include "DissolveKey.h"
#include "SoundKey.h"
#include "PartsKey.h"
#include "PriorityKey.h"

IMPLEMENT_SINGLETON(CAnimToolManager)

int g_iGUIID = 10000; // static, extern 해서 고정해놓는 변수, 매프레임 같은 값으로 초기화

#define PUSHID	ImGui::PushID(g_iGUIID++) // ID 넣기
#define POPID	ImGui::PopID() // 빼기 (주의! 짝맞춰야함)
#define SAMELINE ImGui::SameLine() // 같은줄
#define SEPERATOR ImGui::Separator() // 구분선

void CAnimToolManager::Save_State()
{
	char szBuffer[MAX_PATH] = "";
	char szPath[MAX_PATH] = "../../Data/CharState/";
	_tchar szFileName[MAX_PATH] = TEXT("");

	
	strcat_s(szPath, m_szClassName);
	strcat_s(szPath, "/");
	strcat_s(szPath, m_szClassName);
	strcat_s(szPath, "_");
	_itoa_s(*m_pTargetVTF->Get_StateID(), szBuffer, 10);
	strcat_s(szPath, szBuffer);
	strcat_s(szPath, ".state");
	
	MultiByteToWideChar(CP_ACP, 0,
		szPath,
		int(strlen(szPath)), szFileName,
		sizeof(_tchar) * MAX_PATH);
	
	HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;


	DWORD dwByte = 0;

	if (nullptr != m_pTargetVTF)
	{
		CCharacter::MULTISTATE tMultiState;
		ZeroMemory(&tMultiState, sizeof tMultiState);

		tMultiState.iAnimID[CCharacter::ANIMSET_BASE] = (_uint)m_pMultiState->iAnimID[CCharacter::ANIMSET_BASE];
		tMultiState.iAnimID[CCharacter::ANIMSET_RIBBON] = (_uint)m_pMultiState->iAnimID[CCharacter::ANIMSET_RIBBON];
		tMultiState.iNextState = (_uint)m_pMultiState->iNextState;
		tMultiState.iRotationType = (_uint)m_pMultiState->iRotationType;
		tMultiState.FramePerSec = (_double)m_pMultiState->FramePerSec;
		tMultiState.bLoop = m_pMultiState->bLoop;
		tMultiState.bLerp = m_pMultiState->bLerp;
		tMultiState.bRootMotion = m_pMultiState->bRootMotion;
		tMultiState.bApplyCoolTime = m_pMultiState->bApplyCoolTime;
		tMultiState.bWeaponState = m_pMultiState->bWeaponState;
		tMultiState.CoolTime = (_double)m_pMultiState->CoolTime;
		tMultiState.iPhysicMoveID = (_uint)m_pMultiState->iPhysicMoveID;
		tMultiState.iEnterPriority = (_uint)m_pMultiState->iEnterPriority;
		tMultiState.iLeavePriority = (_uint)m_pMultiState->iLeavePriority;
		tMultiState.iKeyCount = (_uint)m_pMultiState->iKeyCount;

		WriteFile(hFile, &tMultiState, sizeof(CCharacter::MULTISTATE) - sizeof(CStateKey**), &dwByte, nullptr);

		for (_uint i = 0; i < tMultiState.iKeyCount; ++i)
			WriteFile(hFile, m_pMultiState->ppStateKeys[i]->Get_Data(), sizeof(CStateKey::BaseData), &dwByte, nullptr);


	}
	

	else if (nullptr != m_pTargetGeneric)
	{

	}
	
	
	CloseHandle(hFile);

	MSG_BOX("save complete");
	return;
}

void CAnimToolManager::Free()
{
}

void CAnimToolManager::Add_ListBoxItem(_uint ModelListBoxID, _int ItemID, _tchar * pNameTag)
{
	char szBuffer[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, pNameTag, (int)lstrlen(pNameTag) + 1, szBuffer, MAX_PATH, nullptr, nullptr);


	strcat_s(m_szListBoxItemsArray[ModelListBoxID][ItemID], szBuffer);
}

void CAnimToolManager::RenderGUI()
{
	g_iGUIID = 10000;

	CGameInstance* pGI = CGameInstance::GetInstance();

	ImGui::SetNextWindowSize(ImVec2(500.f, 1000.f));
	PUSHID;
	ImGui::Begin("AnimTool");

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);

	if (ImGui::TreeNode("Model"))
	{
		if (nullptr == m_pTargetVTF && nullptr == m_pTargetGeneric)
		{
			PUSHID;
			if (ImGui::RadioButton("VTF", &m_iModelType, 0))
				m_iModelType = MODEL_VTF;
			POPID;
			SAMELINE;
			PUSHID;
			if (ImGui::RadioButton("Generic", &m_iModelType, 1))
				m_iModelType = MODEL_GENERIC;
			POPID;

			if (nullptr == m_pTargetVTF && nullptr == m_pTargetGeneric)
			{
				SAMELINE;
				ImGui::TextDisabled("Load Model First!");
			}

			switch (m_iModelType)
			{
				// MODEL_PLAYER
			case Client::CAnimToolManager::MODEL_VTF:
				PUSHID;
				ImGui::Text("Model_VTF");
				POPID;
				PUSHID;
				if (ImGui::BeginListBox("", ImVec2(ImGui::GetWindowWidth() - 75.f, 10 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (int n = 0; n < IM_ARRAYSIZE(m_szListBoxItemsArray[MODEL_VTF]); n++)
					{
						const bool is_selected = (m_iListBoxIDArray[MODEL_VTF] == n);
						PUSHID;
						if (ImGui::Selectable(m_szListBoxItemsArray[MODEL_VTF][n], is_selected))
							m_iListBoxIDArray[MODEL_VTF] = n;
						POPID;


						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndListBox();
				}
				POPID;

				PUSHID;
				if (ImGui::Button("Load Model"))
				{
					CGameInstance* pGameInstance = CGameInstance::GetInstance();

					if (MODEL_VTF == m_iModelType)
					{
						LoadPCModel();
					}
					else if (MODEL_GENERIC == m_iModelType)
						LoadGenericModel();
				}
				POPID;

				break;

				// MODEL_GENERIC
			case Client::CAnimToolManager::MODEL_GENERIC:
				PUSHID;
				ImGui::Text("GenericModel");
				POPID;
				PUSHID;
				if (ImGui::BeginListBox("", ImVec2(ImGui::GetWindowWidth() - 75.f, 10 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (int n = 0; n < IM_ARRAYSIZE(m_szListBoxItemsArray[MODEL_GENERIC]); n++)
					{
						const bool is_selected = (m_iListBoxIDArray[MODEL_GENERIC] == n);
						PUSHID;
						if (ImGui::Selectable(m_szListBoxItemsArray[MODEL_GENERIC][n], is_selected))
							m_iListBoxIDArray[MODEL_GENERIC] = n;
						POPID;


						// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndListBox();
				}
				POPID;


				break;
			case Client::CAnimToolManager::MODEL_END:
				ImGui::Text("m_eModelType == MODEL_END");
				break;
			default:
				ImGui::Text("Something is wrong");
				break;
			}
		}
		else
		{
			PUSHID;
			if (ImGui::Button("Delete Model"))
			{
				/*CGameInstance* pGameInstance = CGameInstance::GetInstance();

				if (MODEL_VTF == m_iModelType)
				{
				LoadPCModel();
				}
				else if (MODEL_GENERIC == m_iModelType)
				LoadGenericModel();*/
			}
			POPID;
		}
		ImGui::TreePop();
	}

	SEPERATOR;
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Animation"))
	{
		if (nullptr != m_pTargetVTF)
		{
			// 정보 갱신
			if (m_pMultiState->iAnimID[CTestVTF::ANIMSET_BASE] != m_iActionIndex[CTestVTF::ANIMSET_BASE] ||
				m_pMultiState->iAnimID[CTestVTF::ANIMSET_RIBBON] != m_iActionIndex[CTestVTF::ANIMSET_RIBBON])
				Update_Information();

			//
			PUSHID;
			ImGui::Text("Base Action");
			POPID;
			PUSHID;
			ImGui::Text("Anim Name : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::Text(m_szActionName[CTestVTF::ANIMSET_BASE]);
			POPID;
			PUSHID;
			ImGui::Text("Anim ID : ");
			POPID;
			SAMELINE;
			PUSHID;
			PUSHID;
			ImGui::InputInt("", &m_pMultiState->iAnimID[CTestVTF::ANIMSET_BASE]);
			POPID;
			POPID;
			PUSHID;
			ImGui::Text("Track");
			POPID;
			PUSHID;
			ImGui::Text("0");
			POPID;
			SAMELINE;
			PUSHID;
			if (ImGui::SliderFloat("", m_pTargetVTF->Get_TrackPos(CTestVTF::ANIMSET_BASE), 0.0f, m_Duration[CTestVTF::ANIMSET_BASE]))
				m_pTargetVTF->Set_TrackPos(CTestVTF::ANIMSET_BASE);
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::Text("%.1f", m_Duration[CTestVTF::ANIMSET_BASE]);
			POPID;

			//
			PUSHID;
			ImGui::Text("Ribbon Action");
			POPID;
			PUSHID;
			ImGui::Text("Anim Name : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::Text(m_szActionName[CTestVTF::ANIMSET_RIBBON]);
			POPID;
			PUSHID;
			ImGui::Text("Anim ID : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::InputInt("", &m_pMultiState->iAnimID[CTestVTF::ANIMSET_RIBBON]);
			POPID;
			PUSHID;
			ImGui::Text("Track");
			POPID;
			PUSHID;
			ImGui::Text("0");
			POPID;
			SAMELINE;
			PUSHID;
			if (ImGui::SliderFloat("", m_pTargetVTF->Get_TrackPos(CTestVTF::ANIMSET_RIBBON), 0.0f, m_Duration[CTestVTF::ANIMSET_RIBBON]))
				m_pTargetVTF->Set_TrackPos(CTestVTF::ANIMSET_RIBBON);
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::Text("%.1f", m_Duration[CTestVTF::ANIMSET_RIBBON]);
			POPID;


		}
		else if (nullptr != m_pTargetGeneric)
		{

		}




		ImGui::TreePop();
	}
	SEPERATOR;
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("State"))
	{
		if (nullptr != m_pTargetVTF)
		{
			if (m_pMultiState->FramePerSec != m_TicksPerSecond ||
				*(m_pTargetVTF->Get_StateID()) != m_iStateID)
				Update_Information();

			PUSHID;
			ImGui::Text("State ID : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
			ImGui::InputInt("", m_pTargetVTF->Get_StateID());
			m_pTargetVTF->Safe_StateParams();
			POPID;
			ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
			PUSHID;
			ImGui::Text(m_pTargetVTF->Get_StateTag(*m_pTargetVTF->Get_StateID()));
			POPID;

			PUSHID;
			ImGui::Text("Next State ID : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
			ImGui::InputInt("", &m_pMultiState->iNextState);
			m_pTargetVTF->Safe_StateParams();
			POPID;
			ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
			PUSHID;
			ImGui::Text(CTestChar::szSharedStateTag[m_pMultiState->iNextState]);
			POPID;

			PUSHID;
			ImGui::Text("FramePerSec: ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
			ImGui::InputFloat("", &m_pMultiState->FramePerSec, 0.0f, 0.0f, "%.1f");
			POPID;

			PUSHID;
			ImGui::Text("Rotation Type : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
			ImGui::InputInt("", &m_pMultiState->iRotationType);
			m_pTargetVTF->Safe_StateParams();
			POPID;
			ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
			PUSHID;
			ImGui::Text(CTestChar::szRotationTag[m_pMultiState->iRotationType]);
			POPID;


			PUSHID;
			ImGui::Text("EnterPriority : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
			ImGui::InputInt("", &m_pMultiState->iEnterPriority);
			POPID;

			PUSHID;
			ImGui::Text("LeavePriority : ");
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
			ImGui::InputInt("", &m_pMultiState->iLeavePriority);
			POPID;

			//
			PUSHID;
			ImGui::Checkbox("Loop", &m_pMultiState->bLoop);
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::Checkbox("Lerp", &m_pMultiState->bLerp);
			POPID;
			SAMELINE;
			PUSHID;
			ImGui::Checkbox("RootMotion", &m_pMultiState->bRootMotion);
			POPID;

			if (m_pMultiState->bRootMotion == false)
			{
				PUSHID;
				ImGui::Text("PhysicMoveID : ");
				POPID;
				SAMELINE;
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
				PUSHID;
				ImGui::InputInt("", &m_pMultiState->iPhysicMoveID);
				m_pTargetVTF->Safe_StateParams();
				POPID;
				ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
				PUSHID;
				ImGui::Text(CTestChar::szPhysicMoveTag[m_pMultiState->iPhysicMoveID]);
				POPID;
			}

			PUSHID;
			ImGui::Checkbox("Use Weapon", &m_pMultiState->bWeaponState);
			POPID;
			
			SAMELINE;
			
			PUSHID;
			ImGui::Checkbox("Use Cooltime", &m_pMultiState->bApplyCoolTime);
			// 쿨타임 값 입력
			if (true == m_pMultiState->bApplyCoolTime)
			{
				SAMELINE;
				PUSHID;
				ImGui::Text("Cooltime: ");
				POPID;
				SAMELINE;
				PUSHID;
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
				ImGui::InputFloat("", &m_pMultiState->CoolTime, 0.0f, 0.0f, "%.1f");
				POPID;
			}
			//

		


			POPID;
		}
		else if (nullptr != m_pTargetGeneric)
		{

		}

		ImGui::TreePop();
	}
	SEPERATOR;
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("StateKeys"))
	{
		if (nullptr != m_pTargetVTF)
		{
			Safe_Params();

			// 키 카운트 설정
			PUSHID;
			ImGui::Text("Target Count : ");
			POPID;
			SAMELINE;
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
			PUSHID;
			ImGui::InputInt("", &m_iKeyCount);
			POPID;
			SAMELINE;
			PUSHID;
			if (ImGui::Button("Set Key Count"))
			{
				Set_KeyCount();
			}
			POPID;

			SEPERATOR;

			int iKeyCount = m_pMultiState->iKeyCount;
			if (0 != iKeyCount)
			{
				PUSHID;
				m_iKeyExist = 0;
				for (_int i = 0; i < iKeyCount; ++i)
				{
					if (nullptr != m_pMultiState->ppStateKeys[i])
						++m_iKeyExist;
					else
						break;
				}
				ImGui::Text("KeyCount : %d / %d", m_iKeyExist, m_pMultiState->iKeyCount);
				POPID;

				//
				PUSHID;
				if (ImGui::TreeNode("Add Key"))
				{
					PUSHID;
					ImGui::Text("Type : ");
					POPID;
					SAMELINE;
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
					PUSHID;
					ImGui::InputInt("", &m_tKeyData.iType);
					Safe_Params();
					POPID;
					ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
					PUSHID;
					ImGui::Text(CStateKey::szStateKeyTag[m_tKeyData.iType]);
					POPID;

					PUSHID;
					ImGui::Text("ShotFrame : ");
					POPID;
					SAMELINE;
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
					PUSHID;
					ImGui::InputFloat("", &m_tKeyData.ShotFrame, 0.f, 0.f, "%.1f");
					POPID;

					PUSHID;
					if (ImGui::TreeNode("Additional Parameters"))
					{
						PUSHID;
						ImGui::Text("szTag : ");
						POPID;
						SAMELINE;
						PUSHID;
						ImGui::InputText("", m_tKeyData.szTag, MAX_PATH);
						POPID;

						PUSHID;
						ImGui::Text("iInt0 : ");
						POPID;
						SAMELINE;
						ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
						PUSHID;
						ImGui::InputInt("", &m_tKeyData.iInt0);
						Safe_Params();
						POPID;

						PUSHID;
						ImGui::Text("iInt1 : ");
						POPID;
						SAMELINE;
						ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
						PUSHID;
						ImGui::InputInt("", &m_tKeyData.iInt1);
						Safe_Params();
						POPID;

						PUSHID;
						ImGui::Text("iInt2 : ");
						POPID;
						SAMELINE;
						ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
						PUSHID;
						ImGui::InputInt("", &m_tKeyData.iInt2);
						Safe_Params();
						POPID;

						PUSHID;
						ImGui::Text("fFloat : ");
						POPID;
						SAMELINE;
						ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
						PUSHID;
						ImGui::InputFloat("", &m_tKeyData.fFloat, 0.f, 0.f, "%.1f");
						POPID;

						ImGui::TreePop();
					}
					POPID;

					PUSHID;
					if (ImGui::Button("Create Key"))
					{
						Create_Key();
					}
					POPID;

					ImGui::TreePop();
				}
				POPID;
				//



				for (int i = 0; i < m_pMultiState->iKeyCount; i++)
				{
					CStateKey* pStateKey = m_pMultiState->ppStateKeys[i];
					if (nullptr != pStateKey)
					{
						PUSHID;
						if (ImGui::TreeNode(CStateKey::szStateKeyTag[pStateKey->Get_Data()->iType]))
						{
							ImGui::SameLine(ImGui::GetWindowWidth() * 0.7f);
							PUSHID;
							ImGui::Checkbox("Ready", pStateKey->Get_Ready());
							POPID;


							ImGui::TreePop();
						}
						POPID;
					}
					else
					{
						PUSHID;
						ImGui::Text("%d. empty", i);
						POPID;
					}
				}

				SEPERATOR;


			}
			else
			{
				PUSHID;
				ImGui::Text("KeyCount is zero!");
				POPID;
			}
		}
		else if (nullptr != m_pTargetGeneric)
		{

		}

		PUSHID;
		if (ImGui::Button("Save State"))
		{
			Save_State();
		}
		POPID;


		ImGui::TreePop();
	}

	ImGui::End();
	POPID;

}

void CAnimToolManager::Init_AnimToolManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	/*for (_uint i = 0; i < MODEL_END; ++i)
	{
		for (_uint j = 0; j < 10; ++j)
		{
			char szBuffer[MAX_PATH];
			_itoa_s(j, szBuffer, 10);
			strcat_s(szBuffer, ": ");
			strcpy_s(m_szListBoxItemsArray[i][j], szBuffer);
		}
	}*/

	ZeroMemory(&m_tKeyData, sizeof m_tKeyData);
}

void CAnimToolManager::LoadPCModel()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (MODEL_VTF == m_iModelType)
	{
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_ANIMTOOL, VTFENUM(m_iListBoxIDArray[MODEL_VTF]), TEXT("Layer_Player"), TEXT("TestPlayer0"))))
			return;

		strcpy_s(m_szClassName, m_szListBoxItemsArray[MODEL_VTF][m_iListBoxIDArray[MODEL_VTF]]);

		m_pTargetVTF = static_cast<CTestVTF*>(pGameInstance->Find_GameObject(LEVEL_ANIMTOOL, TEXT("TestPlayer0")));

		Update_Information();
	}
}

void CAnimToolManager::LoadGenericModel()
{
}

void CAnimToolManager::Update_Information()
{
	if (MODEL_VTF == m_iModelType && nullptr != m_pTargetVTF)
	{
		m_pTargetVTF->Safe_StateParams();
		m_pTargetVTF->Safe_AnimID();

		m_pMultiState = m_pTargetVTF->Get_CurState();

		// Action - CTestVTF::ANIMSET_BASE
		CModel_Anim* pModel = m_pTargetVTF->Get_Action(CTestVTF::ANIMSET_BASE);
		m_iActionIndex[CTestVTF::ANIMSET_BASE] = m_pMultiState->iAnimID[CTestVTF::ANIMSET_BASE];
		CAnimation* pAnim = pModel->Get_Animation((_uint)m_iActionIndex[CTestVTF::ANIMSET_BASE]);

		_tchar szBuffer[MAX_PATH] = TEXT("");
		lstrcpy(szBuffer, pAnim->Get_Name());
		WideCharToMultiByte(CP_ACP, 0, szBuffer, lstrlen(szBuffer) + 1, m_szActionName[CTestVTF::ANIMSET_BASE], MAX_PATH, nullptr, nullptr);

		m_Duration[CTestVTF::ANIMSET_BASE] = (_float)pAnim->Get_Duration();

		pAnim->Set_TicksPerSecond(m_pMultiState->FramePerSec);
		m_TicksPerSecond = (_float)m_pMultiState->FramePerSec;


		// Action - CTestVTF::ANIMSET_RIBBON
		pModel = m_pTargetVTF->Get_Action(CTestVTF::ANIMSET_RIBBON);
		m_iActionIndex[CTestVTF::ANIMSET_RIBBON] = m_pMultiState->iAnimID[CTestVTF::ANIMSET_RIBBON];
		pAnim = pModel->Get_Animation((_uint)m_iActionIndex[CTestVTF::ANIMSET_RIBBON]);

		lstrcpy(szBuffer, pAnim->Get_Name());
		WideCharToMultiByte(CP_ACP, 0, szBuffer, lstrlen(szBuffer) + 1, m_szActionName[CTestVTF::ANIMSET_RIBBON], MAX_PATH, nullptr, nullptr);

		pAnim->Set_TicksPerSecond(m_pMultiState->FramePerSec);
		m_Duration[CTestVTF::ANIMSET_RIBBON] = (_float)pAnim->Get_Duration();

		// State
		m_iStateID = *m_pTargetVTF->Get_StateID();

		m_pTargetVTF->SetUp_State();
		//m_pTargetVTF->SetUp_Animation();
	}
	else if (MODEL_GENERIC == m_iModelType && nullptr != m_pTargetGeneric)
	{

	}
}

void CAnimToolManager::Set_KeyCount()
{
	if (nullptr != m_pTargetVTF)
	{
		_int iOldCount = m_pMultiState->iKeyCount;

		if (iOldCount != m_iKeyCount)
		{
			CStateKey** ppStateKeys = nullptr;

			if (m_iKeyCount != 0)
			{
				ppStateKeys = new CStateKey*[m_iKeyCount];
				ZeroMemory(ppStateKeys, sizeof(CStateKey*) * m_iKeyCount);
			}

			if (iOldCount != 0)
			{
				if (iOldCount > m_iKeyCount)
				{
					for (_int i = m_iKeyCount; i < iOldCount; ++i)
					{
						Safe_Release(m_pMultiState->ppStateKeys[i]);
					}

					for (_int i = 0; i < m_iKeyCount; ++i)
					{
						ppStateKeys[i] = m_pMultiState->ppStateKeys[i];
					}
				}
				else if (iOldCount < m_iKeyCount)
				{
					for (_int i = 0; i < iOldCount; ++i)
					{
						ppStateKeys[i] = m_pMultiState->ppStateKeys[i];
					}
				}
				Safe_Delete_Array(m_pMultiState->ppStateKeys);
			}
			m_pMultiState->ppStateKeys = ppStateKeys;
			m_pMultiState->iKeyCount = m_iKeyCount;
		}
	}
	else if (nullptr != m_pTargetGeneric)
	{

	}

}

void CAnimToolManager::Create_Key()
{
	if(nullptr != m_pMultiState)
		if (m_pMultiState->iKeyCount == m_iKeyExist)
			return;

	if (nullptr != m_pSingleState)
		if (m_pSingleState->iKeyCount == m_iKeyExist)
			return;


	CStateKey* pStateKey = nullptr;
	switch (m_tKeyData.iType)
	{
	case CStateKey::TYPE_EFFECT:

		break;
	case CStateKey::TYPE_PARTS:
		pStateKey = CPartsKey::Create(m_pDevice, m_pContext, &m_tKeyData);
		break;
	case CStateKey::TYPE_PRIORITY:
		pStateKey = CPriorityKey::Create(m_pDevice, m_pContext, &m_tKeyData);
		break;
	case CStateKey::TYPE_DISSOLVE:
		// 
		//pStateKey = CDissolveKey::Create(m_pDevice, m_pContext, m_tKeyData.ShotFrame, (CDissolveKey::TYPE)m_tKeyData.iInt, (_double)m_tKeyData.fFloat);
		break;
	case CStateKey::TYPE_SOUND:

		break;
	
	case CStateKey::TYPE_MISSILE:

		break;
	default:
		break;
	}

	if (nullptr == pStateKey)
	{
		MSG_BOX("Create_Key Failed");
		return;
	}

	pStateKey->Set_Data(&m_tKeyData);

	if (nullptr != m_pTargetVTF && nullptr != m_pMultiState)
	{
		_bool bFailed = false;
		for (_int i = 0; i < m_pMultiState->iKeyCount; ++i)
		{
			if (nullptr == m_pMultiState->ppStateKeys[i])
			{
				m_pMultiState->ppStateKeys[i] = pStateKey;
				break;
			}

			if (i == m_pMultiState->iKeyCount - 1)
				bFailed = true;
		}

		if (true == bFailed)
		{
			MSG_BOX("No Slot?");
			return;
		}

	}
	else if (nullptr != m_pTargetGeneric && nullptr != m_pSingleState)
	{
		_bool bFailed = false;
		for (_int i = 0; i < m_pSingleState->iKeyCount; ++i)
		{
			if (nullptr == m_pSingleState->ppStateKeys[i])
			{
				m_pSingleState->ppStateKeys[i] = pStateKey;
			}

			if (i == m_pSingleState->iKeyCount - 1)
				bFailed = true;
		}

		if (true == bFailed)
		{
			MSG_BOX("No Slot?");
			return;
		}

	}

}

void CAnimToolManager::Safe_Params()
{
	if (m_tKeyData.iType < 0)
		m_tKeyData.iType = 0;
	else if (m_tKeyData.iType >= CStateKey::TYPE_END)
		m_tKeyData.iType = CStateKey::TYPE_END - 1;

	if (m_tKeyData.iInt0 < 0)
		m_tKeyData.iInt0 = 0;

	if (m_tKeyData.iInt1 < 0)
		m_tKeyData.iInt1 = 0;

	if (m_tKeyData.iInt2 < 0)
		m_tKeyData.iInt2 = 0;
}
