#pragma once
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

	_uint	Get_CurLevel() { return m_iCurLevelIndex; }

private:
	class CLevel* m_pCurLevel = { nullptr };
	_uint		  m_iCurLevelIndex = { 0 };

public:
	virtual void Free() override;

};

END

