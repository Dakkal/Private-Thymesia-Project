#pragma once

#include "Engine_Defines.h"

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	_ulong AddRef();
	_ulong Release();

private:
	_ulong m_dwRefCnt = 0;

public:
	virtual void Free();

};

