/**
 * @file SHT40.cpp
 * @author silvio3105 (www.github.com/silvio3105)
 * @brief SHT40 C++ file.
 * 
 * @copyright silvio3105 (c) 2022
 * 
 */

/*

License

Copyright (c) 2022, silvio3105

Access and use of this Project and its contents are granted free of charge to any Person.
The Person is allowed to copy, modify and use The Project and its contents only for non-commercial use.
Commercial use of this Project and its contents is prohibited.
Modifying this License and/or sublicensing is prohibited.

THE PROJECT AND ITS CONTENT ARE PROVIDED "AS IS" WITH ALL FAULTS AND WITHOUT EXPRESSED OR IMPLIED WARRANTY.
THE AUTHOR KEEPS ALL RIGHTS TO CHANGE OR REMOVE THE CONTENTS OF THIS PROJECT WITHOUT PREVIOUS NOTICE.
THE AUTHOR IS NOT RESPONSIBLE FOR DAMAGE OF ANY KIND OR LIABILITY CAUSED BY USING THE CONTENTS OF THIS PROJECT.

This License shall be included in all functional textual files.

*/

// ----- INCLUDE FILES
#include            <SHT40.h>
#include            <string.h>


// ----- STATIC FUNCTION DECLARATIONS
/**
 * @brief Get required delay for measurement type.
 * 
 * @param type Measurement type. See \ref SHT40_meas_t
 * @return Required delay in ms. 
 */
static uint16_t getDelay(SHT40_meas_t type);


// ----- METHODS DEFINITIONS
SHT40::SHT40(uint8_t addr, extI2C i2cr, extI2C i2cw)
{
	// Store arguments into object
	address = addr; 
	I2CRead = i2cr;
	I2CWrite = i2cw;

	// Set temperature unit
	setUnit(SHT40_UNIT_C);

	// Clear measured data
	clear();
}

SHT40::~SHT40(void)
{
	// Reset object to default
	address = 0x00;
	I2CRead = nullptr;
	I2CWrite = nullptr;

	// Clear measured data
	clear(); 
}


uint8_t SHT40::measure(SHT40_meas_t type)
{
	// Write command to SHT40
	I2CWrite(address, (uint8_t*)&type, 1, getDelay(type));  

	// Read from SHT40  
	I2CRead(address, (uint8_t*)&mData, 6, 0);

	// If NAK is received
	if (header == SHT40_NAK) return SHT40_NOK;
	return SHT40_OK;    
}

uint8_t SHT40::temperature(int16_t& out)
{
	// If temperature data is zero
	if (!mData.temp) return SHT40_NOK;

	// Calculate temperature from SHT40 data in desired temperature unit
	if (getUnit() == SHT40_UNIT_C) out = -45 + (175 * ((mData.temp[0] << 8) | mData.temp[1]) / 655) / 100;
		else out = -49 + (315 * (((mData.temp[0] << 8) | mData.temp[1]) / 655)) / 100;

	// Reset temperature data to zero
	mData.temp[0] = 0;
	mData.temp[1] = 0;
	return SHT40_OK;
}

uint8_t SHT40::rh(uint8_t& out)
{
	// If RH data is zero
	if (!mData.rh) return SHT40_NOK;

	// Calculate RH from SHT40 data
	out =  -6 + (125 * (((mData.rh[0] << 8) | mData.rh[1]) / 655)) / 100;
	if (out > 100) out = 100;
	else if (out < 0) out = 0;

	// Reset RH data to zero
	mData.rh[0] = 0;
	mData.rh[1] = 0;
	return SHT40_OK;
}

uint32_t SHT40::whoAmI(void)
{
	uint8_t cmd = SHT40_SN;

	// Send S/N read command
    I2CWrite(address, (uint8_t*)&cmd, 1, 1);

	// Read S/N from SHT40
    I2CRead(address, (uint8_t*)&snData, 6, 0);

	// If NAK is received
	if (header == SHT40_NAK) return SHT40_NOK;

	// Concat 4x8-bit S/N data to 32-bit S/N
	return ((snData.sn1[1] << 24) | (snData.sn1[0] << 16) | (snData.sn2[1] << 8) | snData.sn2[0]);	
}

void SHT40::reset(void) const
{
	uint8_t cmd = SHT40_RST;

	// Send reset command to SHT40
	I2CWrite(address, (uint8_t*)&cmd, 1, 0); 
}

uint8_t SHT40::init(SHT40_unit_t u)
{
	// If external read and/or write methods are not provided
	if (!I2CRead) return SHT40_I2CH_R;
	if (!I2CWrite) return SHT40_I2CH_W;

	// Set temperature unit
	setUnit(u);

	// Read SHT40's S/N
	uint32_t tmp = whoAmI();

	// If S/N is bits are not zeros or ones
	if (tmp == 0 || tmp == 0xFFFFFFFF) return SHT40_NOK;

	return SHT40_OK;
}

inline void SHT40::clear(void)
{
	// Clear measured data
	memset(&mData, 0, sizeof(mData));
}

inline void SHT40::setUnit(SHT40_unit_t u)
{
	// Set new temperature unit
	unit = u;
}

inline SHT40_unit_t SHT40::getUnit(void) const
{
	// Return temperature unit
	return unit;
}


// ----- STATIC FUNCTION DEFINITIONS
static uint16_t getDelay(SHT40_meas_t type)
{
	switch (type)
	{
		case TRH_H: return 9; // 8.2ms
		case TRH_M: return 5; // 4.5ms
		case TRH_L: return 2; // 1.7ms

		case TRH_H_H02W1S:
		case TRH_H_H011W1S:
		case TRH_H_H002W1S: return 1150; // 1100ms

		case TRH_H_H02W01S:
		case TRH_H_H011W01S:
		case TRH_H_H002W01S: return 120; // 110ms

		default: break;
	}

	return 1;
}

// END WITH NEW LINE
