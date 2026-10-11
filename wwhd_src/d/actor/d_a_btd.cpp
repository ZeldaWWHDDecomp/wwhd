/* Boss - Gohma (Dragon Roost Cavern).
 * GameCube names and layouts on the verified WWHD implementation.
 * HD: the action dispatcher includes wait, attacks and damage; damage_check is a separate unit.
 * HD: hahen_set includes the fragment-initialization helper.
 */
#include "d/actor/d_a_btd.h"
#include "gabi.h"

namespace {
static daBtd_HIO_c l_HIO;
static u32 l_safeStringVtable;
static u32 l_shapeAttributeBaseVtable;
static u32 l_cylinderAabVtable;
static u32 l_hitInfoVtable;
static u32 l_cylinderVtable;
static u32 l_cylinderShapeVtable;
static u32 l_hioVtable;
static u32 l_actorVtable;
static u32 l_collisionStatusVtable;
static u32 l_gameCollisionStatusVtable;
static u32 l_centerRegistration;
static u32 l_firstByteRegistration;
static u32 l_secondByteRegistration;
static u32 l_hioName;
static GXColor l_effectColor;
static dCcD_SrcSph at_sph_src;
static dCcD_SrcSph eye_sph_src;
static dCcD_SrcSph fire_sph_src;
static dCcD_SrcSph sibuki_sph_src;
static dCcD_SrcCyl hand_cyl_src;

static BtdFloat l_negativePi;
static BtdFloat l_positivePi;
static BtdWord l_endPositionInitialized;
static cXyz l_endPosition;
static BtdEmitterPointer l_ykgrEmitter;
static BtdByte l_ykgrAlphaFlag;

static BtdActorPointer kui;
static BtdActorPointer dr2;
static BtdFloat l_constructorFloat1;
static BtdFloat l_constructorFloat2;
static BtdByte l_stateByte1;
static BtdByte l_stateByte2;
static BtdConstructorWords l_constructorWords;
static cXyz static_center_pos;
static BtdByte l_blurEnabled;
static BtdByte l_blurAlpha;
static Mtx34 l_matrixStack;

static BtdLightObjectHD lightDefaults;
static BtdDebugRegisters l_debugRegisters;
static const s32 jab_bck[] = {38, 39, 40, 0, 0, 27, 28, 29};
static const s32 punch_bck[] = {33, 34, 35, 36, 37, 22, 23, 24, 25, 26};
static const u16 hibi_eff[] = {32968, 32969, 32970};
static const s32 hibi_brk[] = {75, 76, 77};
static const u16 hahen_effname[] = {32964, 32964, 32964, 32964, 32965, 32965,
                                    32965, 32965, 32966, 32967, 32967, 32967};
static const s32 hahen_effindex[] = {46, 33, 36, 66, 49, 39, 42, 70, 55, 32, 45, 52};
static const s32 eff_index[] = {67, 71, 32, 45, 52};
static const u16 last_effname[] = {32978, 32979, 32980, 32976, 32976, 32976, 32976,
                                   32976, 32976, 32976, 32976, 32977, 32977, 32977,
                                   32977, 32977, 32977, 32977, 32977};
static const u16 eff_name[] = {32929, 32929, 32930, 32931, 32932};
static const u16 effF_name[] = {32934, 32935, 32928, 32927, 32945, 32947};
static const u16 effR_name[] = {32934, 32935, 32928, 32927, 32944, 32946};
static const s8 joint_check[] = {-1, 5,  -1, -1,  17,  -1, -1, -1, -1, -1, 18, -1, -1, -1, 6,
                                 -1, -1, -1, -1,  -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                 -1, -1, 4,  -1,  -1,  13, -1, -1, 15, -1, -1, 14, -1, -1, 17,
                                 3,  -1, -1, 11,  -1,  -1, 12, -1, -1, -1, 2,  -1, -1, -1, 100,
                                 -1, 0,  1,  101, 102, -1, -1, 7,  8,  -1, -1, 9,  10};
static const s32 last_eff_index[] = {32, 45, 52, 66, 67, 46, 47, 33, 34, 36,
                                     37, 70, 71, 49, 50, 39, 40, 42, 43};
static const f32 at_size[] = {100.0f, 100.0f, 250.0f, 300.0f, 300.0f, 300.0f, 300.0f,
                              100.0f, 250.0f, 100.0f, 250.0f, 150.0f, 150.0f, 150.0f,
                              150.0f, 150.0f, 150.0f, 150.0f, 150.0f};
static const f32 handposX[] = {630.0f, 750.0f, 700.0f, -630.0f, -750.0f, -700.0f};
static const f32 handposY[] = {0.0f, 100.0f, 0.0f, 0.0f, 100.0f, 0.0f};
static const f32 handposZ[] = {1680.0f, 1600.0f, 1320.0f, 1680.0f, 1600.0f, 1320.0f};
static const f32 handposH[] = {250.0f, 200.0f, 200.0f, 250.0f, 200.0f, 200.0f};
static const f32 handposR[] = {30.0f, 170.0f, 180.0f, 30.0f, 170.0f, 180.0f};

inline btd_class *btdActor(u32 p) {
    return gabi::at<btd_class>(p);
}
u32 play() {
    return btdGuest_getPlay();
}
u32 matrix() {
    return gabi::ea(calc_mtx());
}
void copy3(u32 dst, u32 src) {
    u32 x = btdWord(btdVector(src)->x), y = btdWord(btdVector(src)->y),
        z = btdWord(btdVector(src)->z);
    btdSetWord(btdVector(dst)->x, x);
    btdSetWord(btdVector(dst)->y, y);
    btdSetWord(btdVector(dst)->z, z);
}
/* inlined into move, end, daBtd_Execute */
u32 resource(u32 name, s32 index) {

    SafeString text;

    text.mStringTop = name;
    text.__vtbl = gabi::ea(&l_safeStringVtable);
    u32 controller = gabi::ea(dComIfG_resControl());
    return btdGuest_dRes_control_c_getRes(controller, &text, index);
}
} // namespace

namespace {
void writeJointMatrix(u32 model, u32 index, bool finalBranch = false) {
    u32 block = (u32)btdModel(model)->mpMtxBuffer.v, source = matrix();
    u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
    u32 matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
    (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)btdMatrix(source)->m[i / 4][i % 4];
    const u8 regular[] = {0, 4, 8, 5, 6, 7, 1, 2, 3, 9, 10, 11};
    const u8 final[] = {0, 1, 2, 3, 5, 6, 7, 4, 8, 9, 10, 11};
    for (u8 i : finalBranch ? final : regular)
        (btdMatrix(matrices + index * sizeof(Mtx34))->m[i / 4][i % 4] = values[i]);
    source = matrix();
    PSMTXCopy(btdMatrix(source), J3DSys::mCurrentMtx);
}
} // namespace

namespace {
void actorSound(u32 p, u32 id) {
    if (!(p + 0x37C))
        return;
    s32 actorId = p ? (s32)(u32)btdActor(p)->mBsPcId : -1;
    s32 room = (s8)btdActor(p)->current.roomNo;
    s32 reverb = btdGuest_dComIfGp_getReverb(room);
    btdGuest_seStartMonster(id, gabi::ea(&btdActor(p)->eyePos), actorId, 0, reverb);
}
// Most monster-sound sites in move test the actor pointer itself (cmpwi r31,0) before the sound
// position test (addic. r31,0x37C); the others test only the position.
void actorSoundNN(u32 p, u32 id) {
    if (p)
        actorSound(p, id);
}
} // namespace

namespace {
void installGohmaJoints(u32 actor, be<u32> &morphSlot) {
    u32 morf = (u32)morphSlot, model = (u32)btdMorph(morf)->mpModel.v,
        data = (u32)btdModel(model)->mModelData.v;
    u32 count = btdModelData(data)->getJointNum(), index = 0;
    while (index < count) {
        if (index < 73 && (s8)joint_check[index] >= 0) {
            morf = (u32)morphSlot;
            model = (u32)btdMorph(morf)->mpModel.v;
            data = (u32)btdModel(model)->mModelData.v;
            u32 entries = (u32)btdModelData(data)->mJointNum,
                joint = (u32)btdModelData(data)->mJointNodes;
            if (index < entries)
                joint += index * 28;
            btdJoint(joint)->callback = gabi::guest_fn((const void *)&nodeCallBack);
        }
        morf = (u32)morphSlot;
        model = (u32)btdMorph(morf)->mpModel.v;
        index = (index + 1) & 0xFFFF;
        data = (u32)btdModel(model)->mModelData.v;
        count = btdModelData(data)->getJointNum();
    }
    morf = (u32)morphSlot;
    model = (u32)btdMorph(morf)->mpModel.v;
    (btdModel(model)->mUserArea = (u32)(actor));
}
u32 newGohmaAnimation(be<u32> &animationSlot, bool texture, u32 assertionLine, u32 assertionText,
                      bool assertion) {
    u32 handle = btdGuest_allocate(texture ? 0x74 : 0x78);
    if (handle)
        handle = (texture ? btdGuest_btkConstruct(handle) : btdGuest_brkConstruct(handle));
    animationSlot = handle;
    if (!handle && assertion)
        btdGuest_JUTAssertion_getSDevice(gabi::guest_addr("d_a_btd.cpp"), assertionLine,
                                         assertionText);
    return handle;
}
s32 initGohmaAnimation(be<u32> &animationSlot, u32 model, u32 asset, bool texture, s32 loop,
                       s32 second) {
    u32 animation = resource(gabi::guest_addr("Btd"), asset);
    u32 handle, data;
    // Phase one's native code loads the animation handle before the model data;
    // the other groups load data first. Neither load crosses a guest call.
    handle = (u32)animationSlot;
    data = (u32)btdModel(model)->mModelData.v;
    return (texture ? btdGuest_btkInit(handle, data, animation, 1, loop, 1.0f, 0, -1, second)
                    : btdGuest_brkInit(handle, data, animation, 1, loop, 1.0f, 0, -1, second));
}
} // namespace

namespace {
void btdActionAnimation(btd_class *i_this, s32 index, f32 blend, u8 loop = 2, f32 speed = 1.0f,
                        s32 sound = -1) {
    const u32 p = gabi::ea(i_this);
    btdGuest_anm_init(p, index, blend, loop, speed, sound);
}
void btdActionWait(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    u32 player = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;
    play();
    s32 playerAngle =
        btdGuest_fopAcM_searchActorAngleYFullResult(p, (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v);
    s16 delta = s16((s16)i_this->current.angle.y - playerAngle);
    u16 view = u16(delta < 0 ? s16(-delta) : delta);
    f32 height = (f32)l_debugRegisters.floats[5] + 400.0f;
    bool high = (f32)btdPlayer(player)->current.pos.y > height;
    u16 state = u16((s16)i_this->mGohmaState);
    switch (state) {
    case 0:
        btdActionAnimation(btdActor(p), 0x33, 20.0f);
        i_this->mGohmaState = (s16)(s16((s16)i_this->mGohmaState + 1));
        [[fallthrough]];
    case 1:
        if (view < 0x1800 && (s16)i_this->m02EC[0] == 0) {
            i_this->m02E4 = ACT_ATTACK;
            i_this->mGohmaState = (s16)(0);
            f32 choice = cM_rndF(1.0f);
            if (choice < 0.5f) {
                i_this->m02E9 = (s8)(0);
                i_this->m02EA = (s8)(1);
            } else {
                i_this->m02E9 = (s8)(5);
                i_this->m02EA = (s8)(0);
            }
            u8 forced = (u8)l_HIO.m10;
            if (forced == 0) {
                if (view > 0x400) {
                    f32 turnChoice = cM_rndF(1.0f);
                    if (turnChoice < 0.5f || view > 0x800) {
                        i_this->mAttackType = (s8)(5);
                        break;
                    }
                }
                bool phase = (u8)i_this->m02E0 != 0;
                f32 first = cM_rndF(1.0f);
                if (first < (phase ? (f32)l_HIO.m20 : (f32)l_HIO.m18))
                    i_this->mAttackType = (s8)(0);
                else {
                    f32 second = cM_rndF(1.0f);
                    i_this->mAttackType =
                        (s8)(second < (phase ? (f32)l_HIO.m24 : (f32)l_HIO.m1C) ? 1 : 2);
                }
            } else if (forced <= 3)
                i_this->mAttackType = (s8)(forced - 1);
            else if (forced == 4) {
                i_this->mAttackType = (s8)(5);
                i_this->m02E9 = (s8)(5);
                i_this->m02EA = (s8)(0);
            }
        } else {
            i_this->m5E94 = (s16)(0x100);
            if (view >= 0x6000 && (s16)i_this->m02EC[1] == 0) {
                i_this->mGohmaState = (s16)(2);
                btdActionAnimation(btdActor(p), 0x15, 10.0f);
                i_this->m02EC[0] = (s16)(150);
                i_this->m5E86 = (u8)(1);
            } else {
                if (high) {
                    u32 peg = (u32)kui.v;
                    i_this->mGohmaState =
                        (s16)(peg != 0 && (s8)btdBaseActor(peg)->health == 3 ? 10 : 20);
                }
                i_this->m5E86 = (u8)(1);
            }
        }
        break;
    case 2:
        if (view < 0x6000 || (s16)i_this->m02EC[0] == 0) {
            i_this->mGohmaState = (s16)(0);
            i_this->m02EC[1] = (s16)(40);
        }
        break;
    case 10:
        btdActionAnimation(btdActor(p), 0x2F, 20.0f);
        i_this->mGohmaState = (s16)(s16((s16)i_this->mGohmaState + 1));
        i_this->m02EC[0] = (s16)(300);
        {
            s16 timer = s16(gabi::ftoi(cM_rndF(80.0f) + 90.0f));
            i_this->m02EC[1] = (s16)(timer);
        }
        [[fallthrough]];
    case 11:
        if ((s16)i_this->m02EC[1] == 0 && (s16)i_this->m02EC[0] > 150) {
            actorSoundNN(p, 0x483E);
            i_this->m02EC[1] = (s16)(s16(gabi::ftoi(cM_rndF(80.0f) + 90.0f)));
        }
        i_this->m5E86 = (u8)(3);
        i_this->m5E94 = (s16)(0x100);
        {
            s16 facing = (s16)btdPlayer(player)->shape_angle.y, timer = (s16)i_this->m02EC[0];
            i_this->m5E90 = (s16)(s16(facing + 0x4000));
            if (!high)
                i_this->mGohmaState = (s16)(0);
            else {
                u32 peg = (u32)kui.v;
                if (peg != 0 && (s8)btdBaseActor(peg)->health != 3)
                    i_this->mGohmaState = (s16)(0);
            }
            if (timer == 0) {
                i_this->mAttackType = (s8)(3);
                i_this->m02E4 = ACT_ATTACK;
                i_this->mGohmaState = (s16)(0);
            }
        }
        break;
    case 20:
        btdActionAnimation(btdActor(p), 0x39, 20.0f);
        i_this->mGohmaState = (s16)(s16((s16)i_this->mGohmaState + 1));
        i_this->m02EC[0] = (s16)(150);
        [[fallthrough]];
    case 21: {
        s16 timer = (s16)i_this->m02EC[0];
        i_this->m5E94 = (s16)(0x100);
        i_this->m5E86 = (u8)(1);
        if (!high)
            i_this->mGohmaState = (s16)(0);
        else {
            u32 peg = (u32)kui.v;
            if (peg != 0 && (s8)btdBaseActor(peg)->health == 3)
                i_this->mGohmaState = (s16)(0);
        }
        if (timer == 0) {
            i_this->mAttackType = (s8)(4);
            i_this->m02E4 = ACT_ATTACK;
            i_this->mGohmaState = (s16)(0);
        }
    } break;
    }
}
} // namespace

namespace {
bool btdActionStopped(u32 morf) {
    return ((u8)btdMorph(morf)->mFrameCtrl.mState & 1) != 0 ||
           (f32)btdMorph(morf)->mFrameCtrl.mRate == 0.0f;
}
void btdActionReset(btd_class *i_this) {
    i_this->m02E4 = ACT_WAIT;
    i_this->mGohmaState = (s16)(0);
    f32 delay = cM_rndF(50.0f);
    i_this->m02EC[0] = (s16)(s16(gabi::ftoi(delay + (f32)l_HIO.m14)));
}
void btdActionLevelSound(btd_class *i_this, u32 id, u32 position) {
    s32 reverb = btdGuest_dComIfGp_getReverb(s32((s8)i_this->current.roomNo));
    mDoAud_seStart(id, btdVector(position), 0, reverb);
}
void btdActionBodyLevelSound(btd_class *i_this, u32 id) {
    const u32 p = gabi::ea(i_this);
    if (p != 0 && p + 0x37C != 0)
        btdActionLevelSound(btdActor(p), id, p + 0x37C);
}
void btdActionNextAnimation(btd_class *i_this, const be<s32> *table, f32 blend, u8 loop = 0) {
    const u32 p = gabi::ea(i_this);
    s8 index = s8((u8)i_this->m02E9 + 1);
    i_this->m02E9 = (s8)(u8(index));
    s32 animation = (s32)table[index];
    btdActionAnimation(btdActor(p), animation, blend, loop);
}
void btdActionIncrement(btd_class *i_this) {
    i_this->mGohmaState = (s16)(s16((s16)i_this->mGohmaState + 1));
}
void btdActionJab(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    u32 morf = btdGuest_get_anm(p);
    switch (u16((s16)i_this->mGohmaState)) {
    case 0:
        btdActionAnimation(btdActor(p), (s32)jab_bck[(u32(s32((s8)i_this->m02E9) * 4)) / 4], 5.0f,
                           0);
        actorSoundNN(p, 0x4834);
        btdActionIncrement(btdActor(p));
        break;
    case 1:
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), jab_bck, 0.0f);
            btdActionIncrement(btdActor(p));
            s32 joint = (s8)i_this->m02EA != 0 ? 10 : 8;
            btdActionLevelSound(btdActor(p), 0x5818,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
        }
        break;
    case 2:
        i_this->m5E84 = (u8)(1);
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), jab_bck, 0.0f);
            btdActionIncrement(btdActor(p));
        }
        break;
    case 3:
        if (btdActionStopped(morf))
            btdActionReset(btdActor(p));
        break;
    }
}
/* inlined into move, end, daBtd_Execute */
void btdActionPunch(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    u32 morf = btdGuest_get_anm(p);
    s16 state = (s16)i_this->mGohmaState;
    switch (u16(state)) {
    case 0:
        btdActionAnimation(btdActor(p), (s32)punch_bck[(u32(s32((s8)i_this->m02E9) * 4)) / 4], 5.0f,
                           0);
        btdActionIncrement(btdActor(p));
        actorSoundNN(p, 0x4835);
        state = (s16)i_this->mGohmaState;
        break;
    case 1:
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), punch_bck, 0.0f);
            btdActionIncrement(btdActor(p));
            s32 joint = (s8)i_this->m02EA != 0 ? 10 : 8;
            btdActionLevelSound(btdActor(p), 0x5819,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
            state = (s16)i_this->mGohmaState;
        }
        break;
    case 2:
        i_this->m5E84 = (u8)(2);
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), punch_bck, 0.0f);
            btdActionIncrement(btdActor(p));
            s32 joint = (s8)i_this->m02EA != 0 ? 10 : 8;
            btdActionLevelSound(btdActor(p), 0x581B,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
            btdGuest_hahen_set(p, s32((s8)i_this->m02EA));
            u32 game = play();
            s32 magnitude = (s16)l_debugRegisters.shorts[0] + 5;

            cXyz direction;
            direction.x = 0.0f;
            direction.y = 1.0f;
            direction.z = 0.0f;
            btdGuest_dVibration_c_StartShock(game + 0x599C, magnitude, -33, &direction);
            state = (s16)i_this->mGohmaState;
        } else
            state = (s16)i_this->mGohmaState;
        break;
    case 3:
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), punch_bck, 0.0f, 2);
            f32 random = cM_rndF(100.0f);
            f32 timer = random + (f32)l_HIO.m28;
            state = s16((s16)i_this->mGohmaState + 1);
            i_this->m02EC[0] = (s16)(s16(gabi::ftoi(timer)));
            i_this->mGohmaState = (s16)(state);
        }
        break;
    case 4: {
        s32 joint = (s8)i_this->m02EA != 0 ? 10 : 8;
        if (gabi::ftoi((f32)btdMorph(morf)->mFrameCtrl.mFrame) == 2) {
            btdActionLevelSound(btdActor(p), 0x581C,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
            if (p + 0x37C != 0)
                actorSound(p, 0x4838);
        }
        if ((s16)i_this->m02EC[0] == 0) {
            btdActionNextAnimation(btdActor(p), punch_bck, 5.0f);
            btdActionIncrement(btdActor(p));
            actorSoundNN(p, 0x4839);
            btdActionLevelSound(btdActor(p), 0x581D,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
            i_this->m6028[5] = (s16)(23);
            i_this->m6028[4] = (s16)(23);
            i_this->m6028[6] = (s16)(3);
        }
        state = (s16)i_this->mGohmaState;
        break;
    }
    case 5:
        if (btdActionStopped(morf)) {
            i_this->m02E4 = ACT_WAIT;
            i_this->mGohmaState = (s16)(0);
            f32 delay = cM_rndF(50.0f);
            f32 total = delay + (f32)l_HIO.m14;
            state = (s16)i_this->mGohmaState;
            i_this->m02EC[0] = (s16)(s16(gabi::ftoi(total)));
        }
        break;
    }
    if (state == 3 || state == 4)
        i_this->m5E7C = (u8)(u8((u8)i_this->m02EA + 2));
}
} // namespace

