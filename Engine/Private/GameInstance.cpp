#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "CollisionManager.h"
#include "FontManager.h"
#include "TargetManager.h"
#include "Effect_Manager.h"
#include "Save_Loader.h"
#include "Frustum.h"
#include "../Public/Fmod/Sound_Manager.h"
#include "RenderSetting.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device { CGraphic_Device::GetInstance() }
	, m_pLevel_Manager { CLevel_Manager::GetInstance() }
	, m_pObject_Manager { CObject_Manager::GetInstance() }
	, m_pComponent_Manager { CComponent_Manager ::GetInstance() }
	, m_pPipeLine { CPipeLine::GetInstance() }
	, m_pTimer_Manager { CTimer_Manager::GetInstance() }
	, m_pInput_Device { CInput_Device::GetInstance() }
	, m_pSound_Manager { CSound_Manager::GetInstance() }
	, m_pCollision_Manager{ CCollisionManager::GetInstance() }
	, m_pLightManager{ CLightManager::GetInstance() }
	, m_pFontManager{ CFont_Manager::GetInstance() }
	, m_pTargetManager{ CTargetManager::GetInstance() }
	, m_pEffect_Manager{ CEffect_Manager::GetInstance() }
	, m_pSaveLoader{ CSave_Loader::GetInstance() }
	, m_pFrustum{ CFrustum::GetInstance() }
	, m_pRenderSetting{ CRenderSetting::GetInstance() }
{
	Safe_AddRef(m_pEffect_Manager);
	Safe_AddRef(m_pSaveLoader);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pLightManager);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pFontManager);
	Safe_AddRef(m_pTargetManager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pRenderSetting);
}

HRESULT CGameInstance::Engine_Initialize(const GRAPHIC_DESC& GraphicDesc, _uint iLevelCount , _uint iCollisionLayerCount, ID3D11Device** ppDevice_out, ID3D11DeviceContext** ppContext_out)
{
	if (!m_pGraphic_Device || !m_pObject_Manager || !m_pComponent_Manager || !m_pPipeLine || !m_pInput_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice_out, ppContext_out)))
		return E_FAIL;

	//½¦µµ¿ì µª½º ¼Â¾÷
	m_pTargetManager->SetupStaticShadowDepthStencilView(m_pGraphic_Device->GetStaticShadowDepthStencilView());
	m_pTargetManager->SetupShadowDepthStencilView(m_pGraphic_Device->GetShadowDepthStencilView());
	m_pTargetManager->SetupSmallDepthStencilView(m_pGraphic_Device->GetSmallDepthStencilView());

	if (FAILED(m_pInput_Device->Initialize_Input(GraphicDesc.hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iLevelCount)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iLevelCount)))
		return E_FAIL;

	if (FAILED(m_pCollision_Manager->ReserveCollisionLayer(iCollisionLayerCount)))
		return E_FAIL;

	if (FAILED(m_pPipeLine->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::Engine_Tick(_double TimeDelta)
{
	if (!m_pLevel_Manager || !m_pObject_Manager || !m_pPipeLine)
		return E_FAIL;

	m_TimeDelta = TimeDelta;
	m_pInput_Device->Update();

	m_pLevel_Manager->Tick_Level(TimeDelta);
	m_pObject_Manager->Tick(TimeDelta);
	m_pEffect_Manager->Tick(TimeDelta);
	m_pPipeLine->Tick();
	
	m_pFrustum->Tick();
	m_pObject_Manager->LateTick(TimeDelta);
	m_pEffect_Manager->Late_Tick(TimeDelta);

	m_pCollision_Manager->PhysicsUpdate();
	
	return S_OK;
}

const D3D11_VIEWPORT* CGameInstance::GetViewport(CGraphic_Device::VIEWPORT_TYPE eViewportType) const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->GetViewport(eViewportType);
}

HRESULT CGameInstance::Clear_RenderTargetView(_float4 vColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_RenderTargetView(vColor);
}

HRESULT CGameInstance::Clear_DepthStencilView()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencilView();
}

HRESULT CGameInstance::Clear_ShadowDepthStencilView()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_ShadowDepthStencilView();
}

HRESULT CGameInstance::Clear_SmallDepthStencilView()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_SmallDepthStencilView();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pCurrentLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pCurrentLevel);
}

