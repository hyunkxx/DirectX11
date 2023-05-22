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

class CIntro final : public CGameObject
{
public:
	typedef struct tagLoading
	{
		//클라변수
		_float		fX = { 0.f };
		_float		fY = { 0.f };
		_float		fZ = { 0.f };
		_float		fWidth = { 50.f };
		_float		fHeight = { 50.f };
		_float		fColorR = { 0.f };
		_float		fColorG = { 0.f };
		_float		fColorB = { 0.f };
		_float		fColorA = { 0.f };
		_int		iTexNum = { 0 };
		_int		iPass = { 1 };
		_float		fDegree = { 0.f };
		_float		Duration = { 0.f };
		_float		Cool = { 0.f };
		_float		fSpeedfX = { 0.f };
		_float		fSpeedfY = { 0.f };
		_float		fSpeedfZ = { 0.f };
		_float		fSpeedWidth = { 0.f };
		_float		fSpeedHeight = { 0.f };
		_float		fSpeedAlpha = { 0.f };
		//저장은 안하지만 제로메모리로 불러서 각 버퍼마다 쓰는 변수
		_float		TimeAcc = { 0.f };
		_bool		bSizeXAnimP = { false };
		_bool		bSizeXAnimM = { false };
		_bool		bSizeYAnimP = { false };
		_bool		bSizeYAnimM = { false };
		_bool		bAlphaM = { false };
		_bool		bAlphaP = { false };
		_bool		bCoolTime = { false };
		_bool		bRot = { false };

		//툴변수 저장은 안하지만 제로메모리로 불러서 각 버퍼마다 쓰는 변수
		_float4x4	WorldMatrix;
		_float4 Color;

	}LOADINGDESC;
protected:
	explicit CIntro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CIntro(const CIntro& rhs);
	virtual ~CIntro() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;


public:
	void Set_LoadingBar(_float fNum) { m_fLoadingBar += fNum; }

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources(_uint i);

public:
	static CIntro* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	_bool	Get_LoadingEnd() { return m_bLoadingEnd; }
	void	Set_LoadingEnd() { m_bLoadingEnd = false; }
private:
	void	LoadingBar(_uint iNum, _double TimeDelta);
	void	SizeXAnimP(_uint iNum, _double TimeDelta);
	void	SizeXAnimM(_uint iNum, _double TimeDelta);
	void	SizeYAnimP(_uint iNum, _double TimeDelta);
	void	SizeYAnimM(_uint iNum, _double TimeDelta);
	void	SizeXYP(_uint iNum, _double TimeDelta);
	void	SizeXYM(_uint iNum, _double TimeDelta);
	void	AlphaM(_uint iNum, _double TimeDelta);
	void	AlphaP(_uint iNum, _double TimeDelta);
	void	Load();

private:
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_int m_iPass = { 0 };
	_int m_Index = { 0 };
	_float m_fLoadingBar = { 0.f };
	_float m_fLoading = { 0.f };
	_bool  m_bLoadingEnd = { false };
	_bool  m_bXYM = { false };

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexture = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr }; // 생성, list푸시백용

	vector<LOADINGDESC*>	 m_DescList;
	LOADINGDESC*			CurrentDesc = { nullptr }; // imgui랜더용
};

END