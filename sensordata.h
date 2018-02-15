#ifndef SENSORDATA_H
#define SENSORDATA_H


struct SensorData
{
    float laser1_d;
    float laser2_d;
    float laser3_d;
    float ultrasound_d;
};

static_assert(sizeof(SensorData) == 4 * 4, "sizeof float must be 4, no padding");
#endif // SENSORDATA_H
