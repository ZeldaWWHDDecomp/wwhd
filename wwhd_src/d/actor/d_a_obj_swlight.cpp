/* Light-activated moon/sun switch, using the GameCube names and actor layout.
 * HD uses expanded animation caches and shader uniforms; marked functions preserve
 * the verified HD call and store order. */
#include "d/actor/d_a_obj_swlight.h"
using daObjSwlight::Act_c;
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/d/d_kankyo.h"
const gc_tu_data gc_tu = {.safeString = 0x1002FE18};
const char Act_c::M_arcname[] = "Mmirror";
static dCcD_SrcTri M_tri_src;
static void *stts_ct(void *);
static void *tri_ct(void *);
static daObjSwlight::ModeFunc mode_proc[4] = {};
static BOOL jnodeCB_moon(J3DNode *, s32);
static u32 PrmAbstract(fopAc_ac_c *, u32, u32);
static u32 eyeInitialized;
static cXyz local_eye;
static f32 collisionZero;
static f32 collisionDepth;
static f32 collisionSize;
static f32 collisionSine;
static f32 collisionCosine;
static u32 staticHeader[4] = {};
static u32 staticRegistration0[3] = {};
static u32 staticRegistration1[3] = {};
static u32 staticRegistration2[3] = {};
static f32 minimumPi;
static f32 maximumPi;
static u8 staticCharacter;
static u8 staticByte;
static void sound(Act_c *a, u32 id) {
    s32 rev = swlightGetReverb((s8)a->current.roomNo);
    mDoAud_seStart(id, &a->eyePos, 0, rev);
}
static s32 prm(Act_c *a, s32 width, s32 shift) { return (s32)PrmAbstract(a, width, shift); }
static bool sw(Act_c *a) {
    s32 n = prm(a, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    dSv_info_c *save = dComIfGs_info();
    s8 room = a->home.roomNo;
    return swlightIsSwitch(save, n, room) != 0;
}
static void switch_write(Act_c *a, bool on) {
    s32 n = prm(a, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    dSv_info_c *save = dComIfGs_info();
    s8 room = a->home.roomNo;
    if (on)
        swlightOnSwitch(save, n, room);
    else
        swlightOffSwitch(save, n, room);
}
// HD: shader uniforms replace the GameCube blend/depth material writes.
static void set_material_color(SwlightMaterial *material, u32 alpha, bool opaque) {
    material->mShape->show();
    SwlightResourceHeader *data = material->mData;
    u32 rel = gabi::ea(&data->mMaterialMode);
    u32 offset = data->mMaterialMode;
    swlightSetMaterialMode(gabi::at<void>(offset ? rel + offset : 0), opaque ? 1 : 3);
    SwlightTevBlock *block = material->mTevBlock;
    SwlightTevVtable *vtable = block->mVtable;
    SwlightRgba *color = gabi::call_ptr<SwlightRgba *>(vtable->getKonstColor.get(), block, 3);
    color->a = (u8)alpha;
    block = material->mTevBlock;
    vtable = block->mVtable;
    color = gabi::call_ptr<SwlightRgba *>(vtable->getKonstColor.get(), block, 3);
    block = material->mTevBlock;
    vtable = block->mVtable;
    gabi::call_ptr<void>(vtable->setKonstColor.get(), block, 3, color);
    gabi::Local<SwlightFloatColor> rgba, linear;
    swlightNormalizeColor(rgba.get(), color);
    swlightConvertColor(linear.get(), rgba.get(), 1.f);
    u32 flags = material->mFlags;
    material->mFlags = flags | 0x400;
    SwlightFloatColor *param = swlightGetUniform(&material->mFlags, 10);
    f32 alphaF = (f32)color->a / 255.f;
    f32 z = linear->b, y = linear->g, x = linear->r;
    param->g = y;
    param->b = z;
    param->r = x;
    param->a = alphaF;
}
static SwlightMaterial *joint_mesh(J3DModelData *data, u32 index) {
    SwlightJoint *joint = reinterpret_cast<SwlightJoint *>(data->getJointNodePointer(index));
    return joint->mMesh;
}
/* 02395850 */
bool Act_c::is_switch2() {
    s32 n = prm(this, Act_c::PRM_SWSAVE2_W, Act_c::PRM_SWSAVE2_S);
    if (n == 0xFF)
        return false;
    dSv_info_c *save = dComIfGs_info();
    s8 room = home.roomNo;
    return swlightIsSwitch(save, n, room) != 0;
}
/* 02395560 */
static BOOL solidHeapCB(fopAc_ac_c *i_this) {
    // HD: retain the full return register from the callback target.
    return swlightCreateHeapResult(static_cast<Act_c *>(i_this));
}

/* 02395298 */
bool Act_c::create_heap() {
    J3DModelData *mdl =
        (J3DModelData *)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_BDL_MSUSW_e);
    if (!mdl)
        JUTAssertion::showAssert("d_a_obj_swlight.cpp", 0x13D, "modelData != 0");
    m298 = mDoExt_J3DModel__create(mdl, 0x80000, 0x31000202);
    if (m298) {
        for (u32 i = 1; i <= 5; i++) {
            mdl->getJointNodePointer(i)->setCallBack(jnodeCB_moon);
        }
        m298->setUserArea(gabi::ea(this));
    }
    J3DAnmTextureSRTKey *btk = static_cast<J3DAnmTextureSRTKey *>(
        dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_BTK_MSUSW_e));
    if (!btk)
        JUTAssertion::showAssert("d_a_obj_swlight.cpp", 0x158, "btk != 0");
    s32 okbtk = m29C.init(mdl, btk, true, 2, 1.f, 0, -1, false, 0);
    J3DAnmTransform *bck = static_cast<J3DAnmTransform *>(
        dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_BCK_MSUSW_e));
    if (!bck)
        JUTAssertion::showAssert("d_a_obj_swlight.cpp", 0x15F, "bck != 0");
    s32 okbck = m2B0.init(mdl, bck, true, 2, 1.f, 0, -1, false);
    bool okbg = false;
    m2C0 = swlightNewBackground(nullptr);
    if (m2C0) {
        void *bgdata = dComIfG_getObjectRes(M_arcname, dRes_INDEX_MMIRROR_DZB_MSUSW_e);
        if (!bgdata)
            JUTAssertion::showAssert("d_a_obj_swlight.cpp", 0x169, "bgw_data != 0");
        okbg = swlightSetBackground(m2C0, static_cast<cBgD_t *>(bgdata), 1, &mF38) == 0;
    }
    bool ok = m298 != nullptr && okbtk && okbck && okbg;
    if (!ok)
        m2C0 = nullptr;
    return ok;
}
/* 023958E8 */
s32 Act_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = SWLIGHT_ACTOR_VTABLE;
            mDoExt_btkAnm::ct(&m29C);
            swlightConstructFrame(&m2B0, 0);
            auto *cache = reinterpret_cast<SwlightBckCache *>(&m2B0);
            cache->mVtable = SWLIGHT_ANM_BASE_VTABLE;
            swlightConstructCache(cache->mTransformCache);
            cache->mCacheData = 0;
            cache->mFlags = 0;
            cache->mResource = 0;
            cache->mData = 0;
            cache->mCacheVtable = SWLIGHT_CACHE_VTABLE;
            cache->mCacheCount = 0;
            cache->mVtable = SWLIGHT_BCK_VTABLE;
            swlightConstructArray(m2C4, 8, sizeof(dCcD_Tri), tri_ct);
            swlightConstructArray(mD44, 8, sizeof(dCcD_Stts), stts_ct);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    mF24 = prm(this, Act_c::PRM_TYPE_W, Act_c::PRM_TYPE_S);
    s32 phase = dComIfG_resLoad(&mPhase, M_arcname);
    if (phase == 4) {
        mDoMtx_stack_c::transS((f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
        mDoMtx_stack_c::ZXYrotM(shape_angle);
        mDoMtx_stack_c::scaleM((f32)scale.x, (f32)scale.y, (f32)scale.z);
        MTXCopy(mDoMtx_stack_c::get(), &mF38);
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x1F40))
            return 5;
        dComIfG_Bgsp()->Regist(m2C0, this);
        reinterpret_cast<SwlightBackground *>(m2C0.get())->mCorrect.v = 0;
        fopAcM_SetMtx(this, J3DModel_getBaseTRMtx(m298));
        init_mtx();
        swlightSetCullSphere(this, 0.f, 0.f, 0.f, 240.f);
        init_cc();
        init_eye_pos();
        if (mF24 == 0) {
            if (sw(this)) {
                mF30 = 1.f;
                mode_norm_sun_init();
            } else {
                mF30 = 0.f;
                mode_norm_moon_init();
            }
        } else {
            // HD: this type requires both switches and clears the primary switch otherwise.
            bool active = false;
            if (is_switch2())
                active = sw(this);
            if (active) {
                mF30 = 1.f;
                mode_active_sun_init();
            } else {
                if (sw(this))
                    switch_write(this, false);
                mF30 = 0.f;
                mode_active_moon_init();
            }
        }
    }
    return phase;
}

