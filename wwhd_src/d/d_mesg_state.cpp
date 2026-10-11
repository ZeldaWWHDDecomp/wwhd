/* HD message manager state, tagged UTF-16 expansion and scope transitions.
 * String descriptor sizes, virtual assures and sparse guest writes follow the
 * verified HD implementation. */
#include "d/d_mesg_state.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_msg.h"

using StringVtableHD = dMesg_stringVtableHD;
static StringVtableHD byteBufferVtableHD;
static StringVtableHD byteSourceVtableHD;
static StringVtableHD nameBufferVtableHD;
static StringVtableHD keyBufferVtableHD;
static StringVtableHD expandedBufferVtableHD;
static StringVtableHD wideSourceVtableHD;
using WordHD = be<u32>;
static WordHD managerVtableHD;
using SavePointerHD = gptr<dMesg_nameSaveHD>;
static SavePointerHD nameSaveHD;
using VoidPointerHD = gptr<void>;
static VoidPointerHD textResourcesHD;
static VoidPointerHD messageOwnerHD;
using ManagerPointerHD = gptr<dMesg_state_c>;
static ManagerPointerHD activeHandlerHD;
using FormatHD = char[1];
static FormatHD textKeyFormatHD;
static f32 actorOffsetHD;
static f32 actorMinimumHD;

// HD: keep the original bounded scans and wrapping guest cursors, including sparse buffers.
static s32 dMesg_stringLengthHD(u32 p, u32 stride) {
    s32 n = 0;
    if (stride == 1 ? gabi::load<u8>(p) == 0 : gabi::load<u16>(p) == 0)
        return 0;
    do {
        ++n;
        p += stride;
        if (n > 0x40000)
            return 0;
    } while (stride == 1 ? gabi::load<u8>(p) != 0 : gabi::load<u16>(p) != 0);
    return n;
}
// HD: signed capacities can place a terminator before, or far beyond, the advertised buffer.
static void dMesg_terminateBytesHD(void* buffer, u32 offset) {
    gabi::store<u8>(gabi::ea(buffer) + offset, 0);
}
static void dMesg_writeWordHD(u32 buffer, u32 index, u16 value) {
    gabi::store<u16>(buffer + index * 2, value);
}
static u16 dMesg_readWordHD(u32 input) { return gabi::load<u16>(input); }

