#pragma once

#include "PartObject.h"

BEGIN(Client)

class CWeapon_Player_Saber final : public CPartObject
{
protected:
	CWeapon_Player_Saber(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Player_Saber(const CWeapon_Player_Saber& rhs); 
	virtual ~CWeapon_Player_Saber() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CWeapon_Player_Saber* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override; 
	virtual void Free() override;
};

END