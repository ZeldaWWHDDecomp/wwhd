/**
 * d_a_obj_stair.cpp (WWHD)
 * Object - Crumbling stair (Mkdan; breaks when hit by a bomb or stepped on).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_stair.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002F7F0) /* "Mkdan" */
#define SAFESTRING_VTBL 0x1002F6BC
#define STAIR_VTBL 0x1002F7F8
#define AAB_VTBL 0x1002F6D4 /* cM3dGAab (per TU) */
#define cps_src 0x101CCCCC

enum { dRes_INDEX_MKDAN_BDL_MKDAN1_e = 4, dRes_INDEX_MKDAN_DZB_MKDAN1_e = 7 };
enum { cPhs_STOP_e = 3 };
enum { fpcNm_PLAYER_e = 0xA8, fpcNm_BOMB_e = 0x126 };
enum { JA_SE_OBJ_BREAK_STEPS = 0x692D };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
struct Quaternion { be<f32> x, y, z, w; };
/* 0201B12C cXyz::normZP() (this, result) / 0201B080 cXyz::outprod(this, result, other) */
static inline void cXyz_normZP(const cXyz* a, cXyz* res) { gabi::call(0x0201B12C, a, res); }
static inline void cXyz_outprod(const cXyz* a, cXyz* res, const cXyz* b) { gabi::call(0x0201B080, a, res, b); }
/* cXyz::abs2 / abs (inline): PSVECSquareMag, std::sqrtf */
static inline f32 cXyz_abs2(const cXyz* a) { return PSVECSquareMag(a); }
static inline f32 cXyz_abs(const cXyz* a) { return std_sqrtf(PSVECSquareMag(a)); }
/* 028E8B48 PSQUATMultiply(a, b, out) */
static inline void PSQUATMultiply(const Quaternion* a, const Quaternion* b, Quaternion* out) { gabi::call(0x028E8B48, a, b, out); }
/* 025F25CC mDoMtx_stack_c::quatM(q) (static) */
static inline void mDoMtx_stack_quatM(const Quaternion* q) { gabi::call(0x025F25CC, q); }
/* 025164C0 dCcD_Cps::Set(src) / 02018808 cM3dGCps::SetStartEnd(start, end) */
static inline void dCcD_Cps_Set(dCcD_Cps* c, u32 src) { gabi::call(0x025164C0, c, src); }
static inline void cM3dGCps_SetStartEnd(void* cps, const cXyz* s, const cXyz* e) { gabi::call(0x02018808, cps, s, e); }
/* inline dCcD_Cps constructor (HD): GObjInf, ShapeAttr, per-TU cM3dGAab vtable, cM3dGCps (02018150) */
static inline void dCcD_Cps_ct(dCcD_Cps* c, u32 tu_aab_vtbl) {
    gabi::call(0x02515FB8, c);                          /* dCcD_GObjInf::dCcD_GObjInf */
    gabi::store<u32>(gabi::ea(c) + 0x114, 0x100015A8);  /* cCcD_ShapeAttr */
    gabi::store<u32>(gabi::ea(c) + 0x110, tu_aab_vtbl); /* cM3dGAab (per TU) */
    gabi::call(0x02018150, gabi::ea(c) + 0x118);        /* cM3dGCps::cM3dGCps */
    gabi::store<u32>(gabi::ea(c) + 0x114, 0x1004AF70);
    c->__vtbl_hitinf = 0x1004AF18;                      /* dCcD_Cps */
    gabi::store<u32>(gabi::ea(c) + 0x130, 0x1004AF60);  /* cM3dGCps */
}
static inline s16 fopAcM_GetProfName(fopAc_ac_c* a) { return gabi::load<s16>(gabi::ea(a) + 0xE); }

/* HD daobj_stairHIO_c (0x1C, vtable last): m0C is -175 (GameCube -70) */
struct daobj_stairHIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<f32> m0C;
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<f32> m14;
    /* 0x14 */ be<s16> m18;
    /* 0x16 */ u8 _16[2];
    /* 0x18 */ be<u32> __vtbl;
};
static daobj_stairHIO_c& l_HIO() { return *gabi::at<daobj_stairHIO_c>(0x1046BF14); }

struct daObj_Stair_c : dBgS_MoveBgActor {
    cPhs_State _create();
    bool _delete();
    bool _execute();
    BOOL CreateHeap();
    BOOL Create();
    BOOL Execute(Mtx34**);
    BOOL Draw();

