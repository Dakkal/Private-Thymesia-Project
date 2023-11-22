#pragma once
#include "Client_Defines.h"
#include "State.h"

	BEGIN(Client)

	class CState_Attack_Urd final : public CState
{
private:
	CState_Attack_Urd(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual ~CState_Attack_Urd() = default;

public:
	virtual HRESULT			Initialize();
	virtual STATE			Tick(const _float& fTimeDelta);
	virtual STATE			LateTick(const _float& fTimeDelta);

	virtual void			Enter_State();
	virtual void			Reset_State();

	virtual STATE			Key_Input(const _float& fTimeDelta);

private:
	STATE	Combo_1();
	STATE	Combo_2();
	STATE	Combo_3();
	STATE	Combo_4();

private:
	_bool					m_bCombo1 = { false };
	_bool					m_bCombo1_1 = { false };
	_bool					m_bCombo1_2 = { false };
	_bool					m_bCombo1_3 = { false };

	_bool					m_bCombo2 = { false };
	_bool					m_bCombo2_1 = { false };
	_bool					m_bCombo2_2 = { false };
	_bool					m_bCombo2_3 = { false };

	_bool					m_bCombo3 = { false };
	_bool					m_bCombo3_1 = { false };
	_bool					m_bCombo3_2 = { false };
	_bool					m_bCombo3_3 = { false };

	_bool					m_bCombo4 = { false };
	_bool					m_bCombo4_1 = { false };
	_bool					m_bCombo4_2 = { false };
	_bool					m_bCombo4_3 = { false };


public:
	static CState_Attack_Urd* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CStateMachine* pOwner, STATE eState);
	virtual void	Free() override;
};

END





