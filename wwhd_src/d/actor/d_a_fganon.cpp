// Phantom Ganon, reconstructed locally from WWHD.
#include "d/actor/d_a_fganon.h"
// HD: checked collision source records and matrix-stack addresses.
static const u32 l_weponSphSrc[] = {
    0x00000000, 0x00000008, 0x00000000, 0x0000000F, 0x00000000, 0x00000000, 0x00000000, 0x000D0700,
    0x00000001, 0x00000000, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x43480000};
static const u32 l_ballTgSphSrc[] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF1DFEFF, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x42700000};
static const u32 l_ballAtSphSrc[] = {
    0x00000000, 0x00000800, 0x02000000, 0x0000000F, 0x00000000, 0x00000000, 0x00000000, 0x00000A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41F00000};
static const u32 l_cylSrc[] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFF1DFEF7,
                               0x00000003, 0x00000075, 0x00000000, 0x00000000, 0x000D0000,
                               0x00000002, 0x00000000, 0x00000000, 0xC3160000, 0x00000000,
                               0x42C80000, 0x42C80000};
static constexpr u32 l_mtxStack = 0x1048D0CC;
static constexpr u32 l_currentMtx = 0x104B4868;
static constexpr u32 l_judgeById = 0x025E1234;
static constexpr u32 l_searchMagicSound = 0x02134A90;
static constexpr u32 l_useHeapInit = 0x02137954;
static constexpr u32 l_angleStatic = 0x10463D3C;
static constexpr u32 l_globeStatic = 0x10463D3D;
static constexpr u32 l_zeroRegistry = 0x101B4DCC;
static constexpr u32 l_angleRegistry = 0x101B4DD8;
static constexpr u32 l_globeRegistry = 0x101B4DE4;

