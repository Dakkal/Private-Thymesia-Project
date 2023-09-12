#pragma once
#include "Base.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float Compute_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);

public:
	map<const wstring, class CTimer*>		m_mapTimers;

private:
	class CTimer* Find_Timer(const wstring& strTimerTag) const;

public:
	virtual void Free();

};

END