/** Bomb flower (VbakH), using GameCube names and verified HD layouts.
 * Native call/load/store order and guest stack objects are retained. */
#include "d/actor/d_a_bflower.h"

static BFlowerSavePointer l_bflowerSave;
static BFlowerResourcePointer l_bflowerResourceControl;
static dCcD_SrcSph l_sph_src;
static dCcD_SrcSph l_sph_src2;
static dCcD_SrcCyl l_cyl_src;
static BFlowerActionTable action_tbl;
static cXyz bomb_offset;
static BFlowerHeaderStatics l_headerStatics;
static BFlowerFloat l_negativePi;
static BFlowerFloat l_positivePi;
static BFlowerByte l_headerInit1;
static BFlowerByte l_headerInit2;
static BFlowerByte l_headerDtor1;
static BFlowerByte l_headerDtor2;
static BFlowerByte l_headerDtor3;

static BOOL CheckCreateHeap(daBFlower_c *);
namespace {
static dComIfG_inf_c *bflowerPlay() {
    // Reserve the guest linkage area before a non-leaf callee saves LR.
    gabi::Local<be<u32>[4]> linkage;
    return reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
}

// HD: preserve the SafeString and linkage allocation order of the native resource wrapper.
static void *resource(s32 index) {
    gabi::Local<be<u32>[2]> name;
    (*name)[0] = gabi::guest_str("VbakH");
    (*name)[1] = BFLOWER_SAFE_STRING_VTABLE;
    auto control = l_bflowerResourceControl.get();
    gabi::Local<be<u32>[4]> linkage;
    return BFlower_getResource(control, name.get(), index);
}
static void *checkedResource(s32 index, s32 line, const char *expression) {
    auto resourceData = resource(index);
    if (!resourceData)
        BFlower_panic("d_a_bflower.cpp", line, expression);
    return resourceData;
}
static s32 initBck(mDoExt_bckAnm *controller, void *model, void *animation, s32 mode, s32 modify) {
    return controller->init(static_cast<J3DModelData *>(model), static_cast<J3DAnmTransform *>(animation),
                            true, mode, 1.0f, 0, -1, modify);
}
static s32 initBrk(mDoExt_brkAnm *controller, void *model, void *animation) {
    return controller->init(static_cast<J3DModelData *>(model), static_cast<J3DAnmTevRegKey *>(animation),
                            true, 0, 1.0f, 0, -1, false, 0);
}
static void translate(daBFlower_c *p) {
    f32 x = p->current.pos.x, y = p->current.pos.y, z = p->current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
}
static void rotate(daBFlower_c *p) { mDoMtx_stack_c::ZXYrotM(p->current.angle); }
static void copyMatrix(J3DModel *model) { model->setBaseTRMtx(mDoMtx_stack_c::get()); }
static f32 distanceXZ(fopAc_ac_c *other, daBFlower_c *p) {
    gabi::Local<cXyz> delta, flat;
    BFlower_subtractPosition(&other->current.pos, delta.get(), &p->current.pos);
    f32 x = delta->x, z = delta->z;
    flat->x = x;
    flat->y = 0.0f;
    flat->z = z;
    f32 square = BFlower_squareMagnitude(flat.get());
    return BFlower_sqrt(square);
}
static u32 grabId(fopAc_ac_c *player) {
    auto playerView = reinterpret_cast<BFlowerPlayerView *>(player);
    return gabi::call_ptr<u32>(playerView->__vtbl->getGrabActorID, playerView);
}
static void sound(daBFlower_c *p, s32 id) {
    s8 room = p->current.roomNo;
    s32 reverb = BFlower_getReverb(room);
    mDoAud_seStart(id, &p->eyePos, 0, reverb);
}
static fopAc_ac_c *createBomb(daBFlower_c *p, u32 parameter) {
    s8 room = p->current.roomNo;
    return BFlower_createBomb(0x127, parameter, &p->current.pos, room, &p->current.angle, nullptr, -1, 0, 0);
}
} // namespace
/* 02076850 */
BOOL daBFlower_c::CreateHeap() {
    u32 parameters = mParameters;
    mSwitchNo = (parameters >> 8) & 255;
    mState = (parameters >> 4) & 15;
    s8 room = home.roomNo;
    s32 sw = mSwitchNo;
    auto save = l_bflowerSave.get();
    if (save->mSwitchInfo.isSwitch(sw, room))
        mState = STATE_LIVE_E;
    auto model = checkedResource(dRes_INDEX_VBAKH_BDL_VBAKH_e, 434, "modelData != NULL");
    mpModel = mDoExt_J3DModel__create(static_cast<J3DModelData *>(model), 0x80000, 0x11000022);
    if (!mpModel)
        return FALSE;
    auto anim = checkedResource(dRes_INDEX_VBAKH_BCK_VBAHX_e, 451, "pbck != NULL");
    if (!initBck(&mBck1, model, anim, 1, 0))
        return FALSE;
    mBck1.mFrameCtrl.mFrame = (f32)mBck1.mFrameCtrl.mEnd;
    anim = checkedResource(dRes_INDEX_VBAKH_BRK_VBAHX_e, 473, "pbrk != NULL");
    if (!initBrk(&mBrk1, model, anim))
        return FALSE;
    mBrk1.mFrameCtrl.mRate = 0.0f;
    model = checkedResource(dRes_INDEX_VBAKH_BDL_VBAKM_e, 487, "modelData != NULL");
    mpModel2 = mDoExt_J3DModel__create(static_cast<J3DModelData *>(model), 0x80000, 0x11000022);
    if (!mpModel2)
        return FALSE;
    anim = checkedResource(dRes_INDEX_VBAKH_BCK_VBAMX_e, 503, "pbck != NULL");
    if (!initBck(&mBck2, model, anim, 0, 0))
        return FALSE;
    mBck2.mFrameCtrl.mRate = 0.0f;
    anim = checkedResource(dRes_INDEX_VBAKH_BRK_VBAMX_e, 520, "pbrk != NULL");
    if (!initBrk(&mBrk2, model, anim))
        return FALSE;
    mBrk2.mFrameCtrl.mRate = 0.0f;
    if (mState == STATE_LIVE_E) {
        // HD: all three end frames are read before any frame is written.
        f32 flowerBrkEnd = mBrk1.getEndFrame(), bombBrkEnd = mBrk2.getEndFrame(),
            bombBckEnd = mBck2.getEndFrame();
        mBrk2.mFrameCtrl.mFrame = bombBrkEnd;
        mBrk1.mFrameCtrl.mFrame = flowerBrkEnd;
        mBck2.mFrameCtrl.mFrame = bombBckEnd;
    } else if (mState == STATE_DEAD_e) {
        mBck2.mFrameCtrl.mFrame = 0.0f;
        mBrk2.mFrameCtrl.mFrame = 0.0f;
        mBrk1.mFrameCtrl.mFrame = 0.0f;
    }
    return TRUE;
}
/* 02076C34 */
void daBFlower_c::set_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    auto model = mpModel.get();
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    translate(this);
    rotate(this);
    copyMatrix(mpModel);
    x = mBombScale.x;
    y = mBombScale.y;
    z = mBombScale.z;
    model = mpModel2.get();
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
    copyMatrix(mpModel2);
}
/* 02076D94 */
void daBFlower_c::CreateInit() {
    cullMtx = gabi::ea(mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 100.0f, 50.0f);
    mStts.Init(255, 255, this);
    mCyl.Set(&l_cyl_src);
    u8 state = mState;
    mCyl.SetStts(&mStts);
    mSph.Set(state ? &l_sph_src2 : &l_sph_src);
    current.angle.x = home.angle.x;
    current.angle.y = home.angle.y;
    mSph.SetStts(&mStts);
    current.angle.z = home.angle.z;
    set_mtx();
    eyePos.x = 0.0f;
    eyePos.y = 50.0f;
    eyePos.z = 0.0f;
    translate(this);
    rotate(this);
    mDoMtx_stack_c::multVec(&eyePos, &eyePos);
    attention_info.position.copy(eyePos);
    mGrabbable = current.angle.x != 0;
    mpBombActor = nullptr;
    m58C = 1;
    m5B8 = 0;
    mBombScale.copy(scale);
    f32 r = cM_rndF(150.0f);
    m55C = -1;
    mAnimTimer = gabi::ftoi(r + 60.0f);
}
/* 02077530 */
s32 daBFlower_c::init_bck_anm(s16 index) {
    if (m55C != -1)
        return 1;
    m55C = index;
    auto model = resource(dRes_INDEX_VBAKH_BDL_VBAKH_e), anim = resource(index);
    return initBck(&mBck1, model, anim, 1, 1);
}
/* 02077280 */
void daBFlower_c::animPlay() {
    if (mBck1.play())
        m55C = -1;
    mBrk1.play();
}
/* 020772C8 */
void daBFlower_c::setCollision() {
    if (mState == STATE_DEAD_e) {
        mCyl.SetC(&current.pos);
        auto p = bflowerPlay();
        BFlower_setCollision(&p->play.mCcS, &mCyl);
        mSph.SetC(&current.pos);
        p = bflowerPlay();
        BFlower_setCollision(&p->play.mCcS, &mSph);
    } else if (m58C) {
        gabi::Local<cXyz> center;
        center->x = 0.0f;
        center->y = 30.0f;
        center->z = 0.0f;
        translate(this);
        rotate(this);
        mDoMtx_stack_c::multVec(center.get(), center.get());
        mSph.SetC(center.get());
        auto p = bflowerPlay();
        BFlower_setCollision(&p->play.mCcS, &mSph);
    }
}
/* 0207717C */
bool daBFlower_c::_draw() {
    auto e = dKy_getEnvlight();
    settingTevStruct(e, 0, &current.pos, &tevStr);
    e = dKy_getEnvlight();
    auto model = mpModel.get();
    setLightTevColorType(e, model, &tevStr);
    model = mpModel.get();
    auto data = model->getModelData();
    f32 frame = mBck1.mFrameCtrl.mFrame;
    mBck1.entry(data, frame);
    model = mpModel.get();
    data = model->getModelData();
    frame = mBrk1.mFrameCtrl.mFrame;
    mBrk1.entry(data, frame);
    mDoExt_modelUpdateDL(mpModel, 0);
    model = mpModel.get();
    data = model->getModelData();
    reinterpret_cast<BFlowerModelData *>(data)->mJointTree->mpMtxCalc = 0;
    if (m58C) {
        e = dKy_getEnvlight();
        model = mpModel2.get();
        setLightTevColorType(e, model, &tevStr);
        model = mpModel2.get();
        data = model->getModelData();
        frame = mBck2.mFrameCtrl.mFrame;
        mBck2.entry(data, frame);
        model = mpModel2.get();
        data = model->getModelData();
        frame = mBrk2.mFrameCtrl.mFrame;
        mBrk2.entry(data, frame);
        mDoExt_modelUpdateDL(mpModel2, 0);
        model = mpModel2.get();
        data = model->getModelData();
        reinterpret_cast<BFlowerModelData *>(data)->mJointTree->mpMtxCalc = 0;
    }
    return true;
}
/* 020773C4 */
bool daBFlower_c::_execute() {
    auto p = bflowerPlay();
    auto player = p->play.getPlayer(0);
    (this->*action_tbl.actions[u8(mState)])();
    animPlay();
    set_mtx();
    setCollision();
    mPrevPlayerDist = distanceXZ(player, this);
    mPrevGrabActorID = grabId(player);
    return true;
}
/* 020775F8 */
BOOL daBFlower_c::actLive() {
    auto p = bflowerPlay();
    auto player = p->play.getPlayer(0);
    f32 distance = distanceXZ(player, this);
    BFlower_addCalc(&mBombScale.x, 1.0f, 0.05f, 0.1f, 0.05f);
    BFlower_addCalc(&mBombScale.y, 1.0f, 0.05f, 0.1f, 0.05f);
    f32 growing = BFlower_addCalc(&mBombScale.z, 1.0f, 0.05f, 0.1f, 0.05f);
    if (m5B8 && growing == 0.0f)
        init_bck_anm(dRes_INDEX_VBAKH_BCK_VBAKH_e);
    if (distance < 50.0f) {
        f32 change = std::fabs((f32)mPrevPlayerDist - distance);
        if (change > 2.0f) {
            f32 y = current.pos.y, py = f32(player->current.pos.y);
            if (std::fabs(py - y) < 10.0f)
                init_bck_anm(dRes_INDEX_VBAKH_BCK_VBAKH_e);
        }
    }
    if (distance < 200.0f) {
        u32 grabbed = grabId(player), previous = mPrevGrabActorID;
        if (grabbed != previous && grabId(player) == 0xFFFFFFFF) {
            previous = mPrevGrabActorID;
            auto actor = fopAcM_SearchByID(previous);
            if (actor && distanceXZ(actor, this) < 70.0f)
                init_bck_anm(dRes_INDEX_VBAKH_BCK_VBAKH_e);
        }
    }
    mBrk2.play();
    s32 finished = mBck2.play();
    s32 grabbable = mGrabbable;
    if (finished)
        m58D = 1;
    auto bomb = mpBombActor.get();
    if (grabbable == 1)
        m58D = 0;
    u32 id = bomb ? u32(bomb->mBsPcId) : 0xFFFFFFFF;
    if (BFlower_isExecuting(id)) {
        u32 flags = u32(attention_info.flags);
        m58D = 0;
        attention_info.flags = flags & ~u32(fopAc_Attn_ACTION_CARRY_e);
    } else {
        if (!m58C) {
            mBombScale.z = 0.0f;
            mBombScale.x = 0.0f;
            mBombScale.y = 0.0f;
            sound(this, 0x6A0F);
        }
        u8 ready = m58D;
        m58C = 1;
        u32 flags = u32(attention_info.flags);
        attention_info.flags =
            ready ? flags | fopAc_Attn_ACTION_CARRY_e : flags & ~u32(fopAc_Attn_ACTION_CARRY_e);
    }
    if (m58C && growing == 0.0f) {
        if (mSph.ChkTgHit()) {
            auto hit = reinterpret_cast<dCcD_GObjInf *>(mSph.GetTgHitObj());
            if (hit) {
                u32 type = u32(hit->mAtType);
                if (type & 0x20) {
                    mpBombActor = createBomb(this, 0x100);
                    m58C = 0;
                } else if (type & 0xFF1DFEFF) {
                    u32 parameter = mGrabbable == 1 ? 0x101 : 1;
                    mpBombActor = createBomb(this, parameter);
                    m58C = 0;
                }
            }
        }
        mSph.ClrTgHit();
    }
    if ((u32)actor_status & fopAcStts_CARRY_e) {
        if (m58D) {
            m58C = 0;
            mpBombActor = createBomb(this, 2);
            fopAcM_cancelCarryNow(this);
            bomb = mpBombActor.get();
            if (bomb) {
                BFlower_setCarryNow(bomb, 0);
                p = bflowerPlay();
                auto link = p->play.mpPlayerPtr[0].get();
                bomb = mpBombActor.get();
                BFlower_setActorKeep(&reinterpret_cast<BFlowerLinkView *>(link)->mActorKeep, bomb);
            }
            init_bck_anm(dRes_INDEX_VBAKH_BCK_VBAKH_e);
        }
    }
    m5B8 = m58C && growing > 0.0f;
    return TRUE;
}
/* 02077AE8 */
BOOL daBFlower_c::actDead() {
    auto p = bflowerPlay();
    auto player = p->play.getPlayer(0);
    f32 distance = distanceXZ(player, this);
    s32 timer = mAnimTimer;
    if (timer > 0) {
        timer = (s32)((u32)timer - 1);
        mAnimTimer = timer;
    }
    if (distance < 50.0f && std::fabs((f32)mPrevPlayerDist - distance) > 2.0f) {
        init_bck_anm(dRes_INDEX_VBAKH_BCK_VBAHX_e);
        timer = mAnimTimer;
    }
    if (!timer) {
        f32 r = cM_rndF(100.0f);
        mAnimTimer = gabi::ftoi(r + 100.0f);
        init_bck_anm(dRes_INDEX_VBAKH_BCK_VBAHX_e);
    }
    mBrk1.mFrameCtrl.mRate = 0.0f;
    if (mCyl.ChkTgHit()) {
        auto hit = reinterpret_cast<dCcD_GObjInf *>(mCyl.GetTgHitObj());
        if (hit && (u32(hit->mAtType) & 0x100)) {
            sound(this, 0x69EA);
            s8 room = home.roomNo;
            mState = STATE_LIVE_E;
            m594 = 70;
            auto save = l_bflowerSave.get();
            s32 sw = mSwitchNo;
            save->mSwitchInfo.onSwitch(sw, room);
            init_bck_anm(dRes_INDEX_VBAKH_BCK_VBAKH_e);
            mBrk2.mFrameCtrl.mRate = 1.0f;
            mBck2.mFrameCtrl.mRate = 1.0f;
            mBrk1.mFrameCtrl.mRate = 1.0f;
            mSph.Set(&l_sph_src);
        }
    }
    u32 flags = u32(attention_info.flags);
    m58D = 0;
    m58C = 1;
    attention_info.flags = flags & ~u32(fopAc_Attn_ACTION_CARRY_e);
    return TRUE;
}
/* 02076F80 */
s32 daBFlower_c::_create() {
    if (!((u32)actor_condition & 8)) {
        if (gabi::ea(this)) {
            BFlower_actorCtor(this);
            __vtbl = BFLOWER_ACTOR_VTABLE;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, BFLOWER_CYL_AABB_VTABLE);
            BFlower_sphereCtor(&mSph);
            mBck1.mFrameCtrl.init(0);
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->__vtbl = BFLOWER_TRANSFORM_BASE_VTABLE;
            BFlower_transformCtor(reinterpret_cast<BFlowerBckStorage *>(&mBck1)->mTransform);
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->mpTransformInfo = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->__vtbl = BFLOWER_BCK_TRANSFORM_VTABLE;
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->mpQuat = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->mpAnmTransform = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->__vtblMtxCalc = BFLOWER_MTX_CALC_VTABLE;
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->mpCallback2 = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck1)->mpCallback1 = 0;
            mDoExt_brkAnm::ct(&mBrk1);
            mBck2.mFrameCtrl.init(0);
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->__vtbl = BFLOWER_TRANSFORM_BASE_VTABLE;
            BFlower_transformCtor(reinterpret_cast<BFlowerBckStorage *>(&mBck2)->mTransform);
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->__vtblMtxCalc = BFLOWER_MTX_CALC_VTABLE;
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->mpAnmTransform = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->mpCallback2 = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->mpTransformInfo = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->mpCallback1 = 0;
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->__vtbl = BFLOWER_BCK_TRANSFORM_VTABLE;
            reinterpret_cast<BFlowerBckStorage *>(&mBck2)->mpQuat = 0;
            mDoExt_brkAnm::ct(&mBrk2);
        }
        actor_condition = (u32)actor_condition | 8;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "VbakH");
    if (phase == 4) {
        if (!fopAcM_entrySolidHeap(this, (BOOL (*)(fopAc_ac_c *))CheckCreateHeap, 0xFC0))
            return 5;
        CreateInit();
    }
    return phase;
}
/* 02076C30 */
static BOOL CheckCreateHeap(daBFlower_c *p) { return p->CreateHeap(); }
/* 02077148 */
static s32 daBFlower_Create(daBFlower_c *p) { return p->_create(); }
/* 0207714C */
static BOOL daBFlower_Delete(daBFlower_c *p) {
    dComIfG_resDelete(&p->mPhs, "VbakH");
    return TRUE;
}
/* 0207727C */
static BOOL daBFlower_Draw(daBFlower_c *p) { return p->_draw(); }
/* 0207752C */
static BOOL daBFlower_Execute(daBFlower_c *p) { return p->_execute(); }
/* 02077D94 */
static BOOL daBFlower_IsDelete(daBFlower_c *p) { return TRUE; }
// HD: compiler-generated initialization and deleting destructors.
/* 02077CD4 */
static void bflower_sinit() {
    l_headerStatics.unused = 0;
    l_headerStatics.counter = 0;
    l_headerStatics.initialized = 0;
    l_headerStatics.randomSeed = 0;
    BFlower_headerDtor(&l_headerDtor1);
    l_negativePi = -3.1415927410125732f;
    l_positivePi = 3.1415927410125732f;
    BFlower_headerInit1(&l_headerInit1);
    BFlower_headerDtor(&l_headerDtor2);
    BFlower_headerInit2(&l_headerInit2);
    BFlower_headerDtor(&l_headerDtor3);
    bomb_offset.x = 0.0f;
    bomb_offset.y = 0.0f;
    bomb_offset.z = 0.0f;
}
/* 02077D80 */
static void bflower_string_delete(u8 *p, s32 flags) {
    if (p && (flags & 1))
        BFlower_delete(p);
}
/* 02077D9C */
static void bflower_delete(u8 *p, s32 flags) {
    auto actor = reinterpret_cast<daBFlower_c *>(p);
    if (p) {
        BFlower_transformDtor(&reinterpret_cast<BFlowerBckStorage *>(&actor->mBck2)->__vtbl, 0);
        BFlower_transformDtor(&reinterpret_cast<BFlowerBckStorage *>(&actor->mBck1)->__vtbl, 0);
        BFlower_sphereDtor(&actor->mSph, 2);
        dCcD_Cyl_dt(&actor->mCyl, 2);
        dCcD_Stts_dt(&actor->mStts, 2);
        BFlower_actorDtor(actor, 0);
        if (flags & 1)
            BFlower_delete(p);
    }
}
/* 02077E2C */
static void bflower_string_terminate(u8 *p) {}

