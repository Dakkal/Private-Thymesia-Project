#include "Engine_Defines.h"

namespace Engine
{
	_fvector	AXIS::X = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_fvector	AXIS::Y = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_fvector	AXIS::Z = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	const D3D11_INPUT_ELEMENT_DESC VTXPOSTEX::tElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const D3D11_INPUT_ELEMENT_DESC VTXPOSNORTEX::tElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

}

