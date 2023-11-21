#pragma once
#include "Base.h"
#include "FbxExporter.h"

BEGIN(Engine)

class CBinBone final : public CBase
{
private:
	CBinBone();
	CBinBone(const CBinBone& rhs);
	virtual ~CBinBone() = default;

public:
	HRESULT			Initialize(const SAVE_BONE_INFO tBoneInfo);
	HRESULT			Update_CombinedTransformationMatrix(const vector<class CBinBone*> Bones);

public:
	void			Set_Transform(const _matrix TransformMatrix) { m_TransformationMatrix = TransformMatrix; }
	void			Set_RootPos(const _vector RootPos) { m_RootPos = RootPos;}

	string			Get_BoneName() const { return m_strName; }
	_matrix			Get_Transform() const { return m_TransformationMatrix; }
	_matrix			Get_CombinedTransform() const { return m_ComBinedTransformationMatrix; }
	_matrix			Get_RootCombinedTransform() const { return m_RootCombinedMatrix; }
	_vector			Get_RootPos() { return m_RootPos; }

private:
	string		m_strName;
	_matrix		m_TransformationMatrix;
	_matrix		m_ComBinedTransformationMatrix;
	_int		m_iParentBoneIndex = -1;

	_matrix		m_RootCombinedMatrix;
	_vector		m_RootPos = {0.f, 0.f, 0.f, 1.f};

public:
	static CBinBone* Create(const SAVE_BONE_INFO tBoneInfo);
	CBinBone* Clone();
	virtual void Free() override;
};

END