// HD: this unit owns the resource-name SafeString vtable.
const gc_tu_data gc_tu = {.safeString = 0x1000EBFC};
// HD: distinct archive literals retain their original guest addresses.
static const u8 l_archiveAnimation[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_archiveAppear[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_archiveDisappear[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_archiveSwordDisappear[] = {80, 103, 115, 119, 0};
static const u8 l_archiveEnd[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_archiveDemo[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_archiveSwordDelete[] = {80, 103, 115, 119, 0};
static const u8 l_archiveDelete[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_archiveHeap[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_archiveSwordHeap[] = {80, 103, 115, 119, 0};
static const u8 l_archiveSwordCreate[] = {80, 103, 115, 119, 0};
static const u8 l_archiveCreate[] = {70, 103, 97, 110, 111, 110, 0};
static const u8 l_hioLabel1[] = {131, 116, 131, 64,  131, 147, 131, 103, 131, 128,
                                 131, 75,  131, 109, 131, 147, 135, 66,  0};
static const u8 l_hioLabel2[] = {131, 116, 131, 64,  131, 147, 131, 103, 131, 128,
                                 131, 75,  131, 109, 131, 147, 135, 65,  0};
static const u8 l_hioLabel3[] = {131, 116, 131, 64,  131, 147, 131, 103, 131, 128,
                                 131, 75,  131, 109, 131, 147, 135, 64,  0};
static const u8 l_cameraType3[] = {80, 95, 71, 97, 110, 111, 110, 51, 0};
static const u8 l_cameraType1[] = {80, 95, 71, 97, 110, 111, 110, 49, 0};
static const u8 l_cameraType2[] = {80, 95, 71, 97, 110, 111, 110, 50, 0};
static const u8 l_subCountFormat[] = {75, 32, 83, 85, 66, 32, 32,  67, 79,
                                      85, 78, 84, 32, 32, 37, 100, 0};

static u32 morfModel(fganon_class *actor) {
    return actor->mpMorf->mpModel.v;
}

static void sound(fganon_class *actor, u32 id, u32 volume = 0) {
    u32 address = gabi::ea(actor);
    // HD: preserve the guest-address wrap check used by the sound path.
    if (address && (u32)(address + offsetof(fganon_class, eyePos))) {
        s32 reverb = fg_dComIfGp_getReverb((s8)actor->current.roomNo);
        fg_mDoAud_seStart(id, gabi::ea(&actor->eyePos), volume, reverb);
    }
}
static s16 l_swordFallOffset;
static s8 l_stayNo;
static u32 jno[2] = {7, 13};
static u16 eno[2] = {0x81CD, 0x81CC};
static FganonLightDefaults l_lightDefaults;
static FganonHeaderZeros l_headerZeros;
static FganonHeaderPi l_headerPi;

const dBgS_GndChk_vt l_groundCheckVtables = {0x1000ECC4, 0x1000ECD4, 0x1000ECF4, 0x1000ECE4};
const dBgS_LinChk_vt l_lineCheckVtables = {0x1000ED34, 0x1000ED44, 0x1000ED64, 0x1000ED54};
static daFganon_HIO_c l_HIO;
static u32 l_calcMtx;
static u32 l_currentModel;
static u32 l_saveInfo;
static u32 master;
static u8 hio_set;
static u8 l_pauseTimer;
static FganonDebugRegisters l_reg0;
static FganonDebugRegisters l_reg8;
static FganonDebugRegisters l_reg12;
static FganonDebugRegisters l_reg18;

void pos_move(fganon_class *, u8);
void fly_se_set(fganon_class *);

/* 02134108 */
void anm_init(fganon_class *actor, s32 index, f32 blend, u8 loop, f32 speed, s32 soundIndex) {
    void *animation = dComIfG_getObjectRes(fgView<const char>(gabi::ea(l_archiveAnimation)), index,
                                           gc_tu.safeString);
    void *sound = nullptr;
    if (soundIndex >= 0)
        sound = dComIfG_getObjectRes(fgView<const char>(gabi::ea(l_archiveAnimation)), soundIndex,
                                     gc_tu.safeString);
    fg_mDoExt_McaMorf_setAnm((u32)actor->mpMorf.v, gabi::ea(animation), loop, blend, speed, 0.0f,
                             -1.0f, gabi::ea(sound));
}

static u32 jointMatrix(u32 model, u32 index) {
    u32 buffer = (u32)fgView<J3DModel>(model)->mpMtxBuffer.v;
    u16 flags = (u16)fgView<J3DMtxBuffer_hd>(buffer)->mFlags;
    u32 matrices = (u32)fgView<J3DMtxBuffer_hd>(buffer)->mpAnmMtx;
    fgView<J3DMtxBuffer_hd>(buffer)->mFlags = (u16)(flags | 0x10);
    return matrices + index * 0x30;
}
static void copyJointMatrix(u32 model, u32 index) {
    u32 buffer = (u32)fgView<J3DModel>(model)->mpMtxBuffer.v;
    u16 flags = (u16)fgView<J3DMtxBuffer_hd>(buffer)->mFlags;
    u32 source = (u32)l_calcMtx;
    fgView<J3DMtxBuffer_hd>(buffer)->mFlags = (u16)(flags | 0x10);
    u32 matrices = (u32)fgView<J3DMtxBuffer_hd>(buffer)->mpAnmMtx;
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = (f32)fgMatrixFloat(source, i);
    for (u32 i = 0; i < 12; ++i)
        fgMatrixFloat(matrices + index * 0x30, i) = values[i];
}
/* 02134230 */
BOOL nodeCallback(void *node, s32 timing) {
    if (timing == 0) {
        void *joint = fgView<void>(fg_J3DJoint_getJointInfo(gabi::ea(node)));
        u32 model = (u32)l_currentModel;
        u32 actor = (u32)fgView<J3DModel>(model)->mUserArea;
        u32 index = (u16)fgView<FganonJointInfo>(gabi::ea(joint))->mJointNo;
        if (actor) {
            u32 matrix = jointMatrix(model, index);
            fg_PSMTXCopy(matrix, (u32)l_calcMtx);
            fg_mDoMtx_YrotM((u32)l_calcMtx, (s16)fgView<fganon_class>(actor)->mHdNodeY);
            fg_mDoMtx_ZrotM((u32)l_calcMtx, (s16)fgView<fganon_class>(actor)->mHdNodeZ);
            copyJointMatrix(model, index);
            fg_PSMTXCopy((u32)l_calcMtx, l_currentMtx);
            if (index == (u32)(s32)(s16)l_reg0.s[0]) {
                matrix = jointMatrix(model, index);
                fg_PSMTXCopy(matrix, (u32)l_calcMtx);
                fg_mDoMtx_YrotM((u32)l_calcMtx, (s16)l_reg0.s[1]);
                fg_mDoMtx_XrotM((u32)l_calcMtx, (s16)l_reg0.s[2]);
                fg_mDoMtx_ZrotM((u32)l_calcMtx, (s16)l_reg0.s[3]);
                copyJointMatrix(model, index);
                fg_PSMTXCopy((u32)l_calcMtx, l_currentMtx);
            }
        }
    }
    return TRUE;
}

static void brkEntry(u32 animation, u32 model, bool texture) {
    u32 data = (u32)fgView<J3DModel>(model)->mModelData.v;
    f32 frame = (f32)fgView<mDoExt_baseAnm>(animation)->mFrameCtrl.mFrame;
    if (texture)
        fg_mDoExt_btkAnm_entry(animation, data, frame);
    else
        fg_mDoExt_brkAnm_entry(animation, data, frame);
}
/* 02134428 */
BOOL daFganon_Draw(fganon_class *actor) {
    fg_dSnap_RegistFig(0xC2, gabi::ea(actor), 1.0f, 1.0f, 1.0f);
    u32 morf = (u32)actor->mpMorf.v;
    s8 visible = (s8)actor->mbIsMaterialized;
    u32 model = (u32)fgView<mDoExt_McaMorf>(morf)->mpModel.v;
    u32 flags = (u32)fgView<FganonModelFlags>(model)->mFlags;
    flags = visible ? flags | 1 : flags & ~1u;
    fgView<FganonModelFlags>(model)->mFlags = (u32)(flags);
    fg_J3DModel_setFlags(model, flags);
    u32 sword = (u32)actor->mpKenModel.v;
    flags = (u32)fgView<FganonModelFlags>(sword)->mFlags;
    flags = visible ? flags | 1 : flags & ~1u;
    fgView<FganonModelFlags>(sword)->mFlags = (u32)(flags);
    fg_J3DModel_setFlags(sword, flags);
    u32 env = (unsigned int)fg_dComIfGp_getEnvLight();
    fg_setLightTevColorType(env, model, gabi::ea(&actor->tevStr));
    brkEntry((u32)actor->mpBrkAnm1.v, model, false);
    fg_mDoExt_McaMorf_entryDL((u32)actor->mpMorf.v);
    if ((s8)actor->m2D0 != 2) {
        model = (u32)actor->mpKenModel.v;
        env = (unsigned int)fg_dComIfGp_getEnvLight();
        fg_dScnKy_env_light_c_settingTevStruct(env, 0, gabi::ea(&actor->current.pos),
                                               gabi::ea(&actor->mKenTevStr));
        env = (unsigned int)fg_dComIfGp_getEnvLight();
        fg_setLightTevColorType(env, model, gabi::ea(&actor->mKenTevStr));
        brkEntry((u32)actor->mpBrkAnm2.v, model, false);
        fg_mDoExt_modelUpdateDL(model, 0);
    }
    if ((s8)actor->m671) {
        model = (u32)actor->mpEnergySphereModel.v;
        brkEntry((u32)actor->mpBrkAnm3.v, model, false);
        brkEntry((u32)actor->mpBtkAnm.v, model, true);
        fg_mDoExt_modelUpdateDL(model, 0);
    }
    return TRUE;
}

static void initBrk(gptr<mDoExt_baseAnm> *storage, u32 model, u32 resource, s32 index) {
    void *animation = dComIfG_getObjectRes(fgView<const char>(resource), index, gc_tu.safeString);
    u32 brk = storage->v;
    u32 data = (u32)fgView<J3DModel>(model)->mModelData.v;
    fg_mDoExt_brkAnm_initDiscard(brk, data, gabi::ea(animation), 1, 0, 1.0f, 0, -1, 1, 0);
}

/* 021345C8 */
void deru_brk(fganon_class *actor) {
    initBrk(&actor->mpBrkAnm1, morfModel(actor), gabi::ea(l_archiveAppear),
            dRes_INDEX_FGANON_BRK_DERU_MAIN1_e);
    initBrk(&actor->mpBrkAnm2, (u32)actor->mpKenModel.v, gabi::ea(l_archiveAppear),
            dRes_INDEX_FGANON_BRK_DERU_KEN1_e);
    sound(actor, 0x594E);
    actor->mbIsMaterialized = (s8)(1);
}

/* 021346F0 */
void kieru_brk(fganon_class *actor, u8 which) {
    if (which == 0 || which == 1) {
        initBrk(&actor->mpBrkAnm1, morfModel(actor), gabi::ea(l_archiveDisappear),
                dRes_INDEX_FGANON_BRK_KIERU_MAIN1_e);
        sound(actor, 0x594F);
    }
    if (which == 0 || which == 2)
        initBrk(&actor->mpBrkAnm2, (u32)actor->mpKenModel.v, gabi::ea(l_archiveSwordDisappear),
                dRes_INDEX_PGSW_BRK_KIERU_KEN1_e);
    actor->mbIsMaterialized = (s8)(0);
}

static void monsterSound(fganon_class *actor, u32 id) {
    u32 p = gabi::ea(actor);
    if (gabi::ea(&fgView<fopAc_ac_c>(p)->eyePos)) {
        s8 room = (s8)actor->current.roomNo;
        u32 process = p ? (u32)actor->mBsPcId : 0xFFFFFFFF;
        s32 reverb = (int)fg_dComIfGp_getReverb(room);
        fg_mDoAud_monsSeStart(id, gabi::ea(&actor->eyePos), process, 0, reverb);
    }
}
/* 02139AA0 */
void start(fganon_class *actor) {
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    u32 player = (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v;
    s16 mode = (s16)actor->mMode;
    actor->attention_info.flags = (u32)(0);
    actor->m3AE = (s16)(3);
    switch (mode) {
    case 0: {
        gabi::Local<cXyz> offset;
        fg_cXyz___mi(gabi::ea(&fgView<fopAc_ac_c>(player)->current.pos), gabi::ea(offset.get()),
                     gabi::ea(&actor->home.pos));
        f32 x = offset->x, z = offset->z;
        f32 distance = fg_sqrtf(gabi::fmadds(x, x, z * z));
        f32 radius = (f32)(u8)actor->m2BD * 10.0f;
        if (distance < radius) {
            actor->mMode = (s16)(1);
            actor->mB54 = (s16)(1);
            anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 0, 2, 1, -1);
            fg_mDoAud_bgmAllMute(30);
        }
        break;
    }
    case 1: {
        play = (unsigned int)fg_dComIfGp_getGameInfo();
        u32 target = (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v;
        s16 angle = (short)fg_fopAcM_searchActorAngleY(gabi::ea(actor), target);
        actor->shape_angle.y = (s16)(angle);
        break;
    }
    case 2:
        anm_init(actor, dRes_INDEX_FGANON_BCK_WARAU1_e, 5, 2, 1, -1);
        monsterSound(actor, 0x4939);
        actor->mMode = (s16)(3);
        break;
    case 4:
        actor->mMode = (s16)(5);
        actor->m3A4[0] = (s16)((s16)((s16)l_reg0.s[0] + 0x50));
        anm_init(actor, dRes_INDEX_FGANON_BCK_TAME1_e, 10, 2, 1, -1);
        [[fallthrough]];
    case 5: {
        s16 trigger = (s16)((s16)l_reg0.s[0] + 0x4B);
        if ((s16)actor->m3A4[0] == trigger) {
            actor->m408 = (s8)(1);
            fg_mDoAud_subBgmStart(0x80000041u);
            trigger = (s16)((s16)l_reg0.s[0] + 0x4B);
        }
        if ((s16)actor->m3A4[0] <= trigger)
            sound(actor, 0x5152);
        gabi::Local<cXyz> offset;
        offset->x = 0;
        offset->y = (f32)actor->speedF * 0.5f;
        offset->z = -(f32)actor->speedF;
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->shape_angle.y);
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(&actor->speed));
        fg_PSVECAdd(gabi::ea(&actor->current.pos), gabi::ea(&actor->speed),
                    gabi::ea(&actor->current.pos));
        f32 target = (s16)actor->m3A4[0] == 0 ? 0 : (f32)l_reg0.f[7] + 20;
        f32 max = (f32)l_reg0.f[5] + 0.5f;
        fg_cLib_addCalc2(gabi::ea(&actor->speedF), target, 1.0f, max);
        break;
    }
    }
}

/* 02139E80 */
void end(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    actor->attention_info.flags = (u32)(0);
    u32 morf = (u32)actor->mpMorf.v;
    actor->m3AE = (s16)(3);
    s32 frame = gabi::ftoi((f32)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame);
    switch ((s16)actor->mMode) {
    case 0:
        actor->mMode = (s16)(1);
        anm_init(actor, dRes_INDEX_FGANON_BCK_LAST_DAMAGE1_e, 2, 0, 1, -1);
        actor->mB54 = (s16)(50);
        fg_dSv_event_c_onEventBit(
            gabi::ea(&fgView<FganonSaveView>((u32)l_saveInfo)->mSave.mSavedata.mEvent), 0x3F20);
        break;
    case 1:
        if (frame == 104)
            kieru_brk(actor, 0);
        break;
    case 2:
        deru_brk(actor);
        anm_init(actor, dRes_INDEX_FGANON_BCK_WARAU1_e, 1, 2, 1, -1);
        monsterSound(actor, 0x4939);
        actor->mMode = (s16)(3);
        break;
    case 4:
        deru_brk(actor);
        anm_init(actor, dRes_INDEX_FGANON_BCK_SARU1_e, 1, 0, 1, -1);
        actor->mMode = (s16)(5);
        actor->speed.y = (f32)(0);
        sound(actor, 0x5956);
        [[fallthrough]];
    case 5: {
        actor->current.pos.y = (f32)((f32)actor->current.pos.y + (f32)actor->speed.y);
        f32 acceleration = (f32)l_reg8.f[8] + 0.2f;
        f32 speed = (f32)actor->speed.y + acceleration;
        actor->speed.y = (f32)(speed);
        f32 limit = (f32)l_reg8.f[9] + 20;
        if (speed > limit)
            actor->speed.y = (f32)(limit);
        if (frame == 28) {
            initBrk(&actor->mpBrkAnm1, morfModel(actor), gabi::ea(l_archiveEnd),
                    dRes_INDEX_FGANON_BRK_KIERU_MAIN2_e);
            initBrk(&actor->mpBrkAnm2, (u32)actor->mpKenModel.v, gabi::ea(l_archiveEnd),
                    dRes_INDEX_FGANON_BRK_KIERU_KEN2_e);
            sound(actor, 0x594F);
            actor->mbIsMaterialized = (s8)(0);
        } else if (frame == 48)
            actor->mMode = (s16)(6);
        break;
    }
    }
}

static bool animationStopped(fganon_class *actor) {
    u32 morf = (u32)actor->mpMorf.v;
    return ((u8)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mState & 1) ||
           (f32)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mRate == 0;
}
/* 02139680 */
void down(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    s16 mode = (s16)actor->mMode;
    switch (mode) {
    case 0:
        anm_init(actor, dRes_INDEX_FGANON_BCK_DAMAGE1_e, 2, 0, 1, -1);
        mode = (s16)((s16)actor->mMode + 1);
        actor->speed.y = (f32)(0);
        actor->mMode = (s16)(mode);
        [[fallthrough]];
    case 1: {
        u32 ground = (u32)actor->mAcch.m_flags;
        actor->m3AE = (s16)(5);
        if ((ground & 0x20) && animationStopped(actor)) {
            actor->mMode = (s16)((s16)((s16)actor->mMode + 1));
            anm_init(actor, dRes_INDEX_FGANON_BCK_TYAKUTI1_e, 2, 0, 1, -1);
        }
        actor->m3A4[0] = (s16)((s16)l_HIO.m48);
        break;
    }
    case 2:
        if (animationStopped(actor)) {
            actor->mMode = (s16)((s16)(mode + 1));
            anm_init(actor, dRes_INDEX_FGANON_BCK_DOWN1_e, 2, 2, 1, -1);
        }
        break;
    case 3:
        if (gabi::ftoi((f32)actor->mpMorf->mFrameCtrl.mFrame) == 2)
            monsterSound(actor, 0x4940);
        break;
    case 10:
        anm_init(actor, dRes_INDEX_FGANON_BCK_DAMAGE2_e, 2, 0, 1, -1);
        actor->mMode = (s16)(2);
        break;
    }
    f32 speed = (f32)actor->speed.y;
    f32 y = (f32)actor->current.pos.y;
    f32 nextSpeed = speed - 0.5f;
    actor->current.pos.y = (f32)(y + speed);
    if (nextSpeed < -15)
        nextSpeed = -15;
    actor->speed.y = (f32)(nextSpeed);
    gabi::Local<dBgS_GndChk> ground;

    dBgS_GndChk_ct(ground.get(), l_groundCheckVtables, true);
    ground->dBgS_GrpPassChk::mGrp = (u32)(0xE);
    y = (f32)actor->current.pos.y;
    f32 x = (f32)actor->current.pos.x;
    f32 z = (f32)actor->current.pos.z;
    ground->m_pos.x = (f32)(x);
    ground->m_pos.y = (f32)(y + 300);
    ground->m_pos.z = (f32)(z);
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    f32 height = fg_cBgS_GroundCross(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mBgS),
                                     gabi::ea(ground.get()));
    bool below = height != -1000000000.0f && !((f32)actor->current.pos.y > height);
    if (below || (s16)actor->m3A4[0] == 0) {
        u8 variant = (u8)actor->m2BC;
        actor->mAction = ACT_KIERU;
        actor->mMode = (s16)(0);
        if (variant)
            actor->health = (s8)(100);
    }
    ground->cBgS_PolyInfo::__vtbl = (u32)(0x1000EC54);
    ground->dBgS_PolyPassChk::__vtbl = (u32)(0x1000EC74);
    ground->dBgS_GrpPassChk::__vtbl = (u32)(0x1000EC34);
    fg_cBgS_Chk_cBgS_Chk(gabi::ea(ground.get()), 0);
}

/* 02139194 */
void spinattack2(fganon_class *actor) {
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    u32 player = (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v;
    gabi::Local<dBgS_LinChk> line;

    dBgS_LinChk_ct(line.get(), l_lineCheckVtables, false);
    u32 morf = (u32)actor->mpMorf.v;
    s16 mode = (s16)actor->mMode;
    s32 frame = gabi::ftoi((f32)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame);
    switch (mode) {
    case 0: {
        u8 copy = (u8)actor->m68F;
        s16 playerYaw = (s16)fgView<fopAc_ac_c>(player)->shape_angle.y;
        actor->shape_angle.y = (s16)((s16)(playerYaw + copy * 0x3333));
        actor->mMode = (s16)(1);
        if (copy == 0)
            for (u32 i = 1; i < 5; ++i)
                fg_fopAcM_create1(0xF1, (i * 16) | 3, gabi::ea(&actor->current.pos),
                                  (s8)actor->current.roomNo, 0, 0, -1, 0);
        [[fallthrough]];
    }
    case 1: {
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->shape_angle.y);
        gabi::Local<cXyz> offset, transformed, result;
        offset->x = 0;
        offset->y = (f32)l_HIO.m2C;
        offset->z = (f32)l_HIO.m34;
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_cXyz___pl(gabi::ea(&fgView<fopAc_ac_c>(player)->current.pos), gabi::ea(result.get()),
                     gabi::ea(transformed.get()));
        actor->current.pos.copy(*result.get());
        anm_init(actor, dRes_INDEX_FGANON_BCK_KAITEN_S1_e, 1, 2, 1, -1);
        actor->mMode = (s16)(2);
        deru_brk(actor);
        actor->m38C.copy(*&fgView<fopAc_ac_c>(player)->current.pos);
        [[fallthrough]];
    }
    case 2: {
        actor->current.angle.y = (s16)((s16)actor->shape_angle.y);
        f32 speed = (f32)l_reg8.f[4] + 5;
        actor->current.angle.x = (s16)(0);
        actor->speedF = (f32)(speed);
        pos_move(actor, 1);
        gabi::Local<cXyz> result, offset;
        fg_cXyz___mi(gabi::ea(&actor->m38C), gabi::ea(result.get()), gabi::ea(&actor->current.pos));
        offset->copy(*result.get());
        f32 magnitude = fg_PSVECSquareMag(gabi::ea(offset.get()));
        f32 distance = fg_sqrtf(magnitude);
        if (distance < (f32)l_HIO.m38) {
            anm_init(actor, dRes_INDEX_FGANON_BCK_KAITEN1_e, 2, 0, 1, -1);
            u8 copy = (u8)actor->m68F;
            actor->mMode = (s16)(3);
            actor->speedF = (f32)(0);
            if (copy == 0)
                monsterSound(actor, 0x493C);
        }
        break;
    }
    case 3: {
        if (frame == 14 && (u8)actor->m68F == 0)
            sound(actor, 0x5951);
        actor->m685 = (u8)(1);
        actor->mWeponSph.mAtp = (u8)(8);
        s16 trigger = (s16)l_HIO.m3E;
        morf = (u32)actor->mpMorf.v;
        if ((u32)frame == (u32)(s32)trigger)
            actor->m6AC = (s8)((s8)((s16)l_HIO.m40 - trigger));
        if (((u8)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mState & 1) ||
            (f32)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mRate == 0) {
            if ((u8)actor->m68F == 0) {
                actor->mMode = (s16)(4);
                actor->m3A4[0] = (s16)(40);
                anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 10, 2, 1, -1);
            } else {
                actor->mAction = ACT_KIERU;
                actor->mMode = (s16)(0);
            }
        }
        break;
    }
    case 4:
        if ((s16)actor->m3A4[0] == 0) {
            actor->mAction = ACT_KIERU;
            actor->mMode = (s16)(0);
        }
        break;
    }
    line->dBgS_PolyPassChk::__vtbl = (u32)(0x1000ED64);
    line->dBgS_GrpPassChk::__vtbl = (u32)(0x1000EC34);
    line->cBgS_PolyInfo::__vtbl = (u32)(0x1000EC24);
    fg_cBgS_LinChk_dtor(gabi::ea(line.get()), 0);
}

/* 0213A228 */
void last_end(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    s16 mode = (s16)actor->mMode;
    actor->m3AE = (s16)(3);
    actor->attention_info.flags = (u32)(0);
    switch (mode) {
    case 0:
        actor->mMode = (s16)(1);
        anm_init(actor, dRes_INDEX_FGANON_BCK_MOGAKU1_e, 2, 0, 1, -1);
        actor->mB54 = (s16)(100);
        actor->m3C0 = (f32)(10000);
        actor->m3A4[1] = (s16)(150);
        actor->m3A4[0] = (s16)((s16)((s16)l_reg0.s[4] + 25));
        break;
    case 1:
        if ((s16)actor->m3A4[1] == 0) {
            s16 counter = (s16)actor->mB54;
            actor->m2D0 = (s8)(1);
            actor->mMode = (s16)(2);
            actor->mB54 = (s16)((s16)(counter + 1));
            actor->m3A4[1] = (s16)(20);
        }
        break;
    case 2:
        fg_cLib_addCalc2(gabi::ea(&actor->scale.x), 0.1f, 1.0f, 0.05f);
        fg_cLib_addCalc2(gabi::ea(&actor->scale.z), 0.1f, 1.0f, 0.05f);
        fg_cLib_addCalc2(gabi::ea(&actor->scale.y), 0.1f, 1.0f, 0.05f);
        if ((s16)actor->m3A4[1] == 0) {
            actor->mMode = (s16)(3);
            actor->m3A4[1] = (s16)(30);
            sound(actor, 0x5905);
        }
        break;
    case 3:
        fg_cLib_addCalc0(gabi::ea(&actor->scale.x), 0.1f, 0.05f);
        fg_cLib_addCalc0(gabi::ea(&actor->scale.z), 0.1f, 0.05f);
        fg_cLib_addCalc2(gabi::ea(&actor->scale.y), 10.0f, 0.1f, 1.0f);
        if ((s16)actor->m3A4[1] == 0) {
            actor->mMode = (s16)(4);
            actor->scale.x = (f32)(0);
            actor->m3A4[1] = (s16)(30);
            actor->scale.y = (f32)(0);
            actor->scale.z = (f32)(0);
        }
        break;
    case 4: {
        s16 timer = (s16)actor->m3A4[1];
        if (timer <= 4) {
            u32 id;
            if (timer == 4) {
                id = (unsigned int)fg_fopAcM_create2(0x1CF, 5, gabi::ea(&actor->current.pos),
                                                     (s8)actor->current.roomNo, 0, 0, -1, 0);
                actor->mBokoID = (u32)(id);
            } else
                id = (u32)actor->mBokoID;
            gabi::Local<be<u32>> local;
            *local.get() = id;
            u32 boko = 0;
            if (id != 0xFFFFFFFF)
                boko = (unsigned int)fg_fopAcIt_Judge(l_judgeById, gabi::ea(local.get()));
            if (boko) {
                actor->m2D0 = (s8)(2);
                if (!((u32)fgView<fopAc_ac_c>(boko)->actor_status & 0x2000))
                    fg_fopAcM_setCarryNow(boko, 0);
                u32 source = (u32)actor->mpKenModel.v;
                u32 destination = (u32)fgView<FganonBoko>(boko)->mpModel;
                if (source)
                    source += 0xC8;
                if (destination) {
                    f32 values[12];
                    for (u32 i = 0; i < 12; ++i)
                        values[i] = (f32)fgMatrixFloat(source, i);
                    for (u32 i = 0; i < 12; ++i)
                        fgView<J3DModel>(destination)->mBaseTransformMtx.m[i / 4][i % 4] =
                            values[i];
                }
                if ((s16)actor->m3A4[1] == 1) {
                    fg_fopAcM_cancelCarryNow(boko);
                    s16 counter = (s16)actor->mB54;
                    actor->mB56 = (s16)(0);
                    actor->mMode = (s16)(5);
                    actor->mB54 = (s16)((s16)(counter + 1));
                }
            }
        }
        break;
    }
    }
    f32 fogEnd = (f32)actor->m3C0;
    s16 timer = (s16)actor->m3A4[0];
    actor->tevStr.mFogColor.b = (u16)(0xFF);
    actor->tevStr.mFogColor.g = (u16)(0xFF);
    actor->tevStr.mFogColor.r = (u16)(0xFF);
    f32 fogStart = (f32)l_reg8.f[10];
    actor->tevStr.mFogEndZ = (f32)(fogEnd);
    actor->tevStr.mFogStartZ = (f32)(fogStart);
    if (timer <= 1) {
        if (timer == 1) {
            actor->m68E = (s8)(1);
            actor->m3C0 = (f32)(5100);
        }
        f32 max = (f32)l_reg0.f[4] + 63.5f;
        fg_cLib_addCalc2(gabi::ea(&actor->m3C0), 100.0f, 1.0f, max);
    }
    fg_cLib_addCalcAngleS2(gabi::ea(&actor->current.angle.x), 0, 4, 0x800);
}

static void invalidateEmitter(fganon_class *actor, u32 slot) {
    u32 emitter = (u32)fgEmitterAt(actor, slot).v;
    if (emitter) {
        u32 flags = (u32)fgView<FganonEmitter>(emitter)->mStatus;
        fgView<FganonEmitter>(emitter)->mMaxFrame = (s32)(-1);
        fgView<FganonEmitter>(emitter)->mStatus = (u32)(flags | 1);
        fgEmitterAt(actor, slot).v = 0;
    }
}
static u32 createParticle(u32 id, cXyz *position, csXyz *angle = nullptr) {
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    u32 particles = (u32)fgView<dComIfG_inf_c>(play)->play.mParticle.v;
    return (unsigned int)fg_dPa_control_c_set1(particles, 0, id, gabi::ea(position),
                                               gabi::ea(angle), 0, 0xFF, 0, -1, 0, 0, 0);
}
static void emitterJointMatrix(fganon_class *actor, u32 emitter, u32 index) {
    u32 model = morfModel(actor);
    u32 buffer = (u32)fgView<J3DModel>(model)->mpMtxBuffer.v;
    u16 flags = (u16)fgView<J3DMtxBuffer_hd>(buffer)->mFlags;
    u32 matrices = (u32)fgView<J3DMtxBuffer_hd>(buffer)->mpAnmMtx;
    fgView<J3DMtxBuffer_hd>(buffer)->mFlags = (u16)(flags | 0x10);
    fg_JPASetRMtxTVecfromMtx(matrices + index * 0x30,
                             gabi::ea(&fgView<FganonEmitter>(emitter)->mGlobalRotation),
                             gabi::ea(&fgView<FganonEmitter>(emitter)->mGlobalTranslation));
}
static void tama_set(fganon_class *actor) {
    if ((s8)actor->m671 == 0)
        return;
    fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->shape_angle.y);
    gabi::Local<cXyz> offset, transformed, position;
    f32 distance = (f32)actor->m680;
    offset->x = 0;
    offset->z = distance;
    offset->y = -distance;
    fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
    f32 x = (f32)actor->current.pos.x + transformed->x;
    f32 y = (f32)actor->current.pos.y + 400;
    y = y + (f32)l_reg8.f[4];
    y = y + transformed->y;
    f32 z = (f32)actor->current.pos.z + transformed->z;
    s8 state = (s8)actor->m671;
    position->x = x;
    position->y = y;
    position->z = z;
    actor->m664.x = (f32)(x);
    actor->m664.y = (f32)(y);
    actor->m664.z = (f32)(z);
    switch (state) {
    case 1: {
        invalidateEmitter(actor, 4);
        invalidateEmitter(actor, 5);
        u32 emitter = createParticle(0x821A, position.get());
        actor->mEmitters3[0].v = (u32)(emitter);
        emitter = createParticle(0x821B, position.get());
        actor->mEmitters3[1].v = (u32)(emitter);
        u32 brk = (u32)actor->mpBrkAnm3.v;
        actor->m672 = (u8)(250);
        actor->m671 = (s8)(2);
        fgView<mDoExt_baseAnm>(brk)->mFrameCtrl.mRate = 1;
        break;
    }
    case 3:
        actor->mpBrkAnm3->mFrameCtrl.mRate = -1;
        actor->m671 = (s8)(4);
        createParticle(0x8242, position.get(), &actor->shape_angle);
        createParticle(0x8243, position.get(), &actor->shape_angle);
        break;
    case 4: {
        u8 alpha = (u8)((u8)actor->m672 - 50);
        actor->m672 = (u8)(alpha);
        for (u32 slot = 4; slot < 6; ++slot) {
            u32 emitter = (u32)fgEmitterAt(actor, slot).v;
            if (emitter) {
                fgView<FganonEmitter>(emitter)->mGlobalAlpha = (u8)(alpha);
                alpha = (u8)actor->m672;
            }
        }
        if (alpha == 0)
            actor->m671 = (s8)(5);
        break;
    }
    case 5:
    case 6:
    case 7:
    case 8:
        actor->m671 = (s8)((s8)(state + 1));
        break;
    case 9:
        actor->m671 = (s8)(0);
        break;
    }
    for (u32 slot = 4; slot < 6; ++slot) {
        u32 emitter = (u32)fgEmitterAt(actor, slot).v;
        if (emitter) {
            if ((u8)actor->m672) {
                f32 emitterY = position->y;
                if ((u8)fgView<FganonEmitter>(emitter)->mParticleCount >= 7)
                    emitterY = -emitterY;
                fgView<FganonEmitter>(emitter)->mGlobalTranslation.x = (f32)(position->x);
                fgView<FganonEmitter>(emitter)->mGlobalTranslation.y = (f32)(emitterY);
                fgView<FganonEmitter>(emitter)->mGlobalTranslation.z = (f32)(position->z);
            } else
                invalidateEmitter(actor, slot);
        }
    }
    fg_PSMTXTrans(l_mtxStack, (f32)position->x, (f32)position->y, (f32)position->z);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = (f32)mDoMtx_stack_c::get()->m[i / 4][i % 4];
    u32 model = (u32)actor->mpEnergySphereModel.v;
    for (u32 i = 0; i < 12; ++i)
        fgView<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
    fg_mDoExt_baseAnm_play((u32)actor->mpBrkAnm3.v);
    fg_mDoExt_baseAnm_play((u32)actor->mpBtkAnm.v);
}
/* 02138670 */
void shot2(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    s16 angle = (short)fg_fopAcM_searchActorAngleY(
        gabi::ea(actor), (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    fg_cLib_addCalcAngleS2(gabi::ea(&actor->shape_angle.y), angle, 10, 0x400);
    s16 mode = (s16)actor->mMode;
    s32 frame = gabi::ftoi((f32)actor->mpMorf->mFrameCtrl.mFrame);
    switch (mode) {
    case 0: {
        anm_init(actor, dRes_INDEX_FGANON_BCK_TAME_S1_e, 5, 0, 1, -1);
        invalidateEmitter(actor, 2);
        invalidateEmitter(actor, 3);
        u32 emitter = createParticle(0x8218, &actor->current.pos);
        actor->mEmitters2[0].v = (u32)(emitter);
        emitter = createParticle(0x8219, &actor->current.pos);
        actor->mEmitters2[1].v = (u32)(emitter);
        monsterSound(actor, 0x493B);
        actor->mMode = (s16)((s16)((s16)actor->mMode + 1));
        [[fallthrough]];
    }
    case 1:
        sound(actor, 0x5153);
        for (u32 slot = 2; slot < 4; ++slot) {
            u32 emitter = (u32)fgEmitterAt(actor, slot).v;
            if (emitter)
                emitterJointMatrix(actor, emitter, 26);
        }
        if (animationStopped(actor)) {
            anm_init(actor, dRes_INDEX_FGANON_BCK_TAME_S2_e, 1, 2, 1, -1);
            s16 modeNow = (s16)actor->mMode;
            actor->m3A4[0] = (s16)((s16)l_HIO.m3C);
            actor->mMode = (s16)((s16)(modeNow + 1));
            invalidateEmitter(actor, 2);
            invalidateEmitter(actor, 3);
        }
        if (frame == 28) {
            actor->m680 = (f32)(0);
            actor->m671 = (s8)(1);
        }
        break;
    case 2:
        sound(actor, 0x5153);
        if ((s16)actor->m3A4[0] != 0)
            break;
        anm_init(actor, dRes_INDEX_FGANON_BCK_NAGERU_S1_e, 3, 0, 1, -1);
        actor->mMode = (s16)((s16)((s16)actor->mMode + 1));
        {
            u32 emitter = createParticle(0x821C, &actor->current.pos);
            actor->mEmitters2[0].v = (u32)(emitter);
            emitter = createParticle(0x821D, &actor->current.pos);
            actor->mEmitters2[1].v = (u32)(emitter);
        }
        [[fallthrough]];
    case 3: {
        s32 trigger = (s16)l_reg8.s[5] + 15;
        if (frame == trigger) {
            monsterSound(actor, 0x493D);
            sound(actor, 0x5955);
            trigger = (s16)l_reg8.s[5] + 15;
        }
        if (frame >= trigger) {
            f32 target = (f32)l_reg8.f[6] + 250;
            f32 max = (f32)l_reg8.f[7] + 50;
            fg_cLib_addCalc2(gabi::ea(&actor->m680), target, 1.0f, max);
            if ((u32)frame == (u32)(s32)(s16)((s16)l_reg8.s[6] + 16))
                actor->m671 = (s8)(3);
            if ((u32)frame == (u32)(s32)(s16)((s16)l_reg8.s[7] + 16)) {
                for (u32 i = 0; i < 8; ++i)
                    fg_fopAcM_create1(0xF2, i, gabi::ea(&actor->m664), (s8)actor->current.roomNo, 0,
                                      0, -1, 0);
                actor->m670 = (s8)(0);
            }
        }
        for (u32 i = 0; i < 2; ++i) {
            u32 emitter = (u32)actor->mEmitters2[i].v;
            if (emitter)
                emitterJointMatrix(actor, emitter, 14 + i * 9);
        }
        if (animationStopped(actor)) {
            anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 10, 2, 1, -1);
            actor->m3A4[0] = (s16)(60);
            actor->mMode = (s16)(4);
            invalidateEmitter(actor, 2);
            invalidateEmitter(actor, 3);
        }
        break;
    }
    case 4:
        if ((s8)actor->m68A == 1)
            anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 10, 2, 1, -1);
        if ((s16)actor->m3A4[0] == 0) {
            actor->mAction = ACT_FLY2;
            actor->mMode = (s16)(0);
        }
        break;
    }
    f32 max = (f32)l_reg0.f[14] + 1;
    fg_cLib_addCalc0(gabi::ea(&actor->speedF), 1.0f, max);
    pos_move(actor, 0);
    fly_se_set(actor);
    tama_set(actor);
}

static void hitFlash(fganon_class *actor, cXyz *position, f32 size, bool critical,
                     bool swordFlash) {
    if (critical)
        createParticle(0x10, &actor->eyePos);
    gabi::Local<cXyz> scale;
    scale->x = size;
    scale->y = size;
    scale->z = size;
    gabi::Local<csXyz> angle;
    angle->z = 0;
    angle->x = 0;
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    angle->y = (short)fg_fopAcM_searchActorAngleY(
        gabi::ea(actor), (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    play = (unsigned int)fg_dComIfGp_getGameInfo();
    fg_dPa_control_c_set2((u32)fgView<dComIfG_inf_c>(play)->play.mParticle.v, 0, 0xD,
                          gabi::ea(position), gabi::ea(angle.get()), gabi::ea(scale.get()), 0xFF, 0,
                          -1, 0, 0, 0);
    if (swordFlash) {
        gabi::Local<cXyz> point;
        point->x = (f32)actor->eyePos.x;
        point->y = (f32)actor->eyePos.y;
        point->z = (f32)actor->eyePos.z;
        fg_dKy_SordFlush_set(gabi::ea(point.get()), 1);
    }
}
static void knockDown(fganon_class *actor, u32 player) {
    actor->mMode = (s16)(0);
    actor->mAction = ACT_DOWN;
    actor->m3B0 = (f32)((f32)l_reg8.f[11] + 80);
    actor->m3B8 = (s16)((s16)((s16)l_reg8.s[2] + 7));
    gabi::Local<cXyz> delta;
    fg_cXyz___mi(gabi::ea(&actor->current.pos), gabi::ea(delta.get()),
                 gabi::ea(&fgView<fopAc_ac_c>(player)->current.pos));
    s16 yaw = (short)fg_cM_atan2s((f32)delta->x, (f32)delta->z);
    f32 x = delta->x, z = delta->z;
    actor->m3B4 = (s16)(yaw);
    f32 distance = fg_sqrtf(gabi::fmadds(x, x, z * z));
    s16 pitch = (short)fg_cM_atan2s((f32)delta->y, distance);
    actor->speedF = (f32)(0);
    actor->m3B6 = (s16)((s16)-pitch);
}
/* 0213A6FC */
void damage_check(fganon_class *actor) {
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    u32 player = (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v;
    fg_dCcD_GStts_Move(gabi::ea(&actor->mStts.__vtbl_gstts));
    if ((s16)actor->m3AE != 0)
        return;
    // HD: attack information includes the callee-written result word at +0x18.
    gabi::Local<FganonAttackInfo> info;
    info->pParticlePos = (u32)(0);
    if ((int)fg_dCcD_GObjInf_ChkTgHit(gabi::ea(&actor->mCyl))) {
        u32 hit = (unsigned int)fg_dCcD_GObjInf_GetTgHitObj(gabi::ea(&actor->mCyl));
        info->mpObj = (u32)(hit);
        if (hit) {
            info->pParticlePos = (u32)(gabi::ea(&actor->mCyl.mGObjTg.mHitPos));
            u32 status = (u32)fgView<dCcD_GObjInf>(hit)->mStts.v;
            u8 variant = (u8)actor->m2BC;
            u32 source = status ? (u32)fgView<dCcD_Stts>(status)->mp_actor.v : 0;
            info->mpActor = source;
            if (variant == 2) {
                if ((u32)fgView<dCcD_GObjInf>(hit)->mAtType & 0x100000) {
                    monsterSound(actor, 0x4945);
                    sound(actor, 0x5957);
                    hitFlash(actor, &actor->eyePos, 2, true, true);
                    actor->mMode = (s16)(0);
                    actor->m3AE = (s16)(1000);
                    actor->mAction = ACT_LAST_END;
                    fg_mDoAud_subBgmStop();
                    if ((s8)actor->m408 != 0)
                        actor->m408 = (s8)(35);
                    sound(actor, 0x5904);
                    u32 emitter = createParticle(0x8405, &actor->current.pos);
                    if (emitter)
                        emitterJointMatrix(actor, emitter, 10);
                    emitter = createParticle(0x8406, &actor->current.pos);
                    if (emitter)
                        emitterJointMatrix(actor, emitter, 10);
                    actor->speedF = (f32)(0);
                } else {
                    s8 soundState = (s8)actor->m408;
                    actor->mMode = (s16)(0);
                    actor->mAction = ACT_KIERU;
                    if (soundState == 1 || soundState == 2)
                        actor->m408 = (s8)(35);
                }
                actor->m671 = (s8)(0);
                invalidateEmitter(actor, 4);
                invalidateEmitter(actor, 5);
                return;
            }
            if (source && (s16)fgView<fopAc_ac_c>(source)->mProcName == 0xF2) {
                actor->m670 = (s8)((s8)((u8)actor->m670 + 1));
                anm_init(actor, dRes_INDEX_FGANON_BCK_DAMAGE1_e, 2, 0, 1, -1);
                actor->m3A4[0] = (s16)(60);
                monsterSound(actor, 0x493F);
                if ((s8)actor->m670 < 5)
                    return;
                hitFlash(actor, &actor->eyePos, 2, true, true);
                knockDown(actor, player);
                return;
            }
        }
    }
    if ((u8)actor->m68F) {
        if ((int)fg_dCcD_GObjInf_ChkTgHit(gabi::ea(&actor->mCyl)) ||
            (s16)fgView<fganon_class>(master)->mAction == ACT_DOWN) {
            u32 emitter = createParticle(0x826B, &actor->current.pos);
            if (emitter)
                emitterJointMatrix(actor, emitter, 0);
            fg_fopAcM_delete(gabi::ea(actor));
        }
        return;
    }
    if (!(int)fg_dCcD_GObjInf_ChkTgHit(gabi::ea(&actor->mCyl)) && (s8)actor->m68C == 0)
        return;
    actor->m3AE = (s16)(6);
    if ((int)fg_dCcD_GObjInf_ChkTgHit(gabi::ea(&actor->mCyl))) {
        s16 action = (s16)actor->mAction;
        if (action != 8 && action != 7 && action != 10)
            return;
        u32 hit = (unsigned int)fg_dCcD_GObjInf_GetTgHitObj(gabi::ea(&actor->mCyl));
        info->mpObj = (u32)(hit);
        info->pParticlePos = (u32)(gabi::ea(&actor->mCyl.mGObjTg.mHitPos));
        u32 source = (unsigned int)fg_at_power_check1(gabi::ea(info.get()));
        info->mpActor = source;
        hit = (u32)info->mpObj;
        if (!hit || !((u32)fgView<dCcD_GObjInf>(hit)->mAtType & 2))
            return;
        u8 sword = (u8)fgView<FganonSaveEquipment>(l_saveInfo)->mSelectEquip;
        if (sword != 0x39 && sword != 0x3E && sword != 0x3A)
            return;
        source = (unsigned int)fg_cc_at_check(gabi::ea(actor), gabi::ea(info.get()));
        info->mpActor = source;
        if ((u8)info->mResultingAttackType == 1 &&
            (u8)fgView<FganonPlayerCut>(player)->mCutType != 0)
            actor->m3AE = (s16)(2);
        bool dead = (u8)info->mbDead != 0;
        if (dead)
            createParticle(0x10, &actor->eyePos);
        gabi::Local<cXyz> scale;
        scale->x = dead ? 2 : 1;
        scale->y = dead ? 2 : 1;
        scale->z = dead ? 2 : 1;
        monsterSound(actor, (s8)actor->health <= 0 ? 0x4943 : 0x4942);
        gabi::Local<csXyz> angle;
        angle->z = 0;
        angle->x = 0;
        play = (unsigned int)fg_dComIfGp_getGameInfo();
        angle->y = (short)fg_fopAcM_searchActorAngleY(
            gabi::ea(actor), (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
        play = (unsigned int)fg_dComIfGp_getGameInfo();
        fg_dPa_control_c_set2((u32)fgView<dComIfG_inf_c>(play)->play.mParticle.v, 0, 0xD,
                              gabi::ea(&actor->mCyl.mGObjTg.mHitPos), gabi::ea(angle.get()),
                              gabi::ea(scale.get()), 0xFF, 0, -1, 0, 0, 0);
        action = (s16)actor->mAction;
        if (action == 7 || action == 10) {
            actor->mMode = (s16)(0);
            actor->mAction = ACT_DOWN;
            return;
        }
        u8 variant = (u8)actor->m2BC;
        if (variant == 0) {
            if ((s8)actor->health <= 0) {
                actor->mMode = (s16)(0);
                actor->m3AE = (s16)(1000);
                actor->mAction = ACT_END;
                fg_mDoAud_subBgmStop();
            } else {
                s8 remaining = (s8)((u8)actor->m690 - (u8)info->mDamage);
                if (remaining <= 0) {
                    actor->mMode = (s16)(0);
                    actor->mAction = ACT_KIERU;
                    actor->m690 = (s8)(10);
                } else {
                    actor->m690 = (s8)(remaining);
                    actor->mMode = (s16)(10);
                }
            }
        } else if (variant == 1) {
            actor->health = (s8)(0);
            actor->mMode = (s16)(0);
            actor->mAction = ACT_FAIL;
            fg_mDoAud_subBgmStop();
        }
    } else {
        monsterSound(actor, 0x493F);
        hitFlash(actor, &actor->eyePos, 2, true, true);
        knockDown(actor, player);
    }
}

// These movement actions are inlined into move in the HD executable.
static void destroyMovementLine(dBgS_LinChk *line) {
    u32 p = gabi::ea(line);
    line->dBgS_PolyPassChk::__vtbl = (u32)(0x1000ED64);
    line->dBgS_GrpPassChk::__vtbl = (u32)(0x1000EC34);
    line->cBgS_PolyInfo::__vtbl = (u32)(0x1000EC24);
    fg_cBgS_LinChk_dtor(gabi::ea(line), 0);
}
static bool kabe_check(fganon_class *actor) {
    u32 player =
        (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v;
    gabi::Local<dBgS_LinChk> line;

    dBgS_LinChk_ct(line.get(), l_lineCheckVtables, false);
    gabi::Local<cXyz> offset, transformed, temporary, end;
    offset->x = 0;
    offset->y = 0;
    offset->z = (f32)l_HIO.m34 + 100.0f;
    bool hit = false;
    for (int i = 0; i < 8; ++i) {
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)(i * 0x2000));
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_cXyz___pl(gabi::ea(&fgView<fopAc_ac_c>(player)->eyePos), gabi::ea(temporary.get()),
                     gabi::ea(transformed.get()));
        *end.get() = *temporary.get();
        fg_dBgS_LinChk_Set(gabi::ea(line.get()), gabi::ea(&fgView<fopAc_ac_c>(player)->eyePos),
                           gabi::ea(end.get()), gabi::ea(actor));
        u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
        if ((int)fg_cBgS_LineCross(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mBgS),
                                   gabi::ea(line.get()))) {
            hit = true;
            break;
        }
    }
    destroyMovementLine(line.get());
    return hit;
}
static void deru(fganon_class *actor) {
    // This accessor call is retained even when the selected mode does no work.
    fg_dComIfGp_getGameInfoDiscard();
    actor->m3AE = 3;
    if (actor->mMode != 0)
        return;
    u8 choice = (u8)l_HIO.m06;
    if (choice == 0) {
        if (actor->m2BC == 0)
            choice = fg_cM_rndF(1.0f) < 0.5f ? 1 : 2;
        else if (fg_cM_rndF(1.0f) < 0.4f && !kabe_check(actor))
            choice = 4;
        else
            choice = fg_cM_rndF(1.0f) < 0.5f ? 3 : 1;
    }
    switch (choice) {
    case 1:
        actor->mAction = ACT_FLY;
        actor->mMode = -10;
        actor->m3A4[2] = (s16)l_HIO.m0A;
        break;
    case 2:
        actor->mAction = ACT_SPIN;
        actor->mMode = 0;
        break;
    case 3:
        actor->mAction = ACT_FLY2;
        actor->mMode = -10;
        actor->m3A4[2] = (s16)l_HIO.m0A;
        break;
    case 4:
        actor->mAction = ACT_SPIN2;
        actor->mMode = 0;
        break;
    }
}
static void kieru(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    actor->m3AE = 3;
    switch ((s16)actor->mMode) {
    case 0:
        if (actor->m408 == 1 || actor->m408 == 2)
            actor->m408 = 35;
        kieru_brk(actor, 0);
        if (actor->m68F) {
            actor->mMode = 1;
            actor->m3A4[0] = 30;
        } else {
            actor->mAction = ACT_STANDBY;
            actor->mMode = 0;
        }
        break;
    case 1:
        if (actor->m3A4[0] == 0)
            fopAcM_delete(actor);
        break;
    }
}
static void standby(fganon_class *actor) {
    u32 player =
        (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v;
    actor->m3AE = 3;
    switch ((s16)actor->mMode) {
    case -1: {
        if ((actor->m2BC == 2 && !(int)fg_dComIfGs_checkGetItem(0x36)) ||
            (actor->m2BC == 1 && (int)fg_dComIfGs_checkGetItem(0x36))) {
            actor->current.pos.y = (f32)(-20000.0f);
            break;
        }
        f32 z = (f32)fgView<fopAc_ac_c>(player)->current.pos.z - (f32)actor->home.pos.z;
        f32 x = (f32)fgView<fopAc_ac_c>(player)->current.pos.x - (f32)actor->home.pos.x;
        f32 distance = fg_sqrtf(gabi::fmadds(x, x, z * z));
        if (distance < (f32)(u8)actor->m2BD * 10.0f) {
            actor->m3A4[0] = (s16)l_HIO.m08;
            actor->mB89 = 22;
            actor->mMode = 1;
            actor->actor_status = (u32)((u32)actor->actor_status | 0x20);
        }
        break;
    }
    case 0:
        actor->m3A4[0] = (s16)l_HIO.m08;
        actor->mMode = (s16)actor->mMode + 1;
        [[fallthrough]];
    case 1:
        if (actor->m3A4[0] == 0) {
            actor->m69C = 0;
            actor->m698 = 0;
            actor->mAction = ACT_DERU;
            actor->m694 = 0;
            actor->mMode = 0;
        }
        break;
    }
}
static void fail(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    actor->m3AE = 3;
    switch ((s16)actor->mMode) {
    case 0:
        kieru_brk(actor, 1);
        actor->mMode = 1;
        anm_init(actor, dRes_INDEX_FGANON_BCK_DAMAGE2_e, 2.0f, 0, 1.0f, -1);
        break;
    case 1: {
        if (!animationStopped(actor))
            break;
        actor->mMode = 2;
        actor->m3A4[0] = 30;
        gabi::Local<dBgS_LinChk> line;

        dBgS_LinChk_ct(line.get(), l_lineCheckVtables, false);
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->home.angle.y);
        gabi::Local<cXyz> offset, transformed;
        offset->x = 0;
        offset->y = 0;
        offset->z = 10000;
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        offset->x = (f32)actor->home.pos.x;
        offset->y = (f32)actor->home.pos.y + 100;
        offset->z = (f32)actor->home.pos.z;
        fg_PSVECAdd(gabi::ea(transformed.get()), gabi::ea(offset.get()),
                    gabi::ea(transformed.get()));
        fg_dBgS_LinChk_Set(gabi::ea(line.get()), gabi::ea(offset.get()),
                           gabi::ea(transformed.get()), gabi::ea(actor));
        fg_PSMTXCopy(jointMatrix(morfModel(actor), 24), (u32)l_calcMtx);
        offset->x = 0;
        offset->y = 0;
        offset->z = 0;
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
        if ((int)fg_cBgS_LineCross(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mBgS),
                                   gabi::ea(line.get()))) {
            offset->x = (f32)line->mLin.mEnd.x;
            offset->y = (f32)line->mLin.mEnd.y;
            offset->z = (f32)line->mLin.mEnd.z;
        }
        s16 angle = (short)fg_cM_atan2s((f32)transformed->x - (f32)offset->x,
                                        (f32)transformed->z - (f32)offset->z);
        actor->m6A0 = (s16)actor->shape_angle.y - angle + 0x7058 + (s16)l_swordFallOffset;
        actor->m6A8 = 100;
        actor->m6A4 = (s16)l_reg0.s[5] + 0x80;
        destroyMovementLine(line.get());
        break;
    }
    case 2:
        if (actor->m3A4[0] == 0) {
            s16 step = (s16)((s16)gabi::ftoi(actor->m69C) + (s16)actor->m6A4);
            fg_cLib_addCalcAngleS2(gabi::ea(&actor->m6A2), (s16)actor->m6A0, 1, step);
            actor->m694 = (f32)actor->m694 + (f32)actor->m698;
            actor->m698 = (f32)actor->m698 + (f32)actor->m69C;
            actor->m69C = (f32)actor->m69C + ((f32)l_reg12.f[10] + 3.0f);
            f32 limit = (f32)l_reg12.f[3] + 1000.0f;
            if (actor->m69C > limit)
                actor->m69C = limit;
            if (!(actor->m694 < 15884.0f)) {
                actor->m694 = 15884.0f;
                actor->m6A4 = (s16)l_reg0.s[6] + 0x800;
                if (actor->m698 > (f32)l_reg12.f[1] + 700.0f) {
                    actor->m698 = -((f32)actor->m698 * ((f32)l_reg12.f[2] + 0.4f));
                    sound(actor, 0x6998, actor->m6A8);
                    if (actor->m6A8 >= 30)
                        actor->m6A8 = (u32)actor->m6A8 - 20;
                    if (actor->m6A6 == 0)
                        actor->m6A6 = (s16)l_reg0.s[1] + 30;
                } else {
                    actor->m698 = 0;
                    actor->m3A4[0] = 0;
                    actor->mMode = 3;
                }
            }
        }
        break;
    case 3:
        if (actor->m3A4[0] == 0) {
            kieru_brk(actor, 2);
            actor->m3A4[0] = 10;
            actor->mMode = 4;
            actor->mBokoID = (unsigned int)fg_fopAcM_create2(
                0x1CF, 5, gabi::ea(&actor->current.pos), (s8)actor->current.roomNo, 0, 0, -1, 0);
        }
        break;
    case 4: {
        gabi::Local<be<u32>> id;
        *id.get() = actor->mBokoID;
        u32 boko = actor->mBokoID == 0xFFFFFFFFu
                       ? 0
                       : (unsigned int)fg_fopAcIt_Judge(l_judgeById, gabi::ea(id.get()));
        if (!boko)
            break;
        if (!((u32)fgView<fopAc_ac_c>(boko)->actor_status & 0x2000))
            fg_fopAcM_setCarryNow(boko, 0);
        u32 model = (u32)actor->mpKenModel.v, destination = (u32)fgView<FganonBoko>(boko)->mpModel;
        u32 source = model ? gabi::ea(&fgView<J3DModel>(model)->mBaseTransformMtx) : 0;
        if (destination) {
            f32 matrix[12];
            for (u32 i = 0; i < 12; ++i)
                matrix[i] = (f32)fgMatrixFloat(source, i);
            for (u32 i = 0; i < 12; ++i)
                fgView<J3DModel>(destination)->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
        }
        if (actor->m3A4[0] == 0) {
            fopAcM_delete(actor);
            fgView<FganonBoko>(boko)->mFloorFlg = (u8)(1);
            fg_fopAcM_cancelCarryNow(boko);
        }
        break;
    }
    }
}
static void fly_common(fganon_class *actor, bool second) {
    u32 player =
        (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v;
    gabi::Local<dBgS_LinChk> line;

    dBgS_LinChk_ct(line.get(), l_lineCheckVtables, false);
    if (((actor->m384 & 15) == 0 && fg_cM_rndF(1.0f) < 0.5f) || actor->mMode == -10)
        actor->m398 = (s16)fgView<fopAc_ac_c>(player)->shape_angle.y;
    switch ((s16)actor->mMode) {
    case -10: {
        deru_brk(actor);
        for (u32 i = 0; i < 3; ++i)
            fgVectorWord(&actor->current.pos, i) = fgVectorWord(&actor->home.pos, i);
        actor->current.pos.y = (f32)((f32)fgView<fopAc_ac_c>(player)->current.pos.y +
                                     (second ? (f32)l_HIO.m18 : (f32)l_HIO.m10));
        u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
        actor->shape_angle.y = (s16)((short)fg_fopAcM_searchActorAngleY(
            gabi::ea(actor), (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v));
        actor->mMode = 1;
        anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 20.0f, 2, 1.0f, -1);
        actor->m3A4[1] = second ? 40 : 60;
        break;
    }
    case 0:
        anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 20.0f, 2, 1.0f, -1);
        actor->mMode = (s16)actor->mMode + 1;
        actor->m3A0 = 0;
        actor->m3A4[1] = (s16)gabi::ftoi(fg_cM_rndF(50.0f) + 50.0f);
        [[fallthrough]];
    case 1: {
        if (actor->m68A == 1)
            anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 10.0f, 2, 1.0f, -1);
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->m398);
        gabi::Local<cXyz> offset, transformed, temporary;
        offset->x = 0;
        offset->z = (second ? (f32)l_HIO.m14 : (f32)l_HIO.m0C);
        offset->y = (second ? (f32)l_HIO.m18 : (f32)l_HIO.m10);
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_cXyz___pl(gabi::ea(&fgView<fopAc_ac_c>(player)->current.pos), gabi::ea(temporary.get()),
                     gabi::ea(transformed.get()));
        actor->m38C = *temporary.get();
        fg_cXyz___mi(gabi::ea(&actor->m38C), gabi::ea(temporary.get()),
                     gabi::ea(&actor->current.pos));
        *offset.get() = *temporary.get();
        f32 length = fg_sqrtf(fg_PSVECSquareMag(gabi::ea(offset.get())));
        if (length > (f32)l_reg0.f[12] + 300.0f)
            fg_cLib_addCalc2(gabi::ea(&actor->speedF), (f32)l_reg0.f[15] + 30.0f, 1.0f,
                             (f32)l_reg0.f[13] + 2.0f);
        else
            fg_cLib_addCalc0(gabi::ea(&actor->speedF), 1.0f, (f32)l_reg0.f[14] + 1.0f);
        actor->m39C = (f32)l_reg0.f[5] + 1500.0f;
        u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
        s16 yaw = (short)fg_fopAcM_searchActorAngleY(
            gabi::ea(actor), (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
        fg_cLib_addCalcAngleS2(gabi::ea(&actor->shape_angle.y), yaw, 10, 0x400);
        fg_cLib_addCalc2(gabi::ea(&actor->m3BC), (f32)l_reg0.f[11] + 50.0f, 1.0f, 2.0f);
        break;
    }
    }
    pos_move(actor, 0);
    fly_se_set(actor);
    if (actor->m3A4[1] == 0) {
        actor->mAction = second ? ACT_SHOT2 : ACT_SHOT;
        actor->mMode = 0;
    }
    if (actor->m3A4[2] == 0) {
        actor->mAction = ACT_KIERU;
        actor->mMode = 0;
    }
    destroyMovementLine(line.get());
}
static void shot(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    s16 yaw = (short)fg_fopAcM_searchActorAngleY(
        gabi::ea(actor), (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v);
    fg_cLib_addCalcAngleS2(gabi::ea(&actor->shape_angle.y), yaw, 10, 0x400);
    switch ((s16)actor->mMode) {
    case 0:
        if (actor->m408 != 0)
            break;
        anm_init(actor, dRes_INDEX_FGANON_BCK_TAME1_e, 10.0f, 2, 1.0f, -1);
        actor->mMode = (s16)actor->mMode + 1;
        actor->m3A4[0] = 40;
        monsterSound(actor, 0x493A);
        [[fallthrough]];
    case 1:
        if (actor->m3A4[0] == 30)
            actor->m408 = 1;
        if (actor->m3A4[0] < 30)
            sound(actor, 0x5152);
        if (actor->m3A4[0] == 0) {
            anm_init(actor, dRes_INDEX_FGANON_BCK_NAGERU1_e, 3.0f, 0, 1.0f, -1);
            actor->mMode = (s16)actor->mMode + 1;
            monsterSound(actor, 0x493C);
            sound(actor, 0x5954);
        }
        break;
    case 2:
        if (gabi::ftoi((f32)actor->mpMorf->mFrameCtrl.mFrame) == (s32)(s16)l_reg0.s[0] + 14) {
            actor->m688 = 0;
            actor->m687 = 0;
            actor->m689 = 0;
            actor->m409 = 1;
        }
        if (animationStopped(actor)) {
            anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 3.0f, 2, 1.0f, -1);
            actor->mMode = (s16)actor->mMode + 1;
        }
        [[fallthrough]];
    case 3:
        if (actor->m408 == 5) {
            gabi::Local<cXyz> temporary, offset;
            fg_cXyz___mi(gabi::ea(&actor->m3E0), gabi::ea(temporary.get()),
                         gabi::ea(&actor->eyePos));
            *offset.get() = *temporary.get();
            f32 length = fg_sqrtf(fg_PSVECSquareMag(gabi::ea(offset.get())));
            f32 limit =
                gabi::fmadds((f32)actor->m404, (f32)l_reg0.f[0] + 10.0f, 400.0f) + (f32)l_reg0.f[1];
            if (!(length < limit))
                break;
            if (actor->m689 == 0) {
                s32 animation = fg_cM_rndF(1.0f) < 0.5f ? 0x13 : 0x14;
                anm_init(actor, animation, 0.0f, 0, 1.0f, -1);
                actor->mMode = (s16)actor->mMode + 1;
                monsterSound(actor, 0x493E);
                sound(actor, 0x5950);
                actor->m687 = (s8)((s8)actor->m687 + 1);
                if (actor->m688 >= 7 || (actor->m687 >= 4 && fg_cM_rndF(1.0f) < 0.3f))
                    actor->m689 = 1;
            } else if (actor->m2BC == 2) {
                actor->mAction = ACT_KIERU;
                actor->mMode = 0;
                return;
            }
        }
        break;
    case 4: {
        u32 morf = (u32)actor->mpMorf.v;
        s32 frame = gabi::ftoi((f32)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame);
        if (frame >= 5 && frame <= 15)
            actor->m686 = 1;
        if (animationStopped(actor) ||
            (actor->m2BC != 0 && !((f32)fgView<mDoExt_McaMorf>(morf)->mFrameCtrl.mFrame < 20.0f))) {
            anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 10.0f, 2, 1.0f, -1);
            actor->mMode = 3;
        }
        break;
    }
    }
    fg_cLib_addCalc0(gabi::ea(&actor->speedF), 1.0f, (f32)l_reg0.f[14] + 1.0f);
    pos_move(actor, 0);
    fly_se_set(actor);
    if (actor->mMode >= 2 && actor->m408 == 0) {
        actor->mAction = ACT_FLY;
        actor->mMode = 0;
    } else if (actor->mMode == 3 || actor->mMode == 4) {
        s32 reverb = (int)fg_dComIfGp_getReverb((s8)actor->current.roomNo);
        fg_mDoAud_monsSeStartPos(0x6237, gabi::ea(&actor->m3E0), 100, reverb);
    }
}
static void spinattack(fganon_class *actor) {
    u32 player =
        (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v;
    gabi::Local<dBgS_LinChk> line;

    dBgS_LinChk_ct(line.get(), l_lineCheckVtables, false);
    s32 frame = gabi::ftoi((f32)actor->mpMorf->mFrameCtrl.mFrame);
    switch ((s16)actor->mMode) {
    case 0:
        actor->mMode = 1;
        actor->shape_angle.y = (s16)((s16)fgView<fopAc_ac_c>(player)->shape_angle.y);
        [[fallthrough]];
    case 1: {
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->shape_angle.y);
        gabi::Local<cXyz> offset, transformed, temporary;
        offset->x = 0;
        offset->y = (f32)l_HIO.m2C + 100.0f;
        offset->z = (f32)l_HIO.m30;
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_cXyz___pl(gabi::ea(&fgView<fopAc_ac_c>(player)->current.pos), gabi::ea(temporary.get()),
                     gabi::ea(transformed.get()));
        *&actor->current.pos = *temporary.get();
        fg_dBgS_LinChk_Set(gabi::ea(line.get()), gabi::ea(&fgView<fopAc_ac_c>(player)->eyePos),
                           gabi::ea(&actor->current.pos), gabi::ea(actor));
        u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
        if ((int)fg_cBgS_LineCross(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mBgS),
                                   gabi::ea(line.get()))) {
            actor->shape_angle.y = (s16)((s16)gabi::ftoi(fg_cM_rndFX(32768.0f)));
            break;
        }
        actor->current.pos.y = (f32)((f32)actor->current.pos.y - 100.0f);
        anm_init(actor, dRes_INDEX_FGANON_BCK_WAIT1_e, 1.0f, 2, 1.0f, -1);
        actor->mMode = 2;
        actor->m3A4[0] = 30;
        deru_brk(actor);
        [[fallthrough]];
    }
    case 2:
        if (actor->m3A4[0] == 0) {
            anm_init(actor, dRes_INDEX_FGANON_BCK_KAITEN1_e, 2.0f, 0, 1.0f, -1);
            actor->speedF = (f32)(0);
            actor->mMode = 3;
            monsterSound(actor, 0x493C);
        }
        break;
    case 3:
        if (frame == 14)
            sound(actor, 0x5951);
        actor->m685 = 1;
        actor->mWeponSph.mAtp = (u8)(8);
        if (frame == (s16)l_HIO.m3E)
            actor->m6AC = (s16)l_HIO.m40 - (s16)l_HIO.m3E;
        fg_cLib_addCalc2(gabi::ea(&actor->speedF), (u32)(frame - 13) < 12 ? 20.0f : 0.0f, 1.0f,
                         4.0f);
        actor->current.angle.y = (s16)((s16)actor->shape_angle.y);
        actor->current.angle.x = (s16)(0);
        pos_move(actor, 1);
        if (animationStopped(actor)) {
            actor->mAction = ACT_KIERU;
            actor->mMode = 0;
        }
        break;
    }
    destroyMovementLine(line.get());
}
/* 0213B3AC */
s32 move(fganon_class *actor) {
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    if (!((u32)fgView<dComIfG_inf_c>(play)->play.mPlayerStatus[0][0] & 0x10000)) {
        play = (unsigned int)fg_dComIfGp_getGameInfo();
        if (!((u32)fgView<dComIfG_inf_c>(play)->play.mPlayerStatus[0][0] & 0x100000) &&
            actor->mAction != ACT_STANDBY) {
            u32 type = actor->mAction == ACT_SPIN2
                           ? gabi::ea(l_cameraType3)
                           : (actor->m2BC == 1 ? gabi::ea(l_cameraType2) : gabi::ea(l_cameraType1));
            u32 camera = (unsigned int)fg_dCam_getBody();
            fg_dCamera_c_SetTypeForce(camera, type, 0);
        }
    }
    if (actor->m2BC == 0 && actor->mAction < ACT_START) {
        play = (unsigned int)fg_dComIfGp_getGameInfo();
        u32 player = (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v;
        if (actor->mAction == ACT_STANDBY &&
            (f32)fgView<fopAc_ac_c>(player)->current.pos.y < 710.0f)
            return 0;
        if (actor->mAction != ACT_STANDBY && actor->mAction != ACT_KIERU &&
            (f32)fgView<fopAc_ac_c>(player)->current.pos.y < 710.0f) {
            actor->mAction = ACT_KIERU;
            actor->mMode = 0;
        }
    }
    s32 result = 0;
    switch ((s16)actor->mAction) {
    case ACT_STANDBY:
        standby(actor);
        break;
    case ACT_DERU:
        deru(actor);
        break;
    case ACT_KIERU:
        kieru(actor);
        break;
    case ACT_FAIL:
        fail(actor);
        result = 1;
        break;
    case ACT_FLY:
        fly_common(actor, false);
        result = 1;
        break;
    case ACT_SHOT:
        shot(actor);
        result = 1;
        break;
    case ACT_SPIN:
        spinattack(actor);
        result = 1;
        break;
    case ACT_DOWN:
        down(actor);
        result = 1;
        break;
    case ACT_FLY2:
        fly_common(actor, true);
        result = 1;
        break;
    case ACT_SPIN2:
        spinattack2(actor);
        break;
    case ACT_SHOT2:
        shot2(actor);
        result = 1;
        break;
    case ACT_START:
        start(actor);
        break;
    case ACT_END:
        end(actor);
        break;
    case ACT_LAST_END:
        last_end(actor);
        break;
    }
    damage_check(actor);
    actor->m68C = 0;
    if (gabi::ea(&actor->mStts)) {
        for (u32 i = 0; i < 3; ++i)
            fgVectorFloat(&actor->current.pos, i) = (f32)fgVectorFloat(&actor->current.pos, i) +
                                                    (f32)fgVectorFloat(&actor->mStts.m_cc_move, i);
    }
    if (actor->m3B0 > 0.01f) {
        gabi::Local<cXyz> offset, transformed;
        offset->x = 0;
        offset->y = 0;
        offset->z = actor->m3B0;
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->m3B4);
        fg_mDoMtx_XrotM((u32)l_calcMtx, (s16)actor->m3B6);
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_PSVECAdd(gabi::ea(&actor->current.pos), gabi::ea(transformed.get()),
                    gabi::ea(&actor->current.pos));
        fg_cLib_addCalc0(gabi::ea(&actor->m3B0), 1.0f, 7.0f);
    }
    return result;
}

static f32 regFloat(u32 index) {
    return l_reg0.f[index];
}
static s16 regShort(u32 index) {
    return l_reg0.s[index];
}
static f32 angleSine(s32 angle) {
    return cM_ssin(angle);
}
static f32 angleCosine(s32 angle) {
    return cM_scos(angle);
}
static void copyMatrixStorage(u32 source, u32 destination) {
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = (f32)fgMatrixFloat(source, i);
    for (u32 i = 0; i < 12; ++i)
        fgMatrixFloat(destination, i) = matrix[i];
}
static void execute_model(fganon_class *actor) {
    fg_dComIfGp_getGameInfoDiscard();
    gabi::Local<cXyz> zero, offset, temporary, delta, relative;
    zero->x = 0;
    zero->y = 0;
    zero->z = 0;
    if (actor->mB89 >= 2) {
        actor->mB89 = (s8)((s8)actor->mB89 - 1);
        if (actor->mB89 == 1)
            fg_mDoAud_subBgmStart(0x80000047u);
    }
    fg_fpcEx_Search(l_searchMagicSound, gabi::ea(actor));
    u32 env = (unsigned int)fg_dComIfGp_getEnvLight();
    fg_dScnKy_env_light_c_settingTevStruct(env, 0, gabi::ea(&actor->current.pos),
                                           gabi::ea(&actor->tevStr));
    s16 action = actor->mAction;
    if ((action == 5 || action == 7 || (action >= 9 && action <= 11)) && actor->m68B != 0 &&
        actor->m68A == 0) {
        actor->m68B = 0;
        anm_init(actor, dRes_INDEX_FGANON_BCK_WARAU1_e, 6.0f, 2, 1.0f, -1);
        actor->m68A = 60;
        monsterSound(actor, 0x4941);
    }
    actor->m384 = (s16)((s16)actor->m384 + 1);
    for (u32 i = 0; i < 5; ++i)
        if (actor->m3A4[i] != 0)
            actor->m3A4[i] = (s16)((s16)actor->m3A4[i] - 1);
    if (actor->m68A != 0)
        actor->m68A = (s8)((s8)actor->m68A - 1);
    if (actor->m3AE != 0)
        actor->m3AE = (s16)((s16)actor->m3AE - 1);
    if (actor->m3B8 != 0)
        actor->m3B8 = (s16)((s16)actor->m3B8 - 1);
    if ((u8)l_HIO.m05 == 0) {
        actor->attention_info.flags = (u32)(actor->mbIsMaterialized ? 4 : 0);
        if (move(actor) != 0) {
            u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
            fg_dBgS_Acch_CrrPos(gabi::ea(&actor->mAcch),
                                gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mBgS));
        }
        fg_mDoExt_McaMorf_play((u32)actor->mpMorf.v, gabi::ea(&actor->eyePos), 0, 0);
        fg_mDoExt_baseAnm_play((u32)actor->mpBrkAnm1.v);
        fg_mDoExt_baseAnm_play((u32)actor->mpBrkAnm2.v);
    }
    s16 phase = actor->m384;
    s16 adjustment = regShort(6);
    f32 amplitude = actor->m3BC;
    offset->x = angleSine(phase * (adjustment + 700)) * amplitude;
    offset->y = angleSine(phase * (adjustment + 1000)) * amplitude;
    offset->z = angleCosine(phase * (adjustment + 800)) * amplitude;
    fg_cLib_addCalc0(gabi::ea(&actor->m3BC), 1.0f, 1.0f);
    fg_cXyz___pl(gabi::ea(&actor->current.pos), gabi::ea(temporary.get()), gabi::ea(offset.get()));
    fg_cXyz___mi(gabi::ea(&actor->mHdPreviousPos), gabi::ea(delta.get()),
                 gabi::ea(temporary.get()));
    *zero.get() = *delta.get();
    fg_mDoMtx_YrotS((u32)l_calcMtx, (s16) - (s16)actor->shape_angle.y);
    fg_MtxPosition(gabi::ea(zero.get()), gabi::ea(relative.get()));
    actor->mHdNodeY = (s16)((s16)gabi::ftoi((f32)relative->x * ((f32)l_reg18.f[0] - 60.0f)));
    actor->mHdNodeZ = (s16)((s16)gabi::ftoi((f32)relative->z * ((f32)l_reg18.f[1] - 60.0f)));
    u32 model = morfModel(actor);
    f32 scaleX = (f32)actor->scale.x, scaleY = (f32)actor->scale.y, scaleZ = (f32)actor->scale.z;
    fgView<J3DModel>(model)->mBaseScale.y = (f32)(scaleY);
    fgView<J3DModel>(model)->mBaseScale.z = (f32)(scaleZ);
    fgView<J3DModel>(model)->mBaseScale.x = (f32)(scaleX);
    fg_cXyz___pl(gabi::ea(&actor->current.pos), gabi::ea(temporary.get()), gabi::ea(offset.get()));
    *&actor->mHdPreviousPos = *temporary.get();
    fg_PSMTXTrans(l_mtxStack, (f32)temporary->x, (f32)temporary->y, (f32)temporary->z);
    phase = actor->m384;
    f32 wobble = (f32)(s16)actor->m3B8 * (regFloat(14) + 500.0f);
    s16 yaw =
        (s16)((s16)actor->shape_angle.y + (s16)gabi::ftoi(angleSine(phase * 0x2100) * wobble));
    s16 pitchWobble = (s16)gabi::ftoi(angleCosine(phase * 0x2300) * wobble);
    fg_mDoMtx_YrotM(l_mtxStack, yaw);
    fg_mDoMtx_XrotM(l_mtxStack, (s16)((s16)actor->shape_angle.x + pitchWobble));
    fg_mDoMtx_ZrotM(l_mtxStack, (s16)actor->shape_angle.z);
    copyMatrixStorage(l_mtxStack, gabi::ea(&fgView<J3DModel>(model)->mBaseTransformMtx));
    fg_mDoExt_McaMorf_calc((u32)actor->mpMorf.v);
    zero->x = 0;
    zero->y = 0;
    zero->z = 0;
    fg_PSMTXCopy(jointMatrix(model, 27), (u32)l_calcMtx);
    fg_MtxPosition(gabi::ea(zero.get()), gabi::ea(&actor->eyePos));
    f32 eyeX = (f32)actor->eyePos.x, eyeY = (f32)actor->eyePos.y, eyeZ = (f32)actor->eyePos.z;
    actor->attention_info.position.x = (f32)(eyeX);
    actor->attention_info.position.z = (f32)(eyeZ);
    actor->attention_info.position.y = (f32)(eyeY + 30.0f);
    if (actor->m2D0 == 0) {
        fg_PSMTXCopy(jointMatrix(morfModel(actor), 24), (u32)l_calcMtx);
        fg_mDoMtx_YrotM((u32)l_calcMtx, (s16)((s16)l_reg8.s[1] - 400));
        fg_mDoMtx_XrotM((u32)l_calcMtx, (s16)((s16)l_reg8.s[2] - 400));
        fg_mDoMtx_ZrotM((u32)l_calcMtx, (s16)((s16)l_reg8.s[3] + 0x8000));
        fg_MtxTrans(1, (f32)l_reg8.f[9], (f32)l_reg8.f[10], (f32)l_reg8.f[11] + 85.0f);
        fg_MtxTrans(1, regFloat(12), regFloat(13), regFloat(14) + 90.0f);
        fg_mDoMtx_ZrotM((u32)l_calcMtx, (s16)actor->m6A0);
        fg_mDoMtx_XrotM((u32)l_calcMtx, (s16)gabi::ftoi(actor->m694));
        fg_mDoMtx_ZrotM((u32)l_calcMtx, (s16) - (s16)actor->m6A0);
        s16 rotation = 0;
        if (actor->m6A6 != 0) {
            s16 countdown = actor->m6A6;
            f32 displacement = angleSine(countdown * (regShort(2) + 0x1800)) * (f32)countdown;
            rotation = (s16)gabi::ftoi(displacement * (regFloat(14) + 400.0f));
            actor->m6A6 = (s16)(countdown - 1);
        }
        fg_mDoMtx_ZrotM((u32)l_calcMtx, (s16)((s16)actor->m6A2 + rotation));
        fg_MtxTrans(1, -regFloat(12), -regFloat(13), -(regFloat(14) + 90.0f));
        copyMatrixStorage((u32)l_calcMtx, gabi::ea(&actor->mpKenModel->mBaseTransformMtx));
    }
}
static void emitterMatrix(u32 emitter, u32 matrix) {
    if (emitter)
        fg_JPASetRMtxTVecfromMtx(matrix, gabi::ea(&fgView<FganonEmitter>(emitter)->mGlobalRotation),
                                 gabi::ea(&fgView<FganonEmitter>(emitter)->mGlobalTranslation));
}
static f32 vectorLength(cXyz *vector) {
    return fg_sqrtf(fg_PSVECSquareMag(gabi::ea(vector)));
}
static void aimEnergyBall(fganon_class *actor, cXyz *delta, cXyz *offset, bool initial = false) {
    f32 x = delta->x, z = delta->z;
    u32 matrix = (u32)l_calcMtx;
    s16 yaw = (short)fg_cM_atan2s(x, z);
    fg_mDoMtx_YrotS(matrix, yaw);
    matrix = (u32)l_calcMtx;
    f32 horizontal =
        fg_sqrtf(gabi::fmadds((f32)delta->x, (f32)delta->x, (f32)delta->z * (f32)delta->z));
    s16 pitch = (short)fg_cM_atan2s((f32)delta->y, horizontal);
    fg_mDoMtx_XrotM(matrix, (s16)-pitch);
    offset->z = initial ? (actor->m2BC == 0 ? (f32)l_HIO.m1C : (f32)l_HIO.m20) : (f32)actor->m404;
    if (initial)
        actor->m404 = offset->z;
    fg_MtxPosition(gabi::ea(offset), gabi::ea(&actor->m3F8));
}
static bool ball_bg_check(fganon_class *actor) {
    gabi::Local<dBgS_LinChk> line;

    dBgS_LinChk_ct(line.get(), l_lineCheckVtables, false);
    gabi::Local<cXyz> difference, scaled, temporary, end;
    fg_cXyz___mi(gabi::ea(&actor->m3E0), gabi::ea(difference.get()), gabi::ea(&actor->m3EC));
    fg_cXyz___ml(gabi::ea(difference.get()), gabi::ea(scaled.get()), 1.05f);
    fg_cXyz___pl(gabi::ea(&actor->m3E0), gabi::ea(temporary.get()), gabi::ea(scaled.get()));
    *end.get() = *temporary.get();
    fg_dBgS_LinChk_Set(gabi::ea(line.get()), gabi::ea(&actor->m3E0), gabi::ea(end.get()),
                       gabi::ea(actor));
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    bool hit = (int)fg_cBgS_LineCross(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mBgS),
                                      gabi::ea(line.get())) != 0;
    if (hit)
        for (u32 i = 0; i < 3; ++i)
            fgVectorWord(&actor->m3E0, i) = fgVectorWord(&line->mLin.mEnd, i);
    destroyMovementLine(line.get());
    return hit;
}
static void energy_ball_move(fganon_class *actor) {
    u32 player =
        (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v;
    gabi::Local<cXyz> offset, delta, temporary, collisionPosition;
    offset->x = 0;
    offset->y = 0;
    offset->z = 0;
    if (actor->m408 == 0)
        return;

    gabi::Local<FganonAttackInfo> info;
    info->pParticlePos = (u32)(0);
    if (actor->m408 == 35) {
        if (actor->m688 != 0) {
            fg_mDoAud_changeSubBgmStatus(8);
            actor->m688 = 0;
        }
        invalidateEmitter(actor, 2);
        invalidateEmitter(actor, 3);
        actor->m408 = 0;
        return;
    }
    if (actor->m408 == 1) {
        invalidateEmitter(actor, 2);
        invalidateEmitter(actor, 3);
        actor->mEmitters2[0].v = (u32)(createParticle(0x81CE, &actor->current.pos));
        actor->mEmitters2[1].v = (u32)(createParticle(0x81CF, &actor->current.pos));
        actor->m408 = 2;
    }
    if (actor->m408 == 2) {
        fg_PSMTXCopy(jointMatrix(morfModel(actor), 14), (u32)l_calcMtx);
        fg_MtxTrans(1, (f32)l_reg12.f[0] + 30.0f, (f32)l_reg12.f[1] + 30.0f, (f32)l_reg12.f[2]);
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(&actor->m3E0));
        for (u32 i = 0; i < 2; ++i)
            emitterMatrix((u32)actor->mEmitters2[i].v, (u32)l_calcMtx);
        if (actor->m409 != 0) {
            actor->m409 = 0;
            actor->m408 = 3;
            fg_cXyz___mi(gabi::ea(&fgView<fopAc_ac_c>(player)->eyePos), gabi::ea(temporary.get()),
                         gabi::ea(&actor->m3E0));
            *delta.get() = *temporary.get();
            delta->y = (f32)delta->y - (regFloat(18) + 50.0f);
            aimEnergyBall(actor, delta.get(), offset.get(), true);
            actor->m40A = 5;
            fg_dCcD_GObjInf_ClrAtHit(gabi::ea(&actor->mBallAtSph));
        }
    }
    if (actor->m408 >= 3) {
        actor->m3EC = actor->m3E0;
        fg_PSVECAdd(gabi::ea(&actor->m3E0), gabi::ea(&actor->m3F8), gabi::ea(&actor->m3E0));
        fg_PSMTXTrans(l_mtxStack, (f32)actor->m3E0.x, (f32)actor->m3E0.y, (f32)actor->m3E0.z);
        for (u32 i = 0; i < 2; ++i)
            emitterMatrix((u32)actor->mEmitters2[i].v, l_mtxStack);
        bool sword = false, bottle = false, reflected = false;
        fg_cXyz___mi(gabi::ea(&actor->eyePos), gabi::ea(temporary.get()), gabi::ea(&actor->m3E0));
        *delta.get() = *temporary.get();
        delta->y = (f32)delta->y - 50.0f;
        if (vectorLength(delta.get()) < (f32)actor->m404 + 200.0f + regFloat(3)) {
            if (actor->m686 != 0)
                sword = true;
            else if (actor->m408 == 5) {
                if (actor->m2BC != 2)
                    actor->m68C = 1;
                else {
                    if (actor->m40A != 0)
                        return;
                    actor->m40A = 50;
                    actor->mAction = ACT_KIERU;
                    actor->mMode = 0;
                    return;
                }
            }
        }
        if (actor->m408 == 4) {
            u32 vt = (u32)fgView<fopAc_ac_c>(player)->__vtbl;
            if (gabi::call_ptr<s32>((u32)fgView<FganonPlayerVirtuals>(vt)->checkBottleSwing,
                                    player)) {
                fg_cXyz___mi(gabi::ea(&fgView<fopAc_ac_c>(player)->eyePos),
                             gabi::ea(temporary.get()), gabi::ea(&actor->m3E0));
                *delta.get() = *temporary.get();
                delta->y = (f32)delta->y - 30.0f;
                bottle = vectorLength(delta.get()) < regFloat(2) + 100.0f;
            }
        }
        if (((int)fg_dCcD_GObjInf_ChkTgHit(gabi::ea(&actor->mBallTgSph)) || sword || bottle) &&
            actor->m40A == 0) {
            if ((int)fg_dCcD_GObjInf_ChkTgHit(gabi::ea(&actor->mBallTgSph))) {
                info->mpObj =
                    (u32)((unsigned int)fg_dCcD_GObjInf_GetTgHitObj(gabi::ea(&actor->mBallTgSph)));
                fg_at_power_check2(gabi::ea(info.get()));
            } else {
                info->mpObj = (u32)(0);
                info->mResultingAttackType = (u8)(0xFF);
            }
            u32 hit = (u32)info->mpObj;
            bool masterSword = false;
            if (hit && (u8)info->mResultingAttackType == 1 &&
                ((u32)fgView<dCcD_GObjInf>(hit)->mAtType & 2)) {
                u8 equip = (u8)fgView<FganonSaveEquipment>(l_saveInfo)->mSelectEquip;
                masterSword = equip == 0x39 || equip == 0x3E || equip == 0x3A;
            }
            if (masterSword || bottle) {
                fg_cXyz___mi(gabi::ea(&actor->eyePos), gabi::ea(temporary.get()),
                             gabi::ea(&actor->m3E0));
                *delta.get() = *temporary.get();
                delta->y = (f32)delta->y - (regFloat(17) + 30.0f);
                aimEnergyBall(actor, delta.get(), offset.get());
                actor->m408 = 5;
                l_pauseTimer = (u8)(2);
                s32 status = (s8)actor->m688 + 2;
                if (status > 7)
                    status = 7;
                fg_mDoAud_changeSubBgmStatus(status);
                actor->m688 = (s8)((s8)actor->m688 + 1);
                u32 soundPlayer = (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())
                                      ->play.mPlayerInfo[0]
                                      .mpPlayer.v;
                if (soundPlayer && gabi::ea(&fgView<fopAc_ac_c>(soundPlayer)->eyePos)) {
                    s32 reverb = (int)fg_dComIfGp_getReverb(
                        (s8)fgView<fopAc_ac_c>(soundPlayer)->current.roomNo);
                    fg_mDoAud_seStart(0x5954, gabi::ea(&fgView<fopAc_ac_c>(soundPlayer)->eyePos), 0,
                                      reverb);
                }
                fg_dSv_event_c_onEventBit(
                    gabi::ea(&fgView<FganonSaveView>((u32)l_saveInfo)->mSave.mSavedata.mEvent),
                    0x3F20);
                reflected = true;
            } else if (sword) {
                fg_cXyz___mi(gabi::ea(&fgView<fopAc_ac_c>(player)->eyePos),
                             gabi::ea(temporary.get()), gabi::ea(&actor->m3E0));
                *delta.get() = *temporary.get();
                delta->y = (f32)delta->y - (regFloat(18) + 50.0f);
                aimEnergyBall(actor, delta.get(), offset.get());
                actor->m408 = 4;
                reflected = true;
            }
            if (reflected) {
                actor->m404 =
                    (f32)actor->m404 + (actor->m2BC == 0 ? (f32)l_HIO.m24 : (f32)l_HIO.m28);
                gabi::Local<csXyz> angles;
                fg_csXyz_csXyz(gabi::ea(angles.get()), 0, 0, 0);
                angles->y = (short)fg_cM_atan2s((f32)actor->m3F8.x, (f32)actor->m3F8.z);
                createParticle(0x81F0, &actor->m3E0, angles.get());
                s32 reverb = (int)fg_dComIfGp_getReverb((s8)actor->current.roomNo);
                fg_mDoAud_seStart(0x28A5, gabi::ea(&actor->m3E0), 0, reverb);
                fg_PSVECAdd(gabi::ea(&actor->m3E0), gabi::ea(&actor->m3F8), gabi::ea(&actor->m3E0));
                actor->m40A = 5;
            }
        }
        *collisionPosition.get() = actor->m3E0;
        if (actor->m40A != 0) {
            actor->m40A = (s8)((s8)actor->m40A - 1);
            collisionPosition->x = -20000;
            collisionPosition->y = -20000;
            collisionPosition->z = 20000;
        }
        if (actor->m408 == 3) {
            fg_dCcD_Sph_StartCAt(gabi::ea(&actor->mBallAtSph), gabi::ea(collisionPosition.get()));
            actor->m408 = 4;
        } else
            fg_dCcD_Sph_MoveCAt(gabi::ea(&actor->mBallAtSph), gabi::ea(collisionPosition.get()));
        fg_cM3dGSph_SetR(gabi::ea(&actor->mBallAtSph.mSph), regFloat(6) + 25.0f);
        fg_cM3dGSph_SetC(gabi::ea(&actor->mBallTgSph.mSph), gabi::ea(collisionPosition.get()));
        fg_cM3dGSph_SetR(gabi::ea(&actor->mBallTgSph.mSph), regFloat(7) + 100.0f);
        u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
        fg_cCcS_Set(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mCcS),
                    gabi::ea(&actor->mBallTgSph));
        play = (unsigned int)fg_dComIfGp_getGameInfo();
        fg_cCcS_Set(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mCcS),
                    gabi::ea(&actor->mBallAtSph));
        if (actor->m68C != 0 || ball_bg_check(actor) ||
            (int)fg_dCcD_GObjInf_ChkAtHit(gabi::ea(&actor->mBallAtSph))) {
            if (actor->m688 != 0) {
                s32 status =
                    actor->m68C != 0
                        ? 10
                        : ((int)fg_dCcD_GObjInf_ChkAtHit(gabi::ea(&actor->mBallAtSph)) ? 9 : 8);
                fg_mDoAud_changeSubBgmStatus(status);
                actor->m688 = 0;
            }
            actor->m408 = 0;
            u32 emitter = (u32)actor->mEmitters2[0].v;
            if (emitter)
                fgView<FganonEmitter>(emitter)->mGlobalAlpha = (u8)(0);
            invalidateEmitter(actor, 3);
            gabi::Local<csXyz> angles;
            fg_csXyz_csXyz(gabi::ea(angles.get()), 0, 0, 0);
            createParticle(0x81EE, &actor->m3E0, angles.get());
            createParticle(0x81EF, &actor->m3E0, angles.get());
            s32 reverb = (int)fg_dComIfGp_getReverb((s8)actor->current.roomNo);
            fg_mDoAud_seStart(0x6A35, gabi::ea(&actor->m3E0), 100, reverb);
            if ((int)fg_dCcD_GObjInf_ChkAtHit(gabi::ea(&actor->mBallAtSph))) {
                u32 hit = (unsigned int)fg_dCcD_GObjInf_GetAtHitObj(gabi::ea(&actor->mBallAtSph));
                u32 status = hit ? (u32)fgView<dCcD_GObjInf>(hit)->mStts.v : 0;
                u32 source = status ? (u32)fgView<dCcD_Stts>(status)->mp_actor.v : 0;
                info->mpActor = (u32)(source);
                if (source && (s16)fgView<fopAc_ac_c>(source)->mProcName == 0xA8 &&
                    actor->mAction != ACT_LAST_END) {
                    actor->m68B = 1;
                    actor->mAction = ACT_FLY;
                    actor->mMode = 1;
                    actor->m3A4[1] = (s16)gabi::ftoi(fg_cM_rndF(30.0f) + 70.0f);
                }
            }
        }
    }
    fg_cXyz___mi(gabi::ea(&actor->home.pos), gabi::ea(temporary.get()), gabi::ea(&actor->m3E0));
    *delta.get() = *temporary.get();
    if (vectorLength(delta.get()) > 10000.0f)
        actor->m408 = 35;
}
static void execute_collision(fganon_class *actor) {
    gabi::Local<cXyz> zero, center, weaponPosition;
    zero->x = 0;
    zero->y = 0;
    zero->z = 0;
    for (u32 i = 0; i < 2; ++i) {
        u32 slot = i, emitter = (u32)fgEmitterAt(actor, slot).v;
        if (actor->mbIsMaterialized != 0 && actor->mAction != ACT_LAST_END) {
            if (emitter) {
                u32 index = (u32)jno[i];
                u32 matrix = jointMatrix(morfModel(actor), index);
                u32 rotation = (unsigned int)fg_emptyVirtual(
                    gabi::ea(&fgView<FganonEmitter>(emitter)->mGlobalRotation));
                fg_JPASetRMtxTVecfromMtx(
                    matrix, rotation,
                    gabi::ea(&fgView<FganonEmitter>(emitter)->mGlobalTranslation));
            } else {
                u16 id = (u16)eno[i];
                fgEmitterAt(actor, slot).v = createParticle(id, &actor->current.pos);
            }
        } else if (emitter) {
            fgView<FganonEmitter>(emitter)->mMaxFrame = (s32)(-1);
            fg_JPABaseEmitter_setFlags(emitter, 1);
            fgEmitterAt(actor, slot).v = 0;
        }
    }
    if (actor->mbIsMaterialized != 0) {
        fg_PSMTXCopy(jointMatrix(morfModel(actor), 10), (u32)l_calcMtx);
        fg_MtxPosition(gabi::ea(zero.get()), gabi::ea(center.get()));
        center->y = (f32)center->y - (regFloat(0) + 150.0f);
        u32 flags = (u32)actor->mCyl.mObjCo.mSPrm;
        actor->mCyl.mObjCo.mSPrm = (u32)(actor->m68F ? flags & ~1u : flags | 1);
    } else {
        u8 hidden = actor->m68F;
        u32 flags = (u32)actor->mCyl.mObjCo.mSPrm;
        center->x = 20000;
        center->y = 20000;
        actor->mCyl.mObjCo.mSPrm = (u32)(flags & ~1u);
        center->z = -1000.0f * (f32)hidden;
    }
    fg_cM3dGCyl_SetC(gabi::ea(&actor->mCyl.mCyl), gabi::ea(center.get()));
    fg_cM3dGCyl_SetH(gabi::ea(&actor->mCyl.mCyl), (f32)l_reg8.f[3] + 250.0f);
    fg_cM3dGCyl_SetR(gabi::ea(&actor->mCyl.mCyl), (f32)l_reg8.f[4] + 70.0f);
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    fg_cCcS_Set(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mCcS), gabi::ea(&actor->mCyl));
    weaponPosition->x = -10000;
    weaponPosition->y = -10000;
    weaponPosition->z = -10000;
    if (actor->m685 != 0) {
        fg_cM3dGSph_SetR(gabi::ea(&actor->mWeponSph.mSph), regFloat(13) + 60.0f);
        fg_PSMTXCopy(jointMatrix(morfModel(actor), 24), (u32)l_calcMtx);
        zero->x = 0;
        zero->y = 0;
        zero->z = 100;
        fg_MtxPosition(gabi::ea(zero.get()), gabi::ea(weaponPosition.get()));
        s8 active = actor->m684;
        actor->m685 = 0;
        if (active == 0) {
            fg_dCcD_Sph_StartCAt(gabi::ea(&actor->mWeponSph), gabi::ea(weaponPosition.get()));
            actor->m684 = 1;
        } else
            fg_dCcD_Sph_MoveCAt(gabi::ea(&actor->mWeponSph), gabi::ea(weaponPosition.get()));
    } else {
        actor->m684 = 0;
        fg_cM3dGSph_SetC(gabi::ea(&actor->mWeponSph.mSph), gabi::ea(weaponPosition.get()));
    }
    play = (unsigned int)fg_dComIfGp_getGameInfo();
    fg_cCcS_Set(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mCcS), gabi::ea(&actor->mWeponSph));
    u32 capeID = actor->mCapeID;
    actor->m686 = 0;
    u32 cape = (unsigned int)fg_fopAcM_SearchByID(capeID);
    if (cape) {
        fg_PSMTXCopy(jointMatrix(morfModel(actor), (s32)regShort(5) + 20), (u32)l_calcMtx);
        zero->x = regFloat(0) + 35.0f;
        zero->y = regFloat(1);
        zero->z = regFloat(2) - 30.0f;
        fg_MtxPosition(gabi::ea(zero.get()), gabi::ea(&fgView<FganonCape>(cape)->m1BE0));
        fg_PSMTXCopy(jointMatrix(morfModel(actor), (s32)regShort(6) + 11), (u32)l_calcMtx);
        zero->x = regFloat(3) + 35.0f;
        zero->y = regFloat(4);
        zero->z = regFloat(5) + 30.0f;
        fg_MtxPosition(gabi::ea(zero.get()), gabi::ea(&fgView<FganonCape>(cape)->m1BEC));
        for (u32 i = 0; i < 3; ++i)
            fgVectorWord(&fgView<fopAc_ac_c>(cape)->current.pos, i) =
                fgVectorWord(&actor->eyePos, i);
        for (u32 i = 0; i < 3; ++i)
            fgAngleWord(&fgView<fopAc_ac_c>(cape)->current.angle, i) =
                fgAngleWord(&actor->current.angle, i);
        f32 target = actor->mbIsMaterialized != 0 && actor->m68E == 0 ? 1.0f : 0.0f;
        fg_cLib_addCalc2(gabi::ea(&fgView<fopAc_ac_c>(cape)->scale.y), target, 1.0f,
                         regFloat(8) + 0.1f);
    }
    if (actor->m6AC != 0) {
        actor->m6AC = (s8)((s8)actor->m6AC - 1);
        actor->mBtStartFrame = (f32)(0);
        actor->mBtEndFrame = (f32)(10);
        actor->mBtMaxDis = (f32)(10000);
        actor->mBtAttackType = (u8)(1);
        actor->mBtMaxDis = (f32)((f32)l_HIO.m44);
        actor->mBtNowFrame = (f32)(5);
    } else
        actor->mBtNowFrame = (f32)(100);
}
static f32 reg8Float(u32 index) {
    return l_reg8.f[index];
}
static s16 reg8Short(u32 index) {
    return l_reg8.s[index];
}
static void setVector(cXyz *vector, f32 x, f32 y, f32 z) {
    vector->x = x;
    vector->y = y;
    vector->z = z;
}
static void cameraSwitch(fganon_class *actor, u8 index, bool on) {
    u32 save = (u32)l_saveInfo;
    if (on)
        fg_dSv_info_c_onSwitch(gabi::ea(&fgView<FganonSaveView>(save)->mSave), index,
                               (s8)actor->current.roomNo);
    else
        fg_dSv_info_c_offSwitch(gabi::ea(&fgView<FganonSaveView>(save)->mSave), index,
                                (s8)actor->current.roomNo);
}
static void cameraLaugh(fganon_class *actor) {
    s32 reverb = (int)fg_dComIfGp_getReverb((s8)actor->current.roomNo);
    fg_mDoAud_seStart(0x4938, 0, 0, reverb);
}
static void playerPosition(u32 player, cXyz *position, s16 angle) {
    u32 vt = (u32)fgView<fopAc_ac_c>(player)->__vtbl;
    gabi::call_ptr((u32)fgView<FganonPlayerVirtuals>(vt)->setPlayerPosAndAngle, player, position,
                   angle);
}
static void cameraReset(fganon_class *actor, u32 camera) {
    fg_dCamera_c_SetTrimSize(gabi::ea(&fgView<FganonCameraView>(camera)->mBody), 0);
    fg_dCamera_c_Start(gabi::ea(&fgView<FganonCameraView>(camera)->mBody));
    fg_dMeter_mtrShow();
    u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
    fgView<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag =
        (u16)((u16)fgView<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag | 8);
    actor->mB54 = 0;
}
static void demo_camera(fganon_class *actor) {
    u32 player =
        (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v;
    s8 cameraId =
        (s8)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())->play.mPlayerInfo[0].mCameraID;
    u32 camera = (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())
                     ->play.mCameraInfo[(s32)cameraId]
                     .mpCamera;
    gabi::Local<cXyz> offset, transformed, temporary;
    s16 mode = actor->mB54;
    switch (mode) {
    case 1:
    case 50:
    case 100:
        if ((u16)actor->eventInfo.mCommand != 2) {
            fg_fopAcM_orderPotentialEvent(gabi::ea(actor), 2, 0xFFFF, 0);
            actor->eventInfo.mCondition = (u16)((u16)actor->eventInfo.mCondition | 2);
            return;
        }
        actor->mB54 = (s16)(mode + 1);
        fg_dCamera_c_Stop(gabi::ea(&fgView<FganonCameraView>(camera)->mBody));
        fg_dCamera_c_SetTrimSize(gabi::ea(&fgView<FganonCameraView>(camera)->mBody), 2);
        actor->mB56 = 0;
        actor->mB84 = mode == 50 ? 55.0f : 60.0f;
        actor->mB80 = 0;
        if (mode == 1) {
            fgView<FganonPlayerDemo>(player)->mDemoParam = (u32)(0);
            fgView<FganonPlayerDemo>(player)->mDemoMode = (u32)(0x18);
            fgView<FganonPlayerDemo>(player)->mDemoType = (u16)(3);
        } else {
            fgView<FganonPlayerDemo>(player)->mDemoType = (u16)(3);
            fgView<FganonPlayerDemo>(player)->mDemoParam = (u32)(0);
            if (mode == 50)
                actor->shape_angle.y = (s16)(0);
            else {
                u32 mainCamera = (u32)fgView<dComIfG_inf_c>(fg_dComIfGp_getGameInfo())
                                     ->play.mCameraInfo[0]
                                     .mpCamera;
                for (u32 i = 0; i < 3; ++i)
                    fgVectorWord(&actor->mB5C, i) =
                        fgVectorWord(&fgView<FganonCameraView>(mainCamera)->mEye, i);
                for (u32 i = 0; i < 3; ++i)
                    fgVectorWord(&actor->mB68, i) =
                        fgVectorWord(&fgView<FganonCameraView>(mainCamera)->mCenter, i);
                u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
                actor->shape_angle.y = (s16)((short)fg_fopAcM_searchActorAngleY(
                    gabi::ea(actor),
                    (u32)fgView<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v));
            }
        }
        mode = (s16)(mode + 1);
        break;
    }
    switch (mode) {
    case 2:
        setVector(transformed.get(), -300306.0f, 715.0f, -303407.0f);
        playerPosition(player, transformed.get(), -0x7BCD);
        setVector(&actor->mB68, -300319.0f, 812.0f, -303342.0f);
        setVector(&actor->mB5C, -300440.0f, 787.0f, -303137.0f);
        if (actor->mB56 == 2)
            cameraLaugh(actor);
        if (actor->mB56 == 30)
            fgView<FganonPlayerDemo>(player)->mDemoMode = (u32)(0x35);
        if (actor->mB56 == 45) {
            initBrk(&actor->mpBrkAnm1, morfModel(actor), gabi::ea(l_archiveDemo),
                    dRes_INDEX_FGANON_BRK_DERU_MAIN2_e);
            initBrk(&actor->mpBrkAnm2, (u32)actor->mpKenModel.v, gabi::ea(l_archiveDemo),
                    dRes_INDEX_FGANON_BRK_DERU_KEN2_e);
            sound(actor, 0x594E);
            actor->mbIsMaterialized = 1;
        }
        if (actor->mB56 != 47)
            break;
        actor->mB54 = 3;
        actor->mB56 = 0;
        setVector(&actor->current.pos, reg8Float(0) - 300294.0f, reg8Float(1) + 745.0f,
                  reg8Float(2) - 303109.0f);
        setVector(&actor->mB5C, reg8Float(3) - 300169.0f, reg8Float(4) + 770.0f,
                  reg8Float(5) - 303635.0f);
        actor->mB68 = *&actor->current.pos;
        actor->mB68.y = (f32)actor->mB68.y + (regFloat(0) + 160.0f);
        [[fallthrough]];
    case 3:
        if (actor->mB56 > 60) {
            fg_cLib_addCalc2(gabi::ea(&actor->mB5C.x), -300269.0f, 0.1f, 100.0f * (f32)actor->mB80);
            fg_cLib_addCalc2(gabi::ea(&actor->mB5C.y), 870.0f, 0.1f, 100.0f * (f32)actor->mB80);
            fg_cLib_addCalc2(gabi::ea(&actor->mB5C.z), -303335.0f, 0.1f, 300.0f * (f32)actor->mB80);
            actor->mB68.x = (f32)actor->current.pos.x;
            actor->mB68.z = (f32)actor->current.pos.z;
            fg_cLib_addCalc2(gabi::ea(&actor->mB68.y), (f32)actor->current.pos.y + 215.0f, 0.1f,
                             55.0f * (f32)actor->mB80);
            fg_cLib_addCalc2(gabi::ea(&actor->mB80), 0.1f, 1.0f, reg8Float(7) + 0.01f);
            if (actor->mB56 == 110)
                actor->mMode = 2;
            if (actor->mB56 == 180)
                actor->mMode = 4;
            if (actor->mB56 == 280) {
                actor->mAction = ACT_SHOT;
                actor->mMode = 1;
                actor->m3A4[0] = 29;
                actor->mB54 = 150;
                if (actor->m2BF != 0xFF)
                    cameraSwitch(actor, actor->m2BF, true);
            }
        }
        break;
    case 51:
        setVector(&actor->current.pos, -300202.0f, 715.0f, -301859.0f);
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->shape_angle.y);
        setVector(offset.get(), regFloat(0) - 150.0f, regFloat(1) + 20.0f, regFloat(2) + 500.0f);
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_cXyz___pl(gabi::ea(&actor->current.pos), gabi::ea(temporary.get()),
                     gabi::ea(transformed.get()));
        actor->mB5C = *temporary.get();
        setVector(offset.get(), regFloat(3), 0, regFloat(5) + 280.0f);
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_PSVECAdd(gabi::ea(transformed.get()), gabi::ea(&actor->current.pos),
                    gabi::ea(transformed.get()));
        transformed->y = 715;
        playerPosition(player, transformed.get(), -0x8000);
        actor->mB68 = *&actor->eyePos;
        actor->mB68.y = (f32)actor->mB68.y + (regFloat(6) - 40.0f);
        if (actor->mB56 > 150) {
            setVector(&actor->mB68, -299898.0f, 1188.0f, -301158.0f);
            setVector(&actor->mB5C, -299703.0f, 921.0f, -300851.0f);
            actor->mB54 = (s16)((s16)actor->mB54 + 1);
            actor->mB56 = 0;
            fgView<FganonPlayerDemo>(player)->mDemoMode = (u32)(0x1A);
        }
        break;
    case 52:
        setVector(transformed.get(), -300202.0f, (f32)fgView<fopAc_ac_c>(player)->current.pos.y,
                  -301859.0f);
        playerPosition(player, transformed.get(), 0);
        if (actor->mB56 == 10)
            actor->mMode = (s16)((s16)actor->mMode + 1);
        actor->current.pos.x = (f32)((f32)actor->mB68.x - 50.0f + regFloat(4));
        actor->current.pos.y = (f32)((f32)actor->mB68.y + regFloat(5));
        actor->current.pos.z = (f32)((f32)actor->mB68.z - 100.0f + regFloat(6));
        actor->shape_angle.y = (s16)(regShort(0) + 2000);
        if (actor->mB56 == 40) {
            actor->mB54 = (s16)((s16)actor->mB54 + 1);
            actor->mB56 = 0;
        }
        break;
    case 53:
        setVector(&actor->mB68, -300098.0f, 580.0f, -301997.0f);
        setVector(&actor->mB5C, -300274.0f, 929.0f, -301770.0f);
        if (actor->mB56 != 30)
            break;
        setVector(&actor->mB5C, -299703.0f, 921.0f, -300851.0f);
        actor->mB54 = (s16)((s16)actor->mB54 + 1);
        actor->mB56 = 0;
        actor->speed.y = (f32)(0);
        actor->mB68 = *&actor->eyePos;
        actor->mB68.y = (f32)actor->mB68.y - (regFloat(11) + 30.0f);
        actor->mMode = (s16)((s16)actor->mMode + 1);
        [[fallthrough]];
    case 54:
        fg_cLib_addCalc2(gabi::ea(&actor->mB68.y), (f32)actor->eyePos.y - 30.0f + regFloat(11),
                         0.1f, 20.0f);
        if (actor->mB56 != 100)
            break;
        actor->mB54 = 55;
        fgView<FganonPlayerDemo>(player)->mDemoMode = (u32)(0xF);
        actor->mB56 = 0;
        [[fallthrough]];
    case 55:
        if (actor->mB56 == 20)
            cameraLaugh(actor);
        for (u32 i = 0; i < 3; ++i)
            fgVectorWord(&actor->mB68, i) =
                fgVectorWord(&fgView<fopAc_ac_c>(player)->current.pos, i);
        actor->mB68.y = (f32)actor->mB68.y + reg8Float(3);
        setVector(offset.get(), 0, reg8Float(4) + 1500.0f, reg8Float(5) + 2000.0f);
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)((s16)fgView<fopAc_ac_c>(player)->shape_angle.y +
                                              (s16)actor->mB76 + reg8Short(5)));
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        actor->mB76 = (s16)((s16)actor->mB76 + reg8Short(6) + 30);
        fg_cXyz___pl(gabi::ea(&fgView<fopAc_ac_c>(player)->current.pos), gabi::ea(temporary.get()),
                     gabi::ea(transformed.get()));
        actor->mB5C = *temporary.get();
        if (actor->mB56 == 130) {
            if (actor->mSwitchNo != 0xFF)
                cameraSwitch(actor, actor->mSwitchNo, true);
            if (actor->m2BF != 0xFF)
                cameraSwitch(actor, actor->m2BF, false);
            actor->mB54 = 150;
            fopAcM_delete(actor);
            cameraReset(actor, camera);
            return;
        }
        break;
    case 101: {
        fg_cLib_addCalc2(gabi::ea(&actor->mB68.x), (f32)actor->current.pos.x, 0.1f, 200.0f);
        fg_cLib_addCalc2(gabi::ea(&actor->mB68.y), (f32)actor->eyePos.y - 50.0f, 0.1f, 200.0f);
        fg_cLib_addCalc2(gabi::ea(&actor->mB68.z), (f32)actor->current.pos.z, 0.1f, 200.0f);
        fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->shape_angle.y);
        setVector(offset.get(), 0, reg8Float(0) - 100.0f, reg8Float(1) + 400.0f);
        fg_MtxPosition(gabi::ea(offset.get()), gabi::ea(transformed.get()));
        fg_PSVECAdd(gabi::ea(transformed.get()), gabi::ea(&actor->current.pos),
                    gabi::ea(transformed.get()));
        gabi::Local<dBgS_LinChk> line;
        dBgS_LinChk_ct(line.get(), l_lineCheckVtables, false);
        fg_dBgS_LinChk_Set(gabi::ea(line.get()), gabi::ea(&actor->mB68),
                           gabi::ea(transformed.get()), gabi::ea(actor));
        u32 play = (unsigned int)fg_dComIfGp_getGameInfo();
        if ((int)fg_cBgS_LineCross(gabi::ea(&fgView<dComIfG_inf_c>(play)->play.mBgS),
                                   gabi::ea(line.get()))) {
            transformed->x = (f32)line->mLin.mEnd.x;
            transformed->y = (f32)line->mLin.mEnd.y + (reg8Float(18) + 20.0f);
            transformed->z = (f32)line->mLin.mEnd.z;
        }
        destroyMovementLine(line.get());
        fg_cLib_addCalc2(gabi::ea(&actor->mB5C.x), (f32)transformed->x, 0.1f,
                         50.0f * (f32)actor->mB80);
        fg_cLib_addCalc2(gabi::ea(&actor->mB5C.y), (f32)transformed->y, 0.1f,
                         50.0f * (f32)actor->mB80);
        fg_cLib_addCalc2(gabi::ea(&actor->mB5C.z), (f32)transformed->z, 0.1f,
                         50.0f * (f32)actor->mB80);
        fg_cLib_addCalc2(gabi::ea(&actor->mB80), 1.0f, 1.0f, reg8Float(7) + 0.1f);
        break;
    }
    case 103:
        if (actor->mB56 > regShort(2) + 8)
            fg_cLib_addCalc2(gabi::ea(&actor->mB68.y),
                             (f32)fgView<fopAc_ac_c>(player)->current.pos.y + regFloat(9), 0.8f,
                             regFloat(10) + 30.0f);
        if (actor->mB56 <= regShort(3) + 80)
            break;
        actor->mB54 = 150;
        fopAcM_delete(actor);
        if (regShort(3) == 0)
            cameraSwitch(actor, actor->mSwitchNo, true);
        [[fallthrough]];
    case 150:
        cameraReset(actor, camera);
        return;
    }
    if (actor->mB54 != 0) {
        gabi::Local<cXyz> center, eye;
        *center.get() = actor->mB68;
        *eye.get() = actor->mB5C;
        fg_dCamera_c_Set(gabi::ea(&fgView<FganonCameraView>(camera)->mBody), gabi::ea(center.get()),
                         gabi::ea(eye.get()), (f32)actor->mB84, 0);
        fg_JUTReport(410, 430, gabi::ea(l_subCountFormat), (s16)actor->mB56);
        actor->mB56 = (s16)((s16)actor->mB56 + 1);
    }
}
/* 02134B28 */
BOOL daFganon_Execute(fganon_class *actor) {
    execute_model(actor);
    demo_camera(actor);
    energy_ball_move(actor);
    execute_collision(actor);
    return TRUE;
}