namespace {
void btdActionFireMatrix(u32 morf) {
    u32 block = (u32)btdModel((u32)btdMorph(morf)->mpModel.v)->mpMtxBuffer.v;
    u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
    u32 matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
    (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
    PSMTXCopy(btdMatrix(matrices + 0xAE0), btdMatrix(matrix()));
}
/* inlined into move, end, daBtd_Execute */
void btdActionFireCollision(btd_class *i_this, u32 morf, bool swept) {
    const u32 p = gabi::ea(i_this);
    btdActionFireMatrix(morf);
    f32 extent = (f32)i_this->m5E80;

    cXyz local;
    cXyz position;

    local.x = 0.0f;
    local.y = -200.0f * extent;
    local.z = 1000.0f * extent;
    MtxPosition(&local, &position);
    btdGuest_cM3dGSph_SetR(gabi::ea(&i_this->mFireSph.mSph), 200.0f * (f32)i_this->m5E80);
    if (swept) {
        bool first = (s16)i_this->m02EC[0] == 1;
        if (first)
            btdGuest_startFireAttack(gabi::ea(&i_this->mFireSph), &position);
        else
            btdGuest_moveFireAttack(gabi::ea(&i_this->mFireSph), &position);
    } else
        btdGuest_cM3dGSph_SetC(gabi::ea(&i_this->mFireSph.mSph), &position);
    btdGuest_cCcS_Set(gabi::ea(&btdPlay()->mCcS), gabi::ea(&i_this->mFireSph));
}
void btdActionFire(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    u32 morf = btdGuest_get_anm(p);
    s16 state = (s16)i_this->mGohmaState;
    i_this->m5E86 = (u8)(2);
    switch (u16(state)) {
    case 0:
        btdActionAnimation(btdActor(p), 0xD, 5.0f, 0);
        btdActionIncrement(btdActor(p));
        state = (s16)i_this->mGohmaState;
        i_this->m6E7C = (f32)(0.0f);
        i_this->m02EC[0] = (s16)(40);
        break;
    case 1: {
        s16 target = s16((s16)l_debugRegisters.shorts[4] + 16);
        s16 timer = (s16)i_this->m02EC[0];
        if (timer == target) {
            btdGuest_hahen_set2(p);
            timer = (s16)i_this->m02EC[0];
        }
        if (timer == 31)
            actorSoundNN(p, 0x483A);
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), 0xE, 0.0f, 0);
            btdActionIncrement(btdActor(p));
        }
        state = (s16)i_this->mGohmaState;
        break;
    }
    case 2:
        if (btdActionStopped(morf)) {
            if ((u8)i_this->m02E0 != 0) {
                i_this->mGohmaState = (s16)(6);
                btdActionAnimation(btdActor(p), 0x11, 0.0f, 0);
                actorSoundNN(p, 0x5810);
            } else {
                btdActionAnimation(btdActor(p), 0xF, 0.0f, 2);
                i_this->m02EC[0] = (s16)(s16((s16)l_HIO.m2C * 9));
                i_this->mGohmaState = (s16)(3);
            }
        }
        state = (s16)i_this->mGohmaState;
        break;
    case 3: {
        i_this->mKankyoState = (u8)(1);
        btdActionBodyLevelSound(btdActor(p), 0x501E);
        if ((s16)i_this->m02EC[0] > 9)
            i_this->m6028[1] = (s16)(1);
        s16 spawnTimer = s16((s16)l_debugRegisters.shorts[4] + 20), timer = (s16)i_this->m02EC[0];
        i_this->m6028[0] = (s16)(spawnTimer);
        if (timer == 0) {
            if ((u8)i_this->m02E0 != 0) {
                btdActionAnimation(btdActor(p), 0x11, 0.0f, 0);
                i_this->mGohmaState = (s16)(5);
                actorSoundNN(p, 0x5810);
            } else {
                btdActionAnimation(btdActor(p), 0x10, 0.0f, 0);
                btdActionIncrement(btdActor(p));
                i_this->m02EC[0] = (s16)(s16((s16)l_debugRegisters.shorts[1] + 25));
                btdActionBodyLevelSound(btdActor(p), 0x581A);
            }
            i_this->m600C[3].v = 0;
        }
        state = (s16)i_this->mGohmaState;
        break;
    }
    case 4: {
        i_this->m6028[2] = (s16)(12);
        i_this->m6028[3] = (s16)(3);
        if (btdActionStopped(morf))
            btdActionReset(btdActor(p));
        if ((s16)i_this->m02EC[0] > 1)
            i_this->m5E80 = (f32)(0.0f);
        else {
            i_this->mKankyoState = (u8)(2);
            btdActionBodyLevelSound(btdActor(p), 0x501F);
            btdActionFireCollision(btdActor(p), morf, true);
            f32 next = (f32)i_this->m5E80 + 0.1f;
            if (next > 1.0f)
                i_this->m5E80 = (f32)(0.0f);
            else
                i_this->m5E80 = (f32)(next);
        }
        state = (s16)i_this->mGohmaState;
        break;
    }
    case 5:
        i_this->m5E84 = (u8)(3);
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), 0x10, 0.0f, 0);
            i_this->mGohmaState = (s16)(4);
            i_this->m02EC[0] = (s16)(s16((s16)l_debugRegisters.shorts[1] + 25));
            btdActionBodyLevelSound(btdActor(p), 0x581A);
        }
        state = (s16)i_this->mGohmaState;
        break;
    case 6:
        i_this->m5E84 = (u8)(3);
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), 0xF, 15.0f, 2);
            i_this->m02EC[0] = (s16)(s16((s16)l_HIO.m2C * 4));
            i_this->mGohmaState = (s16)(3);
        }
        state = (s16)i_this->mGohmaState;
        break;
    }
    if (state >= 2 || (s16)i_this->m02EC[0] < 16)
        i_this->m5E7C = (u8)(1);
}
} // namespace

namespace {
/* inlined into move, end, daBtd_Execute */
void btdActionHighFire(btd_class *i_this, bool sideFire) {
    const u32 p = gabi::ea(i_this);
    u32 player = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;
    u32 morf = btdGuest_get_anm(p);
    u16 state = u16((s16)i_this->mGohmaState);
    i_this->m5E86 = (u8)(sideFire ? 1 : 3);
    switch (state) {
    case 0:
        btdActionAnimation(btdActor(p), sideFire ? 0x36 : 0x30, 5.0f, 0);
        btdActionIncrement(btdActor(p));
        i_this->m02EC[0] = (s16)(40);
        break;
    case 1:
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), sideFire ? 0x37 : 0x31, 1.0f, 2);
            s16 duration = (s16)l_HIO.m2C, next = s16((s16)i_this->mGohmaState + 1);
            i_this->m02EC[0] = (s16)(s16(duration * (sideFire ? 7 : 9)));
            i_this->mGohmaState = (s16)(next);
        }
        break;
    case 2: {
        i_this->mKankyoState = (u8)(1);
        btdActionBodyLevelSound(btdActor(p), 0x501E);
        s16 timer = (s16)i_this->m02EC[0];
        i_this->m6028[1] = (s16)(1);
        s16 sprayTimer = s16((s16)l_debugRegisters.shorts[4] + 20);
        i_this->m6028[0] = (s16)(sprayTimer);
        if (timer == 0) {
            btdActionAnimation(btdActor(p), sideFire ? 0x38 : 0x32, 5.0f, 0);
            btdActionIncrement(btdActor(p));
            i_this->m02EC[0] = (s16)(s16((s16)l_debugRegisters.shorts[1] + 25));
            btdActionBodyLevelSound(btdActor(p), 0x581A);
            i_this->m600C[3].v = 0;
        }
        break;
    }
    case 3: {
        if (sideFire) {

            cXyz difference;

            btdGuest_cXyz_op_sub(gabi::ea(&btdPlayer(player)->eyePos), &difference,
                                 gabi::ea(&i_this->eyePos));
            f32 z = difference.z, x = difference.x, y = difference.y;
            f32 length = btdGuest_sqrtf(gabi::fmadds(x, x, z * z));
            s32 pitch = btdGuest_cM_atan2sFullResult(y, length);
            i_this->m6028[2] = (s16)(12);
            i_this->m6028[3] = (s16)(3);
            s16 timer = (s16)i_this->m02EC[0];
            i_this->m5E8C = (s16)(s16(-pitch - 2500));
            if (timer != 0)
                i_this->m5E80 = (f32)(0.0f);
            else {
                i_this->mKankyoState = (u8)(2);
                btdActionBodyLevelSound(btdActor(p), 0x501F);
                btdActionFireCollision(btdActor(p), morf, false);
                f32 next = (f32)i_this->m5E80 + 0.1f;
                i_this->m5E80 = (f32)(next);
                f32 limit = (f32)l_debugRegisters.floats[9] + 1.0f;
                if (next > limit)
                    i_this->m5E80 = (f32)(0.0f);
            }
        } else {
            s16 timer = (s16)i_this->m02EC[0];
            i_this->m6028[2] = (s16)(12);
            i_this->m6028[3] = (s16)(3);
            if (timer != 0)
                i_this->m5E80 = (f32)(0.0f);
            else {
                i_this->mKankyoState = (u8)(2);
                btdActionBodyLevelSound(btdActor(p), 0x501F);
                btdActionFireCollision(btdActor(p), morf, false);
                f32 next = (f32)i_this->m5E80 + 0.1f;
                // Native ble keeps unordered values; only an ordered greater result resets.
                i_this->m5E80 = (f32)(next > 1.0f ? 0.0f : next);
            }
        }
        if (btdActionStopped(morf))
            btdActionReset(btdActor(p));
        break;
    }
    }
    f32 threshold = (f32)l_debugRegisters.floats[5] + 400.0f;
    bool reset = (f32)btdPlayer(player)->current.pos.y < threshold;
    if (!reset) {
        u32 peg = (u32)kui.v;
        if (peg != 0)
            reset =
                sideFire ? (s8)btdBaseActor(peg)->health == 3 : (s8)btdBaseActor(peg)->health != 3;
    }
    if (reset)
        btdActionReset(btdActor(p));
    if (sideFire)
        i_this->m5E94 = (s16)(0x200);
}
/* inlined into move, end, daBtd_Execute */
void btdActionTurningPunch(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    u32 morf = btdGuest_get_anm(p);
    switch (u16((s16)i_this->mGohmaState)) {
    case 0:
        btdActionAnimation(btdActor(p), (s32)punch_bck[(u32(s32((s8)i_this->m02E9) * 4)) / 4], 5.0f,
                           0);
        btdActionIncrement(btdActor(p));
        actorSoundNN(p, 0x4835);
        break;
    case 1:
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), punch_bck, 0.0f);
            btdActionIncrement(btdActor(p));
            s32 joint = (s8)i_this->m02EA != 0 ? 10 : 8;
            btdActionLevelSound(btdActor(p), 0x5819,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
        }
        i_this->m5E92 = (s16)(s16((s16)l_debugRegisters.shorts[0] + 15));
        i_this->m5E94 = (s16)(s16((s16)l_debugRegisters.shorts[1] + 0xC00));
        break;
    case 2:
        i_this->m5E84 = (u8)(2);
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), punch_bck, 0.0f);
            btdActionIncrement(btdActor(p));
            s32 joint = (s8)i_this->m02EA != 0 ? 10 : 8;
            btdActionLevelSound(btdActor(p), 0x581B,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
            btdGuest_hahen_set(p, s32((s8)i_this->m02EA));
            u32 game = play();
            s32 magnitude = (s16)l_debugRegisters.shorts[0] + 5;

            cXyz direction;
            direction.x = 0.0f;
            direction.y = 1.0f;
            direction.z = 0.0f;
            btdGuest_dVibration_c_StartShock(game + 0x599C, magnitude, -33, &direction);
        }
        break;
    case 3:
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), punch_bck, 0.0f);
            f32 timer = cM_rndF(80.0f) + 80.0f;
            s16 next = s16((s16)i_this->mGohmaState + 1);
            i_this->m02EC[0] = (s16)(s16(gabi::ftoi(timer)));
            i_this->mGohmaState = (s16)(next);
        }
        break;
    case 4: {
        s32 joint = (s8)i_this->m02EA != 0 ? 10 : 8;
        if (btdActionStopped(morf)) {
            btdActionNextAnimation(btdActor(p), punch_bck, 5.0f);
            btdActionIncrement(btdActor(p));
            actorSoundNN(p, 0x4839);
            btdActionLevelSound(btdActor(p), 0x581D,
                                gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12));
            i_this->m6028[5] = (s16)(23);
            i_this->m6028[4] = (s16)(23);
            i_this->m6028[6] = (s16)(3);
        }
        break;
    }
    case 5:
        if (btdActionStopped(morf)) {
            u8 forced = (u8)l_HIO.m10;
            i_this->mGohmaState = (s16)(0);
            if (forced == 4)
                i_this->m02E9 = (s8)(5);
            else {
                i_this->m02E4 = ACT_WAIT;
                i_this->m02EC[0] = (s16)(0);
            }
        }
        break;
    }
}
} // namespace

namespace {
u32 btdActionParticle(btd_class *i_this, u16 id, u32 angles = 0) {
    u32 control = (u32)btdPlay()->mParticle.v;
    return btdGuest_dPa_control_c_set(control, 0, u32(id), gabi::ea(&i_this->current.pos), angles,
                                      0, 0xFF, 0, -1, 0, 0, 0);
}
void btdActionParticleMatrix(u32 morf, u32 emitter, u32 offset) {
    u32 model = (u32)btdMorph(morf)->mpModel.v, block = (u32)btdModel(model)->mpMtxBuffer.v;
    u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
    u32 matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
    (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
    btdGuest_JPASetRMtxTVecfromMtx(matrices + offset, emitter + 0x1F0, emitter + 0x22C);
}
void btdActionBreakShell(btd_class *i_this, u32 morf) {
    const u32 p = gabi::ea(i_this);
    u8 count = (u8)i_this->m6190;
    u32 model = (u32)btdMorph(morf)->mpModel.v;
    u32 animation = resource(gabi::guest_addr("Btd"), (s32)hibi_brk[(4 * u32(count)) / 4]);
    u32 data = (u32)btdModel(model)->mModelData.v, brk = (u32)i_this->brk.v;
    btdGuest_brkInitNative(brk, data, animation, 1, 2, 1.0f, 0, -1, 1, 0);
    i_this->m6190 = (u8)(u8((u8)i_this->m6190 + 1));
    btdActionBodyLevelSound(btdActor(p), 0x582C);
    for (u32 i = 0; i < 3; ++i) {
        u16 id = (u16)hibi_eff[(2 * i) / 2];
        u32 emitter = btdActionParticle(btdActor(p), id);
        if (emitter)
            btdActionParticleMatrix(morf, emitter, 0xA50);
    }
    if ((u8)l_HIO.m06)
        i_this->m6190 = (u8)(3);
    u8 debug = (u8)l_HIO.m07;
    if (debug && (u8)i_this->m6190 >= 3)
        i_this->m6190 = (u8)(2);
}
/* inlined into move, end, daBtd_Execute */
void btdActionBurst(btd_class *i_this, u32 morf) {
    const u32 p = gabi::ea(i_this);
    u32 dragon = (u32)dr2.v;
    btdDragon(dragon)->m4BA = 10;
    u32 game = play();
    btdGuest_bgmStart(gabi::load<u8>(game + PLAY_OFF(mCurStage.mName)) == 0x58 ? 0x80000054u
                                                                               : 0x80000010u);
    for (u32 i = 0; i < 35; ++i) {
        u32 piece = gabi::ea(&i_this->hahen[i]);
        if ((u8)btdDebris(piece)->m04 == 0) {
            s32 joint = 2;
            if (i >= 5)
                joint = gabi::ftoi(cM_rndF(18.999000549316406f));
            u32 position = gabi::ea(&i_this->m02FC[0].x) + u32(joint * 12);
            (btdDebris(piece)->m04 = (u8)(1));
            f32 x = (f32)btdVector(position)->x;
            (btdDebris(piece)->m08.x = (f32)(x));
            (btdDebris(piece)->m08.y = (f32)((f32)btdVector(position)->y));
            (btdDebris(piece)->m08.z = (f32)((f32)btdVector(position)->z));
            f32 random = cM_rndFX((f32)l_debugRegisters.floats[2] + 50.0f);
            (btdDebris(piece)->m08.x = (f32)(x + random));
            random = cM_rndFX((f32)l_debugRegisters.floats[2] + 50.0f);
            (btdDebris(piece)->m08.y = (f32)((f32)btdDebris(piece)->m08.y + random));
            random = cM_rndFX((f32)l_debugRegisters.floats[2] + 50.0f);
            f32 z = (f32)btdDebris(piece)->m08.z + random, xNow = (f32)btdDebris(piece)->m08.x;
            (btdDebris(piece)->m08.z = (f32)(z));
            s32 angle = btdGuest_cM_atan2sFullResult(xNow, z);
            u32 m = matrix();
            s16 angleRandom = s16(gabi::ftoi(cM_rndFX(2000.0f)));
            btdGuest_mDoMtx_YrotS(m, s32(s16(angle + angleRandom)));

            cXyz velocity;
            velocity.x = 0.0f;
            f32 yRandom = cM_rndF(5.0f) + 20.0f;
            velocity.y = yRandom + (f32)l_debugRegisters.floats[1];
            f32 zRandom = cM_rndF(5.0f) + 20.0f;
            velocity.z = zRandom + (f32)l_debugRegisters.floats[0];
            MtxPosition(&velocity, btdVector(piece + 0x14));
            (btdDebris(piece)->m22 = (s16)(s16(gabi::ftoi(cM_rndF(65536.0f)))));
            (btdDebris(piece)->m20 = (s16)(s16(gabi::ftoi(cM_rndF(65536.0f)))));
        }
    }
    for (u32 i = 0; i < 12; ++i) {
        u32 emitter = btdActionParticle(btdActor(p), (u16)hahen_effname[(2 * i) / 2]);
        if (emitter)
            btdActionParticleMatrix(morf, emitter, (u32)hahen_effindex[(4 * i) / 4] * 0x30);
    }
}
void btdActionDamage(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    play();
    u32 morf = btdGuest_get_anm(p);
    s16 state = (s16)i_this->mGohmaState;
    i_this->m02F0 = (s16)(5);
    bool hideAttention = false;
    if (state >= 10) {
        state = (s16)i_this->mGohmaState;
        i_this->mActionState = (s16)(10);
        hideAttention = state > 11;
    }
    switch (u16(state)) {
    case 0:
        btdActionAnimation(btdActor(p), 9, 5.0f, 0);
        btdActionIncrement(btdActor(p));
        [[fallthrough]];
    case 1:
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), 10, 1.0f, 2);
            s16 stun = (s16)l_HIO.m0C, next = s16((s16)i_this->mGohmaState + 1);
            i_this->mStunTimer = (s16)(stun);
            i_this->mGohmaState = (s16)(next);
        }
        break;
    case 2:
        if ((s16)i_this->mStunTimer == 0) {
            i_this->m02E4 = ACT_WAIT;
            i_this->mGohmaState = (s16)(0);
            i_this->m02EC[0] = (s16)(s16(gabi::ftoi(cM_rndF(50.0f) + 50.0f)));
        }
        break;
    case 3:
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), 10, 1.0f, 2);
            i_this->mGohmaState = (s16)(2);
        }
        break;
    case 10:
        btdActionAnimation(btdActor(p), 11, 5.0f, 0);
        btdActionIncrement(btdActor(p));
        actorSoundNN(p, 0x4837);
        [[fallthrough]];
    case 11:
    case 42:
        if (btdActionStopped(morf)) {
            i_this->m02E4 = ACT_WAIT;
            i_this->mGohmaState = (s16)(0);
            i_this->m02EC[0] = (s16)(s16(gabi::ftoi(cM_rndF(50.0f) + 50.0f)));
        }
        break;
    case 20:
        btdActionAnimation(btdActor(p), 0x2A, 3.0f, 0);
        i_this->m6E7C = (f32)(0.0f);
        btdActionIncrement(btdActor(p));
        break;
    case 21:
    case 22: {
        s32 timer;
        if (state == 21) {
            if (!btdActionStopped(morf))
                break;
            btdActionAnimation(btdActor(p), 0x2B, 1.0f, 2);
            s32 duration = (s16)l_debugRegisters.shorts[5] * 5 + 100;
            s16 next = s16((s16)i_this->mGohmaState + 1);
            i_this->m02EC[0] = (s16)(s16(duration));
            i_this->mGohmaState = (s16)(next);
            timer = s16(duration);
        } else
            timer = (s16)i_this->m02EC[0];
        s32 target = (s16)l_debugRegisters.shorts[3] + 40;
        if (timer == target) {
            btdActionBreakShell(btdActor(p), morf);
            timer = (s16)i_this->m02EC[0];
        }
        if (timer == 0) {
            if ((u8)i_this->m6190 >= 3)
                i_this->mGohmaState = (s16)(40);
            else {
                btdActionAnimation(btdActor(p), 0x2C, 15.0f, 0);
                btdActionIncrement(btdActor(p));
                (static_center_pos.x = (f32)(0.0f));
                (static_center_pos.y = (f32)(0.0f));
                (static_center_pos.z = (f32)(0.0f));
                btdActionLevelSound(btdActor(p), 0x582D, gabi::ea(&static_center_pos));
                if (p + 0x37C != 0)
                    actorSound(p, 0x483E);
                i_this->m6E7C = (f32)(0.0f);
                i_this->m02EC[0] = (s16)(150);
            }
        }
        break;
    }
    case 23: {
        s16 timer = (s16)i_this->m02EC[0];
        if (timer == 1 || timer == 70)
            i_this->m6E7C = (f32)(0.0f);
    }
        if (btdActionStopped(morf)) {
            i_this->mGohmaState = (s16)(30);
            i_this->m02EC[0] = (s16)(0);
        }
        break;
    case 30: {
        s16 timer = (s16)i_this->m02EC[0];
        i_this->m6E88 = (u8)(0);
        if (timer == 0) {
            btdActionAnimation(btdActor(p), 0x1E, 1.0f, 0);
            btdActionIncrement(btdActor(p));
            i_this->m02F6 = (u8)(0);
            btdActionLevelSound(btdActor(p), 0x5826, gabi::ea(&static_center_pos));
            if (p + 0x37C != 0)
                actorSound(p, 0x483D);
            i_this->m6E7C = (f32)(0.0f);
            i_this->mKankyoTimer = (s16)(20);
            i_this->m02EC[0] = (s16)(60);
        }
    } break;
    case 31: {
        s16 timer = (s16)i_this->m02EC[0];
        if (timer == 55) {
            btdGuest_sibuki_set(p);
            timer = (s16)i_this->m02EC[0];
        }
        if (timer == 1)
            i_this->m6E7C = (f32)(0.0f);
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), 0x1F, 1.0f, 0);
            btdActionIncrement(btdActor(p));
            btdActionLevelSound(btdActor(p), 0x582E, gabi::ea(&static_center_pos));
            i_this->m6E7C = (f32)(0.0f);
        }
    } break;
    case 32:
        i_this->m6E88 = (u8)(0);
        if (btdActionStopped(morf)) {
            btdActionAnimation(btdActor(p), 0x20, 3.0f, 0);
            s32 room = (s8)i_this->current.roomNo;
            u32 dragon = (u32)dr2.v;
            s32 reverb = btdGuest_dComIfGp_getReverb(room);
            mDoAud_seStart(0x5830, btdVector(dragon + 0x5C4), 0, reverb);
            btdActionParticle(btdActor(p), 0x80BC, gabi::ea(&i_this->current.angle));
            btdActionIncrement(btdActor(p));
            i_this->m02EC[0] = (s16)(30);
        }
        break;
    case 33:
        if ((s16)i_this->m02EC[0] == 20) {
            btdActionLevelSound(btdActor(p), 0x5831, gabi::ea(&static_center_pos));
            i_this->m6E7C = (f32)(0.0f);
            btdGuest_sibuki_set(p);
        }
        if (btdActionStopped(morf)) {
            i_this->m02E4 = ACT_WAIT;
            i_this->mGohmaState = (s16)(0);
            i_this->m02EC[0] = (s16)(s16(gabi::ftoi(cM_rndF(50.0f) + 50.0f)));
        }
        break;
    case 40:
        btdActionAnimation(btdActor(p), 0x34, 3.0f, 0);
        btdActionIncrement(btdActor(p));
        btdActionBodyLevelSound(btdActor(p), 0x5832);
        btdGuest_bgmStop(30);
        i_this->m6E7C = (f32)(0.0f);
        i_this->m02EC[0] = (s16)(150);
        [[fallthrough]];
    case 41: {
        s16 target = s16((s16)l_debugRegisters.shorts[3] + 38), timer = (s16)i_this->m02EC[0];
        if (timer == target) {
            btdActionBurst(btdActor(p), morf);
            timer = (s16)i_this->m02EC[0];
        }
        if (timer == 101) {
            btdActionBodyLevelSound(btdActor(p), 0x5833);
            if (p != 0 && p + 0x37C != 0)
                actorSound(p, 0x483D);
        }
        if (btdActionStopped(morf)) {
            btdActionBodyLevelSound(btdActor(p), 0x5834);
            if (p != 0 && p + 0x37C != 0)
                actorSound(p, 0x4837);
            i_this->m02E0 = (u8)(1);
            btdActionAnimation(btdActor(p), 0x35, 0.0f, 0);
            btdActionIncrement(btdActor(p));
        }
        break;
    }
    }
    if (hideAttention)
        i_this->attention_info.flags = (u32)(0);
}
} // namespace

