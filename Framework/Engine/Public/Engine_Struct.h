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
		enum class TYPE { DIRECTIONAL, POINT, SHADOW, _END };

		TYPE		eLightType = { TYPE::_END };

		_vector		vLightPos;
		_vector		vLightAt;
		float		fLightRange;
		_vector		vLightDir;

		_vector		vDiffuse, vAmbient, vSpecular;

	}LIGHT_DESC;

	typedef struct tagKeyFrame
	{
		_float	fTime;
		_float3 vScale;
		_vector vRotation;
		_vector vTranslation;

	}KEYFRAME;

	typedef struct tagMeshMaterial
	{
		string				strMaterialName = "";
		class CTexture*		pTextures[AI_TEXTURE_TYPE_MAX] = { nullptr };

	}MESH_MATERIAL;

	/* 버퍼 구조체 */

	typedef struct ENGINE_DLL tagVertex_Pos
	{
		_float3		vPosition;

		static const _uint					  iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC tElements[iNumElements];

	}VTXPOS;

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

	typedef struct ENGINE_DLL tagVertex_Animation_Mesh
	{
		_float3		vPosition;
		_float3		vNormal;
		_float2		vTexcoord;
		_float3		vTangent;

		XMUINT4		vBlendIndices;
		XMFLOAT4	vBlendWeights;

		static const _uint					  iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC tElements[iNumElements];

	}VTXANIMMESH;


	enum class LERP_MODE { DEFAULT, EASE_OUT, EASE_IN, EXPONENTIAL, SMOOTHSTEP, SMOOTHERSTEP, TYPEEND };
	typedef struct ENGINE_DLL MyLerpFloatInfo
	{
		// Time
		_float		fStartTime = 0.f;
		_float		fEndTime = 0.f;
		_float		fCurTime = 0.f;

		// Value
		_float		fStartValue = 0.f;
		_float		fTargetValue = 0.f;
		_float		fCurValue = 0.f;

		_bool		bActive = false;

		LERP_MODE	eMode = LERP_MODE::DEFAULT; // https://chicounity3d.wordpress.com/2014/05/23/how-to-lerp-like-a-pro/

		void Init_Lerp(const LERP_MODE& _eMode = LERP_MODE::DEFAULT)
		{
			bActive = true;
			fCurTime = 0.f;
			fCurValue = 0.f;
			eMode = _eMode;
		}

		void Set_Lerp(const _float& _fTime, const _float _fStartValue, const _float& _fTargetValue)
		{
			
			fEndTime = _fTime;
			fStartValue = fCurValue = _fStartValue;
			fTargetValue = _fTargetValue;
		}

		// Mode
		void Update_Lerp(const _float& fTimeDelta)
		{
			if (!bActive) return;

			fCurTime += fTimeDelta;

			if (fCurTime >= fEndTime)
			{
				bActive = FALSE;
				fCurTime = fEndTime;

				fCurValue = fTargetValue;

				return;
			}

			_float t = fCurTime / fEndTime;

			switch (eMode)
			{
			case Engine::LERP_MODE::DEFAULT:
			{
			}
			break;
			case Engine::LERP_MODE::EASE_OUT:
			{
				t = sinf(t * XM_PI * 0.5f);
			}
			break;
			case Engine::LERP_MODE::EASE_IN:
			{
				t = 1.f - cosf(t * XM_PI * 0.5f);
			}
			break;
			case Engine::LERP_MODE::EXPONENTIAL:
			{
				t = t * t;
			}
			break;
			case Engine::LERP_MODE::SMOOTHSTEP:
			{
				t = t * t * (3.f - 2.f * t);

			}
			break;
			case Engine::LERP_MODE::SMOOTHERSTEP:
			{
				t = t * t * t * (t * (6.f * t - 15.f) + 10.f);
			}
			break;
			}

			fCurValue = Lerp_Float(fStartValue, fTargetValue, t);

		}

		_float Lerp_Float(const _float& _f1, const _float& _f2, const _float _fTime) { return (1 - _fTime) * _f1 + (_fTime * _f2); }

	}LERP_FLOAT_INFO;

	typedef struct ENGINE_DLL MyLerpVec3Info
	{
		_float		fStartTime = 0.f;
		_float		fEndTime = 0.f;
		_float		fCurTime = 0.f;

		_vector		vStartVec{};
		_vector		vEndVec{};
		_vector		vCurVec{};

		_bool		bActive = false;

		LERP_MODE	eMode = LERP_MODE::DEFAULT;

		void Init_Lerp(const LERP_MODE& _eMode = LERP_MODE::DEFAULT)
		{
			bActive = true;
			fCurTime = 0.f;
			vCurVec = _vector::Zero;
			eMode = _eMode;
		}

		void Set_Lerp(const _float& _fTime, const _vector _fStartValue, const _vector& _fTargetValue)
		{
			
			fEndTime = _fTime;
			vStartVec = vCurVec = _fStartValue;
			vEndVec = _fTargetValue;
		}

		void Update_Lerp(const _float& fTimeDelta)
		{
			if (!bActive) return;

			fCurTime += fTimeDelta;

			if (fCurTime >= fEndTime)
			{
				bActive = FALSE;
				fCurTime = fEndTime;
			}

			_float t = fCurTime / fEndTime;

			switch (eMode)
			{
			case Engine::LERP_MODE::DEFAULT:
			{
			}
			break;
			case Engine::LERP_MODE::EASE_OUT:
			{
				t = sinf(t * XM_PI * 0.5f);
			}
			break;
			case Engine::LERP_MODE::EASE_IN:
			{
				t = 1.f - cosf(t * XM_PI * 0.5f);
			}
			break;
			case Engine::LERP_MODE::EXPONENTIAL:
			{
				t = t * t;
			}
			break;
			case Engine::LERP_MODE::SMOOTHSTEP:
			{
				t = t * t * (3.f - 2.f * t);
			}
			break;
			case Engine::LERP_MODE::SMOOTHERSTEP:
			{
				t = t * t * t * (t * (6.f * t - 15.f) + 10.f);
			}
			break;
			}

			vCurVec = XMVectorLerp(vStartVec, vEndVec, t);
		}
	}LERP_VEC3_INFO;
}



#endif // Engine_Struct_h__
