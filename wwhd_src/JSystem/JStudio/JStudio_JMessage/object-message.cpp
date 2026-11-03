// JStudio message adaptor lifecycle and message-code operation; tww object-message.cpp reference.
#include "JSystem/JStudio/JStudio_JMessage/object-message.h"
using namespace JStudio_JMessage;
void* JStudioMessage_allocate(u32);
void JStudioMessage_delete(void*);
void JStudioMessage_destroyBase(JStudio::TAdaptor*, u32);
void JStudioMessage_flush(JMessage::TControl*);
void JStudioMessage_registerGlobal(void*);
namespace {
static u32 sMessageAdaptorVTable;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}
// HD: explicit constructor entry retains GHS allocation and its control parameter.
/* 0283E514 */
TAdaptor_message* TAdaptor_message_construct(TAdaptor_message* adaptor, JMessage::TControl* control) {
    if (!adaptor) adaptor = static_cast<TAdaptor_message*>(JStudioMessage_allocate(sizeof(TAdaptor_message)));
    if (adaptor) {
        adaptor->mControl = control;
        adaptor->mCount = 0;
        adaptor->__vtbl = reinterpret_cast<jst_timeline::AdaptorVTable*>(&sMessageAdaptorVTable);
        adaptor->mVariableValues = nullptr;
    }
    return adaptor;
}
/* 0283E570 */
void TAdaptor_message_destroy(TAdaptor_message* adaptor, u32 flags) {
    if (!adaptor) return;
    JStudioMessage_destroyBase(adaptor, 0);
    if (flags & 1) JStudioMessage_delete(adaptor);
}
/* 0283E5C4 */
void TAdaptor_message::adaptor_do_MESSAGE(u32 operation, const void* data, u32 size) {
    if (operation != OP_MESSAGE) return;
    u32 code = *static_cast<const be<u32>*>(data);
    JMessage::TControl* control = mControl;
    control->mMessageIndex = code;
    control->mGroupID = code >> 16;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioMessage_flush)), control);
}
/* 0283E5E4 */
void initializeMessageAdaptor() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    gabi::tail_ptr<void>(gabi::guest_fn(reinterpret_cast<const void*>(&JStudioMessage_registerGlobal)), &sStaticRegister);
}
/* WWHD glue
data sMessageAdaptorVTable 101733DC
data sStaticBlock 104B6154
data sStaticRegister 101FAE10
ext 0273AD10 void* JStudioMessage_allocate(u32)
ext 0273AF40 void JStudioMessage_delete(void*)
ext 0283B640 void JStudioMessage_destroyBase(JStudio::TAdaptor*, u32)
ext 02842858 void JStudioMessage_flush(JMessage::TControl*) callback
ext 028F026C void JStudioMessage_registerGlobal(void*) callback
*/
