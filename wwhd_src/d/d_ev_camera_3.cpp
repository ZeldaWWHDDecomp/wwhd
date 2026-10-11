#include "d/d_ev_camera_3.h"

namespace event_camera_style {
static EventString mapId;
static EventString styleName;
static EventString defaultStyle;
static EventString saveSlot;
static EventString loadSlot;
static cXyz tactCenterOffset;
static cXyz tactEyeOffset;
static f32 tactFovy;

static void copyPoint(cXyz* destination, const cXyz* source) {
    // HD: integer copies preserve the original load/store order and NaN payloads.
    auto* out = reinterpret_cast<VectorWords*>(destination);
    auto* in = reinterpret_cast<const VectorWords*>(source);
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}
static void SkipSmoother(dCamera_c* camera) {
    camera->m102 = 1;
    camera->m101 = 1;
    camera->m100 = 1;
}
static void runStyleEngine(dCamera_c* camera, s32 style) {
    auto* entry = engineEntry(camStyleAlg(style));
    s16 index = entry->mVtableIndex;
    auto* object = reinterpret_cast<u8*>(camera) + static_cast<s32>(entry->mThisDelta);
    u32 function;
    if (index < 0) {
        function = entry->mFunction;
    } else {
        auto* vtable = engineObject(object, entry->mVtableOffset)->mpVtable.get();
        function = vtable[index].mFunction.get();
    }
    gabi::call_ptr(function, object, style);
}

/* 02535830 */
u32 maptoolIdEvCamera(u32 i_this) {
    auto* camera = gabi::at<dCamera_c>(i_this);
    StageEvent* event;
    if (camera->m108 != 0) {
        event = gabi::at<StageEvent>(camera->mEventData.field_0xcc);
        if (!event)
            return 1;
    } else {
        EventInt id;
        cameraStyleGetEvIntData(camera, &id, mapId.text, -1);
        s32 mapIdValue = id;
        camera->mEventData.field_0x08 = 0;
        camera->m11C = 0;
        auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
        if (mapIdValue == -1) {
            event = cameraStyleGetStageEventDt(&play->mEvtCtrl);
            camera->mEventData.field_0xcc = gabi::ea(event);
        } else {
            auto* stage = reinterpret_cast<StageView*>(play->mStageData);
            auto* info =
                gabi::call_ptr<StageEventInfo*>(stage->mpVtable->getEventInfo.get(), stage);
            if (mapIdValue >= 0 && mapIdValue < info->num)
                event = info->events.get() + mapIdValue;
            else
                event = nullptr;
            camera->mEventData.field_0xcc = gabi::ea(event);
        }
        if (!event)
            return 1;
    }
    s32 room = event->field_0x14;
    u32 flags = event->field_0x12;
    u32 soundFrame = 0xFFFFFFFF;
    u32 type = event->field_0x10;
    if (flags != MAP_KEEP_EVENT) {
        if (flags & MAP_CLEAR_EVENT_FLAG) {
            camera->mEventFlags &= ~0x200000u;
            event = gabi::at<StageEvent>(camera->mEventData.field_0xcc);
            flags = event->field_0x12;
        }
        if (flags & MAP_CLEAR_COUNTER) {
            event = gabi::at<StageEvent>(camera->mEventData.field_0xcc);
            camera->m068 = 0;
            flags = event->field_0x12;
        }
        if (flags & MAP_SOUND_NOW)
            soundFrame = 0;
        if (flags & MAP_SOUND_LATER)
            soundFrame = 30;
    }
    if (camera->m108 == soundFrame)
        cameraStyleSoundStart(MAP_CAMERA_SOUND);
    s32 cameraType = cameraStyleGetCameraTypeFromMapToolID(camera, type, room);
    camera->mEventData.field_0x0c = cameraType;
    if (cameraType == MAP_KEEP_EVENT) {
        camera->mEventData.field_0xcc = 0;
        return 1;
    }
    s32 style = camTypeStyle(cameraType, 0);
    runStyleEngine(camera, style);
    event = gabi::at<StageEvent>(camera->mEventData.field_0xcc);
    u32 duration = event->field_0x11;
    if (duration != MAP_KEEP_EVENT && camera->m11C <= duration * 10)
        return !event ? 1 : 0;
    auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
    auto* next = cameraStyleNextStageEventDt(&play->mEvtCtrl, event);
    camera->m100 = 0;
    camera->m102 = 0;
    camera->mEventData.field_0xcc = gabi::ea(next);
    camera->m101 = 0;
    if (!next)
        return 1;
    event = gabi::at<StageEvent>(camera->mEventData.field_0xcc);
    camera->m11C = 0xFFFFFFFF;
    return !event ? 1 : 0;
}

/* 0253754C */
u32 tactEvCamera(u32 i_this) {
    auto* camera = gabi::at<dCamera_c>(i_this);
    auto* work = reinterpret_cast<TactWork*>(camera->mWork);
    if (camera->m11C == 0) {
        work->mTimer = 0;
        work->mMirror = ((static_cast<u32>(camera->m07C) >> 1) & 1) ^ 1;
        f32 fovy = camera->mViewCache.mFovy;
        camera->mEventData.field_0x20 = 0;
        SkipSmoother(camera);
        s16 bank = camera->mViewCache.mBank;
        auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
        auto* saved = reinterpret_cast<CameraInfoView*>(&play->mCameraInfo[0]);
        copyPoint(&saved->mCameraPos, &camera->mViewCache.mCenter);
        copyPoint(&saved->mCameraTarget, &camera->mViewCache.mEye);
        saved->mCameraBank = bank;
        saved->mCameraFovy = fovy;
    }
    cXyz centerOffset;
    cXyz eyeOffset;
    cXyz relative;
    cXyz reflected;
    centerOffset.y = tactCenterOffset.y;
    centerOffset.x = tactCenterOffset.x;
    centerOffset.z = tactCenterOffset.z;
    eyeOffset.y = tactEyeOffset.y;
    eyeOffset.z = tactEyeOffset.z;
    eyeOffset.x = tactEyeOffset.x;
    if (work->mMode != 1)
        work->mMode = 1;
    cameraStyleRelationalPos(camera, &relative, camera->mpPlayerActor.get(), &centerOffset);
    copyPoint(&camera->mViewCache.mCenter, &relative);
    u32 mirror = work->mMirror;
    auto* player = camera->mpPlayerActor.get();
    if (mirror)
        eyeOffset.x = -static_cast<f32>(eyeOffset.x);
    cameraStyleRelationalPos(camera, &relative, player, &eyeOffset);
    copyPoint(&camera->mViewCache.mEye, &relative);
    if (cameraStyleLineBGCheck(camera, &camera->mViewCache.mCenter, &camera->mViewCache.mEye,
                               0x8F)) {
        eyeOffset.x = -static_cast<f32>(eyeOffset.x);
        cameraStyleRelationalPos(camera, &reflected, camera->mpPlayerActor.get(), &eyeOffset);
        copyPoint(&camera->mViewCache.mEye, &reflected);
    }
    camera->mViewCache.mFovy = tactFovy;
    ++work->mTimer;
    return 1;
}

/* 02538A54 */
u8 styleEvCamera(u32 i_this) {
    auto* camera = gabi::at<dCamera_c>(i_this);
    if (camera->m108 == 0) {
        camera->m11C = 0;
        camera->mEventData.field_0x08 = 0;
    }
    auto* name = cameraStyleGetEvStringPntData(camera, styleName.text, defaultStyle.text);
    s32 style = cameraStyleSearchStyle(camera->mCamParam, *name);
    runStyleEngine(camera, style);
    u8 done = 0;
    if (camera->m100 != 0 && camera->m101 != 0 && camera->m102 != 0)
        done = 1;
    return done;
}

/* 02538B54 */
u32 saveEvCamera(u32 i_this) {
    auto* camera = gabi::at<dCamera_c>(i_this);
    EventInt slot;
    cameraStyleGetEvIntData(camera, &slot, saveSlot.text, 0);
    s32 index = slot;
    if (index == PLAY_CAMERA_SLOT) {
        f32 fovy = camera->mViewCache.mFovy;
        s16 bank = camera->mViewCache.mBank;
        auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
        auto* saved = reinterpret_cast<CameraInfoView*>(&play->mCameraInfo[0]);
        copyPoint(&saved->mCameraPos, &camera->mViewCache.mCenter);
        copyPoint(&saved->mCameraTarget, &camera->mViewCache.mEye);
        saved->mCameraFovy = fovy;
        saved->mCameraBank = bank;
        SkipSmoother(camera);
        return 1;
    }
    auto* saved = &camera->m0A4[static_cast<u32>(index)];
    copyPoint(&saved->mCenter, &camera->mViewCache.mCenter);
    copyPoint(&saved->mEye, &camera->mViewCache.mEye);
    saved->mFovY = camera->mViewCache.mFovy;
    s16 bank = camera->mViewCache.mBank;
    saved->m1E = 1;
    saved->mBank = bank;
    SkipSmoother(camera);
    return 1;
}

/* 02538C7C */
u32 loadEvCamera(u32 i_this) {
    auto* camera = gabi::at<dCamera_c>(i_this);
    EventInt slot;
    cameraStyleGetEvIntData(camera, &slot, loadSlot.text, 0);
    s32 index = slot;
    if (index == PLAY_CAMERA_SLOT) {
        auto* play = &reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play;
        auto* saved = reinterpret_cast<CameraInfoView*>(&play->mCameraInfo[0]);
        copyPoint(&camera->mViewCache.mCenter, &saved->mCameraPos);
        copyPoint(&camera->mViewCache.mEye, &saved->mCameraTarget);
        camera->mViewCache.mFovy = saved->mCameraFovy;
        cSAngle_l bank;
        auto* result = cameraStyleAngleCtor(&bank, saved->mCameraBank);
        camera->mViewCache.mBank = *result;
        SkipSmoother(camera);
        return 1;
    }
    auto* saved = &camera->m0A4[static_cast<u32>(index)];
    copyPoint(&camera->mViewCache.mCenter, &saved->mCenter);
    copyPoint(&camera->mViewCache.mEye, &saved->mEye);
    camera->mViewCache.mFovy = saved->mFovY;
    camera->mViewCache.mBank = saved->mBank;
    cXyz direction;
    cameraStyleSubtract(&camera->mViewCache.mEye, &direction, &camera->mViewCache.mCenter);
    cSGlobe_Val(&camera->mViewCache.mDirection, &direction);
    SkipSmoother(camera);
    return 1;
}
} // namespace event_camera_style
using namespace event_camera_style;

