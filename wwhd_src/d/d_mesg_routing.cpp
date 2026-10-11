/** HD message lookup and renderer routing. Local verification */
#include "d/d_mesg_routing.h"
using namespace MesgRouting;
using Word = be<u32>;
using FloatConstant = be<f32>;
using ResourcePointer = gptr<void>;
static StringVtable l_sourceVtable;
static StringVtable l_formatVtable;
static StringVtable l_keyInitialVtable;
static StringVtable l_keyVtable;
static StringVtable l_pairBufferVtable;
static StringVtable l_pairVtable;
static ResourceKey l_messageKey;
static ResourceKey l_scopeKey;
static ResourceKey l_demoKey;
static Word l_messageKeyInitialized;
static Word l_scopeKeyInitialized;
static Word l_demoKeyInitialized;
static ResourcePointer l_resources;
static u8 l_messageColor;
static u8 l_scopeColor;
static u8 l_demoColor;
static FloatConstant l_tactPosition;

// HD: all entry points retain their original full-width register signatures.
u32 dMesg_routeMessage(u32 i_manager, u32 i_msgNo, u32 i_lookAtPos);
u32 fopMsgM_messageSet(u32 i_manager, u32 i_msgNo, u32 i_lookAtPos);
u32 fopMsgM_scopeMessageSet(u32 i_manager, u32 i_msgNo);
u32 fopMsgM_tactMessageSet(u32 i_manager);
u32 fopMsgM_messageSetDemo(u32 i_manager, u32 i_msgNo);
u32 dMesg_routeNamedMessage(u32 i_manager, u32 i_group, u32 i_name, u32 i_context, u32 i_option);

static void terminate(StringBase* string) { gabi::call_ptr<u32>(string->mVtable->assure, string); }
static u32 stringLength(be<u8>* string) {
    u32 length = 0;
    while (static_cast<u8>(*string)) {
        ++length;
        string = gabi::at<be<u8>>(gabi::ea(string) + 1);
        if (length > STRING_MAX_LENGTH)
            return 0;
    }
    return length;
}
static void terminateAt(void* buffer, u32 length) {
    *gabi::at<be<u8>>(gabi::ea(buffer) + length) = 0;
}
static void formatName(String32* name, u32 i_msgNo, const char* format) {
    name->mData = name->mBuffer;
    name->mVtable = &l_formatVtable;
    name->mCapacity = 32;
    name->mBuffer[0] = 0;
    name->mBuffer[31] = 0;
    MesgRoute_format(name, format, i_msgNo & 0xFFFF);
}
// HD: lazily initialized resource keys keep the native descriptors and virtual copy boundaries.
static void initializeKey(ResourceKey* key, Word* initialized, StringBase* source, const char* text,
                          void* color) {
    source->mVtable = &l_sourceVtable;
    key->mBuffer[5] = 0;
    key->mVtable = &l_keyInitialVtable;
    *initialized = 1;
    key->mCapacity = 6;
    key->mData = key->mBuffer;
    source->mData = gabi::at<be<u8>>(gabi::guest_addr(text));
    MesgRoute_sourceNoop(source);
    u32 length = stringLength(source->mData);
    u32 capacity = key->mCapacity;
    if (static_cast<s32>(length) >= static_cast<s32>(capacity))
        length = capacity - 1;
    terminate(source);
    be<u8>* data = source->mData;
    MesgRoute_copy(key->mBuffer, data, length, 0);
    terminateAt(key->mBuffer, length);
    key->mVtable = &l_keyVtable;
    MesgRoute_initColor(color);
}
static MessageEntry* lookup(Manager* manager, KeyFrame* frame, ResourceKey* key, Word* initialized,
                            const char* prefix, void* color) {
    if (!static_cast<u32>(*initialized))
        initializeKey(key, initialized, &frame->source, prefix, color);
    MesgRoute_lookup(l_resources, manager->mProcessorWork, key, &frame->name);
    return manager->mWordWork;
}
static void setMessageMetadata(MessageEntry* entry) {
    // GHS reloads play on each setter; resource fields are loaded before those calls.
    u32 position = entry->mTextboxPosition;
    MesgRoute_getPlay()->mTextboxPosition = position;
    u32 camera = entry->mInitialCamera;
    MesgRoute_getPlay()->mInitialCamera = camera;
    s32 message = entry->mMsgNo;
    MesgRoute_getPlay()->mMsgNo = message;
}

