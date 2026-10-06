/**
 * d_a_dk.cpp (WWHD)
 * Helmaroc King (DK, "great monster bird") in the Forsaken Fortress / Outset demos: model, mask
 * and four feather tails.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_dk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define DK_VTBL 0x1000DD30         /* dk_class vtable (HD virtual destructor) */
#define SAFESTRING_VTBL 0x1000DCE8 /* this TU's sead::SafeString vtable */

enum {
    dRes_ID_DK_BCK_FLY1_e = 0x11,
    dRes_ID_DK_BDL_DK_e = 0x12,
    dRes_ID_DK_BDL_DK_KAMEN_e = 0x13,
    dRes_ID_DK_BDL_DK_TAIL_e = 0x14,
};
enum {
    DK_JNT_J_DK_ATAMA1_e = 0x18,
    DK_JNT_J_DK_O_LA2_e = 0x3A,
    DK_JNT_J_DK_O_LB2_e = 0x3C,
    DK_JNT_J_DK_O_RA2_e = 0x3E,
    DK_JNT_J_DK_O_RB2_e = 0x40,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 026067F4 HD: dRes_control_c::getIDRes(const sead::SafeString& arc, s32 id) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(u32 data) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, data) + 8); }
/* 025D9F38 fopAcM_searchFromName(name, param mask, param) */
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) { return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm); }
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
/* 025445B8 dEvent_manager_c::startCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
/* dComIfGs_isEventBit: dSv_event_c at save + 0x644 */
static inline BOOL dComIfGs_isEventBit(u16 f) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f);
}
/* 025A5C04 dPa_smokeEcallBack::dPa_smokeEcallBack(u8, u8, u8, u8) */
static inline void dPa_smokeEcallBack_ct(void* p, u8 a, u8 b, u8 c, u8 d) { gabi::call(0x025A5C04, p, a, b, c, d); }
/* 025E2DA8 mDoExt_modelUpdate(J3DModel*) */
static inline void mDoExt_modelUpdate(J3DModel* m) { gabi::call(0x025E2DA8, m); }
/* 025F232C mDoMtx_MtxToRot(const Mtx, csXyz*) */
static inline void mDoMtx_MtxToRot(Mtx34* m, csXyz* rot) { gabi::call(0x025F232C, m, rot); }
/* 02526E70 dDemo_object_c::getActor(u8 id) */
static inline u32 dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<u32>(0x02526E70, obj, id); }
/* 025D672C / 025D673C fopAcM_SetMin / fopAcM_SetMax */
static inline void fopAcM_SetMin_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax_l(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* HD J3D: J3DModel::getAnmMtx(jnt) through the matrix block at +0x2C (marks it dirty, flag 0x10) */
static inline Mtx34* J3DModel_getAnmMtx(J3DModel* model, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    u16 flags = gabi::load<u16>(blk + 4);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(flags | 0x10));
    return gabi::at<Mtx34>(mtx + jnt * 0x30);
}
/* j3dSys.getModel(): J3DModel* at 0x104B462C; userArea at +0xB8 */
static inline u32 j3dSys_getModel() { return gabi::load<u32>(0x104B462C); }
static inline f32 REG0_F_l(int i) { return REG_F(0, i); }

/* HD: daDk_HIO_c is reduced to two floats (field_0x0C at 0x10463B94, field_0x10 at 0x10463B98);
 * field_0x05, field_0x08 and field_0x14 are gone (their GameCube default values are folded in) */
static inline f32 l_HIO_field_0x0C() { return gabi::load<f32>(0x10463B94); }
static inline f32 l_HIO_field_0x10() { return gabi::load<f32>(0x10463B98); }
/* static f32 tial_scale[9] (.data) */
static inline f32 tial_scale(s32 i) { return gabi::load<f32>(0x101B4550 + 4 * i); }

struct tail_s {
    /* 0x000 */ gptr<J3DModel> models[9];
    /* 0x024 */ cXyz field_0x024[10];
    /* 0x09C */ csXyz field_0x09C[10];
    /* 0x0D8 */ cXyz field_0x0D8[10];
    /* 0x150 */ cXyz field_0x150[2];
    /* 0x168 */ csXyz field_0x168;
    /* 0x16E */ u8 _16E[2];
    /* 0x170 */ cXyz field_0x170;
};
WWHD_SIZE(tail_s, 0x17C);

