#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END


BEGIN(Client)

class CEdit_Terrain final : public CGameObject
{
protected:
	CEdit_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Terrain(const CGameObject& rhs);
	virtual	~CEdit_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	HRESULT	Set_WireFrameMode(_bool bWireFrame);

	_vector	Picking_Terrain();

private:
	CRenderer*					m_pRendererCom = { nullptr };
	CTransform*					m_pTransformCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CVIBuffer_Terrain*			m_pVIBufferCom = { nullptr };
	
private:
	_uint						m_iNumVerticesX = { 0 };
	_uint						m_iNumVerticesZ = { 0 };
	_bool						m_bIsWireFrame = { true };

private:
	HRESULT	Ready_Components();
	HRESULT	Bind_ShaderResources();

public:
	static CEdit_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END