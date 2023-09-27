#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CMesh final :	public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _matrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }


private:
	_uint	m_iMaterialIndex = { 0 };

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _matrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END