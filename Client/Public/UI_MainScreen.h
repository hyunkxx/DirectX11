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
class CTerminal;
class CP_PlayerGirl;
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
		_bool  OnRect; // ���콺�� ���� ���� �ִ���
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
	HRESULT Setup_ShaderResourcesRR(_uint Bufferindex); // R�����׵θ�
	HRESULT Setup_ShaderResourcesI0(_uint Bufferindex); // Hp�� ���� �������׵θ�
	HRESULT Setup_ShaderResourcesI1(_uint Bufferindex); // Hp�� ���� �������׵θ�
	HRESULT Setup_ShaderResourcesI2(_uint Bufferindex); // Hp�� ���� �������׵θ�
	HRESULT Setup_ShaderResourcesPlayer(_uint Bufferindex); // �÷��̾��

public:
	void	Set_Texchange(_int Texindex);
	void	Add_PlayerNum() { ++m_HavePlayerNum; ++m_HadPlayerNum; AddPlayerRender(); }


private:
	void	SerectUI();
	void	Set_Damage(_float fDamage) { m_Damage = -fDamage; m_bHit = true; }
	void    UVWave(_double TimeDelta);
	void	HPBar(_double TimeDelta);
	void	HPRedBar(_double TimeDelta);
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

	void T(_double TimeDelta);
	void E(_double TimeDelta);
	void Q(_double TimeDelta);
	void R(_double TimeDelta);

	_bool TEnd(_double TimeDelta);
	_bool EEnd(_double TimeDelta);
	_bool QEnd(_double TimeDelta);
	_bool REnd(_double TimeDelta);

	void	Load();
private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint   m_iPass = { 1 };

	eKeyType m_iCurrentPlayer = { eKeyType::DANSUN };
	eKeyType m_iPrePlayer = { eKeyType::DANSUN };
	_uint	m_HavePlayerNum = { 0 };
	_uint	m_HadPlayerNum = { 0 };

	//Hp
	_bool m_bHit = { false };
	_bool m_bRedStart = { false };
	_float m_Damage = { 0.f }; // ����
	_float m_HP = { 100.f };
	_float m_CurrentHp = { 100.f };
	_float m_PreHp = { 100.f };
	_float m_fWhiteBar = { 1.f };
	_float m_RedDamageACC = { 100.f }; // ������ ���ִ� �κ�
	_float m_fRedBar = { 1.f };

	//��Ÿ��
	_bool  Coolend = { false };
	_float Time = { 1.f };

	_bool  bTEnd = { false }, bEEnd = { false }, bQEnd = { false }, bREnd = { false };
	_bool  UsedTSkill[3] = { false,false,false }, UsedESkill[3] = { false,false,false }, UsedQSkill[3] = { false,false,false }, UsedRSkill[3] = { false,false,false };
	_float StaticTSkillTime = { 3.f }, StaticESkillTime = { 5.f }, StaticQSkillTime = { 7.f }, StaticRSkillTime = { 9.f };
	_float TTimeAcc[3] = { 0.f,0.f,0.f }, ETimeAcc[3] = { 0.f,0.f,0.f }, QTimeAcc[3] = { 0.f, 0.f, 0.f, }, RTimeAcc[3] = { 0.f, 0.f, 0.f, };
	_float TCoolTime[3] = { 3.f,3.f ,3.f }, ECoolTime[3] = { 5.f ,5.f ,5.f }, QCoolTime[3] = { 7.f ,7.f ,7.f }, RCoolTime[3] = { 9.f ,9.f ,9.f };
	_float TCoolRadian[3] = { 0.f ,0.f ,0.f }, ECoolRadian[3] = { 0.f,0.f ,0.f }, QCoolRadian[3] = { 0.f ,0.f ,0.f }, RCoolRadian[3] = { 0.f ,0.f,0.f };
	_float RRRadian[3] = { 0.f ,0.f,0.f };
	_float IconRadian[3] = { 0.f ,0.f,0.f };

	_float2 m_fUV = { 0.f, 0.f }; // �ؽ�óuv�긮�¾�
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




private:
	CP_PlayerGirl*  m_pPlayer = { nullptr };
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexFunc = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; // ����, listǪ�ù��

	vector<CUTRECT*>		  m_CutDescList;



	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


};

END