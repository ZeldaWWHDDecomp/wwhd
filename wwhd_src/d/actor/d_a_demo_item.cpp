/**
 * d_a_demo_item.cpp (WWHD)
 * Item - Cutscene Item (the item Link holds up in get-item cutscenes).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_demo_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_demo_item.h"
#include "gc_api/JSystem/JPAMath.h"

// HD: per-unit inline-constructor vtables and header objects.
const gc_tu_data gc_tu = {.objAcch = {0x1000DA84, 0x1000DAA4, 0x1000DA94},
                          .headerStatics = {0x10463B18, 0x101B43E4, 0x10463B24}};
static constexpr u32 DITEM_VTBL = 0x1000DBE4;
// HD: heap creation uses the native item-base callback from a different unit.
static constexpr u32 CheckItemCreateHeapHD = 0x021841D0;
static u8 m_effect_type[] = {
    4, 3, 3, 3, 3, 3, 3, 0, 0, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 3, 4, 4, 4, 4, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 0, 4, 0, 0, 4, 0, 0, 0, 4, 1,
    4, 4, 0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 4, 0, 2, 2, 2, 0, 0, 2, 2, 0, 2, 4, 4, 4, 4, 4, 4,
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 0,
    4, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 4, 4, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4};
static DitemOffsetTableHD offset_tbl;
static DitemResourceTableHD l_itemResources;
void Ditem_rotateZXY(Mtx34 *, s16, s16, s16);
s16 Ditem_cameraAngleX(camera_class *);
s16 Ditem_cameraAngleY(camera_class *);
// HD: the specular-map helper receives the model rather than its model data.
void Ditem_texSpecmapST(cXyz *, dKy_tevstr_c *, J3DModel *, f32);
void Ditem_addAngleY(fopAc_ac_c *, s16, s16);
void Ditem_destroyItemBase(daItemBase_c *, s32);

// HD: no unused scale/position temporaries.
/* 021233CC */
void daDitem_c::setParticle() {
    if (mpEmitters[0] || mpEmitters[1] || mpEmitters[2] || mpEmitters[3]) {
        return;
    }
    if (chkArgFlag(FLAG_UNK02) || chkArgFlag(FLAG_UNK04) || chkArgFlag(FLAG_UNK08)) {
        return;
    }

    gabi::Local<csXyz> angle;
    angle->x = (s16)(Ditem_cameraAngleX(dComIfGp_getCamera(0)) - 0x2000);
    angle->y = Ditem_cameraAngleY(dComIfGp_getCamera(0));
    angle->z = 0;

    switch (m_effect_type[m_itemNo]) {
    case 0:
        mpEmitters[0] = dComIfGp_particle_set(dPa_name::ID_IT_JN_GETITEM_FLASH_L00, &current.pos, angle);
        [[fallthrough]];
    case 1:
        mpEmitters[1] = dComIfGp_particle_set(dPa_name::ID_IT_JN_GETITEM_FLASH_S00, &current.pos, angle);
        [[fallthrough]];
    case 2:
        mpEmitters[2] = dComIfGp_particle_set(dPa_name::ID_IT_JN_GETITEM_HALO00, &current.pos, angle);
        [[fallthrough]];
    case 3:
        mpEmitters[3] = dComIfGp_particle_set(dPa_name::ID_IT_JN_GETITEM_STAR00, &current.pos, angle);
        [[fallthrough]];
    case 4:
        break;
    }
}

/* 02123164 */
bool daDitem_c::CreateInit() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    hide();
    mFlag = 0; /* clrFlag() */
    u8 argFlag = (fopAcM_GetParam(this) >> 0x10) & 0xFF;
    mArgFlag = argFlag;
    if (!(argFlag & FLAG_UNK02) && !(argFlag & FLAG_UNK04) && !(argFlag & FLAG_UNK08)) {
        current.angle.y = -0x2000;
    }
    for (int i = 0; i < 4; i++) {
        mpEmitters[i] = nullptr;
    }
    return true;
}

