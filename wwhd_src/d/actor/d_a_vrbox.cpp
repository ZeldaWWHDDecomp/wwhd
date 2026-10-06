/**
 * d_a_vrbox.cpp (WWHD)
 * Sky box (vr_sky): sky and horizon-haze colours, dungeon rain/thunder.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_vrbox.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD rewrote Draw like d_a_vrbox2's: daVrbox_color_set (inlined) looks materials up by name and
 * writes HD constant-colour registers, the "A_mori" stage raises the sky, and the model is drawn
 * through an HD layer with its own projection (near 0.0001, far 16000). Create first waits for
 * the "Stage" archive.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x10041CB0    /* this TU's sead::SafeString vtable */
#define VRBOX_VTBL 0x10041CF8         /* HD: vrbox_class vtable */
#define ASSURE_TERMINATION 0x024D1C40 /* this TU's SafeString::assureTerminationImpl_ */
/* 024D0B9C: this TU's sead::Color4f-from-GXColor helper (each channel / 255); it lies in front
 * of daVrbox_Draw and is verified in d_a_tsubo_mode.cpp (color_to_f4) */
#define COLOR4F_SET 0x024D0B9C

struct vrbox_class : fopAc_ac_c {
    /* 0x3AC */ u8 m290[4];
    /* 0x3B0 */ gptr<J3DModel> mpModel;
    /* 0x3B4 */ u8 m298[4];
    /* 0x3B8 */ be<u8> m29C;
};
WWHD_OFFSET(vrbox_class, mpModel, 0x3B0);
WWHD_OFFSET(vrbox_class, m29C, 0x3B8);

struct Color4f_l {
    be<f32> r, g, b, a;
};
struct SkyProjection_l {
    u8 _[0xC4];
};

/* ---- local bindings (SHARED-CANDIDATE; the same as d_a_vrbox2's) ---- */
static inline u32 envl() { return gabi::ea(dKy_getEnvlight()); }
static inline u32 play() { return dComIfGp_ea(); }
static inline s32 JUTNameTab_getIndex(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
static inline u32 dComIfG_getStageRes(const char* arc, const char* name) { return gabi::call<u32>(0x0252447C, arc, name); }
static inline u32 dStage_roomControl_getStatusRoomDt(u32 rc, s32 roomNo) { return gabi::call<u32>(0x025C11DC, rc, roomNo); }
static inline void Color4f_scaleRGB(Color4f_l* out, Color4f_l* in, f32 s) { gabi::call(0x0274D458, out, in, s); }
static inline u32 matColorReg(u32 flagsAddr, s32 i) { return gabi::call<u32>(0x027F9F0C, flagsAddr, i); }
static inline void SkyProjection_ct(SkyProjection_l* p) { gabi::call(0x027389F8, p); }
static inline void SkyProjection_set(SkyProjection_l* p, f32 a, f32 b, f32 c, f32 d) { gabi::call(0x0274E04C, p, a, b, c, d); }
static inline void SkyProjection_copy(SkyProjection_l* p, u32 src) { gabi::call(0x02738AC0, p, src); }
static inline void SkyProjection_dt(SkyProjection_l* p, s32 flags) { gabi::call(0x02738A6C, p, flags); }
static inline u32 vtbl_fn(u32 obj, u32 vtOff, u32 slot) { return gabi::load<u32>(gabi::load<u32>(obj + vtOff) + slot); }
/* d_kankyo */
static inline BOOL dKy_checkEventNightStop() { return gabi::call<BOOL>(0x02556BC0); }
static inline void dKy_change_colpat(u8 p) { gabi::call(0x0255FD48, p); }
static inline void dKyw_rain_set(s32 n) { gabi::call(0x0257E7C0, n); }
/* 02523D08 (HD): synchronise a stage archive: < 0 error, > 0 still loading, 0 ready */
static inline s32 dComIfG_syncStageRes(const char* arc) { return gabi::call<s32>(0x02523D08, arc); }

/* sead::SafeStringBase<char>::isEqual (HD inline): a literal vs the start stage name (play+0x5134) */
static bool startStageIs(const char* name) {
    gabi::Local<SafeString> lhs;
    lhs->mStringTop = gabi::ea(name);
    lhs->__vtbl = SAFESTRING_VTBL;
    u32 p = play();
    gabi::Local<SafeString> rhs;
    rhs->mStringTop = p + 0x5134;
    rhs->__vtbl = SAFESTRING_VTBL;
    gabi::call_ptr(vtbl_fn(gabi::ea(lhs.get()), 4, 0x14), lhs.get()); /* assureTerminationImpl_ */
    gabi::call_ptr(vtbl_fn(gabi::ea(lhs.get()), 4, 0x14), lhs.get()); /* cstr() */
    u32 a = lhs->mStringTop;
    gabi::call_ptr(vtbl_fn(gabi::ea(rhs.get()), 4, 0x14), rhs.get()); /* str.cstr() */
    if (a == rhs->mStringTop) {
        return true;
    }
    u32 b = rhs->mStringTop;
    a = lhs->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(a + i);
        if (c != gabi::load<u8>(b + i)) {
            return false;
        }
        if (c == 0) {
            return true;
        }
    }
    return false;
}

