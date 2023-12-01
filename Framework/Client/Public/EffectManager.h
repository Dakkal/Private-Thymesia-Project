#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CEffectObject;
END

BEGIN(Client)

class CEffectManager : public CBase
{
	DECLARE_SINGLETON(CEffectManager)

private:
	CEffectManager();
	virtual ~CEffectManager() = default;

public:
	HRESULT					Add_Effect(const wstring EffectsName, CEffectObject* pGameObject);
	CEffectObject*	Get_Effect(const wstring EffectsName);

private:
	list<CEffectObject*>* Find_Effects(const wstring EffectsName);

private:
	map<const wstring, list<CEffectObject*>>		m_Effects;

public:
	virtual void Free() override;
};

END