#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	CBone(const CBone& rhs);
	virtual ~CBone() = default;

public:
	HRESULT			Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	HRESULT			Update_CombinedTransformationMatrix(const vector<class CBone*> Bones);

public:
	void			Set_Transform(const _matrix TransformMatrix)  { m_TransformationMatrix = TransformMatrix; }

	const char*		Get_BoneName() const { return m_szName; }
	_matrix			Get_CombinedTransform() const { return m_ComBinedTransformationMatrix; }

private:
	char		m_szName[MAX_PATH] = "";
	_matrix		m_TransformationMatrix;
	_matrix		m_ComBinedTransformationMatrix;
	_int		m_iParentBoneIndex = -1;

public:
	static CBone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	CBone* Clone();
	virtual void Free() override;
};

END