/**
 * @file SHT40.cpp
 * @author silvio3105 (www.github.com/silvio3105)
 * @brief SHT40 driver translation unit file.
 * 
 * @copyright Copyright (c) 2022, silvio3105
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
#include            "SHT40.h"
#include            <string.h>


// ----- STATIC FUNCTION DECLARATIONS
/**
 * @brief Get required delay for measurement type.
 * 
 * @param type Measurement type. See \ref SHT40_meas_t
 * @return Required delay in ms. 
 */
static uint16_t getMeasureDelay(SHT40_meas_t type);


// ----- METHODS DEFINITIONS
SHT40::SHT40(uint8_t addr, extI2C i2cr, extI2C i2cw)
{
	// Store arguments into object
	address = addr; 
	I2CRead = i2cr;
	I2CWrite = i2cw;

	// Set temperature unit
	setUnit(SHT40_unit_t::SHT40_UNIT_C);

	// Clear measured data
	clear();
}

SHT40::~SHT40(void)
{
	// Reset object to default
	address = 0x00;
	I2CRead = nullptr;
	I2CWrite = nullptr;
	controlReg = 0;

	// Clear measured data
	clear(); 
}


uint8_t SHT40::measure(SHT40_meas_t type)
{
	// Write command to SHT40
	I2CWrite(address, (uint8_t*)&type, 1, getMeasureDelay(type));  

	// Read from SHT40  
	I2CRead(address, (uint8_t*)&mData, 6, 0);

	// If NAK is received
	if (header == SHT40_NAK) return SHT40_NOK;

	// Clear old data flags
	controlReg &= ~(SHT40_OLD_RH | SHT40_OLD_TEMP); 

	return SHT40_OK;    
}

uint8_t SHT40::temperature(int8_t& out)
{
	uint8_t ret = SHT40_OK;

	// If temperature data is zero or old
	if (!mData.temp) return SHT40_NOK;
	if (controlReg & SHT40_OLD_TEMP) ret = SHT40_OLD_DATA;

	// Calculate temperature from SHT40 data in desired temperature unit
	if (getUnit() == SHT40_UNIT_C) out = -45 + (175 * ((mData.temp[0] << 8) | mData.temp[1]) / 655) / 100;
		else out = -49 + (315 * (((mData.temp[0] << 8) | mData.temp[1]) / 655)) / 100;

	// Set old data flag for temperature
	controlReg |= SHT40_OLD_TEMP;
	return ret;
}

uint8_t SHT40::rh(uint8_t& out)
{
	uint8_t ret = SHT40_OK;

	// If RH data is zero or old
	if (!mData.rh) return SHT40_NOK;
	if (controlReg & SHT40_OLD_RH) ret = SHT40_OLD_DATA;

	// Calculate RH from SHT40 data
	out =  -6 + (125 * (((mData.rh[0] << 8) | mData.rh[1]) / 655)) / 100;

	// Limit output value between 0 and 100
	if (out > 100) out = 100;
	else if (out < 0) out = 0;

	// Set old data flag FOR RH
	controlReg |= SHT40_OLD_RH;
	return ret;
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

	// Concat 2x16-bit S/N data to 32-bit S/N
	return ((snData.sn1 << 16) | snData.sn2);	
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
	if (!tmp || tmp == 0xFFFFFFFF) return SHT40_NOK;

	return SHT40_OK;
}

void SHT40::setUnit(SHT40_unit_t unit)
{
	// Set new temperature unit
	controlReg &= ~SHT40_UNIT;
	controlReg |= (unit << SHT40_UNIT_POS);
}

SHT40_unit_t SHT40::getUnit(void) const
{
	// Return temperature unit
	return (SHT40_unit_t)(controlReg & SHT40_UNIT);
}

void SHT40::clear(void)
{
	// Clear measured data
	memset(&mData, 0, sizeof(mData));

	// Clear old data flags
	controlReg &= ~(SHT40_OLD_RH | SHT40_OLD_TEMP); 
}	


// ----- STATIC FUNCTION DEFINITIONS
static uint16_t getMeasureDelay(SHT40_meas_t type)
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
