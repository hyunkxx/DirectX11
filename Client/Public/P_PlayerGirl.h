 #pragma once
#include "Client_Defines.h"
#include "Character.h"
#include "Renderer.h"
#include "MissilePool.h"
#include "PlayerState.h"
#include "EchoSystem.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTransform;
class CModel_Anim;
class CModel_VTF;
class CSphereCollider;
class CBone;
class CVIBuffer_Rect;
class CTexture;
class CNavigation;
END

BEGIN(Client)

class CCameraMovement;
class CUI_MainScreen;
class CP_PlayerGirl 
	: public CCharacter	
{
public:
	typedef enum INPUT_STATE {
		INPUT_NONE,
		INPUT_MOVE,
		INPUT_DASH,
		INPUT_FRONTSTEP,
		INPUT_BACKSTEP,
		INPUT_SPACE,
		INPUT_ATTACK,
		INPUT_ATTACK_CHARGE,
		INPUT_ATTACK_RELEASE,
		INPUT_SKILL,	// E : 캐릭터 스킬
		INPUT_BURST,	// R : 캐릭터 필살기
		INPUT_SUMMON,	// Q : 에코 소환
		INPUT_TOOL,		// T : 도구 사용
		INPUT_END
	}INPUT;

	enum IndividualStates
	{
		// 캐릭터별 고유 액션 = 공용 액션 끝번호에서 시작
		IS_START = 160,
		IS_ATTACK_01 = 160,
		IS_ATTACK_02,
		IS_ATTACK_03,
		IS_ATTACK_04, // 4타 공격 마무리
		IS_ATTACK_05, // 스킬2 발동 시 추가타
		IS_ATTACK_09, // 차지 공격
		IS_ATTACK_PO_2, // 저스트 회피 후 공격 1타
		IS_ATTACK_PO_3, // 저스트 회피 후 공격 2타
		IS_AIRATTACK_START,
		IS_AIRATTACK_LOOP,
		IS_AIRATTACK_END,
		IS_SKILL_01,
		IS_SKILL_02,
		IS_SKILL_QTE,
		IS_BURST,
		IS_END
	};

	enum PARTS
	{
		PARTS_WEAPON_MAIN,
		PARTS_WEAPON_SUB,
		PARTS_HULU,
		PARTS_END
	};

	// 파츠용 본 배열 인덱스
	enum PARTSBONE
	{
		PBONE_WEAPON1, // 무기 사용 중 칼집 위치
		PBONE_WEAPON2, // 무기 사용 중 검 위치
		PBONE_WEAPON3, // 무기 수납 중 칼집 위치
		PBONE_WEAPON4, // 무기 수납 중 검 위치
		PBONE_WEAPON5, // 바닥(왼쪽)?
		PBONE_WEAPON6, // 바닥(오른쪽)?
		PBONE_WEAPON7, // 등(가장 멀리 // 세로?)?
		PBONE_HULU,		// 항아리 위치
		PBONE_END
	};

	// 이펙트용 본 월드 행렬 배열 인덱스 
	// 각 본마다 현재 Tracking Effect가 실행중인지 파악하고 사용 중인 이펙트에게 전달할 행렬을 갱신함
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE2,	// 명치
		EBONE_WEAPON01,	// 왼손 무기뼈 
		EBONE_WEAPON02,	// 오른손 무기뼈
		EBONE_LHAND,	// 왼손뼈 
		EBONE_RHAND,	// 오른손뼈
		EBONE_END
	};

	// 벽타기용 본
	enum ClimbBone
	{
		CBONE_HEAD,
		CBONE_SPINE,
		CBONE_ROOT,
		CBONE_END
	};

	// 공격 종류
	enum Attacks
	{
		ATK_NONE,		// 0은 OBB 예외처리용으로 NONE으로 넣어줘야 함
		ATK_ATTACK_01,
		ATK_ATTACK_02,
		ATK_ATTACK_03,
		ATK_ATTACK_04, 
		ATK_ATTACK_05_01, // 스킬2 추가타 中 1타 
		ATK_ATTACK_05_02, // 스킬2 추가타 中 2타
		ATK_ATTACK_09,	// 차지 공격 투사체 다단 히트
		ATK_ATTACK_PO_2, // 저스트 회피 후 공격 1타
		ATK_ATTACK_PO_3, // 저스트 회피 후 공격 2타
		ATK_AIRATTACK,
		ATK_SKILL_01,
		ATK_SKILL_02_01, // 스킬2 1타
		ATK_SKILL_02_02, // 스킬2 2타
		ATK_SKILL_02_03, // 스킬2 미사일
		ATK_SKILL_QTE,	// 교체 QTE
		ATK_BURST_01,	// 필살기
		ATK_BURST_02,
		ATK_END
	};

	// 미사일 종류
	enum Missiles
	{
		MISS_ATTACK_03,
		MISS_ATTACK_09,
		MISS_ATTACK_PO_2, 
		MISS_ATTACK_PO_3, 
		MISS_AIRATTACK,
		MISS_SKILL_02, 
		MISS_SKILL_QTE,
		MISS_BURST_01,	// 필살기 1타
		MISS_BURST_02,	// 필살기 2타
		MISS_END
	};

