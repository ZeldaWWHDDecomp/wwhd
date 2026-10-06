/**
 * d_a_shop_item.cpp (WWHD)
 * Item - Shop Item (+ the four functions of d_a_shop_item_static that follow it in WWHD)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_shop_item.cpp, src/d/d_a_shop_item_static.cpp) to the WWHD
 * layout and code, verified against cking.rpx. "HD:" marks where WWHD's code differs.
 */
#include "d/actor/d_a_shop_item.h"

#define SAFESTRING_VTBL 0x1003A844             /* this TU's copy of the sead::SafeString vtable */
#define m_cloth_arcname STR(0x1003A8CC)        /* "Cloth" */
#define DASHOPITEM_VTBL 0x1003A8D4
#define DAITEMBASE_VTBL 0x10012158

enum {
    dItemNo_GREEN_RUPEE_e = 0x01,
    dItemNo_SMALL_KEY_e = 0x15,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dItemNo_TOWN_FLOWER_e = 0x8C,
    dItemNo_HEROS_FLAG_e = 0x8F,
    dItemNo_BIG_CATCH_FLAG_e = 0x90,
    dItemNo_BIG_SALE_FLAG_e = 0x91,
    dItemNo_FOUNTAIN_IDOL_e = 0x95,
    dItemNo_POSTMAN_STATUE_e = 0x96,
    dItemNo_SHOP_GURU_STATUE_e = 0x97,
};

enum { TEV_TYPE_BG1_PLIGHT = 0x5C }; /* HD value */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025510A8 isUseClothPacket(u8), 02550FAC isDaizaItem(u8) (d_item) */
static inline BOOL isUseClothPacket(u8 itemNo) { return gabi::call<BOOL>(0x025510A8, itemNo); }
static inline BOOL isDaizaItem(u8 itemNo) { return gabi::call<BOOL>(0x02550FAC, itemNo); }
/* 0251B638 dCloth_packet_c::setMtx(Mtx); setScale inline: scale at +0xD8 (HD); cloth_draw is
 * virtual: vtable at +0xC, slot +0x44 */
static inline void dCloth_packet_setMtx(dCloth_packet_c* c, Mtx34* m) { gabi::call(0x0251B638, c, m); }
static inline void dCloth_packet_setScale(dCloth_packet_c* c, const cXyz* s) {
    f32 y = s->y, x = s->x, z = s->z;
    gabi::store<f32>(gabi::ea(c) + 0xD8, x);
    gabi::store<f32>(gabi::ea(c) + 0xDC, y);
    gabi::store<f32>(gabi::ea(c) + 0xE0, z);
}
static inline void dCloth_packet_cloth_draw(dCloth_packet_c* c) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(c) + 0xC) + 0x44), c);
}
/* 0252CA4C dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c*, J3DModel*, f32): HD passes the model
 * (copy of the d_a_itembase local binding) */
static inline void dDlst_texSpecmapST(cXyz* eye, dKy_tevstr_c* tev, J3DModel* model, f32 scale) {
    gabi::call(0x0252CA4C, eye, tev, model, scale);
}
/* mDoLib_clipper::getFar(): the float at 0x1048D04C */
static inline f32 mDoLib_clipper_getFar() { return gabi::load<f32>(0x1048D04C); }
/* d_item_data: the solid heap sizes (item_resource +0x20, field_item_res +0x18) */
static inline u16 dItem_data_getHeapSize(u32 no) { return gabi::load<u16>(dItem_data::item_resource(no) + 0x20); }
static inline u16 dItem_data_getFieldHeapSize(u32 no) { return gabi::load<u16>(dItem_data::field_item_res(no) + 0x18); }

/* HARNESS WORKAROUND (SHARED-CANDIDATE): the original's whole stack frame. set_mtx and
 * clothCreate index four-entry stack arrays with a u8 field they (re)load after calls; any index
 * is possible under clobbering, and the original then reads the rest of its frame (padding, the
 * saved GPRs) or the caller's frame. The candidate keeps its arrays at the original's offsets in
 * a frame of the same size, with the saved GPRs (entry values of rFIRST..r31) and the LR save
 * word (entry SP + 4) the original writes in its prologue. */