/* 02134840 */
void pos_move(fganon_class *actor, u8 direct) {
    gabi::Local<cXyz> delta;
    if (direct == 0) {
        gabi::Local<cXyz> result;
        fg_cXyz___mi(gabi::ea(&actor->m38C), gabi::ea(result.get()), gabi::ea(&actor->current.pos));
        delta->x = result->x;
        delta->y = result->y;
        delta->z = result->z;
        s16 yaw = (short)fg_cM_atan2s((f32)delta->x, (f32)delta->z);
        f32 z = delta->z, x = delta->x;
        f32 horizontal = fg_sqrtf(gabi::fmadds(x, x, z * z));
        s16 pitch = (s16)(-(short)fg_cM_atan2s((f32)delta->y, horizontal));
        f32 max = (f32)actor->m39C * (f32)actor->m3A0;
        s16 scale = (s16)((s16)l_reg0.s[3] + 5);
        fg_cLib_addCalcAngleS2(gabi::ea(&actor->current.angle.y), yaw, scale, (s16)gabi::ftoi(max));
        max = (f32)actor->m39C * (f32)actor->m3A0;
        scale = (s16)((s16)l_reg0.s[3] + 5);
        fg_cLib_addCalcAngleS2(gabi::ea(&actor->current.angle.x), pitch, scale,
                               (s16)gabi::ftoi(max));
    }
    fg_cLib_addCalc2(gabi::ea(&actor->m3A0), 1.0f, 1.0f, 0.05f);
    delta->y = 0.0f;
    delta->x = 0.0f;
    delta->z = (f32)actor->speedF;
    fg_mDoMtx_YrotS((u32)l_calcMtx, (s16)actor->current.angle.y);
    fg_mDoMtx_XrotM((u32)l_calcMtx, (s16)actor->current.angle.x);
    fg_MtxPosition(gabi::ea(delta.get()), gabi::ea(&actor->speed));
    fg_PSVECAdd(gabi::ea(&actor->current.pos), gabi::ea(&actor->speed),
                gabi::ea(&actor->current.pos));
}

