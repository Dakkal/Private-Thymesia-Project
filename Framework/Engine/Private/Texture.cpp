#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(CGameObject* pOwner, const CTexture& rhs)
	: CComponent(pOwner, rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_vecSRVs(rhs.m_vecSRVs)
{
	for (auto& pSRV : m_vecSRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	for (size_t i = 0; i < iNumTextures; i++)
	{
		_tchar		szTextureFilePath[MAX_PATH] = TEXT("");
		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		_tchar		szExt[MAX_PATH] = TEXT("");
		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT hr = 0;
		ID3D11ShaderResourceView* pSRV = nullptr;

		if (false == lstrcmp(szExt, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}
		else if (false == lstrcmp(szExt, TEXT(".tga")))
		{
			return E_FAIL;
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		m_vecSRVs.push_back(pSRV);
	}


	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(const CShader* pShader, const char* pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_Texture(pConstantName, m_vecSRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(const CShader* pShader, const char* pConstantName)
{
	return  pShader->Bind_Textures(pConstantName, m_vecSRVs.data(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(CGameObject* pOwner, void* pArg)
{
	CTexture* pInstance = new CTexture(pOwner, *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_vecSRVs)
		Safe_Release(pSRV);

	if (false == m_IsCloned)
		m_vecSRVs.clear();
}