/* 02395C40 */
bool Act_c::_delete() {
    dBgW *bg = m2C0;
    if (bg && bg->ChkUsed())
        dComIfG_Bgsp()->Release(m2C0);
    dComIfG_resDeleteDemo(&mPhase, M_arcname);
    return true;
}
/* 02395290 */
static BOOL jnodeCB_moon(J3DNode *node, s32 stage) { return TRUE; }
/* 02395564 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS((f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    Mtx34 *m = &m298->mBaseTransformMtx;
    mtx_copy(m, mDoMtx_stack_c::get());
}
/* 02395628 */
void Act_c::init_mtx() {
    J3DModel *m = m298;
    f32 y = scale.y, x = scale.x, z = scale.z;
    m->mBaseScale.x = x;
    m->mBaseScale.y = y;
    m->mBaseScale.z = z;
    set_mtx();
}
/* 02395758 */
void Act_c::init_cc() {
    for (int i = 0; i < 8; i++) {
        mD44[i].Init(0xFF, 0xFF, this);
        swlightSetTriangle(&m2C4[i], &M_tri_src);
        u32 flag = m2C4[i].mGObjTg.mSPrm;
        m2C4[i].SetStts(&mD44[i]);
        m2C4[i].mGObjTg.mSPrm = flag | 4;
    }
    set_cc_pos();
}
/* 02395648 */
void Act_c::set_cc_pos() {
    gabi::Local<cXyz> p0, p1, p2, o0, o1, o2;
    f32 zero = collisionZero;
    p0->set(zero, zero, collisionDepth);
    f32 size = collisionSize;
    f32 x = size * collisionSine;
    f32 y = size * collisionCosine;
    p1->set(zero, size, zero);
    p2->set(x, y, zero);
    for (int i = 0; i < 8; i++) {
        MTXCopy(J3DModel_getBaseTRMtx(m298), mDoMtx_stack_c::get());
        mDoMtx_stack_c::ZrotM((s16)(i * 8192));
        PSMTXMultVec(mDoMtx_stack_c::get(), p0.get(), o0.get());
        PSMTXMultVec(mDoMtx_stack_c::get(), p1.get(), o1.get());
        PSMTXMultVec(mDoMtx_stack_c::get(), p2.get(), o2.get());
        swlightSetTrianglePos(&reinterpret_cast<SwlightTriShape *>(&m2C4[i])->mTriangle, o0.get(),
                              o1.get(), o2.get());
    }
}
/* 0239620C */
void Act_c::set_cc() {
    for (int i = 0; i < 8; i++) {
        swlightRegisterTriangle(dComIfG_Ccsp(), &m2C4[i]);
    }
}
/* 023957D8 */
void Act_c::init_eye_pos() {
    if (eyeInitialized == 0) {
        local_eye.x = 0.f;
        local_eye.z = 40.f;
        local_eye.y = 0.f;
        eyeInitialized = 1;
    }
    PSMTXMultVec(J3DModel_getBaseTRMtx(m298), &local_eye, &eyePos);
}
/* 0239625C */
bool Act_c::chk_light() {
    dComIfG_inf_c *game = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    if (swlightDetectLight(&game->play.mDetect, &eyePos))
        return true;
    bool hit = false;
    for (int i = 0; i < 8; i++) {
        if (m2C4[i].ChkTgHit()) {
            m2C4[i].ClrTgHit();
            hit = true;
        }
    }
    return hit;
}
/* 023962F8 */
bool Act_c::power_up() { return swlightChaseF(&mF30, 1.f, 0.033333335f) != 0; }
/* 02396334 */
bool Act_c::power_down() { return swlightChaseF(&mF30, 0.f, 0.033333335f) != 0; }
/* 0239583C */
void Act_c::mode_norm_moon_init() {
    mF28 = MODE_NORM_MOON;
    mF2C = 20;
}
/* 02396370 */
void Act_c::mode_norm_moon() {
    bool lit = chk_light();
    if (lit) {
        if (mF2C > 0)
            mF2C = (s16)mF2C - 1;
    } else
        mF2C = 20;
    if (sw(this) || mF2C == 0) {
        if (power_up()) {
            sound(this, JA_SE_OBJ_LIGHT_SW_ON);
            switch_write(this, true);
            mode_norm_sun_init();
            return;
        }
    } else
        power_down();
    set_cc();
}
/* 02395830 */
void Act_c::mode_norm_sun_init() { mF28 = MODE_NORM_SUN; }
/* 023964D4 */
void Act_c::mode_norm_sun() {
    if (sw(this)) {
        power_up();
        sound(this, JA_SE_OBJ_LIGHT_SW_SUNSHINE);
    } else if (power_down())
        mode_norm_moon_init();
}
/* 023958D4 */
void Act_c::mode_active_moon_init() {
    mF28 = MODE_ACTIVE_MOON;
    mF2C = 20;
}
/* 0239657C */
void Act_c::mode_active_moon() {
    bool light = chk_light();
    s16 timer;
    if (light) {
        timer = mF2C;
        if (timer > 0) {
            timer--;
            mF2C = timer;
        }
    } else {
        mF2C = 20;
        timer = 20;
    }
    if (timer == 0 && power_up()) {
        sound(this, JA_SE_OBJ_LIGHT_SW_ON);
        switch_write(this, true);
        mode_active_sun_init();
    } else if (timer != 0)
        power_down();
    set_cc();
}
/* 023958C8 */
void Act_c::mode_active_sun_init() { mF28 = MODE_ACTIVE_SUN; }
/* 0239666C */
void Act_c::mode_active_sun() {
    if ((f32)mF30 > 0.5f)
        sound(this, JA_SE_OBJ_LIGHT_SW_SUNSHINE);
    if (is_switch2() || chk_light())
        power_up();
    else if (power_down()) {
        switch_write(this, false);
        mode_active_moon_init();
    }
    set_cc();
}
/* 02395CA4 */
bool Act_c::_execute() {
    (this->*mode_proc[(u32)mF28])();
    m29C.play();
    m2B0.play();
    set_mtx();
    return true;
}
/* 02395D54 */
static void setMaterial(void *material, u32 alpha) {
    SwlightMaterial *mat = static_cast<SwlightMaterial *>(material);
    while (gabi::ea(mat) != 0) {
        if (alpha == 0)
            mat->mShape->hide();
        else
            set_material_color(mat, alpha, alpha == 255);
        mat = mat->mNext;
    }
}

