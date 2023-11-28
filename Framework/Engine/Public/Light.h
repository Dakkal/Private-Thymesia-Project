#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const { return &m_LightDesc; }

public:
	HRESULT	Initialize(const LIGHT_DESC& LightDesc, class CTransform* pPlayerTransform);
	HRESULT Calculate_ShadowLight();
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	LIGHT_DESC				m_LightDesc;
	_vector					m_vLightPos_Origin;
	_vector					m_vLightAt_Origin;
	class CTransform*		m_pPlayerTransform = { nullptr };

public:
	static	CLight* Create(const LIGHT_DESC& LightDesc, class CTransform* pPlayerTransform = nullptr);
	virtual void Free() override;
	
};

END