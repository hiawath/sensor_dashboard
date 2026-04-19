#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include <QString>

typedef enum {
    ID_SYS_HEARTBEAT    = 0,
    ID_SYS_UPTIME       = 1,
    ID_SYS_TEMP         = 2,
    ID_SYS_VREF         = 3,
    ID_ENV_TEMP         = 10,
    ID_ENV_HUMI         = 11,
    ID_ENV_PRESS        = 12,
    ID_ENV_LIGHT        = 13,
    ID_IN_BUTTON_1      = 30,
    ID_IN_BUTTON_2      = 31,
    ID_IN_SW_DIP        = 32,
    ID_IN_ENC_POS       = 33,
    ID_OUT_LED_STATE    = 50,
    ID_OUT_MOTOR_SPEED  = 51,
    ID_OUT_RELAY        = 52,
    ID_IMU_ACCEL_X      = 70,
    ID_IMU_ACCEL_Y      = 71,
    ID_IMU_ACCEL_Z      = 72,
    ID_IMU_GYRO_X       = 73,
    ID_ALARM_CRITICAL   = 100,
    ID_ALARM_WARN       = 101
} SensorID;

typedef enum {
    TYPE_UINT8   = 0,
    TYPE_INT32   = 1,
    TYPE_FLOAT   = 2,
    TYPE_BOOL    = 3,
    TYPE_STRING  = 4
} DataType;

struct SensorData {
    SensorID id;
    DataType type;
    QString value;
};

#endif // SERIAL_PROTOCOL_H
