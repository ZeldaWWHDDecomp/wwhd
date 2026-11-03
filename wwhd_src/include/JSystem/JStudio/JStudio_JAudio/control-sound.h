// JStudio_JAudio factory layout, from the tww control.cpp reference and WWHD.
#pragma once
#include "JSystem/JStudio/jstudio-object.h"
namespace jst_adaptors { struct JAIBasic; struct JPAEmitterManager; }
namespace JStage { struct TSystem; }
namespace JStudio_JAudio {
struct TCreateObject : JStudio::TCreateObject {
    gptr<jst_adaptors::JAIBasic> mSound;
    bool create(gptr<JStudio::TObject>*, const JStudio::stb::data::TParse_TBlock&);
};
WWHD_SIZE(TCreateObject, 0x10);
WWHD_OFFSET(TCreateObject, mSound, 0xC);
}
