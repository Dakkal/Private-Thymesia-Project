#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT	Initialize(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	void	Update_TransformationMatrix(_uint* pCurKeyFrame, vector<class CBone*>& Bones, _float fTrackPosition);

private:
	_char	m_szName[MAX_PATH] = "";
	_uint	m_iNumKeyFrames = { 0 };
	_uint	m_iBoneIndex = { 0 };

	vector<KEYFRAME>	m_KeyFrames;

public:
	static	CChannel* Create(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

END