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
class CUI_MerchantMen final : public CGameObject
{
public:
	enum MENSITUINDEX { MEET, MENU, INMENU, BYE, MENEND};
	
	typedef struct tagMerchantMen
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
	HRESULT Setup_MerchantShader(_uint index);
	HRESULT Setup_MenuShader(_uint index);
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
	HRESULT Setup_MessageShader(_uint index);
	HRESULT Setup_FinalShader(_uint index);

private:
	void	InMenuOpen(_double TimeDelta);
	_bool	InMenuEnd(_double TimeDelta);
	_bool	AddAlpha(MERMENDESC* pDesc, _double TimeDelta);
	_bool	MinusAlpha(MERMENDESC* pDesc, _double TimeDelta);
	_bool	AddAlphaW(vector<MERMENDESC>* pDesc, _double TimeDelta);
	_bool	MinusAlphaW(vector<MERMENDESC>* pDesc, _double TimeDelta);
	void	ColorP(MERMENDESC* pDesc, _float4 fcolor, _double TimeDelta);
	void	ColorM(MERMENDESC * pDesc, _float4 fcolor, _double TimeDelta);
	void	Start_Move(_uint indexstart, _uint indexend, _double TimeDelta);
	void	End_Move(_uint indexstart, _uint indexend, _double TimeDelta);
	_bool	SelectUI(MERMENDESC* pDesc);
	void	Load();
	void	Save();

public: // NPC전용
	void	Set_SituMeet();
	void	Set_END();
	void	Call_END() { m_NPCbye = true;  SetState(ACTIVE); };

private:
	_bool m_NPCbye = { false };
	MENSITUINDEX Situation = { MENSITUINDEX::MEET };
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint		m_iPass = { 1 };
	_int		m_Count = {0};
	_float		m_TimeAcc = { 0.f };
	_bool		m_InMenuRenderStart = { true };
	_bool		m_MenuRenderStart = { true };
	_bool		m_CircleSTurntart = { true };
	_bool		m_CircleLevDown = { false };
	_bool		m_CircleSLevUp = { false };
	_bool		m_bRewardReceived[5] = { false,false ,false ,false ,false };
	_bool		m_MsgboxRender = { false };
	_bool		m_RewardboxRender = { false };
	_bool		m_CancelMsgbox = { false };
	_float		Degree = { 0.f }; // 레벨 원 돌리는 각도
	_float		SubDegree = { 0.f }; // 레벨 원 돌리는 각도
	_int		SettingLevel = { 10 };
	_int		m_PlayerMaxLevel = { 50 };
	CItem::ITEM_DESC itemDesc[6];

	// 플레이어state랑 연결시킬것
	_int		m_PlayerCurrentLevel = { 50 };
	// 임시 변수
	_int		CurrentOwn =  { 2 }; //피스
	_int		NeedNum		= { 0 };
	_int		ItemNum		= { 0 }; //완성품

	//툴
	_int iSituation = { 0 };
	_int m_Index = { 0 };
	_bool bTexMod = { false };
	_int bRender = { 1 };
	_float magnification = { 0.f };
	_int type = { 0 };
	MERMENDESC* CurrentDesc = { nullptr };
private:
	vector<MERMENDESC>		  m_CommonList;		//0
	vector<MERMENDESC>		  m_LevelList;		//1
	vector<MERMENDESC>		  m_CircleList;		//2	
	vector<MERMENDESC>		  m_LButtonList;	//3
	vector<MERMENDESC>		  m_RButtonList;	//4
	vector<MERMENDESC>		  m_CasketList;		//5
	vector<MERMENDESC>		  m_OwnList;		//6
	vector<MERMENDESC>		  m_DeliverList;	//7
	vector<MERMENDESC>		  m_0RewardList;	//8
	vector<MERMENDESC>		  m_1RewardList;	//9
	vector<MERMENDESC>		  m_2RewardList;	//10
	vector<MERMENDESC>		  m_3RewardList;	//11
	vector<MERMENDESC>		  m_4RewardList;	//12
	vector<MERMENDESC>		  m_5RewardList;	//13
	vector<MERMENDESC>		  m_MerchantList;	//14
	vector<MERMENDESC>		  m_MenuList;		//15
	vector<MERMENDESC>		  m_MessageList;	//16
	vector<MERMENDESC>		  m_FinalList;		//17

public:
	static CUI_MerchantMen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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