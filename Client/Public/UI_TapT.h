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

class CUI_TapT final : public CGameObject
{
public:
	typedef struct tagTapT
	{
		//클라변수
		_float		fX = { 0.f };
		_float		fY = { 0.f };
		_float		fZ = { 0.f };
		_float		fWidth = { 50.f };
		_float		fHeight = { 50.f };
		_float		fColorA = { 0.f };
		_int		iTexNum		= { 0 };
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
	}TAPDESC;

protected:
	explicit CUI_TapT(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_TapT(const CUI_TapT& rhs);
	virtual ~CUI_TapT() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources(_uint Bufferindex);

private:
	void	SerectUI();




private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint   m_iPass = { 1 };
	//도구모음창 랜더
	_float m_ShaderY = { 0.f };
	_bool  m_bHolding = { false };
	_vector vMousePos;
public:
	static CUI_TapT* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	void	Rot(TAPDESC* pDesc, _double TimeDelta);
	void	Add_Alpha(_uint indexstart, _uint indexend, _double TimeDelta);
	void	Minus_Alpha(_uint indexstart, _uint indexend, _double TimeDelta);
	void	Add_ShaderY(_double TimeDelta);
	void	Minus_ShaderY(_double TimeDelta);
	void	Start_Move(_uint indexstart, _uint indexend, _double TimeDelta);
	void	Middle_Move(_uint indexstart, _uint indexend, _double TimeDelta);
	void	End_Move(_uint indexstart, _uint indexend, _double TimeDelta);
	_float	Distance(_fvector TargetPos, _fvector CurrentPos);
	void	Load();

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; // 생성, list푸시백용
	CTexture*		m_pTexture = { nullptr };

	vector<CVIBuffer_Rect*> m_BufferList;
	vector<TAPDESC*>		  m_DescList;

	

};

END