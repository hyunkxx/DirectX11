#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Character.h"
#include "TestVTF.h"
#include "TestGeneric.h"

BEGIN(Client)

class CAnimToolManager final : public CBase
{
	DECLARE_SINGLETON(CAnimToolManager)

private:
	explicit CAnimToolManager() = default;
	virtual ~CAnimToolManager() = default;

public:
	enum ModelType
	{
		MODEL_VTF,
		MODEL_GENERIC,
		MODEL_END
	};

	// ItemID = PrototypeID - CURTYPE_BEGIN
	void Add_ListBoxItem(_uint ModelListBoxID, _int ItemID, _tchar* pNameTag);

public:
	void RenderGUI();

	void Init_AnimToolManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// PlayerCharacter
	void LoadPCModel();

	// Char
	void LoadGenericModel();

	//
	void DeleteModel();

	// �� �ε�, ����, �ִϸ��̼� ���� �� ������ ȣ��
	void Update_Information();

	// 
	void Set_KeyCount();
	void Create_Key();
	void Safe_Params();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	// ���� ��� ���� ����� ������
	class CTestVTF*	m_pTargetVTF = { nullptr };
	CTestChar::MULTISTATE_TOOL*	m_pMultiState = { nullptr };
	class CTestGeneric*		m_pTargetGeneric = { nullptr };
	CTestChar::SINGLESTATE_TOOL* m_pSingleState = { nullptr };

	// 'Model' Treenode
	/// Types
	int	m_iModelType = MODEL_VTF;

	/// LIstBoxIndex
	int m_iListBoxIDArray[MODEL_END] = { 0, };

	/// ListBoxItems
	// ����Ʈ�ڽ� MLB_END��, ������Ÿ�� 10�� �̸����� ����, ���ڿ� ���� ���� MAX_PATH
	char	m_szListBoxItemsArray[MODEL_END][10][MAX_PATH] = { "", };

	// 'Animation' Treenode
	// Infos
	char m_szActionName[CTestVTF::ANIMSET_END][MAX_PATH] = { "", };
	_int m_iActionIndex[CTestVTF::ANIMSET_END] = { 0, };
	

	// Parameters
	_float m_Duration[CTestVTF::ANIMSET_END] = { 0.f };
	_float m_TicksPerSecond = { 0.f };


	// 'State' Treenode
	// ��Ʈ �������, ��� �����
	_int m_iStateID = { 0 };
	_bool m_bOpenPhysicWindow = { false };

	// 'StateKey' Treenode
	_int m_iKeyCount = { 0 };
	_int m_iKeyExist = { 0 };
	CStateKey::BaseData m_tKeyData;

	//char m_szClassName[MAX_PATH] = { "" };

	// for Generic
	char m_szActionName_s [MAX_PATH] = { "" };
	_int m_iActionIndex_s = { 0 };

	_float m_Duration_s = { 0.f };

private:
	void Save_State();

public:
	virtual void Free() override;
};

END
