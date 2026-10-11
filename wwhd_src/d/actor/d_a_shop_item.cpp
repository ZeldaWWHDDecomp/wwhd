/**
 * d_a_shop_item.cpp (WWHD)
 * Item - Shop Item (+ the four functions of d_a_shop_item_static that follow it in WWHD)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shop_item.cpp, src/d/d_a_shop_item_static.cpp) to the WWHD
 * layout and code, verified against cking.rpx. "HD:" marks where WWHD's code differs.
 */
#include "d/actor/d_a_shop_item_readable.h"

// HD: the cloth arrays retain the original guest frame, including saved registers.
static ShopText clothArcname;

/* 02483450 */
const char *daShopItem_c::getShopArcname() {
    u8 type = (fopAcM_GetParam(this) >> 8) & 0xF;
    if (type == 1 || (type == 0 && daShopItem_data::mModelType(m_itemNo) == 0x01)) {
        return gabi::at<ShopFieldResource>(dItem_data::field_item_res(m_itemNo))
            ->mArcname; /* getFieldArc */
    } else {
        return gabi::at<ShopItemResource>(dItem_data::item_resource(m_itemNo))
            ->mArcname; /* getArcname */
    }
}

/* 024834AC */
s16 daShopItem_c::getShopBmdIdx() {
    u8 type = (fopAcM_GetParam(this) >> 8) & 0xF;
    if (type == 1 || (type == 0 && daShopItem_data::mModelType(m_itemNo) == 0x01)) {
        return gabi::at<ShopFieldResource>(dItem_data::field_item_res(m_itemNo))
            ->mBmdIdx; /* getFieldBmdIdx */
    } else {
        return gabi::at<ShopItemResource>(dItem_data::item_resource(m_itemNo))
            ->mBmdIdx; /* getBmdIdx */
    }
}

/* 024836F4 */
void daShopItem_c::CreateInit() {
    cullMtx = gabi::ea(&field_0x64C); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -100.0f, 0.0f, -100.0f, 100.0f, 200.0f, 100.0f);
    if (mDoLib_clipper_getFar() > 1.0f) {
        cullSizeFar = 5000.0f / mDoLib_clipper_getFar();
    }
    show();

    scale.copy(gabi::at<daShopItem_c_m_data>(daShopItem_data::mData(m_itemNo))
                   ->mScale); /* getData()[m_itemNo].mScale */
    home.pos.copy(current.pos);
    set_mtx();

    s32 tevType = TEV_TYPE_BG1_PLIGHT;
    if (isDaizaItem(m_itemNo)) {
        tevType = TEV_TYPE_ACTOR;
    }
    J3DModel *model = mpModel;
    mTevType = tevType;
    model->mUserArea = 0; /* mpModel->setUserArea(NULL) */
}

/* 02483C10 */
BOOL daShopItem_c::clothCreate() {
    auto frame = OrigFrame(0x50, 27, 0x3C);
    be<u32> *clothFunc =
        gabi::at<be<u32>>(frame.sp + 0x18); /* dCloth_packet_c::CreateFunc clothFunc[4] */
    be<u32> *clothRes = gabi::at<be<u32>>(frame.sp + 0x28); /* u32 clothRes[4] */

    if (isUseClothPacket(m_itemNo)) {
        clothFunc[0] = shopAbi::ClothVobj03; /* dClothVobj03_create */
        clothFunc[1] = shopAbi::ClothVobj04; /* dClothVobj04_create */
        clothFunc[2] = shopAbi::ClothVobj05; /* dClothVobj05_create */
        clothFunc[3] = shopAbi::ClothVobj07; /* dClothVobj07_0_create */
        clothRes[0] = shopAbi::FTEX03;       /* dRes_INDEX_FDAI_BTI_FTEX03_e */
        clothRes[1] = shopAbi::FTEX04;
        clothRes[2] = shopAbi::FTEX05;
        clothRes[3] = shopAbi::FTEX07;

        u8 idx;
        switch (m_itemNo) {
        case dItemNo_HEROS_FLAG_e:
            idx = 0;
            break;
        case dItemNo_BIG_CATCH_FLAG_e:
            idx = 1;
            break;
        case dItemNo_BIG_SALE_FLAG_e:
            idx = 2;
            break;
        default:
            idx = 3;
        }
        field_0x648 = idx;

        const char *arcName = getShopArcname();
        u32 res = clothRes[idx];
        void *shopArc = dComIfG_getObjectRes(arcName, res, SAFESTRING_VTBL);
        void *clothArc =
            dComIfG_getObjectRes(clothArcname.bytes, shopAbi::CLOTHTOON, SAFESTRING_VTBL);

        u32 fn = clothFunc[field_0x648];
        dCloth_packet_c *cloth =
            gabi::call_ptr<dCloth_packet_c *>(fn, shopArc, clothArc, &tevStr, 0);
        field_0x644 = cloth;
        if (cloth == nullptr) {
            return FALSE;
        }
    } else {
        field_0x644 = nullptr;
    }

    return TRUE;
}