struct dk_class : fopAc_ac_c {
    /* 0x3AC */ u8 field_0x290[0x1C];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ be<u8> field_0x2B4;
    /* 0x3D1 */ u8 _3D1[3];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> field_0x2B8;
    /* 0x3D8 */ be<s16> field_0x2BC;
    /* 0x3DA */ u8 field_0x2BE[2];
    /* 0x3DC */ be<s16> field_0x2C0[5];
    /* 0x3E6 */ u8 _3E6[2];
    /* 0x3E8 */ tail_s tails[4];
    /* 0x9D8 */ gptr<J3DModel> mpModelKamen;
    /* 0x9DC */ dBgS_AcchCir field_0x8C0;
    /* 0xA1C */ dBgS_ObjAcch field_0x900;
    /* 0xBE0 */ u8 field_0xAC4;
    /* 0xBE1 */ be<s8> field_0xAC5;
    /* 0xBE2 */ u8 _BE2[2];
    /* 0xBE4 */ be<s32> field_0xAC8;
    /* 0xBE8 */ u8 field_0xACC[0x20]; /* dPa_smokeEcallBack (vtable at +0) */
    /* 0xC08 */ u8 field_0xAEC[2];
    /* 0xC0A */ be<u8> field_0xAEE;
    /* 0xC0B */ u8 _C0B;
};
WWHD_OFFSET(dk_class, tails, 0x3E8);
WWHD_OFFSET(dk_class, field_0x900, 0xA1C);
WWHD_OFFSET(dk_class, field_0xAEE, 0xC0A);
WWHD_SIZE(dk_class, 0xC0C);

/* 02124B6C */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02124B6C, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = gabi::at<J3DModel>(j3dSys_getModel());
        dk_class* a_this = gabi::at<dk_class>(gabi::load<u32>(gabi::ea(model) + 0xB8)); /* getUserArea */
        s32 jnt_no = gabi::load<u16>(gabi::ea(joint) + 4);
        if (a_this != nullptr) {
            PSMTXCopy(J3DModel_getAnmMtx(model, jnt_no), calc_mtx());

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
VERIFY(0x02124B6C, nodeCallBack);

/* kamen_draw (inlined into Draw) */
static inline void kamen_draw(dk_class* a_this) {
    J3DModel* morfModel = a_this->field_0x2B8->getModel();
    u32 blk = gabi::load<u32>(gabi::ea(morfModel) + 0x2C);
    u16 flags = gabi::load<u16>(blk + 4);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    J3DModel* model = a_this->mpModelKamen;
    gabi::store<u16>(blk + 4, (u16)(flags | 0x10));

    PSMTXCopy(gabi::at<Mtx34>(mtx + DK_JNT_J_DK_ATAMA1_e * 0x30), calc_mtx());
    cMtx_YrotM(calc_mtx(), 0x4000);
    cMtx_ZrotM(calc_mtx(), -0x4000);
    MtxTrans(REG0_F_l(0) * 0.01f, gabi::fmadds(REG0_F_l(1), 0.01f, 40.0f), gabi::fmadds(REG0_F_l(2), 0.01f, 125.0f), 1);
    J3DModel_setBaseTRMtx(model, calc_mtx());

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &a_this->tevStr);
    mDoExt_modelUpdateDL(model);
}

/* tail_draw (inlined into Draw) */
static inline void tail_draw(dk_class* a_this, tail_s* tail) {
    for (int i = 0; i < 9; i++) {
        cXyz* trans = &tail->field_0x024[i];
        csXyz* rot = &tail->field_0x09C[i];
        f32 scale = tial_scale(i) * l_HIO_field_0x0C();

        MtxTrans(trans->x, trans->y, trans->z, 0);
        MtxScale(scale, scale, scale, 1);
        cMtx_YrotM(calc_mtx(), rot->y);
        cMtx_XrotM(calc_mtx(), rot->x);

        J3DModel* model = tail->models[i];

        J3DModel_setBaseTRMtx(model, calc_mtx());
        setLightTevColorType(dKy_getEnvlight(), model, &a_this->tevStr);
        mDoExt_modelUpdate(model);
    }
}

