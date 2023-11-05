#pragma once
#include "Component.h"
#include "FbxExporter.h"
#include "BinAnimation.h"

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
	HRESULT First_Set_Animation(_bool isLoop, _uint iAnimationIndex, _float fAnimSpeed);
	HRESULT Set_Animation(_bool isLoop, _uint iAnimationIndex, _float fAnimSpeed, _uint iStatNumKeyFrames = 0);
	HRESULT Change_Animation(_float fDuration, _float fTimeDelta);
	HRESULT	Play_Animation(_float fTimeDelta);

	HRESULT Bind_BoneMatrices(class CShader* pShader, _uint iMeshIndex, const char* pConstantName);
	HRESULT	Bind_MaterialTexture(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT	Render(_uint iMeshIndex);
	
	HRESULT	Set_OwnerPosToRootPos(class CTransform* pTransform, _float fTimeDelta, class CNavigation* pNavi = nullptr);

public:
	const vector<class CBinMesh*>&			Get_Meshes()		{ return m_Meshes; }
	const vector<MESH_MATERIAL>&			Get_Materials()		{ return m_Materials; }
	const vector<class CBinBone*>&			Get_Bones()			{ return m_Bones; }
	const vector<class CBinAnimation*>&		Get_Animations()	{ return m_Animations; }
	const class CBinAnimation*				Get_CurAnimation()	{ return m_Animations[m_iCurAnimIndex]; }
	const _int&								Get_CurAnimIndex()  { return m_iCurAnimIndex; }
	_bool									Is_CurAnimKeyFrame(_uint iIndex);
	const _bool&							Is_CurAnimFinished(){ return m_Animations[m_iCurAnimIndex]->IsFinished(); }
	

	_uint									Get_NumMeshes() const { return m_iNumMeshes; }
	_int									Get_BoneIndex(const string& strBoneName) const;
	
	class CBinBone*							Get_BonePtr(const string& pBoneName) const;
	_matrix									Get_PivotMatrix() const { return m_PivotMatrix; }
	_int									Get_NumAnim() const { return m_iNumAnims; }

private:
	TYPE									m_eModelType = { TYPE_END };
	
private:
	_uint									m_iNumMeshes = { 0 };
	vector<class CBinMesh*>					m_Meshes;

private:
	_uint									m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>					m_Materials;

	_matrix									m_PivotMatrix;

private:
	vector<class CBinBone*>					m_Bones;

	_vector									m_PrevRootPos = {0.f, 0.f, 0.f, 1.f};
	_vector									m_CurRootPos = { 0.f, 0.f, 0.f, 1.f };

private:
	vector<class CBinChannel*>				CurChannels;
	vector<class CBinChannel*>				NextChannels;
	_float									m_fChangeTrackPosition = { 0.f };
	_bool									m_bIsNextAnimLoop = { false };
	_uint									m_iNextStartNumKeyFrames = { 0 };
	_bool									m_bIsAnimChange = { false };
	_int									m_iCurAnimIndex = { -1 };
	_int									m_iNextAnimIndex = { -1 };
	_uint									m_iNumAnims = { 0 };
	vector<class CBinAnimation*>			m_Animations;
	_bool									m_bIsFirstAnim = { false };

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
