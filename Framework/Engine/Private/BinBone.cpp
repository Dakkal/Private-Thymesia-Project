#include "../Public/BinBone.h"

CBinBone::CBinBone()
{
}

CBinBone::CBinBone(const CBinBone& rhs)
	: m_TransformationMatrix(rhs.m_TransformationMatrix)
	, m_ComBinedTransformationMatrix(rhs.m_ComBinedTransformationMatrix)
	, m_iParentBoneIndex(rhs.m_iParentBoneIndex)
	, m_strName(rhs.m_strName)
{
}

HRESULT CBinBone::Initialize(const SAVE_BONE_INFO tBoneInfo)
{
	m_iParentBoneIndex = tBoneInfo.iParentBoneIndex;

	m_strName = tBoneInfo.strBoneName;
	memcpy(&m_TransformationMatrix, &tBoneInfo.TransformMatrix, sizeof(_matrix));

	return S_OK;
}

HRESULT CBinBone::Update_CombinedTransformationMatrix(const vector<class CBinBone*> Bones)
{
	if (-1 == m_iParentBoneIndex)
		m_ComBinedTransformationMatrix = m_TransformationMatrix;
	else
		m_ComBinedTransformationMatrix = m_TransformationMatrix * Bones[m_iParentBoneIndex]->m_ComBinedTransformationMatrix;

	return S_OK;
}

_matrix CBinBone::Get_RemovePos_CombinedTransform()
{
	_matrix RemovePosMatrix = m_ComBinedTransformationMatrix;
	_vector Zero = _vector(0.f, 0.f, 0.f, 1.f);

	memcpy(&RemovePosMatrix.m[3], &Zero, sizeof(_vector));

	return RemovePosMatrix;
}

_vector CBinBone::Get_BonePos()
{
	memcpy(&m_BonePos, &m_TransformationMatrix.m[3], sizeof(_vector));

	return m_BonePos;
}

CBinBone* CBinBone::Create(const SAVE_BONE_INFO tBoneInfo)
{
	CBinBone* pInstance = new CBinBone;

	if (FAILED(pInstance->Initialize(tBoneInfo)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBinBone* CBinBone::Clone()
{
	return new CBinBone(*this);
}

void CBinBone::Free()
{
	__super::Free();
}
