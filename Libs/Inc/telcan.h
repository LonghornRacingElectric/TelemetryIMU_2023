#ifndef __TELCAN_H
#define __TELCAN_H

#define TELCAN_GROUP_ID_IMU 0x01

#define TELCAN_UNIQUE_ID_IMU_BODY_FRONT 0b0000
#define TELCAN_UNIQUE_ID_IMU_BODY_BACK 0b0001
#define TELCAN_UNIQUE_ID_IMU_WHEEL_LEFT_FRONT 0b0100
#define TELCAN_UNIQUE_ID_IMU_WHEEL_LEFT_BACK 0b0101
#define TELCAN_UNIQUE_ID_IMU_WHEEL_RIGHT_FRONT 0b0110
#define TELCAN_UNIQUE_ID_IMU_WHEEL_RIGHT_BACK 0b0111

#define TELCAN_PACKET_TYPE_IMU_ACCEL 0b0001
#define TELCAN_PACKET_TYPE_IMU_GYRO 0b0010
#define TELCAN_PACKET_TYPE_IMU_TEMP 0b0011
#define TELCAN_PACKET_TYPE_IMU_ACCEL_HIGHRES 0b0101
#define TELCAN_PACKET_TYPE_IMU_GYRO_HIGHRES 0b0110
#define TELCAN_PACKET_TYPE_IMU_TEMP_HIGHRES 0b0111


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f1xx_hal.h"

class TelemetryCanPacket {
	public:
		void writeByte(uint8_t byte);
		uint8_t getSize();
		uint8_t data[8];
	private:
		uint8_t ptr = 0;
};

class TelemetryCan {
	public:
		TelemetryCan(uint8_t groupId, uint8_t uniqueId);
		void send(TelemetryCanPacket *packet);
	private:
		CAN_TxHeaderTypeDef header;
};

class TelemetryCanPacket_IMU_Accel : public TelemetryCanPacket {
	public:
		TelemetryCanPacket_IMU_Accel(uint16_t accelX, uint16_t accelY, uint16_t accelZ);
};

class TelemetryCanPacket_IMU_Gyro : public TelemetryCanPacket {
	public:
		TelemetryCanPacket_IMU_Gyro(uint16_t gyroX, uint16_t gyroY, uint16_t gyroZ);
};

#ifdef __cplusplus
}
#endif

#endif /* __TELCAN_H */
