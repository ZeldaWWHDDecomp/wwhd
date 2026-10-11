/* Enemy life ball. */
#include "d/actor/d_a_ib.h"
const gc_tu_data gc_tu = {.safeString = 0x100118C4};
struct CylinderSource {
    be<u32> words[17];
};
static CylinderSource m_cyl_src;
struct IbShape {
    u8 _00[0x18];
    be<u32> __vtbl_base, __vtbl_shape;
};
struct GroundStorage {
    dBgS_GndChk value;
};
struct ModeTable {
    daIball_c::ModeFunc entries[2];
};
static ModeTable mode_proc;
struct IbData {
    u8 m00, m01, m02, m03;
    be<f32> mPlaySpeeds[4];
};
static IbData m_data;
struct LightColors {
    be<s16> values[3][3];
};
static LightColors light_color;
struct LifeBallCallback {
    be<u32> words[2];
};
struct LifeBallCallbacks {
    LifeBallCallback entries[3];
};
static LifeBallCallbacks lifeBallCallbacks;
struct FloatValue {
    be<f32> value;
};
struct WordValue {
    be<u32> value;
};
struct ByteValue {
    be<u8> value;
};
struct Record {
    be<u32> words[3];
};
static FloatValue radiusBias;
static FloatValue minimum;
static FloatValue maximum;
static FloatValue minimumConstant;
static FloatValue maximumConstant;
static WordValue zero0;
static WordValue zero1;
static WordValue zero2;
static WordValue zero3;
static ByteValue flag0;
static ByteValue flag1;
static Record record0;
static Record record1;
static Record record2;
static cXyz zeroScale;
static csXyz zeroAngles;
struct GuestText {
    be<u8> bytes[64];
};
static GuestText arc;
static GuestText file;
static GuestText modelAssertText;
static GuestText bckAssertText;
static GuestText btkAssertText;
static GuestText brkAssertText;
static const char *text(const GuestText &s) { return reinterpret_cast<const char *>(&s); }
static const char *modelAssertion() { return text(modelAssertText); }
static const char *bckAssertion() { return text(bckAssertText); }
static const char *btkAssertion() { return text(btkAssertText); }
static const char *brkAssertion() { return text(brkAssertText); }
static void ibAssert(s32 line, const char *expression) { JUT_ASSERT_fail(text(file), line, expression); }
// HD: the resource wrapper retains both guest locals and their linkage area.
extern void *getResource(dRes_control_c *, SafeString *, s32);
static void *resource(s32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::ea(&arc);
    name->__vtbl = 0x100118C4;
    gabi::Local<u8[16]> linkage;
    return getResource(dComIfG_resControl(), name.get(), index);
}
// HD: bit copies retain the original floating-point payloads.
static be<u32> &word(be<f32> &v) { return *reinterpret_cast<be<u32> *>(&v); }
static s32 selectColor(f32 frame) {
    s32 color = 2;
    if (frame < 45) {
        if (!(frame < 0))
            color = 0;
        else if (!(frame < 90)) {
            if (!(frame > 135))
                color = 2;
        }
    } else if (frame < 90)
        color = 1;
    else if (!(frame > 135))
        color = 2;
    return color;
}
extern void seStartLevel(u32, const cXyz *, u32, s32);
static void sound(daIball_c *actor, u32 id) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    seStartLevel(id, &actor->current.pos, 0, reverb);
}
static void ibParticle(u16 id, cXyz *position, cXyz *scale, LifeBallCallback *callback) {
    dComIfGp_particle_set(id, position, nullptr, scale, 255, reinterpret_cast<dPa_levelEcallBack *>(callback), -1,
                          nullptr, nullptr, nullptr);
}
// HD: private play view for the character table missing from the completed accessor API.
struct IbCharacterTable {
    u8 _00[0x70];
};
struct IbPlay {
    u8 _00[0x50A0];
    IbCharacterTable mCharacterTable;
    be<u32> mItemEntries[8];
};
static IbPlay *ibPlay() { return reinterpret_cast<IbPlay *>(dComIfGp_get()); }
enum { IB_BREAK_ATTACK_TYPES = 0x101C4040u | 0x07008000u | 0x00010482u };
static daIball_c *ibConstructor(daIball_c *);
static BOOL CheckCreateHeap(fopAc_ac_c *);
static BOOL itemParamSet_CB();
extern BOOL initBck(J3DFrameCtrl *, J3DModelData *, J3DAnmTransform *, u32, u32, f32, s32, s32, u32);
extern BOOL initBtk(mDoExt_btkAnm *, J3DModelData *, J3DAnmTextureSRTKey *, u32, u32, f32, s32, s32, u32, u32);
extern BOOL initBrk(mDoExt_brkAnm *, J3DModelData *, J3DAnmTevRegKey *, u32, u32, f32, s32, s32, u32, u32);
extern void constructAcch(dBgS_ObjAcch *);
extern void constructStatus(dCcD_Stts *);
extern void constructGStatus(void *);
extern void constructObjectInfo(dCcD_GObjInf *);
extern void constructCylinder(cM3dGCyl *);
extern void constructArray(void *, u32, u32, u32);
extern void constructBck(J3DFrameCtrl *, u32);
extern void constructRippleAnimation(void *);
extern void constructRippleTail(IbRippleTail *);
extern void rotateXYZ(Mtx34 *, s16, s16, s16);
extern void addCalc(be<f32> *, f32, f32, f32);
extern void entryBck(J3DFrameCtrl *, J3DModelData *, f32);
extern void entryBrkModel(mDoExt_brkAnm *, J3DModel *, f32);
extern void modelCalc(J3DModel *);
extern void ibRegist(daIball_c *);
extern void ibRemove(daIball_c *);
extern void modelUpdateDL(J3DModel *, u32);
extern void removeRippleTail(IbRippleTail *);
extern void playAnimation(void *);
extern void createDisappearGuest(daIball_c *, s32, s32);
extern BOOL createItemGuest(daIball_c *);
extern void checkGeoGuest(daIball_c *);
extern void movePosition(daIball_c *, dCcD_Stts *);
extern void constructGround(dBgS_GndChk *);
extern f32 groundCross(dBgS *, dBgS_GndChk *);
extern void destroyGround(dBgS_GndChk *, s32);
extern u32 getCharacterInfo(void *, u32, u16);
extern BOOL isLimitedItem(u8);
extern BOOL isNonSavedEmono(u8);
extern BOOL isItemInMemory(dSv_memBit_c *, s32, s32);
extern u32 getItemNoByLife(u8);
using ItemCallback = BOOL (*)();
extern void fastCreateItem(const cXyz *, u32, s32, const csXyz *, const cXyz *, s32, ItemCallback, f32, f32, f32);
extern void registerHeader(void *);
extern void constructHeader0(void *);
extern void constructHeader1(void *);
extern void destroyRipple(IbRipple *, s32);
extern void destroyCylinder(dCcD_Cyl *, s32);
extern void destroyStatus(dCcD_Stts *, s32);
extern void destroyCircle(cM3dGCir_l *, s32);
extern void destroyAcch(dBgS_ObjAcch *, s32);
extern void destroyActor(fopAc_ac_c *, s32);
/* 0217AFB8 */
BOOL daIball_c::CreateHeap() {
    auto *data = static_cast<J3DModelData *>(resource(0x2E));
    if (!data)
        ibAssert(0x47A, modelAssertion());
    J3DModel *model = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
    mpModel = model;
    if (!model)
        return FALSE;
    auto *bck = static_cast<J3DAnmTransform *>(resource(0xE));
    if (!bck)
        ibAssert(0x48B, bckAssertion());
    if (!initBck(&mBckAnm, data, bck, 1, 0, 1, 0, -1, 0))
        return FALSE;
    auto *btk = static_cast<J3DAnmTextureSRTKey *>(resource(0x56));
    if (!btk)
        ibAssert(0x497, btkAssertion());
    if (!initBtk(&mBtkAnm, data, btk, 1, 2, 1, 0, -1, 0, 0))
        return FALSE;
    for (s32 i = 0; i < 2; i++) {
        auto *brk = static_cast<J3DAnmTevRegKey *>(resource(0x4B + i));
        if (!brk)
            ibAssert(0x4A9, brkAssertion());
        if (!initBrk(&mBrkAnm[i], data, brk, 1, 2, 1, 0, -1, 0, 0))
            return FALSE;
    }
    return TRUE;
}
/* 0217B1F4 */
static BOOL CheckCreateHeap(fopAc_ac_c *self) { return static_cast<daIball_c *>(self)->CreateHeap(); }
/* 0217B1F8 */
static daIball_c *ibConstructor(daIball_c *self) {
    if (!self) {
        self = static_cast<daIball_c *>(operator_new(sizeof(daIball_c)));
        if (!self)
            return nullptr;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = 0x10011A14;
    constructAcch(&self->mAcch);
    static_cast<dBgS_PolyPassChk &>(self->mAcch).__vtbl = 0x10011A04;
    static_cast<dBgS_GrpPassChk &>(self->mAcch).__vtbl = 0x100119F4;
    static_cast<cBgS_Chk &>(self->mAcch).__vtbl = 0x100119E4;
    self->mAcch.mbObjThrough = 1;
    dBgS_AcchCir_ct(&self->mAcchCir);
    constructStatus(&self->mStts);
    constructGStatus(&self->mStts.__vtbl_gstts);
    self->mStts.__vtbl = 0x1004AE88;
    self->mStts.__vtbl_gstts = 0x1004AEC0;
    constructObjectInfo(&self->mCyl);
    auto *shape = reinterpret_cast<IbShape *>(self->mCyl.mShapeAttr);
    shape->__vtbl_shape = 0x100015A8;
    shape->__vtbl_base = 0x100118DC;
    constructCylinder(&self->mCyl.mCyl);
    self->mCyl.__vtbl_hitinf = 0x1004B108;
    self->mCyl.mCyl.__vtbl = 0x1004B150;
    shape->__vtbl_shape = 0x1004B160;
    mDoExt_btkAnm::ct(&self->mBtkAnm);
    constructArray(&self->mBrkAnm[0], 2, sizeof(mDoExt_brkAnm), 0x025E80D0);
    constructBck(&self->mBckAnm, 0);
    self->mRippleCb.__vtbl = 0x1016E54C;
    constructRippleAnimation(self->mRippleCb.mAnimation);
    self->mRippleCb.field_0x48 = 0;
    self->mRippleCb.field_0x70 = 0;
    self->mRippleCb.__vtbl = 0x100118EC;
    self->mRippleCb.field_0x74 = 0;
    self->mRippleCb.mAnimationVtable = 0x1016D820;
    self->mRippleCb.field_0x6c = 0;
    self->mRippleCb.field_0x78 = 0;
    constructRippleTail(&self->mRippleCb.mTail);
    self->mLight.mHD20 = 1;
    return self;
}
/* 0217B380 */
void daIball_c::set_mtx() {
    f32 scaleX = scale.x, scaleY = scale.y;
    J3DModel *model = mpModel;
    f32 scaleZ = scale.z;
    model->mBaseScale.x = scaleX;
    model->mBaseScale.z = scaleZ;
    model->mBaseScale.y = scaleY;
    f32 y = (f32)current.pos.y + 40, x = current.pos.x, z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    rotateXYZ(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    // HD: cache the original load order, reload the model before the twelfth load, then store in PPC order.
    constexpr u32 loadOrder[12] = {11, 9, 1, 6, 10, 5, 3, 8, 7, 4, 0, 2};
    f32 values[12];
    Mtx34 *matrix = mDoMtx_stack_c::get();
    for (s32 i = 0; i < 11; i++) {
        u32 n = loadOrder[i];
        values[n] = matrix->m[n / 4][n % 4];
    }
    model = mpModel;
    values[2] = matrix->m[0][2];
    constexpr u32 storeOrder[12] = {11, 4, 0, 10, 2, 8, 6, 9, 1, 7, 5, 3};
    for (s32 i = 0; i < 12; i++) {
        u32 n = storeOrder[i];
        model->mBaseTransformMtx.m[n / 4][n % 4] = values[n];
    }
}
/* 0217B46C */
void daIball_c::setPointLight() {
    f32 frame = mBrkAnm[0].getFrame();
    s32 color = selectColor(frame);
    f32 random = cM_rndF(0.2f);
    addCalc(&mLightFlickerMult, random + 1, 0.5f, 0.02f);
    mLight.mPos.copy(current.pos);
    mLight.mColorR = light_color.values[color][0];
    mLight.mColorG = light_color.values[color][1];
    mLight.mColorB = light_color.values[color][2];
    f32 power = (radiusBias.value + 80) * (f32)mLightFlickerMult;
    mLight.mPower = (f32)(s16)gabi::ftoi(power);
    mLight.mFluctuation = 250;
}
/* 0217B5CC */
void daIball_c::CreateInit() {
    J3DModel *model = mpModel;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model));
    mStts.Init(255, 255, this);
    mCyl.Set(reinterpret_cast<dCcD_SrcCyl *>(&m_cyl_src));
    mCyl.mStts = &mStts;
    mAcchCir.SetWall(30, 30);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    f32 y = current.pos.y;
    u32 flags = mAcch.m_flags;
    mbPlayedSe = 0;
    f32 frame = mBckAnm.getFrame();
    model = mpModel;
    mAcch.m_flags = flags | dBgS_Acch::SEA_CHECK | dBgS_Acch::SEA_WATER_HEIGHT;
    mMode = MODE_WAIT;
    current.pos.y = y - 40;
    gravity = -7;
    entryBck(&mBckAnm, model->getModelData(), frame);
    set_mtx();
    modelCalc(mpModel);
    dKy_plight_set(&mLight);
    setPointLight();
    mLightFlickerMult = 1;
    ibRegist(this);
}
/* 0217B6F8 */
cPhs_State daIball_c::_daIball_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            ibConstructor(this);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x3500))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}
/* 0217B788 */
static cPhs_State daIball_Create(daIball_c *self) { return self->_daIball_create(); }
/* 0217B78C */
BOOL daIball_c::_daIball_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    for (s32 i = 0; i < 2; i++) {
        f32 frame = mBrkAnm[i].getFrame();
        entryBrkModel(&mBrkAnm[i], mpModel, frame);
    }
    J3DModel *model = mpModel;
    f32 frame = mBtkAnm.getFrame();
    J3DModelData *data = model->getModelData();
    mBtkAnm.entry(data, frame);
    model = mpModel;
    frame = mBckAnm.getFrame();
    data = model->getModelData();
    entryBck(&mBckAnm, data, frame);
    // HD: the verified draw path omits the GC simple shadow.
    dComIfGd_setListMaskOff();
    modelUpdateDL(mpModel, 0);
    dComIfGd_setList();
    return TRUE;
}
/* 0217B878 */
static BOOL daIball_Draw(daIball_c *self) { return self->_daIball_draw(); }
/* 0217B87C */
BOOL daIball_c::_daIball_delete() {
    removeRippleTail(&mRippleCb.mTail);
    dKy_plight_cut(&mLight);
    ibRemove(this);
    return TRUE;
}
/* 0217B8C0 */
static BOOL daIball_Delete(daIball_c *self) { return self->_daIball_delete(); }
/* 0217B8C4 */
void daIball_c::checkGeo() {
    gabi::Local<GroundStorage> checker;
    gabi::Local<cXyz> scale;
    gabi::Local<cXyz> position;
    gabi::Local<u8[16]> linkage;
    s32 timer = mTimer;
    mPrevSpeedY = (f32)speed.y;
    if (timer > 32)
        movePosition(this, &mStts);
    mAcch.CrrPos(dComIfG_Bgsp());
    (this->*mode_proc.entries[(u8)mMode])();
    dBgS_GndChk *ground = &checker->value;
    constructGround(ground);
    ground->mbBombThrough = 0;
    ground->mbCamThrough = 0;
    f32 z = current.pos.z, oldY = old.pos.y;
    static_cast<dBgS_PolyPassChk *>(ground)->__vtbl = 0x100119D4;
    static_cast<cBgS_PolyInfo *>(ground)->__vtbl = 0x100119B4;
    ground->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(ground));
    f32 y = oldY + 30;
    ground->mbObjThrough = 1;
    f32 x = current.pos.x;
    ground->mGrp = 4;
    static_cast<cBgS_Chk *>(ground)->__vtbl = 0x100119A4;
    ground->m_pos.z = z;
    ground->mbLinkThrough = 0;
    ground->mbRopeThrough = 0;
    ground->mbBoomerangThrough = 0;
    static_cast<dBgS_GrpPassChk *>(ground)->__vtbl = 0x100119C4;
    y = y + 40;
    ground->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(ground));
    ground->m_pos.x = x;
    ground->mbArrowThrough = 0;
    ground->m_pos.y = y;
    f32 lavaY = groundCross(dComIfG_Bgsp(), ground);
    f32 groundHeight = mAcch.m_ground_h;
    if (lavaY != -1000000000.0f) {
        f32 difference = lavaY - groundHeight, actorY = current.pos.y;
        bool disappear = difference < 20 ? lavaY > actorY : lavaY > actorY + 20;
        if (disappear) {
            sound(this, 0x691B);
            position->x = (f32)current.pos.x;
            position->y = lavaY;
            position->z = (f32)current.pos.z;
            scale->x = 0.25f;
            scale->y = 0.25f;
            scale->z = 0.25f;
            dComIfGp_particle_set(0x80D5, position.get(), nullptr, scale.get(), 255, nullptr, -1, nullptr, nullptr,
                                  nullptr);
            fopAcM_delete(this);
        }
    }
    static_cast<cBgS_PolyInfo *>(ground)->__vtbl = 0x10011934;
    static_cast<dBgS_PolyPassChk *>(ground)->__vtbl = 0x10011954;
    static_cast<dBgS_GrpPassChk *>(ground)->__vtbl = 0x10011914;
    destroyGround(ground, 0);
}
/* 0217BB38 */
void daIball_c::animControl() {
    bool event = dComIfGp_event_runCheck();
    s32 index = mPlaySpeedIdx;
    mBrkAnm[0].mFrameCtrl.mRate = m_data.mPlaySpeeds[(u32)index];
    playAnimation(&mBrkAnm[0]);
    bool looped = mBrkAnm[0].mFrameCtrl.checkState(J3DFrameCtrl::STATE_LOOP_E);
    mBrkAnm[1].mFrameCtrl.mRate = 1;
    playAnimation(&mBrkAnm[1]);
    mBtkAnm.mFrameCtrl.mRate = 1;
    playAnimation(&mBtkAnm);
    playAnimation(&mBckAnm);
    if (!event) {
        f32 frame = mBrkAnm[0].getFrame();
        f32 previous = frame - (f32)mBrkAnm[0].mFrameCtrl.mRate;
        // HD: retain unordered comparisons for the original NaN behavior.
        if (looped || (!(frame < 45) && previous < 45) || (!(frame < 90) && previous < 90))
            sound(this, 0x580E);
    }
}
/* 0217BC58 */
void daIball_c::createDisappearEffect(s32 type, s32 color) {
    gabi::Local<cXyz> scale;
    gabi::Local<cXyz> position;
    gabi::Local<u8[16]> linkage;
    scale->x = 1;
    scale->y = 1;
    scale->z = 1;
    position->x = (f32)current.pos.x;
    position->z = (f32)current.pos.z;
    position->y = (f32)current.pos.y + 40;
    if ((u32)type > 1)
        return;
    if (type == 0) {
        ibParticle(0x1C, position.get(), scale.get(), &lifeBallCallbacks.entries[(u32)color]);
        ibParticle(0x1D, position.get(), scale.get(), nullptr);
        if (color == 2)
            ibParticle(0x47, position.get(), nullptr, nullptr);
    } else {
        ibParticle(0x19, position.get(), scale.get(), nullptr);
        ibParticle(0x1A, position.get(), scale.get(), nullptr);
        ibParticle(0x1B, position.get(), scale.get(), nullptr);
    }
    sound(this, type == 0 ? 0x5816 : 0x580D);
}
/* 0217BE98 */
BOOL daIball_c::dead() {
    f32 frame = mBrkAnm[0].getFrame();
    s32 color = selectColor(frame);
    createDisappearGuest(this, 0, color);
    fopAcM_delete(this);
    return TRUE;
}
/* 0217BF6C */
BOOL daIball_c::createItem() {
    gabi::Local<be<u32>[8]> items;
    gabi::Local<csXyz> angles;
    gabi::Local<cXyz> scale;
    gabi::Local<u8[16]> linkage;
    u16 id = (u32)mParameters;
    for (s32 i = 0; i < 8; i++) {
        IbPlay *play = ibPlay();
        void *table = &play->mCharacterTable;
        play = ibPlay();
        u32 entry = play->mItemEntries[i];
        (*items)[i] = getCharacterInfo(table, entry, id);
    }
    f32 scaleX = zeroScale.x, scaleY = zeroScale.y, scaleZ = zeroScale.z;
    for (s32 i = 0; i < 8; i++) {
        s32 bit = -1;
        if (isLimitedItem((u8)(u32)(*items)[i])) {
            bit = (s8)((u32)mParameters >> 16);
            bool replace = bit == 31 || bit == 255 || bit == -1;
            if (!replace) {
                s32 room = (s8)current.roomNo;
                u8 item = (u8)(u32)(*items)[i];
                dSv_info_c *save = dComIfGs_info();
                // HD: limited item 0x4B checks the fixed saved-memory slot used by the original.
                replace = item == 0x4B ? isItemInMemory(&save->mSavedata.mMemory[14].mMembit, bit, room)
                                       : save->isItem(bit, room);
            }
            if (replace) {
                (*items)[i] = 3;
                bit = -1;
            }
        } else if (isNonSavedEmono((u8)(u32)(*items)[i])) {
            if ((s8)((u32)mParameters >> 16) != 0)
                (*items)[i] = 3;
            bit = -1;
        }
        angles->y = (s16)zeroAngles.y;
        angles->x = (s16)zeroAngles.x;
        angles->z = (s16)zeroAngles.z;
        scale->x = scaleX;
        scale->y = scaleY;
        scale->z = scaleZ;
        f32 direction = cM_rndFX(1);
        f32 sign = direction < 0 ? -1.f : 1.f;
        f32 jitter = cM_rndFX(4);
        f32 horizontal = gabi::fmadds(sign, 10, jitter);
        angles->y = (s16)gabi::ftoi(cM_rndF(32767));
        u32 item = getItemNoByLife((u8)(u32)(*items)[i]);
        (*items)[i] = item;
        f32 random = cM_rndF(10), vertical = 53 + random;
        fastCreateItem(&current.pos, item, (s8)current.roomNo, angles.get(), scale.get(), bit, itemParamSet_CB,
                       horizontal, vertical, -6);
    }
    dead();
    return TRUE;
}
/* 0217C3C0 */
void daIball_c::damage() {
    if (!mCyl.ChkTgHit())
        return;
    auto *hit = static_cast<dCcD_GObjInf *>(mCyl.GetTgHitObj());
    // HD: preserve the verified aggregate attack mask.
    if (hit && ((u32)hit->mAtType & IB_BREAK_ATTACK_TYPES))
        createItemGuest(this);
}
/* 0217C444 */
BOOL daIball_c::_daIball_execute() {
    s32 timer = (s32)((u32)(s32)mTimer + 1);
    f32 eyeY = (f32)current.pos.y + 45;
    u32 x = word(current.pos.x), z = word(current.pos.z);
    word(eyePos.x) = x;
    mTimer = timer;
    eyePos.y = eyeY;
    word(eyePos.z) = z;
    setPointLight();
    if (!mbPlayedSe) {
        sound(this, 0x5802);
        mbPlayedSe = 1;
    }
    checkGeoGuest(this);
    animControl();
    damage();
    set_mtx();
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    return TRUE;
}
/* 0217C518 */
static BOOL daIball_Execute(daIball_c *self) { return self->_daIball_execute(); }
/* 0217C51C */
void daIball_c::mode_wait_init() {
    removeRippleTail(&mRippleCb.mTail);
    mMode = MODE_WAIT;
}
/* 0217C554 */
void daIball_c::mode_water_init() {
    dComIfGp_getParticle()->set(5, 0x33, &current.pos, nullptr, &scale, 255,
                                reinterpret_cast<dPa_levelEcallBack *>(&mRippleCb.mTail), -1, nullptr, nullptr,
                                nullptr);
    mMode = MODE_WATER;
    mRippleCb.mTail.mRate = 0;
}
/* 0217C5CC */
void daIball_c::mode_wait() {
    if ((u32)mAcch.m_flags & dBgS_Acch::GROUND_LANDING) {
        f32 bounce = (f32)mPrevSpeedY * 0.5f, limit = (f32)gravity - 0.5f;
        mPrevSpeedY = bounce;
        if (bounce > limit)
            speedF = 0;
        else {
            speed.x = 0;
            speed.z = 0;
            speed.y = -bounce;
        }
    }
    f32 water = mAcch.m_sea_height;
    if (water > (f32)current.pos.y && water != -1000000000.0f) {
        mode_water_init();
        current.pos.y = water;
    }
}
/* 0217C688 */
void daIball_c::mode_water() {
    f32 water = mAcch.m_sea_height;
    if (water == -1000000000.0f || water < (f32)current.pos.y)
        mode_wait_init();
    if (water != -1000000000.0f)
        current.pos.y = water;
}
/* 0217C710 */
static void ibStaticInit() {
    zero2.value = 0;
    zero0.value = 0;
    zero3.value = 0;
    zero1.value = 0;
    registerHeader(&record0);
    minimum.value = minimumConstant.value;
    maximum.value = maximumConstant.value;
    constructHeader0(&flag0);
    registerHeader(&record1);
    constructHeader1(&flag1);
    registerHeader(&record2);
}
/* 0217C7A4 */
static BOOL itemParamSet_CB() {
    // HD: this callback returns success without the GC item-timer adjustment.
    return TRUE;
}
/* 0217C7C0 */
static BOOL daIball_IsDelete(daIball_c *self) { return TRUE; }
/* 0217C7AC */
static void ibStringDestructor(void *self, s32 flags) {
    if (self && (flags & 1))
        operator_delete(self);
}
/* 0217C870 */
static void ibStringVirtual(void *self) {}
/* 0217C7C8 */
static void ibDestructor(daIball_c *self, s32 flags) {
    if (!self)
        return;
    destroyRipple(&self->mRippleCb, 0);
    destroyCylinder(&self->mCyl, 2);
    destroyStatus(&self->mStts, 2);
    destroyCircle(&self->mAcchCir.m_cir, 2);
    static_cast<dBgS_GrpPassChk &>(self->mAcch).__vtbl = 0x100119F4;
    static_cast<dBgS_PolyPassChk &>(self->mAcch).__vtbl = 0x10011A04;
    destroyAcch(&self->mAcch, 0);
    destroyActor(self, 0);
    if (flags & 1)
        operator_delete(self);
}
/* WWHD glue
data mode_proc 10011B04
data m_data 10011A24
data light_color 10011ADC
data lifeBallCallbacks 1047B26C
data radiusBias 1047BCD0
data minimum 10464798
data maximum 1046479C
data minimumConstant 10011B30
data maximumConstant 10011B34
data zero0 104647A4
data zero1 104647A8
data zero2 104647AC
data zero3 104647B0
data flag0 104647A0
data flag1 104647A1
data record0 101B78A0
data record1 101B78AC
data record2 101B78B8
data zeroScale 101FFBA8
data zeroAngles 101FFB14
data arc 10011B38
data file 10011A88
data modelAssertText 10011A94
data bckAssertText 10011A64
data btkAssertText 10011A70
data brkAssertText 10011A7C
data m_cyl_src 10011B40
ext 026066C4 void* getResource(dRes_control_c*,SafeString*,s32)
ext 025E1A40 void seStartLevel(u32,const cXyz*,u32,s32)
ext 025E8508 BOOL initBck(J3DFrameCtrl*,J3DModelData*,J3DAnmTransform*,u32,u32,f32,s32,s32,u32)
ext 025E7CE0 BOOL initBtk(mDoExt_btkAnm*,J3DModelData*,J3DAnmTextureSRTKey*,u32,u32,f32,s32,s32,u32,u32)
ext 025E8154 BOOL initBrk(mDoExt_brkAnm*,J3DModelData*,J3DAnmTevRegKey*,u32,u32,f32,s32,s32,u32,u32)
ext 024F0474 void constructAcch(dBgS_ObjAcch*)
ext 0200BD2C void constructStatus(dCcD_Stts*)
ext 02515DA0 void constructGStatus(void*)
ext 02515FB8 void constructObjectInfo(dCcD_GObjInf*)
ext 02018590 void constructCylinder(cM3dGCyl*)
ext 028EFFD0 void constructArray(void*,u32,u32,u32)
ext 027F2BC0 void constructBck(J3DFrameCtrl*,u32)
ext 027DA984 void constructRippleAnimation(void*)
ext 025A9084 void constructRippleTail(IbRippleTail*)
ext 025F19F8 void rotateXYZ(Mtx34*,s16,s16,s16)
ext 0200ED84 void addCalc(be<f32>*,f32,f32,f32)
ext 025E86B8 void entryBck(J3DFrameCtrl*,J3DModelData*,f32)
ext 025E8480 void entryBrkModel(mDoExt_brkAnm*,J3DModel*,f32)
ext 027F4D5C void modelCalc(J3DModel*)
ext 02526144 void ibRegist(daIball_c*)
ext 02526180 void ibRemove(daIball_c*)
ext 025E2DE0 void modelUpdateDL(J3DModel*,u32)
ext 025A9270 void removeRippleTail(IbRippleTail*)
ext 025E742C void playAnimation(void*)
ext 0217BC58 void createDisappearGuest(daIball_c*,s32,s32)
ext 0217BF6C BOOL createItemGuest(daIball_c*)
ext 0217B8C4 void checkGeoGuest(daIball_c*)
ext 025D6870 void movePosition(daIball_c*,dCcD_Stts*)
ext 02008E0C void constructGround(dBgS_GndChk*)
ext 02008974 f32 groundCross(dBgS*,dBgS_GndChk*)
ext 02008DAC void destroyGround(dBgS_GndChk*,s32)
ext 0200EA74 u32 getCharacterInfo(void*,u32,u16)
ext 0255106C BOOL isLimitedItem(u8)
ext 02551080 BOOL isNonSavedEmono(u8)
ext 025B8DC8 BOOL isItemInMemory(dSv_memBit_c*,s32,s32)
ext 02551108 u32 getItemNoByLife(u8)
ext 025D8AB0 void fastCreateItem(const cXyz*,u32,s32,const csXyz*,const cXyz*,s32,ItemCallback,f32,f32,f32)
ext 028F026C void registerHeader(void*)
ext 028ED6F8 void constructHeader0(void*)
ext 028EAB2C void constructHeader1(void*)
ext 027F3628 void destroyRipple(IbRipple*,s32)
ext 02515A70 void destroyCylinder(dCcD_Cyl*,s32)
ext 02515860 void destroyStatus(dCcD_Stts*,s32)
ext 02018034 void destroyCircle(cM3dGCir_l*,s32)
ext 024EFD9C void destroyAcch(dBgS_ObjAcch*,s32)
ext 025D50BC void destroyActor(fopAc_ac_c*,s32)
*/
