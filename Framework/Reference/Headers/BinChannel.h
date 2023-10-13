#pragma once
#include "Base.h"
#include "FbxExporter.h"

BEGIN(Engine)

class CBinChannel final : public CBase
{
private:
	CBinChannel();
	virtual ~CBinChannel() = default;

public:
	HRESULT	Initialize(const class CBinModel* pModel, const SAVE_CHANNEL tChannel);
	void	Update_TransformationMatrix(_uint* pCurKeyFrame, vector<class CBinBone*>& Bones, _float fTrackPosition);

private:
	string	m_strName = "";
	_uint	m_iNumKeyFrames = { 0 };
	_uint	m_iBoneIndex = { 0 };

	vector<KEYFRAME>	m_KeyFrames;

public:
	static	CBinChannel* Create(const class CBinModel* pModel, const SAVE_CHANNEL tChannel);
	virtual void Free() override;
};

END