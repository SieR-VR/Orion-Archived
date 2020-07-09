#pragma once

namespace orion {
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

	struct trackerData {
		float w, x, y, z;
		bool connected;
	};
}
