/**
 * d_a_ship_tail.cpp (WWHD)
 * King of Red Lions: static initialisation, the sight packet's draw, constructors and destructors
 * the compiler emits for the translation unit's members.
 *
 * Verified against cking.rpx.
 */
#include "d/actor/d_a_ship.h"

/* 0247D270: HD-only. The cannon-sight packet's draw (J3DPacket virtual): only in pass 2 of the
 * draw info; draws the model's current shape packet and the 25 sight segments with a fresh draw state. */
static void daShip_sightPacket_drawVirtual(u8* pkt, u8* info) {
    WWHD_FUNC(0x0247D270, void, pkt, info);
    if (gabi::load<u32>(gabi::ea(info) + 0xC) != 2) {
        return;
    }
    gabi::Local<u8[0x11C]> st;
    u32 S = gabi::ea(st.get());
    gabi::call(0x02750250, S);
    u32 f = gabi::load<u32>(S + 0xEC);
    gabi::store<u32>(S + 0xC, 0);
    gabi::store<u32>(S + 8, 0);
    gabi::store<u32>(S + 0xEC, (((f & ~0xFu) + 7) & ~0xF0u) + 0x10);
    gabi::call(0x02750370, S);
    u32 p = gabi::ea(pkt);
    u32 model = gabi::load<u32>(p + 0x98);
    if (model != 0 && gabi::load<u32>(p + 0x9C) != 0 && gabi::load<u32>(p + 0xA0) != 0) {
        u32 idx = gabi::load<u16>(gabi::load<u32>(model + 0x134));
        u32 shp = gabi::load<u32>(model + 0x12C);
        if (idx < gabi::load<u32>(model + 0x128)) {
            shp += idx * 0xAC;
        }
        gabi::call(0x027F1FA8, shp, info);
        for (int i = 0; i < 25; i++) {
            u32 m = gabi::load<u32>(p + 0x98);
            u32 seg = gabi::load<u32>(p + 0xA0) + i * 0xA8;
            u32 j = gabi::load<u16>(gabi::load<u32>(m + 0x134));
            u32 s2 = gabi::load<u32>(m + 0x12C);
            if (j < gabi::load<u32>(m + 0x128)) {
                s2 += j * 0xAC;
            }
            gabi::call(0x027F26C4, s2, seg);
        }
    }
    gabi::call(0x02750370, 0x104B474C);
}
VERIFY(0x0247D270, daShip_sightPacket_drawVirtual);

/* 0247BA34: createHeap callback (CheckCreateHeap) */
static BOOL daShip_CheckCreateHeap(daShip_c* i_this) {
    WWHD_FUNC(0x0247BA34, BOOL, i_this);
    return i_this->createHeap();
}
VERIFY(0x0247BA34, daShip_CheckCreateHeap);

/* 0247D26C */
static s32 daShip_Create(daShip_c* i_this) {
    WWHD_FUNC(0x0247D26C, s32, i_this);
    return i_this->create();
}
VERIFY(0x0247D26C, daShip_Create);

/* 02482C1C: __sinit: the header statics and l_rope_base_vec(0.0f, -10.0f, 0.0f) */
static void __sinit_d_a_ship_cpp() {
    WWHD_FUNC(0x02482C1C, void);
    sinit_header_statics(0x1046DCC4, 0x101D0484);
    gabi::store<f32>(0x1046DCE0, 0.0f);
    gabi::store<f32>(0x1046DCE4, -10.0f);
    gabi::store<f32>(0x1046DCE8, 0.0f);
}
VERIFY(0x02482C1C, __sinit_d_a_ship_cpp);

