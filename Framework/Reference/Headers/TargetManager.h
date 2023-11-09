#pragma once

#include "Base.h"

BEGIN(Engine)

class CTargetManager final : public CBase
{
	DECLARE_SINGLETON(CTargetManager)

private:
	CTargetManager();
	virtual ~CTargetManager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _vector& vColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Bind_SRV(class CShader* pShader, const wstring& strTargetTag, const _char* pConstantName);
	HRESULT Begin_MRT(ID3D11DeviceContext* pContext, const wstring& strMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pContext);



#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	map<const wstring, class CRenderTarget*>			m_RenderTargets;
	map<const wstring, list<class CRenderTarget*>>		m_MRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDSV = { nullptr };

private:
	class CRenderTarget*		Find_RenderTarget(const wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);


public:
	virtual void Free() override;
};

END

