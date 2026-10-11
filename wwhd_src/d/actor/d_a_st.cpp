/* Stalfos. GameCube names and layout adapted to the verified WWHD implementation.
 * All 29 entry points retain their guest ABI and explicit outgoing linkage slots. */
#include "d/actor/d_a_st.h"
namespace Stalfos {
using namespace StalfosHD;

static const char stArcAnimation[3] = {83, 116, 0};           // St
static const char stArcHeadAnimation[3] = {83, 116, 0};       // St
static const char stArcDelete[3] = {83, 116, 0};              // St
static const char stArcHeap[3] = {83, 116, 0};                // St
static const char stArcCreate[3] = {83, 116, 0};              // St
static const char stItemTableName[5] = {83, 116, 97, 108, 0}; // Stal
static const char stHioName[5] = {131, 88, 131, 94, 0};       // \203X\203^

static StStaticState l_staticState;
static StRuntimeRegistration stRegisterPi;
static StRuntimeRegistration stRegisterByteGuard;
static StRuntimeRegistration stRegisterWordGuard;
static StSphereSource head_sph_src = {
    0, 0, 0, {0, 0, 0}, 0x0, 0xff1dfeff,         0x3,  0x75, 0, 0, 0, 0, 0x0, 0,
    1, 0, 0, 0x2,       0x0, {0.0f, 0.0f, 0.0f}, 40.0f};
static StSphereSource body_sph_src = {
    0, 0, 0, {0, 0, 0}, 0x0, 0xff01beff,         0x3,  0x75, 0, 0, 0, 0, 0x0, 0,
    0, 0, 0, 0x2,       0x0, {0.0f, 0.0f, 0.0f}, 40.0f};
static StSphereSource wepon_sph_src = {
    0, 16777216, 2,   {0, 0, 0},          0xd,  0x0, 0x0, 0x0, 0, 0, 1, 0, 0x0, 0, 0, 0,
    0, 0x2,      0x0, {0.0f, 0.0f, 0.0f}, 60.0f};

static HIO_c l_HIO;
static u8 hio_set;
static u8 another_hit;
static StPartModelIndices st_part_bdl = {{0,
                                          dRes_INDEX_ST_BDL_ST_HARA_e,
                                          dRes_INDEX_ST_BDL_ST_MUNE_e,
                                          dRes_INDEX_ST_BDL_ST_KATAL_e,
                                          dRes_INDEX_ST_BDL_ST_UDEL_e,
                                          dRes_INDEX_ST_BDL_ST_HANDL_e,
                                          dRes_INDEX_ST_BDL_ST_YUBI1L_e,
                                          dRes_INDEX_ST_BDL_ST_YUBI2L_e,
                                          dRes_INDEX_ST_BDL_ST_KATAR_e,
                                          dRes_INDEX_ST_BDL_ST_UDER_e,
                                          dRes_INDEX_ST_BDL_ST_HANDR_e,
                                          dRes_INDEX_ST_BDL_ST_BUKI_e,
                                          dRes_INDEX_ST_BDL_ST_YUBI1R_e,
                                          dRes_INDEX_ST_BDL_ST_YUBI2R_e,
                                          dRes_INDEX_ST_BDL_ST_KUBI_e,
                                          dRes_INDEX_ST_BDL_ST_HEAD_e,
                                          dRes_INDEX_ST_BDL_ST_AGO_e,
                                          dRes_INDEX_ST_BDL_ST_HAT_e,
                                          0,
                                          dRes_INDEX_ST_BDL_ST_KOTUBAN_e,
                                          dRes_INDEX_ST_BDL_ST_MOMOL_e,
                                          dRes_INDEX_ST_BDL_ST_SUNEL_e,
                                          dRes_INDEX_ST_BDL_ST_ASIL_e,
                                          dRes_INDEX_ST_BDL_ST_MOMOR_e,
                                          dRes_INDEX_ST_BDL_ST_SUNER_e,
                                          dRes_INDEX_ST_BDL_ST_ASIR_e}};
static StCollisionJoints cc_j_no = {{ST_JNT_HEAD_e, ST_JNT_MUNE_e, ST_JNT_HARA_e, ST_JNT_UDEL_e,
                                     ST_JNT_UDER_e, ST_JNT_SUNEL_e, ST_JNT_SUNER_e}};

/* inlined into smoke_set_s, buki_smoke_set, fight, part_posmove */
bool line_cross(gabi::Local<LineCheckLocal> &local, cXyz *start, cXyz *end, st_class *i_this) {
    dBgS_LinChk_Set(&local->check, start, end, i_this);
    u32 play = stGetGameInfoWithLinkage<u32>();
    return cBgS_LineCross<s32>(at<StGameInfoView>(play)->mBgS, &local->check) != 0;
}

/* inlined into ground_wait, kan_wait */
bool ambush_trigger(st_class *i_this) {
    if ((u8)i_this->m02B9 != 0xFF)
        return dSv_info_c_isSwitch<s32>(at<void>((u32)stSaveContext() + 0x20),
                                        (s32)(u8)i_this->m02B9 - 1,
                                        (s8)i_this->current.roomNo) != 0;
    f32 range =
        i_this->mAmbushSightRange == 0xFF ? 500.0f : (f32)(u8)i_this->mAmbushSightRange * 10.0f;
    return player_distance(i_this) < range;
}

/* inlined into copyVectorWords, monster_sound, ke_control, nun_control, ue_move, St_move,
 * damage_check, part_posmove, part_move, daSt_Delete */
u32 word(u32 p) { return *at<be<u32>>(p); }

/* inlined into anm_init, head_anm_init, createHeap */
u32 resource(gabi::Local<ResourceLocal> &local, const void *name, u32 id) {
    local->text = gabi::ea(name);
    local->vtable = 0x1003D4C4;
    return dRes_control_c_getRes<u32>(at<void>((u32)stResourceControl()), &local->text, id);
}

/* inlined into smoke_set_s, jyunkai, buki_smoke_set, fight, ue_move, sita_move, part_move, cc_set
 */
// HD: word copies retain NaN payloads and the original load-before-store order.
void copyVectorWords(u32 destination, u32 source) {
    u32 x = word(source), y = word(source + 4), z = word(source + 8);
    *at<be<u32>>(destination) = x;
    *at<be<u32>>(destination + 4) = y;
    *at<be<u32>>(destination + 8) = z;
}

/* inlined into player_angle, player_distance, jyunkai, fight, ue_move, sita_move, damage_check */
u32 player_pointer() {
    u32 play = stGetGameInfoWithLinkage<u32>();
    return at<StGameInfoView>(play)->mpPlayer.v.get();
}

/* inlined into ue_move */
s16 player_angle(st_class *i_this) {
    return fopAcM_searchActorAngleY<s16>(i_this, at<void>(player_pointer()));
}

/* inlined into ambush_trigger, jyunkai, fight, ue_move, sita_move */
f64 player_distance(st_class *i_this) {
    return fopAcM_searchActorDistance<f64>(i_this, at<void>(player_pointer()));
}

/* inlined into fight, fight2, ground_wait, kan_wait, ue_move, sita_move, part_move */
bool animation_stopped(st_class *i_this) {
    u32 animation = gabi::ea((void *)i_this->mpMorf);
    return (stMorf(animation).mState & 1) || stMorf(animation).mRate == 0.0f;
}

/* inlined into scatter_part, jyunkai, fight, fight2, ground_wait, part_move */
f64 random_positive(f32 range) { return cM_rndF<f64>(range); }

/* inlined into scatter_part, damage_check, part_move */
f64 random_signed(f32 range) { return cM_rndFX<f64>(range); }

/* inlined into fight, part_move */
f32 angle_sine(s32 angle) { return stSinCosTable()[(u16)angle >> 3].mSin; }

/* inlined into part_move */
f32 angle_cosine(s32 angle) { return stSinCosTable()[(u16)angle >> 3].mCos; }

/* inlined into part_move */
f64 magnitude(cXyz *vector) {
    f64 squared = PSVECSquareMag<f64>(vector);
    return sqrtHD<f64>(squared);
}

/* inlined into joint_matrix, part_move */
u32 animation_matrix(u32 animation, s32 joint) {
    u32 model = stMorf(animation).mpModel.v.get(), data = stModel(model).mpNodeData.v.get();
    u16 flags = stNodeData(data).mFlags;
    u32 matrices = stNodeData(data).mpAnmMtx.v.get();
    stNodeData(data).mFlags = flags | 0x10;
    return matrices + (u32)joint * 0x30;
}

/* inlined into buki_smoke_set, fight, fight2, St_move, part_move, cc_set */
void joint_matrix(st_class *i_this, s32 joint) {
    u32 matrix = animation_matrix(gabi::ea((void *)i_this->mpMorf), joint);
    PSMTXCopy(at<void>(matrix), stCalcMatrix().get());
}

/* inlined into part_move, eff_move, daSt_Execute */
u32 particle(u32 id, cXyz *position) {
    u32 play = stGetGameInfoWithLinkage<u32>();
    return dPa_control_c_set<u32>(at<void>(at<StGameInfoView>(play)->mpParticle.v.get()), 0, id,
                                  position, (void *)nullptr, (void *)nullptr, 0xFF, (void *)nullptr,
                                  -1, (void *)nullptr, (void *)nullptr, (void *)nullptr);
}

/* inlined into smoke_set_s, fight2 */
u32 particle_with_angle(u32 id, cXyz *position, csXyz *angle) {
    u32 play = stGetGameInfoWithLinkage<u32>();
    return dPa_control_c_set<u32>(at<void>(at<StGameInfoView>(play)->mpParticle.v.get()), 0, id,
                                  position, angle, (void *)nullptr, 0xFF, (void *)nullptr, -1,
                                  (void *)nullptr, (void *)nullptr, (void *)nullptr);
}

/* inlined into smoke_set_s, buki_smoke_set, ground_wait, St_move */
u32 toon_particle(st_class *i_this, u32 id, cXyz *position, csXyz *angle, void *callback) {
    s32 room = (s8)i_this->current.roomNo;
    u32 play = stGetGameInfoWithLinkage<u32>();
    return dPa_control_c_set<u32>(at<void>(at<StGameInfoView>(play)->mpParticle.v.get()), 2, id,
                                  position, angle, (void *)nullptr, 0xB9, callback, room,
                                  (void *)nullptr, (void *)nullptr, (void *)nullptr);
}

/* inlined into head_effect, nun_control, st_break_wait, head_damage, part_move */
void sound_at(st_class *i_this, u32 sound, cXyz *position) {
    if (i_this && gabi::ea(position)) {
        s32 reverb = stGetReverbWithLinkage<s32>((s8)i_this->current.roomNo);
        mDoAud_seStart(sound, position, 0, reverb);
    }
}

/* inlined into jyunkai, fight, fight2, ground_wait, damage_check */
void sound_at_level(st_class *i_this, u32 sound, u32 level) {
    if (!i_this || !gabi::ea(&i_this->eyePos))
        return;
    s32 reverb = stGetReverbWithLinkage<s32>((s8)i_this->current.roomNo);
    mDoAud_seStart(sound, &i_this->eyePos, level, reverb);
}

/* inlined into jyunkai, fight, fight2, damage_check */
void monster_sound(st_class *i_this, u32 sound, u32 level) {
    if (!i_this || !gabi::ea(&i_this->eyePos))
        return;
    u32 id = word(gabi::ea(i_this) + 4);
    s32 reverb = stGetReverbWithLinkage<s32>((s8)i_this->current.roomNo);
    mDoAud_monsSeStart(sound, &i_this->eyePos, id, level, reverb);
}

/* inlined into smoke_set_s, buki_smoke_set, fight, part_posmove */
void initialize_line(gabi::Local<LineCheckLocal> &local) {
    u32 check = gabi::ea(&local->check);
    cBgS_LinChk_ct(at<void>(check));
    for (u32 i = 0; i < 7; ++i)
        (&local->check.mbObjThrough)[i] = 0;
    local->check.dBgS_GrpPassChk::__vtbl = 0x1003D59C;
    local->check.cBgS_Chk::__vtbl = 0x1003D57C;
    local->check.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(&local->check));
    local->check.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(&local->check));
    local->check.mGrp = 1;
    local->check.cBgS_PolyInfo::__vtbl = 0x1003D58C;
    local->check.dBgS_PolyPassChk::__vtbl = 0x1003D5AC;
}

/* inlined into smoke_set_s, buki_smoke_set, fight, part_posmove */
void finish_line(gabi::Local<LineCheckLocal> &local) {
    u32 check = gabi::ea(&local->check);
    local->check.dBgS_PolyPassChk::__vtbl = 0x1003D5AC;
    local->check.dBgS_GrpPassChk::__vtbl = 0x1003D4FC;
    local->check.cBgS_PolyInfo::__vtbl = 0x1003D4EC;
    cBgS_LinChk_destroy(at<void>(check), 0);
}

/* inlined into head_damage */
void head_effect(st_class *i_this, gabi::Local<VectorLocal> &zero, gabi::Local<VectorLocal> &effect,
                 gabi::Local<AngleLocal> &angle) {
    u32 model = stMorf(gabi::ea((void *)i_this->mpMorf)).mpModel.v.get();
    u32 matrixData = stModel(model).mpNodeData.v.get();
    u16 flags = *at<be<u16>>(matrixData, 4);
    u32 matrix = at<StModelNodeData>(matrixData)->mpAnmMtx.v.get();
    *at<be<u16>>(matrixData, 4) = flags | 0x10;
    PSMTXCopy(at<void>(matrix + 0x2D0), stCalcMatrix().get());
    MtxPosition(&zero->value, &effect->value);
    u32 play = stGetGameInfoWithLinkage<u32>();
    dPa_control_c_set(at<void>(at<StGameInfoView>(play)->mpParticle.v.get()), 0, 0x817A,
                      &effect->value, &angle->value, (void *)nullptr, 0xFF, (void *)nullptr, -1,
                      (void *)nullptr, (void *)nullptr, (void *)nullptr);
    sound_at(i_this, 0x5890, &i_this->eyePos);
}

/* inlined into damage_check, part_move, daSt_Execute */
u32 find_actor(u32 id) {
    gabi::Local<IDLocal> local;
    local->id = id;
    return id == 0xFFFFFFFF ? 0 : fopAcIt_Judge<u32>(at<void>(0x025E1234), &local->id);
}

/* inlined into part_move */
void scatter_part(st_p *part) {
    part->m05 = 1;
    part->mPartVelocity.x = random_signed(20.0f);
    part->mPartVelocity.y = (f32)(random_positive(20.0f) + 40.0f);
    part->mPartVelocity.z = random_signed(20.0f);
}

/* inlined into nun_control, part_move, daSt_Execute */
void copy_model_matrix(u32 model, u32 matrix) {
    f32 values[12];
    for (u32 i = 0; i < 12; ++i)
        values[i] = *at<be<f32>>(matrix, i * 4);
    for (u32 i = 0; i < 12; ++i)
        *at<be<f32>>(model, 0xC8 + i * 4) = values[i];
}

/* inlined into cc_set */
void collision_register(u32 sphere) {
    u32 play = stGetGameInfoWithLinkage<u32>();
    cCcS_Set(at<StGameInfoView>(play)->mCcS, at<void>(sphere));
}

/* inlined into daSt_Execute */
void move_visible_parts(st_class *i_this) {
    for (u32 joint = 0; joint < 26; ++joint)
        if (i_this->mParts[joint].mpPartModel && (!i_this->m1DC8 || (joint != 3 && joint != 4)))
            part_move(i_this, joint);
    if (i_this->mActionState != ST_ACTION_LOWER_BODY && i_this->m02C4 != 35)
        ke_control(i_this);
    if (i_this->m1DC8)
        nun_control(i_this);
}

/* inlined into spin_smoke_move */
void smoke_set_s(st_class *i_this, f32 rate) {
    gabi::Local<LineCheckLocal> line;
    initialize_line(line);
    gabi::Local<VectorLocal> start, end;
    copyVectorWords(gabi::ea(&start->value), gabi::ea(&i_this->m1F6C));
    copyVectorWords(gabi::ea(&end->value), gabi::ea(&i_this->m1F6C));
    start->value.y = (f32)start->value.y + 100.0f;
    end->value.y = (f32)end->value.y - 100.0f;
    u32 attribute = 0;
    if (line_cross(line, &start->value, &end->value, i_this)) {
        u32 check = gabi::ea(&line->check);
        i_this->m1F6C.y = line->check.mLin.mEnd.y + 30.0f;
        u32 play = stGetGameInfoWithLinkage<u32>();
        attribute =
            dBgS_GetAttributeCode<u32>(at<StGameInfoView>(play)->mBgS, at<void>(check + 0x14));
    } else
        i_this->m1F6C.y = (f32)i_this->m1F6C.y - 20000.0f;
    if (!i_this->m1F83 || attribute == 4) {
        ++i_this->m1F83;
        if (attribute < 4 || attribute == 11) {
            dPa_smokeEcallBack_end(&i_this->m1E88[2]);
            u32 emitter =
                toon_particle(i_this, 0x2022, &i_this->m1F6C, &i_this->m1F78, &i_this->m1E88[2]);
            if (emitter) {
                stEmitter(emitter).mRate = rate;
                stEmitter(emitter).mSpread = 1.0f;
                stEmitter(emitter).mGlobalDynamicsScale.x = 0.8f;
                stEmitter(emitter).mGlobalDynamicsScale.y = 0.8f;
                stEmitter(emitter).mGlobalDynamicsScale.z = 0.8f;
                f32 scale = REG0_F(11) + 2.0f;
                stEmitter(emitter).mGlobalParticleScale.x = scale;
                stEmitter(emitter).mGlobalParticleScale.y = scale;
                stEmitter(emitter).mGlobalParticleScale.z = scale;
            }
        } else if (attribute == 4) {
            u32 emitter = particle_with_angle(0x24, &i_this->m1F6C, &i_this->m1F78);
            if (emitter) {
                stEmitter(emitter).mMaxFrame = 3;
                stEmitter(emitter).mRate = rate * 0.5f;
            }
        }
    }
    finish_line(line);
}

