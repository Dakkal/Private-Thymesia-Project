#include "..\Public\Font.h"

CFont::CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont::Initialize(const wstring& strFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, strFontFilePath.c_str());
	m_pBatch = new SpriteBatch(m_pContext);

	return S_OK;
}

HRESULT CFont::Render(const wstring& strText, const _float2& vPos, FXMVECTOR color, _float rotation, XMFLOAT2 const& origin, _float scale)
{
	m_pBatch->Begin();

	m_pFont->DrawString(m_pBatch, strText.c_str(), vPos, color, rotation, origin, scale);

	m_pBatch->End();

	return S_OK;
}

CFont* CFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath)
{
	CFont* pInstance = new CFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX("Failed to Created : CFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pFont);
}
