#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END


BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	enum class TEXTURE { DIFFUSE, MASK, BRUSH, _END };

protected:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual	~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };
	CTexture*			m_pTextureCom[(_uint)TEXTURE::_END] = { nullptr };
	

private:
	HRESULT	Ready_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END