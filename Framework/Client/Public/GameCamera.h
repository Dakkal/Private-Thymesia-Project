#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CGameCamera final : public CCamera
{
private:
	CGameCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameCamera(const CGameCamera& rhs);
	virtual	~CGameCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

private:
	CTransform* m_pTransformCom = { nullptr };

private:
	HRESULT	Ready_Components();

public:
	static CGameCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameCamera* Clone(void* pArg) override;
	virtual void Free() override;
};

END