struct OrigFrame : GuestFrame {
    OrigFrame(u32 n, int firstGpr, u32 gprOff) : GuestFrame(n) {
        const u32 entry = sp + n;
        gabi::store<u32>(sp, entry);
        gabi::store<u32>(entry + 4, gabi::cpu->lr);
        for (int r = firstGpr; r < 32; r++) gabi::store<u32>(sp + gprOff + 4 * (r - firstGpr), gabi::cpu->r[r]);
    }
};

/* 02483450 */
const char* daShopItem_c::getShopArcname() {
    WWHD_FUNC(0x02483450, const char*, this);
    u8 type = (fopAcM_GetParam(this) >> 8) & 0xF;
    if (type == 1 || (type == 0 && daShopItem_data::mModelType(m_itemNo) == 0x01)) {
        return gabi::at<const char>(gabi::load<u32>(dItem_data::field_item_res(m_itemNo))); /* getFieldArc */
    } else {
        return gabi::at<const char>(gabi::load<u32>(dItem_data::item_resource(m_itemNo))); /* getArcname */
    }
}
VERIFY(0x02483450, &daShopItem_c::getShopArcname);

/* 024834AC (not named by the matcher) */
s16 daShopItem_c::getShopBmdIdx() {
    WWHD_FUNC(0x024834AC, s16, this);
    u8 type = (fopAcM_GetParam(this) >> 8) & 0xF;
    if (type == 1 || (type == 0 && daShopItem_data::mModelType(m_itemNo) == 0x01)) {
        return gabi::load<s16>(dItem_data::field_item_res(m_itemNo) + 4); /* getFieldBmdIdx */
    } else {
        return gabi::load<s16>(dItem_data::item_resource(m_itemNo) + 8); /* getBmdIdx */
    }
}
VERIFY(0x024834AC, &daShopItem_c::getShopBmdIdx);

/* 024836F4 */
void daShopItem_c::CreateInit() {
    WWHD_FUNC(0x024836F4, void, this);
    cullMtx = gabi::ea(&field_0x64C); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -100.0f, 0.0f, -100.0f, 100.0f, 200.0f, 100.0f);
    if (mDoLib_clipper_getFar() > 1.0f) {
        cullSizeFar = 5000.0f / mDoLib_clipper_getFar();
    }
    show();

    scale.copy(*gabi::at<cXyz>(daShopItem_data::mData(m_itemNo))); /* getData()[m_itemNo].mScale */
    home.pos.copy(current.pos);
    set_mtx();

    s32 tevType = TEV_TYPE_BG1_PLIGHT;
    if (isDaizaItem(m_itemNo)) {
        tevType = TEV_TYPE_ACTOR;
    }
    J3DModel* model = mpModel;
    mTevType = tevType;
    gabi::store<u32>(gabi::ea(model) + 0xB8, 0); /* mpModel->setUserArea(NULL) */
}
VERIFY(0x024836F4, &daShopItem_c::CreateInit);

/* 02483C10. HD: the switch is a byte table for HEROS_FLAG..BIG_SALE_FLAG */
BOOL daShopItem_c::clothCreate() {
    WWHD_FUNC(0x02483C10, BOOL, this);
    OrigFrame frame(0x50, 27, 0x3C);
    const u32 clothFunc = frame.sp + 0x18; /* dCloth_packet_c::CreateFunc clothFunc[4] */
    const u32 clothRes = frame.sp + 0x28;  /* u32 clothRes[4] */

    if (isUseClothPacket(m_itemNo)) {
        gabi::store<u32>(clothFunc + 0x0, 0x0251BFA4); /* dClothVobj03_create */
        gabi::store<u32>(clothFunc + 0x4, 0x0251C134); /* dClothVobj04_create */
        gabi::store<u32>(clothFunc + 0x8, 0x0251C2D0); /* dClothVobj05_create */
        gabi::store<u32>(clothFunc + 0xC, 0x0251C478); /* dClothVobj07_0_create */
        gabi::store<u32>(clothRes + 0x0, 0x20);        /* dRes_INDEX_FDAI_BTI_FTEX03_e */
        gabi::store<u32>(clothRes + 0x4, 0x21);
        gabi::store<u32>(clothRes + 0x8, 0x22);
        gabi::store<u32>(clothRes + 0xC, 0x23);

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

        const char* arcName = getShopArcname();
        u32 res = gabi::load<u32>(clothRes + 4 * idx);
        void* shopArc = dComIfG_getObjectRes(arcName, res, SAFESTRING_VTBL);
        void* clothArc = dComIfG_getObjectRes(m_cloth_arcname, 3 /* dRes_INDEX_CLOTH_BTI_CLOTHTOON_e */, SAFESTRING_VTBL);

        u32 fn = gabi::load<u32>(clothFunc + 4 * field_0x648);
        dCloth_packet_c* cloth = gabi::call_ptr<dCloth_packet_c*>(fn, shopArc, clothArc, &tevStr, 0);
        field_0x644 = cloth;
        if (cloth == nullptr) {
            return FALSE;
        }
    } else {
        field_0x644 = nullptr;
    }

    return TRUE;
}
VERIFY(0x02483C10, &daShopItem_c::clothCreate);