/* 02395ED4 */
bool Act_c::_draw() {
    // HD: submit each joint material before updating the whole model.
    dScnKy_env_light_c *env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, (J3DModel *)m298, &tevStr);
    J3DModel *model = m298;
    f32 frame = m29C.getFrame();
    m29C.entry(model->getModelData(), frame);
    model = m298;
    frame = m2B0.getFrame();
    m2B0.entry(model->getModelData(), frame);
    dComIfGd_setListBG();
    u8 alpha = (u8)gabi::ftoi(255.f * (f32)mF30);
    J3DModelData *data = m298->getModelData();
    for (u32 i = 1; i <= 3; i++) {
        SwlightMaterial *material = joint_mesh(data, i);
        setMaterial(material, (u32)alpha);
        swlightMaterialEntry(m298, material, alpha < 255 ? 1 : 0);
    }
    alpha = (u8)(255 - alpha);
    SwlightMaterial *material = joint_mesh(data, 4);
    setMaterial(material, (u32)alpha);
    swlightMaterialEntry(m298, material, alpha < 255 ? 1 : 0);
    material = joint_mesh(data, 5);
    SwlightMaterial *mat = material;
    if (alpha) {
        while (mat) {
            set_material_color(mat, alpha, false);
            mat = mat->mNext;
        }
    } else {
        while (mat) {
            mat->mShape->hide();
            mat = mat->mNext;
        }
    }
    swlightMaterialEntry(m298, material, 1);
    mDoExt_modelUpdateDL(m298);
    dComIfGd_setList();
    return true;
}
/* 02396748 */
static s32 Mthd_Create(Act_c *a) { return a->_create(); }
/* 0239674C */
static BOOL Mthd_Delete(Act_c *a) {
    // HD: GHS forwards the full return register without bool normalization.
    return swlightDeleteResult(a);
}
/* 02396750 */
static BOOL Mthd_Execute(Act_c *a) {
    // HD: GHS forwards the full return register without bool normalization.
    return swlightExecuteResult(a);
}
/* 02396754 */
static BOOL Mthd_Draw(Act_c *a) {
    // HD: GHS forwards the full return register without bool normalization.
    return swlightDrawResult(a);
}
/* 02396998 */
static BOOL Mthd_IsDelete(void *a) { return TRUE; }
// HD: compiler-generated and template code of this translation unit.
/* 02396758 */
static void static_init() {
    staticHeader[2] = 0;
    staticHeader[0] = 0;
    staticHeader[3] = 0;
    staticHeader[1] = 0;
    swlightRegister(staticRegistration0);
    minimumPi = -3.1415927410125732f;
    maximumPi = 3.1415927410125732f;
    swlightInitChar(&staticCharacter);
    swlightRegister(staticRegistration1);
    swlightInitByte(&staticByte);
    swlightRegister(staticRegistration2);
}
/* 023967EC */
static void trivial_dt(void *self, s32 flags) {
    if (self && (flags & 1))
        swlightFree(self);
}
/* 02396800 */
static void *stts_ct(void *self) {
    if (!self)
        self = swlightAllocate(0x3C);
    if (self) {
        dCcD_Stts_ct(static_cast<dCcD_Stts *>(self));
    }
    return self;
}
/* 02396868 */
static void *tri_ct(void *self) {
    if (!self)
        self = swlightAllocate(0x150);
    if (self) {
        auto *object = static_cast<dCcD_Tri *>(self);
        auto *shape = static_cast<SwlightTriShape *>(self);
        swlightConstructGObj(object);
        shape->mShapeVtable = SWLIGHT_SHAPE_BASE_VTABLE;
        shape->mAabVtable = SWLIGHT_AAB_VTABLE;
        swlightConstructTriangle(&shape->mTriangle);
        object->__vtbl_hitinf = SWLIGHT_TRI_VTABLE;
        shape->mTriangle.mVtable = SWLIGHT_TRI_GEOMETRY_VTABLE;
        shape->mShapeVtable = SWLIGHT_TRI_SHAPE_VTABLE;
    }
    return self;
}
/* 023968F4 */
static void empty_virtual(void *self) {}
/* 023968F8 */
static void actor_dt(Act_c *a, s32 flags) {
    if (a) {
        swlightDestroyArray(a->mD44, 8, sizeof(dCcD_Stts), SWLIGHT_STTS_DESTRUCTOR, 0, 0);
        swlightDestroyArray(a->m2C4, 8, sizeof(dCcD_Tri), SWLIGHT_TRI_DESTRUCTOR, 0, 0);
        swlightDestroyBckCache(&reinterpret_cast<SwlightBckCache *>(&a->m2B0)->mVtable, 0);
        swlightDestroyActor(a, 0);
        if (flags & 1)
            swlightFree(a);
    }
}
/* 023969A0 */
static u32 PrmAbstract(fopAc_ac_c *a, u32 width, u32 shift) {
    u32 p = fopAcM_GetParam(a);
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 value = (shift & 32) ? 0 : (p >> (shift & 31));
    return value & (mask - 1);
}
/* WWHD glue (tools/verify/autoglue.py)
ext 027E212C void swlightSetMaterialMode(void*, u32)
ext 023951DC void swlightNormalizeColor(SwlightFloatColor*, SwlightRgba*)
ext 0274D458 void swlightConvertColor(SwlightFloatColor*, SwlightFloatColor*, f32)
ext 027F9F0C SwlightFloatColor* swlightGetUniform(be<u32>*, s32)
ext 027F591C void swlightMaterialEntry(J3DModel*, SwlightMaterial*, u32)
data staticHeader 1046C030 nocheck
data staticRegistration0 101CCF9C nocheck
data staticRegistration1 101CCFA8 nocheck
data staticRegistration2 101CCFB4 nocheck
data minimumPi 1046C024 nocheck
data maximumPi 1046C028 nocheck
data staticCharacter 1046C02C nocheck
data staticByte 1046C02D nocheck
ext 028F026C void swlightRegister(be<u32>*)
ext 028ED6F8 void swlightInitChar(be<u8>*)
ext 028EAB2C void swlightInitByte(be<u8>*)
ext 0273AF40 void swlightFree(void*)
ext 0273AD10 void* swlightAllocate(u32)
ext 02515FB8 void swlightConstructGObj(dCcD_Tri*)
ext 02019040 void swlightConstructTriangle(SwlightTriangle*)
ext 028F0164 void swlightDestroyArray(void*, u32, u32, u32, u32, u32)
ext 027F3628 void swlightDestroyBckCache(be<u32>*, s32)
ext 025D50BC void swlightDestroyActor(fopAc_ac_c*, s32)
ext 02395298 BOOL swlightCreateHeapResult(daObjSwlight::Act_c*)
ext 02395C40 BOOL swlightDeleteResult(daObjSwlight::Act_c*)
ext 02395CA4 BOOL swlightExecuteResult(daObjSwlight::Act_c*)
ext 02395ED4 BOOL swlightDrawResult(daObjSwlight::Act_c*)
ext 025D6768 void swlightSetCullSphere(fopAc_ac_c*, f32, f32, f32, f32)
data mode_proc 1002FED4 nocheck
ext 0200E240 void swlightRegisterTriangle(cCcS*, dCcD_Tri*)
ext 024F23F4 dBgW* swlightNewBackground(dBgW*)
ext 0200A030 s32 swlightSetBackground(dBgW*, cBgD_t*, u32, Mtx34*)
ext 027F2BC0 void swlightConstructFrame(mDoExt_bckAnm*, s32)
ext 027DA984 void swlightConstructCache(void*)
ext 028EFFD0 void swlightConstructArray(void*, u32, u32, SwlightConstructor)
data M_tri_src 1002FF10 nocheck
data eyeInitialized 1046C04C nocheck
data local_eye 1046C040 nocheck
data collisionZero 1002FEC4 nocheck
data collisionDepth 1002FEC8 nocheck
data collisionSize 1002FECC nocheck
data collisionSine 104A64F8 nocheck
data collisionCosine 104A64FC nocheck
ext 02520540 s32 swlightGetReverb(s32)
ext 025BA0C0 s32 swlightIsSwitch(dSv_info_c*, s32, s32)
ext 025B9E38 void swlightOnSwitch(dSv_info_c*, s32, s32)
ext 025B9F7C void swlightOffSwitch(dSv_info_c*, s32, s32)
ext 0251650C void swlightSetTriangle(dCcD_Tri*, const dCcD_SrcTri*)
ext 0252A038 s32 swlightDetectLight(dDetect_c*, const cXyz*)
ext 0200F5C8 s32 swlightChaseF(be<f32>*, f32, f32)
ext 0201924C void swlightSetTrianglePos(SwlightTriangle*, cXyz*, cXyz*, cXyz*)
str 1002FF08
str 1002FE78
str 1002FE9C
str 1002FEAC
str 1002FEB8
str 1002FE8C
*/