    /* 0x3E0 */ cXyz field_0x2C8;
    /* 0x3EC */ cXyz field_0x2D4;
    /* 0x3F8 */ be<s32> field_0x2E0;
    /* 0x3FC */ be<s32> field_0x2E4;
    /* 0x400 */ be<f32> field_0x2E8;
    /* 0x404 */ be<f32> field_0x2EC;
    /* 0x408 */ request_of_phase_process_class mPhs;
    /* 0x410 */ gptr<J3DModel> mpModel;
    /* 0x414 */ Mtx34 field_0x2FC;
    /* 0x444 */ dCcD_Stts mStts;
    /* 0x480 */ dCcD_Cps mCps;
    /* 0x5B8 */ Quaternion field_0x4A0;
    /* 0x5C8 */ Quaternion field_0x4B0;
    /* 0x5D8 */ cXyz field_0x4C0;
};
WWHD_OFFSET(daObj_Stair_c, mStts, 0x444);
WWHD_OFFSET(daObj_Stair_c, field_0x4A0, 0x5B8);
WWHD_SIZE(daObj_Stair_c, 0x5E4);

/* 023905F4 */
static void ride_call_back(dBgW* i_arg0, fopAc_ac_c* i_arg1, fopAc_ac_c* i_arg2) {
    WWHD_FUNC(0x023905F4, void, i_arg0, i_arg1, i_arg2);
    daObj_Stair_c* i_this = (daObj_Stair_c*)i_arg1;
    /* HD: fopAcM_GetProfName checks the actor for NULL */
    if (i_arg2 != nullptr && fopAcM_GetProfName(i_arg2) == fpcNm_PLAYER_e && i_this->field_0x2E0 < l_HIO().m18) {
        i_this->field_0x2E4 = 1;
        gabi::Local<cXyz> d;
        cXyz_mi(&i_arg2->current.pos, d, &i_arg1->current.pos);
        i_this->field_0x2C8.copy(*d.get());
        i_this->field_0x2D4.x = 0.0f;
        i_this->field_0x2EC = 1.0f;
        i_this->field_0x2D4.y = -1.0f;
        i_this->field_0x2D4.z = 0.0f;
    }
}
VERIFY(0x023905F4, ride_call_back);

/* 02391248 */
BOOL daObj_Stair_c::CreateHeap() {
    WWHD_FUNC(0x02391248, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKDAN_BDL_MKDAN1_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0xD1, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1002F7A8), 0xD1, STR(0x1002F7BC));
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (l_HIO().mNo < 0) {
        l_HIO().mNo = mDoHIO_createChild(STR(0x1002F7D0) /* "崩れる階段" */, &l_HIO());
    }
    return mpModel != nullptr;
}
VERIFY(0x02391248, &daObj_Stair_c::CreateHeap);

/* 0239130C */
BOOL daObj_Stair_c::Create() {
    WWHD_FUNC(0x0239130C, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    return TRUE;
}
VERIFY(0x0239130C, &daObj_Stair_c::Create);

/* 02391328: set_mtx inlined */
BOOL daObj_Stair_c::Execute(Mtx34** mtx) {
    WWHD_FUNC(0x02391328, BOOL, this, mtx);
    gabi::Local<Quaternion> quat;
    PSQUATMultiply(&field_0x4A0, &field_0x4B0, quat);
    for (u32 i = 0; i < 16; i += 4) /* field_0x4A0 = quat */
        gabi::store<u32>(gabi::ea(&field_0x4A0) + i, gabi::load<u32>(gabi::ea(quat.get()) + i));

    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y + field_0x2E8, current.pos.z);
    mDoMtx_stack_quatM(&field_0x4A0);
    if (field_0x2E4 == 1) {
        gabi::Local<cXyz> temp;
        temp->set(0.0f, 0.0f, 0.0f);
        f32 r = cM_rndFX(175.0f);
        temp->x = 0.0f + r;
        f32 r2 = cM_rndFX(40.0f);
        temp->z = temp->z + r2;
        PSMTXMultVec(mDoMtx_stack_c::get(), temp, &field_0x4C0);
    }
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &field_0x2FC);
    *gabi::at<be<u32>>(gabi::ea(mtx)) = gabi::ea(&field_0x2FC);
    return TRUE;
}
VERIFY(0x02391328, &daObj_Stair_c::Execute);

