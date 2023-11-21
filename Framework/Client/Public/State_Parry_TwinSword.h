#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Parry_TwinSword final : public CState
{
private:
	CState_Parry_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Parry_TwinSword() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	_bool					m_bParryEnd = { false };

	_bool					m_bParryLeft1 = { false };
	_bool					m_bParryLeft2 = { false };
	_bool					m_bParryRight1 = { false };
	_bool					m_bParryRight2 = { false };

public:
	static CState_Parry_TwinSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


