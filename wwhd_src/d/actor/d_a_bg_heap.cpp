/**
 * d_a_bg_heap.cpp (WWHD)
 * Room background models (BG): daBg_c::createHeap
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bg.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD changes: only model*.bmd (no .bdl fallback), no createMatAnm, the name tables are function-local
 * statics initialised on first use, the dKy_tevstr_c constructor is inline (three copies of a 0x44-byte
 * default block), and each model gets a per-stage "special" decision:
 *  - model 2 in Hyrule / GTower: BgModel::mFlag = 1 (drawn in an extra list, see daBg_Draw);
 *  - model 1 in Fairy01..06, model 3 in a list of (stage, room) pairs: every material instance of
 *    the model gets bit 0 of its +0x34 flags (and mFlag = 0).
 * The stage-name tests are sead::SafeString compares, inline or through the TU's out-of-line
 * constructor/operator==/destructor, exactly as GHS emitted them (the call sequence is observable).
 */
#include "d/actor/d_a_bg.h"

#define VT DABG_SAFESTRING_VTBL

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0252447C dComIfG_getStageRes(const char* arc, const char* name) */
static inline void* dComIfG_getStageRes(u32 arc, u32 name) { return gabi::call<void*>(0x0252447C, arc, name); }
/* 027F3F8C: J3DModelData -> its material table (u16 material count at +0x24) */
static inline u32 J3DModelData_getMaterialTable(u32 modelData) { return gabi::call<u32>(0x027F3F8C, modelData); }
static inline void dKy_tevstr_init(u32 t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }

/* this TU's sead::SafeString out-of-line members */
static inline daBg_SafeString* ss_ct(daBg_SafeString* p, u32 str) { return gabi::call<daBg_SafeString*>(0x0207A858, p, str); }
static inline BOOL ss_eq(daBg_SafeString* a, daBg_SafeString* b) { return gabi::call<BOOL>(0x0207A8A8, a, b); }
static inline void ss_dt(daBg_SafeString* p, s32 flags) { gabi::call(0x0207A988, p, flags); }

/* inline SafeString compare with the literal in `a` */
static inline bool cmpA(daBg_SafeString* a, u32 lit) {
    a->__vtbl = VT;
    a->mStr = lit;
    u32 play = dComIfGp_ea();
    gabi::Local<daBg_SafeString> b;
    b->__vtbl = VT;
    b->mStr = play + 0x5134;
    return daBg_ss_cmp(a, b.get());
}
/* a inline, b through the out-of-line constructor, out-of-line ==, b destroyed */
static inline bool cmpB(u32 lit) {
    gabi::Local<daBg_SafeString> a;
    a->__vtbl = VT;
    a->mStr = lit;
    u32 play = dComIfGp_ea();
    gabi::Local<daBg_SafeString> b;
    daBg_SafeString* bp = ss_ct(b.get(), play + 0x5134);
    BOOL r = ss_eq(a.get(), bp);
    ss_dt(b.get(), 2);
    return r != 0;
}
/* both through the out-of-line constructor, out-of-line == (b destroyed unless !dtorB) */
static inline bool cmpC(daBg_SafeString* a, u32 lit, bool dtorB = true) {
    ss_ct(a, lit);
    u32 play = dComIfGp_ea();
    gabi::Local<daBg_SafeString> b;
    daBg_SafeString* bp = ss_ct(b.get(), play + 0x5134);
    BOOL r = ss_eq(a, bp);
    if (dtorB)
        ss_dt(b.get(), 2);
    return r != 0;
}
/* both inline, out-of-line == */
static inline bool cmpD(daBg_SafeString* a, u32 lit) {
    a->__vtbl = VT;
    a->mStr = lit;
    u32 play = dComIfGp_ea();
    gabi::Local<daBg_SafeString> b;
    b->__vtbl = VT;
    b->mStr = play + 0x5134;
    return ss_eq(a, b.get()) != 0;
}
/* a through the out-of-line constructor, b inline, out-of-line == */
static inline bool cmpE(daBg_SafeString* a, u32 lit) {
    ss_ct(a, lit);
    u32 play = dComIfGp_ea();
    gabi::Local<daBg_SafeString> b;
    b->__vtbl = VT;
    b->mStr = play + 0x5134;
    return ss_eq(a, b.get()) != 0;
}
#define A_(lit) cmpA(gabi::Local<daBg_SafeString>().get(), lit)

