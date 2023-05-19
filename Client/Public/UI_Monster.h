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

protected:
	explicit CUI_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Monster(const CUI_Monster& rhs);
	virtual ~CUI_Monster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
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

public:
	static CUI_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	void Set_Damage(_float fDamage) { m_Damage = -fDamage; m_bHit = true; }
	void Set_CharacterPos(_fvector vCharacterPos) { m_vCharacterPos = vCharacterPos; }
private:
	void HPBar(_double TimeDelta);
	void HPRedBar(_double TimeDelta);
	void CommonHP();
	void CommonLevel();
	void Load();


private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	//Hp
	_bool m_bHit = { false };
	_bool m_bRedStart = { false };
	_float m_Damage = { 0.f }; // 공통
	_float m_CurrentHp = { 250.f };
	_float m_HP = { 250.f };
	_float m_PreHp = { 250.f };
	_float m_fWhiteBar = { 1.f };
	_float m_RedDamageACC = { 250.f };
	_float m_fRedBar = { 1.f };
	_int m_Index = { 0 };

	_int m_MonsterLevel = { 10 };
	_int m_MaskTextureNum = { 57 };
	_int m_MaskTextureNum2 = { 67 };
	_float2 m_MonsterUV = { 0.f, 0.f };
	_float m_MonsterGauge = { 0.f };
	MONSTERTYPE m_MonsterType = { MONSTERTYPE::TYPE1 };

	_vector m_vCharacterPos = { 0.f };;

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; // 생성, list푸시백용
	CTexture*		m_pTexture = { nullptr };

	vector<CVIBuffer_Rect*> m_BufferList;
	vector<MONSTERDESC*>	m_DescList;

	
	

};

END 
