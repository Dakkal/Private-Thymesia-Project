#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Hit_TwinSword final : public CState
{
private:
	CState_Hit_TwinSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Hit_TwinSword() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	_uint					m_iHitCnt = { 0 };
	_uint					m_iRecoverCnt = { 0 };

	_bool					m_bLeft_Hit = { false };
	_bool					m_bRight_Hit = { false };

	_bool					m_bParry = { false };

public:
	static CState_Hit_TwinSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


