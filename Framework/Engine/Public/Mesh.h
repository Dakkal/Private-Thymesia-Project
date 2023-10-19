#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final :	public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(class CGameObject* pOwner, const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const CModel* pModel, CModel::TYPE eModelType, const aiMesh* pAIMesh, _matrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT	Bind_BoneMatrices(class CShader* pShader, const vector<class CBone*>& Bones, const char* pConstantName, _matrix PivotMatrix);

public:
	_uint	Get_MaterialIndex() const { return m_iMaterialIndex; }

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _matrix PivotMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const CModel* pModel, const aiMesh* pAIMesh);

private:
	_char			m_szName[MAX_PATH] = "";
	_uint			m_iMaterialIndex = { 0 };
	_uint			m_iNumBones = { 0 };
	vector<_uint>	m_Bones;
	vector<_matrix> m_OffsetMatrices;

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CModel* pModel, CModel::TYPE eModelType, const aiMesh* pAIMesh, _matrix PivotMatrix);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END