void CGameInstance::RenderLevelUI()
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pLevel_Manager->RenderLevelUI();
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _tchar * pObjectTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(iLevelIndex, pObjectTag);
}

HRESULT CGameInstance::Add_Prototype(_int iObjectEnum, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(iObjectEnum, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, _int iObjectEnum, const _tchar* pLayerTag, _tchar* strObjectTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, iObjectEnum, pLayerTag, strObjectTag, pArg);
}

void CGameInstance::RenderGUI()
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->RenderGUI();
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* strObjectTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	CLayer* pLayer = m_pObject_Manager->Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	CGameObject* pGameObject = nullptr;
	pGameObject = pLayer->Find_GameObject(strObjectTag);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

CGameObject * CGameInstance::Clone_GameObject(_int iObjectEnum, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(iObjectEnum, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, _int iComponent, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, iComponent, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, _int iComponent, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, iComponent, pArg);
}

_float4x4 CGameInstance::Get_Transform_float4x4(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_float4x4(eState);
}

_float4x4 CGameInstance::Get_Transform_float4x4_Inverse(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4(0.f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPosition();
}

HRESULT CGameInstance::Initialize_Input(HINSTANCE hInst, HWND hWnd)
{
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	return m_pInput_Device->Initialize_Input(hInst, hWnd);
}

void CGameInstance::Update(void)
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Update();
}

KEY_STATE CGameInstance::InputKey(_int _key)
{
	if (nullptr == m_pInput_Device)
		return KEY_STATE::NONE;

	return m_pInput_Device->Get_DIKeyState_Custom(_key);
}

KEY_STATE CGameInstance::InputMouse(MOUSE_KEYSTATE _key)
{
	if (nullptr == m_pInput_Device)
		return KEY_STATE::NONE;

	return m_pInput_Device->Get_DIMouseState_Custom(_key);
}

_long CGameInstance::InputMouseMove(MOUSE_MOVESTATE eMoveState)
{
	if (nullptr == m_pInput_Device)
		return _long();

	return m_pInput_Device->Get_DIMouseMove(eMoveState);
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontName)
{
	if (nullptr == m_pFontManager)
		return E_FAIL;

	return m_pFontManager->Add_Font(pDevice, pContext, pFontTag, pFontName);
}

void CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & vPosition, _fvector vColor, const _float2 & vScale, _float fRadian, const _float2 & vOrigin)
{
	if (nullptr == m_pFontManager)
		return;

	m_pFontManager->Render_Font(pFontTag, pText, vPosition, vColor, vScale, fRadian, vOrigin);
}

HRESULT CGameInstance::CreateTimer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->CreateTimer(pTimerTag);
}

_float CGameInstance::GetTimer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->GetTimer(pTimerTag);
}

void CGameInstance::SetTimer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->SetTimer(pTimerTag);
}

HRESULT CGameInstance::AddLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLightManager)
		return E_FAIL;

	return m_pLightManager->AddLight(pDevice, pContext, LightDesc);
}

const LIGHT_DESC* CGameInstance::GetLightDesc(_uint Index)
{
	if (nullptr == m_pLightManager)
		return nullptr;

	return m_pLightManager->GetLightDesc(Index);
}

void CGameInstance::SetLightMatrix(_fmatrix LightMatrix, LIGHT_MATRIX eLightMatrix)
{
	if (nullptr == m_pLightManager)
		return;

	m_pLightManager->SetLightMatrix(LightMatrix, eLightMatrix);
}

_float4x4 CGameInstance::GetLightFloat4x4(LIGHT_MATRIX eLightMatrix)
{
	if (nullptr == m_pLightManager)
		return _float4x4();

	return m_pLightManager->GetLightFloat4x4(eLightMatrix);
}

_float4x4 CGameInstance::GetLightInverseFloat4x4(LIGHT_MATRIX eLightMatrix)
{
	if (nullptr == m_pLightManager)
		return _float4x4();

	return m_pLightManager->GetLightInverseFloat4x4(eLightMatrix);
}

void CGameInstance::SetLightPosition(_fvector vLightPos)
{
	if (nullptr == m_pLightManager)
		return;

	return m_pLightManager->SetLightPosition(vLightPos);
}