/* 025F85A8 */
void dMesg_state_c::paneAction() { dMesg_sequenceActionHD(mSequenceWork); }
/* 025F85B0 */
void dMesg_state_c::textAction() { dMesg_resourceActionHD(mResourceWork); }
/* 025F85B8 */
void dMesg_state_c::textAdvance() { dMesg_resourceAdvanceHD(mResourceWork); }
/* 025F85C0 */
u32 dMesg_state_c::textReset() { return dMesg_resourceResetHD(mResourceWork); }
/* 025F85C8 */
void dMesg_state_c::positionActor(dMesg_positionHD* actor) {
    auto* pane = mResourceWork->mpPane.get();
    f32 x = f32(pane->mPositionX) + actorOffsetHD;
    f32 limit = actorMinimumHD;
    u32 y = actor->mPositionYBits;
    u8 flags = actor->mFlags;
    if (!(x >= limit))
        x = limit;
    actor->mPositionX = x;
    actor->mPositionYBits = y;
    actor->mFlags = flags | 0x10;
}
/* 025F8610 */
void dMesg_state_c::setContext(u32 context) { mTextContext = context; }
/* 025F8618 */
void dMesg_state_c::playerName(dMesg_stringHD* destination) {
    dMesg_nameBytesHD narrow;
    dMesg_stringBaseHD source;
    narrow.mData = narrow.mBuffer;
    narrow.mVtable = &byteBufferVtableHD;
    narrow.mCapacity = 20;
    narrow.mBuffer[0] = 0;
    narrow.mBuffer[19] = 0;
    auto* name = dMesg_getSavedNameHD(&nameSaveHD->mPlayerNameSlot);
    name->mName.assure();
    source.mData = name->mName.mData;
    source.mVtable = &byteSourceVtableHD;
    dMesg_byteStringNoopHD(&source);
    s32 n = dMesg_stringLengthHD(source.mData.v, 1);
    u32 capacity = narrow.mCapacity;
    if (n >= s32(capacity))
        n = capacity - 1;
    source.assure();
    dMesg_memcpyHD(narrow.mBuffer, source.mData, u32(n), 0);
    dMesg_terminateBytesHD(narrow.mBuffer, u32(n));
    destination->assure();
    auto* out = destination->mData.get();
    narrow.assure();
    dMesg_bytesToWordsHD(out, destination->mCapacity, narrow.mData, 0xFFFFFFFF);
}
/* 025F8780 */
void dMesg_state_c::expandTagged(dMesg_stringHD* destination, u32 input, u32 context) {
    u16 token = dMesg_readWordHD(input);
    while (token != 0) {
        if (token == MESG_TAG_BEGIN_HD || token == MESG_TAG_END_HD) {
            u32 tag = input;
            input += token == MESG_TAG_BEGIN_HD ? dMesg_readWordHD(tag + 6) + 8 : 6;
            if (dMesg_readWordHD(tag + 2) == MESG_TAG_PLAYER_NAME_HD &&
                dMesg_readWordHD(tag + 4) == 0) {
                dMesg_nameWordsHD name;
                name.mData = name.mBuffer;
                name.mVtable = &nameBufferVtableHD;
                name.mCapacity = 20;
                name.mBuffer[0] = 0;
                name.mBuffer[19] = 0;
                // HD: the original call forwards context in r5 although playerName ignores it.
                dMesg_playerNameWithContextHD(this, &name, context);
                u32 out = destination->mData.v;
                destination->assure();
                s32 current = dMesg_stringLengthHD(destination->mData.v, 2);
                s32 offset = current < 0 ? 0 : current;
                name.assure();
                s32 n = dMesg_stringLengthHD(name.mData.v, 2);
                s32 remaining = s32(u32(destination->mCapacity) - u32(offset));
                if (n >= remaining)
                    n = remaining - 1;
                if (n > 0) {
                    u32 dst = out + u32(offset) * 2;
                    name.assure();
                    dMesg_memcpyHD(gabi::at<void>(dst), name.mData, u32(n) * 2, 0);
                    if (s32(u32(offset) + u32(n)) > current)
                        dMesg_writeWordHD(out, u32(offset) + u32(n), 0);
                }
            }
        } else {
            destination->assure();
            u32 out = destination->mData.v;
            s32 n = dMesg_stringLengthHD(out, 2);
            if (n < s32(u32(destination->mCapacity) - 1)) {
                dMesg_writeWordHD(out, u32(n), token);
                dMesg_writeWordHD(out, u32(n) + 1, 0);
            }
            input += 2;
        }
        token = dMesg_readWordHD(input);
    }
}
/* 025F8A08 */
void dMesg_state_c::lookupText(be<u16>* destination, u32 id) {
    dMesg_textKeyHD key;
    key.mData = key.mBuffer;
    key.mVtable = &keyBufferVtableHD;
    key.mCapacity = 256;
    key.mBuffer[0] = 0;
    key.mBuffer[255] = 0;
    dMesg_formatKeyHD(&key, textKeyFormatHD, id & 0xFFFF);
    auto* table = dMesg_findTextTableHD(textResourcesHD, &key);
    if (table == nullptr)
        return;
    key.assure();
    u32 index = dMesg_findTextIndexHD(table->mTable, key.mData);
    if (index >= 0xFFFFFFFE)
        return;
    u32 text = 0;
    if (index < table->mCount)
        text = dMesg_getTextHD(table->mTable, index);
    dMesg_expandedTextHD expanded;
    expanded.mData = expanded.mBuffer;
    expanded.mVtable = &expandedBufferVtableHD;
    expanded.mCapacity = 17;
    expanded.mBuffer[0] = 0;
    expanded.mBuffer[16] = 0;
    expandTagged(&expanded, text, id);
    expanded.assure();
    u32 p = expanded.mData.v;
    u32 out = gabi::ea(destination);
    u32 n = 0;
    while (n < 16) {
        u16 c = dMesg_readWordHD(p);
        if (c == 0)
            break;
        dMesg_writeWordHD(out, 0, c);
        ++n;
        out += 2;
        p += 2;
    }
    dMesg_writeWordHD(out, 0, 0);
}

