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

	string			Get_BoneName() const { return m_strName; }
	_matrix			Get_CombinedTransform() const { return m_ComBinedTransformationMatrix; }

private:
	string		m_strName;
	_matrix		m_TransformationMatrix;
	_matrix		m_ComBinedTransformationMatrix;
	_int		m_iParentBoneIndex = -1;

public:
	static CBinBone* Create(const SAVE_BONE_INFO tBoneInfo);
	CBinBone* Clone();
	virtual void Free() override;
};

END