/* deleting destructors of classes with nothing to destroy (sead::SafeString and friends) */
static void daShip_emptyDt0(void* p, s32 flags) {
    WWHD_FUNC(0x02482CD0, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02482CD0, daShip_emptyDt0);
static void daShip_emptyDt1(void* p, s32 flags) {
    WWHD_FUNC(0x02482E1C, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x02482E1C, daShip_emptyDt1);
static void daShip_emptyDt2(void* p, s32 flags) {
    WWHD_FUNC(0x024832A4, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x024832A4, daShip_emptyDt2);
static void daShip_emptyDt3(void* p, s32 flags) {
    WWHD_FUNC(0x024832B8, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x024832B8, daShip_emptyDt3);
static void daShip_emptyDt4(void* p, s32 flags) {
    WWHD_FUNC(0x024832CC, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x024832CC, daShip_emptyDt4);

/* 02482CE4: sead::SafeString::assureTerminationImpl_ of a fixed buffer: buffer[size - 1] = 0 */
static void daShip_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02482CE4, void, p);
    u32 s = gabi::ea(p);
    gabi::store<u8>(gabi::load<u32>(s) + gabi::load<u32>(s + 8) - 1, 0);
}
VERIFY(0x02482CE4, daShip_SafeString_assureTermination);

/* 02482CFC: sead::FormatFixedSafeString<32>(fmt, ...) (0x2C bytes): empty buffer, then format */
static void* daShip_FormatFixedSafeString_ct(void* p, u32 fmt, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    WWHD_FUNC(0x02482CFC, void*, p, fmt, a5, a6, a7, a8, a9, a10);
    if (p == nullptr) {
        p = operator_new(0x2C);
        if (p == nullptr) {
            return p;
        }
    }
    u32 s = gabi::ea(p);
    gabi::store<u32>(s, s + 0xC);
    gabi::store<u32>(s + 4, 0x1003A204);
    gabi::store<u32>(s + 8, 0x20);
    gabi::store<u8>(s + 0x2B, 0);
    gabi::store<u32>(s + 4, 0x1003A21C);
    gabi::store<u8>(gabi::load<u32>(s), 0);
    gabi::store<u32>(s + 4, 0x1003A2FC);
    /* the va_list (frame +0x8) and the register save area (frame +0x18), as in the original frame:
     * two integer arguments taken (this, fmt), no float ones; the overflow area is the caller's
     * argument area (frame +0x90) */
    gabi::Local<u8[0x30]> frame;
    u32 v = gabi::ea(frame.get());
    u32 sv = v + 0x10;
    gabi::store<u32>(sv + 0x00, s);
    gabi::store<u32>(sv + 0x04, fmt);
    gabi::store<u32>(sv + 0x08, a5);
    gabi::store<u32>(sv + 0x0C, a6);
    gabi::store<u32>(sv + 0x10, a7);
    gabi::store<u32>(sv + 0x14, a8);
    gabi::store<u32>(sv + 0x18, a9);
    gabi::store<u32>(sv + 0x1C, a10);
    gabi::store<u8>(v, 2);
    gabi::store<u8>(v + 1, 0);
    gabi::store<u32>(v + 4, v + 0x88);
    gabi::store<u32>(v + 8, sv);
    gabi::call(0x02759C10 /* sead::BufferedSafeString::formatV */, s, fmt, v);
    return p;
}
VERIFY(0x02482CFC, daShip_FormatFixedSafeString_ct);

/* 02482E30: HD sight segment (0xA8 bytes): J3DMatPacket-like base, vtable, matrix at +0x74 */
static void* daShip_sightSeg_ct(void* p) {
    WWHD_FUNC(0x02482E30, void*, p);
    if (p == nullptr) {
        p = operator_new(0xA8);
        if (p == nullptr) {
            return p;
        }
    }
    u32 s = gabi::ea(p);
    gabi::call(0x027FB40C, s);
    gabi::store<u32>(s + 0xC, 0x1016EF84);
    gabi::call(0x028F521C /* memset 0 */, s + 0x74, 0x34);
    if (s + 0x74 == 0) {
        operator_new(0x30);
    }
    return p;
}
VERIFY(0x02482E30, daShip_sightSeg_ct);

/* 02482EA0: dCcD_Cyl constructor (this TU's cM3dGAab vtable) */
static dCcD_Cyl* daShip_dCcD_Cyl_ct(dCcD_Cyl* p) {
    WWHD_FUNC(0x02482EA0, dCcD_Cyl*, p);
    if (p == nullptr) {
        p = (dCcD_Cyl*)operator_new(0x130);
        if (p == nullptr) {
            return p;
        }
    }
    dCcD_Cyl_ct(p, 0x1003A24C);
    return p;
}
VERIFY(0x02482EA0, daShip_dCcD_Cyl_ct);

/* 02482F2C: dCcD_Stts constructor */
static dCcD_Stts* daShip_dCcD_Stts_ct(dCcD_Stts* p) {
    WWHD_FUNC(0x02482F2C, dCcD_Stts*, p);
    if (p == nullptr) {
        p = (dCcD_Stts*)operator_new(0x3C);
        if (p == nullptr) {
            return p;
        }
    }
    dCcD_Stts_ct(p);
    return p;
}
VERIFY(0x02482F2C, daShip_dCcD_Stts_ct);

/* 02482F94: constructor of an empty 0xC-byte object */
static void* daShip_empty_ct(void* p) {
    WWHD_FUNC(0x02482F94, void*, p);
    if (p == nullptr) {
        p = operator_new(0xC);
    }
    return p;
}
VERIFY(0x02482F94, daShip_empty_ct);

/* 02482FC0: dBgS_AcchCir deleting destructor (its cM3dGCir at +0x14) */
static void daShip_AcchCir_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02482FC0, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x02018034 /* cM3dGCir::~cM3dGCir */, gabi::ea(p) + 0x14, 2);
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x02482FC0, daShip_AcchCir_dt);

/* 02483014: HD sight segment deleting destructor */
static void daShip_sightSeg_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02483014, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027FB528, p, 0);
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x02483014, daShip_sightSeg_dt);

