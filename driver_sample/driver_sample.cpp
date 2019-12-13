/*
============ Copyright (c) Valve Corporation, All rights reserved. ============
*/

#include <openvr_driver.h>
#include "driverlog.h"

#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring> // <string.h>
#include <string> // c++ string
#include <memory.h>
#include "serialcomm.h"

const double PI = 3.14159265358979323846264338;
#define beta 0.1
#define sampleFreq 512.0

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

inline HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

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


// keys for use with the settings API
static const char * const k_pch_Sample_Section = "driver_sample";
static const char * const k_pch_Sample_SerialNumber_String = "serialNumber";
static const char * const k_pch_Sample_ModelNumber_String = "modelNumber";
static const char * const k_pch_Sample_WindowX_Int32 = "windowX";
static const char * const k_pch_Sample_WindowY_Int32 = "windowY";
static const char * const k_pch_Sample_WindowWidth_Int32 = "windowWidth";
static const char * const k_pch_Sample_WindowHeight_Int32 = "windowHeight";
static const char * const k_pch_Sample_RenderWidth_Int32 = "renderWidth";
static const char * const k_pch_Sample_RenderHeight_Int32 = "renderHeight";
static const char * const k_pch_Sample_SecondsFromVsyncToPhotons_Float = "secondsFromVsyncToPhotons";
static const char * const k_pch_Sample_DisplayFrequency_Float = "displayFrequency";

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

	virtual EVRInitError Init( vr::IVRDriverContext *pDriverContext ) ;
	virtual void Cleanup() ;

private:
	std::thread *m_pWatchdogThread;
};

CWatchdogDriver_Sample g_watchdogDriverNull;


bool g_bExiting = false;

void WatchdogThreadFunction(  )
{
	while ( !g_bExiting )
	{
#if defined( _WINDOWS )
		// on windows send the event when the Y key is pressed.
		if ( (0x01 & GetAsyncKeyState( 'Y' )) != 0 )
		{
			// Y key was pressed. 
			vr::VRWatchdogHost()->WatchdogWakeUp();
		}
		std::this_thread::sleep_for( std::chrono::microseconds( 500 ) );
#else
		// for the other platforms, just send one every five seconds
		std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
		vr::VRWatchdogHost()->WatchdogWakeUp();
#endif
	}
}

EVRInitError CWatchdogDriver_Sample::Init( vr::IVRDriverContext *pDriverContext )
{
	VR_INIT_WATCHDOG_DRIVER_CONTEXT( pDriverContext );
	InitDriverLog( vr::VRDriverLog() );

	// Watchdog mode on Windows starts a thread that listens for the 'Y' key on the keyboard to 
	// be pressed. A real driver should wait for a system button event or something else from the 
	// the hardware that signals that the VR system should start up.
	g_bExiting = false;
	m_pWatchdogThread = new std::thread( WatchdogThreadFunction );
	if ( !m_pWatchdogThread )
	{
		DriverLog( "Unable to create watchdog thread\n");
		return VRInitError_Driver_Failed;
	}

	return VRInitError_None;
}


