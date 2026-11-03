// JMessage default control and Cafe message-index forwarding.
#include "JSystem/JMessage/control.h"
void* JMessage_allocate(u32);
void JMessage_delete(void*);
void JMessage_register(void*);
namespace {
struct VtableStorage { u8 bytes[0x18]; };
static const VtableStorage sControlVtable;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
using namespace JMessage;
/* 028427B8 */
TControl* TControl::construct(TControl* control) {
    if (!control) control=static_cast<TControl*>(JMessage_allocate(sizeof(*control)));
    if (control) {
        control->mGroupID=0; control->__vtbl=gabi::ea(&sControlVtable);
        control->mBaseProcRender=nullptr; control->mMessageDataCurrent=nullptr;
        control->mRenderStack.depth=0; control->mResourceContainer=nullptr; control->mCurrentText=nullptr;
        control->mMessageDataStart=nullptr; control->mResource=nullptr; control->mBaseProcSeq=nullptr;
        control->mMessageEntry=nullptr; control->mMessageIndex=0;
    }
    return control;
}
/* 02842828 */
void TControl::destroy(s32 flags) { if (this && (flags&1)) JMessage_delete(this); }
/* 0284283C */
void TControl::reset_() {
    mRenderStack.depth=0; mCurrentText=nullptr; mMessageEntry=nullptr;
    mMessageDataCurrent=nullptr; mMessageDataStart=nullptr;
}
/* 02842858 */
bool TControl::setMessageCode_flush_() {
    reset_();
    // HD: this is a message-token adapter, so no JMessage resource lookup is performed.
    mMessageEntry=&mMessageIndex;
    return true;
}
/* 02842884 */
static void __sinit_JMessage_control_cpp() {
    sStaticBlock.words[2]=0; sStaticBlock.words[0]=0; sStaticBlock.words[3]=0; sStaticBlock.words[1]=0;
    JMessage_register(&sStaticRegister);
}
/* WWHD glue:
data sControlVtable 10173A64
data sStaticBlock 104B64A4
data sStaticRegister 101FAF30
ext 0273AD10 void* JMessage_allocate(u32)
ext 0273AF40 void JMessage_delete(void*)
ext 028F026C void JMessage_register(void*)
*/
