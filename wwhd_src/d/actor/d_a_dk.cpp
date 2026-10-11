/**
 * d_a_dk.cpp (WWHD)
 * Helmaroc King (DK, "great monster bird") in the Forsaken Fortress / Outset demos: model, mask
 * and four feather tails.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_dk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_dk.h"

// HD: this translation unit's copies of inline class vtables and header statics.
const gc_tu_data gc_tu = {
    .safeString = 0x1000DCE8,
    .objAcch = {0x1000DD00, 0x1000DD20, 0x1000DD10},
    .objAcchDt = {0x1000DD10, 0x1000DD20},
    .headerStatics = {0x10463B9C, 0x101B4574},
};

#define DK_VTBL 0x1000DD30         /* dk_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000DCE8 /* this TU's sead::SafeString vtable */

// HD: only these two HIO fields remain; the other GC defaults are folded into the code.
struct daDk_HIO_c {
    be<f32> field_0x0C;
    be<f32> field_0x10;
};
// HD: separately emitted strings retain their TU addresses, including the repeated archive name.
struct DkStringView {
    char mText[1];
};
static DkStringView l_demoArcName;
static DkStringView l_bdkName;
static DkStringView l_flyEventName;
static DkStringView l_deleteArcName;
static DkStringView l_heapArcName;
static DkStringView l_demoAssertFile;
static DkStringView l_demoAssertExpression;
static DkStringView l_modelAssertFile;
static DkStringView l_modelAssertExpression;
static DkStringView l_createArcName;
static DkStringView l_demoName;
static DkStringView l_currentDemoArchive;
static cXyz l_playerJointOffset;
static daDk_HIO_c l_HIO;
using DkDemoWord = be<u32>;
static DkDemoWord m_demoFrame;
static DkDemoWord m_demoObject;
static const f32 tial_scale[9] = {0.25f, 0.275f, 0.2875f, 0.3f, 0.325f, 0.375f, 0.4625f, 0.575f, 0.8f};

/* 02124B6C */
static BOOL nodeCallBack(J3DNode *node, int calcTiming) {
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint *joint = J3DNode_toJoint(node);
        J3DModel *model = j3dSys.getModel();
        dk_class *a_this = static_cast<dk_class *>(model->getUserArea()); /* getUserArea */
        s32 jnt_no = reinterpret_cast<DkJointNumberView *>(joint)->mJntNo;
        if (a_this != nullptr) {
            PSMTXCopy(Dk_getAnmMtx(model, jnt_no), calc_mtx());

            s32 tail_idx = (jnt_no - DK_JNT_J_DK_O_LA2_e) / 2;
            if ((u32)tail_idx >= 4) /* HD: bounds check */
                return TRUE;

            gabi::Local<cXyz> pos_vec;
            pos_vec->y = 0.0f;
            pos_vec->x = 0.0f;
            pos_vec->z = 0.0f;
            MtxPosition(pos_vec, &a_this->tails[tail_idx].field_0x150[1]);
            pos_vec->x = -10.0f;
            MtxPosition(pos_vec, &a_this->tails[tail_idx].field_0x150[0]);
        }
    }

    return TRUE;
}

/* inlined into daDk_Draw */
static inline void kamen_draw(dk_class *a_this) {
    J3DModel *morfModel = a_this->field_0x2B8->getModel();
    auto buffer = morfModel->mpMtxBuffer.get();
    u16 flags = buffer->mFlags;
    u32 mtx = buffer->mpAnmMtx;
    J3DModel *model = a_this->mpModelKamen;
    buffer->mFlags = flags | 0x10;

    PSMTXCopy(gabi::at<Mtx34>(mtx + DK_JNT_J_DK_ATAMA1_e * 0x30), calc_mtx());
    cMtx_YrotM(calc_mtx(), 0x4000);
    cMtx_ZrotM(calc_mtx(), -0x4000);
    MtxTrans(REG0_F(0) * 0.01f, gabi::fmadds(REG0_F(1), 0.01f, 40.0f), gabi::fmadds(REG0_F(2), 0.01f, 125.0f),
             1);
    J3DModel_setBaseTRMtx(model, calc_mtx());

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &a_this->tevStr);
    mDoExt_modelUpdateDL(model);
}

