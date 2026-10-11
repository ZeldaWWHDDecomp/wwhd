// HD: moving-background base ends at 0x3E0, actor tail ends at 0x404.
#pragma once
#include "d/d_bg_s.h"
#include "JSystem/J3DGraph.h"
#include "bindings.h"
namespace daObjAshut {
struct Act_c : dBgS_MoveBgActor {
    enum Mode_e { MODE_UPPER, MODE_U_L, MODE_LOWER, MODE_L_U, MODE_DEMOREQ, MODE_NONE };
    s32 Mthd_Create();
    s32 Mthd_Delete();
    s32 CreateHeap();
    void set_mtx();
    void init_mtx();
    void mode_upper_init();
    void mode_lower_init();
    s32 Create();
    s32 Execute(be<u32>* output);
    s32 Draw();
    u8 chk_safe_area();
    void mode_u_l_init();
    void mode_l_u_init();
    void mode_demoreq_init(s32 mode);
    void mode_upper();
    void mode_u_l();
    void mode_lower();
    void mode_l_u();
    void mode_demoreq();
    s32 IsDelete();
    s32 Delete();
    u32 prm_get(u32 width, u32 shift);
    request_of_phase_process_class phase;
    gptr<J3DModel> model;
    be<s32> mode;
    be<f32> height, velocity;
    be<u8> bounceCount;
    u8 padding;
    be<s16> delay;
    be<u8> demoStarted;
    u8 padding2;
    be<s16> eventIndex;
    be<s32> requestedMode;
};
WWHD_OFFSET(Act_c, phase, 0x3E0);
WWHD_OFFSET(Act_c, model, 0x3E8);
WWHD_OFFSET(Act_c, requestedMode, 0x400);
WWHD_SIZE(Act_c, 0x404);
}

// HD: views of the game-info accessor and the background flag used by this actor.
struct AshutGameInfo { u8 prefix[0x12A0]; u8 mBgS; u8 gap[0x52C4-0x12A1]; u8 mEvtManager; u8 gap2[0x5B2C-0x52C5]; gptr<fopAc_ac_c> mpPlayer; };
struct AshutBackground { be<u32> mId; u8 opaque[0x71]; be<u8> mStickWall; };
WWHD_OFFSET(AshutGameInfo,mBgS,0x12A0);
WWHD_OFFSET(AshutGameInfo,mEvtManager,0x52C4);
WWHD_OFFSET(AshutGameInfo,mpPlayer,0x5B2C);
WWHD_OFFSET(AshutBackground,mStickWall,0x75);
void ashutDestroyActor(daObjAshut::Act_c*, s32);
void ashutDeleteObject(void*);
void ashutConstructMoveBg(daObjAshut::Act_c*);
s32 ashutLoadResource(request_of_phase_process_class*, const char*);
s32 ashutCreateMoveBg(daObjAshut::Act_c*, const char*, u32, u32, u32);
void ashutAssert(const char*, s32, const char*);
AshutGameInfo* ashutGetGameInfo();
void ashutReleaseBackground(void*, void*);
s32 ashutDeleteMoveBg(daObjAshut::Act_c*);
void ashutDeleteResource(request_of_phase_process_class*, const char*);
void* ashutGetModelResource(void*, SafeString*, s32);
J3DModel* ashutCreateModel(void*, u32, u32);
void ashutTranslateMatrix(void*, f32, f32, f32);
void ashutRotateMatrix(void*, s16, s16, s16);
void ashutCopyMatrix(void*, void*);
void ashutSetCullBox(daObjAshut::Act_c*, f32, f32, f32, f32, f32, f32);
s32 ashutGetEventIndex(void*, u32, u32);
void* ashutGetEnvironment();
void ashutSetLight(void*, s32, cXyz*, dKy_tevstr_c*);
void ashutApplyLight(void*, void*, dKy_tevstr_c*);
void ashutUpdateModel(void*, u32);
void ashutSubtractVector(void*, cXyz*, cXyz*);
void ashutRotateY(void*, s16);
void ashutTransformVector(void*, cXyz*, cXyz*);
s32 ashutGetReverb(s8);
void ashutStartSound(u32, cXyz*, u32, s32);
void ashutRegisterBackground(void*, void*, daObjAshut::Act_c*);
u32 ashutGetEventData(void*, s16);
void ashutOrderEvent(daObjAshut::Act_c*, s16, u8, u32, u32, u32);
s32 ashutExecuteMoveBg(daObjAshut::Act_c*);
void ashutRegisterGlobal(void*);
void ashutInitializeHeaderByte(void*);
void ashutInitializeHeaderFlag(void*);

struct AshutActionTable { gabi::ptmf<void (daObjAshut::Act_c::*)()> mEntries[5]; };
struct AshutSaveData { u8 prefix[0x20]; u8 mSwitch; };
struct AshutHeaderStatics { be<f32> mMinAngle,mMaxAngle; be<u8> mHeaderByte,mHeaderFlag; u8 pad[2]; be<u32> mZero[4]; };
struct AshutDestructorRecord { u8 mRecords[3][0xC]; };
WWHD_SIZE(AshutActionTable,40);
WWHD_SIZE(AshutHeaderStatics,0x1C);
WWHD_SIZE(AshutDestructorRecord,0x24);
s32 ashutCheckSwitch(void*, u32, s8);

struct AshutVectorWords { be<u32> x,y,z; };
WWHD_SIZE(AshutVectorWords,12);

WWHD_OFFSET(daObjAshut::Act_c,mode,0x3EC);
WWHD_OFFSET(daObjAshut::Act_c,height,0x3F0);
WWHD_OFFSET(daObjAshut::Act_c,velocity,0x3F4);
WWHD_OFFSET(daObjAshut::Act_c,bounceCount,0x3F8);
WWHD_OFFSET(daObjAshut::Act_c,delay,0x3FA);
WWHD_OFFSET(daObjAshut::Act_c,demoStarted,0x3FC);
WWHD_OFFSET(daObjAshut::Act_c,eventIndex,0x3FE);
