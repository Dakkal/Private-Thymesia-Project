#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Avoid_Shield final : public CState
{
private:
	CState_Avoid_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Avoid_Shield() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	_uint					m_iGuardTime = { 0 };
	_uint					m_iGuardCount = { 0 };

public:
	static CState_Avoid_Shield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


