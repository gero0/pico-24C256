#include "eeprom_24C256.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include <cstdint>
#include <cstring>

const size_t EEPROM_PAGE_SIZE = 64;
const size_t EEPROM_PAGE_COUNT = 512;

void wait_for_eeprom_ready()
{
    // not sure if ACK polling is even posiible with pico sdk,
    // so we'll just wait a magic amout of 10ms
    sleep_ms(10);
}

EEPROM_24C256::EEPROM_24C256(i2c_inst_t* i2c, uint8_t device_address, bool auto_wait)
    : i2c(i2c)
    , device_address(device_address)
    , auto_wait(auto_wait)
{
}

void EEPROM_24C256::write_byte(uint16_t address, uint8_t byte)
{
    uint8_t first = (address >> 8) & 0xFF;
    uint8_t second = address & 0xFF;

    uint8_t frame[] = { first, second, byte };
    int a = i2c_write_blocking(i2c, device_address, frame, 3, false);
    if (auto_wait) {
        wait_for_eeprom_ready();
    }
}

void EEPROM_24C256::write_page(uint16_t address, uint8_t* data)
{
    uint8_t first = (address >> 8) & 0xFF;
    uint8_t second = address & 0xFF;

    uint8_t buf[EEPROM_PAGE_SIZE + 2];
    memcpy(buf + 2, data, EEPROM_PAGE_SIZE);
    buf[0] = first;
    buf[1] = second;
    int res = i2c_write_blocking(i2c, device_address, buf, EEPROM_PAGE_SIZE + 2, false);
    if (auto_wait) {
        wait_for_eeprom_ready();
    }
}

uint8_t EEPROM_24C256::read_byte(uint16_t address)
{
    uint8_t first = (address >> 8) & 0xFF;
    uint8_t second = address & 0xFF;

    uint8_t addr[] = { first, second };

    int a = i2c_write_blocking(i2c, device_address, addr, 2, true);
    uint8_t buf;
    i2c_read_blocking(i2c, device_address, &buf, 1, false);
    return buf;
}

uint8_t EEPROM_24C256::read_page(uint16_t address, uint8_t* buffer)
{
    uint8_t first = (address >> 8) & 0xFF;
    uint8_t second = address & 0xFF;

    uint8_t addr[] = { first, second };

    int a = i2c_write_blocking(i2c, device_address, addr, 2, true);
    return i2c_read_blocking(i2c, device_address, buffer, EEPROM_PAGE_SIZE, false);
}

void EEPROM_24C256::dump(uint8_t* buffer)
{
    for (int i = 0; i < EEPROM_PAGE_COUNT * EEPROM_PAGE_SIZE; i += EEPROM_PAGE_SIZE) {
        read_page(i, &buffer[i]);
    }
}

void EEPROM_24C256::write_array(uint16_t address, uint8_t* data, size_t length)
{
    for (int i = 0; i < length; i += EEPROM_PAGE_SIZE) {
        if (length - i < EEPROM_PAGE_SIZE) {
            //Remaining data length is less than length of one page - write byte by byte
            for (int j = 0; j < (length - i); j++) {
                write_byte(address + i + j, data[i+ j]);
            }
        } else {
            write_page(address + i, &data[i]);
        }
    }
}
