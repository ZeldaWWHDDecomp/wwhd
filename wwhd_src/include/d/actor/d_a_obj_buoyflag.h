/* daObjBuoyflag (flag on a buoy / barrel), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" (every function is a stub). The HD actor keeps the GameCube
 * cloth simulation (Packet_c: 5x7 vertices, double-buffered positions and normals) but draws
 * through HD GPU objects (vertex buffers, shader/uniform blocks, a texture + sampler for the
 * "Cloth" archive), so Packet_c grew from 0xE00 to 0x2C64 bytes and Act_c from 0x112C to 0x3230.
 * Offsets below are measured from the WWHD code (_create, the calc/draw functions). */
#pragma once
#include "bindings.h"

namespace daObjBuoyflag {

struct Act_c;

/* the cloth: 5 rows (j) x 7 columns (i) */
enum { ROWS = 5, COLS = 7, VTX = ROWS * COLS };

struct ClothBuf {
    /* 0x000 */ cXyz mPos[ROWS][COLS];
    /* 0x1A4 */ cXyz mNrm[ROWS][COLS];      /* front normals (init: cXyz::BaseZ) */
    /* 0x348 */ cXyz mNrmBack[ROWS][COLS];  /* back normals = -front (init: (0, 0, -1)) */
};
WWHD_SIZE(ClothBuf, 0x4EC);

struct Packet_c {
    /* 0x000 */ u8 _000[0xC];               /* J3DPacket (HD, constructor 027F1278) */
    /* 0x00C */ be<u32> __vtbl;             /* 10026860 */
    /* 0x010 */ u8 _010[0x18 - 0x10];
    /* 0x018 */ gptr<Act_c> mpActor;
    /* 0x01C */ u8 _01C[0x98 - 0x1C];
    /* 0x098 */ gptr<dKy_tevstr_c> mpTevStr; /* set by update() */
    /* 0x09C */ ClothBuf mBuf[2];
    /* 0xA74 */ cXyz mSpd[ROWS][COLS];
    /* 0xC18 */ be<s32> mCurBuf;
    /* 0xC1C */ Mtx34 mMtxHasi;              /* pole: setup matrix * actor scale */
    /* 0xC4C */ Mtx34 mMtxHata;              /* flag: ... * trans(0, 60, 0) */
    /* 0xC7C */ Mtx34 mViewMtxHasi;          /* view * mMtxHasi (update) */
    /* 0xCAC */ Mtx34 mViewMtxHata;          /* view * mMtxHata */
    /* 0xCDC */ cXyz mUp;                    /* BaseY in flag space */
    /* 0xCE8 */ cXyz mWind;                  /* wind in flag space */
    /* 0xCF4 */ be<s16> mAng[12];
    /* 0xD0C */ cXyz mSpring;                /* force accumulator (calc_pos) */
    /* 0xD18 */ u8 _D18[0x2C64 - 0xD18];    /* HD GPU objects (raw offsets in the source) */

    void draw_hata(Act_c*);
    void draw_hasi(Act_c*);
    void draw(Act_c*);
    void init(Act_c*);
    void calc_wind_base(Act_c*);
    void calc_pos_spring_near(const cXyz*, const cXyz*, f32, f32);
    void calc_pos(Act_c*);
    void calc_nrm();
    void calc(Act_c*);
    void update(Act_c*);
    /* HD-only */
    void load_texture();
    void update_hata();
    void update_hasi();
    void gpu_init_hata();
    void gpu_init_hasi();
};
WWHD_OFFSET(Packet_c, mBuf, 0x9C);
WWHD_OFFSET(Packet_c, mSpd, 0xA74);
WWHD_OFFSET(Packet_c, mMtxHasi, 0xC1C);
WWHD_OFFSET(Packet_c, mAng, 0xCF4);
WWHD_OFFSET(Packet_c, mSpring, 0xD0C);
WWHD_SIZE(Packet_c, 0x2C64);

/* L_attr_type[4] (0x100267F0): {f32 scale, u8 cc, u8 hata, u8, u8} */
struct Attr_c {
    /* 0x0 */ be<f32> mScale;
    /* 0x4 */ be<u8> mCc;
    /* 0x5 */ be<u8> mHata;
    /* 0x6 */ u8 _6[2];
};

struct Act_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ dCcD_Stts mStts;
    /* 0x3F0 */ dCcD_Cyl mCyl;
    /* 0x520 */ Packet_c mPacket;
    /* 0x3184 */ Mtx34 mMtx;                 /* GameCube m1090 (setup) */
    /* 0x31B4 */ Mtx34 mOldMtx;              /* last frame's */
    /* 0x31E4 */ Mtx34 mJumpMtx;             /* base rotation of mode_jumpToSea (GameCube m10F0) */
    /* 0x3214 */ be<u32> mType;              /* M_type (GameCube m1120) */
    /* 0x3218 */ be<s32> mMode;              /* GameCube m1124 */
    /* 0x321C */ be<u8> mbInit;              /* GameCube m1128: mode_jumpToSea first frame */
    /* 0x321D */ u8 _321D;
    /* 0x321E */ be<s16> mRotAngle;
    /* 0x3220 */ be<f32> mRotSpeed;
    /* 0x3224 */ cXyz mRotAxis;

