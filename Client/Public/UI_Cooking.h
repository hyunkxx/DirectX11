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
class CUI_Cooking final : public CGameObject
{
	enum COOKSITUINDEX { MEET, SETTINGNUM, MENU, READYCONFRIM, CONFRIM, BYE, COOKEND };

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
		_int		SlotItemId;
		//툴변수 저장은 안하지만 제로메모리로 불러서 각 버퍼마다 쓰는 변수
		_float4x4	WorldMatrix;
		_float4 Color;
		_float fColorR = { 0.f };
		_float fColorG = { 0.f };
		_float fColorB = { 0.f };
		_bool  OnRect; // 마우스가 버퍼 위에 있는지
	}COOKDESC;

protected:
	explicit CUI_Cooking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Cooking(const CUI_Cooking& rhs);
	virtual ~CUI_Cooking() = default;

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

	HRESULT Setup_CookShader(_uint index);
	HRESULT Setup_CommonShader(_uint index);
	HRESULT Setup_GradeShader(_uint index);
	HRESULT Setup_DetailsShader(_uint index);
	HRESULT Setup_0MaterialShader(_uint index);
	HRESULT Setup_1MaterialShader(_uint index);
	HRESULT Setup_2MaterialShader(_uint index);
	HRESULT Setup_SlotShader(vector<COOKDESC>* Desc, _uint index);
	HRESULT Setup_LButtonShader(_uint index);
	HRESULT Setup_RButtonShader(_uint index);
	HRESULT Setup_FinalShader(_uint index);

private:
	void	SettingNumbers();
	void	SettingTotal(_int MakeNum);
	void	SettingOwnMaterial0(_int * pFood);
	void	SettingOwnMaterial1(_int * pFood);
	void	SettingOwnMaterial2(_int * pFood);
	void	SettingNeedMaterial0(_int * pFood);
	void	SettingNeedMaterial1(_int * pFood);
	void	SettingNeedMaterial2(_int * pFood);
	void	SettingOwnNum(_int * pFood);
	void	SelectSlot();
	void	SelectGreade();
	void	SettingSlot();
	void	InMenuOpen(_double TimeDelta);
	_bool	InMenuEnd(_double TimeDelta);
	void	IsMouseinRect();
	_bool	AddAlpha(COOKDESC* pDesc, _double TimeDelta);
	_bool	MinusAlpha(COOKDESC* pDesc, _double TimeDelta);
	_bool	AddAlphaW(vector<COOKDESC>* pDesc, _double TimeDelta);
	_bool	MinusAlphaW(vector<COOKDESC>* pDesc, _double TimeDelta);
	void	ColorP(COOKDESC* pDesc, _float4 fcolor, _double TimeDelta);
	void	ColorM(COOKDESC * pDesc, _float4 fcolor, _double TimeDelta);

	_bool	SelectUI(COOKDESC* pDesc);
	void	Load();
	void	Save();

public: // NPC전용
	void	Set_SituMeet();
	void	Set_END();
	void	Call_END() { m_NPCbye = true; SetState(ACTIVE); };
	
