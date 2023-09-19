#pragma once

#include "Renderer.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "Shader.h"
#include "Transform.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iLevelIndex);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);

private:
	_uint	m_iLevelIndex = { 0 };
	typedef map<const wstring, class CComponent*> PROTOTYPES;
	PROTOTYPES*		m_pPrototypes = { nullptr };

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	virtual void Free() override;
};

END