#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _matrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT	Bind_MaterialTexture(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT	Render(_uint iMeshIndex);
	HRESULT Set_Model_WireFrame(_uint iMeshIndex, _bool eWireFrame);

public:
	_uint	Get_NumMeshes() const { return m_iNumMeshes; }

private:
	Assimp::Importer		m_Importer;
	const aiScene*			m_pAIScene = { nullptr };

private:
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private:
	_uint					m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>	m_Materials;

	_matrix					m_PivotMatrix;

private:
	HRESULT Ready_Meshes();
	HRESULT	Ready_Materials(const char* pModelFilePath);

public:
	static	CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pModelFilePath, _matrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END