/**
 * @file SHT40.h
 * @author silvio3105 (www.github.com/silvio3105)
 * @brief SHT40 driver header file.
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
#define SHT40_NOK				(0x00) /**< @brief Return value representing negative OK status. */
#define SHT40_OK				(0x01) /**< @brief Return value representing OK status. */
#define SHT40_I2CH_R			(0x02) /**< @brief Return value representing missing read I2C handler. */
#define SHT40_I2CH_W			(0x03) /**< @brief Return value representing missing write I2C handler. */
#define SHT40_OLD_DATA			(0x04) /**< @brief Return code returned when old data are used for temperature or humidity calculations. */
#define SHT40_NAK				(0x15) /**< @brief NAK symbol. */

// I2C ADDESSES
#define I2C_ADDR_SHT40_AD		(0x44) /**< @brief I2C address for SHT40-AD. */
#define I2C_ADDR_SHT40_B		(0x45) /**< @brief I2C address for SHT40-B. */

// CONFIG
#define SHT40_I2C_DEF_ADDR      I2C_ADDR_SHT40_AD /**< @brief Default SHT40 I2C address. */

// SHT40 COMMANDS
#define SHT40_SN				(0x89) /**< @brief Command for Serial number. */
#define SHT40_RST				(0x94) /**< @brief Command for Soft reset. */

// BITFIELDS
#define SHT40_UNIT_POS			0
#define SHT40_UNIT_MASK			0b00000001
#define SHT40_UNIT				(1 << SHT40_UNIT_POS)

#define SHT40_OLD_TEMP_POS		1
#define SHT40_OLD_TEMP_MASK		0b00000010
#define SHT40_OLD_TEMP			(1 << SHT40_OLD_TEMP_POS)

#define SHT40_OLD_RH_POS		2
#define SHT40_OLD_RH_MASK		0b00000100
#define SHT40_OLD_RH			(1 << SHT40_OLD_RH_POS)


// ----- ENUMATORS
/**
 * @brief Measurement types.
 * 
 */
enum SHT40_meas_t: uint8_t {
	TRH_H = 0xFD, /**< @brief Command for High precision temperature & RH measurement without heater. */
	TRH_M = 0xF6, /**< @brief Command for Medium precision temperature & RH measurement without heater. */
	TRH_L = 0xE0, /**< @brief Command for Low precision temperature & RH measurement without heater. */
	TRH_H_H02W1S = 0x39, /**< @brief Command for Serial number. */
	TRH_H_H02W01S = 0x32, /**< @brief Command for High precision temperature & RH measurement with 0.2W heater 1s. */
	TRH_H_H011W1S = 0x2F, /**< @brief Command for High precision temperature & RH measurement with 0.1W heater 1s. */
	TRH_H_H011W01S = 0x24, /**< @brief Command for High precision temperature & RH measurement with 0.1W heater 0.1s. */
	TRH_H_H002W1S = 0x1E, /**< @brief Command for High precision temperature & RH measurement with 0.02W heater 1s. */ 
	TRH_H_H002W01S = 0x15 /**< @brief Command for High precision temperature & RH measurement with 0.02W heater 0.1s. */ 
};

/**
 * @brief Temperature units.
 * 
 */
enum SHT40_unit_t: uint8_t {
	SHT40_UNIT_C = (0 << SHT40_UNIT_POS), /**< @brief Celsius temperature unit. */
	SHT40_UNIT_F = (1 << SHT40_UNIT_POS) /**< @brief Fahrenheit temperature unit. */
};


// ----- TYPEDEFS
/**
 * @brief Typedef for external I2C function.
 * 
 * @param addr An address of I2C slave device.
 * @param *data Pointer to data array.
 * @param len Lenth of array pointed with *data.
 * @param delay Delay in ms after I2C call.
 * @return No return value.
 * 
 */
typedef void (*extI2C)(uint8_t addr, uint8_t* data, uint8_t len, uint16_t delay); 


