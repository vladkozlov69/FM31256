#include "fm31256.h"

void FM31256::write_register(uint8_t addr, uint8_t reg, uint8_t val) 
{
	Wire.beginTransmission(addr);
	Wire.write((byte)reg);
	Wire.write((byte)val);
	Wire.endTransmission();
}

uint8_t FM31256::read_register(uint8_t addr, uint8_t reg) 
{
  Wire.beginTransmission(addr);
  Wire.write((byte)reg);
  Wire.endTransmission();
  Wire.requestFrom(addr, (byte)1);
  return Wire.read();
}

FM31256::FM31256(void)
{
 	m_i2cAddrMemory = DEFAULT_FRAM_ADDRESS;
	m_i2cAddrCompanion = DEFAULT_COMPANION_ADDRESS;
}

byte FM31256::writeArray (uint16_t framAddr, byte items, uint8_t values[])
{
	Wire.beginTransmission(m_i2cAddrMemory);
	Wire.write(framAddr >> 8);
	Wire.write(framAddr & 0xFF);	

	for (byte i=0; i < items ; i++) 
	{
		Wire.write(values[i]);
	}

	return Wire.endTransmission();
}

byte FM31256::readArray (uint16_t framAddr, byte items, uint8_t values[])
{
	byte result;
	Wire.beginTransmission(m_i2cAddrMemory);
	Wire.write(framAddr >> 8);
	Wire.write(framAddr & 0xFF);	
	result = Wire.endTransmission();
	
	Wire.requestFrom(m_i2cAddrMemory, (uint8_t)items);

	for (byte i=0; i < items; i++) 
	{
		values[i] = Wire.read();
	}

	return result;
}

byte FM31256::writeByte (uint16_t framAddr, uint8_t value)
{
	uint8_t buffer[] = {value}; 
	return writeArray(framAddr, 1, buffer);
}

byte FM31256::readByte (uint16_t framAddr, uint8_t *value) 
{
	uint8_t buffer[1];
	byte result = readArray(framAddr, 1, buffer);
	*value = buffer[0];
	return result;
}

byte FM31256::readWord(uint16_t framAddr, uint16_t *value)
{
	uint8_t buffer[2];
	byte result = readArray(framAddr, 2, buffer);
	*value = *reinterpret_cast<uint16_t *>(buffer);
	return result;
}

byte FM31256::writeWord(uint16_t framAddr, uint16_t value)
{
	uint8_t *buffer = reinterpret_cast<uint8_t *>(&value);
	return writeArray(framAddr, 2, buffer);
}

byte FM31256::readLong(uint16_t framAddr, uint32_t *value)
{
	uint8_t buffer[4];
	byte result = readArray(framAddr, 4, buffer);
	*value = *reinterpret_cast<uint32_t *>(buffer);
	return result;
}

byte FM31256::writeLong(uint16_t framAddr, uint32_t value)
{
	uint8_t *buffer = reinterpret_cast<uint8_t *>(&value);
	return writeArray(framAddr, 4, buffer);
}

void FM31256::enableOsc()
{
	write_register(m_i2cAddrCompanion, REG_CAL_CONTROL, 0x0);
}

void FM31256::disableOsc()
{
	write_register(m_i2cAddrCompanion, REG_CAL_CONTROL, 0x80);
}

void FM31256::prepareReadCounters()
{
	uint8_t counter_control = read_register(m_i2cAddrCompanion, REG_COUNTER_CONTROL) | 0b00001000;
	write_register(m_i2cAddrCompanion, REG_COUNTER_CONTROL, counter_control);
}

void FM31256::beginUpdateRTC()
{
	write_register(m_i2cAddrCompanion, REG_RTC_CONTROL, 0b00000010);	
}

void FM31256::endUpdateRTC()
{
	write_register(m_i2cAddrCompanion, REG_RTC_CONTROL, 0b00000000);
}

void FM31256::prepareReadRTC()
{
	write_register(m_i2cAddrCompanion, REG_RTC_CONTROL, 0b00000000);
	write_register(m_i2cAddrCompanion, REG_RTC_CONTROL, 0b00000001);
}

uint8_t FM31256::readRtcRegister(uint8_t reg)
{
	uint8_t val = read_register(m_i2cAddrCompanion, reg);
	return (10 * (val >> 4)) + (val & 0x0F);
}

void FM31256::writeRtcRegister(uint8_t reg, uint8_t val)
{
	uint8_t encoded = (val % 10) + ((val / 10) << 4);
	write_register(m_i2cAddrCompanion, reg, encoded);
}

uint16_t FM31256::readCounter1()
{
	return readCounter(REG_COUNTER1_LSB, REG_COUNTER1_MSB);
}

uint16_t FM31256::readCounter2()
{
	return readCounter(REG_COUNTER2_LSB, REG_COUNTER2_MSB);
}

void FM31256::writeCounter1(uint16_t val)
{
	writeCounter(REG_COUNTER1_LSB, REG_COUNTER1_MSB, val);
}

void FM31256::writeCounter2(uint16_t val)
{
	writeCounter(REG_COUNTER2_LSB, REG_COUNTER2_MSB, val);
}

uint16_t FM31256::readCounter(uint8_t reg_lsb, uint8_t reg_msb)
{
	prepareReadCounters();
	uint8_t lsb = read_register(m_i2cAddrCompanion, reg_lsb);
	uint16_t msb = read_register(m_i2cAddrCompanion, reg_msb);

	return (msb << 8) + lsb;
}

void FM31256::writeCounter(uint8_t reg_lsb, uint8_t reg_msb, uint16_t val)
{
	uint8_t lsb = val & 0x00FF;
	uint8_t msb = (val & 0xFF00) >> 8;
	write_register(m_i2cAddrCompanion, reg_lsb, lsb);
	write_register(m_i2cAddrCompanion, reg_msb, msb);
}


void FM31256::watchDog(bool enabled)
{
	uint8_t wd_val = read_register(m_i2cAddrCompanion, REG_WD_CONTROL);

	if (enabled)
	{
		resetWatchDog();
		write_register(m_i2cAddrCompanion, REG_WD_CONTROL, wd_val | 0b10000000);
	}
	else
	{
		write_register(m_i2cAddrCompanion, REG_WD_CONTROL, wd_val & 0b01111111);
	}
	
}

void FM31256::resetWatchDog()
{
	uint8_t wd_val = read_register(m_i2cAddrCompanion, REG_WD_FLAGS) & 0xF0;
	write_register(m_i2cAddrCompanion, REG_WD_FLAGS, wd_val | 0b00001010);
}

void FM31256::setWatchDogValue(uint8_t value)
{
	uint8_t wd_val = read_register(m_i2cAddrCompanion, REG_WD_CONTROL) &  0b10000000;
	write_register(m_i2cAddrCompanion, REG_WD_CONTROL, wd_val | (0b00011111 & value));
}
