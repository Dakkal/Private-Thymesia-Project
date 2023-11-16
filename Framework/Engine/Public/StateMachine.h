#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
private:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);;
	CStateMachine(class CGameObject* pOwner, const CStateMachine& rhs);
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg) override;

	// 현재상태의 업데이트의 반환 상태타입을 받아 비교하여 상태가 변했는지 판단함
	HRESULT				Tick(const _float& fTimeDelta);
	HRESULT				LateTick(const _float& fTimeDelta);

public:
	// 상태를 설정 및 변경해주는 함수(애니메이터까지 같이 바꿔줌)
	HRESULT				Set_State(STATE eState);
	// 상태를 맵컨테이너에다가 추가해주는 함수
	HRESULT				Add_State(STATE eState, class CState* pState);

    CState*				Get_State(STATE eState) const;
	const STATE&		Get_CurState()	{ return m_eCurState; }
	const STATE&		Get_PreState()	{ return m_ePreState; }
	const CBinModel*	Get_OwnerModel(){ return m_pOwnerModel; }

protected:
	map<STATE, CState*>		m_StateMap;
	class CState*			m_pCurState = { nullptr };
	class CBinModel*		m_pOwnerModel = { nullptr };
	STATE					m_eCurState = { STATE::_END };
	STATE					m_ePreState = { STATE::_END };

public:
	static CStateMachine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(class CGameObject* pOwner, void* pArg) override;
	virtual void Free() override;

};


END