namespace {
/* inlined into move, end, daBtd_Execute */
void btdActionTrack(btd_class *i_this, u32 player) {
    const u32 p = gabi::ea(i_this);
    if ((s16)i_this->m5E94 != 0) {
        s16 rate = (s16)i_this->m5E92;
        if (rate == 0) {
            rate = 10;
            i_this->m5E92 = (s16)(rate);
        }
        u32 dragon = (u32)dr2.v;
        if (dragon != 0) {
            s16 action = (s16)btdDragon(dragon)->m4BA;
            if (action == 0 || action >= 10) {
                s16 mode = (s16)btdDragon(dragon)->m4CA;
                if (mode == 0 || mode >= 98) {
                    s16 target = (s16)i_this->m5E90;
                    if (target == 0) {
                        target = s16(btdGuest_fopAcM_searchActorAngleYFullResult(
                            p, (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v));
                        rate = (s16)i_this->m5E92;
                    }
                    s16 limit = (s16)i_this->m5E94;
                    btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->current.angle.y), s32(target),
                                                 s32(rate), s32(limit));
                }
            }
        }
        i_this->m5E90 = (s16)(0);
        i_this->m5E92 = (s16)(10);
        i_this->m5E94 = (s16)(0);
    }
    s16 turn = 0, half = 0;
    if ((u8)i_this->m5E86 != 0) {
        f32 z = (f32)btdPlayer(player)->current.pos.z, x = (f32)btdPlayer(player)->current.pos.x;
        if (!(gabi::fmadds(x, x, z * z) < 40000.0f)) {
            s32 angle = btdGuest_fopAcM_searchActorAngleYFullResult(
                p, (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v);
            turn = s16(angle - (s16)i_this->current.angle.y);
            if (turn > 5000)
                turn = 5000;
            else if (turn < -5000)
                turn = -5000;

            cXyz difference;

            btdGuest_cXyz_op_sub(gabi::ea(&btdPlayer(player)->eyePos), &difference,
                                 gabi::ea(&i_this->eyePos));
            s32 eyeAngle = btdGuest_cM_atan2sFullResult(f32(difference.x), f32(difference.z));
            s16 raw = s16(eyeAngle - (s16)i_this->current.angle.y);
            half = s16(s32(raw) / 2);
            if (half > 0x2000)
                half = 0x2000;
            else if (half < -0x2000)
                half = -0x2000;
        }
    }
    btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m5E88), s32(turn), 4, 0x200);
    btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m5E8E), s32(half), 4, 0x200);
    s16 pitch = (s16)i_this->m5E8C;
    btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m5E8A), s32(pitch), 4, 0x100);
    i_this->m5E8C = (s16)(0);
    i_this->m5E86 = (u8)(0);
}
} // namespace
// HD's matcher name "damage" covers the complete action dispatcher, with its
// wait, six attack variants and damage substates inlined into this one body.

#include <cmath>

namespace {
u32 btdEndPlay() {
    return btdGuest_getPlay();
}
u32 btdEndMatrix() {
    return gabi::ea(calc_mtx());
}
void btdEndCopy3(u32 dst, u32 src) {
    u32 x = btdWord(btdVector(src)->x), y = btdWord(btdVector(src)->y),
        z = btdWord(btdVector(src)->z);
    btdSetWord(btdVector(dst)->x, x);
    btdSetWord(btdVector(dst)->y, y);
    btdSetWord(btdVector(dst)->z, z);
}
/* inlined into move, end, daBtd_Execute */
u32 btdEndResource(u32 name, s32 index) {

    SafeString s;

    s.mStringTop = name;
    s.__vtbl = gabi::ea(&l_safeStringVtable);
    return btdGuest_dRes_control_c_getRes(gabi::ea(dComIfG_resControl()), &s, index);
}
void btdEndSound(btd_class *i_this, u32 position, u32 sound) {
    s32 room = (s8)i_this->current.roomNo;
    s32 reverb = btdGuest_dComIfGp_getReverb(room);
    mDoAud_seStart(sound, btdVector(position), 0, reverb);
}
void btdEndMonsterSound(btd_class *i_this, u32 sound, bool allowNullActor = false) {
    const u32 actor = gabi::ea(i_this);
    if ((!actor && !allowNullActor) || !(actor + 0x37C))
        return;
    s32 room = (s8)i_this->current.roomNo;
    u32 id = actor ? (u32)i_this->mBsPcId : 0xFFFFFFFFu;
    s32 reverb = btdGuest_dComIfGp_getReverb(room);
    btdGuest_seStartMonster(sound, gabi::ea(&i_this->eyePos), id, 0, reverb);
}
void btdEndShock(cXyz &axis) {
    u32 game = btdEndPlay();
    s32 power = (s16)l_debugRegisters.shorts[0];
    axis.z = 0.0f;
    axis.y = 1.0f;
    axis.x = 0.0f;
    btdGuest_dVibration_c_StartShock(game + 0x599C, power + 5, -33, &axis);
}
void btdEndFollowEmitter(btd_class *i_this, u32 emitter) {
    u32 model = (u32)btdMorph((u32)i_this->mpDeadHeadMorf.v)->mpModel.v;
    u32 block = (u32)btdModel(model)->mpMtxBuffer.v;
    u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
    u32 matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
    (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
    btdGuest_JPASetRMtxTVecfromMtx(matrices, emitter + 0x1F0, emitter + 0x22C);
}
} // namespace

// daBtd_Execute: 020F1494..020F349C, 8204 bytes (demo_camera inlined).
namespace {
u32 btdExecPlay() {
    return btdGuest_getPlay();
}
u32 btdExecMatrix() {
    return gabi::ea(calc_mtx());
}
void btdExecCopy(u32 dst, u32 src) {
    u32 x = btdWord(btdVector(src)->x), y = btdWord(btdVector(src)->y),
        z = btdWord(btdVector(src)->z);
    btdSetWord(btdVector(dst)->x, x);
    btdSetWord(btdVector(dst)->y, y);
    btdSetWord(btdVector(dst)->z, z);
}
void btdExecModelMatrix(u32 model, u32 source) {
    f32 v[12];
    for (u32 i = 0; i < 12; ++i)
        v[i] = (f32)btdMatrix(source)->m[i / 4][i % 4];
    constexpr u8 order[] = {0, 1, 2, 3, 4, 8, 5, 6, 7, 9, 10, 11};
    for (u8 i : order)
        (btdModel(model)->mBaseTransformMtx.m[i / 4][i % 4] = (f32)(v[i]));
}
void btdExecSetCollision(u32 shape) {
    u32 game = btdExecPlay();
    btdGuest_cCcS_Set(game + 0x26A4, shape);
}
void btdExecCalc(u32 address, f32 target, f32 rate, f32 step) {
    btdGuest_cLib_addCalc2(address, target, rate, step);
}
void btdExecSound(btd_class *i_this, u32 position, u32 sound) {
    s32 room = (s8)i_this->current.roomNo;
    s32 reverb = btdGuest_dComIfGp_getReverb(room);
    mDoAud_seStart(sound, btdVector(position), 0, reverb);
}
void btdExecPlayerPosition(u32 player, u32 position, u32 actor, s32 angleOffset) {

    s16 angle = (s16)((s16)btdActor(actor)->current.angle.y + angleOffset);
    btdPlayer(player)->setPlayerPosAndAngle(btdVector(position), angle);
}
void btdExecQuake(s32 extra, cXyz &axis) {
    u32 game = btdExecPlay();
    s32 power = (s16)l_debugRegisters.shorts[3];
    axis.x = 0.0f;
    axis.y = 1.0f;
    axis.z = 0.0f;
    btdGuest_dVibration_c_StartQuake(game + 0x599C, power + extra, -33, &axis);
}
void btdExecCameraStep(btd_class *i_this, be<f32> &value, f32 target, f32 multiplier,
                       f32 rate = 0.1f) {
    f32 blend = (f32)i_this->m6E40;
    btdExecCalc(gabi::ea(&value), target, rate, gabi::fmuls_ppc(multiplier, blend));
}
void btdExecDebrisMove(btd_class *i_this) {
    for (u32 i = 0; i < 35; ++i) {
        u32 piece = gabi::ea(&i_this->hahen[i]);
        if (!(u8)btdDebris(piece)->m04)
            continue;
        PSVECAdd(btdVector(piece + 8), btdVector(piece + 0x14), btdVector(piece + 8));
        f32 x = (f32)btdDebris(piece)->m08.x;
        s16 ax = (s16)btdDebris(piece)->m20, ay = (s16)btdDebris(piece)->m22;
        f32 y = (f32)btdDebris(piece)->m08.y, vy = (f32)btdDebris(piece)->m14.y,
            z = (f32)btdDebris(piece)->m08.z;
        (btdDebris(piece)->m20 = (s16)((s16)(ax + 0x900)));
        (btdDebris(piece)->m22 = (s16)((s16)(ay + 0x600)));
        (btdDebris(piece)->m14.y = (f32)(gabi::fsubs_ppc(vy, 3.0f)));
        MtxTrans(x, y, z, 0);
        ay = (s16)btdDebris(piece)->m22;
        btdGuest_mDoMtx_YrotM(btdExecMatrix(), ay);
        ax = (s16)btdDebris(piece)->m20;
        btdGuest_mDoMtx_XrotM(btdExecMatrix(), ax);
        u32 matrix = btdExecMatrix(), model = (u32)btdDebris(piece)->model.v;
        btdExecModelMatrix(model, matrix);
        if ((f32)btdDebris(piece)->m08.y < -100.0f)
            (btdDebris(piece)->m04 = (u8)(0));
    }
}
void btdExecSimple(u32 position, u32 effect) {
    u32 control = (u32)btdPlay()->mParticle.v;
    btdGuest_dPa_control_c_setSimple(control, effect, position, 255, gabi::ea(&l_effectColor),
                                     gabi::ea(&l_effectColor), 0);
}
/* inlined into move, end, daBtd_Execute */
void btdExecSplashMove(btd_class *i_this) {
    const u32 actor = gabi::ea(i_this);
    u32 player = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;

    cXyz difference;

    for (u32 i = 0; i < 50; ++i) {
        u32 splash = gabi::ea(&i_this->mBtdSibukiS[i]), sphere = gabi::ea(&i_this->mSibukiSphs[i]);
        s8 status = (s8)btdSplash(splash)->m00;
        if (status == 0)
            continue;
        if (status > 2) {
            s8 next = (s8)(status + 1);
            (btdSplash(splash)->m00 = (s8)(next == 30 ? 0 : next));
            continue;
        }
        bool track = i == 0 && (u8)l_HIO.m54 != 0;
        bool updateCollision;
        if (track) {
            btdExecCalc(splash + 4, (f32)btdPlayer(player)->current.pos.x, 1.0f, 30.0f);
            btdExecCalc(splash + 8, gabi::fadds_ppc((f32)btdPlayer(player)->current.pos.y, 70.0f),
                        1.0f, 30.0f);
            btdExecCalc(splash + 12, (f32)btdPlayer(player)->current.pos.z, 1.0f, 30.0f);
            status = (s8)btdSplash(splash)->m00;
            (btdSplash(splash)->m10.y = (f32)(-10.0f));
            updateCollision = true;
        } else {
            f32 gravity = (f32)l_HIO.m50, vy = (f32)btdSplash(splash)->m10.y;
            (btdSplash(splash)->m10.y = (f32)(gabi::fsubs_ppc(vy, gravity)));
            PSVECAdd(btdVector(splash + 4), btdVector(splash + 0x10), btdVector(splash + 4));
            updateCollision = (f32)btdSplash(splash)->m10.y < 0.0f;
            if (updateCollision)
                status = (s8)btdSplash(splash)->m00;
        }
        bool grounded = false, hit = false;
        if (updateCollision) {
            if (status == 1) {
                btdGuest_dCcD_Sph_StartCAt(sphere, splash + 4);
                (btdSplash(splash)->m00 = (s8)((u8)((u8)btdSplash(splash)->m00 + 1)));
                btdExecSetCollision(sphere);
                grounded = (f32)btdSplash(splash)->m04.y < 0.0f;
            } else {
                btdGuest_dCcD_Sph_MoveCAt(sphere, splash + 4);
                btdExecSetCollision(sphere);
                grounded = (f32)btdSplash(splash)->m04.y < 0.0f;
            }
        }
        if (!grounded)
            hit = btdGuest_dCcD_GObjInf_ChkAtHit(sphere) != 0;
        if (!grounded && !hit) {
            btdExecSimple(splash + 4, 0x8063);
            continue;
        }
        if (grounded)
            (btdSplash(splash)->m04.y = (f32)(0.0f));
        btdGuest_cXyz_op_sub(gabi::ea(&btdPlayer(player)->current.pos), &difference, splash + 4);
        (btdSplash(splash)->m00 = (s8)(0));
        f32 square = PSVECSquareMag(&difference), distance = btdGuest_sqrtf(square);
        // Native blt and fallthrough of bge both require strict LT.
        bool close = distance < 1500.0f;
        if (close) {
            s8 count = (s8)i_this->m6E14;
            if (count < 20) {
                i_this->m6E14 = (s8)((u8)(count + 1));
                btdExecSimple(splash + 4, 0x8064);
                btdExecSound(btdActor(actor), splash + 4, 0x6942);
                (btdSplash(splash)->m00 = (s8)(0));
            }
        }
    }
}
void btdExecCamera(btd_class *i_this);
} // namespace

namespace {
void btdExecCameraTarget(u32 p, cXyz &local, cXyz &transformed) {
    s16 angle = (s16)btdActor(p)->current.angle.y;
    btdGuest_mDoMtx_YrotS(btdExecMatrix(), angle);
    f32 y = (f32)l_debugRegisters.floats[8], x = (f32)l_debugRegisters.floats[7],
        z = (f32)l_debugRegisters.floats[9];
    local.y = gabi::fmadds(y, 0.1f, 70.0f);
    local.x = gabi::fmadds(x, 0.1f, -500.0f);
    local.z = gabi::fmadds(z, 0.1f, 2000.0f);
    MtxPosition(&local, &transformed);
    btdExecCalc(gabi::ea(&btdActor(p)->m6E1C.x), (f32)transformed.x, 0.1f, 50.0f);
    btdExecCalc(gabi::ea(&btdActor(p)->m6E1C.y), (f32)transformed.y, 0.1f, 50.0f);
    btdExecCalc(gabi::ea(&btdActor(p)->m6E1C.z), (f32)transformed.z, 0.1f, 50.0f);
}
void btdExecCameraResume(btd_class *i_this, u32 body) {
    i_this->m6E16 = (s16)(0);
    btdGuest_dCamera_c_Start(body);
    btdGuest_dCamera_c_SetTrimSize(body, 0);
    u32 g = btdExecPlay();
    gabi::at<dEvt_control_c>(g + PLAY_OFF(mEvtCtrl))->mEventFlag |= 8;
    i_this->actor_status = (u32)((u32)i_this->actor_status & ~0x4000u);
}
/* inlined into move, end, daBtd_Execute */
void btdExecCamera(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    const u32 center = gabi::ea(&static_center_pos);
    u32 player = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;
    s8 cameraID = (s8)btdPlay()->mPlayerInfo[0].mCameraID;
    u32 game = btdExecPlay();
    s16 mainCount = (s16)i_this->m6E18, sub = (s16)i_this->m6E1A, mode = (s16)i_this->m6E16;
    u32 camera = (u32)gabi::at<dComIfG_camera_info_class>(
                     game + PLAY_OFF(mCameraInfo) +
                     (u32)((s32)cameraID * sizeof(dComIfG_camera_info_class)))
                     ->mpCamera,
        body = camera + 0x248;
    i_this->m6E1A = (s16)((s16)(sub + 1));
    i_this->m6E18 = (s16)((s16)(mainCount + 1));

    cXyz local;
    cXyz transformed;
    cXyz normalResult;
    cXyz axis;
    cXyz resetCenter;
    cXyz resetEye;

    switch (mode) {
    case 1: {
        i_this->mKankyoState = (u8)(6);
        s16 next = (s16)i_this->m6E16;
        i_this->m6E16 = (s16)((s16)(next + 1));
        i_this->m6E3C = (f32)(gabi::fadds_ppc((f32)l_debugRegisters.floats[12], 50.0f));
        btdGuest_dCamera_c_Stop(body);
        btdGuest_dCamera_c_SetTrimSize(body, 2);
        i_this->m6E1A = (s16)(0);
        i_this->m6E1C.x = (f32)(282.0f);
        i_this->m6E18 = (s16)(0);
        i_this->m6E1C.y = (f32)(2775.0f);
        i_this->m6E1C.z = (f32)(-582.0f);
        i_this->m6E28.x = (f32)(-1200.0f);
        i_this->m6E28.y = (f32)(0.0f);
        i_this->m6E28.z = (f32)(660.0f);
        i_this->m6E40 = (f32)(0.0f);
        (btdVector(center)->y = (f32)(0.0f));
        (btdVector(center)->z = (f32)(0.0f));
        (btdVector(center)->x = (f32)(0.0f));
        [[fallthrough]];
    }
    case 2: {
        sub = (s16)i_this->m6E1A;
        i_this->mKankyoState = (u8)(6);
        if (sub > 50) {
            f32 step = gabi::fadds_ppc((f32)l_debugRegisters.floats[6], 5.0f);
            f32 y = (f32)i_this->m6E1C.y;
            i_this->m6E1C.y = (f32)(gabi::fsubs_ppc(y, step));
        }
        local.x = -1373.0f;
        local.z = 790.0f;
        local.y = (f32)btdPlayer(player)->current.pos.y;
        btdExecPlayerPosition(player, gabi::ea(&local), p, 0x8000);
        if ((s16)i_this->m6E1A != 111)
            break;
        i_this->m6E1A = (s16)(0);
        i_this->m6E1C.x = (f32)(-1728.0f);
        i_this->m6E1C.y = (f32)(111.0f);
        i_this->m6E1C.z = (f32)(844.0f);
        i_this->m6E28.x = (f32)(-1397.0f);
        i_this->m6E28.y = (f32)(110.0f);
        i_this->m6E28.z = (f32)(685.0f);
        i_this->m6E16 = (s16)(3);
        [[fallthrough]];
    }
    case 3: {
        sub = (s16)i_this->m6E1A;
        i_this->mKankyoState = (u8)(6);
        if (sub <= 50)
            break;
        btdExecCameraStep(btdActor(p), i_this->m6E28.x, -1536.0f, 139.0f);
        btdExecCameraStep(btdActor(p), i_this->m6E28.y, 411.0f, 301.0f);
        btdExecCameraStep(btdActor(p), i_this->m6E28.z, 752.0f, 67.0f);
        btdExecCalc(gabi::ea(&i_this->m6E40), 1.0f, 0.1f, 0.005f);
        sub = (s16)i_this->m6E1A;
        if (sub >= 180 && !(s16)l_debugRegisters.shorts[4]) {
            i_this->m6E1A = (s16)(0);
            i_this->m6E40 = (f32)(0.0f);
            i_this->m6E1C.x = (f32)(-2501.0f);
            i_this->m6E1C.y = (f32)(418.0f);
            i_this->m6E1C.z = (f32)(-32.0f);
            i_this->m6E28.x = (f32)(-2139.0f);
            i_this->m6E16 = (s16)(4);
            i_this->m6E28.y = (f32)(373.0f);
            i_this->m6E28.z = (f32)(17.0f);
            i_this->m6E3C = (f32)(60.0f);
        } else {
            if (sub >= 140) {
                i_this->mKankyoState = (u8)(7);
                i_this->m6E48 = (f32)(20.0f);
                btdGuest_changeBgmStatus(0x105E);
                if ((s16)i_this->m6E1A == 140)
                    btdExecQuake(3, axis);
            }
            break;
        }
        [[fallthrough]];
    }
    case 4: {
        i_this->mKankyoState = (u8)(7);
        i_this->m6E48 = (f32)(15.0f);
        btdGuest_changeBgmStatus(0x105E);
        btdExecCameraStep(btdActor(p), i_this->m6E1C.x, -1763.0f, 738.0f);
        btdExecCameraStep(btdActor(p), i_this->m6E1C.y, 167.0f, 251.0f);
        btdExecCameraStep(btdActor(p), i_this->m6E1C.z, 812.0f, 844.0f);
        btdExecCameraStep(btdActor(p), i_this->m6E28.x, -1430.0f, 709.0f);
        btdExecCameraStep(btdActor(p), i_this->m6E28.y, 123.0f, 250.0f);
        btdExecCameraStep(btdActor(p), i_this->m6E28.z, 663.0f, 646.0f);
        btdExecCalc(gabi::ea(&i_this->m6E40),
                    gabi::fadds_ppc((f32)l_debugRegisters.floats[10], 0.01f), 0.1f, 0.001f);
        sub = (s16)i_this->m6E1A;
        if (sub > 40) {
            btdExecCalc(gabi::ea(&i_this->m6E84), 0.0f, 0.1f, 1.0f);
            sub = (s16)i_this->m6E1A;
        }
        if (sub == 30) {
            btdExecSound(btdActor(p), center, 0x5825);
            sub = (s16)i_this->m6E1A;
        }
        if (sub == 80) {
            btdGuest_wave_set(p);
            sub = (s16)i_this->m6E1A;
        }
        if (sub == 145) {
            btdGuest_wave_set(p);
            sub = (s16)i_this->m6E1A;
        }
        if (sub == 150) {
            btdExecQuake(4, axis);
            s32 room = (s8)i_this->current.roomNo;
            i_this->mGohmaState = (s16)(1);
            i_this->m6E40 = (f32)(0.0f);
            i_this->m6E84 = (f32)(0.0f);
            i_this->m6E1A = (s16)(0);
            i_this->m6E16 = (s16)(5);
            s32 reverb = btdGuest_dComIfGp_getReverb(room);
            mDoAud_seStart(0x5826, btdVector(center), 0, reverb);
            i_this->mKankyoState = (u8)(8);
        }
        break;
    }
    case 5: {
        sub = (s16)i_this->m6E1A;
        if (sub == 10) {
            u32 g = btdExecPlay();
            btdGuest_dVibration_c_StopQuake(g + 0x599C, -1);
            sub = (s16)i_this->m6E1A;
        }
        if (sub == 50) {
            btdGuest_wave_set(p);
            sub = (s16)i_this->m6E1A;
        }
        if (sub < 10)
            break;
        if (sub < 205) {
            btdExecCalc(gabi::ea(&i_this->m6E28.x), gabi::fmuls_ppc((f32)i_this->eyePos.x, 0.3f),
                        0.2f, gabi::fmuls_ppc(200.0f, (f32)i_this->m6E40));
            f32 eyeY = (f32)i_this->eyePos.y, bias = (f32)l_debugRegisters.floats[5],
                floor = (f32)l_debugRegisters.floats[6];
            f32 target = gabi::fsubs_ppc(gabi::fmadds(bias, 0.1f, eyeY), 100.0f),
                minimum = gabi::fadds_ppc(floor, 300.0f);
            if (target < minimum)
                target = minimum;
            btdExecCalc(gabi::ea(&i_this->m6E28.y), target, 0.2f,
                        gabi::fmuls_ppc(200.0f, (f32)i_this->m6E40));
            btdExecCalc(gabi::ea(&i_this->m6E28.z), gabi::fmuls_ppc((f32)i_this->eyePos.z, 0.3f),
                        0.2f, gabi::fmuls_ppc(200.0f, (f32)i_this->m6E40));
        } else {
            // Native dedicated205 arm zeroes blend before its first calculation.
            f32 firstStep;
            if (sub == 205) {
                firstStep = gabi::fmuls_ppc(2000.0f, 0.0f);
                i_this->m6E40 = (f32)(0.0f);
            } else
                firstStep = gabi::fmuls_ppc(2000.0f, (f32)i_this->m6E40);
            btdExecCalc(gabi::ea(&i_this->m6E1C.x), -1750.0f, 0.2f, firstStep);
            btdExecCameraStep(btdActor(p), i_this->m6E1C.y, 50.0f, 2000.0f, 0.2f);
            btdExecCameraStep(btdActor(p), i_this->m6E1C.z, 851.0f, 2000.0f, 0.2f);
            btdExecCameraStep(btdActor(p), i_this->m6E28.x, -299.0f, 2000.0f, 0.2f);
            btdExecCameraStep(btdActor(p), i_this->m6E28.y, 319.0f, 2000.0f, 0.2f);
            btdExecCameraStep(btdActor(p), i_this->m6E28.z, 248.0f, 2000.0f, 0.2f);
            if ((s16)i_this->m6E1A >= 255)
                btdExecCalc(gabi::ea(&i_this->m6E3C), 45.0f, 0.2f, 1.5f);
        }
        btdExecCalc(gabi::ea(&i_this->m6E40), 1.0f, 0.1f, 0.01f);
        break;
    }
    case 100: {
        if ((u16)i_this->eventInfo.mCommand != 2) {
            btdGuest_fopAcM_orderPotentialEvent(p, 2, 0xFFFF, 0);
            i_this->eventInfo.mCondition = (u16)((u16)i_this->eventInfo.mCondition | 2);
        } else
            i_this->m6E16 = (s16)(101);
        i_this->m6E3C = (f32)(gabi::fadds_ppc((f32)l_debugRegisters.floats[12], 50.0f));
        u32 view = (u32)btdPlay()->mCameraInfo[0].mpCamera;
        // Native camera copies interleave each read and write, preserving aliases.
        btdSetWord(i_this->m6E1C.x, btdWord(btdCameraView(view)->lookat.eye.x));
        btdSetWord(i_this->m6E1C.y, btdWord(btdCameraView(view)->lookat.eye.y));
        btdSetWord(i_this->m6E1C.z, btdWord(btdCameraView(view)->lookat.eye.z));
        btdSetWord(i_this->m6E28.x, btdWord(btdCameraView(view)->lookat.center.x));
        btdSetWord(i_this->m6E28.y, btdWord(btdCameraView(view)->lookat.center.y));
        btdSetWord(i_this->m6E28.z, btdWord(btdCameraView(view)->lookat.center.z));
        btdGuest_dCamera_c_Stop(body);
        btdGuest_dCamera_c_SetTrimSize(body, 2);
        break;
    }
    case 101: {
        btdExecCopy(gabi::ea(&local), gabi::ea(&btdPlayer(player)->current.pos));
        local.y = 0.0f;
        f32 square = PSVECSquareMag(&local), distance = btdGuest_sqrtf(square);
        if (distance < 1400.0f) {
            btdGuest_cXyz_normalize(&local, &normalResult);
            f32 x = local.x, z = local.z;
            local.x = gabi::fmuls_ppc(x, 1400.0f);
            local.z = gabi::fmuls_ppc(z, 1400.0f);
        }
        local.y = (f32)btdPlayer(player)->current.pos.y;
        btdExecPlayerPosition(player, gabi::ea(&local), p, 0x8000);
        btdExecCalc(gabi::ea(&i_this->m6E3C),
                    gabi::fadds_ppc((f32)l_debugRegisters.floats[4], 50.0f), 0.5f, 3.0f);
        btdExecCalc(gabi::ea(&i_this->m6E28.x), gabi::fmuls_ppc((f32)i_this->eyePos.x, 0.3f), 0.2f,
                    200.0f);
        f32 eyeY = (f32)i_this->eyePos.y, bias = (f32)l_debugRegisters.floats[5],
            minimum = gabi::fadds_ppc((f32)l_debugRegisters.floats[6], 500.0f);
        f32 target = gabi::fsubs_ppc(gabi::fmadds(bias, 0.1f, eyeY), 500.0f);
        if (target < minimum)
            target = minimum;
        btdExecCalc(gabi::ea(&i_this->m6E28.y), target, 0.2f, 200.0f);
        btdExecCalc(gabi::ea(&i_this->m6E28.z), gabi::fmuls_ppc((f32)i_this->eyePos.z, 0.3f), 0.2f,
                    200.0f);
        btdExecCameraTarget(p, local, transformed);
        if ((s16)i_this->m6E1A >= 320)
            i_this->mKankyoState = (u8)(3);
        break;
    }
    case 102: {
        sub = (s16)i_this->m6E1A;
        if (sub < 60) {
            sub = (s16)i_this->m6E1A;
            i_this->mKankyoState = (u8)(4);
        }
        if (sub == 90) {
            sub = (s16)i_this->m6E1A;
            i_this->m6E15 = (u8)(1);
        }
        s16 delay = (s16)((s16)l_debugRegisters.shorts[0] + 90);
        if (sub > delay) {
            f32 blend = (f32)i_this->m6E40, eyeX = (f32)i_this->eyePos.x;
            btdExecCalc(gabi::ea(&i_this->m6E28.x), eyeX, gabi::fmuls_ppc(0.2f, blend),
                        gabi::fmuls_ppc(200.0f, blend));
            f32 eyeY = (f32)i_this->eyePos.y, bias = (f32)l_debugRegisters.floats[5];
            blend = (f32)i_this->m6E40;
            btdExecCalc(gabi::ea(&i_this->m6E28.y), gabi::fmadds(bias, 0.1f, eyeY),
                        gabi::fmuls_ppc(0.5f, blend), gabi::fmuls_ppc(500.0f, blend));
            blend = (f32)i_this->m6E40;
            f32 eyeZ = (f32)i_this->eyePos.z;
            btdExecCalc(gabi::ea(&i_this->m6E28.z), eyeZ, gabi::fmuls_ppc(0.2f, blend),
                        gabi::fmuls_ppc(200.0f, blend));
            btdExecCalc(gabi::ea(&i_this->m6E40), 1.0f, 1.0f, 0.1f);
        }
        btdExecCameraTarget(p, local, transformed);
        f32 x = (f32)l_debugRegisters.floats[10], y = (f32)l_debugRegisters.floats[11],
            z = (f32)l_debugRegisters.floats[12];
        local.x = gabi::fmadds(x, 0.1f, -250.0f);
        local.y = gabi::fmadds(y, 0.1f, 100.0f);
        local.z = gabi::fmadds(z, 0.1f, 1600.0f);
        MtxPosition(&local, &i_this->m6E5C);
        if ((s16)i_this->m6E1A > 110) {
            x = (f32)l_debugRegisters.floats[10];
            z = (f32)l_debugRegisters.floats[12];
            local.y = 0.0f;
            local.x = gabi::fmadds(x, 0.1f, -50.0f);
            local.z = gabi::fmadds(z, 0.1f, 1650.0f);
            MtxPosition(&local, &btdPlayer(player)->current.pos);
            btdExecPlayerPosition(player, gabi::ea(&btdPlayer(player)->current.pos), p, -0x4000);
        }
        break;
    }
    case 103:
        i_this->m6E40 = (f32)(0.0f);
        i_this->m6E28.x = (f32)(0.0f);
        i_this->m6E16 = (s16)((s16)(mode + 1));
        i_this->m6E1A = (s16)(0);
        i_this->m6E28.y = (f32)(0.0f);
        i_this->m6E28.z = (f32)(0.0f);
        i_this->m6E36 = (s16)(-6000);
        i_this->m6E34 = (s16)(-11000);
        i_this->m6E3C = (f32)(55.0f);
        i_this->m6E44 = (f32)(2800.0f);
        [[fallthrough]];
    case 104: {
        f32 blend = (f32)i_this->m6E40;
        s16 limit = (s16)gabi::ftoi(gabi::fmuls_ppc(22384.0f, blend));
        btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m6E36), 0x4000, 16, limit);
        blend = (f32)i_this->m6E40;
        s16 target = (s16)((s16)l_debugRegisters.shorts[0] - 6000);
        limit = (s16)gabi::ftoi(gabi::fmuls_ppc(5000.0f, blend));
        btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m6E34), target, 16, limit);
        btdExecCalc(gabi::ea(&i_this->m6E44), 500.0f, 0.0625f,
                    gabi::fmuls_ppc(2300.0f, (f32)i_this->m6E40));
        s16 angle = (s16)i_this->m6E36;
        btdGuest_mDoMtx_YrotS(btdExecMatrix(), angle);
        angle = (s16)i_this->m6E34;
        btdGuest_mDoMtx_XrotM(btdExecMatrix(), angle);
        local.x = 0.0f;
        local.z = (f32)i_this->m6E44;
        local.y = 0.0f;
        MtxPosition(&local, &i_this->m6E1C);
        btdExecCalc(gabi::ea(&i_this->m6E40),
                    gabi::fadds_ppc((f32)l_debugRegisters.floats[10], 0.0048f), 0.1f, 0.000048f);
        sub = (s16)i_this->m6E1A;
        if (sub < 3)
            break;
        if (sub == 3) {
            if (p && p + 0x37C)
                btdExecSound(btdActor(p), gabi::ea(&i_this->eyePos), 0x1863);
            if ((u32)l_ykgrEmitter.v)
                l_ykgrAlphaFlag = 0;
            if ((s16)i_this->m6E1A < 3)
                break;
        }
        sub = (s16)i_this->m6E1A;
        i_this->mKankyoState = (u8)(5);
        if (sub == 280) {
            u32 save = dComIfGs_ea();
            (btdVector(center)->y = (f32)(-50.0f));
            (btdVector(center)->x = (f32)(0.0f));
            (btdVector(center)->z = (f32)(0.0f));
            btdGuest_dSv_memBit_c_onDungeonItem(save + 0x798, 3);
            s32 room = (s8)i_this->current.roomNo;
            btdGuest_fopAcM_createWarpFlower(center, 0, room, 0);
            sub = (s16)i_this->m6E1A;
            i_this->m6E90 = (s8)(0);
        }
        if (sub == 450) {
            i_this->m6E16 = (s16)(151);
            i_this->mGohmaState = (s16)(57);
        }
        break;
    }
    case 150:
        btdExecCopy(gabi::ea(&resetEye), gabi::ea(&i_this->m6E1C.x));
        btdExecCopy(gabi::ea(&resetCenter), gabi::ea(&i_this->m6E28.x));
        btdGuest_dCamera_c_Reset_2(body, &resetCenter, &resetEye);
        btdExecCameraResume(btdActor(p), body);
        break;
    case 151: {
        f32 x = (f32)btdPlayer(player)->eyePos.x, y = (f32)btdPlayer(player)->eyePos.y,
            z = (f32)btdPlayer(player)->eyePos.z;
        resetCenter.x = x;
        resetCenter.y = (f32)btdPlayer(player)->eyePos.y;
        resetCenter.z = (f32)btdPlayer(player)->eyePos.z;
        resetEye.x = gabi::fmuls_ppc(x, 0.9f);
        resetEye.y = y;
        resetEye.z = gabi::fmuls_ppc(z, 0.9f);
        btdGuest_dCamera_c_Reset_2(body, &resetCenter, &resetEye);
        btdExecCameraResume(btdActor(p), body);
        break;
    }
    default:
        break;
    }
    if (!(s16)i_this->m6E16)
        return;
    sub = (s16)i_this->m6E1A;
    // Native table indexes truncate16 then round down to an8-byte sin/cos pair.
    u32 sinIndex = (u32)(u16)((s32)sub * 0x3300) & ~7u,
        cosIndex = (u32)(u16)((s32)sub * 0x3000) & ~7u;
    f32 eyeY = (f32)i_this->m6E1C.y;
    s16 count = (s16)i_this->m02E2;
    f32 amplitude = (f32)i_this->m6E48, centerX = (f32)i_this->m6E28.x;
    f32 sin = cM_ssin((s32)sinIndex), eyeX = (f32)i_this->m6E1C.x;
    f32 fx = gabi::fmuls_ppc(sin, amplitude), cos = cM_scos((s32)cosIndex),
        fy = gabi::fmuls_ppc(cos, amplitude);
    u32 rollIndex = (u32)(u16)((s32)count * 0x1C00) & ~7u;
    f32 centerY = (f32)i_this->m6E28.y;
    f32 rollCos = cM_scos((s32)rollIndex),
        rollFloat = gabi::fmuls_ppc(gabi::fmuls_ppc(rollCos, amplitude), 7.5f);
    f32 eyeZ = (f32)i_this->m6E1C.z, centerZ = (f32)i_this->m6E28.z;
    s16 roll = (s16)gabi::ftoi(rollFloat);
    resetEye.y = gabi::fadds_ppc(eyeY, fy);
    resetCenter.x = gabi::fadds_ppc(centerX, fx);
    resetCenter.z = centerZ;
    resetEye.z = eyeZ;
    resetEye.x = gabi::fadds_ppc(eyeX, fx);
    resetCenter.y = gabi::fadds_ppc(centerY, fy);
    f32 fov = (f32)i_this->m6E3C;
    btdGuest_dCamera_c_Set_3(body, &resetCenter, &resetEye, roll, fov);
    f32 decay = gabi::fadds_ppc((f32)l_debugRegisters.floats[16], 2.0f);
    btdGuest_cLib_addCalc0(gabi::ea(&i_this->m6E48), 1.0f, decay);
    s32 main = (s16)i_this->m6E18;
    btdGuest_screenReport(30, 430, gabi::guest_addr("K MAIN COUNT  %d"), main);
    s32 subReport = (s16)i_this->m6E1A;
    btdGuest_screenReport(30, 430, gabi::guest_addr("K SUB  COUNT  %d"), subReport);
    mode = (s16)i_this->m6E16;
    main = (s16)i_this->m6E18;
    if (mode < 100) {
        if (main == 2) {
            (btdPlayer(player)->mDemo.mParam0 = (s32)(0));
            btdPlayer(player)->mDemo.mDemoType = (s16)(3);
            main = (s16)i_this->m6E18;
        }
        if (main == 115) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(26));
            main = (s16)i_this->m6E18;
        }
        if (main == 310) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(28));
            main = (s16)i_this->m6E18;
        }
        if (main == 452) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(24));
            main = (s16)i_this->m6E18;
        }
        if (main == 465) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(23));
            (btdPlayer(player)->mDemo.mParam0 = (s32)(2));
            main = (s16)i_this->m6E18;
        }
        if (main == 660) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(27));
            main = (s16)i_this->m6E18;
        }
        if ((u32)main == (u32)((s32)(s16)l_debugRegisters.shorts[1] + 733)) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(23));
            (btdPlayer(player)->mDemo.mParam0 = (s32)(2));
        }
    } else {
        if (main == 2) {
            (btdPlayer(player)->mDemo.mParam0 = (s32)(0));
            btdPlayer(player)->mDemo.mDemoType = (s16)(3);
            main = (s16)i_this->m6E18;
        }
        if (main == 10) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(23));
            (btdPlayer(player)->mDemo.mParam0 = (s32)(2));
            main = (s16)i_this->m6E18;
        }
        s32 bias = (s16)l_debugRegisters.shorts[0];
        if ((u32)main == (u32)(bias + 693)) {
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(50));
            bias = (s16)l_debugRegisters.shorts[0];
            main = (s16)i_this->m6E18;
        }
        if ((u32)main == (u32)(bias + 705))
            (btdPlayer(player)->mDemo.mDemoMode = (u32)(29));
    }
}
} // namespace

