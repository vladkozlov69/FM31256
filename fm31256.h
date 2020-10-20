#ifndef __FM31256_H
#define __FM31256_H

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <Wire.h>

#define DEFAULT_FRAM_ADDRESS 0x50 // b1010x(A1)(A0)
#define DEFAULT_COMPANION_ADDRESS 0x68 // b1101x(A1)(A0)


#define MAX_FLASHSIZE 32 //kBYTE

#define REG_COUNTER2_MSB 	    0X10
#define REG_COUNTER2_LSB 	    0X0F
#define REG_COUNTER1_MSB 	    0X0E
#define REG_COUNTER1_LSB 	    0X0D
#define REG_COUNTER_CONTROL     0X0C	
#define REG_COMPANION_CONTROL   0X0B	
#define REG_WD_CONTROL 			0X0A
#define REG_WD_FLAGS 			0X09
#define REG_YEAR 		        0X08
#define REG_MONTH 		        0X07
#define REG_DAY 		        0X06
#define REG_DAYOFWEEK 	        0X05
#define REG_HOURS 		        0X04
#define REG_MINUTES 	        0X03
#define REG_SECONDS 	        0X02
#define REG_CAL_CONTROL 	    0x01	
#define REG_RTC_CONTROL			0x00

class FM31256 {
public:
	FM31256(void);
	FM31256(bool A0, bool A1);

	// FRAM operations
	byte readArray (uint16_t framAddr, byte items, uint8_t value[]);
	byte writeArray (uint16_t framAddr, byte items, uint8_t value[]);
	byte readByte (uint16_t framAddr, uint8_t *value);
	byte writeByte (uint16_t framAddr, uint8_t value);
	byte readWord(uint16_t framAddr, uint16_t *value);
	byte writeWord(uint16_t framAddr, uint16_t value);
	byte readLong(uint16_t framAddr, uint32_t *value);
	byte writeLong(uint16_t framAddr, uint32_t value);

	// RTC operations
    void enableOsc();
    void disableOsc();
	void beginUpdateRTC();
	void writeRtcRegister(uint8_t reg, uint8_t val);
	void endUpdateRTC();
	void prepareReadRTC();
	uint8_t readRtcRegister(uint8_t reg);
	
	// COUNTER operations
	void prepareReadCounters();
	uint16_t readCounter1();
	uint16_t readCounter2();
	void writeCounter1(uint16_t val);
	void writeCounter2(uint16_t val);

	// WATCHDOG operations
	void watchDog(bool enabled);
	void resetWatchDog();
	void setWatchDogValue(uint8_t value); // 1..30, each step is 100ms
private:
	uint8_t	m_i2cAddrMemory;
    uint8_t	m_i2cAddrCompanion;
	uint16_t readCounter(uint8_t reg_lsb, uint8_t reg_msb);
	void writeCounter(uint8_t reg_lsb, uint8_t reg_msb, uint16_t val);
	uint8_t read_register(uint8_t addr, uint8_t reg);
	void write_register(uint8_t addr, uint8_t reg, uint8_t val);
};

#endif 

