#include "StateMachine.h"
#include "State.h"
#include "BinModel.h"

CStateMachine::CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CStateMachine::CStateMachine(CGameObject* pOwner, const CStateMachine& rhs)
	: CComponent(pOwner, rhs)
{
}

HRESULT CStateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	return S_OK;
}

void CStateMachine::Tick_StateMachine(const _float& fTimeDelta)
{

	STATE eState = STATE::ATTACK; // = m_pCurState->Update_State(fTimeDelta);


	// 현재 상태와 다른상태가 반환되면 반환된 상태로 변경
	if (eState != m_eCurState)
	{
		Set_State(eState);
		
	}
}

void CStateMachine::LateTick_StateMachine(const _float& fTimeDelta)
{
	m_pCurState; // ->LateUpdate_State();
}

void CStateMachine::Render_StateMachine()
{
	m_pCurState; // ->Render_State();
	
}

void CStateMachine::Set_State(STATE eState)
{
	auto	iter = find_if(m_StateMap.begin(), m_StateMap.end(), [&](const pair<STATE, CState*>& pair) {
		return eState == pair.first;
		});

	if (iter == m_StateMap.end())
		return;

	m_pCurState = iter->second;
	m_eCurState = eState;
}

HRESULT CStateMachine::Add_State(STATE eState, CState* pState)
{
	if (nullptr == pState)
		return E_FAIL;

	m_StateMap.insert({ eState, pState });

	return S_OK;
}


CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = new CStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CStateMachine::Clone(CGameObject* pOwner, void* pArg)
{
	CStateMachine* pInstance = new CStateMachine(pOwner, *this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();

	m_StateMap.clear();

	
}
