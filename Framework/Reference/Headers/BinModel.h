#pragma once
#include "Component.h"
#include "FbxExporter.h"

BEGIN(Engine)

class ENGINE_DLL CBinModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CBinModel(ID3D11Device* pDeivce, ID3D11DeviceContext* pContext);
	CBinModel(class CGameObject* pOwner, const CBinModel& rhs);
	virtual ~CBinModel() = default;

public:

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const wstring& pModelFilePath, _matrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Set_Animation(_bool isLoop, _uint iAnimationIndex);
	HRESULT	Play_Animation(_float fTimeDelta);

	HRESULT Bind_BoneMatrices(class CShader* pShader, _uint iMeshIndex, const char* pConstantName);
	HRESULT	Bind_MaterialTexture(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT	Render(_uint iMeshIndex);
	HRESULT Set_Model_WireFrame(_uint iMeshIndex, _bool eWireFrame);

public:
	_uint						Get_NumMeshes() const { return m_iNumMeshes; }
	_int						Get_BoneIndex(const string& strBoneName) const;
	vector<class CBinMesh*>&	Get_Meshes() { return m_Meshes; }
	
	class CBinBone* Get_BonePtr(const string& pBoneName) const;
	_matrix Get_PivotMatrix() const { return m_PivotMatrix; }

private:
	_bool							m_bIsRender = { true };

private:
	TYPE							m_eModelType = { TYPE_END };
	
private:
	_uint							m_iNumMeshes = { 0 };
	vector<class CBinMesh*>			m_Meshes;

private:
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

	_matrix							m_PivotMatrix;

private:
	vector<class CBinBone*>			m_Bones;

private:
	_uint							m_iCurAnimIndex = { 0 };
	_uint							m_iNumAnims = { 0 };
	vector<class CBinAnimation*>	m_Animations;

private:
	HRESULT Ready_Meshes(const SAVE_MESH tMehsDesc);
	HRESULT	Ready_Materials(const SAVE_MATERIAL strTexture, const wstring& pModelFilePath);
	HRESULT Ready_Bones(const SAVE_BONE tBoneDesc);
	HRESULT	Ready_Animations(const SAVE_ANIM tAnimDesc);

public:
	static	CBinModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eModelType, const wstring& pModelFilePath, _matrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;
};

END
