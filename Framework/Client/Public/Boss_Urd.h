#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CBoss_Urd final : public CGameObject
{
public:
	enum class PARTS { BODY, WEAPON, _END };

protected:
	CBoss_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Urd(const CGameObject& rhs);

	virtual ~CBoss_Urd() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strProtoTag);
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	vector<class CGameObject*>		m_Parts;

private:
	CTransform* m_pTransformCom = { nullptr };

	_int	iIndex = 0;

private:
	HRESULT Ready_Components();
	HRESULT Ready_PlayerParts();

public:
	static CBoss_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strProtoTag = TEXT(""));
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
