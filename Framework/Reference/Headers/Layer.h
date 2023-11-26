#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Enter_Layer();
	void PriorityTick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	void Exit_Layer();

public:
	list<class CGameObject*>*	Get_LayerList() { return &m_listGameObject; }
	CGameObject* Last_GameObject();
	CGameObject* Find_GameObject(const wstring& ObjName, _uint iCloneIndex);
	HRESULT		 Delete_GameObject(const wstring& ObjName, _uint iCloneIndex);
	HRESULT		 Delete_Layer();

	HRESULT		 Delete_NonActive_Objects();

private:
	list<class CGameObject*>		m_listGameObject;

public:
	static CLayer* Create();
	virtual void Free() override;

};

END