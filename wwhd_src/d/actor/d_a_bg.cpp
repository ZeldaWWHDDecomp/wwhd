/**
 * d_a_bg.cpp (WWHD)
 * Room background models (BG)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bg.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * The separately verified createHeap remains in d_a_bg_heap.cpp.
 */
#include "d/actor/d_a_bg_readable.h"
enum { ROOM_UNLOADING = 4, ROOM_ACTIVE = 0x10 };
static BgText arcName;
static BgText roomFormat;
static BgText stageOmori;
static BgText stageSea;
static BgClipper clipper;
static f32 defaultFar;
static u32 saveInfo;
static u32 currentOpaList;
static u32 currentXluList;
static u8 monotone;

/* 02077E30 */
static u32 daBg_setArcName(daBg_c *i_this) {
    Bg_sprintf_g(gabi::ea(arcName.bytes), gabi::ea(roomFormat.bytes), fopAcM_GetParam(i_this));
    return gabi::ea(arcName.bytes);
}

// HD: no retail createMatAnm loop.
/* 02077E74 */
static BOOL daBg_btkAnm_create(daBg_btkAnm_c *i_this, J3DModelData *modelData,
                               J3DAnmTextureSRTKey *anmData) {
    mDoExt_btkAnm *anm = (mDoExt_btkAnm *)Bg_mDoExt_btkAnm_new();
    i_this->anm = anm;
    if (anm == nullptr)
        return FALSE;
    if (!anm->init(modelData, anmData, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}

// HD: no retail createMatAnm loops.
/* 02077F20 */
static BOOL daBg_brkAnm_create(daBg_brkAnm_c *i_this, J3DModelData *modelData,
                               J3DAnmTevRegKey *anmData) {
    void *anm = Bg_mDoExt_brkAnm_new();
    i_this->anm = (mDoExt_btkAnm *)anm;
    if (anm == nullptr)
        return FALSE;
    if (!Bg_mDoExt_brkAnm_init(anm, modelData, anmData, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1,
                               false, 0))
        return FALSE;
    return TRUE;
}

/* 02079D04 */
static BOOL checkCreateHeap(fopAc_ac_c *i_ac) {
    return Bg_createHeap(i_ac); /* ((daBg_c*)i_ac)->createHeap(): tail call */
}

// HD: snapshot the curve inputs, evaluate it, then invalidate its animation slot.
static inline void anm_entryFrame(BgAnimation *anm, be<f32> *frameDst, be<u32> *curveSlot) {
    f32 frame = anm->mFrameCtrl.mFrame;
    *frameDst = frame;
    BgCurve *obj = gabi::at<BgCurve>(*curveSlot);
    auto fn = obj->mEvaluate.get();
    f32 p2 = obj->mEnd;
    f32 p1 = obj->mStart;
    u32 ctx = obj->mContext;
    f32 r = gabi::call_ptr<f32>(fn, ctx, frame, p1, p2);
    obj->mResult = r;
    Bg_evaluateCurveSlot(curveSlot);
}

/* 02079D08 */
BOOL daBg_c::draw() {
    s32 roomNo = fopAcM_GetParam(this);
    daBg_BgModel *bgm = &bg[0];

    dComIfGd_setListBG();
    /* HD: in Omori, models 1 and 3 are drawn in the reverse order */
    bool omori = daBg_isStage(gabi::ea(stageOmori.bytes));
    clipper.mFar = 100000.0f; /* mDoLib_clipper::changeFar(100000.0f) */
    Bg_J3DUClipper_calcViewFrustum(&clipper);

    for (s32 i = 0; i < 4; i++, bgm++) {
        if (omori) {
            if (i == 1) {
                i = 3;
                bgm = &bg[3];
            } else if (i == 3) {
                bgm = &bg[1];
                i = 1;
            }
        }
        J3DModel *model = bgm->model;
        if (model != nullptr) {
            daBg_btkAnm_c *btk = bgm->btk;
            if (btk != nullptr) {
                BgAnimation *anm = reinterpret_cast<BgAnimation *>(btk->anm.get());
                anm_entryFrame(anm, anm->mpTextureFrame, &anm->mTextureCurve);
            }
            daBg_brkAnm_c *brk = bgm->brk;
            if (brk != nullptr) {
                BgAnimation *anm = reinterpret_cast<BgAnimation *>(brk->anm.get());
                anm_entryFrame(anm, gabi::at<be<f32>>(anm->mTextureCurve), &anm->mColorCurve);
            }
            J3DModel_calc(model);
            Bg_J3DUClipper_clip(&clipper, model);
            dScnKy_env_light_c *light = dKy_getEnvlight();
            settingTevStruct(light, TEV_TYPE_BG0 + i, nullptr, bgm->mpTevStr);
            light = dKy_getEnvlight();
            setLightTevColorType(light, model, bgm->mpTevStr);
            if (i == 1) {
                currentOpaList = (u32)Bg_play()->mInvisibleOpa;
                Bg_setInvisibleLighting();
            }
            if (bgm->mFlag != 0 && (u8)gabi::at<BgSave>(saveInfo)->mSpecialDraw != 0) {
                currentOpaList = (u32)Bg_play()->mSpecialOpa;
                currentXluList = (u32)Bg_play()->mSpecialXlu;
            }
            mDoExt_modelEntryDL(model);
            if (i == 1) {
                Bg_resetInvisibleLighting();
                dComIfGd_setListBG();
            }
            if (bgm->mFlag != 0) {
                dComIfGd_setListBG();
            }
        }
        if (omori) {
            if (i == 3) {
                i = 1;
                bgm = &bg[1];
            } else if (i == 1) {
                bgm = &bg[3];
                i = 3;
            }
        }
    }

    clipper.mFar = (f32)defaultFar; /* mDoLib_clipper::resetFar() */
    Bg_J3DUClipper_calcViewFrustum(&clipper);
    dComIfGd_setList();
    dComIfGp_get();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, nullptr, &Bg_roomStatus(roomNo)->mTevStr);
    return TRUE;
}

/* 0207A05C */
void daBg_btkAnm_c::play() {
    u8 sp = special;
    mDoExt_btkAnm *a = anm;
    if (sp == 1) {
        /* anm->setFrame(dComIfGp_getWaveFrame()) */
        u16 wave = (u16)Bg_play()->mWaveFrame;
        a->mFrameCtrl.mFrame = (f32)wave;
    } else {
        a->play();
    }
}

/* 0207A0D8 */
u32 daBg_brkAnm_c::play() { return mDoExt_baseAnm_play(anm); }

/* 0207A9A0 */
static u8 cLib_calcTimer_u8(be<u8> *t) {
    u8 v = *t;
    if (v != 0) {
        v = v - 1;
        *t = v;
    }
    return v;
}

/* 0207A0E0 */
BOOL daBg_c::execute() {
    if (mUnloadTimer != 0) {
        if (cLib_calcTimer_u8(&mUnloadTimer) == 0)
            fopAcM_delete(this);
        return TRUE;
    }

    s32 roomNo = fopAcM_GetParam(this);
    dComIfGp_get();
    if ((u8)Bg_roomStatus(roomNo)->mFlags &
        ROOM_UNLOADING) { /* dComIfGp_roomControl_checkStatusFlag(roomNo, 4) */
        if (daBg_isStage(gabi::ea(stageSea.bytes)))
            mUnloadTimer = 16;
        else
            mUnloadTimer = 1;
    } else {
        daBg_BgModel *bgm = &bg[0];
        for (s32 i = 0; i < 4; i++, bgm++) {
            if ((u8)monotone == 0 /* !mDoGph_gInf_c::isMonotone() */ || i == 2) {
                if (bgm->btk != nullptr)
                    bgm->btk->play();
                if (bgm->brk != nullptr)
                    bgm->brk->play();
            }
        }
    }
    return TRUE;
}

/* 0207A2CC */
static BOOL daBg_IsDelete(daBg_c *i_this) { return i_this->mUnloadTimer == 0; }

// HD: destruction runs through the virtual destructor, not this adapter.
/* 0207A2DC */
static BOOL daBg_Delete(daBg_c *i_this) { return TRUE; }

/* 0207A2E4 */
void daBg_btkAnm_c::entry(J3DModelData *modelData) {
    anm->entry(modelData, 0.0f);

    /* anm->getBtkAnm()->getUpdateMaterialName()->getName(0) (HD: self-relative offsets) */
    BgAnimation *animation = reinterpret_cast<BgAnimation *>(anm.get());
    BgMaterialKey *key = reinterpret_cast<BgMaterialKey *>(animation->mpTextureFrame.get());
    BgMaterialNames *names = gabi::at<BgMaterialNames>(key->mNameData);
    u32 p = gabi::ea(&names->mTableOffset);
    u32 off = names->mTableOffset;
    u32 tab = off != 0 ? p + off : 0;
    BgNameTable *table = gabi::at<BgNameTable>(tab);
    u32 off2 = table->mFirstNameOffset;
    u32 nameAddress = off2 != 0 ? gabi::ea(&table->mFirstNameOffset) + off2 : 0;
    be<u8> *name = gabi::at<be<u8>>(nameAddress);
    if (name[0] == 'S' && name[1] == 'C' && name[2] == '_' && name[3] == '0' && name[4] == '1')
        special = 1;
    else
        special = 0;
}

/* 0207A3A4 */
void daBg_brkAnm_c::entry(J3DModelData *modelData) {
    Bg_brkEntry(anm, modelData, 0.0f);
    special = 0;
}

// HD: creation always uses its own solid heap, without the retail room-memory path.
/* 0207A3E4 */
static cPhs_State daBg_Create(daBg_c *i_this) {
    /* fopAcM_ct(this, daBg_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = DABG_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    s32 roomNo = fopAcM_GetParam(i_this);
    if (!fopAcM_entrySolidHeap(i_this, gabi::guest_fn((const void *)checkCreateHeap), 0)) {
        Bg_dStage_escapeRestart();
        return cPhs_ERROR_e;
    }

    daBg_BgModel *bgm = &i_this->bg[0];
    for (s32 i = 0; i < 4; i++, bgm++) {
        J3DModel *model = bgm->model;
        if (model == nullptr)
            continue;
        daBg_btkAnm_c *btk = bgm->btk;
        J3DModelData *modelData = J3DModel_getModelData(model);
        if (btk != nullptr)
            btk->entry(modelData);
        if (bgm->brk != nullptr)
            bgm->brk->entry(modelData);
    }

    gabi::Local<be<f32>> transX;
    gabi::Local<be<f32>> transZ;
    gabi::Local<be<s16>> angleY;
    if (Bg_dComIfGp_getMapTrans(roomNo, transX.get(), transZ.get(), angleY.get())) {
        daBg_BgModel *bgm = &i_this->bg[0];
        for (s32 i = 0; i < 4; i++, bgm++) {
            J3DModel *model = bgm->model;
            if (model == nullptr)
                continue;
            mDoMtx_stack_c::transS(*transX, 0.0f, *transZ);
            mDoMtx_stack_c::YrotM(*angleY);
            J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
        }
    }

    bool regFail = false;
    if (i_this->bgw) {
        dBgS *bgs = dComIfG_Bgsp();
        regFail = dBgS_Regist(bgs, i_this->bgw, i_this);
    }
    if (regFail) {
        Bg_dStage_escapeRestart();
        return cPhs_ERROR_e;
    }

    dComIfGp_get();
    BgRoomStatus *st = Bg_roomStatus(roomNo);
    Bg_dKy_tevstr_init(&st->mTevStr, roomNo, 0xFF);
    /* HD */
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, nullptr, &st->mTevStr);
    dComIfGp_get();
    st->mFlags |= ROOM_ACTIVE; /* dComIfGp_roomControl_onStatusFlag(roomNo, 0x10) */
    return cPhs_COMPLEATE_e;
}

