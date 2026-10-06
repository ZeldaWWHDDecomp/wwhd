/**
 * d_a_switem.cpp (WWHD)
 * Hit trigger that drops an item from a table when struck by a given attack type.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_switem.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101D1414)
#define SWITEM_VTBL 0x1003EBF4     /* daSwItem_c vtable (HD virtual destructor) */
#define AAB_VTBL 0x1003EBE4        /* this TU's copy of the cM3dGAab vtable */

enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_BOMB = 0x20, AT_TYPE_BOOMERANG = 0x40, AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_MACHETE = 0x400, AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_FIRE_ARROW = 0x4000,
    AT_TYPE_ICE_ARROW = 0x40000, AT_TYPE_LIGHT_ARROW = 0x80000, AT_TYPE_NORMAL_ARROW = 0x100000,
    AT_TYPE_STALFOS_MACE = 0x1000000, AT_TYPE_DARKNUT_SWORD = 0x4000000,
};
enum { JA_SE_OBJ_LUPY_OUT = 0x69E9, JA_SE_OBJ_ITEM_OUT = 0x69ED };
enum { daItemType_0_e = 0, daItemAct_1_e = 1 };

struct daSwItem_c : fopAc_ac_c {
    BOOL CreateInit();
    cPhs_State _create();
    bool _execute();
    BOOL isRupeeInAllCreateTable(int tbl);

    /* 0x3AC */ u8 m3AC[0x3B8 - 0x3AC];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ be<u8> mAtTypeTrigger;
    /* 0x525 */ be<u8> mSpawnedItem;
    /* 0x526 */ u8 m526[2];
};
WWHD_OFFSET(daSwItem_c, mCyl, 0x3F4);
WWHD_OFFSET(daSwItem_c, mAtTypeTrigger, 0x524);
WWHD_SIZE(daSwItem_c, 0x528);

namespace daSwItem_prm {
inline u32 getAtType(daSwItem_c* i_this) { return (fopAcM_GetParam(i_this) >> 0x00) & 0xFF; }
inline u32 getItemTbl(daSwItem_c* i_this) { return (fopAcM_GetParam(i_this) >> 0x08) & 0x3F; }
inline u32 getItemBitNo(daSwItem_c* i_this) { return (fopAcM_GetParam(i_this) >> 0x0E) & 0x7F; }
}  // namespace daSwItem_prm

/* 024A2534 */
BOOL daSwItem_c::CreateInit() {
    WWHD_FUNC(0x024A2534, BOOL, this);
    int itemBitNo = daSwItem_prm::getItemBitNo(this);
    mAtTypeTrigger = daSwItem_prm::getAtType(this);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(l_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl.SetR(scale.x * 25.0f);
    mCyl.SetH(scale.y * 50.0f);
    if (fopAcM_isItem(this, itemBitNo) && itemBitNo != 0x7F) {
        return FALSE;
    }
    fopAcM_offDraw(this);
    return TRUE;
}
VERIFY(0x024A2534, &daSwItem_c::CreateInit);

/* 024A2620 */
cPhs_State daSwItem_c::_create() {
    WWHD_FUNC(0x024A2620, cPhs_State, this);
    /* fopAcM_ct(this, daSwItem_c): HD vtable and inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SWITEM_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    if (!CreateInit()) {
        return cPhs_ERROR_e;
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x024A2620, &daSwItem_c::_create);

/* 024A27B4 */
bool daSwItem_c::_execute() {
    WWHD_FUNC(0x024A27B4, bool, this);
    bool triggered = false;
    if (mCyl.ChkTgHit()) {
        void* obj = mCyl.GetTgHitObj();
        if (obj) {
            switch (mAtTypeTrigger) {
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

    if (triggered && !mSpawnedItem) {
        int itemTbl = daSwItem_prm::getItemTbl(this);
        int itemBitNo = daSwItem_prm::getItemBitNo(this);
        if (fopAcM_isItem(this, itemBitNo)) {
            itemBitNo = 0x7F;
        }
        gabi::Local<csXyz> angle;
        csXyz_ct(angle, 0, home.angle.y, 0);
        fpc_ProcID itemProcId = fopAcM_createItemFromTable(&current.pos, itemTbl, itemBitNo, home.roomNo,
                                                           daItemType_0_e, angle, daItemAct_1_e);
        fopAc_ac_c* item = fopAcM_SearchByID(itemProcId);
        if (item) {
            if (isRupee(gabi::load<u8>(gabi::ea(item) + 0x74E) /* daItem_c::m_itemNo */) || isRupeeInAllCreateTable(itemTbl)) {
                fopAcM_seStart(this, JA_SE_OBJ_LUPY_OUT, 0);
            } else {
                fopAcM_seStart(this, JA_SE_OBJ_ITEM_OUT, 0);
            }
        }
        mSpawnedItem = true;
    }

    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    return true;
}
VERIFY(0x024A27B4, &daSwItem_c::_execute);

/* 024A271C */
BOOL daSwItem_c::isRupeeInAllCreateTable(int tbl) {
    WWHD_FUNC(0x024A271C, BOOL, this, tbl);
    u32 itemTableList = dComIfGp_getItemTable();
    if (tbl >= 0x20 && tbl <= 0x3E && itemTableList) {
        u32 itemNo = itemTableList + 0x10 + (tbl - 0x20) * 0x10; /* mItemTables[tbl - 0x20] */
        for (int i = 0; i < 0x10; i++) {
            if (isRupee(gabi::load<u8>(itemNo))) {
                return true;
            }
            itemNo++;
        }
    }
    return false;
}
VERIFY(0x024A271C, &daSwItem_c::isRupeeInAllCreateTable);

/* 024A2708 */
static cPhs_State daSwItem_Create(void* i_this) {
    WWHD_FUNC(0x024A2708, cPhs_State, i_this);
    return static_cast<daSwItem_c*>(i_this)->_create();
}
VERIFY(0x024A2708, daSwItem_Create);

/* 024A270C: _delete inlined */
static BOOL daSwItem_Delete(void* i_this) {
    WWHD_FUNC(0x024A270C, BOOL, i_this);
    return true;
}
VERIFY(0x024A270C, daSwItem_Delete);

/* 024A2714: _draw inlined */
static BOOL daSwItem_Draw(void* i_this) {
    WWHD_FUNC(0x024A2714, BOOL, i_this);
    return true;
}
VERIFY(0x024A2714, daSwItem_Draw);

/* 024A2A20 */
static BOOL daSwItem_Execute(void* i_this) {
    WWHD_FUNC(0x024A2A20, BOOL, i_this);
    return static_cast<daSwItem_c*>(i_this)->_execute();
}
VERIFY(0x024A2A20, daSwItem_Execute);

/* 024A2AB8 */
static BOOL daSwItem_IsDelete(void* i_this) {
    WWHD_FUNC(0x024A2AB8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x024A2AB8, daSwItem_IsDelete);

/* 024A2A24 */
static void __sinit_d_a_switem_cpp() {
    WWHD_FUNC(0x024A2A24, void, (u32)0);
    sinit_header_statics(0x1046E180, 0x101D1458);
}
VERIFY(0x024A2A24, __sinit_d_a_switem_cpp);

/* 024A2AC0: daSwItem_c deleting destructor (compiler-generated) */
static void daSwItem_c_dt(daSwItem_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A2AC0, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A2AC0, daSwItem_c_dt);
