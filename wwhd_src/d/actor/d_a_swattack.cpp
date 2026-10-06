/**
 * d_a_swattack.cpp (WWHD)
 * Hit trigger: sets a switch when struck by a given attack type.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swattack.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101D11C8)
#define SWAT_VTBL 0x1003EA94 /* daSwAt_c vtable (HD virtual destructor) */
#define AAB_VTBL 0x1003EA84  /* this TU's copy of the cM3dGAab vtable */

enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_BOMB = 0x20, AT_TYPE_BOOMERANG = 0x40, AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_MACHETE = 0x400, AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_FIRE_ARROW = 0x4000,
    AT_TYPE_ICE_ARROW = 0x40000, AT_TYPE_LIGHT_ARROW = 0x80000, AT_TYPE_NORMAL_ARROW = 0x100000,
    AT_TYPE_STALFOS_MACE = 0x1000000, AT_TYPE_DARKNUT_SWORD = 0x4000000,
};

struct daSwAt_c : fopAc_ac_c {
    cPhs_State _create();
    bool _execute();
    void CreateInit();

    /* 0x3AC */ u8 field_0x3AC[0xC];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ be<u8> mAtType;
    /* 0x525 */ u8 _525[3];
    /* 0x528 */ be<u32> mSwitchNo;
};
WWHD_OFFSET(daSwAt_c, mStts, 0x3B8);
WWHD_OFFSET(daSwAt_c, mCyl, 0x3F4);
WWHD_OFFSET(daSwAt_c, mAtType, 0x524);
WWHD_OFFSET(daSwAt_c, mSwitchNo, 0x528);
WWHD_SIZE(daSwAt_c, 0x52C);

namespace daSwAt_prm {
inline u8 getAtType(daSwAt_c* ac) { return (fopAcM_GetParam(ac) >> 0) & 0xFF; }
inline u8 getSwitchNo(daSwAt_c* ac) { return (fopAcM_GetParam(ac) >> 8) & 0xFF; }
}  // namespace daSwAt_prm

/* 024A0B30 */
void daSwAt_c::CreateInit() {
    WWHD_FUNC(0x024A0B30, void, this);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(l_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl.SetR(scale.x * 25.0f);
    mCyl.SetH(scale.y * 50.0f);
    mAtType = daSwAt_prm::getAtType(this);
    mSwitchNo = daSwAt_prm::getSwitchNo(this);
    fopAcM_offDraw(this);
}
VERIFY(0x024A0B30, &daSwAt_c::CreateInit);

/* 024A0BCC */
cPhs_State daSwAt_c::_create() {
    WWHD_FUNC(0x024A0BCC, cPhs_State, this);
    /* fopAcM_ct(this, daSwAt_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SWAT_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A0BCC, &daSwAt_c::_create);

/* 024A0CBC */
bool daSwAt_c::_execute() {
    WWHD_FUNC(0x024A0CBC, bool, this);
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
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_BOMB)) triggered = true;
                break;
            case 2:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_BOOMERANG)) triggered = true;
                break;
            case 3:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_SKULL_HAMMER)) triggered = true;
                break;
            case 4:
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_FIRE_ARROW) || cCcD_Obj_ChkAtType(obj, AT_TYPE_ICE_ARROW) ||
                    cCcD_Obj_ChkAtType(obj, AT_TYPE_LIGHT_ARROW) || cCcD_Obj_ChkAtType(obj, AT_TYPE_NORMAL_ARROW))
                {
                    triggered = true;
                }
                break;
            case 5:
                if (cCcD_Obj_ChkAtType(obj, 0x8000)) triggered = true;
                break;
            }
        }
    }

    if (triggered) {
        dComIfGs_onSwitch(mSwitchNo, home.roomNo); /* fopAcM_onSwitch */
    }

    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    return true;
}
VERIFY(0x024A0CBC, &daSwAt_c::_execute);

/* 024A0CA8 */
static cPhs_State daSwAt_Create(void* i_this) {
    WWHD_FUNC(0x024A0CA8, cPhs_State, i_this);
    return static_cast<daSwAt_c*>(i_this)->_create();
}
VERIFY(0x024A0CA8, daSwAt_Create);

/* 024A0CAC: _delete inlined */
static BOOL daSwAt_Delete(void* i_this) {
    WWHD_FUNC(0x024A0CAC, BOOL, i_this);
    return true;
}
VERIFY(0x024A0CAC, daSwAt_Delete);

/* 024A0CB4: _draw inlined */
static BOOL daSwAt_Draw(void* i_this) {
    WWHD_FUNC(0x024A0CB4, BOOL, i_this);
    return true;
}
VERIFY(0x024A0CB4, daSwAt_Draw);

/* 024A0E14 */
static BOOL daSwAt_Execute(void* i_this) {
    WWHD_FUNC(0x024A0E14, BOOL, i_this);
    return static_cast<daSwAt_c*>(i_this)->_execute();
}
VERIFY(0x024A0E14, daSwAt_Execute);

/* 024A0EAC */
static BOOL daSwAt_IsDelete(void* i_this) {
    WWHD_FUNC(0x024A0EAC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A0EAC, daSwAt_IsDelete);

/* 024A0E18 */
static void __sinit_d_a_swattack_cpp() {
    WWHD_FUNC(0x024A0E18, void, (u32)0);
    sinit_header_statics(0x1046E12C, 0x101D120C);
}
VERIFY(0x024A0E18, __sinit_d_a_swattack_cpp);

/* 024A0EB4: daSwAt_c deleting destructor (compiler-generated) */
static void daSwAt_c_dt(daSwAt_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A0EB4, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A0EB4, daSwAt_c_dt);
