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

class CUI_Minimap final : public CGameObject
{
public:
	typedef struct tagMApDesc
	{
		//클라변수
		_float4			 Pos = { 0.f, 0.f, 0.f, 1.f }; // 월드에서의 구조물pos , 얘를 중점으로 범위 정해서 미니맵에 출력
		_float2			IconLU = { 0.f,0.f };
		_float2			IconRB = { 0.f,0.f };
		_float2			IconPointLU = { 0.f,0.f };
		_float2			IconPointRB = { 0.f,0.f };
		_float		fX = { 0.f };
		_float		fY = { 0.f };
		_float		fZ = { 0.f };
		_float		fWidth = { 30.f };
		_float		fHeight = { 30.f };
		_float		fColorA = { 0.f };
		_int		iTexNum = { 0 };
		_int		iPass = { 1 };
		_float2     AimPos;
		_float2		OriPos;
		_bool		bRender;

		_float4x4	WorldMatrix;
		_float4     ColorIcon;
	}MAPDESC;

public:
	enum ICONNUM { ARROW, MISSION, MONSTER, BOX, ICONEND };

	typedef struct tagIconDesc
	{
		_vector		vObjectPos;
		_int		TextureNum;
		_float4x4	IconWorldMatrix;
		_int		Index;
		_bool		bRender;
	}ICONDESC;

protected:
	explicit CUI_Minimap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Minimap(const CUI_Minimap& rhs);
	virtual ~CUI_Minimap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResourcesMiniMap();
	HRESULT Setup_ShaderResourcesFrame();
	HRESULT Setup_ShaderResourcesDefaultIcon(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesIcon(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesIcons(ICONDESC* pDesc);

public:
	
	static CUI_Minimap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


public:
	void Set_ObjectPos(_int Index, _fvector vObjectPos);
	_int Add_Icon(_fvector vObjectPos, _int	TextureNum);
private:
	void DecideRender();



private:
	_int m_Num = { 0 };
	//미니맵 기능
	_bool m_bNull = { true };
	// 전체맵에서 내가 출력하려고 지정한 uv좌표
	_float2			m_TerrainLU = { 0.f,0.f };
	_float2			m_TerrainRB = { 0.f,0.f };

	//미니맵에 출력될 텍스처 uv의 전체 지형에서의 pos X,Z값
	_float2			m_fPointLU = { 0.f,0.f };
	_float2			m_fPointRB = { 0.f,0.f };

	_bool			m_Render = { true }; // 미니맵 위로 사라질때 디폴트+일반아이콘 랜더off
	_float2			AimPosMiniMap = { -540.f , -360.f - 150.f / 2.f };
	_float2			OriPosMiniMap = { -540.f , 260.f };
	_float			m_fXMiniMap = { -540.f };
	_float			m_fYMiniMap = { 260.f };
	_float			m_fZMiniMap = { 0.f };
	_float			m_fWidthMiniMap = { 190.f };
	_float			m_fHeightMiniMap = { 190.f };
	_float4x4		m_WorldMatrixMiniMap;

	_float			m_fXFrame = { -540.f };
	_float			m_fYFrame = { 260.f };
	_float			m_fZFrame = { 0.f };
	_float			m_fWidthFrame = { 200.f };
	_float			m_fHeightFrame = { 200.f };
	_float4x4		m_WorldMatrixFrame;
	_int			m_FramePass = { 1 };
	_float4x4		m_ViewMatrix, m_ProjMatrix;

	_float4 ColorMiniMap;
	_float m_fColorAMiniMap = { 0.f };
	_float m_fColorRMiniMap = { 0.f };
	_float m_fColorGMiniMap = { 0.f };
	_float m_fColorBMiniMap = { 0.f };

	_float4 ColorFrame;
	_float m_fColorAFrame = { 0.f };
	_float m_fColorRFrame = { 244.f };
	_float m_fColorGFrame = { 244.f };
	_float m_fColorBFrame = { 234.f };

	//디폴트 아이콘(플레이어표시,시야) 
	_float2			m_DefaultIconLU[2] = { { 0.f,0.f } };
	_float2			m_DefaultIconRB[2] = { { 0.f,0.f } };

	_float2			m_fDefaultIconPointLU[2] = { { 0.f,0.f } };
	_float2			m_fDefaultIconPointRB[2] = { { 0.f,0.f } };


	_float			m_fXDefaultIcon[2] = { -540.f,-540.f };
	_float			m_fYDefaultIcon[2] = { 260.f ,260.f };
	_float			m_fZDefaultIcon[2] = { 0.f,0.f };
	_float			m_fWidthDefaultIcon[2] = { 30.f ,100.f };
	_float			m_fHeightDefaultIcon[2] = { 30.f,120.f };
	_float4x4		m_WorldMatrixDefaultIcon[2];

	_float4 ColorDefaultIcon[2];
	_float m_fColorADefaultIcon[2] = { 0.f,0.f };
	_float  m_MouseMoveAcc = { 0.f };
	_float  m_DegreeAcc = { 0.f };


	//아이콘 추가용
	

	// 아이콘 배치용 임시변수
	_float4  OriPos = {60.f , 0.f, 20.f, 1.f};
	_float4 fPlayerPos;
	_float4 fPrePlayerPos = { 0.f , 0.f, 0.f, 1.f };

private:
	ICONNUM IconTextureNum = { MISSION };
	_vector		m_vObjectPos = { 6.f, 30.f, 5.f, 1.f };
	_float4x4	m_IconWorldMatrix;
	_float2		IconSize = { 50.f, 50.f };
	_bool		m_bRender = { true };

	vector<ICONDESC>	m_DescList;
	_int  Index = 0;

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	CTexture*		m_pTexIcon = { nullptr };
	CTexture*		m_pTexMiniMap = { nullptr };
	CVIBuffer_Rect* m_pVIBufferMiniMap = { nullptr };

	CTexture*		m_pTexDefaultIcon = { nullptr };
	vector<MAPDESC>		  m_IconDescList;
};

END