/* 020F02D4 */
void wave_set(btd_class *i_this) {
    s32 room = (s8)i_this->current.roomNo;
    s32 reverb = btdGuest_dComIfGp_getReverb(room);
    mDoAud_seStart(0x583Eu, &static_center_pos, 0, reverb);
    i_this->m6E7C = 0.0f;
}

/* 020F032C */
void *get_anm(btd_class *i_this) {
    if (i_this->m02E0 != 0)
        return i_this->m02E4 == 11 ? i_this->mpDeadBodyMorf.get() : i_this->mpPhase2Morf.get();
    return i_this->mpPhase1Morf.get();
}

/* 020F035C */
void *get_btk(btd_class *i_this) {
    if (i_this->m02E0 != 0)
        return i_this->m02E4 == 11 ? i_this->mpDeadBodyBtkAnm.get() : i_this->btkS.get();
    return i_this->btk.get();
}

/* 020F038C */
void *get_brk(btd_class *i_this) {
    if (i_this->m02E0 != 0)
        return i_this->m02E4 == 11 ? i_this->mpDeadBodyBrkAnm.get() : i_this->brkS.get();
    return i_this->brk.get();
}

/* 020F03BC */
void anm_init(btd_class *i_this, s32 animation, f32 blend, u8 loop, f32 speed, s32 sound) {
    void *morf = get_anm(i_this);
    u32 animationResource = resource(gabi::guest_addr("Btd"), animation);
    u32 soundResource = sound >= 0 ? resource(gabi::guest_addr("Btd"), sound) : 0;
    btdGuest_mDoExt_McaMorf_setAnm(morf, animationResource, loop, blend, speed, 0.0f, -1.0f,
                                   soundResource);
}

/* 020F04EC */
BOOL nodeCallBack(void *node, s32 timing) {
    if (timing != 0)
        return TRUE;
    u32 joint = btdGuest_nodeToJoint(node);
    u32 index = (u16)btdJoint(joint)->number;
    if (index >= 73) {
        btdGuest_JUTAssertion_getSDevice(
            gabi::guest_addr("d_a_btd.cpp"), 0x2FF,
            gabi::guest_addr("no < sizeof(joint_check) / sizeof(joint_check[0])"));
        if (index >= 73)
            return TRUE;
    }
    u32 model = gabi::ea(j3dSys.getModel());
    u32 actor = (u32)btdModel(model)->mUserArea;
    s32 role = (s8)joint_check[index];
    if (!actor || role < 0)
        return TRUE;
    u32 block = (u32)btdModel(model)->mpMtxBuffer.v;
    u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
    u32 matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
    (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
    u32 m = matrix();
    PSMTXCopy(btdMatrix(matrices + index * 48), btdMatrix(m));

    cXyz local;

    if (role == 100) {
        f32 z = (f32)l_debugRegisters.floats[11], y = (f32)l_debugRegisters.floats[10],
            x = (f32)l_debugRegisters.floats[9];
        local.z = z;
        local.y = y;
        local.x = x;
        MtxPosition(&local, &btdActor(actor)->m03E0);
        if ((u8)btdActor(actor)->m6E4C == 0) {
            u32 bx = btdWord(btdActor(actor)->m03E0.x), by = btdWord(btdActor(actor)->m03E0.y);
            btdSetWord(btdActor(actor)->eyePos.x, bx);
            btdSetWord(btdActor(actor)->eyePos.y, by);
            f32 eyeY = (f32)btdActor(actor)->eyePos.y, eyeX = (f32)btdActor(actor)->eyePos.x;
            u32 bz = btdWord(btdActor(actor)->m03E0.z);
            btdActor(actor)->attention_info.position.x = (f32)(eyeX);
            btdSetWord(btdActor(actor)->eyePos.z, bz);
            f32 eyeZ = (f32)btdActor(actor)->eyePos.z;
            btdActor(actor)->attention_info.position.y = (f32)(gabi::fadds_ppc(eyeY, 50.0f));
            btdActor(actor)->attention_info.position.z = (f32)(eyeZ);
        }
        y = (f32)l_debugRegisters.floats[10];
        z = (f32)l_debugRegisters.floats[11];
        x = (f32)l_debugRegisters.floats[9];
        local.x = x;
        local.y = gabi::fadds_ppc(y, -50.0f);
        local.z = gabi::fadds_ppc(z, 200.0f);
        MtxPosition(&local, &btdActor(actor)->mLightInfluence.mPos);
        s16 angle = (s16)btdActor(actor)->m5E8E;
        m = matrix();
        btdGuest_mDoMtx_YrotM(m, angle);
        writeJointMatrix(model, index);
        return TRUE;
    }
    if (role == 101 || role == 102) {
        s16 angle = (s16)btdActor(actor)->m02FA;
        if (role == 102)
            angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)angle, -0.6818f));
        m = matrix();
        btdGuest_mDoMtx_XrotM(m, angle);
        writeJointMatrix(model, index);
        return TRUE;
    }
    if (role <= 1) {
        local.x = 0.0f;
        local.y = 0.0f;
        local.z = 300.0f;
    } else if (role == 2) {
        f32 y = (f32)l_debugRegisters.floats[1], x = (f32)l_debugRegisters.floats[0],
            z = (f32)l_debugRegisters.floats[2];
        local.x = x;
        local.y = gabi::fadds_ppc(y, 50.0f);
        local.z = gabi::fadds_ppc(z, -100.0f);
    } else if (role == 8) {
        f32 x = (f32)l_debugRegisters.floats[5];
        local.z = 0.0f;
        local.y = 100.0f;
        local.x = gabi::fadds_ppc(x, 350.0f);
    } else if (role == 10) {
        f32 x = (f32)l_debugRegisters.floats[5];
        local.y = -100.0f;
        local.z = 0.0f;
        local.x = -gabi::fadds_ppc(x, 350.0f);
    } else {
        f32 x = (f32)l_debugRegisters.floats[6], y = (f32)l_debugRegisters.floats[7],
            z = (f32)l_debugRegisters.floats[8];
        local.x = x;
        local.z = z;
        local.y = y;
    }
    MtxPosition(&local, btdVector(gabi::ea(&btdActor(actor)->m02FC[0].x) + (u32)role * 12));
    if (role == 2) {
        s16 angle = (s16)btdActor(actor)->m5E88;
        m = matrix();
        btdGuest_mDoMtx_YrotM(m, angle);
        angle = (s16)btdActor(actor)->m5E8A;
        m = matrix();
        btdGuest_mDoMtx_XrotM(m, angle);
        angle = (s16)btdActor(actor)->m5E88;
        m = matrix();
        btdGuest_mDoMtx_ZrotM(m, angle);
        writeJointMatrix(model, index, true);
    }
    return TRUE;
}

/* 020F0A74 */
BOOL daBtd_Draw(btd_class *i_this) {
    u32 p = gabi::ea(i_this);
    u8 blur = (u8)i_this->m6E15;
    if (blur > 1) {
        (l_blurAlpha = (u8)(blur));
        btdGuest_mDoGph_gInf_c_onBlure();
    } else if (blur == 1) {
        i_this->m6E15 = (u8)(0);
        (l_blurEnabled = (u8)(0));
    }
    s16 angle = (s16)i_this->current.angle.y;
    btdGuest_dSnap_RegistFig(0xC7, p, gabi::ea(&i_this->eyePos), angle, 1.0f, 1.0f, 1.0f);
    if ((u8)i_this->m6E4C) {
        u32 env = btdGuest_getEnvironmentLight();
        btdGuest_dScnKy_env_light_c_settingTevStruct(env, 0, gabi::ea(&i_this->m6E50.x),
                                                     gabi::ea(&i_this->mTevstr));
        u32 model = (u32)btdMorph((u32)i_this->mpDeadHeadMorf.v)->mpModel.v;
        env = btdGuest_getEnvironmentLight();
        btdGuest_setLightTevColorType(env, model, gabi::ea(&i_this->mTevstr));
        u32 btk = (u32)i_this->mpDeadHeadBtkAnm.v;
        u32 data = (u32)btdModel(model)->mModelData.v;
        f32 frame = (f32)btdBaseAnimation(btk)->mFrameCtrl.mFrame;
        btdTextureAnimation(btk)->entry(btdModelData(data), frame);
        u32 brk = (u32)i_this->mpDeadHeadBrkAnm.v;
        data = (u32)btdModel(model)->mModelData.v;
        frame = (f32)btdBaseAnimation(brk)->mFrameCtrl.mFrame;
        btdColorAnimation(brk)->entry(btdModelData(data), frame);
        u32 morf = (u32)i_this->mpDeadHeadMorf.v;
        btdMorph(morf)->entryDL();
        return TRUE;
    }
    u32 morf = gabi::ea(get_anm(i_this));
    u32 btk = gabi::ea(get_btk(i_this));
    u32 brk = gabi::ea(get_brk(i_this));
    u32 env = btdGuest_getEnvironmentLight();
    btdGuest_dScnKy_env_light_c_settingTevStruct(env, 0, gabi::ea(&i_this->eyePos),
                                                 gabi::ea(&i_this->mTevstr));
    u32 model = (u32)btdMorph(morf)->mpModel.v;
    env = btdGuest_getEnvironmentLight();
    btdGuest_setLightTevColorType(env, model, gabi::ea(&i_this->mTevstr));
    f32 frame = (f32)btdBaseAnimation(btk)->mFrameCtrl.mFrame;
    u32 data = (u32)btdModel(model)->mModelData.v;
    btdTextureAnimation(btk)->entry(btdModelData(data), frame);
    frame = (f32)btdBaseAnimation(brk)->mFrameCtrl.mFrame;
    data = (u32)btdModel(model)->mModelData.v;
    btdColorAnimation(brk)->entry(btdModelData(data), frame);
    btdMorph(morf)->entryDL();
    for (u32 i = 0; i < 35; ++i) {
        u32 piece = gabi::ea(&i_this->hahen[i]);
        if ((u8)btdDebris(piece)->m04) {
            env = btdGuest_getEnvironmentLight();
            u32 pieceModel = (u32)btdDebris(piece)->model.v;
            btdGuest_setLightTevColorType(env, pieceModel, gabi::ea(&i_this->mTevstr));
            pieceModel = (u32)btdDebris(piece)->model.v;
            btdGuest_modelUpdateDL(pieceModel);
        }
    }
    return TRUE;
}