/* the sight packet's destructor body: the model (+0x9C) and the segment array (+0xA0) */
static inline void daShip_sightPacket_dtBody(u32 s) {
    gabi::store<u32>(s + 0xC, 0x1003A7F8);
    if (gabi::load<u32>(s + 0x9C) != 0) {
        gabi::call(0x027FD764, gabi::load<u32>(s + 0x9C), 3);
        gabi::store<u32>(s + 0x9C, 0);
    }
    if (gabi::load<u32>(s + 0xA0) != 0) {
        gabi::call(0x028F0164 /* __destroy_arr */, gabi::load<u32>(s + 0xA0), -1, 0xA8, 0x02483014, 1, 0);
        gabi::store<u32>(s + 0xA0, 0);
    }
    gabi::call(0x027F13DC /* J3DPacket::~J3DPacket */, s, 0);
}

/* 02483068: HD sight packet deleting destructor */
static void daShip_sightPacket_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02483068, void, p, flags);
    if (p != nullptr) {
        daShip_sightPacket_dtBody(gabi::ea(p));
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x02483068, daShip_sightPacket_dt);

/* 02483118, 024832A0: empty functions */
static void daShip_empty0() {
    WWHD_FUNC(0x02483118, void);
}
VERIFY(0x02483118, daShip_empty0);
static void daShip_empty1() {
    WWHD_FUNC(0x024832A0, void);
}
VERIFY(0x024832A0, daShip_empty1);

