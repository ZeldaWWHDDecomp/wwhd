/**
 * d_a_bg.cpp (WWHD)
 * Room background models (BG)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bg.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * WWHD range 02077E30..0207A9BF (02077D9C/02077E2C belong to d_a_bflower). daBg_c::createHeap is in
 * d_a_bg_heap.cpp.
 */
#include "d/actor/d_a_bg.h"

#define ARCNAME_BUF 0x104619F8u /* setArcName's static char arcName[32] */
#define J3DUClipper_p gabi::at<void>(0x1048CFF0)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 028F186C sprintf (varargs) */
static inline s32 sprintf_g(u32 buf, u32 fmt, u32 v) { return gabi::call<s32>(0x028F186C, buf, fmt, v); }
static inline void* mDoExt_btkAnm_new() { return gabi::call<void*>(0x025E7C6C, (u32)0); } /* new mDoExt_btkAnm() */
static inline void* mDoExt_brkAnm_new() { return gabi::call<void*>(0x025E80D0, (u32)0); } /* new mDoExt_brkAnm() */
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, J3DAnmTevRegKey* k, bool play, s32 mode, f32 speed, s16 start,
                                      s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, k, play, mode, speed, start, end, modify, entry);
}
static inline void mDoExt_brkAnm_entry_l(void* a, J3DModelData* d, f32 frame) { gabi::call(0x025E83FC, a, d, frame); }
static inline void dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
/* 02520630 dComIfGp_getMapTrans(roomNo, f32* x, f32* z, s16* angle) */
static inline BOOL dComIfGp_getMapTrans(s32 roomNo, be<f32>* x, be<f32>* z, be<s16>* a) { return gabi::call<BOOL>(0x02520630, roomNo, x, z, a); }
static inline void dStage_escapeRestart() { gabi::call(0x025C3ED0); }
static inline void J3DUClipper_calcViewFrustum(void* c) { gabi::call(0x0283801C, c); }
static inline void J3DUClipper_clip(void* c, J3DModel* m) { gabi::call(0x02838524, c, m); }

/* 02077E30 */
static u32 daBg_setArcName(daBg_c* i_this) {
    WWHD_FUNC(0x02077E30, u32, i_this);
    sprintf_g(ARCNAME_BUF, 0x10008850 /* "Room%lu" */, fopAcM_GetParam(i_this));
    return ARCNAME_BUF;
}
VERIFY(0x02077E30, daBg_setArcName);

