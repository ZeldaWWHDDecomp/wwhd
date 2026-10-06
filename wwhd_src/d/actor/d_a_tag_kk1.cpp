/**
 * d_a_tag_kk1.cpp (WWHD)
 * Tag - start of the chase of Mila (Kk1): player near and facing the tag's direction.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_kk1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define TAGKK1_VTBL 0x1003F698 /* HD: daTag_Kk1_c vtable */
#define HIO_VTBL 0x1003F6C0    /* HD: daTag_Kk1_HIO_c vtable */

/* daTag_Kk1_HIO_c, HD: vtable after the members (+0x10), size 0x14 */
struct daTag_Kk1_HIO_c {
    struct hio_prm_c {
        /* 0x0 */ be<f32> mHorizontalDistance;
        /* 0x4 */ be<f32> mVerticalDistance;
        /* 0x8 */ be<u8> field_0x10;
        /* 0x9 */ u8 _9[3];
    };
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ hio_prm_c prm;
    /* 0x10 */ be<u32> __vtbl;
};
WWHD_SIZE(daTag_Kk1_HIO_c, 0x14);
static daTag_Kk1_HIO_c& l_HIO() { return *gabi::at<daTag_Kk1_HIO_c>(0x1046E568); }

/* HD: fopNpc_npc_c is 0x7DC (the GameCube vtable at 0x6C0 is gone): GameCube 0x6C4 -> 0x7DC */
struct daTag_Kk1_c : fopNpc_npc_c {
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();

    /* 0x7DC */ be<u8> padding_0x6C4;
    /* 0x7DD */ be<u8> mTagSet;
    /* 0x7DE */ be<u8> mNameIsWrong;
    /* 0x7DF */ u8 _7DF;
};
WWHD_OFFSET(daTag_Kk1_c, mTagSet, 0x7DD);
WWHD_SIZE(daTag_Kk1_c, 0x7E0);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025A1458 fopNpc_npc_c::fopNpc_npc_c (matcher: cDyl_LinkASync) */
static inline void fopNpc_npc_c_ct(fopNpc_npc_c* p) { gabi::call(0x025A1458, p); }
/* 028E8DE8 PSVECSquareDistance */
static inline f32 PSVECSquareDistance(const cXyz* a, const cXyz* b) { return gabi::call<f32>(0x028E8DE8, a, b); }
/* cXyz::abs(const cXyz&): sqrtf(PSVECSquareDistance) */
static inline f32 cXyz_abs(const cXyz* a, const cXyz* b) { return std_sqrtf(PSVECSquareDistance(a, b)); }

/* 024ACD04 daTag_Kk1_HIO_c::daTag_Kk1_HIO_c (HD: allocates when this == NULL) */
static daTag_Kk1_HIO_c* daTag_Kk1_HIO_c_ct(daTag_Kk1_HIO_c* i_this) {
    WWHD_FUNC(0x024ACD04, daTag_Kk1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daTag_Kk1_HIO_c*)operator_new(0x14);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO_VTBL;
    /* prm = a_prm_tbl (.data 0x101D1C90: 350.0f, 30.0f, 0), copied as words */
    for (u32 i = 0; i < 12; i += 4)
        gabi::store<u32>(gabi::ea(&i_this->prm) + i, gabi::load<u32>(0x101D1C90 + i));
    i_this->mNo = -1;
    return i_this;
}
VERIFY(0x024ACD04, daTag_Kk1_HIO_c_ct);

/* 024ACC98: HD: a debug leftover initialises a function-local static color on first use */
BOOL daTag_Kk1_c::_draw() {
    WWHD_FUNC(0x024ACC98, BOOL, this);
    if (l_HIO().prm.field_0x10 != 0) {
        if (gabi::load<u32>(0x101FDA48) == 0) {
            gabi::store<u32>(0x101FDA48, 1);
            memcpy_g(gabi::at<u8>(0x101FEBEC), gabi::at<u8>(0x1003F670), 4); /* 028FEAC0 memcpy */
        }
    }
    return true;
}
VERIFY(0x024ACC98, &daTag_Kk1_c::_draw);

