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
#include "d/actor/d_a_bg_heap.h"

const gc_tu_data gc_tu = {.safeString = DABG_SAFESTRING_VTBL};
#define VT gc_tu.safeString

static BgHeapNameTable l_brkName;
static BgHeapNameTable l_btkName;
static BgHeapNameTable l_modelName;
static BgHeapNameTable brkTemplate;
static BgHeapNameTable btkTemplate;
static BgHeapNameTable modelTemplate;
static BgHeapGuard brkGuard;
static BgHeapGuard btkGuard;
static BgHeapGuard modelGuard;
static BgHeapLight lightDefaults;

/* inline SafeString compare with the literal in `a` */
static inline bool compareStageInline(daBg_SafeString *a, const char *lit) {
    a->__vtbl = VT;
    a->mStr = gabi::guest_addr(lit);
    BgHeapPlay *play = BgHeap_getPlay();
    gabi::Local<daBg_SafeString> b;
    b->__vtbl = VT;
    b->mStr = gabi::ea(play->mStartStageName);
    return daBg_ss_cmp(a, b.get());
}
/* a inline, b through the out-of-line constructor, out-of-line ==, b destroyed */
static inline bool compareStageWithConstructedStage(const char *lit) {
    gabi::Local<daBg_SafeString> a;
    a->__vtbl = VT;
    a->mStr = gabi::guest_addr(lit);
    BgHeapPlay *play = BgHeap_getPlay();
    gabi::Local<daBg_SafeString> b;
    daBg_SafeString *bp = BgHeap_ss_ct(b.get(), reinterpret_cast<const char *>(play->mStartStageName));
    BOOL r = BgHeap_ss_eq(a.get(), bp);
    BgHeap_ss_dt(b.get(), 2);
    return r != 0;
}
/* both through the out-of-line constructor, out-of-line == (b destroyed unless !dtorB) */
static inline bool compareStageWithConstructedBoth(daBg_SafeString *a, const char *lit, bool dtorB = true) {
    BgHeap_ss_ct(a, lit);
    BgHeapPlay *play = BgHeap_getPlay();
    gabi::Local<daBg_SafeString> b;
    daBg_SafeString *bp = BgHeap_ss_ct(b.get(), reinterpret_cast<const char *>(play->mStartStageName));
    BOOL r = BgHeap_ss_eq(a, bp);
    if (dtorB)
        BgHeap_ss_dt(b.get(), 2);
    return r != 0;
}
/* both inline, out-of-line == */
static inline bool compareStageOutOfLine(daBg_SafeString *a, const char *lit) {
    a->__vtbl = VT;
    a->mStr = gabi::guest_addr(lit);
    BgHeapPlay *play = BgHeap_getPlay();
    gabi::Local<daBg_SafeString> b;
    b->__vtbl = VT;
    b->mStr = gabi::ea(play->mStartStageName);
    return BgHeap_ss_eq(a, b.get()) != 0;
}
/* a through the out-of-line constructor, b inline, out-of-line == */
static inline bool compareStageWithConstructedLiteral(daBg_SafeString *a, const char *lit) {
    BgHeap_ss_ct(a, lit);
    BgHeapPlay *play = BgHeap_getPlay();
    gabi::Local<daBg_SafeString> b;
    b->__vtbl = VT;
    b->mStr = gabi::ea(play->mStartStageName);
    return BgHeap_ss_eq(a, b.get()) != 0;
}
#define STAGE_MATCHES_INLINE(lit) compareStageInline(gabi::Local<daBg_SafeString>().get(), lit)

// HD: initialise name tables in the original order and preserve the memcpy call.
static void init_static_table(BgHeapGuard *guard, BgHeapNameTable *dst, BgHeapNameTable *src) {
    if (guard->initialized == 0) {
        guard->initialized = 1;
        memcpy_g(dst, src, sizeof(BgHeapNameTable));
    }
}
static void copyLight(BgHeapLight *dst) {
    for (int i = 0; i < 6; ++i)
        dst->mVectors[i] = u32(lightDefaults.mVectors[i]);
    for (int i = 0; i < 4; ++i)
        dst->mColor[i] = u8(lightDefaults.mColor[i]);
    for (int i = 0; i < 4; ++i)
        dst->mSettings[i] = u16(lightDefaults.mSettings[i]);
    for (int i = 0; i < 8; ++i)
        dst->mAttenuation[i] = u32(lightDefaults.mAttenuation[i]);
}
static void dKy_tevstr_ct(BgHeapTev *tev) {
    copyLight(&tev->mLight0);
    copyLight(&tev->mLight1);
    copyLight(&tev->mLight2);
}

