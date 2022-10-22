/**
 * @file SHT40.cpp
 * @author silvio3105
 * @brief SHT40 C++ file
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


// ----- FUNCTION DEFINITIONS
SHT40::SHT40(uint8_t addr, extI2C i2cr, extI2C i2cw)
{
	address = addr;
	I2CRead = i2cr;
	I2CWrite = i2cw;
	
	memset(&data.mData, 0, sizeof(data.mData));
}

SHT40::SHT40(extI2C i2cr, extI2C i2cw)
{
	address = SHT40_I2C_DEF_ADDR;
	I2CRead = i2cr;
	I2CWrite = i2cw;

	memset(&data.mData, 0, sizeof(data.mData));
}

SHT40::~SHT40(void)
{
	address = 0x00;
	I2CRead = nullptr;
	I2CWrite = nullptr;

	memset(&data.mData, 0, sizeof(data.mData));   
}


uint8_t SHT40::measure(SHT40_meas_t type)
{
	I2CWrite(address, (uint8_t*)&type, 1);    
	I2CRead(address, (uint8_t*)&data.mData, 6);

	if (data.header == SHT40_NAK) return SHT40_NOK;
	return SHT40_OK;    
}

uint8_t SHT40::temperature(uint32_t& out)
{
	if (!data.mData.temp) return SHT40_NOK;

	out = -45 + (175 * (uint32_t)data.mData.temp / 65535);
	data.mData.temp = 0;
	return SHT40_OK;
}

uint8_t SHT40::rh(uint32_t& out)
{
	if (!data.mData.rh) return SHT40_NOK;

	out =  -6 + (125 * (uint32_t)data.mData.rh / 65535);
	if (out > 100) out = 100;
	else if (out < 0) out = 0;

	data.mData.rh = 0;
	return SHT40_OK;
}

uint32_t SHT40::whoAmI(void)
{
	uint8_t cmd = SHT40_SN;

    I2CWrite(address, (uint8_t*)&cmd, 1);    
    I2CRead(address, (uint8_t*)&data.snData, 6);

	if (data.header == SHT40_NAK) return SHT40_NOK;

	return (uint32_t)((data.snData.sn1 << 16) | data.snData.sn2);	
}

void SHT40::reset() const
{
	uint8_t cmd = SHT40_RST;
	I2CWrite(address, (uint8_t*)&cmd, 1); 
}

uint8_t SHT40::init()
{
	if (!I2CRead) return SHT40_I2CH_R;
	if (!I2CWrite) return SHT40_I2CH_W;

	uint32_t tmp = whoAmI();
	if (!tmp || tmp == 0xFFFFFFFF) return SHT40_NOK;

	return SHT40_OK;
}

// END WITH NEW LINE
