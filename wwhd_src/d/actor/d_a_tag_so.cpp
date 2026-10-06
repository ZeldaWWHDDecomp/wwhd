/**
 * d_a_tag_so.cpp (WWHD)
 * Tag - Seagull/jump area marker used by the minigame (radius from the parameters).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tag_so.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define TAG_SO_VTBL 0x1003FC64     /* daTag_So_c vtable (HD virtual destructor) */
#define TAG_SO_HIO_VTBL 0x1003FC74 /* daTag_So_HIO_c vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02587684 dLib_debugDrawFan(cXyz& center, s16 angle, s16 fan, f32 radius, const GXColor& color) */
static inline void dLib_debugDrawFan(cXyz* center, s16 angle, s16 fan, f32 radius, const GXColor* color) {
    gabi::call(0x02587684, center, angle, fan, radius, color);
}

/* HD: GHS lays out the vtable pointer after the members (+0xC); size 0x10 */
struct daTag_So_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ u8 m06[0xC - 0x2];
    /* 0x0C */ be<u32> __vtbl;
};
WWHD_SIZE(daTag_So_HIO_c, 0x10);
#define l_HIO (*gabi::at<daTag_So_HIO_c>(0x1046E65C))

struct daTag_So_c : fopAc_ac_c {
    bool _execute();
    void debugDraw();
    bool _draw();
    void getArg();
    cPhs_State _create();
    bool _delete();

    /* 0x3AC */ be<u8> mRndNum;
    /* 0x3AD */ u8 _3AD[3];
    /* 0x3B0 */ be<f32> mJumpRange;
    /* 0x3B4 */ be<u8> mType;
    /* 0x3B5 */ u8 m3B5[0x3C0 - 0x3B5];
};
WWHD_OFFSET(daTag_So_c, mJumpRange, 0x3B0);
WWHD_OFFSET(daTag_So_c, mType, 0x3B4);

/* 024B2C74: daTag_So_HIO_c::daTag_So_HIO_c (HD: allocates when this == NULL) */
static daTag_So_HIO_c* daTag_So_HIO_c_ct(daTag_So_HIO_c* i_this) {
    WWHD_FUNC(0x024B2C74, daTag_So_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daTag_So_HIO_c*)operator_new(0x10);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = TAG_SO_HIO_VTBL;
    i_this->mNo = -1;
    i_this->m05 = 0;
    return i_this;
}
VERIFY(0x024B2C74, daTag_So_HIO_c_ct);

/* 024B2B10 */
void daTag_So_c::debugDraw() {
    WWHD_FUNC(0x024B2B10, void, this);
    gabi::Local<cXyz> actorPos;
    actorPos->x = current.pos.x;
    actorPos->y = current.pos.y + 20.0f;
    actorPos->z = current.pos.z;
    /* HD: the GXColor literals are function-local statics initialised on first use */
    be<u32>& guard1 = *gabi::at<be<u32>>(0x101FDA50);
    GXColor* red = gabi::at<GXColor>(0x101FEBF4);
    if (guard1 == 0) {
        guard1 = 1;
        memcpy_g(red, gabi::at<u8>(0x1003FC58), 4);
    }
    if (mType == 1) {
        /* the same colour static is checked again (inline accessor used twice) */
        if (guard1 == 0) {
            guard1 = 1;
            memcpy_g(red, gabi::at<u8>(0x1003FC58), 4);
        }
        dLib_debugDrawFan(actorPos, shape_angle.y, 0x3500, mJumpRange, red);
    } else {
        /* HD: a second, unused colour static is initialised on the other path */
        be<u32>& guard2 = *gabi::at<be<u32>>(0x101FDAC4);
        if (guard2 == 0) {
            guard2 = 1;
            memcpy_g(gabi::at<u8>(0x101FEBFC), gabi::at<u8>(0x1003FC5C), 4);
        }
    }
}
VERIFY(0x024B2B10, &daTag_So_c::debugDraw);

/* 024B2C3C */
bool daTag_So_c::_draw() {
    WWHD_FUNC(0x024B2C3C, bool, this);
    if (l_HIO.m05)
        debugDraw();
    return TRUE;
}
VERIFY(0x024B2C3C, &daTag_So_c::_draw);

/* 024B2A30 */
void daTag_So_c::getArg() {
    WWHD_FUNC(0x024B2A30, void, this);
    u32 param = fopAcM_GetParam(this);
    mRndNum = param & 0xFF;
    s32 paramRadius = (param >> 8) & 0xFF;
    mType = (param >> 16) & 0xFF;
    if (paramRadius == 0xff) {
        mJumpRange = 1600.0f;
    } else {
        mJumpRange = (f32)(paramRadius * 100);
    }
}
VERIFY(0x024B2A30, &daTag_So_c::getArg);

/* 024B2A94 */
cPhs_State daTag_So_c::_create() {
    WWHD_FUNC(0x024B2A94, cPhs_State, this);
    /* fopAcM_ct(this, daTag_So_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TAG_SO_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    getArg();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024B2A94, &daTag_So_c::_create);

/* 024B2AFC */
static cPhs_State daTag_SoCreate(void* i_this) {
    WWHD_FUNC(0x024B2AFC, cPhs_State, i_this);
    return ((daTag_So_c*)i_this)->_create();
}
VERIFY(0x024B2AFC, daTag_SoCreate);

/* 024B2B00: _delete inlined */
static BOOL daTag_SoDelete(void* i_this) {
    WWHD_FUNC(0x024B2B00, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B2B00, daTag_SoDelete);

/* 024B2B08: _execute inlined */
static BOOL daTag_SoExecute(void* i_this) {
    WWHD_FUNC(0x024B2B08, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B2B08, daTag_SoExecute);

/* 024B2C70 */
static BOOL daTag_SoDraw(void* i_this) {
    WWHD_FUNC(0x024B2C70, BOOL, i_this);
    return ((daTag_So_c*)i_this)->_draw();
}
VERIFY(0x024B2C70, daTag_SoDraw);

/* 024B2D64 */
static BOOL daTag_SoIsDelete(void*) {
    WWHD_FUNC(0x024B2D64, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024B2D64, daTag_SoIsDelete);

/* 024B2CC4: static initialisation (header statics, l_HIO) */
static void __sinit_d_a_tag_so_cpp() {
    WWHD_FUNC(0x024B2CC4, void, (u32)0);
    sinit_header_statics_z(0x1046E650, 0x101D2130, 0x1046E66C);
    daTag_So_HIO_c_ct(&l_HIO);
}
VERIFY(0x024B2CC4, __sinit_d_a_tag_so_cpp);

/* 024B2D6C: daTag_So_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daTag_So_c_dt(daTag_So_c* i_this, s32 flags) {
    WWHD_FUNC(0x024B2D6C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024B2D6C, daTag_So_c_dt);