/* 02077FCC */
BOOL daBg_c::createHeap() {
    init_static_table(&brkGuard, &l_brkName, &brkTemplate);       /* l_brkName "model.brk", ... */
    init_static_table(&btkGuard, &l_btkName, &btkTemplate);       /* l_btkName "model.btk", ... */
    init_static_table(&modelGuard, &l_modelName, &modelTemplate); /* l_modelName "model.bmd", ... */

    const char *arcName = BgHeap_setArcName(this);
    u32 roomNo = fopAcM_GetParam(this);

    daBg_BgModel *bgm = bg;
    for (int i = 0; i < 4; bgm++, i++) {
        J3DModelData *modelData =
            (J3DModelData *)dComIfG_getStageRes(arcName, reinterpret_cast<const char *>(l_modelName.names[i]));
        if (modelData == nullptr)
            continue;
        BgHeapModelData *md = reinterpret_cast<BgHeapModelData *>(modelData);

        u32 diffFlag = 0x11000022;

        for (u16 mat_no = 0; mat_no < u16(BgHeap_getMaterialTable(modelData)->mMaterialNum); mat_no++) {
            /* modelData->getMaterialNodePointer(mat_no)->setMaterialAnm(NULL) */
            u32 count = md->mMaterialCount;
            BgHeapMaterial *mat = md->mpMaterials;
            if (mat_no < count)
                mat += mat_no;
            mat->mpMaterialAnm = nullptr;
        }

        J3DAnmTextureSRTKey *btk =
            (J3DAnmTextureSRTKey *)dComIfG_getStageRes(arcName, reinterpret_cast<const char *>(l_btkName.names[i]));
        if (btk != nullptr) {
            daBg_btkAnm_c *p = (daBg_btkAnm_c *)operator_new(8);
            bgm->btk = p;
            if (p == nullptr)
                return FALSE;
            if (!BgHeap_btkCreate(p, modelData, btk)) /* daBg_btkAnm_c::create */
                return FALSE;
            diffFlag |= 0x00001200;
        } else {
            bgm->btk = nullptr;
        }

        J3DAnmTevRegKey *brk =
            (J3DAnmTevRegKey *)dComIfG_getStageRes(arcName, reinterpret_cast<const char *>(l_brkName.names[i]));
        if (brk != nullptr) {
            daBg_brkAnm_c *p = (daBg_brkAnm_c *)operator_new(8);
            bgm->brk = p;
            if (p == nullptr)
                return FALSE;
            if (!BgHeap_brkCreate(p, modelData, brk)) /* daBg_brkAnm_c::create */
                return FALSE;
        } else {
            bgm->brk = nullptr;
        }

        J3DModel *model = mDoExt_J3DModel__create(modelData, 0, diffFlag);
        bgm->model = model;
        if (model == nullptr)
            return FALSE;
        BgHeapTev *tev = reinterpret_cast<BgHeapTev *>(operator_new(sizeof(BgHeapTev)));
        if (tev != nullptr)
            dKy_tevstr_ct(tev);
        bgm->mpTevStr = reinterpret_cast<dKy_tevstr_c *>(tev);
        if (tev == nullptr)
            return FALSE;
        BgHeap_tevstr_init(reinterpret_cast<dKy_tevstr_c *>(tev), (s8)roomNo, 0xFF);

        /* HD: per-stage special cases */
        if (i == 2 && (STAGE_MATCHES_INLINE("Hyrule") || STAGE_MATCHES_INLINE("GTower"))) {
            bgm->mFlag = 1;
            continue;
        }

        gabi::Local<daBg_SafeString> fairy04, ganonJ, majyuE, omori;
        bool constructedOmori = false, constructedMajyuE = false, constructedGanonJ = false, constructedFairy04 = false;
        bool special;
        if (i == 1) {
            special = STAGE_MATCHES_INLINE("Fairy01") || STAGE_MATCHES_INLINE("Fairy02") ||
                      STAGE_MATCHES_INLINE("Fairy03") ||
                      (constructedFairy04 = true, compareStageInline(fairy04.get(), "Fairy04")) ||
                      STAGE_MATCHES_INLINE("Fairy05") || STAGE_MATCHES_INLINE("Fairy06");
        } else if (i == 3) {
            gabi::Local<daBg_SafeString> t[14];
            s32 r = (s32)roomNo;
            special = (STAGE_MATCHES_INLINE("Abesso") && r == 0) || (STAGE_MATCHES_INLINE("Adanmae") && r == 0) ||
                      (STAGE_MATCHES_INLINE("Atorizk") && r == 0) || (STAGE_MATCHES_INLINE("Edaichi") && r == 0) ||
                      (STAGE_MATCHES_INLINE("Ekaze") && r == 0) || (STAGE_MATCHES_INLINE("GanonA") && r == 0) ||
                      ((constructedGanonJ = true, compareStageInline(ganonJ.get(), "GanonJ")) && r == 0xD) ||
                      (STAGE_MATCHES_INLINE("GanonK") && r == 0) || (STAGE_MATCHES_INLINE("Kaisen") && r == 0) ||
                      (STAGE_MATCHES_INLINE("kindan") && (r == 9 || r == 0x10)) ||
                      (STAGE_MATCHES_INLINE("kinMB") && r == 0xA) ||
                      (compareStageWithConstructedStage("LinkRM") && r == 0) ||
                      (compareStageWithConstructedBoth(t[0].get(), "LinkUG") && r == 0) ||
                      (compareStageWithConstructedBoth(t[1].get(), "M_NewD2") && r == 0) ||
                      (compareStageWithConstructedBoth(t[2].get(), "ma3room") && roomNo <= 4) ||
                      ((constructedMajyuE = true, compareStageWithConstructedBoth(majyuE.get(), "MajyuE")) && r == 0) ||
                      (compareStageWithConstructedBoth(t[3].get(), "Mjtower", false) && r == 0) ||
                      (compareStageOutOfLine(t[4].get(), "Nitiyou") && r == 0) ||
                      (compareStageOutOfLine(t[5].get(), "Obombh") && r == 0) ||
                      (compareStageOutOfLine(t[6].get(), "Obshop") && roomNo <= 3) ||
                      (compareStageOutOfLine(t[7].get(), "Ocmera") && r == 0) ||
                      (compareStageOutOfLine(t[8].get(), "Ocrogh") && r == 0) ||
                      (compareStageOutOfLine(t[9].get(), "Ojhous2") && r == 0) ||
                      (compareStageWithConstructedLiteral(t[10].get(), "Omasao") && r == 0) ||
                      ((constructedOmori = true, compareStageOutOfLine(omori.get(), "Omori")) && r == 0) ||
                      (compareStageOutOfLine(t[11].get(), "Onobuta") && r == 0) ||
                      (compareStageOutOfLine(t[12].get(), "Orichh") && r == 0) ||
                      (compareStageOutOfLine(t[13].get(), "Otkura") && r == 0) ||
                      (compareStageWithConstructedLiteral(gabi::Local<daBg_SafeString>().get(), "Pdrgsh") && r == 0) ||
                      (compareStageOutOfLine(gabi::Local<daBg_SafeString>().get(), "Pjavdou") && r == 0) ||
                      (compareStageOutOfLine(gabi::Local<daBg_SafeString>().get(), "Pnezumi") && r == 0) ||
                      (compareStageOutOfLine(gabi::Local<daBg_SafeString>().get(), "sea") &&
                       (r == 1 || r == 0xB || r == 0x2C)) ||
                      (compareStageOutOfLine(gabi::Local<daBg_SafeString>().get(), "ShipD") && r == 0) ||
                      (compareStageOutOfLine(gabi::Local<daBg_SafeString>().get(), "Siren") && r == 0);
        } else {
            special = false;
        }
        /* end of the full expression: the temporaries with a recorded construction are destroyed */
        if (constructedOmori)
            BgHeap_ss_dt(omori.get(), 2);
        if (constructedMajyuE)
            BgHeap_ss_dt(majyuE.get(), 2);
        if (constructedGanonJ)
            BgHeap_ss_dt(ganonJ.get(), 2);
        if (constructedFairy04)
            BgHeap_ss_dt(fairy04.get(), 2);

        if (special) {
            /* every material instance of the model: flags (+0x34) |= 1 */
            for (s32 j = 0; j < (s32)u16(BgHeap_getMaterialTable(J3DModel_getModelData(bgm->model))->mMaterialNum);
                 j++) {
                BgHeapModel *modelView = reinterpret_cast<BgHeapModel *>(bgm->model.get());
                BgHeapMaterialInstance *inst = nullptr;
                if ((u32)j < modelView->mMaterialCount)
                    inst = modelView->mpMaterials.get() + j;
                if (gabi::ea(inst) != 0)
                    inst->mFlags = u8(inst->mFlags) | 1;
            }
        }
        bgm->mFlag = 0;
    }

    cBgD_t *dzb = (cBgD_t *)dComIfG_getStageRes(arcName, "room.dzb");
    if (dzb != nullptr) {
        dBgW *w = new_dBgW();
        bgw = w;
        if (w == nullptr)
            return FALSE;
        if (cBgW_Set(w, dzb, BgHeapBgW::GLOBAL_e, nullptr))
            return FALSE;
        BgHeap_roomStatus(roomNo)->mpBgW = bgw.get();            /* dStage_roomControl_c::setBgW */
        reinterpret_cast<BgHeapBgW *>(bgw.get())->mPriority = 0; /* bgw->SetPriority(0) */
    } else {
        bgw = nullptr;
    }
    return TRUE;
}

