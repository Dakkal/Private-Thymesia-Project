#pragma once

#include "Component_Manager.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Sound_Manager.h"
#include "PipeLine.h"
#include "Input_Device.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(const GRAPHIC_DESC & GraphicDesc, HINSTANCE hInstance, _Inout_ ID3D11Device * *ppDevice, _Inout_ ID3D11DeviceContext * *ppContext, _uint iLevelIndex);
	void Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public: /* For.Timer_Manager */
	_float Compute_TimeDelta(const wstring & strTimerTag);
	HRESULT	Add_Timer(const wstring & strTimerTag);

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_vector vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(CInput_Device::MOUSEKEY_STATE eMouse);
	_long	Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE eMouseState);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring & strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void* pArg = nullptr);
	CGameObject* Find_GameObject(_uint iLevelIndex, const wstring & strLayerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void* pArg = nullptr);

public: /* For.Sound_Manager */
	HRESULT PlaySoundFile(const wstring & strSoundKey, CHANNELID eCh, _float fVolume);
	HRESULT PlayBGM(const wstring & strSoundKey, _float fVolume);
	HRESULT StopSound(CHANNELID eCh);
	HRESULT StopSoundAll();
	HRESULT SetChannelVolume(CHANNELID eCh, _float fVolume);

public: /* For.PipeLine */
	HRESULT Bind_TransformToShader(class CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORM_STATE eState);

private:
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device* m_pInput_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CComponent_Manager*	m_pComponent_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;


};

END