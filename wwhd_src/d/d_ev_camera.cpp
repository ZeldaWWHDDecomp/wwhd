// Event-camera parameter and control helpers; GameCube names, verified HD behavior.
#include "d/d_ev_camera.h"
namespace event_camera {
static EventString intFile;
static EventString intAssert;
static EventString stringPointerFile;
static EventString stringPointerAssert;
static EventString defaultIntFile;
static EventString defaultIntAssert;
static EventString floatFile;
static EventString floatAssert;
static EventString xyzFile;
static EventString xyzAssert;
static EventString stringFile;
static EventString stringAssert;
static EventString defaultStringPointerFile;
static EventString defaultStringPointerAssert;
static EventString timerName;
static EventString stayName;
static HeaderStatics headerStatics;
static DestructorRecords destructorRecords;

static bool debugParams(dCamera_c* camera) {
    return (camera->mEventFlags & DEBUG_EVENT_PARAMS) != 0;
}
static EventParameter* parameters(dCamera_c* camera) {
    return reinterpret_cast<EventParameter*>(camera->mEventData.mEventParams);
}
static GuestWord* paramValue(dCamera_c* camera, s32 index) {
    return parameters(camera)[index].mValue.get();
}
static void SkipSmoother(dCamera_c* camera) {
    camera->m102 = 1;
    camera->m101 = 1;
    camera->m100 = 1;
}
static void copyPoint(cXyz* destination, const cXyz* source) {
    // HD: preserve integer payloads and the interleaved word-copy order.
    auto* out = reinterpret_cast<VectorWords*>(destination);
    auto* in = reinterpret_cast<const VectorWords*>(source);
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}
static void copyString(GuestByte* destination, const char* source) {
    eventCameraStrcpy(reinterpret_cast<char*>(destination), source);
}

/* 025303B4 */
s32 searchEventArgData(dCamera_c* camera, const GuestByte* name) {
    for (s32 i = 0; i < 8; ++i) {
        auto* parameterName = parameters(camera)[i].mName;
        if (parameterName[0] == 0)
            return -1;
        for (u32 k = 0;; ++k) {
            u8 a = parameterName[k];
            u8 b = name[k];
            if (a != b)
                break;
            if (a == 0)
                return i;
        }
    }
    return -1;
}

/* 02530418 */
u32 getEvIntData(dCamera_c* camera, GuestWord* out, const GuestByte* name) {
    if (debugParams(camera)) {
        s32 index = searchEventArgData(camera, name);
        if (index == -1)
            return 0;
        *out = *paramValue(camera, index);
        return 1;
    }
    if (dComIfGp_evmng_getMySubstanceNum(camera->mEventData.mStaffIdx,
                                         reinterpret_cast<const char*>(name)) == 0) {
        camera->mEventData.field_0x10 = 1;
        return 0;
    }
    auto* value = static_cast<GuestWord*>(dComIfGp_evmng_getMySubstanceP(
        camera->mEventData.mStaffIdx, reinterpret_cast<const char*>(name), 3));
    if (!value)
        eventCameraAssert(intFile.text, 0x9A, intAssert.text);
    *out = *value;
    return 1;
}

// HD: pointer-valued entry points retain their verified u32 return width.
/* 02530538 */
u32 getEvStringPntData(dCamera_c* camera, const GuestByte* name) {
    if (debugParams(camera)) {
        s32 index = searchEventArgData(camera, name);
        if (index == -1)
            return 0;
        return gabi::ea(paramValue(camera, index));
    }
    if (dComIfGp_evmng_getMySubstanceNum(camera->mEventData.mStaffIdx,
                                         reinterpret_cast<const char*>(name)) == 0) {
        camera->mEventData.field_0x10 = 1;
        return 0;
    }
    auto* value = dComIfGp_evmng_getMySubstanceP(camera->mEventData.mStaffIdx,
                                                 reinterpret_cast<const char*>(name), 4);
    if (!value)
        eventCameraAssert(stringPointerFile.text, 0x11C, stringPointerAssert.text);
    return gabi::ea(value);
}

// GameCube overload: getEvIntData(out, name, default).
/* 02530634 */
u32 getEvIntDataDefault(dCamera_c* camera, GuestWord* out, const GuestByte* name, s32 fallback) {
    s32 value = fallback;
    if (debugParams(camera)) {
        s32 index = searchEventArgData(camera, name);
        if (index != -1)
            value = static_cast<s32>(*paramValue(camera, index));
        *out = static_cast<u32>(value);
        return 1;
    }
    if (dComIfGp_evmng_getMySubstanceNum(camera->mEventData.mStaffIdx,
                                         reinterpret_cast<const char*>(name)) == 0) {
        *out = static_cast<u32>(value);
        return 0;
    }
    auto* data = static_cast<GuestWord*>(dComIfGp_evmng_getMySubstanceP(
        camera->mEventData.mStaffIdx, reinterpret_cast<const char*>(name), 3));
    if (!data)
        eventCameraAssert(defaultIntFile.text, 0x137, defaultIntAssert.text);
    *out = *data;
    return 1;
}

/* 0253072C */
u32 getEvFloatData(dCamera_c* camera, be<f32>* out, const GuestByte* name, f32 fallback) {
    f32 value = fallback;
    if (debugParams(camera)) {
        s32 index = searchEventArgData(camera, name);
        if (index != -1)
            value = *reinterpret_cast<be<f32>*>(paramValue(camera, index));
        *out = value;
        return 1;
    }
    if (dComIfGp_evmng_getMySubstanceNum(camera->mEventData.mStaffIdx,
                                         reinterpret_cast<const char*>(name)) == 0) {
        *out = value;
        return 0;
    }
    auto* data = static_cast<be<f32>*>(dComIfGp_evmng_getMySubstanceP(
        camera->mEventData.mStaffIdx, reinterpret_cast<const char*>(name), 0));
    if (!data)
        eventCameraAssert(floatFile.text, 0x150, floatAssert.text);
    *out = *data;
    return 1;
}

/* 0253086C */
u32 getEvXyzData(dCamera_c* camera, cXyz* out, const GuestByte* name, const cXyz* fallback) {
    if (debugParams(camera)) {
        s32 index = searchEventArgData(camera, name);
        copyPoint(out, index == -1 ? fallback : reinterpret_cast<cXyz*>(paramValue(camera, index)));
        return 1;
    }
    if (dComIfGp_evmng_getMySubstanceNum(camera->mEventData.mStaffIdx,
                                         reinterpret_cast<const char*>(name)) == 0) {
        copyPoint(out, fallback);
        return 0;
    }
    auto* data = static_cast<cXyz*>(dComIfGp_evmng_getMySubstanceP(
        camera->mEventData.mStaffIdx, reinterpret_cast<const char*>(name), 1));
    if (!data)
        eventCameraAssert(xyzFile.text, 0x169, xyzAssert.text);
    copyPoint(out, data);
    return 1;
}

/* 02530998 */
u32 getEvStringData(dCamera_c* camera, GuestByte* out, const GuestByte* name,
                    const char* fallback) {
    if (debugParams(camera)) {
        s32 index = searchEventArgData(camera, name);
        copyString(out, index == -1 ? fallback
                                    : reinterpret_cast<const char*>(paramValue(camera, index)));
        return 1;
    }
    if (dComIfGp_evmng_getMySubstanceNum(camera->mEventData.mStaffIdx,
                                         reinterpret_cast<const char*>(name)) == 0) {
        copyString(out, fallback);
        return 0;
    }
    auto* data = static_cast<const char*>(dComIfGp_evmng_getMySubstanceP(
        camera->mEventData.mStaffIdx, reinterpret_cast<const char*>(name), 4));
    if (!data)
        eventCameraAssert(stringFile.text, 0x19C, stringAssert.text);
    copyString(out, data);
    return 1;
}

// GameCube overload: getEvStringPntData(name, default).
/* 02530AAC */
u32 getEvStringPntDataDefault(dCamera_c* camera, const GuestByte* name, const char* fallback) {
    if (debugParams(camera)) {
        s32 index = searchEventArgData(camera, name);
        return index == -1 ? gabi::ea(fallback) : gabi::ea(paramValue(camera, index));
    }
    if (dComIfGp_evmng_getMySubstanceNum(camera->mEventData.mStaffIdx,
                                         reinterpret_cast<const char*>(name)) == 0)
        return gabi::ea(fallback);
    auto* data = dComIfGp_evmng_getMySubstanceP(camera->mEventData.mStaffIdx,
                                                reinterpret_cast<const char*>(name), 4);
    if (!data)
        eventCameraAssert(defaultStringPointerFile.text, 0x1B6, defaultStringPointerAssert.text);
    return gabi::ea(data);
}

static u32 actorByName(dCamera_c* camera, u32 prefix, const char* name) {
    switch (prefix) {
    case ACTOR_PLAYER:
        return gabi::ea(camera->mpPlayerActor.get());
    case ACTOR_STARTER:
        return gabi::ea(dComIfGp_event_getPt1());
    case ACTOR_PARTNER:
        return gabi::ea(dComIfGp_event_getPt2());
    case ACTOR_TALK_PARTNER:
        return gabi::ea(dComIfGp_event_getTalkPartner());
    case ACTOR_TARGET:
    case ACTOR_ITEM: {
        auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
        return gabi::ea(eventCameraConvPId(&play->mEvtCtrl, play->mEvtCtrl.mPtItem));
    }
    case ACTOR_LINK:
        return gabi::ea(dComIfGp_getLinkPlayer());
    default:
        return eventCameraActorByName(name, 0, 0);
    }
}

/* 02530B84 */
u32 getEvActor(dCamera_c* camera, const GuestByte* name) {
    u32 stringAddress = getEvStringPntData(camera, name);
    if (!stringAddress)
        return 0;
    auto* string = gabi::at<GuestWord>(stringAddress);
    u32 prefix = *string;
    return actorByName(camera, prefix, reinterpret_cast<const char*>(string));
}

// GameCube overload: getEvActor(name, default).
/* 02530CB4 */
u32 getEvActorDefault(dCamera_c* camera, const GuestByte* name, const char* fallback) {
    // HD: the original frame has a buffer at +8; long strings retain its overrun behavior.
    ActorStringFrame frame;
    frame.mBuffer[0] = 0;
    getEvStringData(camera, frame.mBuffer, name, fallback);
    u32 prefix = *reinterpret_cast<GuestWord*>(frame.mBuffer);
    return actorByName(camera, prefix, reinterpret_cast<const char*>(frame.mBuffer));
}

/* 02530DDC */
u32 pauseEvCamera(dCamera_c* camera) {
    auto* work = reinterpret_cast<PauseWork*>(camera->mWork);
    if (camera->m11C == 0) {
        SkipSmoother(camera);
        work->mTimerSet = static_cast<u8>(getEvIntDataDefault(
            camera, &work->mTimer, reinterpret_cast<const GuestByte*>(timerName.text), -1));
        getEvIntDataDefault(camera, &work->mStay, reinterpret_cast<const GuestByte*>(stayName.text),
                            0);
        if (camera->mHD610 != 0) {
            // HD: keep the eye 120 units from the center along the current direction.
            cXyz direction;
            cXyz normalized;
            cXyz scaled;
            eventCameraSubtract(&camera->mEye, &direction, &camera->mCenter);
            eventCameraNormalize(&direction, &normalized);
            eventCameraScale(&normalized, &scaled, 120.0f);
            copyPoint(&direction, &scaled);
            eventCameraAdd(&camera->mCenter, &normalized, &direction);
            auto* words = reinterpret_cast<VectorWords*>(&normalized);
            u32 x = words->x;
            u32 y = words->y;
            u32 z = words->z;
            auto* eye = reinterpret_cast<VectorWords*>(&camera->mViewCache.mEye);
            eye->z = z;
            eye->x = x;
            eye->y = y;
            eventCameraSubtract(&camera->mViewCache.mEye, &normalized, &camera->mViewCache.mCenter);
            cSGlobe_Val(&camera->mViewCache.mDirection, &normalized);
        }
    }
    if (work->mStay != 0)
        camera->mEventFlags |= 1;
    if (work->mTimerSet != 0 && camera->m11C < work->mTimer)
        return 0;
    return 1;
}

/* 02530F5C */
u8 talktoEvCamera(dCamera_c* camera) {
    dComIfGp_event_getPt1();
    s32 type = camera->mEventData.field_0x0c;
    u32 frame = camera->m108;
    s32 style = camTypeStyle(type, 3);
    if (frame == 0)
        camera->mEventFlags &= ~CLEAR_EVENT_FLAG;
    if (style < 0)
        style = eventCameraSearchStyle(camera->mCamParam, 0x54543031); // TT01
    eventCameraTalktoCamera(camera, style);
    u8 done = 0;
    if (camera->m100 != 0 && camera->m101 != 0 && camera->m102 != 0)
        done = 1;
    return done;
}

/* 0253E70C */
u32 StartEventCamera(dCamera_c* camera, u32 id2, u32 id1, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
    auto* overflow = entryStackArguments();
    EventVarArgsFrame frame;
    const u32 registers[8] = {gabi::ea(camera), id2, id1, a3, a4, a5, a6, a7};
    for (u32 i = 0; i < 8; ++i)
        frame.mRegisters[i] = registers[i];
    if (debugParams(camera))
        return 0;
    camera->mEventData.field_0x14 = id1;
    camera->mEventData.field_0x18 = id2;
    frame.mArgs.mpOverflow = overflow;
    frame.mArgs.mpRegisterSave = frame.mRegisters;
    frame.mArgs.mGpr = 3;
    frame.mArgs.mFpr = 0;
    for (u32 i = 0; i < 8; ++i) {
        u32 nameAddress = *eventCameraVaArg(&frame.mArgs, 1);
        auto* parameter = &parameters(camera)[i];
        if (!nameAddress) {
            parameter->mName[0] = 0;
            break;
        }
        copyString(parameter->mName, gabi::at<const char>(nameAddress));
        u32 value = *eventCameraVaArg(&frame.mArgs, 1);
        parameter->mValue = gabi::at<GuestWord>(value);
    }
    u32 flags = camera->mEventFlags;
    camera->m11C = 0;
    camera->mEventFlags = flags | DEBUG_EVENT_PARAMS;
    return 1;
}

/* 0253E860 */
u32 EndEventCamera(dCamera_c* camera, u32 id) {
    u32 flags = camera->mEventFlags;
    if (!(flags & DEBUG_EVENT_PARAMS))
        return 0;
    if (camera->m0E8 != -1 && id != static_cast<u32>(camera->mEventData.field_0x14))
        return 0;
    camera->mEventFlags = flags & ~DEBUG_EVENT_PARAMS;
    return 1;
}

/* 0253E89C */
void __sinit_d_ev_camera_cpp() {
    for (u32 i = 0; i < 4; ++i)
        headerStatics.mZeroObject[i] = 0;
    eventCameraRegisterGlobal(&destructorRecords.mRecords[0]);
    headerStatics.mPi[0] = -3.1415927f;
    headerStatics.mPi[1] = 3.1415927f;
    eventCameraConstructFirst(&headerStatics.mFirstObject);
    eventCameraRegisterGlobal(&destructorRecords.mRecords[1]);
    eventCameraConstructSecond(&headerStatics.mSecondObject);
    eventCameraRegisterGlobal(&destructorRecords.mRecords[2]);
}

/* 0253E930 */
void destroyEventCameraHeaderObject(void* object, s32 flags) {
    if (!object)
        return;
    if (!(flags & 1))
        return;
    eventCameraFree(object);
}

/* 0253E944 */
void eventCameraEmptyVirtual(void* object) {
}
} // namespace event_camera
using namespace event_camera;