/* 025F7980 */
u32 dMesg_routeMessage(u32 i_manager, u32 i_msgNo, u32 i_lookAtPos) {
    gabi::Local<u8[96]> storage;
    KeyFrame* frame = reinterpret_cast<KeyFrame*>(storage.get());
    Manager* manager = gabi::at<Manager>(i_manager);
    formatName(&frame->name, i_msgNo, "%05d");
    MessageEntry* entry =
        lookup(manager, frame, &l_messageKey, &l_messageKeyInitialized, "dummy", &l_messageColor);
    if (!entry)
        return MESG_ERROR;
    u32 oldMsgNo = manager->mMsgNo;
    manager->mMsgNo = i_msgNo;
    u32 context = manager->mMessageContext;
    if (MesgRoute_getStatus() == MESG_IDLE)
        setMessageMetadata(entry);
    u32 result;
    switch (static_cast<u32>(entry->mTextboxType)) {
    case TEXTBOX_AUX:
        manager->mMode = MODE_AUX;
        result = MesgRoute_aux(manager->mAuxScreenWork, 3, context, i_msgNo);
        break;
    case TEXTBOX_RESET:
        manager->mMode = MODE_RESET;
        result = MesgRoute_reset(manager->mResetWork, 0, context, i_msgNo);
        break;
    case TEXTBOX_OVERLAY:
        manager->mMode = MODE_OVERLAY;
        result = MesgRoute_reset(manager->mResetWork, 2, context, i_msgNo);
        break;
    case TEXTBOX_BOX:
        manager->mMode = MODE_BOX;
        result = MesgRoute_reset(manager->mResetWork, 1, context, i_msgNo);
        break;
    case TEXTBOX_AUX_SMALL:
        manager->mMode = MODE_AUX;
        result = MesgRoute_aux(manager->mAuxScreenWork, 1, context, i_msgNo);
        break;
    case TEXTBOX_AUX_EVENT:
        manager->mMode = MODE_AUX;
        result = MesgRoute_aux(manager->mAuxScreenWork, 0, context, i_msgNo);
        break;
    default:
        // HD: special message numbers select the sequence renderer independently of box type.
        switch (i_msgNo) {
        case MSG_SPECIAL:
            manager->mMode = MODE_SEQUENCE;
            result = MesgRoute_sequence(manager->mSequenceWork, context, MSG_SPECIAL, 2, 0);
            break;
        case MSG_SEQUENCE:
            manager->mMode = MODE_SEQUENCE;
            result = MesgRoute_sequence(manager->mSequenceWork, context, MSG_SEQUENCE, 3, 0);
            break;
        case MSG_SEQUENCE_NEXT:
        case MSG_SEQUENCE_ALT:
        case MSG_SEQUENCE_ALT_NEXT:
            manager->mMode = MODE_SEQUENCE;
            result = MesgRoute_sequence(manager->mSequenceWork, context, i_msgNo, 1, 1);
            break;
        default:
            manager->mMode = MODE_TALK;
            result = MesgRoute_talk(manager->mScreenWork, context, i_msgNo, i_lookAtPos);
            break;
        }
        break;
    }
    if (!result) {
        manager->mMsgNo = oldMsgNo;
        return MESG_ERROR;
    }
    return 0;
}

/* 025F7DB0 */
u32 fopMsgM_messageSet(u32 i_manager, u32 i_msgNo, u32 i_lookAtPos) {
    gabi::Local<u8[24]> frame;
    u32 status = MesgRoute_getStatus();
    if (status == MESG_IDLE) {
        MesgRoute_getPlay()->mMesgAnimeTag = 0xFF;
        MesgRoute_getPlay()->mMesgCameraTag = MESG_ERROR;
        u32 result = MesgRoute_select(i_manager, i_msgNo, i_lookAtPos);
        MesgRoute_setStatus(gabi::at<Manager>(i_manager), 1);
        return result;
    }
    if (status == MESG_CONTINUES)
        return MesgRoute_select(i_manager, i_msgNo, i_lookAtPos);
    return MESG_ERROR;
}

