#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CWeapon_Boss_Urd final : public CPartObject
{
protected:
	CWeapon_Boss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Boss_Urd(const CWeapon_Boss_Urd& rhs);
	virtual ~CWeapon_Boss_Urd() = default;

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

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Boss_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END