/* WWHD glue (tools/verify/autoglue.py)
data intFile 1004CCB8
data intAssert 1004CCC8
data stringPointerFile 1004CCD8
data stringPointerAssert 1004CCE8
data defaultIntFile 1004CCF8
data defaultIntAssert 1004CD08
data floatFile 1004CD18
data floatAssert 1004CD28
data xyzFile 1004CD38
data xyzAssert 1004CD48
data stringFile 1004CD58
data stringAssert 1004CD68
data defaultStringPointerFile 1004CD78
data defaultStringPointerAssert 1004CD88
data timerName 1004CD9C
data stayName 1004CDA4
data headerStatics 10475850
data destructorRecords 101D627C
ext 0273AA24 u32 eventCameraAssert(const char*, s32, const char*)
ext 028F040C void eventCameraStrcpy(char*, const char*)
ext 0253EE04 fopAc_ac_c* eventCameraConvPId(dEvt_control_c*, u32)
ext 025D9F38 u32 eventCameraActorByName(const char*, u32, u32)
ext 024F732C s32 eventCameraSearchStyle(void*, u32)
ext 02508A60 void eventCameraTalktoCamera(dCamera_c*, s32)
ext 0201ADE0 void eventCameraSubtract(cXyz*, cXyz*, cXyz*)
ext 0201B31C void eventCameraNormalize(cXyz*, cXyz*)
ext 0201AE48 void eventCameraScale(cXyz*, cXyz*, f32)
ext 0201AD78 void eventCameraAdd(cXyz*, cXyz*, cXyz*)
ext 028F5C88 GuestWord* eventCameraVaArg(EventVaList*, u32)
ext 028F026C void eventCameraRegisterGlobal(DestructorRecord*)
ext 028ED6F8 void eventCameraConstructFirst(GuestByte*)
ext 028EAB2C void eventCameraConstructSecond(GuestByte*)
ext 0273AF40 void eventCameraFree(void*)
*/