/* inlined into daSt_Execute */
void spin_smoke_move(st_class *i_this) {
    if (!i_this->m1F80)
        return;
    --i_this->m1F80;
    if (i_this->m1F80 >= 10) {
        gabi::Local<VectorLocal> direction;
        direction->value.x = 0.0f;
        direction->value.y = 0.0f;
        i_this->m1F78.x = 0;
        i_this->m1F78.z = 0;
        MtxTrans(0, (f32)i_this->current.pos.x, (f32)i_this->current.pos.y + 7.5f,
                 (f32)i_this->current.pos.z);
        direction->value.z = (REG0_F(2) + 1.0f) * -175.0f;
        mDoMtx_YrotM(stCalcMatrix().get(), (s16)i_this->m1F7E);
        MtxPosition(&direction->value, &i_this->m1F6C);
        i_this->m1F78.y = i_this->m1F7E;
        smoke_set_s(i_this, REG0_F(3) + 3.0f);
        if (i_this->m1F82 == 2)
            i_this->m1F7E += (s16)gabi::ftoi((REG0_F(11) + 5000.0f) * (f32)i_this->m0308);
        else {
            s16 step = REG0_S(7) + 2000;
            i_this->m1F7E += (i_this->m1F82 == 0 ? step : -step);
        }
    } else
        i_this->m1F6C.y = (f32)i_this->current.pos.y + 20000.0f;
    if (!i_this->m1F80) {
        dPa_smokeEcallBack_end(&i_this->m1E88[2]);
        i_this->m1F83 = 0;
    }
}

/* 02491208 */
void anm_init(st_class *i_this, s32 animation, f32 morph, u32 loop, f32 speed, s32 sound) {
    gabi::Local<ResourceLocal> animationName, soundName;
    u32 animationData = resource(animationName, stArcAnimation, animation);
    u32 soundData = sound >= 0 ? resource(soundName, stArcAnimation, sound) : 0;
    mDoExt_McaMorf_setAnm((void *)i_this->mpMorf, at<void>(animationData), loop, morph, speed, 0.0f,
                          -1.0f, at<void>(soundData));
}

/* 02491330 */
void head_anm_init(st_class *i_this, s32 animation, f32 morph, u32 loop, f32 speed, s32 sound) {
    gabi::Local<ResourceLocal> animationName, soundName;
    u32 animationData = resource(animationName, stArcHeadAnimation, animation);
    u32 soundData = sound >= 0 ? resource(soundName, stArcHeadAnimation, sound) : 0;
    mDoExt_McaMorf_setAnm((void *)i_this->mpMorf2, at<void>(animationData), loop, morph, speed,
                          0.0f, -1.0f, at<void>(soundData));
}

// HD: chain control and movement share this entry point.
/* 02491458 */
void ke_control(st_class *i_this) {
    gabi::Local<VectorLocal> direction, bias, rotated;
    for (u32 chainIndex = 0; chainIndex < 3; ++chainIndex) {
        auto *chain = &i_this->mKe[chainIndex];
        f32 floor = -1000000000.0f;
        if (i_this->m1DD8) {
            gabi::Local<GroundCheckLocal> ground;
            u32 check = gabi::ea(&ground->check);
            cBgS_GndChk_cBgS_GndChk(at<void>(check));
            for (u32 i = 0; i < 7; ++i)
                (&ground->check.mbObjThrough)[i] = 0;
            ground->check.dBgS_GrpPassChk::__vtbl = 0x1003D52C;
            ground->check.dBgS_PolyPassChk::__vtbl = 0x1003D53C;
            ground->check.mGrp = 1;
            ground->check.cBgS_PolyInfo::__vtbl = 0x1003D51C;
            ground->check.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(&ground->check));
            ground->check.cBgS_Chk::__vtbl = 0x1003D50C;
            ground->check.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(&ground->check));
            ground->check.m_pos.z = chain->m000[1].z;
            ground->check.m_pos.x = chain->m000[1].x;
            ground->check.m_pos.y = (f32)chain->m000[1].y + 50.0f;
            u32 play = stGetGameInfoWithLinkage<u32>();
            f64 height = cBgS_GroundCross<f64>(at<StGameInfoView>(play)->mBgS, at<void>(check));
            floor = (f32)(height + 3.0f);
            f32 currentY = chain->m000[1].y;
            if (floor - currentY > 50.0f)
                floor = currentY;
            ground->check.cBgS_PolyInfo::__vtbl = 0x1003D51C;
            ground->check.dBgS_PolyPassChk::__vtbl = 0x1003D53C;
            ground->check.dBgS_GrpPassChk::__vtbl = 0x1003D4FC;
            cBgS_Chk_destroy_cBgS_Chk(at<void>(check), 0);
        }
        mDoMtx_YrotS(stCalcMatrix().get(), (s16)chain->m0F0.y);
        mDoMtx_XrotM(stCalcMatrix().get(), (s16)chain->m0F0.x);
        direction->value.x = 0.0f;
        direction->value.y = 30.0f;
        direction->value.z = -20.0f;
        MtxPosition(&direction->value, &bias->value);
        direction->value.x = 0.0f;
        direction->value.y = 0.0f;
        direction->value.z = 17.5f;
        f32 damping = gabi::fmadds((f32)chainIndex, 0.001f, 0.75f);
        for (u32 i = 1; i < 10; ++i) {
            f32 bx = 0.0f, by = 0.0f, bz = 0.0f;
            if (i <= 5) {
                f32 weight = (f32)(5 - (s32)i);
                bz = ((f32)bias->value.z * weight) * 0.1f;
                by = ((f32)bias->value.y * weight) * 0.1f;
                bx = ((f32)bias->value.x * weight) * 0.1f;
            }
            auto &position = chain->m000[i];
            auto &previous = chain->m000[i - 1];
            auto &velocity = chain->m078[i];
            f32 x = (((f32)position.x - (f32)previous.x) + (f32)velocity.x) + bx;
            f32 y = (((f32)position.y + (f32)velocity.y) + by) - 10.0f;
            y = y + REG0_F(2);
            f32 z = (((f32)position.z - (f32)previous.z) + (f32)velocity.z) + bz;
            if (y < floor)
                y = floor;
            y = y - (f32)previous.y;
            s16 angleX = (s16)-cM_atan2s<s32>(y, z);
            f32 squared = gabi::fmadds(y, y, z * z);
            f64 length = sqrtHD<f64>(squared);
            s32 angleY = cM_atan2s<s32>(x, length);
            mDoMtx_XrotS(stCalcMatrix().get(), angleX);
            mDoMtx_YrotM(stCalcMatrix().get(), angleY);
            MtxPosition(&direction->value, &rotated->value);
            // Keep the interleaved raw-word copy order of the actor's chain update.
            for (u32 component = 0; component < 3; ++component)
                *at<be<u32>>(gabi::ea(&velocity), component * 4) =
                    word(gabi::ea(&position) + component * 4);
            f32 newX = (f32)previous.x + (f32)rotated->value.x;
            position.x = newX;
            position.y = (f32)previous.y + (f32)rotated->value.y;
            position.z = (f32)previous.z + (f32)rotated->value.z;
            velocity.x = (newX - (f32)velocity.x) * damping;
            velocity.y = ((f32)position.y - (f32)velocity.y) * damping;
            velocity.z = ((f32)position.z - (f32)velocity.z) * damping;
        }
        u32 lineArray = i_this->mLineMat.mpLine.v.get();
        u32 output = at<StLinePoints>(lineArray)[chainIndex].mpPoints.v.get();
        for (u32 point = 0; point < 10; ++point)
            for (u32 component = 0; component < 3; ++component)
                *at<be<u32>>(output + point * 12, component * 4) =
                    word(gabi::ea(&chain->m000[point]) + component * 4);
    }
}

// HD: weapon-chain control and movement share this entry point.
/* 0249196C */
void nun_control(st_class *i_this) {
    gabi::Local<VectorLocal> direction, bias, rotated;
    auto *chain = &i_this->mNun;
    u32 modelData =
        stModel(stMorf(gabi::ea((void *)i_this->mpMorf)).mpModel.v.get()).mpNodeData.v.get();
    u16 flags = stNodeData(modelData).mFlags;
    u32 matrices = stNodeData(modelData).mpAnmMtx.v.get();
    stNodeData(modelData).mFlags = flags | 0x10;
    PSMTXCopy(at<void>(matrices + 0x210), stCalcMatrix().get());
    f32 tx = REG8_F(5) + 10.0f, ty = REG8_F(6), tz = REG8_F(7);
    MtxTrans(1, tx, ty, tz);
    mDoMtx_YrotM(stCalcMatrix().get(), (s16)REG0_S(5));
    mDoMtx_ZrotM(stCalcMatrix().get(), (s16)REG0_S(6));
    f32 horizontal = i_this->mEnemyIce.mScaleXZ, vertical = i_this->mEnemyIce.mScaleY;
    MtxScale(1, horizontal, vertical, horizontal);
    u32 matrix = stCalcMatrix().v.get(), model = gabi::ea((void *)i_this->mParts[4].mpPartModel);
    copy_model_matrix(model, matrix);
    tx = REG8_F(8) + 60.0f;
    ty = REG8_F(9);
    tz = REG8_F(10);
    MtxTrans(1, tx, ty, tz);
    mDoMtx_YrotM(stCalcMatrix().get(), (s16)REG8_S(7));
    mDoMtx_ZrotM(stCalcMatrix().get(), (s16)REG8_S(8));
    direction->value.z = REG0_F(12);
    direction->value.y = REG0_F(11);
    direction->value.x = REG0_F(10);
    MtxPosition(&direction->value, &chain->m00[1]);
    if (i_this->m1DC8 == 2) {
        bias->value.x = 0.0f;
        bias->value.y = 0.0f;
        bias->value.z = 0.0f;
        s16 angle = (s16)((s16)i_this->shape_angle.y - 8000 + REG8_S(3));
        mDoMtx_YrotS(stCalcMatrix().get(), angle);
        mDoMtx_XrotM(stCalcMatrix().get(), (s16)REG8_S(4));
        s32 count = i_this->m02E8;
        angle = (s16)((u32)count * (u32)(REG8_S(5) + 0x1800));
        mDoMtx_ZrotM(stCalcMatrix().get(), angle);
        direction->value.y = 0.0f;
        direction->value.z = 0.0f;
        direction->value.x = REG8_F(1) + 200.0f;
        MtxPosition(&direction->value, &bias->value);
        if (!(i_this->m02E8 & 7))
            sound_at(i_this, 0x5898, &i_this->eyePos);
    } else {
        bias->value.x = 0.0f;
        bias->value.y = 0.0f;
        bias->value.z = 0.0f;
    }
    f32 damping = REG6_F(2) + 1.3f;
    direction->value.x = 0.0f;
    direction->value.y = 0.0f;
    direction->value.z = REG6_F(4) + 30.0f;
    s8 state = i_this->m1DC8;
    if (state == 2)
        damping = 0.3f;
    else if (state == 3)
        damping = REG8_F(14) + 2.5f;
    auto &position = chain->m00[2];
    auto &previous = chain->m00[1];
    auto &velocity = chain->m38[2];
    f32 y = (((f32)position.y - (f32)previous.y) + (f32)velocity.y) - 20.0f;
    y = y + (f32)bias->value.y;
    f32 z = (((f32)position.z - (f32)previous.z) + (f32)velocity.z) + (f32)bias->value.z;
    f32 x = (((f32)position.x - (f32)previous.x) + (f32)velocity.x) + (f32)bias->value.x;
    s16 angleX = (s16)-cM_atan2s<s32>(y, z);
    f64 length = sqrtHD<f64>(gabi::fmadds(y, y, z * z));
    s32 angleY = cM_atan2s<s32>(x, length);
    mDoMtx_XrotS(stCalcMatrix().get(), angleX);
    mDoMtx_YrotM(stCalcMatrix().get(), angleY);
    MtxPosition(&direction->value, &rotated->value);
    chain->m24[1].y = angleY;
    chain->m24[1].x = angleX;
    MtxPosition(&direction->value, &rotated->value);
    for (u32 i = 0; i < 3; ++i)
        *at<be<u32>>(gabi::ea(&velocity), i * 4) = word(gabi::ea(&position) + i * 4);
    f32 newX = (f32)previous.x + (f32)rotated->value.x;
    position.x = newX;
    position.y = (f32)previous.y + (f32)rotated->value.y;
    position.z = (f32)previous.z + (f32)rotated->value.z;
    velocity.x = (newX - (f32)velocity.x) * damping;
    velocity.y = ((f32)position.y - (f32)velocity.y) * damping;
    velocity.z = ((f32)position.z - (f32)velocity.z) * damping;
    MtxTrans(0, (f32)chain->m00[1].x, (f32)chain->m00[1].y, (f32)chain->m00[1].z);
    mDoMtx_XrotM(stCalcMatrix().get(), (s16)chain->m24[1].x);
    mDoMtx_YrotM(stCalcMatrix().get(), (s16)chain->m24[1].y);
    s16 angle = (s16)(REG8_S(7) - 0x4000);
    mDoMtx_YrotM(stCalcMatrix().get(), angle);
    tx = REG6_F(10) + 80.0f;
    ty = REG6_F(11);
    tz = REG6_F(12);
    MtxTrans(1, tx, ty, tz);
    mDoMtx_YrotM(stCalcMatrix().get(), (s16)-0x8000);
    horizontal = i_this->mEnemyIce.mScaleXZ;
    vertical = i_this->mEnemyIce.mScaleY;
    MtxScale(1, horizontal, vertical, horizontal);
    matrix = stCalcMatrix().v.get();
    model = gabi::ea((void *)i_this->mParts[3].mpPartModel);
    copy_model_matrix(model, matrix);
}

/* 02491F34 */
BOOL daSt_Draw(st_class *i_this) {
    if (i_this->m02B9 || i_this->mCountdownToDeath || i_this->m02FA)
        return 1;
    dSnap_RegistFig(0xC3, i_this, 1.0f, 1.0f, 1.0f);
    for (u32 i = 0; i < 26; ++i) {
        if (i_this->mParts[i].mPartState >= 10)
            continue;
        if (i_this->m1DC8 && (i == 5 || i == 6 || i == 7))
            continue;
        if (i == 11)
            continue;
        u32 model = gabi::ea((void *)i_this->mParts[i].mpPartModel);
        if (!model)
            continue;
        if ((i == 15 || i == 16 || i == 17) && i_this->m1DD8 == 2) {
            if (i != 15)
                continue;
            model = stMorf(gabi::ea((void *)i_this->mpMorf2)).mpModel.v.get();
            u32 environment = dKy_getEnvlight<u32>();
            dScnKy_env_light_c_setLightTevColorType(at<void>(environment), at<void>(model),
                                                    &i_this->tevStr);
            mDoExt_McaMorf_entryDL((void *)i_this->mpMorf2);
        } else {
            u32 environment = dKy_getEnvlight<u32>();
            dScnKy_env_light_c_setLightTevColorType(at<void>(environment), at<void>(model),
                                                    &i_this->tevStr);
            mDoExt_modelUpdateDL(at<void>(model), 0);
        }
    }
    if (i_this->mActionState == ST_ACTION_LOWER_BODY || i_this->m02C4 == 0x23 ||
        i_this->mEnemyIce.mLightShrinkTimer || i_this->mTimers[2])
        return 1;
    mDoExt_3DlineMat0_c_update(&i_this->mLineMat, 10, 1.2f, at<void>(0x101D0E74), 2,
                               &i_this->tevStr);
    u32 play = stGetGameInfoWithLinkage<u32>();
    u32 packet = callVirtualWithLinkage<u32>(i_this->mLineMat.mpVtable.get()[5], &i_this->mLineMat);
    mDoExt_3DlineMatSortPacket_setMat(&at<StGameInfoView>(play)->mSortPacket[packet],
                                      &i_this->mLineMat);
    return 1;
}