/* WWHD glue
 data l_brkName 101FEB4C
 data l_btkName 101FEB80
 data l_modelName 101FEBB4
 data brkTemplate 10190EA8
 data btkTemplate 10190E74
 data modelTemplate 10190E40
 data brkGuard 101FDA38
 data btkGuard 101FDA3C
 data modelGuard 101FDA40
 data lightDefaults 1016E414
 ext 02077E30 const char* BgHeap_setArcName(daBg_c*)
 ext 027F3F8C BgHeapMaterialTable* BgHeap_getMaterialTable(J3DModelData*)
 ext 025200D4 BgHeapPlay* BgHeap_getPlay()
 ext 0255FFF4 void BgHeap_tevstr_init(dKy_tevstr_c*, s8, u8)
 ext 02077E74 BOOL BgHeap_btkCreate(daBg_btkAnm_c*, J3DModelData*, J3DAnmTextureSRTKey*)
 ext 02077F20 BOOL BgHeap_brkCreate(daBg_brkAnm_c*, J3DModelData*, J3DAnmTevRegKey*)
 ext 0207A858 daBg_SafeString* BgHeap_ss_ct(daBg_SafeString*, const char*)
 ext 0207A8A8 BOOL BgHeap_ss_eq(daBg_SafeString*, daBg_SafeString*)
 ext 0207A988 void BgHeap_ss_dt(daBg_SafeString*, s32)
 str 10008884 "Fairy01"
 str 1000888C "Fairy02"
 str 10008894 "Fairy03"
 str 1000889C "Fairy04"
 str 100088A4 "Fairy05"
 str 100088AC "Fairy06"
 str 100088B4 "Adanmae"
 str 100088BC "Atorizk"
 str 100088C4 "Edaichi"
 str 100088CC "M_NewD2"
 str 100088D4 "ma3room"
 str 100088DC "Mjtower"
 str 100088E4 "Nitiyou"
 str 100088EC "Ojhous2"
 str 100088F4 "Onobuta"
 str 100088FC "Pjavdou"
 str 10008904 "Pnezumi"
 str 1000890C "sea"
 str 10008910 "Ekaze"
 str 10008918 "kinMB"
 str 10008920 "Omori"
 str 10008928 "ShipD"
 str 10008930 "Siren"
 str 10008938 "Hyrule"
 str 10008940 "GTower"
 str 10008948 "Abesso"
 str 10008950 "GanonA"
 str 10008958 "GanonJ"
 str 10008960 "GanonK"
 str 10008968 "Kaisen"
 str 10008970 "kindan"
 str 10008978 "LinkRM"
 str 10008980 "LinkUG"
 str 10008988 "MajyuE"
 str 10008990 "Obombh"
 str 10008998 "Obshop"
 str 100089A0 "Ocmera"
 str 100089A8 "Ocrogh"
 str 100089B0 "Omasao"
 str 100089B8 "Orichh"
 str 100089C0 "Otkura"
 str 100089C8 "Pdrgsh"
 str 100089D0 "room.dzb"
*/
