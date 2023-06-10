
#include "Camera.h"
#include "ActionCam.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CShader;
class CTransform;
class CRenderer;

END

BEGIN(Client)

class CUICam final : public CCamera
{
private:
	explicit CUICam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUICam(const CUICam& rhs);
	virtual ~CUICam() = default;

public:
	virtual	HRESULT Initialize_Prototype(CCameraMovement::CAM_TYPE eCamType);
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetMove(_bool bValue) { m_bMove = bValue; }
	_bool IsMoveFinish() const { return m_bMoveFinish; };

public:
	_float GetCameraSpeed();

public:
	static CUICam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCameraMovement::CAM_TYPE);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CCameraMovement::CAM_TYPE m_eCamType;
	CCameraMovement* m_pCamMovement = nullptr;

private:
	_bool m_bMove = false;
	_bool m_bMoveFinish = false;
};

END