/* J3DModelData::getMaterialNodePointer(getMaterialName()->getIndex(name)) (HD inline) */
static u32 getMaterialByName(u32 md, const char* name) {
    gabi::Local<SafeString> s;
    s->mStringTop = gabi::ea(name);
    s->__vtbl = SAFESTRING_VTBL;
    u32 hdr = gabi::load<u32>(md);
    gabi::call(ASSURE_TERMINATION, s.get());
    s32 off = gabi::load<s32>(hdr + 0x18);
    u32 tab = off != 0 ? hdr + 0x18 + off : 0;
    s32 idx = JUTNameTab_getIndex(tab, s->mStringTop);
    if (idx < 0) {
        return 0;
    }
    u32 base = gabi::load<u32>(md + 0x10);
    if ((u32)idx < gabi::load<u32>(md + 0xC)) {
        return base + idx * 0x39C;
    }
    return base;
}

/* mat->setTevKColor(0, k) (virtual on the material's TEV block) and the HD constant colour
 * register 7 = rgb(k) * scale, alpha k.a */
static void setKColor(u32 mat, GXColor* k, f32 scale) {
    u32 tev = gabi::load<u32>(mat + 0x18);
    gabi::call_ptr(vtbl_fn(tev, 4, 0x3C), tev, 0, k);
    gabi::Local<Color4f_l> c;
    gabi::call(COLOR4F_SET, c.get(), k);
    gabi::Local<Color4f_l> s;
    Color4f_scaleRGB(s.get(), c.get(), scale);
    u32 flags = gabi::load<u32>(mat + 0xA0) | 0x80;
    gabi::store<u32>(mat + 0xA0, flags);
    u32 reg = matColorReg(mat + 0xA0, 7);
    f32 a = (f32)(u8)k->a / 255.0f;
    gabi::store<u32>(reg + 0x0, gabi::load<u32>(gabi::ea(s.get()) + 0x0)); /* lfs/stfs: a bit copy in the recompiled code */
    gabi::store<u32>(reg + 0x4, gabi::load<u32>(gabi::ea(s.get()) + 0x4));
    gabi::store<u32>(reg + 0x8, gabi::load<u32>(gabi::ea(s.get()) + 0x8));
    gabi::store<f32>(reg + 0xC, a);
}

