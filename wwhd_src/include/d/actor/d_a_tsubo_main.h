/* daTsubo::Act_c (small carriable objects: pots, skulls, barrels, ...), WWHD layout.
 *
 * GameCube -> WWHD: every member +0x11C (fopAc_ac_c); size 0x93C (constructor 024CA1DC
 * allocates 0x93C; GameCube 0x820). The HD vtable pointer is at +0xB4. The data tables keep
 * their GameCube layouts (Data_c 0xCC, M_data at 0x10040FAC). */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/tu.h"

namespace daTsubo {

/* dBgS_ObjGndChk_Yogan (0x54, the lava ground check: mGrp 4) */
using dBgS_GndChk_l = dBgS_GndChk;

/* dPa_followEcallBack (0x14), see bindings.h; remove() is the virtual at vtable +0x44 */

struct Act_c : fopAc_ac_c {
    void prmZ_init();
    bool create_heap();
    void cull_set_draw();
    void create_init_cull();
    void create_init_cc();
    void create_init_bgc();
    void init_rot_clean();
    void mode_hide_init();
    void mode_appear_init();
    void mode_wait_init();
    void eff_kutani_init();
    void spec_make_boko(int arg1);
    void spec_set_actor();
    void spec_carry_spec();
    void spec_clr_actor();
    void spec_init();
    void set_tensor(const cXyz* pos);
    void spec_mtx();
    void set_mtx();
    void init_mtx();
    cPhs_State _create();
    void spec_remove();
    bool _is_delete();
    bool _delete();
    void cull_set_move();
    void set_senv(int arg1, int arg2);
    enum Type { TYPE_WATER_JUG = 2, TYPE_KUTANI = 14 };
    enum Prm_e {
        PRM_ITEMNO_W = 6, PRM_ITEMNO_S = 0,
        PRM_SPEC_W = 6, PRM_SPEC_S = 8,
        PRM_MOVEBG_W = 2, PRM_MOVEBG_S = 14,
        PRM_ITEMSAVE_W = 7, PRM_ITEMSAVE_S = 16,
        PRM_TYPE_W = 4, PRM_TYPE_S = 24,
        PRM_CULL_W = 3, PRM_CULL_S = 28,
        PRM_STICK_W = 1, PRM_STICK_S = 31,
    };
    enum DataFlag_e {
        DATA_FLAG_1_e = 0x1, DATA_FLAG_2_e = 0x2, DATA_FLAG_4_e = 0x4,
        DATA_FLAG_8_e = 0x8, DATA_FLAG_10_e = 0x10, DATA_FLAG_20_e = 0x20,
    };

