#pragma once
#include "Base.h"

BEGIN(Engine)

class CFont final : public CBase
{
private:
	CFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont() = default;

public:
	HRESULT Initialize(const wstring& strFontFilePath);
	HRESULT Render(const wstring& strText,
		const _float2& vPos, FXMVECTOR color,
		_float rotation, XMFLOAT2 const& origin, _float scale);


private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	SpriteBatch*			m_pBatch = { nullptr };
	SpriteFont*				m_pFont = { nullptr };

public:
	static CFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath);
	virtual void Free() override;
};

END