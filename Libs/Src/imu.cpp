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
	bool ready = (buffer[0] & 0x01);
	setLED(ready); // LED should be on about 1/5 of the time at 208 Hz data rate and 1000 Hz code
	return ready;
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

void IMU::setLED(bool on) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (on) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void IMU::checkAndCalibrate() {
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5)) {
		timer++;
		if(timer == BTN_TIME) {
			setLED(true); // LED on 100% = calibration mode
			timer = 0;
			calibrate();
		}
	} else {
		timer = 0;
	}
}

void IMU::cubeTest(uint8_t *accelXp, uint8_t *accelXn,
		uint8_t *accelYp, uint8_t *accelYn,
		uint8_t *accelZp, uint8_t *accelZn) {

	uint8_t incomplete = 0b111111; // X+ X- Y+ Y- Z+ Z-

	uint16_t accelX, accelY, accelZ;
	float accelXf, accelYf, accelZf;

	uint8_t lastStableAxis = 0;

	std::queue<float> stableSamples;
	float stableSample;

	float possibilities[6][3] = {
			{GRAVITY, 0, 0},
			{-GRAVITY, 0, 0},
			{0, GRAVITY, 0},
			{0, -GRAVITY, 0},
			{0, 0, GRAVITY},
			{0, 0, -GRAVITY}
	};

	while(incomplete) {
		if(!isAccelReady()) continue;
		readAccel(&accelX, &accelY, &accelZ);

		accelXf = accelX * ACCEL_LSB;
		accelYf = accelY * ACCEL_LSB;
		accelZf = accelZ * ACCEL_LSB;

		uint8_t stableAxis = 0x20;

		for(int i = 0; i < 6; i++) {
			if(
					   accelXf > possibilities[i][0] - STABLE_DEADZONE
					&& accelXf < possibilities[i][0] + STABLE_DEADZONE
					&& accelYf > possibilities[i][1] - STABLE_DEADZONE
					&& accelYf < possibilities[i][1] + STABLE_DEADZONE
					&& accelZf > possibilities[i][2] - STABLE_DEADZONE
					&& accelZf < possibilities[i][2] + STABLE_DEADZONE
			) {
				break; // stable
			}
			stableAxis >>= 1;
		}

		switch(stableAxis) {
		case 0x20:
		case 0x10:
			stableSample = abs(accelXf);
			break;
		case 0x08:
		case 0x04:
			stableSample = abs(accelYf);
			break;
		case 0x02:
		case 0x01:
			stableSample = abs(accelZf);
			break;
		}


		if(stableAxis && (stableAxis == lastStableAxis)) {
			stableSamples.push(stableSample);

			if(stableSamples.size() > STABLE_SAMPLES) {
				stableSamples.pop(); // maintain max length

				float average = 0;
				float min = 0;
				float max = 0;

				for(int i = 0; i < STABLE_SAMPLES; i++) {
					float s = stableSamples.front();
					stableSamples.pop();
					stableSamples.push(s);

					if(s < min) {
						min = s;
					}
					if(s > max) {
						max = s;
					}

					average += s;
				}
				average /= STABLE_SAMPLES;

				if((max - min > STABLE_RANGE)) {
					continue;
				}

				switch(stableAxis) {
				case 0x20:
					*accelXp = average;
					break;
				case 0x10:
					*accelXn = -average;
					break;
				case 0x08:
					*accelYp = average;
					break;
				case 0x04:
					*accelYn = -average;
					break;
				case 0x02:
					*accelZp = average;
					break;
				case 0x01:
					*accelZn = -average;
					break;
				}

				incomplete -= stableAxis;
				stableSamples = std::queue<float>();

				setLED(true);
				HAL_Delay(500);
				setLED(false);
			}
		} else {
			stableSamples = std::queue<float>();
			lastStableAxis = stableAxis;
		}

		HAL_Delay(10);
	}


}

void IMU::calibrate() {
	float accelXp, accelXn, accelYp, accelYn, accelZp, accelZn;

	cubeTest(&accelXp, &accelXn, &accelYp, &accelYn, &accelZp, &accelZn);

	accelX0 = -(accelXp + accelXn) / 2.0f;
	accelX1 = GRAVITY / (accelXp + accelX0);
	accelY0 = -(accelYp + accelYn) / 2.0f;
	accelY1 = GRAVITY / (accelYp + accelY0);
	accelZ0 = -(accelZp + accelZn) / 2.0f;
	accelZ1 = GRAVITY / (accelZp + accelZ0);

	// TODO write to memory
}











