/* daVrbox_color_set (HD, inlined into Draw) */
static void daVrbox_color_set(vrbox_class* i_this) {
    u32 e1 = envl();
    u32 e2 = envl();
    s32 sum = gabi::load<u8>(e1 + 0xBA0) + gabi::load<u8>(e2 + 0xBA1); /* mVrKasumiMaeColor */
    sum += gabi::load<u8>(envl() + 0xBA2);
    sum += gabi::load<u8>(envl() + 0xB90); /* mVrSkyColor */
    sum += gabi::load<u8>(envl() + 0xB91);
    sum += gabi::load<u8>(envl() + 0xB92);
    sum += gabi::load<u8>(envl() + 0xB98); /* mVrkumoColor */
    sum += gabi::load<u8>(envl() + 0xB99);
    sum += gabi::load<u8>(envl() + 0xB9A);
    if (sum == 0) {
        gabi::store<u8>(envl() + 0x109E, 1); /* mbVrboxInvisible */
        return;
    }
    gabi::store<u8>(envl() + 0x109E, 0);

    u32 md = gabi::ea(J3DModel_getModelData(i_this->mpModel));
    gabi::Local<GXColor> k;

    u32 mat = getMaterialByName(md, STR(0x10041D08) /* "lambert1_v_x" */);
    k->r = gabi::load<u8>(envl() + 0xBA0);
    k->g = gabi::load<u8>(envl() + 0xBA1);
    u32 e = envl();
    k->a = 0xFF;
    k->b = gabi::load<u8>(e + 0xBA2);
    f32 scale = gabi::load<f32>(envl() + 0x10E4);
    setKColor(mat, k.get(), scale);

    mat = getMaterialByName(md, STR(0x10041CA0) /* "sora_v" */);
    k->r = gabi::load<u8>(envl() + 0xB90);
    k->g = gabi::load<u8>(envl() + 0xB91);
    e = envl();
    k->a = 0xFF;
    k->b = gabi::load<u8>(e + 0xB92);
    scale = gabi::load<f32>(envl() + 0x10D4);
    setKColor(mat, k.get(), scale);
}

