#pragma once
#include "Client_Defines.h"
#include "Character.h"
#include "Renderer.h"
#include "MissilePool.h"
#include "UI_Monster.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CBone;
class CNavigation;
END

BEGIN(Client)
class CUI_Monster;
class CM_Qunjing
	: public CCharacter
{
public:
	enum AI_STATE
	{
		AI_NONE,
		AI_IDLE,
		AI_CHASE,
		AI_STAY,
		AI_ATTACK_MELEE,
		AI_ATTACK_RANGE,
		AI_END
	};

	enum IndividualStates
	{
		IS_IDLE,
		IS_GREETING,
		IS_WALK_B,
		IS_WALK_F,
		IS_WALK_L,
		IS_WALK_R,
		IS_RUN,
		IS_ATTACK01,
		IS_ATTACK02,
		IS_ATTACK03_1,
		IS_ATTACK03_2,
		IS_ATTACK03_3,
		IS_ATTACK03_4,
		IS_ATTACK04,
		IS_ATTACK06,
		IS_ATTACK07,
		IS_ATTACK08,
		IS_BEHIT_S,
		IS_BEHIT_B,
		IS_BEHIT_BLOCK,
		IS_DEAD,
		IS_END
	};

	// 이펙트용 본 월드 행렬 배열 인덱스 
	// 각 본마다 현재 Tracking Effect가 실행중인지 파악하고 사용 중인 이펙트에게 전달할 행렬을 갱신함
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE,
		EBONE_LHAND,
		EBONE_RHAND,
		EBONE_HEAD,
		// 여기까지 몬스터 공통으로 고정
		EBONE_LFOOT,
		EBONE_END
	};

	// 공격 종류
	enum Attacks
	{
		ATK_ATTACK01_1,
		ATK_ATTACK01_2,
		ATK_ATTACK01_3,
		ATK_ATTACK01_4,
		ATK_ATTACK02_1,
		ATK_ATTACK02_2,
		ATK_ATTACK02_3,
		ATK_ATTACK02_4,
		ATK_ATTACK03_1,
		ATK_ATTACK03_2,
		ATK_ATTACK03_4,
		ATK_ATTACK04,
		ATK_ATTACK06,
		ATK_ATTACK07_1,
		ATK_ATTACK07_2,
		ATK_ATTACK08_1,
		ATK_ATTACK08_2,
		ATK_ATTACK08_3,
		ATK_END
	};

	enum Missiles
	{
		MISS_ATTACK01_1,
		MISS_ATTACK01_4,
		MISS_ATTACK02_4,
		MISS_ATTACK03_2,
		MISS_ATTACK03_4,
		MISS_ATTACK04,
		MISS_ATTACK07_1,
		MISS_ATTACK07_2,
		MISS_ATTACK08_2,
		MISS_ATTACK08_3,
		MISS_END
	};

	enum MeleeAttacks
	{
		MA_ATTACK01,
		MA_ATTACK02,
		MA_ATTACK03_1,
		MA_ATTACK08,
		MA_END
	};

private:
	CM_Qunjing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CM_Qunjing(const CM_Qunjing& rhs);
	virtual ~CM_Qunjing() = default;

public:
	virtual HRESULT	Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Start();
	virtual void PreTick(_double TimeDelta);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void RenderGUI() override;

	// static
	// AnimTool에서 저장한 State 정보를 로드
	static HRESULT Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// State 정보 삭제
	static void Release_States();

public: // StateKey 대응 함수 모음
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID, _uint iEffectTypeID, _bool bTracking);
	virtual void Shot_PriorityKey(_uint iLeavePriority);
	virtual void Shot_OBBKey(_bool bOBB, _uint iAttackInfoID);
	virtual void Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID);

public:
	virtual _uint Get_AttackID() override { return m_iCurAttackID; }
	virtual void Get_AttackInfo(_uint iAttackID, TAGATTACK* pAttackInfoOut, _float* pAttackOut) override
	{
		memcpy(pAttackInfoOut, &m_AttackInfos[iAttackID], sizeof(TAGATTACK));
		*pAttackOut = m_tMonsterInfo.fAttack;
	}
	virtual _float Get_PushWeight() override { return m_fPushWeight; }


	virtual void Set_AttackHit(_bool bAttackHit)
	{
		m_bAttackHit = bAttackHit;
	}

	virtual _bool Get_Dying() { return m_bDying; }

	// 포지션 제외한 나머지 변수(체력, 상태 체크용 불값 등) 초기화하는 함수, 몬스터 전용
	virtual void Regen()
	{
		Set_State(ACTIVE);
		Shot_DissolveKey(1, 1.5f);
		m_pHitCollider->SetActive(true);
		m_tMonsterInfo.fCurHP = m_tMonsterInfo.fMaxHP;
		m_bDying = false;
		m_bRender = true;
		m_bAlert = false;
		m_pUIMon->Set_MonHP(m_tMonsterInfo.fCurHP);
	};

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };

private:
	// State
	static SINGLESTATE	m_tStates[IS_END];
	SINGLESTATE			m_tCurState;
	_double				m_StateCoolTimes[IS_END] = { 0.0, };

	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};
	// 매 프레임 이펙트 본 사용중인지 체크해서 저장, 사용중인 본만 행렬 갱신해줌
	_bool				m_bEffectBoneActive[EBONE_END] = { false, };


	// 공격 구조체
	TAGATTACK			m_AttackInfos[ATK_END];
	_uint				m_iCurAttackID = { 0 };	// OBB 히트 시 사용할 공격 구조체ID

	// 미사일 풀
	CMissilePool*		m_MissilePools[MISS_END] = { nullptr, };
	_float3				m_MissileRotAngles[MISS_END];

	//
	_uint				m_iStartAttackID = { 0 };
	_uint				m_iStartAttackArray[MA_END] = {};

	// 몬스터 변수
	MONINFO			m_tMonsterInfo;
	// 타겟 플레이어 > 생성되는 타이밍에 무조건 플레이어 박음
	CCharacter*			m_pTarget = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };
	// 플레이어 인지 여부
	_bool				m_bAlert = { false };
	// 몬스터의 플레이어 인지 거리
	_float				m_fAlertRange = { 0.f };
	// 공격 범위
	_float				m_fAttackRange = { 0.f };
	_float				m_fCloseRange = { 0.f };
	// 몬스터와 플레이어 간의 거리
	_float				m_fTargetDistance = { 0.f };

	// 공격 행동에 대한 글로벌 쿨타임 변수
	_double				m_GlobalCoolTime = { 0.0 };
	_bool				m_bAttackReady = { false };
	_bool				m_bDying = { false };

	// MoveCollider 충돌 시 비교할 무게
	// 밀리는 거리 = 겹친 거리 * (1 - 내 무게 / (상대 무게 + 내 무게))
	_float				m_fPushWeight = {};

	_bool				m_bOBBOnLeft = { false };
	_bool				m_bAttackHit = { false };
	_float				m_fHitPoint = 0;

	//UI추가
	class CUI_Minimap*		m_pUIIcon = { nullptr };
	_int					m_UIIndex = { 0 };
	CUI_Monster*			m_pUIMon = { nullptr };
private:
	HRESULT Add_Components();
	void SetUp_State();
	virtual void Change_Target(CCharacter* pActiveCharacter);
	// 플레이어가 일정 거리 안으로 접근했는지 확인하는 함수
	void Find_Target();
	void Check_Derive();
	// 

	void Init_AttackInfos();
	void Init_Missiles();

	// 적용 중인 쿨타임 TimeDelta 만큼 줄여주는 함수
	void Apply_CoolTime(_double TimeDelta);
	// 다음 행동(상태) 결정
	// void Key_Input(_double TimeDelta);
	void Select_State(_double TimeDelta);
	// 상태에 따른 행동 수행
	void Tick_State(_double TimeDelta);
	// 지형에 의한 예외 처리
	void On_Cell();
	// 
	virtual void On_Hit(CCharacter* pChar, TAGATTACK* pAttackInfo, _float fAttackPoint, _float3* pEffPos, _float fCritRate, _float fCritDMG);

	virtual _float Get_CritRate()
	{
		return m_tMonsterInfo.fCriticalRate;
	}
	virtual _float Get_CritDMG()
	{
		return m_tMonsterInfo.fCriticalDMG;
	}

	// Effect
	HRESULT	Init_EffectBones();
	void	Update_EffectBones();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CM_Qunjing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetHitCollider() const override { return m_pHitCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }
	void CM_Qunjing::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CM_Qunjing::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CM_Qunjing::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CCollider* m_pCollider = nullptr;


	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;
	class CCameraMovement* m_pCamMovement = nullptr;
};

END