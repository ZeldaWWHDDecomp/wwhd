/**
 * d_a_swattack.cpp (WWHD)
 * Hit trigger: sets a switch when struck by a given attack type.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swattack.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_swattack.h"

static dCcD_SrcCyl l_cyl_src;

const gc_tu_data gc_tu = { .headerStatics = {0x1046E12C, 0x101D120C, 0x1046E138} };

enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_BOMB = 0x20, AT_TYPE_BOOMERANG = 0x40, AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_MACHETE = 0x400, AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_FIRE_ARROW = 0x4000,
    AT_TYPE_ICE_ARROW = 0x40000, AT_TYPE_LIGHT_ARROW = 0x80000, AT_TYPE_NORMAL_ARROW = 0x100000,
    AT_TYPE_STALFOS_MACE = 0x1000000, AT_TYPE_DARKNUT_SWORD = 0x4000000,
};

/* 024A0B30 */
void daSwAt_c::CreateInit() {
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl.SetR(scale.x * 25.0f);
    mCyl.SetH(scale.y * 50.0f);
    mAtType = daSwAt_prm::getAtType(this);
    mSwitchNo = daSwAt_prm::getSwitchNo(this);
    fopAcM_offDraw(this);
}

/* 024A0BCC */
cPhs_State daSwAt_c::_create() {
    fopAcM_ct(this, daSwAt_c);
    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 024A0CBC */
bool daSwAt_c::_execute() {
    bool triggered = false;
    if (mCyl.ChkTgHit()) {
        void* obj = mCyl.GetTgHitObj();
        if (obj) {
            switch (mAtType) {
            case 0xFF:
                triggered = true;
                break;
            case 0:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_SWORD) || cCcD_Obj_ChkAtType(obj, AT_TYPE_BOKO_STICK) ||
                    cCcD_Obj_ChkAtType(obj, AT_TYPE_STALFOS_MACE) || cCcD_Obj_ChkAtType(obj, AT_TYPE_MACHETE) ||
                    cCcD_Obj_ChkAtType(obj, AT_TYPE_DARKNUT_SWORD))
                {
                    triggered = true;
                }
                break;
            case 1:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_BOMB)) {
                    triggered = true;
                }
                break;
            case 2:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_BOOMERANG)) {
                    triggered = true;
                }
                break;
            case 3:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_SKULL_HAMMER)) {
                    triggered = true;
                }
                break;
            case 4:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_FIRE_ARROW) || cCcD_Obj_ChkAtType(obj, AT_TYPE_ICE_ARROW) ||
                    cCcD_Obj_ChkAtType(obj, AT_TYPE_LIGHT_ARROW) || cCcD_Obj_ChkAtType(obj, AT_TYPE_NORMAL_ARROW))
                {
                    triggered = true;
                }
                break;
            case 5:
                if (cCcD_Obj_ChkAtType(obj, 0x8000)) {
                    triggered = true;
                }
                break;
            }
        }
    }

    if (triggered) {
        dComIfGs_onSwitch(mSwitchNo, home.roomNo);
    }

    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    return true;
}

/* 024A0CA8 */
static cPhs_State daSwAt_Create(void* i_this) {
    return static_cast<daSwAt_c*>(i_this)->_create();
}

/* 024A0CAC */
// HD: _delete is inlined.
static BOOL daSwAt_Delete(void* i_this) {
    return true;
}

/* 024A0CB4 */
// HD: _draw is inlined.
static BOOL daSwAt_Draw(void* i_this) {
    return true;
}

/* 024A0E14 */
static BOOL daSwAt_Execute(void* i_this) {
    return static_cast<daSwAt_c*>(i_this)->_execute();
}

/* 024A0EAC */
static BOOL daSwAt_IsDelete(void* i_this) {
    return TRUE;
}

// HD: compiler-generated code of this translation unit.
/* 024A0E18 */
static void __sinit_d_a_swattack_cpp() {
    __sinit_header_statics();
}

/* 024A0EB4 */
static void daSwAt_c_dt(daSwAt_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data l_cyl_src 101D11C8
*/