/* 021349B8 */
void fly_se_set(fganon_class *actor) {
    gabi::Local<cXyz> delta;
    fg_cXyz___mi(gabi::ea(&actor->current.pos), gabi::ea(delta.get()), gabi::ea(&actor->old.pos));
    f32 magnitude = fg_PSVECSquareMag(gabi::ea(delta.get()));
    f32 distance = fg_sqrtf(magnitude);
    f32 value = distance * 3.5f;
    /* bge is taken on NaN: a NaN volume goes through the high-range path (result 0) */
    u32 volume = !(value < 2147483648.0f) ? (u32)gabi::ftoi(value - 2147483648.0f) + 0x80000000u
                                          : (u32)gabi::ftoi(value);
    if (volume > 100)
        volume = 100;
    sound(actor, 0x303E, volume);
}

/* 02134A90 */
void *mahou_se_set(fopAc_ac_c *actor, void *context) {
    if (!(int)fg_fopAc_IsActor(gabi::ea(actor)) || !actor)
        return nullptr;
    u32 p = gabi::ea(actor);
    if ((s16)actor->mProcName != 0xF2)
        return nullptr;
    if ((s8)actor->health == 0 && gabi::ea(&fgView<fopAc_ac_c>(p)->current.pos)) {
        s32 reverb = (int)fg_dComIfGp_getReverb((s8)actor->current.roomNo);
        fg_mDoAud_seStart(0x6238, gabi::ea(&fgView<fopAc_ac_c>(p)->current.pos), 0, reverb);
    }
    return actor;
}

