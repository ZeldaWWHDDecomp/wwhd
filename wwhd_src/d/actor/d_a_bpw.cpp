/* d_a_bpw.cpp (WWHD): Jalhalla (big Poe boss).
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_bpw.cpp) to the WWHD layout and code, verified against cking.rpx.
 * Field offsets are the GameCube ones + 0x11C (see d_a_bpw.h); comments name the GameCube
 * fields. "HD:" marks where WWHD's code differs from the GameCube source. */
#include "d/actor/d_a_bpw.h"
#include "gc_api/tu.h"
using namespace gabi;

const gc_tu_data gc_tu = {
    .safeString = 0x1000B07C,
    .objAcch = {0x1000B0B4, 0x1000B0D4, 0x1000B0C4},
    .objAcchDt = {0x1000B0C4, 0x1000B0D4},
    .headerStatics = {0x10462760, 0x1019266C, 0x1046276C},
};

enum Actor_Type {
    ACTOR_TYPE_BODY = 0,
    ACTOR_TYPE_KANTERA = 1,
    ACTOR_TYPE_DAMAGE_BALL = 2,
    ACTOR_TYPE_TORITUKI = 3,
};

enum Damage_Action {
    DAMAGE_ACTION_LINE = 0,
    DAMAGE_ACTION_DAMAGE_BALL = 1,
};

enum Action {
    ACTION_MOVE_DOUSA = 0,
    ACTION_ATTACK_KOUGEKI = 1,
    ACTION_BODY_DOWN_KARADA_TAORE = 2,
    ACTION_DAMAGE = 3,
    ACTION_SEPARATION_BUNRI_DOUSA = 4,
    ACTION_START_DEMO = 20,
};

enum Action_State {
    ACTION_STATE_DAMAGE_CARRIED = 0x54,
    ACTION_STATE_DAMAGE_THROWN = 0x55,
    ACTION_STATE_DAMAGE_HIT = 0x65,
    ACTION_STATE_SEPARATE_BUNRI_DOUSA_INIT = 0x6E,
    ACTION_STATE_SEPARATE_BUNRI_DOUSA_EXECUTE = 0x6F,
    ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_INIT = 0x72,
    ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_GATHER = 0x73,
    ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_GROW = 0x74,
    ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_FINISH = 0x75,
};

static u8 GOUEN_FIRE_HIT;
static BpwActorSearchTable check_info;
static s32 get_check_count;
static s16 light_on_dt[] = {0, 1, 2, 1, 2, 0, 2, 0, 1};
static s16 l_turnLimit[] = {0x100, 0x300, 0x500};
static s8 l_healthThreshold[] = {10, 5, 0};

static BpwHioBank &BpwHio(u32 bank);

namespace {
// HD: the common-header statics and blur state retain their guest layout.

static BpwHeaderStateHD &l_headerState = *at<BpwHeaderStateHD>(0x10462760);
static be<f32> (&l_headerConstants)[2] = *at<be<f32>[2]>(0x1000B258);
static BpwBlurHD &l_blur = *at<BpwBlurHD>(0x101F4825);
constexpr u32 l_actorVtable = 0x1000B124;
static BpwSinCosHD (&l_sinCos)[8192] = *at<BpwSinCosHD[8192]>(0x104A44F8);
static be<u32> &l_calcMtx = *at<be<u32>>(0x1018C7B0);
static be<u32> &l_resControl = *at<be<u32>>(0x101F4F28);
static be<u32> &l_saveInfo = *at<be<u32>>(0x101F84DC);
static BpwJ3DSysHD &l_j3dSys = *at<BpwJ3DSysHD>(0x104B462C);

inline f32 add(f32 a, f32 b) {
    return fadds_ppc(a, b);
}
inline f32 sub(f32 a, f32 b) {
    return fsubs_ppc(a, b);
}
inline f32 mul(f32 a, f32 b) {
    return fmuls_ppc(a, b);
}
// HD: integer component copies preserve signalling NaNs and interleaved stores.
inline void Bpw_copyWord(void *dst, const void *src) {
    *at<be<u32>>(ea(dst)) = u32(*at<be<u32>>(ea(src)));
}

constexpr u32 stack_now = 0x1048D0CC; // mDoMtx_stack_c::now
inline u32 BpwHD_getGameInfo() {
    return ea(::dComIfGp_get());
}
inline u32 player_actor() {
    return u32(at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v);
}
inline f32 Bpw_sin(u16 angle) {
    return f32(l_sinCos[angle >> 3].mSin);
}
inline f32 Bpw_cos(u16 angle) {
    return f32(l_sinCos[angle >> 3].mCos);
}
/* dComIfG_getObjectRes("BPW", index): the archive name is a sead::BpwResourceNameHD temporary
 * {data, vtable}; every use has its own "BPW" literal (str) */
inline u32 getObjectRes(u32 str, u32 index) {
    Local<BpwResourceNameHD> name;
    name->data = str;
    name->vtable = gc_tu.safeString;
    return BpwHD_dRes_control_c_getRes(u32(l_resControl), name.get(), index);
}
} // namespace

// Forward declarations for the HD inline helpers below.
u32 bpw_smoke_ctor(u32 object);
void BpwHD_deleteEmptyObject(u32 object, u32 flags);
void bpw_smoke_dtor(u32 object, u32 flags);
void bpw_empty_virtual(u32 object);
void bpw_color_to_float(u32 output, u32 color);
void BpwHD_seStart(u32 actor, u32 sfx, u32 param);
void bpw_sinit();
u32 bpw_class_ct(u32 a);
void bpw_class_dt(u32 a, u32 flags);
BOOL daBPW_IsDelete(bpw_class *i_this);
BOOL daBPW_Delete(bpw_class *i_this);
void draw_SUB(bpw_class *i_this);
void kantera_draw_SUB(bpw_class *i_this);
void anm_init(bpw_class *i_this, s32 bckFileIdx, f32 morf, u8 loopMode, f32 speed,
              s32 soundFileIdx);
void BG_check(bpw_class *i_this);
void alpha_anime(bpw_class *i_this);
void fuwafuwa_calc(bpw_class *i_this);
BOOL body_nodeCallBack(u32 node, s32 calcTiming);
BOOL kantera_nodeCallBack(u32 node, s32 calcTiming);
void kankyou_hendou(bpw_class *i_this);
void noroi_check(bpw_class *i_this);
void fire_and_emitter_clear(bpw_class *i_this);
void noroi_brk_check(bpw_class *i_this, u8 mode);
BOOL next_att_wait_check(bpw_class *i_this);
void next_status_clear(bpw_class *i_this, u8 keepAlpha);
void light_on_off(bpw_class *i_this);
u32 skull_search_sub(u32 actor, u32 unused);
void search_get_skull(bpw_class *i_this, u8 mode);
void maai_sub(bpw_class *i_this);
s32 kantera_pos_search(bpw_class *i_this);
void kantera_calc(bpw_class *i_this);
BOOL boss_useHeapInit(bpw_class *i_this);
BOOL kantera_useHeapInit(bpw_class *i_this);
BOOL fire_useHeapInit(bpw_class *i_this);
BOOL tori_useHeapInit(bpw_class *i_this);
BOOL body_atari_check(bpw_class *i_this);
BOOL daBPW_Draw(bpw_class *i_this);
s32 daBPW_Create(bpw_class *i_this);
void action_damage(bpw_class *i_this);
void action_start_demo(bpw_class *i_this);
BOOL daBPW_Execute(bpw_class *i_this);
void body_execute(bpw_class *i_this);
void action_bunri_dousa(bpw_class *i_this);

/* tail of the joint callbacks: store calc_mtx back into the joint's anim matrix, then
 * J3DSys::mCurrentMtx (HD: J3DModel matrix block at modelData+0x10, dirty flag 0x10) */
static void bpw_joint_store(u32 model, u32 jnt) {
    u32 data = u32(at<BpwModelHD>(model)->mpMtxBuffer), mtx = u32(l_calcMtx);
    u16 flags = u16(at<BpwMtxBufferHD>(data)->mFlags);
    u32 mats = u32(at<BpwMtxBufferHD>(data)->mpAnmMtx);
    at<BpwMtxBufferHD>(data)->mFlags = (u16)(flags | 0x10);
    f32 m[12];
    for (u32 i = 0; i < 12; i++)
        m[i] = f32(at<Mtx34>(mtx)->m[i / 4][i % 4]);
    for (u32 i = 0; i < 12; i++)
        at<Mtx34>(mats + jnt * sizeof(Mtx34))->m[i / 4][i % 4] = m[i];
    BpwHD_PSMTXCopy(u32(l_calcMtx), ea(&l_j3dSys.mCurrentMtx));
}
static void bpw_joint_load(u32 model, u32 jnt) {
    u32 data = u32(at<BpwModelHD>(model)->mpMtxBuffer);
    u32 mats = u32(at<BpwMtxBufferHD>(data)->mpAnmMtx);
    at<BpwMtxBufferHD>(data)->mFlags = (u16)(u16(at<BpwMtxBufferHD>(data)->mFlags) | 0x10);
    BpwHD_PSMTXCopy_2(mats + jnt * 0x30, u32(l_calcMtx));
}

static void brk_init(u32 a, be<u32> bpw_class::*brkField, u32 model, u32 str, u32 index) {
    u32 res = getObjectRes(str, index);
    u32 brk = u32(at<bpw_class>(a)->*brkField), md = u32(at<BpwModelHD>(model)->mpModelData);
    BpwHD_mDoExt_brkAnm_initWithFlags(
        brk, md, res, 1, 0, 1.f, 0, -1, 1,
        0); // mDoExt_brkAnm::init(modelData, res, true, NONE, 1.0, 0, -1, true, 0)
}

static u32 save_info() {
    return u32(l_saveInfo) + 0x20;
}
static void onSwitch(u32 a, s32 no) {
    BpwHD_dSv_info_c_onSwitch(save_info(), no, (s32)s8(at<bpw_class>(a)->actor.current.roomNo));
}
static void offSwitch(u32 a, s32 no) {
    BpwHD_dSv_info_c_offSwitch(save_info(), no, (s32)s8(at<bpw_class>(a)->actor.current.roomNo));
}
constexpr u32 REG0 = 0x1047B608; // HIO register block (REG0_S(n) at +0x80+2n)
static BpwHioBank &BpwHio(u32 bank) {
    return *at<BpwHioBank>(REG0 + bank * sizeof(BpwHioBank));
}

/* new mDoExt_McaMorf(modelData, NULL, NULL, bck, mode, 1.0f, 0, -1, 1, NULL, 0x80000, ) */
static u32 new_McaMorf(u32 md, u32 bck, u32 mode) {
    return BpwHD_mDoExt_McaMorf_mDoExt_McaMorf(0u, md, 0u, 0u, bck, mode, 1.f, 0u, -1, 1, 0u,
                                               0x80000u, 0x37441422u);
}
/* joint callback loop: J3DModelData joint tree () +8 = joint count; node[i].callback = cb */
static void set_joint_callbacks(u32 a, u32 cb) {
    u32 morf = u32(at<bpw_class>(a)->mpMorf);
    u32 tree = BpwHD_J3DModelData_getJointTree(
        u32(at<BpwModelHD>(u32(at<mDoExt_McaMorf>(morf)->mpModel.v))->mpModelData));
    u16 n = u16(at<BpwJointTreeHD>(tree)->mJointCount);
    morf = u32(at<bpw_class>(a)->mpMorf);
    for (u16 i = 0; i < n;) {
        u32 md = u32(at<BpwModelHD>(u32(at<mDoExt_McaMorf>(morf)->mpModel.v))->mpModelData);
        u32 cnt = u32(at<BpwModelDataHD>(md)->mJointCount),
            node = u32(at<BpwModelDataHD>(md)->mpJoints);
        if (i < cnt)
            node += i * 0x1C;
        at<BpwJointHD>(node)->mpCallBack = cb;
        i++;
        tree = BpwHD_J3DModelData_getJointTree(u32(
            at<BpwModelHD>(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mpModel.v)->mpModelData));
        n = u16(at<BpwJointTreeHD>(tree)->mJointCount);
        morf = u32(at<bpw_class>(a)->mpMorf);
    }
}
static u32 new_brkAnm() {
    u32 p = BpwHD_operator_new(0x78u);
    if (p)
        p = BpwHD_mDoExt_brkAnm_construct(p);
    return p;
}

/* fopAcM_seStart, inline (HD: actor and eyePos null checks) */
static void se_start(u32 a, u32 sfx, u32 param) {
    if (a && ea(&at<bpw_class>(a)->actor.eyePos)) {
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
        BpwHD_mDoAud_seStart(sfx, ea(&at<bpw_class>(a)->actor.eyePos), param, reverb);
    }
}

/* HD material loop (body_draw / torituki_draw): K color 3 alpha = m47E, then the HD material
 * constant-color block is refreshed from it (vcalls on the material's TEV block at +0x18). */
static void bpw_material_alpha(u32 a, u32 md) {
    u16 i = 0;
    u32 t = BpwHD_J3DModelData_getMaterialTable(md);
    u16 n = u16(at<BpwMaterialTableHD>(t)->mMaterialCount);
    Local<be<f32>[4]> rgba;
    Local<be<f32>[4]> conv;
    while (i < n) {
        u32 cnt = u32(at<BpwModelDataHD>(md)->mMaterialCount),
            mat = u32(at<BpwModelDataHD>(md)->mpMaterials);
        if (i < cnt)
            mat += i * 0x39C;
        u32 tev = u32(at<BpwMaterialHD>(mat)->mpTevBlock);
        u32 c =
            call_ptr<u32>(u32(at<BpwTevVtableHD>(at<BpwTevBlockHD>(tev)->mpVtable)->getTevKColor),
                          tev, 3); // getTevKColor(3)
        at<GXColor>(c)->a = (u8)s16(at<bpw_class>(a)->m47E);
        tev = u32(at<BpwMaterialHD>(mat)->mpTevBlock);
        c = call_ptr<u32>(u32(at<BpwTevVtableHD>(at<BpwTevBlockHD>(tev)->mpVtable)->getTevKColor),
                          tev, 3);
        tev = u32(at<BpwMaterialHD>(mat)->mpTevBlock);
        call_ptr<void>(u32(at<BpwTevVtableHD>(at<BpwTevBlockHD>(tev)->mpVtable)->setTevKColor), tev,
                       3, c); // setTevKColor(3, c)
        BpwHD_GXColor_toFloat(rgba.get(), c);
        BpwHD_GXColor_fromFloat(conv.get(), rgba.get(), 1.f);
        at<BpwMaterialHD>(mat)->mDirtyFlags = u32(at<BpwMaterialHD>(mat)->mDirtyFlags) | 0x400;
        u32 dst = BpwHD_J3DMaterial_getConstantColor(mat + 0xA0, 10);
        f32 alpha = (f32)u8(at<GXColor>(c)->a) / 255.f;
        f32 r = (*conv)[0], g = (*conv)[1], b = (*conv)[2];
        at<BpwColorF32>(dst)->g = g;
        at<BpwColorF32>(dst)->b = b;
        at<BpwColorF32>(dst)->r = r;
        at<BpwColorF32>(dst)->a = alpha;
        i++;
        t = BpwHD_J3DModelData_getMaterialTable(md);
        n = u16(at<BpwMaterialTableHD>(t)->mMaterialCount);
    }
}
/* dComIfGd_setListMaskOff / dComIfGd_setList (HD: two list pointers copied from the play object) */
static void setListMaskOff() {
    l_j3dSys.mpOpaDrawBuffer = u32(
        at<BpwDrawListHD>(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->drawlist))->mpOpaListMaskOff);
    l_j3dSys.mpXluDrawBuffer = u32(
        at<BpwDrawListHD>(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->drawlist))->mpXluListMaskOff);
}
static void setList() {
    l_j3dSys.mpOpaDrawBuffer =
        u32(at<BpwDrawListHD>(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->drawlist))->mpOpaList);
    l_j3dSys.mpXluDrawBuffer =
        u32(at<BpwDrawListHD>(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->drawlist))->mpXluList);
}
static void brk_entry(u32 brk, u32 md) {
    BpwHD_mDoExt_brkAnm_entry(brk, md, f32(at<mDoExt_brkAnm>(brk)->mFrameCtrl.mFrame));
}

static void set_mtx_and_cull(u32 a, f32 lo, f32 hi) {
    u32 m = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    at<bpw_class>(a)->actor.cullMtx = m ? m + 0xC8 : 0u; // fopAcM_SetMtx(getBaseTRMtx())
    BpwHD_fopAcM_setCullSizeBox(a, lo, lo, lo, hi, hi, hi);
}
static void acch_set(u32 a) {
    BpwHD_dBgS_Acch_Set(ea(&at<bpw_class>(a)->mAcch), ea(&at<bpw_class>(a)->actor.current.pos),
                        ea(&at<bpw_class>(a)->actor.old.pos), a, 1, ea(&at<bpw_class>(a)->mAcchCir),
                        ea(&at<bpw_class>(a)->actor.speed), 0, 0);
}
static u32 actor_id(u32 a) {
    return a ? u32(at<bpw_class>(a)->actor.mBsPcId) : 0xFFFFFFFFu;
}
/* dComIfGp_getStartStageName()[0] != 'X' && (REG0_S(9) != 0 || !dComIfGs_isStageBossDemo()) */
static bool start_demo_check() {
    if (u8(*at<be<u8>>(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCurStage.mName[0]))) ==
        0x58)
        return false;
    if (s16(BpwHio(1).mHeader[1]) != 0)
        return true;
    return !BpwHD_dSv_memBit_c_isDungeonItem(u32(l_saveInfo) + 0x798, 5);
}

/* fopAcM_seStart with only the eyePos null check (the compiler knew this != NULL) */
static void se_start_e(u32 a, u32 sfx, u32 param) {
    if (ea(&at<bpw_class>(a)->actor.eyePos)) {
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
        BpwHD_mDoAud_seStart(sfx, ea(&at<bpw_class>(a)->actor.eyePos), param, reverb);
    }
}
/* fopAcM_monsSeStart, inline: mDoAud_monsSeStart(sfx, &eyePos, fopAcM_GetID(this), param, reverb)
 * (HD: eyePos null check; checkThis: also this != NULL) */
static void mons_se(u32 a, u32 sfx, u32 param, bool checkThis) {
    if (checkThis && !a)
        return;
    if (!(ea(&at<bpw_class>(a)->actor.eyePos)))
        return;
    s32 room = s8(at<bpw_class>(a)->actor.current.roomNo);
    u32 id = actor_id(a);
    s32 reverb = BpwHD_dComIfGp_getReverb(room);
    BpwHD_mDoAud_seStartLevel(sfx, ea(&at<bpw_class>(a)->actor.eyePos), id, param, reverb);
}
static bool morf_isStop(u32 morf) { // mDoExt_McaMorf::isStop (frame control at +0x98)
    return (u8(at<mDoExt_McaMorf>(morf)->mFrameCtrl.mState) & 1) ||
           f32(at<mDoExt_McaMorf>(morf)->mFrameCtrl.mRate) == 0.f;
}
static bool brk_isStop(u32 brk) { // mDoExt_brkAnm::isStop (frame control at +0)
    return (u8(at<mDoExt_brkAnm>(brk)->mFrameCtrl.mState) & 1) ||
           f32(at<mDoExt_brkAnm>(brk)->mFrameCtrl.mRate) == 0.f;
}
static s16 reg_ftoi(u32 off, f32 k) {
    return (s16)ftoi(add(f32(BpwHio(off / 0x90).mFloat[((off % 0x90) - 8) / 4]), k));
}

/* HIO registers: REGn_F(i) / REGn_S(i) */

static f32 regf(u32 n, u32 i, f32 k) {
    return add(f32(BpwHio(n).mFloat[i]), k);
}
static s16 regf_t(u32 n, u32 i, f32 k) {
    return (s16)ftoi(regf(n, i, k));
}
/* if (REG20_S(1) == 0) REG20_S(0) = 1; if (REG20_S(0) == 0) -> false; REG20_S(0) = 0 */
static bool reg20_step() {
    if (s16(BpwHio(20).mShort[1]) != 0 && s16(BpwHio(20).mShort[0]) == 0)
        return false;
    BpwHio(20).mShort[0] = 0;
    return true;
}
/* fopAcM_SearchByID inline (HD: no search for fpcM_ERROR_PROCESS_ID_e) */
static u32 search_by_id(u32 id) {
    Local<be<u32>> key;
    key->set(id);
    if (id == 0xFFFFFFFF)
        return 0;
    return BpwHD_fopAcIt_Judge(0x025E1234u, key.get());
}
/* cLib_addCalc2(p, target, scale, fabs(*p - target) * k * m43C) */
static void demo_calc(u32 a, be<f32> *field, f32 target, f32 scale, u32 kn, u32 ki, f32 kb) {
    f32 v = *field;
    f32 d = __builtin_fabsf(sub(v, target));
    f32 step = mul(mul(d, regf(kn, ki, kb)), f32(at<bpw_class>(a)->m43C));
    BpwHD_cLib_addCalc2_4(ea(field), target, scale, step);
}
static void set_cam_pos(u32 a, f32 cx, f32 cy, f32 cz, f32 ex, f32 ey, f32 ez) {
    at<bpw_class>(a)->m40C.x = cx;
    at<bpw_class>(a)->m40C.y = cy;
    at<bpw_class>(a)->m40C.z = cz;
    at<bpw_class>(a)->m418.x = ex;
    at<bpw_class>(a)->m418.y = ey;
    at<bpw_class>(a)->m418.z = ez;
}

void body_execute(bpw_class *i_this);
void action_bunri_dousa(bpw_class *i_this);

static void dPa_set(u32 id, u32 pos, u32 angle, u32 scale, u32 cb) {
    u32 play = BpwHD_getGameInfo();
    BpwHD_dPa_control_c_set_5(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0, id, pos, angle,
                              scale, 0xFF, cb, -1, 0, 0, 0);
}
/* SetC(center); SetR(radiusOff ? this->radius : 70); dComIfG_Ccsp()->Set(sph) */
static void ccs_set(u32 a, u32 sph, u32 center, const be<f32> *radius) {
    BpwHD_cM3dGSph_SetC(ea(&at<dCcD_Sph>(sph)->mSph), center); // cM3dGSph::SetC
    f32 r = radius ? f32(*radius) : 70.f;
    BpwHD_cM3dGSph_SetR(ea(&at<dCcD_Sph>(sph)->mSph), r); // cM3dGSph::SetR
    BpwHD_cCcS_Set(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCcS),
                   sph); // dComIfG_Ccsp()->Set(sph)
}
static void clamp1(f32 &v) {
    if (v > 1.f)
        v = 1.f;
    else if (v < -1.f)
        v = -1.f;
}

/* inline dBgS_LinChk construction / destruction on the stack (HD vtables) */
static void linchk_ct(u32 l) {
    BpwHD_cBgS_LinChk_ct_2(l); // cBgS_LinChk::ct
    at<dBgS_LinChk>(l)->mbObjThrough = 0;
    at<dBgS_LinChk>(l)->mbCamThrough = 0;
    at<dBgS_LinChk>(l)->mbLinkThrough = 0;
    at<dBgS_LinChk>(l)->mbArrowThrough = 0;
    at<dBgS_LinChk>(l)->mbBombThrough = 0;
    at<dBgS_LinChk>(l)->mbBoomerangThrough = 0;
    at<dBgS_LinChk>(l)->mbRopeThrough = 0;
    at<dBgS_LinChk>(l)->dBgS_GrpPassChk::__vtbl = 0x1000B104;
    at<dBgS_LinChk>(l)->mGrp = 1;
    at<dBgS_LinChk>(l)->dBgS_PolyPassChk::__vtbl = 0x1000B114;
    at<dBgS_LinChk>(l)->mPolyPassChk = l + 0x58;
    at<dBgS_LinChk>(l)->mGrpPassChk = l + 0x64;
    at<dBgS_LinChk>(l)->cBgS_Chk::__vtbl = 0x1000B0E4;
    at<dBgS_LinChk>(l)->cBgS_PolyInfo::__vtbl = 0x1000B0F4;
}
static void linchk_dt(u32 l) {
    at<dBgS_LinChk>(l)->dBgS_PolyPassChk::__vtbl = 0x1000B114;
    at<dBgS_LinChk>(l)->dBgS_GrpPassChk::__vtbl = 0x1000B0A4;
    at<dBgS_LinChk>(l)->cBgS_PolyInfo::__vtbl = 0x1000B094;
    BpwHD_cBgS_LinChk_destroy_2(l, 0);
}
/* JPABaseEmitter::setGlobalSRTMatrix(model->getAnmMtx(2)) (HD: dirty flag on the joint block) */
static void emitter_srt_jnt2(u32 a, u32 em) {
    u32 model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    u32 data = u32(at<BpwModelHD>(model)->mpMtxBuffer);
    u16 f = u16(at<BpwMtxBufferHD>(data)->mFlags);
    u32 mats = u32(at<BpwMtxBufferHD>(data)->mpAnmMtx);
    at<BpwMtxBufferHD>(data)->mFlags = (u16)(f | 0x10);
    BpwHD_JPASetRMtxSTVecfromMtx(mats + 0x60, em + 0x1F0, em + 0x220, em + 0x22C);
}
/* dComIfGp_particle_setToon(id, &mFire1DousaPos, &shape_angle, NULL, 0xB9, cb, room) + sea colors
 */
static void toon_smoke(u32 a, u32 id, u32 cb) {
    s32 room = s8(at<bpw_class>(a)->actor.current.roomNo);
    u32 play = BpwHD_getGameInfo();
    u32 em = BpwHD_dPa_control_c_set_6(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 2, id,
                                       ea(&at<bpw_class>(a)->mFire1DousaPos),
                                       ea(&at<bpw_class>(a)->actor.shape_angle), 0, 0xB9, cb, room,
                                       0, 0, 0);
    if (em) {
        Local<be<u32>> c1, c2;
        BpwHD_dKy_get_seacolor(c1.get(), c2.get()); // dKy_get_seacolor
        u32 c1a = ea(c1.get()), c2a = ea(c2.get());
        u8 b = u8(at<GXColor>(c1a)->b), r = u8(at<GXColor>(c1a)->r), g = u8(at<GXColor>(c1a)->g);
        at<BpwEmitterHD>(em)->mGlobalPrmColor.r = r;
        at<BpwEmitterHD>(em)->mGlobalPrmColor.g = g;
        at<BpwEmitterHD>(em)->mGlobalPrmColor.b = b;
        u8 b2 = u8(at<GXColor>(c2a)->b), r2 = u8(at<GXColor>(c2a)->r), g2 = u8(at<GXColor>(c2a)->g);
        at<BpwEmitterHD>(em)->mGlobalEnvColor.r = r2;
        at<BpwEmitterHD>(em)->mGlobalEnvColor.g = g2;
        at<BpwEmitterHD>(em)->mGlobalEnvColor.b = b2;
    }
}
static void fire_pos_ground(u32 a) { // mFire1DousaPos = current.pos; .y = mAcch.GetGroundH()
    Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x, &at<bpw_class>(a)->actor.current.pos.x);
    at<bpw_class>(a)->mFire1DousaPos.y = f32(at<bpw_class>(a)->mAcch.m_ground_h);
    Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z, &at<bpw_class>(a)->actor.current.pos.z);
}
static u32 player_by_play() {
    return u32(at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mPlayerInfo[0].mpPlayer.v);
}
/* daPy_py_c::setOutPower(power, angle, 1): player vtable +0xEC */
static void set_out_power(u32 a, u32 pl, f32 power, s16 add_) {
    u32 vt = u32(at<fopAc_ac_c>(pl)->__vtbl);
    s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_by_play());
    call_ptr<void>(u32(at<BpwPlayerVtableHD>(vt)->setOutPower), pl, (s32)(s16)(ang + add_), 1,
                   power);
}
static void inc_state(u32 a) {
    at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
}

/* gouen_maai_sub (inline) */
static void gouen_maai_sub(u32 a) {
    u32 pl = player_actor();
    f32 step = regf(9, 7, 2.f);
    f32 d = BpwHD_fopAcM_searchActorDistance_2(a, player_actor());
    if (d > regf(9, 9, 500.f)) {
        u32 mtx = u32(l_calcMtx);
        s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_by_play());
        BpwHD_mDoMtx_YrotS(mtx, (s32)ang);
        Local<BpwVecHD> v, out;
        f32 z = regf(9, 9, 500.f);
        v->x = 0.f;
        v->y = 0.f;
        v->z = z;
        BpwHD_MtxPosition_2(v.get(), out.get());
        BpwHD_PSVECAdd(out.get(), ea(&at<fopAc_ac_c>(pl)->current.pos), out.get());
        BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->actor.current.pos), (f32)out->x, 1.f, step);
        BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->actor.current.pos.z), (f32)out->z, 1.f, step);
    }
    d = BpwHD_fopAcM_searchActorDistance_2(a, player_actor());
    if (d < regf(9, 10, 200.f)) {
        u32 mtx = u32(l_calcMtx);
        s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_by_play());
        BpwHD_mDoMtx_YrotS(mtx, (s32)ang);
        Local<BpwVecHD> v, out;
        f32 z = -regf(9, 10, 200.f);
        f32 step2 = regf(9, 8, 10.f);
        v->y = 0.f;
        v->x = 0.f;
        v->z = z;
        BpwHD_MtxPosition_2(v.get(), out.get());
        BpwHD_PSVECAdd(out.get(), ea(&at<fopAc_ac_c>(pl)->current.pos), out.get());
        BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->actor.current.pos), (f32)out->x, 1.f, step2);
        BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->actor.current.pos.z), (f32)out->z, 1.f, step2);
    }
}

/* action_dousa (inline in body_execute) */
static void action_dousa(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 pl = player_actor();
    using LinChk = dBgS_LinChk;
    Local<LinChk> lc;
    u32 l = ea(lc.get());
    linchk_ct(l);
    auto clear_timers = [&] {
        for (u32 i = 0; i < 10; i++)
            at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
    };
    bool idle = false; // states 10..13 skip the angle/alpha part
    switch (s16(at<bpw_class>(a)->mActionState)) {
    case 0: {
        at<bpw_class>(a)->m488 = 1.f;
        search_get_skull(i_this, 2);
        clear_timers();
        if (u32(at<bpw_class>(a)->m3F8) != 0x2A)
            anm_init(i_this, dRes_INDEX_BPW_BCK_WAIT1_e, 15.f, 2, 1.f, -1); // WAIT1
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 10;
        f32 r = BpwHD_cM_rndF(10.f);
        s16 m = s16(at<bpw_class>(a)->m3F6);
        at<bpw_class>(a)->mSomeCountdownTimers[0] = (s16)((s16)ftoi(r) + 10);
        if ((u32)(s32)m <= 2)
            at<bpw_class>(a)->m47A = l_turnLimit[m]; // m47A = {0x100,0x300,0x500}[m3F6]
        inc_state(a);
    }
        /* fallthrough */
    case 1:
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0)
            break;
        inc_state(a);
        /* fallthrough */
    case 2: {
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 0x41;
        at<bpw_class>(a)->m490 = 10.f;
        f32 r = BpwHD_cM_rndF(65.f);
        s16 m = s16(at<bpw_class>(a)->m3F6);
        at<bpw_class>(a)->m488 = 1.f;
        u32 anm = u32(at<bpw_class>(a)->m3F8);
        at<bpw_class>(a)->mSomeCountdownTimers[0] = (s16)((s16)ftoi(r) + 0x41);
        if (m == 2) {
            at<bpw_class>(a)->m490 = mul(f32(at<bpw_class>(a)->m490), regf(8, 0, 1.35f));
            at<bpw_class>(a)->m488 = 5.f;
        }
        if (anm != 0x1D)
            anm_init(i_this, dRes_INDEX_BPW_BCK_MOVEF1_e, 15.f, 2, 1.f, -1); // MOVEF1
        inc_state(a);
    }
        /* fallthrough */
    case 3: {
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0)
            break;
        s16 m = s16(at<bpw_class>(a)->m3F6);
        at<bpw_class>(a)->m490 = 0.f;
        if (m == 0) {
            at<bpw_class>(a)->mBodyAction = ACTION_ATTACK_KOUGEKI;
            at<bpw_class>(a)->mActionState = 0x14;
            break;
        }
        if ((u32)(s32)m > 2)
            break;
        if (BpwHD_dDetect_c_chk_light(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mDetect),
                                      ea(&at<fopAc_ac_c>(pl)->current.pos))) {
            at<bpw_class>(a)->mBodyAction = ACTION_ATTACK_KOUGEKI;
            at<bpw_class>(a)->mActionState = 0x28;
            break;
        }
        if (s16(BpwHio(12).mShort[9]) == 0 && BpwHD_cM_rnd() < 0.5f) {
            f32 dz =
                sub(f32(at<bpw_class>(a)->mBodyPos.z), f32(at<bpw_class>(a)->actor.current.pos.z));
            f32 dx =
                sub(f32(at<bpw_class>(a)->mBodyPos.x), f32(at<bpw_class>(a)->actor.current.pos.x));
            if (BpwHD_sqrtf(fmadds(dx, dx, mul(dz, dz))) < 1100.f) {
                at<bpw_class>(a)->mBodyAction = ACTION_BODY_DOWN_KARADA_TAORE;
                at<bpw_class>(a)->mActionState = 0x46;
                break;
            }
        }
        at<bpw_class>(a)->mActionState = 4;
        break;
    }
    case 4:
        anm_init(i_this, dRes_INDEX_BPW_BCK_MOVEB1_e, regf(12, 0, 15.f), 0, 1.f, -1); // MOVEB1
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(12, 1, 40.f);
        noroi_brk_check(i_this, 0);
        mons_se(a, 0x4969, 0, true); // JA_SE_CV_BPW_CURSE
        inc_state(a);
        /* fallthrough */
    case 5:
        maai_sub(i_this);
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0)
            break;
        at<bpw_class>(a)->m3E6 = 1;
        at<bpw_class>(a)->actor.speedF = regf(8, 12, 75.f);
        at<bpw_class>(a)->m488 = regf(8, 13, 2.f);
        at<bpw_class>(a)->m490 = 0.f;
        anm_init(i_this, dRes_INDEX_BPW_BCK_MOVES1_e, 15.f, 2, 1.f, -1); // MOVES1
        inc_state(a);
        /* fallthrough */
    case 6: {
        if (f32(at<bpw_class>(a)->actor.speedF) < 1.f)
            at<bpw_class>(a)->actor.speedF = 0.f;
        s16 ang = s16(at<bpw_class>(a)->actor.current.angle.y);
        BpwHD_mDoMtx_YrotS_2(u32(l_calcMtx), (s32)ang);
        Local<BpwVecHD> v;
        v->x = 0.f;
        v->y = 0.f;
        v->z = 500.f;
        BpwHD_MtxPosition(v.get(), ea(&at<bpw_class>(a)->m328));
        BpwHD_PSVECAdd_2(ea(&at<bpw_class>(a)->m328), ea(&at<bpw_class>(a)->actor.current.pos),
                         ea(&at<bpw_class>(a)->m328));
        f32 x = f32(at<bpw_class>(a)->actor.current.pos.x), my = f32(at<bpw_class>(a)->m328[0].y),
            z = f32(at<bpw_class>(a)->actor.current.pos.z),
            y = f32(at<bpw_class>(a)->actor.current.pos.y);
        at<bpw_class>(a)->m304[0].x = x;
        at<bpw_class>(a)->m328[0].y = add(my, 400.f);
        at<bpw_class>(a)->m304[0].z = z;
        at<bpw_class>(a)->m304[0].y = add(y, 400.f);
        BpwHD_dBgS_LinChk_Set_2(l, ea(&at<bpw_class>(a)->m304), ea(&at<bpw_class>(a)->m328), a);
        bool stop;
        if (BpwHD_cBgS_LineCross_2(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mBgS), l)) {
            at<bpw_class>(a)->actor.speedF = 0.f;
            stop = true;
        } else
            stop = f32(at<bpw_class>(a)->actor.speedF) == 0.f;
        if (stop) {
            noroi_brk_check(i_this, 1);
            next_status_clear(i_this, 1);
            if (u32(at<daPy_py_c>(pl)->mNoResetFlg1) & 0x100) {
                at<bpw_class>(a)->mBodyAction = ACTION_ATTACK_KOUGEKI;
                at<bpw_class>(a)->mActionState = 0x14;
            } else
                at<bpw_class>(a)->mActionState = 0;
        }
        break;
    }
    case 10:
        search_get_skull(i_this, 2);
        clear_timers();
        if (u32(at<bpw_class>(a)->m3F8) != 0x2A)
            anm_init(i_this, dRes_INDEX_BPW_BCK_WAIT1_e, 15.f, 2, 1.f, -1);
        inc_state(a);
        /* fallthrough */
    case 11:
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m47E), 0, 1, 10);
        if (s16(at<bpw_class>(a)->m47E) < 2) {
            at<bpw_class>(a)->m47E = 0;
            if (kantera_pos_search(i_this))
                inc_state(a);
        }
        break;
    case 12:
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m47E), 0x96, 1, 0x1E);
        if (s16(at<bpw_class>(a)->m47E) >= 0x95) {
            at<bpw_class>(a)->m47E = 0x96;
            at<bpw_class>(a)->m480 = 0x96;
            anm_init(i_this, dRes_INDEX_BPW_BCK_HIROU1_e, 15.f, 0, 1.f, -1); // HIROU1
            kantera_pos_search(i_this);
            inc_state(a);
        }
        break;
    case 13:
        if (s16(at<bpw_class>(a)->m462) == 0)
            kantera_pos_search(i_this);
        if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 12.f)) {
            se_start(a, 0x5967, 0); // JA_SE_CM_BPW_GET_KANTERA
            u32 id = u32(at<bpw_class>(a)->m3FC);
            if (id != 0xFFFFFFFF) {
                u32 k = search_by_id(id);
                if (k) {
                    at<fopAc_ac_c>(k)->gravity = 0.f;
                    at<fopAc_ac_c>(k)->speedF = 0.f;
                    at<fopAc_ac_c>(k)->speed.x = 0.f;
                    at<fopAc_ac_c>(k)->speed.y = 0.f;
                    at<fopAc_ac_c>(k)->speed.z = 0.f;
                    at<bpw_class>(k)->actor.attention_info.flags = 0;
                    at<bpw_class>(k)->mActionState = 0;
                    at<bpw_class>(a)->m462 = 1;
                }
            }
            at<bpw_class>(a)->m482 = 0;
            at<bpw_class>(a)->mAttWaitTimer = 1;
            at<bpw_class>(a)->m388 = 0.f;
            at<bpw_class>(a)->m38C = 0.f;
            at<bpw_class>(a)->m390 = 0.f;
        }
        if (morf_isStop(u32(at<bpw_class>(a)->mpMorf))) {
            next_status_clear(i_this, 1);
            at<bpw_class>(a)->mSomeCountdownTimers[2] = 0;
            at<bpw_class>(a)->mBodyAction = ACTION_ATTACK_KOUGEKI;
            at<bpw_class>(a)->mActionState = 0x14;
        }
        break;
    }
    (void)idle;
    at<bpw_class>(a)->m4A0 = 0.f;
    BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->actor.speedF), f32(at<bpw_class>(a)->m490), 1.f,
                          f32(at<bpw_class>(a)->m488));
    BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->m48C), 1.f, 10.f);
    u32 st = (u32)(s32)s16(at<bpw_class>(a)->mActionState);
    if (st == 6) {
        alpha_anime(i_this);
        fuwafuwa_calc(i_this);
    } else if (!(st >= 10 && st <= 13)) {
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_by_play());
        alpha_anime(i_this);
        fuwafuwa_calc(i_this);
    }
    noroi_check(i_this);
    linchk_dt(l);
}

