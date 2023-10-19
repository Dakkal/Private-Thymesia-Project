#include "../Public/BinAnimation.h"
#include "BinBone.h"
#include "BinChannel.h"

CBinAnimation::CBinAnimation()
{
}

CBinAnimation::CBinAnimation(const CBinAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTrackPosition(rhs.m_fTrackPosition)
	, m_isFinished(rhs.m_isFinished)
	, m_isLoop(rhs.m_isLoop)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_iCurKeyFrames(rhs.m_iCurKeyFrames)
	, m_strName(rhs.m_strName)
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CBinAnimation::Initialize(const CBinModel* pModel, const SAVE_ANIM_INFO tAnimDesc)
{
	m_strName = tAnimDesc.strAnimName;

	m_fDuration = tAnimDesc.fDuration;

	m_fTickPerSecond = tAnimDesc.fTickPerSecond;

	m_iNumChannels = tAnimDesc.iNumChannel;

	m_iCurKeyFrames.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CBinChannel* pChannel = CBinChannel::Create(pModel, tAnimDesc.vecChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
}

void CBinAnimation::Update_TransformationMatrix(vector<class CBinBone*>& Bones, _float fTimeDelta)
{
	if (true == m_isStop)
		return;
	else
	{
		if (true == m_isFinished)
			m_isFinished = false;
	}

	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fTrackPosition >= m_fDuration)
	{
		if (true == m_isLoop)
		{
			m_fTrackPosition = 0.f;
			m_isFinished = true;
		}
		else
		{
			m_isStop = true;
			m_isFinished = true;
		}
			
	}

	_uint	iNumChannel = 0;

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(&m_iCurKeyFrames[iNumChannel++], Bones, m_fTrackPosition);
	}
}

void CBinAnimation::Reset()
{
	m_fTrackPosition = 0.f;
	m_isFinished = false;
	m_isLoop = false;
	m_isStop = false;

	for (auto& iCurKeyFrame : m_iCurKeyFrames)
	{
		iCurKeyFrame = 0;
	}
}

CBinAnimation* CBinAnimation::Create(const CBinModel* pModel, const SAVE_ANIM_INFO tAnimDesc)
{
	CBinAnimation* pInstance = new CBinAnimation();

	if (FAILED(pInstance->Initialize(pModel, tAnimDesc)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBinAnimation* CBinAnimation::Clone()
{
	return new CBinAnimation(*this);;
}

void CBinAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