/* 0249210C */
void wait_set(st_class *i_this) {
    if (i_this->m1DC8) {
        anm_init(i_this, dRes_INDEX_ST_BCK_HYUNHYUN_e, 10.0f, 2, 1.0f, -1);
        i_this->m1DC8 = 2;
        if (i_this && gabi::ea(i_this) + 0x37C) {
            s8 room = i_this->current.roomNo;
            u32 id = i_this->mBsPcId;
            s32 reverb = stGetReverbWithLinkage<s32>(room);
            mDoAud_monsSeStart(0x4882, &i_this->eyePos, id, 0, reverb);
        }
    } else {
        anm_init(i_this, i_this->m02CC ? 0x21 : 0x33, 10.0f, 2, 1.0f, -1);
    }
}

/* 024961B8 */
void speed_pos_calc(st_class *i_this) {
    gabi::Local<VectorLocal> direction, rotated;
    if ((i_this->mAcch.m_flags & 0x20) && i_this->m02E4) {
        f32 x = i_this->mPlayerPos.x - i_this->current.pos.x;
        f32 z = i_this->mPlayerPos.z - i_this->current.pos.z;
        s16 angle = cM_atan2s<s16>(x, z);
        s16 step = i_this->m02E4;
        cLib_addCalcAngleS2(&i_this->current.angle.y, angle, 2, step);
        i_this->m02E4 = 0;
    }
    s16 angle = (s16)((s16)i_this->current.angle.y + i_this->m0302);
    mDoMtx_YrotS(stCalcMatrix().get(), angle);
    direction->value.x = 0.0f;
    direction->value.y = 0.0f;
    direction->value.z = i_this->speedF;
    MtxPosition(&direction->value, &rotated->value);
    f32 target = i_this->m02DC, step = i_this->m02E0;
    cLib_addCalc2(&i_this->speedF, target, 1.0f, step);
    i_this->speed.x = rotated->value.x;
    f32 extra = i_this->m1DE0;
    i_this->speed.z = rotated->value.z;
    i_this->m02E0 = 1.0f;
    // PPC ble skips the branch for unordered comparisons.
    if (extra > 0.1f) {
        s16 extraAngle = i_this->m1DDE;
        mDoMtx_YrotS(stCalcMatrix().get(), extraAngle);
        direction->value.x = 0.0f;
        direction->value.y = 0.0f;
        direction->value.z = i_this->m1DE0;
        MtxPosition(&direction->value, &rotated->value);
        PSVECAdd(&i_this->speed, &rotated->value, &i_this->speed);
    }
    f32 decay = REG0_F(9) + 2.5f;
    cLib_addCalc0(&i_this->m1DE0, 1.0f, decay);
    f32 x = i_this->current.pos.x, vertical = i_this->speed.y, dx = i_this->speed.x;
    f32 falling = vertical - 5.0f, y = i_this->current.pos.y;
    i_this->current.pos.x = x + dx;
    i_this->current.pos.y = y + vertical;
    if (falling < -50.0f)
        falling = -50.0f;
    f32 z = i_this->current.pos.z, dz = i_this->speed.z;
    i_this->speed.y = falling;
    i_this->current.pos.z = z + dz;
    u32 play = stGetGameInfoWithLinkage<u32>();
    dBgS_Acch_CrrPos(&i_this->mAcch, at<StGameInfoView>(play)->mBgS);
}

/* inlined into St_move */
void jyunkai(st_class *i_this) {
    u32 player = player_pointer();
    i_this->attention_info.flags = 4;
    i_this->actor_status = (u32)i_this->actor_status | 0x20;
    i_this->m02DC = 0.0f;
    i_this->m02E0 = 5.0f;
    switch ((s16)i_this->m02C4) {
    case 0: {
        i_this->m1F84 = 2;
        s32 frame = gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame);
        s16 start1 = l_HIO.m06, start2 = l_HIO.m08;
        if ((frame >= start1 && frame <= l_HIO.m0A) || (frame >= start2 && frame <= l_HIO.m0C)) {
            i_this->m02DC = l_HIO.m10;
            i_this->m02E4 = 0x800;
            if (frame == start1 || frame == start2)
                sound_at_level(i_this, 0x3832, 0);
        }
        copyVectorWords(gabi::ea(&i_this->mPlayerPos), gabi::ea(&stActor(player).current.pos));
        if (!i_this->mTimers[0]) {
            wait_set(i_this);
            i_this->m02C4 = 1;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(60.0f) + 100.0f);
        }
        break;
    }
    case 1:
        if (!i_this->mTimers[0]) {
            f32 speed = REG8_F(0) + 1.0f;
            if (i_this->m1DC8) {
                anm_init(i_this, dRes_INDEX_ST_BCK_HYUNHYUN_e, 10.0f, 2, 1.0f, -1);
                i_this->m1DC8 = 2;
                monster_sound(i_this, 0x4882, 0);
            } else
                anm_init(i_this, i_this->m02CC ? 0x22 : 0x34, 10.0f, 2, speed, -1);
            i_this->m02C4 = 0;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(60.0f) + 100.0f);
            i_this->m1F84 = 1;
        }
        break;
    }
    u32 play = stGetGameInfoWithLinkage<u32>();
    if (!*at<be<u8>>(play, 0x5292) && !i_this->mTimers[1] && player_distance(i_this) < 280.0f) {
        if (i_this->m1DD0 == 10) {
            i_this->mActionState = ST_ACTION_FIGHT;
            i_this->m02C4 = 0;
            i_this->m0308 = 0.0f;
            if (random_positive(1.0f) < 0.5f)
                i_this->mFightBehavior = i_this->m02CC ? 5 : 0;
            else
                i_this->mFightBehavior = i_this->m02CC ? 15 : 10;
        } else if (!i_this->m1DD0) {
            i_this->mActionState = ST_ACTION_FIGHT_UNARMED;
            i_this->m02C4 = i_this->m1DC8 ? 3 : 0;
        }
    }
}

/* 02492200 */
void attack_set(st_class *i_this, s32 animation, f32 morph, u32 loop) {
    anm_init(i_this, animation, morph, loop, 1.0f, -1);
}

/* inlined into fight */
void buki_smoke_set(st_class *i_this) {
    gabi::Local<VectorLocal> direction, center, start, end;
    dPa_smokeEcallBack_end(&i_this->m1E88[1]);
    joint_matrix(i_this, ST_JNT_BUKI_e);
    direction->value.x = REG0_F(13) + 110.0f;
    direction->value.y = 0.0f;
    direction->value.z = 0.0f;
    MtxPosition(&direction->value, &center->value);
    center->value.y = (i_this->mAcch.m_ground_h + 30.0f) + REG0_F(8);
    u32 emitter = toon_particle(i_this, 0x2027, &center->value, nullptr, &i_this->m1E88[1]);
    if (emitter) {
        stEmitter(emitter).mMaxFrame = 1;
        stEmitter(emitter).mRate = 20.0f;
        stEmitter(emitter).mAwayFromCenterSpeed = 20.0f;
        stEmitter(emitter).mEmitterScale.x = 1.0f;
        stEmitter(emitter).mEmitterScale.y = 0.0f;
        stEmitter(emitter).mEmitterScale.z = 1.0f;
    }
    gabi::Local<LineCheckLocal> line;
    initialize_line(line);
    copyVectorWords(gabi::ea(&start->value), gabi::ea(&center->value));
    copyVectorWords(gabi::ea(&end->value), gabi::ea(&center->value));
    start->value.y = (f32)start->value.y + 100.0f;
    end->value.y = (f32)end->value.y - 200.0f;
    if (line_cross(line, &start->value, &end->value, i_this)) {
        u32 play = stGetGameInfoWithLinkage<u32>();
        s32 material = dBgS_GetMtrlSndId<s32>(at<StGameInfoView>(play)->mBgS,
                                              at<void>(gabi::ea(&line->check) + 0x14));
        s32 reverb = stGetReverbWithLinkage<s32>((s8)i_this->current.roomNo);
        mDoAud_seStart(0x2855, &i_this->current.pos, material, reverb);
    }
    finish_line(line);
}

/* inlined into St_move */
void fight(st_class *i_this) {
    f64 distance;
    f32 spinTarget;
    f32 spinStep;
    gabi::Local<VectorLocal> local;
    auto &local_d8 = local->value;

    u32 player = player_pointer();
    spinTarget = 0.0f;
    spinStep = 0.02f;
    distance = player_distance(i_this);
    copyVectorWords(gabi::ea(&i_this->mPlayerPos), gabi::ea(&stActor(player).current.pos));
    i_this->m02E4 = 0x200;
    switch (i_this->m02C4) {
    case 0:
        i_this->m02DC = 0.0f;
        break;
    }
    i_this->m0ED1 = 0;
    switch (i_this->mFightBehavior) {
    case 0:
        attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKR1_e, 5.0f, 0);
        i_this->mFightBehavior++;
        break;
    case 1:
        if (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 10) {
            sound_at_level(i_this, 0x5893, 0);
            monster_sound(i_this, 0x487F, 0);
        }
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKR1_END_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 2:
        if (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 6) {
            sound_at_level(i_this, 0x5894, 0);
            i_this->m1F80 = (REG0_S(5)) + 0x17;
            i_this->m1F82 = 0;
            i_this->m1F7E = (i_this->current.angle.y + 0x4ee0) + (REG0_S(6));
        }
        i_this->m0ED1 = 1;
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_OTOTOR_e, 1.0f, 2);
            i_this->mFightBehavior++;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
        }
        break;
    case 3:
        i_this->m02DC = (REG8_F(3)) + -2.0f;
        i_this->m0302 =
            gabi::ftoi(((REG0_F(13)) + 10000.0f) * angle_sine(i_this->m02E8 * ((REG0_S(8)) + 500)));
        if (i_this->mTimers[0] == 0) {
            if ((random_positive(1.0f) < 0.5f) || (distance < 300.0f)) {
                if (random_positive(1.0f) < 0.5f) {
                    attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKREND_e, 5.0f, 0);
                    i_this->m02CC = 1;
                    i_this->mFightBehavior = 0x65;
                } else {
                    attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKR2_e, 5.0f, 0);
                    i_this->m02CC = 0;
                    i_this->mFightBehavior = 100;
                }
            } else {
                i_this->mFightBehavior = 0x19;
            }
        }
        break;
    case 5:
        attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKL1_e, 5.0f, 0);
        i_this->mFightBehavior++;
        break;
    case 6:
        if (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 15) {
            sound_at_level(i_this, 0x5893, 0);
            monster_sound(i_this, 0x487F, 0);
        }
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKL1_END_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 7:
        if (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 8) {
            sound_at_level(i_this, 0x5894, 0);
            i_this->m1F80 = (REG0_S(5)) + 0x17;
            i_this->m1F82 = 1;
            i_this->m1F7E = (i_this->current.angle.y + 0x10000 + (REG0_S(6))) - 0x72a0;
        }
        i_this->m0ED1 = 1;
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_OTOTOL_e, 1.0f, 2);
            i_this->mFightBehavior++;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
        }
        break;
    case 8:
        i_this->m02DC = (REG8_F(3)) + -2.0f;
        i_this->m0302 =
            gabi::ftoi(((REG0_F(13)) + 10000.0f) * angle_sine(i_this->m02E8 * ((REG0_S(8)) + 500)));
        if (i_this->mTimers[0] == 0) {
            if ((random_positive(1.0f) < 0.5f) || (distance < 300.0f)) {
                if (random_positive(1.0f) < 0.5f) {
                    attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKLEND_e, 5.0f, 0);
                    i_this->m02CC = 0;
                    i_this->mFightBehavior = 0x65;
                } else {
                    attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKL2_e, 5.0f, 0);
                    i_this->m02CC = 1;
                    i_this->mFightBehavior = 100;
                }
            } else {
                i_this->mFightBehavior = 0x14;
            }
        }
        break;
    case 10:
        attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKR1_e, 5.0f, 0);
        i_this->mFightBehavior++;
        monster_sound(i_this, 0x4880, 0);
        break;
    case 0xb:
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_KAITENR1_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 0xc:
        spinTarget = 1.0f;
        spinStep = 0.1f;
        if (animation_stopped(i_this)) {
            i_this->mFightBehavior++;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
            i_this->m1F80 = 0xf;
            i_this->m1F82 = 2;
            i_this->m1F7E = (i_this->current.angle.y + 0x2000) + (REG0_S(8));
        }
        break;
    case 0xd:
        i_this->m1F80 = 0xf;
        i_this->m02DC = (REG8_F(3)) + 10.0f;
        spinTarget = 1.0f;
        spinStep = 0.1f;
        i_this->m0302 = (s16)gabi::ftoi(((REG0_F(13)) + 10000.0f) *
                                        angle_sine(i_this->m02E8 * ((REG0_S(8)) + 500)));
        if (((i_this->mTimers[0] == 0) && (-0x400 < i_this->m0304)) && (i_this->m0304 < 0x400)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_KAITENR1_END_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 0xe:
        if ((animation_stopped(i_this)) && (std::fabs(i_this->m0308) < 0.01f)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_OTOTOR_e, 5.0f, 2);
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
            i_this->mFightBehavior = 3;
        }
        break;
    case 0xf:
        attack_set(i_this, dRes_INDEX_ST_BCK_ATTACKL1_e, 5.0f, 0);
        i_this->mFightBehavior++;
        monster_sound(i_this, 0x4880, 0);
        break;
    case 0x10:
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_KAITENL1_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 0x11:
        spinTarget = -1.0f;
        spinStep = 0.1f;
        if (animation_stopped(i_this)) {
            i_this->mFightBehavior++;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
            i_this->m1F80 = 0xf;
            i_this->m1F82 = 2;
            i_this->m1F7E = (i_this->current.angle.y + 0x2000) + (REG0_S(9));
        }
        break;
    case 0x12:
        i_this->m1F80 = 0xf;
        i_this->m02DC = (REG8_F(3)) + 10.0f;
        spinTarget = -1.0f;
        spinStep = 0.1f;
        i_this->m0302 = (s16)gabi::ftoi(((REG0_F(13)) + 10000.0f) *
                                        angle_sine(i_this->m02E8 * ((REG0_S(8)) + 500)));
        if (((i_this->mTimers[0] == 0) && (-0x400 < i_this->m0304)) && (i_this->m0304 < 0x400)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_KAITENL1_END_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 0x13:
        if ((animation_stopped(i_this)) && (std::fabs(i_this->m0308) < 0.01f)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_OTOTOL_e, 5.0f, 2);
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
            i_this->mFightBehavior = 8;
        }
        break;
    case 0x14:
        attack_set(i_this, dRes_INDEX_ST_BCK_KAITENR2_e, 5.0f, 0);
        i_this->mFightBehavior++;
        break;
    case 0x15:
        spinTarget = 1.0f;
        spinStep = 0.1f;
        if (animation_stopped(i_this)) {
            i_this->mFightBehavior++;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
            i_this->m1F80 = 0xf;
            i_this->m1F82 = 2;
            i_this->m1F7E = (i_this->current.angle.y + 0x2000) + (REG0_S(8));
        }
        break;
    case 0x16:
        i_this->m1F80 = 0xf;
        i_this->m02DC = (REG8_F(3)) + 10.0f;
        spinTarget = 1.0f;
        spinStep = 0.1f;
        i_this->m0302 = (s16)gabi::ftoi(((REG0_F(13)) + 10000.0f) *
                                        angle_sine(i_this->m02E8 * ((REG0_S(8)) + 500)));
        if (((i_this->mTimers[0] == 0) && (-0x400 < i_this->m0304)) && (i_this->m0304 < 0x400)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_KAITENR2_END_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 0x17:
        if ((animation_stopped(i_this)) && (std::fabs(i_this->m0308) < 0.01f)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_OTOTOR_e, 5.0f, 2);
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
            i_this->mFightBehavior = 3;
        }
        break;
    case 0x19:
        attack_set(i_this, dRes_INDEX_ST_BCK_KAITENL2_e, 5.0f, 0);
        i_this->mFightBehavior++;
        break;
    case 0x1a:
        i_this->m0ED1 = 2;
        spinTarget = -1.0f;
        spinStep = 0.1f;
        if (animation_stopped(i_this)) {
            i_this->mFightBehavior++;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
            i_this->m1F80 = 0xf;
            i_this->m1F82 = 2;
            i_this->m1F7E = (i_this->current.angle.y + 0x2000) + (REG0_S(9));
        }
        break;
    case 0x1b:
        i_this->m1F80 = 0xf;
        i_this->m02DC = (REG8_F(3)) + 10.0f;
        spinTarget = -1.0f;
        spinStep = 0.1f;
        i_this->m0302 = (s16)gabi::ftoi(((REG0_F(13)) + 10000.0f) *
                                        angle_sine(i_this->m02E8 * ((REG0_S(8)) + 500)));
        if (((i_this->mTimers[0] == 0) && (-0x400 < i_this->m0304)) && (i_this->m0304 < 0x400)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_KAITENL2_END_e, 1.0f, 0);
            i_this->mFightBehavior++;
        }
        break;
    case 0x1c:
        if ((animation_stopped(i_this)) && (std::fabs(i_this->m0308) < 0.01f)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_OTOTOL_e, 5.0f, 2);
            i_this->mFightBehavior = 8;
            i_this->mTimers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
        }
        break;
    case 100:
        if (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 15) {
            sound_at_level(i_this, 0x5894, 0);
        }
        if (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 24) {
            buki_smoke_set(i_this);
        }
        i_this->m0ED1 = 1;
    case 0x65:
        if (animation_stopped(i_this)) {
            i_this->mActionState = ST_ACTION_PATROL;
            wait_set(i_this);
            i_this->m02C4 = 1;
            i_this->mTimers[1] = gabi::ftoi(random_positive(40.0f));
        }
        break;
    }
    cLib_addCalc2(&i_this->m0308, spinTarget, 1.0f, spinStep);
    i_this->m0304 += (s16)gabi::ftoi(((REG0_F(11)) + 5000.0f) * i_this->m0308);
    if (std::fabs(i_this->m0308) > 0.05f) {
        s16 r4 = i_this->m02CA;
        i_this->m02CA = r4 + gabi::ftoi(((REG0_F(11)) + 5000.0f) * std::fabs(i_this->m0308));
        if ((i_this->m02CA >= 100) && (r4 < 100)) {
            u32 r23 = gabi::ftoi(std::fabs((f32)i_this->m0308 * 100.0f));
            if (r23 < 0x32) {
                r23 = 0x32;
            }
            sound_at_level(i_this, 0x5895, r23);
        }
    } else {
        i_this->m02CA = 0;
    }
    joint_matrix(i_this, ST_JNT_BUKI_e);
    local_d8.x = (REG0_F(10)) + 80.0f;
    local_d8.y = 0.0f;
    local_d8.z = 0.0f;
    copyVectorWords(gabi::ea(&i_this->m0EC4), gabi::ea(&i_this->m0EB8));
    MtxPosition(&local_d8, &i_this->m0EB8);
    if (std::fabs(i_this->m0308) > 0.5f) {
        gabi::Local<LineCheckLocal> line;
        initialize_line(line);
        i_this->m0ED1 = 2;
        if ((std::fabs(i_this->m0308) > 0.9f) &&
            (std::fabs(i_this->speedF) > std::fabs(i_this->m02DC) * 0.9f)) {
            if (line_cross(line, &i_this->m0EC4, &i_this->m0EB8, i_this)) {
                i_this->mActionState = ST_ACTION_PATROL;
                wait_set(i_this);
                i_this->m02C4 = 1;
                i_this->mTimers[1] = gabi::ftoi(random_positive(40.0f));
            }
        }
        finish_line(line);
    }
    if (std::fabs(i_this->m0308) > 0.5f) {
        i_this->m0ED1 = 2;
    }
}