/* action_kougeki (inline in body_execute) */
static void action_kougeki(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 pl = player_actor();
    switch (s16(at<bpw_class>(a)->mActionState)) {
    case 0x14:
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[2]) != 0)
            break;
        for (u32 i = 0; i < 10; i++)
            at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
        if (u32(at<bpw_class>(a)->m3F8) != 0x2B)
            anm_init(i_this, dRes_INDEX_BPW_BCK_WARAU1_e, 15.f, 2, 1.f, -1); // WARAU1
        at<bpw_class>(a)->m4A0 = 0.f;
        at<bpw_class>(a)->m47A = 0x500;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 30;
        inc_state(a);
        /* fallthrough */
    case 0x15: {
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        maai_sub(i_this);
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0)
            break;
        s16 m = s16(at<bpw_class>(a)->m3F6);
        if (m == 0 || ((u32)(s32)m <= 2 && !(u32(at<daPy_py_c>(pl)->mNoResetFlg1) & 0x100))) {
            at<bpw_class>(a)->mActionState = 0x28;
            if (BpwHD_fopAcM_searchActorDistance_2(a, player_by_play()) < 1050.f)
                at<bpw_class>(a)->mActionState = 0x3C;
        } else if ((u32)(s32)m <= 2) {
            at<bpw_class>(a)->mActionState = 0x3C;
        }
        if (u8(at<bpw_class>(a)->m3E9) != 0) {
            at<bpw_class>(a)->mActionState = 0x32;
            at<bpw_class>(a)->m3E9 = 0;
        }
        break;
    }
    case 0x28: {
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[2]) != 0)
            break;
        anm_init(i_this, dRes_INDEX_BPW_BCK_SUIKOMU1_e, 15.f, 2, 1.f, -1); // SUIKOMU1
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 30;
        f32 r = BpwHD_cM_rndF(30.f);
        u32 em = u32(at<bpw_class>(a)->m540.mpEmitter.v);
        at<bpw_class>(a)->mSomeCountdownTimers[0] = (s16)((s16)ftoi(r) + 30);
        if (em)
            dPa_set(0x82CE, ea(&at<bpw_class>(a)->m394), 0, 0,
                    ea(&at<bpw_class>(a)
                            ->m540)); // BPWINHALE01 (only when the emitter exists, as GameCube)
        inc_state(a);
    }
        /* fallthrough */
    case 0x29: {
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
        BpwHD_mDoAud_seStart_2(0x7054u, ea(&at<bpw_class>(a)->m3C4), 0u,
                               reverb); // JA_SE_OBJ_BPW_BREATH_IN at m3C4
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        search_get_skull(i_this, 0);
        u32 em = u32(at<bpw_class>(a)->m540.mpEmitter.v);
        if (em)
            emitter_srt_jnt2(a, em);
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[1]) == 0) {
            u32 play = BpwHD_getGameInfo();
            u32 e2 = BpwHD_dPa_control_c_set_7(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0,
                                               0x82CD, ea(&at<bpw_class>(a)->m394),
                                               ea(&at<bpw_class>(a)->actor.shape_angle), 0, 0xFF, 0,
                                               -1, 0, 0, 0); // BPWINHALE00
            if (e2)
                emitter_srt_jnt2(a, e2);
            at<bpw_class>(a)->mSomeCountdownTimers[1] = 6;
        }
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0) {
            if (BpwHD_fopAcM_searchActorDistance_2(a, player_by_play()) > 300.f)
                set_out_power(a, pl, 10.f, (s16)0x8000);
        } else {
            BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->m540));    // m540.remove()
            anm_init(i_this, dRes_INDEX_BPW_BCK_IKI1_e, 15.f, 0, 1.f, -1); // IKI1
            inc_state(a);
        }
        break;
    }
    case 0x2A: {
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        search_get_skull(i_this, 2);
        if (!morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 0x5A;
        f32 r = BpwHD_cM_rndF(90.f);
        u32 em = u32(at<bpw_class>(a)->m52C.mpEmitter.v);
        at<bpw_class>(a)->mSomeCountdownTimers[0] = (s16)((s16)ftoi(r) + 0x5A);
        if (!em)
            dPa_set(0x82CF, ea(&at<bpw_class>(a)->m394), 0, 0,
                    ea(&at<bpw_class>(a)->m52C)); // BPWBLOW00
        fire_pos_ground(a);
        at<bpw_class>(a)->mFire1DousaRot.x = u16(at<bpw_class>(a)->actor.shape_angle.x);
        at<bpw_class>(a)->mFire1DousaRot.y = u16(at<bpw_class>(a)->actor.shape_angle.y);
        at<bpw_class>(a)->mFire1DousaRot.z = u16(at<bpw_class>(a)->actor.shape_angle.z);
        if (!u32(at<bpw_class>(a)->m50C.mpEmitter.v)) {
            s32 room = s8(at<bpw_class>(a)->actor.current.roomNo);
            u32 play = BpwHD_getGameInfo();
            BpwHD_dPa_control_c_set_8(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 2, 0xA2D0,
                                      ea(&at<bpw_class>(a)->mFire1DousaPos),
                                      ea(&at<bpw_class>(a)->mFire1DousaRot), 0, 0xA0,
                                      ea(&at<bpw_class>(a)->m50C), room, 0, 0, 0); // BPWBLOWSMOKE00
        }
        anm_init(i_this, dRes_INDEX_BPW_BCK_IKI_WAIT1_e, 0.f, 2, 1.f, -1); // IKI_WAIT1
        inc_state(a);
        break;
    }
    case 0x2B: {
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
        BpwHD_mDoAud_seStart_2(0x7055u, ea(&at<bpw_class>(a)->m3C4), 0u,
                               reverb); // JA_SE_OBJ_BPW_BREATH_OUT
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        search_get_skull(i_this, 1);
        u32 em = u32(at<bpw_class>(a)->m52C.mpEmitter.v);
        if (em)
            emitter_srt_jnt2(a, em);
        fire_pos_ground(a);
        at<bpw_class>(a)->mFire1DousaRot.x = u16(at<bpw_class>(a)->actor.shape_angle.x);
        at<bpw_class>(a)->mFire1DousaRot.y = u16(at<bpw_class>(a)->actor.shape_angle.y);
        s16 t0 = s16(at<bpw_class>(a)->mSomeCountdownTimers[0]);
        at<bpw_class>(a)->mFire1DousaRot.z = u16(at<bpw_class>(a)->actor.shape_angle.z);
        if (t0 != 0)
            set_out_power(a, pl, 14.f, 0);
        else {
            search_get_skull(i_this, 2);
            fire_and_emitter_clear(i_this);
            next_att_wait_check(i_this);
            at<bpw_class>(a)->mSomeCountdownTimers[2] = regf_t(12, 7, 90.f);
        }
        break;
    }
    case 0x32:
        anm_init(i_this, dRes_INDEX_BPW_BCK_SUIKOMU1_e, 15.f, 2, 1.f, -1);
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 0x4B;
        at<bpw_class>(a)->mKankyouHendouState = 2;
        at<bpw_class>(a)->mSomeCountdownTimers[1] = regf_t(8, 2, 20.f);
        inc_state(a);
        /* fallthrough */
    case 0x33: {
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
        BpwHD_mDoAud_seStart_2(0x7054u, ea(&at<bpw_class>(a)->m3C4), 0u, reverb);
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[1]) != 0)
            at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_by_play());
        at<bpw_class>(a)->m47A = regf_t(21, 2, 140.f);
        search_get_skull(i_this, 0);
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0) {
            if (BpwHD_fopAcM_searchActorDistance_2(a, player_by_play()) > 300.f)
                set_out_power(a, pl, 10.f, (s16)0x8000);
        } else {
            anm_init(i_this, dRes_INDEX_BPW_BCK_HONOO1_e, 15.f, 0, 1.f, -1); // HONOO1
            inc_state(a);
        }
        break;
    }
    case 0x34: {
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        at<bpw_class>(a)->m47A = regf_t(21, 2, 140.f);
        search_get_skull(i_this, 2);
        if (!morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
            break;
        u32 k = search_by_id(u32(at<bpw_class>(a)->m3FC));
        if (!k)
            break;
        u32 id = BpwHD_fopAcM_create_2(0xD3, 2, ea(&at<bpw_class>(k)->m358),
                                       (s32)s8(at<bpw_class>(a)->actor.current.roomNo),
                                       ea(&at<bpw_class>(a)->actor.current.angle), 0, -1, 0);
        at<bpw_class>(a)->m400 = id;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 0x46;
        f32 r = BpwHD_cM_rndF(70.f);
        at<bpw_class>(a)->mSomeCountdownTimers[0] = (s16)((s16)ftoi(r) + 0x46);
        at<bpw_class>(a)->m47A = 0x300;
        at<bpw_class>(a)->mSomeCountdownTimers[5] = 0;
        GOUEN_FIRE_HIT = 0;                                                  // GOUEN_FIRE_HIT = 0
        anm_init(i_this, dRes_INDEX_BPW_BCK_HONOO_WAIT1_e, 0.f, 2, 1.f, -1); // HONOO_WAIT1
        inc_state(a);
        break;
    }
    case 0x35: {
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
        BpwHD_mDoAud_seStart_2(0x7056u, ea(&at<bpw_class>(a)->m3C4), 0u,
                               reverb); // JA_SE_OBJ_BPW_FIREBLAST
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        gouen_maai_sub(a);
        u32 k = search_by_id(u32(at<bpw_class>(a)->m3FC));
        if (k) {
            u32 f = search_by_id(u32(at<bpw_class>(a)->m400));
            if (f) {
                Bpw_copyWord(&at<bpw_class>(f)->actor.current.pos.x, &at<bpw_class>(k)->m358.x);
                Bpw_copyWord(&at<bpw_class>(f)->actor.current.pos.y, &at<bpw_class>(k)->m358.y);
                Bpw_copyWord(&at<bpw_class>(f)->actor.current.pos.z, &at<bpw_class>(k)->m358.z);
                at<fopAc_ac_c>(f)->current.angle.x = u16(at<bpw_class>(a)->actor.shape_angle.x);
                at<fopAc_ac_c>(f)->current.angle.y = u16(at<bpw_class>(a)->actor.shape_angle.y);
                at<fopAc_ac_c>(f)->current.angle.z = u16(at<bpw_class>(a)->actor.shape_angle.z);
                at<fopAc_ac_c>(f)->shape_angle.y = s16(at<bpw_class>(a)->actor.shape_angle.y);
                f32 dz = sub(f32(at<fopAc_ac_c>(pl)->current.pos.z),
                             f32(at<fopAc_ac_c>(f)->current.pos.z));
                f32 dx = sub(f32(at<fopAc_ac_c>(pl)->current.pos.x),
                             f32(at<fopAc_ac_c>(f)->current.pos.x));
                f32 dy = sub(f32(at<fopAc_ac_c>(pl)->current.pos.y),
                             f32(at<fopAc_ac_c>(f)->current.pos.y));
                f32 h = BpwHD_sqrtf(fmadds(dx, dx, mul(dz, dz)));
                at<fopAc_ac_c>(f)->shape_angle.x = (s16)-BpwHD_cM_atan2s(dy, h);
            }
        }
        at<bpw_class>(a)->m47A = regf_t(21, 2, 140.f);
        if (s16(BpwHio(8).mShort[4]) != 0)
            break;
        if (GOUEN_FIRE_HIT == 0 && s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0)
            break;
        fire_and_emitter_clear(i_this);
        next_status_clear(i_this, 1);
        next_att_wait_check(i_this);
        GOUEN_FIRE_HIT = 0;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 0;
        at<bpw_class>(a)->mSomeCountdownTimers[5] = (s16)(s16(BpwHio(21).mShort[0]) + 10);
        break;
    }
    case 0x3C: {
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 0x28;
        f32 r = BpwHD_cM_rndF(40.f);
        u32 anm = u32(at<bpw_class>(a)->m3F8);
        at<bpw_class>(a)->mKankyouHendouState = 1;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = (s16)((s16)ftoi(r) + 0x28);
        at<bpw_class>(a)->mSomeCountdownTimers[5] = 0;
        at<bpw_class>(a)->mAttWaitTimer = 2;
        if (anm != 5) {
            anm_init(i_this, dRes_INDEX_BPW_BCK_ATTACK_KAN1_e, 15.f, 2, 1.f, -1); // ATTACK_KAN1
            mons_se(a, 0x4962, 0, true); // JA_SE_CV_BPW_ATTACK
        }
        inc_state(a);
    }
        /* fallthrough */
    case 0x3D: {
        maai_sub(i_this);
        at<bpw_class>(a)->m476 = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        se_start(a, 0x515F, 0); // JA_SE_CM_BPW_SWING_KANTERA
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[1]) == 0) {
            se_start(a, 0x6A3A, 0); // JA_SE_OBJ_BPW_FIRE_OUT
            u32 k = search_by_id(u32(at<bpw_class>(a)->m3FC));
            if (k) {
                BpwHD_mDoMtx_YrotS_2(u32(l_calcMtx),
                                     (s32)s16(at<bpw_class>(a)->actor.shape_angle.y));
                Local<BpwVecHD> v, out;
                v->x = 0.f;
                v->y = 0.f;
                v->z = 20.f;
                BpwHD_MtxPosition_2(v.get(), out.get());
                BpwHD_PSVECAdd(out.get(), ea(&at<bpw_class>(k)->m358), out.get());
                u32 id = BpwHD_fopAcM_create_3(
                    0xD3, 0x102, out.get(), (s32)s8(at<bpw_class>(a)->actor.current.roomNo),
                    ea(&at<bpw_class>(a)->actor.current.angle), 0, -1, 0);
                at<bpw_class>(a)->m400 = id;
                at<bpw_class>(a)->mSomeCountdownTimers[1] = (s16)(s16(BpwHio(12).mShort[4]) + 7);
                if (u32(at<daPy_py_c>(pl)->mNoResetFlg1) & 0x100)
                    at<bpw_class>(a)->mSomeCountdownTimers[1] =
                        (s16)(s16(BpwHio(12).mShort[4]) + 9);
            }
        }
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) == 0) {
            at<bpw_class>(a)->mAttWaitTimer = 1;
            at<bpw_class>(a)->mSomeCountdownTimers[5] =
                (s16)ftoi(add((f32)s16(BpwHio(21).mShort[1]), 90.f));
            if (!(u32(at<daPy_py_c>(pl)->mNoResetFlg1) & 0x100))
                next_att_wait_check(i_this);
            else
                at<bpw_class>(a)->mActionState = 0x28;
        }
        break;
    }
    }
    BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->actor.speedF), 1.f, 1.f);
    alpha_anime(i_this);
    fuwafuwa_calc(i_this);
}

/* action_karada_taore (inline in body_execute) */
static void action_karada_taore(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 pl = player_actor();
    switch (s16(at<bpw_class>(a)->mActionState)) {
    case 0x46:
        for (u32 i = 0; i < 10; i++)
            at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
        if (u32(at<bpw_class>(a)->m3F8) != 0x28)
            anm_init(i_this, dRes_INDEX_BPW_BCK_TAME1_e, 15.f, 0, 1.f, -1); // TAME1
        mons_se(a, 0x4969, 0, true);                                        // JA_SE_CV_BPW_CURSE
        noroi_brk_check(i_this, 0);
        at<bpw_class>(a)->actor.speedF = 0.f;
        search_get_skull(i_this, 2);
        inc_state(a);
        /* fallthrough */
    case 0x47: {
        s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_by_play());
        u32 morf = u32(at<bpw_class>(a)->mpMorf);
        at<bpw_class>(a)->m476 = ang;
        if (!BpwHD_J3DFrameCtrl_checkPass_3(ea(&at<mDoExt_McaMorf>(morf)->mFrameCtrl), 26.f))
            break;
        fire_pos_ground(a);
        BpwHD_dPa_smokeEcallBack_end(ea(&at<bpw_class>(a)->m5E0[1])); // m5E0[1].remove()
        toon_smoke(a, 0xA401, ea(&at<bpw_class>(a)->m5E0[1]));        // BPWJUMPSMOKE00
        se_start_e(a, 0x596A, 0);                                     // JA_SE_CM_BPW_J_BPRESS_JUMP
        at<bpw_class>(a)->actor.speed.y = regf(8, 6, 100.f);
        at<bpw_class>(a)->actor.gravity = -regf(8, 7, 6.f);
        f32 sp = regf(8, 8, 30.f);
        inc_state(a);
        at<bpw_class>(a)->m4A8 = 500.f;
        at<bpw_class>(a)->actor.speedF = sp;
        break;
    }
    case 0x48:
        if (morf_isStop(u32(at<bpw_class>(a)->mpMorf)) && u32(at<bpw_class>(a)->m3F8) != 0x19) {
            anm_init(i_this, dRes_INDEX_BPW_BCK_JUMP1_e, 0.f, 2, 1.f, -1); // JUMP1
            at<bpw_class>(a)->mAttWaitTimer = 0;
            inc_state(a);
        }
        break;
    case 0x49:
        if (!(u32(at<bpw_class>(a)->mAcch.m_flags) & 0x20))
            break;
        se_start(a, 0x596B, 0); // JA_SE_CM_BPW_J_BPRESS_PRESS
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        at<bpw_class>(a)->actor.speedF = 0.f;
        at<bpw_class>(a)->mActionState = 0x4A;
        /* fallthrough */
    case 0x4A: {
        fire_and_emitter_clear(i_this);
        at<bpw_class>(a)->m47E = 200;
        at<bpw_class>(a)->m480 = 200;
        at<bpw_class>(a)->m4A4 = 100.f;
        search_get_skull(i_this, 2);
        at<bpw_class>(a)->actor.gravity = -3.f;
        anm_init(i_this, dRes_INDEX_BPW_BCK_PRESS1_e, 0.f, 0, 1.f, -1); // PRESS1
        at<bpw_class>(a)->m3E8 = 2;
        s16 t6 = regf_t(21, 0, 20.f);
        f32 ground = f32(at<bpw_class>(a)->mAcch.m_ground_h);
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x, &at<bpw_class>(a)->actor.current.pos.x);
        at<bpw_class>(a)->mSomeCountdownTimers[6] = t6;
        at<bpw_class>(a)->mFire1DousaPos.y = ground;
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z, &at<bpw_class>(a)->actor.current.pos.z);
        BpwHD_dPa_smokeEcallBack_end(ea(&at<bpw_class>(a)->m5E0[2])); // m5E0[2].remove()
        toon_smoke(a, 0xA402, ea(&at<bpw_class>(a)->m5E0[2]));        // BPWPRESSSMOKE00
        inc_state(a);
        break;
    }
    case 0x4B:
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m3AC.y), 240.f, 1.f, 10.f);
        at<bpw_class>(a)->m47E = 200;
        at<bpw_class>(a)->m480 = 200;
        if (!(u32(at<bpw_class>(a)->mAcch.m_flags) & 0x20))
            break;
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        at<bpw_class>(a)->actor.speedF = 0.f;
        at<bpw_class>(a)->actor.gravity = 0.f;
        inc_state(a);
        /* fallthrough */
    case 0x4C: {
        u32 anm = u32(at<bpw_class>(a)->m3F8);
        if (anm == 0x23) {
            if (!(f32(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mFrame) <
                  regf(8, 1, 40.f)) &&
                s16(at<bpw_class>(a)->mAttWaitTimer) == 0)
                at<bpw_class>(a)->mAttWaitTimer = 1;
            BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m3AC.y), 340.f, 1.f, 10.f);
        } else if (anm == 0x25) {
            if (!(f32(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mFrame) <
                  regf(8, 1, 30.f)) &&
                s16(at<bpw_class>(a)->mAttWaitTimer) == 0)
                at<bpw_class>(a)->mAttWaitTimer = 1;
        }
        if (!morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
            break;
        at<bpw_class>(a)->m3AC.y = 340.f;
        noroi_brk_check(i_this, 1);
        next_status_clear(i_this, 1);
        if (!next_att_wait_check(i_this) && (u32(at<daPy_py_c>(pl)->mNoResetFlg1) & 0x100)) {
            at<bpw_class>(a)->mBodyAction = ACTION_ATTACK_KOUGEKI;
            at<bpw_class>(a)->mActionState = 0x14;
        }
        break;
    }
    case 0x4D:
        fire_and_emitter_clear(i_this);
        at<bpw_class>(a)->m47E = 200;
        at<bpw_class>(a)->m480 = 200;
        at<bpw_class>(a)->m4A4 = 100.f;
        at<bpw_class>(a)->m4A8 = regf(8, 16, 220.f);
        search_get_skull(i_this, 2);
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        at<bpw_class>(a)->actor.speedF = 0.f;
        noroi_brk_check(i_this, 0);
        anm_init(i_this, dRes_INDEX_BPW_BCK_SONOBA_PRESS1_e, regf(12, 2, 1.f), 0, 1.f,
                 -1); // SONOBA_PRESS1
        mons_se(a, 0x4969, 0, false);
        at<bpw_class>(a)->mAttWaitTimer = 0;
        at<bpw_class>(a)->actor.speedF = 14.75f;
        inc_state(a);
        break;
    case 0x4E: {
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m3AC.y), 240.f, 1.f, 10.f);
        if (!(f32(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mFrame) > 20.f)) {
            s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_by_play());
            at<bpw_class>(a)->m476 = ang;
        }
        if (!(f32(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mFrame) < 40.f))
            BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->actor.speedF), 1.f, 0.55f);
        if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 40.f)) {
            fire_pos_ground(a);
            BpwHD_dPa_smokeEcallBack_end(ea(&at<bpw_class>(a)->m5E0[3])); // m5E0[3].remove()
            toon_smoke(a, 0xA403, ea(&at<bpw_class>(a)->m5E0[3]));        // BPWPRESSSMOKE01
            at<bpw_class>(a)->actor.gravity = -3.f;
            se_start_e(a, 0x5969, 0); // JA_SE_CM_BPW_BODY_PRESS
            at<bpw_class>(a)->m3E8 = 1;
            at<bpw_class>(a)->mSomeCountdownTimers[6] = regf_t(21, 1, 20.f);
            at<bpw_class>(a)->m4A8 = regf(8, 17, 400.f);
        }
        if (morf_isStop(u32(at<bpw_class>(a)->mpMorf))) {
            at<bpw_class>(a)->mSomeCountdownTimers[0] = 5;
            inc_state(a);
        }
        break;
    }
    case 0x4F:
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0)
            break;
        anm_init(i_this, dRes_INDEX_BPW_BCK_SONOBA_PRESS2_e, regf(12, 2, 1.f), 0, 1.f,
                 -1); // SONOBA_PRESS2
        at<bpw_class>(a)->mActionState = 0x4C;
        break;
    }
    if (s16(at<bpw_class>(a)->mActionState) >= 0x4A)
        alpha_anime(i_this);
    noroi_check(i_this);
}

/* player->setPlayerPosAndAngle(&pos, angle): player vtable +0x114 */
static void set_player_pos(u32 pl, u32 posLocal, s16 angle) {
    call_ptr<void>(u32(at<BpwPlayerVtableHD>(at<fopAc_ac_c>(pl)->__vtbl)->setPlayerPosAndAngle), pl,
                   posLocal, (s32)angle);
}
/* sp70 = (-36, player y, -777); setPlayerPosAndAngle(&sp70, atan2s(player - mBodyPos) + 0x8000) */
static void player_to_room_body(u32 a, u32 pl) {
    Local<BpwVecHD> p;
    f32 bz = f32(at<bpw_class>(a)->mBodyPos.z), pz = f32(at<fopAc_ac_c>(pl)->current.pos.z),
        bx = f32(at<bpw_class>(a)->mBodyPos.x), px = f32(at<fopAc_ac_c>(pl)->current.pos.x);
    p->x = -36.f;
    p->y = f32(at<fopAc_ac_c>(pl)->current.pos.y);
    p->z = -777.f;
    u32 vt = u32(at<fopAc_ac_c>(pl)->__vtbl);
    s16 ang = BpwHD_cM_atan2s(sub(px, bx), sub(pz, bz));
    call_ptr<void>(u32(at<BpwPlayerVtableHD>(vt)->setPlayerPosAndAngle), pl, p.get(),
                   (s32)(s16)(ang + 0x8000));
}
/* sp70 = (-36, player y, -777); setPlayerPosAndAngle(&sp70, searchPlayerAngleY(actor) + 0x8000) */
static void player_to_room_face(u32 a, u32 pl) {
    Local<BpwVecHD> p;
    p->x = -36.f;
    p->y = f32(at<fopAc_ac_c>(pl)->current.pos.y);
    p->z = -777.f;
    u32 vt = u32(at<fopAc_ac_c>(pl)->__vtbl);
    s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
    call_ptr<void>(u32(at<BpwPlayerVtableHD>(vt)->setPlayerPosAndAngle), pl, p.get(),
                   (s32)(s16)(ang + 0x8000));
}
static void cam_reset_start(u32 a, u32 cam, u32 pl) {
    BpwHD_dVibration_c_StopQuake(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mVibration),
                                 0x20); // StopQuake(0x20)
    Local<BpwVecHD> center, eye;
    eye->z = f32(at<bpw_class>(a)->m418.z);
    center->y = f32(at<bpw_class>(a)->m40C.y);
    eye->x = f32(at<bpw_class>(a)->m418.x);
    center->x = f32(at<bpw_class>(a)->m40C.x);
    eye->y = f32(at<bpw_class>(a)->m418.y);
    center->z = f32(at<bpw_class>(a)->m40C.z);
    BpwHD_dCamera_c_Reset(ea(&at<BpwCameraHD>(cam)->mCamera), center.get(), eye.get());
    BpwHD_dCamera_c_Start(ea(&at<BpwCameraHD>(cam)->mCamera));
    BpwHD_dCamera_c_SetTrimSize(ea(&at<BpwCameraHD>(cam)->mCamera), 0);
    at<daPy_py_c>(pl)->mDemo.mDemoType = 2;
    at<daPy_py_c>(pl)->mDemo.mDemoMode = 1; // cancelOriginalDemo()
    u32 play = BpwHD_getGameInfo();
    at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag =
        (u16)(u16(at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag) | 8); // dComIfGp_event_reset()
}
static bool demo_accept(u32 a) { // eventInfo.checkCommandDemoAccrpt(), else order the event
    if (u16(at<bpw_class>(a)->actor.eventInfo.mCommand) == 2)
        return true;
    u32 play = BpwHD_getGameInfo();
    at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag =
        (u16)(u16(at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag) | 1);
    BpwHD_fopAcM_orderPotentialEvent(a, 2, 0xFFFF, 0);
    at<bpw_class>(a)->actor.eventInfo.mCondition =
        (u16)(u16(at<bpw_class>(a)->actor.eventInfo.mCondition) | 2);
    return false;
}

/* 020D0C04 */
BOOL body_nodeCallBack(u32 node, s32 calcTiming) {
    if (calcTiming != 0)
        return TRUE;
    u32 joint = BpwHD_J3DNode_toJoint(node);
    u32 model = u32(l_j3dSys.mpModel), a = u32(at<BpwModelHD>(model)->mUserArea);
    u32 jnt = u16(at<BpwJointHD>(joint)->mJntNo);
    if (!a)
        return TRUE;
    if (!(jnt == BPW_JNT_BPW_ALLROOT_e || jnt == BPW_JNT_J_BPW_KOSI1_e ||
          jnt == BPW_JNT_J_BPW_UDE_R3_e || jnt == BPW_JNT_J_BPW_ITEM_e ||
          jnt == BPW_JNT_J_BPW_AGO1_e))
        return TRUE;
    bpw_joint_load(model, jnt);
    Local<BpwVecHD> v;
    if (jnt == BPW_JNT_BPW_ALLROOT_e) { // BPW_ALLROOT
        v->x = 0.f;
        v->y = f32(at<bpw_class>(a)->m4A0);
        v->z = 0.f;
        BpwHD_MtxPosition(v.get(), ea(&at<bpw_class>(a)->m358)); // m358
        v->x = 0.f;
        v->y = -50.f;
        v->z = 0.f;
        BpwHD_MtxPosition(v.get(), ea(&at<bpw_class>(a)->m370)); // m370
    } else {
        v->x = 0.f;
        v->y = 0.f;
        v->z = 0.f;
        cXyz *dst = jnt == BPW_JNT_J_BPW_KOSI1_e    ? &at<bpw_class>(a)->m394
                    : jnt == BPW_JNT_J_BPW_UDE_R3_e ? &at<bpw_class>(a)->m3B8
                    : jnt == BPW_JNT_J_BPW_ITEM_e   ? &at<bpw_class>(a)->mChildActorPos
                                                    : &at<bpw_class>(a)->m3C4;
        BpwHD_MtxPosition(v.get(), ea(dst));
    }
    bpw_joint_store(model, jnt);
    return TRUE;
}

/* 020D0DF0 */
BOOL kantera_nodeCallBack(u32 node, s32 calcTiming) {
    if (calcTiming != 0)
        return TRUE;
    u32 joint = BpwHD_J3DNode_toJoint(node);
    u32 model = u32(l_j3dSys.mpModel), a = u32(at<BpwModelHD>(model)->mUserArea);
    u32 jnt = u16(at<BpwJointHD>(joint)->mJntNo);
    if (!a)
        return TRUE;
    if (jnt != 0 && jnt != 1)
        return TRUE;
    bpw_joint_load(model, jnt);
    if (jnt == 1) { // BPW_KAN1_JNT_KANTERA
        BpwHD_mDoMtx_YrotM_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->mKanteraDousaRot.y));
        BpwHD_mDoMtx_XrotM_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->mKanteraDousaRot.x));
        BpwHD_mDoMtx_ZrotM_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->mKanteraDousaRot.z));
        Local<BpwVecHD> v;
        v->x = 0.f;
        v->y = -100.f;
        v->z = 0.f;
        BpwHD_MtxPosition(v.get(), ea(&at<bpw_class>(a)->m358)); // m358
    }
    bpw_joint_store(model, jnt);
    return TRUE;
}

/* 020D0FEC */
void draw_SUB(bpw_class *i_this) {
    u32 a = ea(i_this), model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    at<BpwModelHD>(model)->mBaseScale.x = f32(at<bpw_class>(a)->actor.scale.x);
    at<BpwModelHD>(model)->mBaseScale.z = f32(at<bpw_class>(a)->actor.scale.z);
    at<BpwModelHD>(model)->mBaseScale.y = f32(at<bpw_class>(a)->actor.scale.y);
    f32 y = f32(at<bpw_class>(a)->actor.current.pos.y), oz = f32(at<bpw_class>(a)->m3AC.z),
        ox = f32(at<bpw_class>(a)->m3AC.x), x = f32(at<bpw_class>(a)->actor.current.pos.x);
    f32 oy = f32(at<bpw_class>(a)->m3AC.y), z = f32(at<bpw_class>(a)->actor.current.pos.z);
    BpwHD_PSMTXTrans(stack_now, add(x, ox), add(y, oy), add(z, oz));
    BpwHD_mDoMtx_YrotM(stack_now, (s32)s16(at<bpw_class>(a)->m3D6.y));
    BpwHD_mDoMtx_XrotM(stack_now, (s32)s16(at<bpw_class>(a)->m3D6.x));
    BpwHD_mDoMtx_YrotM(stack_now, (s32)(s16)-s16(at<bpw_class>(a)->m3D6.y));
    BpwHD_mDoMtx_YrotM(stack_now, (s32)s16(at<bpw_class>(a)->actor.shape_angle.y));
    BpwHD_mDoMtx_XrotM(stack_now, (s32)s16(at<bpw_class>(a)->actor.shape_angle.x));
    BpwHD_mDoMtx_ZrotM(stack_now, (s32)s16(at<bpw_class>(a)->actor.shape_angle.z));
    f32 m[12];
    for (u32 i = 0; i < 12; i++)
        m[i] = f32(at<Mtx34>(stack_now)->m[i / 4][i % 4]);
    for (u32 i = 0; i < 12; i++)
        at<BpwModelHD>(model)->mBaseTransformMtx[i] = m[i];
    u8 type = u8(at<bpw_class>(a)->mType);
    if (type == 2)
        return;
    if (type != 3)
        BpwHD_mDoExt_McaMorf_calc(u32(at<bpw_class>(a)->mpMorf));
    u32 env = BpwHD_dKy_getEnvlight();
    BpwHD_dScnKy_env_light_c_settingTevStruct(env, 0, ea(&at<bpw_class>(a)->actor.current.pos),
                                              ea(&at<bpw_class>(a)->actor.tevStr));
}