/* 023914D8 */
BOOL daObj_Stair_c::Draw() {
    WWHD_FUNC(0x023914D8, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x023914D8, &daObj_Stair_c::Draw);

/* 0239069C: daObj_StairCreate (_create inlined) */
cPhs_State daObj_Stair_c::_create() {
    WWHD_FUNC(0x0239069C, cPhs_State, this);
    cPhs_State phase_state;
    /* fopAcM_ct(this, daObj_Stair_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = STAIR_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cps_ct(&mCps, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    u32 switchIdx = fopAcM_GetParam(this) & 0xFF;

    if (switchIdx != 0xFF && fopAcM_isSwitch(this, switchIdx)) {
        return cPhs_STOP_e;
    }
    phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        /* Quaternion quat = {0, 0, 0, 1} (.rodata 1002F6F4, copied as words) */
        u32 q0 = gabi::load<u32>(0x1002F6F4), q1 = gabi::load<u32>(0x1002F6F8);
        u32 q2 = gabi::load<u32>(0x1002F6FC), q3 = gabi::load<u32>(0x1002F700);
        f32 f29 = 0.0f;
        f32 sn = cM_ssin(current.angle.y >> 1);
        f32 cs = cM_scos(current.angle.y >> 1);
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MKDAN_DZB_MKDAN1_e, 0, 0x8A0);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x113, ...) */
            JUT_ASSERT_fail(STR(0x1002F704), 0x113, STR(0x1002F718));

        field_0x4A0.x = f29;
        field_0x4A0.y = sn;
        field_0x4A0.z = f29;
        field_0x4A0.w = cs;
        u32 b = gabi::ea(&field_0x4B0);
        gabi::store<u32>(b + 0, q0);
        gabi::store<u32>(b + 4, q1);
        gabi::store<u32>(b + 8, q2);
        gabi::store<u32>(b + 12, q3);
        field_0x2E0 = 0;

        gabi::store<u32>(gabi::ea(mpBgW.get()) + 0xB0, 0x023905F4); /* mpBgW->SetRideCallback(ride_call_back) */
        mStts.Init(0xFF, 0xFF, this);
        dCcD_Cps_Set(&mCps, cps_src);
        mCps.SetStts(&mStts);

        gabi::Local<cXyz> start;
        gabi::Local<cXyz> end;
        end->copy(current.pos);
        start->copy(*end.get());
        s16 ay = current.angle.y;
        f32 c = cM_scos(ay), s = cM_ssin(ay);
        f32 ex = gabi::fmadds(-125.0f, c, current.pos.x);
        f32 ez = gabi::fmadds(125.0f, s, current.pos.z);
        start->x = gabi::fmadds(125.0f, c, start->x);
        start->z = gabi::fmadds(-125.0f, s, start->z);
        end->x = ex;
        end->z = ez;
        cM3dGCps_SetStartEnd(gabi::at<u8>(gabi::ea(&mCps) + 0x118), start, end);
        field_0x2E8 = 0.0f;
        field_0x2EC = 0.0f;
        gabi::store<f32>(gabi::ea(&mCps) + 0x134, 50.0f); /* mCps.SetR(50.0f) */
    }
    return phase_state;
}
VERIFY(0x0239069C, &daObj_Stair_c::_create);

/* 02390A58: daObj_StairDelete (_delete inlined) */
bool daObj_Stair_c::_delete() {
    WWHD_FUNC(0x02390A58, bool, this);
    if (l_HIO().mNo >= 0) {
        mDoHIO_deleteChild(l_HIO().mNo);
        l_HIO().mNo = -1;
    }
    dComIfG_resDelete(&mPhs, M_arcname);
    return MoveBGDelete() != 0;
}
VERIFY(0x02390A58, &daObj_Stair_c::_delete);

