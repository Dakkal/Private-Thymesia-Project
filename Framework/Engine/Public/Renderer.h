#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_SHADOW, RG_NONLIGHT, RG_NONBLEND, RG_BLEND, RG_UI, RG_TOOL, RG_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderer(class CGameObject* pOwner, const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRG, class CGameObject* pGameobject);
	HRESULT Draw_RenderObject();

#ifdef _DEBUG
public:
	HRESULT Add_Debug(class CComponent* pDebug) {
		m_RenderDebug.push_back(pDebug);
		Safe_AddRef(pDebug);
		return S_OK;
	}
#endif

private:
	list<class CGameObject*>	m_listRenderObject[static_cast<_uint>(RENDERGROUP::RG_END)];

private:
	HRESULT Create_Shadow_ViewPort(POINT Shadow);
	HRESULT Create_Shadow_DSV(POINT Shadow);

	D3D11_VIEWPORT	m_ClientViewPort;
	D3D11_VIEWPORT	m_ShadowViewPort;
	ID3D11DepthStencilView* m_pShadowDSV = { nullptr };

private:
	HRESULT	Render_Priority();
	HRESULT	Render_NonLight();
	HRESULT Render_LightDepth();
	HRESULT	Render_NonBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT	Render_Blend();
	HRESULT	Render_UI();
	HRESULT	Render_Tool();
#ifdef _DEBUG
	HRESULT Render_Debug();

private:
	list<class CComponent*>				m_RenderDebug;
#endif

private:

	class CTargetManager*	m_pTarget_Manager = { nullptr };
	class CLight_Manager*	m_pLight_Manager = { nullptr };

private:
	class CVIBuffer_Rect*	m_pVIBuffer = { nullptr };
	class CShader*			m_pShader = { nullptr };

	_matrix					m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;

};

END