/* 02137808 */
BOOL daFganon_IsDelete(fganon_class *actor) {
    return TRUE;
}

/* 02137810 */
BOOL daFganon_Delete(fganon_class *actor) {
    if ((u8)actor->mB8A != 0) {
        fg_dComIfG_resDelete(gabi::ea(&actor->mPhs2),
                             gabi::ea(fgView<const char>(gabi::ea(l_archiveSwordDelete))));
        if ((u8)actor->mB8A != 1)
            fg_dComIfG_resDelete(gabi::ea(&actor->mPhs1),
                                 gabi::ea(fgView<const char>(gabi::ea(l_archiveDelete))));
    }
    if ((u8)actor->mB88 != 0) {
        s8 child = (s8)l_HIO.mNo;
        hio_set = (u8)(0);
        fg_mDoHIO_subRoot_c_deleteChild(child);
    }
    fg_JAIZelBasic_seDeleteObject(gabi::ea(&actor->m3E0.x));
    u32 capeId = (u32)actor->mCapeID;
    if (capeId != 0xFFFF) {
        gabi::Local<be<u32>> id;
        *id.get() = capeId;
        void *cape = nullptr;
        if (capeId != 0xFFFFFFFF)
            cape = fgView<void>(fg_fopAcIt_Judge(l_judgeById, gabi::ea(id.get())));
        fg_fopAcM_delete(gabi::ea(cape));
    }
    if ((s8)actor->mB89 != 0)
        fg_mDoAud_subBgmStop();
    for (u32 offset = 2; offset < 6; ++offset) {
        u32 emitter = (u32)fgEmitterAt(actor, offset).v;
        if (emitter) {
            u32 flags = (u32)fgView<FganonEmitter>(emitter)->mStatus;
            fgView<FganonEmitter>(emitter)->mMaxFrame = (s32)(-1);
            fgView<FganonEmitter>(emitter)->mStatus = (u32)(flags | 1);
        }
    }
    for (u32 offset = 0; offset < 2; ++offset) {
        u32 emitter = (u32)fgEmitterAt(actor, offset).v;
        if (emitter) {
            u32 flags = (u32)fgView<FganonEmitter>(emitter)->mStatus;
            fgView<FganonEmitter>(emitter)->mMaxFrame = (s32)(-1);
            fgView<FganonEmitter>(emitter)->mStatus = (u32)(flags | 1);
        }
    }
    return TRUE;
}

