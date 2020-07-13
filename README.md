# Orion

## 1. 개요
### 1-1. 소개
Orion 프로젝트는 더 값싸고 간편한 트래커를 만들 수 있지 않을까 하여 만들어진 프로젝트입니다.
### 1-2. 장점
      1. 외부 센서 (베이스 스테이션, 적외선 카메라) 가 불필요합니다.
      2. 20만원 가량으로 제작이 가능하게 만들어, HTC Vive tracker에 비하면 1/4 (베이스 스테이션 포함) 정도의 가격입니다.
      3. OpenVR 기반으로 만들어져 SteamVR을 지원하는 6DOF 기기라면 모두 호환이 가능합니다.
### 1-3. 단점
      1. 총 8개의 트래커를 몸에 부착해야 합니다. (명치, 허리, 양 허벅지, 양 종아리, 양 발)
      2. 8개 각각을 충전하는 데에도 문제가 많습니다.      
## 2. 빌드 (윈도우 기준으로 서술)
### 2-0. Git Clone
$ git clone --recurse-submodules https://github.com/SieR-VR/Orion.git 을 이용하여 git clone 명령어를 사용해야 합니다. 그렇지 않으면 OpenVR API가 받아지지 않는 참사가 일어납니다.
### 2-1. Client
먼저, [**Qt**](https://www.qt.io/)에서 Qt Creator를 받아줍니다. 그 후 대충 구글링해서 세팅해준 후 빌드해주시면 됩니다.
### 2-2. Device
[**VSCode**](https://code.visualstudio.com/) 에서 Visual Studio Code를 받아줍니다. 그 다음 [**PlatformIO**](https://platformio.org/platformio-ide) 를 참고하여 VSCode에 PlatformIO 확장을 받아줍니다. 그 다음 BLEPeripheral 라이브러리를 받아주면 세팅 끝입니다.
### 2-3. Driver
[**Visual Studio**](https://visualstudio.microsoft.com/ko/) 에서 Visual Studio를 받아줍니다. (Code로도 가능할까 싶지만 당장은 Visual Studio로 하게 만들었습니다.)
## 3. 원리
OpenVR Driver API를 통해 HMD의 위치를 얻어낼 수 있다는 것에 착안하여, 방향만을 측정하는 센서로 온 몸을 추적할 수 있게 만들었습니다.

    HmdVector3_t movePosition(const HmdQuaternion_t& qrot, const HmdVector3_t& position, const float& distance) {

	HmdVector3_t rvec = { position.v[0], position.v[1], position.v[2] };
	double sqw = qrot.w*qrot.w, sqx = qrot.x*qrot.x, sqy = qrot.y*qrot.y, sqz = qrot.z*qrot.z;

	rvec.v[0] += -(2.0f * float(qrot.x*qrot.z + qrot.w * qrot.y)) * distance;
	rvec.v[1] += -(2.0f * float(qrot.y * qrot.z - qrot.w * qrot.x)) * distance;
	rvec.v[2] += (2.0f * float(sqx + sqy) - 1) * distance;

	return rvec;
    }
     
이 함수는 position 위치로부터 qrot 각도로 distance 거리만큼 떨어진 곳의 좌표를 구하는 함수입니다.

movePosition([HMD의 위치], [HMD 방향 - 90º], [머리에서부터 목까지의 거리]) = [목의 좌표]이고,

movePosition([목의 좌표], [명치 트래커 방향 - 90º], [목에서부터 명치까지의 거리]) = [명치의 좌표]이므로,

명치 트래커의 방향과 HMD의 좌표, 방향만으로 트래커의 위치를 알아낼 수 있습니다.

또한 이와 같은 방법을 계속 적용해나간다면 발까지의 모든 좌표를 알 수 있게 됩니다.
