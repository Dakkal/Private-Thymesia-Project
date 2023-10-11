#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _matrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Set_Animation(_bool isLoop, _uint iAnimationIndex);
	HRESULT	Play_Animation(_float fTimeDelta);

	HRESULT Bind_BoneMatrices(class CShader* pShader, _uint iMeshIndex, const char* pConstantName);
	HRESULT	Bind_MaterialTexture(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT	Render(_uint iMeshIndex);
	HRESULT Set_Model_WireFrame(_uint iMeshIndex, _bool eWireFrame);

public:
	_uint	Get_NumMeshes() const { return m_iNumMeshes; }
	_int	Get_BoneIndex(const char* pBoneName) const;

private:
	Assimp::Importer		m_Importer;
	const aiScene*			m_pAIScene = { nullptr };
	TYPE					m_eModelType = { TYPE_END };

private:
	_uint					m_iNumMeshes = { 0 };
	vector<class CMesh*>	m_Meshes;

private:
	_uint					m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>	m_Materials;

	_matrix					m_PivotMatrix;

private:
	vector<class CBone*>	m_Bones;

private:
	_uint						m_iCurAnimIndex = { 0 };
	_uint						m_iNumAnims = { 0 };
	vector<class CAnimation*>	m_Animations;

private:
	HRESULT Ready_Meshes();
	HRESULT	Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT	Ready_Animations();

public:
	static	CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const char* pModelFilePath, _matrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END