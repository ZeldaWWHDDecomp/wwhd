// JMessage control, checked against Cafe constructor and message-code forwarding.
// HD: the flush implementation exposes the message index directly instead of resolving a resource entry.
#pragma once
#include "wwhd.h"
namespace JMessage {
struct TResourceContainer;
struct TResource;
struct TSequenceProcessor;
struct TRenderingProcessor;
struct TControl {
    gptr<TResourceContainer> mResourceContainer;
    gptr<TResource> mResource;
    gptr<TSequenceProcessor> mBaseProcSeq;
    gptr<TRenderingProcessor> mBaseProcRender;
    be<u16> mGroupID, mMessageIndex;
    gptr<const void> mMessageEntry;
    gptr<const char> mMessageDataStart, mMessageDataCurrent, mCurrentText;
    struct TStack_ { gptr<const void> stack[4]; be<u32> depth; } mRenderStack;
    be<u32> __vtbl;  /* GHS vtable at the tail; be<u32> like the other merged vtable words */
    static TControl* construct(TControl*);
    void destroy(s32);
    void reset_();
    bool setMessageCode_flush_();
};
WWHD_SIZE(TControl,0x3C);
WWHD_OFFSET(TControl,mResourceContainer,0); WWHD_OFFSET(TControl,mResource,4);
WWHD_OFFSET(TControl,mBaseProcSeq,8); WWHD_OFFSET(TControl,mBaseProcRender,0xC);
WWHD_OFFSET(TControl,mGroupID,0x10); WWHD_OFFSET(TControl,mMessageIndex,0x12);
WWHD_OFFSET(TControl,mMessageEntry,0x14); WWHD_OFFSET(TControl,mMessageDataStart,0x18);
WWHD_OFFSET(TControl,mMessageDataCurrent,0x1C); WWHD_OFFSET(TControl,mCurrentText,0x20);
WWHD_OFFSET(TControl,mRenderStack,0x24); WWHD_OFFSET(TControl::TStack_,depth,0x10);
WWHD_OFFSET(TControl,__vtbl,0x38);
}
