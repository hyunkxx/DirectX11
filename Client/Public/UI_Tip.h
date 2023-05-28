#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
class CRenderTarget;
END

BEGIN(Client)
class CTerminal;

class CUI_Tip final : public CGameObject
{
public:
	typedef struct tagTip
	{
		//Ŭ�󺯼�
		_float		fX = { 0.f };
		_float		fY = { 0.f };
		_float		fZ = { 0.f };
		_float		fWidth = { 50.f };
		_float		fHeight = { 50.f };
		_float		fColorA = { 0.f };
		_int		iTexNum		= { 0 };
		_int		iPass = { 1 };
		_bool		bRender = { true };
		_float2     AimPos;
		_float2     OriPos;
		
		//������ ������ �������� ���θ޸𸮷� �ҷ��� �� ���۸��� ���� ����
		_float4x4	WorldMatrix;
		_float4 Color;
		_float fColorR = { 0.f };
		_float fColorG = { 0.f };
		_float fColorB = { 0.f };
		_bool  OnRect; // ���콺�� ���� ���� �ִ���
	}TIPDESC;
protected:
	explicit CUI_Tip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Tip(const CUI_Tip& rhs);
	virtual ~CUI_Tip() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources(_uint index);
private:
	_bool	AddAlpha(_double TimeDelta);
	_bool	MinusAlpha(_double TimeDelta);
	void    UVWave(_double TimeDelta);
	void	Save();
	void	Load();
private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint   m_iPass = { 1 };

	_float2 m_fUV = { 1.f, 0.f }; // �ؽ�óuv�긮�¾�
	_int m_MainMaskNum = { 6 };

	// ��
	_int m_Index = { 0 };

	_bool  m_bUIRender = { true };
	_float m_TimeAcc = { 0.f };

	
	
public:
	static CUI_Tip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	


	

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexture = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; 
	vector<TIPDESC>		  m_DescList;
};

END