/* 020F0C38 */
void eff_off(btd_class *i_this) {
    for (u32 i = 0; i < 7; ++i)
        if ((s16)i_this->m6028[i] > 2)
            (i_this->m6028[i] = (s16)(2));
}

/* 020F0C68 */
void hahen_set2(btd_class *i_this) {
    cXyz position;

    f32 debug = (f32)l_debugRegisters.floats[14];
    f32 x = (f32)i_this->m02FC[8].x, y = (f32)i_this->m02FC[8].y, z = (f32)i_this->m02FC[8].z;
    f32 height = gabi::fadds_ppc(debug, 100.0f);
    position.z = z;
    position.x = x;
    position.y = gabi::fsubs_ppc(y, height);
    u32 control = (u32)btdPlay()->mParticle.v;
    btdGuest_dPa_control_c_setDiscardResult(control, 0, 0x80AA, &position, 0, 0, 255, 0, -1, 0, 0,
                                            0);
    debug = (f32)l_debugRegisters.floats[14];
    x = (f32)i_this->m02FC[10].x;
    height = gabi::fadds_ppc(debug, 100.0f);
    y = (f32)i_this->m02FC[10].y;
    z = (f32)i_this->m02FC[10].z;
    position.x = x;
    position.z = z;
    position.y = gabi::fsubs_ppc(y, height);
    control = (u32)btdPlay()->mParticle.v;
    btdGuest_dPa_control_c_setDiscardResult(control, 0, 0x80AA, &position, 0, 0, 255, 0, -1, 0, 0,
                                            0);
    s32 room = (s8)i_this->current.roomNo, reverb = btdGuest_dComIfGp_getReverb(room);
    mDoAud_seStart(0x581B, &i_this->m02FC[8], 0, reverb);
    room = (s8)i_this->current.roomNo;
    reverb = btdGuest_dComIfGp_getReverb(room);
    mDoAud_seStart(0x581B, &i_this->m02FC[10], 0, reverb);
    u32 game = play();
    s16 strength = (s16)l_debugRegisters.shorts[0];
    position.x = 0.0f;
    position.y = 1.0f;
    position.z = 0.0f;
    btdGuest_dVibration_c_StartShock(game + 0x599C, (s32)strength + 5, -33, &position);
}

/* 020F0EB4 */
void hahen_set(btd_class *i_this, s32 side) {
    u32 p = gabi::ea(i_this);
    s16 x = (s16)i_this->current.angle.x, y = (s16)i_this->current.angle.y,
        z = (s16)i_this->current.angle.z;
    i_this->m6048.y = (s16)(y);
    i_this->m6048.z = (s16)(z);
    i_this->m6048.x = (s16)(x);
    i_this->m6048.y = (s16)((s16)(y + (side ? 0x4000 : -0x4000)));
    u32 source = p + (side ? 0x49C : 0x484);
    // cXyz assignment: copied as words (lwz/stw), so a signalling-NaN bit pattern is kept
    u32 px = btdWord(btdVector(source)->x), pyBits = btdWord(btdVector(source)->y),
        pz = btdWord(btdVector(source)->z);
    btdSetWord(i_this->m603C.x, px);
    btdSetWord(i_this->m603C.y, pyBits);
    btdSetWord(i_this->m603C.z, pz);
    f32 py = (f32)btdVector(source)->y;
    f32 debug = (f32)l_debugRegisters.floats[14];
    i_this->m603C.y = (f32)(gabi::fsubs_ppc(py, gabi::fadds_ppc(debug, 100.0f)));
    u32 modelData = resource(gabi::guest_addr("Always"), 0x30),
        animation = resource(gabi::guest_addr("Always"), 0x66);
    u32 control = (u32)btdPlay()->mParticle.v;
    u32 emitter = btdGuest_dPa_control_c_set(control, 0, 0x80A8, gabi::ea(&i_this->m603C.x),
                                             gabi::ea(&i_this->m6048.x), 0, 255, 0, -1, 0, 0, 0);
    if (emitter) {
        (btdEmitter(emitter)->globalTranslation.y = (f32)(4.0f));
        (btdEmitter(emitter)->globalTranslation.z = (f32)(4.0f));
        (btdEmitter(emitter)->globalTranslation.x = (f32)(4.0f));
        u32 modelEmitter = btdGuest_dPa_J3DmodelEmitter_c_dPa_J3DmodelEmitter_c(
            0, emitter, modelData, 1, gabi::ea(&i_this->mTevstr), animation, 1, 0);
        if (modelEmitter) {
            control = (u32)btdPlay()->mParticle.v;
            u32 list = (u32)btdParticleControl(control)->emitterList;
            btdGuest_cLs_Addition(list, modelEmitter);
        }
    }
    control = (u32)btdPlay()->mParticle.v;
    btdGuest_dPa_control_c_setDiscardResult(control, 0, 0x80A9, gabi::ea(&i_this->m603C.x), 0, 0,
                                            255, 0, -1, 0, 0, 0);
    smoke_set_s(i_this, &i_this->m603C,
                &i_this->m6048);
}

/* 020F0DFC */
void smoke_set_s(btd_class *i_this, cXyz *position, csXyz *angles) {
    s32 room = (s8)i_this->current.roomNo;
    u32 control = (u32)btdPlay()->mParticle.v;
    btdGuest_dPa_control_c_setDiscardResult(control, 2, 0xA0AB, position, angles, 0, 0xB9,
                                            gabi::ea(&i_this->mPa_SmokeEcallBack[0]), room, 0, 0,
                                            0);
    room = (s8)i_this->current.roomNo;
    control = (u32)btdPlay()->mParticle.v;
    btdGuest_dPa_control_c_setDiscardResult(control, 2, 0xA0AC, position, angles, 0, 0xB9,
                                            gabi::ea(&i_this->mPa_SmokeEcallBack[1]), room, 0, 0,
                                            0);
}

/* 020F4924 */
void startdemo(btd_class *i_this) {
    u32 p = gabi::ea(i_this), player = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;
    i_this->m02F0 = (s16)(30);
    u32 target = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;
    s16 angle = btdGuest_fopAcM_searchActorAngleY(p, target);
    i_this->current.angle.y = (s16)(angle);

    cXyz center;
    cXyz difference;

    center.z = 909.0f;
    center.x = -1534.0f;
    center.y = 0.0f;
    btdGuest_cXyz_op_sub(gabi::ea(&btdPlayer(player)->current.pos), &difference, &center);
    u16 mode = (u16)(s16)i_this->mGohmaState;
    if (mode == 0) {
        if ((s16)i_this->m6E16 != 0)
            return;
        i_this->m6E88 = (u8)(0);
        i_this->mKankyoState = (u8)(6);
        i_this->current.pos.y = (f32)(-10000.0f);
        if ((u16)i_this->eventInfo.mCommand == 2) {
            i_this->m6E16 = (s16)(1);
            btdGuest_subBgmStop();
            return;
        }
        f32 magnitude = PSVECSquareMag(&difference);
        f32 distance = btdGuest_sqrtf(magnitude);
        f32 radius = (f32)l_debugRegisters.floats[13];
        if (distance > gabi::fadds_ppc(radius, 200.0f)) {
            btdGuest_fopAcM_orderPotentialEvent(p, 2, 0xFFFF, 0);
            u16 flags = (u16)i_this->eventInfo.mCondition;
            i_this->eventInfo.mCondition = (u16)(flags | 2);
        }
        return;
    }
    if (mode == 1) {
        i_this->mKankyoState = (u8)(8);
        anm_init(i_this, 0x2D, 1.0f, 0, 1.0f, -1);
        f32 y = (f32)i_this->home.pos.y;
        i_this->mGohmaState = (s16)(2);
        i_this->current.pos.y = (f32)(y);
        actorSound(p, 0x483D);
        center.x = 0.0f;
        center.y = 0.0f;
        center.z = 0.0f;
        u32 control = (u32)btdPlay()->mParticle.v;
        btdGuest_dPa_control_c_setDiscardResult(
            control, 0, 0x80BD, &center, gabi::ea(&i_this->current.angle), 0, 255, 0, -1, 0, 0, 0);
        return;
    }
    if (mode != 2)
        return;
    s16 timer = (s16)i_this->m6E1A;
    if (timer <= 70) {
        timer = (s16)i_this->m6E1A;
        i_this->mKankyoState = (u8)(8);
    }
    if (timer == 40 || timer == 220) {
        actorSound(p, 0x4837);
        timer = (s16)i_this->m6E1A;
    }
    if (timer == 215) {
        wave_set(i_this);
        i_this->m6E48 = (f32)(40.0f);
        hahen_set2(i_this);
        timer = (s16)i_this->m6E1A;
    }
    if ((u32)((s32)timer - 70) < 126) {
        timer = (s16)i_this->m6E1A;
        i_this->m6028[3] = (s16)(3);
        if (timer >= 90) {
            if (p + 0x37C) {
                s32 room = (s8)i_this->current.roomNo, reverb = btdGuest_dComIfGp_getReverb(room);
                mDoAud_seStart(0x501F, &i_this->eyePos, 0, reverb);
            }
            timer = (s16)i_this->m6E1A;
            i_this->mKankyoState = (u8)(2);
        }
    }
    u32 morf = (u32)i_this->mpPhase1Morf.v;
    if (timer >= 70)
        i_this->m6028[2] = (s16)(3);
    if (!((u8)btdMorph(morf)->mFrameCtrl.mState & 1) &&
        (f32)btdMorph(morf)->mFrameCtrl.mRate != 0.0f)
        return;
    if ((f32)l_debugRegisters.floats[7] != 0.0f)
        return;
    i_this->m02E4 = ACT_WAIT;
    i_this->m6E16 = (s16)(150);
    i_this->mGohmaState = (s16)(0);
    f32 random = cM_rndF(50.0f);
    i_this->m02EC[0] = (s16)((s16)gabi::ftoi(gabi::fadds_ppc(random, 70.0f)));
    u8 stage = gabi::load<u8>(gabi::ea(&btdPlay()->mCurStage.mName[0]));
    btdGuest_bgmStart(stage == 0x58 ? 0x8000004Bu : 0x80000003u);
    u32 save = dComIfGs_ea();
    btdGuest_dSv_memBit_c_onDungeonItem(save + 0x798, 5);
}

/* 020F10B4 */
void sibuki_set(btd_class *i_this) {
    cXyz local;

    local.x = 0.0f;
    i_this->m6E14 = (s8)(0);
    for (u32 i = 0; i < 50; ++i) {
        u32 spray = gabi::ea(&i_this->mBtdSibukiS[i]);
        if ((s8)btdSplash(spray)->m00 != 0)
            continue;
        (btdSplash(spray)->m00 = (s8)(1));
        u32 m = matrix();
        f32 random = cM_rndFX(32768.0f);
        btdGuest_mDoMtx_YrotS(m, (s16)gabi::ftoi(random));
        random = cM_rndFX(200.0f);
        f32 radius = gabi::fadds_ppc(random, 300.0f);
        f32 debug = (f32)l_debugRegisters.floats[13];
        local.z = gabi::fadds_ppc(radius, debug);
        local.y = cM_rndF(50.0f);
        MtxPosition(&local, btdVector(spray + 4));
        f32 forward = (f32)l_HIO.m48;
        random = cM_rndF(gabi::fmuls_ppc(forward, 0.5f));
        f32 upward = (f32)l_HIO.m4C;
        f32 vz = gabi::fadds_ppc(forward, random);
        local.z = vz;
        random = cM_rndF(gabi::fmuls_ppc(upward, 0.5f));
        local.y = gabi::fadds_ppc(upward, random);
        MtxPosition(&local, btdVector(spray + 0x10));
        m = matrix();
        random = cM_rndFX(32768.0f);
        btdGuest_mDoMtx_YrotS(m, (s16)gabi::ftoi(random));
    }
}

/* 020F1288 */
void kubi_calc(btd_class *i_this) {
    f32 x = (f32)i_this->m6E50.x, y = (f32)i_this->m6E50.y, z = (f32)i_this->m6E50.z;
    MtxTrans(x, y, z, 0);
    s16 angle = (s16)i_this->m6E74.y;
    u32 m = matrix();
    btdGuest_mDoMtx_YrotM(m, angle);
    angle = (s16)i_this->m6E74.x;
    m = matrix();
    btdGuest_mDoMtx_XrotM(m, angle);
    m = matrix();
    angle = (s16)i_this->m6E74.z;
    btdGuest_mDoMtx_ZrotM(m, angle);
    u32 morf = (u32)i_this->mpDeadHeadMorf.v;
    m = matrix();
    u32 model = (u32)btdMorph(morf)->mpModel.v;
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)btdMatrix(m)->m[i / 4][i % 4];
    // The source matrix is fully loaded before any destination component is stored.
    const u8 storeOrder[] = {7, 3, 6, 4, 9, 5, 10, 0, 1, 8, 2, 11};
    for (u8 i : storeOrder)
        (btdModel(model)->mBaseTransformMtx.m[i / 4][i % 4] = (f32)(values[i]));
    morf = (u32)i_this->mpDeadHeadMorf.v;
    btdMorph(morf)->calc();
    u32 btk = (u32)i_this->mpDeadHeadBtkAnm.v;
    btdBaseAnimation(btk)->play();
    u32 brk = (u32)i_this->mpDeadHeadBrkAnm.v;
    btdBaseAnimation(brk)->play();
}

/* 020F52B8 */
void end(btd_class *i_this) {
    const u32 p = gabi::ea(i_this), center = gabi::ea(&static_center_pos);
    btdEndPlay();
    const u32 morf = btdGuest_get_anm(p);
    const s16 state = (s16)i_this->mGohmaState;
    i_this->mActionState = (s16)(10);
    switch (state) {
    case 0x32:
        if (gabi::load<u8>(gabi::ea(&btdPlay()->mCurStage.mName[0])) == 0x58) {
            s32 room = (s8)i_this->current.roomNo;
            btdGuest_dLib_setNextStageBySclsNum(0, room);
            btdEndSound(i_this, 0, 0x2888);
            u32 save = dComIfGs_ea();
            btdGuest_dSv_event_c_onEventBit(save + 0x644, 0x3240);
            save = dComIfGs_ea();
            btdGuest_dSv_event_c_onEventBit(save + 0x1178, 0x480);
            btdGuest_bgmStop(30);
        } else {
            i_this->mGohmaState = (s16)(0x33);
            btdGuest_anm_init(p, 12, 0.0f, 0, 1.0f, -1);
            i_this->m6E18 = (s16)(0);
            i_this->m6E1A = (s16)(0);
            i_this->m6E16 = (s16)(100);
            s32 delay = (s16)l_debugRegisters.shorts[1];
            i_this->m02EC[0] = (s16)((s16)(delay + 540));
            btdGuest_bgmStop(20);
            btdGuest_subBgmStart(0xC0000003u);
            btdGuest_wave_set(p);
        }
        return;
    case 0x33: {
        s16 frame = (s16)i_this->m6E1A;
        if (frame == 3) {
            btdEndMonsterSound(i_this, 0x4837);
            frame = (s16)i_this->m6E1A;
        }
        if (frame == 64) {
            btdEndMonsterSound(i_this, 0x483B);
            btdGuest_wave_set(p);
            frame = (s16)i_this->m6E1A;
        }
        if (frame == 120) {
            if (p && p + 0x37C)
                btdEndSound(i_this, gabi::ea(&i_this->eyePos), 0x5837);
            btdGuest_wave_set(p);
            frame = (s16)i_this->m6E1A;
        }
        if (frame == 160) {
            btdEndMonsterSound(i_this, 0x4839);
            frame = (s16)i_this->m6E1A;
        }
        if (frame == 200) {
            (btdVector(center)->x = (f32)(0.0f));
            (btdVector(center)->y = (f32)(300.0f));
            (btdVector(center)->z = (f32)(0.0f));
            btdEndSound(i_this, center, 0x5838);
            btdEndMonsterSound(i_this, 0x483C, true);
            frame = (s16)i_this->m6E1A;
        }
        if (frame == 310)
            btdEndSound(i_this, center, 0x5839);
        s16 timer = (s16)i_this->m02EC[0];
        if (timer == 101) {
            btdGuest_subBgmStop();
            u32 z = btdWord(i_this->eyePos.z), y = btdWord(i_this->eyePos.y);
            btdSetWord(i_this->m6E50.z, z);
            u16 ax = (u16)i_this->current.angle.x;
            btdSetWord(i_this->m6E50.y, y);
            i_this->m6E74.x = (s16)(ax);
            u16 ay = (u16)i_this->current.angle.y;
            i_this->m6E4C = (u8)(1);
            s16 next = (s16)i_this->mGohmaState, count = (s16)i_this->m6E16;
            i_this->m6E40 = (f32)(0.0f);
            i_this->m6E74.y = (s16)(ay);
            u32 x = btdWord(i_this->eyePos.x);
            i_this->mGohmaState = (s16)((s16)(next + 1));
            btdSetWord(i_this->m6E50.x, x);
            u16 az = (u16)i_this->current.angle.z;
            i_this->current.pos.y = (f32)(-10000.0f);
            i_this->m6E74.z = (s16)(az);
            i_this->m02EC[0] = (s16)(70);
            i_this->m6E1A = (s16)(0);
            i_this->m6E16 = (s16)((s16)(count + 1));
            i_this->speedF = (f32)(30.0f);
            btdEndSound(i_this, center, 0x583A);
            u32 control = (u32)btdPlay()->mParticle.v;
            u32 emitter = btdGuest_dPa_control_c_set(control, 0, 0x80DE, gabi::ea(&i_this->m6E50.x),
                                                     0, 0, 255, 0, -1, 0, 0, 0);
            i_this->m6038.v = (u32)(emitter);
            i_this->m6E48 = (f32)(30.0f);

            cXyz axis;
            btdEndShock(axis);
            i_this->m6E15 = (u8)(180);
            return;
        }
        if (timer == 460) {
            timer = (s16)i_this->m02EC[0];
            i_this->m02E0 = (u8)(2);
        }
        if (timer == 241) {
            u32 model = (u32)btdMorph(morf)->mpModel.v;
            u32 animation = btdEndResource(gabi::guest_addr("Btd"), 0x4A);
            u32 data = (u32)btdModel(model)->mModelData.v, brk = (u32)i_this->mpDeadBodyBrkAnm.v;
            btdGuest_brkInitNative(brk, data, animation, 1, 1.0f, 0, 0, -1, 1, 0);
            animation = btdEndResource(gabi::guest_addr("Btd"), 0x54);
            u32 btk = (u32)i_this->mpDeadBodyBtkAnm.v;
            data = (u32)btdModel(model)->mModelData.v;
            btdGuest_btkInitNative(btk, data, animation, 1, 1.0f, 0, 0, -1, 1, 0);
            timer = (s16)i_this->m02EC[0];
        }
        if (timer == 191)
            for (u32 i = 0; i < 19; ++i) {
                u16 effect = (u16)last_effname[(2 * i) / 2];
                u32 control = (u32)btdPlay()->mParticle.v;
                u32 emitter = btdGuest_dPa_control_c_set(
                    control, 0, effect, gabi::ea(&i_this->current.pos), 0, 0, 255, 0, -1, 0, 0, 0);
                if (emitter) {
                    u32 model = (u32)btdMorph(morf)->mpModel.v,
                        joint = (u32)last_eff_index[(4 * i) / 4];
                    u32 block = (u32)btdModel(model)->mpMtxBuffer.v,
                        matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
                    u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
                    (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
                    btdGuest_JPASetRMtxTVecfromMtx(matrices + joint * 48, emitter + 0x1F0,
                                                   emitter + 0x22C);
                }
            }
        return;
    }
    case 0x34: {

        cXyz difference;
        cXyz vector;

        i_this->m6E88 = (u8)(0);
        btdGuest_cXyz_op_sub(gabi::ea(&i_this->m6E5C.x), &difference, gabi::ea(&i_this->m6E50.x));
        f32 z = difference.z, x = difference.x, y = difference.y;
        vector.x = x;
        vector.y = y;
        vector.z = z;
        f32 distance = btdGuest_sqrtf(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
        f32 factor = gabi::fadds_ppc((f32)l_debugRegisters.floats[14], 3.5f);
        vector.y = gabi::fmadds(distance, factor, (f32)vector.y);
        s16 yaw = cM_atan2s((f32)vector.x, (f32)vector.z);
        z = vector.z;
        x = vector.x;
        i_this->m6E74.y = (s16)(yaw);
        distance = btdGuest_sqrtf(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
        s32 pitch = btdGuest_cM_atan2sFullResult((f32)vector.y, distance);
        s16 negativePitch = (s16)(0u - (u32)pitch);
        yaw = (s16)i_this->m6E74.y;
        btdGuest_mDoMtx_YrotS(btdEndMatrix(), yaw);
        btdGuest_mDoMtx_XrotM(btdEndMatrix(), negativePitch);
        vector.x = 0.0f;
        vector.y = 0.0f;
        vector.z = (f32)i_this->speedF;
        MtxPosition(&vector, &i_this->m6E68);
        PSVECAdd(&i_this->m6E50, &i_this->m6E68, &i_this->m6E50);
        // Native bge is !LT; only strict LT takes the update.
        if ((f32)i_this->m6E68.y < 0.0f)
            btdGuest_cLib_addCalc2(gabi::ea(&i_this->speedF), 300.0f, 1.0f, 2.0f);
        btdEndCopy3(gabi::ea(&i_this->eyePos), gabi::ea(&i_this->m6E50.x));
        btdGuest_cXyz_op_sub(gabi::ea(&i_this->m6E5C.x), &difference, gabi::ea(&i_this->m6E50.x));
        btdEndCopy3(gabi::ea(&vector), gabi::ea(&difference));
        f32 square = PSVECSquareMag(&vector);
        distance = btdGuest_sqrtf(square);
        f32 speed = (f32)i_this->speedF;
        if (distance < gabi::fadds_ppc(speed, speed))
            i_this->mGohmaState = (s16)(0x35);
        break;
    }
    case 0x35: {
        f32 vx = (f32)i_this->m6E68.x, targetX = (f32)i_this->m6E5C.x;
        i_this->m6E88 = (u8)(0);
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m6E50.x), targetX, 1.0f, std::fabs(vx));
        f32 vy = (f32)i_this->m6E68.y, y = (f32)i_this->m6E50.y;
        y = gabi::fadds_ppc(y, vy);
        i_this->m6E50.y = (f32)(y);
        f32 gravity = gabi::fadds_ppc((f32)l_debugRegisters.floats[3], 5.0f),
            targetY = (f32)i_this->m6E5C.y;
        i_this->m6E68.y = (f32)(gabi::fsubs_ppc(vy, gravity));
        if (!(y > targetY)) {
            vy = (f32)i_this->m6E68.y;
            i_this->m6E50.y = (f32)(targetY);
            if (vy < -50.0f) {
                f32 bounce = gabi::fadds_ppc((f32)l_debugRegisters.floats[4], -0.3f);
                i_this->m6E48 = (f32)(40.0f);
                i_this->m6E68.y = (f32)(gabi::fmuls_ppc(vy, bounce));

                cXyz axis;
                btdEndShock(axis);
                if (p && p + 0x37C)
                    btdEndSound(i_this, gabi::ea(&i_this->eyePos), 0x583B);
                i_this->mGohmaState = (s16)(0x36);
                i_this->m02EC[0] = (s16)(50);
                const u32 staticPosition = gabi::ea(&l_endPosition);
                if (!(u32)l_endPositionInitialized) {
                    l_endPositionInitialized = 1;
                    (btdVector(staticPosition)->x = (f32)((f32)i_this->m6E50.x));
                    (btdVector(staticPosition)->y = (f32)((f32)i_this->m6E50.y));
                    (btdVector(staticPosition)->z = (f32)((f32)i_this->m6E50.z));
                }
                btdVector(staticPosition)->y =
                    gabi::fsubs_ppc((f32)btdVector(staticPosition)->y, 100.0f);
                btdGuest_smoke_set_s(p, staticPosition, gabi::ea(&i_this->m6E74.x));
                u32 emitter = (u32)i_this->m6038.v;
                if (emitter) {
                    u32 flags = (u32)btdEmitter(emitter)->status;
                    (btdEmitter(emitter)->maxFrame = (s32)(-1));
                    (btdEmitter(emitter)->status = (u32)(flags | 1));
                    i_this->m6038.v = (u32)(0);
                }
            }
        }
        f32 vz = (f32)i_this->m6E68.z, targetZ = (f32)i_this->m6E5C.z;
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m6E50.z), targetZ, 1.0f, std::fabs(vz));
        btdEndCopy3(gabi::ea(&i_this->eyePos), gabi::ea(&i_this->m6E50.x));
        break;
    }
    case 0x36: {
        i_this->m6E88 = (u8)(0);
        btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m6E74.x), 0x2000, 2, 0x200);
        btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m6E74.z), 0x2000, 2, 0x200);
        f32 vy = (f32)i_this->m6E68.y, y = (f32)i_this->m6E50.y;
        y = gabi::fadds_ppc(y, vy);
        i_this->m6E50.y = (f32)(y);
        f32 gravity = gabi::fadds_ppc((f32)l_debugRegisters.floats[3], 5.0f),
            targetY = (f32)i_this->m6E5C.y;
        f32 x = (f32)i_this->m6E50.x;
        i_this->m6E68.y = (f32)(gabi::fsubs_ppc(vy, gravity));
        if (!(y > targetY)) {
            y = targetY;
            i_this->m6E50.y = (f32)(y);
            i_this->m6E68.y = (f32)(0.0f);
        }
        s16 timer = (s16)i_this->m02EC[0];
        f32 z = (f32)i_this->m6E50.z;
        i_this->eyePos.x = (f32)(x);
        i_this->eyePos.y = (f32)(y);
        i_this->eyePos.z = (f32)(z);
        if (timer == 1) {
            i_this->m6E50.y = (f32)(gabi::fsubs_ppc((f32)i_this->m6E50.y, 100.0f));
            if (p + 0x37C)
                btdEndSound(i_this, gabi::ea(&i_this->eyePos), 0x583C);
            btdGuest_fopAcM_createDisappear(p, gabi::ea(&i_this->m6E50.x), 15, 2, 255);
            i_this->m6E50.y = (f32)(-10000.0f);
            i_this->m02EC[0] = (s16)(150);
            i_this->mGohmaState = (s16)(0x37);
        }
        btdGuest_kubi_calc(p);
        return;
    }
    case 0x37: {
        s16 timer = (s16)i_this->m02EC[0];
        i_this->m6E88 = (u8)(0);
        if (timer == 2) {
            u32 dragon = (u32)dr2.v;
            btdDragon(dragon)->m43C = 1;
            timer = (s16)i_this->m02EC[0];
            i_this->m6E84 = (f32)(-200.0f);
        }
        if (timer == 1) {
            s16 count = (s16)i_this->m6E16;
            i_this->mGohmaState = (s16)(0x38);
            i_this->m6E16 = (s16)((s16)(count + 1));
        }
        return;
    }
    case 0x39:
        i_this->mKankyoState = (u8)(6);
        return;
    default:
        return;
    }
    // Shared native tail for states34/35: tumble, calculate head, follow emitter.
    s32 ax = (s16)l_debugRegisters.shorts[1], angle = (s16)i_this->m6E74.x;
    i_this->m6E74.x = (s16)((s16)(angle + ax + 1000));
    s32 az = (s16)l_debugRegisters.shorts[2];
    angle = (s16)i_this->m6E74.z;
    i_this->m6E74.z = (s16)((s16)(angle + az + 1500));
    btdGuest_kubi_calc(p);
    u32 emitter = (u32)i_this->m6038.v;
    if (emitter)
        btdEndFollowEmitter(i_this, emitter);
}

