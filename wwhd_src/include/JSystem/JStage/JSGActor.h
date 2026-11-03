#pragma once
#include "JSystem/JStage/JSGObject.h"
namespace JStage {
struct TActor : TObject { s32 JSGFGetType() const; f32 JSGGetAnimationFrame() const; f32 JSGGetAnimationFrameMax() const; f32 JSGGetAnimationTransition() const; void destroy(s32); };
WWHD_SIZE(TActor,4);
}
