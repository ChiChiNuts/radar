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

#define AS5600_ADDR 0x36

/* Definitions of AS5600 internal register  */
/* Configuration Registers */
#define ZMCO 0x00
#define ZPOS_L 0x01
#define ZPOS_H 0x02
#define MPOS_L 0x03
#define MPOS_H 0x04
#define MANG_L 0x05
#define MANG_H 0x06
#define CONF_L 0x07
#define CONF_H 0x08
/* Output Registers */
#define RAW_ANGLE_L 0x0c
#define RAW_ANGLE_H 0x0d
#define ANGLE_H 0x0f
#define ANGLE_L 0x0e
/* Status Registers */
#define STATUS 0x0b
#define AGC 0x1a
#define MAGNITUDE_H 0x1b
#define MAGNITUDE_L 0x1c
/* Burn Commands */
#define BURN 0xff

/*
 *  @brief Writing a byte to the memory of a IIC device.
 *  @reg The destination memory address.
 *  @date The data to be written.
 */
static void WriteByte(uint8_t reg, uint8_t data)
{
    uint8_t data_write[2];
    data_write[0] = reg & 0xff;
    data_write[1] = data & 0xff;
    while (HAL_I2C_Master_Transmit(&hi2c1, VL6180x_ADDR, (uint8_t*) data_write,
                                   2, 1000) != HAL_OK);
    if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_NONE) {
        log_e("i2c tx error!");
    }
}

/*
 * @brief Reading a byte from the memory of a IIC device.
 * @reg The destination memory address to be read.
 * @return The value aimed to be read.
 */
static uint8_t ReadByte(uint8_t reg)
{
    uint8_t receive_data = 0;

    if (HAL_I2C_Mem_Read(&hi2c1, VL6180x_ADDR, reg, I2C_MEMADD_SIZE_8BIT,
                         (uint8_t*) &receive_data, 1, 1000) != HAL_OK) {
        log_e("i2c rx error!");
    }

    return receive_data;
}
