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

class CUI_MainScreen final : public CGameObject
{
public:
	typedef struct tagCutRect
	{
		//클라변수
		_float2		LUCutUV;
		_float2		RBCutUV;
		_float		fXCut = { 0.f };
		_float		fYCut = { 0.f };
		_float		fZCut = { 0.f };
		_float		fWidthCut = { 50.f };
		_float		fHeightCut = { 50.f };
		_float		fColorACut = { 0.f };
		_float		Duration = { 0.f };
		_float		TimeAcc = { 0.f };
		_float		Cool = { 0.f };
		_float		fSpeedfX= { 0.f };
		_float		fSpeedfY= { 0.f };
		_float		fSpeedfZ= { 0.f };
		_float		fSpeedWidth = { 0.f };
		_float		fSpeedHeight = { 0.f };
		_float		fSpeedAlpha = { 0.f };
		_int		iTexNum		= { 0 };
		_int		iPass = { 1 };
		_bool		bSizeXAnimP = { false };
		_bool		bSizeXAnimM = { false };
		_bool		bSizeYAnimP = { false };
		_bool		bSizeYAnimM = { false };
		_bool		bAlphaM = { false };
		_bool		bAlphaP = { false };
		_bool		bCoolTime = { false };
		_bool		bRot = { false };
		_float		fDegree = { 0.f };
		
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
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResourcesCut(_uint Bufferindex);



public:
	void	Set_Texchange(_int Texindex);
private:
	void	SerectUI();
	void    UVWave(_double TimeDelta);
	void    UVCircle(_double TimeDelta);
	void	HPBar(_double TimeDelta);
	void	HPRedBar(_double TimeDelta);
	


private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_uint   m_iPass = { 1 };
	_float  m_RadianAcc = { 0.f };
	_float2 m_fUV = { 0.f, 0.f };
	_uint m_iCurrentPlayer = { 1 };
	_uint m_iPrePlayer = { 1 };
	//Hp
	_bool m_bHit = { false };
	_bool m_bRedStart = { false };
	_float m_Damage = { 0.f }; // 공통
	_float m_CurrentHp = { 250.f };
	_float m_PreHp = { 250.f };
	_float m_fWhiteBar = { 1.f };
	_float m_RedDamageACC = {250.f };
	_float m_fRedBar = { 1.f };
	_float Point = 250.f;

	
public:
	static CUI_MainScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


	

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexFunc = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCut = { nullptr }; // 생성, list푸시백용

	// 내가 지정한 uv좌표
	list<CVIBuffer_Rect*> m_BufferCutList;
	list<CUTRECT*>		  m_CutDescList;
	CUTRECT*			m_CutDesc = { nullptr }; // 생성, list푸시백용
	CUTRECT*			CurrentCutDesc = { nullptr }; // imgui랜더용
	
	

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
private:
	void	SizeXAnimP(CUTRECT* pDesc, _double TimeDelta);
	void	SizeXAnimM(CUTRECT* pDesc, _double TimeDelta);
	void	SizeYAnimP(CUTRECT* pDesc, _double TimeDelta);
	void	SizeYAnimM(CUTRECT* pDesc, _double TimeDelta);
	void	AlphaM(CUTRECT* pDesc, _double TimeDelta);
	void	AlphaP(CUTRECT* pDesc, _double TimeDelta);
	void	CoolTime(CUTRECT* pDesc, _double TimeDelta);
	void	Rot(CUTRECT* pDesc, _double TimeDelta);
	void	Load();


	

};

END