_float4 CGameInstance::GetLightPosition() const
{
	if (nullptr == m_pLightManager)
		return _float4();

	return m_pLightManager->GetLightPosition();
}

void CGameInstance::SetLightDirection(_fvector vLightDir)
{
	if (nullptr == m_pLightManager)
		return;

	return m_pLightManager->SetLightDirection(vLightDir);
}

_float4 CGameInstance::GetLightDirection() const
{
	if (nullptr == m_pLightManager)
		return _float4();

	return m_pLightManager->GetLightDirection();
}

void CGameInstance::ShadowUpdate()
{
	if (nullptr == m_pLightManager)
		return;

	return m_pLightManager->ShadowUpdate();
}

HRESULT CGameInstance::Set_ShaderRenderTargetResourceView(CShader * pShader, const _tchar * pTargetTag, const char * pContantName)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;

	return m_pTargetManager->Set_ShaderResourceView(pShader, pTargetTag, pContantName);
}

void CGameInstance::SetCollisionDebugRender(_bool value)
{
	if (nullptr == m_pCollision_Manager)
		return;

	return m_pCollision_Manager->SetDebugRender(value);
}

HRESULT CGameInstance::AddCollider(CCollider * collider, _uint iLayerIndex)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	return m_pCollision_Manager->AddCollider(collider, iLayerIndex);
}

void CGameInstance::CollisionRender()
{
	if (nullptr == m_pCollision_Manager)
		return;

	return m_pCollision_Manager->Render();
}

HRESULT CGameInstance::PlaySoundEx(TCHAR * pSoundKey, int eChannel, SOUND_VOLUME eVolume, _float fVolume)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	return m_pSound_Manager->Play_Sound(pSoundKey, eChannel, eVolume, fVolume);
}

HRESULT CGameInstance::PlaySoundEx(TCHAR * pSoundKey, int eChannel, _float fVolume)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	return m_pSound_Manager->Play_Sound(pSoundKey, eChannel, fVolume);
}

HRESULT CGameInstance::SetSoundVolume(int eChannel, SOUND_VOLUME eVolum, _float fVolume)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	return m_pSound_Manager->Set_SoundVolume(eChannel, eVolum, fVolume);
}

HRESULT CGameInstance::StopSound(int eChannel)
{
	if (m_pSound_Manager == nullptr)
		return E_FAIL;

	return m_pSound_Manager->Stop_Sound(eChannel);
}

void CGameInstance::StopAllSound()
{
	if (m_pSound_Manager == nullptr)
		return;

	m_pSound_Manager->Stop_AllSound();
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	if (nullptr == m_pFrustum)
		return;

	return m_pFrustum->Transform_ToLocalSpace(WorldMatrixInv);
}

_bool CGameInstance::InWorldSpace(_fvector vPoint, _float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->InWorldSpace(vPoint, fRadius);
}

_bool CGameInstance::InLocalSpace(_fvector vPoint, _float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->InLocalSpace(vPoint, fRadius);
}

void CGameInstance::DebugTargetToggle()
{
	if (nullptr == m_pRenderSetting)
		return;

	m_pRenderSetting->DebugTargetToggle();
}

_bool CGameInstance::IsDebug() const
{
	if (nullptr == m_pRenderSetting)
		return false;

	return m_pRenderSetting->IsDebug();
}

void CGameInstance::SetShadowLevel(SHADOW_LEVEL eLevel)
{
	if (nullptr == m_pRenderSetting)
		return;

	m_pRenderSetting->SetShadowLevel(eLevel);
}

_bool CGameInstance::IsActiveShadow() const
{
	if (nullptr == m_pRenderSetting)
		return nullptr;

	return m_pRenderSetting->IsActiveShadow();
}

SHADOW_LEVEL CGameInstance::GetShadowLevel() const
{
	if (nullptr == m_pRenderSetting)
		return CRenderSetting::SHADOW_LEVEL::SHADOW_OFF;

	return m_pRenderSetting->GetShadowLevel();
}

void CGameInstance::SSAOToggle()
{
	if (nullptr == m_pRenderSetting)
		return;

	m_pRenderSetting->SSAOToggle();
}

void CGameInstance::OutlineToggle()
{
	if (nullptr == m_pRenderSetting)
		return;

	m_pRenderSetting->OutlineToggle();
}

