#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Transform.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc, class CTransform* pPlayerTransform)
{
	memmove(&m_LightDesc, &LightDesc, sizeof LightDesc);

	m_pPlayerTransform = pPlayerTransform;

	m_vLightPos_Origin = m_LightDesc.vLightPos;
	m_vLightAt_Origin = m_LightDesc.vLightAt;

	return S_OK;
}

HRESULT CLight::Calculate_ShadowLight()
{
	if (nullptr == m_pPlayerTransform)
		return E_FAIL;

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POS);

	m_LightDesc.vLightPos = m_vLightPos_Origin + vPlayerPos;
	m_LightDesc.vLightAt = m_vLightAt_Origin + vPlayerPos;


	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint		iPassIndex = 0;

	if (LIGHT_DESC::TYPE::DIRECTIONAL == m_LightDesc.eLightType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vLightDir, sizeof(_vector))))
			return E_FAIL;
		iPassIndex = 1;
	}
	else if (LIGHT_DESC::TYPE::POINT == m_LightDesc.eLightType || LIGHT_DESC::TYPE::SHADOW == m_LightDesc.eLightType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vLightPos, sizeof(_vector))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fLightRange, sizeof(_float))))
			return E_FAIL;
		iPassIndex = 2;
	}


	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_vector))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(pShader->Begin(iPassIndex)))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc, CTransform* pPlayerTransform)
{
	CLight* pInstance = new CLight;

	if (FAILED(pInstance->Initialize(LightDesc, pPlayerTransform)))
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
