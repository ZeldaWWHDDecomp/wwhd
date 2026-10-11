/**
 * d_a_switem.cpp (WWHD)
 * Hit trigger that drops an item from a table when struck by a given attack type.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_switem.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_switem.h"
const gc_tu_data gc_tu = { .headerStatics={0x1046E180, 0x101D1458, 0x1046E18C} };
static dCcD_SrcCyl l_cyl_src;

/* 024A2534 */
BOOL daSwItem_c::CreateInit() {
    int itemBitNo = daSwItem_prm::getItemBitNo(this);
    mAtTypeTrigger = daSwItem_prm::getAtType(this);
    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    mCyl.SetR(scale.x * 25.0f);
    mCyl.SetH(scale.y * 50.0f);
    if (fopAcM_isItem(this, itemBitNo) && itemBitNo != 0x7F) {
        return FALSE;
    }
    fopAcM_offDraw(this);
    return TRUE;
}

/* 024A2620 */
cPhs_State daSwItem_c::_create() {
    fopAcM_ct(this, daSwItem_c);
    if (!CreateInit()) {
        return cPhs_ERROR_e;
    }
    return cPhs_COMPLEATE_e;
}

/* 024A27B4 */
bool daSwItem_c::_execute() {
    bool triggered = false;
    if (mCyl.ChkTgHit()) {
        dCcD_GObjInf* obj = (dCcD_GObjInf*)mCyl.GetTgHitObj();
        if (obj) {
            switch (mAtTypeTrigger) {
            case 0xFF:
                triggered = true;
                break;
            case 0:
                if (obj->ChkAtType(AT_TYPE_SWORD) || obj->ChkAtType(AT_TYPE_BOKO_STICK) ||
                    obj->ChkAtType(AT_TYPE_STALFOS_MACE) || obj->ChkAtType(AT_TYPE_MACHETE) ||
                    obj->ChkAtType(AT_TYPE_DARKNUT_SWORD))
                {
                    triggered = true;
                }
                break;
            case 1:
                if (obj->ChkAtType(AT_TYPE_BOMB)) triggered = true;
                break;
            case 2:
                if (obj->ChkAtType(AT_TYPE_BOOMERANG)) triggered = true;
                break;
            case 3:
                if (obj->ChkAtType(AT_TYPE_SKULL_HAMMER)) triggered = true;
                break;
            case 4:
                if (obj->ChkAtType(AT_TYPE_FIRE_ARROW) || obj->ChkAtType(AT_TYPE_ICE_ARROW) ||
                    obj->ChkAtType(AT_TYPE_LIGHT_ARROW) || obj->ChkAtType(AT_TYPE_NORMAL_ARROW))
                {
                    triggered = true;
                }
                break;
            case 5:
                if (obj->ChkAtType(0x8000)) triggered = true;
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
        daItem_c* item = (daItem_c*)fopAcM_SearchByID(itemProcId);
        if (item) {
            if (isRupee(item->m_itemNo) || isRupeeInAllCreateTable(itemTbl)) {
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

/* 024A271C */
BOOL daSwItem_c::isRupeeInAllCreateTable(int tbl) {
    ItemTableList* itemTableList = gabi::at<ItemTableList>(dComIfGp_getItemTable());
    if (tbl >= 0x20 && tbl <= 0x3E && itemTableList) {
        be<u8>* itemNo = itemTableList->mItemTables[tbl - 0x20];
        for (int i = 0; i < 0x10; i++) {
            if (isRupee(*itemNo)) {
                return true;
            }
            itemNo++;
        }
    }
    return false;
}

/* 024A2708 */
static cPhs_State daSwItem_Create(void* i_this) {
    return static_cast<daSwItem_c*>(i_this)->_create();
}

/* 024A270C */
// HD: _delete inlined
static BOOL daSwItem_Delete(void* i_this) {
    return true;
}

/* 024A2714 */
// HD: _draw inlined
static BOOL daSwItem_Draw(void* i_this) {
    return true;
}

/* 024A2A20 */
static BOOL daSwItem_Execute(void* i_this) {
    return static_cast<daSwItem_c*>(i_this)->_execute();
}

/* 024A2AB8 */
static BOOL daSwItem_IsDelete(void* i_this) {
    return TRUE;
}

/* 024A2A24 */
static void __sinit_d_a_switem_cpp() {
    __sinit_header_statics();
}

/* 024A2AC0 */
// HD: daSwItem_c deleting destructor (compiler-generated)
static void daSwItem_c_dt(daSwItem_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data
data l_cyl_src 101D1414
*/
