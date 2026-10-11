/**
 * Door key locks and the HD door-stop constructor.
 * Based on zeldaret/tww d/d_door.cpp; verified against WWHD.
 */
#include "d/d_door_key.h"

const gc_tu_data gc_tu = {.safeString = 0x1004C524};

static f32 l_zero = 0.0f;
static f32 l_keyRate = 1.0f;
static f32 l_bossKeyHeight = 230.0f;
static f32 l_keyHeight = 150.0f;
static f32 l_keyDepth = 20.0f;

/* 0252B484 */
s32 dDoor_key2_c::keyResLoad() {
    return dComIfG_resLoad(&mPhs, "Key");
}

/* 0252B494 */
void dDoor_key2_c::keyResDelete() {
    dComIfG_resDelete(&mPhs, "Key");
}

/* 0252B4A4 */
void dDoor_key2_c::keyInit(dDoor_info_c* door) {
    if (mpModel != NULL && mbEnabled && !door->mFrontCheck) {
        // HD: getSwbit is called once; the save call forwards the full result.
        s32 bit = door->getSwbit();
        if (bit < 0x80) {
            dComIfGs_onSwitch(bit, -1);
        }
        if (!mbIsBossDoor) {
            DoorKeyGameInfo* game = dComIfGp_getDoorKeyInfo();
            s32 count = game->mItemKeyNumCount;
            game->mItemKeyNumCount = (u16)(count - 1);
        }
        // HD: select the sound before the reverb callback, retaining its full return register.
        if (door != NULL && gabi::ea(&door->eyePos) != 0) {
            u8 big = mbIsBossDoor;
            s32 room = fopAcM_GetRoomNo(door);
            u32 sound = big == 1 ? JA_SE_OBJ_BOSS_LOCK_OPEN : JA_SE_OBJ_DOOR_CHAIN_OPEN;
            s32 reverb = dComIfGp_getReverb(room);
            mDoAud_seStart(sound, &door->eyePos, 0, reverb);
        }
        m20 = 1;
    } else {
        m20 = 0;
    }
}

/* 0252B5A8 */
void dDoor_key2_c::keyOff() {
    mbEnabled = false;
}

/* 0252B5B4 */
BOOL dDoor_key2_c::keyProc() {
    if (!m20) {
        return TRUE;
    }
    if (!mBckAnim.play()) {
        return FALSE;
    }
    keyOff();
    m20 = 0;
    return TRUE;
}

/* inlined into dDoor_key2_c::keyCreate_Bkey and dDoor_key2_c::keyCreate_Nkey */
static BOOL keyCreate(dDoor_key2_c* key, bool big, const char* name,
                      const char* file, const char* assertion) {
    // HD: both SafeString stack slots live across both resource calls.
    SafeString first;
    SafeString second;
    first.mStringTop = gabi::guest_addr(name);
    first.__vtbl = gc_tu.safeString;
    J3DModelData* modelData = (J3DModelData*)dDoor_getObjectRes(
        dComIfG_resControl(), &first, big ? dRes_INDEX_KEY_BDL_VLOCB_e : dRes_INDEX_KEY_BDL_VLOCN_e);
    if (modelData == NULL) {
        // HD: shorten the path; each branch keeps its original assert-string address.
        JUT_ASSERT_fail(file, big ? 0x35A : 0x342, assertion);
    }
    key->mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (key->mpModel == NULL) {
        return FALSE;
    }
    second.mStringTop = gabi::guest_addr(name);
    second.__vtbl = gc_tu.safeString;
    J3DAnmTransform* bck = (J3DAnmTransform*)dDoor_getObjectRes(
        dComIfG_resControl(), &second, big ? dRes_INDEX_KEY_BCK_VLOCB_e : dRes_INDEX_KEY_BCK_VLOCN_e);
    // HD: boss keys have no skin-deform allocation; the animation result is normalized to BOOL.
    f32 rate = l_keyRate;
    return dDoor_initBck(&key->mBckAnim, modelData, bck, 1, 0, 0, -1, 0, rate) != 0;
}

