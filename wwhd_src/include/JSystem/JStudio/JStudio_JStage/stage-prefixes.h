// External stage object prefixes and type IDs, checked against WWHD virtual calls.
#pragma once
#include "wwhd.h"
namespace JStudio_JStage {
struct StageObjectVTable { u8 prefix[0x14]; gfn getType; };
struct StageObject { gptr<StageObjectVTable> __vtbl; };
struct StageSystemVTable { u8 prefix[0x6C]; gfn findObject; };
struct StageSystem { gptr<StageSystemVTable> __vtbl; };
enum ObjectType : u32 { OBJECT_ACTOR=2, OBJECT_CAMERA=3, OBJECT_AMBIENT=4, OBJECT_LIGHT=5, OBJECT_FOG=6 };
}
