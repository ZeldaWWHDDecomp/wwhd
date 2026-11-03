#pragma once
#include "JSystem/JStage/JSGObject.h"
namespace JStage {
struct TFog : TObject { s32 JSGFGetType() const; f32 JSGGetStartZ() const; f32 JSGGetEndZ() const; u32 JSGGetColor() const; void destroy(s32); };
WWHD_SIZE(TFog,4);
}