/* 02483508 */
void daShopItem_c::set_mtx() {
    WWHD_FUNC(0x02483508, void, this);
    OrigFrame frame(0x48, 29, 0x3C);

    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &field_0x64C);

    u32 data = daShopItem_data::mData(m_itemNo); /* getData()[m_itemNo].field_0x0C */
    mDoMtx_stack_c::transM(gabi::load<f32>(data + 0xC), gabi::load<f32>(data + 0x10), gabi::load<f32>(data + 0x14));
    data = daShopItem_data::mData(m_itemNo); /* .field_0x18 */
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), gabi::load<s16>(data + 0x18), gabi::load<s16>(data + 0x1A), gabi::load<s16>(data + 0x1C));
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());

    if (field_0x644 != nullptr) {
        /* cXyz local[4] at sp+8 (GameCube: two arrays; HD keeps only the one it reads) */
        const u32 local = frame.sp + 8;
        u8 idx = field_0x648;
        f32 y94 = REG_F(10, 15) + 94.0f;
        f32 y97 = REG_F(10, 15) + 97.5f;
        const f32 ys[4] = {y94, y94, y97, y94};
        for (int i = 0; i < 4; i++) {
            gabi::store<f32>(local + 12 * i + 0, 0.0f);
            gabi::store<f32>(local + 12 * i + 4, ys[i]);
            gabi::store<f32>(local + 12 * i + 8, 0.0f);
        }
        u32 p = local + 12 * idx;
        mDoMtx_stack_c::transM(gabi::load<f32>(p), gabi::load<f32>(p + 4), gabi::load<f32>(p + 8));
        mDoMtx_stack_c::YrotM(0x4000);
        dCloth_packet_setScale(field_0x644, &scale);
        dCloth_packet_setMtx(field_0x644, mDoMtx_stack_c::get());
    }
}
VERIFY(0x02483508, &daShopItem_c::set_mtx);

/* 02483BBC */
bool daShopItem_c::_execute() {
    WWHD_FUNC(0x02483BBC, bool, this);
    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    set_mtx();
    return true;
}
VERIFY(0x02483BBC, &daShopItem_c::_execute);

/* 02483B28 */
bool daShopItem_c::_draw() {
    WWHD_FUNC(0x02483B28, bool, this);
    if (!chkDraw())
        return true;

    if (m_itemNo == dItemNo_FOUNTAIN_IDOL_e || m_itemNo == dItemNo_POSTMAN_STATUE_e) {
        /* mpModel->getModelData()->getJointNodePointer(0)->setMtxCalc(0) (HD: model data +0xAC,
         * root joint at +8, its calc pointer at +0x14) */
        u32 modelData = gabi::load<u32>(gabi::ea(mpModel.get()) + 0xAC);
        u32 joint = gabi::load<u32>(modelData + 8);
        gabi::store<u32>(joint + 0x14, 0);
    }
    gabi::call_ptr(vfn(daItemBase_VT_DRAWBASE), this); /* DrawBase() (virtual) */

    if (field_0x644 != nullptr)
        dCloth_packet_cloth_draw(field_0x644);

    return true;
}
VERIFY(0x02483B28, &daShopItem_c::_draw);

/* 02483E24 */
void daShopItem_c::settingBeforeDraw() {
    WWHD_FUNC(0x02483E24, void, this);
    if (isBomb(m_itemNo) || (m_itemNo == dItemNo_BOMB_BAG_e) || (m_itemNo == dItemNo_SKULL_HAMMER_e) ||
        m_itemNo == dItemNo_SMALL_KEY_e || m_itemNo == dItemNo_SHOP_GURU_STATUE_e) {
        dDlst_texSpecmapST(&eyePos, &tevStr, mpModel, 1.0f);
    }
}
VERIFY(0x02483E24, &daShopItem_c::settingBeforeDraw);