/* WWHD glue (tools/verify/autoglue.py): translation unit data
data l_bflowerSave 101F84DC
data l_bflowerResourceControl 101F4F28
data l_sph_src 10190CD8
data l_sph_src2 10190D18
data l_cyl_src 10190D88
data action_tbl 10190D58
data bomb_offset 104619EC
data l_headerStatics 104619DC
data l_negativePi 104619D0
data l_positivePi 104619D4
data l_headerInit1 104619D8
data l_headerInit2 104619D9
data l_headerDtor1 10190DCC
data l_headerDtor2 10190DD8
data l_headerDtor3 10190DE4
str 10008848 "VbakH"
str 100087D4 "d_a_bflower.cpp"
str 100087FC "modelData != NULL"
str 100087E4 "pbck != NULL"
str 100087F0 "pbrk != NULL"
ext 026066C4 void* BFlower_getResource(void*,void*,s32)
ext 0273AA24 void BFlower_panic(const char*,s32,const char*)
ext 0201ADE0 void BFlower_subtractPosition(cXyz*,cXyz*,cXyz*)
ext 028E8DD0 f32 BFlower_squareMagnitude(cXyz*)
ext 028F4384 f32 BFlower_sqrt(f32)
ext 02520540 s32 BFlower_getReverb(s8)
ext 025D5928 fopAc_ac_c* BFlower_createBomb(s32,u32,cXyz*,s8,csXyz*,void*,s32,s32,s32)
ext 0200ECD4 f32 BFlower_addCalc(be<f32>*,f32,f32,f32,f32)
ext 025DE564 s32 BFlower_isExecuting(u32)
ext 025D9D0C void BFlower_setCarryNow(fopAc_ac_c*,s32)
ext 023DE638 void BFlower_setActorKeep(BFlowerCarryController*,fopAc_ac_c*)
ext 025D4ED0 void BFlower_actorCtor(fopAc_ac_c*)
ext 025166F0 void BFlower_sphereCtor(dCcD_Sph*)
ext 027DA984 void BFlower_transformCtor(void*)
ext 028F026C void BFlower_headerDtor(void*)
ext 028ED6F8 void BFlower_headerInit1(void*)
ext 028EAB2C void BFlower_headerInit2(void*)
ext 0273AF40 void BFlower_delete(void*)
ext 027F3628 void BFlower_transformDtor(void*,s32)
ext 02515AE8 void BFlower_sphereDtor(dCcD_Sph*,s32)
ext 025D50BC void BFlower_actorDtor(fopAc_ac_c*,s32)
ext 0200E240 void BFlower_setCollision(cCcS*,dCcD_GObjInf*)
*/
