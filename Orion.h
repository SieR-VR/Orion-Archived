#pragma once

enum TrackerType {
	CHEST = 1, WAIST, RIGHT_THIGH, LEFT_THIGH, RIGHT_CALF, LEFT_CALF, RIGHT_FOOT, LEFT_FOOT
};

struct OverlayShereMem {
	bool connecting[9];
	bool connection[9];
	double position[9][3];
	bool calibration;
};