/* 020D1158 */
void kantera_draw_SUB(bpw_class *i_this) {
    u32 a = ea(i_this), model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    at<BpwModelHD>(model)->mBaseScale.x = f32(at<bpw_class>(a)->actor.scale.x);
    at<BpwModelHD>(model)->mBaseScale.z = f32(at<bpw_class>(a)->actor.scale.z);
    at<BpwModelHD>(model)->mBaseScale.y = f32(at<bpw_class>(a)->actor.scale.y);
    BpwHD_MtxTrans(f32(at<bpw_class>(a)->actor.current.pos.x),
                   f32(at<bpw_class>(a)->actor.current.pos.y),
                   f32(at<bpw_class>(a)->actor.current.pos.z), 0);
    BpwHD_mDoMtx_YrotM_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->actor.shape_angle.y));
    BpwHD_mDoMtx_XrotM_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->actor.shape_angle.x));
    BpwHD_mDoMtx_ZrotM_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->actor.shape_angle.z));
    BpwHD_MtxTrans(f32(at<bpw_class>(a)->m3AC.x), f32(at<bpw_class>(a)->m3AC.y),
                   f32(at<bpw_class>(a)->m3AC.z), 1);
    u32 mtx = u32(l_calcMtx);
    f32 m[12];
    for (u32 i = 0; i < 12; i++)
        m[i] = f32(at<Mtx34>(mtx)->m[i / 4][i % 4]);
    for (u32 i = 0; i < 12; i++)
        at<BpwModelHD>(model)->mBaseTransformMtx[i] = m[i];
    BpwHD_mDoExt_McaMorf_calc(u32(at<bpw_class>(a)->mpMorf));
    u32 env = BpwHD_dKy_getEnvlight();
    BpwHD_dScnKy_env_light_c_settingTevStruct(env, 0, ea(&at<bpw_class>(a)->actor.current.pos),
                                              ea(&at<bpw_class>(a)->actor.tevStr));
}

/* : inlines body_draw, kantera_draw, damage_ball_draw, torituki_draw.
 * HD: no ground shadows (setShadow/addRealShadow), no alpha-model spheres for the lantern and the
 * damage ball (only their matrices are still computed), no Ganondorf figure registration; the
 * brk remove() is a store of 0 to modelData+0x48; material alpha goes through the HD TEV block. */
/* 020D1334 */
BOOL daBPW_Draw(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    s16 blur = s16(at<bpw_class>(a)->m408); // m408
    if (blur > 1) {
        l_blur.mRate = (u8)blur;
        BpwHD_mDoGph_gInf_c_onBlure();
    } // setBlureRate, onBlure
    else if (blur == 1) {
        at<bpw_class>(a)->m408 = 0;
        l_blur.mEnabled = 0;
    } // offBlure
    if (u8(at<bpw_class>(a)->mType) != ACTOR_TYPE_DAMAGE_BALL) {
        u32 env = BpwHD_dKy_getEnvlight();
        BpwHD_dKy_setLightTevColorType(
            env, model,
            ea(&at<bpw_class>(a)->actor.tevStr)); // setLightTevColorType(model, &tevStr)
    }
    BpwHD_getGameInfo();
    switch (u8(at<bpw_class>(a)->mType)) {
    case 0: { // body_draw
        u32 m = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v),
            md = u32(at<BpwModelHD>(m)->mpModelData);
        bpw_material_alpha(a, md);
        if (u8(at<bpw_class>(a)->m3E0) == 0) {
            setListMaskOff();
            BpwHD_J3DModel_setVisibility(m, 1);
        } else
            BpwHD_J3DModel_setVisibility(m, 0);
        u32 md2 = u32(at<BpwModelHD>(m)->mpModelData);
        u32 brk;
        if (u8(at<bpw_class>(a)->m3E0))
            brk = u32(at<bpw_class>(a)->mpLightFreezeBrkAnm);
        else if (u8(at<bpw_class>(a)->m3E1))
            brk = u32(at<bpw_class>(a)->mpLightStunBrkAnm);
        else {
            u8 e2 = u8(at<bpw_class>(a)->m3E2);
            brk = u32(at<bpw_class>(a)->*(e2 == 2   ? &bpw_class::mpCurseStartBrkAnm
                                          : e2 == 1 ? &bpw_class::mpCurseEndBrkAnm
                                                    : &bpw_class::mpDefaultBrkAnm));
        }
        brk_entry(brk, md2);
        BpwHD_mDoExt_McaMorf_entryDL(u32(at<bpw_class>(a)->mpMorf)); // entryDL
        setList();
        if (u8(at<bpw_class>(a)->m3E0) == 0)
            BpwHD_mDoExt_invisibleModel_entryMaskOff(
                ea(&at<bpw_class>(a)->mInvisibleModel)); // mInvisibleModel.entryMaskOff()
        else
            BpwHD_mDoExt_invisibleModel_entry(
                ea(&at<bpw_class>(a)->mInvisibleModel)); // mInvisibleModel.entry()
        u8 e0 = u8(at<bpw_class>(a)->m3E0);
        u32 md3 = u32(at<BpwModelHD>(m)->mpModelData);
        // HD: keep the otherwise unused conditional field load.
        if (e0 == 0 && u8(at<bpw_class>(a)->m3E1) == 0)
            (void)u8(at<bpw_class>(a)->m3E2);
        at<BpwModelDataHD>(md3)->mpTevRegAnimator = 0; // brk remove()
        if (s16(at<bpw_class>(a)->m47E) == 0)
            return TRUE;
        if (u32(at<bpw_class>(a)->actor.actor_status) & 0x2000)
            return TRUE;                               // fopAcM_checkCarryNow
        BpwHD_dSnap_RegistFig(0xCB, a, 1.f, 1.f, 1.f); // dSnap_RegistFig(DSNAP_TYPE_BPW)
        return TRUE;
    }
    case 1: { // kantera_draw
        u32 morf = u32(at<bpw_class>(a)->mpMorf);
        Local<BpwVecHD> zero, pos;
        u32 m = u32(at<mDoExt_McaMorf>(morf)->mpModel.v);
        zero->x = 0.f;
        zero->z = 0.f;
        zero->y = 0.f;
        if (u8(at<bpw_class>(a)->m3E7) == 0) {
            brk_entry(u32(at<bpw_class>(a)->mpLanternGlowBrkAnm),
                      u32(at<BpwModelHD>(m)->mpModelData));
            morf = u32(at<bpw_class>(a)->mpMorf);
        }
        BpwHD_mDoExt_McaMorf_entryDL_2(morf);
        if (u8(at<bpw_class>(a)->m3E7) != 0)
            return TRUE;
        at<BpwModelDataHD>(at<BpwModelHD>(m)->mpModelData)->mpTevRegAnimator = 0;
        if (u8(at<bpw_class>(a)->m3E7) != 0)
            return TRUE;
        u32 bm = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
        u32 mtx = u32(l_calcMtx);
        BpwHD_PSMTXCopy_3(bm ? bm + 0xC8 : 0u, mtx);
        BpwHD_MtxPosition_2(zero.get(), pos.get());
        BpwHD_MtxTrans_2((f32)pos->x, (f32)pos->y, (f32)pos->z, 0);
        BpwHD_MtxScale(3.f, 3.f, 3.f, 1);
        BpwHD_PSMTXCopy_3(u32(l_calcMtx), ea(&at<bpw_class>(a)->m2D4)); // m2D4
        return TRUE;
    }
    case 2: { // damage_ball_draw
        if (u8(at<bpw_class>(a)->mUnknownParam2) != 1)
            return TRUE;
        BpwHD_MtxTrans(f32(at<bpw_class>(a)->actor.current.pos.x),
                       f32(at<bpw_class>(a)->actor.current.pos.y),
                       f32(at<bpw_class>(a)->actor.current.pos.z), 0);
        BpwHD_MtxScale(0.8f, 0.8f, 0.8f, 1);
        BpwHD_mDoMtx_YrotM_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->actor.shape_angle.y));
        BpwHD_PSMTXCopy_3(u32(l_calcMtx), ea(&at<bpw_class>(a)->m2D4));
        return TRUE;
    }
    case 3: { // torituki_draw
        u32 md = u32(
            at<BpwModelHD>(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mpModel.v)->mpModelData);
        bpw_material_alpha(a, md);
        setListMaskOff();
        BpwHD_mDoExt_McaMorf_updateDL(u32(at<bpw_class>(a)->mpMorf)); // updateDL
        setList();
        BpwHD_mDoExt_invisibleModel_entryMaskOff(ea(&at<bpw_class>(a)->mInvisibleModel));
        return TRUE;
    }
    }
    return TRUE;
}

/* 020D1A64 */
void anm_init(bpw_class *i_this, s32 bckFileIdx, f32 morf, u8 loopMode, f32 speed,
              s32 soundFileIdx) {
    u32 a = ea(i_this);
    at<bpw_class>(a)->m3F8 = bckFileIdx; // m3F8
    if (soundFileIdx >= 0) {
        u32 bck = getObjectRes(0x1000B160, bckFileIdx);
        u32 snd = getObjectRes(0x1000B160, soundFileIdx);
        BpwHD_mDoExt_McaMorf_setAnm(u32(at<bpw_class>(a)->mpMorf), bck, (u32)loopMode, morf, speed,
                                    0.f, -1.f, snd);
    } else {
        u32 bck = getObjectRes(0x1000B160, bckFileIdx);
        BpwHD_mDoExt_McaMorf_setAnm_2(u32(at<bpw_class>(a)->mpMorf), bck, (u32)loopMode, morf,
                                      speed, 0.f, -1.f, 0u);
    }
}

/* (HD: placed after daBPW_Create) */
/* 020D6D98 */
BOOL body_atari_check(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 player = player_actor();
    at<bpw_class>(a)->mHitType = 0; // mHitType
    if (s16(at<bpw_class>(a)->m47E) == 0)
        return FALSE;
    BpwHD_dCcD_GStts_Move(ea(&at<bpw_class>(a)->mStts.__vtbl_gstts)); // mStts.Move()
    if (!BpwHD_dCcD_GObjInf_ChkTgHit(ea(&at<bpw_class>(a)->mBodyCoSph)))
        return FALSE; // mBodyCoSph.ChkTgHit()
    u32 hit = BpwHD_dCcD_GObjInf_GetTgHitObj(ea(&at<bpw_class>(a)->mBodyCoSph));
    if (!hit)
        return FALSE;
    BpwHD_dCcD_GObjInf_GetTgHitObj(ea(&at<bpw_class>(a)->mBodyCoSph));
    at<bpw_class>(a)->mHitType = 1;
    if (u8(at<bpw_class>(a)->m3E0) == 0) { // m3E0
        u32 type = u32(at<dCcD_GObjInf>(hit)->mAtType);
        if (type == 0x100000 || type == 0x800000) { // AT_TYPE_LIGHT_ARROW / AT_TYPE_LIGHT
            at<bpw_class>(a)->mHitType = type == 0x100000 ? 0xB : 0xA;
            if (s16(at<bpw_class>(a)->mBodyAction) != ACTION_DAMAGE) {
                at<bpw_class>(a)->mBodyAction = ACTION_DAMAGE;
                at<bpw_class>(a)->mActionState = 0x50;
            }
        }
        return TRUE;
    }
    if (s16(at<bpw_class>(a)->mActionState) != 0x53)
        return FALSE;
    u32 anm = 7; // BOYON_S1
    switch (u32(at<dCcD_GObjInf>(hit)->mAtType)) {
    case 2:
    case 0x400:
    case 0x800:
    case 0x4000000:
    case 0x10000000: { // swords
        se_start(a, 0x2803, 0x44);
        u8 cut = u8(at<BpwPlayerCutHD>(player)->mCutType);
        if ((cut >= 5 && cut <= 10) || cut == 12 || (cut >= 14 && cut <= 16) || cut == 21 ||
            cut == 23 || (cut >= 25 && cut <= 27) || cut == 30 || cut == 31) {
            at<bpw_class>(a)->mHitType = 2;
            anm = 6;
        }
        break;
    }
    case 0x8000: // hookshot
        se_start(a, 0x2834, 0x44);
        at<bpw_class>(a)->mHitType = 0xC;
        break;
    case 0x200000: // wind
        at<bpw_class>(a)->mHitType = 4;
        return FALSE;
    case 0x800000: // light
        at<bpw_class>(a)->mHitType = 10;
        return FALSE;
    case 0x40: // boomerang
        at<bpw_class>(a)->mHitType = 5;
        /* fallthrough */
    case 0x80:
    case 0x1000000: // mace, boko stick
        se_start(a, 0x2833, 0x44);
        break;
    case 0x10000: // skull hammer
        se_start(a, 0x2855, 0x44);
        at<bpw_class>(a)->mHitType = 8;
        if (u8(at<BpwPlayerCutHD>(player)->mCutType) == 0x11)
            at<bpw_class>(a)->mHitType = 9;
        anm = 6;
        break;
    case 0x20: // bomb
        at<bpw_class>(a)->mHitType = 7;
        anm = 6;
        break;
    case 0x80000: // ice arrow
        at<bpw_class>(a)->mHitType = 6;
        se_start(a, 0x2834, 0x44);
        break;
    case 0x100000: // light arrow
        at<bpw_class>(a)->mHitType = 0xB;
        se_start(a, 0x2834, 0x44);
        break;
    default:
        at<bpw_class>(a)->mHitType = 0;
        se_start(a, 0x2834, 0x44);
        break;
    }
    anm_init(i_this, anm, 2.f, 0, 1.f, -1);
    Local<BpwVecHD> pos, scale;
    scale->y = 2.f;
    f32 py = f32(at<bpw_class>(a)->mBodyCoSph.mGObjTg.mHitPos.y),
        px = f32(at<bpw_class>(a)->mBodyCoSph.mGObjTg.mHitPos.x); // *mBodyCoSph.GetTgHitPosP()
    pos->y = py;
    pos->x = px;
    scale->z = 2.f;
    pos->z = f32(at<bpw_class>(a)->mBodyCoSph.mGObjTg.mHitPos.z);
    scale->x = 2.f;
    u32 play = BpwHD_getGameInfo();
    BpwHD_dPa_control_c_set_2(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0, 0xC, pos.get(),
                              player + 0x328, scale.get(), 0xFF, 0, -1, 0, 0, 0);
    return FALSE;
}

/* HD: keeps the boss in front of a z limit (global at  + 1800) */
/* 020D1B90 */
void BG_check(bpw_class *i_this) {
    u32 a = ea(i_this);
    BpwHD_dBgS_AcchCir_SetWall(ea(&at<bpw_class>(a)->mAcchCir), f32(at<bpw_class>(a)->m498),
                               f32(at<bpw_class>(a)->m49C)); // mAcchCir.SetWall(m498, m49C)
    f32 off = f32(at<bpw_class>(a)->m48C);                   // m48C
    at<bpw_class>(a)->actor.current.pos.y = sub(f32(at<bpw_class>(a)->actor.current.pos.y), off);
    at<bpw_class>(a)->actor.old.pos.y = sub(f32(at<bpw_class>(a)->actor.old.pos.y), off);
    u32 play = BpwHD_getGameInfo();
    BpwHD_dBgS_Acch_CrrPos(ea(&at<bpw_class>(a)->mAcch), play + 0x12A0);
    off = f32(at<bpw_class>(a)->m48C);
    f32 y = f32(at<bpw_class>(a)->actor.current.pos.y), oy = f32(at<bpw_class>(a)->actor.old.pos.y);
    at<bpw_class>(a)->actor.current.pos.y = add(y, off);
    at<bpw_class>(a)->actor.old.pos.y = add(oy, off);
    f32 lim = add(f32(BpwHio(10).mFloat[5]), 1800.f);
    if (f32(at<bpw_class>(a)->actor.current.pos.z) > lim) {
        at<bpw_class>(a)->actor.current.pos.z = lim;
        at<bpw_class>(a)->actor.old.pos.z = add(f32(BpwHio(10).mFloat[5]), 1800.f);
    }
}

/* 020D1C40 */
void alpha_anime(bpw_class *i_this) {
    u32 a = ea(i_this);
    if (u8(at<bpw_class>(a)->m3E0) != 0)
        return; // m3E0
    u32 play = BpwHD_getGameInfo();
    if (BpwHD_dDetect_c_chk_light(
            play + 0x5A20,
            ea(&at<bpw_class>(a)
                    ->actor.current.pos))) { // dComIfGp_getDetect().chk_light(&current.pos)
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m47E), 0, 1,
                                  0x14); // cLib_addCalcAngleS2(&m47E, 0, 1, 0x14)
        at<bpw_class>(a)->m480 = 0;
        at<bpw_class>(a)->m478 = 0;
    } else {
        at<bpw_class>(a)->m478 = (s16)(s16(at<bpw_class>(a)->m478) + 0x400);
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m480), 0x96, 1, 10);
        f32 base = (f32)s16(at<bpw_class>(a)->m480);
        f32 v = fmadds(Bpw_sin(u16(at<bpw_class>(a)->m478)), 30.f, base);
        at<bpw_class>(a)->m47E = (s16)ftoi(v);
    }
}

/* HD: calls BpwHD_getGameInfo() (result unused) */
/* 020D1D3C */
void fuwafuwa_calc(bpw_class *i_this) {
    u32 a = ea(i_this);
    BpwHD_getGameInfo();
    s16 t = (s16)(s16(at<bpw_class>(a)->m47C) + 700);     // m47C
    f32 ground = f32(at<bpw_class>(a)->mAcch.m_ground_h); // mAcch.GetGroundH()
    at<bpw_class>(a)->m47C = t;
    f32 y = add(ground, 30.f);
    at<bpw_class>(a)->actor.current.pos.y = y;
    at<bpw_class>(a)->actor.current.pos.y = fmadds(Bpw_sin((u16)t), 30.f, y);
}

/* HD: calls BpwHD_getGameInfo() first (result unused) */
/* 020D1DA8 */
void kankyou_hendou(bpw_class *i_this) {
    u32 a = ea(i_this);
    BpwHD_getGameInfo();
    f32 target = 0.f;
    u32 col = 1;
    switch (u8(at<bpw_class>(a)->mKankyouHendouState)) { // mKankyouHendouState
    case 0:
        break;
    case 1:
        target = 1.f;
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[5]) == 1)
            at<bpw_class>(a)->mKankyouHendouState = 0;
        break;
    case 2:
        target = 1.f;
        col = 2;
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[5]) == 1)
            at<bpw_class>(a)->mKankyouHendouState = 4;
        break;
    case 3:
        target = 1.f;
        col = 3;
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[5]) == 1)
            at<bpw_class>(a)->mKankyouHendouState = 5;
        break;
    case 4:
        col = 2;
        break;
    case 5:
        col = 3;
        break;
    case 6:
        target = 1.f;
        col = 0;
        break;
    default:
        break;
    }
    BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m3EC), target, 1.f,
                        0.1f); // cLib_addCalc2(&m3EC, ...)
    u32 base = u8(at<bpw_class>(a)->mKankyouHendouState) < 6 ? 4 : 3;
    f32 v = f32(at<bpw_class>(a)->m3EC);
    if (v > 0.5f)
        BpwHD_dKy_custom_colset(base, col, v);
    else
        BpwHD_dKy_custom_colset(col, base, sub(1.f, v));
}

/* 020D2058 */
void noroi_check(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 player = player_actor();
    if (u32(at<daPy_py_c>(player)->mNoResetFlg1) & 0x100)
        return; // checkConfuse()
    if (s16(at<bpw_class>(a)->m47E) == 0)
        return; // m47E
    s16 t = s16(at<bpw_class>(a)->mAttWaitTimer);
    if (t != 1 && t != 2 && t != 0)
        return;
    if (s16(at<bpw_class>(a)->m3F6) != 0 && u8(at<bpw_class>(a)->m3E6) == 0 &&
        s16(at<bpw_class>(a)->mBodyAction) != ACTION_BODY_DOWN_KARADA_TAORE) {
        s16 st = s16(at<bpw_class>(a)->mActionState);
        if (st == 10 || st == 11 || st == 12 || st == 13)
            return;
        u32 anm = u32(at<bpw_class>(a)->m3F8);
        if (!(anm == 0x1C || anm == 0x1D || anm == 0x2A || anm == 0x2B))
            return;
        u32 play = BpwHD_getGameInfo();
        f32 d = BpwHD_fopAcM_searchActorDistance(
            a, u32(at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v));
        if (d < 530.f) {
            at<bpw_class>(a)->mBodyAction = ACTION_BODY_DOWN_KARADA_TAORE;
            at<bpw_class>(a)->mActionState = 0x4D;
        }
        return;
    }
    s16 st = s16(at<bpw_class>(a)->mActionState);
    if (!(st == 5 || st == 6 ||
          s16(at<bpw_class>(a)->mBodyAction) == ACTION_BODY_DOWN_KARADA_TAORE))
        return;
    if (s16(at<bpw_class>(a)->mSomeCountdownTimers[3]) != 0)
        return;
    if (!BpwHD_dCcD_GObjInf_ChkAtHit(ea(&at<bpw_class>(a)->mBodyAtSph)))
        return; // mBodyAtSph.ChkAtHit()
    u32 hit = BpwHD_dCcD_GAtTgCoCommonBase_GetAc(
        ea(&at<bpw_class>(a)->mBodyAtSph.mGObjAt)); // GetAtHitAc()
    if (!hit || hit != player)
        return;
    at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm =
        u32(at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm) & ~1u; // mBodyCoSph.OffTgSetBit()
    at<bpw_class>(a)->m490 = 0.f;
    at<bpw_class>(a)->actor.speedF = 0.f;
    BpwHD_dCcD_GObjInf_ClrTgHit(ea(&at<bpw_class>(a)->mBodyCoSph)); // mBodyCoSph.ClrTgHit()
    at<daPy_py_c>(player)->mNoResetFlg1 =
        u32(at<daPy_py_c>(player)->mNoResetFlg1) | 0x100; // onConfuse()
    struct SVec {
        be<u16> x, y, z;
    };
    Local<SVec> ang;
    ang->x = u16(at<bpw_class>(a)->actor.shape_angle.x);
    ang->y = u16(at<bpw_class>(a)->actor.shape_angle.y);
    at<bpw_class>(a)->mSomeCountdownTimers[3] = 0x14;
    ang->z = u16(at<bpw_class>(a)->actor.shape_angle.z);
    s32 camId = s8(at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mPlayerInfo[0].mCameraID);
    u32 play = BpwHD_getGameInfo();
    u32 camera =
        u32(at<dComIfG_camera_info_class>(ea(&at<dComIfG_inf_c>(play)->play.mCameraInfo[0]) +
                                          camId * sizeof(dComIfG_camera_info_class))
                ->mpCamera);
    at<bpw_class>(a)->mFire1DousaPos.x = f32(at<daPy_py_c>(player)->mHeadTopPos.x);
    at<bpw_class>(a)->mFire1DousaPos.z = f32(at<daPy_py_c>(player)->mHeadTopPos.z);
    at<bpw_class>(a)->mFire1DousaPos.y =
        f32(at<daPy_py_c>(player)->mHeadTopPos.y); // mFire1DousaPos = getHeadTopPos()
    play = BpwHD_getGameInfo();
    BpwHD_dPa_control_c_set(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0, 0x27B,
                            ea(&at<bpw_class>(a)->mFire1DousaPos), 0, 0, 0xFF, 0, -1, 0, 0, 0);
    ang->y = u16(at<BpwCameraHD>(camera)->mAngleY); // fopCamM_GetAngleY
    BpwHD_fopAcM_create(0xD3, 3, ea(&at<bpw_class>(a)->mFire1DousaPos),
                        (s32)s8(at<bpw_class>(a)->actor.current.roomNo), ang.get(), 0, -1, 0);
}

/* 020D22B8 */
void fire_and_emitter_clear(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 id = u32(at<bpw_class>(a)->m400); // m400
    if (id != 0xFFFFFFFF) {
        Local<be<u32>> key;
        key->set(id);
        u32 other = BpwHD_fopAcIt_Judge(0x025E1234u, key.get()); // fopAcM_SearchByID
        if (other && u8(at<bpw_class>(other)->mUnknownParam2) == 0) {
            BpwHD_fopAcM_delete(other);
            at<bpw_class>(a)->m400 = 0xFFFFFFFF;
        }
    }
    BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->mFire1Dousa_Pa_followEcallBack));
    BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->mFire1Dousa_Pa_followEcallBack2));
    BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->m590));
    BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->m540));
    BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->m52C));
    BpwHD_dPa_smokeEcallBack_end(ea(&at<bpw_class>(a)->m50C));
}

/* HD: calls BpwHD_getGameInfo() first (result unused) */
/* 020D2350 */
void noroi_brk_check(bpw_class *i_this, u8 mode) {
    u32 a = ea(i_this);
    BpwHD_getGameInfo();
    u32 model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    switch (mode) {
    case 0:
        if (u8(at<bpw_class>(a)->m3E2) != 2) {
            at<bpw_class>(a)->m3E2 = 2;
            brk_init(a, &bpw_class::mpCurseStartBrkAnm, model, 0x1000B180, 0x37);
        }
        break;
    case 1:
        if (u8(at<bpw_class>(a)->m3E2) == 2) {
            at<bpw_class>(a)->m3E2 = 1;
            brk_init(a, &bpw_class::mpCurseEndBrkAnm, model, 0x1000B180, 0x36);
        }
        break;
    case 2:
        at<bpw_class>(a)->m3E2 = 0;
        brk_init(a, &bpw_class::mpDefaultBrkAnm, model, 0x1000B180, 0x33);
        break;
    }
}

/* HD: calls BpwHD_getGameInfo() first (result unused) */
/* 020D24F8 */
BOOL next_att_wait_check(bpw_class *i_this) {
    u32 a = ea(i_this);
    BpwHD_getGameInfo();
    s16 t = s16(at<bpw_class>(a)->mAttWaitTimer);
    if (t != 1 && t != 2 && t != 0) {
        at<bpw_class>(a)->mBodyAction = ACTION_MOVE_DOUSA;
        at<bpw_class>(a)->mActionState = 10;
        return TRUE;
    }
    at<bpw_class>(a)->mBodyAction = ACTION_MOVE_DOUSA;
    at<bpw_class>(a)->mActionState = 0;
    at<bpw_class>(a)->mAttWaitTimer = 1;
    return FALSE;
}

/* HD: calls BpwHD_getGameInfo() first (result unused) */
/* 020D257C */
void next_status_clear(bpw_class *i_this, u8 keepAlpha) {
    u32 a = ea(i_this);
    BpwHD_getGameInfo();
    at<bpw_class>(a)->actor.gravity = 0.f; // gravity
    at<bpw_class>(a)->actor.speed.x = 0.f;
    at<bpw_class>(a)->actor.speed.y = 0.f;
    at<bpw_class>(a)->actor.speed.z = 0.f; // speed
    at<bpw_class>(a)->actor.speedF = 0.f;
    at<bpw_class>(a)->m490 = 0.f; // speedF, m490
    at<bpw_class>(a)->m3E6 = 0;
    at<bpw_class>(a)->m3E5 = 0;                                     // m3E6, m3E5
    at<bpw_class>(a)->m4A8 = add(f32(BpwHio(8).mFloat[16]), 220.f); // m4A8 = REG8_F(16) + 220
    at<bpw_class>(a)->m3AC.y = 340.f;                               // m3AC.y
    noroi_brk_check(i_this, 1);
    u8 e1 = u8(at<bpw_class>(a)->m3E1);
    at<bpw_class>(a)->actor.shape_angle.x = 0;
    at<bpw_class>(a)->m4A4 = 300.f; // m4A4
    if (keepAlpha == 0) {
        at<bpw_class>(a)->m480 = 0;
        at<bpw_class>(a)->m478 = 0;
        at<bpw_class>(a)->m3E0 = 0;
    }
    if (e1 != 0) {
        u32 model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
        at<bpw_class>(a)->m3E1 = 0;
        brk_init(a, &bpw_class::mpDefaultBrkAnm, model, 0x1000B198, 0x33);
    }
    if (s16(at<bpw_class>(a)->mAttWaitTimer) == 0)
        at<bpw_class>(a)->mAttWaitTimer = 1;
    at<bpw_class>(a)->mBodyCoSph.mGObjTg.mSPrm =
        u32(at<bpw_class>(a)->mBodyCoSph.mGObjTg.mSPrm) & ~1u; // mBodyCoSph.OffTgShield()
    at<bpw_class>(a)->mBodyAtSph.mObjAt.mSPrm =
        u32(at<bpw_class>(a)->mBodyAtSph.mObjAt.mSPrm) | 1u; // mBodyAtSph.OnAtSPrmBit(Set)
    at<bpw_class>(a)->mBodyAtSph.mObjAt.mRPrm = 1;           // mBodyAtSph.OnAtHitBit()
    at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm =
        u32(at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm) | 1u; // mBodyCoSph.OnTgSetBit()
    at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
        u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) & ~1u; // mBodyCoSph.ClrCoSet()
    at<bpw_class>(a)->m498 = 350.f;
    at<bpw_class>(a)->m49C = 400.f;
    at<bpw_class>(a)->m47A = 0x500; // m47A
}

/* HD: BpwHD_getGameInfo() first (unused); timer 240 + rnd(120) (GameCube USA values) */
/* 020D271C */
void light_on_off(bpw_class *i_this) {
    u32 a = ea(i_this);
    BpwHD_getGameInfo();
    s32 idx = ftoi(BpwHD_cM_rndF(2.99f));
    s16 state = s16(at<bpw_class>(a)->m3F6); // m3F6
    if (state == 0)
        return;
    u8 sw = u8(at<bpw_class>(a)->mLightState); // mLightState
    if (state == 3) {
        onSwitch(a, sw);
        onSwitch(a, sw + 1);
        onSwitch(a, sw + 2);
        return;
    }
    if (state == 4) {
        offSwitch(a, sw);
        offSwitch(a, sw + 1);
        offSwitch(a, sw + 2);
        return;
    }
    if (s16(at<bpw_class>(a)->mSomeCountdownTimers[7]) != 0)
        return; // mSomeCountdownTimers[7]
    idx = ftoi(mul((f32)idx, 3.f));
    s32 no = sw + light_on_dt[idx];
    if (state == 2)
        onSwitch(a, no);
    else
        offSwitch(a, no);
    if (s16(BpwHio(0).mShort[1]) != 0)
        offSwitch(a, no);
    idx++;
    no = u8(at<bpw_class>(a)->mLightState) + light_on_dt[idx];
    if (s16(at<bpw_class>(a)->m3F6) == 2)
        onSwitch(a, no);
    else
        offSwitch(a, no);
    if (s16(BpwHio(0).mShort[1]) != 0)
        offSwitch(a, no);
    idx++;
    no = u8(at<bpw_class>(a)->mLightState) + light_on_dt[idx];
    if (s16(at<bpw_class>(a)->m3F6) == 2)
        offSwitch(a, no);
    else
        onSwitch(a, no);
    if (s16(BpwHio(0).mShort[1]) != 0)
        offSwitch(a, no);
    s32 t = s16(BpwHio(0).mShort[4]) + 240;
    at<bpw_class>(a)->mSomeCountdownTimers[7] = (s16)t;
    f32 r = BpwHD_cM_rndF_2((f32)(s16(BpwHio(0).mShort[5]) + 120));
    at<bpw_class>(a)->mSomeCountdownTimers[7] = (s16)(t + (s16)ftoi(r));
}

/* 020D2A48 */
u32 skull_search_sub(u32 actor, u32 unused) {
    if (get_check_count >= 100)
        return 0;
    if (!BpwHD_fopAc_IsActor(actor))
        return 0; // fopAc_IsActor
    if (!actor || s16(at<fopAc_ac_c>(actor)->mProcName) != 0xCF)
        return 0; // fpcNm_BL_e (skull)
    if (s8(at<fopAc_ac_c>(actor)->health) <= 0)
        return 0;
    s32 n = get_check_count;
    get_check_count = n + 1;
    check_info.mActors[n] = actor;
    return 0;
}

/* 020D2AD4 */
void search_get_skull(bpw_class *i_this, u8 mode) {
    u32 a = ea(i_this);
    get_check_count = 0;
    BpwHD_fpcEx_Search(0x020D2A48u, a); // fpcM_Search(skull_search_sub, i_this)
    s32 n = get_check_count;
    if (n == 0)
        return;
    for (s32 i = 0; i < n;) {
        u32 sk = check_info.mActors[i];
        if (u32(at<fopAc_ac_c>(sk)->actor_status) & 0x2000)
            return; // fopAcM_checkCarryNow
        if (mode == 0) {
            f32 d = BpwHD_fopAcM_searchActorDistance_2(a, sk);
            if (d > 300.f) {
                s16 ang = BpwHD_fopAcM_searchActorAngleY(a, sk);
                s16 diff = BpwHD_cLib_distanceAngleS(
                    (s32)s16(at<bpw_class>(a)->actor.current.angle.y), (s32)ang);
                if (diff < 22000) {
                    at<fopAc_ac_c>(sk)->current.angle.y = BpwHD_fopAcM_searchActorAngleY(sk, a);
                    f32 r = BpwHD_cM_rndFX(5.f);
                    at<fopAc_ac_c>(sk)->speedF = add(r, 10.f);
                    u32 id = sk ? u32(at<fopAc_ac_c>(sk)->mBsPcId) : 0xFFFFFFFFu;
                    at<fopAc_ac_c>(sk)->speed.y = 0.f;
                    at<fopAc_ac_c>(sk)->shape_angle.x =
                        (s16)(s16(at<fopAc_ac_c>(sk)->shape_angle.x) - ((id & 3) * 0x300 + 0x1000));
                }
            } else
                at<fopAc_ac_c>(sk)->speedF = 0.f;
        } else if (mode == 1) {
            s16 ang = BpwHD_fopAcM_searchActorAngleY(a, sk);
            s16 diff = BpwHD_cLib_distanceAngleS((s32)s16(at<bpw_class>(a)->actor.current.angle.y),
                                                 (s32)ang);
            if (diff < 22000) {
                if (s16(at<bpw_class>(a)->mActionState) == 0x35) {
                    f32 r = BpwHD_cM_rndFX(3.f);
                    f32 vy = f32(at<fopAc_ac_c>(sk)->speed.y);
                    at<fopAc_ac_c>(sk)->speedF = add(r, 3.f);
                    if (vy > 20.f)
                        at<fopAc_ac_c>(sk)->speed.y = 20.f;
                } else {
                    f32 r = BpwHD_cM_rndFX(5.f);
                    at<fopAc_ac_c>(sk)->speedF = add(r, 20.f);
                    f32 r2 = BpwHD_cM_rndF(5.f);
                    f32 vy = add(f32(at<fopAc_ac_c>(sk)->speed.y), add(r2, 5.f));
                    at<fopAc_ac_c>(sk)->speed.y = vy;
                    if (vy > 20.f)
                        at<fopAc_ac_c>(sk)->speed.y = 20.f;
                }
                s16 ang2 = BpwHD_fopAcM_searchActorAngleY(a, sk);
                at<fopAc_ac_c>(sk)->current.angle.y = ang2;
                f32 r = BpwHD_cM_rndFX(4096.f);
                s16 sx = s16(at<fopAc_ac_c>(sk)->shape_angle.x);
                at<fopAc_ac_c>(sk)->current.angle.y = (s16)(ang2 + (s16)ftoi(r));
                at<fopAc_ac_c>(sk)->shape_angle.x = (s16)(sx - 0x2000);
            }
        } else if (mode == 2) {
            at<fopAc_ac_c>(sk)->speedF = 0.f;
        } else {
            i++;
            continue;
        } // n not re-read
        n = get_check_count;
        i++;
    }
}

/* HD: |speedF| read before the distance check */
/* 020D2DAC */
void maai_sub(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 player = player_actor();
    f32 spd = __builtin_fabsf(f32(at<fopAc_ac_c>(player)->speedF));
    f32 d = BpwHD_fopAcM_searchActorDistance_2(a, player_actor());
    if (d > 500.f)
        return;
    u32 mtx = u32(l_calcMtx);
    s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
    BpwHD_mDoMtx_YrotS(mtx, (s32)ang);
    Local<BpwVecHD> v, out;
    v->y = 0.f;
    v->z = -500.f;
    v->x = 0.f;
    BpwHD_MtxPosition_2(v.get(), out.get());
    BpwHD_PSVECAdd(out.get(), player + 0x314, out.get());
    spd = mul(spd, 1.5f);
    BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->actor.current.pos), (f32)out->x, 1.f, spd);
    BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->actor.current.pos.z), (f32)out->z, 1.f, spd);
}

