#pragma once
#include "JSystem/JStage/JSGObject.h"
namespace JStage {
struct TLight : TObject { s32 JSGFGetType() const; void JSGGetDirection(void*) const; void JSGSetDirection(const void*); u32 JSGGetColor() const; void destroy(s32); };
WWHD_SIZE(TLight,4);
}
