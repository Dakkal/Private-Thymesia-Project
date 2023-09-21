#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct tagTerrain_Desc 
	{
		_ulong			iNumVerticesX;
		_ulong			iNumVerticesZ;
		_bool			bIsHeightMap;

	}TERRAIN_DESC;

private:
	CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring & strHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_ulong			m_iNumVerticesX = { 0 };
	_ulong			m_iNumVerticesZ = { 0 };
	_bool			m_bIsHeightMap = { true };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strHeightMapFilePath = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END