/* 025F7E68 */
u32 fopMsgM_scopeMessageSet(u32 i_manager, u32 i_msgNo) {
    gabi::Local<u8[88]> storage;
    KeyFrame* frame = reinterpret_cast<KeyFrame*>(storage.get());
    Manager* manager = gabi::at<Manager>(i_manager);
    formatName(&frame->name, i_msgNo, "%05d");
    MessageEntry* entry =
        lookup(manager, frame, &l_scopeKey, &l_scopeKeyInitialized, "dummy", &l_scopeColor);
    if (!entry)
        return MESG_ERROR;
    if ((static_cast<u32>(MesgRoute_getPlay()->mPlayerStatus) & PLAYER_TELESCOPE_LOOK) &&
        MesgRoute_getPlay()->mScopeMesgStatus == MESG_SCOPE_ACTIVE) {
        setMessageMetadata(entry);
        MesgRoute_getPlay()->mScopeMesgStatus = MESG_BOX_OPENING;
    }
    u32 context = manager->mMessageContext;
    if (!MesgRoute_render(manager->mRenderWork, context, i_msgNo))
        return MESG_ERROR;
    manager->mMsgNo = i_msgNo;
    return 0;
}

/* 025F8088 */
u32 fopMsgM_tactMessageSet(u32 i_manager) {
    gabi::Local<u8[32]> storage;
    PositionFrame* frame = reinterpret_cast<PositionFrame*>(storage.get());
    // HD: tact routing seeds all three position components from the TU's 1.0f constant.
    f64 coordinate = l_tactPosition;
    frame->y = coordinate;
    frame->x = coordinate;
    frame->z = coordinate;
    u32 status = MesgRoute_getStatus();
    Manager* manager = gabi::at<Manager>(i_manager);
    if (status == MESG_IDLE || status == MESG_CONTINUES) {
        manager->mTactMsgFlag = 1;
        u32 result = MesgRoute_select(i_manager, MSG_TACT, gabi::ea(&frame->x));
        if (status == MESG_IDLE)
            MesgRoute_setStatus(manager, 2);
        return result;
    }
    if (MesgRoute_getPlay()->mScopeMesgStatus != 0)
        MesgRoute_setStatus(manager, 0);
    return MESG_ERROR;
}

/* 025F8154 */
u32 fopMsgM_messageSetDemo(u32 i_manager, u32 i_msgNo) {
    gabi::Local<u8[88]> storage;
    KeyFrame* frame = reinterpret_cast<KeyFrame*>(storage.get());
    Manager* manager = gabi::at<Manager>(i_manager);
    MesgRoute_getPlay()->mMesgAnimeTag = 0xFF;
    MesgRoute_getPlay()->mMesgCameraTag = MESG_ERROR;
    u32 status = MesgRoute_getStatus();
    if (status != MESG_IDLE && status != MESG_CONTINUES)
        return MESG_ERROR;
    formatName(&frame->name, i_msgNo, "%05d");
    MessageEntry* entry =
        lookup(manager, frame, &l_demoKey, &l_demoKeyInitialized, "dummy", &l_demoColor);
    u32 context = manager->mMessageContext;
    if (!entry)
        return MESG_ERROR;
    setMessageMetadata(entry);
    manager->mMode = MODE_AUX;
    MesgRoute_aux(manager->mAuxScreenWork, 2, context, i_msgNo);
    manager->mMsgNo = i_msgNo;
    MesgRoute_setStatus(manager, 2);
    return 0;
}

static void copyName(String64* destination, StringBase* source) {
    terminate(source);
    u32 length = stringLength(source->mData);
    u32 capacity = destination->mCapacity;
    if (static_cast<s32>(length) >= static_cast<s32>(capacity))
        length = capacity - 1;
    terminate(source);
    be<u8>* data = source->mData;
    MesgRoute_copy(destination->mBuffer, data, length, 0);
    terminateAt(destination->mBuffer, length);
}

