#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CGameObject& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*			m_pRendererCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };

private:
	_float				m_fX = { 0.f }, m_fY = { 0.f }, m_fSizeX = { 0.f }, m_fSizeY = { 0.f };
	_matrix		m_ViewMatrix, m_ProjMatrix;



private:
	HRESULT	Ready_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END