/* WWHD barrel items. Derived from zeldaret/tww; */
#include "d/actor/d_a_race_item.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_bg_s.h"
#include "gc_api/f_op/f_op_actor_mng.h"
const gc_tu_data gc_tu = {.objAcch = {0x10038D9C, 0x10038DBC, 0x10038DAC},
                          .headerStatics = {0x1046D634, 0x101CF2F8, 0x1046D640}};
static RaceItemFieldResources l_fieldItemRes;
static RaceItemResources l_itemResource;
static RaceItemRotateData l_rotateData;
static RaceItemActorVtable l_itemBaseVtable;
static RaceItemActorVtable l_raceItemVtable;
static RaceItemActorVtable l_aabVtable;
static dCcD_SrcCyl l_cyl_src;
// HD: external callback registrations pending the shared T-07 autoglue follow-up.
static gabi::HostFn l_heapBinding(reinterpret_cast<const void*>(CheckFieldItemCreateHeap), 0x0218422C);
static gabi::HostFn l_searchBinding(reinterpret_cast<const void*>(fpcSch_JudgeForPName), 0x025E121C);

/* 02459158 */
void daRaceItem_c::set_mtx() {
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    J3DModel* model = mpModel;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 0245979C */
BOOL daRaceItem_c::Delete() {
    return DeleteBase(l_fieldItemRes.mItems[u32(m_itemNo)].mArc.get());
}

/* 02459924 */
cPhs_State daRaceItem_c::create() {
    if (!(actor_condition & 8)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_itemBaseVtable);
            __ct(&mAcch);
            __ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, gabi::ea(&l_aabVtable));
            __vtbl = gabi::ea(&l_raceItemVtable);
        }
        actor_condition |= 8;
    }
    u32 params = mParameters;
    s32 bit = (params >> 8) & 127;
    s8 room = home.roomNo;
    m_itemNo = params & 255;
    mItemBitNo = bit;
    if (dComIfGs_isItem(bit, room) && mItemBitNo != 127) {
        setLoadError();
        return cPhs_ERROR_e;
    }
    const char* arc = l_fieldItemRes.mItems[u32(m_itemNo)].mArc.get();
    cPhs_State phase = dComIfG_resLoad(&mPhs, arc);
    if (phase == cPhs_COMPLEATE_e) {
        u32 heap = l_itemResource.mItems[u32(m_itemNo)].mHeapSize;
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(CheckFieldItemCreateHeap)), heap))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}