/* 02077E74 (HD: no createMatAnm loop) */
static BOOL daBg_btkAnm_create(daBg_btkAnm_c* i_this, J3DModelData* modelData, J3DAnmTextureSRTKey* anmData) {
    WWHD_FUNC(0x02077E74, BOOL, i_this, modelData, anmData);
    mDoExt_btkAnm* anm = (mDoExt_btkAnm*)mDoExt_btkAnm_new();
    i_this->anm = anm;
    if (anm == nullptr)
        return FALSE;
    if (!anm->init(modelData, anmData, true, 2 /* EMode_LOOP */, 1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}
VERIFY(0x02077E74, daBg_btkAnm_create);

/* 02077F20 (HD: no createMatAnm loops) */
static BOOL daBg_brkAnm_create(daBg_brkAnm_c* i_this, J3DModelData* modelData, J3DAnmTevRegKey* anmData) {
    WWHD_FUNC(0x02077F20, BOOL, i_this, modelData, anmData);
    void* anm = mDoExt_brkAnm_new();
    i_this->anm = (mDoExt_btkAnm*)anm;
    if (anm == nullptr)
        return FALSE;
    if (!mDoExt_brkAnm_init(anm, modelData, anmData, true, 2, 1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}
VERIFY(0x02077F20, daBg_brkAnm_create);

/* 02079D04 */
static BOOL checkCreateHeap(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x02079D04, BOOL, i_ac);
    return gabi::call<u32>(0x02077FCC, i_ac); /* ((daBg_c*)i_ac)->createHeap(): tail call */
}
VERIFY(0x02079D04, checkCreateHeap);

/* btkAnm/brkAnm entryFrame (HD inline): the frame goes to the animation, then the HD animation
 * object at +off is evaluated through its function pointer */
static inline void anm_entryFrame(u32 anm, u32 frameDst, u32 off) {
    f32 frame = gabi::load<f32>(anm + 4);
    gabi::store<f32>(frameDst, frame);
    u32 obj = gabi::load<u32>(anm + off);
    u32 fn = gabi::load<u32>(obj + 0x10);
    f32 p2 = gabi::load<f32>(obj + 8);
    f32 p1 = gabi::load<f32>(obj + 4);
    u32 ctx = gabi::load<u32>(obj + 0x14);
    f32 r = gabi::call_ptr<f32>(fn, ctx, frame, p1, p2);
    gabi::store<f32>(obj, r);
    gabi::call(0x027DF40C, anm + off);
}

/* 02079D08: daBg_c::draw (inline) */
static BOOL daBg_Draw(daBg_c* i_this) {
    WWHD_FUNC(0x02079D08, BOOL, i_this);
    s32 roomNo = fopAcM_GetParam(i_this);
    daBg_BgModel* bgm = &i_this->bg[0];

    dComIfGd_setListBG();
    /* HD: in Omori, models 1 and 3 are drawn in the reverse order */
    bool omori = daBg_isStage(0x10008864 /* "Omori" */);
    gabi::store<f32>(gabi::ea(J3DUClipper_p) + 0x54, 100000.0f); /* mDoLib_clipper::changeFar(100000.0f) */
    J3DUClipper_calcViewFrustum(J3DUClipper_p);

    for (s32 i = 0; i < 4; i++, bgm++) {
        if (omori) {
            if (i == 1) {
                i = 3;
                bgm = &i_this->bg[3];
            } else if (i == 3) {
                bgm = &i_this->bg[1];
                i = 1;
            }
        }
        J3DModel* model = bgm->model;
        if (model != nullptr) {
            daBg_btkAnm_c* btk = bgm->btk;
            if (btk != nullptr) {
                u32 anm = gabi::ea((mDoExt_btkAnm*)btk->anm);
                anm_entryFrame(anm, gabi::load<u32>(anm + 0x68), 0x10);
            }
            daBg_brkAnm_c* brk = bgm->brk;
            if (brk != nullptr) {
                u32 anm = gabi::ea((mDoExt_btkAnm*)brk->anm);
                anm_entryFrame(anm, gabi::load<u32>(anm + 0x10), 0x20);
            }
            J3DModel_calc(model);
            J3DUClipper_clip(J3DUClipper_p, model);
            dScnKy_env_light_c* light = dKy_getEnvlight();
            settingTevStruct(light, TEV_TYPE_BG0 + i, nullptr, bgm->mpTevStr);
            light = dKy_getEnvlight();
            setLightTevColorType(light, model, bgm->mpTevStr);
            if (i == 1) {
                gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5DA0));
                gabi::call(0x0255F8A0);
            }
            if (bgm->mFlag != 0 && gabi::load<u8>(gabi::load<u32>(0x101F86E8) + 0x1688) != 0) {
                gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D4C));
                gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D50));
            }
            mDoExt_modelEntryDL(model);
            if (i == 1) {
                gabi::call(0x0255F84C);
                dComIfGd_setListBG();
            }
            if (bgm->mFlag != 0) {
                dComIfGd_setListBG();
            }
        }
        if (omori) {
            if (i == 3) {
                i = 1;
                bgm = &i_this->bg[1];
            } else if (i == 1) {
                bgm = &i_this->bg[3];
                i = 3;
            }
        }
    }

    gabi::store<f32>(gabi::ea(J3DUClipper_p) + 0x54, gabi::load<f32>(0x1048D04C)); /* mDoLib_clipper::resetFar() */
    J3DUClipper_calcViewFrustum(J3DUClipper_p);
    dComIfGd_setList();
    dComIfGp_get();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, nullptr, gabi::at<dKy_tevstr_c>(daBg_roomStatus(roomNo) + 0x54));
    return TRUE;
}
VERIFY(0x02079D08, daBg_Draw);

/* 0207A05C */
void daBg_btkAnm_c::play() {
    WWHD_FUNC(0x0207A05C, void, this);
    u8 sp = special;
    mDoExt_btkAnm* a = anm;
    if (sp == 1) {
        /* anm->setFrame(dComIfGp_getWaveFrame()) */
        u16 wave = gabi::load<u16>(dComIfGp_ea() + 0x5D00);
        gabi::store<f32>(gabi::ea(a) + 4, (f32)wave);
    } else {
        a->play();
    }
}
VERIFY(0x0207A05C, &daBg_btkAnm_c::play);

/* 0207A0D8 */
u32 daBg_brkAnm_c::play() {
    WWHD_FUNC(0x0207A0D8, u32, this);
    return mDoExt_baseAnm_play(anm);
}
VERIFY(0x0207A0D8, &daBg_brkAnm_c::play);

/* 0207A9A0: cLib_calcTimer<u8> (per-TU copy) */
static u8 cLib_calcTimer_u8(be<u8>* t) {
    WWHD_FUNC(0x0207A9A0, u8, t);
    u8 v = *t;
    if (v != 0) {
        v = v - 1;
        *t = v;
    }
    return v;
}
VERIFY(0x0207A9A0, cLib_calcTimer_u8);

