#include "GameInstance.h"
#include "TargetManager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pSound_Manager(CSound_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pCalculator(CCalculator::GetInstance())
	, m_pCollider_Manager(CCollideManager::GetInstance())
	, m_pRandom_Manager(CRandomManager::GetInstance())
	, m_pTarget_Manager(CTargetManager::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
	, m_pFrustrum_Cull(CFrustrum_Cull::GetInstance())
{
	Safe_AddRef(m_pFrustrum_Cull);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pCalculator);
	Safe_AddRef(m_pCollider_Manager);
	Safe_AddRef(m_pRandom_Manager);
}

HRESULT CGameInstance::Initialize_Engine(const GRAPHIC_DESC& GraphicDesc, HINSTANCE hInstance, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext, _uint iLevelIndex)
{
	/* 그래픽디바이스 초기화 처리. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* 사운드디바이스 초기화 처리. */
	if (FAILED(m_pSound_Manager->Ready_Sound()))
		return E_FAIL;


	/* 입력디바이스 초기화 처리. */
	if (FAILED(m_pInput_Device->Reserve_InputDevice(hInstance, GraphicDesc.hWnd)))
		return E_FAIL;


	/* 오브젝트 매니져의 예약 처리. */
	if (FAILED(m_pObject_Manager->Reserve_Manager(iLevelIndex)))
		return E_FAIL;

	/* 컴포넌트 매니져의 예약 처리. */
	if (FAILED(m_pComponent_Manager->Reserve_Manager(iLevelIndex)))
		return E_FAIL;

	/* 파이프라인 생성 */
	if (FAILED(m_pPipeLine->Initialize()))
		return E_FAIL;

	/* 절두체 생성 */
	if (FAILED(m_pFrustrum_Cull->Initialize()))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick(_float fTimeDelta)
{
	m_pInput_Device->Tick();

	m_pObject_Manager->PriorityTick(fTimeDelta);
	m_pLevel_Manager->PriorityTick(fTimeDelta);

	m_pObject_Manager->Tick(fTimeDelta);
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pPipeLine->Tick();
	m_pFrustrum_Cull->Tick();

	m_pObject_Manager->LateTick(fTimeDelta);

	m_pLight_Manager->Caculate_ShadowLight();

	m_pLevel_Manager->LateTick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_vector vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEY_STATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVE_STATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Is_MouseMove()
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Is_MosueMove();
}

_bool CGameInstance::Key_Pressing(_int _iKey)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Pressing(_iKey);
}

_bool CGameInstance::Key_Down(_int _iKey)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Down(_iKey);
}

_bool CGameInstance::Key_Up(_int _iKey)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Up(_iKey);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

_uint CGameInstance::Get_CurLevel()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_CurLevel();
}

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

const map<const wstring, class CGameObject*>* CGameInstance::Get_Prototypes()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Prototypes();
}

list<class CGameObject*>* CGameInstance::Get_LayerList(_uint iLevelIndex, const _uint& iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return  nullptr;

	return m_pObject_Manager->Get_LayerList(iLevelIndex, iLayerIndex);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const _uint& iLayerIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, iLayerIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Direct_Add_GameObject(_uint iLevelIndex, const _uint& iLayerIndex, CGameObject* pObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Direct_Add_GameObject(iLevelIndex, iLayerIndex, pObject);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLevelIndex, const _uint& iLayerIndex, const wstring& ObjName, _uint iCloneIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(iLevelIndex, iLayerIndex, ObjName, iCloneIndex);
}

CGameObject* CGameInstance::Last_GameObject(_uint iLevelIndex, const _uint& iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Last_GameObject(iLevelIndex, iLayerIndex);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Get_Player(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Player(iLevelIndex);
}

HRESULT CGameInstance::Delete_GameObject(_uint iLevelIndex, const _uint& iLayerIndex, const wstring& ObjName, _uint iCloneIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_GameObject(iLevelIndex, iLayerIndex, ObjName, iCloneIndex);
}

HRESULT CGameInstance::Delete_Layer(_uint iLevelIndex, const _uint& iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_Layer(iLevelIndex, iLayerIndex);
}

HRESULT CGameInstance::Delete_NonActive_Objects(_uint iLevelIndex, const _uint& iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Delete_NonActive_Objects(iLevelIndex, iLayerIndex);
}

void CGameInstance::Enter_Objects(_uint iLevelIndex, const _uint& iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	return m_pObject_Manager->Enter_Objects(iLevelIndex, iLayerIndex);
}

void CGameInstance::Exit_Objects(_uint iLevelIndex, const _uint& iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	return m_pObject_Manager->Exit_Objects(iLevelIndex, iLayerIndex);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, CGameObject* pOwner, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;


	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pOwner, pArg);
}

HRESULT CGameInstance::PlaySoundFile(const wstring& strSoundKey, CHANNELID eCh, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->PlaySoundFile(strSoundKey, eCh, fVolume);
}

HRESULT CGameInstance::PlayBGM(const wstring& strSoundKey, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->PlayBGM(strSoundKey, fVolume);
}

HRESULT CGameInstance::StopSound(CHANNELID eCh)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->StopSound(eCh);
}

HRESULT CGameInstance::StopSoundAll()
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->StopSoundAll();
}

