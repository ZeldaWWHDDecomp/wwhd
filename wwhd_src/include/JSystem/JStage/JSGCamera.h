#pragma once
#include "JSystem/JStage/JSGObject.h"
namespace JStage {
struct TCamera : TObject { s32 JSGFGetType() const; void destroy(s32); };
WWHD_SIZE(TCamera,4);
}
