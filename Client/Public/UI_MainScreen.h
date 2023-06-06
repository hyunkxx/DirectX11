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
class CUI_MainScreen final : public CGameObject
{
public: enum class eKeyType
{
	DANSUN, YANGYANG, RED, T, Q, E, R, I0, I1, I2, RR, GRAPH0, GRAPH1, GRAPH2, PLAYEREND
};
public:
	typedef struct tagCutRect
	{
		//Ŭ�󺯼�
		_float		fXCut = { 0.f };
		_float		fYCut = { 0.f };
		_float		fZCut = { 0.f };
		_float		fWidthCut = { 50.f };
		_float		fHeightCut = { 50.f };
		_float		fColorACut = { 0.f };
		_float		Duration = { 0.f };
		_float		TimeAcc = { 0.f };
		_float		Cool = { 0.f }; // ��Ÿ��UV 0~1���̰�
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
		//������ ������ �������� ���θ޸𸮷� �ҷ��� �� ���۸��� ���� ����
		_float4x4	WorldMatrixCut;
		_float4 ColorCut;
		_float fColorRCut = { 0.f };
		_float fColorGCut = { 0.f };
		_float fColorBCut = { 0.f };
	}CUTRECT;

	typedef struct tagDamage
	{
		_float3		Pos;
		_float2		Size;
		_int		TextureNum;
		_float4x4	WorldMat;
		_float4		Color;
	}DAMAGEDESC;

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
	HRESULT Setup_ShaderResourcesDamage(DAMAGEDESC * pDamage);
	HRESULT Setup_ShaderResourcesCut(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesGraphs0(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesGraphs1(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesGraphs2(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesT(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesQ(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesE(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesR(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesRR(_uint Bufferindex); // R�����׵θ�
	HRESULT Setup_ShaderResourcesI0(_uint Bufferindex); // Hp�� ���� �������׵θ�
	HRESULT Setup_ShaderResourcesI1(_uint Bufferindex); // Hp�� ���� �������׵θ�
	HRESULT Setup_ShaderResourcesI2(_uint Bufferindex); // Hp�� ���� �������׵θ�
	HRESULT Setup_ShaderResourcesPlayer(_uint Bufferindex); // �÷��̾��

public:
	void	Set_Texchange(_int Texindex);
	void	Set_Damage(_float fDamage) { m_Damage = -fDamage; m_bHit = true; Damage(m_Damage);}


private:
	void	SetHP();
	void	OtherobjIsActive(_double TimeDelta);
	void	OffRender(_double TimeDelta);
	void	OnRender(_double TimeDelta);
	void	Counting();
	void	SetStaticSkillCoolTime();
	void	SetCurCoolTime();
	void	SetCurCoolRadian();
	void	SetPlayer();
	void	SetSlotRender();
	void	HP(_double TimeDelta);
	void	HPBar(_double TimeDelta);
	void	HPRedBar(_double TimeDelta);
	void    UVWave(_double TimeDelta);
	_bool	QTEFull();
	void	QTEAct(_double TimeDelta);
	_bool	RRFull();
	void	RRAct(_double TimeDelta);
	void	Font(_double TimeDelta);
	void	Damage(_float Damage);
	void	T(_double TimeDelta);
	void	E(_double TimeDelta);
	void	Q(_double TimeDelta);
	void	R(_double TimeDelta);


	void	AlphaM(CUTRECT* pDesc, _double TimeDelta);
	void	AlphaP(CUTRECT* pDesc, _double TimeDelta);

	void	CoolTimeEnd(_double TimeDelta);
	_bool	CoolTime(CUTRECT* pDesc, _double TimeDelta);


	_float	Distance(_fvector TargetPos, _fvector CurrentPos);
	_bool	SizeXYP(CUTRECT* pDesc, _double TimeDelta);
	_bool	SizeXYM(CUTRECT* pDesc, _double TimeDelta);

	
	
	void RenderPlayer1();
	void RenderPlayer2();
	void RenderPlayer3();

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
	_int m_Index = { 0 }; // �ӽ�
	_bool m_bRenderCheck = { true };
	_int  m_EntireCount = { 0 };
	_int  m_Count = { 0 };
	_bool	m_bRender = { true }; // ��ü ����onoff
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint   m_iPass = { 1 };
	_uint	m_HavePlayerNum = { 1 };
	_uint	m_HadPlayerNum = { 1 };
	//�÷��̾� ����
	_float CurTagCool = { 0.f }, MaxTagCool = { 0.f }, TagRadian = { 0.f };

	//Hp
	_bool m_bHit = { false };
	_bool m_bRedStart = { false };
	_float m_Damage = { 0.f }; // ����
	_float m_HP = { 100.f };
	_float m_CurrentHp = { 100.f };
	_float m_PreHp = { 100.f };
	_float m_fWhiteBar = { 1.f };
	_float m_RedHP = { 500.f }; // ������ ���ִ� �κ�
	_float m_fRedBar = { 1.f };

	//��Ÿ��
	_bool  Coolend = { false };
	_bool  bTEnd = { false }, bEEnd = { false }, bQEnd = { false }, bREnd = { false };
	_float StaticHookCoolTime = { 0.f }, StaticLevitationCoolTime = { 0.f }, StaticScannerCoolTimel = { 0.f };
	_float StaticESkillTime = { 0.f }, StaticQSkillTime = { 0.f }, StaticRSkillTime = { 0.f };
	_float TCoolTime = { 0.f }, ECoolTime = { 0.f }, QCoolTime = { 0.f }, RCoolTime = { 0.f };
	_float TCoolRadian = { 0.f }, ECoolRadian = { 0.f }, QCoolRadian = { 0.f }, RCoolRadian = { 0.f };
	_float RRCurGauge = { 0.f }, RRMaxGauge = { 0.f }, RRRadian = { 0.f }; // �ð�����
	_float QTERadian = { 0.f }, QTECurGauge = { 0.f }, QTEMaxGauge = { 0.f }; // ü�¹ٿ���������
	_float SkillRadian = { 0.f }, SkillCurGauge = { 0.f }, SkillMaxGauge = { 0.f }; // ���α׷���

	//�׷���
	_float2 m_fUV = { 0.f, 0.f }; // �ؽ�óuv�긮�¾�
	_int m_MainMaskNum;
	_int m_MainMaskNum2;
	_int m_Mini0MaskNum;
	_int m_Mini0MaskNum2;
	_int m_Mini1MaskNum;
	_int m_Mini1MaskNum2;
	_int m_Mini2MaskNum;
	_int m_Mini2MaskNum2;

	//��������Ʈ
	_float	Acc = { 0.f };
	_float4 fFontColor = { 0.f, 0.f, 0.f, 0.f };

public:
	static CUI_MainScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	class CP_PlayerGirl*  m_pPlayer = { nullptr };
	class CPlayerState*   m_pPlayerStateClass = { nullptr };
	class CTerminalUI*	m_pTerminalUI = { nullptr };
	class CUI_Tip*		m_pTip = { nullptr };
	class CUI_MerchantMen*	m_pUIMen = { nullptr };
private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexFunc = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; 

	vector<CUTRECT*>	  m_CutDescList;
	list<DAMAGEDESC>	  DamageList;
};

END