/* 020F6004 */
void move(btd_class *i_this) {
    u32 p = gabi::ea(i_this), player = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;
    i_this->m5E84 = (u8)(0);
    i_this->attention_info.flags = (u32)(4);
    switch (u16((s16)i_this->m02E4)) {
    case ACT_WAIT:
        btdActionWait(i_this);
        break;
    case ACT_ATTACK: {
        s32 attack = (s8)i_this->mAttackType;
        i_this->m02F0 = (s16)(5);
        switch (attack) {
        case ATTACK_JAB:
            btdActionJab(i_this);
            break;
        case ATTACK_PUNCH:
            btdActionPunch(i_this);
            break;
        case ATTACK_FIRE:
            btdActionFire(i_this);
            break;
        case ATTACK_HIGH_FIRE:
            btdActionHighFire(i_this, false);
            break;
        case ATTACK_SIDE_FIRE:
            btdActionHighFire(i_this, true);
            break;
        case ATTACK_TURNING_PUNCH:
            btdActionTurningPunch(i_this);
            break;
        }
    } break;
    case ACT_DAMAGE:
        btdActionDamage(i_this);
        break;
    case ACT_START_DEMO:
        btdGuest_startdemo(p);
        i_this->attention_info.flags = (u32)(0);
        break;
    case ACT_END:
        btdGuest_end(p);
        i_this->attention_info.flags = (u32)(0);
        break;
    }
    btdGuest_damage_check(p);
    btdActionTrack(i_this, player);
}

/* 020F1378 */
void *k_a_d_sub(void *actor, void *context) {
    s32 isActor = btdGuest_fopAc_IsActor(actor);
    u32 p = gabi::ea(actor);
    if (isActor && p && (s16)btdActor(p)->mProcName == 0xFA &&
        (s32)btdActor(p)->mParameters == 0x511)
        return actor;
    return nullptr;
}

/* 020F13D4 */
void *dr2_a_d_sub(void *actor, void *context) {
    s32 isActor = btdGuest_fopAc_IsActor(actor);
    u32 p = gabi::ea(actor);
    if (isActor && p && (s16)btdActor(p)->mProcName == 0xDF)
        return actor;
    return nullptr;
}

/* 020F1424 */
void *wepon_s_sub(void *actor, void *context) {
    s32 isActor = btdGuest_fopAc_IsActor(actor);
    u32 p = gabi::ea(actor);
    if (!p)
        return nullptr;
    s16 id = (s16)btdActor(p)->mProcName;
    return (isActor && id == 0x1BE) || id == 0x1B0 ? actor : nullptr;
}

/* 020F90C4 */
void btd_effect(btd_class *i_this) {
    u32 morf = gabi::ea(get_anm(i_this));
    for (u32 i = 0; i < 5; ++i) {
        u32 emitter = (u32)i_this->m5FF8[i].v;
        if (emitter) {
            u32 joint = (u32)eff_index[(4 * i) / 4], model = (u32)btdMorph(morf)->mpModel.v;
            u32 block = (u32)btdModel(model)->mpMtxBuffer.v;
            u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
            u32 matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
            (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
            u32 m = matrix();
            PSMTXCopy(btdMatrix(matrices + joint * 48), btdMatrix(m));
            if ((u8)i_this->m02E0 >= 2)
                MtxTrans(0.0f, -10000.0f, 0.0f, 0);
            emitter = (u32)i_this->m5FF8[i].v;
            m = matrix();
            btdGuest_JPASetRMtxTVecfromMtx(m, emitter + 0x1F0, emitter + 0x22C);
        } else {
            u16 id = (u16)eff_name[(2 * i) / 2];
            u32 control = (u32)btdPlay()->mParticle.v;
            emitter = btdGuest_dPa_control_c_set(control, 0, id, gabi::ea(&i_this->current.pos), 0,
                                                 0, 255, 0, -1, 0, 0, 0);
            (i_this->m5FF8[i].v = (u32)(emitter));
        }
    }
    for (u32 i = 0; i < 7; ++i) {
        s16 duration = (s16)i_this->m6028[i];
        if (duration) {
            (i_this->m6028[i] = (s16)((s16)(duration - 1)));
            u32 emitter = (u32)i_this->m600C[i].v;
            if (emitter) {
                u32 model = (u32)btdMorph(morf)->mpModel.v,
                    block = (u32)btdModel(model)->mpMtxBuffer.v;
                u32 jointOffset = 0xAE0;
                if (i >= 4)
                    jointOffset = (s8)i_this->m02EA ? 0xD80 : 0xCC0;
                u16 flags = (u16)btdMatrixBuffer(block)->mFlags;
                u32 matrices = (u32)btdMatrixBuffer(block)->mpAnmMtx;
                (btdMatrixBuffer(block)->mFlags = (u16)(flags | 0x10));
                btdGuest_JPASetRMtxTVecfromMtx(matrices + jointOffset, emitter + 0x1F0,
                                               emitter + 0x22C);
            } else if (i == 6) {
                s8 side = (s8)i_this->m02EA, room = (s8)i_this->current.roomNo;
                u32 id = side ? 0xA0B4 : 0xA0B5, control = (u32)btdPlay()->mParticle.v;
                emitter = btdGuest_dPa_control_c_set(
                    control, 2, id, gabi::ea(&i_this->current.pos), 0, 0, 185,
                    gabi::ea(&i_this->mPa_SmokeEcallBack[2]), (s32)room, 0, 0, 0);
                i_this->m600C[6].v = emitter;
            } else {
                auto *table = (s8)i_this->m02EA ? effR_name : effF_name;
                u16 id = (u16)table[i];
                u32 control = (u32)btdPlay()->mParticle.v;
                emitter = btdGuest_dPa_control_c_set(control, 0, id, gabi::ea(&i_this->current.pos),
                                                     0, 0, 255, 0, -1, 0, 0, 0);
                (i_this->m600C[i].v = (u32)(emitter));
                if (i == 5) {
                    emitter = (u32)i_this->m600C[5].v;
                    u32 flags = (u32)btdEmitter(emitter)->status;
                    (btdEmitter(emitter)->status = (u32)(flags | 0x40));
                }
            }
        } else {
            u32 emitter = (u32)i_this->m600C[i].v;
            if (emitter) {
                if (i != 3 && i != 6) {
                    if (i == 5) {
                        u32 last = (u32)i_this->m600C[5].v;
                        u32 flags = (u32)btdEmitter(last)->status;
                        (btdEmitter(last)->status = (u32)(flags & ~0x40u));
                        emitter = (u32)i_this->m600C[i].v;
                    }
                    u32 flags = (u32)btdEmitter(emitter)->status;
                    (btdEmitter(emitter)->maxFrame = (s32)(-1));
                    (btdEmitter(emitter)->status = (u32)(flags | 1));
                }
                (i_this->m600C[i].v = (u32)(0));
            }
        }
    }
}

/* 020F94DC */
void btd_kankyo(btd_class *i_this) {
    s16 timer = (s16)i_this->mKankyoTimer;
    if (timer) {
        i_this->mKankyoTimer = (s16)((s16)(timer - 1));
        i_this->mKankyoState = (u8)(2);
    }
    f32 time = (f32)l_HIO.m30;
    btdGuest_dKy_custom_timeset(time);
    u8 state = (u8)i_this->mKankyoState;
    u32 palette = 0;
    switch (state) {
    case 0:
        btdGuest_cLib_addCalc0(gabi::ea(&i_this->m60D0), 1.0f, 0.04f);
        break;
    case 1:
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m60D4), 1.0f, 1.0f, 0.065f);
        i_this->m60D8 = (u8)((u8)(s16)l_HIO.m3C);
        i_this->m60D9 = (u8)((u8)(s16)l_HIO.m3E);
        i_this->m60DA = (u8)((u8)(s16)l_HIO.m40);
        break;
    case 2:
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m60D0), 1.0f, 1.0f, 0.2f);
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m60D4), 1.0f, 1.0f, 0.22500001f);
        time = (f32)l_HIO.m34;
        btdGuest_dKy_custom_timeset(time);
        i_this->m60D8 = (u8)((u8)(s16)l_HIO.m42);
        i_this->m60D9 = (u8)((u8)(s16)l_HIO.m44);
        i_this->m60DA = (u8)((u8)(s16)l_HIO.m46);
        break;
    case 3:
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m60D0), 0.6f, 1.0f, 0.01f);
        break;
    case 4:
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m60D0), 1.0f, 1.0f, 0.2f);
        break;
    case 5:
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m60D0), 1.0f, 1.0f, 0.005f);
        palette = 1;
        break;
    case 6:
        i_this->m60D0 = (f32)(0.0f);
        palette = 2;
        break;
    case 7:
        btdGuest_cLib_addCalc2(gabi::ea(&i_this->m60D0), 1.0f, 1.0f, 0.03333f);
        time = (f32)l_HIO.m38;
        palette = 2;
        btdGuest_dKy_custom_timeset(time);
        break;
    case 8:
        btdGuest_cLib_addCalc0(gabi::ea(&i_this->m60D0), 1.0f, 0.06666f);
        palette = 3;
        break;
    }
    u8 red = (u8)i_this->m60D8;
    f32 brightness = (f32)i_this->m60D4;
    u8 r = (u8)gabi::ftoi(gabi::fmuls_ppc((f32)red, brightness));
    u8 green = (u8)i_this->m60D9, blue = (u8)i_this->m60DA;
    u8 g = (u8)gabi::ftoi(gabi::fmuls_ppc((f32)green, brightness));
    (i_this->mLightInfluence.mColorR = (s16)(r));
    (i_this->mLightInfluence.mColorG = (s16)(g));
    u8 b = (u8)gabi::ftoi(gabi::fmuls_ppc((f32)blue, brightness));
    (i_this->mLightInfluence.mColorB = (s16)(b));
    f32 radius = (f32)l_debugRegisters.floats[8];
    i_this->mLightInfluence.mPower = gabi::fmuls_ppc(gabi::fadds_ppc(radius, 5000.0f), brightness);
    f32 fluctuation = (f32)l_debugRegisters.floats[9];
    i_this->mLightInfluence.mFluctuation = fluctuation;
    f32 blend = (f32)i_this->m60D0;
    const u8 from[] = {0, 0, 6, 0}, to[] = {3, 5, 7, 7};
    btdGuest_dKy_custom_colset(from[palette], to[palette], blend);
    i_this->mKankyoState = (u8)(0);
    btdGuest_cLib_addCalc0(gabi::ea(&i_this->m60D4), 1.0f, 0.025f);
}

/* 020F1494 */
BOOL daBtd_Execute(btd_class *i_this) {
    const u32 p = gabi::ea(i_this);
    u32 player = (u32)btdPlay()->mPlayerInfo[0].mpPlayer.v;
    u8 stage = gabi::load<u8>(gabi::ea(&btdPlay()->mCurStage.mName[0]));
    s8 force = (s8)i_this->m6E90;
    if (stage == 0x58)
        i_this->m6E15 = (u8)(50);
    if (force) {
        u32 camera = btdGuest_dCam_getBody();
        btdGuest_dCamera_c_SetTypeForce_2(camera, gabi::guest_addr("Boss01"), 0);
    }
    u32 floor = (u32)i_this->floor.v;
    f32 floorY = (f32)i_this->m6E80;
    i_this->m6E88 = (u8)(1);
    btdMagmaFloor(floor)->position.y = floorY;
    f32 rise = gabi::fadds_ppc((f32)l_debugRegisters.floats[15], 30.0f),
        height = (f32)i_this->m6E7C;
    height = gabi::fadds_ppc(height, rise);
    i_this->m6E7C = (f32)(height);
    f32 ceiling = gabi::fadds_ppc((f32)l_debugRegisters.floats[18], 2300.0f);
    if (height > ceiling)
        i_this->m6E7C = (f32)(ceiling);
    u32 btk = btdGuest_getBtkNative(p), brk = btdGuest_getBrkNative(p);
    if (!(u32)kui.v)
        (kui.v = (u32)(btdGuest_fpcEx_Search(gabi::guest_fn((const void *)&k_a_d_sub), p)));
    if (!(u32)dr2.v)
        (dr2.v = (u32)(btdGuest_fpcEx_Search(gabi::guest_fn((const void *)&dr2_a_d_sub), p)));
    s16 count = (s16)i_this->m02E2;
    i_this->attention_info.distances[2] = (u8)(4);
    i_this->m02E2 = (s16)((s16)(count + 1));
    be<s16> *timers[] = {&i_this->m02EC[0], &i_this->m02EC[1], &i_this->m02F0, &i_this->mStunTimer};
    for (be<s16> *timer : timers) {
        s16 remaining = *timer;
        if (remaining)
            *timer = (s16)(remaining - 1);
    }
    s16 actionTimer = (s16)i_this->mActionState;
    if (actionTimer)
        i_this->mActionState = (s16)((s16)(actionTimer - 1));
    btdGuest_move(p);
    f32 scale = (f32)l_HIO.m08;
    i_this->scale.z = (f32)(scale);
    i_this->scale.y = (f32)(scale);
    i_this->scale.x = (f32)(scale);
    u32 morf = btdGuest_get_anm(p), model = (u32)btdMorph(morf)->mpModel.v;
    f32 sz = (f32)i_this->scale.z, sy = (f32)i_this->scale.y;
    (btdModel(model)->mBaseScale.x = (f32)(scale));
    (btdModel(model)->mBaseScale.y = (f32)(sy));
    (btdModel(model)->mBaseScale.z = (f32)(sz));
    f32 z = (f32)i_this->current.pos.z, x = (f32)i_this->current.pos.x,
        y = (f32)i_this->current.pos.y;
    const u32 stackMatrix = gabi::ea(&l_matrixStack);
    PSMTXTrans(btdMatrix(stackMatrix), x, y, z);
    s16 angle = (s16)i_this->current.angle.y;
    btdGuest_mDoMtx_YrotM(stackMatrix, angle);
    angle = (s16)i_this->current.angle.x;
    btdGuest_mDoMtx_XrotM(stackMatrix, angle);
    angle = (s16)i_this->current.angle.z;
    btdGuest_mDoMtx_ZrotM(stackMatrix, angle);
    btdExecModelMatrix(model, stackMatrix);
    if (!(u8)l_HIO.m05)
        btdMorph(morf)->play(&i_this->eyePos, 0, 0);
    btdMorph(morf)->calc();
    u8 phase = (u8)i_this->m02E0;
    f32 radiusScale = phase ? 0.6f : 1.0f;

    cXyz hidden;
    cXyz cylinderLocal;
    cXyz difference;
    cXyz vector;

    for (u32 i = 0; i < 19; ++i) {
        u32 sphere = gabi::ea(&i_this->mAtSphs[i]);
        if (phase && i != 8 && i != 10)
            (btdSphere(sphere)->mGObjTg.mSPrm = (u32)((u32)btdSphere(sphere)->mGObjTg.mSPrm & ~1u));
        f32 radius = (f32)at_size[(4 * i) / 4];
        btdGuest_cM3dGSph_SetR(sphere + 0x118, gabi::fmuls_ppc(radius, radiusScale));
        if ((u8)i_this->m6E4C) {
            hidden.x = 0.0f;
            hidden.y = -20000.0f;
            hidden.z = 0.0f;
            btdGuest_cM3dGSph_SetC(sphere + 0x118, &hidden);
        } else
            btdGuest_cM3dGSph_SetC(sphere + 0x118, gabi::ea(&i_this->m02FC[i]));
        if (i == 8 || i == 10) {
            u8 attack = (u8)i_this->m5E84;
            if (attack == 1 || attack == 2) {
                (btdSphere(sphere)->mObjAt.mSPrm = (u32)((u32)btdSphere(sphere)->mObjAt.mSPrm | 1));
                attack = (u8)i_this->m5E84;
                (btdSphere(sphere)->mAtp = (u8)(attack == 2 ? 2 : 1));
            } else
                (btdSphere(sphere)->mObjAt.mSPrm =
                     (u32)((u32)btdSphere(sphere)->mObjAt.mSPrm & ~1u));
            u32 co = (u32)btdSphere(sphere)->mObjCo.mSPrm,
                target = (u32)btdSphere(sphere)->mObjTg.mSPrm;
            (btdSphere(sphere)->mObjCo.mSPrm = (u32)(co & ~1u));
            (btdSphere(sphere)->mObjTg.mSPrm = (u32)(target & ~1u));
        } else {
            bool attack = i < 2 && (u8)i_this->m5E84 == 3;
            u32 bits = (u32)btdSphere(sphere)->mObjAt.mSPrm;
            (btdSphere(sphere)->mObjAt.mSPrm = (u32)(attack ? bits | 1 : bits & ~1u));
            if (i < 2 && !(u8)i_this->m02E0) {
                u32 co = (u32)btdSphere(sphere)->mObjCo.mSPrm,
                    target = (u32)btdSphere(sphere)->mObjTg.mSPrm;
                (btdSphere(sphere)->mObjCo.mSPrm = (u32)(co & ~1u));
                (btdSphere(sphere)->mObjTg.mSPrm = (u32)(target & ~1u));
            }
        }
        btdExecSetCollision(sphere);
        if (i != 18)
            phase = (u8)i_this->m02E0;
    }
    u8 hand = (u8)i_this->m5E7C;
    s16 adjustment = 0;
    f32 reach = 1.0f;
    if (hand == 2 || hand == 3) {
        s32 delta = (s16)l_debugRegisters.shorts[3] + 4200;
        adjustment = (s16)(hand == 2 ? -delta : delta);
        reach = gabi::fadds_ppc((f32)l_debugRegisters.floats[10], 0.95f);
    }
    hidden.x = 0.0f;
    hidden.y = -10000.0f;
    hidden.z = 0.0f;
    for (u32 i = 0; i < 6; ++i)
        btdGuest_cM3dGCyl_SetC(gabi::ea(&i_this->mHandCyls[i].mCyl), &hidden);
    angle = (s16)((s16)i_this->current.angle.y + adjustment);
    btdGuest_mDoMtx_YrotS(btdExecMatrix(), angle);
    for (u32 i = 0; i < 6; ++i) {
        u32 cylinder = gabi::ea(&i_this->mHandCyls[i]);
        hand = (u8)i_this->m5E7C;
        if (hand == 1 || (hand == 2 && i <= 2) || (hand == 3 && i >= 3)) {
            f32 lx = (f32)handposX[(4 * i) / 4], ly = (f32)handposY[(4 * i) / 4],
                lz = (f32)handposZ[(4 * i) / 4];
            cylinderLocal.x = lx;
            cylinderLocal.y = ly;
            cylinderLocal.z = gabi::fmuls_ppc(lz, reach);
            u32 position = gabi::ea(&i_this->m5E98[19 + i]);
            MtxPosition(&cylinderLocal, btdVector(position));
            PSVECAdd(btdVector(position), &i_this->current.pos, btdVector(position));
            btdGuest_cM3dGCyl_SetC(cylinder + 0x118, position);
            btdGuest_cM3dGCyl_SetH(cylinder + 0x118, (f32)handposH[(4 * i) / 4]);
            btdGuest_cM3dGCyl_SetR(cylinder + 0x118, (f32)handposR[(4 * i) / 4]);
        }
        btdExecSetCollision(cylinder);
    }
    i_this->m5E7C = (u8)(0);
    u32 item = btdGuest_fpcEx_Search(gabi::guest_fn((const void *)&wepon_s_sub), p);
    if (item && (f32)btdBaseActor(item)->speedF > 10.0f) {
        btdGuest_cXyz_op_sub(gabi::ea(&btdActor(item)->current.pos), &difference,
                             gabi::ea(&i_this->eyePos));
        btdExecCopy(gabi::ea(&vector), gabi::ea(&difference));
        f32 square = PSVECSquareMag(&vector), distance = btdGuest_sqrtf(square);
        if (distance < 400.0f)
            i_this->m02F8 = (s16)(20);
    }
    btdGuest_cXyz_op_sub(gabi::ea(&btdPlayer(player)->eyePos), &difference,
                         gabi::ea(&i_this->eyePos));
    btdExecCopy(gabi::ea(&vector), gabi::ea(&difference));
    if (gabi::load<u8>(gabi::ea(&btdPlayer(player)->mCutType))) {
        f32 square = PSVECSquareMag(&vector), distance = btdGuest_sqrtf(square);
        if (distance < 400.0f)
            i_this->m02F8 = (s16)(20);
    }
    s16 eyeTimer = (s16)i_this->m02F8;
    if (eyeTimer && !(u8)i_this->m02E0) {
        i_this->m02F8 = (s16)((s16)(eyeTimer - 1));
        btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m02FA), 13000, 1, 0x1000);
    } else
        btdGuest_cLib_addCalcAngleS2(gabi::ea(&i_this->m02FA), 0, 4, 0x400);
    if (!(s16)i_this->m02F0) {
        f32 random = cM_rndF(150.0f);
        i_this->m02F0 = (s16)((s16)gabi::ftoi(gabi::fadds_ppc(random, 100.0f)));
        i_this->m02F8 = (s16)(20);
    }
    if ((u8)i_this->m02E0)
        i_this->mEyeSph.mGObjTg.mSPrm = (u32)i_this->mEyeSph.mGObjTg.mSPrm & ~1u;
    f32 radius = gabi::fadds_ppc((f32)l_debugRegisters.floats[5], 80.0f);
    btdGuest_cM3dGSph_SetR(gabi::ea(&i_this->mEyeSph.mSph), radius);
    btdGuest_cM3dGSph_SetC(gabi::ea(&i_this->mEyeSph.mSph), gabi::ea(&i_this->m03E0.x));
    btdExecSetCollision(gabi::ea(&i_this->mEyeSph));
    btdBaseAnimation(btk)->play();
    btdBaseAnimation(brk)->play();
    u32 light = btdGuest_getEnvironmentLight();
    btdGuest_dScnKy_env_light_c_settingTevStruct(light, 1, gabi::ea(&i_this->eyePos),
                                                 gabi::ea(&i_this->mTevstr));
    btdExecDebrisMove(i_this);
    btdExecSplashMove(i_this);
    btdExecCamera(i_this);
    btdGuest_btd_effect(p);
    btdGuest_btd_kankyo(p);
    return TRUE;
}

