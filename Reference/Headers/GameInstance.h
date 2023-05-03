#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"
#include "LightManager.h"
#include "Graphic_Device.h"

BEGIN(Engine)

typedef CLightManager::LIGHT_MATRIX LIGHT_MATRIX;
typedef CGraphic_Device::VIEWPORT_TYPE VIEWPORT_TYPE;

class ENGINE_DLL CGameInstance final: public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Engine_Initialize(const GRAPHIC_DESC& GraphicDesc, _uint iLevelCount, _uint iCollisionLayerCount, ID3D11Device** ppDevice_out, ID3D11DeviceContext** ppContext_out);
	HRESULT Engine_Tick(_double TimeDelta);

public: //Graphic_Device
	const D3D11_VIEWPORT* GetViewport(VIEWPORT_TYPE eViewportType) const;
	HRESULT Clear_RenderTargetView(_float4 vColor);
	HRESULT Clear_DepthStencilView();
	HRESULT Clear_ShadowDepthStencilView();
	HRESULT Present();

public: //Level_Manager
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pCurrentLevel);
	void RenderLevelUI();

public: //Object_Manager
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _tchar * pObjectTag);
	HRESULT Add_Prototype(_int iObjectEnum, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, _int iObjectEnum, const _tchar* pLayerTag, _tchar* strObjectTag, void* pArg = nullptr);
	void	RenderGUI();

	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	class CGameObject* Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* strObjectTag);
	class CGameObject* Clone_GameObject(_int iObjectEnum, void* pArg = nullptr);

public: //Component_Manager
	HRESULT Add_Prototype(_uint iLevelIndex, _int iComponent, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, _int iComponent, void* pArg = nullptr);

public: //PipeLine
	_float4x4 Get_Transform_float4x4(CPipeLine::TRANSFORM_STATE eState);
	_float4x4 Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORM_STATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE eState);
	_float4 Get_CamPosition();

public: //Input_Device
	HRESULT	Initialize_Input(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

	KEY_STATE	InputKey(_int _key);
	KEY_STATE	InputMouse(MOUSE_KEYSTATE _key);
	_long		InputMouseMove(MOUSE_MOVESTATE eMoveState);

public: // Font_Manager
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontName);
	void Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2 & vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), const _float2 & vScale = _float2(1.f, 1.f), _float fRadian = 0.f, const _float2 & vOrigin = _float2(0.f, 0.f));

public: //Timer_Manager
	HRESULT	CreateTimer(const _tchar* pTimerTag);
	_float	GetTimer(const _tchar* pTimerTag);
	void	SetTimer(const _tchar* pTimerTag);

public: //LightManager
	HRESULT AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* GetLightDesc(_uint Index);

	void SetLightMatrix(_fmatrix LightMatrix, LIGHT_MATRIX eLightMatrix);

	_float4x4 GetLightFloat4x4(LIGHT_MATRIX eLightMatrix);
	_float4x4 GetLightInverseFloat4x4(LIGHT_MATRIX eLightMatrix);

	void SetLightPosition(_fvector vLightPos);
	_float4 GetLightPosition() const;
	void SetLightDirection(_fvector vLightDir);
	_float4 GetLightDirection() const;
	
	void ShadowUpdate();

public: //TargetManager
	HRESULT Set_ShaderRenderTargetResourceView(class CShader* pShader, const _tchar* pTargetTag, const char* pContantName);

public: //CollisionManager
	void SetCollisionDebugRender(_bool value);
	HRESULT AddCollider(class CCollider* collider, _uint iLayerIndex = 0);
	void CollisionRender();

public: // Sound_Manager
	HRESULT PlaySoundEx(TCHAR* pSoundKey, int eChannel, SOUND_VOLUME eVolum = CUSTOM_VOLUME, _float fVolume = 0.1f);
	HRESULT PlaySoundEx(TCHAR* pSoundKey, int eChannel, _float fVolume = 0.1f);
	HRESULT SetSoundVolume(int eChannel, SOUND_VOLUME eVolum = CUSTOM_VOLUME, _float fVolume = 0.1f);
	HRESULT StopSound(int eChannel);
	void StopAllSound();

public: // Frustum
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
	_bool InWorldSpace(_fvector vPoint, _float fRadius = 0.f);
	_bool InLocalSpace(_fvector vPoint, _float fRadius = 0.f);

public:
	static void Engine_Release();
	virtual void Free() override;

private:
	class CCollisionManager*		m_pCollision_Manager = { nullptr };
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };
	class CFont_Manager*			m_pFontManager = { nullptr };
	class CLightManager*			m_pLightManager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CTargetManager*			m_pTargetManager = { nullptr };
};

END