    struct Data_c {
        /* 0x00 */ be<f32> mGravity;
        /* 0x04 */ be<f32> m04;
        /* 0x08 */ be<f32> mAttnY;
        /* 0x0C */ be<f32> mModelScale;
        /* 0x10 */ be<u8> m10;
        /* 0x11 */ u8 _11[3];
        /* 0x14 */ be<f32> m14;
        /* 0x18 */ be<f32> m18;
        /* 0x1C */ be<f32> m1C;
        /* 0x20 */ be<f32> m20;
        /* 0x24 */ be<f32> m24;
        /* 0x28 */ be<s16> m28;
        /* 0x2A */ be<s16> m2A;
        /* 0x2C */ be<s16> m2C;
        /* 0x2E */ u8 _2E[2];
        /* 0x30 */ be<f32> m30;
        /* 0x34 */ be<f32> m34;
        /* 0x38 */ be<f32> m38;
        /* 0x3C */ be<f32> m3C;
        /* 0x40 */ be<f32> m40;
        /* 0x44 */ be<f32> m44;
        /* 0x48 */ be<f32> m48;
        /* 0x4C */ be<f32> m4C;
        /* 0x50 */ be<f32> m50;
        /* 0x54 */ be<s16> m54;
        /* 0x56 */ be<s16> m56;
        /* 0x58 */ be<f32> m58;
        /* 0x5C */ be<f32> m5C;
        /* 0x60 */ be<u8> m60;
        /* 0x61 */ be<u8> m61;
        /* 0x62 */ be<u8> m62;
        /* 0x63 */ be<u8> m63;
        /* 0x64 */ be<u8> m64;
        /* 0x65 */ be<u8> m65;
        /* 0x66 */ u8 _66[2];
        /* 0x68 */ be<u32> mFlag;
        /* 0x6C */ be<u16> m6C;
        /* 0x6E */ be<u8> m6E;
        /* 0x6F */ be<u8> mAcchCirRad;
        /* 0x70 */ be<u8> m70;
        /* 0x71 */ be<u8> mAcchRoofHeight;
        /* 0x72 */ be<u8> mAttnDist;
        /* 0x73 */ u8 _73;
        /* 0x74 */ cXyz m74;
        /* 0x80 */ be<u32> m80;
        /* 0x84 */ be<s32> mSoundID_Break;
        /* 0x88 */ be<s32> m88;
        /* 0x8C */ be<s32> mSoundID_FallLava;
        /* 0x90 */ be<s32> mSoundID_FallWater;
        /* 0x94 */ be<s32> mSoundID_Hit;
        /* 0x98 */ be<s16> mCullSphX_Move;
        /* 0x9A */ be<s16> mCullSphY_Move;
        /* 0x9C */ be<s16> mCullSphZ_Move;
        /* 0x9E */ be<s16> mCullSphR_Move;
        /* 0xA0 */ u8 mA0[4];
        /* 0xA4 */ be<s16> mCullSphX_Draw;
        /* 0xA6 */ be<s16> mCullSphY_Draw;
        /* 0xA8 */ be<s16> mCullSphZ_Draw;
        /* 0xAA */ be<s16> mCullSphR_Draw;
        /* 0xAC */ u8 mAC[4];
        /* 0xB0 */ be<f32> mB0;
        /* 0xB4 */ u8 mB4[4];
        /* 0xB8 */ be<u32> mHeapSize;
        /* 0xBC */ be<f32> mBC;
        /* 0xC0 */ be<f32> mC0;
        /* 0xC4 */ be<f32> mC4;
        /* 0xC8 */ be<f32> mC8;
    };

    struct AttrSpine_c {
        /* 0x00 */ be<f32> m00;
        /* 0x04 */ be<f32> m04;
        /* 0x08 */ be<f32> m08;
        /* 0x0C */ be<f32> m0C;
        /* 0x10 */ be<f32> m10;
        /* 0x14 */ be<f32> m14;
        /* 0x18 */ be<s16> m18;
        /* 0x1A */ be<s16> m1A;
        /* 0x1C */ be<s16> m1C;
        /* 0x1E */ be<s16> m1E;
        /* 0x20 */ be<f32> m20;
        /* 0x24 */ be<f32> m24;
        /* 0x28 */ be<f32> m28;
        /* 0x2C */ be<f32> m2C;
        /* 0x30 */ be<s16> m30;
        /* 0x32 */ u8 _32[2];
    };

    struct SpecBokoData {
        /* 0x00 */ be<s16> m00;
        /* 0x02 */ be<s16> m02;
        /* 0x04 */ be<f32> m04;
        /* 0x08 */ be<f32> m08;
    };