/* inlined into daDk_Draw */
static inline void tail_draw(dk_class *a_this, tail_s *tail) {
    for (int i = 0; i < 9; i++) {
        cXyz *trans = &tail->field_0x024[i];
        csXyz *rot = &tail->field_0x09C[i];
        f32 scale = tial_scale[i] * l_HIO.field_0x0C;

        MtxTrans(trans->x, trans->y, trans->z, 0);
        MtxScale(scale, scale, scale, 1);
        cMtx_YrotM(calc_mtx(), rot->y);
        cMtx_XrotM(calc_mtx(), rot->x);

        J3DModel *model = tail->models[i];

        J3DModel_setBaseTRMtx(model, calc_mtx());
        setLightTevColorType(dKy_getEnvlight(), model, &a_this->tevStr);
        mDoExt_modelUpdate(model);
    }
}

/* 02124C44 */
static BOOL daDk_Draw(dk_class *a_this) {
    /* HD: l_HIO.field_0x14 is gone (0) */
    if (a_this->field_0xAEE != 0) {
        return TRUE;
    }

    if (a_this->field_0xAC5 > 2) {
        J3DModel *model = a_this->field_0x2B8->getModel();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
        setLightTevColorType(dKy_getEnvlight(), model, &a_this->tevStr);

        a_this->field_0x2B8->entryDL();

        kamen_draw(a_this);
        for (s32 i = 0; i < 4; i++) {
            tail_draw(a_this, &a_this->tails[i]);
        }
    }
    return TRUE;
}

/* inlined into daDk_Execute */
static inline void tail_control(dk_class *a_this, tail_s *tail) {
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> vec;
    cXyz_mi(&tail->field_0x150[1], diff, &tail->field_0x150[0]);
    vec->z = diff->z;
    vec->y = diff->y;
    vec->x = diff->x;

    tail->field_0x168.y = cM_atan2s(vec->x, vec->z);
    f32 xz_dist = std_sqrtf(gabi::fmadds(vec->x, vec->x, vec->z * vec->z));
    tail->field_0x168.x = -cM_atan2s(vec->y, xz_dist);

    f32 vz = REG0_F(5) + 30.0f; /* REG0_F(5) + 25.0f + 5.0f */
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = vz;
    cMtx_YrotS(calc_mtx(), tail->field_0x168.y);
    cMtx_XrotM(calc_mtx(), tail->field_0x168.x);
    MtxPosition(vec, &tail->field_0x170);

    tail->field_0x024[0].copy(tail->field_0x150[1]);

    f32 f22 = REG0_F(2) + 0.77000004f;
    f32 f21;
    if (a_this->field_0x2B4 >= 2) {
        f21 = -1000000000.0f; /* -G_CM3D_F_INF */
    } else {
        f21 = a_this->field_0x900.GetGroundH() + 5.0f;
    }

    gabi::Local<cXyz> pos_vec;
    for (int i = 1; i < 10; i++) {
        cXyz *cur = &tail->field_0x024[i];
        cXyz *prev = &tail->field_0x024[i - 1];
        cXyz *d8 = &tail->field_0x0D8[i];

        f32 multiplier = gabi::fnmsubs((f32)(i - 1), REG0_F(4) + 0.1f, 1.0f);
        f32 new_x = gabi::fmadds(tail->field_0x170.x, multiplier, d8->x);
        f32 new_y = gabi::fmadds(tail->field_0x170.y, multiplier, d8->y);
        f32 new_z = gabi::fmadds(tail->field_0x170.z, multiplier, d8->z);

        f32 y_base = cur->y + new_y;
        if (y_base < f21) {
            y_base = f21;
        }

        f32 tx = (cur->x - prev->x) + new_x;
        f32 tz = (cur->z - prev->z) + new_z;
        f32 ty = y_base - prev->y;

        s16 new_y_angle = cM_atan2s(tx, tz);
        f32 xz = std_sqrtf(gabi::fmadds(tx, tx, tz * tz));
        s16 new_x_angle = -cM_atan2s(ty, xz);
        tail->field_0x09C[i - 1].y = new_y_angle;
        tail->field_0x09C[i - 1].x = new_x_angle;

        f32 len = 20.0f * gabi::fmadds((f32)i, 0.03f, 0.25f);
        len = len + len;
        len = len * l_HIO.field_0x0C;
        len = len * l_HIO.field_0x10;
        vec->x = 0.0f;
        vec->y = 0.0f;
        vec->z = len;

        cMtx_YrotS(calc_mtx(), new_y_angle);
        cMtx_XrotM(calc_mtx(), new_x_angle);

        MtxPosition(vec, pos_vec);
        d8->copy(*cur);

        f32 nx = prev->x + pos_vec->x;
        cur->x = nx;
        f32 ny = prev->y + pos_vec->y;
        cur->y = ny;
        cur->z = prev->z + pos_vec->z;

        d8->x = (nx - d8->x) * f22;
        d8->y = (cur->y - d8->y) * f22;
        d8->z = (cur->z - d8->z) * f22;
    }
}

