/*
* gsl1680.h - library for talking to the GSL1680 800x480 panel.
*/

#include "gsl1680.h"

gsl1680::gsl1680()
{

}
bool gsl1680::initialise(uint8_t pinWake)
{
	_WAKE = pinWake;

	digitalWrite(_WAKE, LOW);
	delay(100);
	Wire.setSpeed(CLOCK_SPEED_400KHZ);
    Wire.begin();

	_init_chip();

 return 1;
}

bool gsl1680::readData()
{
	uint8_t touch_data[24] = {0};
	bool n = _i2c_read(GSL_DATA_REG, touch_data, 24);

	if (n == 1)
	{
		event.numberOfFingers = touch_data[0];
		for(int i=0; i < event.numberOfFingers; i++){
			event.fingerPositions[i].y = ( (((uint32_t)touch_data[(i*4)+5])<<8) | (uint32_t)touch_data[(i*4)+4] ) & 0x00000FFF; // 12 bits of X coord
			event.fingerPositions[i].x = ( (((uint32_t)touch_data[(i*4)+7])<<8) | (uint32_t)touch_data[(i*4)+6] ) & 0x00000FFF; // 12 bits of Y coord
			event.fingerPositions[i].finger = (uint32_t)touch_data[(i*4)+7] >> 4; // finger that did the touch
			event.timeStamp = millis();
		}
	}else{
		return 0;
	}

	return 1;
}

bool gsl1680::_i2c_write(uint8_t reg, uint8_t *buf, int cnt)
{
	#if _DEBUG > 5
		Serial.printf("i2c write: 0x%.2X -> ", reg);
		for(int i = 0; i < cnt; i++){
			Serial.printf("0x%.2X, ", buf[i]);
		}
		Serial.printlnf(" cnt: %d", cnt);
	#endif

	Wire.beginTransmission(GSLX680_I2C_ADDR);
  Wire.write(reg);
	for(int i = 0; i < cnt; i++){
		Wire.write(buf[i]);
	}

	int r = Wire.endTransmission();
  if(r != 0){
		Serial.print("i2c write error: "); Serial.print(r); Serial.print(" "); Serial.println(reg, HEX);
		return 0;
	}

	return 1;

}

bool gsl1680::_i2c_read(uint8_t reg, uint8_t *buf, int cnt)
{
	Wire.beginTransmission(GSLX680_I2C_ADDR);
	Wire.write(reg);
	int r = Wire.endTransmission();
	if(r != 0){
		Serial.print("i2c read error: "); Serial.print(r); Serial.print(" "); Serial.println(reg, HEX);
		return 0;
	}

	int n = Wire.requestFrom(GSLX680_I2C_ADDR, cnt);
	if(n != cnt){
		Serial.print("i2c read error: did not get expected count "); Serial.print(n); Serial.print(" - "); Serial.println(cnt);
		return 0;
	}

	for(int i = 0; i<n; i++){
		buf[i] = Wire.read();
	}
	return 1;
}

void gsl1680::_init_chip()
{
	Serial.println("Initialising GSL1680 chip...");
	// Toggle Wake
	digitalWrite(_WAKE, HIGH);
	delay(50);
	digitalWrite(_WAKE, LOW);
	delay(50);
	digitalWrite(_WAKE, HIGH);
	delay(30);

	// CTP startup sequence
	_clr_reg();
	_reset_chip();
	_load_fw();
	_reset_chip();
	_startup_chip();
}

void gsl1680::_clr_reg()
{
	Serial.println("Clearing GSL1680 registers...");
	uint8_t buf[4];

	buf[0] = 0x88;
	_i2c_write(0xe0, buf, 1);
	delay(20);

	buf[0] = 0x01;
	_i2c_write(0x80, buf, 1);
	delay(5);

	buf[0] = 0x04;
	_i2c_write(0xe4, buf, 1);
	delay(5);

	buf[0] = 0x00;
	_i2c_write(0xe0, buf, 1);
	delay(20);
}

void gsl1680::_reset_chip()
{
	Serial.println("Resetting GSL1680 chip...");
	uint8_t buf[4];

	buf[0] = 0x88;
	_i2c_write(GSL_STATUS_REG, buf, 1);
	delay(20);

	buf[0] = 0x04;
	_i2c_write(0xe4, buf, 1);
	delay(10);

	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	_i2c_write(0xbc, buf, 4);
	delay(10);
}

void gsl1680::_load_fw()
{
	Serial.println("Loading GSL1680 frimware...");
	uint8_t addr;
	uint8_t Wrbuf[4];
	uint16_t source_line = 0;
	uint16_t source_len = sizeof(GSLX680_FW) / sizeof(struct fw_data);


	for (source_line = 0; source_line < source_len; source_line++) {
		addr = GSLX680_FW[source_line].offset;
		Wrbuf[0] = (char)(GSLX680_FW[source_line].val & 0x000000ff);
		Wrbuf[1] = (char)((GSLX680_FW[source_line].val & 0x0000ff00) >> 8);
		Wrbuf[2] = (char)((GSLX680_FW[source_line].val & 0x00ff0000) >> 16);
		Wrbuf[3] = (char)((GSLX680_FW[source_line].val & 0xff000000) >> 24);

		_i2c_write(addr, Wrbuf, 4);
	}
}

void gsl1680::_startup_chip()
{
	Serial.println("Starting up GSL1680 chip...");
	uint8_t buf[4];

	buf[0] = 0x00;
	_i2c_write(0xe0, buf, 1);
}