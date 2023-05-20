#pragma once

namespace Engine
{
	typedef	signed char		_byte;
	typedef	unsigned char	_ubyte;

	typedef	wchar_t			_tchar;

	typedef	signed short	_short;
	typedef	unsigned short	_ushort;

	typedef	signed int		_int;
	typedef	unsigned int	_uint;

	typedef	signed long		_long;
	typedef	unsigned long	_ulong;

	typedef	float			_float;
	typedef	double			_double;

	typedef	bool			_bool;

	typedef	XMFLOAT2		_float2;
	typedef	XMFLOAT3		_float3;
	typedef	XMFLOAT4		_float4;
	
	typedef	XMVECTOR		_vector;
	typedef	FXMVECTOR		_fvector;	// 1~3번째
	typedef GXMVECTOR		_gvector;	// 4번째
	typedef	CXMVECTOR		_cvector;

	typedef	XMFLOAT4X4		_float4x4;
	typedef	XMMATRIX		_matrix;
	typedef	FXMMATRIX		_fmatrix;

#pragma region MyTextureType
	//aiTextureType 이름만 바꿈
	enum MytextureType
	{
		MyTextureType_NONE = 0,
		MyTextureType_DIFFUSE = 1,
		MyTextureType_SPECULAR = 2,
		MyTextureType_AMBIENT = 3,
		MyTextureType_EMISSIVE = 4,
		MyTextureType_HEIGHT = 5,
		MyTextureType_NORMALS = 6,
		MyTextureType_SHININESS = 7,
		MyTextureType_OPACITY = 8,
		MyTextureType_DISPLACEMENT = 9,
		MyTextureType_LIGHTMAP = 10,
		MyTextureType_REFLECTION = 11,
		MyTextureType_BASE_COLOR = 12,
		MyTextureType_NORMAL_CAMERA = 13,
		MyTextureType_EMISSION_COLOR = 14,
		MyTextureType_METALNESS = 15,
		MyTextureType_DIFFUSE_ROUGHNESS = 16,
		MyTextureType_AMBIENT_OCCLUSION = 17,
		MyTextureType_UNKNOWN = 18,

#ifndef SWIG
		_MyTextureType_Force32Bit = INT_MAX
#endif
	};

#define MY_TEXTURE_TYPE_MAX  MyTextureType_UNKNOWN
#pragma endregion

#define NO_ANIM	UINT_MAX

}