/* 02390AC4: daObj_StairExecute (_execute inlined) */
bool daObj_Stair_c::_execute() {
    WWHD_FUNC(0x02390AC4, bool, this);
    if (field_0x2E4 == 1) {
        if (field_0x2E0 == l_HIO().m18) {
            gabi::Local<cXyz> temp;
            cXyz_normZP(&field_0x2C8, temp);
            gabi::Local<cXyz> op;
            cXyz_outprod(temp, op, &field_0x2D4);
            gabi::Local<cXyz> temp2;
            temp2->copy(*op.get());

            if (cXyz_abs(temp2) > 8e-11f) {
                gabi::Local<cXyz> temp3;
                cXyz_normZP(temp2, temp3);
                f32 len = cXyz_abs(&field_0x2C8);
                f32 f2EC = field_0x2EC;
                f32 temp4 = l_HIO().m10 * 0.5f * len * 0.01f * f2EC;
                s16 ang = (s16)gabi::ftoi(temp4 * 182.04444885253906f); /* DEG2S */
                f32 sn = cM_ssin(ang);
                f32 cs = cM_scos(ang);

                field_0x4B0.w = cs;
                field_0x4B0.y = sn * temp3->y;
                field_0x4B0.x = sn * temp3->x;
                field_0x4B0.z = sn * temp3->z;

                speed.x = l_HIO().m14 * cs * f2EC * field_0x2D4.x;
                speed.y = l_HIO().m14 * cs * f2EC * field_0x2D4.y;
                speed.z = l_HIO().m14 * cs * field_0x2EC * field_0x2D4.z;
            } else {
                field_0x4B0.y = 0.0f;
                field_0x4B0.z = 0.0f;
                field_0x4B0.x = 0.0f;
                field_0x4B0.w = 1.0f;

                f32 f2EC = field_0x2EC;
                speed.x = f2EC * l_HIO().m14 * field_0x2D4.x;
                speed.y = f2EC * l_HIO().m14 * field_0x2D4.y;
                speed.z = f2EC * l_HIO().m14 * field_0x2D4.z;
            }
            /* fopAcM_seStart(this, JA_SE_OBJ_BREAK_STEPS, 0) */
            s32 reverb = dComIfGp_getReverb(current.roomNo);
            mDoAud_seStart(JA_SE_OBJ_BREAK_STEPS, &eyePos, 0, reverb);
        }

        field_0x2E0 = field_0x2E0 + 1;

        if (field_0x2E0 > l_HIO().m18) {
            cLib_addCalc2(&speed.y, l_HIO().m0C, 0.5f, l_HIO().m08);
            current.pos.y = current.pos.y + speed.y;
        }
        if (field_0x2E0 > l_HIO().m18 + 0xF0) {
            fopAcM_delete(this);
        }
    } else {
        dComIfG_Ccsp_Set(&mCps);

        if (mCps.ChkTgHit()) {
            fopAc_ac_c* ac = mCps.GetTgHitAc();
            if (ac) {
                /* HD: fopAcM_GetProfName checks the actor for NULL */
                if (fopAc_IsActor(ac) && ac != nullptr && fopAcM_GetProfName(ac) == fpcNm_BOMB_e) {
                    f32 sn = cM_ssin(current.angle.y);
                    f32 cs = cM_scos(current.angle.y);
                    field_0x2E4 = 1;
                    gabi::Local<cXyz> d;
                    cXyz_mi(&ac->current.pos, d, &current.pos);
                    field_0x2C8.copy(*d.get());
                    cXyz_mi(&current.pos, d, &ac->current.pos);
                    gabi::Local<cXyz> temp4;
                    gabi::Local<cXyz> temp5;
                    temp4->set(cs, 0.0f, -sn);
                    temp5->set(sn, 0.0f, cs);
                    field_0x2D4.copy(*d.get());
                    f32 dot1 = PSVECDotProduct(temp4, &field_0x2D4);
                    f32 dot2 = PSVECDotProduct(temp5, &field_0x2D4);
                    if (dot1 > 175.0f) {
                        field_0x2D4.x = gabi::fnmsubs(175.0f, cs, field_0x2D4.x);
                        field_0x2D4.z = gabi::fnmsubs(-175.0f, sn, field_0x2D4.z);
                    } else if (dot1 < -175.0f) {
                        field_0x2D4.z = gabi::fmadds(-175.0f, sn, field_0x2D4.z);
                        field_0x2D4.x = gabi::fmadds(175.0f, cs, field_0x2D4.x);
                    } else {
                        field_0x2D4.x = gabi::fnmsubs(dot1, cs, field_0x2D4.x);
                        field_0x2D4.z = gabi::fmadds(dot1, sn, field_0x2D4.z);
                    }

                    if (dot2 > 40.0f) {
                        field_0x2D4.x = gabi::fnmsubs(40.0f, sn, field_0x2D4.x);
                        field_0x2D4.z = gabi::fnmsubs(40.0f, cs, field_0x2D4.z);
                    } else if (dot2 < -40.0f) {
                        field_0x2D4.x = gabi::fmadds(40.0f, sn, field_0x2D4.x);
                        field_0x2D4.z = gabi::fmadds(40.0f, cs, field_0x2D4.z);
                    } else {
                        field_0x2D4.x = gabi::fnmsubs(dot2, sn, field_0x2D4.x);
                        field_0x2D4.z = gabi::fnmsubs(dot2, cs, field_0x2D4.z);
                    }
                    cXyz_normZP(&field_0x2D4, d);
                    field_0x2D4.copy(*d.get());
                    f32 a2 = cXyz_abs2(&field_0x2C8);
                    s16 sub = (s16)gabi::ftoi(0.0002f * a2);
                    field_0x2E0 = l_HIO().m18 - sub;
                    if (cXyz_abs(&field_0x2C8) < 80.0f) {
                        field_0x2EC = 0.7f;
                    } else {
                        field_0x2EC = 56.0f / cXyz_abs(&field_0x2C8);
                    }
                } else {
                    field_0x2E4 = 1;
                    gabi::Local<cXyz> d;
                    cXyz_mi(mCps.GetTgHitPosP(), d, &current.pos);
                    field_0x2C8.copy(*d.get());
                    field_0x2D4.copy(mCps.mGObjTg.mRVec); /* *mCps.GetTgRVecP() */
                    cXyz_normZP(&field_0x2D4, d);
                    field_0x2D4.copy(*d.get());

                    field_0x2E0 = l_HIO().m18 - 2;
                    field_0x2EC = 1.0f;
                }
            }
        }
    }
    s32 temp5 = field_0x2E0;
    s32 m18 = l_HIO().m18;
    if (temp5 > m18 - 10 && temp5 < m18) {
        f32 sinResult = cM_ssin((((temp5 - m18) + 10) & 0x3) << 14);
        field_0x2E8 = 1.5f * sinResult;
    }
    return MoveBGExecute() != 0;
}
VERIFY(0x02390AC4, &daObj_Stair_c::_execute);

