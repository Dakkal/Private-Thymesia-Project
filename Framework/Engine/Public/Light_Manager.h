#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)

private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iLightIndex);
	const LIGHT_DESC* Get_ShadowLightDesc(_uint iLightIndex);

public:
	HRESULT	Add_Light(const LIGHT_DESC& LightDesc, class CTransform* pPlayerTransform = nullptr);
	HRESULT	Caculate_ShadowLight();
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	list<class CLight*>		m_Lights;
	list<class CLight*>		m_ShadowLights;

public:
	virtual	void Free() override;
};

END