/* 0207A628 */
static void daBg_c_dt(daBg_c *i_this, s32 flags) {
    if (i_this == nullptr)
        return;
    i_this->__vtbl = DABG_VTBL;
    s32 roomNo = fopAcM_GetParam(i_this);
    BgRoomStatus *st;

    if (i_this->heap && i_this->bgw) {
        dBgS *bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, i_this->bgw);
        st = Bg_roomStatus(roomNo);
        st->mpBgW = nullptr; /* dStage_roomControl_c::setBgW(roomNo, NULL) */
    } else {
        st = Bg_roomStatus(roomNo);
    }
    if ((u32)Bg_play()->mMagma != 0) /* magma */
        Bg_deleteMagmaRoom((u32)Bg_play()->mMagma, roomNo);
    if ((u32)Bg_play()->mGrass != 0) /* grass: mRoom[roomNo].deleteData() */
        Bg_deleteGrassRoom(gabi::ea(&gabi::at<BgGrassRoom>(Bg_play()->mGrass)->mRooms[roomNo]));
    if ((u32)Bg_play()->mTree != 0) /* tree */
        Bg_deleteTreeRoom(gabi::ea(&gabi::at<BgTree>(Bg_play()->mTree)->mRooms[roomNo]));
    if ((u32)Bg_play()->mWood != 0) /* wood: delete_room */
        Bg_deleteWoodRoom((u32)Bg_play()->mWood, roomNo);
    if ((u32)Bg_play()->mFlower != 0) /* flower */
        Bg_deleteFlowerRoom(gabi::ea(&gabi::at<BgFlower>(Bg_play()->mFlower)->mRooms[roomNo]));
    dComIfGp_get();
    st->mFlags &= ~ROOM_ACTIVE; /* offStatusFlag(roomNo, 0x10) */

    Bg_destroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(i_this);
}

// HD: compiler-generated and template code of this translation unit.
/* 0207A7C4 */
static void __sinit_d_a_bg_cpp() { sinit_header_statics(0x10461A18, 0x10190EDC); }

/* 0207A858 */
static daBg_SafeString *daBg_SafeString_ct(daBg_SafeString *p, u32 str) {
    if (p == nullptr) {
        p = (daBg_SafeString *)operator_new(8);
        if (p == nullptr)
            return p;
    }
    p->mStr = str;
    p->__vtbl = DABG_SAFESTRING_VTBL;
    return p;
}

/* 0207A8A8 */
static BOOL daBg_SafeString_eq(daBg_SafeString *a, daBg_SafeString *b) { return daBg_ss_cmp(a, b); }

/* 0207A988 */
static void daBg_SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0207A99C */
static void daBg_SafeString_assure(void *p) {}

/* WWHD glue (tools/verify/autoglue.py)
data arcName 104619F8 nocheck
data roomFormat 10008850 nocheck
data stageOmori 10008864 nocheck
data stageSea 10008860 nocheck
data clipper 1048CFF0
data defaultFar 1048D04C
data saveInfo 101F86E8
data currentOpaList 104B4634
data currentXluList 104B4638
data monotone 101F4829
*/