/* inlined into St_move */
void fight2(st_class *i_this) {
    stGetGameInfoWithLinkage<u32>();
    gabi::Local<VectorLocal> zero, effect;
    auto &local_2c = zero->value;
    auto &cStack_38 = effect->value;

    bool r30 = false;
    local_2c.x = 0.0f;
    local_2c.y = 0.0f;
    local_2c.z = 0.0f;
    i_this->m02DC = 0.0f;
    switch (i_this->m02C4) {
    case 0:
        i_this->m02F6 = 10;
        i_this->m0ED0 = 1;
        attack_set(i_this, dRes_INDEX_ST_BCK_KYORO_e, 5.0f, 0);
        i_this->m02C4++;
        monster_sound(i_this, 0x4881, 0);
        break;
    case 1:
        i_this->m02F6 = 10;
        i_this->m0ED0 = 1;
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_NUKU1_e, 1.0f, 0);
            i_this->m02C4++;
        }
        break;
    case 2:
        i_this->m02F6 = 10;
        i_this->m0ED0 = 1;
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_NUKU2_e, 1.0f, 0);
            i_this->m02C4 = 0x65;
            i_this->m1DC8 = 1;
            joint_matrix(i_this, ST_JNT_KATAL_e);
            MtxPosition(&local_2c, &cStack_38);
            particle_with_angle(0x817A, &cStack_38, &i_this->shape_angle);
            sound_at_level(i_this, 0x5897, 0);
        }
        break;
    case 3:
        i_this->m1DC8 = 3;
        attack_set(i_this, dRes_INDEX_ST_BCK_HONE_ATTACK_e, 3.0f, 2);
        i_this->m02C4++;
        i_this->mTimers[0] = gabi::ftoi(random_positive(3.0f)) * 0x28;
        monster_sound(i_this, 0x4883, 0);
        break;
    case 4:
        i_this->m0ED1 = 1;
        if ((((gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 1) ||
              (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 11)) ||
             (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 21)) ||
            (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) == 31)) {
            sound_at_level(i_this, 0x5899, 0);
        }
        if (i_this->mTimers[0] == 0) {
            r30 = true;
        }
        break;
    case 0x32:
        anm_init(i_this, dRes_INDEX_ST_BCK_WAIT_e, 10.0f, 2, 1.0f, -1);
        i_this->m1DC8 = 1;
        i_this->m02C4++;
        i_this->mTimers[0] = 0x14;
    case 0x33:
        i_this->m02F6 = 10;
        i_this->m0ED0 = 1;
        if (i_this->mTimers[0] == 0) {
            attack_set(i_this, dRes_INDEX_ST_BCK_TUKERU1_e, 5.0f, 0);
            i_this->m02C4++;
        }
        break;
    case 0x34:
        i_this->m02F6 = 10;
        i_this->m0ED0 = 1;
        if (animation_stopped(i_this)) {
            attack_set(i_this, dRes_INDEX_ST_BCK_TUKERU2_e, 1.0f, 0);
            i_this->m02C4++;
            i_this->m1DC8 = 0;
        }
        break;
    case 0x35:
        i_this->m02F6 = 10;
        i_this->m0ED0 = 1;
        if (animation_stopped(i_this)) {
            i_this->m02C4 = 100;
            anm_init(i_this, dRes_INDEX_ST_BCK_FUKKI_WAIT_e, 10.0f, 2, 1.0f, -1);
        }
        break;
    case 0x64:
        if (i_this->m1DD0 == 10) {
            r30 = true;
        }
        break;
    case 0x65:
        if (animation_stopped(i_this)) {
            r30 = true;
        }
        break;
    }
    if (r30) {
        i_this->mActionState = ST_ACTION_PATROL;
        wait_set(i_this);
        i_this->m02C4 = 1;
        i_this->mTimers[1] = gabi::ftoi(random_positive(40.0f));
    }
}

/* inlined into St_move */
void ground_wait(st_class *i_this) {
    i_this->m02F6 = 10;
    i_this->m0ED0 = 2;
    switch ((s16)i_this->m02C4) {
    case 0:
        i_this->attention_info.flags = 0;
        i_this->actor_status = (u32)i_this->actor_status & ~0x20u;
        if (ambush_trigger(i_this))
            i_this->m02C4 = 1;
        break;
    case 1:
        i_this->m02B9 = 0;
        sound_at_level(i_this, 0x589A, 0);
        anm_init(i_this, dRes_INDEX_ST_BCK_START_e, 1.0f, 0, 1.0f, -1);
        i_this->m02C4 = 2;
        i_this->attention_info.flags = 4;
        i_this->actor_status = (u32)i_this->actor_status | 0x20;
        dPa_smokeEcallBack_end(&i_this->m1E88[0]);
        toon_particle(i_this, 0xA17C, &i_this->current.pos, nullptr, &i_this->m1E88[0]);
        i_this->mTimers[0] = 60;
        break;
    case 2:
        if (((u32)i_this->actor_status & 0x4000000) && i_this->mTimers[0] &&
            ((s16)i_this->mTimers[0] & 7) == 0 && random_positive(1.0f) < 0.5f) {
            u32 play = stGetGameInfoWithLinkage<u32>();
            gabi::Local<VectorLocal> direction;
            direction->value.x = 0.0f;
            direction->value.y = 1.0f;
            direction->value.z = 0.0f;
            dVibration_c_StartShock(at<StGameInfoView>(play)->mVibration, (s32)REG0_S(2) + 4, -0x21,
                                    &direction->value);
        }
        if (animation_stopped(i_this)) {
            i_this->mActionState = ST_ACTION_PATROL;
            i_this->m02C4 = 0;
        }
        break;
    }
}

/* inlined into St_move */
void kan_wait(st_class *i_this) {
    i_this->m02F6 = 10;
    i_this->m0ED0 = 2;
    switch ((s16)i_this->m02C4) {
    case 0:
        i_this->attention_info.flags = 0;
        i_this->actor_status = (u32)i_this->actor_status & ~0x20u;
        if (ambush_trigger(i_this))
            i_this->m02C4 = 1;
        break;
    case 1:
        anm_init(i_this, i_this->mBehaviorType == BEHAVIOR_IN_HORIZONTAL_COFFIN ? 0x35 : 0x2C, 1.0f,
                 0, 0.0f, -1);
        i_this->m02C4 = 2;
        i_this->m02B9 = 0;
        i_this->mTimers[0] = l_HIO.m18;
        i_this->mTimers[2] = (s16)i_this->mTimers[0] + 20;
        [[fallthrough]];
    case 2:
        if (!i_this->mTimers[0]) {
            stMorf(gabi::ea((void *)i_this->mpMorf)).mRate = 1.0f;
            i_this->m02C4 = 3;
            i_this->attention_info.flags = 4;
            i_this->actor_status = (u32)i_this->actor_status | 0x20;
        }
        break;
    case 3:
        if (animation_stopped(i_this)) {
            i_this->mActionState = ST_ACTION_PATROL;
            i_this->m02C4 = 0;
        }
        break;
    }
}

/* 024963A4 */
void st_break_wait(st_class *i_this) {
    auto &state = i_this->m02C4;
    auto &recoverTimer = i_this->mTimers[0];
    auto &lifetime = i_this->m02F8;
    i_this->m02DC = 0.0f;
    if (!i_this->m1DDC)
        i_this->m02F6 = 10;
    i_this->m0ED0 = 1;
    switch ((s16)state) {
    case 0:
        anm_init(i_this, dRes_INDEX_ST_BCK_FUKKI_WAIT_e, 10.0f, 2, 1.0f, -1);
        state = 1;
        recoverTimer = l_HIO.m1A;
        i_this->m1DDC = l_HIO.m1A;
        [[fallthrough]];
    case 1:
        if (!recoverTimer) {
            state = 2;
            i_this->m0ED3 = 2;
            sound_at(i_this, 0x5892, &i_this->current.pos);
            lifetime = 0x1F3; // The HD body stores499, unlike GC500.
            return;
        }
        break;
    case 2:
    case 3:
    case 0xB:
        i_this->m0ED0 = 2;
        i_this->m1F68 = 1;
        if (i_this->m0ED2 >= 25 && ((s16)state != 3 || i_this->m1DD0 == 10)) {
            state = 100;
            recoverTimer = 10;
        }
        break;
    case 0xA:
        anm_init(i_this, dRes_INDEX_ST_BCK_FUKKI_WAIT_e, 10.0f, 2, 1.0f, -1);
        state = 0xB;
        i_this->m0ED0 = 2;
        i_this->m1F68 = 1;
        if (i_this->m0ED2 >= 25) {
            state = 100;
            recoverTimer = 10;
        }
        break;
    case 0x14:
    case 0x23:
        i_this->m1DDC = 100;
        recoverTimer = 100;
        if (state == 0x23) {
            i_this->attention_info.flags = 0;
            i_this->actor_status = (u32)i_this->actor_status & ~0x20u;
        }
        break;
    case 100:
        i_this->m1F68 = 2;
        if (!recoverTimer) {
            i_this->mActionState = ST_ACTION_PATROL;
            wait_set(i_this);
            state = 1;
            f64 random = cM_rndF<f64>(40.0f);
            i_this->m1E84 = 16;
            i_this->mTimers[1] = (s16)gabi::ftoi(random);
        }
        break;
    }
    if (lifetime) {
        --lifetime;
        if (!lifetime) {
            gabi::Local<VectorLocal> effect;
            effect->value.x = i_this->current.pos.x;
            effect->value.y = i_this->current.pos.y;
            effect->value.z = i_this->current.pos.z;
            effect->value.y = (f32)effect->value.y + 100.0f;
            fopAcM_createDisappear(i_this, &effect->value, 10, 0, 0xFF);
            fopAcM_delete(i_this);
            if (i_this->mDeathSwitch)
                dSv_info_c_onSwitch(at<void>((u32)stSaveContext() + 0x20), (u8)i_this->mDeathSwitch,
                                    (s8)i_this->current.roomNo);
            dSv_info_c_onActor(at<void>((u32)stSaveContext() + 0x20), (u16)i_this->setID,
                               (s8)i_this->home.roomNo);
            gabi::Local<IDLocal> weapon;
            weapon->id = i_this->mHeldWeaponEntityId;
            u32 held = 0;
            if ((u32)weapon->id != 0xFFFFFFFF)
                held = fopAcIt_Judge<u32>(at<void>(0x025E1234), &weapon->id);
            if (held)
                fopAcM_cancelCarryNow(at<void>(held));
        }
    }
}

/* 02496718 */
void head_damage(st_class *i_this) {
    gabi::Local<VectorLocal> zero, effect;
    gabi::Local<AngleLocal> angle;
    zero->value.x = 0.0f;
    zero->value.z = 0.0f;
    zero->value.y = 0.0f;
    angle->value.x = i_this->shape_angle.x;
    angle->value.y = i_this->shape_angle.y;
    angle->value.z = i_this->shape_angle.z;
    angle->value.y = (s16)((s16)angle->value.y - 0x8000);
    i_this->m02DC = 0.0f;
    auto &state = i_this->m02C4;
    switch ((s16)state) {
    case 0:
    case 10: {
        bool upper = state == 10;
        anm_init(i_this, upper ? 0x23 : 0x25, 5.0f, 0, 1.0f, -1);
        state = upper ? 11 : 1;
        sound_at(i_this, 0x588F, &i_this->eyePos);
        break;
    }
    case 1:
    case 11: {
        bool upper = state == 11;
        u32 animation = gabi::ea((void *)i_this->mpMorf);
        s32 frame = gabi::ftoi(*at<be<f32>>(animation, 0x9C));
        if (frame == (upper ? 30 : 32))
            head_effect(i_this, zero, effect, angle);
        animation = gabi::ea((void *)i_this->mpMorf);
        if ((stMorf(animation).mState & 1) || stMorf(animation).mRate == 0.0f) {
            if (upper) {
                i_this->mActionState = ST_ACTION_UPPER_BODY;
                state = -1;
            } else {
                i_this->mActionState = ST_ACTION_PATROL;
                wait_set(i_this);
                state = 1;
                f64 random = cM_rndF<f64>(40.0f);
                i_this->mTimers[1] = (s16)gabi::ftoi(random);
            }
        }
        break;
    }
    }
}

/* 02496AA0 */
void ue_move(st_class *i_this) {
    u32 player = player_pointer();
    i_this->actor_status = (u32)i_this->actor_status | 0x20;
    auto &state = i_this->m02C4;
    i_this->attention_info.flags = 4;
    switch ((s16)state) {
    case -1:
        anm_init(i_this, dRes_INDEX_ST_BCK_UE_JUMP1_e, 2.0f, 0, 1.0f, -1);
        state = 1;
        i_this->speed.y = 30.0f;
        break;
    case 0:
        anm_init(i_this, dRes_INDEX_ST_BCK_UE_JUMP1_e, 2.0f, 0, 1.0f, -1);
        state = 1;
        i_this->current.angle.y = (s16)(player_angle(i_this) + 0x8000);
        i_this->speed.y = 40.0f;
        i_this->speedF = 20.0f;
        i_this->m02DC = 20.0f;
        break;
    case 1:
        if (i_this->mAcch.m_flags & 0x20) {
            anm_init(i_this, dRes_INDEX_ST_BCK_UE_JUMP2_e, 2.0f, 0, 1.0f, -1);
            state = 2;
        }
        break;
    case 2:
        if (animation_stopped(i_this)) {
            anm_init(i_this, dRes_INDEX_ST_BCK_UE_WALK_e, 5.0f, 2, 1.2f, -1);
            state = 3;
        }
        break;
    case 3: {
        gabi::Local<VectorLocal> direction, rotated, target;
        u32 matrix = stCalcMatrix().v.get();
        s16 angle = player_angle(i_this);
        f64 random = cM_rndFX<f64>(10000.0f);
        angle = (s16)(angle + 0x8000 + (s16)gabi::ftoi(random));
        mDoMtx_YrotS(at<void>(matrix), angle);
        direction->value.x = 0.0f;
        direction->value.y = 0.0f;
        direction->value.z = 500.0f;
        MtxPosition(&direction->value, &rotated->value);
        cXyz___pl(at<void>(gabi::ea(&stActor(player).current.pos)), &target->value,
                  &rotated->value);
        copyVectorWords(gabi::ea(i_this) + 0x3EC, gabi::ea(&target->value));
        state = 4;
        i_this->mTimers[0] = 20;
        [[fallthrough]];
    }
    case 4: {
        s32 frame = gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame);
        if (frame == 10) {
            u32 play = stGetGameInfoWithLinkage<u32>();
            u32 emitter = dPa_control_c_set<u32>(
                at<void>(at<StGameInfoView>(play)->mpParticle.v.get()), 0, 0x8179, &i_this->eyePos,
                (void *)nullptr, (void *)nullptr, 0xFF, (void *)nullptr, -1, (void *)nullptr,
                (void *)nullptr, (void *)nullptr);
            if (emitter) {
                u32 model = stMorf(gabi::ea((void *)i_this->mpMorf)).mpModel.v.get();
                u32 matrixData = stModel(model).mpNodeData.v.get();
                u16 flags = *at<be<u16>>(matrixData, 4);
                u32 matrix = at<StModelNodeData>(matrixData)->mpAnmMtx.v.get();
                *at<be<u16>>(matrixData, 4) = flags | 0x10;
                JPASetRMtxTVecfromMtx(at<void>(matrix + 0x2D0), &stEmitter(emitter).mGlobalRotation,
                                      &stEmitter(emitter).mGlobalTranslation);
            }
        }
        i_this->m02E4 = 0x1000;
        i_this->m02DC = REG8_F(1) + 10.0f;
        if (!i_this->mTimers[0])
            state = 3;
        if (player_distance(i_this) > 530.0f) {
            anm_init(i_this, dRes_INDEX_ST_BCK_UE_WAIT_e, 5.0f, 2, 1.0f, -1);
            state = 5;
            i_this->mTimers[0] = 10;
        }
        break;
    }
    case 5:
        i_this->m02DC = 0.0f;
        i_this->m02E4 = 0x2000;
        copyVectorWords(gabi::ea(i_this) + 0x3EC, gabi::ea(&stActor(player).current.pos));
        if (!i_this->mTimers[0] && player_distance(i_this) < 480.0f)
            state = -1;
        break;
    }
    if (i_this->mAcch.m_ground_h == -1000000000.0f) {
        u32 x = word(gabi::ea(i_this) + 0x2EC), y = word(gabi::ea(i_this) + 0x2F0),
            z = word(gabi::ea(i_this) + 0x2F4);
        stFloatWord(i_this->current.pos.x) = x;
        stFloatWord(i_this->current.pos.y) = y;
        stFloatWord(i_this->current.pos.z) = z;
        stFloatWord(i_this->old.pos.x) = x;
        stFloatWord(i_this->old.pos.y) = y;
        stFloatWord(i_this->old.pos.z) = z;
    }
}

