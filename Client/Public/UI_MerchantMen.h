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
class CUI_MerchantMen final : public CGameObject
{
public:
	enum MENSITUINDEX { MEET, MENU, INMENU, MINUSLEVEL, ADDLEVEL, DELIVER, DELIVERCONFIRM, MENEND};
	
	typedef struct tagMerchantMen
	{
		//Ŭ�󺯼�
		_float		fX = { 0.f };
		_float		fY = { 0.f };
		_float		fZ = { 0.f };
		_float		fWidth = { 50.f };
		_float		fHeight = { 50.f };
		_float		fColorA = { 0.f };
		_int		iTexNum = { 0 };
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
	}MERMENDESC;

protected:
	explicit CUI_MerchantMen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_MerchantMen(const CUI_MerchantMen& rhs);
	virtual ~CUI_MerchantMen() = default;

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
	HRESULT Setup_CommonShader(_uint index);
	HRESULT Setup_LevelShader(_uint index);
	HRESULT Setup_CircleShader(_uint index);
	HRESULT Setup_LButtonShader(_uint index);
	HRESULT Setup_RButtonShader(_uint index);
	HRESULT Setup_CasketShader(_uint index);
	HRESULT Setup_OwnShader(_uint index);
	HRESULT Setup_DeliverShader(_uint index);
	HRESULT Setup_0RewardShader(_uint index);
	HRESULT Setup_1RewardShader(_uint index);
	HRESULT Setup_2RewardShader(_uint index);
	HRESULT Setup_3RewardShader(_uint index);
	HRESULT Setup_4RewardShader(_uint index);
	HRESULT Setup_5RewardShader(_uint index);
	
private:
	_bool	AddAlpha(vector<MERMENDESC>* pDesc, _double TimeDelta);
	_bool	MinusAlpha(vector<MERMENDESC>* pDesc, _double TimeDelta);
	_bool	AddAlphaW(vector<MERMENDESC>* pDesc, _double TimeDelta);
	_bool	MinusAlphaW(vector<MERMENDESC>* pDesc, _double TimeDelta);
	void	ColorP(MERMENDESC* pDesc, _float4 fcolor, _double TimeDelta);
	void	ColorM(MERMENDESC * pDesc, _float4 fcolor, _double TimeDelta);
	_bool	SelectUI(MERMENDESC* pDesc);
	void	Load();
	void	Save();

public:
	_bool	IsMouseActive() { return m_bMouseActive; }
	
private:
	MENSITUINDEX Situation = { MENSITUINDEX::MENEND };
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint		m_iPass = { 1 };
	_int		m_Count = {0};
	_bool		m_bMouseActive = { false };




	//��
	_int m_Index = { 0 };
	_bool bTexMod = { false };
	_int bRender = { 1 };
	_float magnification = { 0.f };
	_int type = { 0 };
	MERMENDESC* CurrentDesc = { nullptr };

private:
	vector<MERMENDESC>		  m_MerchantList; // 14
	vector<MERMENDESC>		  m_MenuList; //15
	vector<MERMENDESC>		  m_CommonList;  //0
	vector<MERMENDESC>		  m_LevelList;	//1
	vector<MERMENDESC>		  m_CircleList;	//2	
	vector<MERMENDESC>		  m_LButtonList;//3
	vector<MERMENDESC>		  m_RButtonList;//4
	vector<MERMENDESC>		  m_CasketList;//5
	vector<MERMENDESC>		  m_OwnList;//6
	vector<MERMENDESC>		  m_DeliverList;//7
	vector<MERMENDESC>		  m_0RewardList;//8
	vector<MERMENDESC>		  m_1RewardList;//9
	vector<MERMENDESC>		  m_2RewardList;//10
	vector<MERMENDESC>		  m_3RewardList;//11
	vector<MERMENDESC>		  m_4RewardList;//12
	vector<MERMENDESC>		  m_5RewardList;//13


public:
	static CUI_MerchantMen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	class CUI_Mouse*		m_pUIMouse = { nullptr };

	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexture = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };

};

END