/**
 * d_a_tama.cpp (WWHD)
 * Projectile thrown by an NPC partner (deleted on wall/ground/foreign collision or out of range).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_tama.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_tama.h"

// HD: constructor/destructor vtables and common-header statics are per translation unit.
const gc_tu_data gc_tu = {
    .objAcch = {0x1003FD64, 0x1003FD84, 0x1003FD74},
    .objAcchDt = {0x1003FD74, 0x1003FD84},
    .headerStatics = {0x1046E6B4, 0x101D22CC, 0x1046E6C0},
};
static constexpr u32 TAMA_VTBL = 0x1003FD94;
static dCcD_SrcSph l_sph_src;
enum { fpcNm_PLAYER_e = 0xA8 };

/* 024B3DE0 */
BOOL daTama_c::createInit() {
    mAcchCir.SetWall(0.0f, 35.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mStts.Init(0, 0xFF, this);
    mSph.SetStts(&mStts);
    mSph.Set(&l_sph_src);
    return TRUE;
}

/* 024B3F7C */
BOOL daTama_c::_execute() {
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
                dCcD_GObjInf* hitobj = tamaGetCoHitObj(&mSph);
                del = true;
                if (hitobj != NULL) {
                    dCcD_Stts* stts = hitobj->mStts;
                    fopAc_ac_c* hitac = stts ? stts->GetActor() : nullptr;
                    if (hitac != NULL) {
                        if (hitac != partner) {
                            if (fpcM_GetName(hitac) == fpcNm_PLAYER_e) {
                                ((TamaPartner*)partner)->field_0x6ba = 1;
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

/* 024B3E80 */
cPhs_State daTama_c::_create() {
    cPhs_State rt = cPhs_COMPLEATE_e;
    /* fopAcM_ct(this, daTama_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = TAMA_VTBL;
            __ct(&mAcch);
            __ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            tamaSphere_ct(&mSph);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!createInit())
        rt = cPhs_ERROR_e;
    return rt;
}

/* 024B3F6C */
static cPhs_State daTama_Create(fopAc_ac_c* i_this) { return ((daTama_c*)i_this)->_create(); }

/* 024B3F70 */
BOOL daTama_c::_delete() { return TRUE; }

/* 024B3F78 */
static BOOL daTama_Delete(daTama_c* i_this) { return i_this->_delete(); }

/* 024B41D0 */
static BOOL daTama_Execute(daTama_c* i_this) { return i_this->_execute(); }

/* 024B41D4 */
static BOOL daTama_IsDelete(daTama_c* i_this) { return TRUE; }

// HD: _draw was inlined into this wrapper.
/* 024B41DC */
static BOOL daTama_Draw(daTama_c* i_this) { return TRUE; }

// HD: compiler-generated header initialization and deleting destructor.
/* 024B41E4 */
static void __sinit_d_a_tama_cpp() { __sinit_header_statics(); }

/* 024B4278 */
static void daTama_c_dt(daTama_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        tamaSphere_dt(&i_this->mSph, 2);
        __dt(&i_this->mStts);
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mAcch);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

// clang-format off
/* WWHD glue: collision source and the remaining sphere bindings.
data l_sph_src 101D226C
ext 025163BC dCcD_GObjInf* tamaGetCoHitObj(dCcD_GObjInf*)
ext 025166F0 void tamaSphere_ct(dCcD_Sph*)
ext 02515AE8 void tamaSphere_dt(dCcD_Sph*, u32)
*/