    struct M7A0 {
        /* 0x00 */ be<u32> m00;  /* fpc_ProcID */
        /* 0x04 */ be<u8> m04;
        /* 0x05 */ u8 _05;
        /* 0x06 */ be<s16> m06;
        /* 0x08 */ be<s16> m08;
        /* 0x0A */ u8 _0A[2];
        /* 0x0C */ be<f32> m0C;
        /* 0x10 */ be<f32> m10;
        /* 0x14 */ be<f32> m14;
        /* 0x18 */ be<s8> m18;
        /* 0x19 */ u8 _19[3];
    };

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_brkAnm> mpBrk;
    /* 0x3BC */ dBgS_ObjAcch mAcch;
    /* 0x580 */ dBgS_AcchCir mAcchCir;
    /* 0x5C0 */ dBgS_GndChk_l mGndChkYogan;
    /* 0x614 */ be<f32> m4F8;
    /* 0x618 */ be<f32> m4FC;
    /* 0x61C */ be<f32> m500;
    /* 0x620 */ be<u8> m504;
    /* 0x621 */ be<u8> m505;
    /* 0x622 */ u8 _622[2];
    /* 0x624 */ dCcD_Cyl mCyl;
    /* 0x754 */ dCcD_Stts mStts;
    /* 0x790 */ be<s32> mType;
    /* 0x794 */ be<s32> m678;
    /* 0x798 */ be<u16> mPrmZ;
    /* 0x79A */ be<u8> m67E;
    /* 0x79B */ be<u8> m67F;
    /* 0x79C */ be<u8> m680;
    /* 0x79D */ be<s8> m681;
    /* 0x79E */ be<u8> m682;
    /* 0x79F */ be<u8> m683;
    /* 0x7A0 */ be<u8> m684;
    /* 0x7A1 */ be<u8> m685;
    /* 0x7A2 */ be<u8> m686;
    /* 0x7A3 */ u8 _7A3;
    /* 0x7A4 */ be<s16> m688;          /* cSAngle */
    /* 0x7A6 */ be<s16> m68A;
    /* 0x7A8 */ be<s16> m68C;
    /* 0x7AA */ be<s16> m68E;
    /* 0x7AC */ be<s16> m690;
    /* 0x7AE */ be<s16> m692;
    /* 0x7B0 */ be<f32> m694[4];       /* Quaternion x, y, z, w */
    /* 0x7C0 */ u8 m6A4[0xC];
    /* 0x7CC */ cXyz m6B0;
    /* 0x7D8 */ Mtx34 mPoseMtx;
    /* 0x808 */ be<f32> m6EC;
    /* 0x80C */ cXyz m6F0;
    /* 0x818 */ be<f32> m6FC;
    /* 0x81C */ cXyz m700;
    /* 0x828 */ dPa_followEcallBack m70C[3];
    /* 0x864 */ dPa_followEcallBack m748[3];
    /* 0x8A0 */ dPa_followEcallBack m784;
    /* 0x8B4 */ be<u8> m798;
    /* 0x8B5 */ u8 m799[7];
    /* 0x8BC */ M7A0 m7A0[3];
    /* 0x910 */ cXyz m7F4;
    /* 0x91C */ be<s16> m800;
    /* 0x91E */ be<s16> m802;
    /* 0x920 */ be<s16> m804;
    /* 0x922 */ be<s16> m806;
    /* 0x924 */ be<f32> m808;
    /* 0x928 */ be<f32> m80C;
    /* 0x92C */ be<u8> m810;
    /* 0x92D */ be<u8> m811;
    /* 0x92E */ u8 _92E[2];
    /* 0x930 */ cXyz m814;
};
WWHD_OFFSET(Act_c, mAcch, 0x3BC);
WWHD_OFFSET(Act_c, mAcchCir, 0x580);
WWHD_OFFSET(Act_c, mCyl, 0x624);
WWHD_OFFSET(Act_c, mStts, 0x754);
WWHD_OFFSET(Act_c, mType, 0x790);
WWHD_OFFSET(Act_c, m688, 0x7A4);
WWHD_OFFSET(Act_c, mPoseMtx, 0x7D8);
WWHD_OFFSET(Act_c, m70C, 0x828);
WWHD_OFFSET(Act_c, m7A0, 0x8BC);
WWHD_OFFSET(Act_c, m814, 0x930);
WWHD_SIZE(Act_c::Data_c, 0xCC);
WWHD_SIZE(Act_c::AttrSpine_c, 0x34);
WWHD_SIZE(Act_c::M7A0, 0x1C);
WWHD_SIZE(Act_c, 0x93C);


namespace Method {
cPhs_State Create(void* actor);
bool Delete(void* actor);
bool Execute(void* actor);
bool Draw(void* actor);
bool IsDelete(void* actor);
}
struct MaterialTableView {
    u8 _00[0x18];
    be<u32> mMaterialNameOffset;
};
WWHD_OFFSET(MaterialTableView, mMaterialNameOffset, 0x18);
struct StageDataView { gptr<be<u32>> mVtable; };
struct StageInfoView { u8 _00[0x12]; be<u16> mCullPoint; };
WWHD_OFFSET(StageInfoView, mCullPoint, 0x12);
struct BokoModelView { u8 _00[0x3B4]; gptr<J3DModel> mpModel; };
WWHD_OFFSET(BokoModelView, mpModel, 0x3B4);
struct SafeStringVtableView { u8 _00[0x14]; be<u32> mAssureTermination; };
WWHD_OFFSET(SafeStringVtableView, mAssureTermination, 0x14);
struct PlayStageView { u8 _00[0x5134]; be<u8> mStartStage[0x1C]; StageDataView mStageData; };
WWHD_OFFSET(PlayStageView, mStartStage, 0x5134);
WWHD_OFFSET(PlayStageView, mStageData, 0x5150);
} // namespace daTsubo

