#include "..\Public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iLightIndex)
{
	if (iLightIndex >= m_Lights.size())
		return nullptr;

	auto iter = m_Lights.begin();

	for (size_t i = 0; i < iLightIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc();
}

const LIGHT_DESC* CLight_Manager::Get_ShadowLightDesc(_uint iLightIndex)
{
	if (iLightIndex >= m_ShadowLights.size())
		return nullptr;

	auto iter = m_ShadowLights.begin();

	for (size_t i = 0; i < iLightIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc, class CTransform* pPlayerTransform)
{
	CLight* pLight = CLight::Create(LightDesc, pPlayerTransform);

	if (nullptr == pLight)
		return E_FAIL;

	if (nullptr == pPlayerTransform)
		m_Lights.push_back(pLight);
	else
		m_ShadowLights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Caculate_ShadowLight()
{
	for (auto& pLight : m_ShadowLights)
	{
		pLight->Calculate_ShadowLight();
	}

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

void CLight_Manager::Free()
{
	__super::Free();

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);
	m_Lights.clear();

	for (auto& pLight : m_ShadowLights)
		Safe_Release(pLight);
	m_ShadowLights.clear();
}