/* 020D2ED8 */
s32 kantera_pos_search(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 id = u32(at<bpw_class>(a)->m3FC); // m3FC
    if (id == 0xFFFFFFFF)
        return 0;
    Local<be<u32>> key;
    key->set(id);
    u32 k = BpwHD_fopAcIt_Judge(0x025E1234u, key.get());
    if (!k)
        return 0;
    f32 dx = sub(f32(at<fopAc_ac_c>(k)->current.pos.x), f32(at<bpw_class>(a)->mBodyPos.x));
    f32 dz = sub(f32(at<fopAc_ac_c>(k)->current.pos.z), f32(at<bpw_class>(a)->mBodyPos.z));
    u32 mtx = u32(l_calcMtx);
    s16 ang = BpwHD_cM_atan2s(dx, dz);
    BpwHD_mDoMtx_YrotS(mtx, (s32)ang);
    Local<BpwVecHD> v, out;
    v->x = -120.f;
    v->y = -20.f;
    v->z = -320.f;
    BpwHD_MtxPosition_2(v.get(), out.get());
    BpwHD_PSVECAdd(out.get(), ea(&at<bpw_class>(k)->actor.current.pos), out.get());
    BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->actor.current.pos), (f32)out->x, 1.f, 120.f);
    BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->actor.current.pos.y), (f32)out->y, 1.f, 120.f);
    BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->actor.current.pos.z), (f32)out->z, 1.f, 120.f);
    s16 ang2 = BpwHD_cM_atan2s(dx, dz);
    f32 ey = sub(f32(at<bpw_class>(a)->actor.current.pos.y), (f32)out->y);
    f32 ex = sub(f32(at<bpw_class>(a)->actor.current.pos.x), (f32)out->x);
    f32 ez = sub(f32(at<bpw_class>(a)->actor.current.pos.z), (f32)out->z);
    f32 s = fmadds(ez, ez, fmadds(ex, ex, mul(ey, ey)));
    at<bpw_class>(a)->m476 = ang2; // m476
    f32 len = BpwHD_sqrtf(s);
    if (len < 2.f)
        return 1;
    return 0;
}

/* inlines wall_HIT_check. HD: m5E0[0].remove() -> end(); smoke via dPa set (group 2) */
/* 020D72BC */
void action_damage(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 player = player_actor();
    u32 morf = u32(at<bpw_class>(a)->mpMorf);
    u32 model = u32(at<mDoExt_McaMorf>(morf)->mpModel.v);
    constexpr u32 str = 0x1000B078;
    auto speed_tail = [&] {
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->actor.speedF), f32(at<bpw_class>(a)->m490), 1.f,
                              1.f);
    };
    auto initBrk = [&](be<u32> bpw_class::*field, u32 res, u32 mode) {
        u32 r = getObjectRes(str, res);
        u32 brk = u32(at<bpw_class>(a)->*field), md = u32(at<BpwModelHD>(model)->mpModelData);
        BpwHD_mDoExt_brkAnm_initWithFlags_4(brk, md, r, 1, mode, 1.f, 0, -1, 1, 0);
    };
    auto cancel_carry = [&] {
        BpwHD_fopAcM_cancelCarryNow(a); // fopAcM_cancelCarryNow
        at<bpw_class>(a)->actor.attention_info.flags =
            u32(at<bpw_class>(a)->actor.attention_info.flags) & ~0x10u;
        at<bpw_class>(a)->actor.current.angle.y = s16(at<bpw_class>(a)->actor.shape_angle.y);
        next_status_clear(i_this, 0);
        next_att_wait_check(i_this);
    };
    switch (s16(at<bpw_class>(a)->mActionState)) {
    case 0x50: {
        search_get_skull(i_this, 2);
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[5]) == 0) {
            switch (u8(at<bpw_class>(a)->mKankyouHendouState)) {
            case 1:
                at<bpw_class>(a)->mSomeCountdownTimers[5] =
                    (s16)ftoi(add((f32)s16(BpwHio(21).mShort[1]), 90.f));
                break;
            case 2:
                at<bpw_class>(a)->mSomeCountdownTimers[5] = (s16)(s16(BpwHio(21).mShort[0]) + 10);
                break;
            case 3:
                at<bpw_class>(a)->mSomeCountdownTimers[5] = (s16)(s16(BpwHio(21).mShort[2]) + 10);
                break;
            }
        }
        next_status_clear(i_this, 1);
        at<bpw_class>(a)->m4A4 = 300.f;
        fire_and_emitter_clear(i_this);
        at<bpw_class>(a)->m478 = 0;
        at<bpw_class>(a)->m47E = 200;
        at<bpw_class>(a)->m490 = 0.f;
        at<bpw_class>(a)->actor.speedF = 0.f;
        at<bpw_class>(a)->m480 = 200;
        anm_init(i_this, dRes_INDEX_BPW_BCK_HIT1_e, 15.f, 2, 1.f, -1); // HIT1
        at<bpw_class>(a)->mSomeCountdownTimers[9] = 0;
        u32 play = BpwHD_getGameInfo();
        s16 ang = BpwHD_fopAcM_searchActorAngleY_2(
            a, u32(at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.v));
        s16 st = s16(at<bpw_class>(a)->mActionState);
        at<bpw_class>(a)->actor.current.angle.y = ang;
        at<bpw_class>(a)->actor.shape_angle.y = ang;
        at<bpw_class>(a)->m476 = ang;
        at<bpw_class>(a)->mActionState = (s16)(st + 1);
    }
    /* fallthrough */
    case 0x51: {
        u8 hit = u8(at<bpw_class>(a)->mHitType);
        bool solid = false;
        if (hit == 0xB) {
            se_start(a, 0x5961, 0); // JA_SE_CM_BPW_SOLID_END
            at<bpw_class>(a)->m47E = 0xFF;
            at<bpw_class>(a)->m478 = 0;
            solid = true;
        } else if (hit == 0xA) {
            at<bpw_class>(a)->m47E = (s16)(s16(at<bpw_class>(a)->m47E) + 1);
            se_start(a, 0x5160, 0); // JA_SE_CM_BPW_BECOME_SOLID
            if (s16(at<bpw_class>(a)->mSomeCountdownTimers[9]) == 0) {
                mons_se(a, 0x4963, 0, true); // JA_SE_CV_BPW_LIGHT_DAMAGE
                at<bpw_class>(a)->mSomeCountdownTimers[9] = 30;
            }
            if (u8(at<bpw_class>(a)->m3E1) == 0) {
                at<bpw_class>(a)->m3E1 = 1;
                at<bpw_class>(a)->m3E2 = 0;
                initBrk(&bpw_class::mpLightStunBrkAnm, dRes_INDEX_BPW_BRK_HIT_e, 2); // HIT
            }
            if (s16(at<bpw_class>(a)->m47E) < 0xFF)
                break;
            at<bpw_class>(a)->m47E = 0xFF;
            at<bpw_class>(a)->m478 = 0;
            solid = true;
        }
        if (solid) {
            se_start(a, 0x5961, 0);
            s16 t = s16(at<bpw_class>(a)->mAttWaitTimer);
            at<bpw_class>(a)->mSomeCountdownTimers[0] = 0x12;
            at<bpw_class>(a)->m3E0 = 1;
            if ((u32)(s32)t <= 2)
                at<bpw_class>(a)->mAttWaitTimer = 3;
            initBrk(&bpw_class::mpLightFreezeBrkAnm, dRes_INDEX_BPW_BRK_HIRARU1_e, 0); // HIRARU1
            anm_init(i_this, dRes_INDEX_BPW_BCK_DOSUN1_e, 0.f, 0, 0.f, -1);            // DOSUN1
            at<bpw_class>(a)->mBodyAtSph.mObjAt.mSPrm =
                u32(at<bpw_class>(a)->mBodyAtSph.mObjAt.mSPrm) & ~1u; // mBodyAtSph.ClrAtSet()
            at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
            at<bpw_class>(a)->m4A0 = 50.f; // m4A0
            speed_tail();
            return;
        }
        at<bpw_class>(a)->mSomeCountdownTimers[2] =
            reg_ftoi(0x6E4, 90.f); // mSomeCountdownTimers[2] = REG12_F(7) + 90
        BOOL r = next_att_wait_check(i_this);
        u32 anm = u32(at<bpw_class>(a)->m3F8);
        if (!r) {
            at<bpw_class>(a)->mBodyAction = ACTION_ATTACK_KOUGEKI;
            at<bpw_class>(a)->mActionState = 0x14;
        }
        if (anm != 0x2A)
            anm_init(i_this, dRes_INDEX_BPW_BCK_WAIT1_e, 15.f, 2, 1.f, -1); // WAIT1
        if (u8(at<bpw_class>(a)->m3E1) != 0) {
            at<bpw_class>(a)->m3E1 = 0;
            initBrk(&bpw_class::mpDefaultBrkAnm, dRes_INDEX_BPW_BRK_DEFAULT_e, 0); // DEFAULT
        }
        break;
    }
    case 0x52: {
        if (brk_isStop(u32(at<bpw_class>(a)->mpLightFreezeBrkAnm)))
            mons_se(a, 0x4964, 0, true); // JA_SE_CV_BPW_BECOME_SOLID
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) != 0)
            break;
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m48C), -120.f, 1.f, 10.f);
        at<bpw_class>(a)->actor.gravity = -2.f;
        if (!(u32(at<bpw_class>(a)->mAcch.m_flags) & 0x20))
            break; // mAcch.ChkGroundHit()
        at<bpw_class>(a)->actor.gravity = 0.f;
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        at<bpw_class>(a)->mBodyCoSph.mGObjTg.mSPrm =
            u32(at<bpw_class>(a)->mBodyCoSph.mGObjTg.mSPrm) | 1u; // mBodyCoSph.OnTgShield()
        at<bpw_class>(a)->m3E8 = 1;
        at<bpw_class>(a)->m48C = -120.f;
        at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
            u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) | 1u; // mBodyCoSph.OnCoSetBit()
        at<bpw_class>(a)->mSomeCountdownTimers[6] =
            reg_ftoi(0xBDC, 20.f); // timers[6] = REG21_F(1) + 20
        at<bpw_class>(a)->mActionState = 0x53;
        if (ea(&at<bpw_class>(a)->actor.eyePos)) {
            se_start_e(a, 0x5962, 0);     // JA_SE_CM_BPW_SOLID_FALL
            mons_se(a, 0x4968, 0, false); // JA_SE_CV_BPW_LANDING
        }
        at<bpw_class>(a)->mSomeCountdownTimers[1] = 0;
        anm_init(i_this, dRes_INDEX_BPW_BCK_DOSUN1_e, 0.f, 0, 1.f, -1);
        at<bpw_class>(a)->actor.gravity = 0.f;
        speed_tail();
        return;
    }
    case 0x53: {
        if (u32(at<bpw_class>(a)->m3F8) != 0xC && morf_isStop(morf)) {
            anm_init(i_this, dRes_INDEX_BPW_BCK_DOSUN_WAIT1_e, 15.f, 2, 1.f, -1); // DOSUN_WAIT1
            at<bpw_class>(a)->mSomeCountdownTimers[1] = 450;
            at<bpw_class>(a)->actor.attention_info.flags =
                u32(at<bpw_class>(a)->actor.attention_info.flags) | 0x10;
            at<bpw_class>(a)->m684 = s16(at<bpw_class>(a)->actor.shape_angle.y);
        }
        if (u32(at<bpw_class>(a)->actor.actor_status) & 0x2000) {
            anm_init(i_this, dRes_INDEX_BPW_BCK_JITA1_e, 15.f, 2, 1.f, -1); // JITA1
            at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
                u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) & ~1u;
            s16 t1 = s16(at<bpw_class>(a)->mSomeCountdownTimers[1]);
            at<bpw_class>(a)->m3AC.y = 300.f;
            at<bpw_class>(a)->actor.current.angle.y = s16(at<fopAc_ac_c>(player)->shape_angle.y);
            at<bpw_class>(a)->m47A = 0;
            at<bpw_class>(a)->mSomeCountdownTimers[9] = 0;
            at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
            if (t1 != 1)
                break;
        } else if (s16(at<bpw_class>(a)->mSomeCountdownTimers[1]) != 1)
            break;
        cancel_carry();
        speed_tail();
        return;
    }
    case ACTION_STATE_DAMAGE_CARRIED: {
        s16 py = s16(at<fopAc_ac_c>(player)->shape_angle.y);
        if (s16(BpwHio(8).mShort[3]) != 0) { // REG8_S(3)
            s16 v = (s16)(s16(at<bpw_class>(a)->m684) +
                          (py - s16(at<bpw_class>(a)->actor.current.angle.y)));
            at<bpw_class>(a)->actor.shape_angle.y = v;
            at<bpw_class>(a)->m684 = v;
            at<bpw_class>(a)->actor.current.angle.y = s16(at<fopAc_ac_c>(player)->shape_angle.y);
            if (s16(at<bpw_class>(a)->mSomeCountdownTimers[9]) == 0) {
                mons_se(a, 0x4965, 0, false);
                at<bpw_class>(a)->mSomeCountdownTimers[9] = 0x20;
            }
        } else {
            at<bpw_class>(a)->actor.current.angle.y = py;
            BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(a)->actor.shape_angle.y),
                                        (s32)s16(at<fopAc_ac_c>(player)->shape_angle.y), 1, 0x700);
            if (s16(at<bpw_class>(a)->mSomeCountdownTimers[9]) == 0) {
                mons_se(a, 0x4965, 0, false);
                at<bpw_class>(a)->mSomeCountdownTimers[9] = 0x20;
            }
        }
        if (u32(at<bpw_class>(a)->actor.actor_status) & 0x2000) {
            if (s16(at<bpw_class>(a)->mSomeCountdownTimers[1]) != 1)
                break;
            cancel_carry();
            speed_tail();
            return;
        }
        at<bpw_class>(a)->actor.gravity = -3.f;
        at<bpw_class>(a)->actor.speed.y = 10.f;
        at<bpw_class>(a)->m3AC.y = 340.f;
        at<bpw_class>(a)->m490 = 120.f;
        at<bpw_class>(a)->m47A = 0;
        u32 flags = u32(at<bpw_class>(a)->actor.attention_info.flags);
        if (s16(BpwHio(8).mShort[3]) != 0) {
            at<bpw_class>(a)->m3D6.y = s16(at<fopAc_ac_c>(player)->shape_angle.y);
            at<bpw_class>(a)->actor.shape_angle.x = -0x4000;
        } else {
            at<bpw_class>(a)->actor.shape_angle.x = 0;
        }
        at<bpw_class>(a)->actor.shape_angle.z = 0;
        at<bpw_class>(a)->actor.attention_info.flags = flags & ~0x10u;
        at<bpw_class>(a)->m49C = 300.f;
        at<bpw_class>(a)->m3E5 = 1;
        at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
            u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) | 1u;
        mons_se(a, 0x4966, 0, false);                                  // JA_SE_CV_BPW_THROWN
        anm_init(i_this, dRes_INDEX_BPW_BCK_TAMA1_e, 0.f, 2, 1.f, -1); // TAMA1
        at<bpw_class>(a)->mActionState = 0x55;
        speed_tail();
        return;
    }
    case ACTION_STATE_DAMAGE_THROWN: {
        se_start(a, 0x5163, 0); // JA_SE_CM_BPW_ROLLING
        if (s16(BpwHio(8).mShort[3]) != 0)
            at<bpw_class>(a)->m3D6.x = (s16)(s16(at<bpw_class>(a)->m3D6.x) + 5000);
        else
            at<bpw_class>(a)->actor.shape_angle.x =
                (s16)(s16(at<bpw_class>(a)->actor.shape_angle.x) + 0x1000);
        /* wall_HIT_check */
        u32 pl = player_actor();
        f32 sc = add(f32(BpwHio(21).mFloat[4]), 6.f); // REG21_F(4) + 6
        Local<BpwVecHD> scale;
        scale->y = sc;
        scale->z = sc;
        scale->x = sc;
        if (!(u32(at<bpw_class>(a)->mAcchCir.m_flags) & 2))
            break; // mAcchCir.ChkWallHit()
        Local<BpwVecHD> p0, p1, p2;
        BpwHD_cBgS_GetTriPnt(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mBgS),
                             ea(&at<bpw_class>(a)->mAcchCir), p0.get(), p1.get(), p2.get());
        s32 attr =
            BpwHD_dBgS_GetAttributeCode(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mBgS),
                                        ea(&at<bpw_class>(a)->mAcchCir));
        u32 play = BpwHD_getGameInfo();
        BpwHD_dPa_control_c_set_2(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0, 0xF, p0.get(),
                                  ea(&at<fopAc_ac_c>(pl)->shape_angle), scale.get(), 0xFF, 0, -1, 0,
                                  0, 0);
        s32 res;
        if (attr == 9) {
            at<bpw_class>(a)->m3E5 = 0;
            at<bpw_class>(a)->m408 = 0xB5;
            res = 2;
        } // dBgS_Attr_DAMAGE_e
        else {
            res = 1;
            at<bpw_class>(a)->m3E5 = 0;
        }
        at<bpw_class>(a)->m3D6.x = 0;
        at<bpw_class>(a)->m3D6.y = 0;
        at<bpw_class>(a)->m3D6.z = 0;
        u32 cam = u32(at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCameraInfo[0].mpCamera);
        BpwHD_dCamera_c_ForceLockOff(ea(&at<BpwCameraHD>(cam)->mCamera),
                                     actor_id(a)); // ForceLockOff
        f32 dx = sub(f32(at<bpw_class>(a)->mBodyPos.x), f32(at<bpw_class>(a)->actor.current.pos.x));
        f32 dz = sub(f32(at<bpw_class>(a)->mBodyPos.z), f32(at<bpw_class>(a)->actor.current.pos.z));
        at<bpw_class>(a)->actor.current.angle.y = BpwHD_cM_atan2s(dx, dz);
        if (res == 2) {
            if (ea(&at<bpw_class>(a)->actor.eyePos)) {
                se_start_e(a, 0x5964, 0);
                mons_se(a, 0x4967, 0, false);
            } // DAMAGE_TOGE, CV_DAMAGE
            at<bpw_class>(a)->m3E8 = 2;
            at<bpw_class>(a)->mSomeCountdownTimers[6] = reg_ftoi(0xBD8, 20.f);
            at<bpw_class>(a)->mActionState = 100;
        } else {
            if (ea(&at<bpw_class>(a)->actor.eyePos)) {
                se_start_e(a, 0x5968, 0);
                mons_se(a, 0x4968, 0, false);
            } // BOUND_NO_DMG, LANDING
            at<bpw_class>(a)->actor.shape_angle.x = 0;
            at<bpw_class>(a)->m3E8 = 1;
            at<bpw_class>(a)->mSomeCountdownTimers[6] = reg_ftoi(0xBDC, 20.f);
            at<bpw_class>(a)->mActionState = 0x5A;
        }
        speed_tail();
        return;
    }
    case 0x5A:
        at<bpw_class>(a)->actor.speed.y = add(f32(BpwHio(12).mFloat[4]), 30.f);
        at<bpw_class>(a)->actor.gravity = -add(f32(BpwHio(12).mFloat[5]), 3.f);
        at<bpw_class>(a)->actor.speedF = add(f32(BpwHio(12).mFloat[6]), 20.f);
        at<bpw_class>(a)->m490 = add(f32(BpwHio(12).mFloat[6]), 20.f);
        at<bpw_class>(a)->m47A = 0;
        anm_init(i_this, dRes_INDEX_BPW_BCK_DOSUN1_e, 0.f, 0, 1.f, -1);
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        speed_tail();
        return;
    case 0x5B:
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->actor.shape_angle), 0, 10, 0x2000);
        if (!(u32(at<bpw_class>(a)->mAcch.m_flags) & 0x20))
            break;
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0x5C:
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->actor.shape_angle), 0, 10, 0x2000);
        if (!morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = 0;
        at<bpw_class>(a)->mSomeCountdownTimers[1] = 0;
        next_status_clear(i_this, 0);
        next_att_wait_check(i_this);
        speed_tail();
        return;
    case 100:
        at<bpw_class>(a)->m47A = 0;
        at<bpw_class>(a)->actor.speedF = 5.f;
        at<bpw_class>(a)->actor.gravity = -3.f;
        at<bpw_class>(a)->m490 = 5.f;
        at<bpw_class>(a)->actor.speed.y = 70.f;
        anm_init(i_this, dRes_INDEX_BPW_BCK_DAMAGE1_e, 15.f, 2, 1.f, -1); // DAMAGE1
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        speed_tail();
        return;
    case ACTION_STATE_DAMAGE_HIT: {
        at<bpw_class>(a)->actor.shape_angle.y =
            (s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + 0x1500);
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->actor.shape_angle), 0, 10, 0x2000);
        if (!(u32(at<bpw_class>(a)->mAcch.m_flags) & 0x20))
            break;
        at<bpw_class>(a)->m3E8 = 2;
        s16 t6 = reg_ftoi(0xBD8, 20.f);
        f32 ground = f32(at<bpw_class>(a)->mAcch.m_ground_h);
        at<bpw_class>(a)->mSomeCountdownTimers[6] = t6;
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x, &at<bpw_class>(a)->actor.current.pos.x);
        at<bpw_class>(a)->mFire1DousaPos.y = ground;
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z, &at<bpw_class>(a)->actor.current.pos.z);
        BpwHD_dPa_smokeEcallBack_end(ea(&at<bpw_class>(a)->m5E0)); // m5E0[0].remove()
        s32 room = s8(at<bpw_class>(a)->actor.current.roomNo);
        u32 play = BpwHD_getGameInfo();
        u32 em = BpwHD_dPa_control_c_set_3(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 2,
                                           0xA404, ea(&at<bpw_class>(a)->mFire1DousaPos),
                                           ea(&at<bpw_class>(a)->actor.shape_angle), 0, 0xB9,
                                           ea(&at<bpw_class>(a)->m5E0), room, 0, 0, 0);
        if (em) {
            Local<be<u32>> c1, c2;
            BpwHD_dKy_get_seacolor(c1.get(), c2.get()); // dKy_get_seacolor
            u32 c1a = ea(c1.get()), c2a = ea(c2.get());
            u8 b = u8(at<GXColor>(c1a)->b), r = u8(at<GXColor>(c1a)->r),
               g = u8(at<GXColor>(c1a)->g);
            at<BpwEmitterHD>(em)->mGlobalPrmColor.r = r;
            at<BpwEmitterHD>(em)->mGlobalPrmColor.g = g;
            at<BpwEmitterHD>(em)->mGlobalPrmColor.b = b;
            u8 b2 = u8(at<GXColor>(c2a)->b), r2 = u8(at<GXColor>(c2a)->r),
               g2 = u8(at<GXColor>(c2a)->g);
            at<BpwEmitterHD>(em)->mGlobalEnvColor.r = r2;
            at<BpwEmitterHD>(em)->mGlobalEnvColor.g = g2;
            at<BpwEmitterHD>(em)->mGlobalEnvColor.b = b2;
        }
        se_start_e(a, 0x5965, 0); // JA_SE_CM_BPW_DAMAGE_EXPLODE
        play = BpwHD_getGameInfo();
        BpwHD_dPa_control_c_set(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0, 0x8444,
                                ea(&at<bpw_class>(a)->actor.current.pos), 0, 0, 0xFF, 0, -1, 0, 0,
                                0);
        struct SVec {
            be<u16> x, y, z;
        };
        Local<SVec> ang;
        Local<BpwVecHD> pos;
        ang->x = u16(at<bpw_class>(a)->actor.shape_angle.x);
        ang->y = u16(at<bpw_class>(a)->actor.shape_angle.y);
        ang->z = u16(at<bpw_class>(a)->actor.shape_angle.z);
        pos->x = f32(at<bpw_class>(a)->actor.current.pos.x);
        pos->y = f32(at<bpw_class>(a)->actor.current.pos.y);
        pos->z = f32(at<bpw_class>(a)->actor.current.pos.z);
        s32 hp = s8(at<bpw_class>(a)->actor.health);
        at<bpw_class>(a)->m3E9 = 0;
        f32 q = 65536.f / (f32)hp;
        pos->y = add(f32(at<bpw_class>(a)->mAcch.m_ground_h), 20.f);
        for (u32 i = 0; i < 15; i++)
            at<bpw_class>(a)->mChildPoeIds[i] = 0xFFFFFFFF;
        s16 spread = (s16)ftoi(q);
        s16 cur = (s16)ftoi(BpwHD_cM_rndFX(32768.f));
        s16 kind = (s16)ftoi(BpwHD_cM_rndF(4.99f));
        u32 k = (u32)(s32)kind;
        u32 prm = (k << 9) | 0xFF000003u;
        s32 n = s8(at<bpw_class>(a)->actor.health);
        if (0 < n) {
            f32 jitter = mul((f32)spread, 0.25f);
            for (s32 i = 0; i < n;) {
                s32 r = s8(at<bpw_class>(a)->actor.current.roomNo);
                u32 id = BpwHD_fopAcM_createChild_2(0xD4, actor_id(a), prm, pos.get(), r, ang.get(),
                                                    0, -1, 0);
                cur = (s16)(cur + spread);
                at<bpw_class>(a)->mChildPoeIds[i] = id;
                ang->y = (u16)cur;
                f32 j = BpwHD_cM_rndFX_2(jitter);
                ang->y = (u16)(s16)(cur + (s16)ftoi(j));
                k++;
                if (k > 5)
                    k = 0;
                n = s8(at<bpw_class>(a)->actor.health);
                i++;
                prm = (k << 9) | 0xFF000003u;
            }
        }
        at<bpw_class>(a)->mBodyAction = ACTION_SEPARATION_BUNRI_DOUSA;
        at<bpw_class>(a)->mActionState = 0x6E;
        break;
    }
    }
    speed_tail();
}

/* . HD: hp phase thresholds from a table (m3F6 0..2); the camera targets move toward
 * REG + constant (the GameCube decompilation passes the distance); SearchByID skips invalid ids;
 * player demo calls are inline stores; m5E0/m554 remove() -> end(). */
