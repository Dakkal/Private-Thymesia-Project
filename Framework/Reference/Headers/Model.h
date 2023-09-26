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
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT	Render();
	HRESULT Set_Model_WireFrame(_bool eWireFrame);


private:
	Assimp::Importer		m_Importer;
	const aiScene*			m_pAIScene = { nullptr };

private:
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private:
	HRESULT Ready_Meshes();

public:
	static	CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END