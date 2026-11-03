/* JAudio sound handle layout from CC0 tww, checked against WWHD.
 * HD moves the GC fields four bytes earlier and appends a dispatch object at 0x40.
 * Legacy game-code aliases retain their established names at offsets 0x14/0x18. */
#pragma once
#include "bindings.h"
struct JAISoundInfoView;
namespace JAInter {
struct Actor;
struct SeParameter;
struct SeqParameter;
struct StreamParameter;
struct SePositionInfo { cXyz current, previous; be<f32> distance; };
struct SeSoundVtable {
    union {
        be<u32> reserved[13];
        struct { be<u32> header[3],destroy,delta10,unknown14,delta18,positionDopplarCommon,delta20,distanceVolumeCommon,delta28,distancePanCommon,delta30; };
    };
    union { be<u32> distance; be<u32> distanceParameters; };
    be<u32> delta38,setDistanceVolume,delta40,setDistancePan,delta48,setDistancePitch,delta50,setDistanceFxmix,delta58,setDistanceFir,delta60,setDistanceDolby,delta68,setPositionDopplar;
};
WWHD_OFFSET(SeSoundVtable,positionDopplarCommon,0x1C);
WWHD_OFFSET(SeSoundVtable,distanceVolumeCommon,0x24);
WWHD_OFFSET(SeSoundVtable,distancePanCommon,0x2C);
WWHD_OFFSET(SeSoundVtable,distanceParameters,0x34);
WWHD_OFFSET(SeSoundVtable,setDistanceVolume,0x3C);
WWHD_OFFSET(SeSoundVtable,setDistancePan,0x44);
WWHD_OFFSET(SeSoundVtable,setDistancePitch,0x4C);
WWHD_OFFSET(SeSoundVtable,setDistanceFxmix,0x54);
WWHD_OFFSET(SeSoundVtable,setDistanceFir,0x5C);
WWHD_OFFSET(SeSoundVtable,setDistanceDolby,0x64);
WWHD_OFFSET(SeSoundVtable,setPositionDopplar,0x6C);
WWHD_SIZE(SeSoundVtable,0x70);
WWHD_SIZE(SePositionInfo, 28);
}
struct JAISound {
    be<u8> track, state, camera, flags;
    be<u8> cameraIndex;
    be<u8> reserved5;
    be<s16> priorityAdjustment;
    be<u32> mSoundID;
    be<u32> elapsed;
    be<u32> fadeCounter;
    union { be<u32> frameCounter; be<u32> field_0x18; };
    union { be<u32> mapInfo; be<u32> field_0x1c; };
    gptr<JAInter::SePositionInfo> positionInfo;
    gptr<void> actor;
    gptr<cXyz> position;
    be<u32> positionState;
    gptr<JAISound> previous, next;
    gptr<gptr<JAISound>> owner;
    gptr<JAInter::SeParameter> parameter;
    gptr<JAISoundInfoView> info;
    gptr<void> extra;
    gptr<JAInter::SeSoundVtable> vtable;

    u32 getID();
    void setSeqPrepareFlag(u32);
    void setStreamPrepareFlag(u32);
    void setPrepareFlag(u32);
    void start(u32);
    void stop(u32);
    void setVolume(f32,u32,u32);
    void setSeqTempoProportion(f32,u32);
    void setTempoProportion(f32,u32);
    u32 getTrackPortRoute(u32,u32);
    u32 checkSoundHandle(u32,JAISoundInfoView*);
    void initParameter(gptr<JAISound>*,JAInter::Actor*,u32,u32,u32,JAISoundInfoView*);

    JAInter::SeqParameter* getSeqParameter();  // 0280B05C
    JAInter::SeParameter* getSeParameter();  // 0280B384
    JAInter::StreamParameter* getStreamParameter();  // 0280B0B4
    u32 getSwBit();  // 0280CFF0
    u32 checkSwBit(u32);  // 0280D300
    u8 getInfoPriority();  // 0280D2C4
    u8 getSeCategoryNumber();  // 0280D2D0
    void clearMainSoundPPointer();  // 0280CB4C

    void setSeqInterVolume(u32,f32,u32);
    void setSeqInterPan(u32,f32,u32);
    void setSeqInterPitch(u32,f32,u32);
    void setSeqInterFxmix(u32,f32,u32);
    void setSeqInterDolby(u32,f32,u32);
    void setStreamInterVolume(u32,f32,u32);
    void setStreamInterPan(u32,f32,u32);
    void setStreamInterPitch(u32,f32,u32);
    void setSeInterVolume(u32,f32,u32,u32);
    void setSeInterPan(u32,f32,u32,u32);
    void setSeInterFxmix(u32,f32,u32,u32);
    void setSeInterDolby(u32,f32,u32,u32);
    void setSeInterPitch(u32,f32,u32,f32);
    void setPan(f32,u32,u32);
    void setPitch(f32,u32,u32);
    void setFxmix(f32,u32,u32);
    void setDolby(f32,u32,u32);
    void setSeqPortData(u32,u32,u32);
    void setPortData(u32,u32);
    void setTrackPortData(u32,u32,u32);
    void setSePortData(u32,u32);
    void setTrackVolume(u32,f32,u32);
    void setTrackInterruptSwitch(u32,u32);
    void setPauseMode(u32,u32);
    void setSeDistanceFir(u8);  // 0280DCDC

    f32 setPositionDopplarCommon(u32);
    f32 setDistanceVolumeCommon(f32,u32);
    f32 setDistancePanCommon();
    void setSeDistanceParameters();
    void setSeDistanceVolume(u32);
    void setSeDistancePan(u32);
    void setSeDistancePitch(u32);
    void setSeDistanceFxmix(u32);
    void setSeDistanceDolby(u32);
    void setSePositionDopplar();
    f32 getSeqInterVolume(u32);
    void setSeInterRandomPara(be<f32>*,u32,f32,f32);  // 0280B38C
};
WWHD_OFFSET(JAISound, mSoundID, 8);
WWHD_OFFSET(JAISound, frameCounter, 0x14);
WWHD_OFFSET(JAISound, field_0x18, 0x14);
WWHD_OFFSET(JAISound, mapInfo, 0x18);
WWHD_OFFSET(JAISound, field_0x1c, 0x18);
WWHD_OFFSET(JAISound, positionInfo, 0x1C);
WWHD_OFFSET(JAISound, actor, 0x20);
WWHD_OFFSET(JAISound, next, 0x30);
WWHD_OFFSET(JAISound, owner, 0x34);
WWHD_OFFSET(JAISound, parameter, 0x38);
WWHD_OFFSET(JAISound, info, 0x3C);
WWHD_OFFSET(JAISound, extra, 0x40);
WWHD_OFFSET(JAISound, vtable, 0x44);
WWHD_SIZE(JAISound, 0x48);
