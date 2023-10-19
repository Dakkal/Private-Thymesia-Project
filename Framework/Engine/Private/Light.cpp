#include "..\Public\Light.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
	memmove(&m_LightDesc, &LightDesc, sizeof LightDesc);

	return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight;

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("CLight Create Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLight::Free()
{
	__super::Free();
}