/* 024597B8 */
BOOL daRaceItem_c::CreateInit() {
    set_mtx();
    cullMtx = gabi::ea(mpModel->getBaseTRMtx());
    mStts.Init(0, 255, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    f32 height = f32(RaceItem_getHeight(this));
    f32 radius = f32(RaceItem_getR(this));
    f32 sx = scale.x;
    if (sx > 1.0f) {
        radius *= sx;
        height *= sx;
    }
    mCyl.SetR(radius);
    mCyl.SetH(height);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    u32 flags = mAcch.m_flags;
    field_0x640 = RaceItem_Wait;
    u32 params = mParameters;
    mAcch.m_flags = flags & ~(dBgS_Acch::WATER_NONE | dBgS_Acch::ROOF_NONE);
    field_0x644 = (params >> 15) & 15;
    return true;
}

/* 0245910C */
void daRaceItem_c::checkGet() {
    if (field_0x644 == RaceItem_Normal && mCyl.ChkCoHit())
        normalItemGet();
}

/* 024590FC */
static BOOL daRaceItem_Draw(daRaceItem_c* p) {
    return p->draw();
}

/* 02459230 */
BOOL daRaceItem_c::execute() {
    dComIfGp_ea(); // HD accessor retained even though its result is unused.
    m_timer = u32(m_timer) + 1;
    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    f32 y = current.pos.y, z = current.pos.z, x = current.pos.x;
    u32 item = m_itemNo;
    eyePos.y = y;
    eyePos.z = z;
    eyePos.x = x;
    f32 h = f32(dItemDataView::getH(item));
    u32 state = field_0x640;
    eyePos.y = gabi::fmadds(h * f32(scale.x), 0.5f, y);
    switch (state) {
    case RaceItem_Wait: {
        checkGet();
        s32 divisor = l_rotateData.mRotateYSpeed;
        s16 spin = divisor ? s16(65535 / divisor) : 0;
        s16 angle = current.angle.y;
        fopAcM_addAngleY(this, s16(angle + spin), spin);
        mCyl.SetC(&current.pos);
        dComIfG_Ccsp_Set(&mCyl);
        break;
    }
    case RaceItem_GetStart: {
        auto* player = static_cast<daPy_py_c*>(dComIfGp_getLinkPlayer());
        f32 hy = (f32)player->mHeadTopPos.y + 15.0f;
        f32 hx = (f32)player->mHeadTopPos.x, hz = (f32)player->mHeadTopPos.z;
        current.pos.x = hx;
        current.angle.z = 0;
        current.pos.y = hy;
        current.angle.x = 0;
        current.pos.z = hz;
        scale.x = 1.0f;
        scale.y = 1.0f;
        scale.z = 1.0f;
        field_0x63C = 13;
        speed.x = 0.0f;
        speed.y = 23.0f;
        shape_angle.z = 0;
        speed.z = 0.0f;
        shape_angle.x = 0;
        gravity = -6.0f;
        mCyl.ClrTgHit();
        RaceItem_ClrCoHit(&mCyl);
        u32 co = mCyl.mObjCo.mSPrm, tg = mCyl.mObjTg.mSPrm;
        u8 flags = field_0x645;
        field_0x640 = RaceItem_Get;
        mCyl.mObjCo.mSPrm = co & ~1u;
        mCyl.mObjTg.mSPrm = tg & ~1u;
        field_0x645 = flags & ~u32(RaceItem_BoomerangCarry);
        [[fallthrough]];
    }
    case RaceItem_Get: {
        field_0x63C = u32(field_0x63C) - 1;
        auto* player = static_cast<daPy_py_c*>(dComIfGp_getLinkPlayer());
        f32 hz = (f32)player->mHeadTopPos.z, hx = (f32)player->mHeadTopPos.x;
        f32 hy = (f32)player->mHeadTopPos.y + 15.0f;
        current.pos.x = hx;
        current.pos.z = hz;
        fopAcM_posMoveF(this, nullptr);
        if (f32(current.pos.y) < hy)
            current.pos.y = hy;
        if (field_0x63C < 0)
            fopAcM_delete(this);
        break;
    }
    case RaceItem_DropStart:
        field_0x640 = RaceItem_Drop;
        [[fallthrough]];
    case RaceItem_Drop: {
        checkGet();
        fopAcM_posMoveF(this, &mStts.m_cc_move);
        mAcch.CrrPos(dComIfG_Bgsp());
        if (!(field_0x645 & RaceItem_BoomerangCarry) && !(actor_status & fopAcStts_HOOK_CARRY_e)) {
            if (mAcch.m_flags & dBgS_Acch::WATER_IN) {
                s32 reverb = dComIfGp_getReverb(s32(current.roomNo));
                mDoAud_seStart(JA_SE_OBJ_FALL_WATER_S, &eyePos, 0, reverb);
                fopAcM_delete(this);
            }
            f32 px = current.pos.x, pz = current.pos.z;
            if (daSea_ChkArea(px, pz)) {
                px = current.pos.x;
                pz = current.pos.z;
                f32 wave = daSea_calcWave(px, pz);
                if (wave != 0.0f) {
                    f32 py = current.pos.y;
                    if (py - wave < 0.0f)
                        fopAcM_delete(this);
                    // The compiler retains py if no delete intervenes.
                }
            }
        }
        if (f32(current.pos.y) < -5000.0f)
            fopAcM_delete(this);
        mCyl.SetC(&current.pos);
        dComIfG_Ccsp_Set(&mCyl);
        break;
    }
    }
    if (mCyl.ChkTgHit() && field_0x644 == RaceItem_Normal) {
        void* obj = mCyl.GetTgHitObj();
        if (obj) {
            u32 type = static_cast<dCcD_GObjInf*>(obj)->mAtType;
            if (type & AT_TYPE_BOOMERANG)
                field_0x645 = u8(field_0x645) | RaceItem_BoomerangCarry;
            else if (type & AT_TYPE_HOOKSHOT) {
                f32 height = f32(dItemDataView::getH(u32(m_itemNo)));
                gabi::Local<cXyz> offset;
                offset->z = 0.0f;
                offset->x = 0.0f;
                offset->y = height * 0.5f;
                auto* player = dComIfGp_getPlayer(0);
                auto* vt = reinterpret_cast<RaceItemPlayerVtableLink*>(player)->mpVtable.get();
                u32 method = vt->setHookshotCarryOffset.get();
                u32 id = mBsPcId;
                gabi::call_ptr<void>(method, player, id, offset.get());
            }
        }
    }
    if (field_0x645 & RaceItem_BoomerangCarry) {
        gabi::Local<be<s16>> name;
        *name = fpcNm_BOOMERANG_e;
        fopAc_ac_c* boom = static_cast<fopAc_ac_c*>(fopAcM_Search(fpcSch_JudgeForPName, name.get()));
        if (boom)
            current.pos.copy(boom->current.pos);
        else
            field_0x645 = u8(field_0x645) & ~u32(RaceItem_BoomerangCarry);
    }
    if (!checkOffsetPos()) {
        scale.x = 1.0f;
        scale.y = 1.0f;
        scale.z = 1.0f;
    }
    set_mtx();
    return true;
}

/* 02459794 */
static BOOL daRaceItem_IsDelete(daRaceItem_c* p) {
    return true;
}

/* 024597B4 */
static BOOL daRaceItem_Delete(daRaceItem_c* p) {
    return p->Delete();
}

/* 02459B18 */
static cPhs_State daRaceItem_Create(daRaceItem_c* p) {
    return p->create();
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 02459B1C */
static void __sinit_d_a_race_item_cpp() {
    __sinit_header_statics();
}

/* 02459BB0 */
void daRaceItem_c::setListStart() {}

/* 02459BB4 */
static void daRaceItem_c_dtor(daRaceItem_c* p, s32 flags) {
    if (p) {
        RaceItemBase_dt(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue
data l_fieldItemRes 101E6A74
data l_itemResource 101E4674
data l_rotateData 100121CC
data l_itemBaseVtable 10012158
data l_raceItemVtable 10038DCC
data l_aabVtable 10038D8C
data l_cyl_src 101CF2B4
ext 02183788 BOOL daItemBase_c::DeleteBase(const char*)
ext 02184288 u32 RaceItem_getHeight(daItemBase_c*)
ext 021842A0 u32 RaceItem_getR(daItemBase_c*)
ext 02184350 void daItemBase_c::setLoadError()
ext 0218422C BOOL CheckFieldItemCreateHeap(fopAc_ac_c*)
ext 021837B0 void daItemBase_c::animPlay(f32,f32,f32,f32,f32)
ext 025D677C void fopAcM_addAngleY(fopAc_ac_c*,s16,s16)
ext 0251641C void RaceItem_ClrCoHit(dCcD_Cyl*)
ext 025E121C void* fpcSch_JudgeForPName(void*,void*)
ext 021832F4 void RaceItemBase_dt(daItemBase_c*,s32)
*/
