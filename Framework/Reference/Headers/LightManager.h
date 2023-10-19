#pragma once
#include "Base.h"

BEGIN(Engine)

class CLightManager final : public CBase
{
	DECLARE_SINGLETON(CLightManager)

private:
	CLightManager();
	virtual ~CLightManager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iLightIndex);

public:
	HRESULT	Add_Light(const LIGHT_DESC& LightDesc);


private:
	list<class CLight*>		m_Lights;

public:
	virtual	void Free() override;
};

END