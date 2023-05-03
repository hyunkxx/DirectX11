#pragma once

#include "Model.h"
#include "Bone.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Anim : public CModel
{
protected:
	CModel_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Anim(const CModel_Anim& rhs);
	virtual ~CModel_Anim() = default;

public:
	vector<class CBone*>&	Get_Bones() { return m_Bones; }
	class CBone* Get_BonePtr(const _tchar* pBoneName);

	class CBone* Get_BonePtr(const _uint iBoneIndex)
	{
		if (iBoneIndex >= m_Bones.size())
			return nullptr;

		return m_Bones[iBoneIndex];
	}
	_uint	Get_BoneIndex(const _tchar* pBoneName);


	class CAnimation* Get_Animation(const _uint iAnimIndex)
	{
		if (iAnimIndex >= m_Animations.size())
			return nullptr;

		return m_Animations[iAnimIndex];
	}

	const _uint	Get_NumBones()
	{
		return m_iNumBones;
	}

	const _bool Get_AnimInterpolating() const;

	void Set_RootBone(const _tchar* pBoneName)
	{
		m_pRootBone = Get_BonePtr(pBoneName);
	}

	void Set_ShotBone(const _tchar* pBoneName)
	{
		m_pShotBone = Get_BonePtr(pBoneName);
	}

	void Set_TopBone(const _tchar* pTopBone)
	{
		m_pTopBone = Get_BonePtr(pTopBone);
	}

	void Set_BottomBones(const _tchar* pRightBoneName, const _tchar* pLeftBoneName)
	{
		m_pRightBottom = Get_BonePtr(pRightBoneName);
		m_pLeftBottom = Get_BonePtr(pLeftBoneName);
	}

	CBone* Get_RootBone() { return m_pRootBone; }
	CBone* Get_ShotBone() { return m_pShotBone; }
	CBone* Get_TopBone() { return m_pTopBone; }
	CBone* Get_RightBottomBone() { return m_pRightBottom; }
	CBone* Get_LeftBottomBone() { return m_pLeftBottom; }

	void	Set_OffsetZero();

	void Set_TrackPos(_float fTrackPos);


	_uint	Get_AnimCount()
	{
		return (_uint)m_Animations.size();
	}

	vector<class CAnimation*>& Get_Animations();


public:
	virtual HRESULT Initialize_Prototype(const _tchar* pModelFilePath);
	virtual HRESULT Initialize(void* pArg = nullptr) override;

public:
	HRESULT	SetUp_BoneMatrices(class CShader* pShaderCom, const char* pConstantName, _uint iMeshIndex);

	HRESULT SetUp_Animation(_uint iAnimationIndex, _bool bInterpolate, _bool bFootAltitude); /* 어떤 애니메이션을 구동할거야. */
	_float3 Play_Animation(_double TimeDelta, _double* pFrameAccOut = nullptr, _bool* pFinishedOut = nullptr, _bool bContinue = false); /* 애니메이션 재생한다.(모든 뼈들의 Combined행렬을 셋팅한다) */
	void	Invalidate_CombinedMatrices();

	void	Update_TargetBones();
	void	Ribbon_TargetBones();

protected:
	_uint					m_iNumBones = { 0 };
	vector<class CBone*>	m_Bones;
	CBone*					m_pRootBone = { nullptr };
	CBone*					m_pShotBone = { nullptr };
	CBone*					m_pTopBone = { nullptr };
	CBone*					m_pRightBottom = { nullptr };
	CBone*					m_pLeftBottom = { nullptr };


protected:
	class CAnimController*		m_pAnimController = { nullptr };
	_uint						m_iNumAnimations = { 0 };
	vector<class CAnimation*>	m_Animations;

protected:
	HRESULT	Ready_Bones(DMODELINFO* pModel);
	HRESULT Ready_Meshes(DMODELINFO* pModel);
	HRESULT Ready_Materials(DMODELINFO* pModel, const _tchar* pModelFilePath);
	HRESULT Ready_Animations(DMODELINFO* pModel);

public:
	static CModel_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pModelFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free();
};

END