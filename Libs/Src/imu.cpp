#include "imu.h"

extern I2C_HandleTypeDef hi2c1;

IMU::IMU() {
	buffer[0] = VAL_CTRL1_XL;
	regWrite(REG_CTRL1_XL);

	buffer[0] = VAL_CTRL2_G;
	regWrite(REG_CTRL2_G);
}

bool IMU::isAccelReady() {
	regRead(REG_STATUS);
	return (buffer[0] & 0x01);
}

void IMU::readAccel(uint16_t *accelX, uint16_t *accelY, uint16_t *accelZ) {
	regRead(REG_ACCEL_OUT, 6);

	*accelX = buffer[0] + (buffer[1] << 8);
	*accelY = buffer[2] + (buffer[3] << 8);
	*accelZ = buffer[4] + (buffer[5] << 8);
}

bool IMU::isGyroReady() {
	regRead(REG_STATUS);
	return (buffer[0] & 0x02);
}

void IMU::readGyro(uint16_t *gyroX, uint16_t *gyroY, uint16_t *gyroZ) {
	regRead(REG_GYRO_OUT, 6);

	*gyroX = buffer[0] + (buffer[1] << 8);
	*gyroY = buffer[2] + (buffer[3] << 8);
	*gyroZ = buffer[4] + (buffer[5] << 8);
}

void IMU::regWrite(uint8_t address, uint8_t length) {
	HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, address, 1, buffer, length, TIMEOUT);
}

void IMU::regWrite(uint8_t address) {
	regWrite(address, 1);
}

void IMU::regRead(uint8_t address, uint8_t length) {
	HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, address, 1, buffer, length, TIMEOUT);
}

void IMU::regRead(uint8_t address) {
	regRead(address, 1);
}
