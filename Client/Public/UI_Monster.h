#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)
class CCharacter;
class CPlayerState;

class CUI_Monster final : public CGameObject
{
public: enum class MONSTERTYPE
{
	TYPE0, // 레벨, HP
	TYPE1, // 레벨, HP, 실드아이콘, 추가방어력바
	BOSS

};

public:
	typedef struct tagMonsterInfo
	{
		MONSTERTYPE	 Type;
		_int		 Level;
		// 몬스터 이름 추가해서 텍스처 넘버랑 연결해야함
	}MONINFO;

	typedef struct tagMonster
	{
		//클라변수
		_float		fX = { 0.f };
		_float		fY = { 0.f };
		_float		fZ = { 0.f };
		_float		fWidth = { 5.f };
		_float		fHeight = { 5.f };
		_float		fColorA = { 0.f };
		_int		iTexNum = { 0 };
		_int		iPass = { 1 };
		_bool		bRot = { false };
		_float		fDegree = { 0.f };
		_float2		PosSet = { 0.f,0.f }; // 플레이어 위치에서 추가로 이동할 거리

										  //툴변수 저장은 안하지만 제로메모리로 불러서 각 버퍼마다 쓰는 변수
		_bool		bRender = { true };
		_float4x4	WorldMatrix;
		_float4 Color;
		_float fColorR = { 0.f };
		_float fColorG = { 0.f };
		_float fColorB = { 0.f };
	}MONSTERDESC;

	typedef struct tagDamage
	{
		_float3		Pos;
		_float2		Size;
		_int		TextureNum;
		_int		HitCount;
		_float4x4	WorldMat;
		_float4		Color;
		ELEMENT		ElementType;
	}DAMAGEDESC;

protected:
	explicit CUI_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Monster(const CUI_Monster& rhs);
	virtual ~CUI_Monster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResourcesHP(_int index);
	HRESULT Setup_ShaderResources(_int index);
	HRESULT Setup_ShaderResourcesMask(_int index);
	HRESULT Setup_ShaderResourcesBoss(_int index);
	HRESULT Setup_ShaderResourcesBossG1(_int index);
	HRESULT Setup_ShaderResourcesBossG2(_int index);
	HRESULT Setup_ShaderResourcesDamage(DAMAGEDESC* pDamage);

public:
	static CUI_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	void Set_MonHP(_float HP) { m_HP = m_CurrentHp = m_RedHP = HP; }
	void Set_Damage(_float fDamage, _bool bCrit) { m_Damage = -fDamage; m_bHit = true; Damage(m_Damage); m_bCrit = bCrit; }
	void Set_CharacterPos(_fvector vCharacterPos) { m_vCharacterPos = vCharacterPos; }
	void Set_ElementType(ELEMENT ElementType) { ElementType = ElementType; }
	void Set_Render(_bool brender) { m_bRender = brender; }
private:
	void HPBar(_double TimeDelta);
	void HPRedBar(_double TimeDelta);
	void CommonHP();
	void CommonLevel();
	void DecideRender(_double TimeDelta);
	void Font(_double TimeDelta);
	void FontColor();
	void Damage(_float Damage);
	void Load();



private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	//Hp
	_bool m_bCrit = { false }; // 크리티컬 판정
	_bool m_bHit = { false };
	_bool m_bRedStart = { false };
	_float m_Damage = { 0.f }; // 공통
	_float m_CurrentHp = { 500.f };
	_float m_HP = { 500.f };
	_float m_PreHp = { 500.f };
	_float m_fWhiteBar = { 1.f };
	_float m_RedHP = { 500.f };
	_float m_fRedBar = { 1.f };
	_int m_Index = { 0 };
	_int m_MonsterLevel = { 10 };
	_int m_MaskTextureNum = { 57 };
	_int m_MaskTextureNum2 = { 54 };
	_float2 m_MonsterUV = { 0.f, 0.f }; // 텍스처uv흘리는애
	_float m_MonsterGauge1 = { 1.f };
	_float m_MonsterGauge2 = { 1.f };
	MONSTERTYPE m_MonsterType = { MONSTERTYPE::BOSS };

	_bool  m_bRender = { true };
	_bool	m_bNameRender = { false };
	_vector m_vCharacterPos = { 0.f, 0.f, 0.f, 1.f };
	_int    m_HitCount = { 0 };
	_float Acc = { 0.f };
	_float4 fFontColor = { 0.f, 0.f, 0.f, 0.f };
	_float DamageY = { 0.f };
private:
	CPlayerState*   m_pPlayerStateClass = { nullptr };

	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; // 생성, list푸시백용
	CTexture*		m_pTexture = { nullptr };

	vector<MONSTERDESC>	m_DescList;

	list<DAMAGEDESC> DamageList;

};

END
