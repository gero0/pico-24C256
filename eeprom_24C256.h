#ifndef __EEPROM_24C256__
#define __EEPROM_24C256__

#include <cstdint>
#include <hardware/i2c.h>

class EEPROM_24C256 {
private:
    i2c_inst_t* i2c;
    uint8_t device_address;
    bool auto_wait;

public:
    /**
    * @brief Simple 24C256 i2c EEPROM driver for Raspberry Pi Pico!
    * To use only with 24C256 256kbit EEPROM!
    * 
    * @param i2c pointer to i2c peripheral to use. The i2c peripheral is assumed to be initialized.
    * @param device_address right-aligned 7-bit device address
    * @param auto_wait If set to true, a delay will be added after every write operation.
    * This is done to make sure eeprom has time to finish writing data before the next operation.
    * Delay may be replaced by ACK polling in the future.
    * Disable if you want to add delay/ack polling after write manually.
    */
    EEPROM_24C256(i2c_inst_t* i2c, uint8_t device_address, bool auto_wait = true);

    /**
     * @brief Write a byte at given address
     * 
     * @param address 16-bit memory address
     * @param byte value to write
     */
    void write_byte(uint16_t address, uint8_t byte);

    /**
     * @brief Write a page at given address
     * 
     * @param address 16-bit memory address
     * @param data Pointer to buffer containing the data. Must be 64 bytes long.
     */
    void write_page(uint16_t address, uint8_t* data);

    /**
     * @brief Read a byte from given address
     * 
     * @param address 16-bit memory address
     * @returns Byte read.
     */
    uint8_t read_byte(uint16_t address);

    /**
     * @brief Read a 64-byte page from given address
     * 
     * @param address 16-bit memory address
     * @param buffer pointer tu buffer to write data into
     * @returns Number of bytes read, or PICO_ERROR_GENERIC if address not acknowledged or no device present.
     */
    uint8_t read_page(uint16_t address, uint8_t* buffer);

    /**
     * @brief Dumps the entire content of the EEPROM into 32768 byte long buffer
     * 
     * @param buffer Pointer to a buffer to write data into. Must be able to hold 32768 bytes.
     */
    void dump(uint8_t* buffer);

    /**
     * @brief Write arbitrary size array into eeprom.
     *        It is your responsibility to make sure data fits in the memory.
     * @param address adress to start writing to
     * @param data pointer to data
     * @param length length of data array.
     */
    void write_array(uint16_t address, uint8_t* data, size_t length);
};

#endif