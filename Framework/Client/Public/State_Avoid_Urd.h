#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)

class CState_Avoid_Urd final : public CState
{
private:
	enum TYPE { FRONT, SIDE, BEHIND, _END };

private:
	CState_Avoid_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Avoid_Urd() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	TYPE	m_eAvoidType = { TYPE::_END };

public:
	static CState_Avoid_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END