/* 02124C44 */
static BOOL daDk_Draw(dk_class* a_this) {
    WWHD_FUNC(0x02124C44, BOOL, a_this);
    /* HD: l_HIO.field_0x14 is gone (0) */
    if (a_this->field_0xAEE != 0) {
        return TRUE;
    }

    if (a_this->field_0xAC5 > 2) {
        J3DModel* model = a_this->field_0x2B8->getModel();
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
VERIFY(0x02124C44, daDk_Draw);

/* tail_control (inlined into Execute) */
static inline void tail_control(dk_class* a_this, tail_s* tail) {
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> vec;
    cXyz_mi(&tail->field_0x150[1], diff, &tail->field_0x150[0]);
    vec->z = diff->z;
    vec->y = diff->y;
    vec->x = diff->x;

    tail->field_0x168.y = cM_atan2s(vec->x, vec->z);
    f32 xz_dist = std_sqrtf(gabi::fmadds(vec->x, vec->x, vec->z * vec->z));
    tail->field_0x168.x = -cM_atan2s(vec->y, xz_dist);

    f32 vz = REG0_F_l(5) + 30.0f; /* REG0_F(5) + 25.0f + 5.0f */
    vec->x = 0.0f;
    vec->y = 0.0f;
    vec->z = vz;
    cMtx_YrotS(calc_mtx(), tail->field_0x168.y);
    cMtx_XrotM(calc_mtx(), tail->field_0x168.x);
    MtxPosition(vec, &tail->field_0x170);

    tail->field_0x024[0].copy(tail->field_0x150[1]);

    f32 f22 = REG0_F_l(2) + 0.77000004f;
    f32 f21;
    if (a_this->field_0x2B4 >= 2) {
        f21 = -1000000000.0f; /* -G_CM3D_F_INF */
    } else {
        f21 = a_this->field_0x900.GetGroundH() + 5.0f;
    }

    gabi::Local<cXyz> pos_vec;
    for (int i = 1; i < 10; i++) {
        cXyz* cur = &tail->field_0x024[i];
        cXyz* prev = &tail->field_0x024[i - 1];
        cXyz* d8 = &tail->field_0x0D8[i];

        f32 multiplier = gabi::fnmsubs((f32)(i - 1), REG0_F_l(4) + 0.1f, 1.0f);
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
        len = len * l_HIO_field_0x0C();
        len = len * l_HIO_field_0x10();
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
static BOOL daDk_Execute(dk_class* a_this) {
    WWHD_FUNC(0x02124F00, BOOL, a_this);
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

        BOOL demo_set = dDemo_setDemoData(a_this, 0x6A, a_this->field_0x2B8, STR(0x1000DDA8) /* "Dk" */, 0, nullptr, 0, 0);

        if (demo_set) {
            a_this->field_0xAEE = 0;
            /* daDk_delete_Bdk() */
            fopAc_ac_c* actor = fopAcM_searchFromName(STR(0x1000DCE4) /* "Bdk" */, 0, 0);
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
            if (dComIfGp_evmng_startCheckOld(STR(0x1000DDC0) /* "zelda_fly" */)) {
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
    J3DModel* model = a_this->field_0x2B8->getModel();
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
        a->mStringTop = 0x1000DDAC; /* "Demo07" */
        b->mStringTop = 0x1047E6B8;
        gabi::call(0x02125E24, a.get()); /* SafeString::assureTerminationImpl_ (direct) */
        gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
        u32 pa = a->mStringTop;
        gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
        u32 pb = b->mStringTop;
        if (pa != pb) {
            u32 i = 0;
            for (;; i++) {
                if (i >= 0x40001)
                    return TRUE;
                u8 ca = gabi::load<u8>(pa + i);
                u8 cb = gabi::load<u8>(pb + i);
                if (ca != cb)
                    return TRUE;
                if (ca == 0)
                    break;
            }
        }

        u32 frame = gabi::load<u32>(0x101D600C);
        if (frame - 0x48E >= 0x1E5)
            return TRUE;
        u32 player = gabi::load<u32>(dComIfGp_ea() + 0x5B34);
        if (frame == 0x672) {
            gabi::store<u8>(player + 0x68DE, 0);
            return TRUE;
        }
        gabi::store<u8>(player + 0x68DE, 0xC);
        Mtx34* mtx = J3DModel_getAnmMtx(model, 25);
        gabi::Local<cXyz> pos;
        gabi::Local<csXyz> rot;
        PSMTXMultVec(mtx, gabi::at<cXyz>(0x1000DDB4), pos);
        mDoMtx_MtxToRot(mtx, rot);
        u8 id = gabi::load<u8>(player + 0x2DC); /* demoActorID */
        u32 actor;
        if (id == 0 || id > 0x20) {
            actor = 0; /* dComIfGp_demo_getActor: NULL; the stores below then go to address 8.. */
        } else {
            if (gabi::load<u32>(0x101D5FFC) == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
                JUT_ASSERT_fail(STR(0x1000DD50), 0x23A, STR(0x1000DD40));
            actor = dDemo_object_getActor(gabi::load<u32>(0x101D5FFC), id);
        }
        gabi::store<u32>(actor + 8, gabi::load<u32>(gabi::ea(pos.get()) + 0));
        gabi::store<u32>(actor + 0xC, gabi::load<u32>(gabi::ea(pos.get()) + 4));
        gabi::store<u32>(actor + 0x10, gabi::load<u32>(gabi::ea(pos.get()) + 8));
        s16 ry = rot->y;
        gabi::store<s16>(actor + 0x20, 0);
        gabi::store<s16>(actor + 0x22, (s16)(ry + 0x8000));
        gabi::store<s16>(actor + 0x24, 0);
    }
    return TRUE;
}
VERIFY(0x02124F00, daDk_Execute);

/* 02125800 */
static BOOL daDk_IsDelete(dk_class*) {
    WWHD_FUNC(0x02125800, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02125800, daDk_IsDelete);

/* 02125808: HD: no mDoHIO_deleteChild */
static BOOL daDk_Delete(dk_class* a_this) {
    WWHD_FUNC(0x02125808, BOOL, a_this);
    dComIfG_resDelete(&a_this->mPhs, STR(0x1000DDCC) /* "Dk" */);
    u32 cb = gabi::ea(a_this->field_0xACC);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(cb) + 0x44), cb); /* field_0xACC.remove() (virtual) */
    return TRUE;
}
VERIFY(0x02125808, daDk_Delete);

/* 02125858 */
static BOOL useHeapInit(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02125858, BOOL, i_this);
    dk_class* a_this = (dk_class*)i_this;

    J3DModelData* data = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1000DDCF) /* "Dk" */, dRes_ID_DK_BDL_DK_e);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x1000DDCF), dRes_ID_DK_BCK_FLY1_e);
    a_this->field_0x2B8 = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                                 nullptr, 0x80000, 0x11020002);

    if (a_this->field_0x2B8 == nullptr || a_this->field_0x2B8->getModel() == nullptr) {
        return FALSE;
    }

    for (u16 i = 0; i < J3DModelData_getJointNum(gabi::load<u32>(gabi::ea(a_this->field_0x2B8->getModel()) + 0xAC)); i++) {
        if (i == DK_JNT_J_DK_O_LA2_e || i == DK_JNT_J_DK_O_LB2_e || i == DK_JNT_J_DK_O_RA2_e || i == DK_JNT_J_DK_O_RB2_e) {
            /* getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack) (HD joint array, 0x1C per joint) */
            u32 md = gabi::load<u32>(gabi::ea(a_this->field_0x2B8->getModel()) + 0xAC);
            u32 n = gabi::load<u32>(md + 4);
            u32 p = gabi::load<u32>(md + 8);
            if (i < n)
                p += i * 0x1C;
            gabi::store<u32>(p + 8, 0x02124B6C);
        }
    }
    gabi::store<u32>(gabi::ea(a_this->field_0x2B8->getModel()) + 0xB8, gabi::ea(a_this)); /* setUserArea */

    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1000DDCF), dRes_ID_DK_BDL_DK_KAMEN_e);
    a_this->mpModelKamen = mDoExt_J3DModel__create(modelData, 0x80000, 0x11020002);
    if (a_this->mpModelKamen == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x1000DDCF), dRes_ID_DK_BDL_DK_TAIL_e);
    if (modelData == nullptr) /* JUT_ASSERT(0x380, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1000DDD4), 0x380, STR(0x1000DDE0));

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
VERIFY(0x02125858, useHeapInit);

/* 02125AA0 */
static cPhs_State daDk_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02125AA0, cPhs_State, i_this);
    dk_class* a_this = (dk_class*)i_this;
    /* fopAcM_ct(i_this, dk_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = DK_VTBL;
            dBgS_AcchCir_ct(&a_this->field_0x8C0);
            dBgS_ObjAcch_ct(&a_this->field_0x900, dBgS_ObjAcch_vt{0x1000DD00, 0x1000DD20, 0x1000DD10});
            dPa_smokeEcallBack_ct(a_this->field_0xACC, 1, 1, 0, 0);
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

    cPhs_State res = dComIfG_resLoad(&a_this->mPhs, STR(0x1000DE00) /* "Dk" */);
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, 0x02125858 /* useHeapInit */, 0x1BD80)) {
            return cPhs_ERROR_e;
        }

        i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(a_this->field_0x2B8->getModel())); /* fopAcM_SetMtx */
        fopAcM_SetMin_l(i_this, -1000.0f, -1000.0f, -1000.0f);
        fopAcM_SetMax_l(i_this, 1000.0f, 1000.0f, 1000.0f);

        /* HD: no mDoHIO_createChild */
        if (a_this->field_0x2B4 < 2) {
            a_this->field_0x900.Set(&i_this->current.pos, &i_this->old.pos, a_this, 1, &a_this->field_0x8C0, &i_this->speed,
                                    nullptr, nullptr);
        }
        a_this->field_0x8C0.SetWall(0.0f, 300.0f);
        a_this->field_0xAC8 = 0;
    }

    return res;
}
VERIFY(0x02125AA0, daDk_Create);