/* 02496F34 */
void sita_move(st_class *i_this) {
    u32 player = player_pointer();
    f32 y = i_this->current.pos.y + 70.0f, z = i_this->current.pos.z;
    i_this->eyePos.z = z;
    i_this->eyePos.y = y;
    i_this->attention_info.position.y = y;
    f32 x = i_this->current.pos.x;
    i_this->eyePos.x = x;
    i_this->attention_info.position.x = x;
    i_this->attention_info.flags = 4;
    i_this->actor_status = (u32)i_this->actor_status | 0x20;
    i_this->attention_info.position.z = z;
    auto &state = i_this->m02C4;
    switch ((s16)state) {
    case 0: {
        state = 1;
        i_this->mTimers[0] = 60;
        f64 random = cM_rndF<f64>(200.0f);
        i_this->mTimers[1] = (s16)gabi::ftoi((f32)(random + 350.0f));
        [[fallthrough]];
    }
    case 1:
        anm_init(i_this, dRes_INDEX_ST_BCK_SITA_WALK_e, 5.0f, 2, 1.2f, -1);
        [[fallthrough]];
    case 2:
        state = 3;
        [[fallthrough]];
    case 3:
        i_this->m02E4 = 0x800;
        i_this->m02DC = REG8_F(1) + 10.0f;
        copyVectorWords(gabi::ea(i_this) + 0x3EC, gabi::ea(&stActor(player).current.pos));
        if (!i_this->mTimers[0] && player_distance(i_this) < 260.0f)
            state = 5;
        break;
    case 5:
        copyVectorWords(gabi::ea(i_this) + 0x3EC, gabi::ea(&stActor(player).current.pos));
        i_this->m02E4 = 0x2000;
        anm_init(i_this, dRes_INDEX_ST_BCK_KICK1_e, 3.0f, 0, 1.0f, -1);
        state = 6;
        break;
    case 6:
        i_this->m02DC = REG8_F(1) + 5.0f;
        if (animation_stopped(i_this)) {
            anm_init(i_this, dRes_INDEX_ST_BCK_KICK2_e, 1.0f, 0, 1.0f, -1);
            state = 7;
        }
        break;
    case 7:
        if (gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf)).mFrame) < 10)
            i_this->m0ED1 = 3;
        i_this->m02DC = REG8_F(1);
        if (animation_stopped(i_this)) {
            state = 1;
            i_this->mTimers[0] = 20;
        }
        break;
    }
    gabi::Local<IDLocal> upperID;
    upperID->id = i_this->mUpperBodyEntityId;
    u32 upper = 0;
    if ((u32)upperID->id != 0xFFFFFFFF)
        upper = fopAcIt_Judge<u32>(at<void>(0x025E1234), &upperID->id);
    if (upper) {
        s16 timer = i_this->mTimers[1];
        if (timer < 5)
            i_this->m0ED0 = 2;
        i_this->m1E86 = 1;
        if (!timer) {
            i_this->m0ED3 = 11;
            i_this->mActionState = ST_ACTION_BREAK_WAIT;
            state = 10;
            i_this->m02F8 = 0;
            fopAcM_delete(at<void>(upper));
            i_this->m1E86 = 0;
        }
    }
}

/* 024972A4 */
void St_move(st_class *i_this) {
    stGetGameInfoWithLinkage<u32>();
    joint_matrix(i_this, ST_JNT_BUKI_e);
    gabi::Local<VectorLocal> direction;
    direction->value.x = 160.0f;
    direction->value.y = REG0_F(11);
    direction->value.z = REG0_F(12);
    MtxPosition(&direction->value, &i_this->m0EAC);
    i_this->m1F84 = 0;
    switch ((s16)i_this->mActionState) {
    case ST_ACTION_PATROL:
        i_this->bodyForm = 0;
        jyunkai(i_this);
        break;
    case ST_ACTION_FIGHT:
        i_this->bodyForm = 0;
        fight(i_this);
        break;
    case ST_ACTION_GROUND_WAIT:
        i_this->bodyForm = 0;
        ground_wait(i_this);
        break;
    case ST_ACTION_COFFIN_WAIT:
        i_this->bodyForm = 0;
        kan_wait(i_this);
        break;
    case ST_ACTION_FIGHT_UNARMED:
        i_this->bodyForm = 0;
        fight2(i_this);
        break;
    case ST_ACTION_BREAK_WAIT:
        i_this->bodyForm = 0;
        st_break_wait(i_this);
        break;
    case ST_ACTION_HEAD_DAMAGE:
        head_damage(i_this);
        break;
    case ST_ACTION_UPPER_BODY:
        i_this->bodyForm = 1;
        ue_move(i_this);
        break;
    case ST_ACTION_LOWER_BODY:
        i_this->bodyForm = 2;
        sita_move(i_this);
        break;
    }
    if (i_this->mActionState != ST_ACTION_COFFIN_WAIT)
        speed_pos_calc(i_this);
    if (i_this->m1F84 && i_this->m1DD0 == 10) {
        if (i_this->m1F84 == 1 && !word(gabi::ea(i_this) + 0x21D4))
            toon_particle(i_this, 0xA17D, &i_this->m0EAC, &i_this->shape_angle, &i_this->m1F88);
    } else
        dPa_smokeEcallBack_end(&i_this->m1F88);
}

/* inlined into daSt_Execute */
void damage_check(st_class *i_this) {
    u32 player = player_pointer();
    dCcD_GStts_Move(&i_this->mStts.__vtbl_gstts);
    if (!i_this->m02F6) {
        gabi::Local<AttackInfoLocal> local;
        auto &atInfo = local->value;
        atInfo.mPlCutBit = 0;
        atInfo.mHitSoundId = 0;
        for (u32 index = 0; index < 7; ++index) {
            u32 sphere = gabi::ea(&i_this->m054C[index]);
            if (!dCcD_GObjInf_ChkTgHit<s32>(at<void>(sphere)))
                continue;
            u32 object = dCcD_GObjInf_GetTgHitObj<u32>(at<void>(sphere));
            atInfo.mpObj = at<void>(object);
            atInfo.pParticlePos = at<cXyz>(sphere + 0xCC);
            if (word(object + 0x10) & 0x100000) {
                i_this->mEnemyIce.mLightShrinkTimer = 1;
                u32 held = find_actor(i_this->mHeldWeaponEntityId);
                if (held)
                    fopAcM_cancelCarryNow(at<void>(held));
                i_this->m1DD0 = 0;
                return;
            }
            i_this->m02F6 = 5;
            if (index == 0 && i_this->m1DD8) {
                s8 remaining = i_this->stealItemLeft;
                i_this->stealItemLeft = 0;
                cc_at_check(i_this, &atInfo);
                i_this->stealItemLeft = remaining;
                if (atInfo.mResultingAttackType == 1 && stPlayer(player).mJumpCutExtraDamage)
                    i_this->m02F6 = 2;
                auto *head = &i_this->mParts[15];
                if (i_this->health <= 0 || atInfo.mResultingAttackType == 9 ||
                    atInfo.mResultingAttackType == 2) {
                    head_anm_init(i_this, dRes_INDEX_ST_BCK_HEADB_DEAD_e, 1.0f, 0, 1.0f, -1);
                    head->mPartState = 8;
                    if (atInfo.mResultingAttackType == 9 && stPlayer(player).mCutType != 0x11) {
                        head->mPartVelocity.x = 0.0f;
                        head->mPartVelocity.y = -5.0f;
                        head->mPartVelocity.z = 0.0f;
                        *at<be<u8>>(0x101EACB7) = 6;
                    } else {
                        head->mPartRotAdd.x = gabi::ftoi(random_signed(9000.0f));
                        head->mPartRotAdd.z = gabi::ftoi(random_signed(9000.0f));
                        mDoMtx_YrotS(stCalcMatrix().get(), (s16)stPlayer(player).shape_angle.y);
                        gabi::Local<VectorLocal> direction;
                        if (stPlayer(player).mCutType == 0x11) {
                            direction->value.x = REG8_F(4) - 100.0f;
                            direction->value.y = 30.0f;
                            direction->value.z = 0.0f;
                            *at<be<u8>>(0x101EACB7) = 6;
                        } else {
                            direction->value.x = 0.0f;
                            direction->value.y = 50.0f;
                            direction->value.z = 50.0f;
                        }
                        MtxPosition(&direction->value, &head->mPartVelocity);
                    }
                    head->mWaitTimer = 30;
                    monster_sound(i_this, 0x4884, 0);
                } else {
                    head_anm_init(i_this, dRes_INDEX_ST_BCK_HEADB_DAMAGE_e, 1.0f, 0, 1.0f, -1);
                    head->mPartState = 7;
                    head->mWaitTimer = 127;
                    i_this->m1E82 = REG8_S(2) + 20;
                    monster_sound(i_this, 0x487B, 0);
                }
            } else {
                at_power_check(&atInfo);
                if (atInfo.mDamage)
                    mDoAud_onEnemyDamage();
                u32 attacker = gabi::ea((void *)atInfo.mpActor);
                if (attacker && *at<be<s16>>(attacker, 8) == 0x1BE && i_this->stealItemLeft) {
                    --i_this->stealItemLeft;
                    fopAcM_createStealItem(&i_this->current.pos, (u32)i_this->itemTableIdx,
                                           (s8)i_this->current.roomNo, 0,
                                           (u8)i_this->stealItemBitNo);
                    ++i_this->stealItemBitNo;
                    attacker = gabi::ea((void *)atInfo.mpActor);
                }
                if ((attacker && *at<be<s16>>(attacker, 8) == 0xA8) ||
                    atInfo.mResultingAttackType == 2) {
                    if (atInfo.mResultingAttackType == 2)
                        i_this->m1E84 = 0;
                    def_se_set(i_this, (void *)atInfo.mpObj, 0x34);
                    monster_sound(i_this, 0x487B, 0);
                    i_this->m1E84 = (s8)i_this->m1E84 - (u8)atInfo.mDamage;
                    i_this->m1DE4 = REG6_S(atInfo.mResultingAttackType == 9 ? 7 : 6) +
                                    (atInfo.mResultingAttackType == 9 ? 20 : 10);
                    if (atInfo.mResultingAttackType == 1 && stPlayer(player).mJumpCutExtraDamage) {
                        i_this->m02F6 = 2;
                        u8 cut = stPlayer(player).mCutType;
                        if (cut == 8 || cut == 9)
                            i_this->m1E84 = (u8)i_this->m1E84 - (u8)atInfo.mDamage;
                    }
                    mDoAud_bgmHitSound((s32)atInfo.mHitSoundId);
                    if ((s8)i_this->m1E84 <= 0) {
                        i_this->m1DC8 = 0;
                        u8 form = i_this->bodyForm;
                        if (atInfo.mPlCutBit == 0x80 && !form) {
                            i_this->mActionState = ST_ACTION_LOWER_BODY;
                            i_this->m02C4 = 0;
                            i_this->m0ED3 = 10;
                            i_this->mUpperBodyEntityId = fopAcM_create<u32>(
                                0xBE, 0xE, &i_this->current.pos, (s8)i_this->current.roomNo,
                                &i_this->home.angle, (void *)nullptr, -1, (void *)nullptr);
                            i_this->m1E84 = 5;
                            i_this->m1E86 = 0;
                        } else {
                            i_this->m02C4 = form == 2 ? 35 : form == 1 ? 20 : 0;
                            i_this->mActionState = ST_ACTION_BREAK_WAIT;
                            i_this->m0ED3 = 1;
                            i_this->m02F8 = 0;
                            sound_at_level(i_this, 0x5891, 0);
                            monster_sound(i_this, 0x487C, 0);
                        }
                    }
                } else if (index == 0) {
                    i_this->m1DE4 = REG6_S(7) + 5;
                    u8 form = i_this->bodyForm;
                    if (form == 1) {
                        i_this->mActionState = ST_ACTION_HEAD_DAMAGE;
                        i_this->m02C4 = 10;
                    } else if (!form) {
                        i_this->mActionState = ST_ACTION_HEAD_DAMAGE;
                        i_this->m02C4 = 0;
                    }
                    sound_at_level(i_this, 0x588E, 0);
                    monster_sound(i_this, 0x487A, 0);
                } else {
                    def_se_set(i_this, (void *)atInfo.mpObj, 0x34);
                    monster_sound(i_this, 0x487B, 0);
                    i_this->m1DE4 = REG6_S(7) + 8;
                    if (!i_this->bodyForm) {
                        i_this->mActionState = ST_ACTION_PATROL;
                        wait_set(i_this);
                        i_this->m02C4 = 1;
                        i_this->mTimers[1] = 10;
                    }
                }
            }
            break;
        }
    }
    if (i_this->m1DE4)
        --i_this->m1DE4;
}

/* 02492210 */
f32 part_posmove(st_class *i_this, st_p *part) {
    gabi::Local<VectorLocal> start, end, difference;
    gabi::Local<LineCheckLocal> line;
    initialize_line(line);
    // The previous position is a raw word copy, preserving NaN payload bits.
    u32 y = word(gabi::ea(part) + 0xC), x = word(gabi::ea(part) + 8),
        z = word(gabi::ea(part) + 0x10);
    *at<be<u32>>(gabi::ea(part), 0x14) = x;
    *at<be<u32>>(gabi::ea(part), 0x1C) = z;
    *at<be<u32>>(gabi::ea(part), 0x18) = y;
    PSVECAdd(&part->mPartPos, &part->mPartVelocity, &part->mPartPos);
    f32 vertical = part->mPartVelocity.y;
    if (!(vertical < -50.0f))
        part->mPartVelocity.y = vertical - 5.0f;
    f32 floor = (f32)part->mPartPos.y - 100.0f;
    csXyz___apl(&part->mPartRot, &part->mPartRotAdd);
    f32 px = part->mPartPos.x, py = part->mPartPos.y, pz = part->mPartPos.z;
    start->value.x = px;
    start->value.y = py + 100.0f;
    start->value.z = pz;
    end->value.x = px;
    end->value.y = py - 100.0f;
    end->value.z = pz;
    if (part->mPartVelocity.y > 0.0f) {
        if (line_cross(line, &end->value, &start->value, i_this)) {
            u32 check = gabi::ea(&line->check);
            f32 crossZ = line->check.mLin.mEnd.z, crossY = line->check.mLin.mEnd.y,
                crossX = line->check.mLin.mEnd.x;
            end->value.z = crossZ;
            end->value.y = crossY;
            end->value.x = crossX;
            if (!((f32)part->mPartPos.y < crossY)) {
                part->mPartVelocity.y = -5.0f;
                part->mPartPos.y = crossY - 10.0f;
            }
        }
    } else if (line_cross(line, &start->value, &end->value, i_this)) {
        u32 check = gabi::ea(&line->check);
        f32 crossX = line->check.mLin.mEnd.x;
        floor = line->check.mLin.mEnd.y;
        end->value.x = crossX;
        end->value.y = floor;
        end->value.z = line->check.mLin.mEnd.z;
    }
    cXyz___mi(&part->mPartPos, &difference->value, &part->mPartPrevPos);
    *at<be<u32>>(gabi::ea(&start->value), 0) = word(gabi::ea(&difference->value));
    *at<be<u32>>(gabi::ea(&start->value), 8) = word(gabi::ea(&difference->value) + 8);
    start->value.y = 0.0f;
    f32 magnitudeSquared = PSVECSquareMag<f32>(&start->value);
    f64 magnitude = sqrtHD<f64>(magnitudeSquared);
    if (magnitude > 0.0f) {
        u32 matrix = stCalcMatrix().v.get();
        s16 angle = cM_atan2s<s16>((f32)start->value.x, (f32)start->value.z);
        mDoMtx_YrotS(at<void>(matrix), angle);
        start->value.x = 0.0f;
        start->value.y = 30.0f;
        f32 velocitySquared = PSVECSquareMag<f32>(&part->mPartVelocity);
        f64 velocityMagnitude = sqrtHD<f64>(velocitySquared);
        start->value.z = (f32)(velocityMagnitude + 30.0f);
        MtxPosition(&start->value, &end->value);
        f32 y = part->mPartPos.y, x = part->mPartPos.x, z = part->mPartPos.z;
        start->value.x = x;
        start->value.y = y + 30.0f;
        start->value.z = z;
        PSVECAdd(&end->value, &part->mPartPos, &end->value);
        if (line_cross(line, &start->value, &end->value, i_this)) {
            f32 vx = part->mPartVelocity.x, px = part->mPartPrevPos.x, vz = part->mPartVelocity.z,
                pz = part->mPartPrevPos.z;
            part->mPartPos.x = px;
            part->mPartPos.z = pz;
            part->mPartVelocity.x = vx * -0.3f;
            part->mPartVelocity.z = vz * -0.3f;
        }
    }
    finish_line(line);
    return floor;
}