/* 020D8564 */
void action_bunri_dousa(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 pl = player_actor();
    s32 camId = s8(at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mPlayerInfo[0].mCameraID);
    u32 cam = u32(at<dComIfG_camera_info_class>(
                      ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCameraInfo[0]) +
                      camId * sizeof(dComIfG_camera_info_class))
                      ->mpCamera);
    auto timer = [&](u32 i) -> s16 { return s16(at<bpw_class>(a)->mSomeCountdownTimers[i]); };
    auto set_timer = [&](u32 i, s16 v) { at<bpw_class>(a)->mSomeCountdownTimers[i] = v; };
    auto poe = [&](u32 i) -> u32 { return search_by_id(u32(at<bpw_class>(a)->mChildPoeIds[i])); };
    switch (s16(at<bpw_class>(a)->mActionState)) {
    case ACTION_STATE_SEPARATE_BUNRI_DOUSA_INIT:
        for (u32 i = 0; i < 10; i++)
            at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
        set_timer(5, 0);
        set_timer(0, 0);
        at<bpw_class>(a)->mKankyouHendouState = 3;
        set_timer(1, 0);
        set_timer(2, 0);
        at<bpw_class>(a)->m47C = 0;                                    // m47C
        anm_init(i_this, dRes_INDEX_BPW_BCK_CORE1_e, 0.f, 2, 1.f, -1); // CORE1
        at<bpw_class>(a)->mBodyAtSph.mObjAt.mSPrm =
            u32(at<bpw_class>(a)->mBodyAtSph.mObjAt.mSPrm) & ~1u;
        at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm =
            u32(at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm) & ~1u;
        at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
            u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) & ~1u;
        BpwHD_dCcD_GObjInf_ClrTgHit(ea(&at<bpw_class>(a)->mBodyCoSph)); // mBodyCoSph.ClrTgHit()
        at<bpw_class>(a)->actor.gravity = 0.f;
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        at<bpw_class>(a)->actor.speedF = 0.f;
        at<bpw_class>(a)->actor.scale.x = 0.f;
        at<bpw_class>(a)->actor.scale.y = 0.f;
        at<bpw_class>(a)->actor.scale.z = 0.f;
        {
            s16 st = s16(at<bpw_class>(a)->mActionState);
            Bpw_copyWord(&at<bpw_class>(a)->actor.current.pos.x, &at<bpw_class>(a)->mBodyPos.x);
            Bpw_copyWord(&at<bpw_class>(a)->actor.current.pos.y, &at<bpw_class>(a)->mBodyPos.y);
            at<bpw_class>(a)->m47A = 0x500;
            at<bpw_class>(a)->actor.attention_info.flags = 0;
            Bpw_copyWord(&at<bpw_class>(a)->actor.current.pos.z, &at<bpw_class>(a)->mBodyPos.z);
            set_timer(0, 0x1A4);
            at<bpw_class>(a)->mActionState = (s16)(st + 1);
            at<bpw_class>(a)->m3E6 = 0;
            at<bpw_class>(a)->m3E5 = 0;
        }
        /* fallthrough */
    case ACTION_STATE_SEPARATE_BUNRI_DOUSA_EXECUTE: {
        s16 m = s16(at<bpw_class>(a)->m3F6);
        s32 thr = 0;
        if ((u32)(s32)m <= 2)
            thr = l_healthThreshold[m]; // {10, 5, 0}
        s32 hp = s8(at<bpw_class>(a)->actor.health);
        if (hp <= 0) {
            if (u8(*at<be<u8>>(ea(
                    &at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCurStage.mName[0]))) == 0x58) {
                BpwHD_dSv_event_c_onEventBit(u32(l_saveInfo) + 0x644,
                                             0x3210); // onEventBit(JALHALLA_TRIALS_CLEAR)
                BpwHD_dSv_event_c_onEventBit(u32(l_saveInfo) + 0x1178, 0x480); // onTmpBit(UNK_0480)
                BpwHD_dLib_setNextStageBySclsNum(
                    0,
                    (s32)s8(at<bpw_class>(a)->actor.current.roomNo)); // dLib_setNextStageBySclsNum
                s32 reverb =
                    BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
                BpwHD_mDoAud_seStart_3(0x2888u, 0u, 0u, reverb); // JA_SE_LK_B_BOSS_WARP
            } else {
                at<bpw_class>(a)->actor.actor_status =
                    u32(at<bpw_class>(a)->actor.actor_status) | 0x4000;
                at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
                    u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) & ~1u;
                at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm =
                    u32(at<bpw_class>(a)->mBodyCoSph.mObjTg.mSPrm) & ~1u;
                BpwHD_dCcD_GObjInf_ClrTgHit(ea(&at<bpw_class>(a)->mBodyCoSph));
                at<bpw_class>(a)->mActionState = 0x78;
            }
            break;
        }
        if (timer(0) != 0 && hp > thr)
            break;
        at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
            u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) | 1u; // OnCoSetBit
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        at<bpw_class>(a)->actor.actor_status = u32(at<bpw_class>(a)->actor.actor_status) | 0x4000;
    }
        /* fallthrough */
    case 0x70:
        for (s32 i = 0; i < s8(at<bpw_class>(a)->actor.health); i++) {
            u32 p = poe(i);
            if (p && s8(at<fopAc_ac_c>(p)->health) > 0)
                at<fopAc_ac_c>(p)->health = 0x14;
        }
        at<bpw_class>(a)->m47E = 0;
        inc_state(a);
        /* fallthrough */
    case 0x71: {
        if (!demo_accept(a))
            break;
        at<daPy_py_c>(pl)->mDemo.mParam0 = 0;
        at<daPy_py_c>(pl)->mDemo.mDemoMode = 1;
        at<daPy_py_c>(pl)->mDemo.mDemoType = 3; // changeOriginalDemo, DEMO_N_WAIT
        anm_init(i_this, dRes_INDEX_BPW_BCK_CORE1_e, 0.f, 2, 1.f, -1);
        BpwHD_dCamera_c_Stop(ea(&at<BpwCameraHD>(cam)->mCamera));
        BpwHD_dCamera_c_SetTrimSize(ea(&at<BpwCameraHD>(cam)->mCamera), 2);
        at<bpw_class>(a)->m440 = 50.f;
        Local<BpwVecHD> p;
        p->x = 0.f;
        at<bpw_class>(a)->m480 = 0x96;
        at<bpw_class>(a)->mSomeCountdownTimers[2] = 0xF;
        p->z = 1625.f;
        at<bpw_class>(a)->m474 = 1;
        at<bpw_class>(a)->m3E0 = 0;
        p->y = 0.f;
        u32 vt = u32(at<fopAc_ac_c>(pl)->__vtbl);
        s16 ang = BpwHD_fopAcM_searchActorAngleY(pl, a);
        call_ptr<void>(u32(at<BpwPlayerVtableHD>(vt)->setPlayerPosAndAngle), pl, p.get(), (s32)ang);
        inc_state(a);
    }
        /* fallthrough */
    case ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_INIT: {
        set_cam_pos(a, 156.f, 515.f, 1001.f, 181.f, 603.f, 1196.f);
        if (!reg20_step())
            break;
        if (timer(2) != 0)
            break;
        s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        at<bpw_class>(a)->m476 = ang;
        at<bpw_class>(a)->actor.current.angle.y = ang;
        at<bpw_class>(a)->actor.shape_angle.y = ang;
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(a)->m47E), (s32)s16(at<bpw_class>(a)->m480),
                                    1, 0xF);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->actor.scale), 1.f, 1.f, 0.1f);
        f32 s = f32(at<bpw_class>(a)->actor.scale.x);
        at<bpw_class>(a)->actor.scale.z = s;
        at<bpw_class>(a)->actor.scale.y = s;
        if (u8(at<bpw_class>(a)->m3E9) == 0 && f32(at<bpw_class>(a)->actor.scale.x) > 0.9f) {
            at<bpw_class>(a)->actor.scale.x = 1.f;
            at<bpw_class>(a)->actor.scale.y = 1.f;
            at<bpw_class>(a)->m3E9 = 1;
            at<bpw_class>(a)->actor.scale.z = 1.f;
            set_timer(5, (s16)(s16(BpwHio(21).mShort[2]) + 10));
            set_timer(1, 300);
            for (s32 i = 0; i < s8(at<bpw_class>(a)->actor.health); i++) {
                u32 p = poe(i);
                if (p)
                    at<fopAc_ac_c>(p)->actor_status = u32(at<fopAc_ac_c>(p)->actor_status) | 0x4000;
            }
        }
        if (u32(at<bpw_class>(a)->m3F8) == 9) {
            if (morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
                anm_init(i_this, dRes_INDEX_BPW_BCK_CORE1_e, 15.f, 2, 1.f, -1);
            se_start_e(a, 0x623D, 0); // JA_SE_OBJ_BPW_MASK_LAUGH
        }
        {
            Local<BpwVecHD> p;
            p->x = 0.f;
            p->y = 0.f;
            p->z = 1625.f;
            u32 vt = u32(at<fopAc_ac_c>(pl)->__vtbl);
            s16 a2 = BpwHD_fopAcM_searchActorAngleY(pl, a);
            call_ptr<void>(u32(at<BpwPlayerVtableHD>(vt)->setPlayerPosAndAngle), pl, p.get(),
                           (s32)a2);
        }
        if (s16(at<bpw_class>(a)->m464) != 0) {
            anm_init(i_this, dRes_INDEX_BPW_BCK_CORE_NIGE1_e, 0.f, 0, 1.f, -1);
            at<bpw_class>(a)->m464 = 0;
        }
        if (timer(1) == 1 || s16(at<bpw_class>(a)->m462) >= s8(at<bpw_class>(a)->actor.health)) {
            anm_init(i_this, dRes_INDEX_BPW_BCK_CORE_NIGE1_e, 0.f, 2, 1.f, -1); // CORE_NIGE1
            set_timer(0, 30);
            inc_state(a);
        }
        at<bpw_class>(a)->m3AC.y = 140.f;
        fuwafuwa_calc(i_this);
        break;
    }
    case ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_GATHER:
        if (timer(0) != 0)
            break;
        if (!reg20_step())
            break;
        anm_init(i_this, dRes_INDEX_BPW_BCK_HUKKATU1_e, 15.f, 0, 1.f, -1); // HUKKATU1
        se_start(a, 0x5966, 0); // JA_SE_CM_BPW_MASK_TO_BPW
        inc_state(a);
        at<bpw_class>(a)->m43C = 0.f;
        break;
    case ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_GROW:
        demo_calc(a, &at<bpw_class>(a)->m40C.x, regf(20, 12, 152.f), 1.f, 20, 18, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, regf(20, 13, 498.f), 1.f, 20, 18, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, regf(20, 14, 983.f), 1.f, 20, 18, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.x, regf(20, 15, 181.f), 1.f, 20, 18, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.y, regf(20, 16, 504.f), 1.f, 20, 18, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.z, regf(20, 17, 1196.f), 1.f, 20, 18, 0.08f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->m48C), 1.f, 10.f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m3AC.y), 340.f, 1.f, 10.f);
        if (!morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
            break;
        if (!reg20_step())
            break;
        anm_init(i_this, dRes_INDEX_BPW_BCK_WAIT1_e, 15.f, 2, 1.f, -1); // WAIT1
        inc_state(a);
        set_timer(0, 0x14);
        break;
    case ACTION_STATE_SEPARATE_BUNRI_DOUSA_END_FINISH: {
        if (!reg20_step())
            break;
        if (timer(0) != 0)
            break;
        cam_reset_start(a, cam, pl);
        next_status_clear(i_this, 1);
        next_att_wait_check(i_this);
        s32 hp = s8(at<bpw_class>(a)->actor.health);
        if (hp <= 5)
            at<bpw_class>(a)->m3F6 = 2;
        else if (hp <= 10)
            at<bpw_class>(a)->m3F6 = 1;
        at<bpw_class>(a)->actor.attention_info.flags = 4;
        at<bpw_class>(a)->actor.actor_status = u32(at<bpw_class>(a)->actor.actor_status) & ~0x4000u;
        break;
    }
    case 0x78: {
        if (!demo_accept(a))
            break;
        set_timer(5, 3);
        at<bpw_class>(a)->m3F6 = 3;
        for (u32 i = 0; i < 10; i++)
            at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
        f32 by = f32(at<bpw_class>(a)->mBodyPos.y);
        at<bpw_class>(a)->actor.gravity = -3.f;
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        at<bpw_class>(a)->actor.speedF = 0.f;
        at<bpw_class>(a)->actor.current.pos.y = add(add(by, 200.f), f32(BpwHio(18).mFloat[0]));
        f32 r1 = f32(BpwHio(18).mFloat[1]);
        at<bpw_class>(a)->actor.scale.x = 0.f;
        at<bpw_class>(a)->actor.scale.y = 0.f;
        at<bpw_class>(a)->actor.scale.z = 0.f;
        at<bpw_class>(a)->m48C = r1;
        at<bpw_class>(a)->m3E6 = 0;
        at<bpw_class>(a)->m3E5 = 0;
        anm_init(i_this, dRes_INDEX_BPW_BCK_CORE1_e, 15.f, 2, 1.f, -1);
        u32 kid = u32(at<bpw_class>(a)->m3FC);
        if (kid != 0xFFFFFFFF) {
            Local<be<u32>> key;
            key->set(kid);
            u32 k = BpwHD_fopAcIt_Judge(0x025E1234u, key.get());
            BpwHD_fopAcM_createDisappear(k, ea(&at<bpw_class>(k)->actor.current.pos), 5, 3,
                                         0xFF); // fopAcM_createDisappear
            BpwHD_fopAcM_delete(k);
        }
        for (s32 i = 0; i < s8(at<bpw_class>(a)->actor.max_health); i++) {
            u32 p = poe(i);
            if (p && u8(at<BpwChildPoeHD>(p)->m344) != 0) { // pw_class m344
                at<fopAc_ac_c>(p)->actor_status = u32(at<fopAc_ac_c>(p)->actor_status) | 0x4000;
                at<bpw_class>(a)->m462 = (s16)i;
                Bpw_copyWord(&at<bpw_class>(a)->m424.x, &at<bpw_class>(p)->actor.current.pos.x);
                Bpw_copyWord(&at<bpw_class>(a)->m424.y, &at<bpw_class>(p)->actor.current.pos.y);
                Bpw_copyWord(&at<bpw_class>(a)->m424.z, &at<bpw_class>(p)->actor.current.pos.z);
                at<bpw_class>(a)->m464 = BpwHD_fopAcM_searchActorAngleY(p, player_by_play());
            }
        }
        at<bpw_class>(a)->m47E = 0;
        at<bpw_class>(a)->m480 = 0x96;
        at<daPy_py_c>(pl)->mDemo.mParam0 = 0;
        at<daPy_py_c>(pl)->mDemo.mDemoMode = 0x1A;
        at<daPy_py_c>(pl)->mDemo.mDemoType = 3; // DEMO_LOOKUP
        BpwHD_dCamera_c_Stop(ea(&at<BpwCameraHD>(cam)->mCamera));
        BpwHD_dCamera_c_SetTrimSize(ea(&at<BpwCameraHD>(cam)->mCamera), 2);
        BpwHD_mDoAud_bgmStop(0x1E); // mDoAud_bgmStop(30)
        at<bpw_class>(a)->m474 = 1;
        set_timer(0, regf_t(20, 0, 135.f));
        inc_state(a);
        {
            u32 p = poe(s16(at<bpw_class>(a)->m462));
            if (p) {
                Bpw_copyWord(&at<bpw_class>(a)->m40C.x, &at<bpw_class>(p)->actor.current.pos.x);
                Bpw_copyWord(&at<bpw_class>(a)->m40C.y, &at<bpw_class>(p)->actor.current.pos.y);
                Bpw_copyWord(&at<bpw_class>(a)->m40C.z, &at<bpw_class>(p)->actor.current.pos.z);
            }
        }
    }
        /* fallthrough */
    case 0x79: {
        u32 p = poe(s16(at<bpw_class>(a)->m462));
        if (p) {
            Bpw_copyWord(&at<bpw_class>(a)->m424.x, &at<bpw_class>(p)->actor.current.pos.x);
            Bpw_copyWord(&at<bpw_class>(a)->m424.y, &at<bpw_class>(p)->actor.current.pos.y);
            Bpw_copyWord(&at<bpw_class>(a)->m424.z, &at<bpw_class>(p)->actor.current.pos.z);
            at<bpw_class>(a)->m464 = BpwHD_fopAcM_searchActorAngleY(p, player_by_play());
        }
        f32 mx = f32(at<bpw_class>(a)->m424.x);
        at<bpw_class>(a)->m440 = 50.f;
        f32 mz = f32(at<bpw_class>(a)->m424.z), my = f32(at<bpw_class>(a)->m424.y);
        at<bpw_class>(a)->m40C.x = mx;
        at<bpw_class>(a)->m40C.z = mz;
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m40C.y),
                            add(add(my, 150.f), f32(BpwHio(12).mFloat[17])), 0.1f, 10.f);
        BpwHD_mDoMtx_YrotS_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->m464));
        Local<BpwVecHD> v, out;
        v->x = 0.f;
        v->y = 0.f;
        v->z = 800.f;
        BpwHD_MtxPosition_2(v.get(), out.get());
        f32 ox = add((f32)out->x, f32(at<bpw_class>(a)->m424.x));
        f32 oz = add((f32)out->z, f32(at<bpw_class>(a)->m424.z));
        f32 oy = fmadds(f32(at<bpw_class>(a)->m424.y), 0.3f, (f32)out->y);
        out->x = ox;
        out->z = oz;
        out->y = oy;
        at<bpw_class>(a)->m418.x = ox;
        at<bpw_class>(a)->m418.y = add(oy, 100.f);
        at<bpw_class>(a)->m418.z = oz;
        if (timer(0) != 0)
            break;
        set_timer(0, regf_t(20, 1, 25.f));
        if (!reg20_step())
            break;
        at<daPy_py_c>(pl)->mDemo.mDemoMode = 1; // DEMO_N_WAIT
        {
            Local<BpwVecHD> q;
            q->x = -36.f;
            f32 m424x = f32(at<bpw_class>(a)->m424.x);
            q->y = f32(at<fopAc_ac_c>(pl)->current.pos.y);
            q->z = -777.f;
            f32 px = f32(at<fopAc_ac_c>(pl)->current.pos.x),
                pz = f32(at<fopAc_ac_c>(pl)->current.pos.z), m424z = f32(at<bpw_class>(a)->m424.z);
            u32 vt = u32(at<fopAc_ac_c>(pl)->__vtbl);
            s16 ang = BpwHD_cM_atan2s(sub(px, m424x), sub(pz, m424z));
            call_ptr<void>(u32(at<BpwPlayerVtableHD>(vt)->setPlayerPosAndAngle), pl, q.get(),
                           (s32)ang);
        }
        inc_state(a);
        break;
    }
    case 0x7A: {
        s16 ang = BpwHD_fopAcM_searchActorAngleY(a, player_actor());
        at<bpw_class>(a)->actor.shape_angle.y = ang;
        at<bpw_class>(a)->actor.current.angle.y = ang;
        at<bpw_class>(a)->m476 = ang;
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(20, 2, 50.f), 1.f, 0.5f);
        set_cam_pos(a, -484.f, 628.f, -1495.f, -527.f, 698.f, -1645.f);
        player_to_room_face(a, pl);
        s16 t0 = timer(0);
        if (t0 != 0) {
            if (t0 == 1)
                se_start_e(a, 0x6A3C, 0); // JA_SE_OBJ_BPW_MASK_APPEAR
            break;
        }
        at<daPy_py_c>(pl)->mDemo.mDemoMode = 0x31; // DEMO_S_SURP
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->actor.scale), 1.f, 1.f, 0.1f);
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(a)->m47E), (s32)s16(at<bpw_class>(a)->m480),
                                    1, 0xF);
        f32 s = f32(at<bpw_class>(a)->actor.scale.x);
        at<bpw_class>(a)->actor.scale.z = s;
        at<bpw_class>(a)->actor.scale.y = s;
        if (!reg20_step())
            break;
        if (f32(at<bpw_class>(a)->actor.scale.x) < 0.9f)
            break;
        at<bpw_class>(a)->actor.scale.x = 1.f;
        at<bpw_class>(a)->actor.scale.y = 1.f;
        at<bpw_class>(a)->actor.scale.z = 1.f;
        set_timer(0, regf_t(20, 9, 10.f));
        set_timer(1, regf_t(20, 10, 100.f));
        set_timer(4, regf_t(12, 12, 30.f));
        at<bpw_class>(a)->m43C = 0.f;
        inc_state(a);
    }
        /* fallthrough */
    case 0x7B:
        if (timer(0) != 0) {
            set_timer(1, regf_t(20, 10, 100.f));
            set_timer(4, regf_t(12, 12, 30.f));
            break;
        }
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(20, 11, 50.f), 1.f, 0.5f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, regf(20, 12, -114.f), 0.1f, 20, 18, 0.02f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, regf(20, 13, 95.f), 0.1f, 20, 18, 0.02f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, regf(20, 14, -293.f), 0.1f, 20, 18, 0.02f);
        demo_calc(a, &at<bpw_class>(a)->m418.x, regf(20, 15, -169.f), 1.f, 20, 18, 0.02f);
        demo_calc(a, &at<bpw_class>(a)->m418.y, regf(20, 16, 98.f), 1.f, 20, 18, 0.02f);
        demo_calc(a, &at<bpw_class>(a)->m418.z, regf(20, 17, -462.f), 1.f, 20, 18, 0.02f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        if (u32(at<bpw_class>(a)->m3F8) == 0x1B) {
            if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 1.f))
                mons_se(a, 0x496A, 0, true); // LOOK_RIGHT
            if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 41.f))
                mons_se(a, 0x496B, 0, true); // LOOK_LEFT
            if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 76.f))
                mons_se(a, 0x496C, 0, true); // SURPRISE
        }
        if (timer(4) == 1)
            anm_init(i_this, dRes_INDEX_BPW_BCK_KYORO_GARN1_e, 8.f, 0, 1.f, -1); // KYORO_GARN1
        if (timer(1) != 0)
            break;
        if (!reg20_step())
            break;
        inc_state(a);
        /* fallthrough */
    case 0x7C:
        if (u32(at<bpw_class>(a)->m3F8) == 0x1B) {
            if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 1.f))
                mons_se(a, 0x496A, 0, true);
            if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 41.f))
                mons_se(a, 0x496B, 0, true);
            if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 76.f))
                mons_se(a, 0x496C, 0, true);
        }
        if (!morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
            break;
        set_timer(0, regf_t(19, 0, 60.f));
        inc_state(a);
        /* fallthrough */
    case 0x7D:
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(19, 1, 50.f), 1.f, 0.5f);
        set_cam_pos(a, 61.f, 125.f, -159.f, -146.f, 39.f, -965.f);
        if (timer(0) != 0)
            break;
        if (!reg20_step())
            break;
        anm_init(i_this, dRes_INDEX_BPW_BCK_CORE_NIGE1_e, 8.f, 2, 1.f, -1);
        at<bpw_class>(a)->m43C = 0.f;
        set_timer(0, 4);
        inc_state(a);
        /* fallthrough */
    case 0x7E: {
        if (timer(0) == 0)
            se_start(a, 0x516C, 0); // JA_SE_CM_BPW_MASK_RUN_AWAY
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(19, 9, 50.f), 1.f, 0.5f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, regf(19, 10, -471.f), 0.1f, 19, 12, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, regf(19, 10, 120.f), 0.1f, 19, 12, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, regf(19, 11, -498.f), 0.1f, 19, 12, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m418.x, regf(20, 15, 115.f), 1.f, 19, 12, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m418.y, regf(20, 16, 58.f), 1.f, 19, 12, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m418.z, regf(20, 17, -1062.f), 1.f, 19, 12, 0.05f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        BpwHD_cLib_addCalc2_5(ea(&at<bpw_class>(a)->actor.current.pos), -1130.f, 1.f,
                              regf(19, 13, 20.f));
        BpwHD_cLib_addCalc2_5(ea(&at<bpw_class>(a)->actor.current.pos.z), 30.f, 1.f,
                              regf(19, 13, 20.f));
        f32 dx = sub(f32(at<bpw_class>(a)->actor.current.pos.x), -1130.f);
        f32 dz = sub(f32(at<bpw_class>(a)->actor.current.pos.z), 30.f);
        at<bpw_class>(a)->m476 = (s16)(BpwHD_cM_atan2s(dx, dz) + 0x8000);
        player_to_room_face(a, pl);
        if (!(BpwHD_sqrtf(fmadds(dx, dx, mul(dz, dz))) < 5.f))
            break;
        at<bpw_class>(a)->actor.gravity = 0.f;
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        at<bpw_class>(a)->actor.speedF = 0.f;
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m476), -0x8000, 1, 0x2000);
        if (!reg20_step())
            break;
        at<bpw_class>(a)->m47C = 0;
        set_timer(0, regf_t(18, 5, 63.f));
        set_timer(1, regf_t(18, 6, 75.f));
        BpwHD_mDoAud_bgmStreamPrepare(0xC0000003u); // mDoAud_bgmStreamPrepare(JA_STRM_BOSS_CLEAR)
        inc_state(a);
        at<bpw_class>(a)->m43C = 0.f;
        break;
    }
    case 0x7F: {
        if (timer(0) != 0)
            se_start(a, 0x516C, 0);
        if (timer(1) == 1)
            BpwHD_mDoAud_bgmStreamPlay(); // mDoAud_bgmStreamPlay()
        u16 ph = (u16)ftoi(add((f32)s16(at<bpw_class>(a)->m47C), regf(18, 2, 700.f)));
        at<bpw_class>(a)->m47C = ph;
        f32 sn = Bpw_sin(ph);
        at<bpw_class>(a)->m3AC.x = mul(regf(18, 3, 30.f), sn);
        at<bpw_class>(a)->m3AC.z = mul(regf(18, 4, 30.f), sn);
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m476), -0x8000, 1, 0x2000);
        at<bpw_class>(a)->actor.speed.y = regf(19, 14, 30.f);
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(19, 15, 50.f), 1.f, 0.5f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, regf(19, 16, -538.f), 0.1f, 19, 19, 0.04f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, regf(19, 17, 748.f), 0.1f, 19, 19, 0.04f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, regf(19, 18, -567.f), 0.1f, 19, 19, 0.04f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        if (timer(0) == 1) {
            at<bpw_class>(a)->m3F6 = 4;
            se_start_e(a, 0x6A3F, 0); // JA_SE_OBJ_BPW_LIGHT_ON
            set_timer(5, 0);
            at<bpw_class>(a)->mKankyouHendouState = 3;
            anm_init(i_this, dRes_INDEX_BPW_BCK_GATAGATA1_e, regf(12, 10, 8.f), 2, 1.f,
                     -1); // GATAGATA1
        }
        if (!(f32(at<bpw_class>(a)->actor.current.pos.y) > 2000.f))
            break;
        at<bpw_class>(a)->actor.speedF = 0.f;
        at<bpw_class>(a)->actor.speed.x = 0.f;
        at<bpw_class>(a)->actor.speed.y = 0.f;
        at<bpw_class>(a)->actor.speed.z = 0.f;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->m3F6 = 4;
        mons_se(a, 0x496D, 0, false); // JA_SE_CV_BPW_DIE
        set_timer(0, regf_t(18, 8, 40.f));
        inc_state(a);
        break;
    }
    case 0x80:
        if (timer(1) == 1)
            BpwHD_mDoAud_bgmStreamPlay();
        if (timer(0) != 0)
            break;
        at<bpw_class>(a)->m3AC.x = 0.f;
        at<bpw_class>(a)->m3AC.y = 0.f;
        at<bpw_class>(a)->m3AC.z = 0.f;
        at<bpw_class>(a)->m480 = 0xFF;
        inc_state(a);
        /* fallthrough */
    case 0x81:
        if (timer(1) == 1)
            BpwHD_mDoAud_bgmStreamPlay();
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(a)->m47E), (s32)s16(at<bpw_class>(a)->m480),
                                    1, 5);
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(18, 9, 50.f), 1.f, 0.5f);
        at<bpw_class>(a)->m476 = -0x483F;
        at<bpw_class>(a)->actor.shape_angle.y = -0x483F;
        at<bpw_class>(a)->actor.shape_angle.x = -0x2337;
        at<bpw_class>(a)->actor.shape_angle.z = 0x3E9;
        set_cam_pos(a, -1030.f, 2017.f, 119.f, -1332.f, 2722.f, -153.f);
        if (s16(at<bpw_class>(a)->m47E) < 0xFE)
            break;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->m3E0 = 1;
        at<bpw_class>(a)->m47E = 0xFF;
        at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mRate =
            0.f; // mpMorf->setPlaySpeed(0)
        set_timer(0, regf_t(18, 17, 40.f));
        inc_state(a);
        /* fallthrough */
    case 0x82:
        if (timer(0) != 0)
            break;
        set_timer(0, regf_t(18, 18, 20.f));
        at<bpw_class>(a)->actor.gravity = -1.f;
        inc_state(a);
        /* fallthrough */
    case 0x83:
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(a)->actor.shape_angle),
                                    (s32)(s16)ftoi(regf(18, 19, 49152.f)), 1, 0x300);
        if (timer(0) != 0)
            break;
        inc_state(a);
        at<bpw_class>(a)->actor.gravity = -3.f;
        /* fallthrough */
    case 0x84:
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(6, 7, 50.f), 1.f, 0.5f);
        set_cam_pos(a, -893.f, 1874.f, 66.f, 142.f, 68.f, -1348.f);
        if (!reg20_step())
            break;
        at<daPy_py_c>(pl)->mDemo.mDemoMode = 0x1D; // DEMO_UNK_029
        at<bpw_class>(a)->m43C = 0.f;
        inc_state(a);
        /* fallthrough */
    case 0x85:
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(a)->actor.shape_angle),
                                    (s32)(s16)ftoi(regf(18, 19, 49152.f)), 1, 0x300);
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->actor.shape_angle.z), 0, 1, 0x300);
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(6, 7, 50.f), 1.f, 0.5f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, regf(6, 8, -1352.f), 0.1f, 6, 6, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, regf(6, 9, 174.f), 0.1f, 6, 6, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, regf(6, 10, 680.f), 0.1f, 6, 6, 0.05f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->m3AC.y), 1.f, 3.f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m48C), -120.f, 1.f, 3.f);
        player_to_room_face(a, pl);
        if (add(add(f32(at<bpw_class>(a)->mAcch.m_ground_h), 1350.f), f32(BpwHio(6).mFloat[19])) <
            f32(at<bpw_class>(a)->actor.current.pos.y))
            break;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->m43C = 0.f;
        inc_state(a);
        /* fallthrough */
    case 0x86: {
        BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(6, 11, 50.f), 1.f, 0.5f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, regf(6, 12, -2340.f), 0.1f, 6, 18, 0.1f);
        { // m40C.y: target REG12_F(13) + 186, scale 0.5, step from REG6_F(13) + 36
            f32 v = f32(at<bpw_class>(a)->m40C.y);
            f32 d = __builtin_fabsf(sub(v, regf(6, 13, 36.f)));
            f32 k = regf(6, 18, 0.1f);
            f32 step = mul(mul(d, k), f32(at<bpw_class>(a)->m43C));
            BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->m40C.y), regf(12, 13, 186.f), 0.5f, step);
        }
        demo_calc(a, &at<bpw_class>(a)->m40C.z, regf(6, 14, 1442.f), 0.1f, 6, 18, 0.1f);
        demo_calc(a, &at<bpw_class>(a)->m418.x, regf(6, 15, -744.f), 1.f, 6, 18, 0.1f);
        demo_calc(a, &at<bpw_class>(a)->m418.y, regf(6, 16, 74.f), 1.f, 6, 18, 0.1f);
        demo_calc(a, &at<bpw_class>(a)->m418.z, regf(6, 17, -432.f), 1.f, 6, 18, 0.1f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->m3AC.y), 1.f, 3.f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m48C), -120.f, 1.f, 3.f);
        if (add(f32(at<bpw_class>(a)->mAcch.m_ground_h), f32(BpwHio(12).mFloat[14])) <
            f32(at<bpw_class>(a)->actor.current.pos.y))
            break;
        if (!reg20_step())
            break;
        u32 play = BpwHD_getGameInfo();
        u32 em = BpwHD_dPa_control_c_set_7(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0,
                                           0x8456, ea(&at<bpw_class>(a)->actor.current.pos),
                                           ea(&at<bpw_class>(a)->actor.shape_angle), 0, 0xFF, 0, -1,
                                           0, 0, 0); // BPWBREAKMASK00
        if (em) {
            u32 env = BpwHD_dKy_getEnvlight();
            u8 b = u8(at<BpwEnvLightHD>(env)->mActorColor.b),
               r = u8(at<BpwEnvLightHD>(env)->mActorColor.r),
               g = u8(at<BpwEnvLightHD>(env)->mActorColor.g);
            at<BpwEmitterHD>(em)->mGlobalPrmColor.r = r;
            at<BpwEmitterHD>(em)->mGlobalPrmColor.g = g;
            at<BpwEmitterHD>(em)->mGlobalPrmColor.b = b;
        }
        se_start_e(a, 0x583C, 0); // JA_SE_CM_BOSS_EXPLODE
        {
            Local<BpwVecHD> p;
            f32 k = regf(8, 5, 80.f);
            p->x = f32(at<bpw_class>(a)->actor.current.pos.x);
            f32 y = f32(at<bpw_class>(a)->actor.current.pos.y);
            p->y = y;
            p->z = f32(at<bpw_class>(a)->actor.current.pos.z);
            p->y = add(y, k);
            BpwHD_fopAcM_createDisappear_2(a, p.get(), 0xF, 2,
                                           0xFF); // fopAcM_createDisappear(HEART_CONTAINER)
        }
        if (ea(&at<bpw_class>(a)->actor.eyePos)) {
            s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
            BpwHD_mDoAud_seStart_2(0x596Du, ea(&at<bpw_class>(a)->actor.eyePos), 0u,
                                   reverb); // JA_SE_CM_BPW_MASK_BREAK
        }
        at<bpw_class>(a)->actor.scale.x = 0.f;
        at<bpw_class>(a)->actor.scale.y = 0.f;
        inc_state(a);
        at<bpw_class>(a)->actor.scale.z = 0.f;
        set_timer(0, regf_t(12, 16, 120.f));
    }
        /* fallthrough */
    case 0x87: {
        if (timer(0) != 0) {
            BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->m40C.y), regf(12, 13, 186.f), 0.3f,
                                  regf(12, 14, 100.f));
            break;
        }
        player_to_room_body(a, pl);
        at<bpw_class>(a)->m40C.x = 0.f;
        at<bpw_class>(a)->m40C.y = 0.f;
        at<bpw_class>(a)->m40C.z = 0.f;
        f32 rad = regf(9, 1, 1400.f);
        at<bpw_class>(a)->m3F0 = rad;
        u16 an = (u16)ftoi(regf(9, 2, -15000.f));
        at<bpw_class>(a)->m3F4 = an;
        at<bpw_class>(a)->m418.x = mul(Bpw_sin(an), rad);
        at<bpw_class>(a)->m418.y = regf(9, 3, 900.f);
        at<bpw_class>(a)->m418.z = mul(Bpw_cos(an), rad);
        if (!reg20_step())
            break;
        at<bpw_class>(a)->m462 = 0;
        if (s16(BpwHio(0).mShort[3]) == 0)
            BpwHD_dSv_memBit_c_onDungeonItem(u32(l_saveInfo) + 0x798,
                                             3); // dComIfGs_onStageBossEnemy()
        set_timer(1, regf_t(9, 4, 80.f));
        set_timer(2, regf_t(9, 5, 470.f));
        set_timer(9, 0);
        inc_state(a);
    }
        /* fallthrough */
    case 0x88: {
        player_to_room_body(a, pl);
        s16 t9 = timer(9), t2 = timer(2);
        if (t9 == 1) {
            at<bpw_class>(a)->m3EC = 0.f;
            at<bpw_class>(a)->mKankyouHendouState = 6;
        }
        if (t2 != 0) {
            if (timer(1) == 1) {
                BpwHD_fopAcM_createWarpFlower(ea(&at<bpw_class>(a)->mBodyPos), 0,
                                              (s32)s8(at<bpw_class>(a)->actor.current.roomNo),
                                              0); // fopAcM_createWarpFlower
                set_timer(9, (s16)(s16(BpwHio(6).mShort[5]) + 30));
            }
            BpwHD_cLib_addCalc2(ea(&at<bpw_class>(a)->m440), regf(9, 6, 50.f), 1.f, 0.5f);
            s16 an = (s16)(s16(at<bpw_class>(a)->m3F4) + (s16)ftoi(regf(9, 7, 33.f)));
            if (an > 0)
                an = 0;
            at<bpw_class>(a)->m3F4 = an;
            BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->m3F0), regf(9, 8, 600.f), 1.f,
                                  regf(9, 9, 2.f));
            u16 a1 = u16(at<bpw_class>(a)->m3F4);
            at<bpw_class>(a)->m418.x = mul(Bpw_sin(a1), f32(at<bpw_class>(a)->m3F0));
            BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->m418.y), regf(9, 10, 900.f), 1.f,
                                  regf(9, 11, 10.f));
            u16 a2 = u16(at<bpw_class>(a)->m3F4);
            f32 c = Bpw_cos(a2);
            at<bpw_class>(a)->m418.z = mul(c, f32(at<bpw_class>(a)->m3F0));
            break;
        }
        if (!(f32(BpwHio(9).mFloat[13]) == 0.f)) {
            at<bpw_class>(a)->mActionState = 0x87;
            set_timer(0, 0);
            break;
        }
        if (!(f32(BpwHio(9).mFloat[12]) == 0.f)) {
            at<bpw_class>(a)->mActionState = 0x87;
            BpwHio(9).mFloat[12] = 0.f;
            set_timer(0, 0);
            break;
        }
        if (!reg20_step())
            break;
        cam_reset_start(a, cam, pl);
        BpwHD_fopAcM_delete(a);
        break;
    }
    }
    if (s16(at<bpw_class>(a)->mActionState) >= 0x78) {
        at<bpw_class>(a)->m498 = 0.f;
        at<bpw_class>(a)->m3AC.y = 100.f;
        at<bpw_class>(a)->m49C = 160.f;
    }
    if (s16(at<bpw_class>(a)->m474) != 0) {
        Local<BpwVecHD> center, eye;
        eye->x = f32(at<bpw_class>(a)->m418.x);
        center->y = f32(at<bpw_class>(a)->m40C.y);
        eye->z = f32(at<bpw_class>(a)->m418.z);
        center->x = f32(at<bpw_class>(a)->m40C.x);
        eye->y = f32(at<bpw_class>(a)->m418.y);
        center->z = f32(at<bpw_class>(a)->m40C.z);
        BpwHD_dCamera_c_Set(ea(&at<BpwCameraHD>(cam)->mCamera), center.get(), eye.get(),
                            f32(at<bpw_class>(a)->m440), 0);
    }
}

/* . HD: SearchByID inline skips the search for an invalid id; the player demo calls are
 * inline stores (demo type 0x420, mode 0x430); camera via the play object's camera table. */
/* 020DB380 */
void action_start_demo(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 player = player_actor();
    s32 camId = s8(at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mPlayerInfo[0].mCameraID);
    u32 cam = u32(at<dComIfG_camera_info_class>(
                      ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCameraInfo[0]) +
                      camId * sizeof(dComIfG_camera_info_class))
                      ->mpCamera);
    s16 st = s16(at<bpw_class>(a)->mActionState);
    auto timer = [&](u32 i) -> s16 { return s16(at<bpw_class>(a)->mSomeCountdownTimers[i]); };
    switch (st) {
    case 200:
        at<bpw_class>(a)->actor.scale.x = 0.f;
        at<bpw_class>(a)->actor.scale.y = 0.f;
        at<bpw_class>(a)->actor.scale.z = 0.f;
        at<bpw_class>(a)->m47E = 0;
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xC9:
        if (u16(at<bpw_class>(a)->actor.eventInfo.mCommand) !=
            2) { // !eventInfo.checkCommandDemoAccrpt()
            u32 play = BpwHD_getGameInfo();
            at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag =
                (u16)(u16(at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag) |
                      1); // dComIfGp_event_onEventFlag(1)
            BpwHD_fopAcM_orderPotentialEvent(a, 2, 0xFFFF,
                                             0); // fopAcM_orderPotentialEvent(STAFF_ALL)
            at<bpw_class>(a)->actor.eventInfo.mCondition =
                (u16)(u16(at<bpw_class>(a)->actor.eventInfo.mCondition) |
                      2); // onCondition(dEvtCnd_UNK2_e)
            break;
        }
        at<daPy_py_c>(player)->mDemo.mParam0 = 0; // changeOriginalDemo()
        at<daPy_py_c>(player)->mDemo.mDemoType = 3;
        at<daPy_py_c>(player)->mDemo.mDemoMode = 0x31; // changeDemoMode(DEMO_S_SURP_e)
        at<bpw_class>(a)->m3AC.y = 140.f;
        BpwHD_dCamera_c_Stop(ea(&at<BpwCameraHD>(cam)->mCamera));           // Stop()
        BpwHD_dCamera_c_SetTrimSize(ea(&at<BpwCameraHD>(cam)->mCamera), 2); // SetTrimSize(2)
        at<bpw_class>(a)->m440 = regf(6, 0, 55.f);
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(9, 0, 40.f);
        anm_init(i_this, dRes_INDEX_BPW_BCK_CORE1_e, 0.f, 2, 1.f, -1); // CORE1
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xCA:
        set_cam_pos(a, 72.f, 103.f, 1618.f, 89.f, 108.f, 1788.f);
        if (timer(0) != 0)
            break;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(9, 1, 50.f);
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xCB:
        at<bpw_class>(a)->m440 = regf(6, 1, 50.f);
        set_cam_pos(a, 1646.f, 1215.f, -72.f, 1767.f, 1321.f, -129.f);
        if (timer(0) != 0)
            break;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(9, 2, 40.f);
        at<bpw_class>(a)->mSomeCountdownTimers[1] = regf_t(9, 3, 20.f);
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xCC:
        if (timer(1) == 1) {
            at<bpw_class>(a)->m3EA = 1;
            BpwHD_mDoAud_bgmStreamPlay();
        } // mDoAud_bgmStreamPlay()
        if (timer(0) != 0)
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(9, 4, 38.f);
        at<bpw_class>(a)->mSomeCountdownTimers[1] = regf_t(9, 5, 28.f);
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xCD:
        at<bpw_class>(a)->m440 = regf(6, 2, 55.f);
        set_cam_pos(a, 72.f, 103.f, 1618.f, 89.f, 108.f, 1788.f);
        if (timer(1) == 0)
            at<bpw_class>(a)->m3EA = 2;
        if (timer(0) != 0)
            break;
        at<daPy_py_c>(player)->mDemo.mDemoMode = 0x18; // changeDemoMode(DEMO_SURPRISED_e)
        if (!reg20_step())
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(18, 0, 40.f);
        at<bpw_class>(a)->mSomeCountdownTimers[1] = regf_t(18, 1, 110.f);
        at<bpw_class>(a)->m43C = 0.f;
        at<bpw_class>(a)->mSomeCountdownTimers[2] = regf_t(18, 11, 20.f);
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xCE:
        if (timer(2) == 1)
            at<bpw_class>(a)->m3EA = 3;
        if (timer(0) != 0)
            break;
        demo_calc(a, &at<bpw_class>(a)->m440, regf(6, 3, 55.f), 1.f, 18, 2, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, 0.f, 0.1f, 18, 2, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, 233.f, 0.1f, 18, 2, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, 552.f, 0.1f, 18, 2, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m418.x, 0.f, 1.f, 18, 2, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m418.y, 270.f, 1.f, 18, 2, 0.05f);
        demo_calc(a, &at<bpw_class>(a)->m418.z, 722.f, 1.f, 18, 2, 0.05f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        at<bpw_class>(a)->m3EA = 4;
        if (timer(1) != 0)
            break;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->m3EA = 5;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(18, 3, 30.f);
        at<bpw_class>(a)->m480 = 0x96;
        se_start(a, 0x6A3C, 0); // JA_SE_OBJ_BPW_MASK_APPEAR
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xCF: {
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(a)->m47E), (s32)s16(at<bpw_class>(a)->m480),
                                    1, 5);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->actor.scale), 1.f, 1.f, 0.1f);
        f32 s = f32(at<bpw_class>(a)->actor.scale.x);
        at<bpw_class>(a)->actor.scale.z = s;
        at<bpw_class>(a)->actor.scale.y = s;
        if (timer(0) != 0)
            break;
        at<bpw_class>(a)->m3EA = 6;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[5] = (s16)(s16(BpwHio(21).mShort[2]) + 10);
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        break;
    }
    case 0xD0:
        if (u32(at<bpw_class>(a)->m3F8) == 9) {
            if (morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
                anm_init(i_this, dRes_INDEX_BPW_BCK_CORE1_e, 15.f, 2, 1.f, -1);
            BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->m3AC.y), regf(18, 12, 100.f), 1.f,
                                  regf(18, 13, 10.f));
            se_start(a, 0x623D, 0); // JA_SE_OBJ_BPW_MASK_LAUGH
        } else {
            BpwHD_cLib_addCalc2_5(ea(&at<bpw_class>(a)->m3AC.y), 140.f, 1.f, regf(18, 13, 10.f));
        }
        if (s16(at<bpw_class>(a)->m464) != 0) {
            anm_init(i_this, dRes_INDEX_BPW_BCK_CORE_NIGE1_e, 0.f, 2, 1.f, -1);
            at<bpw_class>(a)->m464 = 0;
        } // CORE_NIGE1
        if (s16(at<bpw_class>(a)->m462) < 15)
            break;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(18, 4, 30.f);
        at<bpw_class>(a)->mSomeCountdownTimers[1] = regf_t(18, 5, 55.f);
        at<bpw_class>(a)->m43C = 0.f;
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xD1: {
        if (timer(1) != 0)
            break;
        demo_calc(a, &at<bpw_class>(a)->m440, regf(6, 4, 65.f), 1.f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, 281.f, 0.1f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, 93.f, 0.1f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, 725.f, 0.1f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.x, 336.f, 1.f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.y, 30.f, 1.f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.z, 877.f, 1.f, 18, 8, 0.08f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        if (!reg20_step())
            break;
        if (timer(0) != 0)
            break;
        anm_init(i_this, dRes_INDEX_BPW_BCK_OPENING1_e, regf(18, 7, 10.f), 0, 1.f, -1); // OPENING1
        u32 k = search_by_id(u32(at<bpw_class>(a)->m3FC));
        if (k)
            anm_init((bpw_class *)at<bpw_class>(k), dRes_INDEX_BPW_BCK_OPENING_KAN1_e,
                     regf(18, 7, 10.f), 0, 1.f, -1); // OPENING_KAN1
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x, &at<bpw_class>(a)->m3B8.x);
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.y, &at<bpw_class>(a)->m3B8.y);
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z,
                     &at<bpw_class>(a)->m3B8.z); // mFire1DousaPos = m3B8
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaRot.x, &at<bpw_class>(a)->actor.shape_angle.x);
        at<bpw_class>(a)->mFire1DousaRot.z =
            u16(at<bpw_class>(a)->actor.shape_angle.z);     // mFire1DousaRot = shape_angle
        if (u32(at<bpw_class>(a)->m554.mpEmitter.v) == 0) { // m554.getEmitter() == NULL
            u32 play = BpwHD_getGameInfo();
            BpwHD_dPa_control_c_set_4(u32(at<dComIfG_inf_c>(play)->play.mParticle.v), 0, 0x845A,
                                      ea(&at<bpw_class>(a)->mFire1DousaPos),
                                      ea(&at<bpw_class>(a)->mFire1DousaRot), 0, 0xFF,
                                      ea(&at<bpw_class>(a)->m554), -1, 0, 0, 0);
        }
        se_start_e(a, 0x5966, 0); // JA_SE_CM_BPW_MASK_TO_BPW
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
    }
        /* fallthrough */
    case 0xD2:
        if (u32(at<bpw_class>(a)->m554.mpEmitter.v) != 0) {
            Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x, &at<bpw_class>(a)->m3B8.x);
            Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.y, &at<bpw_class>(a)->m3B8.y);
            Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z, &at<bpw_class>(a)->m3B8.z);
            Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaRot.x,
                         &at<bpw_class>(a)->actor.shape_angle.x);
            at<bpw_class>(a)->mFire1DousaRot.z = u16(at<bpw_class>(a)->actor.shape_angle.z);
        }
        demo_calc(a, &at<bpw_class>(a)->m440, regf(6, 4, 65.f), 1.f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.x, 281.f, 0.1f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.y, 93.f, 0.1f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m40C.z, 725.f, 0.1f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.x, 336.f, 1.f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.y, 30.f, 1.f, 18, 8, 0.08f);
        demo_calc(a, &at<bpw_class>(a)->m418.z, 877.f, 1.f, 18, 8, 0.08f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m43C), 1.f, 1.f, 0.04f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m3AC.y), 340.f, 1.f, 5.f);
        if (!reg20_step())
            break;
        if (!morf_isStop(u32(at<bpw_class>(a)->mpMorf)))
            break;
        BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->m554)); // m554.remove()
        at<bpw_class>(a)->mSomeCountdownTimers[0] = regf_t(6, 5, 40.f);
        anm_init(i_this, dRes_INDEX_BPW_BCK_OPENING2_e, 0.f, 2, 1.f, -1); // OPENING2
        {
            u32 k = search_by_id(u32(at<bpw_class>(a)->m3FC));
            if (k)
                anm_init((bpw_class *)at<bpw_class>(k), dRes_INDEX_BPW_BCK_OPENING_KAN2_e, 0.f, 2,
                         1.f, -1); // OPENING_KAN2
        }
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xD3:
        if (timer(0) != 0)
            break;
        if (!reg20_step())
            break;
        at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        /* fallthrough */
    case 0xD4: {
        if (timer(0) != 0)
            break;
        BpwHD_dVibration_c_StopQuake(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mVibration),
                                     0x20); // StopQuake(0x20)
        u32 k = search_by_id(u32(at<bpw_class>(a)->m3FC));
        if (k)
            anm_init((bpw_class *)at<bpw_class>(k), dRes_INDEX_BPW_BCK_KAN_DEFAULT1_e, 0.f, 0, 1.f,
                     -1); // KAN_DEFAULT1
        at<bpw_class>(a)->mSomeCountdownTimers[1] = 0;
        at<bpw_class>(a)->m3F6 = 4;
        light_on_off(i_this);
        Local<BpwVecHD> center, eye;
        center->y = f32(at<bpw_class>(a)->m40C.y);
        eye->x = f32(at<bpw_class>(a)->m418.x);
        center->z = f32(at<bpw_class>(a)->m40C.z);
        center->x = f32(at<bpw_class>(a)->m40C.x);
        eye->y = f32(at<bpw_class>(a)->m418.y);
        eye->z = f32(at<bpw_class>(a)->m418.z);
        BpwHD_dCamera_c_Reset(ea(&at<BpwCameraHD>(cam)->mCamera), center.get(),
                              eye.get());                                   // Reset(m40C, m418)
        BpwHD_dCamera_c_Start(ea(&at<BpwCameraHD>(cam)->mCamera));          // Start()
        BpwHD_dCamera_c_SetTrimSize(ea(&at<BpwCameraHD>(cam)->mCamera), 0); // SetTrimSize(0)
        at<daPy_py_c>(player)->mDemo.mDemoType = 2;
        at<daPy_py_c>(player)->mDemo.mDemoMode = 1; // cancelOriginalDemo()
        u32 play = BpwHD_getGameInfo();
        at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag =
            (u16)(u16(at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag) |
                  8);                         // dComIfGp_event_reset()
        BpwHD_mDoAud_bgmStart_2(0x80000048u); // mDoAud_bgmStart(JA_BGM_BIG_POW)
        BpwHD_dSv_memBit_c_onDungeonItem(u32(l_saveInfo) + 0x798, 5); // dComIfGs_onStageBossDemo()
        at<bpw_class>(a)->mBodyAction = ACTION_MOVE_DOUSA;
        at<bpw_class>(a)->m3F6 = 0;
        at<bpw_class>(a)->actor.actor_status = u32(at<bpw_class>(a)->actor.actor_status) & ~0x4000u;
        at<bpw_class>(a)->mActionState = 0;
        break;
    }
    default:
        break;
    }
    u32 anm = u32(at<bpw_class>(a)->m3F8);
    if (anm == 0x1F) {
        if (BpwHD_J3DFrameCtrl_checkPass(u32(at<bpw_class>(a)->mpMorf) + 0x98, 170.f))
            mons_se(a, 0x4961, 0, true); // CV_BPW_LAUGH_ENTER
        anm = u32(at<bpw_class>(a)->m3F8);
    }
    bool swing =
        (anm == 0x1F &&
         !(f32(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mFrame) < 170.f)) ||
        (anm != 0x1F && anm == 0x20);
    if (swing) {
        if (a && ea(&at<bpw_class>(a)->actor.eyePos)) {
            s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
            BpwHD_mDoAud_seStart_2(0x515Fu, ea(&at<bpw_class>(a)->actor.eyePos), 0u,
                                   reverb); // JA_SE_CM_BPW_SWING_KANTERA
            anm = u32(at<bpw_class>(a)->m3F8);
        }
        if ((anm == 0x1F &&
             !(f32(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mFrame) < 60.f)) ||
            (anm != 0x1F && anm == 0x20))
            se_start(a, 0x7057, 0);
    } else if (anm == 0x1F &&
               !(f32(at<mDoExt_McaMorf>(at<bpw_class>(a)->mpMorf)->mFrameCtrl.mFrame) < 60.f)) {
        se_start(a, 0x7057, 0); // JA_SE_OBJ_BPW_FLYING
    }
    if (s16(at<bpw_class>(a)->mActionState) >= 0xCA) {
        Local<BpwVecHD> center, eye;
        center->x = f32(at<bpw_class>(a)->m40C.x);
        center->y = f32(at<bpw_class>(a)->m40C.y);
        eye->z = f32(at<bpw_class>(a)->m418.z);
        eye->y = f32(at<bpw_class>(a)->m418.y);
        eye->x = f32(at<bpw_class>(a)->m418.x);
        center->z = f32(at<bpw_class>(a)->m40C.z);
        BpwHD_dCamera_c_Set(ea(&at<BpwCameraHD>(cam)->mCamera), center.get(), eye.get(),
                            f32(at<bpw_class>(a)->m440), 0); // Set(m40C, m418, m440, 0)
        if (s16(at<bpw_class>(a)->mActionState) >= 0xD1) {
            u32 k = search_by_id(u32(at<bpw_class>(a)->m3FC));
            if (k) {
                BpwHD_mDoExt_McaMorf_play(u32(at<bpw_class>(k)->mpMorf), 0, 0, 0);
                if (BpwHD_J3DFrameCtrl_checkPass_2(u32(at<bpw_class>(k)->mpMorf) + 0x98,
                                                   regf(8, 9, 128.f))) {
                    se_start(a, 0x596E, 0); // JA_SE_CM_BPW_KANTERA_APPEAR
                    at<bpw_class>(k)->m3E7 = 0;
                }
            }
        }
    }
}