BOOL tsuboSearchByID(u32 id, gptr<fopAc_ac_c>* out);
void tsuboSetCarryNow(fopAc_ac_c* actor, s32 stageLayer);
void tsuboSetCullSphere(fopAc_ac_c* actor, f32 x, f32 y, f32 z, f32 radius);
void tsuboQuatRotAxisRad(Quaternion* quat, const cXyz* axis, f32 rad);
void tsuboMtxQuat(Mtx34* matrix, Quaternion* quat);
void tsuboMtxIdentity(Mtx34* matrix);
void tsuboMtxConcat(const Mtx34* a, const Mtx34* b, Mtx34* result);
BOOL tsuboChkMoveBg(dBgS* bg, cBgS_PolyInfo* poly);
void tsuboSoundSet(cXyz* pos, s32 sound, u32 id, s32 flags);
s32 tsuboGetMaterialIndex(JUTNameTab* table, const char* name);
BOOL tsuboBrkInit(void* animation, J3DModelData* model, void* key, s32 play, s32 attr, s16 start, s16 end, bool modify, s32 entry, f32 rate);
void tsuboGroundCheckCtor(cBgS_GndChk* check);
void tsuboConstructArray(void* array, s32 count, s32 stride, u32 constructor);
void tsuboSetMaterial(J3DModel* model, J3DMaterial* material, s32 enabled);
void* tsuboBrkCtor(void* animation);
u8 tsuboHeapResult(fopAc_ac_c* actor);
void tsuboBreakPinecone(daTsubo::Act_c* actor);
void tsuboBreakStool(daTsubo::Act_c* actor);
void tsuboBreakSkull(daTsubo::Act_c* actor);
bool tsuboExecute(void* actor);
bool tsuboDraw(void* actor);
void tsuboDestroyArray(void* array, s32 count, s32 stride, u32 destructor, s32 flags, s32 reserved);
void tsuboGroundCheckDtor(cBgS_GndChk* check, s32 flags);
void tsuboCircleDtor(cM3dGCir_l* circle, s32 flags);
void tsuboAcchDtor(dBgS_Acch* acch, s32 flags);
void tsuboActorDtor(fopAc_ac_c* actor, s32 flags);
u32 tsuboPrmAbstract(const fopAc_ac_c* actor, s32 width, s32 shift);
void tsuboAngleCtor(be<s16>* angle);
void tsuboAngleCopy(be<s16>* angle, const be<s16>* value);
void tsuboAngleSet(be<s16>* angle, s16 value);
namespace daTsubo {
// HD: per-translation-unit tables and vtable copies.
#define TSUBO_SAFESTRING_VTBL 0x10040AD4 /* sead::SafeString vtable (per TU) */
#define TSUBO_ACT_VTBL 0x10040BFC        /* Act_c vtable (HD virtual destructor) */
#define TSUBO_CYL_AAB_VTBL 0x10040AEC    /* cM3dGAab vtable (per TU) */
static const dBgS_ObjAcch_vt TSUBO_OBJACCH_VT = {0x10040BCC, 0x10040BEC, 0x10040BDC};
/* dBgS_ObjGndChk_Yogan vtables at +0x10/+0x20/+0x40/+0x4C */
static const dBgS_GndChk_vt TSUBO_YOGAN_VT = {0x10040B8C, 0x10040B9C, 0x10040BBC, 0x10040BAC};

/* const tables (.rodata): GHS folds the constant members of M_attrSpine / M_data_spec_boko into
 * immediates where it can; where the code reads them, these accessors give the addresses */
inline Act_c::Data_c* M_data(s32 type) { return gabi::at<Act_c::Data_c>(0x10040FAC + 0xCC * type); }
inline const char* M_arcname(s32 type) { return gabi::at<const char>(gabi::load<u32>(0x10040F00 + 4 * type)); }
inline Act_c::SpecBokoData* M_data_spec_boko(s32 i) { return gabi::at<Act_c::SpecBokoData>(0x10040F40 + 0xC * i); }
#define M_cyl_src gabi::at<dCcD_SrcCyl>(0x10040F68)
/* static fopAc_ac_c* Act_c::M_spec_act[3] */
struct SpecActorSlot { gptr<fopAc_ac_c> mActor; };
gptr<fopAc_ac_c>* M_spec_act();
/* the data of this object's type: mType is reloaded at each use */
inline Act_c::Data_c* data(Act_c* a) { return M_data(a->mType); }

// HD parameter helpers retain the out-of-line call.
/* 02048038 daObj::PrmAbstract(actor, width, shift): (mParameters >> shift) & ((1 << width) - 1)
 * (one shared out-of-line copy in HD) */
inline u32 daObj_PrmAbstract(const fopAc_ac_c* a, s32 w, s32 s) { return tsuboPrmAbstract(a, w, s); }
inline s32 prm_get_type(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_TYPE_W, Act_c::PRM_TYPE_S); }
inline s32 prm_get_spec(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_SPEC_W, Act_c::PRM_SPEC_S); }
inline s32 prm_get_cull(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_CULL_W, Act_c::PRM_CULL_S); }
inline s32 prm_get_itemNo(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_ITEMNO_W, Act_c::PRM_ITEMNO_S); }
inline s32 prm_get_itemSave(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_ITEMSAVE_W, Act_c::PRM_ITEMSAVE_S); }
inline bool prm_get_moveBg(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_MOVEBG_W, Act_c::PRM_MOVEBG_S) == 1u; }
inline u32 prm_get_stick(Act_c* a) { return daObj_PrmAbstract(a, Act_c::PRM_STICK_W, Act_c::PRM_STICK_S); }
inline void prm_set_cull_non(Act_c* a) { a->mParameters = a->mParameters & ~0x70000000u; }
inline void prm_off_moveBg(Act_c* a) { a->mParameters = a->mParameters | 0xC000u; }
inline void prm_off_stick(Act_c* a) { a->mParameters = a->mParameters & ~0x80000000u; }
inline bool is_switch(Act_c* a) { return fopAcM_isSwitch(a, a->mPrmZ & 0xFF); }
inline bool spec_is_boko(s32 spec) { return spec == 1 || spec == 2 || spec == 3; }


