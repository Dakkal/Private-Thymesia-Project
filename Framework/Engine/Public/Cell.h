#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	void Set_Neighbor(LINE eLine, CCell* pCell) { m_iNeighborIndicies[eLine] = pCell->m_iIndex; }
public:
	const _float3* Get_Point(POINTS ePoint) const { return &m_vPoints_Origin[ePoint]; }

public:
	HRESULT Initialize(const _float3* pPoints, _uint iIndex);
	void	Update(_matrix WorldMatrix);
	_bool	Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool	IsOut(_vector vPoint, _matrix WorldMatrix, _int& pNeighborIndex);

#ifndef NDEBUG
public:
	HRESULT Render();

private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif // !NDEBUG

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	_uint				m_iIndex = {};

	_float3				m_vPoints_Origin[POINT_END];
	
	_float3				m_vPoints_World[POINT_END];
	_float3				m_vNormals[LINE_END];

	_int				m_iNeighborIndicies[LINE_END] = { -1, -1, -1 };

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex);
	virtual void Free() override;
};

END