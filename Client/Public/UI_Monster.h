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
public:
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

		//툴변수 저장은 안하지만 제로메모리로 불러서 각 버퍼마다 쓰는 변수
		_float4x4	WorldMatrix;
		_float4 Color;
		_float fColorR = { 0.f };
		_float fColorG = { 0.f };
		_float fColorB = { 0.f };
		_bool  OnRect; // 마우스가 버퍼 위에 있는지
		_float2 PrePos;
		_float2 OriPos;
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
	HRESULT Setup_ShaderResources(_int index);

private:

	_float4x4	m_ViewMatrix, m_ProjMatrix;

	_float  m_fX = { 0.f }, m_fY = { 0.f }, m_fZ = { 0.f }, m_fWidth = { 10.f }, m_fHeight = { 10.f };
	_float  m_fColorR = { 0.f };
	_float  m_fColorG = { 0.f };
	_float  m_fColorB = { 0.f };
	_float  m_fColorA = { 0.f };
	_float4 Color = { 0.f, 0.f, 0.f, 0.f };
public:
	static CUI_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	void HPBar(_double TimeDelta);
	void HPRedBar(_double TimeDelta);
	void Load();


private:
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
	_float Point = 250.f;
	_int m_Index = { 0 };
	_float Degree = { 0.f };

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; // 생성, list푸시백용
	CTexture*		m_pTexture = { nullptr };

	vector<CVIBuffer_Rect*> m_BufferList;
	vector<MONSTERDESC*>	m_DescList;

	
	

};

END 