/* 021235A4 */
void daDitem_c::set_effect() {
    s16 angleX = (s16)(Ditem_cameraAngleX(dComIfGp_getCamera(0)) - 0x2000);
    s16 angleY = Ditem_cameraAngleY(dComIfGp_getCamera(0));
    for (int i = 0; i < 4; i++) {
        if (mpEmitters[i] == nullptr) {
            continue;
        }
        /* playCreateParticle() */
        DitemEmitterHD *e = (DitemEmitterHD *)mpEmitters[i].get();
        e->mFlags &= ~1u;
        // HD: translation modes seven and above negate global Y.
        e = (DitemEmitterHD *)mpEmitters[i].get();
        f32 y = current.pos.y;
        u8 kind = e->mTranslationMode;
        f32 x = current.pos.x;
        f32 z = current.pos.z;
        e->mGlobalTranslation.x = x;
        e->mGlobalTranslation.z = z;
        e->mGlobalTranslation.y = y;
        if (kind >= 7) {
            e->mGlobalTranslation.y = -(f32)e->mGlobalTranslation.y;
        }
        /* setGlobalRotation(rot) */
        JPAGetXYZRotateMtx(angleX, angleY, 0, &((DitemEmitterHD *)mpEmitters[i].get())->mGlobalRotation);
    }
}

/* 021236A8 */
void daDitem_c::set_pos() {
    // HD: retain the guard write between the component stores; generic static initialization differs.
    if (offset_tbl.mInitialized == 0) {
        offset_tbl.mOffsets[2].x = 30.0f;
        offset_tbl.mOffsets[1].x = 0.0f;
        offset_tbl.mOffsets[2].y = 140.0f;
        offset_tbl.mOffsets[1].z = 50.0f;
        offset_tbl.mOffsets[0].y = 130.0f;
        offset_tbl.mInitialized = 1;
        offset_tbl.mOffsets[2].z = 20.0f;
        offset_tbl.mOffsets[0].z = 0.0f;
        offset_tbl.mOffsets[1].y = 90.0f;
        offset_tbl.mOffsets[0].x = 0.0f;
    }

    gabi::Local<cXyz> offset;
    gabi::Local<cXyz> pos;
    if (chkArgFlag(FLAG_UNK02)) {
        offset->copy(offset_tbl.mOffsets[1]);
    } else if (chkArgFlag(FLAG_UNK04)) {
        offset->copy(offset_tbl.mOffsets[2]);
    } else if (chkArgFlag(FLAG_UNK08)) {
        offset->copy(mOffsetPos);
    } else {
        offset->copy(offset_tbl.mOffsets[0]);
    }

    if (!chkArgFlag(FLAG_UNK08)) {
        s16 ax = dComIfGp_getPlayer(0)->current.angle.x;
        s16 ay = dComIfGp_getPlayer(0)->shape_angle.y;
        s16 az = dComIfGp_getPlayer(0)->current.angle.z;
        Ditem_rotateZXY(mDoMtx_stack_c::get(), ax, ay, az);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset, offset);
        pos->copy(dComIfGp_getPlayer(0)->current.pos);
    } else {
        pos->copy(home.pos);
    }

    PSVECAdd(pos, offset, pos);
    current.pos.copy(*pos);
}

// HD: bottle items are handled as one contiguous range.
/* 0212366C */
void daDitem_c::anim_control() {
    u8 no = m_itemNo;
    if (no >= dItemNo_EMPTY_BSHIP_e && no <= dItemNo_UNK_BOTTLE_60_e) {
        if (m_timer > 30) {
            animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    } else {
        animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
}

/* 021238EC */
void daDitem_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    Ditem_addAngleY(this, (s16)(current.angle.y + 0x0111), (s16)0x0111); /* fopAcM_addAngleY */

    if (chkArgFlag(FLAG_UNK02) || chkArgFlag(FLAG_UNK04) || chkArgFlag(FLAG_UNK08)) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(current.angle.y);
    } else {
        PSMTXCopy(dComIfGd_getInvViewMtx(), mDoMtx_stack_c::get());
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), current.angle.z);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), 0x12C0);
        mDoMtx_stack_c::YrotM(current.angle.y);
        mDoMtx_stack_c::get()->m[0][3] = current.pos.x;
        mDoMtx_stack_c::get()->m[1][3] = current.pos.y;
        mDoMtx_stack_c::get()->m[2][3] = current.pos.z;
    }
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 02123B70 */
void daDitem_c::settingBeforeDraw() {
    if (isBomb(m_itemNo) || m_itemNo == dItemNo_BOMB_BAG_e || m_itemNo == dItemNo_SKULL_HAMMER_e ||
        m_itemNo == dItemNo_SMALL_KEY_e) {
        Ditem_texSpecmapST(&eyePos, &tevStr, mpModel, 1.0f);
    }
}