// HD: retain full r3 result widths for the two GameCube boolean scope helpers.
/* 025F8B5C */
u32 fopMsgM_getScopeMode() {
    if (dComIfGp_checkPlayerStatus0(0, daPyStts0_TELESCOPE_LOOK_e) != 0) {
        if (dComIfGp_getScopeMesgStatus() == fopMsgStts_SCOPE_ACTIVE_e) {
            if (!dComIfGp_event_runCheck()) {
                dComIfGp_setScopeMesgStatus(fopMsgStts_SCOPE_WAIT_e);
                return 1;
            }
        }
    }
    if (dComIfGp_getScopeMesgStatus() == fopMsgStts_BOX_CLOSING_e) {
        // HD: this path writes the general message status, without a manager method call.
        reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mMesgStatus =
            fopMsgStts_SCOPE_WAIT_e;
        return 1;
    }
    return 0;
}
/* 025F8BEC */
u32 fopMsgM_releaseScopeMode() {
    if (dComIfGp_getScopeMesgStatus() == fopMsgStts_SCOPE_WAIT_e) {
        dComIfGp_setScopeMesgStatus(fopMsgStts_SCOPE_ACTIVE_e);
        return 1;
    }
    return 0;
}
/* 025F8C38 */
u32 dMesg_state_c::taggedLength(u32 input) {
    u32 n = 0;
    u16 token = dMesg_readWordHD(input);
    while (token != 0) {
        if (token == MESG_TAG_BEGIN_HD) {
            u32 skip = dMesg_readWordHD(input + 6) + 8;
            input += skip;
            n += u32(s32(skip) >> 1);
        } else if (token == MESG_TAG_END_HD) {
            input += 6;
            n += 3;
        } else {
            input += 2;
            ++n;
        }
        token = dMesg_readWordHD(input);
    }
    return n;
}
/* 025F8CDC */
u32 dMesg_state_c::copyTagged(dMesg_stringHD* destination, u32 input) {
    u32 count = taggedLength(input);
    auto* out = destination->mData.get();
    dMesg_stringBaseHD source;
    source.mData.v = input;
    source.mVtable = &wideSourceVtableHD;
    s32 n = s32(count);
    if (n < 0) {
        dMesg_wideStringNoopHD(&source);
        n = dMesg_stringLengthHD(source.mData.v, 2);
    }
    if (n >= s32(destination->mCapacity))
        n = u32(destination->mCapacity) - 1;
    source.assure();
    dMesg_memcpyHD(out, source.mData, u32(n) * 2, 0);
    dMesg_writeWordHD(gabi::ea(out), u32(n), 0);
    return count;
}
/* 025F8DD0 */
u32 dMesg_state_c::ready() {
    if (mMode != 0)
        return 0;
    return dMesg_getStatusHD(this) != 0;
}
/* 025F8E18 */
void dMesg_state_c::paneReset() {
    auto* pane = mSequenceWork.get();
    if (pane != nullptr)
        dMesg_sequenceResetHD(pane);
}
// HD: compiler-generated deleting destructor for the registered manager handler.
/* 025F8E28 */
void dMesg_state_c__dt(dMesg_state_c* i_this, u32 flags) {
    if (i_this == nullptr)
        return;
    i_this->mVtable = gabi::ea(&managerVtableHD);
    if (i_this == activeHandlerHD.get()) {
        auto* owner = messageOwnerHD.get();
        activeHandlerHD = nullptr;
        dMesg_ownerResetHD(owner, 2);
        messageOwnerHD = nullptr;
    }
    dMesg_disposerDestroyHD(i_this, 0);
    if (flags & 1)
        dMesg_deleteHD(i_this);
}

// clang-format off
/* WWHD glue
data byteBufferVtableHD 100E0C34
data byteSourceVtableHD 100E0AE4
data nameBufferVtableHD 100E0C1C
data keyBufferVtableHD 100E0B74
data expandedBufferVtableHD 100E0BD4
data wideSourceVtableHD 100E0B14
data managerVtableHD 100E0CD0
data nameSaveHD 101F84DC
data textResourcesHD 101F4AE8
data messageOwnerHD 101F4B5C
data activeHandlerHD 101F4B60
data textKeyFormatHD 100E0CA0
data actorOffsetHD 100E0C98
data actorMinimumHD 100E0C9C
ext 026AE410 void dMesg_sequenceActionHD(void*)
ext 02651EE4 void dMesg_resourceActionHD(dMesg_resourceStateHD*)
ext 02652020 void dMesg_resourceAdvanceHD(dMesg_resourceStateHD*)
ext 026521B8 u32 dMesg_resourceResetHD(dMesg_resourceStateHD*)
ext 02720170 dMesg_nameEntryHD* dMesg_getSavedNameHD(void*)
ext 025F8F80 void dMesg_byteStringNoopHD(dMesg_stringBaseHD*)
ext C0009988 void dMesg_memcpyHD(void*,void*,u32,u32)
ext 0275BAA0 u32 dMesg_bytesToWordsHD(void*,u32,void*,u32)
ext 025F8618 void dMesg_playerNameWithContextHD(dMesg_state_c*,dMesg_stringHD*,u32)
ext 02759C28 void dMesg_formatKeyHD(dMesg_stringHD*,const char*,u32)
ext 025F4FC0 dMesg_textTableHD* dMesg_findTextTableHD(void*,dMesg_stringHD*)
ext 0273A1CC u32 dMesg_findTextIndexHD(void*,void*)
ext 0273A2E8 u32 dMesg_getTextHD(void*,u32)
ext 025F8FB0 void dMesg_wideStringNoopHD(dMesg_stringBaseHD*)
ext 025F795C u32 dMesg_getStatusHD(dMesg_state_c*)
ext 026AE41C void dMesg_sequenceResetHD(void*)
ext 025F76EC void dMesg_ownerResetHD(void*,u32)
ext 02752BEC void dMesg_disposerDestroyHD(dMesg_state_c*,u32)
ext 0273AF40 void dMesg_deleteHD(void*)
*/
// clang-format on
