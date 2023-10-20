#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END


BEGIN(Client)

class CState_Walk final : public CState
{
private:
	CState_Walk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Walk() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);
	
	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	class CPartObject*			m_pOwnerBodyPart;
	CTransform*					m_pOwnerTransform;


public:
	static CState_Walk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END

