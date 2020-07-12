/*
============ Copyright (c) Valve Corporation, All rights reserved. ============
*/

#pragma warning(disable:4996)

#include <openvr_driver.h>
#include <Orion.h>
#include "driverlog.h"

#include <vector>
#include <thread>
#include <memory.h>

#if defined( _WINDOWS )
#include <windows.h>
#endif

using namespace vr;

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif


//쿼터니언 이니셜라이징
inline HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}


//HmdMatrix 초기화
inline void HmdMatrix_SetIdentity( HmdMatrix34_t *pMatrix )
{
	pMatrix->m[0][0] = 1.f;
	pMatrix->m[0][1] = 0.f;
	pMatrix->m[0][2] = 0.f;
	pMatrix->m[0][3] = 0.f;
	pMatrix->m[1][0] = 0.f;
	pMatrix->m[1][1] = 1.f;
	pMatrix->m[1][2] = 0.f;
	pMatrix->m[1][3] = 0.f;
	pMatrix->m[2][0] = 0.f;
	pMatrix->m[2][1] = 0.f;
	pMatrix->m[2][2] = 1.f;
	pMatrix->m[2][3] = 0.f;
}

// HmdMatrix34_t에서 쿼터니언 회전 성분 추출
HmdQuaternion_t getRotation(const HmdMatrix34_t& matrix) {
	HmdQuaternion_t q;
	q.w = sqrtf(fmaxf(0, 1.0f + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2.0f;
	q.x = sqrtf(fmaxf(0, 1.0f + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2.0f;
	q.y = sqrtf(fmaxf(0, 1.0f - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2.0f;
	q.z = sqrtf(fmaxf(0, 1.0f - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2.0f;
	q.x = copysignf(q.x, matrix.m[2][1] - matrix.m[1][2]);
	q.y = copysignf(q.y, matrix.m[0][2] - matrix.m[2][0]);
	q.z = copysignf(q.z, matrix.m[1][0] - matrix.m[0][1]);
	return q;
}

// HmdMatrix34_t에서 위치 성분 추출
HmdVector3_t getPosition(const HmdMatrix34_t& matrix) {
	HmdVector3_t vector;

	vector.v[0] = matrix.m[0][3];
	vector.v[1] = matrix.m[1][3];
	vector.v[2] = matrix.m[2][3];

	return vector;
}

// position 위치에서 qrot 방향으로 distance만큼 떨어진 곳의 좌표
HmdVector3_t movePosition(const HmdQuaternion_t& qrot, const HmdVector3_t& position, const float& distance) {

	HmdVector3_t rvec = { position.v[0], position.v[1], position.v[2] };
	double sqw = qrot.w*qrot.w, sqx = qrot.x*qrot.x, sqy = qrot.y*qrot.y, sqz = qrot.z*qrot.z;

	rvec.v[0] += -(2.0f * float(qrot.x*qrot.z + qrot.w * qrot.y)) * distance;
	rvec.v[1] += -(2.0f * float(qrot.y * qrot.z - qrot.w * qrot.x)) * distance;
	rvec.v[2] += (2.0f * float(sqx + sqy) - 1) * distance;

	return rvec;
}

// 쿼터니언 곱
HmdQuaternion_t multipleQuat(const HmdQuaternion_t& q1,const HmdQuaternion_t& q2) {
	HmdQuaternion_t rq = {
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
		q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
	};
	return rq;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

class CWatchdogDriver_Sample : public IVRWatchdogProvider
{
public:
	CWatchdogDriver_Sample()
	{
		m_pWatchdogThread = nullptr;
	}

	virtual EVRInitError Init(vr::IVRDriverContext* pDriverContext);
	virtual void Cleanup();

private:
	std::thread* m_pWatchdogThread;
};

CWatchdogDriver_Sample g_watchdogDriverNull;
bool g_bExiting = false;

void WatchdogThreadFunction()
{
	while (!g_bExiting) { }
}

EVRInitError CWatchdogDriver_Sample::Init(vr::IVRDriverContext* pDriverContext)
{
	VR_INIT_WATCHDOG_DRIVER_CONTEXT(pDriverContext);
	InitDriverLog(vr::VRDriverLog());

	// Watchdog mode on Windows starts a thread that listens for the 'Y' key on the keyboard to 
	// be pressed. A real driver should wait for a system button event or something else from the 
	// the hardware that signals that the VR system should start up.
	g_bExiting = false;
	m_pWatchdogThread = new std::thread(WatchdogThreadFunction);
	if (!m_pWatchdogThread)
	{
		DriverLog("Unable to create watchdog thread\n");
		return VRInitError_Driver_Failed;
	}

	return VRInitError_None;
}

void CWatchdogDriver_Sample::Cleanup()
{
	g_bExiting = true;
	if (m_pWatchdogThread)
	{
		m_pWatchdogThread->join();
		delete m_pWatchdogThread;
		m_pWatchdogThread = nullptr;
	}

	CleanupDriverLog();
}

// Tracker Driver
class Tracker_Driver : public vr::ITrackedDeviceServerDriver
{
public:
	Tracker_Driver(const std::string& Modelname, const Orion::trackerType& tType)
		: m_tType(tType)
	{
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
		m_ulPropertyContainer = k_ulInvalidPropertyContainer;

		m_sSerialNumber = Modelname;
		m_sModelNumber = Modelname;
	}

	virtual ~Tracker_Driver() {
		// nothing to do
	}

	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId) {
		m_unObjectId = unObjectId;
		m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, "vr_controller_vive_1_5");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, m_sSerialNumber.c_str());

		vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_NeverTracked_Bool, false);
		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_WillDriftInYaw_Bool, false);

		m_devicePose.poseIsValid = true;
		m_devicePose.willDriftInYaw = false;
		m_devicePose.shouldApplyHeadModel = true;
		m_devicePose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
		m_devicePose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

		shereMemhandler.createShereMem();
		return VRInitError_None;
	}

	virtual void Deactivate() {
		m_unObjectId = vr::TrackedDeviceClass_Invalid;
	}

	virtual void EnterStandby() {}

	void *GetComponent(const char *pchComponentNameAndVersion) {
		// override this to add a component to a driver
		if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
		{
			return (vr::IVRDisplayComponent*)this;
		}
		return NULL;
	}

	virtual void PowerOff() { }

	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) {
		if (unResponseBufferSize >= 1)
			pchResponseBuffer[0] = 0;
	}

	// RunFrame에서 호출되어 DriverPose_t 반환
	virtual DriverPose_t GetPose() {
		OrionSensorData data = shereMemhandler.getData();
		if (data.isConnected) {
			m_devicePose.qRotation = HmdQuaternion_Init(data.qw, data.qy, data.qz, data.qx); //w, y, z, x
			m_devicePose.deviceIsConnected = true;
			m_devicePose.vecPosition[1] = 1;
		}
		else {
			m_devicePose.deviceIsConnected = false;
		}
		return m_devicePose;
	}

	void RunFrame() {
		if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
		}
	}

	void ProcessEvent(const vr::VREvent_t & vrEvent) { }

	std::string GetSerialNumber() const { return m_sSerialNumber; }

private:
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	Orion::trackerType m_tType;
	DriverPose_t m_devicePose = { 0 };

	Orion shereMemhandler;
};
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CServerDriver_Sample: public IServerTrackedDeviceProvider
{
public:
	virtual EVRInitError Init( vr::IVRDriverContext *pDriverContext ) ;
	virtual void Cleanup() ;
	virtual const char * const *GetInterfaceVersions() { return vr::k_InterfaceVersions; }
	virtual void RunFrame() ;
	virtual bool ShouldBlockStandbyMode()  { return false; }
	virtual void EnterStandby()  {}
	virtual void LeaveStandby()  {}

	Tracker_Driver* m_pchest_tracker = nullptr;
};

CServerDriver_Sample g_serverDriverNull;


EVRInitError CServerDriver_Sample::Init( vr::IVRDriverContext *pDriverContext )
{
	VR_INIT_SERVER_DRIVER_CONTEXT( pDriverContext );
	InitDriverLog( vr::VRDriverLog() );

	m_pchest_tracker = new Tracker_Driver("Chest_Tracker", Orion::trackerType::CHEST);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pchest_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pchest_tracker
	);

	return VRInitError_None;
}

void CServerDriver_Sample::Cleanup() 
{
	CleanupDriverLog();

	delete m_pchest_tracker;
	m_pchest_tracker = nullptr;
}

void CServerDriver_Sample::RunFrame()
{
	if (m_pchest_tracker)
		m_pchest_tracker->RunFrame();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
HMD_DLL_EXPORT void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
	if( 0 == strcmp( IServerTrackedDeviceProvider_Version, pInterfaceName ) )
	{
		return &g_serverDriverNull;
	}
	if( 0 == strcmp( IVRWatchdogProvider_Version, pInterfaceName ) )
	{
		return &g_watchdogDriverNull;
	}

	if( pReturnCode )
		*pReturnCode = VRInitError_Init_InterfaceNotFound;

	return NULL;
}
