#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum class RENDERGROUP { RG_PRIORITY, RG_NONLIGHT, RG_NONBLEND, RG_BLEND, RG_UI, RG_TOOL, RG_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRG, class CGameObject* pGameobject);
	HRESULT Draw_RenderObject();

private:
	list<class CGameObject*>	m_listRenderObject[static_cast<_uint>(RENDERGROUP::RG_END)];

private:
	HRESULT	Render_Priority();
	HRESULT	Render_NonLight();
	HRESULT	Render_NonBlend();
	HRESULT	Render_Blend();
	HRESULT	Render_UI();
	HRESULT	Render_Tool();


public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END