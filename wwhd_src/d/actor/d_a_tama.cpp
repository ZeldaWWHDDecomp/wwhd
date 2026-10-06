/**
 * d_a_tama.cpp (WWHD)
 * Projectile thrown by an NPC partner (deleted on wall/ground/foreign collision or out of range).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tama.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define TAMA_VTBL 0x1003FD94 /* daTama_c vtable (HD virtual destructor) */
static const dBgS_ObjAcch_vt l_objacch_vt = {0x1003FD64, 0x1003FD84, 0x1003FD74}; /* this TU's copies */
#define l_sph_src gabi::at<dCcD_SrcSph>(0x101D226C)

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025163BC dCcD_GObjInf::GetCoHitObj -> cCcD_Obj* (its dCcD_Stts* at +0x44, the stts' actor at +0xC) */
static inline void* dCcD_GetCoHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x025163BC, o); }
/* cCcD_Obj::GetAc (inline): mStts ? mStts->mp_actor : NULL */
static inline fopAc_ac_c* cCcD_Obj_GetAc(void* obj) {
    u32 stts = gabi::load<u32>(gabi::ea(obj) + 0x44);
    if (stts == 0)
        return nullptr;
    return gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC));
}

enum { fpcNm_PLAYER_e = 0xA8 };

struct daTama_c : fopAc_ac_c {
    BOOL createInit();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();

    /* 0x3AC */ be<u32> mPartnerID;
    /* 0x3B0 */ dBgS_ObjAcch mAcch;
    /* 0x574 */ dBgS_AcchCir mAcchCir;
    /* 0x5B4 */ dCcD_Stts mStts;
    /* 0x5F0 */ dCcD_Sph mSph;
    /* 0x71C */ be<f32> mDis;
};
WWHD_OFFSET(daTama_c, mAcchCir, 0x574);
WWHD_OFFSET(daTama_c, mSph, 0x5F0);
WWHD_OFFSET(daTama_c, mDis, 0x71C);

/* 024B3DE0 */
BOOL daTama_c::createInit() {
    WWHD_FUNC(0x024B3DE0, BOOL, this);
    mAcchCir.SetWall(0.0f, 35.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mStts.Init(0, 0xFF, this);
    mSph.SetStts(&mStts);
    mSph.Set(l_sph_src);
    return TRUE;
}
VERIFY(0x024B3DE0, &daTama_c::createInit);

/* 024B3F7C */
BOOL daTama_c::_execute() {
    WWHD_FUNC(0x024B3F7C, BOOL, this);
    fopAc_ac_c* partner = fopAcM_SearchByID(mPartnerID);
    bool del = true;
    if (partner != NULL) {
        f32 distXZ = fopAcM_searchActorDistanceXZ(this, partner);
        f32 distY = std::fabs(current.pos.y - partner->current.pos.y);
        if (distXZ < mDis && distY < 400.0f) {
            del = false;
            speed.y = speedF * cM_ssin(current.angle.x);
            fopAcM_posMoveF(this, &mStts.m_cc_move);
            mAcch.CrrPos(dComIfG_Bgsp());
            if (mAcch.ChkWallHit()) {
                del = true;
            } else if (mAcch.ChkGroundHit()) {
                del = true;
            }

            if (!del && mSph.ChkCoHit()) {
                void* hitobj = dCcD_GetCoHitObj(&mSph);
                del = true;
                if (hitobj != NULL) {
                    fopAc_ac_c* hitac = cCcD_Obj_GetAc(hitobj);
                    if (hitac != NULL) {
                        if (hitac != partner) {
                            if (fpcM_GetName(hitac) == fpcNm_PLAYER_e) {
                                gabi::store<s16>(gabi::ea(partner) + 0x7D6, 1); /* fopNpc_npc_c::field_0x6ba */
                            }
                        } else {
                            del = false;
                        }
                    }
                }
            }

            mSph.SetC(&current.pos);
            mSph.SetR(20.0f);
            dComIfG_Ccsp_Set(&mSph);
        }
    }
    if (del) {
        fopAcM_delete(this);
    }
    return TRUE;
}
VERIFY(0x024B3F7C, &daTama_c::_execute);

/* 024B3E80 */
cPhs_State daTama_c::_create() {
    WWHD_FUNC(0x024B3E80, cPhs_State, this);
    cPhs_State rt = cPhs_COMPLEATE_e;
    /* fopAcM_ct(this, daTama_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = TAMA_VTBL;
            dBgS_ObjAcch_ct(&mAcch, l_objacch_vt);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            gabi::call(0x025166F0, &mSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!createInit())
        rt = cPhs_ERROR_e;
    return rt;
}
VERIFY(0x024B3E80, &daTama_c::_create);

/* 024B3F6C */
static cPhs_State daTama_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024B3F6C, cPhs_State, i_this);
    return ((daTama_c*)i_this)->_create();
}
VERIFY(0x024B3F6C, daTama_Create);

/* 024B3F70 */
BOOL daTama_c::_delete() {
    WWHD_FUNC(0x024B3F70, BOOL, this);
    return TRUE;
}
VERIFY(0x024B3F70, &daTama_c::_delete);

/* 024B3F78 */
static BOOL daTama_Delete(daTama_c* i_this) {
    WWHD_FUNC(0x024B3F78, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x024B3F78, daTama_Delete);

/* 024B41D0 */
static BOOL daTama_Execute(daTama_c* i_this) {
    WWHD_FUNC(0x024B41D0, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x024B41D0, daTama_Execute);

/* 024B41D4 */
static BOOL daTama_IsDelete(daTama_c* i_this) {
    WWHD_FUNC(0x024B41D4, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B41D4, daTama_IsDelete);

/* 024B41DC: _draw inlined */
static BOOL daTama_Draw(daTama_c* i_this) {
    WWHD_FUNC(0x024B41DC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024B41DC, daTama_Draw);

/* 024B41E4: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_tama_cpp() {
    WWHD_FUNC(0x024B41E4, void, (u32)0);
    sinit_header_statics(0x1046E6B4, 0x101D22CC);
}
VERIFY(0x024B41E4, __sinit_d_a_tama_cpp);

/* 024B4278: daTama_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daTama_c_dt(daTama_c* i_this, s32 flags) {
    WWHD_FUNC(0x024B4278, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph, 2);  /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mAcchCir) + 0x14), 2); /* dBgS_AcchCir dtor (inline): cM3dGCir */
        /* dBgS_ObjAcch::~dBgS_ObjAcch (inline): this TU's vtables, then dBgS_Acch::~dBgS_Acch */
        u32 b = gabi::ea(&i_this->mAcch);
        gabi::store<u32>(b + 0x20, l_objacch_vt.v20);
        gabi::store<u32>(b + 0x14, l_objacch_vt.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024B4278, daTama_c_dt);
