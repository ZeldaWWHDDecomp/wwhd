// WWHD tablet input conversion; sample coordinates use the platform calibration boundary.
#pragma once
#include "sead/DeviceConstruction.h"
#include "sead/RuntimeType.h"
struct TabletCoordinates { be<u16> mX; be<u16> mY; };
WWHD_SIZE(TabletCoordinates,4);
