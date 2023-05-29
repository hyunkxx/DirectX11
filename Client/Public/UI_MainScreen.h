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
class CP_PlayerGirl;
class CPlayerState;

class CUI_MainScreen final : public CGameObject
{
public: enum class eKeyType
{
	DANSUN, YANGYANG, RED, T, Q, E, R, I0, I1, I2, RR, GRAPH0, GRAPH1, GRAPH2, PLAYEREND
};
public:
	typedef struct tagCutRect
	{
		//클라변수
		_float		fXCut = { 0.f };
		_float		fYCut = { 0.f };
		_float		fZCut = { 0.f };
		_float		fWidthCut = { 50.f };
		_float		fHeightCut = { 50.f };
		_float		fColorACut = { 0.f };
		_float		Duration = { 0.f };
		_float		TimeAcc = { 0.f };
		_float		Cool = { 0.f }; // 쿨타임UV 0~1사이값
		_float		fSpeedfX = { 0.f };
		_float		fSpeedfY = { 0.f };
		_float		fSpeedfZ = { 0.f };
		_float		fSpeedWidth = { 0.f };
		_float		fSpeedHeight = { 0.f };
		_float		fSpeedAlpha = { 0.f };
		_int		iTexNum = { 0 };
		_int		iPass = { 1 };
		_bool		bAlphaM = { false };
		_bool		bAlphaP = { false };
		_bool		bCoolTime = { false };
		_int		eKeyType;
		_bool		bRender = { true };
		_float2     AimPos;
		_float2     OriPos;

		//툴변수 저장은 안하지만 제로메모리로 불러서 각 버퍼마다 쓰는 변수
		_float4x4	WorldMatrixCut;
		_float4 ColorCut;
		_float fColorRCut = { 0.f };
		_float fColorGCut = { 0.f };
		_float fColorBCut = { 0.f };
		_bool  OnRect; // 마우스가 버퍼 위에 있는지
	}CUTRECT;

protected:
	explicit CUI_MainScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_MainScreen(const CUI_MainScreen& rhs);
	virtual ~CUI_MainScreen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Start();
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResourcesCut(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesGraphs0(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesGraphs1(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesGraphs2(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesT(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesQ(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesE(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesR(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesRR(_uint Bufferindex); // R색깔테두리
	HRESULT Setup_ShaderResourcesI0(_uint Bufferindex); // Hp바 옆에 아이콘테두리
	HRESULT Setup_ShaderResourcesI1(_uint Bufferindex); // Hp바 옆에 아이콘테두리
	HRESULT Setup_ShaderResourcesI2(_uint Bufferindex); // Hp바 옆에 아이콘테두리
	HRESULT Setup_ShaderResourcesPlayer(_uint Bufferindex); // 플레이어변경

public:
	void	Set_Texchange(_int Texindex);
	void	Set_Damage(_float fDamage) { m_Damage = -fDamage; m_bHit = true; }
	void	Add_PlayerNum() { ++m_HavePlayerNum; ++m_HadPlayerNum; AddPlayerRender(); }


private:
	void	SerectUI();
	void	SetHP();
	void	SetStaticSkillCoolTime();
	void	SetCurCoolTime();
	void	SetCurCoolRadian();
	void	SetPlayer();
	void	HPBar(_double TimeDelta);
	void	HPRedBar(_double TimeDelta);
	void    UVWave(_double TimeDelta);
	_bool	QTEFull();
	void	QTEAct(_double TimeDelta);
	_bool	RRFull();
	void	RRAct(_double TimeDelta);
	
	void	AlphaM(CUTRECT* pDesc, _double TimeDelta);
	void	AlphaP(CUTRECT* pDesc, _double TimeDelta);

	void	CountDownCool(_double TimeDelta);
	void	CoolTimeEnd(_double TimeDelta);
	_bool	CoolTime(CUTRECT* pDesc, _double TimeDelta);


	_float	Distance(_fvector TargetPos, _fvector CurrentPos);
	_bool	SizeXYP(CUTRECT* pDesc, _double TimeDelta);
	_bool	SizeXYM(CUTRECT* pDesc, _double TimeDelta);

	void	Start_Go(_double TimeDelta);
	void	Start_Come(_double TimeDelta);
	void	DisappearIcon(_uint indexstart, _uint indexend, _double TimeDelta);
	void	AppearIcon(_uint indexstart, _uint indexend, _double TimeDelta);

	void RenderCurrentPlayer1();
	void RenderCurrentPlayer2();
	void RenderCurrentPlayer3();

	void RenderPlayer1();
	void RenderPlayer2();
	void RenderPlayer3();

	void RenderOffPlayer1();
	void RenderOffPlayer2();
	void RenderOffPlayer3();

	void AddPlayerRender();

	void ChoicePlayer1();
	void ChoicePlayer2();
	void ChoicePlayer3();

	void TCoolRender();
	void ECoolRender();
	void QCoolRender();
	void RCoolRender();

	void TCoolRenderOff();
	void ECoolRenderOff();
	void QCoolRenderOff();
	void RCoolRenderOff();

	_bool TEnd(_double TimeDelta);
	_bool EEnd(_double TimeDelta);
	_bool QEnd(_double TimeDelta);
	_bool REnd(_double TimeDelta);

	void	Load();
private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint   m_iPass = { 1 };
	_bool	m_bRender = { true }; // 전체 랜더onoff
	eKeyType m_iCurrentPlayer = { eKeyType::DANSUN };
	eKeyType m_iPrePlayer = { eKeyType::DANSUN };
	_uint	m_HavePlayerNum = { 0 };
	_uint	m_HadPlayerNum = { 0 };

	//Hp
	_bool m_bHit = { false };
	_bool m_bRedStart = { false };
	_float m_Damage = { 0.f }; // 공통
	_float m_HP = { 100.f };
	_float m_CurrentHp = { 100.f };
	_float m_PreHp = { 100.f };
	_float m_fWhiteBar = { 1.f };
	_float m_RedHP = { 500.f }; // 빨간바 차있는 부분
	_float m_fRedBar = { 1.f };

	//쿨타임
	_bool  Coolend = { false };
	_float Time = { 1.f };

	_bool  bTEnd = { false }, bEEnd = { false }, bQEnd = { false }, bREnd = { false };
	_float StaticHookCoolTime = { 0.f }, StaticLevitationCoolTime = { 0.f }, StaticScannerCoolTimel = { 0.f };
	_float StaticESkillTime = { 0.f }, StaticQSkillTime = { 0.f }, StaticRSkillTime = { 0.f };
	_float TCoolTime = { 0.f }, ECoolTime = { 0.f }, QCoolTime = { 0.f }, RCoolTime = { 0.f };
	_float TCoolRadian = { 0.f }, ECoolRadian = { 0.f }, QCoolRadian = { 0.f }, RCoolRadian = { 0.f };
	_float RRCurGauge = { 0.f }, RRMaxGauge = { 0.f }, RRRadian = { 0.f };
	_float QTERadian = { 0.f }, QTECurGauge = { 0.f }, QTEMaxGauge = { 0.f };
	_float SkillRadian = { 0.f }, SkillCurGauge = { 0.f }, SkillMaxGauge = { 0.f };

	_float2 m_fUV = { 0.f, 0.f }; // 텍스처uv흘리는애
	_int m_MainMaskNum;
	_int m_MainMaskNum2;
	_int m_Mini0MaskNum;
	_int m_Mini0MaskNum2;
	_int m_Mini1MaskNum;
	_int m_Mini1MaskNum2;
	_int m_Mini2MaskNum;
	_int m_Mini2MaskNum2;

public:
	static CUI_MainScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:/*
	ELEMENT::ELMT_SPECTRA = _float3{ 214.959f * 255.f, 208.637f * 255.f, 126.347f * 255.f };
	ELEMENT::ELMT_AERO = _float3{ 107.f * 255.f, 234.f * 255.f, 219.f * 255.f };
	ELEMENT::ELMT_FUSION = _float3{ 158.058f * 255.f, -255.f * 255.f, -231.818f * 255.f };*/

private:
	CP_PlayerGirl*  m_pPlayer = { nullptr };
	CPlayerState*   m_pPlayerStateClass = { nullptr };




private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexFunc = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; 

	vector<CUTRECT*>		  m_CutDescList;

};

END