void CWatchdogDriver_Sample::Cleanup()
{
	g_bExiting = true;
	if ( m_pWatchdogThread )
	{
		m_pWatchdogThread->join();
		delete m_pWatchdogThread;
		m_pWatchdogThread = nullptr;
	}

	CleanupDriverLog();
}
class Chest_Tracker_Driver : public vr::ITrackedDeviceServerDriver
{
public:
	Chest_Tracker_Driver() {
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
		m_ulPropertyContainer = k_ulInvalidPropertyContainer;
		DriverLog("ObjectID and Property updated");

		m_sSerialNumber = "Chest_Tracker";
		m_sModelNumber = "Chest_Tracker";
		chest_pose = { 0 };
	}
	virtual ~Chest_Tracker_Driver()
	{
	}
	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId)
	{
		m_unObjectId = unObjectId;
		m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, "vr_controller_vive_1_5");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, m_sSerialNumber.c_str());


		// return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
		vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_NeverTracked_Bool, false);
		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_WillDriftInYaw_Bool, false);

		if (!serialComm.connect(trackercomport)) {
			DriverLog("connect failed");
			connect = false;
			return VRInitError_IPC_ConnectFailed;
		}
		else {
			DriverLog("connect successed");
			connect = true;
			serialComm.sendCommand(sendbuffer);
			double b[5];
			while (1) {
				serialComm.serial.ReadByte(ibuffer);
				s[idx++] = ibuffer;
				if (ibuffer == '\n') {
					break;
				}
			}
			idx = 0;
			while (count == 0) {
				if (s[idx] == 10) {
					b[0] = atoi(k[4]) / 1000.0;
					b[1] = atoi(k[0]) / 16384.0;
					b[2] = atoi(k[1]) / 16384.0;
					b[3] = atoi(k[2]) / 16384.0;
					b[4] = atoi(k[3]) / 16384.0;

					idx = ir = ic = 0;
					memset(k, 0, sizeof(k));
					break;
				}
				if (s[idx] == 32) {
					ir++;
					idx++;
					ic = 0;
				}
				k[ir][ic++] = s[idx++];
			}
			idx = 0;
			memset(s, 0, sizeof(s));
			serialComm.sendCommand(sendbuffer);
			while (1) {
				serialComm.serial.ReadByte(ibuffer);
				s[idx++] = ibuffer;
				if (ibuffer == '\n') {
					break;
				}
			}
			idx = 0;
			while (count == 0) {
				if (s[idx] == 10) {
					b[0] = atoi(k[4]) / 1000.0;
					b[1] = atoi(k[0]) / 16384.0;
					b[2] = atoi(k[1]) / 16384.0;
					b[3] = atoi(k[2]) / 16384.0;
					b[4] = atoi(k[3]) / 16384.0;

					idx = ir = ic = 0;
					memset(k, 0, sizeof(k));
					break;
				}
				if (s[idx] == 32) {
					ir++;
					idx++;
					ic = 0;
				}
				k[ir][ic++] = s[idx++];
			}
			idx = 0;
			memset(s, 0, sizeof(s));
			HmdVector3_t hv = ToEuler(HmdQuaternion_Init(b[1], -b[2], b[4], b[3]));
			hv.v[0] *= -1;
			hv.v[1] *= -1;
			hv.v[2] *= -1;
			caliquat = ToQuat(hv);
			serialComm.sendCommand(sendbuffer);
		}
		return VRInitError_None;
	}

	virtual void Deactivate()
	{
		m_unObjectId = vr::TrackedDeviceClass_Invalid;
	}

	virtual void EnterStandby()
	{
	}

	void *GetComponent(const char *pchComponentNameAndVersion)
	{
		// override this to add a component to a driver
		if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version))
		{
			return (vr::IVRDisplayComponent*)this;
		}
		return NULL;
	}

	virtual void PowerOff()
	{
	}

	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
	{
		if (unResponseBufferSize >= 1)
			pchResponseBuffer[0] = 0;
	}

	virtual DriverPose_t GetPose()
	{
		double b[5] = { 0 };

		chest_pose.poseIsValid = true;
		chest_pose.deviceIsConnected = true;
		chest_pose.willDriftInYaw = true;
		chest_pose.shouldApplyHeadModel = true;
		chest_pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
		chest_pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

		while (1) {
			serialComm.serial.ReadByte(ibuffer);
			s[idx++] = ibuffer;
			if (ibuffer == '\n') {
				break;
			}
		}
		idx = 0;
		while (count == 0) {
			if (s[idx] == 10) {
				b[0] = atoi(k[4]) / 1000.0;
				b[1] = atoi(k[0]) / 16384.0;
				b[2] = atoi(k[1]) / 16384.0;
				b[3] = atoi(k[2]) / 16384.0;
				b[4] = atoi(k[3]) / 16384.0;

				idx = ir = ic = 0;
				memset(k, 0, sizeof(k));
				break;
			}
			if (s[idx] == 32) {
				ir++;
				idx++;
				ic = 0;
			}
			k[ir][ic++] = s[idx++];
		}
		idx = 0;
		memset(s, 0, sizeof(s)); 
		HmdQuaternion_t raw_chest_quat = HmdQuaternion_Init(b[1], -b[2], b[4], b[3]);

		vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0, &HMDpose, 1);
		HmdQuaternion_t HMDq = GetRotation(HMDpose.mDeviceToAbsoluteTracking);
		HmdVector3_t HMDp = GetPosition(HMDpose.mDeviceToAbsoluteTracking);

		chest_pose.poseTimeOffset = b[0];

		//chest_pose.qRotation = mulq(mulq(mulq(raw_chest_quat, HmdQuaternion_Init(0.7071068, 0, -0.7071068, 0)), HmdQuaternion_Init(0.7071068, -0.7071068, 0, 0)),HmdQuaternion_Init(0.7071068, 0, 0, 0.7071068));
		chest_pose.qRotation = mulq(
			mulq(
				raw_chest_quat,
				HmdQuaternion_Init(caliquat.w,caliquat.x, caliquat.y, caliquat.z)
			),
			HmdQuaternion_Init(0.7071068,0,0.7071068,0)
		);

		if (chest_pose.qRotation.w == 0 && chest_pose.qRotation.x == 0 && chest_pose.qRotation.y == 0 && chest_pose.qRotation.z == 0) {
			chest_pose.qRotation = recenttrackerquat;
		}
		if (1 < chest_pose.qRotation.w || chest_pose.qRotation.w < -1 || 1 < chest_pose.qRotation.x || chest_pose.qRotation.x < -1 || 1 < chest_pose.qRotation.y || chest_pose.qRotation.y < -1 || 1 < chest_pose.qRotation.z || chest_pose.qRotation.z < -1) {
			chest_pose.qRotation = recenttrackerquat;
		}

		HmdVector3_t Trackerp = PositionMove(chest_pose.qRotation, PositionMove(mulq(HMDq, HmdQuaternion_Init(0.7071068, -0.7071068, 0, 0)), HMDp, 0.2), 0.2);
		//HmdVector3_t Trackerp = PositionMove(HMDq, HMDp, 0.5);	
		chest_pose.vecPosition[0] = Trackerp.v[0];
		chest_pose.vecPosition[1] = Trackerp.v[1];
		chest_pose.vecPosition[2] = Trackerp.v[2];

		recenttrackerquat = chest_pose.qRotation;

		chest_pose.result = TrackingResult_Running_OK;
		serialComm.sendCommand(sendbuffer);
		return chest_pose;
	}

	void RunFrame() {
		if ((m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) && connect)
		{
				vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
		}
		
	}
	void ProcessEvent(const vr::VREvent_t & vrEvent) {

	}
	std::string GetSerialNumber() const { return m_sSerialNumber; }

	HmdQuaternion_t GetRotation(HmdMatrix34_t matrix) {
		HmdQuaternion_t q;
		q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
		q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
		q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
		q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
		q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
		q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
		q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);
		return q;
	}

	HmdVector3_t GetPosition(HmdMatrix34_t matrix) {
		HmdVector3_t vector;

		vector.v[0] = matrix.m[0][3];
		vector.v[1] = matrix.m[1][3];
		vector.v[2] = matrix.m[2][3];

		return vector;
	}

	HmdVector3_t PositionMove(HmdQuaternion_t qrot, HmdVector3_t position, double distance) {

		HmdVector3_t rvec = { position.v[0], position.v[1], position.v[2] };
		double sqw = qrot.w*qrot.w, sqx = qrot.x*qrot.x, sqy = qrot.y*qrot.y, sqz = qrot.z*qrot.z;

		rvec.v[0] += -(2 * (qrot.x*qrot.z + qrot.w * qrot.y)) * distance;
		rvec.v[1] += -(2 * (qrot.y*qrot.z - qrot.w * qrot.x)) * distance;
		rvec.v[2] += (2 * (sqx + sqy) - 1) * distance;

		return rvec;
	}

	HmdQuaternion_t mulq(HmdQuaternion_t q1, HmdQuaternion_t q2) {
		HmdQuaternion_t rq = {
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
			q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
		};
		return rq;
	}

	HmdVector3_t ToEuler(HmdQuaternion_t q) {
		HmdVector3_t angles;
		HmdVector3_t rangles;
		// roll (x-axis rotation)
		double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		angles.v[0] = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
			angles.v[1] = std::copysign(PI / 2, sinp); // use 90 degrees if out of range
		else
			angles.v[1] = std::asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.v[2] = std::atan2(siny_cosp, cosy_cosp);
		
		rangles.v[0] = angles.v[0];
		rangles.v[1] = angles.v[2];
		rangles.v[2] = angles.v[1];
		
		return rangles;
	}

	HmdQuaternion_t ToQuat(HmdVector3_t angles) {
		double cy = cos(angles.v[0] * 0.5);
		double sy = sin(angles.v[0] * 0.5);
		double cp = cos(angles.v[1] * 0.5);
		double sp = sin(angles.v[1] * 0.5);
		double cr = cos(angles.v[2] * 0.5);
		double sr = sin(angles.v[2] * 0.5);

		HmdQuaternion_t q;
		q.w = cy * cp * cr + sy * sp * sr;
		q.x = cy * cp * sr - sy * sp * cr;
		q.y = sy * cp * sr + cy * sp * cr;
		q.z = sy * cp * cr - cy * sp * sr;

		return q;
	}

	HmdQuaternion_t correcrawtrackerq(HmdQuaternion_t quat) {
		return ToQuat(ToEuler(quat));
	}