/* 02483508 */
void daShopItem_c::set_mtx() {
    auto frame = OrigFrame(0x48, 29, 0x3C);

    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &field_0x64C);

    auto *data = gabi::at<daShopItem_c_m_data>(
        daShopItem_data::mData(m_itemNo)); /* getData()[m_itemNo].field_0x0C */
    mDoMtx_stack_c::transM(data->field_0x0C.x, data->field_0x0C.y, data->field_0x0C.z);
    data = gabi::at<daShopItem_c_m_data>(daShopItem_data::mData(m_itemNo)); /* .field_0x18 */
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), data->field_0x18.x, data->field_0x18.y,
                   data->field_0x18.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());

    if (field_0x644 != nullptr) {
        /* cXyz local[4] at sp+8 (GameCube: two arrays; HD keeps only the one it reads) */
        cXyz *local = gabi::at<cXyz>(frame.sp + 8);
        u8 idx = field_0x648;
        f32 y94 = REG_F(10, 15) + 94.0f;
        f32 y97 = REG_F(10, 15) + 97.5f;
        const f32 ys[4] = {y94, y94, y97, y94};
        for (int i = 0; i < 4; i++) {
            local[i].x = 0.0f;
            local[i].y = ys[i];
            local[i].z = 0.0f;
        }
        const cXyz &clothOffset = local[idx];
        mDoMtx_stack_c::transM(clothOffset.x, clothOffset.y, clothOffset.z);
        mDoMtx_stack_c::YrotM(0x4000);
        dCloth_packet_setScale(field_0x644, &scale);
        dCloth_packet_setMtx(field_0x644, mDoMtx_stack_c::get());
    }
}

/* 02483BBC */
bool daShopItem_c::_execute() {
    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    set_mtx();
    return true;
}

/* 02483B28 */
bool daShopItem_c::_draw() {
    if (!chkDraw())
        return true;

    if (m_itemNo == dItemNo_FOUNTAIN_IDOL_e || m_itemNo == dItemNo_POSTMAN_STATUE_e) {
        /* mpModel->getModelData()->getJointNodePointer(0)->setMtxCalc(0) (HD: model data +0xAC,
         * root joint at +8, its calc pointer at +0x14) */
        J3DModelData *modelData = mpModel->mModelData;
        auto *joint = gabi::at<ShopJointView>(modelData->mJointNodes);
        joint->mMtxCalc = 0;
    }
    gabi::call_ptr(vfn(daItemBase_VT_DRAWBASE), this); /* DrawBase() (virtual) */

    if (field_0x644 != nullptr)
        dCloth_packet_cloth_draw(field_0x644);

    return true;
}

/* 02483E24 */
void daShopItem_c::settingBeforeDraw() {
    if (isBomb(m_itemNo) || (m_itemNo == dItemNo_BOMB_BAG_e) ||
        (m_itemNo == dItemNo_SKULL_HAMMER_e) || m_itemNo == dItemNo_SMALL_KEY_e ||
        m_itemNo == dItemNo_SHOP_GURU_STATUE_e) {
        dDlst_texSpecmapST(&eyePos, &tevStr, mpModel, 1.0f);
    }
}

