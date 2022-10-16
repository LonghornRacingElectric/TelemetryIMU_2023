#include "project.h"
#include "telcan.h"

void projectMain() {
	// setup
	TelemetryCan telcan = TelemetryCan(TELCAN_GROUP_ID_IMU, TELCAN_UNIQUE_ID_IMU_WHEEL_LEFT_FRONT);

	// loop
	while(1) {
		uint16_t accelX = 0xA1B2;
		uint16_t accelY = 0xC3D4;
		uint16_t accelZ = 0xE5F6;

		TelemetryCanPacket_IMU_Accel packet = TelemetryCanPacket_IMU_Accel(accelX, accelY, accelZ);
		telcan.send(&packet);
	}
}
