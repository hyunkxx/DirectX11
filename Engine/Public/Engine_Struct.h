#pragma once

namespace Engine
{
	typedef struct tagOrthoDesc
	{
		float fX;
		float fY;
		float fWidth;
		float fHeight;
		XMFLOAT4X4 WorldMatrix;
	}ORTHO_DESC;

	typedef struct tagKeyFrame
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;
		double		Time;
	}KEYFRAME;

	typedef struct tagModelMaterial
	{
		class CTexture* pMaterialTexture[MY_TEXTURE_TYPE_MAX] = { nullptr };
	}MODEL_MATERIAL;

	typedef struct tagLight_Desc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };
		TYPE eLightType = TYPE_END;
		XMFLOAT4 vDirection;
		XMFLOAT4 vPosition;
		float fRange;

		XMFLOAT4 vDiffuse;
		XMFLOAT4 vAmbient;
		XMFLOAT4 vSpecular;

	}LIGHT_DESC;

	typedef struct tagEffectDesc
	{
#pragma region Effect
		EFFECT_TYPE eEffectType;

		_float	fLifeTime;
		_float	fEffectTime;

		_bool	bSprite;
		_bool	bSetColor;

		_bool	bDelay;
		_float	fDelayTime;

		_float  fStartAlpha;
		_float  fCurAlpha;
		_float  fEndAlpha;

		_float3 vStartColor;
		_float3 vTextrueColor;
		_float3 vEndColor;

		_float2	vUV;
		_float2	fUVSpeed;
		_float2	vFrame;
		_float	fFrameSpeed;

		_bool  	bRhombus;
		_bool	bRect;
		_bool 	bTriangle;

		_bool	bGlow;
		_int	iRenderGroup;
		_int	iPass;

		_bool	bLoop;
		_bool	bTracking;
#pragma endregion

#pragma region Particle
		_bool	bSpark;
		_bool	bCircle;

		_bool	bParticleDelay;
		_float	fParticleDelayTime;
		_int	iParticleDelay;

		_float3 vDir;
		_float2 vDir_Angle;

		_float fMinSpeed;
		_float fMaxSpeed;

		_float3 vMinAngle;
		_float3 vMaxAngle;

		_float fMinDist;
		_float fMaxDist;

		_float3 vMinScale;
		_float3 vMaxScale;

		_float3 vMinPosition;
		_float3 vMaxPosition;

		_int iNumInstance;

		_float fParticlePower;
		_float fGravityPower;

		_bool bGravity;
		_bool bParticleLoop;

		Figure	eFigure;
#pragma endregion

#pragma region Mesh

		_float3 vStartPosition;
		_float3 vCurPosition;
		_float3 vEndPosition;

		_float3 vStartScale;
		_float3 vCurScale;
		_float3 vEndScale;

		_float3 vStartAngle;
		_float3 vCurAngle;
		_float3 vEndAngle;

		_float	fStartDelay;

		char MeshName[MAX_PATH];

#pragma endregion
		_bool bTexture;
		_bool bNoiseTexure;
		_bool bDissolveTexure;

		char TexName[MAX_PATH];
		char NoiseTexName[MAX_PATH];
		char DissolveTexName[MAX_PATH];
	}EFFECT_DESC;

	typedef struct tagGraphic_Desc
	{
		enum WIN_MODE { MODE_FULL, MODE_WINDOW, MODE_END };
		WIN_MODE eMode;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		HINSTANCE hInstance;
		HWND hWnd;
	}GRAPHIC_DESC;

	typedef struct tagFaceIndices16
	{
		unsigned short _0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long _0, _1, _2;
	}FACEINDICES32;


	typedef struct tagVertex_Position_Color
	{
		XMFLOAT3		vPosition;
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Color_Declaration
	{
		const static unsigned int						iNumElements = 1;
		static D3D11_INPUT_ELEMENT_DESC					Elements[1];
	}VTXPOS_DECLARATION;

	// 내가 어떤 모델을 그리기위한 정점 구성요소
	typedef struct tagVertex_Position_TexCoord
	{
		XMFLOAT3 vPosition;
		XMFLOAT2 vTexUV;
	}VTXTEX;

	// 정점 요소에대한 구체적인 정보
	typedef struct ENGINE_DLL tagVertex_Position_TexCoord_Declaration
	{
		const static unsigned int						ElementCount = 2;
		static D3D11_INPUT_ELEMENT_DESC					Elements[2];
	}VTXTEX_DECLARATION;

	typedef struct tagVertex_Position_Normal_TexCoord
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexUV;
		XMFLOAT3 vTangent;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_TexCoord_Declaration
	{
		const static unsigned int						ElementCount = 4;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXNORTEX_DECLARATION;

	// Cube
	typedef struct tagVertex_Position_Normal_TexCoord_CUBE
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT3 vTexUV;
	}VTXCUBE;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Cube_Declaration
	{
		const static unsigned int						ElementCount = 2;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXCUBE_DECLARATION;

	// Model
	typedef struct tagVertex_Model_Static
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexUV;
		XMFLOAT3	vTangent;
	}VTXSMODEL;

	typedef struct ENGINE_DLL tagVertex_Model_Static_Declaration
	{
		const static unsigned int			iNumElements = 4;
		static D3D11_INPUT_ELEMENT_DESC		ElementDesc[iNumElements];
	}VTXSMODEL_DECLARATION;

	typedef struct tagVertex_Model_Dynamic
	{
		XMFLOAT3	vPosition;
		XMFLOAT3	vNormal;
		XMFLOAT2	vTexUV;
		XMFLOAT3	vTangent;

		XMUINT4		vBlendIndices;
		XMFLOAT4	vBlendWeights;
	}VTXDMODEL;

	typedef struct ENGINE_DLL tagVertex_Anim_Dynamic_Declaration
	{
		const static unsigned int			iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC		ElementDesc[iNumElements];
	}VTXDMODEL_DECLARATION;

	typedef struct tagMaterial
	{
		wchar_t s_szNameDotExt[MY_TEXTURE_TYPE_MAX][MAX_PATH];
	}MTRLINFO;

	typedef struct tagMeshInfo_Static
	{
		wchar_t s_szName[MAX_PATH] = TEXT("");
		unsigned int s_iMaterialIndex = 0;
		unsigned int s_iNumVertices = 0;
		unsigned int s_iNumFaces = 0;
		VTXSMODEL*	s_pVertices = nullptr;
		FACEINDICES32* s_pFaces = nullptr;
	}SMESHINFO;

	typedef struct tagModelInfo_Static
	{
		unsigned int s_iNumMeshes = 0;
		unsigned int s_iNumMaterials = 0;

		SMESHINFO* s_pMeshes = nullptr;
		MTRLINFO* s_pMtrls = nullptr;
	}SMODELINFO;

	typedef struct tagMeshInfo_Dynamic
	{
		wchar_t s_szName[MAX_PATH] = TEXT("");
		unsigned int s_iMaterialIndex = 0;
		unsigned int s_iNumVertices = 0;
		unsigned int s_iNumFaces = 0;
		unsigned int s_iNumBones = 0;
		VTXDMODEL*	s_pVertices = nullptr;
		FACEINDICES32* s_pFaces = nullptr;
		wchar_t** s_pszBoneNameArray = nullptr;
	}DMESHINFO;

	typedef struct tagBoneInfo
	{
		wchar_t	s_szName[MAX_PATH] = TEXT("");
		wchar_t	s_szParentName[MAX_PATH] = TEXT("");
		XMFLOAT4X4 s_TransformationMatrix;
		XMFLOAT4X4 s_OffsetMatrix;
	}BONEINFO;


	typedef struct tagChannelInfo
	{
		wchar_t	s_szName[MAX_PATH] = TEXT("");
		unsigned int s_iNumKeyFrames = 0;
		unsigned int s_iNumScalingKeys = 0;
		unsigned int s_iNumRotationKeys = 0;
		unsigned int s_iNumPositionKeys = 0;
		KEYFRAME* s_pKeyFrames = nullptr;
	}CHANNELINFO;

	typedef struct tagAnimInfo
	{
		wchar_t s_szName[MAX_PATH] = TEXT("");
		double	s_Duration = 0.0;
		double	s_TicksPerSecond = 0.0;
		unsigned int s_iNumChannels = 0;
		CHANNELINFO* s_pChannels = nullptr;
	}ANIMINFO;

	typedef struct tagModelInfo_Dynamic
	{
		unsigned int s_iNumBones = 0;
		unsigned int s_iNumMeshes = 0;
		unsigned int s_iNumMaterials = 0;
		unsigned int s_iNumAnimations = 0;

		BONEINFO* s_pRootBone = nullptr;
		DMESHINFO* s_pMeshes = nullptr;
		MTRLINFO* s_pMtrls = nullptr;
		ANIMINFO* s_pAnimations = nullptr;
	}DMODELINFO;

	typedef struct tagVertex_Matrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVertex_Instance_Declaration
	{
		const static unsigned int						ElementCount = 6;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXINSTANCE_DECLARATION;

	typedef struct tagVertex_Point
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertex_Point_Instance_Declaration
	{
		const static unsigned int						ElementCount = 6;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXPOINTINSTANCE_DECLARATION;

#pragma region MODEL_INSTANCE
	typedef struct ENGINE_DLL tagVertex_Model_Static_Instance_Declaration
	{
		const static unsigned int						ElementCount = 8;
		static D3D11_INPUT_ELEMENT_DESC					Elements[ElementCount];
	}VTXSMODELINSTANCE_DECLARATION;
#pragma endregion MODEL_INSTANCE

#pragma region STATIC_OBJECT
	typedef struct tagStaticObject_Desc
	{
		XMFLOAT3		vP = {};
		XMFLOAT3		vS = {};
		XMFLOAT3		vA = {};
	}SOBJECT_DESC;

	typedef struct tagStaticMapObject_Desc
	{
		SOBJECT_DESC	SObjectDesc = {};

		_uint			iModelTypeID = {};
		_uint			iDiffuseTexID = {};
	}SMAP_OBJECT_DESC;

	typedef struct tagStaticMapObjectEdition_Desc
	{
		// 밖에서 전달 받을 정보
		const _tchar*		pEditionFilePath = { L"" };
		_float				fCullingRatio = { 0.0f };

		// 인스턴싱 모델 구분용
		_uint				iSIMD_ID = { 0 };

		// 오브젝트 구분 용
		_uint				iTypeID = { 0 };

		// Tree
		_bool				UseEditionColor = { false };
		_uint				iEditionColor_MeshNum = { 0 };
		_float3				vEditionColor = { 0.0f, 0.0f, 0.0f };

		// Rock
		_uint				iDiffuseTex_ID = { 0 };

		_uint				iMaskTex_ID = { 0 };
		// 디퓨즈 없는객체 서브컬러
		_float3				vSubEditionColor = { 0.0f, 0.0f, 0.0f };

		// Glow 사용 유무
		_bool				UseGlow = { false };

		_bool				UseShake = { false };

		_float				fShakePower = { 0.0f };
		_float				fShakeRange_Ratio = { 0.0f };

		_bool				IsDetail_Shake = { false };
		_uint				iShake_MeshNum = { 0 };
	}SMAP_OBJECT_EDITION_DESC;
#pragma endregion STATIC_OBJECT

#pragma region NAVIGATION
	typedef struct tagNaviPoint_Desc
	{
		XMFLOAT3						vP[3] = {};
		int								iCell_State = {};
	}NAVIPOINT_DESC;
#pragma endregion NAVIGATION

#pragma region CITY_OBJECT
	typedef struct tagStaticCityObject_Desc
	{
		SOBJECT_DESC		PSA = {};

		// 모델 컴포넌트 구분
		_uint				iSMD_ID = { 0 };

		// 오브젝트 종류 구분
		_uint				iType_ID = { 0 };

		// 옵션 구분
		_uint				iOption_ID = { 0 };

		// 몇번째 오브젝트인지 구분
		_uint				iSelf_ID = { 0 };

		_bool				Use_Glow = { false };

	}SCITY_OBJECT_DESC;
#pragma endregion CITY_OBJECT

#pragma region TRIGGER
	typedef struct tagTriggerDesc {
		// 포탈같은 매쉬 추가할때 그놈 포지션
		_float3			vP = { 0.0f, 0.0f, 0.0f };
		_float3			vS = { 0.0f, 0.0f, 0.0f };
		_float3			vA = { 0.0f, 0.0f, 0.0f };

		// 트리거 적용 범위 ( 플레이어와 거리비교 거리 )
		_float			fRange = { 0.0f };
		_uint			iTriggerType = { 0 };

		_uint			iTriggerID = { 0 };

		const _tchar*	pFilePath = { L"" };

		const _tchar*	pEditionFilePath = { L"" };

		const _tchar*	pMonsterLayerTag = { L"" };

	}TRIGGER_DESC;

	typedef struct tagSpawnPoint {
		_float3			vP = { 0.0f, 0.0f, 0.0f };
		_uint			iCellIndex = { 0 };
	}SPAWN_POINT;

#pragma endregion TRIGGER
}