/* 020D30EC */
void kantera_calc(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 player = player_actor();
    u32 id = u32(at<bpw_class>(a)->m3FC);
    if (id == 0xFFFFFFFF)
        return;
    Local<be<u32>> key;
    key->set(id);
    u32 k = BpwHD_fopAcIt_Judge(0x025E1234u, key.get());
    if (!k)
        return;
    s16 t = s16(at<bpw_class>(a)->mAttWaitTimer); // mAttWaitTimer
    switch ((u32)(s32)t) {
    case 0:
        BpwHD_cLib_addCalcAngleS2_2(
            ea(&at<bpw_class>(k)->actor.shape_angle),
            (s32)(s16)(s16(at<bpw_class>(a)->actor.shape_angle.x) + s16(BpwHio(12).mShort[0])), 1,
            0x1000);
        BpwHD_cLib_addCalcAngleS2_2(
            ea(&at<bpw_class>(k)->actor.shape_angle.y),
            (s32)(s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + s16(BpwHio(12).mShort[1])), 1,
            0x1000);
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->actor.shape_angle.z),
                                    (s32)(s16)(s16(at<bpw_class>(a)->actor.shape_angle.z) - 0x4000),
                                    1, 0x1000);
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->mKanteraDousaRot),
                                    (s32)s16(BpwHio(12).mShort[3]), 1, 0x1000);
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(k)->mKanteraDousaRot.y), 0x2000, 1, 0x1000);
        BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->mKanteraDousaRot.z),
                                    (s32)s16(BpwHio(12).mShort[5]), 1, 0x1000);
        if (s16(at<bpw_class>(a)->mAttWaitTimer) == 4)
            return;
        break;
    case 1:
        if (s16(at<bpw_class>(a)->mBodyAction) == ACTION_START_DEMO)
            break;
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m484), 3000, 1, 100);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m388), 2000.f, 1.f, 1000.f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m390), 2000.f, 1.f, 1000.f);
        if (s16(at<bpw_class>(a)->mAttWaitTimer) == 4)
            return;
        break;
    case 2:
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m484), 6000, 1, 100);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m388), 6000.f, 1.f, 1000.f);
        BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->m390), 6000.f, 1.f, 1000.f);
        if (s16(at<bpw_class>(a)->mAttWaitTimer) == 4)
            return;
        break;
    case 3: {
        s16 py = s16(at<fopAc_ac_c>(player)->shape_angle.y);
        at<bpw_class>(k)->mActionState = 2;
        at<fopAc_ac_c>(k)->current.angle.y = py;
        s16 nt = (s16)(s16(at<bpw_class>(a)->mAttWaitTimer) + 1);
        at<bpw_class>(a)->mAttWaitTimer = nt;
        if (nt == 4)
            return;
        break;
    }
    default:
        if (t == 4)
            return;
        break;
    }
    Bpw_copyWord(&at<bpw_class>(k)->actor.current.pos.x, &at<bpw_class>(a)->mChildActorPos.x);
    Bpw_copyWord(&at<bpw_class>(k)->actor.current.pos.y, &at<bpw_class>(a)->mChildActorPos.y);
    Bpw_copyWord(&at<bpw_class>(k)->actor.current.pos.z,
                 &at<bpw_class>(a)->mChildActorPos.z); // current.pos = mChildActorPos
    if (s16(at<bpw_class>(a)->mAttWaitTimer) == 0)
        return;
    BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->actor.shape_angle),
                                (s32)s16(at<bpw_class>(a)->actor.shape_angle.x), 1, 0x1000);
    BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->actor.shape_angle.y),
                                (s32)(s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + 0x4000), 1,
                                0x1000);
    BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->actor.shape_angle.z),
                                (s32)s16(at<bpw_class>(a)->actor.shape_angle.z), 1, 0x1000);
    s16 ph = (s16)(s16(at<bpw_class>(a)->m482) + s16(at<bpw_class>(a)->m484)); // m482 += m484
    f32 amp = f32(at<bpw_class>(a)->m388);
    at<bpw_class>(a)->m482 = ph;
    BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->mKanteraDousaRot),
                                (s32)(s16)ftoi(mul(Bpw_sin((u16)ph), amp)), 1, 0x1000);
    BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(k)->mKanteraDousaRot.y), 0, 1, 0x1000);
    u16 ph2 = u16(at<bpw_class>(a)->m482);
    f32 amp2 = f32(at<bpw_class>(a)->m390);
    BpwHD_cLib_addCalcAngleS2_2(ea(&at<bpw_class>(k)->mKanteraDousaRot.z),
                                (s32)(s16)ftoi(mul(Bpw_cos(ph2), amp2)), 1, 0x1000);
}

/* : body_execute (matcher: action_karada_taore). Inlines action_dousa, action_kougeki,
 * action_karada_taore, gouen_maai_sub, wall checks. HD: the curse/flying sounds go through the
 * out-of-line fopAcM_seStart . */
/* 020DC704 */
void body_execute(bpw_class *i_this) {
    u32 a = ea(i_this);
    Local<BpwVecHD> c;
    c->x = f32(at<bpw_class>(a)->m358.x);
    c->y = f32(at<bpw_class>(a)->m358.y);
    c->z = f32(at<bpw_class>(a)->m358.z); // local_28 = m358
    if (s16(BpwHio(8).mShort[2]) != 0) {
        f32 x = f32(at<bpw_class>(a)->actor.current.pos.x),
            y = f32(at<bpw_class>(a)->actor.current.pos.y),
            z = f32(at<bpw_class>(a)->actor.current.pos.z);
        at<bpw_class>(a)->actor.attention_info.position.x = x;
        at<bpw_class>(a)->actor.speedF = 0.f;
        at<bpw_class>(a)->actor.attention_info.position.z = z;
        at<bpw_class>(a)->actor.attention_info.position.y = add(y, 700.f);
        at<bpw_class>(a)->actor.eyePos.y = add(y, 300.f);
        at<bpw_class>(a)->actor.eyePos.x = x;
        at<bpw_class>(a)->actor.eyePos.z = z;
        BpwHD_cM3dGSph_SetC_2(ea(&at<bpw_class>(a)->mBodyCoSph.mSph), c.get());
        BpwHD_cM3dGSph_SetR_2(ea(&at<bpw_class>(a)->mBodyCoSph.mSph), 240.f);
        BpwHD_cCcS_Set(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCcS),
                       ea(&at<bpw_class>(a)->mBodyCoSph));
        Bpw_copyWord(&c.get()->x, &at<bpw_class>(a)->m370.x);
        Bpw_copyWord(&c.get()->y, &at<bpw_class>(a)->m370.y);
        Bpw_copyWord(&c.get()->z, &at<bpw_class>(a)->m370.z);
        BpwHD_cM3dGSph_SetC_2(ea(&at<bpw_class>(a)->mBodyAtSph.mSph), c.get());
        BpwHD_cM3dGSph_SetR_2(ea(&at<bpw_class>(a)->mBodyAtSph.mSph), f32(at<bpw_class>(a)->m4A8));
        BpwHD_cCcS_Set(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCcS),
                       ea(&at<bpw_class>(a)->mBodyAtSph));
        return;
    }
    switch (s16(at<bpw_class>(a)->mBodyAction)) {
    case 0:
        action_dousa(i_this);
        break;
    case 1:
        action_kougeki(i_this);
        break;
    case 2:
        action_karada_taore(i_this);
        break;
    case 3:
        action_damage(i_this);
        break;
    case 4:
        action_bunri_dousa(i_this);
        break;
    case 0x14:
        action_start_demo(i_this);
        break;
    }
    if (u8(at<bpw_class>(a)->m3E5) == 0) {
        BpwHD_cLib_addCalcAngleS2_3(ea(&at<bpw_class>(a)->actor.current.angle.y),
                                    (s32)s16(at<bpw_class>(a)->m476), 1,
                                    (s32)s16(at<bpw_class>(a)->m47A));
        BpwHD_cLib_addCalcAngleS2_3(ea(&at<bpw_class>(a)->actor.shape_angle.y),
                                    (s32)s16(at<bpw_class>(a)->actor.current.angle.y), 1,
                                    (s32)s16(at<bpw_class>(a)->m47A));
    }
    BpwHD_mDoExt_McaMorf_play(u32(at<bpw_class>(a)->mpMorf), 0, 0, 0);
    if (u8(at<bpw_class>(a)->m3E0))
        BpwHD_mDoExt_baseAnm_play(u32(at<bpw_class>(a)->mpLightFreezeBrkAnm));
    else if (u8(at<bpw_class>(a)->m3E1))
        BpwHD_mDoExt_baseAnm_play(u32(at<bpw_class>(a)->mpLightStunBrkAnm));
    else {
        u8 e2 = u8(at<bpw_class>(a)->m3E2);
        if (e2 == 2) {
            BpwHD_mDoExt_baseAnm_play(u32(at<bpw_class>(a)->mpCurseStartBrkAnm));
            BpwHD_seStart(a, 0x7058, 0);
        } // JA_SE_OBJ_BPW_FLYING_CURSE
        else if (e2 == 1) {
            u32 brk = u32(at<bpw_class>(a)->mpCurseEndBrkAnm);
            if (brk_isStop(brk))
                noroi_brk_check(i_this, 2);
            else {
                BpwHD_mDoExt_baseAnm_play_2(brk);
                BpwHD_seStart(a, 0x7058, 0);
            }
        } else {
            BpwHD_mDoExt_baseAnm_play(u32(at<bpw_class>(a)->mpDefaultBrkAnm));
            if (s16(at<bpw_class>(a)->mBodyAction) != ACTION_START_DEMO)
                BpwHD_seStart(a, 0x7057, 0); // JA_SE_OBJ_BPW_FLYING
        }
    }
    f32 y = f32(at<bpw_class>(a)->actor.current.pos.y),
        z = f32(at<bpw_class>(a)->actor.current.pos.z);
    at<bpw_class>(a)->actor.attention_info.position.y = y;
    at<bpw_class>(a)->actor.attention_info.position.z = z;
    f32 x = f32(at<bpw_class>(a)->actor.current.pos.x);
    at<bpw_class>(a)->actor.attention_info.position.x = x;
    f32 h = f32(at<bpw_class>(a)->m4A4);
    at<bpw_class>(a)->actor.eyePos.x = x;
    at<bpw_class>(a)->actor.eyePos.y = y;
    at<bpw_class>(a)->actor.attention_info.position.y = add(y, add(add(h, h), 100.f));
    at<bpw_class>(a)->actor.eyePos.z = z;
    at<bpw_class>(a)->actor.eyePos.y = add(y, f32(at<bpw_class>(a)->m4A4));
    BpwHD_cM3dGSph_SetC_2(ea(&at<bpw_class>(a)->mBodyCoSph.mSph), c.get());
    BpwHD_cM3dGSph_SetR(ea(&at<bpw_class>(a)->mBodyCoSph.mSph),
                        add((f32)s16(BpwHio(0).mShort[6]), 260.f));
    BpwHD_cCcS_Set(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCcS),
                   ea(&at<bpw_class>(a)->mBodyCoSph));
    Bpw_copyWord(&c.get()->x, &at<bpw_class>(a)->m370.x);
    Bpw_copyWord(&c.get()->y, &at<bpw_class>(a)->m370.y);
    Bpw_copyWord(&c.get()->z, &at<bpw_class>(a)->m370.z);
    BpwHD_cM3dGSph_SetC_2(ea(&at<bpw_class>(a)->mBodyAtSph.mSph), c.get());
    BpwHD_cM3dGSph_SetR_2(ea(&at<bpw_class>(a)->mBodyAtSph.mSph), f32(at<bpw_class>(a)->m4A8));
    BpwHD_cCcS_Set(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCcS),
                   ea(&at<bpw_class>(a)->mBodyAtSph));
    body_atari_check(i_this);
}

/* : inlines kantera_execute (+ action_kantera_dousa, kantera_atari_check),
 * damage_ball_execute (+ action_b_fire_1/2_dousa), torituki_execute, vib_mode_check.
 * HD: the damage-ball light is refreshed in every fire-2 state; dPa callbacks end() instead of
 * remove(); SearchByID skips invalid ids. */
