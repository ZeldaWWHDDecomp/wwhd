/* WWHD four sliding gate panels. */
#include "d/actor/d_a_obj_hami4.h"
enum { dRes_INDEX_HAMI4_BDL_HAMI4_e = 4, dRes_INDEX_HAMI4_DZB_HAMI4_e = 7 };
static f32 l_openingSine;
static f32 l_openingCosine;
/* 02350410 */
static u32 PrmAbstract(daObjHami4_c *p, u32 width, u32 shift) {
    u32 parameters = p->mParameters;
    u32 mask = (width & 32) ? 0 : (1U << (width & 31));
    u32 value = (shift & 32) ? 0 : (parameters >> (shift & 31));
    return value & (mask - 1);
}
static void translateOpening(daObjHami4_c *p) {
    f32 amount = p->mOpening;
    f32 sine = l_openingSine;
    f32 cosine = l_openingCosine;
    f32 x = (s16)gabi::ftoi(-(amount * sine));
    f32 z = (s16)gabi::ftoi(-(amount * cosine));
    mDoMtx_stack_c::transM(x, 0.0f, z);
}
/* 0234F914 */
BOOL daObjHami4_c::CreateHeap() {
    auto data = (J3DModelData *)dComIfG_getObjectRes(STR(0x10029E40), dRes_INDEX_HAMI4_BDL_HAMI4_e,
                                                     0x10029E04);
    if (!data)
        JUT_ASSERT_fail(STR(0x10029E48), 0x5F, STR(0x10029E5C));
    u32 sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (dComIfGs_isSwitch(sw, home.roomNo)) {
        mState = OPEN_STOP;
        mOpening = 1500.0f;
    } else {
        mOpening = 0.0f;
        mState = CLOSE_STOP;
    }
    for (u32 i = 0; i < 4; ++i) {
        J3DModel *created = mDoExt_J3DModel__create(data, 0, 0x11020203);
        mpModels[i] = created;
        if (!created)
            return FALSE;
        s16 angle = (s16)((s32)current.angle.y + i * 0x4000);
        f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
        mDoMtx_stack_c::transS(x, y, z);
        mDoMtx_stack_c::YrotM(angle);
        translateOpening(this);
        f32 sy = scale.y, sx = scale.x, sz = scale.z;
        mDoMtx_stack_c::scaleM(sx, sy, sz);
        MTXCopy(mDoMtx_stack_c::get(), &mBackgroundMtx[i]);
        J3DModel_setBaseTRMtx(mpModels[i], mDoMtx_stack_c::get());
        dBgW *bg = new_dBgW();
        mdBgW[i] = bg;
        if (!bg)
            return FALSE;
        void *resource =
            dComIfG_getObjectRes(STR(0x10029E40), dRes_INDEX_HAMI4_DZB_HAMI4_e, 0x10029E04);
        bg = mdBgW[i];
        if (bg->Set((cBgD_t *)resource, dBgW::MOVE_BG_e, &mBackgroundMtx[i]))
            return FALSE;
    }
    return TRUE;
}
/* 0234FC24 */
static BOOL CheckCreateHeap(fopAc_ac_c *p) {
    return ((daObjHami4_c*)p)->CreateHeap();
}
/* 0234FC28 */
void daObjHami4_c::set_mtx() {
    for (u32 i = 0; i < 4; ++i) {
        f32 sy = scale.y;
        s16 angle = (s16)((s32)current.angle.y + i * 0x4000);
        f32 sx = scale.x, sz = scale.z;
        gabi::Local<cXyz> modelScale;
        modelScale->x = sx;
        modelScale->y = sy;
        modelScale->z = sz;
        J3DModel_setBaseScale(mpModels[i], modelScale);
        f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
        mDoMtx_stack_c::transS(x, y, z);
        mDoMtx_stack_c::YrotM(angle);
        translateOpening(this);
        J3DModel_setBaseTRMtx(mpModels[i], mDoMtx_stack_c::get());
        MTXCopy(mDoMtx_stack_c::get(), &mBackgroundMtx[i]);
        mdBgW[i]->Move();
    }
}
/* 0234FDE8 */
void daObjHami4_c::CreateInit() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModels[0]));
    for (u32 i = 0; i < 4; ++i) {
        auto bgsp = dComIfG_Bgsp();
        bgsp->Regist(mdBgW[i], this);
    }
    fopAcM_setCullSizeBox(this, -10000.0f, -100.0f, -10000.0f, 10000.0f, 100.0f, 10000.0f);
    fopAcM_setCullSizeFar(this, 1.0f);
    set_mtx();
}
/* 0234FE9C */
static s32 Create(daObjHami4_c *p) {
    fopAcM_ct(p, daObjHami4_c);
    for (u32 i = 0; i < 4; ++i)
        p->mdBgW[i] = nullptr;
    s32 status = dComIfG_resLoad(&p->mPhs, STR(0x10029DF4));
    if (status == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(p, CheckCreateHeap, 0x4900))
            return cPhs_ERROR_e;
        p->CreateInit();
    }
    return status;
}
/* 0234FF78 */
static BOOL Delete(daObjHami4_c *p) {
    for (u32 i = 0; i < 4; ++i) {
        dBgW *bg = p->mdBgW[i];
        if (bg && bg->ChkUsed()) {
            auto bgsp = dComIfG_Bgsp();
            bgsp->Release(p->mdBgW[i]);
        }
    }
    dComIfG_resDelete(&p->mPhs, STR(0x10029DFC));
    return TRUE;
}
/* 0235000C */
static BOOL Draw(daObjHami4_c *p) {
    settingTevStruct(dKy_getEnvlight(), 1, &p->current.pos, &p->tevStr);
    dComIfGd_setListBG();
    for (u32 i = 0; i < 4; ++i) {
        auto env = dKy_getEnvlight();
        setLightTevColorType(env, p->mpModels[i], &p->tevStr);
        mDoExt_modelUpdateDL(p->mpModels[i]);
    }
    dComIfGd_setList();
    return TRUE;
}
/* 023500C4 */
void daObjHami4_c::daObjHami4_close_stop() {
    u32 sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (dComIfGs_isSwitch(sw, home.roomNo)) {
        hami4_orderOtherEvent(this, STR(0x10029E84), 1, 0xFFFF);
        mState = DEMO_WAIT;
    }
}
/* 02350140 */
void daObjHami4_c::daObjHami4_open_demo_wait() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        mState = OPEN_DEMO;
        hami4_seStart(0x806);
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(0x69C7, &current.pos, 0, reverb);
    } else
        hami4_orderOtherEvent(this, STR(0x10029E90), 1, 0xFFFF);
}
/* 023501D4 */
void daObjHami4_c::daObjHami4_open_demo() {
    f32 next = mOpening + 10.0f;
    // HD: the fallthrough clamps an unordered NaN as well.
    if (next < 1500.0f) {
        mOpening = next;
        return;
    }
    mOpening = 1500.0f;
    mState = OPEN_STOP;
    auto vibration = dComIfGp_getVibration();
    gabi::Local<cXyz> direction;
    direction->z = 0.0f;
    direction->y = 1.0f;
    direction->x = 0.0f;
    vibration->StartShock(4, -0x21, direction);
    dComIfGp_event_reset();
}
/* 02350274 */
static BOOL Execute(daObjHami4_c *p) {
    u32 mState = p->mState;
    if (mState == daObjHami4_c::CLOSE_STOP)
        p->daObjHami4_close_stop();
    else if (mState == daObjHami4_c::DEMO_WAIT)
        p->daObjHami4_open_demo_wait();
    else if (mState == daObjHami4_c::OPEN_DEMO)
        p->daObjHami4_open_demo();
    p->set_mtx();
    return TRUE;
}
/* 02350308 */
static void sinit() {
    sinit_header_statics(0x10469D98, 0x101C9C5C);
}
/* 0235039C */
static void trivial_dt(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 023503B0 */
static BOOL IsDelete(daObjHami4_c *p) {
    return TRUE;
}
/* 023503B8 */
static void destroy(daObjHami4_c *p, s32 flags) {
    if (p) {
        hami4_actor_dt(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 0235040C */
static void open_stop(daObjHami4_c *p) {
}

daObjHami4_c::daObjHami4_c() {
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue
data l_openingSine 104A64F8
data l_openingCosine 104A64FC
ext 025D50BC void hami4_actor_dt(void*, s32)
ext 025D77DC void hami4_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16)
ext 025E1988 void hami4_seStart(u32)
*/
