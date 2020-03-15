/*
============ Copyright (c) Valve Corporation, All rights reserved. ============
*/

#pragma warning(disable:4996)

#include <openvr_driver.h>
#include "driverlog.h"

#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <cstring> // <string.h>
#include <string> // c++ string
#include <memory.h>
#include <tchar.h>
#include "serialcomm.h"

#define T_BUFFERLEN 8
#define HISTORY 10
#define T_QUATCHECK 0.05

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

enum TrackerType {
	CHEST = 1, WAIST, RIGHT_THIGH, LEFT_THIGH, RIGHT_CALF, LEFT_CALF, RIGHT_FOOT, LEFT_FOOT
};

/*
쿼터니언 이니셜라이징
*/
inline HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

/*
HmdMatrix 초기화
행렬 회전에 관한 코드
*/
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
HmdQuaternion_t GetRotation(const HmdMatrix34_t& matrix) {
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

// HmdMatrix34_t에서 위치 성분 추출
HmdVector3_t GetPosition(const HmdMatrix34_t& matrix) {
	HmdVector3_t vector;

	vector.v[0] = matrix.m[0][3];
	vector.v[1] = matrix.m[1][3];
	vector.v[2] = matrix.m[2][3];

	return vector;
}

// position 위치에서 qrot 방향으로 distance만큼 떨어진 곳의 좌표
HmdVector3_t MovePosition(const HmdQuaternion_t& qrot, const HmdVector3_t& position, double distance) {

	HmdVector3_t rvec = { position.v[0], position.v[1], position.v[2] };
	double sqw = qrot.w*qrot.w, sqx = qrot.x*qrot.x, sqy = qrot.y*qrot.y, sqz = qrot.z*qrot.z;

	rvec.v[0] += -(2 * (qrot.x*qrot.z + qrot.w * qrot.y)) * distance;
	rvec.v[1] += -(2 * (qrot.y*qrot.z - qrot.w * qrot.x)) * distance;
	rvec.v[2] += (2 * (sqx + sqy) - 1) * distance;

	return rvec;
}

// 쿼터니언 곱
HmdQuaternion_t mulq(const HmdQuaternion_t& q1,const HmdQuaternion_t& q2) {
	HmdQuaternion_t rq = {
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
		q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
	};
	return rq;
}

// Euler to quat
HmdQuaternion_t ToQuat(const HmdVector3_t& angles) {
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

struct OverlayShereMem {
	bool connecting[9];
	bool connection[9];
	double position[3][9];
	bool calibration;
};

HANDLE hMapping = CreateFileMapping(
	INVALID_HANDLE_VALUE,
	NULL,
	PAGE_READWRITE,
	0,
	sizeof(OverlayShereMem),
	L"SH_MEM"
);

LPBYTE buf = (BYTE*)MapViewOfFile(
	hMapping,
	FILE_MAP_ALL_ACCESS,
	0,
	0,
	0
);

OverlayShereMem* pShereMem = (OverlayShereMem*)buf;

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
	while (!g_bExiting)
	{
#if defined( _WINDOWS )
		// on windows send the event when the Y key is pressed.
		if ((0x01 & GetAsyncKeyState('Y')) != 0)
		{
			// Y key was pressed. 
			vr::VRWatchdogHost()->WatchdogWakeUp(vr::TrackedDeviceClass_HMD);
		}
		std::this_thread::sleep_for(std::chrono::microseconds(500));
#else
		// for the other platforms, just send one every five seconds
		std::this_thread::sleep_for(std::chrono::seconds(5));
		vr::VRWatchdogHost()->WatchdogWakeUp(vr::TrackedDeviceClass_HMD);
#endif
	}
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

DriverPose_t Tracker_pose[9] = { 0 };

// Tracker Driver
class Tracker_Driver : public vr::ITrackedDeviceServerDriver
{
public:
	Tracker_Driver(const char* comport, const std::string& Modelname, TrackerType tType)
		: tType(tType)
	{
		m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
		m_ulPropertyContainer = k_ulInvalidPropertyContainer;
		DriverLog("ObjectID and Property updated");

		m_sSerialNumber = Modelname;
		m_sModelNumber = Modelname;
		
		this->comport = new char[strlen(comport) + 1];
		strcpy(this->comport, comport);

		Tracker_pose[tType] = { 0 };
	}

	virtual ~Tracker_Driver()
	{
		// nothing to do
	}

	virtual EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId)
	{
		m_unObjectId = unObjectId;
		m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, "vr_controller_vive_1_5");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, m_sSerialNumber.c_str());

		vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_NeverTracked_Bool, false);
		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_WillDriftInYaw_Bool, false);

		Tracker_pose[tType].poseIsValid = true;
		Tracker_pose[tType].deviceIsConnected = false;
		Tracker_pose[tType].willDriftInYaw = true;
		Tracker_pose[tType].shouldApplyHeadModel = true;
		Tracker_pose[tType].qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
		Tracker_pose[tType].qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);
		
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

	// RunFrame에서 호출되어 DriverPose_t 반환
	virtual DriverPose_t GetPose()
	{
		if(Tracker_pose[tType].deviceIsConnected)
		{
			int readdatawait = serialComm.serial.ReadDataWaiting();
			if (readdatawait == T_BUFFERLEN) {
				for (int i = 0; i < T_BUFFERLEN; i++) {
					serialComm.serial.ReadByte(ibuffer[i]);
				}
				serialComm.sendCommand('r');
				qw = ibuffer[0] << 8 | ibuffer[1];
				qx = ibuffer[2] << 8 | ibuffer[3];
				qy = ibuffer[4] << 8 | ibuffer[5];
				qz = ibuffer[6] << 8 | ibuffer[7];
				memset(ibuffer, 0, sizeof(ibuffer));

				// 센서 좌표계와 SteamVR 좌표계 변환
				HmdQuaternion_t raw_chest_quat = HmdQuaternion_Init(
					qw / 16384.0,
					-qy / 16384.0,
					qz / 16384.0,
					-qx / 16384.0
				);

				// 캘리브레이션
				Tracker_pose[tType].qRotation = mulq(
					raw_chest_quat,
					HmdQuaternion_Init(caliquat.w, caliquat.x, caliquat.y, caliquat.z)
				);
				CompareReadDataWaiting(8);
				DriverLog(
					"quat : %f, %f, %f, %f",
					Tracker_pose[tType].qRotation.w,
					Tracker_pose[tType].qRotation.x,
					Tracker_pose[tType].qRotation.y,
					Tracker_pose[tType].qRotation.z
				);
			}
			else if (CompareReadDataWaiting(readdatawait) || readdatawait > T_BUFFERLEN) {
				for (int i = 0; i < readdatawait; i++) {
					serialComm.serial.ReadByte(ibuffer[i]);
				}
				memset(ibuffer, 0, sizeof(ibuffer));
				serialComm.sendCommand(sendbuffer);
				DriverLog("Buffer Reset..");
				CompareReadDataWaiting(1);
				Tracker_pose[tType].qRotation = recenttrackerquat;
			}

			if (tType == CHEST) {
				vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0, &HMDpose, 1);
				HmdQuaternion_t HMDq = GetRotation(HMDpose.mDeviceToAbsoluteTracking);
				HmdVector3_t HMDp = GetPosition(HMDpose.mDeviceToAbsoluteTracking);
				HMDp = MovePosition(
					mulq(
						HMDq,
						HmdQuaternion_Init(0, 0, 1, 0)
					),
					HMDp,
					0.15
				);
				HmdVector3_t Trackerp = MovePosition(
					mulq(
						Tracker_pose[tType].qRotation,
						HmdQuaternion_Init(0.7071068, -0.7071068, 0, 0)
					),
					MovePosition(
						mulq(
							HMDq,
							HmdQuaternion_Init(0.7071068, -0.7071068, 0, 0)
						),
						HMDp,
						0.2
					),
					0.2
				);
				Tracker_pose[tType].vecPosition[0] = Trackerp.v[0];
				Tracker_pose[tType].vecPosition[1] = Trackerp.v[1];
				Tracker_pose[tType].vecPosition[2] = Trackerp.v[2];

				recenttrackerquat = Tracker_pose[tType].qRotation;

				Tracker_pose[tType].result = TrackingResult_Running_OK;
				return Tracker_pose[tType];
			}
			else if (tType == WAIST) {
				HmdVector3_t Chestpose;
				for (int i = 0; i < 3; i++)
					Chestpose.v[i] = Tracker_pose[CHEST].vecPosition[i];
				HmdVector3_t TrackerLoc = MovePosition(
					mulq(
						Tracker_pose[tType].qRotation,
						HmdQuaternion_Init(0.7071068, -0.7071068, 0, 0)
					),
					MovePosition(
						Tracker_pose[tType].qRotation,
						Chestpose,
						0.1
					),
					0.1
				);
				Tracker_pose[tType].vecPosition[0] = TrackerLoc.v[0];
				Tracker_pose[tType].vecPosition[1] = TrackerLoc.v[1];
				Tracker_pose[tType].vecPosition[2] = TrackerLoc.v[2];

				recenttrackerquat = Tracker_pose[tType].qRotation;

				Tracker_pose[tType].result = TrackingResult_Running_OK;
				return Tracker_pose[tType];
			}
			else if (tType == LEFT_THIGH) {
				HmdVector3_t Waistpose;
				for (int i = 0; i < 3; i++)
					Waistpose.v[i] = Tracker_pose[WAIST].vecPosition[i];

			}
		}
		else if (pShereMem->connection[tType]) {
			DriverLog("Try Connect..");
			if (serialComm.connect(comport)) {
				while (1) {
					serialComm.sendCommand(sendbuffer);
					Sleep(100);
					int rdw = serialComm.serial.ReadDataWaiting();
					if (rdw == T_BUFFERLEN) {
						for (int i = 0; i < T_BUFFERLEN; i++) {
							serialComm.serial.ReadByte(ibuffer[i]);
						}
						serialComm.sendCommand('r');
						qw = ibuffer[0] << 8 | ibuffer[1];
						qx = ibuffer[2] << 8 | ibuffer[3];
						qy = ibuffer[4] << 8 | ibuffer[5];
						qz = ibuffer[6] << 8 | ibuffer[7];
						memset(ibuffer, 0, sizeof(ibuffer));
						break;
					}
					else {
						for (int i = 0; i < rdw; i++) {
							serialComm.serial.ReadByte(ibuffer[i]);
						}
						memset(ibuffer, 0, sizeof(ibuffer));
					}
				}
				// 단위 쿼터니언 값으로 변환, 동시에 캘리브레이션 값 이니셜라이징
				caliquat.w = qw / 16384.0;
				caliquat.x = qy / 16384.0;
				caliquat.y = -qz / 16384.0;
				caliquat.z = qx / 16384.0;
				// 캘리브레이션 값의 반대로 이전 쿼터니언 값 넘겨주기
				recenttrackerquat = HmdQuaternion_Init(caliquat.w, -caliquat.x, -caliquat.y, -caliquat.z);
				serialComm.sendCommand(sendbuffer);
				DriverLog("raw calibration data : %d, %d, %d, %d", qw, qx, qy, qz);
				Tracker_pose[tType].deviceIsConnected = true;
				pShereMem->connection[tType] = false;
				return Tracker_pose[tType];
			}
			return Tracker_pose[tType];
		}
		else {
			return Tracker_pose[tType];
		}
	}

	void RunFrame() {
		if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid)
		{
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
		}
	}

	void ProcessEvent(const vr::VREvent_t & vrEvent) {

	}
	std::string GetSerialNumber() const { return m_sSerialNumber; }

	bool CompareReadDataWaiting(const int data) {
		static int datahistory[HISTORY] = { 8 };
		memmove(&datahistory[1], &datahistory[0], sizeof(int) * (HISTORY - 1));
		datahistory[0] = data;
		for (int i = 0; i < HISTORY - 1; i++) {
			bool datacompare;
			datacompare = (datahistory[i] == datahistory[i + 1]);
			if (!datacompare) {
				return false;
			}
		}
		return true;
	}