/* 025F8388 */
u32 dMesg_routeNamedMessage(u32 i_manager, u32 i_group, u32 i_name, u32 i_context, u32 i_option) {
    gabi::Local<u8[208]> storage;
    PairFrame* frame = reinterpret_cast<PairFrame*>(storage.get());
    Manager* manager = gabi::at<Manager>(i_manager);
    StringBase* group = gabi::at<StringBase>(i_group);
    StringBase* name = gabi::at<StringBase>(i_name);
    frame->group.mData = frame->group.mBuffer;
    frame->group.mCapacity = 64;
    frame->group.mVtable = &l_pairBufferVtable;
    frame->group.mBuffer[63] = 0;
    copyName(&frame->group, group);
    frame->name.mVtable = &l_pairBufferVtable;
    frame->name.mData = frame->name.mBuffer;
    frame->name.mCapacity = 64;
    frame->group.mVtable = &l_pairVtable;
    frame->name.mBuffer[63] = 0;
    copyName(&frame->name, name);
    MesgRoute_setNames(manager->mProcessorWork, group, name);
    frame->name.mVtable = &l_pairVtable;
    MesgRoute_lookup(l_resources, manager->mProcessorWork, &frame->group, &frame->name);
    MessageEntry* entry = manager->mWordWork;
    manager->mRouteCode = 0xFFFFFFF0u;
    if (!entry)
        return MESG_ERROR;
    setMessageMetadata(entry);
    manager->mMode = MODE_SEQUENCE;
    MesgRoute_sequenceNamed(manager->mSequenceWork, i_context, i_option);
    return 0;
}
/* WWHD glue
data l_sourceVtable 100E0AE4
data l_formatVtable 100E0BA4
data l_keyInitialVtable 100E0BEC
data l_keyVtable 100E0C04
data l_pairBufferVtable 100E0B2C
data l_pairVtable 100E0B44
data l_messageKey 1048D760
data l_scopeKey 1048D738
data l_demoKey 1048D74C
data l_messageKeyInitialized 1048D774
data l_scopeKeyInitialized 1048D778
data l_demoKeyInitialized 1048D77C
data l_resources 101F4AE8
data l_messageColor 101F4B14
data l_scopeColor 101F4B20
data l_demoColor 101F4B2C
data l_tactPosition 100E0C4C
str 100E0C70 "%05d" in 025F7980
str 100E0C80 "%05d" in 025F7E68
str 100E0C90 "%05d" in 025F8154
str 100E0C68 "dummy" in 025F7980
str 100E0C78 "dummy" in 025F7E68
str 100E0C88 "dummy" in 025F8154
ext 02759C28 u32 MesgRoute_format(MesgRouting::String*, const char*, u32)
ext 025F8F80 u32 MesgRoute_sourceNoop(MesgRouting::StringBase*)
ext C0009988 u32 MesgRoute_copy(void*, void*, u32, u32)
ext 028F026C u32 MesgRoute_initColor(void*)
ext 025F50E8 u32 MesgRoute_lookup(void*, void*, MesgRouting::StringBase*, MesgRouting::StringBase*)
ext 025200D4 MesgRouting::Play* MesgRoute_getPlay()
ext 025F795C u32 MesgRoute_getStatus()
ext 026AE194 u32 MesgRoute_sequence(void*, u32, u32, u32, u32)
ext 026B3F18 u32 MesgRoute_aux(void*, u32, u32, u32)
ext 026BCBF0 u32 MesgRoute_reset(void*, u32, u32, u32)
ext 026B699C u32 MesgRoute_talk(void*, u32, u32, u32)
ext 026B8FD0 u32 MesgRoute_render(void*, u32, u32)
ext 025F7980 u32 MesgRoute_select(u32, u32, u32)
ext 025F74D0 u32 MesgRoute_setStatus(MesgRouting::Manager*, u32)
ext 025F6D70 u32 MesgRoute_setNames(void*, MesgRouting::StringBase*, MesgRouting::StringBase*)
ext 026AE330 u32 MesgRoute_sequenceNamed(void*, u32, u32)
*/