/* 02137D1C */
fganon_class *fganon_class_ctor(fganon_class *actor) {
    if (!actor) {
        actor = fgView<fganon_class>(fg_operatorNew(0xDD0));
        if (!actor)
            return actor;
    }
    fg_fopAc_ac_c_fopAc_ac_c(gabi::ea(actor));
    actor->__vtbl = (u32)(0x1000ED84);
    // Three embedded lighting records receive the same typed default members.
    f32 floats[17] = {};
    u8 bytes[4];
    s16 shorts[4];
    for (u32 i = 0; i < 6; ++i)
        floats[i] = (f32)l_lightDefaults.mPositionDirection[i];
    for (u32 i = 0; i < 4; ++i)
        bytes[i] = (u8)l_lightDefaults.mColor[i];
    for (u32 i = 0; i < 4; ++i)
        shorts[i] = (s16)l_lightDefaults.mAngles[i];
    for (u32 i = 9; i < 17; ++i)
        floats[i] = (f32)l_lightDefaults.mFactors[i - 9];
    for (FganonLightDefaults *record :
         {fgView<FganonLightDefaults>(gabi::ea(&actor->mKenTevStr.mLightObj[0])),
          fgView<FganonLightDefaults>(gabi::ea(&actor->mKenTevStr.mHdLightParam[0])),
          fgView<FganonLightDefaults>(gabi::ea(&actor->mKenTevStr.mHdLightParam[0x84]))}) {
        for (u32 i = 0; i < 6; ++i)
            record->mPositionDirection[i] = floats[i];
        for (u32 i = 0; i < 4; ++i)
            record->mColor[i] = bytes[i];
        for (u32 i = 0; i < 4; ++i)
            record->mAngles[i] = shorts[i];
        for (u32 i = 9; i < 17; ++i)
            record->mFactors[i - 9] = floats[i];
    }
    fg_dCcD_Sph_ctor(gabi::ea(&actor->mBallTgSph));
    fg_dCcD_Sph_ctor(gabi::ea(&actor->mBallAtSph));
    fg_dBgS_AcchCir_dBgS_AcchCir(gabi::ea(&actor->mAcchCir));
    fg_dBgS_Acch_dBgS_Acch(gabi::ea(&actor->mAcch));
    actor->mAcch.cBgS_Chk::__vtbl = (u32)(0x1000ED04);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = (u32)(0x1000ED14);
    actor->mAcch.dBgS_PolyPassChk::__vtbl = (u32)(0x1000ED24);
    actor->mAcch.mbObjThrough = (u8)(1);
    fg_cCcD_Stts_ctor(gabi::ea(&actor->mStts));
    fg_dCcD_GStts_dCcD_GStts(gabi::ea(&actor->mStts.__vtbl_gstts));
    actor->mStts.__vtbl = (u32)(0x1004AE88);
    actor->mStts.__vtbl_gstts = (u32)(0x1004AEC0);
    fg_dCcD_GObjInf_dCcD_GObjInf(gabi::ea(&actor->mCyl));
    fgView<FganonCylinderShapeVtables>(gabi::ea(&actor->mCyl))->mShapeVtable = (u32)(0x100015A8);
    fgView<FganonCylinderShapeVtables>(gabi::ea(&actor->mCyl))->mVtable = (u32)(0x1000EC14);
    fg_cM3dGCyl_ctor(gabi::ea(&actor->mCyl.mCyl));
    actor->mCyl.__vtbl_hitinf = (u32)(0x1004B108);
    fgView<FganonCylinderShapeVtables>(gabi::ea(&actor->mCyl))->mShapeVtable = (u32)(0x1004B160);
    fgView<FganonCylinderGeometryVtable>(gabi::ea(&actor->mCyl))->mVtable = (u32)(0x1004B150);
    fg_dCcD_Sph_ctor(gabi::ea(&actor->mWeponSph));
    return actor;
}

