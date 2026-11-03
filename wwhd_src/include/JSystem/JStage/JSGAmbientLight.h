#pragma once
#include "JSystem/JStage/JSGObject.h"
namespace JStage {
struct TAmbientLight : TObject { s32 JSGFGetType() const; u32 JSGGetColor() const; void destroy(s32); };
WWHD_SIZE(TAmbientLight,4);
}

// Shared lazy color cache used by the JStage default color getters.
extern be<u32>& sJStageColorInitialized;
extern be<u32>& sJStageDefaultColor;
