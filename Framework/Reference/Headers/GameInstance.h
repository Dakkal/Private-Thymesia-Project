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
#include "RandomManager.h"
#include "Font_Manager.h"
#include "Frustrum_Cull.h"

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
	_bool	Key_Pressing(_int _iKey);
	_bool	Key_Down(_int _iKey);
	_bool	Key_Up(_int _iKey);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint	Get_CurLevel();

public: /* For.Object_Manager */
	HRESULT			Add_Prototype(const wstring & strPrototypeTag, class CGameObject* pPrototype);
	HRESULT			Add_GameObject(_uint iLevelIndex, const _uint & iLayerIndex, const wstring & strPrototypeTag, void* pArg = nullptr);
	HRESULT			Direct_Add_GameObject(_uint iLevelIndex, const _uint & iLayerIndex, class CGameObject* pObject);
	CGameObject*	Find_GameObject(_uint iLevelIndex, const _uint & iLayerIndex, const wstring & ObjName, _uint iCloneIndex = 1);
	CGameObject*	Last_GameObject(_uint iLevelIndex, const _uint & iLayerIndex);
	CGameObject*	Clone_GameObject(const wstring & strPrototypeTag, void* pArg = nullptr);
	CGameObject*	Get_Player(_uint iLevelIndex);
	HRESULT			Delete_GameObject(_uint iLevelIndex, const _uint & iLayerIndex, const wstring & ObjName, _uint iCloneIndex = 1);
	HRESULT			Delete_Layer(_uint iLevelIndex, const _uint & iLayerIndex);
	HRESULT			Delete_NonActive_Objects(_uint iLevelIndex, const _uint & iLayerIndex);

	void			Enter_Objects(_uint iLevelIndex, const _uint & iLayerIndex);
	void			Exit_Objects(_uint iLevelIndex, const _uint & iLayerIndex);

	const map<const wstring, class CGameObject*>*	Get_Prototypes();
	list<class CGameObject*>*						Get_LayerList(_uint iLevelIndex, const _uint & iLayerIndex);

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
	HRESULT		Detrude_AABB_Collide(class CGameObject* pColObj, class CCollider* pObjCol, class CTransform* pObjTransform);
	HRESULT		Detrude_Sphere_Collide(class CGameObject* pColObj, class CCollider* pObjCol, class CTransform* pObjTransform, class CNavigation* pNavigation = nullptr);
	_float3		QuaternionToEuler(_vector vQuaternion);
	_float		Clamp(float value, float min, float max);

public: /* For.Collider_Manager */
	void		Check_Collision(const _uint iLevel, const LAYER_TAG & _eType1, const LAYER_TAG & _eType2, _float fTimedelta);

public: /* For.Random_Manager */
	const _float&	Random_Float(_float fMin, _float fMax);
	const _int&		Random_Int(_int iMin, _int iMax);
	const _bool&	Random_Coin(_float fProbality);

public: /* Font_Manager */
	HRESULT Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strFontTag, const wstring & strFontFilePath);
	HRESULT Render_Font(const wstring & strFontTag, const wstring & strText,
		const _float2 & vPos,
		FXMVECTOR color = XMVectorZero(),
		float rotation = 0.f,
		XMFLOAT2 const& origin = _float2(0.f, 0.f),
		float scale = 1.f);

public: /* For. Frustrum_Cull */
	_bool		IsIn_Frustum_World(_vector vWorldPos, _float fRadius);

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
	class CRandomManager*		m_pRandom_Manager = { nullptr };
	class CTargetManager*		m_pTarget_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CFrustrum_Cull*		m_pFrustrum_Cull = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;


};

END