private:
	_bool m_NPCbye = { false };
	COOKSITUINDEX Situation = { COOKSITUINDEX::COOKEND };
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint		m_iPass = { 21 };
	_bool		m_MenuRenderStart = { true };
	_bool		m_MenuOutStart = { false };
	_bool		m_ReadyConfirmRender = { false };
	_bool		m_ConfirmRenderStart = { true };
	_int		m_Count = { 0 };
	COOKDESC*   pSelectSlot = { nullptr };
	_int*		pOwnFood = { nullptr }; // 보유 요리 숫자를 띄울 변수
	_int*		pNeedMaterial0 = { nullptr };
	_int*		pNeedMaterial1 = { nullptr };
	_int*		pNeedMaterial2 = { nullptr };
	_int*		pOwnMaterial0 = { nullptr };
	_int*		pOwnMaterial1 = { nullptr };
	_int*		pOwnMaterial2 = { nullptr };
	_int		MakeNum = { 1 };	// 만들 요리의 수량
	_bool		bMake = { false };
	_bool		bEnoughMaterial0 = { false };
	_bool		bEnoughMaterial1 = { false };
	_bool		bEnoughMaterial2 = { false };
	_bool		bUsedRecipe0 = false;
	_bool		bUsedRecipe1 = false;

	// 임시값 설정 나중에 인벤에 연결시키기
	_int    GradeIndex = { 0 }; // 0 = all, 1 = 1
								// 요리에 필요한 재료 수량
	_int	NeedMaterial0 = { 0 };
	_int    NeedMaterial1 = { 0 };
	_int    NeedMaterial2 = { 0 };

	// 인벤토리에 있는 재료 수량
	_int	InvenRice = { 0 };
	_int    InvenTofu = { 0 };
	_int    InvenFlour = { 0 };
	_int    InvenMushroom = { 0 };
	_int    InvenChicken = { 0 };
	_int    InvenEgg = { 0 };
	_int    InvenMeat = { 0 };
	_int    InvenHerb = { 0 };
	_int    InvenViolet = { 0 };
	_int    InvenDandelion = { 0 };
	_int    InvenPepper = { 0 };
	_int    InvenSugar = { 0 };
	_int    InvenSalt = { 0 };
	_int    InvenOil = { 0 };
	_int    InvenSoysauce = { 0 };
	_int    InvenRecipe0 = { 0 };
	_int    InvenRecipe1 = { 0 };

	// 인벤토리에 있는 요리 수량 ->Own:
	_int    InvenFlatbread = { 0 };
	_int    InvenSalad = { 0 };
	_int    InvenFriedTofu = { 0 };
	_int    InvenSaltedTea = { 0 };
	_int    InvenHerbTea = { 0 };
	_int    InvenDragonNoodle = { 0 };
	_int    InvenOmurice = { 0 };
	_int    InvenFriedrice = { 0 };
	_int    InvenFriedchicken = { 0 };
	_int    InvenRabbitbread = { 0 };
	_int    InvenFriedMushroom = { 0 };
	_int    InvenMapotofu = { 0 };
	_int    InvenPorkBelly = { 0 };
	_int    InvenDucknoodles = { 0 };

	// 컬러
	_float3 color0 = { 0.f ,0.f, 0.f };
	_float3 color1 = { 0.f ,0.f, 0.f };
	_float3 color2 = { 0.f ,0.f, 0.f };
	_float3 color3 = { 0.f ,0.f, 0.f };
	_float3 color4 = { 0.f ,0.f, 0.f };

	//툴
	_int iSituation = { 0 };
	_int m_Index = { 0 };
	_bool bTexMod = { false };
	_int bRender = { 1 };

	_float magnification = { 0.f };
	_int type = { 0 };
	COOKDESC* CurrentDesc = { nullptr };
	_float2 Size = { 0.f, 0.f };

private:
	vector<COOKDESC>		  m_Cookist;		//0
	vector<COOKDESC>		  m_CommonList;		//1
	vector<COOKDESC>		  m_GradeList;		//2
	vector<COOKDESC>		  m_DetailsList;	//3
	vector<COOKDESC>		  m_0MaterialSlot;	//4
	vector<COOKDESC>		  m_1MaterialSlot;	//5
	vector<COOKDESC>		  m_2MaterialSlot;	//6
	vector<COOKDESC>		  m_LButtonList;	//7
	vector<COOKDESC>		  m_RButtonList;	//8
	vector<COOKDESC>		  m_FinalList;		//9

	vector<COOKDESC>		  m_0Slot;			//0
	vector<COOKDESC>		  m_1Slot;			//1
	vector<COOKDESC>		  m_2Slot;			//2
	vector<COOKDESC>		  m_3Slot;			//3
	vector<COOKDESC>		  m_4Slot;			//4
	vector<COOKDESC>		  m_5Slot;			//5
	vector<COOKDESC>		  m_6Slot;			//6
	vector<COOKDESC>		  m_7Slot;			//7
	vector<COOKDESC>		  m_8Slot;			//8
	vector<COOKDESC>		  m_9Slot;			//9
	vector<COOKDESC>		  m_10Slot;			//10
	vector<COOKDESC>		  m_11Slot;			//11
	vector<COOKDESC>		  m_12Slot;			//12
	vector<COOKDESC>		  m_13Slot;			//13

public:
	static CUI_Cooking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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