/* 020F34A0 */
BOOL daBtd_IsDelete(btd_class *i_this) {
    return TRUE;
}

/* 020F34A8 */
BOOL daBtd_Delete(btd_class *i_this) {
    u32 p = gabi::ea(i_this);
    btdGuest_dComIfG_resDelete(gabi::ea(&i_this->mPhaseBtd), gabi::guest_addr("Btd"));
    btdGuest_dComIfG_resDelete(gabi::ea(&i_this->mPhaseMagma), gabi::guest_addr("Magma"));
    s32 child = (s8)l_HIO.mNo;
    btdGuest_mDoHIO_subRoot_c_deleteChild(child);
    for (u32 i = 0; i < 19; ++i)
        btdGuest_JAIZelBasic_seDeleteObject(gabi::ea(&i_this->m5E98[i]));
    for (u32 i = 0; i < 50; ++i)
        btdGuest_JAIZelBasic_seDeleteObject(gabi::ea(&i_this->mBtdSibukiS[i].m04));
    for (u32 i = 0; i < 19; ++i)
        btdGuest_JAIZelBasic_seDeleteObject(gabi::ea(&i_this->m02FC[i]));
    u32 peg = (u32)dr2.v;
    if (peg)
        btdGuest_JAIZelBasic_seDeleteObject(gabi::ea(&btdDragon(peg)->m4A8));
    btdGuest_JAIZelBasic_seDeleteObject(gabi::ea(&static_center_pos));
    btdGuest_dKy_efplight_cut(gabi::ea(&i_this->mLightInfluence));
    return TRUE;
}

/* 020F3594 */
BOOL useHeapInit(btd_class *i_this) {
    u32 p = gabi::ea(i_this);
    u32 modelData = resource(gabi::guest_addr("Btd"), 0x40),
        animation = resource(gabi::guest_addr("Btd"), 0x33);
    u32 morf = btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(0, modelData, 0, 0, animation, 2, 1.0f, 0, -1,
                                                      0, 0, 0, 0x11020203u);
    i_this->mpPhase1Morf.v = (u32)(morf);
    if (!morf || !(u32)btdMorph(morf)->mpModel.v)
        return FALSE;
    installGohmaJoints(p, i_this->mpPhase1Morf.v);
    u32 model = (u32)btdMorph((u32)i_this->mpPhase1Morf.v)->mpModel.v;
    newGohmaAnimation(i_this->btk.v, true, 0x156F, gabi::guest_addr("i_this->btk"), true);
    if (!initGohmaAnimation(i_this->btk.v, model, 0x52, true, 2, 0))
        return FALSE;
    newGohmaAnimation(i_this->brk.v, false, 0x1583, gabi::guest_addr("i_this->brk"), true);
    if (!initGohmaAnimation(i_this->brk.v, model, 0x48, false, 2, 0))
        return FALSE;
    modelData = resource(gabi::guest_addr("Btd"), 0x42);
    animation = resource(gabi::guest_addr("Btd"), 0x33);
    morf = btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(0, modelData, 0, 0, animation, 2, 1.0f, 0, -1, 0,
                                                  0, 0, 0x11020203u);
    i_this->mpPhase2Morf.v = (u32)(morf);
    if (!morf || !(u32)btdMorph(morf)->mpModel.v)
        return FALSE;
    installGohmaJoints(p, i_this->mpPhase2Morf.v);
    model = (u32)btdMorph((u32)i_this->mpPhase2Morf.v)->mpModel.v;
    newGohmaAnimation(i_this->btkS.v, true, 0x15AF, gabi::guest_addr("i_this->btkS"), true);
    if (!initGohmaAnimation(i_this->btkS.v, model, 0x53, true, 0, 0))
        return FALSE;
    initGohmaAnimation(i_this->btkS.v, model, 0x56, true, 2, 1);
    newGohmaAnimation(i_this->brkS.v, false, 0x15CE, gabi::guest_addr("i_this->brkS"), true);
    if (!initGohmaAnimation(i_this->brkS.v, model, 0x49, false, 0, 0))
        return FALSE;
    initGohmaAnimation(i_this->brkS.v, model, 0x4F, false, 2, 1);
    modelData = resource(gabi::guest_addr("Btd"), 0x45);
    animation = resource(gabi::guest_addr("Btd"), 0x33);
    morf = btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(0, modelData, 0, 0, animation, 2, 1.0f, 0, -1, 0,
                                                  0, 0, 0x11020203u);
    i_this->mpDeadBodyMorf.v = (u32)(morf);
    if (!morf || !(u32)btdMorph(morf)->mpModel.v)
        return FALSE;
    installGohmaJoints(p, i_this->mpDeadBodyMorf.v);
    model = (u32)btdMorph((u32)i_this->mpDeadBodyMorf.v)->mpModel.v;
    newGohmaAnimation(i_this->mpDeadBodyBtkAnm.v, true, 0x1606,
                      gabi::guest_addr("i_this->btkS_bmt"), true);
    if (!initGohmaAnimation(i_this->mpDeadBodyBtkAnm.v, model, 0x53, true, 0, 0))
        return FALSE;
    newGohmaAnimation(i_this->mpDeadBodyBrkAnm.v, false, 0x161D,
                      gabi::guest_addr("i_this->brkS_bmt"), true);
    if (!initGohmaAnimation(i_this->mpDeadBodyBrkAnm.v, model, 0x49, false, 0, 0))
        return FALSE;
    modelData = resource(gabi::guest_addr("Btd"), 0x41);
    morf = btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(0, modelData, 0, 0, 0, 2, 1.0f, 0, -1, 0, 0, 0,
                                                  0x11020203u);
    i_this->mpDeadHeadMorf.v = (u32)(morf);
    if (!morf || !(u32)btdMorph(morf)->mpModel.v)
        return FALSE;
    MtxScale(0.0f, 0.0f, 0.0f, 0);
    morf = (u32)i_this->mpDeadHeadMorf.v;
    u32 m = matrix();
    model = (u32)btdMorph(morf)->mpModel.v;
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)btdMatrix(m)->m[i / 4][i % 4];
    const u8 order[] = {5, 3, 6, 7, 2, 0, 9, 8, 1, 10, 4, 11};
    for (u8 i : order)
        (btdModel(model)->mBaseTransformMtx.m[i / 4][i % 4] = (f32)(values[i]));
    morf = (u32)i_this->mpDeadHeadMorf.v;
    btdMorph(morf)->calc();
    model = (u32)btdMorph((u32)i_this->mpDeadHeadMorf.v)->mpModel.v;
    if (!newGohmaAnimation(i_this->mpDeadHeadBtkAnm.v, true, 0, 0, false))
        return FALSE;
    if (!initGohmaAnimation(i_this->mpDeadHeadBtkAnm.v, model, 0x55, true, 0, 0))
        return FALSE;
    if (!newGohmaAnimation(i_this->mpDeadHeadBrkAnm.v, false, 0, 0, false))
        return FALSE;
    if (!initGohmaAnimation(i_this->mpDeadHeadBrkAnm.v, model, 0x4E, false, 0, 0))
        return FALSE;
    u32 large = resource(gabi::guest_addr("Btd"), 0x3C),
        small = resource(gabi::guest_addr("Btd"), 0x3D);
    if (!large || !small)
        btdGuest_JUTAssertion_getSDevice(gabi::guest_addr("d_a_btd.cpp"), 0x166C,
                                         gabi::guest_addr("modelDataA && modelDataB"));
    for (u32 i = 0; i < 35; ++i) {
        u32 model = btdGuest_mDoExt_J3DModel_create(i < 5 ? small : large, 0, 0x11020203u);
        u32 piece = gabi::ea(&i_this->hahen[i]);
        (btdDebris(piece)->model.v = (u32)(model));
        if (!model)
            btdGuest_JUTAssertion_getSDevice(gabi::guest_addr("d_a_btd.cpp"), 0x16A0,
                                             gabi::guest_addr("i_this->hahen[i].model"));
        f32 x = cM_rndF(0.5f);
        x = gabi::fadds_ppc(x, 1.0f);
        f32 y = cM_rndF(0.5f);
        y = gabi::fadds_ppc(y, 1.0f);
        f32 z = cM_rndF(0.5f);
        model = (u32)btdDebris(piece)->model.v;
        z = gabi::fadds_ppc(z, 1.0f);
        (btdModel(model)->mBaseScale.x = (f32)(x));
        (btdModel(model)->mBaseScale.y = (f32)(y));
        (btdModel(model)->mBaseScale.z = (f32)(z));
    }
    return TRUE;
}

/* 020F4308 */
s32 daBtd_Create(btd_class *i_this) {
    u32 p = gabi::ea(i_this), flags = (u32)i_this->actor_condition;
    if (!(flags & 8)) {
        if (p) {
            btd_class_ct(i_this);
            flags = (u32)i_this->actor_condition;
        }
        i_this->actor_condition = (u32)(flags | 8);
    }
    s32 status = btdGuest_dComIfG_resLoad(gabi::ea(&i_this->mPhaseBtd), gabi::guest_addr("Btd"));
    if (status != 4)
        return status;
    status = btdGuest_dComIfG_resLoad(gabi::ea(&i_this->mPhaseMagma), gabi::guest_addr("Magma"));
    if (status != 4)
        return status;
    (kui.v = (u32)(0));
    (dr2.v = (u32)(0));
    (i_this->mPa_SmokeEcallBack[0].m12 = (u8)(1));
    (i_this->mPa_SmokeEcallBack[1].m12 = (u8)(1));
    (i_this->mPa_SmokeEcallBack[2].m12 = (u8)(1));
    u32 save = dComIfGs_ea();
    s32 completed = btdGuest_dSv_memBit_c_isDungeonItem(save + 0x798, 3);
    if (completed && gabi::load<u8>(gabi::ea(&btdPlay()->mCurStage.mName[0])) != 0x58)
        return 5;
    if (!btdGuest_fopAcM_entrySolidHeap(p, gabi::guest_fn((const void *)&useHeapInit), 0x5B8C0))
        return 5;
    i_this->attention_info.flags = (u32)(4);
    s32 child = btdGuest_mDoHIO_subRoot_c_createChild(gabi::ea(&l_hioName), gabi::ea(&l_HIO));
    l_HIO.mNo = (s8)((u8)child);
    u32 stts = gabi::ea(&i_this->mStts);
    btdGuest_dCcD_Stts_Init(stts, 255, 255, p);
    for (u32 i = 0; i < 19; ++i) {
        u32 sph = gabi::ea(&i_this->mAtSphs[i]);
        btdGuest_dCcD_Sph_Set(sph, gabi::ea(&at_sph_src));
        btdSphere(sph)->mStts.v = stts;
    }
    (i_this->mEyeSph.mStts.v = (u32)(stts));
    btdGuest_dCcD_Sph_Set(gabi::ea(&i_this->mEyeSph), gabi::ea(&eye_sph_src));
    (i_this->mFireSph.mStts.v = (u32)(stts));
    btdGuest_dCcD_Sph_Set(gabi::ea(&i_this->mFireSph), gabi::ea(&fire_sph_src));
    for (u32 i = 0; i < 50; ++i) {
        u32 sph = gabi::ea(&i_this->mSibukiSphs[i]);
        btdGuest_dCcD_Sph_Set(sph, gabi::ea(&sibuki_sph_src));
        btdSphere(sph)->mStts.v = stts;
    }
    for (u32 i = 0; i < 6; ++i) {
        u32 cyl = gabi::ea(&i_this->mHandCyls[i]);
        btdGuest_dCcD_Cyl_Set(cyl, gabi::ea(&hand_cyl_src));
        btdCylinder(cyl)->mStts.v = stts;
    }
    f32 random = cM_rndF(50.0f);
    f32 duration = (f32)l_HIO.m14;
    i_this->m02EC[0] = (s16)((s16)gabi::ftoi(gabi::fadds_ppc(random, duration)));
    btdGuest_dKy_efplight_set(gabi::ea(&i_this->mLightInfluence));
    i_this->health = (s8)(10);
    i_this->max_health = (s8)(10);
    save = dComIfGs_ea();
    completed = btdGuest_dSv_memBit_c_isDungeonItem(save + 0x798, 5);
    bool active = completed != 0;
    if (!active)
        active = gabi::load<u8>(gabi::ea(&btdPlay()->mCurStage.mName[0])) == 0x58;
    if (active) {
        save = dComIfGs_ea();
        btdGuest_dSv_event_c_offEventBit(save + 0x1178, 0x480);
        i_this->m6E80 = (f32)(-50.0f);
        i_this->m6E84 = (f32)(0.0f);
        u8 stage = gabi::load<u8>(gabi::ea(&btdPlay()->mCurStage.mName[0]));
        btdGuest_bgmStart(stage == 0x58 ? 0x8000004Bu : 0x80000003u);
        random = cM_rndF(50.0f);
        flags = (u32)i_this->actor_status;
        i_this->m02EC[0] = (s16)((s16)gabi::ftoi(gabi::fadds_ppc(random, 170.0f)));
        i_this->actor_status = (u32)(flags | 0x4020);
        i_this->m6E90 = (s8)(1);
    } else {
        i_this->m02E4 = ACT_START_DEMO;
        i_this->m6E80 = (f32)(-50.0f);
        i_this->m6E84 = (f32)(-30.0f);
        s16 delay = (s16)l_debugRegisters.shorts[2];
        i_this->m02EC[0] = (s16)((s16)(delay + 150));
        f32 height = (f32)l_debugRegisters.floats[18];
        flags = (u32)i_this->actor_status;
        i_this->m6E90 = (s8)(1);
        i_this->actor_status = (u32)(flags | 0x4020);
        i_this->m6E7C = (f32)(gabi::fadds_ppc(height, 2300.0f));
    }
    u32 game = play();
    s32 magma = btdGuest_dComIfG_play_c_createMagma(game + 0x12A0);
    if (magma) {

        cXyz position;
        position.x = 0.0f;
        position.z = 0.0f;
        position.y = -300.0f;
        u32 packet = (u32)btdPlay()->mpMagmaPacket;
        s32 room = (s8)i_this->current.roomNo;
        u32 floor = btdGuest_dMagma_packet_c_newFloor(packet, &position, gabi::ea(&i_this->scale.x),
                                                      room, -250);
        i_this->floor.v = (u32)(floor);
        if (!floor)
            btdGuest_JUTAssertion_getSDevice(gabi::guest_addr("d_a_btd.cpp"), 0x189A,
                                             gabi::guest_addr("i_this->floor != (0)"));
    }
    btdGuest_fopAcM_create(0x18D, 0x10FF00, gabi::ea(&i_this->current.pos), -1, 0, 0, -1, 0);
    f32 time = (f32)l_HIO.m30;
    btdGuest_dKy_custom_timeset(time);
    return 4;
}

// HD: compiler-generated and template code of this translation unit.

/* 020F40A0 */
void *smokeEcallBack_ct(void *self) {
    return btdGuest_dPa_smokeEcallBack_dPa_smokeEcallBack(self, 1);
}

/* 020F4910 */
void SafeString_dt(void *self, s32 flags) {
    if (self && (flags & 1))
        btdGuest_op_delete(self);
}

/* 020F98E4 */
void smokeEcallBack_dt(void *self, s32 flags) {
    if (self && (flags & 1))
        btdGuest_op_delete(self);
}

/* 020F99F4 */
void SafeString_v14(void *self) {}

/* 020F9858 */
void *btdCylConstructor(void *self) {
    u32 p = gabi::ea(self);
    if (!p)
        p = btdGuest_allocate(0x130);
    if (p) {
        btdGuest_dCcD_GObjInf_dCcD_GObjInf(p);
        btdCylinderShape(p)->shapeVtable = gabi::ea(&l_shapeAttributeBaseVtable);
        btdCylinderShape(p)->aabVtable = gabi::ea(&l_cylinderAabVtable);
        btdGuest_cM3dGCyl_construct(gabi::ea(&btdCylinder(p)->mCyl));
        btdCylinder(p)->__vtbl_hitinf = gabi::ea(&l_hitInfoVtable);
        btdCylinder(p)->mCyl.__vtbl = gabi::ea(&l_cylinderVtable);
        btdCylinderShape(p)->shapeVtable = gabi::ea(&l_cylinderShapeVtable);
    }
    return gabi::at<void>(p);
}

/* 020F98F8 */
void btd_class_dt(btd_class *i_this, s32 flags) {
    u32 p = gabi::ea(i_this);
    if (!p)
        return;
    btdGuest_destroy_arr(gabi::ea(&i_this->mPa_SmokeEcallBack[0]), 3, 0x20,
                         gabi::guest_fn((const void *)&smokeEcallBack_dt), 0, 0);
    btdGuest_destroy_arr(gabi::ea(&i_this->mHandCyls[0]), 6, 0x130, 0x02515A70u, 0, 0);
    btdGuest_destroy_arr(gabi::ea(&i_this->mSibukiSphs[0]), 50, 0x12C, 0x02515AE8u, 0, 0);
    btdGuest_dCcD_Sph_destroy(gabi::ea(&i_this->mFireSph), 2);
    btdGuest_dCcD_Sph_destroy(gabi::ea(&i_this->mEyeSph), 2);
    btdGuest_destroy_arr(gabi::ea(&i_this->mAtSphs[0]), 19, 0x12C, 0x02515AE8u, 0, 0);
    btdGuest_dCcD_Stts_destroy(gabi::ea(&i_this->mStts), 2);
    btdGuest_fopAc_ac_c_dtor(p, 0);
    if (flags & 1)
        btdGuest_op_delete(p);
}

/* 020F4744 */
void *daBtd_HIO_ct(void *self) {
    u32 p = gabi::ea(self);
    if (!p)
        p = btdGuest_allocate(0x58);
    if (p) {
        daBtd_HIO_c *object = gabi::at<daBtd_HIO_c>(p);
        object->m1C = 0.5f;
        object->m30 = 4.0f;
        object->mNo = -1;
        object->m18 = 0.5f;
        object->m28 = 150.0f;
        object->m14 = 100.0f;
        object->m34 = 20.0f;
        object->m24 = 0.2f;
        object->m20 = 0.2f;
        object->m2C = 10;
        object->m05 = 0;
        object->m0C = 150;
        object->vtable = gabi::ea(&l_hioVtable);
        object->m54 = 0;
        object->m42 = 255;
        object->m07 = 0;
        object->m06 = 0;
        object->m10 = 0;
        object->m38 = 6.0f;
        object->m3C = 182;
        object->m48 = 12.0f;
        object->m4C = 35.0f;
        object->m3E = 156;
        object->m0E = 5;
        object->m46 = 36;
        object->m40 = 86;
        object->m50 = 1.0f;
        object->m08 = 1.0f;
        object->m44 = 142;
    }
    return gabi::at<void>(p);
}

