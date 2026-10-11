/** Door stop model and motion. Local only. */
#include "d/d_door_smoke.h"
static const f32 l_zero;
static const f32 l_closedHeight;
static const f32 l_closeSpeed;
static const f32 l_closeStep;
static const f32 l_openSpeed;
static const f32 l_openStep;
/* 0252BA9C */
void dDoor_stop_c::calcMtx(dDoor_info_c* door) {
    if (!m8 || !mpModel) return;
    f32 y = door->current.pos.y, offset = mOffsY;
    f32 x = door->current.pos.x, z = door->current.pos.z;
    y = gabi::fadds_ppc(y, offset);
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(door->current.angle.y);
    if (mFrontCheck == 1) mDoMtx_stack_c::YrotM(0x7FFF);
    // HD: capture all twelve floats before reloading the model and writing its matrix.
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) values[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    J3DModel* model = mpModel;
    for (u32 i = 0; i < 12; ++i) model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
}
/* 0252BB9C */
void dDoor_stop_c::closeInit(dDoor_info_c* door) {
    f32 height = l_closedHeight, zero = l_zero;
    mOffsY = height; door->speedF = zero;
    // HD: inline sound helper guards both actor and the wrapping eye-position address.
    if (door && gabi::ea(&door->eyePos)) {
        u32 arg = dDoor_getArg1(door);
        s32 room = door->current.roomNo;
        u32 sound = arg == 17 ? JA_SE_OBJ_CAGE_CLOSE : JA_SE_OBJ_STN_DOOR_STL_BAR;
        u32 reverb = dDoor_getReverb(room);
        dDoor_startSound(sound, &door->eyePos, 0, reverb);
    }
    mB = 1;
}
/* 0252BC3C */
s32 dDoor_stop_c::closeProc(dDoor_info_c* door) {
    if (!mB) return 1;
    f32 target = l_closeSpeed, step = l_closeStep;
    cLib_chaseF(&door->speedF, target, step);
    step = door->speedF; target = l_zero;
    if (!cLib_chaseF(&mOffsY, target, step)) return 0;
    mB = 0;
    return 2;
}
/* 0252BCF4 */
void dDoor_stop_c::openInit(dDoor_info_c* door) {
    f32 zero = l_zero;
    mOffsY = zero; door->speedF = zero;
    if (door && gabi::ea(&door->eyePos)) {
        u32 arg = dDoor_getArg1(door);
        s32 room = door->current.roomNo;
        u32 sound = arg == 17 ? JA_SE_OBJ_CAGE_OPEN : JA_SE_OBJ_STN_DOOR_STL_BAR;
        u32 reverb = dDoor_getReverb(room);
        dDoor_startSound(sound, &door->eyePos, 0, reverb);
    }
    mB = 1;
}
/* 0252BD8C */
s32 dDoor_stop_c::openProc(dDoor_info_c* door) {
    if (!mB) return 1;
    f32 target = l_openSpeed, step = l_openStep;
    cLib_chaseF(&door->speedF, target, step);
    step = door->speedF; target = l_closedHeight;
    if (!cLib_chaseF(&mOffsY, target, step)) return 0;
    mB = 0; m8 = 0;
    return 2;
}
/* 0252BE48 */
BOOL dDoor_stop_c::create() {
    // HD: only the primary stage resource is requested; no GameCube fallback lookup.
    J3DModelData* data = dDoor_getStageRes("Stage", "stop10.bmd");
    if (!data) return TRUE;
    J3DModel* model = mDoExt_J3DModel__create(data, 0, 0x11020203);
    mpModel = model;
    return model != nullptr;
}

/* WWHD glue (tools/verify/autoglue.py)
str 1004C844 "Stage"
str 1004C84C "stop10.bmd"
data l_zero 1004C590
data l_closedHeight 1004C830
data l_closeSpeed 1004C834
data l_closeStep 1004C838
data l_openSpeed 1004C83C
data l_openStep 1004C840
ext 0252AA58 u32 dDoor_getArg1(dDoor_info_c*)
ext 02520540 u32 dDoor_getReverb(s32)
ext 025E1A40 void dDoor_startSound(u32, cXyz*, u32, u32)
ext 0252447C J3DModelData* dDoor_getStageRes(const char*, const char*)
*/
