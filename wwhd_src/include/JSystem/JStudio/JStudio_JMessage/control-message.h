// Message factory extensions, following tww JStudio_JMessage/control.cpp.
#pragma once
#include "JSystem/JStudio/JStudio_JMessage/object-message.h"
namespace JStudio_JMessage {
struct TCreateObject_base : JStudio::TCreateObject {
    bool create(gptr<JStudio::TObject>*, const JStudio::stb::data::TParse_TBlock&);
};
struct TCreateObject : TCreateObject_base {
    gptr<JMessage::TControl> mControl;
    JMessage::TControl* find(const JStudio::stb::data::TParse_TBlock&);
};
struct CreateVTable { u8 prefix[0x1C]; gfn find; };
WWHD_SIZE(TCreateObject_base, 0xC);
WWHD_SIZE(TCreateObject, 0x10);
WWHD_OFFSET(TCreateObject, mControl, 0xC);
WWHD_OFFSET(CreateVTable, find, 0x1C);
}
