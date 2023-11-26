#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Skill_Urd final : public CState
{
private:
	CState_Skill_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Skill_Urd() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	void					Skill_LeftRight();
	void					Skill_Front();

private:
	_bool					m_bLeft = { false };
	_bool					m_bRight = { false };

	_bool					m_bSkil_LeftRight = { false };
	_bool					m_bSkil_Front = { false };

	_bool					m_bCreateProjectile = { false };

public:
	static CState_Skill_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


