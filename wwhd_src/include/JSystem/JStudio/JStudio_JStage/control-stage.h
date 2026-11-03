// Stage factory and external object prefixes, following the tww JStage adaptor control.
#pragma once
#include "JSystem/JStudio/jstudio-object.h"
#include "JSystem/JStudio/JStudio_JStage/stage-prefixes.h"
namespace JStudio_JStage {
struct CreatorVTable { u8 prefix[0x1C]; gfn find; };
struct TCreateObject : JStudio::TCreateObject {
    gptr<const StageSystem> mSystem;
    bool create(gptr<JStudio::TObject>*, const JStudio::stb::data::TParse_TBlock&);
    StageObject* find(const JStudio::stb::data::TParse_TBlock&, u32);
};
enum BlockType : u32 { BLOCK_ACTOR=0x4A414354, BLOCK_CAMERA=0x4A434D52, BLOCK_AMBIENT=0x4A41424C, BLOCK_LIGHT=0x4A4C4954, BLOCK_FOG=0x4A464F47 };
WWHD_SIZE(TCreateObject, 0x10);
WWHD_OFFSET(TCreateObject, mSystem, 0xC);
WWHD_OFFSET(CreatorVTable, find, 0x1C);
WWHD_OFFSET(StageSystemVTable, findObject, 0x6C);
WWHD_OFFSET(StageObjectVTable, getType, 0x14);
WWHD_SIZE(StageObject, 4);
WWHD_SIZE(StageSystem, 4);
}
