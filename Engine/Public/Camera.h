#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
private:
	enum SHAKE_TYPE { WAVE, VIBRATION };

public:
	typedef struct tagCameraDesc
	{
		CTransform::TRANSFORM_DESC TransformDesc;

		_float3 vEye;
		_float3 vAt;
		_float3 vAxisY;

		_float fFovy;
		_float fAspect;
		_float fNear;
		_float fFar;

	}CAMERA_DESC;

	typedef struct tagShakeDesc
	{
		_float fPower;
		_float fSpeed;
		_float fDuration;
	}SHAKE_DESC;

protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Use(_bool bUse) { m_bUse = bUse; }

	void StartWave();
	void StartWave(SHAKE_DESC tagShakeDesc);
	void StartVibration();
	void StartVibration(_float fRange, _float fDuration = 0.5f);

private:
	void shakeWave(_double TimeDelta);
	void shakeVibration(_double TimeDelta);
	void resetShakeTimer();

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	class CPipeLine*	m_pPipeLine = { nullptr };
	CAMERA_DESC			m_CameraDesc;
	_bool				m_bUse = { false };

private:
	SHAKE_TYPE m_eShakeType;
	SHAKE_DESC m_SakeDesc;

	_bool m_bShakeActive = false;
	_float m_fCurShakeAcc = 0.f;
	_float m_fCurTimeAcc = 0.f;

	_float m_fVibeRange = 1.f;
	_float m_fVibeDuration = 1.f;
	_float m_fVibeTimeAcc = 0.f;

};

END