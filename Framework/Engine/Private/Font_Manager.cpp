#include "..\Public\Font_Manager.h"
#include "Font.h"

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr != Find_Font(strFontTag))
		return E_FAIL;

	CFont* pFont = CFont::Create(pDevice, pContext, strFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render(const wstring& strFontTag, const wstring& strText, const _float2& vPos, FXMVECTOR color, _float rotation, XMFLOAT2 const& origin, _float scale)
{
	CFont* pFont = Find_Font(strFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPos, color, rotation, origin, scale);
}

CFont* CFont_Manager::Find_Font(const wstring& strFontTag)
{
	auto	iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

void CFont_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();
}