/* 024925B8 */
void part_move(st_class *i_this, s32 joint) {
    stGetGameInfoWithLinkage<u32>();
    auto *part = &i_this->mParts[(u32)joint];
    gabi::Local<VectorLocal> direction, target, difference;
    direction->value.x = 0.0f;
    direction->value.y = 0.0f;
    direction->value.z = 0.0f;
    u32 weapon = joint == 11 ? find_actor(i_this->mHeldWeaponEntityId) : 0;
    ++part->m3E;
    if (part->mWaitTimer)
        --part->mWaitTimer;
    if (!i_this->mEnemyIce.mLightShrinkTimer) {
        switch ((s8)part->mPartState) {
        case 0:
            joint_matrix(i_this, joint);
            MtxPosition(&direction->value, &part->mPartPos);
            if (i_this->m0ED3 == 1) {
                scatter_part(part);
                if (i_this->m0ED2 && joint != 11)
                    --i_this->m0ED2;
                if (joint == 15) {
                    i_this->m1DD8 = 1;
                    part->mPartState = 6;
                    part->mPartRotAdd.x = 0;
                    part->mPartRotAdd.y = 0;
                    part->mPartRotAdd.z = 0;
                    i_this->m1DDA = part->mPartRot.y;
                    head_anm_init(i_this, dRes_INDEX_ST_BCK_HEADB_JUMP1_e, 1.0f, 0, 1.0f, -1);
                } else {
                    part->mPartState = 5;
                    part->mPartRotAdd.x = (s16)gabi::ftoi(random_signed(6000.0f));
                    part->mPartRotAdd.y = (s16)gabi::ftoi(random_signed(6000.0f));
                }
            }
            if (i_this->m0ED3 == 10) {
                if (joint < 19) {
                    part->mPartState = 10;
                    if (joint == 11) {
                        part->mPartState = 5;
                        scatter_part(part);
                        part->mPartRotAdd.x = (s16)gabi::ftoi(random_signed(6000.0f));
                        part->mPartRotAdd.y = (s16)gabi::ftoi(random_signed(6000.0f));
                    }
                    if (i_this->m0ED2 && joint != 11)
                        --i_this->m0ED2;
                }
            } else if (i_this->m0ED3 == 20 && joint >= 19) {
                part->mPartState = 11;
                if (i_this->m0ED2 && joint != 11)
                    --i_this->m0ED2;
            }
            break;
        case 1:
            if (part->mWaitTimer) {
                part->mPartRot.y =
                    (s16)((s16)part->mPartRot.y + ((part->mWaitTimer & 4) ? 0x100 : -0x100));
            } else {
                joint_matrix(i_this, joint);
                MtxPosition(&direction->value, &target->value);
                if (joint == 15)
                    target->value.y = (f32)target->value.y - (REG6_F(13) + 20.0f);
                cXyz___mi(&target->value, &difference->value, &part->mPartPos);
                copyVectorWords(gabi::ea(&direction->value), gabi::ea(&difference->value));
                f64 distance = magnitude(&direction->value);
                if (distance < REG13_F(3) + 60.0f)
                    part->mPartState = 2;
                else {
                    f32 amplitude = (f32)(distance - 50.0f) * (REG8_F(8) + 1.0f);
                    if (amplitude > REG8_F(9) + 300.0f)
                        amplitude = REG8_F(10) + 300.0f;
                    s16 count = part->m3E;
                    direction->value.x =
                        gabi::fmadds(amplitude, angle_sine(count * 1000), direction->value.x);
                    f32 oscillation = (amplitude * angle_cosine(count * 1100)) * 0.7f;
                    f32 vertical = gabi::fmadds(amplitude, 0.5f, oscillation);
                    direction->value.y = (f32)direction->value.y + vertical;
                    direction->value.z =
                        gabi::fmadds(amplitude, angle_cosine(count * 1200), direction->value.z);
                    s16 angle = cM_atan2s<s16>((f32)direction->value.x, (f32)direction->value.z);
                    cLib_addCalcAngleS2(&part->m3A, angle, 4, 0x2000);
                    f32 x = direction->value.x, z = direction->value.z;
                    f64 horizontal = sqrtHD<f64>(gabi::fmadds(x, x, z * z));
                    s16 elevation = (s16)-cM_atan2s<s32>((f32)direction->value.y, horizontal);
                    cLib_addCalcAngleS2(&part->m38, elevation, 4, 0x2000);
                    mDoMtx_YrotS(stCalcMatrix().get(), (s16)part->m3A);
                    mDoMtx_XrotM(stCalcMatrix().get(), (s16)part->m38);
                    direction->value.x = 0.0f;
                    direction->value.y = 0.0f;
                    direction->value.z = (f32)part->m44 * 20.0f;
                    MtxPosition(&direction->value, &part->mPartVelocity);
                    PSVECAdd(&part->mPartPos, &part->mPartVelocity, &part->mPartPos);
                    if (joint == 15) {
                        part->mPartRot.x = part->m38;
                        part->mPartRot.y = part->m3A;
                        part->mPartRot.z = part->m3C;
                    } else
                        csXyz___apl(&part->mPartRot, &part->mPartRotAdd);
                    cLib_addCalc2(&part->m44, 1.0f, 1.0f, 0.01f);
                }
            }
            break;
        case 2: {
            joint_matrix(i_this, joint);
            MtxPosition(&direction->value, &target->value);
            if (joint == 15)
                target->value.y = (f32)target->value.y - (REG6_F(13) + 20.0f);
            cLib_addCalc2(&part->mPartPos.x, (f32)target->value.x, 1.0f,
                          std::fabs((f32)part->mPartVelocity.x) + 5.0f);
            cLib_addCalc2(&part->mPartPos.y, (f32)target->value.y, 1.0f,
                          std::fabs((f32)part->mPartVelocity.y) + 5.0f);
            cLib_addCalc2(&part->mPartPos.z, (f32)target->value.z, 1.0f,
                          std::fabs((f32)part->mPartVelocity.z) + 5.0f);
            cXyz___mi(&part->mPartPos, &difference->value, &target->value);
            copyVectorWords(gabi::ea(&direction->value), gabi::ea(&difference->value));
            csXyz___apl(&part->mPartRot, &part->mPartRotAdd);
            if (!(magnitude(&direction->value) > 5.0f)) {
                part->mPartState = 0;
                part->m05 = 0;
                if (i_this->m0ED2 < 25 && joint != 11)
                    ++i_this->m0ED2;
                if (joint == 11)
                    i_this->m1DD0 = 10;
                if (joint == 15)
                    i_this->m1DD8 = 0;
            }
            break;
        }
        case 5: {
            f32 floor = part_posmove(i_this, part) + 10.0f;
            if (std::fabs((f32)i_this->home.pos.y - (f32)part->mPartPos.y) > 2000.0f) {
                floor = i_this->current.pos.y;
                copyVectorWords(gabi::ea(&part->mPartPos), gabi::ea(&i_this->current.pos));
            }
            if (!(part->mPartPos.y > floor)) {
                part->mPartPos.y = floor;
                if (joint == 11 && weapon && i_this->m1DD0 >= 2) {
                    fopAcM_cancelCarryNow(at<void>(weapon));
                    i_this->m1DD0 = 0;
                }
                if (part->mPartVelocity.y < -40.0f) {
                    part->mPartVelocity.y = (f32)(random_positive(10.0f) + 20.0f);
                    f64 random = random_signed(0.3f);
                    part->mPartVelocity.x = (f32)((f32)part->mPartVelocity.x * random);
                    random = random_signed(0.3f);
                    part->mPartVelocity.z = (f32)((f32)part->mPartVelocity.z * random);
                } else {
                    part->mPartVelocity.x = 0.0f;
                    part->mPartVelocity.y = 0.0f;
                    part->mPartVelocity.z = 0.0f;
                    part->mPartRotAdd.x = 0;
                    part->mPartRotAdd.y = 0;
                    part->mPartRotAdd.z = 0;
                    cLib_addCalcAngleS2(&part->mPartRot.x, (s16)-0x8000, 1, 0xC00);
                    cLib_addCalcAngleS2(&part->mPartRot.y, 0, 1, 0xC00);
                }
            }
            s8 command = i_this->m0ED3;
            if (command == 2 || command == 11) {
                part->m3E = (s16)gabi::ftoi(random_positive(65536.0f));
                part->m44 = 0.0f;
                if (joint == 11) {
                    if (weapon && !((u32)stActor(weapon).actor_status & 0x2000)) {
                        fopAcM_setCarryNow(at<void>(weapon), 0);
                        copyVectorWords(gabi::ea(&part->mPartPos),
                                        gabi::ea(&stActor(weapon).current.pos));
                        i_this->m1DD0 = 2;
                        part->mWaitTimer = 0;
                        part->mPartRotAdd.y = 0x800;
                        part->mPartRotAdd.x = 0;
                        part->mPartState = 1;
                        if (i_this->m1DC8) {
                            i_this->mActionState = ST_ACTION_FIGHT_UNARMED;
                            i_this->m02C4 = 50;
                        } else {
                            i_this->mActionState = ST_ACTION_BREAK_WAIT;
                            i_this->m02C4 = 3;
                            i_this->m02F8 = 500;
                            anm_init(i_this, dRes_INDEX_ST_BCK_FUKKI_WAIT_e, 10.0f, 2, 1.0f, -1);
                        }
                    }
                } else {
                    part->mPartState = 1;
                    part->mPartRotAdd.x = (s16)gabi::ftoi(random_signed(4000.0f));
                    part->mPartRotAdd.y = (s16)gabi::ftoi(random_signed(4000.0f));
                    part->mWaitTimer = (s8)gabi::ftoi((f32)(random_positive(50.0f) + 30.0f));
                }
            }
            break;
        }
        case 6: {
            if (!i_this->m1DDC && !(part->mPartVelocity.y > 0.0f)) {
                part->m3E = (s16)gabi::ftoi(random_positive(65536.0f));
                part->m44 = 0.0f;
                part->mPartState = 1;
                part->mWaitTimer = 0;
                break;
            }
            f32 floor = part_posmove(i_this, part);
            if (std::fabs((f32)i_this->home.pos.y - (f32)part->mPartPos.y) > 2000.0f) {
                floor = i_this->current.pos.y;
                copyVectorWords(gabi::ea(&part->mPartPos), gabi::ea(&i_this->current.pos));
                part->mPartVelocity.y = 0.0f;
            }
            if (!(part->mPartPos.y > floor)) {
                part->mPartPos.y = floor;
                if (part->mPartVelocity.y < -10.0f) {
                    part->mWaitTimer = (s8)gabi::ftoi((f32)(random_positive(10.0f) + 2.0f));
                    head_anm_init(i_this, dRes_INDEX_ST_BCK_HEADB_JUMP2_e, 1.0f, 0, 1.0f, -1);
                    sound_at(i_this, 0x5896, &i_this->eyePos);
                }
                if (part->mWaitTimer) {
                    part->mPartVelocity.x = 0.0f;
                    part->mPartVelocity.y = 0.0f;
                    part->mPartVelocity.z = 0.0f;
                    if (part->mWaitTimer == 1) {
                        cXyz___mi(&i_this->current.pos, &difference->value, &part->mPartPos);
                        copyVectorWords(gabi::ea(&direction->value), gabi::ea(&difference->value));
                        s16 angle =
                            cM_atan2s<s16>((f32)direction->value.x, (f32)direction->value.z);
                        angle = (s16)(angle + (s16)gabi::ftoi(random_signed(20000.0f)));
                        i_this->m1DDA = angle;
                        mDoMtx_YrotS(stCalcMatrix().get(), angle);
                        direction->value.x = 0.0f;
                        f64 random = random_positive(35.0f);
                        direction->value.y = (f32)((f32)(random + 20.0f) + REG13_F(0));
                        direction->value.z = REG13_F(1) + 20.0f;
                        MtxPosition(&direction->value, &part->mPartVelocity);
                        head_anm_init(i_this, dRes_INDEX_ST_BCK_HEADB_JUMP1_e, 1.0f, 0, 1.0f, -1);
                        u32 actorID = word(gabi::ea(i_this) + 4);
                        s32 reverb = stGetReverbWithLinkage<s32>((s8)i_this->current.roomNo);
                        mDoAud_monsSeStart(0x487D, &i_this->eyePos, actorID, 0, reverb);
                    }
                }
                cLib_addCalcAngleS2(&part->mPartRot.x, 0, 2, 0xC00);
                cLib_addCalcAngleS2(&part->mPartRot.y, (s16)i_this->m1DDA, 2, 0xC00);
            } else {
                s32 frame = gabi::ftoi(stMorf(gabi::ea((void *)i_this->mpMorf2)).mFrame);
                if (frame == 5) {
                    u32 emitter = particle(0x8179, &part->mPartPos);
                    if (emitter) {
                        u32 matrix = animation_matrix(gabi::ea((void *)i_this->mpMorf2), 1);
                        JPASetRMtxTVecfromMtx(at<void>(matrix), &stEmitter(emitter).mGlobalRotation,
                                              &stEmitter(emitter).mGlobalTranslation);
                    }
                }
            }
            break;
        }
        case 7: {
            if (!i_this->m1DDC && !(part->mPartVelocity.y > 0.0f)) {
                part->m3E = (s16)gabi::ftoi(random_positive(65536.0f));
                part->m44 = 0.0f;
                part->mPartState = 1;
                part->mWaitTimer = 0;
            } else {
                f32 floor = part_posmove(i_this, part);
                if (std::fabs((f32)i_this->home.pos.y - (f32)part->mPartPos.y) > 2000.0f) {
                    floor = i_this->current.pos.y;
                    copyVectorWords(gabi::ea(&part->mPartPos), gabi::ea(&i_this->current.pos));
                    part->mPartVelocity.y = 0.0f;
                }
                if (!(part->mPartPos.y > floor)) {
                    part->mPartPos.y = floor;
                    if (animation_stopped(i_this) || part->mPartVelocity.y < -20.0f)
                        head_anm_init(i_this, dRes_INDEX_ST_BCK_HEADB_WAIT_e, 1.0f, 2, 1.0f, -1);
                    part->mPartVelocity.x = 0.0f;
                    part->mPartVelocity.y = 0.0f;
                    part->mPartVelocity.z = 0.0f;
                }
                if (!part->mWaitTimer)
                    part->mPartState = 6;
            }
            break;
        }
        case 8: {
            i_this->m02F6 = 10;
            f32 floor = part_posmove(i_this, part) + 20.0f;
            if (std::fabs((f32)i_this->home.pos.y - (f32)part->mPartPos.y) > 3000.0f)
                floor = part->mPartPos.y;
            if (!(!(part->mPartPos.y > floor)))
                break;
            part->mPartPos.y = floor;
            for (u32 i = 0; i < 26; ++i) {
                if (i != 11 && gabi::ea((void *)i_this->mParts[i].mpPartModel)) {
                    if (i == 15)
                        fopAcM_createDisappear(i_this, &i_this->mParts[i].mPartPos, 5, 0, 0xFF);
                    else
                        particle(0x817E, &i_this->mParts[i].mPartPos);
                }
            }
            i_this->mCountdownToDeath = 5;
            if (i_this->mDeathSwitch)
                dSv_info_c_onSwitch(at<void>((u32)stSaveContext() + 0x20), (u8)i_this->mDeathSwitch,
                                    (s8)i_this->current.roomNo);
            dSv_info_c_onActor(at<void>((u32)stSaveContext() + 0x20), (u16)i_this->setID,
                               (s8)i_this->home.roomNo);
            u32 held = find_actor(i_this->mHeldWeaponEntityId);
            if (held)
                fopAcM_cancelCarryNow(at<void>(held));
            return;
        }
        case 10:
            if (i_this->m0ED3 == 11) {
                u32 upper = find_actor(i_this->mUpperBodyEntityId);
                if (upper) {
                    part->mPartState = 1;
                    copyVectorWords(gabi::ea(&part->mPartPos), upper + 0xFF8 + (u32)joint * 0x6C);
                    if (joint != 15) {
                        part->mPartRotAdd.x = (s16)gabi::ftoi(random_signed(4000.0f));
                        part->mPartRotAdd.y = (s16)gabi::ftoi(random_signed(4000.0f));
                    } else {
                        gabi::Local<AngleLocal> zero;
                        u32 result = csXyz_csXyz<u32>(&zero->value, 0, 0, 0);
                        u16 x = *at<be<u16>>(result), y = *at<be<u16>>(result, 2),
                            z = *at<be<u16>>(result, 4);
                        part->mPartRotAdd.x = x;
                        part->mPartRotAdd.y = y;
                        part->mPartRotAdd.z = z;
                        part->mPartRot.x = x;
                        part->mPartRot.y = y;
                        part->mPartRot.z = z;
                    }
                    part->m3E = (s16)gabi::ftoi(random_positive(65536.0f));
                    part->mWaitTimer = 0;
                    part->m44 = 0.5f;
                    part->m05 = 1;
                }
            }
            break;
        }
    }
    switch ((s8)part->m05) {
    case 0: {
        joint_matrix(i_this, joint);
        s8 shake = i_this->m1DE4;
        if (shake) {
            f32 amplitude = (f32)shake * (REG8_F(7) + 2.0f);
            u32 count = i_this->m02E8;
            f32 x = angle_sine(count * 0x3800 + (u32)joint * 0x3000) * amplitude;
            f32 z = angle_cosine(count * 0x3A00 + (u32)joint * 0x3100) * amplitude;
            f32 y = angle_cosine(count * 0x3C00 + (u32)joint * 0x2800) * amplitude;
            MtxTrans(1, x, y, z);
        }
        copy_model_matrix(gabi::ea((void *)part->mpPartModel), stCalcMatrix().v.get());
        if (joint == 11 && weapon && i_this->m1DD0) {
            s8 state = i_this->m1DD0;
            if (state == 1) {
                fopAcM_setCarryNow(at<void>(weapon), 0);
                i_this->m1DD0 = 10;
                return;
            } else if (state >= 2 && ((u32)stActor(weapon).actor_status & 0x2000)) {
                mDoMtx_YrotM(stCalcMatrix().get(), (s16)(REG8_S(1) + 16000));
                mDoMtx_XrotM(stCalcMatrix().get(), (s16)REG8_S(2));
                mDoMtx_ZrotM(stCalcMatrix().get(), (s16)REG8_S(3));
                MtxTrans(1, (f32)REG8_F(9), (f32)REG8_F(10), REG8_F(11) + 65.0f);
                if (i_this->m02B9)
                    MtxTrans(0, 20000.0f, 20000.0f, 20000.0f);
                u32 model = stHeldWeapon(weapon).mpModel.v.get();
                if (model)
                    copy_model_matrix(model, stCalcMatrix().v.get());
            }
        }
        if (joint == 15) {
            csXyz___pl(&i_this->shape_angle, &i_this->m02FC);
            // csXyz value return occupies r3/r4 in the guest ABI.
            u32 first = gabi::cpu->r[3], second = gabi::cpu->r[4];
            part->mPartRot.x = first >> 16;
            part->mPartRot.y = first;
            part->mPartRot.z = second >> 16;
        }
        break;
    }
    case 1: {
        MtxTrans(0, (f32)part->mPartPos.x, (f32)part->mPartPos.y, (f32)part->mPartPos.z);
        mDoMtx_YrotM(stCalcMatrix().get(), (s16)part->mPartRot.y);
        mDoMtx_XrotM(stCalcMatrix().get(), (s16)part->mPartRot.x);
        f32 scaleX = i_this->mEnemyIce.mScaleXZ, scaleY = i_this->mEnemyIce.mScaleY;
        MtxScale(1, scaleX, scaleY, scaleX);
        if (joint == 15 && i_this->m1DD8) {
            auto &shakeTimer = i_this->m1E82;
            if (shakeTimer) {
                --shakeTimer;
                f32 amplitude = (f32)(s16)shakeTimer * (REG8_F(5) + 1.0f);
                u32 count = i_this->m02E8;
                MtxTrans(1, amplitude * angle_sine(count * 0x3800), 0.0f,
                         amplitude * angle_cosine(count * 0x3C00));
            }
            u32 model = stMorf(gabi::ea((void *)i_this->mpMorf2)).mpModel.v.get();
            copy_model_matrix(model, stCalcMatrix().v.get());
            i_this->m1DD8 = 2;
            if (i_this->mEnemyIce.mLightShrinkTimer) {
                i_this->mEnemyIce.mYOffset = REG8_F(3) + 10.0f;
                copyVectorWords(gabi::ea(&i_this->current.pos), gabi::ea(&part->mPartPos));
            }
        } else
            copy_model_matrix(gabi::ea((void *)part->mpPartModel), stCalcMatrix().v.get());
        if (weapon && joint == 11 && i_this->m1DD0 >= 2) {
            u32 model = stHeldWeapon(weapon).mpModel.v.get();
            if (model)
                copy_model_matrix(model, stCalcMatrix().v.get());
        }
        break;
    }
    }
    if (joint == 15) {
        if (!i_this->m1E86) {
            copyVectorWords(gabi::ea(&i_this->eyePos), gabi::ea(&part->mPartPos));
            i_this->attention_info.position.x = part->mPartPos.x;
            i_this->attention_info.position.y = part->mPartPos.y;
            i_this->attention_info.position.z = part->mPartPos.z;
            i_this->attention_info.position.y = (f32)i_this->attention_info.position.y + 30.0f;
        }
        if (i_this->m1DD8 == 2) {
            u32 matrix = animation_matrix(gabi::ea((void *)i_this->mpMorf2), 2);
            PSMTXCopy(at<void>(matrix), stCalcMatrix().get());
            direction->value.x = REG0_F(4);
            direction->value.y = REG0_F(5);
            direction->value.z = REG0_F(6);
        } else {
            u32 model = gabi::ea((void *)part->mpPartModel);
            PSMTXCopy(at<void>(model ? model + 0xC8 : 0), stCalcMatrix().get());
            direction->value.x = REG0_F(4) - 5.0f;
            direction->value.y = REG0_F(5) - 25.0f;
            direction->value.z = REG0_F(6);
        }
        for (u32 i = 0; i < 3; ++i) {
            auto *chain = &i_this->mKe[i];
            MtxPosition(&direction->value, &chain->m000[0]);
            for (u32 k = 0; k < 3; ++k)
                *at<be<u16>>(gabi::ea(&chain->m0F0), k * 2) =
                    *at<be<u16>>(gabi::ea(&part->mPartRot), k * 2);
            chain->m0F0.y = (s16)((s16)chain->m0F0.y + ((s32)i - 1) * 0x800);
            if (i == 1)
                chain->m0F0.x = (s16)((s16)chain->m0F0.x + 0x500);
        }
    }
}

