#include "..\Public\Channel.h"
#include "Bone.h"
#include "Model.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CModel* pModel, const aiNodeAnim* pAIChannel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3 vScale;
	_vector vRotation;
	_vector vTranslation;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame;
		ZeroMemory(&KeyFrame, sizeof KeyFrame);

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTime = pAIChannel->mRotationKeys[i].mTime;
		}

		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			vTranslation.w = 1.f;
			KeyFrame.fTime = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}
	return S_OK;
}

void CChannel::Update_TransformationMatrix(_uint* pCurKeyFrame, vector<class CBone*>& Bones, _float fTrackPosition)
{
	if (0.f == fTrackPosition)
		*pCurKeyFrame = 0;

	_float3		vScale;
	_vector		vRotation;
	_vector		vTranslation;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back();

	if (fTrackPosition >= LastKeyFrame.fTime)
	{
		*pCurKeyFrame = m_KeyFrames.size() - 1;
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vTranslation = LastKeyFrame.vTranslation;
	}
	else
	{
		while (fTrackPosition >= m_KeyFrames[*pCurKeyFrame + 1].fTime)
			++* pCurKeyFrame;

		_float	fRatio = (fTrackPosition - m_KeyFrames[*pCurKeyFrame].fTime) / (m_KeyFrames[*pCurKeyFrame + 1].fTime - m_KeyFrames[*pCurKeyFrame].fTime);

		_vector vSourScale = _vector(m_KeyFrames[*pCurKeyFrame].vScale);
		_vector vDestScale = _vector(m_KeyFrames[*pCurKeyFrame + 1].vScale);
		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);

		_vector vSourRotation = _vector(m_KeyFrames[*pCurKeyFrame].vRotation);
		_vector vDestRotation = _vector(m_KeyFrames[*pCurKeyFrame + 1].vRotation);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);

		_vector vSourTranslation = m_KeyFrames[*pCurKeyFrame].vTranslation;
		_vector vDestTranslation = m_KeyFrames[*pCurKeyFrame + 1].vTranslation;
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, _vector(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_Transform(TransformationMatrix);
}

CChannel* CChannel::Create(const CModel* pModel, const aiNodeAnim* pAIChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pModel, pAIChannel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}
