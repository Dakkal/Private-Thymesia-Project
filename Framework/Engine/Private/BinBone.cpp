#include "../Public/BinBone.h"
#include "BinMesh.h"

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
	{
		m_ComBinedTransformationMatrix = m_TransformationMatrix * Bones[m_iParentBoneIndex]->m_ComBinedTransformationMatrix;

		if ("root" == m_strName)
		{
			m_RootCombinedMatrix = m_ComBinedTransformationMatrix;
			memcpy(&m_RootPos, &m_RootCombinedMatrix.m[3], sizeof(_vector));

			_vector Zero = _vector(0.f, 0.f, 0.f, 1.f);
			memcpy(&m_ComBinedTransformationMatrix.m[3], &Zero, sizeof(_vector));
		}
	}
		
	return S_OK;
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
