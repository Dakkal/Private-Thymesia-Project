#include "StateMachine.h"
#include "State.h"
#include "BinModel.h"
#include "GameObject.h"

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

HRESULT CStateMachine::Tick(const _float& fTimeDelta)
{
	STATE eState = m_pCurState->Tick(fTimeDelta);

	// ���� ���¿� �ٸ����°� ��ȯ�Ǹ� ��ȯ�� ���·� ����
	if (eState != m_eCurState)
	{
		Set_State(eState);
	}

	if (STATE::_END == eState)
		return E_FAIL;
	else
		return S_OK;
}

HRESULT CStateMachine::LateTick(const _float& fTimeDelta)
{
	STATE eState = m_pCurState->LateTick(fTimeDelta);

	if (STATE::_END == eState)
		return E_FAIL;
	else
		return S_OK;
}

HRESULT CStateMachine::Set_State(STATE eState)
{
	auto	iter = find_if(
		m_StateMap.begin(), m_StateMap.end(), 
		[&](const pair<STATE, CState*>& pair) 
		{
			return eState == pair.first;
		});

	if (iter == m_StateMap.end())
		return E_FAIL;

	if (STATE::_END == m_eCurState)
		m_ePreState = eState;
	else
		m_ePreState = m_eCurState;

	if(nullptr != m_pCurState)
		m_pCurState->Reset_State();

	m_pCurState = iter->second;
	m_eCurState = eState;
	m_pCurState->Enter_State();

	return S_OK;
}

HRESULT CStateMachine::Add_State(STATE eState, CState* pState)
{
	if (nullptr == pState || eState != pState->Get_State())
		return E_FAIL;

	m_StateMap.emplace(eState, pState);

	return S_OK;
}

CState* CStateMachine::Get_State(STATE eState)	const
{
	auto	iter = find_if(m_StateMap.begin(), m_StateMap.end(), [&](const pair<STATE, CState*>& pair) {
		return eState == pair.first;
		});

	if (iter == m_StateMap.end())
		return nullptr;

	return iter->second;
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

	for (auto& iter : m_StateMap)
		Safe_Release(iter.second);

	m_StateMap.clear();
}
