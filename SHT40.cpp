/**
 * @file SHT40.cpp
 * @author silvio3105 (www.github.com/silvio3105)
 * @brief SHT40 C++ file.
 * @version v0.1r1
 * @date 2022-10-22
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


// ----- METHODS DEFINITIONS
SHT40::SHT40(uint8_t addr = SHT40_I2C_DEF_ADDR, extI2C i2cr, extI2C i2cw)
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
	I2CWrite(address, (uint8_t*)&type, 1);  

	// Read from SHT40  
	I2CRead(address, (uint8_t*)&data.mData, 6);

	// If NAK is received
	if (data.header == SHT40_NAK) return SHT40_NOK;
	return SHT40_OK;    
}

uint8_t SHT40::temperature(uint32_t& out)
{
	// If temperature data is zero
	if (!data.mData.temp) return SHT40_NOK;

	// Calculate temperature from SHT40 data in desired temperature unit
	if (getUnit() == SHT40_UNIT_C) out = -45 + (175 * ((uint32_t)data.mData.temp / 65535));
		else out = -49 + (315 * ((uint32_t)data.mData.temp / 65535));

	// Reset temperature data to zero
	data.mData.temp = 0;
	return SHT40_OK;
}

uint8_t SHT40::rh(int16_t& out)
{
	// If RH data is zero
	if (!data.mData.rh) return SHT40_NOK;

	// Calculate RH from SHT40 data
	out =  -6 + (125 * ((uint32_t)data.mData.rh / 65535));
	if (out > 100) out = 100;
	else if (out < 0) out = 0;

	// Reset RH data to zero
	data.mData.rh = 0;
	return SHT40_OK;
}

uint32_t SHT40::whoAmI(void)
{
	uint8_t cmd = SHT40_SN;

	// Send S/N read command
    I2CWrite(address, (uint8_t*)&cmd, 1);

	// Read S/N from SHT40
    I2CRead(address, (uint8_t*)&data.snData, 6);

	// If NAK is received
	if (data.header == SHT40_NAK) return SHT40_NOK;

	// Concat 2x16-bit S/N data to 32-bit S/N
	return (uint32_t)((data.snData.sn1 << 16) | data.snData.sn2);	
}

void SHT40::reset(void) const
{
	uint8_t cmd = SHT40_RST;

	// Send reset command to SHT40
	I2CWrite(address, (uint8_t*)&cmd, 1); 
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

inline void SHT40::clear(void)
{
	// Clear measured data
	memset(&data.mData, 0, sizeof(data.mData));
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

// END WITH NEW LINE
