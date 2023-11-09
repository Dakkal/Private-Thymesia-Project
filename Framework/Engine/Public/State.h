#pragma once
#include "Base.h"

BEGIN(Engine)

class CStateMachine;

class ENGINE_DLL CState abstract : public CBase
{
protected:
	CState(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState() = default;

public:
	virtual HRESULT			Initialize();
	// 상태타입을 반환하여 체인지 검사;
	virtual STATE			Tick(const _float & fTimeDelta) PURE;
	virtual STATE			LateTick(const _float & fTimeDelta) PURE;
	virtual void			Reset_State() PURE;
	virtual void			Enter_State() PURE;

	virtual STATE			Key_Input(const _float & fTimeDelta) PURE;
	

	virtual	STATE			Get_State() { return m_eState; }

protected:
	class CPartObject*		m_pOwnerBodyPart = { nullptr };
	class CTransform*		m_pOwnerTransform = { nullptr };

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	// 어떤 상태머신이 자신을 지니고 있는지 알려주는 변수
	CStateMachine*			m_pStateOwner = { nullptr };
	class CGameObject*		m_pRealOwner = { nullptr };
	// 어떤 상태인지 정해주는 변수
	STATE					m_eState = { STATE::_END};
	// 처음들어오면 
	_bool					m_bEnter = { false };

public:
	virtual void	Free() override;
};

END

