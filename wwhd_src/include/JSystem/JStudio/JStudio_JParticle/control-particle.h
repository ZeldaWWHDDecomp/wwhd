// JStudio_JParticle factory layout, from the tww control.cpp reference and WWHD.
#pragma once
#include "JSystem/JStudio/jstudio-object.h"
namespace jst_adaptors { struct JAIBasic; struct JPAEmitterManager; }
namespace JStage { struct TSystem; }
namespace JStudio_JParticle {
struct TCreateObject : JStudio::TCreateObject {
    gptr<jst_adaptors::JPAEmitterManager> mEmitterMgr;
    gptr<const JStage::TSystem> mSystem;
    bool create(gptr<JStudio::TObject>*, const JStudio::stb::data::TParse_TBlock&);
};
WWHD_SIZE(TCreateObject, 0x14);
WWHD_OFFSET(TCreateObject, mEmitterMgr, 0xC);
}
