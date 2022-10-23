#include "telcan.h"

extern CAN_HandleTypeDef hcan1;

void TelemetryCanPacket::writeByte(uint8_t byte) {
	if(ptr == 8) return;
	data[ptr++] = byte;
}

uint8_t TelemetryCanPacket::getSize() {
	return ptr;
}

TelemetryCanPacket_IMU_Accel::TelemetryCanPacket_IMU_Accel(uint16_t accelX, uint16_t accelY, uint16_t accelZ) {
	writeByte(TELCAN_PACKET_TYPE_IMU_ACCEL << 4);
	writeByte(accelX >> 8);
	writeByte(accelX & 0xFF);
	writeByte(accelY >> 8);
	writeByte(accelY & 0xFF);
	writeByte(accelZ >> 8);
	writeByte(accelZ & 0xFF);
}

TelemetryCanPacket_IMU_Gyro::TelemetryCanPacket_IMU_Gyro(uint16_t gyroX, uint16_t gyroY, uint16_t gyroZ) {
	writeByte(TELCAN_PACKET_TYPE_IMU_GYRO << 4);
	writeByte(gyroX >> 8);
	writeByte(gyroX & 0xFF);
	writeByte(gyroY >> 8);
	writeByte(gyroY & 0xFF);
	writeByte(gyroZ >> 8);
	writeByte(gyroZ & 0xFF);
}


TelemetryCan::TelemetryCan(uint8_t groupId, uint8_t uniqueId) {
	HAL_CAN_Start(&hcan1);

	header.StdId = (groupId << 4) | uniqueId;
	header.ExtId = 0;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
}

void TelemetryCan::send(TelemetryCanPacket *packet, uint32_t mailbox) {
	header.DLC = packet->getSize();
	HAL_CAN_AddTxMessage(&hcan1, &header, packet->data, (uint32_t*) mailbox);
}
