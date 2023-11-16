#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBinBone;
class CBinModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBody_GreatSword final : public CPartObject
{
protected:
	CBody_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_GreatSword(const CBody_GreatSword& rhs);

	virtual ~CBody_GreatSword() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void OnCollision_Enter(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Stay(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Exit(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Enter(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Stay(CGameObject* _pColObj, _float fTimeDelta) override;
	virtual void OnCollision_Part_Exit(CGameObject* _pColObj, _float fTimeDelta) override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	class CTexture* m_pTextureCom = { nullptr };

public:
	static CBody_GreatSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END