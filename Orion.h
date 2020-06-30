#pragma once

enum class trackerType {
	CHEST = 0, WAIST, RIGHT_THIGH, LEFT_THIGH, RIGHT_CALF, LEFT_CALF, RIGHT_FOOT, LEFT_FOOT
};

struct overlayShereMem {
	float q1, q2, q3, q4;
	bool isConnected;
};