/* stage names (.rodata) */
enum : u32 {
    S_Fairy01 = 0x10008884, S_Fairy02 = 0x1000888C, S_Fairy03 = 0x10008894, S_Fairy04 = 0x1000889C,
    S_Fairy05 = 0x100088A4, S_Fairy06 = 0x100088AC, S_Adanmae = 0x100088B4, S_Atorizk = 0x100088BC,
    S_Edaichi = 0x100088C4, S_M_NewD2 = 0x100088CC, S_ma3room = 0x100088D4, S_Mjtower = 0x100088DC,
    S_Nitiyou = 0x100088E4, S_Ojhous2 = 0x100088EC, S_Onobuta = 0x100088F4, S_Pjavdou = 0x100088FC,
    S_Pnezumi = 0x10008904, S_sea = 0x1000890C, S_Ekaze = 0x10008910, S_kinMB = 0x10008918,
    S_Omori = 0x10008920, S_ShipD = 0x10008928, S_Siren = 0x10008930, S_Hyrule = 0x10008938,
    S_GTower = 0x10008940, S_Abesso = 0x10008948, S_GanonA = 0x10008950, S_GanonJ = 0x10008958,
    S_GanonK = 0x10008960, S_Kaisen = 0x10008968, S_kindan = 0x10008970, S_LinkRM = 0x10008978,
    S_LinkUG = 0x10008980, S_MajyuE = 0x10008988, S_Obombh = 0x10008990, S_Obshop = 0x10008998,
    S_Ocmera = 0x100089A0, S_Ocrogh = 0x100089A8, S_Omasao = 0x100089B0, S_Orichh = 0x100089B8,
    S_Otkura = 0x100089C0, S_Pdrgsh = 0x100089C8,
};

/* function-local static name tables (13 bytes per name), initialised on first use */
static inline void init_static_table(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<void>(dst), gabi::at<void>(src), 0x34); /* 028FEAC0 -> import memcpy */
    }
}

/* dKy_tevstr_c::dKy_tevstr_c (HD inline): three copies of the 0x44-byte default block at 0x1016E414 */
static inline void dKy_tevstr_ct(u32 t) {
    const u32 src = 0x1016E414;
    static const u8 sz[] = {4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4};
    for (u32 base : {0x0u, 0xC0u, 0x144u}) {
        u32 off = 0;
        for (u8 s : sz) {
            if (s == 4)
                gabi::store<u32>(t + base + off, gabi::load<u32>(src + off));
            else if (s == 2)
                gabi::store<u16>(t + base + off, gabi::load<u16>(src + off));
            else
                gabi::store<u8>(t + base + off, gabi::load<u8>(src + off));
            off += s;
        }
        gabi::store<u32>(t + base + 0x40, gabi::load<u32>(src + 0x40));
    }
}

