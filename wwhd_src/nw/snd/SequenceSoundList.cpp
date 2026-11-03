/* Sequence sound list transitions, reconstructed from WWHD. */
#include "nw/snd/SequenceManager.h"
using namespace nw::snd;
void finalizeSequenceBasicSound(BasicSound* sound);
void removeSequenceSoundNode(SequenceList* list,SequenceListNode* node);
void insertSequenceSoundNode(SequenceList* list,gptr<SequenceListNode>* position,SequenceListNode* node);
/* Inlined priority clamp; the priority byte is read unsigned by these two paths. */
static s32 sequencePriority(const BasicSound& sound) {
    s32 value=s32(u32(sound.ambient.priority)+u32(u8(static_cast<s8>(sound.playerPriority))));
    if (value>127) return 127;
    return value<0 ? 0 : value;
}
/* 028ADFA8 */
void SequenceSound::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mActive==0) return;
    mActive=0;
    finalizeSequenceBasicSound(&mBasic);
    SequenceManager* manager=mManager;
    SequenceListNode* link=reinterpret_cast<SequenceListNode*>(&mBasic.poolLink);
    removeSequenceSoundNode(&manager->mSounds,link);
    gabi::call_ptr<void>(mBasic.vtable->finalize.function,this);
    gabi::FrameLocal<gptr<SequenceListNode>> position(8);
    *position=&manager->mFreeSounds.sentinel;
    insertSequenceSoundNode(&manager->mFreeSounds,position.get(),link);
}
/* 028AE048 */
void SequenceSound::updatePriority() {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 priority=sequencePriority(mBasic);
    SequenceManager* manager=mManager;
    SequenceListNode* link=reinterpret_cast<SequenceListNode*>(&mBasic.poolLink);
    removeSequenceSoundNode(&manager->mSounds,link);
    SequenceListNode* node=manager->mSounds.sentinel.next;
    while (node!=&manager->mSounds.sentinel) {
        const BasicSound* other=gabi::at<const BasicSound>(gabi::ea(node)-0x274);
        if (priority<sequencePriority(*other)) break;
        node=node->next;
    }
    gabi::FrameLocal<gptr<SequenceListNode>> position(8);
    *position=node;
    insertSequenceSoundNode(&manager->mSounds,position.get(),link);
}
/* WWHD glue
ext 02885724 void finalizeSequenceBasicSound(nw::snd::BasicSound*)
ext 0286DA9C void removeSequenceSoundNode(nw::snd::SequenceList*,nw::snd::SequenceListNode*)
ext 0286DA70 void insertSequenceSoundNode(nw::snd::SequenceList*,gptr<nw::snd::SequenceListNode>*,nw::snd::SequenceListNode*)
*/