/* 02124F00 */
static BOOL daDk_Execute(dk_class *a_this) {
    a_this->field_0x2BC = a_this->field_0x2BC + 1;

    for (s32 i = 0; i < 5; i++) {
        if (a_this->field_0x2C0[i] != 0) {
            a_this->field_0x2C0[i] = a_this->field_0x2C0[i] - 1;
        }
    }

    /* HD: l_HIO.field_0x05 is gone (0); move() and daDk_demoProc() are empty */
    {
        f32 x = a_this->current.pos.x;
        f32 z = a_this->current.pos.z;
        f32 y = a_this->current.pos.y;
        a_this->eyePos.z = z;
        a_this->eyePos.x = x;
        a_this->eyePos.y = y + 100.0f;

        BOOL demo_set = dDemo_setDemoData(a_this, 0x6A, a_this->field_0x2B8, l_demoArcName.mText /* "Dk" */,
                                          0, nullptr, 0, 0);

        if (demo_set) {
            a_this->field_0xAEE = 0;
            /* daDk_delete_Bdk() */
            fopAc_ac_c *actor = fopAcM_searchFromName(l_bdkName.mText /* "Bdk" */, 0, 0);
            if (actor != nullptr) {
                fopAcM_delete(actor);
            }
        } else {
            a_this->field_0x2B8->play(&a_this->eyePos, 0, 0);
            a_this->field_0xAEE = 1;
        }

        switch (a_this->field_0xAC5) {
        case 0:
            /* HD: returns at once until the event bit is set */
            if (!dComIfGs_isEventBit(0x0310)) {
                return TRUE;
            }
            a_this->field_0xAC5 = 2;
            break;
        case 2:
            if (Dk_startCheckOld(dComIfGp_getPEvtManager(), l_flyEventName.mText /* "zelda_fly" */)) {
                a_this->field_0xAC5 = 3;
            }
            // fallthrough
        case 3:
            if (dComIfGs_isEventBit(0x0001)) {
                a_this->field_0xAC5 = -1;
                fopAcM_delete(a_this); /* HD */
            }
            break;
        default:
            break;
        }
    }

    if (a_this->field_0x2B4 < 2) {
        a_this->field_0x900.CrrPos(dComIfG_Bgsp());
    }

    /* HD: the scale is not set from l_HIO.field_0x08 */
    J3DModel *model = a_this->field_0x2B8->getModel();
    J3DModel_setBaseScale(model, &a_this->scale);

    mDoMtx_stack_c::transS(a_this->current.pos.x, a_this->current.pos.y, a_this->current.pos.z);
    mDoMtx_stack_c::YrotM(a_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), a_this->current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), a_this->current.angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    a_this->field_0x2B8->calc();
    for (s32 i = 0; i < 4; i++) {
        tail_control(a_this, &a_this->tails[i]);
    }

    /* HD: during the "Demo07" demo (frames 0x48E..0x672), the player's demo actor follows the
     * bird's joint 25 (rotated by 0x8000), and the player's byte at +0x68DE is set to 0xC (0 at
     * frame 0x672). The demo archive name is a SafeString compare against the string at 0x1047E6B8. */
    {
        gabi::Local<SafeString> a;
        gabi::Local<SafeString> b;
        a->__vtbl = SAFESTRING_VTBL;
        b->__vtbl = SAFESTRING_VTBL;
        a->mStringTop = gabi::ea(l_demoName.mText); /* "Demo07" */
        b->mStringTop = gabi::ea(l_currentDemoArchive.mText);
        Dk_assureString(a.get()); /* SafeString::assureTerminationImpl_ (direct) */
        gabi::call_ptr(gabi::at<DkSafeStringVtable>(a->__vtbl)->assureTermination, a.get());
        u32 pa = a->mStringTop;
        gabi::call_ptr(gabi::at<DkSafeStringVtable>(b->__vtbl)->assureTermination, b.get());
        u32 pb = b->mStringTop;
        if (pa != pb) {
            u32 i = 0;
            for (;; i++) {
                if (i >= 0x40001)
                    return TRUE;
                u8 ca = *gabi::at<be<u8>>(pa + i);
                u8 cb = *gabi::at<be<u8>>(pb + i);
                if (ca != cb)
                    return TRUE;
                if (ca == 0)
                    break;
            }
        }

        u32 frame = m_demoFrame;
        if (frame - 0x48E >= 0x1E5)
            return TRUE;
        auto player = reinterpret_cast<DkPlayerDemoView *>(
            reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.getPlayerPtr(0));
        if (frame == 0x672) {
            player->mDemoMode = 0;
            return TRUE;
        }
        player->mDemoMode = 0xC;
        Mtx34 *mtx = Dk_getAnmMtx(model, 25);
        gabi::Local<cXyz> pos;
        gabi::Local<csXyz> rot;
        PSMTXMultVec(mtx, &l_playerJointOffset, pos);
        mDoMtx_MtxToRot(mtx, rot);
        u8 id = player->demoActorID; /* demoActorID */
        u32 actor;
        if (id == 0 || id > 0x20) {
            actor = 0; /* dComIfGp_demo_getActor: NULL; the stores below then go to address 8.. */
        } else {
            if (m_demoObject == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
                JUT_ASSERT_fail(l_demoAssertFile.mText, 0x23A, l_demoAssertExpression.mText);
            actor = dDemo_object_getActor(m_demoObject, id);
        }
        // HD: add the offsets before translating the guest address: null actors still write at 8 and 0x20.
        auto translation = gabi::at<cXyz>(actor + offsetof(DkDemoActorView, mTranslation));
        auto rotation = gabi::at<csXyz>(actor + offsetof(DkDemoActorView, mRotation));
        auto words = reinterpret_cast<be<u32> *>(pos.get());
        auto output = reinterpret_cast<be<u32> *>(translation);
        output[0] = words[0];
        output[1] = words[1];
        output[2] = words[2];
        s16 ry = rot->y;
        rotation->x = 0;
        rotation->y = (s16)(ry + 0x8000);
        rotation->z = 0;
    }
    return TRUE;
}

