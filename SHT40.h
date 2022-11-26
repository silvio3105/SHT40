/**
 * @file SHT40.h
 * @author silvio3105 (www.github.com/silvio3105)
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


/** \addtogroup SHT40
 * @{
 * Driver for SHT40 sensor.
*/

// ----- DEFINES
// RETURN CODES
#define SHT40_NAK				(0x15) /**< @brief NAK symbol. */
#define SHT40_NOK				(0x00) /**< @brief Return value representing negative OK status. */
#define SHT40_OK				(0x01) /**< @brief Return value representing OK status. */
#define SHT40_I2CH_R			(0x02) /**< @brief Return value representing missing read I2C handler. */
#define SHT40_I2CH_W			(0x03) /**< @brief Return value representing missing write I2C handler. */

// CONFIG
/**
 * @brief Default SHT40 I2C address.
 * 
 * @note 0x44 for SHT40-A
 * @note 0x45 for SHT40-B
 */
#define SHT40_I2C_DEF_ADDR      (0x44) // For SHT40-AD
//#define SHT40_I2C_DEF_ADDR      (0x45) // For SHT40-B

// SHT40 COMMANDS
#define SHT40_M_TRH_HP			(0xFD) /**< @brief High precision temperature & RH measurement without heater. */
#define SHT40_M_TRH_MP			(0xF6) /**< @brief Medium precision temperature & RH measurement without heater. */
#define SHT40_M_TRH_LP			(0xE0) /**< @brief Low precision temperature & RH measurement without heater. */
#define SHT40_SN				(0x89) /**< @brief Serial number command. */
#define SHT40_RST				(0x94) /**< @brief Soft reset command. */
#define SHT40_TRH_H_H02W1S		(0x39) /**< @brief High precision temperature & RH measurement with 0.2W heater 1s. */
#define SHT40_TRH_H_H02W01S		(0x32) /**< @brief High precision temperature & RH measurement with 0.2W heater 0.1s. */
#define SHT40_TRH_H_H011W1S		(0x2F) /**< @brief High precision temperature & RH measurement with 0.1W heater 1s. */
#define SHT40_TRH_H_H011W01S	(0x24) /**< @brief High precision temperature & RH measurement with 0.1W heater 0.1s. */
#define SHT40_TRH_H_H002W1S		(0x1E) /**< @brief High precision temperature & RH measurement with 0.02W heater 1s. */  
#define SHT40_TRH_H_H002W01S	(0x15) /**< @brief High precision temperature & RH measurement with 0.02W heater 0.1s. */ 


// ----- ENUMATORS
/**
 * @brief Measurement types.
 * 
 */
typedef enum SHT40_meas_t: uint8_t {
	TRH_H = SHT40_M_TRH_HP,
	TRH_M = SHT40_M_TRH_MP,
	TRH_L = SHT40_M_TRH_LP,
	TRH_H_H02W1S = SHT40_TRH_H_H02W1S,
	TRH_H_H02W01S = SHT40_TRH_H_H02W01S,
	TRH_H_H011W1S = SHT40_TRH_H_H011W1S,
	TRH_H_H011W01S = SHT40_TRH_H_H011W01S,
	TRH_H_H002W1S = SHT40_TRH_H_H002W1S,
	TRH_H_H002W01S = SHT40_TRH_H_H002W01S
};

/**
 * @brief Temperature units.
 * 
 */
typedef enum SHT40_unit_t: uint8_t {
	SHT40_UNIT_C,
	SHT40_UNIT_F
};


// ----- TYPEDEFS
/**
 * @brief Typedef for external I2C function.
 * 
 * @param addr An address of I2C slave device.
 * @param *data Pointer to data array.
 * @param len Lenth of array pointed with *data.
 * @return No return value.
 * 
 */
typedef void (*extI2C)(uint8_t addr, uint8_t* data, uint8_t len); 


// ----- CLASSES
/**
 * @brief SHT40 Class
 * 
 */
