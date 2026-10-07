// Unit-local HD field layout.
#pragma once
#include "wwhd.h"
struct JPAFieldData_a_l {
    gptr<void> baseField;
    u8 link[0x10];
    be<f32> velocity[3];
    be<f32> work0[3];
    be<f32> work1[3];
    be<f32> work2[3];
    be<f32> maxDistanceSq, fadeOutRate, fadeInRate;
    be<f32> position[3], direction[3];
    be<f32> magnitude, magnitudeRandom, maxDistance;
    be<f32> value1, value2, value3;
    be<f32> fadeIn, fadeOut, enableTime, disableTime;
    be<u16> flags;
    u8 type, id, velocityType, cycle;
};
WWHD_OFFSET(JPAFieldData_a_l, velocity, 0x14);
WWHD_OFFSET(JPAFieldData_a_l, work0, 0x20);
WWHD_OFFSET(JPAFieldData_a_l, work1, 0x2C);
WWHD_OFFSET(JPAFieldData_a_l, work2, 0x38);
WWHD_OFFSET(JPAFieldData_a_l, maxDistanceSq, 0x44);
WWHD_OFFSET(JPAFieldData_a_l, position, 0x50);
WWHD_OFFSET(JPAFieldData_a_l, direction, 0x5C);
WWHD_OFFSET(JPAFieldData_a_l, magnitude, 0x68);
WWHD_OFFSET(JPAFieldData_a_l, value2, 0x78);
WWHD_OFFSET(JPAFieldData_a_l, flags, 0x90);
WWHD_OFFSET(JPAFieldData_a_l, velocityType, 0x94);