/* 020D3464 */
BOOL daBPW_Execute(bpw_class *i_this) {
    u32 a = ea(i_this);
    if (u8(*at<be<u8>>(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCurStage.mName[0]))) ==
        0x58)
        at<bpw_class>(a)->m408 = 0x32;
    else {
        s16 b = s16(at<bpw_class>(a)->m408);
        if (b > 2)
            at<bpw_class>(a)->m408 = (s16)(b - 2);
    }
    for (u32 i = 0; i < 10; i++) {
        s16 t = s16(at<bpw_class>(a)->mSomeCountdownTimers[i]);
        if (t != 0)
            at<bpw_class>(a)->mSomeCountdownTimers[i] = (s16)(t - 1);
    }
    Local<BpwVecHD> spd, spdOut, cen;
    switch (u8(at<bpw_class>(a)->mType)) {
    case 0:
        body_execute(i_this);
        light_on_off(i_this);
        break;
    case 1: { /* kantera_execute */
        Local<BpwVecHD> c;
        c->x = f32(at<bpw_class>(a)->m358.x);
        c->y = f32(at<bpw_class>(a)->m358.y);
        c->z = f32(at<bpw_class>(a)->m358.z);
        if (s16(at<bpw_class>(a)->mBodyAction) == ACTION_MOVE_DOUSA) { /* action_kantera_dousa */
            BpwHD_getGameInfo();
            BpwHD_getGameInfo();
            BpwHD_getGameInfo();
            switch (s16(at<bpw_class>(a)->mActionState)) {
            case 0:
                for (u32 i = 0; i < 10; i++)
                    at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
                at<bpw_class>(a)->m3E4 = 0;
                at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
                at<bpw_class>(a)->m498 = 50.f;
                at<bpw_class>(a)->m3AC.x = 0.f;
                at<bpw_class>(a)->m3AC.y = 0.f;
                at<bpw_class>(a)->m3AC.z = 0.f;
                at<bpw_class>(a)->m49C = 200.f;
                at<bpw_class>(a)->mKanteraCoSph.mObjCo.mSPrm =
                    u32(at<bpw_class>(a)->mKanteraCoSph.mObjCo.mSPrm) &
                    ~1u; // mKanteraCoSph.ClrCoSet()
                at<bpw_class>(a)->m48C = 60.f;
                break;
            case 2:
                at<bpw_class>(a)->actor.speed.y = 10.f;
                at<bpw_class>(a)->actor.speedF = 10.f;
                at<bpw_class>(a)->m3AC.y = 140.f;
                at<bpw_class>(a)->actor.gravity = -3.f;
                at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
                /* fallthrough */
            case 3:
                switch (s16(at<bpw_class>(a)->m462)) { // m462
                case 0:
                    if (u32(at<bpw_class>(a)->mAcch.m_flags) & 0x20) {
                        at<bpw_class>(a)->mKanteraCoSph.mObjCo.mSPrm =
                            u32(at<bpw_class>(a)->mKanteraCoSph.mObjCo.mSPrm) | 1u; // OnCoSetBit
                        at<bpw_class>(a)->actor.speed.y = 17.f;
                        at<bpw_class>(a)->actor.speedF = 0.f;
                        se_start_e(a, 0x6A3E, 0); // JA_SE_OBJ_BPW_KANTERA_FALL
                        at<bpw_class>(a)->m462 = (s16)(s16(at<bpw_class>(a)->m462) + 1);
                    }
                    break;
                case 1:
                    at<bpw_class>(a)->actor.shape_angle.y =
                        (s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + 0x1000);
                    BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->actor.shape_angle), 0x2000, 1,
                                              0x700);
                    BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->actor.shape_angle.z), 0x4000, 1,
                                              0x600);
                    if (BpwHD_cLib_distanceAngleS_2((s32)s16(at<bpw_class>(a)->actor.shape_angle.z),
                                                    0x4000) < 0x100 &&
                        (u32(at<bpw_class>(a)->mAcch.m_flags) & 0x20)) {
                        at<bpw_class>(a)->m462 = (s16)(s16(at<bpw_class>(a)->m462) + 1);
                        at<bpw_class>(a)->actor.shape_angle.x = 0x2000;
                        at<bpw_class>(a)->actor.shape_angle.z = 0x4000;
                    }
                    break;
                case 2:
                    BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->mKanteraDousaRot), 0, 1, 0x500);
                    BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->mKanteraDousaRot.z), 0, 1,
                                              0x500);
                    break;
                }
                break;
            case 4:
                if (u32(at<bpw_class>(a)->mAcch.m_flags) & 0x10)
                    at<bpw_class>(a)->actor.current.angle.y =
                        (s16)(s16(at<bpw_class>(a)->actor.current.angle.y) - 0x4000); // ChkWallHit
                at<bpw_class>(a)->actor.shape_angle.y =
                    (s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + s16(at<bpw_class>(a)->m464));
                BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m464), 0, 1, 0x200);
                BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->actor.speedF), 1.f, 3.f);
                if (f32(at<bpw_class>(a)->actor.speedF) < 1.f) {
                    at<bpw_class>(a)->actor.current.angle.y =
                        s16(at<bpw_class>(a)->actor.shape_angle.y);
                    at<bpw_class>(a)->actor.speedF = 0.f;
                    at<bpw_class>(a)->m462 = 1;
                    at<bpw_class>(a)->mActionState = 3;
                }
                break;
            case 5: {
                u32 p = search_by_id(u32(at<bpw_class>(a)->m404)); // m404
                if (p)
                    at<bpw_class>(a)->actor.current.angle.y = BpwHD_fopAcM_searchActorAngleY(a, p);
                BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->actor.speedF), 1.f, 3.f);
                if (f32(at<bpw_class>(a)->actor.speedF) < 1.f) {
                    at<bpw_class>(a)->m3E4 = 0;
                    at<bpw_class>(a)->mActionState = 3;
                }
                break;
            }
            }
            s16 st = s16(at<bpw_class>(a)->mActionState);
            if (u8(at<bpw_class>(a)->m3E4) != 0 && st != 5) {
                at<bpw_class>(a)->actor.speedF = 15.f;
                at<bpw_class>(a)->mKanteraDousaRot.x = 0;
                at<bpw_class>(a)->mKanteraDousaRot.y = 0;
                at<bpw_class>(a)->mActionState = 5;
                at<bpw_class>(a)->mKanteraDousaRot.z = 0;
                st = 5;
            }
            if (st != 4) { /* kantera_atari_check */
                BpwHD_dCcD_GStts_Move(ea(&at<bpw_class>(a)->mStts.__vtbl_gstts));
                if (BpwHD_dCcD_GObjInf_ChkTgHit(ea(&at<bpw_class>(a)->mKanteraCoSph))) {
                    u32 pl = player_actor();
                    u32 hit = BpwHD_dCcD_GObjInf_GetTgHitObj(ea(&at<bpw_class>(a)->mKanteraCoSph));
                    if (hit &&
                        u32(at<dCcD_GObjInf>(hit)->mAtType) == 0x10000) { // AT_TYPE_SKULL_HAMMER
                        at<bpw_class>(a)->actor.speedF = 60.f;
                        at<bpw_class>(a)->actor.current.angle.y =
                            s16(at<fopAc_ac_c>(pl)->shape_angle.y);
                        at<bpw_class>(a)->mHitType = 8;
                        if (u8(at<BpwPlayerCutHD>(pl)->mCutType) == 0x11) {
                            at<bpw_class>(a)->mHitType = 9;
                            at<bpw_class>(a)->actor.current.angle.y =
                                (s16)(s16(at<fopAc_ac_c>(pl)->shape_angle.y) - 0x4000);
                        }
                        se_start_e(a, 0x2855, 0x42); // JA_SE_LK_HAMMER_HIT
                        at<bpw_class>(a)->mKanteraDousaRot.x = 0;
                        at<bpw_class>(a)->mKanteraDousaRot.y = 0;
                        at<bpw_class>(a)->m464 = 0x2000;
                        at<bpw_class>(a)->mActionState = 4;
                        at<bpw_class>(a)->mKanteraDousaRot.z = 0;
                    }
                }
            }
        }
        ccs_set(a, ea(&at<bpw_class>(a)->mKanteraCoSph), ea(c.get()), nullptr);
        if (u8(at<bpw_class>(a)->m3E7) == 0)
            BpwHD_mDoExt_baseAnm_play(
                u32(at<bpw_class>(a)->mpLanternGlowBrkAnm)); // mpLanternGlowBrkAnm->play()
        if (BpwHD_dCcD_GObjInf_ChkTgHit(ea(&at<bpw_class>(a)->mKanteraCoSph))) {
            u32 hit = BpwHD_dCcD_GObjInf_GetTgHitObj(ea(&at<bpw_class>(a)->mKanteraCoSph));
            u32 sfx;
            switch (u32(at<dCcD_GObjInf>(hit)->mAtType)) {
            case 2:
            case 0x400:
            case 0x800:
            case 0x4000000:
            case 0x10000000:
                sfx = 0x2803;
                break;
            case 0x40:
            case 0x80:
            case 0x2000:
            case 0x1000000:
                sfx = 0x2833;
                break;
            default:
                sfx = 0x2834;
                break;
            }
            se_start(a, sfx, 0x42);
        }
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[0]) == 0) {
            at<bpw_class>(a)->mSomeCountdownTimers[0] = (s16)ftoi(BpwHD_cM_rndF(5.f));
            f32 r = BpwHD_cM_rndF(5.f);
            at<bpw_class>(a)->m47E = (s16)ftoi(add(r, 8.f));
        }
        f32 x = f32(at<bpw_class>(a)->m358.x), y = f32(at<bpw_class>(a)->m358.y),
            z = f32(at<bpw_class>(a)->m358.z);
        at<bpw_class>(a)->actor.attention_info.position.x = x;
        at<bpw_class>(a)->actor.attention_info.position.z = z;
        at<bpw_class>(a)->actor.attention_info.position.y = add(y, 50.f); // attention_info.position
        at<bpw_class>(a)->actor.eyePos.x = x;                             // eyePos
        y = f32(at<bpw_class>(a)->m358.y);
        at<bpw_class>(a)->actor.eyePos.y = y;
        at<bpw_class>(a)->actor.eyePos.z = f32(at<bpw_class>(a)->m358.z);
        at<bpw_class>(a)->actor.eyePos.y = add(y, 50.f);
        break;
    }
    case 2: { /* damage_ball_execute */
        s16 act = s16(at<bpw_class>(a)->mBodyAction);
        if (act == 0) { /* action_b_fire_1_dousa */
            BpwHD_getGameInfo();
            using LinChk = dBgS_LinChk;
            Local<LinChk> lc;
            u32 l = ea(lc.get());
            BpwHD_cBgS_LinChk_ct(lc.get());
            at<dBgS_LinChk>(l)->mbObjThrough = 0;
            at<dBgS_LinChk>(l)->mbCamThrough = 0;
            at<dBgS_LinChk>(l)->mbLinkThrough = 0;
            at<dBgS_LinChk>(l)->mbArrowThrough = 0;
            at<dBgS_LinChk>(l)->mbBombThrough = 0;
            at<dBgS_LinChk>(l)->mbBoomerangThrough = 0;
            at<dBgS_LinChk>(l)->mbRopeThrough = 0;
            at<dBgS_LinChk>(l)->mPolyPassChk = l + 0x58;
            at<dBgS_LinChk>(l)->mGrp = 1;
            at<dBgS_LinChk>(l)->cBgS_Chk::__vtbl = 0x1000B0E4;
            at<dBgS_LinChk>(l)->mGrpPassChk = l + 0x64;
            s16 st = s16(at<bpw_class>(a)->mActionState);
            if (st == 10 || st == 11) {
                at<dBgS_LinChk>(l)->cBgS_PolyInfo::__vtbl = 0x1000B0F4;
                at<dBgS_LinChk>(l)->dBgS_PolyPassChk::__vtbl = 0x1000B114;
                at<dBgS_LinChk>(l)->dBgS_GrpPassChk::__vtbl = 0x1000B104;
                if (st == 10) {
                    for (u32 i = 0; i < 10; i++)
                        at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
                    u32 e = u32(at<bpw_class>(a)->mFire1Dousa_Pa_followEcallBack.mpEmitter.v);
                    at<bpw_class>(a)->m498 = 0.f;
                    at<bpw_class>(a)->m49C = 40.f;
                    if (!e)
                        dPa_set(0x8310, ea(&at<bpw_class>(a)->mFire1DousaPos),
                                ea(&at<bpw_class>(a)->mFire1DousaRot), 0,
                                ea(&at<bpw_class>(a)
                                        ->mFire1Dousa_Pa_followEcallBack)); // BPWFLAMETHROWER00
                    if (!u32(at<bpw_class>(a)->mFire1Dousa_Pa_followEcallBack2.mpEmitter.v))
                        dPa_set(0x8311, ea(&at<bpw_class>(a)->mFire1DousaPos),
                                ea(&at<bpw_class>(a)->mFire1DousaRot), 0,
                                ea(&at<bpw_class>(a)
                                        ->mFire1Dousa_Pa_followEcallBack2)); // BPWFLAMETHROWER01
                    at<bpw_class>(a)->mSomeCountdownTimers[0] = 5;
                    at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
                    Bpw_copyWord(&at<bpw_class>(a)->m3A0.x, &at<bpw_class>(a)->actor.current.pos.x);
                    Bpw_copyWord(&at<bpw_class>(a)->m3A0.y, &at<bpw_class>(a)->actor.current.pos.y);
                    Bpw_copyWord(&at<bpw_class>(a)->m3A0.z,
                                 &at<bpw_class>(a)->actor.current.pos.z); // m3A0 = pos
                }
                s16 ph =
                    (s16)(s16(at<bpw_class>(a)->m462) + (s16)(s16(BpwHio(6).mShort[0]) + 0x5DC));
                at<bpw_class>(a)->m462 = ph;
                at<bpw_class>(a)->m494 = (f32)(s32)(s16(BpwHio(6).mShort[1]) + 0x32); // m494
                f32 amp = add((f32)s16(BpwHio(6).mShort[2]), 2000.f);
                s16 d = (s16)ftoi(mul(Bpw_sin((u16)ph), amp));
                s16 ry = (s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + d);
                at<bpw_class>(a)->actor.shape_angle.y = ry;
                Bpw_copyWord(&at<bpw_class>(a)->m304[0].x, &at<bpw_class>(a)->actor.current.pos.x);
                Bpw_copyWord(&at<bpw_class>(a)->m304[0].y, &at<bpw_class>(a)->actor.current.pos.y);
                Bpw_copyWord(&at<bpw_class>(a)->m304[0].z,
                             &at<bpw_class>(a)->actor.current.pos.z); // m304[0] = pos
                BpwHD_mDoMtx_YrotS_2(u32(l_calcMtx), (s32)ry);
                BpwHD_mDoMtx_XrotM_2(u32(l_calcMtx),
                                     (s32)s16(at<bpw_class>(a)->actor.shape_angle.x));
                Local<BpwVecHD> v;
                v->x = 0.f;
                v->y = 0.f;
                v->z = 1000.f;
                BpwHD_MtxPosition(v.get(), ea(&at<bpw_class>(a)->m328));
                BpwHD_PSVECAdd_2(ea(&at<bpw_class>(a)->m328),
                                 ea(&at<bpw_class>(a)->actor.current.pos),
                                 ea(&at<bpw_class>(a)->m328));
                BpwHD_dBgS_LinChk_Set(lc.get(), ea(&at<bpw_class>(a)->m304),
                                      ea(&at<bpw_class>(a)->m328),
                                      a); // linChk.Set(m304, m328, actor)
                if (BpwHD_cBgS_LineCross(ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mBgS),
                                         lc.get())) {
                    u32 play = BpwHD_getGameInfo();
                    u32 pla = BpwHD_cBgS_GetTriPla(
                        play + 0x12A0, (u32)u16(at<dBgS_LinChk>(l)->mBgIndex),
                        (u32)u16(at<dBgS_LinChk>(l)
                                     ->mPolyIndex)); // GetTriPla(bg index +0x16, poly index +0x14)
                    f32 cx = f32(at<dBgS_LinChk>(l)->mLin.mEnd.x),
                        cy = f32(at<dBgS_LinChk>(l)->mLin.mEnd.y),
                        cz = f32(at<dBgS_LinChk>(l)->mLin.mEnd.z);
                    at<bpw_class>(a)->m66C.x = cx;
                    at<bpw_class>(a)->m66C.y = cy;
                    at<bpw_class>(a)->m66C.z = cz; // m66C = *GetCrossP()
                    if (pla && f32(at<BpwPlaneHD>(pla)->mNormal.y) == 1.f) {
                        at<bpw_class>(a)->m67E = 0;
                    } else {
                        f32 dx = sub(f32(at<bpw_class>(a)->m304[0].x), cx),
                            dz = sub(f32(at<bpw_class>(a)->m304[0].z), cz);
                        at<bpw_class>(a)->m67E = 0x4000;
                        at<bpw_class>(a)->m680 = BpwHD_cM_atan2s(dx, dz);
                    }
                    if (!u32(at<bpw_class>(a)->m590.mpEmitter.v))
                        dPa_set(0x8312, ea(&at<bpw_class>(a)->m66C), ea(&at<bpw_class>(a)->m67E), 0,
                                ea(&at<bpw_class>(a)->m590)); // BPWFLAMETHROWER02
                } else {
                    BpwHD_dPa_followEcallBack_end(ea(&at<bpw_class>(a)->m590)); // m590.remove()
                }
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x,
                             &at<bpw_class>(a)->actor.current.pos.x);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.y,
                             &at<bpw_class>(a)->actor.current.pos.y);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z,
                             &at<bpw_class>(a)->actor.current.pos.z); // mFire1DousaPos
                at<bpw_class>(a)->mFire1DousaRot.x = u16(at<bpw_class>(a)->actor.shape_angle.x);
                at<bpw_class>(a)->mFire1DousaRot.y = u16(at<bpw_class>(a)->actor.shape_angle.y);
                at<bpw_class>(a)->mFire1DousaRot.z = u16(at<bpw_class>(a)->actor.shape_angle.z);
                f32 mx = f32(at<bpw_class>(a)->m328[0].x), my = f32(at<bpw_class>(a)->m328[0].y),
                    mz = f32(at<bpw_class>(a)->m328[0].z);
                f32 tx = mx, ty = my, tz = mz;
                if (s16(at<bpw_class>(a)->m464) != 0) {
                    tx = f32(at<bpw_class>(a)->m304[0].x);
                    ty = f32(at<bpw_class>(a)->m304[0].y);
                    tz = f32(at<bpw_class>(a)->m304[0].z);
                }
                BpwHD_cLib_addCalc2_2(
                    ea(&at<bpw_class>(a)->m3A0), tx, 1.f,
                    mul(__builtin_fabsf(sub(f32(at<bpw_class>(a)->m304[0].x), mx)), 0.3f));
                BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->m3A0.y), ty, 1.f,
                                      mul(__builtin_fabsf(sub(f32(at<bpw_class>(a)->m304[0].y),
                                                              f32(at<bpw_class>(a)->m328[0].y))),
                                          0.3f));
                BpwHD_cLib_addCalc2_2(ea(&at<bpw_class>(a)->m3A0.z), tz, 1.f,
                                      mul(__builtin_fabsf(sub(f32(at<bpw_class>(a)->m304[0].z),
                                                              f32(at<bpw_class>(a)->m328[0].z))),
                                          0.3f));
                f32 ey = sub(f32(at<bpw_class>(a)->m3A0.y), ty),
                    ex = sub(f32(at<bpw_class>(a)->m3A0.x), tx),
                    ez = sub(f32(at<bpw_class>(a)->m3A0.z), tz);
                f32 len = BpwHD_sqrtf(fmadds(ez, ez, fmadds(ex, ex, mul(ey, ey))));
                if (len < 2.f)
                    at<bpw_class>(a)->m464 = (s16)((s16(at<bpw_class>(a)->m464) + 1) & 1);
            }
            at<dBgS_LinChk>(l)->dBgS_PolyPassChk::__vtbl = 0x1000B114;
            at<dBgS_LinChk>(l)->dBgS_GrpPassChk::__vtbl = 0x1000B0A4;
            at<dBgS_LinChk>(l)->cBgS_PolyInfo::__vtbl = 0x1000B094;
            BpwHD_cBgS_LinChk_destroy(lc.get(), 0); // ~dBgS_LinChk
            if (BpwHD_dCcD_GObjInf_ChkAtHit(
                    ea(&at<bpw_class>(a)->mDamageBallCoSph))) { // mDamageBallCoSph.ChkAtHit()
                u32 pl = player_actor();
                u32 ac = BpwHD_dCcD_GAtTgCoCommonBase_GetAc(
                    ea(&at<bpw_class>(a)->mDamageBallCoSph.mGObjAt));
                if (ac && ac == pl)
                    GOUEN_FIRE_HIT = 1; // GOUEN_FIRE_HIT
            }
            Bpw_copyWord(&cen.get()->x, &at<bpw_class>(a)->m3A0.x);
            Bpw_copyWord(&cen.get()->y, &at<bpw_class>(a)->m3A0.y);
            Bpw_copyWord(&cen.get()->z, &at<bpw_class>(a)->m3A0.z);
            ccs_set(a, ea(&at<bpw_class>(a)->mDamageBallCoSph), ea(cen.get()),
                    &at<bpw_class>(a)->m494);
        } else if (act == 1) { /* action_b_fire_2_dousa */
            u32 pl = player_actor();
            struct SVec {
                be<u16> x, y, z;
            };
            Local<SVec> l38;
            l38->x = u16(at<bpw_class>(a)->actor.shape_angle.x);
            l38->y = u16(at<bpw_class>(a)->actor.shape_angle.y);
            l38->z = u16(at<bpw_class>(a)->actor.shape_angle.z);
            switch (s16(at<bpw_class>(a)->mActionState)) {
            case 0x14: {
                at<bpw_class>(a)->actor.actor_status =
                    u32(at<bpw_class>(a)->actor.actor_status) | 0x4000;
                for (u32 i = 0; i < 10; i++)
                    at<BpwResetTimersHD>(a)->mResetTimers[i] = 0;
                u32 e = u32(at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack.mpEmitter.v);
                at<bpw_class>(a)->m498 = 0.f;
                at<bpw_class>(a)->m49C = 40.f;
                if (!e)
                    dPa_set(0x82D1, ea(&at<bpw_class>(a)->mFire1DousaPos),
                            ea(&at<bpw_class>(a)->mFire1DousaRot), 0,
                            ea(&at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack)); // BPWFIRE00
                if (!u32(at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack2.mpEmitter.v))
                    dPa_set(0x82D2, ea(&at<bpw_class>(a)->mFire1DousaPos),
                            ea(&at<bpw_class>(a)->mFire1DousaRot), 0,
                            ea(&at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack2)); // BPWFIRE01
                f32 dx = sub(f32(at<fopAc_ac_c>(pl)->current.pos.x),
                             f32(at<bpw_class>(a)->actor.current.pos.x));
                f32 dz = sub(f32(at<fopAc_ac_c>(pl)->current.pos.z),
                             f32(at<bpw_class>(a)->actor.current.pos.z));
                s16 ang = BpwHD_cM_atan2s(dx, dz);
                at<bpw_class>(a)->actor.current.angle.y = ang;
                f32 r = BpwHD_cM_rndFX(14384.f);
                ang = (s16)(ang + (s16)ftoi(r));
                at<bpw_class>(a)->actor.speedF = 15.f;
                at<bpw_class>(a)->actor.current.angle.y = ang;
                at<bpw_class>(a)->actor.shape_angle.y = ang;
                r = BpwHD_cM_rndFX(20.f);
                at<bpw_class>(a)->actor.speedF = add(15.f, r);
                at<bpw_class>(a)->actor.speed.y = 25.f;
                r = BpwHD_cM_rndFX(10.f);
                at<bpw_class>(a)->actor.shape_angle.x = -0x2000;
                at<bpw_class>(a)->actor.speed.y = add(25.f, r);
                at<bpw_class>(a)->actor.gravity = -3.f;
                at<bpw_class>(a)->m494 = (f32)(s32)(s16(BpwHio(6).mShort[3]) + 0x1E);
                at<bpw_class>(a)->mFire1DousaRot.x = u16(at<bpw_class>(a)->actor.shape_angle.x);
                at<bpw_class>(a)->mFire1DousaRot.y = u16(at<bpw_class>(a)->actor.shape_angle.y);
                at<bpw_class>(a)->mFire1DousaRot.z = u16(at<bpw_class>(a)->actor.shape_angle.z);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x,
                             &at<bpw_class>(a)->actor.current.pos.x);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.y,
                             &at<bpw_class>(a)->actor.current.pos.y);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z,
                             &at<bpw_class>(a)->actor.current.pos.z);
                at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
                break;
            }
            case 0x15: {
                BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->actor.shape_angle), 0x4000, 1,
                                          3000);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x,
                             &at<bpw_class>(a)->actor.current.pos.x);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.y,
                             &at<bpw_class>(a)->actor.current.pos.y);
                at<bpw_class>(a)->mFire1DousaRot.x = u16(at<bpw_class>(a)->actor.shape_angle.x);
                at<bpw_class>(a)->mFire1DousaRot.y = u16(at<bpw_class>(a)->actor.shape_angle.y);
                u32 fl = u32(at<bpw_class>(a)->mAcch.m_flags);
                Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z,
                             &at<bpw_class>(a)->actor.current.pos.z);
                at<bpw_class>(a)->mFire1DousaRot.z = u16(at<bpw_class>(a)->actor.shape_angle.z);
                if (fl & 0x20) {
                    se_start_e(a, 0x6A3B, 0); // JA_SE_OBJ_BPW_FIRE_EXPLODE
                    Local<BpwVecHD> p;
                    u32 pa = ea(p.get());
                    Bpw_copyWord(&at<cXyz>(pa)->x, &at<bpw_class>(a)->actor.current.pos.x);
                    Bpw_copyWord(&at<cXyz>(pa)->y, &at<bpw_class>(a)->actor.current.pos.y);
                    at<cXyz>(pa)->y = f32(at<bpw_class>(a)->mAcch.m_ground_h);
                    Bpw_copyWord(&at<cXyz>(pa)->z, &at<bpw_class>(a)->actor.current.pos.z);
                    l38->x = 0;
                    l38->z = 0;
                    dPa_set(0x82D5, pa, ea(l38.get()), 0, 0); // BPWFLOORFIRE01
                    dPa_set(0x82D6, pa, ea(l38.get()), 0, 0); // BPWFLOORFIRE02
                    dPa_set(0x82D3, pa, ea(&at<bpw_class>(a)->actor.current.angle), 0,
                            0); // BPWHITFIRE00
                    BpwHD_dPa_followEcallBack_end(
                        ea(&at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack));
                    BpwHD_dPa_followEcallBack_end(
                        ea(&at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack2));
                    if (s16(at<bpw_class>(a)->mSomeCountdownTimers[1]) == 0) {
                        at<bpw_class>(a)->mSomeCountdownTimers[1] = (s16)ftoi(BpwHD_cM_rndF(5.f));
                        f32 r = BpwHD_cM_rndF(5.f);
                        at<bpw_class>(a)->m47E = (s16)ftoi(add(r, 8.f));
                    }
                    at<bpw_class>(a)->mSomeCountdownTimers[0] = 0x5A;
                    at<bpw_class>(a)->actor.speedF = 0.f;
                    at<bpw_class>(a)->actor.gravity = 0.f;
                    at<bpw_class>(a)->actor.speed.x = 0.f;
                    at<bpw_class>(a)->actor.speed.y = 0.f;
                    at<bpw_class>(a)->actor.speed.z = 0.f;
                    at<bpw_class>(a)->m494 = (f32)(s32)(s16(BpwHio(6).mShort[4]) + 0x23);
                    at<bpw_class>(a)->mActionState = 0x16;
                } else if ((fl & 0x10) ||
                           BpwHD_dCcD_GObjInf_ChkAtHit(ea(&at<bpw_class>(a)->mDamageBallCoSph))) {
                    dPa_set(0x82D3, ea(&at<bpw_class>(a)->actor.current.pos),
                            ea(&at<bpw_class>(a)->actor.current.angle), 0, 0);
                    at<bpw_class>(a)->mActionState = 0x17;
                }
                break;
            }
            case 0x16: {
                at<bpw_class>(a)->actor.shape_angle.y =
                    (s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + 0x100);
                s16 t0 = s16(at<bpw_class>(a)->mSomeCountdownTimers[0]);
                if (t0 > 10) {
                    if (s16(at<bpw_class>(a)->mSomeCountdownTimers[1]) == 0) {
                        at<bpw_class>(a)->mSomeCountdownTimers[1] = (s16)ftoi(BpwHD_cM_rndF(5.f));
                        f32 r = BpwHD_cM_rndF(5.f);
                        at<bpw_class>(a)->m47E = (s16)ftoi(add(r, 8.f));
                        t0 = s16(at<bpw_class>(a)->mSomeCountdownTimers[0]);
                    }
                } else {
                    s16 al = s16(at<bpw_class>(a)->m47E);
                    if (al > 0) {
                        at<bpw_class>(a)->m47E = (s16)(al - 1);
                        t0 = s16(at<bpw_class>(a)->mSomeCountdownTimers[0]);
                    }
                }
                if (t0 == 0)
                    BpwHD_fopAcM_delete(a);
                else
                    se_start_e(a, 0x614F, 0); // JA_SE_OBJ_BAR_FRAME_BURN
                break;
            }
            case 0x17:
                BpwHD_dPa_followEcallBack_end(
                    ea(&at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack));
                BpwHD_dPa_followEcallBack_end(
                    ea(&at<bpw_class>(a)->mFireDousa2_Pa_followEcallBack2));
                BpwHD_fopAcM_delete(a);
                break;
            }
            Bpw_copyWord(&cen.get()->x, &at<bpw_class>(a)->actor.current.pos.x);
            Bpw_copyWord(&cen.get()->y, &at<bpw_class>(a)->actor.current.pos.y);
            Bpw_copyWord(&cen.get()->z, &at<bpw_class>(a)->actor.current.pos.z);
            Bpw_copyWord(&at<bpw_class>(a)->mLightInfluence.mPos.x,
                         &at<bpw_class>(a)->actor.current.pos.x);
            Bpw_copyWord(&at<bpw_class>(a)->mLightInfluence.mPos.y,
                         &at<bpw_class>(a)->actor.current.pos.y);
            Bpw_copyWord(&at<bpw_class>(a)->mLightInfluence.mPos.z,
                         &at<bpw_class>(a)->actor.current.pos.z); // mLightInfluence.mPos
            at<bpw_class>(a)->mLightInfluence.mColor.r = 300;
            at<bpw_class>(a)->mLightInfluence.mColor.g = 0x96;
            at<bpw_class>(a)->mLightInfluence.mColor.b = 0x14;
            at<bpw_class>(a)->mLightInfluence.mPower = 450.f;
            at<bpw_class>(a)->mLightInfluence.mFluctuation = 250.f;
            ccs_set(a, ea(&at<bpw_class>(a)->mDamageBallCoSph), ea(cen.get()),
                    &at<bpw_class>(a)->m494);
        } else {
            ccs_set(a, ea(&at<bpw_class>(a)->mDamageBallCoSph), ea(cen.get()),
                    &at<bpw_class>(a)->m494);
        }
        break;
    }
    case 3: { /* torituki_execute */
        u32 pl0 = player_actor();
        switch (s16(at<bpw_class>(a)->mActionState)) {
        case 0:
            BpwHD_mDoAud_bgmStart_2(
                u8(*at<be<u8>>(
                    ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCurStage.mName[0]))) == 0x58
                    ? 0x80000150u
                    : 0x80000140u);
            at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
            /* fallthrough */
        case 1: {
            BpwHD_cLib_addCalc2_3(ea(&at<bpw_class>(a)->actor.scale), 0.4f, 1.f, 0.1f);
            f32 s = f32(at<bpw_class>(a)->actor.scale.x);
            at<bpw_class>(a)->actor.scale.z = s;
            at<bpw_class>(a)->actor.scale.y = s;
            if (s < 0.3f)
                break;
            anm_init(i_this, dRes_INDEX_BPW_BCK_TORITUKI1_e, 15.f, 2, 1.f, -1); // TORITUKI1
            at<bpw_class>(a)->mSomeCountdownTimers[8] = 0x96;
            at<bpw_class>(a)->actor.scale.x = 0.4f;
            at<bpw_class>(a)->actor.scale.y = 0.4f;
            at<bpw_class>(a)->actor.scale.z = 0.4f;
            at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
        }
            /* fallthrough */
        case 2: {
            at<bpw_class>(a)->actor.shape_angle.y =
                (s16)(s16(at<bpw_class>(a)->actor.shape_angle.y) + 1000);
            if (s16(at<bpw_class>(a)->mSomeCountdownTimers[8]) == 0)
                break;
            u32 pl = player_actor();
            bool done = s16(at<bpw_class>(a)->mSomeCountdownTimers[8]) == 1;
            if (!done)
                done = BpwHD_dDetect_c_chk_light(
                           ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mDetect),
                           ea(&at<bpw_class>(a)->actor.current.pos)) != 0;
            if (!done)
                done = (u32(at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mPlayerStatus[0][1]) &
                        0x2000) != 0;
            if (!done)
                done = s16(at<daPy_py_c>(pl)->mDamageWaitTimer) != 0;
            if (!done)
                done = (u32(at<daPy_py_c>(pl)->mResetFlg0) & 0x02000000) != 0;
            if (!done)
                break;
            at<daPy_py_c>(pl)->mNoResetFlg1 =
                u32(at<daPy_py_c>(pl)->mNoResetFlg1) & ~0x100u; // offConfuse()
            at<bpw_class>(a)->mSomeCountdownTimers[8] = 0;
            BpwHD_mDoAud_bgmStart_2(
                u8(*at<be<u8>>(
                    ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCurStage.mName[0]))) == 0x58
                    ? 0x80000049u
                    : 0x80000048u);
            at<bpw_class>(a)->mActionState = (s16)(s16(at<bpw_class>(a)->mActionState) + 1);
            break;
        }
        case 3: {
            BpwHD_cLib_addCalc0(ea(&at<bpw_class>(a)->actor.scale), 1.f, 0.1f);
            f32 s = f32(at<bpw_class>(a)->actor.scale.x);
            at<bpw_class>(a)->actor.scale.z = s;
            at<bpw_class>(a)->actor.scale.y = s;
            if (s < 0.1f)
                BpwHD_fopAcM_delete(a);
            break;
        }
        }
        BpwHD_mDoExt_McaMorf_play(u32(at<bpw_class>(a)->mpMorf), 0, 0, 0);
        BpwHD_cLib_addCalcAngleS2(ea(&at<bpw_class>(a)->m47E), 0xFF, 1, 10);
        if (s16(at<bpw_class>(a)->mActionState) == 2) {
            f32 hx = f32(at<daPy_py_c>(pl0)->mHeadTopPos.x),
                hz = f32(at<daPy_py_c>(pl0)->mHeadTopPos.z),
                hy = f32(at<daPy_py_c>(pl0)->mHeadTopPos.y);
            at<bpw_class>(a)->actor.current.pos.x = hx;
            s16 t = (s16)(s16(at<bpw_class>(a)->m47C) + 700);
            at<bpw_class>(a)->m47C = t;
            u16 ph = u16(at<bpw_class>(a)->m47C);
            at<bpw_class>(a)->actor.current.pos.z = hz;
            f32 y = add(hy, 100.f);
            at<bpw_class>(a)->actor.current.pos.y = y;
            at<bpw_class>(a)->actor.current.pos.y = fmadds(Bpw_sin(ph), 30.f, y);
        }
        break;
    }
    }
    BpwHD_mDoMtx_YrotS_2(u32(l_calcMtx), (s32)s16(at<bpw_class>(a)->actor.current.angle.y));
    spd->x = 0.f;
    spd->y = 0.f;
    spd->z = f32(at<bpw_class>(a)->actor.speedF);
    BpwHD_MtxPosition_2(spd.get(), spdOut.get());
    f32 g = f32(at<bpw_class>(a)->actor.gravity);
    at<bpw_class>(a)->actor.speed.x = (f32)spdOut->x;
    f32 vy = add(f32(at<bpw_class>(a)->actor.speed.y), g);
    at<bpw_class>(a)->actor.speed.z = (f32)spdOut->z;
    at<bpw_class>(a)->actor.speed.y = vy;
    if (vy < -100.f)
        at<bpw_class>(a)->actor.speed.y = -100.f;
    switch (u8(at<bpw_class>(a)->mType)) {
    case 0:
        BpwHD_fopAcM_posMove(a, (u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) & 1)
                                    ? ea(&at<bpw_class>(a)->mStts)
                                    : 0u); // fopAcM_posMove(ChkCoSet ? GetCCMoveP : NULL)
        BG_check(i_this);
        draw_SUB(i_this);
        break;
    case 1: {
        if (s16(at<bpw_class>(a)->mActionState) != 1) {
            BpwHD_fopAcM_posMove_2(a, ea(&at<bpw_class>(a)->mStts));
            BG_check(i_this);
        }
        kantera_draw_SUB(i_this);
        if (u8(at<bpw_class>(a)->m3E7) != 0)
            break;
        u32 em = u32(at<bpw_class>(a)->m5CC.mpEmitter.v);
        if (!em) {
            dPa_set(0x1EA, ea(&at<bpw_class>(a)->m358), 0, ea(&at<bpw_class>(a)->actor.scale),
                    ea(&at<bpw_class>(a)->m5CC)); // ID_AK_JN_TORCH
            Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x, &at<bpw_class>(a)->m358.x);
            Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.y, &at<bpw_class>(a)->m358.y);
            Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z, &at<bpw_class>(a)->m358.z);
            em = u32(at<bpw_class>(a)->m5CC.mpEmitter.v);
        }
        if (!em)
            break;
        f32 k = add(f32(BpwHio(0).mFloat[4]), -0.03f);
        f32 vx =
            mul(sub(f32(at<bpw_class>(a)->m358.x), f32(at<bpw_class>(a)->mFire1DousaPos.x)), k);
        clamp1(vx);
        f32 vz =
            mul(sub(f32(at<bpw_class>(a)->m358.z), f32(at<bpw_class>(a)->mFire1DousaPos.z)), k);
        clamp1(vz);
        at<BpwEmitterHD>(em)->mLocalDirection.x = vx;
        at<BpwEmitterHD>(em)->mLocalDirection.y = 0.1f;
        at<BpwEmitterHD>(em)->mLocalDirection.z = vz; // setDirection
        Local<BpwVecHD> vel;
        BpwHD_cXyz___mi(ea(&at<bpw_class>(a)->actor.current.pos), vel.get(),
                        ea(&at<bpw_class>(a)->actor.old.pos)); // current.pos - old.pos
        f32 vy2 = vel->y, vx2 = vel->x, vz2 = vel->z;
        f32 len = BpwHD_sqrtf(fmadds(vz2, vz2, fmadds(vx2, vx2, mul(vy2, vy2))));
        f32 sy = fmadds(len, 0.05f, 1.f);
        if (sy > 2.f)
            sy = 2.f;
        at<BpwEmitterHD>(em)->mGlobalParticleScale.x = 1.f;
        at<BpwEmitterHD>(em)->mGlobalParticleScale.y = sy;
        at<BpwEmitterHD>(em)->mGlobalParticleScale.z = 1.f; // setGlobalParticleScale
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.x, &at<bpw_class>(a)->m358.x);
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.y, &at<bpw_class>(a)->m358.y);
        Bpw_copyWord(&at<bpw_class>(a)->mFire1DousaPos.z, &at<bpw_class>(a)->m358.z);
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<bpw_class>(a)->actor.current.roomNo));
        BpwHD_mDoAud_seStart_2(0x6103u, ea(&at<bpw_class>(a)->m358), 0u,
                               reverb); // mDoAud_seStart(JA_SE_OBJ_TORCH_BURNING, &m358)
        break;
    }
    case 2:
        BpwHD_fopAcM_posMove(a, 0u);
        BG_check(i_this);
        draw_SUB(i_this);
        break;
    case 3:
        draw_SUB(i_this);
        break;
    }
    if (u8(at<bpw_class>(a)->mType) != ACTOR_TYPE_BODY)
        return TRUE;
    kantera_calc(i_this);
    BpwHD_getGameInfo();
    u8 vib = u8(at<bpw_class>(a)->m3E8);
    switch (vib) { /* vib_mode_check */
    case 1:
    case 2: {
        u32 play = BpwHD_getGameInfo();
        Local<BpwVecHD> dir;
        dir->x = 0.f;
        dir->y = 1.f;
        dir->z = 0.f;
        BpwHD_dVibration_c_StartShock(play + 0x599C, vib == 1 ? 5 : 3, -0x21, dir.get());
        at<bpw_class>(a)->m3E8 = 0;
        break;
    }
    case 3:
        if (s16(at<bpw_class>(a)->mSomeCountdownTimers[6]) == 0) {
            BpwHD_dVibration_c_StopQuake(
                ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mVibration), -1);
            at<bpw_class>(a)->m3E8 = 0;
        }
        break;
    }
    kankyou_hendou(i_this);
    return TRUE;
}

/* ---- d_a_bpw.cpp ---- */

/* 020D57A0 */
BOOL daBPW_IsDelete(bpw_class *i_this) {
    return TRUE;
}

/* HD: remove() is the callbacks' virtual at vtable+0x44 */
/* 020D57A8 */
BOOL daBPW_Delete(bpw_class *i_this) {
    u32 a = ea(i_this);
    BpwHD_JAIZelBasic_seDeleteObject(ea(&at<bpw_class>(a)->m358)); // mDoAud_seDeleteObject(&m358)
    BpwHD_JAIZelBasic_seDeleteObject(ea(&at<bpw_class>(a)->m3C4)); // mDoAud_seDeleteObject(&m3C4)
    BpwHD_dComIfG_resDelete(ea(&at<bpw_class>(a)->mPhase),
                            0x1000B228u); // dComIfG_resDeleteDemo(&mPhase, "BPW")
    auto remove = [&](u32 o) { at<dPa_followEcallBack>(a + o)->remove(); };
    if (u8(at<bpw_class>(a)->mType) == ACTOR_TYPE_KANTERA)
        remove(0x6E8); // m5CC
    for (u32 i = 0; i < 4; i++)
        remove(0x6FC + i * 0x20); // m5E0[i]
    remove(0x684);
    remove(0x698);
    remove(0x6AC);
    remove(0x670);
    remove(0x6C0);
    remove(0x6D4);
    remove(0x65C);
    remove(0x648);
    remove(0x628);
    if (u8(at<bpw_class>(a)->mUnknownParam2) == 1)
        BpwHD_dKy_plight_cut(
            ea(&at<bpw_class>(a)->mLightInfluence)); // dKy_plight_cut(&mLightInfluence)
    if (s16(at<bpw_class>(a)->mSomeCountdownTimers[8]) !=
        0) { // mSomeCountdownTimers[8]: player->offConfuse()
        u32 player = player_actor();
        at<daPy_py_c>(player)->mNoResetFlg1 = u32(at<daPy_py_c>(player)->mNoResetFlg1) & ~0x100u;
    }
    return TRUE;
}

/* 020D5930 */
BOOL boss_useHeapInit(bpw_class *i_this) {
    u32 a = ea(i_this);
    constexpr u32 str = 0x1000B22C;
    u32 md = getObjectRes(str, dRes_INDEX_BPW_BDL_BPW_e);
    u32 bck = getObjectRes(str, dRes_INDEX_BPW_BCK_WAIT1_e);
    u32 morf = new_McaMorf(md, bck, 2);
    at<bpw_class>(a)->mpMorf = morf;
    if (!morf || !u32(at<mDoExt_McaMorf>(morf)->mpModel.v))
        return FALSE;
    at<BpwModelHD>(u32(at<mDoExt_McaMorf>(morf)->mpModel.v))->mUserArea = a; // setUserArea(this)
    set_joint_callbacks(a, 0x020D0C04u);                                     // body_nodeCallBack
    u32 model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    const struct {
        be<u32> bpw_class::*field;
        u32 res, mode;
    } brks[] = {{&bpw_class::mpLightFreezeBrkAnm, dRes_INDEX_BPW_BRK_HIRARU1_e, 0},
                {&bpw_class::mpLightStunBrkAnm, dRes_INDEX_BPW_BRK_HIT_e, 2},
                {&bpw_class::mpCurseStartBrkAnm, dRes_INDEX_BPW_BRK_NOROI_S1_e, 0},
                {&bpw_class::mpCurseEndBrkAnm, dRes_INDEX_BPW_BRK_NOROI_E1_e, 0},
                {&bpw_class::mpDefaultBrkAnm, dRes_INDEX_BPW_BRK_DEFAULT_e, 2}};
    for (auto &b : brks) {
        u32 p = new_brkAnm();
        at<bpw_class>(a)->*b.field = p;
        if (!p)
            return FALSE;
        u32 res = getObjectRes(str, b.res);
        u32 brk = u32(at<bpw_class>(a)->*b.field), mdata = u32(at<BpwModelHD>(model)->mpModelData);
        if (!BpwHD_mDoExt_brkAnm_initWithFlags_2(brk, mdata, res, 1, b.mode, 1.f, 0, -1, 0, 0))
            return FALSE;
    }
    u32 m = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    if (!BpwHD_mDoExt_invisibleModel_create(ea(&at<bpw_class>(a)->mInvisibleModel), m))
        return FALSE; // mInvisibleModel.create(model)
    return TRUE;
}

/* 020D5D04 */
BOOL kantera_useHeapInit(bpw_class *i_this) {
    u32 a = ea(i_this);
    constexpr u32 str = 0x1000B230;
    u32 md = getObjectRes(str, dRes_INDEX_BPW_BDL_BPW_KAN1_e);
    u32 morf = new_McaMorf(md, 0, 1);
    at<bpw_class>(a)->mpMorf = morf;
    if (!morf || !u32(at<mDoExt_McaMorf>(morf)->mpModel.v))
        return FALSE;
    at<BpwModelHD>(u32(at<mDoExt_McaMorf>(morf)->mpModel.v))->mUserArea = a;
    set_joint_callbacks(a, 0x020D0DF0u); // kantera_nodeCallBack
    u32 model = u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
    u32 p = new_brkAnm();
    at<bpw_class>(a)->mpLanternGlowBrkAnm = p; // mpLanternGlowBrkAnm
    if (!p)
        return FALSE;
    u32 res = getObjectRes(str, dRes_INDEX_BPW_BRK_BPW_KAN1_e);
    u32 brk = u32(at<bpw_class>(a)->mpLanternGlowBrkAnm),
        mdata = u32(at<BpwModelHD>(model)->mpModelData);
    if (!BpwHD_mDoExt_brkAnm_initWithFlags_3(brk, mdata, res, 1, 2, 1.f, 0, -1, 0, 0))
        return FALSE;
    return TRUE;
}

/* 020D5EF0 */
BOOL fire_useHeapInit(bpw_class *i_this) {
    u32 a = ea(i_this);
    u32 md = getObjectRes(0x1000B234, dRes_INDEX_BPW_BDL_BPW_KAN1_e);
    u32 morf = new_McaMorf(md, 0, 1);
    at<bpw_class>(a)->mpMorf = morf;
    if (!morf || !u32(at<mDoExt_McaMorf>(morf)->mpModel.v))
        return FALSE;
    return TRUE;
}

/* 020D5FC0 */
BOOL tori_useHeapInit(bpw_class *i_this) {
    u32 a = ea(i_this);
    constexpr u32 str = 0x1000B238;
    u32 md = getObjectRes(str, dRes_INDEX_BPW_BDL_BPW_e);
    u32 bck = getObjectRes(str, dRes_INDEX_BPW_BCK_TORITUKI1_e);
    u32 morf = new_McaMorf(md, bck, 1);
    at<bpw_class>(a)->mpMorf = morf;
    if (!morf)
        return FALSE;
    u32 model = u32(at<mDoExt_McaMorf>(morf)->mpModel.v);
    if (!model)
        return FALSE;
    if (!BpwHD_mDoExt_invisibleModel_create(ea(&at<bpw_class>(a)->mInvisibleModel), model))
        return FALSE;
    return TRUE;
}

/* : inlines fopAcM_ct, body/kantera/damage_ball/tori_create_init.
 * HD: no shadow id; the start-stage check reads the play object's stage name. */