/* 02125800 */
static BOOL daDk_IsDelete(dk_class *) { return TRUE; }

// HD: no mDoHIO_deleteChild
/* 02125808 */
static BOOL daDk_Delete(dk_class *a_this) {
    dComIfG_resDelete(&a_this->mPhs, l_deleteArcName.mText /* "Dk" */);
    a_this->field_0xACC.remove();
    return TRUE;
}

/* 02125858 */
static BOOL useHeapInit(fopAc_ac_c *i_this) {
    dk_class *a_this = (dk_class *)i_this;

    J3DModelData *data =
        (J3DModelData *)dComIfG_getObjectIDRes(l_heapArcName.mText /* "Dk" */, dRes_ID_DK_BDL_DK_e);
    J3DAnmTransform *anm =
        (J3DAnmTransform *)dComIfG_getObjectIDRes(l_heapArcName.mText, dRes_ID_DK_BCK_FLY1_e);
    a_this->field_0x2B8 =
        mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                               nullptr, 0x80000, 0x11020002);

    if (a_this->field_0x2B8 == nullptr || a_this->field_0x2B8->getModel() == nullptr) {
        return FALSE;
    }

    for (u16 i = 0; i < a_this->field_0x2B8->getModel()->getModelData()->getJointNum(); i++) {
        if (i == DK_JNT_J_DK_O_LA2_e || i == DK_JNT_J_DK_O_LB2_e || i == DK_JNT_J_DK_O_RA2_e ||
            i == DK_JNT_J_DK_O_RB2_e) {
            /* getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack) (HD joint array, 0x1C per
             * joint) */
            a_this->field_0x2B8->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(
                nodeCallBack);
        }
    }
    a_this->field_0x2B8->getModel()->mUserArea = gabi::ea(a_this); /* setUserArea */

    J3DModelData *modelData =
        (J3DModelData *)dComIfG_getObjectIDRes(l_heapArcName.mText, dRes_ID_DK_BDL_DK_KAMEN_e);
    a_this->mpModelKamen = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
    if (a_this->mpModelKamen == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData *)dComIfG_getObjectIDRes(l_heapArcName.mText, dRes_ID_DK_BDL_DK_TAIL_e);
    if (modelData == nullptr) /* JUT_ASSERT(0x380, modelData != NULL) */
        JUT_ASSERT_fail(l_modelAssertFile.mText, 0x380, l_modelAssertExpression.mText);

    for (s32 i = 0; i < 4; i++) {
        for (s32 j = 0; j < 9; j++) {
            a_this->tails[i].models[j] = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
            if (a_this->tails[i].models[j] == nullptr) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/* 02125AA0 */
static cPhs_State daDk_Create(fopAc_ac_c *i_this) {
    dk_class *a_this = (dk_class *)i_this;
    /* fopAcM_ct(i_this, dk_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = DK_VTBL;
            dBgS_AcchCir_ct(&a_this->field_0x8C0);
            dBgS_ObjAcch_ct(&a_this->field_0x900, gc_tu.objAcch);
            dPa_smokeEcallBack_ct(&a_this->field_0xACC, 1, 1, 0, 0);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    /* HD: the event checks of Execute's state 0 moved here */
    u8 prm = fopAcM_GetParam(a_this) & 0xFF;
    a_this->field_0x2B4 = prm;
    if (prm == 1) {
        if (dComIfGs_isEventBit(0x0310) && dComIfGs_isEventBit(0x0001)) {
            return cPhs_ERROR_e;
        }
        a_this->field_0xAC5 = 0;
    } else {
        a_this->field_0xAC5 = 10;
    }

    cPhs_State res = dComIfG_resLoad(&a_this->mPhs, l_createArcName.mText /* "Dk" */);
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, useHeapInit, 0x1BD80)) {
            return cPhs_ERROR_e;
        }

        i_this->cullMtx =
            gabi::ea(J3DModel_getBaseTRMtx(a_this->field_0x2B8->getModel())); /* fopAcM_SetMtx */
        fopAcM_SetMin_l(i_this, -1000.0f, -1000.0f, -1000.0f);
        fopAcM_SetMax_l(i_this, 1000.0f, 1000.0f, 1000.0f);

        /* HD: no mDoHIO_createChild */
        if (a_this->field_0x2B4 < 2) {
            a_this->field_0x900.Set(&i_this->current.pos, &i_this->old.pos, a_this, 1, &a_this->field_0x8C0,
                                    &i_this->speed, nullptr, nullptr);
        }
        a_this->field_0x8C0.SetWall(0.0f, 300.0f);
        a_this->field_0xAC8 = 0;
    }

    return res;
}

