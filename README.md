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