static u32 newAnimation(u32 size, bool texture) {
    u32 p = (unsigned int)fg_operatorNew(size);
    if (p)
        p = texture ? fg_mDoExt_btkAnm_ctor(p) : fg_mDoExt_brkAnm_ctor(p);
    return p;
}
static BOOL initializeAnimation(gptr<mDoExt_baseAnm> *storage, u32 modelData, u32 resource,
                                s32 index, bool texture, s32 loop) {
    void *resourceData =
        dComIfG_getObjectRes(fgView<const char>(resource), index, gc_tu.safeString);
    u32 animation = storage->v;
    if (texture)
        return fg_mDoExt_btkAnm_init(animation, modelData, gabi::ea(resourceData), 1, loop, 1.0f, 0,
                                     -1, 0, 0);
    return fg_mDoExt_brkAnm_init(animation, modelData, gabi::ea(resourceData), 1, loop, 1.0f, 0, -1,
                                 0, 0);
}
/* 02137954 */
BOOL useHeapInit(fopAc_ac_c *base) {
    fganon_class *actor = (fganon_class *)base;
    void *modelData = dComIfG_getObjectRes(fgView<const char>(gabi::ea(l_archiveHeap)),
                                           dRes_INDEX_FGANON_BDL_BPG_e, gc_tu.safeString);
    void *animation = dComIfG_getObjectRes(fgView<const char>(gabi::ea(l_archiveHeap)),
                                           dRes_INDEX_FGANON_BCK_WAIT1_e, gc_tu.safeString);
    u32 morf = (unsigned int)fg_mDoExt_McaMorf_mDoExt_McaMorf(
        0, gabi::ea(modelData), 0, 0, gabi::ea(animation), 2, 1.0f, 0, -1, 1, 0, 0, 0x11020203);
    actor->mpMorf.v = (u32)(morf);
    if (!morf || !(u32)fgView<mDoExt_McaMorf>(morf)->mpModel.v)
        return FALSE;
    u32 model = (u32)fgView<mDoExt_McaMorf>(morf)->mpModel.v;
    u32 data = (u32)fgView<J3DModel>(model)->mModelData.v;
    fgView<J3DModel>(model)->mUserArea = gabi::ea(actor);
    u32 joints = (unsigned int)fg_J3DModelData_getJointTree(data);
    u16 count = (u16)fgView<FganonJointTree>(joints)->mJointCount;
    for (u16 i = 0; i < count; i = (u16)(i + 1)) {
        if ((i >= 2 && i <= 4) || (i >= 6 && i <= 7)) {
            data = (u32)fgView<J3DModel>(model)->mModelData.v;
            u32 actualCount = (u32)fgView<J3DModelData>(data)->mJointNum;
            u32 joint = (u32)fgView<J3DModelData>(data)->mJointNodes;
            if (i < actualCount)
                joint += i * 0x1C;
            fgView<J3DJoint>(joint)->mCallBack = gabi::guest_fn((const void *)nodeCallback);
        }
        joints =
            (unsigned int)fg_J3DModelData_getJointTree((u32)fgView<J3DModel>(model)->mModelData.v);
        count = (u16)fgView<FganonJointTree>(joints)->mJointCount;
    }
    u32 brk = newAnimation(0x78, false);
    actor->mpBrkAnm1.v = (u32)(brk);
    if (!brk)
        return FALSE;
    model = morfModel(actor);
    void *resource = dComIfG_getObjectRes(fgView<const char>(gabi::ea(l_archiveHeap)),
                                          dRes_INDEX_FGANON_BRK_DERU_MAIN1_e, gc_tu.safeString);
    if (!(int)fg_mDoExt_brkAnm_initResult((u32)actor->mpBrkAnm1.v,
                                          (u32)fgView<J3DModel>(model)->mModelData.v,
                                          gabi::ea(resource), 1, 0, 1.0f, 0, -1, 0, 0))
        return FALSE;
    resource = dComIfG_getObjectRes(fgView<const char>(gabi::ea(l_archiveSwordHeap)),
                                    dRes_INDEX_PGSW_BDL_BPG_KEN1_e, gc_tu.safeString);
    u32 sword = (unsigned int)fg_mDoExt_J3DModel__create(gabi::ea(resource), 0, 0x11020203);
    actor->mpKenModel.v = (u32)(sword);
    if (!sword)
        return FALSE;
    data = (u32)fgView<J3DModel>(sword)->mModelData.v;
    brk = newAnimation(0x78, false);
    actor->mpBrkAnm2.v = (u32)(brk);
    if (!brk || !initializeAnimation(&actor->mpBrkAnm2, data, gabi::ea(l_archiveHeap),
                                     dRes_INDEX_FGANON_BRK_DERU_KEN1_e, false, 0))
        return FALSE;
    resource = dComIfG_getObjectRes(fgView<const char>(gabi::ea(l_archiveHeap)),
                                    dRes_INDEX_FGANON_BDL_YDKBL00_e, gc_tu.safeString);
    data = gabi::ea(resource);
    model = (unsigned int)fg_mDoExt_J3DModel__create(gabi::ea(resource), 0, 0x11020203);
    actor->mpEnergySphereModel.v = (u32)(model);
    if (!model)
        return FALSE;
    u32 btk = newAnimation(0x74, true);
    actor->mpBtkAnm.v = (u32)(btk);
    if (!btk || !initializeAnimation(&actor->mpBtkAnm, data, gabi::ea(l_archiveHeap),
                                     dRes_INDEX_FGANON_BTK_YDKBL00_e, true, 2))
        return FALSE;
    brk = newAnimation(0x78, false);
    actor->mpBrkAnm3.v = (u32)(brk);
    if (!brk || !initializeAnimation(&actor->mpBrkAnm3, data, gabi::ea(l_archiveHeap),
                                     dRes_INDEX_FGANON_BRK_YDKBL00_e, false, 0))
        return FALSE;
    return TRUE;
}

/* 02137F9C */
cPhs_State daFganon_Create(fopAc_ac_c *base) {
    fganon_class *actor = (fganon_class *)base;
    u32 condition = (u32)actor->actor_condition;
    if (!(condition & 8)) {
        if (actor) {
            fganon_class_ctor(actor);
            condition = (u32)actor->actor_condition;
        }
        actor->actor_condition = (u32)(condition | 8);
    }
    u8 switchNo;
    if ((u8)actor->mInitState == 0) {
        switchNo = (u8)((u32)actor->mParameters >> 16);
        actor->mSwitchNo = (u8)(switchNo);
    } else
        switchNo = (u8)actor->mSwitchNo;
    if (switchNo != 0xFF) {
        u32 save = (u32)l_saveInfo;
        s8 room = (s8)l_stayNo;
        if ((int)fg_dSv_info_c_isSwitch(gabi::ea(&fgView<FganonSaveView>(save)->mSave), switchNo,
                                        room)) {
            if (((u32)actor->mParameters & 15) == 2 &&
                !(int)fg_dSv_event_c_isEventBit(
                    gabi::ea(&fgView<FganonSaveView>((u32)l_saveInfo)->mSave.mSavedata.mEvent),
                    0x3A08))
                fg_fopAcM_create1(0x1CF, 5, gabi::ea(&actor->current.pos),
                                  (s8)actor->current.roomNo, 0, 0, -1, 0);
            return cPhs_ERROR_e;
        }
    }
    actor->mB8A = (u8)(1);
    cPhs_State result = (int)fg_dComIfG_resLoad(
        gabi::ea(&actor->mPhs2), gabi::ea(fgView<const char>(gabi::ea(l_archiveSwordCreate))));
    if (result != cPhs_COMPLEATE_e)
        return result;
    actor->mB8A = (u8)(2);
    result = (int)fg_dComIfG_resLoad(gabi::ea(&actor->mPhs1),
                                     gabi::ea(fgView<const char>(gabi::ea(l_archiveCreate))));
    if (result != cPhs_COMPLEATE_e)
        return result;
    u32 params = (u32)actor->mParameters;
    actor->m2BC = (u8)(params & 15);
    actor->m2BD = (u8)((u8)(params >> 8));
    actor->mSwitchNo = (u8)((u8)(params >> 16));
    actor->m2BF = (u8)((u8)(params >> 24));
    if ((params & 15) == 3)
        actor->m68F = (u8)((u8)(((u32)actor->mParameters >> 4) & 15));
    if (!(int)fg_fopAcM_entrySolidHeap(gabi::ea(actor), l_useHeapInit, 0x96000))
        return cPhs_ERROR_e;
    actor->attention_info.flags = (u32)(4);
    actor->attention_info.distances[2] = (u8)(4);
    if ((u8)hio_set == 0) {
        actor->mB88 = (u8)(1);
        hio_set = (u8)(1);
        u8 variant = (u8)actor->m2BC;
        u32 label = variant == 2   ? gabi::ea(l_hioLabel1)
                    : variant == 1 ? gabi::ea(l_hioLabel2)
                                   : gabi::ea(l_hioLabel3);
        s8 child = (signed char)fg_mDoHIO_subRoot_c_createChild(gabi::ea(fgView<const char>(label)),
                                                                gabi::ea(&l_HIO));
        l_HIO.mNo = (s8)(child);
    }
    fg_dBgS_Acch_Set(gabi::ea(&actor->mAcch), gabi::ea(&actor->current.pos),
                     gabi::ea(&actor->old.pos), gabi::ea(actor), 1, gabi::ea(&actor->mAcchCir),
                     gabi::ea(&actor->speed), 0, 0);
    actor->mAcch.mSameActorChk = (u8)(0);
    fg_dBgS_AcchCir_SetWall(gabi::ea(&actor->mAcchCir), 200.0f, 200.0f);
    u32 status = gabi::ea(&actor->mStts);
    fg_dCcD_Stts_Init(status, 0xFA, 0xFF, gabi::ea(actor));
    fg_dCcD_Cyl_Set(gabi::ea(&actor->mCyl), gabi::ea(l_cylSrc));
    u32 flags = (u32)actor->mCyl.mGObjTg.mSPrm;
    actor->mCyl.mStts.v = (u32)(status);
    actor->mCyl.mGObjTg.mSPrm = (u32)(flags | 4);
    fg_dCcD_Sph_Set(gabi::ea(&actor->mBallTgSph), gabi::ea(l_ballTgSphSrc));
    flags = (u32)actor->mBallTgSph.mGObjTg.mSPrm;
    actor->mBallTgSph.mStts.v = (u32)(status);
    actor->mBallTgSph.mGObjTg.mSPrm = (u32)(flags | 4);
    fg_dCcD_Sph_Set(gabi::ea(&actor->mBallAtSph), gabi::ea(l_ballAtSphSrc));
    actor->mBallAtSph.mStts.v = (u32)(status);
    fg_dCcD_Sph_Set(gabi::ea(&actor->mWeponSph), gabi::ea(l_weponSphSrc));
    u8 copy = (u8)actor->m68F;
    actor->mWeponSph.mStts.v = (u32)(status);
    if (copy == 0) {
        master = (u32)(gabi::ea(actor));
        if ((u8)actor->m2BC == 0) {
            actor->mMode = (s16)(0);
            actor->mAction = ACT_START;
            kieru_brk(actor, 0);
            f32 y = (f32)actor->current.pos.y;
            actor->health = (u8)(30);
            actor->max_health = (u8)(30);
            actor->m690 = (s8)(10);
            actor->current.pos.y = (f32)(y + 10000);
        } else {
            f32 y = (f32)actor->current.pos.y;
            actor->health = (u8)(100);
            actor->max_health = (u8)(100);
            actor->mMode = (s16)(-1);
            actor->mAction = ACT_STANDBY;
            actor->current.pos.y = (f32)(y + 10000);
        }
    } else {
        actor->mMode = (s16)(0);
        actor->mAction = ACT_SPIN2;
        deru_brk(actor);
    }
    f32 gravity = (f32)l_reg8.f[8] + 300;
    s8 room = (s8)actor->current.roomNo;
    actor->mBtBodyR = (f32)(300);
    actor->mBtHeight = (f32)(gravity);
    u32 cape =
        (unsigned int)fg_fopAcM_create2(0xC0, 1, gabi::ea(&actor->current.pos), room, 0, 0, -1, 0);
    actor->mCapeID = (u32)(cape);
    fg_daFganon_Execute_inlines_demo_camera(gabi::ea(actor));
    return result;
}