/* inlined into daSt_Execute */
void cc_set(st_class *i_this) {
    gabi::Local<VectorLocal> zero, center, disabled, disabledWeapon;
    zero->value.x = 0.0f;
    zero->value.y = 0.0f;
    zero->value.z = 0.0f;
    disabled->value.x = -50000.0f;
    disabled->value.y = -50000.0f;
    disabled->value.z = 0.0f;
    disabledWeapon->value.x = 50000.0f;
    disabledWeapon->value.y = -50000.0f;
    disabledWeapon->value.z = 0.0f;
    for (u32 index = 0; index < 7; ++index) {
        bool active = false;
        if (i_this->m0ED0 != 2) {
            if (i_this->mActionState == ST_ACTION_LOWER_BODY || i_this->m02C4 == 35)
                active = !i_this->m0ED0 && index >= 5;
            else if (i_this->m0ED0)
                active = index == 0;
            else
                active = i_this->mActionState != ST_ACTION_UPPER_BODY || index < 5;
        }
        u32 sphere = gabi::ea(&i_this->m054C[index]);
        if (active) {
            if (!index && i_this->m1DD8) {
                u32 model = stMorf(gabi::ea((void *)i_this->mpMorf2)).mpModel.v.get();
                PSMTXCopy(at<void>(model ? model + 0xC8 : 0), stCalcMatrix().get());
            } else
                joint_matrix(i_this, (s32)cc_j_no.values[index]);
            MtxPosition(&zero->value, &center->value);
            cM3dGSph_SetC(&stSphere(sphere).mSph, &center->value);
            cM3dGSph_SetR(&stSphere(sphere).mSph, index ? 25.0f : 45.0f);
            stSphere(sphere).mObjTg.mSPrm = (u32)stSphere(sphere).mObjTg.mSPrm | 1;
        } else {
            cM3dGSph_SetR(&stSphere(sphere).mSph, -500.0f);
            cM3dGSph_SetC(&stSphere(sphere).mSph, &disabled->value);
        }
        collision_register(sphere);
    }
    u32 weapon = gabi::ea(&i_this->m0D80);
    if (i_this->m0ED1 && !i_this->m0ED0) {
        if (i_this->m1DC8) {
            copyVectorWords(gabi::ea(&center->value), gabi::ea(i_this) + 0x1FCC);
            cM3dGSph_SetR(&stSphere(weapon).mSph, 30.0f);
        } else if (i_this->m0ED1 == 3) {
            joint_matrix(i_this, ST_JNT_ASIR_e);
            zero->value.x = 0.0f;
            zero->value.y = 0.0f;
            zero->value.z = 0.0f;
            MtxPosition(&zero->value, &center->value);
            cM3dGSph_SetR(&stSphere(weapon).mSph, 30.0f);
            stSphere(weapon).mGObjAt.mSpl = 0;
        } else {
            stSphere(weapon).mGObjAt.mSpl = 6;
            joint_matrix(i_this, ST_JNT_BUKI_e);
            f32 factor = 1.0f;
            if (i_this->m0ED1 == 2) {
                f32 blend = std::fabs((f32)i_this->m0308);
                if (blend > 0.5f)
                    stSphere(weapon).mGObjAt.mSpl = 7;
                factor = blend * 1.5f;
            }
            bool odd = (u32)i_this->m02E8 & 1;
            zero->value.x = REG0_F(10) + (odd ? 80.0f : 30.0f);
            cM3dGSph_SetR(&stSphere(weapon).mSph, factor * (odd ? 70.0f : 50.0f));
            zero->value.y = 0.0f;
            zero->value.z = 0.0f;
            MtxPosition(&zero->value, &center->value);
        }
        if (i_this->m1E87)
            dCcD_Sph_MoveCAt(&i_this->m0D80, &center->value);
        else {
            dCcD_Sph_StartCAt(&i_this->m0D80, &center->value);
            ++i_this->m1E87;
        }
        stSphere(weapon).mObjAt.mSPrm = (u32)stSphere(weapon).mObjAt.mSPrm | 1;
    } else {
        i_this->m1E87 = 0;
        cM3dGSph_SetC(&stSphere(weapon).mSph, &disabledWeapon->value);
    }
    collision_register(weapon);
    i_this->m0ED0 = 0;
    i_this->m0ED1 = 0;
}

/* inlined into daSt_Execute */
void eff_move(st_class *i_this) {
    if (i_this->m1F60)
        --i_this->m1F60;
    if (i_this->m1F68 == 1) {
        i_this->m1F64 = 1.0f;
        if (!i_this->m1F60) {
            i_this->m1F60 = 15;
            for (u32 index = 0; index < 20; ++index)
                if (!i_this->m1EE8[index]) {
                    u32 emitter = particle(0x817B, &i_this->current.pos);
                    i_this->m1EE8[index] = at<void>(emitter);
                    if (emitter) {
                        stEmitter(emitter).mFlags = (u32)stEmitter(emitter).mFlags | 0x40;
                        i_this->m1F38[index] = 200;
                    }
                    break;
                }
        }
    }
    if (i_this->m1F68 == 2)
        cLib_addCalc0(&i_this->m1F64, 1.0f, 0.1f);
    for (u32 index = 0; index < 20; ++index) {
        u32 emitter = gabi::ea((void *)i_this->m1EE8[index]);
        if (emitter) {
            stEmitter(emitter).mGlobalParticleScale.x = i_this->m1F64;
            if (i_this->m1F38[index] && (--i_this->m1F38[index], !i_this->m1F38[index])) {
                emitter = gabi::ea((void *)i_this->m1EE8[index]);
                stEmitter(emitter).mMaxFrame = 0xFFFFFFFF;
                stEmitter(emitter).mFlags = (u32)stEmitter(emitter).mFlags | 1;
                i_this->m1EE8[index] = nullptr;
            }
        }
    }
}

/* 02493FB0 */
s32 daSt_Execute(st_class *i_this) {
    stGetGameInfoWithLinkage<u32>();
    if (i_this->mCountdownToDeath) {
        --i_this->mCountdownToDeath;
        if (!i_this->mCountdownToDeath)
            fopAcM_delete(i_this);
        return 1;
    }
    if (i_this->m02FA || ((f32)i_this->home.pos.y - (f32)i_this->current.pos.y > 4000.0f)) {
        fopAcM_delete(i_this);
        return 1;
    }
    if (i_this->m2364 && (--i_this->m2364, !i_this->m2364))
        i_this->actor_status = (u32)i_this->actor_status & ~0x4000u;
    if (enemy_ice<s32>(&i_this->mEnemyIce)) {
        u32 model = stMorf(gabi::ea((void *)i_this->mpMorf)).mpModel.v.get();
        MtxTrans(0, (f32)i_this->current.pos.x, (f32)i_this->current.pos.y,
                 (f32)i_this->current.pos.z);
        mDoMtx_YrotM(stCalcMatrix().get(), (s16)((s16)i_this->shape_angle.y + (s16)i_this->m0304));
        f32 horizontal = i_this->mEnemyIce.mScaleXZ, vertical = i_this->mEnemyIce.mScaleY;
        MtxScale(1, horizontal, vertical, horizontal);
        copy_model_matrix(model, stCalcMatrix().v.get());
        mDoExt_McaMorf_calc((void *)i_this->mpMorf);
        mDoExt_McaMorf_calc((void *)i_this->mpMorf2);
        move_visible_parts(i_this);
        return 1;
    }
    another_hit = 0;
    if (i_this->mUpperBodyEntityId != 0xFFF) {
        u32 upper = find_actor(i_this->mUpperBodyEntityId);
        if (upper && *at<be<s8>>(upper, 0x20CD)) {
            fopAcM_delete(i_this);
            for (u32 index = 0; index < 26; ++index)
                if (i_this->mParts[index].mPartState < 10)
                    particle(0x817E, &i_this->mParts[index].mPartPos);
            u32 weapon = find_actor(i_this->mHeldWeaponEntityId);
            if (weapon)
                fopAcM_cancelCarryNow(at<void>(weapon));
            return 1;
        }
    }
    ++i_this->m02E8;
    for (u32 index = 0; index < 5; ++index)
        if (i_this->mTimers[index])
            --i_this->mTimers[index];
    if (i_this->m02F6)
        --i_this->m02F6;
    if (i_this->m1DDC)
        --i_this->m1DDC;
    St_move(i_this);
    damage_check(i_this);
    cLib_addCalcAngleS2(&i_this->shape_angle.y,
                        (s16)((s16)i_this->current.angle.y + (s16)i_this->m0302), 2, 0x1000);
    i_this->m0302 = 0;
    mDoExt_McaMorf_play((void *)i_this->mpMorf, &i_this->eyePos, 0, 0);
    mDoExt_McaMorf_play((void *)i_this->mpMorf2, &i_this->eyePos, 0, 0);
    u32 model = stMorf(gabi::ea((void *)i_this->mpMorf)).mpModel.v.get();
    f32 sx = i_this->scale.x, sy = i_this->scale.y, sz = i_this->scale.z;
    stModel(model).mBaseScale.x = sx;
    stModel(model).mBaseScale.y = sy;
    stModel(model).mBaseScale.z = sz;
    MtxTrans(0, (f32)i_this->current.pos.x, (f32)i_this->current.pos.y, (f32)i_this->current.pos.z);
    mDoMtx_YrotM(stCalcMatrix().get(), (s16)((s16)i_this->shape_angle.y + (s16)i_this->m0304));
    copy_model_matrix(model, stCalcMatrix().v.get());
    mDoExt_McaMorf_calc((void *)i_this->mpMorf);
    mDoExt_McaMorf_calc((void *)i_this->mpMorf2);
    cc_set(i_this);
    for (u32 joint = 0; joint < 26; ++joint)
        if (i_this->mParts[joint].mpPartModel && (!i_this->m1DC8 || (joint != 3 && joint != 4)))
            part_move(i_this, joint);
    if (i_this->m0ED3 != 2)
        i_this->m0ED3 = 0;
    cLib_addCalcAngleS2(&i_this->m0304, 0, 2, 0x40);
    if (i_this->mActionState != ST_ACTION_LOWER_BODY && i_this->m02C4 != 35)
        ke_control(i_this);
    if (i_this->m1DC8)
        nun_control(i_this);
    eff_move(i_this);
    spin_smoke_move(i_this);
    u32 environment = dKy_getEnvlight<u32>();
    dScnKy_env_light_c_settingTevStruct(at<void>(environment), 0, &i_this->current.pos,
                                        &i_this->tevStr);
    return 1;
}

