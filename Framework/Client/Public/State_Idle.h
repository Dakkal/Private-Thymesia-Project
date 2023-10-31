#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Idle final : public CState
{
private:
	CState_Idle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Idle() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();


	virtual STATE			Key_Input(const _float& fTimeDelta);

private:

public:
	static CState_Idle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


