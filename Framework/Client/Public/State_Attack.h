#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END


BEGIN(Client)

class CState_Attack final : public CState
{

private:
	CState_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Attack() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	_bool	m_bAttack1 = { false };
	_bool	m_bAttack2 = { false };
	_bool	m_bAttack3 = { false };
	_bool	m_bAttackRe = { false };

	_bool	m_IsKeepAttack = { false };

	_bool	m_bIdle = { false };
	_bool	m_bAvoid = { false };
	_bool	m_bWalk = { false };
	_bool	m_bParry = { false };

public:
	static CState_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END