#pragma once
#include "Base.h"

BEGIN(Engine)

class CCalculator final: public CBase
{
	DECLARE_SINGLETON(CCalculator)

private:
	CCalculator();
	virtual ~CCalculator() = default;

public:
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);

public:
	const _vector& Picking_Mouse(CVIBuffer* pBuffer, POINT pt);

public:
	virtual void Free() override;
};

END