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
class CUI_Souvenir final : public CGameObject
{
	enum SOUSITUINDEX { MEET, MENU, INMENU, DETAILS, CONFRIM, BYE, SOUEND};

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
	}SOUDESC;

protected:
	explicit CUI_Souvenir(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Souvenir(const CUI_Souvenir& rhs);
	virtual ~CUI_Souvenir() = default;

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
	HRESULT Setup_SouShader(_uint index);
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
	_bool	AddAlpha(SOUDESC* pDesc, _double TimeDelta);
	_bool	MinusAlpha(SOUDESC* pDesc, _double TimeDelta);
	_bool	AddAlphaW(vector<SOUDESC>* pDesc, _double TimeDelta);
	_bool	MinusAlphaW(vector<SOUDESC>* pDesc, _double TimeDelta);
	void	ColorP(SOUDESC* pDesc, _float4 fcolor, _double TimeDelta);
	void	ColorM(SOUDESC * pDesc, _float4 fcolor, _double TimeDelta);
	void	MouseMove();
	_bool	SelectUI(SOUDESC* pDesc);
	void	Total();
	void	Load();
	void	Save();

public:
	_bool	IsMouseActive() { return m_bMouseActive; }
	void	Set_SituMeet() { Situation = SOUSITUINDEX::MEET; }
private:
	SOUSITUINDEX Situation = { SOUSITUINDEX::SOUEND };
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint		m_iPass = { 1 };
	_int		m_Count = {0};
	_bool		m_bMouseActive = { false };
	_bool		m_InMenuRenderStart = { true };
	_bool		m_MenuRenderStart = { true };
	_bool		m_DetailRenderStart = { true };
	_bool		m_ConfirmRenderStart = { true };
	_bool		m_bOverPurchase = { false };
	CItem::ITEM_DESC itemDesc[13];
	vector<SOUDESC>*	pSelectSlot = { nullptr };

	//마우스
	_long mouse = 0l;
	_bool m_bMouseMoveStart = { false };

	
	// 임시값 설정 나중에 인벤에 연결시키기
	_int Limibuycount[13] = { 0 };
	_int* pLimibuycount = nullptr;
	_int Limited = 5;
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
	SOUDESC* CurrentDesc = { nullptr };
	_float2 Size = { 0.f, 0.f };

private:
	vector<SOUDESC>		  m_SouList;		//0
	vector<SOUDESC>		  m_MenuList;		//1
	vector<SOUDESC>		  m_CommonList;		//2
	vector<SOUDESC>		  m_DetailsList;	//3
	vector<SOUDESC>		  m_FinalList;		//4
	vector<SOUDESC>		  m_0Slot;			//5
	vector<SOUDESC>		  m_1Slot;			//6
	vector<SOUDESC>		  m_2Slot;			//7
	vector<SOUDESC>		  m_3Slot;			//8
	vector<SOUDESC>		  m_4Slot;			//9
	vector<SOUDESC>		  m_5Slot;			//10
	vector<SOUDESC>		  m_6Slot;			//11
	vector<SOUDESC>		  m_7Slot;			//12
	vector<SOUDESC>		  m_8Slot;			//13
	vector<SOUDESC>		  m_9Slot;			//14
	vector<SOUDESC>		  m_10Slot;			//15
	vector<SOUDESC>		  m_11Slot;			//16
	vector<SOUDESC>		  m_12Slot;			//17

public:
	static CUI_Souvenir* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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