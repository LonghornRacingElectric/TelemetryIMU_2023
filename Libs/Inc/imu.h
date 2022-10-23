#ifndef __IMU_H
#define __IMU_H

#include <stdint.h>
#include <queue>
#include <stdlib.h>
#include "stm32f1xx_hal.h"



/* CONSTANTS */

#define I2C_ADDRESS		0x6B
#define ACCEL_LSB		0.00048828125 // 2^-11
#define TIMEOUT			1000
#define GRAVITY			9.81
#define BTN_TIME		3000
#define STABLE_DEADZONE	0.5
#define STABLE_RANGE	0.4
#define STABLE_SAMPLES	500



/* IMU REGISTERS */

#define REG_CTRL1_XL	0x10
#define REG_CTRL2_G		0x11

#define REG_WHO_AM_I	0x0F
#define REG_STATUS		0x1E

#define REG_GYRO_OUT	0x22 // XL, XH, YL, YH, ZL, ZH
#define REG_ACCEL_OUT	0x28

#define VAL_CTRL1_XL	0b01010100 // 208 Hz, +/- 16g
#define VAL_CTRL2_G		0b01010001 // 208 Hz, +/- 4000dps



/* CLASSES */

class IMU {
	public:
		IMU();
		bool isAccelReady();
		void readAccel(uint16_t *accelX, uint16_t *accelY, uint16_t *accelZ);
		bool isGyroReady();
		void readGyro(uint16_t *gyroX, uint16_t *gyroY, uint16_t *gyroZ);

		void checkAndCalibrate();
	private:
		uint8_t buffer[6];
		uint16_t timer = 0;

		void regWrite(uint8_t address, uint8_t length);
		void regWrite(uint8_t address);
		void regRead(uint8_t address, uint8_t length);
		void regRead(uint8_t address);

		void setLED(bool on);
		void cubeTest(float *accelXp, float *accelXn,
				float *accelYp, float *accelYn,
				float *accelZp, float *accelZn);
		void calibrate();
};


#endif /* __IMU_H */
