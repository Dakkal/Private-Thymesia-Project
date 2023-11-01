#pragma once

#include "Component_Manager.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Sound_Manager.h"
#include "Light_Manager.h"
#include "CollideManager.h"
#include "PipeLine.h"
#include "Input_Device.h"
#include "Calculator.h"

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
	_bool	Is_MouseMove();

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint	Get_CurLevel();

public: /* For.Object_Manager */
	HRESULT			Add_Prototype(const wstring & strPrototypeTag, class CGameObject* pPrototype);
	HRESULT			Add_GameObject(_uint iLevelIndex, const _uint & iLayerIndex, const wstring & strPrototypeTag, void* pArg = nullptr);
	CGameObject*	Find_GameObject(_uint iLevelIndex, const _uint & iLayerIndex, const wstring & ObjName, _uint iCloneIndex = 1);
	CGameObject*	Last_GameObject(_uint iLevelIndex, const _uint & iLayerIndex);
	CGameObject*	Clone_GameObject(const wstring & strPrototypeTag, void* pArg = nullptr);
	HRESULT			Delete_GameObject(_uint iLevelIndex, const _uint & iLayerIndex, const wstring & ObjName, _uint iCloneIndex = 1);
	HRESULT			Delete_Layer(_uint iLevelIndex, const _uint & iLayerIndex);
	const map<const wstring, class CGameObject*>*	Get_Prototypes();
	list<class CGameObject*>*					Get_LayerList(_uint iLevelIndex, const _uint & iLayerIndex);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, class CGameObject * pOwner, void* pArg = nullptr);

public: /* For.Sound_Manager */
	HRESULT PlaySoundFile(const wstring & strSoundKey, CHANNELID eCh, _float fVolume);
	HRESULT PlayBGM(const wstring & strSoundKey, _float fVolume);
	HRESULT StopSound(CHANNELID eCh);
	HRESULT StopSoundAll();
	HRESULT SetChannelVolume(CHANNELID eCh, _float fVolume);

public: /* For.PipeLine */
	HRESULT Bind_TransformToShader(class CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORM_STATE eState);
	HRESULT Bind_CamPosToShader(class CShader* pShader, const char* pConstantName);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORM_STATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE eState) const;
	_vector Get_CamPosition_Vector() const;
	_vector Get_CamLook_Vector() const;

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iLightIndex);
	HRESULT Add_Light(const LIGHT_DESC & LightDesc);

public: /* For.Calculaotr */
	_vector		Picking_Terrain(RECT rc, POINT pt, class CTransform* pTransform, class CVIBuffer* pBuffer);
	_vector		Picking_Object(RECT rc, POINT pt, class CTransform* pTransform, class CVIBuffer* pBuffer);

public: /* For.Collider_Manager */
	void	Check_Collision(const _uint iLevel, const LAYER_TAG & _eType1, const LAYER_TAG & _eType2);

private:
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CComponent_Manager*	m_pComponent_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CCalculator*			m_pCalculator = { nullptr };
	class CCollideManager*		m_pCollider_Manager = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;


};

END