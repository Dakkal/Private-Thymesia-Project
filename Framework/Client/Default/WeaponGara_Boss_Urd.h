#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CWeaponGara_Boss_Urd final : public CPartObject
{
protected:
	CWeaponGara_Boss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeaponGara_Boss_Urd(const CWeaponGara_Boss_Urd& rhs);
	virtual ~CWeaponGara_Boss_Urd() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	vector<CGameObject*>	m_vecWeapons;

	class CTexture* m_pTextureCom = { nullptr };

public:
	static CWeaponGara_Boss_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END