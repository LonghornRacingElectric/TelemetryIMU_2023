#include "project.h"

#include "telcan.h"
#include "imu.h"

void projectMain() {
	// setup
	TelemetryCan can = TelemetryCan(TELCAN_GROUP_ID_IMU, TELCAN_UNIQUE_ID_IMU_WHEEL_LEFT_FRONT);

	uint16_t accelX, accelY, accelZ;

	#ifdef GYRO_ENABLE
	uint16_t gyroX, gyroY, gyroZ;
	#endif

	IMU imu = IMU();

	// loop
	while(1) {
		if(imu.isAccelReady()) {
			imu.readAccel(&accelX, &accelY, &accelZ);
			TelemetryCanPacket_IMU_Accel accelPacket = TelemetryCanPacket_IMU_Accel(accelX, accelY, accelZ);
			can.send(&accelPacket, CAN_TX_MAILBOX0);
		}

		#ifdef GYRO_ENABLE
		if(imu.isGyroReady()) {
			imu.readGyro(&gyroX, &gyroY, &gyroZ);
			TelemetryCanPacket_IMU_Gyro gyroPacket = TelemetryCanPacket_IMU_Gyro(gyroX, gyroY, gyroZ);
			can.send(&gyroPacket, CAN_TX_MAILBOX1);
		}
		#endif

		HAL_Delay(1000);
	}
}