    BOOL mode_afl();
    BOOL mode_jumpToSea();
    void mtx_init();
};
WWHD_OFFSET(Act_c, mPacket, 0x520);
WWHD_OFFSET(Act_c, mMtx, 0x3184);
WWHD_OFFSET(Act_c, mType, 0x3214);
WWHD_OFFSET(Act_c, mRotAxis, 0x3224);
WWHD_SIZE(Act_c, 0x3230);

/* L_attr_type with the inline's JUT_ASSERT (each inline instance has its own literals) */
inline Attr_c* attr_type(Act_c* a, u32 file, u32 msg) {
    u32 t = a->mType;
    if (t >= 4) {
        JUT_ASSERT_fail(STR(file), 0xA5A, STR(msg)); /* (M_type >= 0) && (M_type < Type_Max) */
        t = a->mType;
    }
    return gabi::at<Attr_c>(0x100267F0 + 8 * t);
}

/* 0232EDC4 daObj::PrmAbstract<Act_c::Prm_e> */
u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift);

}  // namespace daObjBuoyflag

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define cXyz_BaseY gabi::at<cXyz>(0x101FFBC0)
#define cXyz_BaseZ gabi::at<cXyz>(0x101FFBCC)
static inline void cXyz_dv(const cXyz* a, cXyz* res, f32 s) { gabi::call(0x0201AEAC, a, res, s); }
static inline void cXyz_outprod(const cXyz* a, cXyz* res, const cXyz* b) { gabi::call(0x0201B080, a, res, b); }
static inline BOOL cXyz_normalizeRS(cXyz* a) { return gabi::call<BOOL>(0x0201B47C, a); }
static inline void cXyz_normalize(cXyz* a, cXyz* res) { gabi::call(0x0201B31C, a, res); }
static inline void PSMTXInverse(const Mtx34* a, Mtx34* b) { gabi::call(0x028E91EC, a, b); }
static inline void PSMTXConcat_l(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void mDoMtx_ZXYrotS(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F1AA4, m, x, y, z); }
static inline void C_QUATRotAxisRad(u32 q, const cXyz* axis, f32 rad) { gabi::call(0x028E9B14, q, axis, rad); }
static inline void mDoMtx_stack_quatM(u32 q) { gabi::call(0x025F25CC, q); }
static inline u32 mDoMtx_quatStack_get() { return gabi::load<u32>(0x1048D3FC); }
static inline void fopAcM_setCullSizeSphere(fopAc_ac_c* a, f32 x, f32 y, f32 z, f32 r) { gabi::call(0x025D6768, a, x, y, z, r); }
/* 0201864C cM3dGCyl::Set(const cXyz& center, f32 r, f32 h) */
static inline void cM3dGCyl_Set(cM3dGCyl* c, const cXyz* center, f32 r, f32 h) { gabi::call(0x0201864C, c, center, r, h); }
/* 0257E34C dKyw_get_AllWind_vecpow(cXyz* pos): cXyz through a hidden result pointer */
static inline void dKyw_get_AllWind_vecpow(cXyz* res, cXyz* pos) { gabi::call(0x0257E34C, res, pos); }
/* bits of a float as an lfs/stfs pair (or an FPR round trip) leaves them: SNaN quieted */
static inline u32 fbits(u32 a) {
    f32 f = gabi::load<f32>(a);
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
static inline u32 fbits_of(f32 f) {
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
static inline void copy3_u32(u32 dst, u32 src) {
    for (u32 i = 0; i < 12; i += 4) gabi::store<u32>(dst + i, gabi::load<u32>(src + i));
}