/* 020D6268 */
s32 daBPW_Create(bpw_class *i_this) {
    u32 a = ea(i_this);
    struct SVec {
        be<u16> x, y, z;
    };
    Local<SVec> sp18;
    sp18->y = u16(at<bpw_class>(a)->actor.shape_angle.y);
    u32 cond = u32(at<bpw_class>(a)->actor.actor_condition);
    sp18->z = u16(at<bpw_class>(a)->actor.shape_angle.z);
    sp18->x = u16(at<bpw_class>(a)->actor.shape_angle.x);
    if (!(cond & 8)) { // fopAcM_ct
        if (a) {
            bpw_class_ct(a);
            cond = u32(at<bpw_class>(a)->actor.actor_condition);
        }
        at<bpw_class>(a)->actor.actor_condition = cond | 8;
    }
    s32 res = BpwHD_dComIfG_resLoad(ea(&at<bpw_class>(a)->mPhase),
                                    0x1000B250u); // dComIfG_resLoad(&mPhase, "BPW")
    if (res != 4)
        return res;
    u32 p = u32(at<bpw_class>(a)->actor.mParameters);
    at<bpw_class>(a)->mType = (u8)p;
    u8 p2 = (u8)(u32(at<bpw_class>(a)->actor.mParameters) >> 8);
    at<bpw_class>(a)->mUnknownParam2 = p2;
    at<bpw_class>(a)->mLightState = (u8)(u32(at<bpw_class>(a)->actor.mParameters) >> 16);
    u8 type = (u8)p;
    if (type == 0xFF) {
        at<bpw_class>(a)->mType = 0;
        type = 0;
    }
    if (p2 == 0xFF) {
        type = u8(at<bpw_class>(a)->mType);
        at<bpw_class>(a)->mUnknownParam2 = 0;
    }
    switch (type) {
    case 0: { // body
        if (BpwHD_dSv_memBit_c_isDungeonItem(u32(l_saveInfo) + 0x798, 3))
            return 5; // dComIfGs_isStageBossEnemy()
        if (!BpwHD_fopAcM_entrySolidHeap(a, 0x020D5930u, 0x36E0u))
            return 5;
        BpwHD_getGameInfo();
        at<bpw_class>(a)->actor.max_health = 15;
        at<bpw_class>(a)->actor.health = 15;
        if (s16(BpwHio(0).mShort[1]) != 0) {
            at<bpw_class>(a)->actor.max_health = 1;
            at<bpw_class>(a)->actor.health = 1;
            at<bpw_class>(a)->m3F6 = 2;
        }
        Bpw_copyWord(&at<bpw_class>(a)->mBodyPos.x, &at<bpw_class>(a)->actor.current.pos.x);
        Bpw_copyWord(&at<bpw_class>(a)->mBodyPos.y, &at<bpw_class>(a)->actor.current.pos.y);
        Bpw_copyWord(&at<bpw_class>(a)->mBodyPos.z,
                     &at<bpw_class>(a)->actor.current.pos.z); // mBodyPos
        set_mtx_and_cull(a, -300.f, 300.f);
        at<bpw_class>(a)->actor.attention_info.distances[2] = 4;
        at<bpw_class>(a)->actor.attention_info.flags = 4;
        at<bpw_class>(a)->actor.actor_status =
            u32(at<bpw_class>(a)->actor.actor_status) | 0x4000000; // fopAcStts_BOSS_e
        acch_set(a);
        BpwHD_dCcD_Stts_Init(ea(&at<bpw_class>(a)->mStts), 0xFE, 1,
                             a); // mStts.Init(0xfe, 1, actor)
        at<bpw_class>(a)->actor.attention_info.distances[4] = 0x25;
        at<bpw_class>(a)->actor.actor_status = u32(at<bpw_class>(a)->actor.actor_status) | 0x10000;
        BpwHD_dCcD_Sph_Set(ea(&at<bpw_class>(a)->mBodyCoSph),
                           0x10192558u); // mBodyCoSph.Set(body_co_sph_src)
        at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm =
            u32(at<bpw_class>(a)->mBodyCoSph.mObjCo.mSPrm) & ~1u;
        at<bpw_class>(a)->mBodyCoSph.mStts = &at<bpw_class>(a)->mStts;
        BpwHD_dCcD_Sph_Set(ea(&at<bpw_class>(a)->mBodyAtSph),
                           0x10192598u); // mBodyAtSph.Set(body_at_sph_src)
        at<bpw_class>(a)->mBodyAtSph.mStts = &at<bpw_class>(a)->mStts;
        at<bpw_class>(a)->m476 = s16(at<bpw_class>(a)->actor.current.angle.y);
        at<bpw_class>(a)->actor.model =
            u32(at<mDoExt_McaMorf>(u32(at<bpw_class>(a)->mpMorf))->mpModel.v);
        at<bpw_class>(a)->m4A4 = 300.f;
        at<bpw_class>(a)->m4A0 = 0.f;
        for (u32 i = 0; i < 4; i++)
            at<bpw_class>(a)->m5E0[i].m11 = 0; // m5E0[i].setRateOff(0)
        at<bpw_class>(a)->mAttWaitTimer = 1;
        at<bpw_class>(a)->m3AC.y = 340.f;
        at<bpw_class>(a)->m4A8 = add(f32(BpwHio(8).mFloat[16]), 220.f);
        Bpw_copyWord(&at<bpw_class>(a)->mChildActorPos.x, &at<bpw_class>(a)->actor.current.pos.x);
        Bpw_copyWord(&at<bpw_class>(a)->mChildActorPos.y, &at<bpw_class>(a)->actor.current.pos.y);
        Bpw_copyWord(&at<bpw_class>(a)->mChildActorPos.z,
                     &at<bpw_class>(a)->actor.current.pos.z); // mChildActorPos
        for (u32 i = 0; i < 3; i++) {
            Bpw_copyWord(&at<bpw_class>(a)->m304[i].x, &at<bpw_class>(a)->actor.current.pos.x);
            Bpw_copyWord(&at<bpw_class>(a)->m304[i].y, &at<bpw_class>(a)->actor.current.pos.y);
            Bpw_copyWord(&at<bpw_class>(a)->m304[i].z, &at<bpw_class>(a)->actor.current.pos.z);
            Bpw_copyWord(&at<bpw_class>(a)->m328[i].x, &at<bpw_class>(a)->actor.current.pos.x);
            Bpw_copyWord(&at<bpw_class>(a)->m328[i].y, &at<bpw_class>(a)->actor.current.pos.y);
            Bpw_copyWord(&at<bpw_class>(a)->m328[i].z, &at<bpw_class>(a)->actor.current.pos.z);
        }
        at<bpw_class>(a)->m498 = 350.f;
        at<bpw_class>(a)->m49C = 400.f;
        BpwHD_mDoExt_McaMorf_play(u32(at<bpw_class>(a)->mpMorf), 0, 0,
                                  0); // mpMorf->play(NULL, 0, 0)
        BG_check(i_this);
        draw_SUB(i_this);
        sp18->y = (u16)(s16(sp18->y) + 0x4000);
        s32 room = s8(at<bpw_class>(a)->actor.current.roomNo);
        u32 kid =
            BpwHD_fopAcM_createChild(0xD3, actor_id(a), 1, ea(&at<bpw_class>(a)->mChildActorPos),
                                     room, sp18.get(), 0, -1, 0);
        at<bpw_class>(a)->m3FC = kid;
        at<bpw_class>(a)->mAttWaitTimer = 1;
        kantera_calc(i_this);
        if (start_demo_check()) {
            s32 hp = s8(at<bpw_class>(a)->actor.health);
            Local<SVec> ang;
            ang->x = u16(at<bpw_class>(a)->actor.shape_angle.x);
            f32 q = 65536.f / (f32)hp;
            ang->y = u16(at<bpw_class>(a)->actor.shape_angle.y);
            ang->z = u16(at<bpw_class>(a)->actor.shape_angle.z);
            at<bpw_class>(a)->actor.actor_status =
                u32(at<bpw_class>(a)->actor.actor_status) | 0x4000;
            Local<BpwVecHD> pos;
            f32 ground = f32(at<bpw_class>(a)->mAcch.m_ground_h);
            pos->x = f32(at<bpw_class>(a)->actor.current.pos.x);
            s16 spread = (s16)ftoi(q);
            pos->z = f32(at<bpw_class>(a)->actor.current.pos.z);
            pos->y = add(ground, 20.f);
            s16 cur = (s16)ftoi(BpwHD_cM_rndFX(32768.f));
            s16 kind = (s16)ftoi(BpwHD_cM_rndF(4.99f));
            f32 jitter = mul((f32)spread, 0.25f);
            u32 prm = ((u32)kind << 9) | 0xFF000004u;
            u32 k = (u32)(s32)kind;
            for (u32 i = 0; i < 15; i++) {
                s32 r = s8(at<bpw_class>(a)->actor.current.roomNo);
                u32 id = BpwHD_fopAcM_createChild_2(0xD4, actor_id(a), prm, pos.get(), r, ang.get(),
                                                    0, -1, 0);
                cur = (s16)(cur + spread);
                at<bpw_class>(a)->mChildPoeIds[i] = id; // mChildPoeIds[i]
                ang->y = (u16)cur;
                f32 j = BpwHD_cM_rndFX_2(jitter);
                ang->y = (u16)(s16)(cur + (s16)ftoi(j));
                k++;
                if (k > 5)
                    k = 0;
                prm = (k << 9) | 0xFF000004u;
            }
            at<bpw_class>(a)->m3F6 = 3;
            light_on_off(i_this);
            at<bpw_class>(a)->mKankyouHendouState = 3;
            at<bpw_class>(a)->mSomeCountdownTimers[5] = 0;
            at<bpw_class>(a)->m3EC = 1.f;
            kankyou_hendou(i_this);
            at<bpw_class>(a)->mBodyAction = ACTION_START_DEMO;
            at<bpw_class>(a)->mActionState = 200;
        } else {
            u32 bgm =
                u8(*at<be<u8>>(
                    ea(&at<dComIfG_inf_c>(BpwHD_getGameInfo())->play.mCurStage.mName[0]))) == 0x58
                    ? 0x80000049u
                    : 0x80000048u;
            BpwHD_mDoAud_bgmStart(bgm); // mDoAud_bgmStart(PAST_BIG_POW / BIG_POW)
            kankyou_hendou(i_this);
            at<bpw_class>(a)->mBodyAction = ACTION_MOVE_DOUSA;
            at<bpw_class>(a)->mActionState = 0;
        }
        break;
    }
    case 1: { // kantera
        if (!BpwHD_fopAcM_entrySolidHeap(a, 0x020D5D04u, 0x14C0u))
            return 5;
        BpwHD_getGameInfo();
        set_mtx_and_cull(a, -100.f, 100.f);
        acch_set(a);
        BpwHD_dCcD_Stts_Init(ea(&at<bpw_class>(a)->mStts), 100, 1, a);
        BpwHD_dCcD_Sph_Set(ea(&at<bpw_class>(a)->mKanteraCoSph),
                           0x101925D8u); // mKanteraCoSph.Set(kantera_co_sph_src)
        u32 parent = u32(at<bpw_class>(a)->actor.parentActorID);
        at<bpw_class>(a)->mBodyAction = ACTION_MOVE_DOUSA;
        at<bpw_class>(a)->mKanteraCoSph.mObjCo.mSPrm =
            u32(at<bpw_class>(a)->mKanteraCoSph.mObjCo.mSPrm) & ~1u;
        at<bpw_class>(a)->mKanteraCoSph.mStts = &at<bpw_class>(a)->mStts;
        at<bpw_class>(a)->m404 = parent; // m404
        at<bpw_class>(a)->mActionState = 0;
        if (parent == 0xFFFFFFFF)
            BpwHD_fopAcM_delete(a);
        at<bpw_class>(a)->actor.actor_status = u32(at<bpw_class>(a)->actor.actor_status) | 0x4000;
        if (start_demo_check()) {
            at<bpw_class>(a)->m3E7 = 1;
            anm_init(i_this, dRes_INDEX_BPW_BCK_OPENING_KAN1_e, f32(BpwHio(18).mFloat[4]), 0, 0.f,
                     -1); // REG18_F(4)
        }
        at<bpw_class>(a)->m498 = 0.f;
        at<bpw_class>(a)->m49C = 200.f;
        BG_check(i_this);
        kantera_draw_SUB(i_this);
        break;
    }
    case 2: { // damage ball
        if (!BpwHD_fopAcM_entrySolidHeap(a, 0x020D5EF0u, 0x14C0u))
            return 5;
        Bpw_copyWord(&at<bpw_class>(a)->m358.x, &at<bpw_class>(a)->actor.current.pos.x);
        Bpw_copyWord(&at<bpw_class>(a)->m358.y, &at<bpw_class>(a)->actor.current.pos.y);
        Bpw_copyWord(&at<bpw_class>(a)->m358.z, &at<bpw_class>(a)->actor.current.pos.z); // m358
        BpwHD_getGameInfo();
        set_mtx_and_cull(a, -100.f, 100.f);
        acch_set(a);
        BpwHD_dCcD_Stts_Init(ea(&at<bpw_class>(a)->mStts), 0xFE, 1, a);
        BpwHD_dCcD_Sph_Set(ea(&at<bpw_class>(a)->mDamageBallCoSph),
                           0x10192618u); // mDamageBallCoSph.Set(damage_ball_co_sph_src)
        u8 p2b = u8(at<bpw_class>(a)->mUnknownParam2);
        at<bpw_class>(a)->mDamageBallCoSph.mStts = &at<bpw_class>(a)->mStts;
        if (p2b == 0) {
            at<bpw_class>(a)->mDamageBallCoSph.mGObjAt.mSpl = 9;
            at<bpw_class>(a)->mBodyAction = ACTION_MOVE_DOUSA;
            at<bpw_class>(a)->mActionState = 10;
            at<bpw_class>(a)->mDamageBallCoSph.mAtp = 6;
        } else if (p2b == 1) {
            BpwHD_dKy_plight_set(
                ea(&at<bpw_class>(a)->mLightInfluence)); // dKy_plight_set(&mLightInfluence)
            at<bpw_class>(a)->mDamageBallCoSph.mGObjAt.mSpl = 8;
            at<bpw_class>(a)->mBodyAction = ACTION_ATTACK_KOUGEKI;
            at<bpw_class>(a)->mActionState = 0x14;
        }
        BG_check(i_this);
        draw_SUB(i_this);
        break;
    }
    case 3: { // torituki
        if (!BpwHD_fopAcM_entrySolidHeap(a, 0x020D5FC0u, 0x10000u))
            return 5;
        at<bpw_class>(a)->m3E0 = 1;
        at<bpw_class>(a)->actor.scale.x = 0.f;
        at<bpw_class>(a)->actor.scale.y = 0.f;
        at<bpw_class>(a)->actor.scale.z = 0.f;
        u32 player = player_actor();
        Bpw_copyWord(&at<bpw_class>(a)->mBodyPos.x, &at<bpw_class>(a)->actor.current.pos.x);
        Bpw_copyWord(&at<bpw_class>(a)->mBodyPos.y, &at<bpw_class>(a)->actor.current.pos.y);
        Bpw_copyWord(&at<bpw_class>(a)->mBodyPos.z, &at<bpw_class>(a)->actor.current.pos.z);
        at<bpw_class>(a)->actor.actor_status = u32(at<bpw_class>(a)->actor.actor_status) | 0x4000;
        set_mtx_and_cull(a, -300.f, 300.f);
        at<bpw_class>(a)->actor.attention_info.flags = 0;
        at<bpw_class>(a)->actor.current.pos.y = f32(at<daPy_py_c>(player)->mHeadTopPos.y);
        at<bpw_class>(a)->actor.current.pos.x = f32(at<daPy_py_c>(player)->mHeadTopPos.x);
        at<bpw_class>(a)->actor.current.pos.z =
            f32(at<daPy_py_c>(player)->mHeadTopPos.z); // getHeadTopPos()
        BpwHD_dCcD_Stts_Init(ea(&at<bpw_class>(a)->mStts), 0, 1, a);
        draw_SUB(i_this);
        break;
    }
    }
    return res;
}

// HD: compiler-generated and template code of this translation unit.

/* ---- compiler-generated / out-of-line inline helpers of this TU ---- */

/* array element constructor of m5E0 (dPa_smokeEcallBack(1)) */
/* 020D60C0 */
u32 bpw_smoke_ctor(u32 object) {
    return BpwHD_dPa_smokeEcallBack_dPa_smokeEcallBack(object, 1);
}

/* / : deleting destructors of empty-destructor classes */
/* 020D6D84 */
void BpwHD_deleteEmptyObject(u32 object, u32 flags) {
    if (object && (flags & 1))
        BpwHD___dl(object);
}

/* 020DEEAC */
void bpw_smoke_dtor(u32 object, u32 flags) {
    if (object && (flags & 1))
        BpwHD___dl(object);
}

/* empty virtual */
/* 020DEFAC */
void bpw_empty_virtual(u32 object) {}

/* GXColor -> four floats (/255) */
/* 020D1280 */
void bpw_color_to_float(u32 output, u32 color) {
    f32 r = (f32)u8(at<GXColor>(color)->r), g = (f32)u8(at<GXColor>(color)->g);
    f32 b = (f32)u8(at<GXColor>(color)->b), a = (f32)u8(at<GXColor>(color)->a);
    at<BpwColorF32>(output)->r = r / 255.f;
    at<BpwColorF32>(output)->g = g / 255.f;
    at<BpwColorF32>(output)->b = b / 255.f;
    at<BpwColorF32>(output)->a = a / 255.f;
}

/* BpwHD_seStart(actor, sfx, param) (HD: eyePos null check) */
/* 020D6D18 */
void BpwHD_seStart(u32 actor, u32 sfx, u32 param) {
    if (actor && actor + 0x37C) {
        s32 reverb = BpwHD_dComIfGp_getReverb((s32)s8(at<fopAc_ac_c>(actor)->current.roomNo));
        BpwHD_mDoAud_seStart(sfx, actor + 0x37C, param, reverb);
    }
}

/* __sinit_d_a_bpw_cpp */
/* 020D6C84 */
void bpw_sinit() {
    l_headerState.mZeroWords[2] = 0;
    l_headerState.mZeroWords[0] = 0;
    l_headerState.mZeroWords[3] = 0;
    l_headerState.mZeroWords[1] = 0;
    BpwHD___register_global_object(0x1019266Cu);
    f32 low = l_headerConstants[0], high = l_headerConstants[1];
    l_headerState.mLow = low;
    l_headerState.mHigh = high;
    BpwHD_firstHeaderObject_construct(ea(&l_headerState.mFirstObject));
    BpwHD___register_global_object(0x10192678u);
    BpwHD_secondHeaderObject_construct(ea(&l_headerState.mSecondObject));
    BpwHD___register_global_object(0x10192684u);
}

/* bpw_class::bpw_class() (allocates when this == NULL) */
/* 020D60C8 */
u32 bpw_class_ct(u32 a) {
    if (!a) {
        a = BpwHD_operator_new(0xE9Cu);
        if (!a)
            return 0;
    }
    BpwHD_fopAc_ac_c_fopAc_ac_c(a);
    at<bpw_class>(a)->mLightInfluence.mHD20 = 1.f;
    at<bpw_class>(a)->actor.__vtbl = l_actorVtable;
    BpwHD_dPa_smokeEcallBack_dPa_smokeEcallBack_2(ea(&at<bpw_class>(a)->m50C), 1);
    for (u32 o = 0x648; o <= 0x6E8; o += 0x14)
        BpwHD_dPa_followEcallBack_dPa_followEcallBack(a + o, 0, 0);
    BpwHD___construct_array(ea(&at<bpw_class>(a)->m5E0), 4, 0x20, 0x020D60C0u);
    BpwHD_dBgS_AcchCir_dBgS_AcchCir(ea(&at<bpw_class>(a)->mAcchCir));
    BpwHD_dBgS_Acch_dBgS_Acch(ea(&at<bpw_class>(a)->mAcch));
    at<bpw_class>(a)->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    at<bpw_class>(a)->mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    at<bpw_class>(a)->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    at<bpw_class>(a)->mAcch.mbObjThrough = 1;
    BpwHD_dCcD_Stts_construct(ea(&at<bpw_class>(a)->mStts));
    BpwHD_dCcD_GStts_dCcD_GStts(ea(&at<bpw_class>(a)->mStts.__vtbl_gstts));
    at<bpw_class>(a)->mStts.__vtbl = 0x1004AE88;
    at<bpw_class>(a)->mStts.__vtbl_gstts = 0x1004AEC0;
    BpwHD_dCcD_Sph_construct(ea(&at<bpw_class>(a)->mBodyCoSph));
    BpwHD_dCcD_Sph_construct(ea(&at<bpw_class>(a)->mBodyAtSph));
    BpwHD_dCcD_Sph_construct(ea(&at<bpw_class>(a)->mKanteraCoSph));
    BpwHD_dCcD_Sph_construct(ea(&at<bpw_class>(a)->mDamageBallCoSph));
    BpwHD_mDoExt_invisibleModel_construct(ea(&at<bpw_class>(a)->mInvisibleModel));
    return a;
}

/* bpw_class::~bpw_class() (deleting) */
/* 020DEEC0 */
void bpw_class_dt(u32 a, u32 flags) {
    if (!a)
        return;
    BpwHD_mDoExt_invisibleModel_destroy(ea(&at<bpw_class>(a)->mInvisibleModel), 2);
    BpwHD_dCcD_Sph_destroy(ea(&at<bpw_class>(a)->mDamageBallCoSph), 2);
    BpwHD_dCcD_Sph_destroy(ea(&at<bpw_class>(a)->mKanteraCoSph), 2);
    BpwHD_dCcD_Sph_destroy(ea(&at<bpw_class>(a)->mBodyAtSph), 2);
    BpwHD_dCcD_Sph_destroy(ea(&at<bpw_class>(a)->mBodyCoSph), 2);
    BpwHD_dCcD_Stts_destroy(ea(&at<bpw_class>(a)->mStts), 2);
    at<bpw_class>(a)->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    at<bpw_class>(a)->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    BpwHD_dBgS_Acch_destroy(ea(&at<bpw_class>(a)->mAcch), 0);
    BpwHD_cM3dGPla_destroy(ea(&at<bpw_class>(a)->mAcchCir.m_cir), 2);
    BpwHD___destroy_arr(ea(&at<bpw_class>(a)->m5E0), 4, 0x20, 0x020DEEACu, 0, 0);
    BpwHD_fopAc_ac_c_destroy_fopAc_ac_c(a, 0);
    if (flags & 1)
        BpwHD___dl(a);
}

// clang-format off
/* WWHD glue
data GOUEN_FIRE_HIT 10192534
data check_info 1046277C
data get_check_count 1046275C
data light_on_dt 10192658
data l_turnLimit 1000B458
data l_healthThreshold 1000B3BC
ext 025200D4 unsigned int BpwHD_dComIfGp_getPlay()
ext 026066C4 unsigned int BpwHD_dRes_control_c_getRes(unsigned int, void *, u32)
ext 025A5B18 unsigned int BpwHD_dPa_smokeEcallBack_dPa_smokeEcallBack(u32, int)
ext 0273AF40 void BpwHD___dl(u32)
ext 02520540 int BpwHD_dComIfGp_getReverb(s32)
ext 025E1A40 void BpwHD_mDoAud_seStart(u32, u32, u32, s32)
ext 028F026C void BpwHD___register_global_object(unsigned int)
ext 028ED6F8 void BpwHD_firstHeaderObject_construct(unsigned int)
ext 028EAB2C void BpwHD_secondHeaderObject_construct(unsigned int)
ext 0273AD10 unsigned int BpwHD_operator_new(unsigned int)
ext 025D4ED0 void BpwHD_fopAc_ac_c_fopAc_ac_c(u32)
ext 025A5B18 void BpwHD_dPa_smokeEcallBack_dPa_smokeEcallBack_2(u32, int)
ext 025A5894 void BpwHD_dPa_followEcallBack_dPa_followEcallBack(u32, int, int)
ext 028EFFD0 void BpwHD___construct_array(u32, int, int, unsigned int)
ext 024EFE94 void BpwHD_dBgS_AcchCir_dBgS_AcchCir(u32)
ext 024F0474 void BpwHD_dBgS_Acch_dBgS_Acch(u32)
ext 0200BD2C void BpwHD_dCcD_Stts_construct(u32)
ext 02515DA0 void BpwHD_dCcD_GStts_dCcD_GStts(u32)
ext 025166F0 void BpwHD_dCcD_Sph_construct(u32)
ext 025E895C void BpwHD_mDoExt_invisibleModel_construct(u32)
ext 025E89F8 void BpwHD_mDoExt_invisibleModel_destroy(u32, int)
ext 02515AE8 void BpwHD_dCcD_Sph_destroy(u32, int)
ext 02515860 void BpwHD_dCcD_Stts_destroy(u32, int)
ext 024EFD9C void BpwHD_dBgS_Acch_destroy(u32, int)
ext 02018034 void BpwHD_cM3dGPla_destroy(u32, int)
ext 028F0164 void BpwHD___destroy_arr(u32, int, int, unsigned int, int, int)
ext 025D50BC void BpwHD_fopAc_ac_c_destroy_fopAc_ac_c(u32, int)
ext 025E1B34 void BpwHD_JAIZelBasic_seDeleteObject(u32)
ext 025204C8 void BpwHD_dComIfG_resDelete(u32, unsigned int)
ext 0255A374 void BpwHD_dKy_plight_cut(u32)
ext 028E93CC void BpwHD_PSMTXTrans(u32, f32, f32, f32)
ext 025F1C28 void BpwHD_mDoMtx_YrotM(u32, s32)
ext 025F1BF4 void BpwHD_mDoMtx_XrotM(u32, s32)
ext 025F1C5C void BpwHD_mDoMtx_ZrotM(u32, s32)
ext 025E55A0 void BpwHD_mDoExt_McaMorf_calc(unsigned int)
ext 02555D0C unsigned int BpwHD_dKy_getEnvlight()
ext 025626A4 void BpwHD_dScnKy_env_light_c_settingTevStruct(u32, int, u32, u32)
ext 0200FAD8 void BpwHD_MtxTrans(float, float, float, int)
ext 025F1C28 void BpwHD_mDoMtx_YrotM_2(unsigned int, s32)
ext 025F1BF4 void BpwHD_mDoMtx_XrotM_2(unsigned int, s32)
ext 025F1C5C void BpwHD_mDoMtx_ZrotM_2(unsigned int, s32)
ext 025E4A98 void BpwHD_mDoExt_McaMorf_setAnm(unsigned int, u32, u32, f32, f32, float, float, u32)
ext 025E4A98 void BpwHD_mDoExt_McaMorf_setAnm_2(unsigned int, u32, u32, f32, f32, float, float, unsigned int)
ext 024EFF44 void BpwHD_dBgS_AcchCir_SetWall(u32, float, float)
ext 024F08A8 void BpwHD_dBgS_Acch_CrrPos(u32, u32)
ext 0252A038 int BpwHD_dDetect_c_chk_light(u32, u32)
ext 0200F428 void BpwHD_cLib_addCalcAngleS2(u32, int, int, int)
ext 028E90D4 void BpwHD_PSMTXCopy(unsigned int, unsigned int)
ext 028E90D4 void BpwHD_PSMTXCopy_2(u32, unsigned int)
ext 027F7878 unsigned int BpwHD_J3DNode_toJoint(u32)
ext 0200FCD8 void BpwHD_MtxPosition(void *, u32)
ext 0200ED84 void BpwHD_cLib_addCalc2(u32, f32, float, float)
ext 0255FDA4 void BpwHD_dKy_custom_colset(u32, u32, f32)
ext 025D68EC float BpwHD_fopAcM_searchActorDistance(u32, unsigned int)
ext 025160DC int BpwHD_dCcD_GObjInf_ChkAtHit(u32)
ext 02515BBC unsigned int BpwHD_dCcD_GAtTgCoCommonBase_GetAc(u32)
ext 0251621C void BpwHD_dCcD_GObjInf_ClrTgHit(u32)
ext 025A847C void BpwHD_dPa_control_c_set(unsigned int, int, int, u32, int, int, int, int, int, int, int, int)
ext 025D5834 void BpwHD_fopAcM_create(int, int, u32, s32, void *, int, int, int)
ext 025D5218 unsigned int BpwHD_fopAcIt_Judge(unsigned int, gabi::be<unsigned int> *)
ext 025D57E0 void BpwHD_fopAcM_delete(u32)
ext 025A5AC8 void BpwHD_dPa_followEcallBack_end(u32)
ext 025A5F88 void BpwHD_dPa_smokeEcallBack_end(u32)
ext 025E8154 void BpwHD_mDoExt_brkAnm_initWithFlags(u32, u32, u32, int, int, float, int, int, int, int)
ext 025B9E38 void BpwHD_dSv_info_c_onSwitch(u32, s32, s32)
ext 025B9F7C void BpwHD_dSv_info_c_offSwitch(u32, s32, s32)
ext 020198D8 float BpwHD_cM_rndF(float)
ext 020198D8 float BpwHD_cM_rndF_2(f32)
ext 025D4604 int BpwHD_fopAc_IsActor(u32)
ext 025DE508 void BpwHD_fpcEx_Search(unsigned int, u32)
ext 025D68EC float BpwHD_fopAcM_searchActorDistance_2(u32, u32)
ext 025D6894 short BpwHD_fopAcM_searchActorAngleY(u32, u32)
ext 0200FAAC short BpwHD_cLib_distanceAngleS(s32, s32)
ext 02019918 float BpwHD_cM_rndFX(float)
ext 025F1884 void BpwHD_mDoMtx_YrotS(u32, s32)
ext 0200FCD8 void BpwHD_MtxPosition_2(void *, void *)
ext 028E8D88 void BpwHD_PSVECAdd(void *, u32, void *)
ext 0200ED84 void BpwHD_cLib_addCalc2_2(u32, f32, float, f32)
ext 020195B0 short BpwHD_cM_atan2s(f32, f32)
ext 028F4384 float BpwHD_sqrtf(f32)
ext 0200F428 void BpwHD_cLib_addCalcAngleS2_2(u32, s32, int, int)
ext 0200ED84 void BpwHD_cLib_addCalc2_3(u32, float, float, float)
ext 025E4F64 unsigned int BpwHD_mDoExt_McaMorf_mDoExt_McaMorf(unsigned int, u32, unsigned int, unsigned int, u32, u32, float, unsigned int, int, int, unsigned int, unsigned int, unsigned int)
ext 027F3F94 unsigned int BpwHD_J3DModelData_getJointTree(unsigned int)
ext 025E80D0 unsigned int BpwHD_mDoExt_brkAnm_construct(u32)
ext 025E8154 int BpwHD_mDoExt_brkAnm_initWithFlags_2(u32, u32, u32, int, u32, float, int, int, int, int)
ext 025E8A48 int BpwHD_mDoExt_invisibleModel_create(u32, u32)
ext 025E8154 int BpwHD_mDoExt_brkAnm_initWithFlags_3(u32, u32, u32, int, int, float, int, int, int, int)
ext 02515E50 void BpwHD_dCcD_GStts_Move(u32)
ext 025162A4 int BpwHD_dCcD_GObjInf_ChkTgHit(u32)
ext 02516300 unsigned int BpwHD_dCcD_GObjInf_GetTgHitObj(u32)
ext 025A847C void BpwHD_dPa_control_c_set_2(unsigned int, int, int, void *, u32, void *, int, int, int, int, int, int)
ext 027F3F8C unsigned int BpwHD_J3DModelData_getMaterialTable(u32)
ext 020D1280 void BpwHD_GXColor_toFloat(void *, u32)
ext 0274D458 void BpwHD_GXColor_fromFloat(void *, void *, float)
ext 027F9F0C unsigned int BpwHD_J3DMaterial_getConstantColor(u32, int)
ext 025E83FC void BpwHD_mDoExt_brkAnm_entry(u32, u32, float)
ext 025F064C void BpwHD_mDoGph_gInf_c_onBlure()
ext 02562F5C void BpwHD_dKy_setLightTevColorType(u32, u32, u32)
ext 027F58E0 void BpwHD_J3DModel_setVisibility(u32, int)
ext 025E5590 void BpwHD_mDoExt_McaMorf_entryDL(unsigned int)
ext 025E8CD8 void BpwHD_mDoExt_invisibleModel_entryMaskOff(u32)
ext 025E8EC0 void BpwHD_mDoExt_invisibleModel_entry(u32)
ext 025BED80 void BpwHD_dSnap_RegistFig(int, u32, float, float, float)
ext 025E5590 void BpwHD_mDoExt_McaMorf_entryDL_2(u32)
ext 028E90D4 void BpwHD_PSMTXCopy_3(unsigned int, u32)
ext 0200FAD8 void BpwHD_MtxTrans_2(f32, f32, f32, int)
ext 0200FC74 void BpwHD_MtxScale(float, float, float, int)
ext 025E54D8 void BpwHD_mDoExt_McaMorf_updateDL(unsigned int)
ext 025D674C void BpwHD_fopAcM_setCullSizeBox(u32, f32, f32, f32, f32, f32, f32)
ext 024F06B4 void BpwHD_dBgS_Acch_Set(u32, u32, u32, u32, int, u32, u32, int, int)
ext 025B9100 int BpwHD_dSv_memBit_c_isDungeonItem(unsigned int, int)
ext 02520460 int BpwHD_dComIfG_resLoad(u32, unsigned int)
ext 025D63E8 int BpwHD_fopAcM_entrySolidHeap(u32, unsigned int, unsigned int)
ext 02515F14 void BpwHD_dCcD_Stts_Init(u32, int, int, u32)
ext 0251677C void BpwHD_dCcD_Sph_Set(u32, unsigned int)
ext 025E535C void BpwHD_mDoExt_McaMorf_play(unsigned int, int, int, int)
ext 025D5A20 unsigned int BpwHD_fopAcM_createChild(int, u32, int, u32, s32, void *, int, int, int)
ext 025D5A20 unsigned int BpwHD_fopAcM_createChild_2(int, u32, u32, void *, s32, void *, int, int, int)
ext 02019918 float BpwHD_cM_rndFX_2(f32)
ext 025E18EC void BpwHD_mDoAud_bgmStart(u32)
ext 025564B4 void BpwHD_dKy_plight_set(u32)
ext 025E1AA4 void BpwHD_mDoAud_seStartLevel(u32, u32, u32, u32, s32)
ext 025E8154 void BpwHD_mDoExt_brkAnm_initWithFlags_4(u32, u32, u32, int, u32, float, int, int, int, int)
ext 025D9D24 void BpwHD_fopAcM_cancelCarryNow(u32)
ext 025D6894 short BpwHD_fopAcM_searchActorAngleY_2(u32, unsigned int)
ext 02008570 void BpwHD_cBgS_GetTriPnt(u32, u32, void *, void *, void *)
ext 024EF0F4 int BpwHD_dBgS_GetAttributeCode(u32, u32)
ext 025052BC void BpwHD_dCamera_c_ForceLockOff(u32, u32)
ext 025A847C unsigned int BpwHD_dPa_control_c_set_3(unsigned int, int, int, u32, u32, int, int, u32, s32, int, int, int)
ext 025602F0 void BpwHD_dKy_get_seacolor(gabi::be<unsigned int> *, gabi::be<unsigned int> *)
ext 0200ED84 void BpwHD_cLib_addCalc2_4(u32, f32, f32, f32)
ext 025D7B24 void BpwHD_fopAcM_orderPotentialEvent(u32, int, int, int)
ext 02514F2C void BpwHD_dCamera_c_Stop(u32)
ext 02515280 void BpwHD_dCamera_c_SetTrimSize(u32, int)
ext 025E1944 void BpwHD_mDoAud_bgmStreamPlay()
ext 0200ED84 void BpwHD_cLib_addCalc2_5(u32, float, float, f32)
ext 025A847C void BpwHD_dPa_control_c_set_4(unsigned int, int, int, u32, u32, int, int, u32, int, int, int, int)
ext 025CB610 void BpwHD_dVibration_c_StopQuake(u32, int)
ext 0251510C void BpwHD_dCamera_c_Reset(u32, void *, void *)
ext 02514F38 void BpwHD_dCamera_c_Start(u32)
ext 025E18EC void BpwHD_mDoAud_bgmStart_2(unsigned int)
ext 025B9098 void BpwHD_dSv_memBit_c_onDungeonItem(unsigned int, int)
ext 027F2BF8 int BpwHD_J3DFrameCtrl_checkPass(unsigned int, float)
ext 025E1A40 void BpwHD_mDoAud_seStart_2(unsigned int, u32, unsigned int, s32)
ext 02514F88 void BpwHD_dCamera_c_Set(u32, void *, void *, float, int)
ext 027F2BF8 int BpwHD_J3DFrameCtrl_checkPass_2(unsigned int, f32)
ext 025A847C void BpwHD_dPa_control_c_set_5(unsigned int, int, u32, u32, u32, u32, int, u32, int, int, int, int)
ext 02018D40 void BpwHD_cM3dGSph_SetC(u32, u32)
ext 02018C8C void BpwHD_cM3dGSph_SetR(u32, f32)
ext 0200E240 void BpwHD_cCcS_Set(u32, u32)
ext 0200FAAC short BpwHD_cLib_distanceAngleS_2(s32, int)
ext 0200EDC8 void BpwHD_cLib_addCalc0(u32, float, float)
ext 025E742C void BpwHD_mDoExt_baseAnm_play(unsigned int)
ext 02008FEC void BpwHD_cBgS_LinChk_ct(void *)
ext 025F1884 void BpwHD_mDoMtx_YrotS_2(unsigned int, s32)
ext 028E8D88 void BpwHD_PSVECAdd_2(u32, u32, u32)
ext 024F1AFC void BpwHD_dBgS_LinChk_Set(void *, u32, u32, u32)
ext 02008860 int BpwHD_cBgS_LineCross(u32, void *)
ext 020084C8 unsigned int BpwHD_cBgS_GetTriPla(u32, u32, u32)
ext 02008B4C void BpwHD_cBgS_LinChk_destroy(void *, int)
ext 025D6800 void BpwHD_fopAcM_posMove(u32, unsigned int)
ext 025D6800 void BpwHD_fopAcM_posMove_2(u32, u32)
ext 0201ADE0 void BpwHD_cXyz___mi(u32, void *, u32)
ext 025CB374 void BpwHD_dVibration_c_StartShock(u32, int, int, void *)
ext 02008FEC void BpwHD_cBgS_LinChk_ct_2(u32)
ext 02008B4C void BpwHD_cBgS_LinChk_destroy_2(u32, int)
ext 02824890 void BpwHD_JPASetRMtxSTVecfromMtx(u32, u32, u32, u32)
ext 025A847C unsigned int BpwHD_dPa_control_c_set_6(unsigned int, int, u32, u32, u32, int, int, u32, s32, int, int, int)
ext 02019788 float BpwHD_cM_rnd()
ext 024F1AFC void BpwHD_dBgS_LinChk_Set_2(u32, u32, u32, u32)
ext 02008860 int BpwHD_cBgS_LineCross_2(u32, u32)
ext 025A847C unsigned int BpwHD_dPa_control_c_set_7(unsigned int, int, int, u32, u32, int, int, int, int, int, int, int)
ext 025A847C void BpwHD_dPa_control_c_set_8(unsigned int, int, int, u32, u32, int, int, u32, s32, int, int, int)
ext 025D5834 unsigned int BpwHD_fopAcM_create_2(int, int, u32, s32, u32, int, int, int)
ext 025D5834 unsigned int BpwHD_fopAcM_create_3(int, int, void *, s32, u32, int, int, int)
ext 027F2BF8 int BpwHD_J3DFrameCtrl_checkPass_3(u32, float)
ext 02018D40 void BpwHD_cM3dGSph_SetC_2(u32, void *)
ext 02018C8C void BpwHD_cM3dGSph_SetR_2(u32, float)
ext 0200F428 void BpwHD_cLib_addCalcAngleS2_3(u32, s32, int, s32)
ext 025E742C void BpwHD_mDoExt_baseAnm_play_2(u32)
ext 025B8B68 void BpwHD_dSv_event_c_onEventBit(unsigned int, int)
ext 02587EFC void BpwHD_dLib_setNextStageBySclsNum(int, s32)
ext 025E1A40 void BpwHD_mDoAud_seStart_3(unsigned int, unsigned int, unsigned int, s32)
ext 025D99E8 void BpwHD_fopAcM_createDisappear(u32, u32, int, int, int)
ext 025E1904 void BpwHD_mDoAud_bgmStop(int)
ext 025E1934 void BpwHD_mDoAud_bgmStreamPrepare(unsigned int)
ext 025D99E8 void BpwHD_fopAcM_createDisappear_2(u32, void *, int, int, int)
ext 025D9874 void BpwHD_fopAcM_createWarpFlower(u32, int, s32, int)
*/
// clang-format on
