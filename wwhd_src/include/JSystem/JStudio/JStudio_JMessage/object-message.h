// JStudio message adaptor, reconstructed from WWHD with the tww object-message reference.
#pragma once
#include "JSystem/JStudio/jstudio-object.h"
#include "JSystem/JMessage/control.h"
namespace JStudio_JMessage {
struct TAdaptor_message : JStudio::TAdaptor {
    gptr<JMessage::TControl> mControl;
    enum Operation : u32 { OP_MESSAGE = 0x19 };
    void adaptor_do_MESSAGE(u32, const void*, u32);
};
WWHD_SIZE(TAdaptor_message, 0x10);
WWHD_OFFSET(TAdaptor_message, mControl, 0xC);
}
