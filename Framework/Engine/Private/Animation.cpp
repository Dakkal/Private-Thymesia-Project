#include "..\Public\Animation.h"
#include "Bone.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTrackPosition(rhs.m_fTrackPosition)
	, m_isFinished(rhs.m_isFinished)
	, m_isLoop(rhs.m_isLoop)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_iCurKeyFrames(rhs.m_iCurKeyFrames)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const CModel* pModel, const aiAnimation* pAIAnimation)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;

	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_iCurKeyFrames.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(vector<class CBone*>& Bones, _float fTimeDelta)
{
	if (true == m_isFinished)
		return;

	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fTrackPosition >= m_fDuration)
	{
		if (true == m_isLoop)
			m_fTrackPosition = 0.f;
		else
			m_isFinished = true;
	}

	_uint	iNumChannel = 0;

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(&m_iCurKeyFrames[iNumChannel++], Bones, m_fTrackPosition);
	}
}

void CAnimation::Reset()
{
	m_fTrackPosition = 0.f;
	m_isFinished = false;
	m_isLoop = false;

	for (auto& iCurKeyFrame : m_iCurKeyFrames)
	{
		iCurKeyFrame = 0;
	}
}

CAnimation* CAnimation::Create(const CModel* pModel, const aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