_bool CGameInstance::IsActiveOutline() const
{
	if (nullptr == m_pRenderSetting)
		return false;

	return m_pRenderSetting->IsActiveOutline();
}

_bool CGameInstance::IsActiveSSAO() const
{
	if (nullptr == m_pRenderSetting)
		return false;

	return m_pRenderSetting->IsActiveSSAO();
}

void CGameInstance::SetLUT(CRenderer::LUT eLUT)
{
	if (nullptr == m_pRenderSetting)
		return;

	m_pRenderSetting->SetLUT(eLUT);
}

CRenderer::LUT CGameInstance::GetLUT()
{
	if (nullptr == m_pRenderSetting)
		return CRenderer::LUT_DEFAULT;

	return m_pRenderSetting->GetLUT();
}

void CGameInstance::StartBlackWhite(_double TotalTime)
{
	if (nullptr == m_pRenderSetting)
		return;

	return m_pRenderSetting->StartBlackWhite(TotalTime);
}

void CGameInstance::BlackWhiteTimeAcc(_double TimeDelta)
{
	if (nullptr == m_pRenderSetting)
		return;

	return m_pRenderSetting->BlackWhiteTimeAcc(TimeDelta);
}

_bool CGameInstance::IsActiveBlackWhite() const
{
	if (nullptr == m_pRenderSetting)
		return false;

	return m_pRenderSetting->IsActiveBlackWhite();
}

HRESULT CGameInstance::Load_Effect(HWND hWnd, wstring strFileName, list<EFFECT_DESC*>* pEffectDesc)
{
	if (!m_pSaveLoader)
		return E_FAIL;

	return m_pSaveLoader->Load_Effect(hWnd , strFileName , pEffectDesc);
}

HRESULT CGameInstance::Push_Effect(const _tchar * pEffectTag, CEffect * pEffect)
{
	if (!m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Push_Effect(pEffectTag, pEffect);
}

CEffect * CGameInstance::Get_Effect(const _tchar * EffectTag)
{
	if (!m_pEffect_Manager)
		return nullptr;

	return m_pEffect_Manager->Get_Effect(EffectTag);
}

void CGameInstance::SetSplitDesc(CRenderSetting::RGB_SPLIT_DESC tagDesc)
{
	if (nullptr == m_pRenderSetting)
		return;

	m_pRenderSetting->SetSplitDesc(tagDesc);
}

CRenderSetting::RGB_SPLIT_DESC & CGameInstance::GetSplitDesc()
{
	return m_pRenderSetting->GetSplitDesc();
}

void CGameInstance::StartRGBSplit(_double TotalTime)
{
	if (nullptr == m_pRenderSetting)
		return;

	return m_pRenderSetting->StartRGBSplit(TotalTime);
}

void CGameInstance::RGBSpiltTimeAcc(_double TimeDelta)
{
	if (nullptr == m_pRenderSetting)
		return;

	return m_pRenderSetting->RGBSpiltTimeAcc(TimeDelta);
}

_bool CGameInstance::IsActiveRGBSplit() const
{
	if (nullptr == m_pRenderSetting)
		return false;

	return m_pRenderSetting->IsActiveRGBSplit();
}

_float CGameInstance::GetRGBSplitRatio() const
{
	if (nullptr == m_pRenderSetting)
		return false;

	return m_pRenderSetting->GetRGBSplitRatio();
}

void CGameInstance::Engine_Release()
{
	CRenderSetting::DestroyInstance();
	CLightManager::DestroyInstance();
	CSound_Manager::DestroyInstance();
	CGameInstance::DestroyInstance();
	CInput_Device::DestroyInstance();
	CTimer_Manager::DestroyInstance();
	CObject_Manager::DestroyInstance();
	CComponent_Manager::DestroyInstance();
	CLevel_Manager::DestroyInstance();
	CPipeLine::DestroyInstance();
	CGraphic_Device::DestroyInstance();
	CFont_Manager::DestroyInstance();
	CEffect_Manager::DestroyInstance();
	CSave_Loader::DestroyInstance();
	CFrustum::DestroyInstance();
	CCollisionManager::DestroyInstance();
	CTargetManager::DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pRenderSetting);
	Safe_Release(m_pLightManager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pTargetManager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pSaveLoader);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pFontManager);
	Safe_Release(m_pFrustum);
}