/* 02391214: daObj_StairDraw (_draw inlined: MoveBGDraw, the virtual Draw) */
static BOOL daObj_StairDraw(void* i_this) {
    WWHD_FUNC(0x02391214, BOOL, i_this);
    return ((daObj_Stair_c*)i_this)->Draw_v() != FALSE;
}
VERIFY(0x02391214, daObj_StairDraw);

/* 02391688 */
static BOOL daObj_StairIsDelete(void* i_this) {
    WWHD_FUNC(0x02391688, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02391688, daObj_StairIsDelete);

/* 02391570: daobj_stairHIO_c::daobj_stairHIO_c (allocates when this == NULL) */
static daobj_stairHIO_c* daobj_stairHIO_c_ct(daobj_stairHIO_c* h) {
    WWHD_FUNC(0x02391570, daobj_stairHIO_c*, h);
    if (h == nullptr) {
        h = (daobj_stairHIO_c*)operator_new(0x1C);
        if (h == nullptr)
            return h;
    }
    h->mNo = -1;
    h->m10 = 5.0f;
    h->__vtbl = 0x1002F6E4;
    h->m14 = 2.5f;
    h->m08 = 2.5f;
    h->m0C = -175.0f; /* HD: GameCube -70 */
    h->m18 = 6;
    return h;
}
VERIFY(0x02391570, daobj_stairHIO_c_ct);

/* 023915E8 */
static void __sinit_d_a_obj_stair_cpp() {
    WWHD_FUNC(0x023915E8, void, (u32)0);
    sinit_header_statics(0x1046BEF8, 0x101CCD18);
    daobj_stairHIO_c_ct(&l_HIO());
}
VERIFY(0x023915E8, __sinit_d_a_obj_stair_cpp);

/* 02391690: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02391690, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02391690, trivial_dt);

/* 023916A4: dBgS_MoveBgActor::IsDelete (per-TU copy; the matcher files it under d_a_fan) */
static BOOL MoveBgActor_IsDelete(void* i_this) {
    WWHD_FUNC(0x023916A4, BOOL, i_this);
    return TRUE;
}
VERIFY(0x023916A4, MoveBgActor_IsDelete);

/* 023916AC: daObj_Stair_c::Delete */
static BOOL daObj_Stair_c_Delete(daObj_Stair_c* i_this) {
    WWHD_FUNC(0x023916AC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x023916AC, daObj_Stair_c_Delete);

/* 023916B4: daObj_Stair_c deleting destructor (inline member destructors) */
static void daObj_Stair_c_dt(daObj_Stair_c* i_this, s32 flags) {
    WWHD_FUNC(0x023916B4, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515980, &i_this->mCps, 2); /* dCcD_Cps::~dCcD_Cps (matcher: dCcD_GObjInf::~dCcD_GObjInf) */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0);        /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023916B4, daObj_Stair_c_dt);

/* 02391720: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02391720, void, p);
}
VERIFY(0x02391720, empty_virtual);
