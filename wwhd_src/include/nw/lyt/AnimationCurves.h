/* Animation key formats proven by the interpolation strides and field loads. */
#pragma once
#include "wwhd.h"
namespace nw { namespace lyt {
struct HermiteKey { be<f32> time, value, slope; };
struct DiscreteKey { be<f32> time; be<u16> value, padding; };
WWHD_SIZE(HermiteKey, 12);
WWHD_SIZE(DiscreteKey, 8);
f32 interpolateHermiteKeys(const HermiteKey* keys, u32 count, f32 time);
u32 sampleDiscreteKeys(const DiscreteKey* keys, u32 count, f32 time);
void initializeLayoutNumericLimits();
} }