private:
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	CSerialComm serialComm;
	std::string m_sSerialNumber;
	std::string m_sModelNumber;
	char trackercomport[6] = "COM25";	
	char s[100] = { 0 };
	char k[5][10];
	int idx = 0, ir = 0, ic = 0;

	bool count = 0; 
	bool connect;

	unsigned char ibuffer, sendbuffer = 114;

	DriverPose_t chest_pose;
	HmdQuaternion_t recenttrackerquat = HmdQuaternion_Init(1, 0, 0, 0);
	HmdQuaternion_t caliquat = HmdQuaternion_Init(1, 0, 0, 0);
	TrackedDevicePose_t HMDpose;
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

private:
	Chest_Tracker_Driver *m_psampletracker = nullptr;
};

CServerDriver_Sample g_serverDriverNull;


EVRInitError CServerDriver_Sample::Init( vr::IVRDriverContext *pDriverContext )
{
	VR_INIT_SERVER_DRIVER_CONTEXT( pDriverContext );
	InitDriverLog( vr::VRDriverLog() );

	m_psampletracker = new Chest_Tracker_Driver();
	vr::VRServerDriverHost()->TrackedDeviceAdded(m_psampletracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_psampletracker);

	return VRInitError_None;
}

void CServerDriver_Sample::Cleanup() 
{
	CleanupDriverLog();
	delete m_psampletracker;
	m_psampletracker = NULL;
}


void CServerDriver_Sample::RunFrame()
{
	if (m_psampletracker)
	{
		m_psampletracker->RunFrame();
	}
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
