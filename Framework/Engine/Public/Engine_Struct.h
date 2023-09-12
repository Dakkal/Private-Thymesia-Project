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


	typedef struct ENGINE_DLL tagVertex_Pos_Tex
	{
		_float3		vPosition;
		_float2		vTexcoord;

		static const _uint					  iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC tElements[iNumElements];

	}VTXPOSTEX;

}


#endif // Engine_Struct_h__
