#pragma once

#if defined _WIN32
#include <Windows.h>
#endif

#define ORION_SHMEM_KEY L"ORION_SH_MEM"

struct OrionSensorData {
    float qw, qx, qy, qz;
    bool isConnected;
};

class Orion 
{
public :
    Orion() {
        handle = nullptr;
        dataPtr = nullptr;
    }

    bool connectToShereMem() {
        if (!handle) {
            handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, ORION_SHMEM_KEY);
            if (!handle)
                return false;
        }
        
        if (!dataPtr) {
            dataPtr = (char*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, NULL, NULL, sizeof(OrionSensorData));
            if (!dataPtr)
                return false;
        }

        return true;
    }

    bool createShereMem() {
        if (!handle) {
            handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, sizeof(OrionSensorData), ORION_SHMEM_KEY);
            if (!handle)
                return false;
        }

        if (!dataPtr) {
            dataPtr = (char*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, NULL, NULL, sizeof(OrionSensorData));
            if (!dataPtr)
                return false;
        }

        return true;
    }

    void dataUpdate(const OrionSensorData &data) {
        memcpy_s(dataPtr, sizeof(OrionSensorData), &data, sizeof(OrionSensorData));
    }

    OrionSensorData getData() {
        OrionSensorData resData;
        memcpy_s(&resData, sizeof(OrionSensorData), dataPtr, sizeof(OrionSensorData));
        return resData;
    }

    enum class trackerType {
        CHEST = 0,
        WAIST = 1,
        LEFT_THIGH = 2,
        RIGHT_THIGH = 3,
        LEFT_CALF = 4,
        RIGHT_CALF = 5,
        LEFT_FOOT = 6,
        RIGHT_FOOT = 7
    };

private:
    HANDLE handle;
    char* dataPtr;
};
