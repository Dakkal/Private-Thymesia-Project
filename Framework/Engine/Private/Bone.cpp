#include "..\Public\Bone.h"

CBone::CBone()
{
}

CBone::CBone(const CBone& rhs)
	: m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_ComBinedTransformationMatrix(rhs.m_ComBinedTransformationMatrix)
	, m_iParentBoneIndex(rhs.m_iParentBoneIndex)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
	m_iParentBoneIndex = iParentBoneIndex;

	strcpy_s(m_szName, pAINode->mName.data);
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_matrix));
	m_TransformationMatrix = XMMatrixTranspose(m_TransformationMatrix);

	return S_OK;
}

HRESULT CBone::Update_CombinedTransformationMatrix(const vector<class CBone*> Bones)
{
	if (-1 == m_iParentBoneIndex)
		m_ComBinedTransformationMatrix = m_TransformationMatrix;
	else
		m_ComBinedTransformationMatrix = m_TransformationMatrix * Bones[m_iParentBoneIndex]->m_ComBinedTransformationMatrix;


	return S_OK;
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
	CBone* pInstance = new CBone;

	if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
