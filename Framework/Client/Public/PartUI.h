#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBinBone;
class CBinModel;
class CShader;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPartUI final : public CPartObject
{
protected:
	CPartUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartUI(const CPartUI& rhs);

	virtual ~CPartUI() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources_Key();
	HRESULT Bind_ShaderResources_Back();
	HRESULT Bind_ShaderResources_Line();

private:
	CTexture* m_pTexture_Back = { nullptr };
	CTexture* m_pTexture_Key = { nullptr };
	CTexture* m_pTexture_Line = { nullptr };

	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CTransform* m_pTransform_Back = { nullptr };
	CTransform* m_pTransform_Key = { nullptr };
	CTransform* m_pTransform_Line = { nullptr };

private:
	_float		m_fTime = { 0.f };

	_float		m_fX = { 0.f }, m_fY = { 0.f }, m_fSizeX = { 0.f }, m_fSizeY = { 0.f };
	_matrix		m_ViewMatrix, m_ProjMatrix;


public:
	static CPartUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual void Free() override;
};

END