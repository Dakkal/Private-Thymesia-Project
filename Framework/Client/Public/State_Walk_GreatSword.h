#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Walk_GreatSword final : public CState
{
private:
	CState_Walk_GreatSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Walk_GreatSword() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	STATE					WalkState(_float fTimeDelta);

private:
	_float					m_fAttackTime = { 0.f };
	_bool					m_bAttack = { false };

	_bool					m_bGoRight = { false };
	_bool					m_bGoLeft = { false };

public:
	static CState_Walk_GreatSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