/* 0248311C: daShip_c deleting destructor */
static void daShip_c_dt(daShip_c* p, s32 flags) {
    WWHD_FUNC(0x0248311C, void, p, flags);
    if (p == nullptr) {
        return;
    }
    u32 s = gabi::ea(p);
    gabi::call(0x028F0164, s + 0x39F8, 0x80, 0x12C, 0x02515AE8 /* dCcD_Sph dtor */, 0, 0);
    gabi::call(0x028F0164, s + 0x1BF8, 0x80, 0x3C, 0x02515860 /* dCcD_Stts dtor */, 0, 0);
    daShip_sightPacket_dtBody(s + 0x1B54);
    gabi::call(0x02515AE8, s + 0x19F8, 2);
    gabi::call(0x028F0164, s + 0x1668, 3, 0x130, 0x02515A70 /* dCcD_Cyl dtor */, 0, 0);
    gabi::call(0x02515860, s + 0x162C, 2);
    gabi::store<u32>(s + 0x1488, 0x1003A28C);
    gabi::store<u32>(s + 0x147C, 0x1003A29C);
    gabi::call(0x024EFD9C /* dBgS_Acch::~dBgS_Acch */, s + 0x1468, 0);
    gabi::call(0x028F0164, s + 0x1368, 4, 0x40, 0x02482FC0, 0, 0);
    gabi::call(0x025EB8B8 /* mDoExt_3DlineMat1_c dtor */, s + 0x424, 2);
    gabi::call(0x025D50BC /* fopAc_ac_c::~fopAc_ac_c */, s, 0);
    if (flags & 1) operator_delete(p);
}
VERIFY(0x0248311C, daShip_c_dt);

/* ---- leftover functions of the translation unit ---- */

/* 024832E0 daShip_c::initStartPos (d_a_ship_static.cpp on GameCube; called from other actors' demos):
 * current/old position, both angles, clear the 0x10 state flag, gravity -2.5, stop the effect callbacks.
 * HD: the two follow callbacks (+0x148/+0x15C) are stopped through their vtable (slot +0x44). */
static void daShip_initStartPos(void* ship, void* pos, s16 rotY) {
    WWHD_FUNC(0x024832E0, void, ship, pos, rotY);
    u32 s = gabi::ea(ship), p = gabi::ea(pos);
    gabi::store<u32>(s + 0x314, gabi::load<u32>(p));
    gabi::store<u32>(s + 0x318, gabi::load<u32>(p + 4));
    gabi::store<u32>(s + 0x31C, gabi::load<u32>(p + 8));
    gabi::store<u32>(s + 0x300, gabi::load<u32>(p));
    gabi::store<u32>(s + 0x304, gabi::load<u32>(p + 4));
    u32 z = gabi::load<u32>(p + 8);
    u32 flg = gabi::load<u32>(s + 0x644);
    gabi::store<f32>(s + 0x374, -2.5f);
    gabi::store<u32>(s + 0x308, z);
    gabi::store<s16>(s + 0x32A, rotY);
    gabi::store<s16>(s + 0x322, rotY);
    gabi::store<u32>(s + 0x644, flg & ~0x10u);
    u32 cb = s + 0xCFF8;
    gabi::call(0x025A92C0, cb + 0x64); /* mWaveL.remove() */
    gabi::call(0x025A92C0, cb);        /* mWaveR.remove() */
    gabi::call(0x025A99B8, cb + 0xC8); /* mSplash.remove() */
    gabi::call(0x025A9E38, cb + 0xE4); /* mTrack.remove() */
    gabi::call(0x025A9270, cb + 0x134); /* mRipple.end() */
    gabi::call(gabi::load<u32>(gabi::load<u32>(cb + 0x148) + 0x44), cb + 0x148);
    gabi::call(gabi::load<u32>(gabi::load<u32>(cb + 0x15C) + 0x44), cb + 0x15C);
    gabi::call(0x025A5AC8, cb + 0x170); /* dPa_followEcallBack::end */
    gabi::call(0x025A9270, cb + 0x184); /* dPa_rippleEcallBack::end */
}
VERIFY(0x024832E0, daShip_initStartPos);

/* 024833BC __sinit_d_a_ship_static_cpp (ctor list 1018B480): the header statics only */
static void __sinit_d_a_ship_static_cpp() {
    WWHD_FUNC(0x024833BC, void);
    sinit_header_statics(0x1046DDEC, 0x101D04D8);
}
VERIFY(0x024833BC, __sinit_d_a_ship_static_cpp);
