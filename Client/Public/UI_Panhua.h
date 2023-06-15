#pragma once

#include "GameObject.h"
#include "Item.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)
class CUI_Panhua final : public CGameObject
{
	enum PANSITUINDEX { MEET, MENU, INMENU, DETAILS, CONFRIM, BYE, SOUEND};

	typedef struct tagSou
	{
		//클라변수
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

		//툴변수 저장은 안하지만 제로메모리로 불러서 각 버퍼마다 쓰는 변수
		_float4x4	WorldMatrix;
		_float4 Color;
		_float fColorR = { 0.f };
		_float fColorG = { 0.f };
		_float fColorB = { 0.f };
		_bool  OnRect; // 마우스가 버퍼 위에 있는지
	}PANDESC;

protected:
	explicit CUI_Panhua(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Panhua(const CUI_Panhua& rhs);
	virtual ~CUI_Panhua() = default;

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
	HRESULT Setup_PanShader(_uint index);
	HRESULT Setup_MenuShader(_uint index);
	HRESULT Setup_CommonShader(_uint index);
	HRESULT Setup_DetailsShader(_uint index);
	HRESULT Setup_FinalShader(_uint index);
	HRESULT Setup_0SlotShader(_uint index);
	HRESULT Setup_1SlotShader(_uint index);
	HRESULT Setup_2SlotShader(_uint index);
	HRESULT Setup_3SlotShader(_uint index);
	HRESULT Setup_4SlotShader(_uint index);
	HRESULT Setup_5SlotShader(_uint index);
	HRESULT Setup_6SlotShader(_uint index);
	HRESULT Setup_7SlotShader(_uint index);
	HRESULT Setup_8SlotShader(_uint index);
	HRESULT Setup_9SlotShader(_uint index);
	HRESULT Setup_10SlotShader(_uint index);
	HRESULT Setup_11SlotShader(_uint index);
	HRESULT Setup_12SlotShader(_uint index);
	HRESULT Setup_13SlotShader(_uint index);
	HRESULT Setup_14SlotShader(_uint index);
	HRESULT Setup_15SlotShader(_uint index);
	HRESULT Setup_16SlotShader(_uint index);
private:
	void	InMenuOpen(_double TimeDelta);
	_bool	InMenuEnd(_double TimeDelta);
	void	Start_Move(_uint indexstart, _uint indexend, _double TimeDelta);
	void	End_Move(_uint indexstart, _uint indexend, _double TimeDelta);
	void	SettingLimitTexNum();
	void	SettingOverPurchase(); //
	void	SettingCurMoneyTexNum();
	void	SettingCurOwnTexNum();
	void	SettingBuyTexNum();
	void	IsMouseinRect();
	_bool	AddAlpha(PANDESC* pDesc, _double TimeDelta);
	_bool	MinusAlpha(PANDESC* pDesc, _double TimeDelta);
	_bool	AddAlphaW(vector<PANDESC>* pDesc, _double TimeDelta);
	_bool	MinusAlphaW(vector<PANDESC>* pDesc, _double TimeDelta);
	void	ColorP(PANDESC* pDesc, _float4 fcolor, _double TimeDelta);
	void	ColorM(PANDESC * pDesc, _float4 fcolor, _double TimeDelta);
	void	MouseMove();
	_bool	SelectUI(PANDESC* pDesc);
	void	Total();
	void	Load();
	void	Save();

public:
	void	Set_SituMeet() { Situation = PANSITUINDEX::MEET; }
private:
	PANSITUINDEX Situation = { PANSITUINDEX::SOUEND };
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint		m_iPass = { 1 };
	_int		m_Count = {0};
	_bool		m_bMouseActive = { false };
	_bool		m_InMenuRenderStart = { true };
	_bool		m_MenuRenderStart = { true };
	_bool		m_DetailRenderStart = { true };
	_bool		m_ConfirmRenderStart = { true };
	_bool		m_bOverPurchase = { false };
	CItem::ITEM_DESC	itemDesc[17];
	vector<PANDESC>*	pSelectSlot = { nullptr };

	//마우스
	_long mouse = 0l;
	_bool m_bMouseMoveStart = { false };

	
	// 임시값 설정 나중에 인벤에 연결시키기
	_int Limibuycount[17] = { 0 };
	_int* pLimibuycount = nullptr;
	_int Limited = 1;
	_int BuyNum = 0; 
	_int CurrentMoney = 0;
	_int CurrentOwn = 0;
	_int Cost = 10; // 물건 하나당 가격
	_int iTotal = 0; // 총 구매비용


	//툴
	_int iSituation = { 0 };
	_int m_Index = { 0 };
	_bool bTexMod = { false };
	_int bRender = { 1 };

	_float magnification = { 0.f };
	_int type = { 0 };
	PANDESC* CurrentDesc = { nullptr };
	_float2 Size = { 0.f, 0.f };

private:
	vector<PANDESC>		  m_PanList;		//0
	vector<PANDESC>		  m_MenuList;		//1
	vector<PANDESC>		  m_CommonList;		//2
	vector<PANDESC>		  m_DetailsList;	//3
	vector<PANDESC>		  m_FinalList;		//4
	vector<PANDESC>		  m_0Slot;			//5
	vector<PANDESC>		  m_1Slot;			//6
	vector<PANDESC>		  m_2Slot;			//7
	vector<PANDESC>		  m_3Slot;			//8
	vector<PANDESC>		  m_4Slot;			//9
	vector<PANDESC>		  m_5Slot;			//10
	vector<PANDESC>		  m_6Slot;			//11
	vector<PANDESC>		  m_7Slot;			//12
	vector<PANDESC>		  m_8Slot;			//13
	vector<PANDESC>		  m_9Slot;			//14
	vector<PANDESC>		  m_10Slot;			//15
	vector<PANDESC>		  m_11Slot;			//16
	vector<PANDESC>		  m_12Slot;			//17
	vector<PANDESC>		  m_13Slot;			//18
	vector<PANDESC>		  m_14Slot;			//19
	vector<PANDESC>		  m_15Slot;			//20
	vector<PANDESC>		  m_16Slot;			//21

public:
	static CUI_Panhua* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	class CUI_Mouse*		m_pUIMouse = { nullptr };
	class CPlayerState*		m_pPlayerStateClass = { nullptr };
	class CItemDB*			m_pDB = { nullptr };
	class CInventory*		m_pInven = { nullptr };

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexture = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };

};

END