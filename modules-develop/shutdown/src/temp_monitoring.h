/*
 * Temperature Monitoring thread
 * Caltech Racing 2016
 */
#ifndef TEMP_MONITORING_H
#define TEMP_MONITORING_H

#include <common.h> // StatusInt
#include <i2c.h>

struct tempSensor {
    uint8_t i2c;      // 0-2
    uint8_t mux;      // 0-7
    uint8_t address;  // 0-26
};

static const struct tempSensor temp_sensors[] = {
    {I2C_1, 0, 0},
    {I2C_1, 0, 1},
    {I2C_1, 0, 2},
    {I2C_1, 0, 3},
    {I2C_1, 0, 4},
    {I2C_1, 0, 5},
    {I2C_1, 0, 6},
    {I2C_1, 0, 7},
    {I2C_1, 0, 8},
    {I2C_1, 0, 9},
    {I2C_1, 0, 10},
    {I2C_1, 0, 11},
    {I2C_1, 0, 12},
    {I2C_1, 0, 13},
    {I2C_1, 0, 14},
    {I2C_1, 0, 15},
    {I2C_1, 0, 16},
    {I2C_1, 0, 17},
    {I2C_1, 0, 18},
    {I2C_1, 0, 19},
    {I2C_1, 0, 20},
    {I2C_1, 0, 21},
    {I2C_1, 0, 22},
    {I2C_1, 0, 23},
    {I2C_1, 0, 24},
    {I2C_1, 0, 25},
    {I2C_1, 0, 26}
};

#define DATA_READ_PER_OUT 10 // (# of temp reads / # of temps sent over CAN)

/* Temperature thresholds */
#define TEMP_LIMP_THRESHOLD  40.0f
#define TEMP_FAULT_THRESHOLD 60.0f

StatusInt Init_TempMonitor( void );

#endif // TEMP_MONITORING_H