/* WWHD glue (tools/verify/autoglue.py)
data mapId 1004D0B0
data styleName 1004D288
data defaultStyle 1004D290
data saveSlot 1004D298
data loadSlot 1004D2A0
data tactCenterOffset 1004D1A8
data tactEyeOffset 1004D1B4
data tactFovy 1004D1C0
ext 02530634 u32 cameraStyleGetEvIntData(dCamera_c*, be<s32>*, const char*, s32)
ext 02530AAC be<u32>* cameraStyleGetEvStringPntData(dCamera_c*, const char*, const char*)
ext 02540348 StageEvent* cameraStyleGetStageEventDt(dEvt_control_c*)
ext 025403D4 StageEvent* cameraStyleNextStageEventDt(dEvt_control_c*, StageEvent*)
ext 024FA208 s32 cameraStyleGetCameraTypeFromMapToolID(dCamera_c*, u32, s32)
ext 024F732C s32 cameraStyleSearchStyle(void*, u32)
ext 025E1988 void cameraStyleSoundStart(u32)
ext 0250242C void cameraStyleRelationalPos(dCamera_c*, cXyz*, fopAc_ac_c*, cXyz*)
ext 024FCBE8 bool cameraStyleLineBGCheck(dCamera_c*, cXyz*, cXyz*, u32)
ext 0201ADE0 void cameraStyleSubtract(cXyz*, cXyz*, cXyz*)
ext 0200658C cSAngle_l* cameraStyleAngleCtor(cSAngle_l*, s32)
*/
