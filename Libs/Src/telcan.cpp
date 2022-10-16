#include "telcan.h"


extern CAN_HandleTypeDef hcan1;


class TelemetryCanTxPacket {
	public:
		void writeByte(uint8_t byte);
		uint8_t getSize();
	private:
		uint8_t ptr = 0;
		uint8_t data[8];
};

void TelemetryCanTxPacket::writeByte(uint8_t byte) {
	if(ptr == 8) return;
	data[ptr++] = byte;
}

uint8_t TelemetryCanTxPacket::getSize() {
	return ptr;
}

class TelemetryCanTxPacket_IMU_Accel : private TelemetryCanTxPacket {
	public:
		TelemetryCanTxPacket_IMU_Accel(uint16_t accelX, uint16_t accelY, uint16_t accelZ) {
			writeByte(TELCAN_PACKET_TYPE_ACCEL << 4);
			writeByte(accelX >> 8);
			writeByte(accelX & 0xFF);
			writeByte(accelY >> 8);
			writeByte(accelY & 0xFF);
			writeByte(accelZ >> 8);
			writeByte(accelZ & 0xFF);
		}
};

class TelemetryCanTxPacket_IMU_Gyro : private TelemetryCanTxPacket {
	public:
		TelemetryCanTxPacket_IMU_Gyro(uint16_t gyroX, uint16_t gyroY, uint16_t gyroZ) {
			writeByte(TELCAN_PACKET_TYPE_GYRO << 4);
			writeByte(gyroX >> 8);
			writeByte(gyroX & 0xFF);
			writeByte(gyroY >> 8);
			writeByte(gyroY & 0xFF);
			writeByte(gyroZ >> 8);
			writeByte(gyroZ & 0xFF);
		}
};

class TelemetryCanTxPacket_IMU_Temp : private TelemetryCanTxPacket {
	public:
		TelemetryCanTxPacket_IMU_Temp(uint8_t temp) {
			writeByte(TELCAN_PACKET_TYPE_TEMP << 4);
			writeByte(temp);
		}
};

class TelemetryCanTxPacket_IMU_Accel_HighRes : private TelemetryCanTxPacket {
	public:
		TelemetryCanTxPacket_IMU_Accel_HighRes(uint32_t accelX, uint32_t accelY, uint32_t accelZ) {
			writeByte((TELCAN_PACKET_TYPE_ACCEL_HIGHRES << 4) | (accelX >> 16));
			writeByte((accelX >> 8) & 0xFF);
			writeByte(accelX & 0xFF);
			writeByte(accelY >> 12);
			writeByte((accelY >> 8) & 0xFF);
			writeByte(((accelY << 4) & 0xF0) | (accelZ >> 16));
			writeByte((accelZ >> 8) & 0xFF);
			writeByte(accelZ & 0xFF);
		}
};

class TelemetryCanTxPacket_IMU_Gyro_HighRes : private TelemetryCanTxPacket {
	public:
		TelemetryCanTxPacket_IMU_Gyro_HighRes(uint32_t gyroX, uint32_t gyroY, uint32_t gyroZ) {
			writeByte((TELCAN_PACKET_TYPE_GYRO_HIGHRES << 4) | (gyroX >> 16));
			writeByte((gyroX >> 8) & 0xFF);
			writeByte(gyroX & 0xFF);
			writeByte(gyroY >> 12);
			writeByte((gyroY >> 8) & 0xFF);
			writeByte(((gyroY << 4) & 0xF0) | (gyroZ >> 16));
			writeByte((gyroZ >> 8) & 0xFF);
			writeByte(gyroZ & 0xFF);
		}
};

class TelemetryCanTxPacket_IMU_Temp_HighRes : private TelemetryCanTxPacket {
	public:
		TelemetryCanTxPacket_IMU_Temp_HighRes(uint16_t temp) {
			writeByte(TELCAN_PACKET_TYPE_TEMP_HIGHRES << 4);
			writeByte(temp >> 8);
			writeByte(temp & 0xFF);
		}
};



class TelemetryCanRxPacket {
	public:
		CAN_RxHeaderTypeDef header;
		uint8_t data[8];
	private:
		uint8_t ptr = 0;
};





class TelemetryCan {
	public:
		TelemetryCan(uint8_t groupId, uint8_t uniqueId);
		void send(TelemetryCanTxPacket *packet);
		void receive(TelemetryCanRxPacket *packet);
	private:
		CAN_TxHeaderTypeDef header;
};

TelemetryCan::TelemetryCan(uint8_t groupId, uint8_t uniqueId) {
	HAL_CAN_Start(&hcan1);

	header.StdId = (groupId << 4) | uniqueId;
	header.ExtId = 0;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
}

void TelemetryCan::send(TelemetryCanTxPacket *packet) {
	header.DLC = packet->ptr;
	HAL_CAN_AddTxMessage(&hcan1, &header, packet->data, (uint32_t*) CAN_TX_MAILBOX0);
}

void TelemetryCan::receive(TelemetryCanRxPacket *packet) {
	uint32_t num_messages = 0;
	while(num_messages == 0) {
		num_messages = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);
	}

	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &packet->header, packet->data);
}
