#include "telcan.h"

class TelemetryTx {
	public:
		TelemetryTx(uint16_t id);
		void addByte(uint8_t byte);
		void addBytes(uint8_t bytes[], uint8_t len);
		void send();
	private:
		CAN_TxHeaderTypeDef header;
		uint8_t data[8];
};

TelemetryTx::TelemetryTx(uint16_t id) {
	HAL_CAN_Start(&hcan1);

	header.StdId = id;
	header.ExtId = 0;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 0;
}

void TelemetryTx::addByte(uint8_t byte) {
	if(header.DLC == 8) return;
	data[header.DLC++] = byte;
}

void TelemetryTx::addBytes(uint8_t bytes[]) {
	uint8_t len = sizeof(bytes);
	for(uint8_t i = 0; i < len; i++) {
		this->addByte(bytes[i]);
	}
}

void TelemetryTx::send() {
	HAL_CAN_AddTxMessage(&hcan1, &header, &data, &CAN_TX_MAILBOX0);
}





class TelemetryRx {

};