/* 0213846C */
void *daFganon_HIO_c_ctor(void *h) {
    if (!h) {
        h = fgView<void>(fg_operatorNew(0x4C));
        if (!h)
            return h;
    }
    u32 p = gabi::ea(h);
    // HD places the vtable after the data, so these member offsets differ from GameCube.
    fgView<daFganon_HIO_c>(p)->mNo = (s8)(-1);
    fgView<daFganon_HIO_c>(p)->m05 = (u8)(0);
    fgView<daFganon_HIO_c>(p)->m06 = (u8)(0);
    fgView<daFganon_HIO_c>(p)->m08 = (s16)(60);
    fgView<daFganon_HIO_c>(p)->m0A = (s16)(500);
    fgView<daFganon_HIO_c>(p)->m0C = 1500;
    fgView<daFganon_HIO_c>(p)->m10 = (f32)(400);
    fgView<daFganon_HIO_c>(p)->m14 = (f32)(1500);
    fgView<daFganon_HIO_c>(p)->m18 = (f32)(400);
    fgView<daFganon_HIO_c>(p)->m1C = (f32)(45);
    fgView<daFganon_HIO_c>(p)->m20 = (f32)(45);
    fgView<daFganon_HIO_c>(p)->m24 = (f32)(2);
    fgView<daFganon_HIO_c>(p)->m28 = (f32)(2);
    fgView<daFganon_HIO_c>(p)->m2C = (f32)(30);
    fgView<daFganon_HIO_c>(p)->m30 = (f32)(-270);
    fgView<daFganon_HIO_c>(p)->m34 = (f32)(-600);
    fgView<daFganon_HIO_c>(p)->m38 = (f32)(270);
    fgView<daFganon_HIO_c>(p)->m3C = (s16)(50);
    fgView<daFganon_HIO_c>(p)->m3E = (s16)(1);
    fgView<daFganon_HIO_c>(p)->m40 = (s16)(32);
    fgView<daFganon_HIO_c>(p)->m44 = (f32)(600);
    fgView<daFganon_HIO_c>(p)->m48 = (s16)(150);
    fgView<daFganon_HIO_c>(p)->__vtbl = (u32)(0x1000ED74);
    return h;
}

/* 0213856C */
void staticInitializer() {
    l_headerZeros.values[2] = 0;
    l_headerZeros.values[0] = 0;
    l_headerZeros.values[3] = 0;
    l_headerZeros.values[1] = 0;
    fg_register_global_object(l_zeroRegistry);
    l_headerPi.values[0] = -3.1415927410125732f;
    l_headerPi.values[1] = 3.1415927410125732f;
    fg_cSAngle_ctor(l_angleStatic);
    fg_register_global_object(l_angleRegistry);
    fg_cSGlobe_ctor(l_globeStatic);
    fg_register_global_object(l_globeRegistry);
    daFganon_HIO_c_ctor(fgView<void>(gabi::ea(&l_HIO)));
}

/* 0213860C */
void *findActor(u32 id) {
    if (id == 0xFFFFFFFF)
        return nullptr;
    gabi::Local<be<u32>> local;
    *local.get() = id;
    return fgView<void>(fg_fopAcIt_Judge(l_judgeById, gabi::ea(local.get())));
}

/* 02138648 */
void trivialDestructor(void *p, s32 flags) {
    if (p && (flags & 1))
        fg_dl(gabi::ea(p));
}

/* 0213865C */
void emitterSetFlags(void *p, u32 flags) {
    fgView<FganonEmitter>(gabi::ea(p))->mStatus =
        (u32)fgView<FganonEmitter>(gabi::ea(p))->mStatus | flags;
}

/* 0213866C */
void emptyVirtual(void *p) {}

/* 0213D41C */
void fganon_class_dtor(fganon_class *actor, s32 flags) {
    if (!actor)
        return;
    fg_dCcD_Sph_dtor(gabi::ea(&actor->mWeponSph), 2);
    fg_dCcD_Cyl_dtor(gabi::ea(&actor->mCyl), 2);
    fg_dCcD_Stts_dtor(gabi::ea(&actor->mStts), 2);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = (u32)(0x1000ED14);
    actor->mAcch.dBgS_PolyPassChk::__vtbl = (u32)(0x1000ED24);
    fg_dBgS_Acch_dtor(gabi::ea(&actor->mAcch), 0);
    fg_cM3dGCir_dtor(gabi::ea(&actor->mAcchCir.m_cir), 2);
    fg_dCcD_Sph_dtor(gabi::ea(&actor->mBallAtSph), 2);
    fg_dCcD_Sph_dtor(gabi::ea(&actor->mBallTgSph), 2);
    fg_fopAc_ac_c_dtor(gabi::ea(actor), 0);
    if (flags & 1)
        fg_dl(gabi::ea(actor));
}
/* 0213D4DC */
void actorEmptyVirtual(void *p) {}

// clang-format off
/* WWHD glue
data l_weponSphSrc 101B4CC8
data l_ballTgSphSrc 101B4D08
data l_ballAtSphSrc 101B4D48
data l_cylSrc 101B4D88
data l_archiveAnimation 1000EDD4
data l_archiveAppear 1000EDE0
data l_archiveDisappear 1000EDE8
data l_archiveSwordDisappear 1000EDF0
data l_archiveEnd 1000EBF4
data l_archiveDemo 1000EBEC
data l_archiveSwordDelete 1000EF30
data l_archiveDelete 1000EF38
data l_archiveHeap 1000EF40
data l_archiveSwordHeap 1000EF48
data l_archiveSwordCreate 1000EF50
data l_archiveCreate 1000EF58
data l_hioLabel1 1000EF60
data l_hioLabel2 1000EF74
data l_hioLabel3 1000EF88
data l_cameraType3 1000ED94
data l_cameraType1 1000EDA0
data l_cameraType2 1000EDAC
data l_subCountFormat 1000EDB8
ext 025E83FC void fg_mDoExt_brkAnm_entry(u32 animation,u32 data,f32 frame)
ext 025E7FC4 void fg_mDoExt_btkAnm_entry(u32 animation,u32 data,f32 frame)
ext 025E80D0 u32 fg_mDoExt_brkAnm_ctor(u32 self)
ext 025E7C6C u32 fg_mDoExt_btkAnm_ctor(u32 self)
ext 025E8154 BOOL fg_mDoExt_brkAnm_init(u32 animation,u32 data,u32 resource,u32 enable,s32 loop,f32 speed,s32 start,s32 end,u32 modify,u32 allocator)
ext 025E7CE0 BOOL fg_mDoExt_btkAnm_init(u32 animation,u32 data,u32 resource,u32 enable,s32 loop,f32 speed,s32 start,s32 end,u32 modify,u32 allocator)
ext 025B9E38 void fg_dSv_info_c_onSwitch(u32 self,u32 index,s32 room)
ext 025B9F7C void fg_dSv_info_c_offSwitch(u32 self,u32 index,s32 room)
data l_swordFallOffset 1047B698
data l_stayNo 1047E6C8
data jno 101B4CBC
data eno 101B4CC4
data l_lightDefaults 1016E414
data l_headerZeros 10463D40
data l_headerPi 10463D34
data l_HIO 10463D50
data l_calcMtx 1018C7B0
data l_currentModel 104B462C
data l_saveInfo 101F84DC
data master 10463D30
data hio_set 101B4C98
data l_pauseTimer 101EACB7
data l_reg0 1047B608
data l_reg8 1047BA88
data l_reg12 1047BCC8
data l_reg18 1047C028
ext 02008860 u32 fg_cBgS_LineCross(u32 a0, u32 a1)
ext 02008974 f32 fg_cBgS_GroundCross(u32 a0, u32 a1)
ext 02008B4C void fg_cBgS_LinChk_dtor(u32 a0, u32 a1)
ext 02008DAC void fg_cBgS_Chk_cBgS_Chk(u32 a0, u32 a1)
ext 0200BD2C void fg_cCcD_Stts_ctor(u32 a0)
ext 0200E240 void fg_cCcS_Set(u32 a0, u32 a1)
ext 0200ED84 void fg_cLib_addCalc2(u32 a0, f32 a1, f32 a2, f32 a3)
ext 0200EDC8 void fg_cLib_addCalc0(u32 a0, f32 a1, f32 a2)
ext 0200F428 void fg_cLib_addCalcAngleS2(u32 a0, u32 a1, u32 a2, u32 a3)
ext 0200FAD8 void fg_MtxTrans(u32 a0, f32 a1, f32 a2, f32 a3)
ext 0200FCD8 void fg_MtxPosition(u32 a0, u32 a1)
ext 02018034 void fg_cM3dGCir_dtor(u32 a0, u32 a1)
ext 020182E0 void fg_cM3dGCyl_SetC(u32 a0, u32 a1)
ext 02018428 void fg_cM3dGCyl_SetH(u32 a0, f32 a1)
ext 020184DC void fg_cM3dGCyl_SetR(u32 a0, f32 a1)
ext 02018590 void fg_cM3dGCyl_ctor(u32 a0)
ext 02018C8C void fg_cM3dGSph_SetR(u32 a0, f32 a1)
ext 02018D40 void fg_cM3dGSph_SetC(u32 a0, u32 a1)
ext 020195B0 u32 fg_cM_atan2s(f32 a0, f32 a1)
ext 020198D8 f32 fg_cM_rndF(f32 a0)
ext 02019918 f32 fg_cM_rndFX(f32 a0)
ext 0201A478 void fg_csXyz_csXyz(u32 a0, u32 a1, u32 a2, u32 a3)
ext 0201AD78 void fg_cXyz___pl(u32 a0, u32 a1, u32 a2)
ext 0201ADE0 void fg_cXyz___mi(u32 a0, u32 a1, u32 a2)
ext 0201AE48 void fg_cXyz___ml(u32 a0, u32 a1, f32 a2)
ext 02134B28 void fg_daFganon_Execute_inlines_demo_camera(u32 a0)
ext 0213860C u32 fg_fopAcM_SearchByID(u32 a0)
ext 0213865C void fg_JPABaseEmitter_setFlags(u32 a0, u32 a1)
ext 0213866C u32 fg_emptyVirtual(u32 a0)
ext 024EFD9C void fg_dBgS_Acch_dtor(u32 a0, u32 a1)
ext 024EFE94 void fg_dBgS_AcchCir_dBgS_AcchCir(u32 a0)
ext 024EFF44 void fg_dBgS_AcchCir_SetWall(u32 a0, f32 a1, f32 a2)
ext 024F0474 void fg_dBgS_Acch_dBgS_Acch(u32 a0)
ext 024F06B4 void fg_dBgS_Acch_Set(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8)
ext 024F08A8 void fg_dBgS_Acch_CrrPos(u32 a0, u32 a1)
ext 024F1AFC void fg_dBgS_LinChk_Set(u32 a0, u32 a1, u32 a2, u32 a3)
ext 024F8044 u32 fg_dCam_getBody()
ext 02514EE4 void fg_dCamera_c_SetTypeForce(u32 a0, u32 a1, u32 a2)
ext 02514F2C void fg_dCamera_c_Stop(u32 a0)
ext 02514F38 void fg_dCamera_c_Start(u32 a0)
ext 02514F88 void fg_dCamera_c_Set(u32 a0, u32 a1, u32 a2, f32 a3, u32 a4)
ext 02515280 void fg_dCamera_c_SetTrimSize(u32 a0, u32 a1)
ext 02515860 void fg_dCcD_Stts_dtor(u32 a0, u32 a1)
ext 02515A70 void fg_dCcD_Cyl_dtor(u32 a0, u32 a1)
ext 02515AE8 void fg_dCcD_Sph_dtor(u32 a0, u32 a1)
ext 02515DA0 void fg_dCcD_GStts_dCcD_GStts(u32 a0)
ext 02515E50 void fg_dCcD_GStts_Move(u32 a0)
ext 02515F14 void fg_dCcD_Stts_Init(u32 a0, u32 a1, u32 a2, u32 a3)
ext 02515FB8 void fg_dCcD_GObjInf_dCcD_GObjInf(u32 a0)
ext 02516094 void fg_dCcD_GObjInf_ClrAtHit(u32 a0)
ext 025160DC u32 fg_dCcD_GObjInf_ChkAtHit(u32 a0)
ext 02516178 u32 fg_dCcD_GObjInf_GetAtHitObj(u32 a0)
ext 025162A4 u32 fg_dCcD_GObjInf_ChkTgHit(u32 a0)
ext 02516300 u32 fg_dCcD_GObjInf_GetTgHitObj(u32 a0)
ext 02516518 void fg_dCcD_Cyl_Set(u32 a0, u32 a1)
ext 025166F0 void fg_dCcD_Sph_ctor(u32 a0)
ext 0251677C void fg_dCcD_Sph_Set(u32 a0, u32 a1)
ext 025167C0 void fg_dCcD_Sph_StartCAt(u32 a0, u32 a1)
ext 025167E4 void fg_dCcD_Sph_MoveCAt(u32 a0, u32 a1)
ext 02518DB0 u32 fg_at_power_check1(u32 a0)
ext 02518DB0 void fg_at_power_check2(u32 a0)
ext 025192A8 u32 fg_cc_at_check(u32 a0, u32 a1)
ext 025200D4 u32 fg_dComIfGp_getGameInfo()
ext 025200D4 void fg_dComIfGp_getGameInfoDiscard()
ext 02520460 u32 fg_dComIfG_resLoad(u32 a0, u32 a1)
ext 025204C8 void fg_dComIfG_resDelete(u32 a0, u32 a1)
ext 02520540 u32 fg_dComIfGp_getReverb(u32 a0)
ext 02520C0C u32 fg_dComIfGs_checkGetItem(u32 a0)
ext 02555D0C u32 fg_dComIfGp_getEnvLight()
ext 0255F554 void fg_dKy_SordFlush_set(u32 a0, u32 a1)
ext 025626A4 void fg_dScnKy_env_light_c_settingTevStruct(u32 a0, u32 a1, u32 a2, u32 a3)
ext 02562F5C void fg_setLightTevColorType(u32 a0, u32 a1, u32 a2)
ext 0259169C void fg_dMeter_mtrShow()
ext 025A847C u32 fg_dPa_control_c_set1(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10, u32 a11)
ext 025A847C void fg_dPa_control_c_set2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10, u32 a11)
ext 025B8B68 void fg_dSv_event_c_onEventBit(u32 a0, u32 a1)
ext 025B8B94 u32 fg_dSv_event_c_isEventBit(u32 a0, u32 a1)
ext 025BA0C0 u32 fg_dSv_info_c_isSwitch(u32 a0, u32 a1, u32 a2)
ext 025BED80 void fg_dSnap_RegistFig(u32 a0, u32 a1, f32 a2, f32 a3, f32 a4)
ext 025D4604 u32 fg_fopAc_IsActor(u32 a0)
ext 025D4ED0 void fg_fopAc_ac_c_fopAc_ac_c(u32 a0)
ext 025D50BC void fg_fopAc_ac_c_dtor(u32 a0, u32 a1)
ext 025D5218 u32 fg_fopAcIt_Judge(u32 a0, u32 a1)
ext 025D57E0 void fg_fopAcM_delete(u32 a0)
ext 025D5834 void fg_fopAcM_create1(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7)
ext 025D5834 u32 fg_fopAcM_create2(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7)
ext 025D63E8 u32 fg_fopAcM_entrySolidHeap(u32 a0, u32 a1, u32 a2)
ext 025D6894 u32 fg_fopAcM_searchActorAngleY(u32 a0, u32 a1)
ext 025D7B24 void fg_fopAcM_orderPotentialEvent(u32 a0, u32 a1, u32 a2, u32 a3)
ext 025D9D0C void fg_fopAcM_setCarryNow(u32 a0, u32 a1)
ext 025D9D24 void fg_fopAcM_cancelCarryNow(u32 a0)
ext 025DE508 void fg_fpcEx_Search(u32 a0, u32 a1)
ext 025E1918 void fg_mDoAud_subBgmStart(u32 a0)
ext 025E1928 void fg_mDoAud_subBgmStop()
ext 025E1960 void fg_mDoAud_bgmAllMute(u32 a0)
ext 025E1A40 void fg_mDoAud_seStart(u32 a0, u32 a1, u32 a2, u32 a3)
ext 025E1A7C void fg_mDoAud_monsSeStartPos(u32 a0, u32 a1, u32 a2, u32 a3)
ext 025E1AA4 void fg_mDoAud_monsSeStart(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
ext 025E1B34 void fg_JAIZelBasic_seDeleteObject(u32 a0)
ext 025E1E6C void fg_mDoAud_changeSubBgmStatus(u32 a0)
ext 025E2DE0 void fg_mDoExt_modelUpdateDL(u32 a0, u32 a1)
ext 025E38E0 u32 fg_mDoExt_J3DModel__create(u32 a0, u32 a1, u32 a2)
ext 025E4A98 void fg_mDoExt_McaMorf_setAnm(u32 a0, u32 a1, u32 a2, f32 a3, f32 a4, f32 a5, f32 a6, u32 a7)
ext 025E4F64 u32 fg_mDoExt_McaMorf_mDoExt_McaMorf(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, f32 a6, u32 a7, u32 a8, u32 a9, u32 a10, u32 a11, u32 a12)
ext 025E535C void fg_mDoExt_McaMorf_play(u32 a0, u32 a1, u32 a2, u32 a3)
ext 025E5590 void fg_mDoExt_McaMorf_entryDL(u32 a0)
ext 025E55A0 void fg_mDoExt_McaMorf_calc(u32 a0)
ext 025E742C void fg_mDoExt_baseAnm_play(u32 a0)
ext 025E8154 void fg_mDoExt_brkAnm_initDiscard(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5, u32 a6, u32 a7, u32 a8, u32 a9)
ext 025E8154 u32 fg_mDoExt_brkAnm_initResult(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, f32 a5, u32 a6, u32 a7, u32 a8, u32 a9)
ext 025F0A10 u32 fg_mDoHIO_subRoot_c_createChild(u32 a0, u32 a1)
ext 025F0A18 void fg_mDoHIO_subRoot_c_deleteChild(u32 a0)
ext 025F1884 void fg_mDoMtx_YrotS(u32 a0, u32 a1)
ext 025F1BF4 void fg_mDoMtx_XrotM(u32 a0, u32 a1)
ext 025F1C28 void fg_mDoMtx_YrotM(u32 a0, u32 a1)
ext 025F1C5C void fg_mDoMtx_ZrotM(u32 a0, u32 a1)
ext 0273AD10 u32 fg_operatorNew(u32 a0)
ext 0273AF40 void fg_dl(u32 a0)
ext 027EC9E8 void fg_JUTReport(u32 a0, u32 a1, u32 a2, u32 a3)
ext 027F3F94 u32 fg_J3DModelData_getJointTree(u32 a0)
ext 027F596C void fg_J3DModel_setFlags(u32 a0, u32 a1)
ext 027F7878 u32 fg_J3DJoint_getJointInfo(u32 a0)
ext 028249B0 void fg_JPASetRMtxTVecfromMtx(u32 a0, u32 a1, u32 a2)
ext 028E8D88 void fg_PSVECAdd(u32 a0, u32 a1, u32 a2)
ext 028E8DD0 f32 fg_PSVECSquareMag(u32 a0)
ext 028E90D4 void fg_PSMTXCopy(u32 a0, u32 a1)
ext 028E93CC void fg_PSMTXTrans(u32 a0, f32 a1, f32 a2, f32 a3)
ext 028EAB2C void fg_cSGlobe_ctor(u32 a0)
ext 028ED6F8 void fg_cSAngle_ctor(u32 a0)
ext 028F026C void fg_register_global_object(u32 a0)
ext 028F4384 f32 fg_sqrtf(f32 a0)
*/
// clang-format on