/* 02123C70 */
static void daDitem_setListStart(daDitem_c *i_this) {}

/* 021230C8 */
BOOL daDitem_c::Delete() {
    if (!chkArgFlag(FLAG_UNK01)) {
        execItemGet(m_itemNo);
    }

    for (int i = 0; i < 4; i++) {
        if (mpEmitters[i]) {
            ((DitemEmitterHD *)mpEmitters[i].get())->mGlobalAlpha = 0; /* setGlobalAlpha(0) */
            /* becomeInvalidEmitter() (HD: max frame -1, stop flag) */
            DitemEmitterHD *e = (DitemEmitterHD *)mpEmitters[i].get();
            u32 flags = (u32)e->mFlags;
            e->mMaxFrame = -1;
            e->mFlags = flags | 1;
            mpEmitters[i] = nullptr;
        }
    }

    return DeleteBase(l_itemResources.mItems[m_itemNo].mArcname);
}

/* 021231F8 */
cPhs_State daDitem_c::create() {
    /* fopAcM_ct(this, daDitem_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x10012158; /* daItemBase_c */
            dBgS_ObjAcch_ct(&mAcch, gc_tu.objAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, 0x1000DA74);
            __vtbl = DITEM_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    u8 no = (u8)fopAcM_GetParam(this);
    m_itemNo = no;
    const DitemResourceHD &res = l_itemResources.mItems[no];
    const char *arcName = res.mArcname;
    if ((s16)res.mBmdIdx == -1 || arcName == nullptr) { /* getBmdIdx, getArcname */
        m_itemNo = dItemNo_GREEN_RUPEE_e;
        arcName = l_itemResources.mItems[dItemNo_GREEN_RUPEE_e].mArcname;
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, arcName);
    if (phase_state == cPhs_COMPLEATE_e) {
        u32 heap_size = l_itemResources.mItems[m_itemNo].mHeapSize; /* getHeapSize */
        if (!fopAcM_entrySolidHeap(this, CheckItemCreateHeapHD, heap_size)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }
    return phase_state;
}

/* 021233C4 */
static BOOL daDitem_IsDelete(daDitem_c *i_this) {
    return TRUE;
}

/* 02123AA4 */
BOOL daDitem_c::execute() {
    m_timer = m_timer + 1;

    if (chkDraw()) {
        setParticle();
        set_effect();
        Ditem_onDraw(this);
    } else {
        fopAcM_offDraw(this);
    }

    anim_control();

    if (chkDead()) {
        fopAcM_delete(this);
    }

    set_pos();
    set_mtx();
    return TRUE;
}

/* 02123B60 */
static BOOL daDitem_Draw(daDitem_c *i_this) {
    return Ditem_drawBaseVirtual(i_this);
}

// HD: compiler-generated common-header initialization and deleting destructor.
/* 02123BDC */
static void __sinit_d_a_demo_item_cpp() {
    __sinit_header_statics();
}

/* 02123C74 */
static void daDitem_c_dt(daDitem_c *self, s32 flags) {
    if (self != nullptr) {
        Ditem_destroyItemBase(self, 0); /* daItemBase_c::~daItemBase_c */
        if (flags & 1)
            operator_delete(self);
    }
}

/* WWHD glue
data m_effect_type 1000DAE4
data offset_tbl 10463B34
data l_itemResources 101E4674
ext 025F1AA4 void Ditem_rotateZXY(Mtx34*, s16, s16, s16)
ext 024F8008 s16 Ditem_cameraAngleX(camera_class*)
ext 024F8000 s16 Ditem_cameraAngleY(camera_class*)
ext 0252CA4C void Ditem_texSpecmapST(cXyz*, dKy_tevstr_c*, J3DModel*, f32)
ext 025D677C void Ditem_addAngleY(fopAc_ac_c*, s16, s16)
ext 025DF2B8 s16 Ditem_getPriority(fopAc_ac_c*)
ext 025DA874 void Ditem_addToDrawQueue(create_tag_class*, s16)
ext 021842B8 void daItemBase_c::hide()
ext 021842D8 bool daItemBase_c::chkDraw()
ext 0218433C bool daItemBase_c::chkDead()
ext 021837B0 void daItemBase_c::animPlay(f32, f32, f32, f32, f32)
ext 02183788 BOOL daItemBase_c::DeleteBase(const char*)
ext 021832F4 void Ditem_destroyItemBase(daItemBase_c*, s32)
*/
