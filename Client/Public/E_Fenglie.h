#pragma once
#include "Client_Defines.h"
#include "Character.h"
#include "Renderer.h"
#include "MissilePool.h"
#include "EchoSystem.h"

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
class CE_Fenglie
	: public CCharacter
{
public:
	// ����Ʈ�� �� ���� ��� �迭 �ε��� 
	// �� ������ ���� Tracking Effect�� ���������� �ľ��ϰ� ��� ���� ����Ʈ���� ������ ����� ������
	enum EffectBone
	{
		EBONE_NONE,
		EBONE_SPINE,
		EBONE_LHAND,
		EBONE_RHAND,
		EBONE_HEAD,
		// ������� ���� �������� ����
		EBONE_WEAPON,
		EBONE_END
	};

	// ���� ����
	enum Attacks
	{
		ATK_ATTACK_02,
		ATK_END
	};

	// �̻��� ����
	enum Missiles
	{
		MISS_ATTACK_02,
		MISS_END
	};

private:
	CE_Fenglie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Fenglie(const CE_Fenglie& rhs);
	virtual ~CE_Fenglie() = default;

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
	// AnimTool���� ������ State ������ �ε�
	HRESULT Init_State(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// State ���� ����
	void Release_State();


	virtual HRESULT	Reload_Components();
	virtual void Release_Reloadable();



public: // StateKey ���� �Լ� ����
	virtual void Shot_EffectKey(_tchar* szEffectTag, _uint EffectBoneID, _uint iEffectTypeID, _bool bTracking);
	virtual void Shot_MissileKey(_uint iMissilePoolID, _uint iEffectBoneID);
	virtual void Shot_OBBKey(_bool bOBB, _uint iAttackInfoID);
	virtual void Shot_DissolveKey(_bool bDissolveType, _float fDissolveSpeed);

	virtual void Shot_Echo(CTransform * pTransform, CCharacter * pTarget, _uint iNaviCellID);

public:
	virtual _uint Get_AttackID() override { return 0; }
	virtual void Get_AttackInfo(_uint iAttackID, TAGATTACK* pAttackInfoOut, _float* pAttackOut) override
	{
		memcpy(pAttackInfoOut, &m_AttackInfo, sizeof(TAGATTACK));
		*pAttackOut = m_pEchoSystem->GetEcho(CEchoSystem::EC_PNENUMA_PREDATOR).fAttack;
	}
	virtual _float Get_PushWeight() override { return m_fPushWeight; }



private:
	CRenderer*			m_pRendererCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel_Anim*		m_pModelCom = { nullptr };
	CNavigation*		m_pNaviCom = { nullptr };

private:
	CEchoSystem*		m_pEchoSystem;
	// State
	SINGLESTATE			m_tCurState;
	// Effects
	CBone*				m_EffectBones[EBONE_END] = { nullptr, };
	_float4x4			m_EffectBoneMatrices[EBONE_END] = {};
	// �� ������ ����Ʈ �� ��������� üũ�ؼ� ����, ������� ���� ��� ��������
	_bool				m_bEffectBoneActive[EBONE_END] = { false, };


	// ���� ����ü
	//TAGATTACK			m_AttackInfos[ATK_END];						// �ʿ� �� �迭��
	//CMissilePool*		m_MissilePools[MISS_END] = { nullptr, };	// �ʿ� �� �迭��
	TAGATTACK			m_AttackInfo;

	// �̻��� Ǯ
	CMissilePool*		m_MissilePool = { nullptr, };

	// Ÿ�� �÷��̾� > �����Ǵ� Ÿ�ֿ̹� ������ �÷��̾� ����
	CCharacter*			m_pTarget = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };

	_float				m_fTargetDistance = { 0.f };

	// MoveCollider �浹 �� ���� ����
	_float				m_fPushWeight = {};

private:
	HRESULT Add_Components();
	void SetUp_State();
	virtual void Change_Target(CCharacter* pActiveCharacter);
	// �÷��̾ ���� �Ÿ� ������ �����ߴ��� Ȯ���ϴ� �Լ�

	void Init_AttackInfos();
	void Init_Missiles();
	void Release_Missiles();

	// ���¿� ���� �ൿ ����
	void Tick_State(_double TimeDelta);
	// ������ ���� ���� ó��
	void On_Cell();
	// 

	virtual _float Get_CritRate()
	{
		return 10.f;
	}
	virtual _float Get_CritDMG()
	{
		return 150.f;
	}

	// Effect
	HRESULT	Init_EffectBones();
	void	Update_EffectBones();

	HRESULT	SetUp_ShaderResources();
	HRESULT Setup_ShadowShaderResource();

public:
	static CE_Fenglie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg);
	virtual void Free() override;

	virtual CCollider* GetAttackCollider() const override { return m_pAttackCollider; }
	virtual CCollider* GetMoveCollider() const override { return m_pMoveCollider; }
	void CE_Fenglie::OnCollisionEnter(CCollider * src, CCollider * dest) override;
	void CE_Fenglie::OnCollisionStay(CCollider * src, CCollider * dest) override;
	void CE_Fenglie::OnCollisionExit(CCollider * src, CCollider * dest) override;


	CCollider* m_pAttackCollider = nullptr;
	CCollider* m_pMoveCollider = nullptr;
	class CCameraMovement* m_pCamMovement = nullptr;
};

END