/* 02483D8C */
void daShopItem_c::setTevStr() {
    WWHD_FUNC(0x02483D8C, void, this);
    dScnKy_env_light_c* env = dKy_getEnvlight();
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
VERIFY(0x02483D8C, &daShopItem_c::setTevStr);

/* 024837F8: daShopItem_c::_create() inlined. HD: an item number of 0xFF becomes a green rupee
 * before the model check; the arc name of the model check is kept for the resource load */
static cPhs_State daShopItem_Create(void* i_this) {
    WWHD_FUNC(0x024837F8, cPhs_State, i_this);
    daShopItem_c* a = static_cast<daShopItem_c*>(i_this);

    /* fopAcM_ct(this, daShopItem_c) */
    if (!fopAcM_CheckCondition(a, fopAcCnd_INIT_e)) {
        if (a != nullptr) {
            fopAc_ac_c_ct(a);
            a->__vtbl = DAITEMBASE_VTBL;
            dBgS_ObjAcch_ct(&a->mAcch, {0x1003A86C, 0x1003A88C, 0x1003A87C});
            dBgS_AcchCir_ct(&a->mAcchCir);
            dCcD_Stts_ct(&a->mStts);
            dCcD_Cyl_ct(&a->mCyl, 0x1003A85C);
            a->__vtbl = DASHOPITEM_VTBL;
        }
        fopAcM_OnCondition(a, fopAcCnd_INIT_e);
    }

    u8 itemNo = gabi::load<u8>(gabi::ea(a) + 0xB3); /* fopAcM_GetParamBit(param, 0, 8) */
    if (itemNo < 0xFF) {
        a->m_itemNo = itemNo;
    } else {
        a->m_itemNo = dItemNo_GREEN_RUPEE_e;
    }

    const char* arcName = a->getShopArcname();
    if (a->getShopBmdIdx() == -1 || arcName == nullptr) {
        a->m_itemNo = dItemNo_GREEN_RUPEE_e;
        arcName = a->getShopArcname();
    }

    cPhs_State result = dComIfG_resLoad(&a->mPhs, arcName);
    if (result != cPhs_COMPLEATE_e) {
        return result;
    }
    if (isUseClothPacket(a->m_itemNo)) {
        cPhs_State result2 = dComIfG_resLoad(&a->mPhase, m_cloth_arcname);
        if (result2 != cPhs_COMPLEATE_e) {
            return result2;
        }
    }

    u8 type = (fopAcM_GetParam(a) >> 8) & 0xF;
    u8 no = a->m_itemNo;
    if (type == 2 || (type == 0 && daShopItem_data::mModelType(no) == 0x02)) {
        if (fopAcM_entrySolidHeap(a, 0x021841D0 /* CheckItemCreateHeap */, dItem_data_getHeapSize(no)) == 0) {
            return cPhs_ERROR_e;
        }
    } else if (type == 1 || (type == 0 && daShopItem_data::mModelType(no) == 0x01)) {
        if (fopAcM_entrySolidHeap(a, 0x0218422C /* CheckFieldItemCreateHeap */, dItem_data_getFieldHeapSize(no)) == 0) {
            return cPhs_ERROR_e;
        }
    } else {
        if (fopAcM_entrySolidHeap(a, 0x021841D0 /* CheckItemCreateHeap */, dItem_data_getHeapSize(no)) == 0) {
            return cPhs_ERROR_e;
        }
    }

    a->CreateInit();
    return result;
}
VERIFY(0x024837F8, daShopItem_Create);

/* 02483AC8 */
static BOOL daShopItem_Delete(void* i_this) {
    WWHD_FUNC(0x02483AC8, BOOL, i_this);
    daShopItem_c* inst = static_cast<daShopItem_c*>(i_this);

    if (isUseClothPacket(inst->m_itemNo)) {
        dComIfG_resDelete(&inst->mPhase, m_cloth_arcname);
    }
    inst->DeleteBase(inst->getShopArcname());

    return TRUE;
}
VERIFY(0x02483AC8, daShopItem_Delete);

/* 02483BB8 (a tail branch: _draw's r3 is returned as is) */
static BOOL daShopItem_Draw(void* i_this) {
    WWHD_FUNC(0x02483BB8, BOOL, i_this);
    return gabi::call<BOOL>(0x02483B28, i_this); /* static_cast<daShopItem_c*>(i_this)->_draw() */
}
VERIFY(0x02483BB8, daShopItem_Draw);

/* 02483C0C */
static BOOL daShopItem_Execute(void* i_this) {
    WWHD_FUNC(0x02483C0C, BOOL, i_this);
    return gabi::call<BOOL>(0x02483BBC, i_this); /* static_cast<daShopItem_c*>(i_this)->_execute() */
}
VERIFY(0x02483C0C, daShopItem_Execute);

/* 02483F2C */
static BOOL daShopItem_IsDelete(void*) {
    WWHD_FUNC(0x02483F2C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02483F2C, daShopItem_IsDelete);

/* 02483E98 __sinit_d_a_shop_item_cpp (new: compiler-generated header statics) */
static void shop_item_sinit() {
    WWHD_FUNC(0x02483E98, void, (u32)0);
    sinit_header_statics(0x1046DE08, 0x101D051C);
}
VERIFY(0x02483E98, shop_item_sinit);

/* 02483F34 sead::SafeString deleting destructor (this TU's copy; vtable 0x1003A844 slot 0xC) */
static void shop_item_SafeString_dtor(void* p, s32 flags) {
    WWHD_FUNC(0x02483F34, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02483F34, shop_item_SafeString_dtor);

/* 02483F48 daShopItem_c::setListStart() (empty, vtable slot 0x1C) */
static void shop_item_setListStart(daShopItem_c*) {
    WWHD_FUNC(0x02483F48, void, (u32)0);
}
VERIFY(0x02483F48, shop_item_setListStart);

/* 02483F4C daShopItem_c deleting destructor (vtable slot 0xC): ~daItemBase_c (021832F4) */
static void shop_item_dtor(daShopItem_c* p, s32 flags) {
    WWHD_FUNC(0x02483F4C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x021832F4, p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02483F4C, shop_item_dtor);

/* 02483FA0 sead::SafeString virtual (empty; this TU's vtable 0x1003A844 slot 0x14) */
static void shop_item_SafeString_v14(void*) {
    WWHD_FUNC(0x02483FA0, void, (u32)0);
}
VERIFY(0x02483FA0, shop_item_SafeString_v14);

/* ---- d_a_shop_item_static (02483FA4..024840D8) ---- */

/* 02483FA4 */
cXyz* daShopItem_c::getScaleP() {
    WWHD_FUNC(0x02483FA4, cXyz*, this);
    return &scale;
}
VERIFY(0x02483FA4, &daShopItem_c::getScaleP);

/* 02483FAC */
csXyz* daShopItem_c::getRotateP() {
    WWHD_FUNC(0x02483FAC, csXyz*, this);
    return &current.angle;
}
VERIFY(0x02483FAC, &daShopItem_c::getRotateP);

/* 02483FB4 */
cXyz* daShopItem_c::getPosP() {
    WWHD_FUNC(0x02483FB4, cXyz*, this);
    return &current.pos;
}
VERIFY(0x02483FB4, &daShopItem_c::getPosP);

/* 02483FBC daShopItem_c::getCenter(): the cXyz result is returned through a hidden pointer (r4);
 * HD: like a constructor, GHS allocates it when that pointer is NULL */
static void daShopItem_getCenter(daShopItem_c* self, cXyz* ret) {
    WWHD_FUNC(0x02483FBC, void, self, ret);
    f32 height;
    if (dItemNo_TOWN_FLOWER_e <= self->m_itemNo && self->m_itemNo <= dItemNo_SHOP_GURU_STATUE_e) {
        height = 80.0f;
    } else {
        height = 40.0f;
    }
    f32 half = height * 0.5f;
    if (ret == nullptr) {
        ret = (cXyz*)operator_new(sizeof(cXyz));
        if (ret == nullptr)
            return;
    }
    ret->y = half;
    ret->x = 0.0f;
    ret->z = 0.0f;
}
VERIFY(0x02483FBC, daShopItem_getCenter);

/* 02484048 __sinit_d_a_shop_item_static_cpp */
static void shop_item_static_sinit() {
    WWHD_FUNC(0x02484048, void, (u32)0);
    sinit_header_statics(0x1046DE24, 0x101D0570);
}
VERIFY(0x02484048, shop_item_static_sinit);