/* 020F40A8 */
btd_class *btd_class_ct(btd_class *self) {
    u32 p = gabi::ea(self);
    if (!p)
        p = btdGuest_allocate(0x70D8);
    if (!p)
        return nullptr;
    btdGuest_fopAc_ac_c_fopAc_ac_c(p);
    btdActor(p)->__vtbl = (u32)(gabi::ea(&l_actorVtable));
    btdGuest_cCcD_Stts_construct(gabi::ea(&btdActor(p)->mStts));
    btdGuest_dCcD_GStts_dCcD_GStts(gabi::ea(&btdActor(p)->mStts.__vtbl_gstts));
    (btdActor(p)->mStts.__vtbl = (u32)(gabi::ea(&l_collisionStatusVtable)));
    (btdActor(p)->mStts.__vtbl_gstts = (u32)(gabi::ea(&l_gameCollisionStatusVtable)));
    btdGuest_construct_array(gabi::ea(&btdActor(p)->mAtSphs[0]), 19, 0x12C, 0x025166F0u);
    btdGuest_dCcD_Sph_construct(gabi::ea(&btdActor(p)->mEyeSph));
    btdGuest_dCcD_Sph_construct(gabi::ea(&btdActor(p)->mFireSph));
    btdGuest_construct_array(gabi::ea(&btdActor(p)->mSibukiSphs[0]), 50, 0x12C, 0x025166F0u);
    btdGuest_construct_array(gabi::ea(&btdActor(p)->mHandCyls[0]), 6, 0x130,
                             gabi::guest_fn((const void *)&btdCylConstructor));
    btdGuest_construct_array(gabi::ea(&btdActor(p)->mPa_SmokeEcallBack[0]), 3, 0x20,
                             gabi::guest_fn((const void *)&smokeEcallBack_ct));
    (btdActor(p)->mLightInfluence.mHD20 = (f32)(1.0f));

    f32 f[14];
    u8 b[4];
    s16 h[4];
    f[0] = lightDefaults.position.x;
    btdLighting(p)->light.position.x = f[0];
    f[1] = lightDefaults.position.y;
    btdLighting(p)->light.position.y = f[1];
    f[2] = lightDefaults.position.z;
    btdLighting(p)->light.position.z = f[2];
    f[3] = lightDefaults.direction.x;
    btdLighting(p)->light.direction.x = f[3];
    f[4] = lightDefaults.direction.y;
    btdLighting(p)->light.direction.y = f[4];
    f[5] = lightDefaults.direction.z;
    btdLighting(p)->light.direction.z = f[5];
    b[0] = lightDefaults.color.r;
    btdLighting(p)->light.color.r = b[0];
    b[1] = lightDefaults.color.g;
    btdLighting(p)->light.color.g = b[1];
    b[2] = lightDefaults.color.b;
    btdLighting(p)->light.color.b = b[2];
    b[3] = lightDefaults.color.a;
    btdLighting(p)->light.color.a = b[3];
    h[0] = lightDefaults.ambientColor.r;
    btdLighting(p)->light.ambientColor.r = h[0];
    h[1] = lightDefaults.ambientColor.g;
    btdLighting(p)->light.ambientColor.g = h[1];
    h[2] = lightDefaults.ambientColor.b;
    btdLighting(p)->light.ambientColor.b = h[2];
    h[3] = lightDefaults.ambientColor.a;
    btdLighting(p)->light.ambientColor.a = h[3];
    f[6] = lightDefaults.k0Intensity;
    btdLighting(p)->light.k0Intensity = f[6];
    f[7] = lightDefaults.c0Intensity;
    btdLighting(p)->light.c0Intensity = f[7];
    f[8] = lightDefaults.cosineAttenuation[0];
    btdLighting(p)->light.cosineAttenuation[0] = f[8];
    f[9] = lightDefaults.cosineAttenuation[1];
    btdLighting(p)->light.cosineAttenuation[1] = f[9];
    f[10] = lightDefaults.cosineAttenuation[2];
    btdLighting(p)->light.cosineAttenuation[2] = f[10];
    f[11] = lightDefaults.distanceAttenuation[0];
    btdLighting(p)->light.distanceAttenuation[0] = f[11];
    f[12] = lightDefaults.distanceAttenuation[1];
    btdLighting(p)->light.distanceAttenuation[1] = f[12];
    f[13] = lightDefaults.distanceAttenuation[2];
    (btdLighting(p)->secondMaterialLight.ambientColor.b = (s16)(h[2]));
    (btdLighting(p)->light.distanceAttenuation[2] = (f32)(f[13]));
    (btdLighting(p)->materialLight.color.b = (u8)(b[2]));
    (btdLighting(p)->materialLight.distanceAttenuation[2] = (f32)(f[13]));
    (btdLighting(p)->materialLight.direction.y = (f32)(f[4]));
    (btdLighting(p)->materialLight.direction.x = (f32)(f[3]));
    (btdLighting(p)->secondMaterialLight.color.r = (u8)(b[0]));
    (btdLighting(p)->materialLight.direction.z = (f32)(f[5]));
    (btdLighting(p)->materialLight.cosineAttenuation[0] = (f32)(f[8]));
    (btdLighting(p)->secondMaterialLight.direction.x = (f32)(f[3]));
    (btdLighting(p)->secondMaterialLight.color.g = (u8)(b[1]));
    (btdLighting(p)->secondMaterialLight.ambientColor.r = (s16)(h[0]));
    (btdLighting(p)->materialLight.ambientColor.r = (s16)(h[0]));
    (btdLighting(p)->secondMaterialLight.k0Intensity = (f32)(f[6]));
    (btdLighting(p)->materialLight.cosineAttenuation[2] = (f32)(f[10]));
    (btdLighting(p)->materialLight.ambientColor.a = (s16)(h[3]));
    (btdLighting(p)->secondMaterialLight.c0Intensity = (f32)(f[7]));
    (btdLighting(p)->materialLight.color.a = (u8)(b[3]));
    (btdLighting(p)->materialLight.k0Intensity = (f32)(f[6]));
    (btdLighting(p)->materialLight.color.g = (u8)(b[1]));
    (btdLighting(p)->secondMaterialLight.position.z = (f32)(f[2]));
    (btdLighting(p)->materialLight.position.y = (f32)(f[1]));
    (btdLighting(p)->materialLight.distanceAttenuation[1] = (f32)(f[12]));
    (btdLighting(p)->secondMaterialLight.color.a = (u8)(b[3]));
    (btdLighting(p)->materialLight.position.z = (f32)(f[2]));
    (btdLighting(p)->secondMaterialLight.direction.y = (f32)(f[4]));
    (btdLighting(p)->materialLight.ambientColor.b = (s16)(h[2]));
    (btdLighting(p)->materialLight.color.r = (u8)(b[0]));
    (btdLighting(p)->secondMaterialLight.position.x = (f32)(f[0]));
    (btdLighting(p)->secondMaterialLight.ambientColor.a = (s16)(h[3]));
    (btdLighting(p)->materialLight.position.x = (f32)(f[0]));
    (btdLighting(p)->secondMaterialLight.color.b = (u8)(b[2]));
    (btdLighting(p)->secondMaterialLight.cosineAttenuation[0] = (f32)(f[8]));
    (btdLighting(p)->materialLight.ambientColor.g = (s16)(h[1]));
    (btdLighting(p)->secondMaterialLight.ambientColor.g = (s16)(h[1]));
    (btdLighting(p)->secondMaterialLight.direction.z = (f32)(f[5]));
    (btdLighting(p)->secondMaterialLight.position.y = (f32)(f[1]));
    (btdLighting(p)->materialLight.c0Intensity = (f32)(f[7]));
    (btdLighting(p)->materialLight.distanceAttenuation[0] = (f32)(f[11]));
    (btdLighting(p)->materialLight.cosineAttenuation[1] = (f32)(f[9]));
    (btdLighting(p)->secondMaterialLight.cosineAttenuation[1] = (f32)(f[9]));
    (btdLighting(p)->secondMaterialLight.cosineAttenuation[2] = (f32)(f[10]));
    (btdLighting(p)->secondMaterialLight.distanceAttenuation[0] = (f32)(f[11]));
    (btdLighting(p)->secondMaterialLight.distanceAttenuation[1] = (f32)(f[12]));
    (btdLighting(p)->secondMaterialLight.distanceAttenuation[2] = (f32)(f[13]));
    return gabi::at<btd_class>(p);
}

/* 020F4870 */
void __sinit_d_a_btd_cpp() {
    (l_constructorWords.values[2] = (u32)(0));
    (l_constructorWords.values[0] = (u32)(0));
    (l_constructorWords.values[3] = (u32)(0));
    (l_constructorWords.values[1] = (u32)(0));
    btdGuest_register_global_object(gabi::ea(&l_centerRegistration));
    (l_constructorFloat1 = (f32)((f32)l_negativePi));
    (l_constructorFloat2 = (f32)((f32)l_positivePi));
    btdGuest_zeroByte_construct(gabi::ea(&l_stateByte1));
    btdGuest_register_global_object(gabi::ea(&l_firstByteRegistration));
    btdGuest_oneByte_construct(gabi::ea(&l_stateByte2));
    btdGuest_register_global_object(gabi::ea(&l_secondByteRegistration));
    daBtd_HIO_ct(&l_HIO);
}

// clang-format off
/* WWHD glue
data l_safeStringVtable 1000B9F4
data l_shapeAttributeBaseVtable 100015A8
data l_cylinderAabVtable 1000BA0C
data l_hitInfoVtable 1004B108
data l_cylinderVtable 1004B150
data l_cylinderShapeVtable 1004B160
data l_hioVtable 1000BA1C
data l_actorVtable 1000BA2C
data l_collisionStatusVtable 1004AE88
data l_gameCollisionStatusVtable 1004AEC0
data l_centerRegistration 10192ECC
data l_firstByteRegistration 10192ED8
data l_secondByteRegistration 10192EE4
data l_hioName 1000BD18
data l_effectColor 101D5E98
data at_sph_src 10192D88
data eye_sph_src 10192DC8
data fire_sph_src 10192E08
data sibuki_sph_src 10192E48
data hand_cyl_src 10192E88
str 1000B9E4 in 020F6004
str 1000B9E8 in 020F52B8
str 1000B9EC
str 1000BA3C
str 1000BA50
str 1000BA6C in 020F03BC
str 1000BA9C in 020F04EC
str 1000BAA8
str 1000BC3C
str 1000BC44 in 020F34A8
str 1000BC48 in 020F34A8
str 1000BC50 in 020F3594
str 1000BC54 in 020F3594
str 1000BC60
str 1000BC6C
str 1000BC78
str 1000BC88
str 1000BC98
str 1000BCAC
str 1000BCC0
str 1000BCDC
str 1000BD00 in 020F4308
str 1000BD04 in 020F4308
str 1000BD0C in 020F4308
str 1000BD24
data l_negativePi 1000BD48
data l_positivePi 1000BD4C
data l_endPositionInitialized 104629CC
data l_endPosition 104629F0
data l_ykgrEmitter 10475628
data l_ykgrAlphaFlag 10475652
data kui 104629BC
data dr2 104629C0
data l_constructorFloat1 104629C4
data l_constructorFloat2 104629C8
data l_stateByte1 104629D0
data l_stateByte2 104629D1
data l_constructorWords 104629D4
data static_center_pos 104629E4
data l_blurEnabled 101F4825
data l_blurAlpha 101F4826
data l_matrixStack 1048D0CC
data lightDefaults 1016E414
ext 025167E4 void btdGuest_moveFireAttack(u32 sphere, cXyz* position)
ext 025167C0 void btdGuest_startFireAttack(u32 sphere, cXyz* position)
ext 025E8154 s32 btdGuest_brkInit(u32 handle, u32 data, u32 animation, s32 enable, s32 loop, f32 speed, s32 start, s32 end, s32 second)
ext 025E7CE0 s32 btdGuest_btkInit(u32 handle, u32 data, u32 animation, s32 enable, s32 loop, f32 speed, s32 start, s32 end, s32 second)
ext 025E80D0 u32 btdGuest_brkConstruct(u32 handle)
ext 025E7C6C u32 btdGuest_btkConstruct(u32 handle)
data l_debugRegisters 1047B608
data jab_bck 10192B08
data punch_bck 10192B28
data hibi_eff 10192B00
data hibi_brk 10192BB8
data hahen_effname 10192B70
data hahen_effindex 10192B88
data eff_index 10192BC4
data last_effname 10192BD8
data eff_name 10192C00
data effF_name 10192C0C
data effR_name 10192C1C
data joint_check 10192C2C
data last_eff_index 10192C78
data at_size 10192CC4
data handposX 10192D10
data handposY 10192D28
data handposZ 10192D40
data handposH 10192D58
data handposR 10192D70
ext 0273AD10 u32 btdGuest_allocate(s32 a0)
ext 025E18EC void btdGuest_bgmStart(u32 a0)
ext 025E1904 void btdGuest_bgmStop(s32 a0)
ext 025E8154 void btdGuest_brkInitNative(u32 a0, u32 a1, u32 a2, s32 a3, f32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9)
ext 025E8154 void btdGuest_brkInitNative(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4, f32 a5, s32 a6, s32 a7, s32 a8, s32 a9)
ext 025E7CE0 void btdGuest_btkInitNative(u32 a0, u32 a1, u32 a2, s32 a3, f32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9)
ext 0200BD2C void btdGuest_cCcD_Stts_construct(u32 a0)
ext 02018590 void btdGuest_cM3dGCyl_construct(u32 a0)
ext 025E1988 void btdGuest_changeBgmStatus(s32 a0)
ext 025166F0 void btdGuest_dCcD_Sph_construct(u32 a0)
ext 02515AE8 void btdGuest_dCcD_Sph_destroy(u32 a0, s32 a1)
ext 02515860 void btdGuest_dCcD_Stts_destroy(u32 a0, s32 a1)
ext 020F038C u32 btdGuest_getBrkNative(u32 a0)
ext 020F035C u32 btdGuest_getBtkNative(u32 a0)
ext 025200D4 u32 btdGuest_getPlay()
ext 025E2DA8 void btdGuest_modelUpdateDL(u32 a0)
ext 027F7878 u32 btdGuest_nodeToJoint(void* a0)
ext 028EAB2C void btdGuest_oneByte_construct(u32 a0)
ext 027EC9E8 void btdGuest_screenReport(s32 a0, s32 a1, u32 a2, s32 a3)
ext 025E1AA4 void btdGuest_seStartMonster(u32 a0, u32 a1, s32 a2, s32 a3, s32 a4)
ext 025E1AA4 void btdGuest_seStartMonster(u32 a0, u32 a1, u32 a2, s32 a3, s32 a4)
ext 02562F5C void btdGuest_setLightTevColorType(u32 a0, u32 a1, u32 a2)
ext 028F4384 f32 btdGuest_sqrtf(f32 a0)
ext 025E1934 void btdGuest_subBgmStart(u32 a0)
ext 025E1944 void btdGuest_subBgmStop()
ext 028ED6F8 void btdGuest_zeroByte_construct(u32 a0)
data l_HIO 104629FC
ext 025E1B34 void btdGuest_JAIZelBasic_seDeleteObject(u32 a0)
ext 028249B0 void btdGuest_JPASetRMtxTVecfromMtx(u32 a0, u32 a1, u32 a2)
ext 0273AA24 void btdGuest_JUTAssertion_getSDevice(u32 a0, s32 a1, u32 a2)
ext 0273AA24 void btdGuest_JUTAssertion_getSDevice(u32 a0, u32 a1, u32 a2)
ext 020F03BC void btdGuest_anm_init(u32 a0, s32 a1, f32 a2, s32 a3, f32 a4, s32 a5)
ext 020F03BC void btdGuest_anm_init(u32 a0, s32 a1, f32 a2, u8 a3, f32 a4, s32 a5)
ext 020F90C4 void btdGuest_btd_effect(u32 a0)
ext 020F94DC void btdGuest_btd_kankyo(u32 a0)
ext 0200E240 void btdGuest_cCcS_Set(u32 a0, u32 a1)
ext 0200EDC8 void btdGuest_cLib_addCalc0(u32 a0, f32 a1, f32 a2)
ext 0200ED84 void btdGuest_cLib_addCalc2(u32 a0, f32 a1, f32 a2, f32 a3)
ext 0200F428 void btdGuest_cLib_addCalcAngleS2(u32 a0, s16 a1, s32 a2, s16 a3)
ext 0200F428 void btdGuest_cLib_addCalcAngleS2(u32 a0, s32 a1, s32 a2, s16 a3)
ext 0200F428 void btdGuest_cLib_addCalcAngleS2(u32 a0, s32 a1, s32 a2, s32 a3)
ext 0200FE78 void btdGuest_cLs_Addition(u32 a0, u32 a1)
ext 020182E0 void btdGuest_cM3dGCyl_SetC(u32 a0, u32 a1)
ext 020182E0 void btdGuest_cM3dGCyl_SetC(u32 a0, void* a1)
ext 02018428 void btdGuest_cM3dGCyl_SetH(u32 a0, f32 a1)
ext 020184DC void btdGuest_cM3dGCyl_SetR(u32 a0, f32 a1)
ext 02018D40 void btdGuest_cM3dGSph_SetC(u32 a0, u32 a1)
ext 02018D40 void btdGuest_cM3dGSph_SetC(u32 a0, void* a1)
ext 02018C8C void btdGuest_cM3dGSph_SetR(u32 a0, f32 a1)
ext 020195B0 s32 btdGuest_cM_atan2sFullResult(f32 a0, f32 a1)
ext 0201B31C void btdGuest_cXyz_normalize(void* a0, void* a1)
ext 0201ADE0 void btdGuest_cXyz_op_sub(u32 a0, void* a1, u32 a2)
ext 0201ADE0 void btdGuest_cXyz_op_sub(u32 a0, void* a1, void* a2)
ext 028EFFD0 void btdGuest_construct_array(u32 a0, s32 a1, s32 a2, u32 a3)
ext 024F8044 u32 btdGuest_dCam_getBody()
ext 0251510C void btdGuest_dCamera_c_Reset_2(u32 a0, void* a1, void* a2)
ext 02515280 void btdGuest_dCamera_c_SetTrimSize(u32 a0, s32 a1)
ext 02514EE4 void btdGuest_dCamera_c_SetTypeForce_2(u32 a0, u32 a1, s32 a2)
ext 02514FE8 void btdGuest_dCamera_c_Set_3(u32 a0, void* a1, void* a2, s16 a3, f32 a4)
ext 02514F38 void btdGuest_dCamera_c_Start(u32 a0)
ext 02514F2C void btdGuest_dCamera_c_Stop(u32 a0)
ext 02516518 void btdGuest_dCcD_Cyl_Set(u32 a0, u32 a1)
ext 025160DC s32 btdGuest_dCcD_GObjInf_ChkAtHit(u32 a0)
ext 02515FB8 void btdGuest_dCcD_GObjInf_dCcD_GObjInf(u32 a0)
ext 02515DA0 void btdGuest_dCcD_GStts_dCcD_GStts(u32 a0)
ext 025167E4 void btdGuest_dCcD_Sph_MoveCAt(u32 a0, u32 a1)
ext 0251677C void btdGuest_dCcD_Sph_Set(u32 a0, u32 a1)
ext 025167C0 void btdGuest_dCcD_Sph_StartCAt(u32 a0, u32 a1)
ext 02515F14 void btdGuest_dCcD_Stts_Init(u32 a0, s32 a1, s32 a2, u32 a3)
ext 02524C10 s32 btdGuest_dComIfG_play_c_createMagma(u32 a0)
ext 025204C8 void btdGuest_dComIfG_resDelete(u32 a0, u32 a1)
ext 02520460 s32 btdGuest_dComIfG_resLoad(u32 a0, u32 a1)
ext 02520540 s32 btdGuest_dComIfGp_getReverb(s32 a0)
ext 0255FDA4 void btdGuest_dKy_custom_colset(u8 a0, u8 a1, f32 a2)
ext 0255FE50 void btdGuest_dKy_custom_timeset(f32 a0)
ext 0255BA9C void btdGuest_dKy_efplight_cut(u32 a0)
ext 0255B9C8 void btdGuest_dKy_efplight_set(u32 a0)
ext 02587EFC void btdGuest_dLib_setNextStageBySclsNum(s32 a0, s32 a1)
ext 0258CC64 u32 btdGuest_dMagma_packet_c_newFloor(u32 a0, void* a1, u32 a2, s32 a3, s32 a4)
ext 025A3BDC u32 btdGuest_dPa_J3DmodelEmitter_c_dPa_J3DmodelEmitter_c(s32 a0, u32 a1, u32 a2, s32 a3, u32 a4, u32 a5, s32 a6, s32 a7)
ext 025A847C u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, s32 a2, u32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, s32 a2, u32 a3, u32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, u16 a2, u32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, u32 a2, u32 a3, s32 a4, s32 a5, s32 a6, u32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C u32 btdGuest_dPa_control_c_set(u32 a0, s32 a1, u32 a2, u32 a3, u32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, u32 a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, void* a3, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, void* a3, u32 a4, s32 a5, s32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A847C void btdGuest_dPa_control_c_setDiscardResult(u32 a0, s32 a1, s32 a2, void* a3, void* a4, s32 a5, s32 a6, u32 a7, s32 a8, s32 a9, s32 a10, s32 a11)
ext 025A8D40 void btdGuest_dPa_control_c_setSimple(u32 a0, u32 a1, u32 a2, s32 a3, u32 a4, u32 a5, s32 a6)
ext 025A5B18 void* btdGuest_dPa_smokeEcallBack_dPa_smokeEcallBack(void* a0, s32 a1)
ext 026066C4 u32 btdGuest_dRes_control_c_getRes(u32 a0, void* a1, s32 a2)
ext 02555D0C u32 btdGuest_getEnvironmentLight()
ext 025626A4 void btdGuest_dScnKy_env_light_c_settingTevStruct(u32 a0, s32 a1, u32 a2, u32 a3)
ext 025BEBB8 void btdGuest_dSnap_RegistFig(s32 a0, u32 a1, u32 a2, s16 a3, f32 a4, f32 a5, f32 a6)
ext 025B8B7C void btdGuest_dSv_event_c_offEventBit(u32 a0, s32 a1)
ext 025B8B68 void btdGuest_dSv_event_c_onEventBit(u32 a0, s32 a1)
ext 025B9100 s32 btdGuest_dSv_memBit_c_isDungeonItem(u32 a0, s32 a1)
ext 025B9098 void btdGuest_dSv_memBit_c_onDungeonItem(u32 a0, s32 a1)
ext 025CB408 void btdGuest_dVibration_c_StartQuake(u32 a0, s32 a1, s32 a2, void* a3)
ext 025CB374 void btdGuest_dVibration_c_StartShock(u32 a0, s32 a1, s32 a2, void* a3)
ext 025CB610 void btdGuest_dVibration_c_StopQuake(u32 a0, s32 a1)
ext 020F6004 void btdGuest_move(u32 a0)
ext 020F4D80 void btdGuest_damage_check(u32 a0)
ext 028F0164 void btdGuest_destroy_arr(u32 a0, s32 a1, s32 a2, u32 a3, s32 a4, s32 a5)
ext 020F52B8 void btdGuest_end(u32 a0)
ext 025D5834 void btdGuest_fopAcM_create(s32 a0, s32 a1, u32 a2, s32 a3, s32 a4, s32 a5, s32 a6, s32 a7)
ext 025D99E8 void btdGuest_fopAcM_createDisappear(u32 a0, u32 a1, s32 a2, s32 a3, s32 a4)
ext 025D9874 void btdGuest_fopAcM_createWarpFlower(u32 a0, s32 a1, s32 a2, s32 a3)
ext 025D63E8 s32 btdGuest_fopAcM_entrySolidHeap(u32 a0, u32 a1, s32 a2)
ext 025D7B24 void btdGuest_fopAcM_orderPotentialEvent(u32 a0, s32 a1, s32 a2, s32 a3)
ext 025D6894 s16 btdGuest_fopAcM_searchActorAngleY(u32 a0, u32 a1)
ext 025D6894 s32 btdGuest_fopAcM_searchActorAngleYFullResult(u32 a0, u32 a1)
ext 025D4604 s32 btdGuest_fopAc_IsActor(void* a0)
ext 025D50BC void btdGuest_fopAc_ac_c_dtor(u32 a0, s32 a1)
ext 025D4ED0 void btdGuest_fopAc_ac_c_fopAc_ac_c(u32 a0)
ext 025DE508 u32 btdGuest_fpcEx_Search(u32 a0, u32 a1)
ext 020F032C u32 btdGuest_get_anm(u32 a0)
ext 020F0C68 void btdGuest_hahen_set2(u32 a0)
ext 020F0EB4 void btdGuest_hahen_set(u32 a0, s32 a1)
ext 020F1288 void btdGuest_kubi_calc(u32 a0)
ext 025E38E0 u32 btdGuest_mDoExt_J3DModel_create(u32 a0, s32 a1, u32 a2)
ext 025E4F64 u32 btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(s32 a0, u32 a1, s32 a2, s32 a3, s32 a4, s32 a5, f32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, u32 a12)
ext 025E4F64 u32 btdGuest_mDoExt_McaMorf_mDoExt_McaMorf(s32 a0, u32 a1, s32 a2, s32 a3, u32 a4, s32 a5, f32 a6, s32 a7, s32 a8, s32 a9, s32 a10, s32 a11, u32 a12)
ext 025E4A98 void btdGuest_mDoExt_McaMorf_setAnm(void* a0, u32 a1, u8 a2, f32 a3, f32 a4, f32 a5, f32 a6, u32 a7)
ext 025F064C void btdGuest_mDoGph_gInf_c_onBlure()
ext 025F0A10 s32 btdGuest_mDoHIO_subRoot_c_createChild(u32 a0, u32 a1)
ext 025F0A18 void btdGuest_mDoHIO_subRoot_c_deleteChild(s32 a0)
ext 025F1BF4 void btdGuest_mDoMtx_XrotM(u32 a0, s16 a1)
ext 025F1C28 void btdGuest_mDoMtx_YrotM(u32 a0, s16 a1)
ext 025F1884 void btdGuest_mDoMtx_YrotS(u32 a0, s16 a1)
ext 025F1884 void btdGuest_mDoMtx_YrotS(u32 a0, s32 a1)
ext 025F1C5C void btdGuest_mDoMtx_ZrotM(u32 a0, s16 a1)
ext 0273AF40 void btdGuest_op_delete(u32 a0)
ext 0273AF40 void btdGuest_op_delete(void* a0)
ext 028F026C void btdGuest_register_global_object(u32 a0)
ext 020F10B4 void btdGuest_sibuki_set(u32 a0)
ext 020F0DFC void btdGuest_smoke_set_s(u32 a0, u32 a1, u32 a2)
ext 020F4924 void btdGuest_startdemo(u32 a0)
ext 020F02D4 void btdGuest_wave_set(u32 a0)
*/
// clang-format on