HRESULT CGameInstance::SetChannelVolume(CHANNELID eCh, _float fVolume)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	return m_pSound_Manager->SetChannelVolume(eCh, fVolume);
}

HRESULT CGameInstance::Bind_TransformToShader(CShader* pShader, const char* pConstantName, CPipeLine::TRANSFORM_STATE eState)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Bind_TransformToShader(pShader, pConstantName, eState);
}

HRESULT CGameInstance::Bind_CamPosToShader(CShader* pShader, const char* pConstantName)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Bind_CamPosToShader(pShader, pConstantName);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORM_STATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORM_STATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_vector CGameInstance::Get_CamPosition_Vector() const 
{
	if (nullptr == m_pPipeLine)
		return _vector();

	return m_pPipeLine->Get_CamPosition_Vector();
}

_vector CGameInstance::Get_CamLook_Vector() const
{
	if (nullptr == m_pPipeLine)
		return _vector();

	return m_pPipeLine->Get_CamLook_Vector();
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iLightIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iLightIndex);
}

const LIGHT_DESC* CGameInstance::Get_ShadowLightDesc(_uint iLightIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_ShadowLightDesc(iLightIndex);
}

HRESULT CGameInstance::Caculate_ShadowLight()
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Caculate_ShadowLight();
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc, CTransform* pPlayerTransform)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;


	return m_pLight_Manager->Add_Light(LightDesc, pPlayerTransform);
}

_vector CGameInstance::Picking_Terrain(RECT rc, POINT pt, CTransform* pTransform, CVIBuffer* pBuffer)
{
	if (nullptr == m_pCalculator)
		return _vector();

	return m_pCalculator->Picking_Terrain(rc, pt, pTransform, pBuffer);
}

_vector CGameInstance::Picking_Object(RECT rc, POINT pt, CTransform* pTransform, CVIBuffer* pBuffer)
{
	if (nullptr == m_pCalculator)
		return _vector();

	return m_pCalculator->Picking_Object(rc, pt, pTransform, pBuffer);
}

HRESULT CGameInstance::Detrude_AABB_Collide(CGameObject* pColObj, CCollider* pObjCol, CTransform* pObjTransform, CNavigation* pNavigation)
{
	if (nullptr == m_pCalculator)
		return E_FAIL;

	return m_pCalculator->Detrude_Collide(pColObj, pObjCol, pObjTransform, pNavigation);
}

HRESULT CGameInstance::Detrude_Sphere_Collide(CGameObject* pColObj, CCollider* pObjCol, CTransform* pObjTransform, CNavigation* pNavigation)
{
	if (nullptr == m_pCalculator)
		return E_FAIL;

	return m_pCalculator->Detrude_Sphere_Collide(pColObj, pObjCol, pObjTransform, pNavigation);
}

_float3 CGameInstance::QuaternionToEuler(_vector vQuaternion)
{
	if (nullptr == m_pCalculator)
		return _float3();

	return m_pCalculator->QuaternionToEuler(vQuaternion);
}

_float CGameInstance::Clamp(float value, float min, float max)
{
	if (nullptr == m_pCalculator)
		return _float();

	return m_pCalculator->Clamp(value, min, max);
}

void CGameInstance::Check_Collision(const _uint iLevel, const LAYER_TAG& _eType1, const LAYER_TAG& _eType2, _float fTimedelta)
{
	if (nullptr == m_pCollider_Manager)
		return;

	return m_pCollider_Manager->Check_Collision(iLevel, _eType1, _eType2, fTimedelta);
}

const _float& CGameInstance::Random_Float(_float fMin, _float fMax)
{
	if (nullptr == m_pRandom_Manager)
		return _float();

	return m_pRandom_Manager->Random_Float(fMin, fMax);
}

const _int& CGameInstance::Random_Int(_int iMin, _int iMax)
{
	if (nullptr == m_pRandom_Manager)
		return _int();

	return m_pRandom_Manager->Random_Int(iMin, iMax);
}

const _bool& CGameInstance::Random_Coin(_float fProbality)
{
	if (nullptr == m_pRandom_Manager)
		return false;

	return m_pRandom_Manager->Random_Coin(fProbality);
}

HRESULT CGameInstance::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pContext, strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPos, FXMVECTOR color, float rotation, XMFLOAT2 const& origin, float scale)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render(strFontTag, strText, vPos, color, rotation, origin, scale);
}

_bool CGameInstance::IsIn_Frustum_World(_vector vWorldPos, _float fRadius)
{
	if (nullptr == m_pFrustrum_Cull)
		return false;

	return m_pFrustrum_Cull->IsIn_Frustum_World(vWorldPos, fRadius);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();
	CTargetManager::GetInstance()->DestroyInstance();
	CLevel_Manager::GetInstance()->DestroyInstance();
	CObject_Manager::GetInstance()->DestroyInstance();
	CComponent_Manager::GetInstance()->DestroyInstance();
	CTimer_Manager::GetInstance()->DestroyInstance();
	CSound_Manager::GetInstance()->DestroyInstance();
	CFrustrum_Cull::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CRandomManager::GetInstance()->DestroyInstance();
	CCalculator::GetInstance()->DestroyInstance();
	CCollideManager::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CLight_Manager::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();

	Safe_Release(m_pFrustrum_Cull);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pRandom_Manager);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pGraphic_Device);
}