/* 024D0C50 */
static BOOL daVrbox_Draw(vrbox_class* i_this) {
    WWHD_FUNC(0x024D0C50, BOOL, i_this);
    J3DModel* model = i_this->mpModel;
    f32 y_origin = 0.0f;

    daVrbox_color_set(i_this);

    if (gabi::load<u8>(envl() + 0x109E) != 0) { /* g_env_light.mbVrboxInvisible */
        return TRUE;
    }

    s32 roomNo = gabi::load<s8>(0x1047E6C8); /* dComIfGp_roomControl_getStayNo() */
    if (roomNo >= 0) {
        u32 rc = dStage_roomControl_getStatusRoomDt(play() + 0x51CC, roomNo);
        if (rc != 0) {
            u32 fili = gabi::call_ptr<u32>(vtbl_fn(rc, 0, 0x1DC), rc); /* getFileListInfo() */
            if (fili != 0) {
                y_origin = gabi::load<f32>(fili + 4); /* mSeaLevel */
            }
        }
    }

    f32 y_offset;
    if (gabi::load<u32>(play() + 0x5FA4) != 0) { /* dComIfGd_getView() */
        u32 view = gabi::load<u32>(play() + 0x5FA4);
        y_offset = (gabi::load<f32>(view + 0x190) - y_origin) * 0.09f;
    } else {
        y_offset = 0.0f;
    }

    /* HD: the sky of Forest Haven is raised */
    if (startStageIs(STR(0x10041D28) /* "A_mori" */)) {
        y_offset += 30000.0f;
    }

    {
        /* mDoMtx_stack_c::transS(view->mInvViewMtx[0][3], [1][3] - y_offset, [2][3]) */
        f32 x = gabi::load<f32>(gabi::load<u32>(play() + 0x5FA4) + 0x180);
        f32 y = gabi::load<f32>(gabi::load<u32>(play() + 0x5FA4) + 0x190) - y_offset;
        f32 z = gabi::load<f32>(gabi::load<u32>(play() + 0x5FA4) + 0x1A0);
        mDoMtx_stack_c::transS(x, y, z);
    }
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    /* HD: draw through the sky layer with its own projection */
    u32 mgr = gabi::load<u32>(0x101F95D0);
    u32 n = gabi::load<u32>(mgr + 0x1020);
    u32 lp = gabi::load<u32>(mgr + 0x1024);
    if (n > 1) {
        lp += 4;
    }
    u32 layer = gabi::load<u32>(lp);
    if (layer != 0) {
        u32 lflags = gabi::load<u32>(layer + 0x50);
        u32 saved = gabi::load<u32>(layer + 0x4C);
        u32 cam;
        if (((lflags >> 12) & 1) == 0) {
            cam = saved != 0 ? saved : 0x104A20FC;
        } else {
            cam = gabi::load<u32>(layer + 0x164);
        }
        gabi::Local<SkyProjection_l> proj;
        u32 pj = gabi::ea(proj.get());
        SkyProjection_ct(proj.get());
        SkyProjection_set(proj.get(), gabi::load<f32>(cam + 0x94), gabi::load<f32>(cam + 0x98), gabi::load<f32>(cam + 0x9C),
                          gabi::load<f32>(cam + 0xAC));
        gabi::store<u32>(pj + 0xB0, gabi::load<u32>(cam + 0xB0)); /* lfs/stfs */
        gabi::store<u32>(pj + 0xB4, gabi::load<u32>(cam + 0xB4));
        gabi::store<u8>(pj + 0, 1);
        if (gabi::load<u32>(0x101FD9F0) == 0) { /* function-local static RTTI */
            gabi::store<u32>(0x101FD9F0, 1);
            gabi::store<u32>(0x101FDCD4, 0x10041CE8);
        }
        if (gabi::call_ptr<u32>(vtbl_fn(cam, 0x90, 0xC), cam, 0x101FDCD4) != 0 && cam != 0) {
            SkyProjection_copy(proj.get(), cam + 0xB8);
        }
        gabi::store<f32>(pj + 0x94, 0.0001f); /* near */
        gabi::store<f32>(pj + 0x98, 16000.0f); /* far */
        gabi::store<u8>(pj + 0, 1);
        gabi::store<u32>(layer + 0x4C, pj);

        /* dComIfGd_setListSky() */
        gabi::store<u32>(0x104B4634, gabi::load<u32>(play() + 0x5D4C));
        gabi::store<u32>(0x104B4638, gabi::load<u32>(play() + 0x5D50));
        mDoExt_modelUpdateDL(model);

        gabi::store<u32>(layer + 0x4C, saved);
        SkyProjection_dt(proj.get(), 2);
    }

    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x024D0C50, daVrbox_Draw);

/* dungeon_rain_proc (inlined in Execute) */
static inline void dungeon_rain_proc() {
    u32 env_light = envl();
    u8 mode = 0;
    s32 roomNo = gabi::load<s8>(0x1047E6C8); /* dComIfGp_roomControl_getStayNo() */

    if (!dKy_checkEventNightStop()) {
        return;
    }

    if (startStageIs(STR(0x10041C84) /* "M_NewD2" */)) {
        if (roomNo == 3) {
            mode = 1;
        }
    } else if (startStageIs(STR(0x10041C8C) /* "M_Dra09" */)) {
        mode = 1;
    } else if (startStageIs(STR(0x10041C98) /* "kinMB" */)) {
        mode = 1;
    } else if (startStageIs(STR(0x10041CA8) /* "kindan" */)) {
        if (roomNo == 2 || roomNo == 13) {
            mode = 1;
        } else if (roomNo == 4) {
            mode = 2;
        }
    } else {
        return;
    }

    if (mode == 1) { /* rain and thunder */
        if (gabi::load<s32>(env_light + 0x106C) != 250) { /* mRainCountOrig */
            dKy_change_colpat(1);
            dKyw_rain_set(250);
            gabi::store<s32>(envl() + 0xAB4, 1); /* mThunderEff.mMode */
        }
    } else if (mode == 2) { /* thunder, no rain */
        if (gabi::load<s32>(envl() + 0xAB4) == 0) {
            dKy_change_colpat(1);
            gabi::store<s32>(envl() + 0xAB4, 0xA);
        }
    } else { /* no rain or thunder */
        if (gabi::load<s32>(envl() + 0xAB4) != 0) {
            dKyw_rain_set(0);
            gabi::store<s32>(envl() + 0xAB4, 0);
        }
    }
}

