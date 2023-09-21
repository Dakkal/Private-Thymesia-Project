#pragma once
#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CToolCamera final : public CCamera
{
public:
	typedef struct tagCamera_Tool_Desc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensitive = { 0.0f };
	}CAMERA_TOOL_DESC;
private:
	CToolCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolCamera(const CToolCamera& rhs);
	virtual	~CToolCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	void	MouseMove(_float fTimeDelta);

private:
	_float			m_fMouseSensitive = { 0.0f };

public:
	static CToolCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CToolCamera* Clone(void* pArg) override;
	virtual void Free() override;
};

END