// ----- CLASSES
/**
 * @brief SHT40 sensor class.
 * 
 */
class SHT40 {
	// PUBLIC CLASS
	public:
	// CONSTUCTOR AND DECONSTRUCTOR DECLARATIONS
	/**
	 * @brief SHT40 object constructor.
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
	 * @brief SHT40 object deconstructor.
	 * 
	 * @return No return value.
	 * 
	 */
	~SHT40(void);

	// METHODS DECLARATIONS
	/**
	 * @brief Measure temerature and RH.
	 * 
	 * @param type Type of measurement. See \ref SHT40_meas_t
	 * @return \c SHT40_NOK if NAK/NACK is received, SHT40 is busy.
	 * @return \c SHT40_OK if measurement was OK. 
	 */
	uint8_t measure(SHT40_meas_t type);

	/**
	 * @brief Calculate temperature using data from \ref measure method.
	 * 
	 * @param out Reference to output variable for temperature.
	 * @return \c SHT40_NOK if measurement data does not exist.
	 * @return \c SHT40_OK if temperature is calculated.
	 */
	uint8_t temperature(int8_t& out);

	/**
	 * @brief Calculate RH using data from \ref measure method.
	 * 
	 * @param out Reference to output variable for relative humidity in %(0-100%).
	 * @return \c SHT40_NOK if measurement data does not exist.
	 * @return \c SHT40_OK	if relative humidity is calculated.
	 */
	uint8_t rh(uint8_t& out);

	/**
	 * @brief Read unique serial number.
	 * 
	 * Values \c 0 and \c 0xFFFFFFFF likely means sensor is not working properly.
	 * 
	 * @return Chip's unique serial number.
	 */
	uint32_t whoAmI(void);	

	/**
	 * @brief Reset the chip.
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
	 * @brief Set temperature unit.
	 * 
	 * @param u Desired temperature unit. See \ref SHT40_unit_t
	 * @return No return value.
	 * 
	 * @see SHT40_unit_t
	 */
	void setUnit(SHT40_unit_t unit);

	/**
	 * @brief Get temperature unit.
	 * 
	 * @return Selected temperature unit. See \ref SHT40_unit_t
	 * @see SHT40_unit_t
	 */
	SHT40_unit_t getUnit(void) const;

	/**
	 * @brief Clear measured data.
	 * 
	 * @return No return value.
	 */
	void clear(void);


	// PRIVATE STUFF
	private:
	// VARIABLES
	/**
	 * @brief Union of SHT40 data.
	 * 
	 */
	union {
		uint8_t header; /**< @brief Header for NACK/NAK is sensor is busy. */

		/**
		 * @brief Struct for SHT40 serial number data.
		 * 
		 * @note This struct is aligned by one byte.
		 */
		struct {
			uint16_t sn2;
			uint8_t crc2;
			uint16_t sn1;
			uint8_t crc1;
		} __attribute__((packed, aligned(1))) snData;

		/**
		 * @brief Struct for SHT40 measurement data.
		 * 
		 * @note This struct is aligned by one byte.
		 */
		struct {
			uint8_t temp[2];
			uint8_t crc2;
			uint8_t rh[2];
			uint8_t crc1;
		} __attribute__((packed, aligned(1))) mData;
	};


	uint8_t address = SHT40_I2C_DEF_ADDR; /**< @brief I2C slave device address. */
	extI2C I2CRead = nullptr; /**< @brief Pointer to external I2C read function. */
	extI2C I2CWrite = nullptr; /**< @brief Pointer to external I2C write function. */

	/**
	 * @brief Control register.
	 * 
	 * Bit 0: Temperature unit. See \ref SHT40_unit_t
	 * Bit 1: Old temperature data.
	 * Bit 2: Old RH data.
	 */
	uint8_t controlReg = 0;
};

/**
 * @}
*/

#endif // _SHT40_H_

// END WITH NEW LINE