class SHT40 {
	// PUBLIC CLASS
	public:
	// CONSTUCTORS AND DECONSTRUCTORS DECLARATIONS
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
	 * @brief Object deconstructor.
	 * 
	 * @return No return value.
	 * 
	 */
	~SHT40(void);

	// METHODS DECLARATIONS
	/**
	 * @brief Measures temerature and RH.
	 * 
	 * @param type Type of measurement. See \ref SHT40_meas_t
	 * @return \c SHT40_NOK if NAK/NACK is received, SHT40 is busy.
	 * @return \c SHT40_OK if measurement was OK. 
	 */
	uint8_t measure(SHT40_meas_t type);

	/**
	 * @brief Calculates temperature from last measure call.
	 * 
	 * @param out Reference to output variable for temperature.
	 * @return \c SHT40_NOK if measurement data does not exist.
	 * @return \c SHT40_OK if temperature is calculated.
	 */
	uint8_t temperature(uint32_t& out);

	/**
	 * @brief Calculates RH from last measure call.
	 * 
	 * @param out Reference to output variable for relative humidity in %.
	 * @return \c SHT40_NOK if measurement data does not exist.
	 * @return \c SHT40_OK	if relative humidity is calculated.
	 */
	uint8_t rh(int16_t& out);

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
	 * @brief Initialize SHT40 object.
	 * 
	 * Check provided external I2C operations handlers and sets temperature unit
	 * 
	 * @param u Desired temperature unit. See \ref SHT40_unit_t
	 * @return \c SHT40_I2CH_R if I2C read handler is null.
	 * @return \c SHT40_I2CH_W if I2C write handler is null.
	 * @return \c SHT40_NOK if SN is 0x00000000 or 0xFFFFFFFF.
	 * @return \c SHT40_OK if everything is OK.
	 */
	uint8_t init(SHT40_unit_t u);

	/**
	 * @brief Clear measured data.
	 * 
	 * @return No return value.
	 */
	inline void clear(void);

	/**
	 * @brief Sets temperature unit.
	 * 
	 * @param u Desired temperature unit. See \ref SHT40_unit_t
	 * @return No return value.
	 * 
	 * @see SHT40_unit_t
	 */
	inline void setUnit(SHT40_unit_t u);

	/**
	 * @brief Get temperature unit.
	 * 
	 * @return Selected temperature unit. See \ref SHT40_unit_t
	 * @see SHT40_unit_t
	 */
	inline SHT40_unit_t getUnit(void) const;


	// PRIVATE STUFF
	private:
	// VARIABLES
	/**
	 * @brief Union for SHT40 data.
	 * 
	 * @note This union is aligned by one byte.
	 */
	union {
		uint8_t header; /**< @brief Header for NACK/NAK is sensor is busy. */

		/**
		 * @brief Struct for SHT40 serial number data.
		 * 
		 * @note This struct is aligned by one byte.
		 */
		struct {
			uint16_t sn1;
			uint8_t crc1;
			uint16_t sn2;
			uint8_t crc2;
		} __attribute__((packed, aligned(1))) snData;

		/**
		 * @brief Struct for SHT40 measurement data.
		 * 
		 * @note This struct is aligned by one byte.
		 */
		struct {
			uint16_t temp;
			uint8_t crc1;
			uint16_t rh;
			uint8_t crc2;
		} __attribute__((packed, aligned(1))) mData;
	};

	uint8_t address = SHT40_I2C_DEF_ADDR; /**< @brief I2C slave device address. */
	extI2C I2CRead = nullptr; /**< @brief Pointer to external I2C read function. */
	extI2C I2CWrite = nullptr; /**< @brief Pointer to external I2C write function. */
	SHT40_unit_t unit = SHT40_UNIT_C; /**< @brief Temperature unit variable. */
};

/**
 * @}
*/

#endif // _SHT40_H_

// END WITH NEW LINE