/* 0207A0E0: daBg_c::execute (inline) */
static BOOL daBg_Execute(daBg_c* i_this) {
    WWHD_FUNC(0x0207A0E0, BOOL, i_this);
    if (i_this->mUnloadTimer != 0) {
        if (cLib_calcTimer_u8(&i_this->mUnloadTimer) == 0)
            fopAcM_delete(i_this);
        return TRUE;
    }

    s32 roomNo = fopAcM_GetParam(i_this);
    dComIfGp_get();
    if (gabi::load<u8>(daBg_roomStatus(roomNo) + 0x21C) & 4) { /* dComIfGp_roomControl_checkStatusFlag(roomNo, 4) */
        if (daBg_isStage(0x10008860 /* "sea" */))
            i_this->mUnloadTimer = 16;
        else
            i_this->mUnloadTimer = 1;
    } else {
        daBg_BgModel* bgm = &i_this->bg[0];
        for (s32 i = 0; i < 4; i++, bgm++) {
            if (gabi::load<u8>(0x101F4829) == 0 /* !mDoGph_gInf_c::isMonotone() */ || i == 2) {
                if (bgm->btk != nullptr)
                    bgm->btk->play();
                if (bgm->brk != nullptr)
                    bgm->brk->play();
            }
        }
    }
    return TRUE;
}
VERIFY(0x0207A0E0, daBg_Execute);

/* 0207A2CC */
static BOOL daBg_IsDelete(daBg_c* i_this) {
    WWHD_FUNC(0x0207A2CC, BOOL, i_this);
    return i_this->mUnloadTimer == 0;
}
VERIFY(0x0207A2CC, daBg_IsDelete);

/* 0207A2DC: HD: the destructor runs from the virtual destructor (0207A628), not here */
static BOOL daBg_Delete(daBg_c* i_this) {
    WWHD_FUNC(0x0207A2DC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0207A2DC, daBg_Delete);

/* 0207A2E4 */
void daBg_btkAnm_c::entry(J3DModelData* modelData) {
    WWHD_FUNC(0x0207A2E4, void, this, modelData);
    anm->entry(modelData, 0.0f);

    /* anm->getBtkAnm()->getUpdateMaterialName()->getName(0) (HD: self-relative offsets) */
    u32 key = gabi::load<u32>(gabi::ea((mDoExt_btkAnm*)anm) + 0x68);
    u32 p = gabi::load<u32>(key + 0xC) + 0x2C;
    u32 off = gabi::load<u32>(p);
    u32 tab = off != 0 ? p + off : 0;
    u32 off2 = gabi::load<u32>(tab + 0x10);
    u32 name = off2 != 0 ? tab + 0x10 + off2 : 0;
    if (gabi::load<u8>(name) == 'S' && gabi::load<u8>(name + 1) == 'C' && gabi::load<u8>(name + 2) == '_' &&
        gabi::load<u8>(name + 3) == '0' && gabi::load<u8>(name + 4) == '1')
        special = 1;
    else
        special = 0;
}
VERIFY(0x0207A2E4, &daBg_btkAnm_c::entry);

/* 0207A3A4 */
void daBg_brkAnm_c::entry(J3DModelData* modelData) {
    WWHD_FUNC(0x0207A3A4, void, this, modelData);
    mDoExt_brkAnm_entry_l(anm, modelData, 0.0f);
    special = 0;
}
VERIFY(0x0207A3A4, &daBg_brkAnm_c::entry);

/* 0207A3E4: daBg_c::create (inline). HD: always a solid heap of its own (no room memory block) */
static cPhs_State daBg_Create(daBg_c* i_this) {
    WWHD_FUNC(0x0207A3E4, cPhs_State, i_this);
    /* fopAcM_ct(this, daBg_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = DABG_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    s32 roomNo = fopAcM_GetParam(i_this);
    if (!fopAcM_entrySolidHeap(i_this, 0x02079D04 /* checkCreateHeap */, 0)) {
        dStage_escapeRestart();
        return cPhs_ERROR_e;
    }

    daBg_BgModel* bgm = &i_this->bg[0];
    for (s32 i = 0; i < 4; i++, bgm++) {
        J3DModel* model = bgm->model;
        if (model == nullptr)
            continue;
        daBg_btkAnm_c* btk = bgm->btk;
        J3DModelData* modelData = J3DModel_getModelData(model);
        if (btk != nullptr)
            btk->entry(modelData);
        if (bgm->brk != nullptr)
            bgm->brk->entry(modelData);
    }

    gabi::Local<be<f32>> transX;
    gabi::Local<be<f32>> transZ;
    gabi::Local<be<s16>> angleY;
    if (dComIfGp_getMapTrans(roomNo, transX.get(), transZ.get(), angleY.get())) {
        daBg_BgModel* bgm = &i_this->bg[0];
        for (s32 i = 0; i < 4; i++, bgm++) {
            J3DModel* model = bgm->model;
            if (model == nullptr)
                continue;
            mDoMtx_stack_c::transS(*transX, 0.0f, *transZ);
            mDoMtx_stack_c::YrotM(*angleY);
            J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        }
    }

    bool regFail = false;
    if (i_this->bgw) {
        dBgS* bgs = dComIfG_Bgsp();
        regFail = dBgS_Regist(bgs, i_this->bgw, i_this);
    }
    if (regFail) {
        dStage_escapeRestart();
        return cPhs_ERROR_e;
    }

    dComIfGp_get();
    u32 st = daBg_roomStatus(roomNo);
    dKy_tevstr_init(gabi::at<dKy_tevstr_c>(st + 0x54), roomNo, 0xFF);
    /* HD */
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, nullptr, gabi::at<dKy_tevstr_c>(st + 0x54));
    dComIfGp_get();
    gabi::store<u8>(st + 0x21C, gabi::load<u8>(st + 0x21C) | 0x10); /* dComIfGp_roomControl_onStatusFlag(roomNo, 0x10) */
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0207A3E4, daBg_Create);