/* 02077FCC */
BOOL daBg_c::createHeap() {
    WWHD_FUNC(0x02077FCC, BOOL, this);
    init_static_table(0x101FDA38, 0x101FEB4C, 0x10190EA8); /* l_brkName "model.brk", ... */
    init_static_table(0x101FDA3C, 0x101FEB80, 0x10190E74); /* l_btkName "model.btk", ... */
    init_static_table(0x101FDA40, 0x101FEBB4, 0x10190E40); /* l_modelName "model.bmd", ... */

    u32 arcName = gabi::call<u32>(0x02077E30, this); /* setArcName() */
    u32 roomNo = fopAcM_GetParam(this);

    daBg_BgModel* bgm = bg;
    for (int i = 0; i < 4; bgm++, i++) {
        J3DModelData* modelData = (J3DModelData*)dComIfG_getStageRes(arcName, 0x101FEBB4 + 13 * i);
        if (modelData == nullptr)
            continue;
        u32 md = gabi::ea(modelData);

        u32 diffFlag = 0x11000022;

        for (u16 mat_no = 0; mat_no < gabi::load<u16>(J3DModelData_getMaterialTable(md) + 0x24); mat_no++) {
            /* modelData->getMaterialNodePointer(mat_no)->setMaterialAnm(NULL) */
            u32 n = gabi::load<u32>(md + 0xC);
            u32 mat = gabi::load<u32>(md + 0x10);
            if (mat_no < n)
                mat += mat_no * 0x39C;
            gabi::store<u32>(mat + 0x24, 0);
        }

        J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getStageRes(arcName, 0x101FEB80 + 13 * i);
        if (btk != nullptr) {
            daBg_btkAnm_c* p = (daBg_btkAnm_c*)operator_new(8);
            bgm->btk = p;
            if (p == nullptr)
                return FALSE;
            if (!gabi::call<BOOL>(0x02077E74, p, modelData, btk)) /* daBg_btkAnm_c::create */
                return FALSE;
            diffFlag |= 0x00001200;
        } else {
            bgm->btk = nullptr;
        }

        J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getStageRes(arcName, 0x101FEB4C + 13 * i);
        if (brk != nullptr) {
            daBg_brkAnm_c* p = (daBg_brkAnm_c*)operator_new(8);
            bgm->brk = p;
            if (p == nullptr)
                return FALSE;
            if (!gabi::call<BOOL>(0x02077F20, p, modelData, brk)) /* daBg_brkAnm_c::create */
                return FALSE;
        } else {
            bgm->brk = nullptr;
        }

        J3DModel* model = mDoExt_J3DModel__create(modelData, 0, diffFlag);
        bgm->model = model;
        if (model == nullptr)
            return FALSE;
        u32 tev = gabi::ea(operator_new(0x1C8));
        if (tev != 0)
            dKy_tevstr_ct(tev);
        bgm->mpTevStr = gabi::at<dKy_tevstr_c>(tev);
        if (tev == 0)
            return FALSE;
        dKy_tevstr_init(tev, (s8)roomNo, 0xFF);

        /* HD: per-stage special cases */
        if (i == 2 && (A_(S_Hyrule) || A_(S_GTower))) {
            bgm->mFlag = 1;
            continue;
        }

        gabi::Local<daBg_SafeString> fairy04, ganonJ, majyuE, omori;
        bool c_omori = false, c_majyuE = false, c_ganonJ = false, c_fairy04 = false;
        bool special;
        if (i == 1) {
            special = A_(S_Fairy01) || A_(S_Fairy02) || A_(S_Fairy03) || (c_fairy04 = true, cmpA(fairy04.get(), S_Fairy04)) ||
                      A_(S_Fairy05) || A_(S_Fairy06);
        } else if (i == 3) {
            gabi::Local<daBg_SafeString> t[14];
            s32 r = (s32)roomNo;
            special = (A_(S_Abesso) && r == 0) || (A_(S_Adanmae) && r == 0) || (A_(S_Atorizk) && r == 0) ||
                      (A_(S_Edaichi) && r == 0) || (A_(S_Ekaze) && r == 0) || (A_(S_GanonA) && r == 0) ||
                      ((c_ganonJ = true, cmpA(ganonJ.get(), S_GanonJ)) && r == 0xD) || (A_(S_GanonK) && r == 0) ||
                      (A_(S_Kaisen) && r == 0) || (A_(S_kindan) && (r == 9 || r == 0x10)) || (A_(S_kinMB) && r == 0xA) ||
                      (cmpB(S_LinkRM) && r == 0) || (cmpC(t[0].get(), S_LinkUG) && r == 0) ||
                      (cmpC(t[1].get(), S_M_NewD2) && r == 0) || (cmpC(t[2].get(), S_ma3room) && roomNo <= 4) ||
                      ((c_majyuE = true, cmpC(majyuE.get(), S_MajyuE)) && r == 0) ||
                      (cmpC(t[3].get(), S_Mjtower, false) && r == 0) || (cmpD(t[4].get(), S_Nitiyou) && r == 0) ||
                      (cmpD(t[5].get(), S_Obombh) && r == 0) || (cmpD(t[6].get(), S_Obshop) && roomNo <= 3) ||
                      (cmpD(t[7].get(), S_Ocmera) && r == 0) || (cmpD(t[8].get(), S_Ocrogh) && r == 0) ||
                      (cmpD(t[9].get(), S_Ojhous2) && r == 0) || (cmpE(t[10].get(), S_Omasao) && r == 0) ||
                      ((c_omori = true, cmpD(omori.get(), S_Omori)) && r == 0) ||
                      (cmpD(t[11].get(), S_Onobuta) && r == 0) || (cmpD(t[12].get(), S_Orichh) && r == 0) ||
                      (cmpD(t[13].get(), S_Otkura) && r == 0) ||
                      (cmpE(gabi::Local<daBg_SafeString>().get(), S_Pdrgsh) && r == 0) ||
                      (cmpD(gabi::Local<daBg_SafeString>().get(), S_Pjavdou) && r == 0) ||
                      (cmpD(gabi::Local<daBg_SafeString>().get(), S_Pnezumi) && r == 0) ||
                      (cmpD(gabi::Local<daBg_SafeString>().get(), S_sea) && (r == 1 || r == 0xB || r == 0x2C)) ||
                      (cmpD(gabi::Local<daBg_SafeString>().get(), S_ShipD) && r == 0) ||
                      (cmpD(gabi::Local<daBg_SafeString>().get(), S_Siren) && r == 0);
        } else {
            special = false;
        }
        /* end of the full expression: the temporaries with a recorded construction are destroyed */
        if (c_omori)
            ss_dt(omori.get(), 2);
        if (c_majyuE)
            ss_dt(majyuE.get(), 2);
        if (c_ganonJ)
            ss_dt(ganonJ.get(), 2);
        if (c_fairy04)
            ss_dt(fairy04.get(), 2);

        if (special) {
            /* every material instance of the model: flags (+0x34) |= 1 */
            for (s32 j = 0; j < (s32)gabi::load<u16>(J3DModelData_getMaterialTable(gabi::ea(J3DModel_getModelData(bgm->model))) + 0x24); j++) {
                u32 m = gabi::ea((J3DModel*)bgm->model);
                u32 inst = 0;
                if ((u32)j < gabi::load<u32>(m + 0x138))
                    inst = gabi::load<u32>(m + 0x13C) + j * 0x38;
                if (inst != 0)
                    gabi::store<u8>(inst + 0x34, gabi::load<u8>(inst + 0x34) | 1);
            }
        }
        bgm->mFlag = 0;
    }

    cBgD_t* dzb = (cBgD_t*)dComIfG_getStageRes(arcName, 0x100089D0 /* "room.dzb" */);
    if (dzb != nullptr) {
        dBgW* w = new_dBgW();
        bgw = w;
        if (w == nullptr)
            return FALSE;
        if (cBgW_Set(w, dzb, 0x20 /* cBgW::GLOBAL_e */, nullptr))
            return FALSE;
        gabi::store<u32>(daBg_roomStatus(roomNo) + 0x228, gabi::ea((dBgW*)bgw)); /* dStage_roomControl_c::setBgW */
        gabi::store<u8>(gabi::ea((dBgW*)bgw) + 0x75, 0);                       /* bgw->SetPriority(0) */
    } else {
        bgw = nullptr;
    }
    return TRUE;
}
VERIFY(0x02077FCC, &daBg_c::createHeap);
