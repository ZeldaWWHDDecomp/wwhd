/* Big Octo/Jabun whirlpool, WWHD implementation. */
#include "d/actor/d_a_obj_auzu.h"
using namespace daObjAuzu;
enum { dRes_INDEX_AUZU_BDL_AUZU_e = 4, dRes_INDEX_AUZU_BTK_AUZU_e = 7 };
enum {
    fpcNm_SHIP_e = 0xA5,
    fpcNm_KYTAG01_e = 0x180,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    JA_BGM_DIOCTA_BATTLE = 0x8000002B
};
static AuzuAttributes M_attr;
static f32 L_radius;
static AuzuHeaderZeros l_headerZeros;
static u8 l_registrationZ;
static u8 l_registrationA;
static u8 l_registrationB;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_initA;
static u8 l_initB;
namespace daObjAuzu {
/* 0231A62C */
static s32 parameter(Act_c *actor, s32 width, s32 shift) {
    u32 value = actor->mParameters, mask = (width & 32) ? 0 : (1u << (width & 31));
    return ((shift & 32) ? 0 : (value >> (shift & 31))) & (mask - 1);
}
/* 023197A0 */
static s32 parameter_copy(Act_c *actor, s32 width, s32 shift) {
    u32 value = actor->mParameters, mask = (width & 32) ? 0 : (1u << (width & 31));
    return ((shift & 32) ? 0 : (value >> (shift & 31))) & (mask - 1);
}
/* 023197BC */
static void color_to_float(FloatColor *output, u8 *input) {
    auto *color = reinterpret_cast<AuzuColor *>(input);
    f32 r = color->r, g = color->g, b = color->b, a = color->a;
    f32 rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f, af = a / 255.0f;
    output->r = rf;
    output->g = gf;
    output->b = bf;
    output->a = af;
}
/* 02319870 */
u32 Act_c::is_exist() {
    if (mType != 0)
        return true;
    return dComIfGs_isEventBit(0xA02) != 0;
}
/* 023198C8 */
bool Act_c::create_heap() {
    auto *modelData =
        (J3DModelData *)dComIfG_getObjectRes("Auzu", dRes_INDEX_AUZU_BDL_AUZU_e, 0x10025248);
    if (!modelData)
        JUT_ASSERT_fail(STR(0x10025290), 0xE2, STR(0x10025270));
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    auto *animation =
        (J3DAnmTextureSRTKey *)dComIfG_getObjectRes("Auzu", dRes_INDEX_AUZU_BTK_AUZU_e, 0x10025248);
    if (!animation)
        JUT_ASSERT_fail(STR(0x10025290), 0xEC, STR(0x10025280));
    f32 speed = attr().mAnmSpeed;
    BOOL result =
        mBtkAnm.init(modelData, animation, true, J3DFrameCtrl::EMode_LOOP, speed, 0, -1, false, 0);
    J3DModel *model = mpModel;
    if (!model || !result)
        return false;
    // HD: mark the model flags and update its shape packets.
    auto *hd = reinterpret_cast<AuzuModel *>(model);
    u32 flags = hd->mFlags & ~1u;
    hd->mFlags = flags;
    auzuSetModelFlags(model, flags);
    return true;
}
/* 023199F4 */
static BOOL solidHeapCB(fopAc_ac_c *base) {
    return static_cast<Act_c *>(base)->create_heap();
}
/* 023199F8 */
void Act_c::set_mtx() {
    auto *stack = mDoMtx_stack_c::get();
    PSMTXTrans(stack, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_ZXYrotM(stack, (s16)shape_angle.x, (s16)shape_angle.y, (s16)shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, stack);
}
/* 02319ABC */
void Act_c::init_mtx() {
    f32 y = scale.y, x = scale.x, mult = attr().mRadiusMult;
    J3DModel *model = mpModel;
    x = x * mult;
    f32 z = scale.z;
    model->mBaseScale.y = y;
    z = z * mult;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    set_mtx();
}
/* 02319AF8 */
void Act_c::set_state_map() {
    if (parameter(this, PRM_APPEAR_W, PRM_APPEAR_S) == 1)
        return;
    f32 factor = mScaleAnimFactor;
    u32 status = actor_status;
    actor_status = factor > 0.9990000128746033f ? status | fopAcStts_SHOWMAP_e
                                                : status & ~u32(fopAcStts_SHOWMAP_e);
}
/* 02319B70 */
s32 Act_c::_create() {
    mType = parameter(this, PRM_TYPE_W, PRM_TYPE_S);
    fopAcM_ct(this, Act_c);
    u32 exists = is_exist();
    mbIsExist = (u8)exists;
    if (!exists)
        return 5;
    s32 phase = dComIfG_resLoad(&mPhs, "Auzu");
    if (phase != 4)
        return phase;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0xD80))
        return 5;
    J3DModel *model = mpModel;
    fopAcM_SetMtx(this, model->getBaseTRMtx());
    init_mtx();
    f32 extent = 3000.0f * attr().mRadiusMult;
    fopAcM_setCullSizeBox(this, -extent, -30.0f, -extent, extent, 30.0f, extent);
    mbToAppear = 0;
    bool hidden = parameter(this, PRM_APPEAR_W, PRM_APPEAR_S) == 1;
    if (!hidden) {
        s32 sw = parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        hidden = fopAcM_isSwitch(this, sw) != 0;
    }
    mbBgmStarted = 0;
    mKytagPcId = 0xFFFFFFFF;
    mScaleAnimFactor = hidden ? 0.0f : 1.0f;
    set_state_map();
    return phase;
}
/* 02319D48 */
bool Act_c::_delete() {
    if (mbIsExist)
        dComIfG_resDelete(&mPhs, "Auzu");
    return true;
}
/* 02319D84 */
void Act_c::bgm_start() {
    if (mType == 0 && !mbBgmStarted) {
        mbBgmStarted = 1;
        auzuSubBgmStart(JA_BGM_DIOCTA_BATTLE);
    }
}
/* 02319DB4 */
void Act_c::ship_whirl() {
    auto *play = auzuPlay();
    AuzuShip *ship = play->mShip;
    if (!ship || ship->mProcName != fpcNm_SHIP_e)
        return;
    f32 distance = fopAcM_searchActorDistanceXZ2(this, reinterpret_cast<fopAc_ac_c *>(ship));
    const Attr_c &attrs = attr();
    f32 radius = L_radius;
    f32 outer = radius * (f32)attrs.mOuterActivationMult;
    f32 inner = radius * (f32)attrs.mInnerActivationMult;
    outer = outer * outer;
    inner = inner * inner;
    if (!(distance < outer))
        return;
    if (mScaleAnimFactor > 0.009999999776482582f) {
        bgm_start();
        u32 id = (u32)mBsPcId;
        s32 link = parameter(this, PRM_LINK_ID_W, PRM_LINK_ID_S);
        if (distance < inner) {
            ship->mWhirlID = id;
            ship->m03B2 = (u16)link;
            ship->m0352 = 1;
        } else {
            ship->m03B2 = (u16)link;
            ship->mWhirlID = id;
            ship->m0352 = 0;
        }
    } else {
        ship->mWhirlActor = 0;
        ship->mWhirlID = fpcM_ERROR_PROCESS_ID_e;
    }
}
/* 02319F4C */
bool Act_c::_execute() {
    set_mtx();
    auto *play = auzuPlay();
    if (play->mPlayerStatus & daPyStts0_SHIP_RIDE_e)
        ship_whirl();
    else if (mScaleAnimFactor > 0.009999999776482582f) {
        play = auzuPlay();
        AuzuPlayer *player = play->mPlayer;
        fopAcM_searchActorDistanceXZ2(this, reinterpret_cast<fopAc_ac_c *>(player));
        f32 distance = fopAcM_searchActorDistanceXZ2(this, reinterpret_cast<fopAc_ac_c *>(player));
        f32 radius = (f32)L_radius * attr().mOuterActivationMult;
        radius = radius * radius;
        if (distance < radius)
            player->mWhirlId = mBsPcId;
    }
    mDoExt_baseAnm_play(&mBtkAnm);
    f32 target = 0.0f;
    if (parameter(this, PRM_APPEAR_W, PRM_APPEAR_S) == 1) {
        if (mbToAppear)
            target = 1.0f;
    } else {
        s32 sw = parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        if (!fopAcM_isSwitch(this, sw))
            target = 1.0f;
    }
    cLib_chaseF(&mScaleAnimFactor, target, 0.019999999552965164f);
    s8 soundParam = (s8)gabi::ftoi((f32)mScaleAnimFactor * 100.0f);
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(0x1068, &eyePos, soundParam, reverb);
    f32 factor = mScaleAnimFactor;
    if (factor > 0.0f) {
        u32 tag = mKytagPcId;
        if (tag == 0xFFFFFFFF) {
            f32 radius = (f32)L_radius;
            f32 mult = attr().mRadiusMult;
            f32 inner = radius * mult, outer = (radius + 500.0f) * mult;
            gabi::Local<cXyz> size;
            size->x = (inner * factor) / 5000.0f;
            size->z = (outer * factor) / 5000.0f;
            // HD: GHS intentionally leaves the Y slot unspecified.
            s8 room = tevStr.mRoomNo;
            mKytagPcId = fopAcM_create(fpcNm_KYTAG01_e, -1, &current.pos, room, &current.angle,
                                       size.get(), -1, 0);
        } else {
            auto *found = reinterpret_cast<AuzuWaveTag *>(fopAcM_SearchByID(tag));
            if (found) {
                f32 inner = (f32)L_radius * attr().mRadiusMult;
                inner = inner * (f32)mScaleAnimFactor;
                found->mInnerRadius = inner;
                f32 outer = ((f32)L_radius + 500.0f) * attr().mRadiusMult;
                outer = outer * (f32)mScaleAnimFactor;
                found->mOuterRadius = outer;
            }
        }
    } else if (factor == 0.0f && mKytagPcId != 0xFFFFFFFF) {
        auto *found = fopAcM_SearchByID(mKytagPcId);
        if (found)
            fopAcM_delete(found);
    }
    set_state_map();
    return true;
}
/* 0231A2D0 */
static void set_material(u8 *material, u32 alpha) {
    auto *mat = reinterpret_cast<AuzuMaterial *>(material);
    while (mat) {
        J3DShape *shape = mat->mShape;
        shape->mVisible = alpha ? 1 : 0;
        if (alpha) {
            // HD: update both the TEV color and the material's float uniform.
            AuzuTevBlock *tev = mat->mTevBlock;
            AuzuColor *color = tev->getTevKColor(3);
            color->a = (u8)alpha;
            tev = mat->mTevBlock;
            color = tev->getTevKColor(3);
            tev = mat->mTevBlock;
            tev->setTevKColor(3, color);
            gabi::Local<FloatColor> input, output;
            color_to_float(input.get(), reinterpret_cast<u8 *>(color));
            auzuConvertColor(output.get(), input.get(), 1.0f);
            mat->mUniform.mFlags = mat->mUniform.mFlags | 0x400;
            FloatColor *dest = auzuGetUniform(&mat->mUniform, 10);
            f32 opacity = color->a, r = output->r, g = output->g, b = output->b;
            dest->r = r;
            dest->g = g;
            dest->b = b;
            dest->a = opacity / 255.0f;
        }
        mat = mat->mNext;
    }
}
/* 0231A470 */
bool Act_c::_draw() {
    auto *env = dKy_getEnvlight();
    settingTevStruct(env, 2, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    J3DModel *model = mpModel;
    setLightTevColorType(env, model, &tevStr);
    model = mpModel;
    J3DModelData *data = model->mModelData;
    f32 frame = mBtkAnm.mFrameCtrl.mFrame;
    mBtkAnm.entry(data, frame);
    u8 alpha = (u8)gabi::ftoi(255.5f * (f32)mScaleAnimFactor);
    model = mpModel;
    data = model->mModelData;
    AuzuJoint *joint = reinterpret_cast<AuzuModelData *>(data)->mJointNodes;
    set_material(reinterpret_cast<u8 *>(static_cast<AuzuMaterial *>(joint->mMesh)), alpha);
    model = mpModel;
    mDoExt_modelUpdateDL(model, 0);
    return true;
}
/* 0231A514 */
static s32 create(Act_c *actor) {
    return actor->_create();
}
/* 0231A518 */
static bool remove(Act_c *actor) {
    return actor->_delete();
}
/* 0231A51C */
static bool execute(Act_c *actor) {
    return actor->_execute();
}
/* 0231A520 */
static bool draw(Act_c *actor) {
    return actor->_draw();
}
/* 0231A624 */
static BOOL is_delete(Act_c *actor) {
    return TRUE;
}
/* 0231974C */
static void base_destructor(Act_c *actor, s32 flags) {
    if (actor) {
        auzuActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0231A5D0 */
static void actor_destructor(Act_c *actor, s32 flags) {
    if (actor) {
        auzuActorDt(actor, 0);
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 0231A5B8 */
static void empty_destructor(u8 *object, s32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 0231A5CC */
static void empty_inline(Act_c *actor) {
}
/* 0231A524 */
static void static_init() {
    // HD: preserve Z storage initialization before the P angle constants.
    l_headerZeros.words[2] = 0;
    l_headerZeros.words[0] = 0;
    l_headerZeros.words[3] = 0;
    l_headerZeros.words[1] = 0;
    __register_global_object(gabi::ea(&l_registrationZ));
    l_minAngle = -3.1415927410125732f;
    l_maxAngle = 3.1415927410125732f;
    auzuInitA(&l_initA);
    __register_global_object(gabi::ea(&l_registrationA));
    auzuInitB(&l_initB);
    __register_global_object(gabi::ea(&l_registrationB));
}
} // namespace daObjAuzu

using namespace daObjAuzu;
const Attr_c &Act_c::attr() const {
    return M_attr.values[(s32)mType];
}
Act_c::Act_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x10025260;
    mDoExt_btkAnm::ct(&mBtkAnm);
}
/* WWHD glue
str 100252D8 "Auzu"
data M_attr 100252E0
data L_radius 101C7D0C
data l_headerZeros 10469130
data l_registrationZ 101C7CE8
data l_minAngle 10469124
data l_maxAngle 10469128
data l_initA 1046912C
data l_initB 1046912D
data l_registrationA 101C7CF4
data l_registrationB 101C7D00
ext 027F596C void auzuSetModelFlags(J3DModel*,u32)
ext 025E1918 void auzuSubBgmStart(u32)
ext 0274D458 void auzuConvertColor(FloatColor*,FloatColor*,f32)
ext 027F9F0C FloatColor* auzuGetUniform(AuzuUniform*,u32)
ext 028ED6F8 void auzuInitA(void*)
ext 028EAB2C void auzuInitB(void*)
ext 025D50BC void auzuActorDt(void*,s32)
*/