private:
	CP_PlayerGirl(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CP_PlayerGirl(const CP_PlayerGirl& rhs);
	virtual ~CP_PlayerGirl() = default;

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
	static HRESULT Init_States(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static void Release_States();
	static const char szIndividualStateTag[IS_END - IS_START][MAX_PATH];
	static const _int iStateLimit;

	// 
	virtual void Check_Nearst(CCharacter* pChar, _float fDist) override;
	virtual void Check_TimeDelay(_double TimeDelta);

	virtual void Recover_Gauge(_float fSP, _float fBP, _float fTP)
	{
		m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] += fSP;
		m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] += fBP;

		if (m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] > m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL])
			m_pCharacterState->fCurGauge[CPlayerState::GAUGE_SPECIAL] = m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_SPECIAL];

		if (m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] > m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST])
			m_pCharacterState->fCurGauge[CPlayerState::GAUGE_BURST] = m_pCharacterState->fMaxGauge[CPlayerState::GAUGE_BURST];
		
		m_pPlayerStateClass->Gain_QTEGauge(fTP);
	}

	// 교대 처리용 함수
	virtual void Appear(CTransform* pTransform, CCharacter* pTarget, _uint iNaviCellID, _float fDissolveSpeed = 5.f);
	virtual void Disappear(class CTransform** ppTransform, CCharacter** ppTarget, _uint* iNaviCellID);

	virtual void Appear_QTE(CTransform* pTransform, CCharacter* pTarget, _uint iNaviCellID);
	virtual void Disappear_QTE(class CTransform** ppTransform, CCharacter** ppTarget, _uint* iNaviCellID);
	
	virtual void Set_OnControl(_bool bControl)
	{
		m_bOnControl = bControl;
	}

	virtual void Set_ForceIdle()
	{
		m_Scon.iNextState = SS_STAND1_ACTION02;
		SetUp_State();
		SetUp_Animations(false);
	}


public: // StateKey 대응 함수 모음
	virtual void Shot_PartsKey(_uint iParts, _uint iState, _uint iDissolve, _float fDissSpeed);
	virtual void Shot_PriorityKey(_uint iLeavePriority);
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID , _uint iEffectTypeID, _bool bTracking);
	virtual void Shot_OBBKey(_bool bOBB, _uint iAttackInfoID);
	virtual void Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID);

public:
	virtual _uint Get_AttackID() override { return m_iCurAttackID; }
	virtual void Get_AttackInfo(_uint iAttackID, TAGATTACK* pAttackInfoOut, _float* pAttackOut) override
	{
		// 모션 계수
		memcpy(pAttackInfoOut, &m_AttackInfos[iAttackID], sizeof(TAGATTACK));
		// 최종 공격력
		*pAttackOut = m_pCharacterState[CPlayerState::CHARACTER_ROVER].fAttack[CPlayerState::STAT_TOTAL];
	}
	virtual _float Get_PushWeight() override { return m_fPushWeight; }

	virtual _bool Get_Attack()
	{
		return m_bAttack;
	}

	virtual _bool Get_Hit()
	{
		return m_bHit;
	}

	virtual void Set_TimeDelay(_double DelayDuration, _double DelayValue)
	{
		m_TimeDelay = DelayValue;
		m_DelayDuration = DelayDuration;
	}

	void updateAttackDesc();

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pAnimSetCom[ANIMSET_END] = { nullptr };
	CModel_VTF*			m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };

private:
	// State
	static MULTISTATE	m_tStates[CP_PlayerGirl::IS_END];
	MULTISTATE			m_tCurState;
	_double				m_StateCoolTimes[CP_PlayerGirl::IS_END] = { 0.0, };

	// Parts 
	class CParts*		m_Parts[PARTS_END] = { nullptr, };
	CBone*				m_PartsBone[PBONE_END] = { nullptr, };
	_bool				m_bWeaponUsing = { false };

	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};

	// 벽타기용 본
	CBone*				m_pClimbBones[CBONE_END] = { nullptr };
	_float3				m_vClimbExitPos = {};
	_float				m_fClimbExitYGap = {};

	// 공격 구조체
	TAGATTACK			m_AttackInfos[ATK_END];
	_uint				m_iCurAttackID = { 0 };	// OBB 히트 시 사용할 공격 구조체ID

	// 미사일 풀
	CMissilePool*		m_MissilePools[MISS_END] = { nullptr,};
	_float3				m_MissileRotAngles[MISS_END];

	// 플레이어 변수
	// 공중 점프 가능 횟수
	_uint				m_iAirJumpCount = { 100 };
	// 점프 위치 기록 - 착지 조건처리
	_float3				m_vJumpPos = {};
	// 스킬 강화 여부 체크
	_float				m_fSkillGauge = { 100.f };

	_double				m_AimChargeAcc = { 0.0 };

	// 이펙트 재생용 임시
	_float4x4			m_WorldMatrix;

	_bool				m_bInputLock = { true };
	_bool				m_bOnControl = { false };

	// 타겟 관련 변수들, 캐릭터 교체 시 값 변경에 주의
	CCharacter*			m_pNearst = { nullptr };
	_float				m_fNearstDist = { 0.f };
	CCharacter*			m_pFixedTarget = { nullptr };
	_float				m_fTargetDist = { 0.f };
	_double				m_ReleaseTargetTimeAcc = { 0.0 };

	_bool				m_bInputDirMove = { false };
	_float3				m_vInputDir = { };
	
	
	// MoveCollider 충돌 시 비교할 무게
	// 밀리는 거리 = 겹친 거리 * (1 - 내 무게 / (상대 무게 + 내 무게))
	_float				m_fPushWeight = { 0.f };

	// 몬스터한테 공격 타이밍 알려주기 위한 변수
	// 미사일, OBB 발사 프레임에만 true로 변경
	_bool				m_bAttack = { false };
	// 몬스터한테 피격 여부 알려주기 위한 변수
	// 회피x, 대미지 처리 시에만 true로 변경
	_bool				m_bHit = { false };
	// 자신에게만 적용되는 시간계수
	_double				m_TimeDelay = { 1.0 };
	_double				m_DelayDuration = { 0.0 };

	// Rim Light Alpha
	_bool m_bRimToggle = false;
	_float m_fBurstRim = 1.f;
	_float m_fRimAlpha = 0.f;
	
	//파괴 오브젝트 확인용 레이더
	class CRader* m_pRader = { nullptr };

	_bool				m_bShowIdleAction = { true };
	_double				m_IdleTimeAcc = { 0.0 };

private:
	HRESULT Add_Components();
	void Init_AnimSystem();
	void Init_AttackInfos();
	void Init_Missiles();

	void SetUp_State();
	// bContinue == 잔여 프레임을 사용하는 애니메이션인지?
	void SetUp_Animations(_bool bContinue);

	// 적용 중인 쿨타임 TimeDelta 만큼 줄여주는 함수
	void Apply_CoolTime(_double TimeDelta);
	// 다음 행동(상태) 결정
	void Key_Input(_double TimeDelta);
	// 상태에 따른 행동 수행
	void Tick_State(_double TimeDelta);
	// 지형에 의한 예외 처리
	void On_Cell();
	// 피격 처리 함수 
	virtual void On_Hit(CCharacter* pGameObject, TAGATTACK* pAttackInfo, _float fAttackPoint, _float3* pEffPos, _float fCritRate, _float fCritDMG);

	virtual _float Get_CritRate()
	{
		return m_pCharacterState->fCriticalRate[CPlayerState::STAT_TOTAL];
	}
	virtual _float Get_CritDMG()
	{
		return m_pCharacterState->fCriticalDamage[CPlayerState::STAT_TOTAL];
	}

	void On_Dodge();
	
	// Parts
	HRESULT Init_Parts();

	// Effect
	HRESULT	Init_EffectBones();
	void	Update_EffectBones();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

private:
	void Set_WeaponUse(_bool bBool);

public:
	static CP_PlayerGirl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	CCollider* GetDefaultCollider() const { return m_pCollider; }
	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetHitCollider() const override { return m_pHitCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }
	void CP_PlayerGirl::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CP_PlayerGirl::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CP_PlayerGirl::OnCollisionExit(CCollider * src, CCollider * dest) override;

	CTexture* m_pEyeBurstTexture = nullptr;
	CTexture* m_pEyeMaskTexture = nullptr;

	CCollider* m_pCollider = nullptr;

	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pHitCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;

	CGameObject* pStaticObject = nullptr;

	CPlayerState* m_pPlayerStateClass;
	CPlayerState::CHARACTER_STATE* m_pCharacterState;
	class CInventory* m_pInven = nullptr;
	class CCameraMovement* m_pCamMovement = nullptr;
	class CUI_MainScreen* m_pUIMain = { nullptr };


	CEchoSystem* m_pEchoSystem = { nullptr };
};

END