/* 0207A628: daBg_c::~daBg_c (deleting) */
static void daBg_c_dt(daBg_c* i_this, s32 flags) {
    WWHD_FUNC(0x0207A628, void, i_this, flags);
    if (i_this == nullptr)
        return;
    i_this->__vtbl = DABG_VTBL;
    s32 roomNo = fopAcM_GetParam(i_this);
    u32 st;

    if (i_this->heap && i_this->bgw) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, i_this->bgw);
        st = daBg_roomStatus(roomNo);
        gabi::store<u32>(st + 0x228, 0); /* dStage_roomControl_c::setBgW(roomNo, NULL) */
    } else {
        st = daBg_roomStatus(roomNo);
    }
    if (gabi::load<u32>(dComIfGp_ea() + 0x5AB4) != 0) /* magma */
        gabi::call(0x0258CE1C, gabi::load<u32>(dComIfGp_ea() + 0x5AB4), roomNo);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5AB8) != 0) /* grass: mRoom[roomNo].deleteData() */
        gabi::call(0x0254AC50, gabi::load<u32>(dComIfGp_ea() + 0x5AB8) + 0x1A5CC + roomNo * 4);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5ABC) != 0) /* tree */
        gabi::call(0x025C73B8, gabi::load<u32>(dComIfGp_ea() + 0x5ABC) + 0x4FBC + roomNo * 0x10C);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5AC0) != 0) /* wood: delete_room */
        gabi::call(0x025D0738, gabi::load<u32>(dComIfGp_ea() + 0x5AC0), roomNo);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5AC4) != 0) /* flower */
        gabi::call(0x025459A8, gabi::load<u32>(dComIfGp_ea() + 0x5AC4) + 0x457C + roomNo * 4);
    dComIfGp_get();
    gabi::store<u8>(st + 0x21C, gabi::load<u8>(st + 0x21C) & 0xEF); /* offStatusFlag(roomNo, 0x10) */

    gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(i_this);
}
VERIFY(0x0207A628, daBg_c_dt);

/* 0207A7C4 */
static void __sinit_d_a_bg_cpp() {
    WWHD_FUNC(0x0207A7C4, void, (u32)0);
    sinit_header_statics(0x10461A18, 0x10190EDC);
}
VERIFY(0x0207A7C4, __sinit_d_a_bg_cpp);

/* 0207A858: sead::SafeString::SafeString(const char*) (per-TU copy; allocates when this == NULL) */
static daBg_SafeString* daBg_SafeString_ct(daBg_SafeString* p, u32 str) {
    WWHD_FUNC(0x0207A858, daBg_SafeString*, p, str);
    if (p == nullptr) {
        p = (daBg_SafeString*)operator_new(8);
        if (p == nullptr)
            return p;
    }
    p->mStr = str;
    p->__vtbl = DABG_SAFESTRING_VTBL;
    return p;
}
VERIFY(0x0207A858, daBg_SafeString_ct);

/* 0207A8A8: sead::SafeString operator== (per-TU copy) */
static BOOL daBg_SafeString_eq(daBg_SafeString* a, daBg_SafeString* b) {
    WWHD_FUNC(0x0207A8A8, BOOL, a, b);
    return daBg_ss_cmp(a, b);
}
VERIFY(0x0207A8A8, daBg_SafeString_eq);

/* 0207A988: sead::SafeString deleting destructor (per-TU copy) */
static void daBg_SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0207A988, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0207A988, daBg_SafeString_dt);

/* 0207A99C: sead::SafeString::assureTermination (empty, per-TU copy) */
static void daBg_SafeString_assure(void* p) {
    WWHD_FUNC(0x0207A99C, void, p);
}
VERIFY(0x0207A99C, daBg_SafeString_assure);
