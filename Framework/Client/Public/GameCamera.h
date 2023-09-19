#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CGameCamera final : public CCamera
{
public:
	typedef struct tagCamera_Game_Desc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensitive = { 0.0f };
	}CAMERA_GAME_DESC;
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
	_float			m_fMouseSensitive = { 0.0f };

public:
	static CGameCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameCamera* Clone(void* pArg) override;
	virtual void Free() override;
};

END