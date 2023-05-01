#pragma once

#include "VIBuffer_Triangle.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"

#include "Model.h"
#include "Model_Anim.h"
#include "Model_VTF.h"
#include "Model_Instance.h"
#include "Animation.h"
#include "Channel.h"

#include "Collider.h"
#include "SphereCollider.h"
#include "OBBCollider.h"

#include "Calculator.h"

BEGIN(Engine)
/* 컴포넌트들의 원형을 레벨별로 나누어 보관한다. */
class ENGINE_DLL CComponent_Manager : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	explicit CComponent_Manager() = default;
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iLevelMaxCount);
	HRESULT Add_Prototype(_uint iLevelIndex, _int iComponent, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, _int iComponent, void* pArg = nullptr);
	void Clear(_uint iLevelIndex);

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, _int iComponent);

public:
	virtual void Free() override;

private:
	_uint m_iLevelMaxCount = { 0 };
	unordered_map<_int , class CComponent*>* m_pPrototypes = { nullptr };
	typedef unordered_map<_int, class CComponent*> PROTOTYPES;

};

END