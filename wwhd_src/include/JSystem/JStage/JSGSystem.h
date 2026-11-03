#pragma once
#include "JSystem/JStage/JSGObject.h"
namespace JStage {
struct TSystem : TObject { s32 JSGFGetType() const; void destroy(s32); };
WWHD_SIZE(TSystem,4);
}