// static initialisation (header statics; l_HIO: HD keeps only two floats)
/* 02125CDC */
static void __sinit_d_a_dk_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    l_HIO.field_0x0C = 4.0f; /* l_HIO.field_0x0C */
    l_HIO.field_0x10 = 1.5f; /* l_HIO.field_0x10 */
}

// sead::SafeString deleting destructor (this TU's copy; vtable 0x1000DCE8 +0xC)
/* 02125D8C */
static void SafeString_dt(SafeString *s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

// dk_class deleting destructor (compiler-generated, HD virtual destructor)
/* 02125DA0 */
static void dk_class_dt(dk_class *i_this, s32 flags) {
    if (i_this != nullptr) {
        i_this->field_0x900.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
        i_this->field_0x900.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
        Dk_destroyAcch(&i_this->field_0x900, 0);
        Dk_destroyCircle(&i_this->field_0x8C0.m_cir, 2);
        Dk_destroyActor(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

// sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy)
/* 02125E24 */
static void SafeString_assureTermination(SafeString *) {}

/* WWHD glue
ext 025D9F38 fopAc_ac_c* fopAcM_searchFromName(const char*, u32, u32)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, void*, u32, s8)
ext 025445B8 BOOL Dk_startCheckOld(void*, const char*)
ext 025A5C04 void dPa_smokeEcallBack_ct(void*, u8, u8, u8, u8)
ext 025E2DA8 void mDoExt_modelUpdate(J3DModel*)
ext 025F232C void mDoMtx_MtxToRot(Mtx34*, csXyz*)
ext 02526E70 u32 dDemo_object_getActor(u32, u8)
ext 025D672C void fopAcM_SetMin_l(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax_l(fopAc_ac_c*, f32, f32, f32)

 ext 02125E24 void Dk_assureString(SafeString*)
 ext 024EFD9C void Dk_destroyAcch(dBgS_Acch*, s32)
 ext 02018034 void Dk_destroyCircle(cM3dGCir_l*, s32)
 ext 025D50BC void Dk_destroyActor(fopAc_ac_c*, s32)
 data m_demoFrame 101D600C
 data m_demoObject 101D5FFC
 data l_demoArcName 1000DDA8
 data l_bdkName 1000DCE4
 data l_flyEventName 1000DDC0
 data l_deleteArcName 1000DDCC
 data l_heapArcName 1000DDCF
 data l_demoAssertFile 1000DD50
 data l_demoAssertExpression 1000DD40
 data l_modelAssertFile 1000DDD4
 data l_modelAssertExpression 1000DDE0
 data l_createArcName 1000DE00
 data l_demoName 1000DDAC
 data l_currentDemoArchive 1047E6B8
 data l_playerJointOffset 1000DDB4
 data l_HIO 10463B94
 data tial_scale 101B4550
*/