/* 024ACBC0 */
BOOL daTag_Kk1_c::_execute() {
    WWHD_FUNC(0x024ACBC0, BOOL, this);
    f32 distance = cXyz_abs(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
    f32 vert_distance = dComIfGp_getPlayer(0)->current.pos.y - current.pos.y;
    mTagSet = false;
    if (distance < l_HIO().prm.mHorizontalDistance && vert_distance < l_HIO().prm.mVerticalDistance) {
        s16 angle_deviation = dComIfGp_getPlayer(0)->shape_angle.y - current.angle.y;
        angle_deviation = (s16)(angle_deviation < 0 ? -angle_deviation : angle_deviation);
        if (angle_deviation < 0x1000) {
            mTagSet = true;
        }
    }
    return true;
}
VERIFY(0x024ACBC0, &daTag_Kk1_c::_execute);

/* 024ACB78 */
BOOL daTag_Kk1_c::_delete() {
    WWHD_FUNC(0x024ACB78, BOOL, this);
    if (l_HIO().mNo >= 0) {
        mDoHIO_deleteChild(l_HIO().mNo);
        l_HIO().mNo = -1;
    }
    return TRUE;
}
VERIFY(0x024ACB78, &daTag_Kk1_c::_delete);

/* 024ACAC0 (createInit inlined; HD: no fpcM_GetName(&name_int) leftover) */
cPhs_State daTag_Kk1_c::_create() {
    WWHD_FUNC(0x024ACAC0, cPhs_State, this);
    /* fopAcM_ct(this, daTag_Kk1_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopNpc_npc_c_ct(this);
            __vtbl = TAGKK1_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    switch (fpcM_GetName(this)) {
    case 0x19F: /* fpcNm_TAG_KK1_e */
        mNameIsWrong = false;
        break;
    default:
        return cPhs_ERROR_e;
    }

    if (l_HIO().mNo < 0) {
        /* "貧乏ム−ル追跡起動タグ" Poor Muuru (Mila) chase startup tag */
        l_HIO().mNo = mDoHIO_createChild(STR(0x1003F6D0), &l_HIO());
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024ACAC0, &daTag_Kk1_c::_create);

/* 024ACB74 */
static cPhs_State daTag_Kk1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024ACB74, cPhs_State, i_this);
    return static_cast<daTag_Kk1_c*>(i_this)->_create();
}
VERIFY(0x024ACB74, daTag_Kk1_Create);

/* 024ACBBC */
static BOOL daTag_Kk1_Delete(daTag_Kk1_c* i_this) {
    WWHD_FUNC(0x024ACBBC, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x024ACBBC, daTag_Kk1_Delete);

/* 024ACC94 */
static BOOL daTag_Kk1_Execute(daTag_Kk1_c* i_this) {
    WWHD_FUNC(0x024ACC94, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x024ACC94, daTag_Kk1_Execute);

/* 024ACCF8 */
static BOOL daTag_Kk1_Draw(daTag_Kk1_c* i_this) {
    WWHD_FUNC(0x024ACCF8, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x024ACCF8, daTag_Kk1_Draw);

/* 024ACCFC */
static BOOL daTag_Kk1_IsDelete(daTag_Kk1_c* i_this) {
    WWHD_FUNC(0x024ACCFC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024ACCFC, daTag_Kk1_IsDelete);

/* 024ACD68 */
static void __sinit_d_a_tag_kk1_cpp() {
    WWHD_FUNC(0x024ACD68, void, (u32)0);
    sinit_header_statics(0x1046E54C, 0x101D1C9C);
    daTag_Kk1_HIO_c_ct(&l_HIO()); /* static daTag_Kk1_HIO_c l_HIO */
}
VERIFY(0x024ACD68, __sinit_d_a_tag_kk1_cpp);

/* 024ACE08: daTag_Kk1_c deleting destructor (compiler-generated, vtable +0xC) */
static void daTag_Kk1_c_dt(daTag_Kk1_c* i_this, s32 flags) {
    WWHD_FUNC(0x024ACE08, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1003F678);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1003F688);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024ACE08, daTag_Kk1_c_dt);