/* 024957AC */
BOOL daSt_IsDelete(st_class *i_this) { return 1; }

/* 024957B4 */
BOOL daSt_Delete(st_class *i_this) {
    dComIfG_resDelete(&i_this->mPhase, stArcDelete);
    if (i_this->m02FB) {
        s8 child = l_HIO.mNo;
        hio_set = 0;
        mDoHIO_subRoot_c_deleteChild(child);
    }
    for (u32 i = 0; i < 3; ++i) {
        u32 callback = gabi::ea(i_this) + 0x20D0 + i * 0x20;
        callVirtualWithLinkage(word(word(callback) + 0x44), at<void>(callback));
    }
    u32 callback = gabi::ea(i_this) + 0x21D0;
    callVirtualWithLinkage(word(word(callback) + 0x44), at<void>(callback));
    for (u32 i = 0; i < 20; ++i) {
        u32 emitter = gabi::ea((void *)i_this->m1EE8[i]);
        if (emitter) {
            u32 flags = stEmitter(emitter).mFlags;
            stEmitter(emitter).mMaxFrame = 0xFFFFFFFF;
            stEmitter(emitter).mFlags = flags | 1;
            i_this->m1EE8[i] = nullptr;
        }
    }
    return 1;
}

/* 024958A0 */
BOOL createHeap(st_class *i_this) {
    gabi::Local<ResourceLocal> bodyModelName, bodyAnimationName, headModelName, headAnimationName,
        partModelName;
    u32 modelData = resource(bodyModelName, stArcHeap, dRes_INDEX_ST_BDL_ST_e);
    u32 animationData = resource(bodyAnimationName, stArcHeap, dRes_INDEX_ST_BCK_WAIT_e);
    u32 animation = mDoExt_McaMorf_mDoExt_McaMorf<u32>(
        (void *)nullptr, at<void>(modelData), (void *)nullptr, (void *)nullptr,
        at<void>(animationData), 2, 1.0f, (void *)nullptr, -1, 1, 0, 0x80000, 0x11000022);
    i_this->mpMorf = at<void>(animation);
    if (!animation || !stMorf(animation).mpModel.v.get())
        return 0;
    modelData = resource(headModelName, stArcHeap, dRes_INDEX_ST_BDL_HEADB_e);
    animationData = resource(headAnimationName, stArcHeap, dRes_INDEX_ST_BCK_HEADB_WAIT_e);
    animation = mDoExt_McaMorf_mDoExt_McaMorf<u32>(
        (void *)nullptr, at<void>(modelData), (void *)nullptr, (void *)nullptr,
        at<void>(animationData), 2, 1.0f, (void *)nullptr, -1, 1, 0, 0x80000, 0x11000022);
    i_this->mpMorf2 = at<void>(animation);
    if (!animation || !stMorf(animation).mpModel.v.get())
        return 0;
    for (u32 i = 0; i < 26; ++i) {
        u16 modelIndex = st_part_bdl.values[i];
        if (modelIndex) {
            u32 data = resource(partModelName, stArcHeap, modelIndex);
            u32 model = mDoExt_J3DModel__create<u32>(at<void>(data), 0x80000, 0x11000022);
            i_this->mParts[i].mpPartModel = at<void>(model);
            if (!model)
                return 0;
        }
    }
    return mDoExt_3DlineMat0_c_init<s32>(&i_this->mLineMat, 3, 10, 0) != 0;
}

/* 02495C70 */
cPhs_State daSt_Create(st_class *i_this) {
    u32 condition = i_this->actor_condition;
    if (!(condition & 8)) {
        if (i_this)
            actor_construct(i_this);
        i_this->actor_condition = (u32)i_this->actor_condition | 8;
    }
    s32 phase = dComIfG_resLoad<s32>(&i_this->mPhase, stArcCreate);
    if (phase != 4)
        return phase;
    for (u32 i = 0; i < 2; ++i)
        i_this->m1E88[i].m12 = 1;
    u32 parameters = i_this->mParameters;
    s16 deathSwitch = i_this->current.angle.z;
    i_this->gbaName = 4;
    i_this->mAmbushSightRange = parameters >> 8;
    i_this->mBehaviorType = parameters & 0xF;
    i_this->mDeathSwitch = deathSwitch;
    i_this->mUnusedParam = parameters >> 16;
    i_this->mAmbushSwitch = parameters >> 24;
    i_this->current.angle.z = 0;
    u8 switchID = (u8)deathSwitch;
    if (switchID) {
        if (switchID <= 0x7F)
            i_this->actor_status = (u32)i_this->actor_status | 0x04000000;
        if (switchID == 0xFF)
            i_this->mDeathSwitch = 0;
        else if (dSv_info_c_isSwitch<s32>(at<void>((u32)stSaveContext() + 0x20), switchID,
                                          (s8)i_this->current.roomNo))
            return 5;
    }
    u32 play = stGetGameInfoWithLinkage<u32>();
    i_this->itemTableIdx =
        cDT_NamePTbl_GetIndex<s32>(at<StGameInfoView>(play)->mNameTable, stItemTableName, 0);
    if (!fopAcM_entrySolidHeap<s32>(i_this, at<void>(gabi::guest_fn((const void *)createHeap)), 0))
        return 5;
    if (!hio_set) {
        s32 child = mDoHIO_subRoot_c_createChild<s32>(stHioName, &l_HIO);
        l_HIO.mNo = child;
        i_this->m02FB = 1;
        hio_set = 1;
    }
    u32 body = gabi::ea((void *)i_this->mpMorf);
    if (!body || !stMorf(body).mpModel.v.get())
        return 5;
    u8 behavior = i_this->mBehaviorType;
    if (behavior == 1 || behavior == 2 || behavior == 3) {
        i_this->mActionState = behavior == 1 ? 0xF : 0x10;
        u8 ambushSwitch = i_this->mAmbushSwitch;
        i_this->m02B9 = ambushSwitch == 0xFF ? 0xFF : (u8)(ambushSwitch + 1);
    }
    i_this->actor_status = (u32)i_this->actor_status | 0x4000;
    i_this->m2364 = 5;
    u32 model = stMorf(gabi::ea((void *)i_this->mpMorf)).mpModel.v.get();
    *at<be<u32>>(model, 0xB8) = gabi::ea(i_this);
    i_this->mBtBodyR = 125.0f;
    i_this->mBtHeight = 162.5f;
    i_this->actor_status = (u32)i_this->actor_status | 0x20;
    i_this->attention_info.flags = 4;
    dBgS_Acch_Set(&i_this->mAcch, &i_this->current.pos, &i_this->old.pos, i_this, 1,
                  &i_this->mAcchCir, &i_this->speed, (void *)nullptr, (void *)nullptr);
    dBgS_AcchCir_SetWall(&i_this->mAcchCir, 60.0f, 100.0f);
    dCcD_Stts_Init(&i_this->mStts, 0x32, 2, i_this);
    for (u32 i = 0; i < 7; ++i) {
        dCcD_Sph_Set(&i_this->m054C[i], (i ? &body_sph_src : &head_sph_src));
        i_this->m054C[i].mStts = &i_this->mStts;
    }
    dCcD_Sph_Set(&i_this->m0D80, &wepon_sph_src);
    i_this->m0D80.mStts = &i_this->mStts;
    if (i_this->mBehaviorType != 14) {
        u32 weapon = fopAcM_create<u32>(0x1CF, 2, &i_this->current.pos, (s8)i_this->current.roomNo,
                                        (void *)nullptr, (void *)nullptr, -1, (void *)nullptr);
        i_this->mEnemyIce.mpActor = i_this;
        i_this->health = 16;
        i_this->m1E84 = 16;
        i_this->max_health = 16;
        i_this->m0ED2 = 0x19;
        i_this->mHeldWeaponEntityId = weapon;
        i_this->mUpperBodyEntityId = 0xFFF;
        i_this->m1DD0 = 1;
    } else {
        i_this->health = 16;
        i_this->m1E84 = 5;
        i_this->mActionState = ST_ACTION_UPPER_BODY;
        i_this->mUpperBodyEntityId = 0xFFF;
        i_this->mEnemyIce.mpActor = i_this;
        i_this->m0ED3 = 0x14;
        i_this->max_health = 16;
        i_this->m0ED2 = 0x19;
    }
    i_this->mEnemyIce.mDeathSwitch = i_this->mDeathSwitch;
    i_this->stealItemLeft = 3;
    i_this->m02F6 = 15;
    native_daSt_Execute(i_this);
    return phase;
}

// HD: compiler-generated and template code of this translation unit.

/* 02495AC8 */
u32 smoke_construct(void *callback) {
    return dPa_smokeEcallBack_dPa_smokeEcallBack<u32>(
        callback, 1); /* dPa_smokeEcallBack ctor, returns this */
}

/* 024961A4 */
void hio_destroy(void *self, u32 flags) {
    if (self && (flags & 1))
        __dl(self);
}

/* 0249A0FC */
void smoke_destroy(void *self, u32 flags) {
    if (self && (flags & 1))
        __dl(self);
}

/* 0249A110 */
void part_destroy(st_p *self, u32 flags) {
    if (self && (flags & 1))
        __dl(self);
}

/* 0249A0B0 */
st_p *part_construct(st_p *part) {
    if (!part)
        part = at<st_p>(operator_new<u32>(0x6C));
    if (part)
        dPa_smokeEcallBack_dPa_smokeEcallBack(&part->m48, 1);
    return part;
}

/* 02495AD0 */
st_class *actor_construct(st_class *i_this) {
    if (!i_this)
        i_this = at<st_class>(operator_new<u32>(0x25B0));
    if (!i_this)
        return i_this;
    fopAc_ac_c_fopAc_ac_c(i_this);
    i_this->__vtbl = 0x1003D5BC;
    dBgS_AcchCir_dBgS_AcchCir(&i_this->mAcchCir);
    dBgS_Acch_dBgS_Acch(&i_this->mAcch);
    i_this->mAcch.cBgS_Chk::__vtbl = 0x1003D54C;
    i_this->mAcch.mbObjThrough = 1;
    i_this->mAcch.dBgS_GrpPassChk::__vtbl = 0x1003D55C;
    i_this->mAcch.dBgS_PolyPassChk::__vtbl = 0x1003D56C;
    cCcD_Stts_construct(&i_this->mStts);
    dCcD_GStts_dCcD_GStts(&i_this->mStts.__vtbl_gstts);
    i_this->mStts.__vtbl = 0x1004AE88;
    i_this->mStts.__vtbl_gstts = 0x1004AEC0;
    __construct_array(&i_this->m054C, 7, 0x12C, at<void>(0x025166F0));
    dCcD_Sph_construct(&i_this->m0D80);
    __construct_array(&i_this->mParts, 26, 0x6C,
                      at<void>(gabi::guest_fn((const void *)part_construct)));
    mDoExt_3DlineMat0_c_construct(&i_this->mLineMat);
    __construct_array(&i_this->m1E88, 3, 0x20,
                      at<void>(gabi::guest_fn((const void *)smoke_construct)));
    dPa_smokeEcallBack_dPa_smokeEcallBack(&i_this->m1F88, 1);
    cCcD_Stts_construct(&i_this->mEnemyIce.mStts);
    dCcD_GStts_dCcD_GStts(&i_this->mEnemyIce.mStts.__vtbl_gstts);
    i_this->mEnemyIce.mStts.__vtbl = 0x1004AE88;
    i_this->mEnemyIce.mStts.__vtbl_gstts = 0x1004AEC0;
    dCcD_GObjInf_dCcD_GObjInf(&i_this->mEnemyIce.mCyl);
    stShapeVtable(i_this->mEnemyIce.mCyl) = 0x100015A8;
    stShapeAuxVtable(i_this->mEnemyIce.mCyl) = 0x1003D4DC;
    cM3dGCyl_construct(&i_this->mEnemyIce.mCyl.mCyl);
    i_this->mEnemyIce.mCyl.__vtbl_hitinf = 0x1004B108;
    i_this->mEnemyIce.mCyl.mCyl.__vtbl = 0x1004B150;
    stShapeVtable(i_this->mEnemyIce.mCyl) = 0x1004B160;
    dBgS_AcchCir_dBgS_AcchCir(&i_this->mEnemyIce.mBgAcchCir);
    dBgS_Acch_dBgS_Acch(&i_this->mEnemyIce.mBgAcch);
    i_this->mEnemyIce.mBgAcch.cBgS_Chk::__vtbl = 0x1003D54C;
    i_this->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = 0x1003D56C;
    i_this->mEnemyIce.mBgAcch.mbObjThrough = 1;
    i_this->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = 0x1003D55C;
    return i_this;
}

/* 0249A124 */
void actor_destroy(st_class *i_this, u32 flags) {
    if (!i_this)
        return;
    i_this->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = 0x1003D55C;
    i_this->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = 0x1003D56C;
    dBgS_Acch_destroy(&i_this->mEnemyIce.mBgAcch, 0);
    cM3dGCir_destroy(&i_this->mEnemyIce.mBgAcchCir.m_cir, 2);
    dCcD_Cyl_destroy(&i_this->mEnemyIce.mCyl, 2);
    dCcD_Stts_destroy(&i_this->mEnemyIce.mStts, 2);
    __destroy_arr(&i_this->m1E88, 3, 0x20, at<void>(gabi::guest_fn((const void *)smoke_destroy)), 0,
                  0);
    mDoExt_3DlineMat0_c_destroy(&i_this->mLineMat, 2);
    __destroy_arr(&i_this->mParts, 26, 0x6C, at<void>(gabi::guest_fn((const void *)part_destroy)),
                  0, 0);
    dCcD_Sph_destroy(&i_this->m0D80, 2);
    __destroy_arr(&i_this->m054C, 7, 0x12C, at<void>(0x02515AE8), 0, 0);
    dCcD_Stts_destroy(&i_this->mStts, 2);
    i_this->mAcch.dBgS_GrpPassChk::__vtbl = 0x1003D55C;
    i_this->mAcch.dBgS_PolyPassChk::__vtbl = 0x1003D56C;
    dBgS_Acch_destroy(&i_this->mAcch, 0);
    cM3dGCir_destroy(&i_this->mAcchCir.m_cir, 2);
    fopAc_ac_c_destroy_fopAc_ac_c(i_this, 0);
    if (flags & 1)
        __dl(i_this);
}

/* 0249A274 */
void hio_message(void *self, void *context) {}

/* 024960B4 */
void static_initialize() {
    l_staticState.mRuntimeWords[2] = 0;
    l_staticState.mRuntimeWords[0] = 0;
    l_staticState.mRuntimeWords[3] = 0;
    l_staticState.mRuntimeWords[1] = 0;
    __register_global_object(&stRegisterPi);
    l_staticState.mMinusPi = -3.1415927410125732f;
    l_staticState.mPi = 3.1415927410125732f;
    runtime_initializeGuardByte(&l_staticState.mByteGuard);
    __register_global_object(&stRegisterByteGuard);
    runtime_initializeGuardWord(&l_staticState.mWordGuard);
    __register_global_object(&stRegisterWordGuard);
    l_HIO.m10 = 6.0f;
    l_HIO.m06 = 9;
    l_HIO.__vtbl = 0x1003D5CC;
    l_HIO.m08 = 0x2C;
    l_HIO.m0A = 0x15;
    l_HIO.m14 = 7.0f;
    l_HIO.m0C = 0x35;
    l_HIO.m18 = 0x46;
    l_HIO.m1A = 200;
}

} // namespace Stalfos
using namespace Stalfos;

// clang-format off
/* WWHD glue
data l_staticState 1046DFD4
data stRegisterPi 101D0FAC
data stRegisterByteGuard 101D0FB8
data stRegisterWordGuard 101D0FC4
data head_sph_src 101D0EEC
data body_sph_src 101D0F2C
data wepon_sph_src 101D0F6C
data stArcAnimation 1003D5E4
data stArcHeadAnimation 1003D5E7
data stArcDelete 1003D6CC
data stArcHeap 1003D6CF
data stArcCreate 1003D6DC
data stItemTableName 1003D6E0
data stHioName 1003D6E8
data l_HIO 1046DFF0
data hio_set 101D0E78
data another_hit 1046DFDC
data st_part_bdl 101D0EB8
data cc_j_no 101D0E9C
*/
