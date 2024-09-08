/*
 * as5600.c
 *
 *  Created on: Sep 6, 2024
 *      Author: wormarz
 */
#include "as5600.h"
#include "misc.h"
#include "elog.h"
#include "gpio.h"
#include "i2c.h"

#undef LOG_TAG
#define LOG_TAG    "Potentiometer"

#define AS5600_ADDR 0x6c

/* Definitions of AS5600 internal register  */
/* Configuration Registers */
#define ZMCO 0x00
#define ZPOS_H 0x01
#define ZPOS_L 0x02
#define MPOS_H 0x03
#define MPOS_L 0x04
#define MANG_H 0x05
#define MANG_L 0x06
#define CONF_H 0x07
#define CONF_L 0x08
/* Output Registers */
#define RAW_ANGLE_H 0x0c
#define RAW_ANGLE_L 0x0d
#define ANGLE_H 0x0e
#define ANGLE_L 0x0f
/* Status Registers */
#define STATUS 0x0b
#define AGC 0x1a
#define MAGNITUDE_H 0x1b
#define MAGNITUDE_L 0x1c
/* Burn Commands */
#define BURN 0xff

/*
 *  @brief Writing to the register on a IIC device.
 *  @reg The destination memory address.
 *  @date The data to be written.
 *  @dbyte Writing double bytes. It must be 0 or 1.
 */
static void write_reg(uint8_t reg, uint16_t data, uint8_t dbyte)
{
    uint8_t data_write[3];
    data_write[0] = reg & 0xff;
    data_write[1] = dbyte ? ((data >> 8) & 0xff) : (data & 0xff);
    data_write[2] = dbyte ? (data & 0xff) : 0x00;

    while (HAL_I2C_Master_Transmit(&hi2c1, AS5600_ADDR, (uint8_t*) data_write,
                                   2 + dbyte, 1000) != HAL_OK);
    if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_NONE) {
        log_e("i2c tx error!");
    }
}

/*
 * @brief Reading from the memory on a IIC device.
 * @reg The destination memory address to be read.
 * @dbyte Reading double bytes. It must be 0 or 1.
 * @return The value aimed to be read.
 */
static uint16_t read_reg(uint8_t reg, uint8_t dbyte)
{
    uint8_t data_read[2] = {0};

    if (HAL_I2C_Mem_Read(&hi2c1, AS5600_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                         (uint8_t*) data_read, 1 + dbyte, 1000) != HAL_OK) {
        log_e("i2c rx error!");
    }

    return (dbyte ? ((data_read[0] << 8) & 0xff00) : (data_read[0] & 0xff)) |
            (0xff & data_read[1]);
}

int as5600_init(void)
{
    return 0;
}

uint8_t get_as5600_status(void)
{
    return read_reg(STATUS, 0);
}

uint16_t get_as5600_angle(uint8_t is_raw)
{
    return read_reg(is_raw ? RAW_ANGLE_H : ANGLE_H, 1);
}

void as5600_dump(void)
{
    for (int i = 0; i< 9; ++i) {
        log_i("as5600 addr[0x%02x]: 0x%02x", i, read_reg(i, 0));
    }

    for (int i = 0x0b; i < 0x10; ++i) {
        log_i("as5600 addr[0x%02x]: 0x%02x", i, read_reg(i, 0));
    }

    for (int i = 0x1a; i < 0x1d; ++i) {
        log_i("as5600 addr[0x%02x]: 0x%02x", i, read_reg(i, 0));
    }
}