/* 0252B63C */
BOOL dDoor_key2_c::keyCreate_Bkey() {
    return ::keyCreate(this, true, "Key", "d_door.cpp", "modelData != NULL");
}

/* 0252B738 */
BOOL dDoor_key2_c::keyCreate_Nkey() {
    return ::keyCreate(this, false, "Key", "d_door.cpp", "modelData != NULL");
}

/* 0252B834 */
BOOL dDoor_key2_c::keyCreate(s32 type) {
    mbIsBossDoor = (u8)type;
    return type == 1 ? keyCreate_Bkey() : keyCreate_Nkey();
}

/* 0252B848 */
void dDoor_key2_c::keyOn() {
    mbEnabled = true;
}

/* 0252B854 */
void dDoor_key2_c::calcMtx(dDoor_info_c* door) {
    if (!mbEnabled) {
        return;
    }
    mDoMtx_stack_c::transS(door->current.pos.x, door->current.pos.y, door->current.pos.z);
    mDoMtx_stack_c::YrotM(door->current.angle.y);
    // HD: read the tuning values after YrotM, then copy all twelve floats before storing any.
    bool big = mbIsBossDoor == 1;
    f32 height = big ? l_bossKeyHeight : l_keyHeight;
    f32 depth = l_keyDepth;
    f32 zero = l_zero;
    mDoMtx_stack_c::transM(zero, height, depth);
    f32 values[12];
    for (u32 i = 0; i < 12; ++i) {
        values[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    }
    J3DModel* model = mpModel;
    for (u32 i = 0; i < 12; ++i) {
        model->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    }
}

/* 0252B9D4 */
void dDoor_key2_c::draw(dDoor_info_c* door) {
    J3DModel* model = mpModel;
    J3DModelData* modelData = model->getModelData();
    // HD: the environment callback can replace mpModel; modelData remains the pre-call value.
    dScnKy_env_light_c* env = dKy_getEnvlight();
    model = mpModel;
    setLightTevColorType(env, model, &door->tevStr);
    f32 frame = mBckAnim.getFrame();
    mBckAnim.entry(modelData, frame);
    mDoExt_modelUpdateDL(mpModel, 0);
}

// HD: compiler-generated door-stop constructor allocates when this is NULL.
/* 0252BA44 */
static dDoor_stop_c* dDoor_stop_c_ctor(dDoor_stop_c* i_this) {
    if (i_this == NULL) {
        i_this = (dDoor_stop_c*)operator_new(sizeof(dDoor_stop_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    f32 zero = l_zero;
    i_this->mA = 0;
    i_this->mOffsY = zero;
    i_this->mB = 0;
    i_this->mFrontCheck = 0;
    i_this->mpModel = NULL;
    i_this->m8 = 0;
    return i_this;
}

/* WWHD glue (tools/verify/autoglue.py):
str 1004C7D8 "Key" in 0252B484
str 1004C7DC "Key" in 0252B494
str 1004C7E0 "Key" in 0252B63C
str 1004C804 "Key" in 0252B738
str 1004C7E4 "d_door.cpp" renamed in 0252B63C
str 1004C808 "d_door.cpp" renamed in 0252B738
str 1004C7F0 "modelData != NULL" renamed in 0252B63C
str 1004C814 "modelData != NULL" renamed in 0252B738
data l_zero 1004C590
data l_keyRate 1004C7C8
data l_bossKeyHeight 1004C828
data l_keyHeight 1004C698
data l_keyDepth 1004C82C
ext 0252AA14 s32 dDoor_info_c::getSwbit()
ext 025200D4 DoorKeyGameInfo* dComIfGp_getDoorKeyInfo()
ext 026066C4 void* dDoor_getObjectRes(dRes_control_c*, SafeString*, s32)
ext 025E8508 BOOL dDoor_initBck(mDoExt_bckAnm*, J3DModelData*, J3DAnmTransform*, s32, s32, s32, s32, s32, f32)
*/
