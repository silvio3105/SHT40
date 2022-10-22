/**
 * @file SHT40.h
 * @author silvio3105
 * @brief Header file for SHT40 driver.
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

#ifndef _SHT40_H_
#define _SHT40_H_

// ----- INCLUDE FILES
#include            <stdint.h>


// ----- DEFINES
/**
 * @brief NAK symbol.
 * @warning User should not alter this value! 
 * 
 */
#define SHT40_NAK				(0x15)

/**
 * @brief Return value representing negative OK status.
 * 
 */
#define SHT40_NOK				(0x00)

/**
 * @brief Return value representing OK status.
 * 
 */
#define SHT40_OK				(0x01)

/**
 * @brief Return value representing missing read I2C handler.
 * 
 */
#define SHT40_I2CH_R			(0x02)

/**
 * @brief Return value representing missing write I2C handler.
 * 
 */
#define SHT40_I2CH_W			(0x03)

/**
 * @brief Macro definition for default SHT40 I2C address.
 * 
 */
#define SHT40_I2C_DEF_ADDR      (0x44) // For SHT40-AD
//#define SHT40_I2C_DEF_ADDR      (0x45) // For SHT40-B

/**
 * @brief High precision temperature & RH measurement without heater.
 * 
 */
#define SHT40_M_TRH_HP			(0xFD)

/**
 * @brief Medium precision temperature & RH measurement without heater.
 * 
 */
#define SHT40_M_TRH_MP			(0xF6)  

/**
 * @brief Low precision temperature & RH measurement without heater.
 * 
 */
#define SHT40_M_TRH_LP			(0xE0)  

/**
 * @brief Serial number command.
 * 
 */
#define SHT40_SN				(0x89)  

/**
 * @brief Soft reset command.
 * 
 */
#define SHT40_RST				(0x94)  

/**
 * @brief High precision temperature & RH measurement with 0.2W heater 1s.
 * 
 */
#define SHT40_TRH_H_H02W1S		(0x39)  

/**
 * @brief High precision temperature & RH measurement with 0.2W heater 0.1s.
 * 
 */
#define SHT40_TRH_H_H02W01S		(0x32)  

/**
 * @brief High precision temperature & RH measurement with 0.1W heater 1s.
 * 
 */
#define SHT40_TRH_H_H011W1S		(0x2F)  

/**
 * @brief High precision temperature & RH measurement with 0.1W heater 0.1s.
 * 
 */
#define SHT40_TRH_H_H011W01S	(0x24)  

/**
 * @brief High precision temperature & RH measurement with 0.02W heater 1s.
 * 
 */
#define SHT40_TRH_H_H002W1S		(0x1E)  

/**
 * @brief High precision temperature & RH measurement with 0.02W heater 0.1s.
 * 
 */
#define SHT40_TRH_H_H002W01S	(0x15)  


// ----- ENUMATORS
/**
 * @brief Enum for measurement types
 * 
 */
typedef enum: uint8_t {
	TRH_H = SHT40_M_TRH_HP,
	TRH_M = SHT40_M_TRH_MP,
	TRH_L = SHT40_M_TRH_LP,
	TRH_H_H02W1S = SHT40_TRH_H_H02W1S,
	TRH_H_H02W01S = SHT40_TRH_H_H02W01S,
	TRH_H_H011W1S = SHT40_TRH_H_H011W1S,
	TRH_H_H011W01S = SHT40_TRH_H_H011W01S,
	TRH_H_H002W1S = SHT40_TRH_H_H002W1S,
	TRH_H_H002W01S = SHT40_TRH_H_H002W01S
} SHT40_meas_t;


// ----- TYPEDEFS
/**
 * @brief Typedef for external I2C function.
 * 
 * @param addr An address of I2C slave device.
 * @param *data Pointer to data array
 * @param len Lenth of array pointed with *data
 * 
 */
typedef void (*extI2C)(uint8_t addr, uint8_t* data, uint8_t len); 


// ----- CLASSES
/**
 * @brief SHT40 Class
 * 
 */
class SHT40 {
	// Public stuff
	public:

	/**
	 * @brief Object constructor.
	 * 
	 * @param addr Address of I2C slave device.
	 * @param i2cr Pointer to external I2C read function.
	 * @param i2cw Pointer to external I2C write function. 
	 * @return No return value.
	 * 
	 * @see SHT40(extI2C, extI2C)
	 * 
	 */
	SHT40(uint8_t addr, extI2C i2cr, extI2C i2cw);

	/**
	 * @brief Overloaded object constructor.
	 * 
	 * @param i2cr Pointer to external I2C read function.
	 * @param i2cw Pointer to external I2C write function. 
	 * @return No return value.
	 * 
	 * @see SHT40(uint8_t, extI2C, extI2C)
	 * 
	 */
	SHT40(extI2C i2cr, extI2C i2cw);

	/**
	 * @brief Object deconstructor.
	 * 
	 * @return No return value.
	 * 
	 */
	~SHT40(void);

	/**
	 * @brief Measures temerature and RH.
	 * 
	 * @param type Type of measurement. 
	 * @return SHT40_NOK if NAK/NACK is received, SHT40 is busy.
	 * @return SHT40_OK if measurement was OK. 
	 */
	uint8_t measure(SHT40_meas_t type);

	/**
	 * @brief Calculates temperature from last measure call.
	 * 
	 * @param &out Output variable for temperature in °C.
	 * @return SHT40_NOK if measurement data does not exist.
	 * @return SHT40_OK if temperature is calculated.
	 */
	uint8_t temperature(uint32_t& out);

	/**
	 * @brief Calculates RH from last measure call.
	 * 
	 * @param &out Output variable for relative humidity in %.
	 * @return SHT40_NOK if measurement data does not exist.
	 * @return SHT40_OK	if relative humidity is calculated.
	 */
	uint8_t rh(uint32_t& out);

	/**
	 * @brief Reads unique serial number.
	 * 
	 * @return Chip's unique serial number.
	 */
	uint32_t whoAmI(void);	

	/**
	 * @brief Function to reset the chip.
	 * 
	 * @return No return value.
	 */
	void reset(void) const;

	/**
	 * @brief Validates provided data to the object.
	 * 
	 * @return SHT40_I2CH_R if I2C read handler is null.
	 * @return SHT40_I2CH_W if I2C write handler is null.
	 * @return SHT40_NOK if SN is 0x00000000 or 0xFFFFFFFF.
	 * @return SHT40_OK if everything is OK.
	 */
	uint8_t init(void);



	// Private stuff
	private:

	/**
	 * @brief Union of SHT40 data.
	 * 
	 */
	union {
		/**
		 * @brief Header for NACK/NAK is sensor is busy.
		 * 
		 */
		uint8_t header;

		/**
		 * @brief Struct for SHT40 serial number data.
		 * 
		 */
		struct {
			uint16_t sn1;
			uint8_t crc1;
			uint16_t sn2;
			uint8_t crc2;		
		} snData;

		/**
		 * @brief Struct for SHT40 measurement data.
		 * 
		 */
		struct {
			uint16_t temp;
			uint8_t crc1;
			uint16_t rh;
			uint8_t crc2;
		} mData;
	} data;

	/**
	 * @brief I2C slave device address
	 * 
	 */
	uint8_t address = SHT40_I2C_DEF_ADDR;

	/**
	 * @brief Pointer to external I2C read function.
	 * 
	 */
	extI2C I2CRead = nullptr;

	/**
	 * @brief Pointer to external I2C write function.
	 * 
	 */
	extI2C I2CWrite = nullptr;

};


#endif // _SHT40_H_

// END WITH NEW LINE
