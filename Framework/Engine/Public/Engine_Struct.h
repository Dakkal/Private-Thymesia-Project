#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		HWND		hWnd;
		WINMODE		eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;

	}GRAPHIC_DESC;

	typedef struct ENGINE_DLL tagAxis
	{
		static	_vector	X ;
		static	_vector	Y ;
		static	_vector	Z ;

	}AXIS;

	typedef struct tagLightDesc
	{
		enum class TYPE { DIRECTIONAL, POINT, _END };

		TYPE		eLightType = { TYPE::_END };

		_vector		vLightPos;
		float		fLightRange;
		_vector		vLightDir;

		_vector		vDiffuse, vAmbient, vSpecular;

	}LIGHT_DESC;

	typedef struct tagMeshMaterial
	{
		class CTexture*		pTextures[AI_TEXTURE_TYPE_MAX];

	}MESH_MATERIAL;

	typedef struct ENGINE_DLL tagVertex_Pos_Tex
	{
		_float3		vPosition;
		_float2		vTexcoord;

		static const _uint					  iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC tElements[iNumElements];

	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Pos_Normal_Tex
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;

		static const _uint					  iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC tElements[iNumElements];

	}VTXPOSNORTEX;

	typedef struct ENGINE_DLL tagVertex_Mesh
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;
		_float3		vTangent;

		static const _uint					  iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC tElements[iNumElements];

	}VTXMESH;

}


#endif // Engine_Struct_h__
