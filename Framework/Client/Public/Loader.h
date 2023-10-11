#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual~CLoader() = default;

public:
	HRESULT Initialize(LEVELID eNextLevel);
	_int	Loading();

public:
	const wstring Get_LoadingText() const { return m_strLoading; }
	const _bool Get_Finished() const { return m_isFinished; }

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	LEVELID					m_eNextLevel = { LEVEL_END };
	wstring					m_strLoading = { TEXT("") };
	_bool					m_isFinished = { false };

private:
	CGameInstance*			m_pGameInstance = { nullptr };

	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_Critical_Section;

private:
	HRESULT Loading_For_Level_Logo();
	HRESULT Loading_For_Level_GamePlay();
	HRESULT Loading_For_Level_Edit();

	/* For.Loading */
	HRESULT Loading_Texture();
	HRESULT Loading_Mesh();
	HRESULT Loading_Shader();
	HRESULT Loading_Object();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free() override;

};

END