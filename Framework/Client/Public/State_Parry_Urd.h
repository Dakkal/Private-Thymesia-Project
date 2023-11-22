#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Parry_Urd final : public CState
{
private:
	CState_Parry_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Parry_Urd() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	_bool					m_bParryEnd = { false };

	_bool					m_bLeft = { false };
	_bool					m_bRight = { false };

public:
	static CState_Parry_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


