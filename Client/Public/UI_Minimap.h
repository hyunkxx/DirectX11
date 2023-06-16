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
		_float      Dist;
		_float4x4	WorldMatrix;
		_float4     ColorIcon;
	}MAPDESC;

public:
	enum ICONNUM {MOSTER, BOSS, BOX, ROBOT, MERCHANTMEN, SOUVENIR, PANHUA, POTAL, ICONEND };

	typedef struct tagIconDesc
	{
		_vector		vObjectPos;
		_int		TextureNum;
		_float4x4	IconWorldMatrix;
		_int		Index;
		_bool		bRender;
		_float      Dist;
	}ICONDESC;

protected:
	explicit CUI_Minimap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Minimap(const CUI_Minimap& rhs);
	virtual ~CUI_Minimap() = default;

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
	HRESULT Setup_ShaderResourcesMiniMap();
	HRESULT Setup_ShaderResourcesDefaultIcon(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesIcon(_uint Bufferindex);
	HRESULT Setup_ShaderResourcesIcons(ICONDESC* pDesc);

public:

	static CUI_Minimap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


public:
	void Set_ObjectPos(_int Index, _fvector vObjectPos);
	//void Set_Disable(_int Index);
	_bool GetRenderState(_int Index); // 미니맵 아이콘만 리턴
	void SetRender(_int index, _bool bRender); //미니맵, 메인아이콘 둘다 on
	_int Add_Icon(_fvector vObjectPos, _int	TextureNum);
private:
	void DecideRender();
	void OtherobjIsActive();


private:
	_int m_Num = { 0 }; // 미니맵 오브젝트 아이콘 enum
	//미니맵 기능
	_bool m_bNull = { true };
	_bool	m_bRender = { true }; // 전체 랜더onoff
	// 전체맵에서 내가 출력하려고 지정한 uv좌표
	_float2			m_TerrainLU = { 0.f,0.f };
	_float2			m_TerrainRB = { 0.f,0.f };

	//미니맵에 출력될 텍스처 uv의 전체 지형에서의 pos X,Z값
	_float2			m_fPointLU = { 0.f,0.f };
	_float2			m_fPointRB = { 0.f,0.f };

	_bool			m_Render = { true }; // 미니맵 위로 사라질때 디폴트+일반아이콘 랜더off
	_float2			AimPosMiniMap = { -540.f , -360.f - 190.f / 2.f };
	_float2			OriPosMiniMap = { -540.f , 260.f };
	_float			m_fXMiniMap = { -540.f };
	_float			m_fYMiniMap = { 260.f };
	_float			m_fZMiniMap = { 0.f };
	_float			m_fWidthMiniMap = { 190.f };
	_float			m_fHeightMiniMap = { 190.f };
	_float4x4		m_WorldMatrixMiniMap;
	_float4x4		m_ViewMatrix, m_ProjMatrix;

	_float4 ColorMiniMap;
	_float m_fColorAMiniMap = { 30.f };
	_float m_fColorRMiniMap = { 0.f };
	_float m_fColorGMiniMap = { 0.f };
	_float m_fColorBMiniMap = { 0.f };

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

	_float m_fColorADefaultIcon[2] = { 0.f,0.f };

	// 아이콘 배치용 변수
	_float4 fPlayerPos;
	_float2		IconSize = { 30.f, 30.f };
	vector<ICONDESC>	m_DescList; //메인아이콘
	_int  Index = 0;

private:
	class CP_PlayerGirl*		m_pPlayer = { nullptr };
	class CTerminalUI*			m_pTerminalUI = { nullptr };
	class CUI_Tip*				m_pTip = { nullptr };
	class CUI_MerchantMen*		m_pUIMen = { nullptr };
	class CUI_Souvenir*			m_pUISovi = { nullptr };
	class CUI_Panhua*			m_pUIPanhua = { nullptr };
	class CUI_Cooking*			m_pUICook = { nullptr };
private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CTexture*		m_pTexIcon = { nullptr };
	CVIBuffer_Rect* m_pVIBufferMiniMap = { nullptr };

	CTexture*		m_pTexDefaultIcon = { nullptr };
	vector<MAPDESC>		 m_IconDescList;
};

END