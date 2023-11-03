#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END


BEGIN(Client)

class CState_Lockon_Parry final : public CState
{

private:
	CState_Lockon_Parry(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Lockon_Parry() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	_bool	m_bParry1 = { false };
	_bool	m_bParry2 = { false };
	_bool	m_bParryRe = { false };

	_bool	m_IsKeepParry = { false };

	_bool	m_bIdle = { false };
	_bool	m_bAvoid = { false };
	_bool	m_bWalk = { false };
	_bool	m_bAttack = { false };

public:
	static CState_Lockon_Parry* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END