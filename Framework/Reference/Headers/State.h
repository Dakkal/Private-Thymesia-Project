#pragma once
#include "Base.h"

BEGIN(Engine)

class CStateMachine;

class ENGINE_DLL CState final : public CBase
{
public:
//	explicit CState(LPDIRECT3DDEVICE9 pGraphicDev);
//	virtual ~CState();
//
//public:
//	virtual HRESULT			Ready_State(CStateMachine* pOwner) PURE;
//	// 상태타입을 반환
//	virtual STATE_TYPE		Update_State(const _float& fTimeDelta) PURE;
//	virtual void			LateUpdate_State() PURE;
//	virtual void			Render_State() PURE;
//
//	virtual STATE_TYPE		Key_Input(const _float& fTimeDelta) PURE;
//
//	virtual	STATE_TYPE		Get_State() { return m_eState; }
//
//protected:
//	// 어떤 상태머신이 자신을 지니고 있는지 알려주는 변수
//	CStateMachine*			m_pOwner;
//	// 어떤 상태인지 정해주는 변수
//	STATE_TYPE				m_eState;
//
//	LPDIRECT3DDEVICE9		m_pGraphicDev;
//
//	_bool					m_bEnter;
//
//	// State를 상속받는 상태클래스를 만들때 이걸 넣어주면 됨
//	/*static C(만들상태클래스)* Create(LPDIRECT3DDEVICE9 pGraphicDev, CStateMachine* pOwner);
//	{
//		C(만들상태클래스)* pInstance = new CC(만들상태클래스)(pGraphicDev);
//
//		if (FAILED(pInstance->Ready_State(pOwner)))
//		{
//			Safe_Release(pInstance);
//			MSG_BOX("(만들상태클래스) Create Failed");
//			return nullptr;
//		}
//
//		return pInstance;
//	}*/
//
//public:
//	virtual void	Free();
};

END

