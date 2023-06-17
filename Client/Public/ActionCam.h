#pragma once
#include "Camera.h"
#include "CameraMovement.h"

BEGIN(Engine)
class CTransform;
class CBone;

END

BEGIN(Client)

struct IAttachTargetTransform abstract {
	virtual void AttachTargetTransform(CTransform* pTransform) = 0;
};

class CActionCam final 
	: public CCamera
	, public IAttachTargetTransform
{
private:
	CActionCam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActionCam(const CActionCam& rhs);
	virtual ~CActionCam() = default;

public:
	virtual HRESULT	Initialize_Prototype(CCameraMovement::CAM_TYPE eActionType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Start() override;
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	//virtual HRESULT Render();
	virtual void RenderGUI() override;
	virtual void AttachTargetTransform(CTransform * pTransform) override;

public:
	void ActionInit();
	void SetupBone(CBone* pBone) { m_pTargetBone = pBone; };
	
private:
	void RevertPrevCam(_double TimeDelta);
	void actionInit_Bangsun();
	void actionInit_Yangyang();
	void actionInit_Chixia();

	void actionLoop_Bangsung(_double TimeDelta);
	void actionLoop_Yangyang(_double TimeDelta);
	void actionLoop_Chixia(_double TimeDelta);

public:
	static CActionCam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCameraMovement::CAM_TYPE eActionType);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
 
private:
	CBone* m_pTargetBone = nullptr;
	CTransform* m_pTargetTransform = nullptr;
	CTransform* m_pSubTransform = nullptr;

private:
	CCameraMovement::CAM_TYPE m_eCamType;
	CCameraMovement* m_pCamMovement = nullptr;
	class CPlayerState* m_pPlayerState = nullptr;

private:
	// ¿¢¼ÇÄ· »óÅÂ
	_uint m_iAction = 0;
	_float m_fTimeAcc = 0.f;

	// Revert
	_float m_fRevertAcc = 0.f;
	_float m_fChixiaValue = 2.f;
};

END