private:
	vr::TrackedDeviceIndex_t m_unObjectId;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	CSerialComm serialComm;
	char *comport;	
	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	bool count = 0; 
	int16_t qw, qx, qy, qz;
	BYTE ibuffer[T_BUFFERLEN * 2];
	unsigned char sendbuffer = 0;

	HmdQuaternion_t recenttrackerquat = HmdQuaternion_Init(1, 0, 0, 0);
	HmdQuaternion_t caliquat = HmdQuaternion_Init(1, 0, 0, 0);
	TrackedDevicePose_t HMDpose;
	TrackerType tType;
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
	Tracker_Driver* m_pwaist_tracker = nullptr;
	Tracker_Driver* m_pright_thigh_tracker = nullptr;
	Tracker_Driver* m_pleft_thigh_tracker = nullptr;
	Tracker_Driver* m_pright_calf_tracker = nullptr;
	Tracker_Driver* m_pleft_calf_tracker = nullptr;
	Tracker_Driver* m_pright_foot_tracker = nullptr;
	Tracker_Driver* m_pleft_foot_tracker = nullptr;
};

CServerDriver_Sample g_serverDriverNull;


EVRInitError CServerDriver_Sample::Init( vr::IVRDriverContext *pDriverContext )
{
	VR_INIT_SERVER_DRIVER_CONTEXT( pDriverContext );
	InitDriverLog( vr::VRDriverLog() );

	m_pchest_tracker = new Tracker_Driver("COM7", "Chest_Tracker", CHEST);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pchest_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pchest_tracker
	);
	m_pwaist_tracker = new Tracker_Driver("COM6", "Waist_Tracker", WAIST);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pwaist_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pwaist_tracker
	);
	m_pright_thigh_tracker = new Tracker_Driver("COM8", "Right_Thigh_Tracker", RIGHT_THIGH);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pright_thigh_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pright_thigh_tracker
	);
	m_pleft_thigh_tracker = new Tracker_Driver("COM9", "Left_Thigh_Tracker", LEFT_THIGH);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pleft_thigh_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pleft_thigh_tracker
	);
	m_pright_calf_tracker = new Tracker_Driver("COM10", "Right_Calf_Tracker", RIGHT_CALF);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pright_calf_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pright_calf_tracker
	);
	m_pleft_calf_tracker = new Tracker_Driver("COM11", "Left_Calf_Tracker", LEFT_CALF);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pleft_calf_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pleft_calf_tracker
	);
	m_pright_foot_tracker = new Tracker_Driver("COM12", "Right_Foot_Tracker", RIGHT_FOOT);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pright_foot_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pright_foot_tracker
	);
	m_pleft_foot_tracker = new Tracker_Driver("COM13", "Left_Foot_Tracker", LEFT_FOOT);
	vr::VRServerDriverHost()->TrackedDeviceAdded(
		m_pleft_foot_tracker->GetSerialNumber().c_str(), vr::TrackedDeviceClass_GenericTracker, m_pleft_foot_tracker
	);

	return VRInitError_None;
}

void CServerDriver_Sample::Cleanup() 
{
	CleanupDriverLog();
	delete m_pwaist_tracker;
	delete m_pchest_tracker;

	m_pwaist_tracker = NULL;
	m_pchest_tracker = NULL;
}

void CServerDriver_Sample::RunFrame()
{
	if (m_pchest_tracker)
	{
		m_pchest_tracker->RunFrame();
	}
	if (m_pwaist_tracker)
	{
		m_pwaist_tracker->RunFrame();
	}
	
	pShereMem->connecting[CHEST] = Tracker_pose[CHEST].deviceIsConnected;
	pShereMem->connecting[WAIST] = Tracker_pose[WAIST].deviceIsConnected;

	memcpy(pShereMem->position[CHEST], Tracker_pose[CHEST].vecPosition, sizeof(double) * 3);
	memcpy(pShereMem->position[WAIST], Tracker_pose[WAIST].vecPosition, sizeof(double) * 3);

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