/* 02483D8C */
void daShopItem_c::setTevStr() {
    dScnKy_env_light_c *env = dKy_getEnvlight();
    settingTevStruct(env, mTevType, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    for (int i = 0; i < 2; i++) {
        if (mpModelArrow[i] != nullptr) {
            env = dKy_getEnvlight();
            setLightTevColorType(env, mpModelArrow[i], &tevStr);
        }
    }
}

/*: daShopItem_c::_create() inlined. HD: an item number of 0xFF becomes a green rupee
 * before the model check; the arc name of the model check is kept for the resource load */
/* 024837F8 */
static cPhs_State daShopItem_Create(void *i_this) {
    daShopItem_c *a = static_cast<daShopItem_c *>(i_this);

    /* fopAcM_ct(this, daShopItem_c) */
    if (!fopAcM_CheckCondition(a, fopAcCnd_INIT_e)) {
        if (a != nullptr) {
            fopAc_ac_c_ct(a);
            a->__vtbl = DAITEMBASE_VTBL;
            dBgS_ObjAcch_ct(&a->mAcch,
                            {shopAbi::AcchVtable, shopAbi::AcchVtable2, shopAbi::AcchVtable3});
            dBgS_AcchCir_ct(&a->mAcchCir);
            dCcD_Stts_ct(&a->mStts);
            dCcD_Cyl_ct(&a->mCyl, shopAbi::CylVtable);
            a->__vtbl = DASHOPITEM_VTBL;
        }
        fopAcM_OnCondition(a, fopAcCnd_INIT_e);
    }

    u8 itemNo = (u8)fopAcM_GetParam(a); /* fopAcM_GetParamBit(param, 0, 8) */
    if (itemNo < 0xFF) {
        a->m_itemNo = itemNo;
    } else {
        a->m_itemNo = dItemNo_GREEN_RUPEE_e;
    }

    const char *arcName = a->getShopArcname();
    if (a->getShopBmdIdx() == -1 || arcName == nullptr) {
        a->m_itemNo = dItemNo_GREEN_RUPEE_e;
        arcName = a->getShopArcname();
    }

    cPhs_State result = dComIfG_resLoad(&a->mPhs, arcName);
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }
    if (isUseClothPacket(a->m_itemNo)) {
        cPhs_State result2 = dComIfG_resLoad(&a->mPhase, clothArcname.bytes);
        if (result2 != cPhs_COMPLEATE_e) {
            return result2;
        }
    }

    u8 type = (fopAcM_GetParam(a) >> 8) & 0xF;
    u8 no = a->m_itemNo;
    if (type == 2 || (type == 0 && daShopItem_data::mModelType(no) == 0x02)) {
        if (fopAcM_entrySolidHeap(a, shopAbi::CheckItemCreateHeap /* CheckItemCreateHeap */,
                                  dItem_data_getHeapSize(no)) == 0) {
            return cPhs_ERROR_e;
        }
    } else if (type == 1 || (type == 0 && daShopItem_data::mModelType(no) == 0x01)) {
        if (fopAcM_entrySolidHeap(a,
                                  shopAbi::CheckFieldItemCreateHeap /* CheckFieldItemCreateHeap */,
                                  dItem_data_getFieldHeapSize(no)) == 0) {
            return cPhs_ERROR_e;
        }
    } else {
        if (fopAcM_entrySolidHeap(a, shopAbi::CheckItemCreateHeap /* CheckItemCreateHeap */,
                                  dItem_data_getHeapSize(no)) == 0) {
            return cPhs_ERROR_e;
        }
    }

    a->CreateInit();
    return result;
}

/* 02483AC8 */
static BOOL daShopItem_Delete(void *i_this) {
    daShopItem_c *inst = static_cast<daShopItem_c *>(i_this);

    if (isUseClothPacket(inst->m_itemNo)) {
        dComIfG_resDelete(&inst->mPhase, clothArcname.bytes);
    }
    inst->DeleteBase(inst->getShopArcname());

    return TRUE;
}

/* 02483BB8 */
static BOOL daShopItem_Draw(void *i_this) {
    return shopDrawTail(i_this); /* static_cast<daShopItem_c*>(i_this)->_draw() */
}

/* 02483C0C */
static BOOL daShopItem_Execute(void *i_this) {
    return shopExecuteTail(i_this); /* static_cast<daShopItem_c*>(i_this)->_execute() */
}

/* 02483F2C */
static BOOL daShopItem_IsDelete(void *) { return TRUE; }

/* 02483E98 */
static void shop_item_sinit() {
    sinit_header_statics(shopAbi::HeaderStatics, shopAbi::HeaderStaticsData);
}

/* 02483F34 */
static void shop_item_SafeString_dtor(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02483F48 */
static void shop_item_setListStart(daShopItem_c *) {}

/* 02483F4C */
static void shop_item_dtor(daShopItem_c *p, s32 flags) {
    if (p != nullptr) {
        shopDestroyItemBase(p);
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02483FA0 */
static void shop_item_SafeString_v14(void *) {}

/* ---- d_a_shop_item_static (02483FA4..024840D8) ---- */

/* 02483FA4 */
cXyz *daShopItem_c::getScaleP() { return &scale; }

/* 02483FAC */
csXyz *daShopItem_c::getRotateP() { return &current.angle; }

/* 02483FB4 */
cXyz *daShopItem_c::getPosP() { return &current.pos; }

/* daShopItem_c::getCenter(): the cXyz result is returned through a hidden pointer (r4);
 * HD: like a constructor, GHS allocates it when that pointer is NULL */
/* 02483FBC */
static void daShopItem_getCenter(daShopItem_c *self, cXyz *ret) {
    f32 height;
    if (dItemNo_TOWN_FLOWER_e <= self->m_itemNo && self->m_itemNo <= dItemNo_SHOP_GURU_STATUE_e) {
        height = 80.0f;
    } else {
        height = 40.0f;
    }
    f32 half = height * 0.5f;
    if (ret == nullptr) {
        ret = (cXyz *)operator_new(sizeof(cXyz));
        if (ret == nullptr)
            return;
    }
    ret->y = half;
    ret->x = 0.0f;
    ret->z = 0.0f;
}

/* 02484048 */
static void shop_item_static_sinit() {
    sinit_header_statics(shopAbi::StaticHeaderStatics, shopAbi::StaticHeaderStaticsData);
}

/* WWHD glue (tools/verify/autoglue.py)
data clothArcname 1003A8CC nocheck
*/
