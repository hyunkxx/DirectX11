#pragma once

#include "Character.h"


BEGIN(Client)

class CTestChar abstract : public CCharacter
{
public:
	typedef struct tagMultiAnimStateInfo_Tool
	{
		_int iAnimID[ANIMSET_END];
		//
		_int iNextState;

		_int iRotationType;
		_float FramePerSec; // == TicksPerSec
		_bool bLoop;
		_bool bLerp;
		_bool bRootMotion;
		_bool bApplyCoolTime;
		_bool bWeaponState;
		_float CoolTime;
		_int iPhysicMoveID; // RootMotion�� false �� ��� ����� ��� � ������ ID
		_int iPriority; // �켱����


		// StateKeys
		_int iKeyCount;
		CStateKey** ppStateKeys;
	}MULTISTATE_TOOL;

	typedef struct tagSingleAnimStateInfo_Tool
	{
		_int iAnimID;
		//
		_int iNextState;
		_int iRotationType;
		_float FramePerSec; // == TicksPerSec
		_bool bLoop;
		_bool bLerp;
		_bool bRootMotion;
		_bool bApplyCoolTime;
		_bool bWeaponState;
		_float CoolTime;
		_int iPhysicMoveID; // RootMotion�� false �� ��� ����� ��� � ������ ID
		_int iPriority;
		// StateKeys
		_int iKeyCount;
		CStateKey** ppStateKeys;
	}SINGLESTATE_TOOL;


	// NONE : ȸ�� ����, ONSTART : ���� ���� �� 1ȸ Ÿ�� ������ �ٶ�, LOOKAT : ���� ���� �� �� ������ Ÿ�� ������ �ٶ�
	// TURN : ���� ���� �� �� ������ Ÿ�� �������� Transform�� ȸ�� ���ӵ� ��ŭ ȸ����
	enum StateRotationType
	{
		ROT_NONE,
		ROT_ONSTART,
		ROT_LOOKAT,
		ROT_TURN,
		ROT_END,
	};


protected:
	CTestChar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestChar(const CTestChar& rhs);
	virtual ~CTestChar() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();
	virtual void RenderGUI();
	virtual const char* Get_StateTag(_uint iIndex) { return nullptr; }
	virtual const char* Get_RotationTag(_uint iIndex) { return szRotationTag[iIndex]; }
	virtual const char* Get_PhysicMoveTag(_uint iIndex) { return szPhysicMoveTag[iIndex]; }


	static const char szSharedStateTag[SS_END][MAX_PATH];
	static const char szRotationTag[ROT_END][MAX_PATH];
	static const char szPhysicMoveTag[CCharacter::PSP_END][MAX_PATH];



protected:
	//�׽�Ʈ�� ����
	_int	m_iStateID = { 0 }; // ���� State index
	_bool	m_bPlay = { true }; // �ִϸ��̼� ��� ����, false == ����

	//
	_float3				m_vMovement = { 0.f, 0.f, 0.f };
	_float3				m_vPrevMovement = { 0.f, 0.f, 0.f };

	_float4				m_vRotation = { 0.f, 0.f, 0.f, 0.f };

public:
	virtual void Free() override;
};

END