/* 02125CDC: static initialisation (header statics; l_HIO: HD keeps only two floats) */
static void __sinit_d_a_dk_cpp() {
    WWHD_FUNC(0x02125CDC, void, (u32)0);
    sinit_header_statics(0x10463B9C, 0x101B4574);
    gabi::store<f32>(0x10463B94, 4.0f); /* l_HIO.field_0x0C */
    gabi::store<f32>(0x10463B98, 1.5f); /* l_HIO.field_0x10 */
}
VERIFY(0x02125CDC, __sinit_d_a_dk_cpp);

/* 02125D8C: sead::SafeString deleting destructor (this TU's copy; vtable 0x1000DCE8 +0xC) */
static void SafeString_dt(SafeString* s, s32 flags) {
    WWHD_FUNC(0x02125D8C, void, s, flags);
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}
VERIFY(0x02125D8C, SafeString_dt);

/* 02125DA0: dk_class deleting destructor (compiler-generated, HD virtual destructor) */
static void dk_class_dt(dk_class* i_this, s32 flags) {
    WWHD_FUNC(0x02125DA0, void, i_this, flags);
    if (i_this != nullptr) {
        u32 acch = gabi::ea(&i_this->field_0x900);
        gabi::store<u32>(acch + 0x20, 0x1000DD10); /* ~dBgS_ObjAcch: its vtables */
        gabi::store<u32>(acch + 0x14, 0x1000DD20);
        gabi::call(0x024EFD9C, acch, 0);                           /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02018034, gabi::ea(&i_this->field_0x8C0) + 0x14, 2); /* ~dBgS_AcchCir: its cM3dGCir */
        gabi::call(0x025D50BC, i_this, 0);                         /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02125DA0, dk_class_dt);

/* 02125E24: sead::SafeString virtual at vtable +0x14 (assureTerminationImpl_, empty in this copy) */
static void SafeString_assureTermination(SafeString*) {
    WWHD_FUNC(0x02125E24, void, (u32)0);
}
VERIFY(0x02125E24, SafeString_assureTermination);
