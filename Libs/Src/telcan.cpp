#include "telcan.h"

class TelemetryTx {
	public:
		TelemetryTx(uint16_t id);
		void send(TelemetryTxPacket *packet);
	private:
		CAN_TxHeaderTypeDef header;
};

TelemetryTx::TelemetryTX(uint16_t id) {
	HAL_CAN_Start(&hcan1);

	header.StdId = id;
	header.ExtId = 0;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
}

void TelemetryTx::send(TelemetryTxPacket *packet) {
	header.DLC = sizeof(data);
	HAL_CAN_AddTxMessage(&hcan1, &header, &packet->data, &CAN_TX_MAILBOX0);
}

class TelemetryTxPacket {
	public:
		void writeByte(uint8_t byte);
		void writeBytes(uint8_t *bytes[]);
	private:
		uint8_t ptr = 0;
		uint8_t data[8];
};


void TelemetryTxPacket::writeByte(uint8_t byte) {
	if(ptr == 8) return;
	data[ptr++] = byte;
}

void TelemetryTxPacket::writeBytes(uint8_t bytes[]) {
	uint8_t len = sizeof(bytes);
	for(uint8_t i = 0; i < len; i++) {
		addByte(bytes[i]);
	}
}





class TelemetryRx {
	public:
		TelemetryRx(uint16_t ids[]);
		void receive(TelemetryRxPacket *packet);
};

void TelemetryRx::receive(TelemetryRxPacket *packet) {
	// TODO
}

class TelemetryRxPacket {
	public:
		void readByte(uint8_t *byte);
		void readBytes(uint8_t *bytes[]);
};


void TelemetryTxPacket::readByte(uint8_t *byte) {
	*byte = 255; // TODO
}

void TelemetryTxPacket::readBytes(uint8_t *bytes[]) {
	uint8_t len = sizeof(bytes);
	for(uint8_t i = 0; i < len; i++) {
		readByte(bytes + i);
	}
}
