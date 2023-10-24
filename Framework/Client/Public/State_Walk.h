#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
END


BEGIN(Client)

class CState_Walk final : public CState
{
	enum KEY { KEY_W, KEY_D, KEY_S, KEY_A, KEY_END };

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
	virtual STATE			Mouse_Move(const _float& fTimeDelta);

private:
	void	Set_KeyVector();

private:
	class CPartObject*			m_pOwnerBodyPart;
	CTransform*					m_pOwnerTransform;

	_bool						m_bIsMouseMove = { false };
	_bool						m_bIsMouseStop = { false };
	_float						m_fMouseMoveTime = { 0.f };
	_vector						m_vDir;
	_vector						m_vKeyVector[KEY_END];

	_bool						m_bIsClick = { false };

public:
	static CState_Walk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END