/* 024D15BC */
static BOOL daVrbox_Execute(vrbox_class* i_this) {
    WWHD_FUNC(0x024D15BC, BOOL, i_this);
    dungeon_rain_proc();
    return TRUE;
}
VERIFY(0x024D15BC, daVrbox_Execute);

/* 024D1988 */
static BOOL daVrbox_IsDelete(vrbox_class* i_this) {
    WWHD_FUNC(0x024D1988, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024D1988, daVrbox_IsDelete);

/* 024D1990 */
static BOOL daVrbox_Delete(vrbox_class* i_this) {
    WWHD_FUNC(0x024D1990, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024D1990, daVrbox_Delete);

/* 024D1998 (HD: "vr_sky.bmd") */
static BOOL daVrbox_solidHeapCB(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x024D1998, BOOL, i_actor);
    vrbox_class* i_this = static_cast<vrbox_class*>(i_actor);

    u32 modelData = dComIfG_getStageRes(STR(0x10041D30) /* "Stage" */, STR(0x10041D48) /* "vr_sky.bmd" */);
    if (modelData == 0) /* JUT_ASSERT(0x219, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10041D38), 0x219, STR(0x10041D54));

    i_this->mpModel = mDoExt_J3DModel__create(gabi::at<J3DModelData>(modelData), 0x80000, 0x11020202);

    bool success = FALSE;
    if (modelData != 0 && i_this->mpModel != nullptr) {
        success = TRUE;
    }
    return success;
}
VERIFY(0x024D1998, daVrbox_solidHeapCB);

/* 024D1A30 */
static cPhs_State daVrbox_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x024D1A30, cPhs_State, i_actor);
    /* HD: wait for the stage archive first */
    s32 rt = dComIfG_syncStageRes(STR(0x10041D68) /* "Stage" */);
    if (rt < 0) {
        return cPhs_ERROR_e;
    }
    if (rt > 0) {
        return cPhs_INIT_e;
    }

    /* fopAcM_ct(i_actor, vrbox_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            fopAc_ac_c_ct(i_actor);
            i_actor->__vtbl = VRBOX_VTBL;
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    vrbox_class* i_this = static_cast<vrbox_class*>(i_actor);

    cPhs_State phase_state = cPhs_COMPLEATE_e;
    i_this->m29C = 0;
    if (fopAcM_entrySolidHeap(i_this, 0x024D1998 /* daVrbox_solidHeapCB */, 0xC60)) {
        u32 st = play() + 0x5ACA; /* dComIfGp_onStatus(1) */
        gabi::store<u16>(st, (u16)(gabi::load<u16>(st) | 1));
        gabi::store<u8>(envl() + 0x109E, 0); /* g_env_light.mbVrboxInvisible */
    } else {
        phase_state = cPhs_ERROR_e;
    }
    return phase_state;
}
VERIFY(0x024D1A30, daVrbox_Create);

/* 024D1B44: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_vrbox_cpp() {
    WWHD_FUNC(0x024D1B44, void, (u32)0);
    sinit_header_statics(0x1046EAA0, 0x101D2B8C);
}
VERIFY(0x024D1B44, __sinit_d_a_vrbox_cpp);

/* 024D1BD8: sead::SafeString deleting destructor (this TU's vtable 0x10041CB0) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024D1BD8, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x024D1BD8, SafeString_dt);

/* 024D1BEC: vrbox_class deleting destructor (compiler-generated, HD virtual destructor) */
static void vrbox_class_dt(vrbox_class* i_this, s32 flags) {
    WWHD_FUNC(0x024D1BEC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024D1BEC, vrbox_class_dt);

/* 024D1C40: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024D1C40, void, (u32)0);
}
VERIFY(0x024D1C40, SafeString_assureTerminationImpl);