inline void cSAngle_ct(be<s16>* angle) { tsuboAngleCtor(angle); }
inline void cSAngle_Val(be<s16>* angle, const be<s16>* value) { tsuboAngleCopy(angle, value); }
inline void cSAngle_Val(be<s16>* angle, s16 value) { tsuboAngleSet(angle, value); }
#define cSAngle__0 gabi::at<be<s16>>(0x101FF354)
inline void Acch_ClrRoofNone(dBgS_Acch* acch) { acch->m_flags &= ~(u32)dBgS_Acch::ROOF_NONE; }
inline void Acch_ClrWallNone(dBgS_Acch* acch) { acch->m_flags &= ~(u32)dBgS_Acch::WALL_NONE; }
inline void Acch_ClrWaterNone(dBgS_Acch* acch) { acch->m_flags &= ~(u32)dBgS_Acch::WATER_NONE; }
inline void Acch_ClrGroundLanding(dBgS_Acch* acch) { acch->m_flags &= ~(u32)dBgS_Acch::GROUND_LANDING; }
inline cBgS_PolyInfo* Acch_gnd(dBgS_Acch* acch) { return static_cast<cBgS_PolyInfo*>(&acch->m_gnd); }
inline void attn_onBit(fopAc_ac_c* actor, u32 flags) { actor->attention_info.flags |= flags; }
inline void attn_offBit(fopAc_ac_c* actor, u32 flags) { actor->attention_info.flags &= ~flags; }
// HD: restart stage and stage-data virtuals are reached through the verified play layout.
inline StageInfoView* tsuboStageInfo() {
    StageDataView* data = &gabi::at<PlayStageView>(dComIfGp_ea())->mStageData;
    return gabi::call_ptr<StageInfoView*>(data->mVtable.get()[0x15C / 4], data);
}
inline Quaternion* tsuboQuatStack() { return *gabi::at<gptr<Quaternion>>(0x1048D3FC); }
// HD: literal termination runs twice before the stage string virtual, then bounded byte comparison.
template <class F> inline bool tsuboStageEquals(const char* literal, F stageName) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_addr(literal);
    a->__vtbl = TSUBO_SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    u32 stage = stageName();
    b->__vtbl = TSUBO_SAFESTRING_VTBL;
    b->mStringTop = stage;
    auto assure = [](SafeString* str) {
        auto* table = gabi::at<SafeStringVtableView>(str->__vtbl);
        gabi::call_ptr(table->mAssureTermination, str);
    };
    assure(a); assure(a);
    u32 bv = b->__vtbl;
    u32 first = a->mStringTop;
    gabi::call_ptr(gabi::at<SafeStringVtableView>(bv)->mAssureTermination, b.get());
    u32 second = b->mStringTop;
    if (first == second) return true;
    const be<u8>* lhs = gabi::at<const be<u8>>(first);
    const be<u8>* rhs = gabi::at<const be<u8>>(second);
    for (u32 remaining = 0x40001; remaining != 0; --remaining, ++lhs, ++rhs) {
        u8 c1 = *lhs, c2 = *rhs;
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
    return false;
}
inline MaterialTableView* tsuboMaterialTable(J3DModelData* data) { return gabi::at<MaterialTableView>(data->mpResHeader); }
inline const char* tsuboStringTop(SafeString* str) { return gabi::at<const char>(str->mStringTop); }
inline J3DMaterial* tsuboMaterials(J3DModelData* data) { return gabi::at<J3DMaterial>(data->mMaterials); }
inline const be<u32>* tsuboZeroQuatWords() { return gabi::at<const be<u32>>(0x101E9C38); }
inline cXyz* tsuboBaseX() { return gabi::at<cXyz>(0x101FFBB4); }
} // namespace daTsubo

WWHD_SIZE(daTsubo::MaterialTableView, 0x1C);
WWHD_SIZE(daTsubo::StageDataView, 4);
WWHD_SIZE(daTsubo::StageInfoView, 0x14);
WWHD_SIZE(daTsubo::BokoModelView, 0x3B8);
WWHD_SIZE(daTsubo::SafeStringVtableView, 0x18);
WWHD_SIZE(daTsubo::PlayStageView, 0x5154);
WWHD_SIZE(daTsubo::SpecActorSlot, 4);
