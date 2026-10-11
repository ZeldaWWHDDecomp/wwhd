/**
 * d_a_item.cpp (WWHD)
 * Item - Field Item
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_item.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "SSystem/SComponent/c_m3d_g_pla.h"
#include "d/actor/d_a_item.h"
#include "f_op/f_op_camera.h"
#include "gc_api/gc_api.h"

enum {
    dItemNo_HEART_CONTAINER_e = 0x08,
    dItemNo_BOMB_5_e = 0x0B,
    dItemNo_ARROW_20_e = 0x11,
    dItemNo_SMALL_KEY_e = 0x15,
    dItemNo_SWORD_e = 0x38,
    dItemNo_SHIELD_e = 0x3B,
    dItemNo_DROPPED_SWORD_e = 0x3D,
    dItemNo_BLUE_JELLY_e = 0x4B,
    dItemNo_GREEN_RUPEE_e = 0x01,
    dItemNo_BLUE_RUPEE_e = 0x02,
    dItemNo_YELLOW_RUPEE_e = 0x03,
    dItemNo_RED_RUPEE_e = 0x04,
    dItemNo_PURPLE_RUPEE_e = 0x05,
    dItemNo_ORANGE_RUPEE_e = 0x06,
    dItemNo_HEART_PIECE_e = 0x07,
    dItemNo_SMALL_MAGIC_e = 0x09,
    dItemNo_LARGE_MAGIC_e = 0x0A,
    dItemNo_BOMB_20_e = 0x0D,
    dItemNo_SILVER_RUPEE_e = 0x0F,
    dItemNo_HD_THROWN_ITEM_e = 0x13,
    dItemNo_TINGLE_BOTTLE_e = 0x14,
    dItemNo_TRIPLE_HEART_e = 0x1E,
    dItemNo_JOY_PENDANT_e = 0x1F,
    dItemNo_DEKU_LEAF_e = 0x34,
    dItemNo_MAGIC_ARROW_e = 0x35,
    dItemNo_LIGHT_ARROW_e = 0x36,
    dItemNo_SKULL_NECKLACE_e = 0x45,
    dItemNo_BOKOBABA_SEED_e = 0x46,
    dItemNo_GOLDEN_FEATHER_e = 0x47,
    dItemNo_KNIGHTS_CREST_e = 0x48,
    dItemNo_RED_JELLY_e = 0x49,
    dItemNo_GREEN_JELLY_e = 0x4A,
};
enum { fpcNm_BST_e = 0xF0, fpcNm_BOOMERANG_e = 0x1B0, fpcNm_HIMO2_e = 0x1BE };

enum {
    daItemAct_0_e = 0x0,
    daItemAct_1_e = 0x1,
    daItemAct_2_e = 0x2,
    daItemAct_3_e = 0x3,
    daItemAct_4_e = 0x4,
    daItemAct_BOSS_DISAPPEAR_e = 0x5,
    daItemAct_6_e = 0x6,
    daItemAct_7_e = 0x7,
    daItemAct_8_e = 0x8,
    daItemAct_9_e = 0x9,
    daItemAct_A_e = 0xA,
    daItemAct_B_e = 0xB,
    daItemAct_BOSS_e = 0xC,
    daItemAct_THROW_TO_PLAYER_e = 0xD,
    daItemAct_FOLLOW_SHIP_e = 0xE,
    daItemAct_NO_COLLECT_e = 0xF,
};

// HD: sound IDs follow the HD audio tables.
enum {
    JA_SE_HEART_PIECE = 0x821,
    JA_SE_LUPY_GET = 0x826,
    JA_SE_BLUE_LUPY_GET = 0x835,
    JA_SE_RED_LUPY_GET = 0x836,
    JA_SE_MAGIC_POT_GET_S = 0x879,
    JA_SE_MAGIC_POT_GET_L = 0x87A,
    JA_SE_CONSUMP_ITEM_GET = 0x827,
    JA_SE_SPOILS_GET = 0x87B,
    JA_SE_ESA_GET = 0x8EC,
    JA_SE_CM_BOSS_HEART_APPEAR = 0x583D,
    JA_SE_OBJ_LNK_SWORD_FALL = 0x6971,
    JA_SE_OBJ_LNK_SHIELD_FALL = 0x6972,
    JA_SE_CM_BST_ARROW_BOUND = 0x6988,
    ITEM_THROWN_WALL_SOUND = 0x6A07,
    ITEM_VANISH_SOUND = 0x6A57,
    ITEM_SPLASH_SOUND = 0x6918,
};

#define DAITEMBASE_VTBL 0x10012158
#define DAITEM_VTBL 0x10011FF8

// HD: the companion static source owns its own gc_tu; this source keeps distinct TU data.
const gc_tu_data item_tu = {
    .objAcch = {0x10011E14, 0x10011E34, 0x10011E24},
    .gndChk = {0x10011D94, 0x10011DA4, 0x10011DC4, 0x10011DB4},
    .gndChkDt = {0x10011D64, 0x10011D84, 0x10011D44},
    .headerStatics = {0x104647F8, 0x101B7A18, 0},
};
const dBgS_GndChk_vt l_lavaVtable = {0x10011DD4, 0x10011DE4, 0x10011E04, 0x10011DF4};
const dBgS_LinChk_vt l_lineVtable = {0x10011E44, 0x10011E54, 0x10011E74, 0x10011E64};
const u32 l_linePolyVtable = 0x10011D34;
const u32 l_cylVtable = 0x10011D24;
static daItemBase_c_m_data l_itemData;
static dCcD_SrcCyl l_cylSource;
static ItemInfoTable l_itemInfo;
static ItemResourceTable l_fieldResources;
static ItemBottleGlobal l_bottleState;
static ItemWaterGlobal l_waterState;
static dPa_levelEcallBack l_smokeCallback;
static ItemBoundSounds l_boundSounds;
static ItemArrowSounds l_arrowBoundSounds;
static ItemModeEntry l_modeProc[] = {
    {&daItem_c::mode_wait},
    {&daItem_c::mode_wait},
    {&daItem_c::mode_water},
};
const daItemBase_c_m_data *daItem_getData() { return &l_itemData; }

// HD: the search is inlined; its process-name key remains a guest stack object.
static fopAc_ac_c *itemSearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C, key.get());
}
static void invalidateItemEmitter(JPABaseEmitter *emitter) {
    ItemEmitter *e = reinterpret_cast<ItemEmitter *>(emitter);
    u32 flags = e->mFlags;
    e->mMaxFrame = -1;
    e->mFlags = flags | 1;
}
static void removeItemCallback(void *callback) {
    ItemCallback *cb = static_cast<ItemCallback *>(callback);
    gabi::call_ptr(cb->mVtable->mRemove, cb);
}
static f32 item_getH(u8 itemNo) { return (f32)dItem_data::getH(itemNo); }

// GHS float-to-unsigned conversion preserves the upper half of the register range.
static inline u32 f2u(f32 v) {
    if (!(v < 2147483648.0f)) {
        return (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
    }
    return (u32)gabi::ftoi(v);
}

static f32 dist_xz(const cXyz *a, const cXyz *b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    return std_sqrtf(PSVECSquareMag(xz));
}

// HD: the original frame supplies an uninitialized landing height; retain that stack slot.
static void throwToTarget(daItem_c *i_this, u32 sp) {
    cXyz *landing = &gabi::at<ItemThrowFrame>(sp)->mLanding; /* cXyz; only x and z are written */
    f32 d1 = dist_xz(&i_this->current.pos, &i_this->mHDTargetPos);
    f32 d2 = dist_xz(&i_this->old.pos, &i_this->mHDTargetPos);
    int n = 0;
    if (d1 < 2000.0f && d2 > 2000.0f) {
        f32 vy = i_this->speed.y;
        f32 groundY = landing->y;
        s16 angleY = i_this->current.angle.y;
        f32 spd = i_this->speedF;
        f32 posY = i_this->current.pos.y;
        f32 grav = i_this->gravity;
        if (!(vy < 0.0f) || !(posY < groundY)) {
            gabi::Local<cXyz> p;
            gabi::Local<cXyz> v;
            p->y = posY;
            p->x = i_this->current.pos.x;
            p->z = i_this->current.pos.z;
            v->y = vy;
            v->x = spd * cM_ssin(angleY);
            v->z = spd * cM_scos(angleY);
            bool fail = false;
            if (posY > groundY) {
                do {
                    PSVECAdd(p, v, p);
                    v->y = v->y + grav;
                    n++;
                    if (n > 100) {
                        fail = true;
                        break;
                    }
                } while (p->y > landing->y);
            }
            if (!fail) {
                landing->x = p->x;
                landing->z = p->z;
                if (n > 0) {
                    f32 d3 = dist_xz(&i_this->home.pos, landing);
                    f32 d4 = dist_xz(&i_this->home.pos, &i_this->mHDTargetPos);
                    f32 cur = i_this->speedF;
                    f32 add = 0.0f;
                    if (d3 < d4 - 100.0f || d3 > d4 + 100.0f) {
                        add = (d4 - d3) / (f32)n;
                    }
                    f32 target = cur + add;
                    i_this->mHDTargetSpeedF = target;
                    if (target > 0.0f) {
                        cLib_chaseF(&i_this->speedF, target, 1.0f);
                    }
                    return;
                }
            }
        }
    }
    f32 target = i_this->mHDTargetSpeedF;
    if (target > 0.0f) {
        cLib_chaseF(&i_this->speedF, target, 1.0f);
    }
}

static inline f32 acch_waterH(daItem_c *i_this) { return i_this->mAcch.m_wtr.m_height; }

static void yoganChk_ct(dBgS_GndChk *c) {
    dBgS_GndChk_ct(c, l_lavaVtable, true);
    c->dBgS_GrpPassChk::mGrp = 4;
}

static void gndChk_dt(dBgS_GndChk *c) {
    dBgS_GndChk *g = c;
    g->cBgS_PolyInfo::__vtbl = item_tu.gndChkDt[0];
    g->dBgS_PolyPassChk::__vtbl = item_tu.gndChkDt[1];
    g->dBgS_GrpPassChk::__vtbl = item_tu.gndChkDt[2];
    itemDestroyCheck(g, 0); /* cBgS_Chk::~cBgS_Chk */
}

/* 0217F1B4 */
f32 daItem_c::getYOffset() {

    switch (m_itemNo) {
    case dItemNo_SHIELD_e:
        return 23.0f;
    case dItemNo_SWORD_e:
        return 20.0f;
    case dItemNo_DROPPED_SWORD_e:
        return 10.0f;
    default:
        return 0.0f;
    }
}

/* 0217F3F0 */
void daItem_c::set_mtx() {

    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    s16 rx = current.angle.x;
    s16 ry = current.angle.y;
    s16 rz = current.angle.z;
    if (m_itemNo == dItemNo_HEART_CONTAINER_e) {
        ry = shape_angle.y;
    }
    {
        gabi::Local<cXyz> p;
        gabi::Local<csXyz> r;
        p->x = pos->x, p->y = pos->y, p->z = pos->z;
        r->x = rx, r->y = ry, r->z = rz;
        set_mtx_base(mpModel, p, r);
    }

    if (isArrow(m_itemNo)) {
        gabi::Local<cXyz> offset;
        offset->x = 5.0f, offset->y = 0.0f, offset->z = 10.0f;
        /* setArrowTrans(current.angle.y, offset) */
        mDoMtx_YrotS(mDoMtx_stack_c::get(), current.angle.y);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset, offset);
        gabi::Local<cXyz> arrowTrans;
        arrowTrans->x = offset->x, arrowTrans->y = offset->y, arrowTrans->z = offset->z;

        PSVECAdd(pos, arrowTrans, pos);
        {
            gabi::Local<cXyz> p;
            gabi::Local<csXyz> r;
            p->x = pos->x, p->y = pos->y, p->z = pos->z;
            r->x = rx, r->y = ry, r->z = rz;
            set_mtx_base(mpModelArrow[0], p, r);
        }

        PSVECAdd(pos, arrowTrans, pos);
        {
            gabi::Local<cXyz> p;
            gabi::Local<csXyz> r;
            p->x = pos->x, p->y = pos->y, p->z = pos->z;
            r->x = rx, r->y = ry, r->z = rz;
            set_mtx_base(mpModelArrow[1], p, r);
        }
    }
}

// HD: Add the HD model rotation to the retail item rotation.
/* 0217F200 */
void daItem_c::set_mtx_base(J3DModel *pModel, cXyz *pos, csXyz *rot) {

    if (!pModel) {
        return;
    }

    J3DModel_setBaseScale(pModel, &scale);

    f32 x = pos->x;
    f32 yOffset = getYOffset();
    mDoMtx_stack_c::transS(x, pos->y + yOffset, pos->z);

    if (isRupee(m_itemNo)) {
        u8 height = dItem_data::getH(m_itemNo);
        mDoMtx_stack_c::transM(0.0f, (f32)height * 0.5f, 0.0f);
    }

    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rot->x, rot->y + mHDRotY, rot->z + mExtraZRot);

    if (isRupee(m_itemNo)) {
        u8 height = dItem_data::getH(m_itemNo);
        mDoMtx_stack_c::transM(0.0f, (f32)(-(s32)height) * 0.5f, 0.0f);
    }

    J3DModel_setBaseTRMtx(pModel, mDoMtx_stack_c::get());
}

// HD: A collision with the ship also collects the item while the player is aboard.
/* 0217E50C */
static void itemGetCallBack(fopAc_ac_c *item_actor, dCcD_GObjInf *item_info,
                            fopAc_ac_c *collided_actor, dCcD_GObjInf *collided_info) {

    daItem_c *item = (daItem_c *)item_actor;
    fopAc_ac_c *link = daPy_getPlayerLinkActorClass();
    if (item && item->checkPlayerGet() && collided_actor) {
        if (collided_actor == link) {
            item->itemGetExecute();
        }
        if (collided_actor &&
            reinterpret_cast<ItemCollisionActor *>(collided_actor)->mName == 0xA5 &&
            (dComIfGp_getPlayerStatus0(0) & 0x10000)) {
            item->itemGetExecute();
        }
    }
}

// HD: No-collect pickups expire after thirty frames; bottles get random rotation; thrown pickups use roof correction.
/* 021800F0 */
void daItem_c::CreateInit() {

    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mAcch.m_flags = mAcch.m_flags & ~(u32)(dBgS_Acch::WATER_NONE |
                                           dBgS_Acch::ROOF_NONE); /* ClrWaterNone, ClrRoofNone */

    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */

    mStts.Init(0, 0xFF, this);
    mCyl.Set(&l_cylSource);
    mCyl.SetStts(&mStts);
    mCyl.mGObjCo.mHitCallback = 0x0217E50C; /* SetCoHitCallback(itemGetCallBack) */

    f32 height = item_getH(m_itemNo);
    f32 radius = (f32)dItem_data::getR(m_itemNo);

    if (scale.x > 1.0f) {
        radius *= scale.x;
        height *= scale.x;
    }
    mCyl.SetR(radius);
    mCyl.SetH(height);

    u32 param = fopAcM_GetParam(this);
    mWaitTimer = daItem_getData()->mWaitTime;
    field_0x650 = speed.y;
    mDisappearTimer = daItem_getData()->mDisappearTime;
    u8 type = daItem_prm::getType(this);
    mItemStatus = STATUS_UNK0;
    mType = type;
    mHDRotY = 0;
    mThrownItemState = THROWN_WAIT;
    if (type == 3 || type == 1) {
        setFlag(FLAG_UNK02);
    }
    mAction = param >> 0x1A;
    mNoCollect = 0;
    if ((param >> 0x1A) == daItemAct_NO_COLLECT_e) { /* HD */
        mAction = daItemAct_4_e;
        mNoCollect = 1;
        clrFlag(FLAG_UNK02);
        actor_status = actor_status | fopAcStts_UNK4000_e;
        mDisappearTimer = 30;
    }

    show();

    if (checkSpecialEffect(m_itemNo) &&
        (m_itemNo != dItemNo_SMALL_KEY_e || checkFlag(FLAG_UNK02))) {
        u16 particleID = getSpecialEffect(m_itemNo);
        dComIfGp_particle_set(particleID, &current.pos, NULL, NULL, 0xFF,
                              (dPa_levelEcallBack *)&mPtclFollowCb);
    }

    if (m_itemNo >= dItemNo_BOMB_5_e && m_itemNo <= dItemNo_BOMB_30_e) {
        mScaleTarget.x = 0.6f, mScaleTarget.y = 0.6f, mScaleTarget.z = 0.6f;
    } else {
        mScaleTarget.x = 1.0f, mScaleTarget.y = 1.0f, mScaleTarget.z = 1.0f;
    }

    s32 sw = daItem_prm::getSwitchNo2(this);
    mSpawnSwitchNo = sw;
    if (sw != 0xFF && !fopAcM_isSwitch(this, sw)) {
        hide();
        setFlag(FLAG_UNK02);
    }
    mCollideSwitchNo = daItem_prm::getSwitchNo(this);

    current.angle.z = 0;
    home.angle.z = 0;
    initAction();

    switch (m_itemNo) {
    case dItemNo_SWORD_e:
    case dItemNo_SHIELD_e:
        actor_status = actor_status | fopAcStts_UNK4000_e;
        break;
    case dItemNo_DROPPED_SWORD_e:
        current.angle.x = 0x4000;
        break;
    }

    if (m_itemNo == dItemNo_TINGLE_BOTTLE_e) {
        current.angle.x = (s16)gabi::ftoi(cM_rndF(4096.0f));
        current.angle.y = (s16)gabi::ftoi(cM_rndF(32768.0f));
        current.angle.z = (s16)gabi::ftoi(cM_rndF(4096.0f));
    } else if (m_itemNo == dItemNo_HD_THROWN_ITEM_e) {
        mAcch.SetRoofCrrHeight(30.0f);
    }
    set_mtx();
    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    if (m_itemNo == dItemNo_HD_THROWN_ITEM_e) {
        return;
    }
    if (itemSearchByName(fpcNm_BST_e)) { /* Gohdan */
        mpParticleEmitter =
            dComIfGp_particle_set(0x81E1 /* ID_AK_SN_BSTKIRAKIRAARROW00 */, &current.pos);
    }
}

// HD: The game-mode settings can suppress heart creation.
/* 021805CC */
cPhs_State daItem_c::_daItem_create() {

    /* fopAcM_ct(this, daItem_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = DAITEMBASE_VTBL;
            dBgS_ObjAcch_ct(&mAcch, item_tu.objAcch);
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, l_cylVtable);
            __vtbl = DAITEM_VTBL;
            itemRippleCtor(mPtclRippleCb); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
            dPa_followEcallBack_ct(&mPtclFollowCb, 0, 0);
            itemSmokeCtor(mPtclSmokeCb, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    m_itemNo = daItem_prm::getItemNo(this);

    if (l_fieldResources.mItems[(u8)m_itemNo].mArcName.v == 0) { /* getFieldArc */
        setLoadError();
        return cPhs_ERROR_e;
    }

    mItemBitNo = daItem_prm::getItemBitNo(this);
    if (m_itemNo != dItemNo_BLUE_JELLY_e) {
        mItemBitNo = mItemBitNo & 0x7F;
        if (fopAcM_isItem(this, mItemBitNo) && mItemBitNo != 0x7F) {
            setLoadError();
            return cPhs_ERROR_e;
        }
    }
    /* HD */
    u32 mode = itemGetGameMode(&gabi::at<ItemSave>(dComIfGs_ea())->mGameMode);
    if (itemDisallowHearts(mode) && isHeart(m_itemNo)) {
        setLoadError();
        return cPhs_ERROR_e;
    }

    cPhs_State phase_state =
        dComIfG_resLoad(&mPhs, (const char *)l_fieldResources.mItems[(u8)m_itemNo].mArcName.get());
    if (phase_state == cPhs_COMPLEATE_e) {
        u32 heap_size = l_fieldResources.mItems[(u8)m_itemNo].mHeapSize; /* getFieldHeapSize */
        if (!fopAcM_entrySolidHeap(this, 0x0218422C /* CheckFieldItemCreateHeap */, heap_size)) {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }

    return phase_state;
}

/* 0217F5B0 */
BOOL daItem_c::_daItem_execute() {

    if (mSpawnSwitchNo != 0xFF && !fopAcM_isSwitch(this, mSpawnSwitchNo)) {
        return TRUE;
    }
    if (mSpawnSwitchNo != 0xFF && fopAcM_isSwitch(this, mSpawnSwitchNo)) {
        show();
    }

    timeCount();

    f32 y = current.pos.y;
    eyePos.y = y;
    eyePos.z = current.pos.z;
    eyePos.x = current.pos.x;
    attention_info.position.copy(current.pos); /* attention_info.position */
    eyePos.y = gabi::fmadds(item_getH(m_itemNo), 0.5f, y);

    switch (mItemStatus) {
    case STATUS_BRING_NEZUMI:
        execBringNezumi();
        break;
    case STATUS_UNK0:
    case STATUS_UNK1:
        mItemStatus = checkActionNow() != 0 ? STATUS_UNK1 : STATUS_UNK0;
    case STATUS_WAIT_MAIN:
        execWaitMain();
        break;
    case STATUS_INIT_NORMAL:
        execInitNormalDirection();
    case STATUS_MAIN_NORMAL:
        execMainNormalDirection();
        break;
    case STATUS_INIT_GET_DEMO:
        execInitGetDemoDirection();
        break;
    case STATUS_WAIT_GET_DEMO:
        execWaitGetDemoDirection();
        break;
    case STATUS_MAIN_GET_DEMO:
        execMainGetDemoDirection();
        break;
    case STATUS_WAIT_BOSS1:
    case STATUS_WAIT_BOSS2:
        execWaitMainFromBoss();
        break;
    }

    animPlay(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    set_mtx();

    return TRUE;
}

/* 0217E6D4 */
void daItem_c::mode_proc_call() {

    if (mType == 1) {
        itemDefaultRotateY();
    } else {
        (this->*l_modeProc[mMode].mProc)();
    }

    if (checkFlag(FLAG_BOOMERANG)) {
        fopAc_ac_c *boomerang = itemSearchByName(fpcNm_BOOMERANG_e);
        if (boomerang) {
            current.pos.copy(boomerang->current.pos);
        } else {
            clrFlag(FLAG_BOOMERANG);
        }
    }

    if (checkFlag(FLAG_HOOK)) {
        fopAc_ac_c *grappling_hook = itemSearchByName(fpcNm_HIMO2_e);
        if (grappling_hook) {
            current.pos.copy(grappling_hook->current.pos);
        } else {
            clrFlag(FLAG_HOOK);
        }
    }

    if (mType == 1 && ((actor_status & fopAcStts_HOOK_CARRY_e) || checkFlag(FLAG_BOOMERANG))) {
        mType = 3;
    }
}

// HD: Tingle Bottles stay visible for thirty extra frames; set the no-cull status.
/* 0217EBAC */
void daItem_c::execInitNormalDirection() {

    daPy_py_c *player = static_cast<daPy_py_c *>(dComIfGp_getPlayer(0));
    f32 hx = player->mHeadTopPos.x;
    f32 hy = player->mHeadTopPos.y + 15.0f;
    f32 hz = player->mHeadTopPos.z;
    current.pos.x = hx;
    current.pos.y = hy;
    current.pos.z = hz;
    current.angle.z = 0;
    current.angle.x = 0;
    scale.x = mScaleTarget.x;
    scale.y = mScaleTarget.y;
    scale.z = mScaleTarget.z;
    mExtraZRot = 0;

    mSimpleExistTimer = daItem_getData()->mSimpleExistTime;
    if (m_itemNo == dItemNo_TINGLE_BOTTLE_e) {
        mSimpleExistTimer = daItem_getData()->mSimpleExistTime + 30;
    }
    actor_status = actor_status | fopAcStts_UNK4000_e;
    speed.x = 0.0f;
    speed.y = daItem_getData()->mGetDemoLaunchSpeed;
    speed.z = 0.0f;
    gravity = daItem_getData()->mGetDemoGravity;

    show();

    mCyl.SetTgType(0);
    mCyl.OffCoSPrmBit(1); /* OffCoSetBit */

    removeItemCallback(mPtclSmokeCb);
    if (mpParticleEmitter) {
        invalidateItemEmitter(mpParticleEmitter);
        mpParticleEmitter = NULL;
    }

    mItemStatus = STATUS_MAIN_NORMAL;
}

/* 0217ECE0 */
void daItem_c::execMainNormalDirection() {

    daPy_py_c *player = static_cast<daPy_py_c *>(dComIfGp_getPlayer(0));
    f32 hz = player->mHeadTopPos.z;
    f32 hy = player->mHeadTopPos.y + 15.0f;
    f32 hx = player->mHeadTopPos.x;
    current.pos.z = hz;
    current.pos.x = hx;
    fopAcM_posMoveF(this, NULL);
    if (current.pos.y < hy) {
        current.pos.y = hy;
    }

    camera_class *camera = dComIfGp_getCamera(0); /* dComIfGp_getCamera(0) */
    current.angle.x = camera->mAngle.x;
    current.angle.y = camera->mAngle.y;
    current.angle.z = camera->mAngle.z;

    mSimpleExistTimer = mSimpleExistTimer - 1;
    if (mSimpleExistTimer < 0) {
        fopAcM_delete(this);
    }
}

// HD: Heart pieces and containers use item-event parameter zero; other items use the sentinel.
/* 0217EDAC */
void daItem_c::orderItemEvent() {

    u16 p = 0xFFFF;
    if (m_itemNo >= 7 && m_itemNo <= 8) {
        p = 0;
    }
    fopAcM_orderItemEvent(this, p);
    eventInfo_onCondition(this, dEvtCnd_CANGETITEM_e);
}

// HD: Create the presentation actor through the HD present-demo API.
/* 0217EE08 */
void daItem_c::execInitGetDemoDirection() {

    daPy_py_c *player = static_cast<daPy_py_c *>(dComIfGp_getPlayer(0));
    fopAc_ac_c *link = daPy_getPlayerLinkActorClass();

    hide();
    removeItemCallback(&mPtclFollowCb);

    if (player == link) {
        orderItemEvent();
        mDemoItemBsPcId = fopAcM_createItemForPresentDemo(&current.pos, m_itemNo, 1, -1,
                                                          current.roomNo, NULL, NULL);
        mItemStatus = STATUS_WAIT_GET_DEMO;
    }
}

/* 0217EEAC */
void daItem_c::execWaitGetDemoDirection() {

    hide();

    if (eventInfo.mCommand == dEvtCmd_INGETITEM_e) { /* eventInfo.checkCommandItem() */
        u32 id = mDemoItemBsPcId;
        mItemStatus = STATUS_MAIN_GET_DEMO;
        if (id != fpcM_ERROR_PROCESS_ID_e) {
            dComIfGp_event_setItemPartnerId(id); /* dComIfGp_event_setItemPartnerId */
        }
    } else {
        orderItemEvent();
    }
}

// HD: Save and grant the item here when its event ends; the parent can also set an event bit.
/* 0217EF28 */
void daItem_c::execMainGetDemoDirection() {

    hide();

    if (itemEndCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_GETITEM")) {
        if (mDemoItemBsPcId != fpcM_ERROR_PROCESS_ID_e) {
            ItemSave *save = gabi::at<ItemSave>(dComIfGs_ea());
            if (m_itemNo == dItemNo_BLUE_JELLY_e) {
                itemOnSwitch(&save->mMemory, (s32)mItemBitNo); /* dSv_memBit_c::onSwitch */
            } else {
                itemOnItem(&save->mSave, (s32)mItemBitNo,
                           (s32)current.roomNo); /* dSv_info_c::onItem */
            }
            if (parentActorID != fpcM_ERROR_PROCESS_ID_e) {
                fopAc_ac_c *parent = fopAcM_SearchByID(parentActorID);
                if (parent && parent && fpcM_GetName(parent) == 0x83) {
                    dComIfGs_onEventBit(0x2E20); /* dSv_event_c::onEventBit */
                }
            }
            execItemGet(m_itemNo);
        }
        dComIfGp_event_reset();
        fopAcM_delete(this);
    }
}

/* 0217E850 */
void daItem_c::execBringNezumi() {

    if (mType != 1) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
    }
    mode_proc_call();
}

/* 0217EA1C */
void daItem_c::execWaitMain() {

    checkGetItem();
    if (mType != 1) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
    }
    mode_proc_call();

    if (!checkFlag(FLAG_UNK02)) {
        f32 scaleStepX = mScaleTarget.x / daItem_getData()->mScaleAnimSpeed;
        f32 scaleStepY = mScaleTarget.y / daItem_getData()->mScaleAnimSpeed;
        f32 scaleStepZ = mScaleTarget.z / daItem_getData()->mScaleAnimSpeed;
        cLib_chaseF(&scale.x, mScaleTarget.x, scaleStepX);
        cLib_chaseF(&scale.y, mScaleTarget.y, scaleStepY);
        cLib_chaseF(&scale.z, mScaleTarget.z, scaleStepZ);
    }

    if (checkItemDisappear() && mWaitTimer == 0) {
        if (mDisappearTimer == 0) {
            fopAcM_delete(this);
        }

        if (m_timer % daItem_getData()->mFlashCycleTime == 0) {
            changeDraw();
        }
    }

    if (!(dItem_data::getFlag(m_itemNo) & 2)) {
        if (mCollideSwitchNo == 0xFF || fopAcM_isSwitch(this, mCollideSwitchNo)) {
            mCyl.SetC(&current.pos);
            dComIfG_Ccsp_Set(&mCyl);
        }
    }
}

/* 0217F134 */
void daItem_c::execWaitMainFromBoss() {

    checkGetItem();
    if (mType != 1) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
    }
    mode_proc_call();

    if (mItemStatus != STATUS_WAIT_BOSS2) {
        scaleAnimFromBossItem();
    }

    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
}

/* 0217F030 */
void daItem_c::scaleAnimFromBossItem() {

    if (m_get_timer <= 30) {
        if (m_get_timer < 30) {
            f32 s = cM_ssin(m_timer * 0x2000 - 0x4000);
            f32 x = (s + s) / (f32)m_timer + 1.0f;
            if (x < 0.0f) {
                x = 0.0f;
            }
            scale.x = x;
            scale.y = x;
            scale.z = x;
            return;
        }
        /* fopAcM_seStart(this, JA_SE_CM_BOSS_HEART_APPEAR, 0) (HD: no NULL checks here) */
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(JA_SE_CM_BOSS_HEART_APPEAR, &eyePos, 0, reverb);
    }
    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;
}

/* 0217E5A4 */
BOOL daItem_c::_daItem_draw() {

    if (chkDraw()) {
        return gabi::call_ptr<BOOL>(gabi::at<ItemBaseVtable>(__vtbl)->mDrawBase, this);
    }
    return TRUE;
}

/* 021809EC */
void daItem_c::setTevStr() {

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

    ItemTevColors *tev = reinterpret_cast<ItemTevColors *>(&tevStr);
    tev->mColorC0.r = 0x96; /* mColorC0.r */
    tev->mColorC0.g = 0x96; /* mColorC0.g */
    tev->mColorC0.b = 0x96; /* mColorC0.b */
    tev->mColorK0.r = 0xFF; /* mColorK0.r */
    tev->mColorK0.g = 0xFF; /* mColorK0.g */
    tev->mColorK0.b = 0xFF; /* mColorK0.b */
    dScnKy_env_light_c *light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);

    for (int i = 0; i < 2; i++) {
        if (!mpModelArrow[i]) {
            continue;
        }
        light = dKy_getEnvlight();
        setLightTevColorType(light, mpModelArrow[i], &tevStr);
    }
}

/* 0217F898 */
BOOL daItem_c::_daItem_delete() {

    dPa_rippleEcallBack_end((dPa_rippleEcallBack *)mPtclRippleCb);
    removeItemCallback(&mPtclFollowCb);
    removeItemCallback(mPtclSmokeCb);
    if (mpParticleEmitter) {
        invalidateItemEmitter(mpParticleEmitter);
        mpParticleEmitter = NULL;
    }

    DeleteBase((const char *)l_fieldResources.mItems[(u8)m_itemNo].mArcName.get());

    return TRUE;
}

// HD: Demo items are saved and granted when the event ends; Tingle Bottles play a named sound.
/* 0217DE18 */
void daItem_c::itemGetExecute() {

    if (mItemStatus == STATUS_INIT_NORMAL) {
        return;
    }
    mItemStatus = STATUS_INIT_NORMAL;

    enum { GET, DEMO, BEAST, BAIT, SAVE };
    int kind = SAVE;
    u32 se = 0;
    u8 idx = 0;
    switch (m_itemNo) {
    case dItemNo_HEART_e:
    case dItemNo_TRIPLE_HEART_e:
        se = JA_SE_HEART_PIECE, kind = GET;
        break;
    case dItemNo_GREEN_RUPEE_e:
        se = JA_SE_LUPY_GET, kind = GET;
        break;
    case dItemNo_BLUE_RUPEE_e:
    case dItemNo_YELLOW_RUPEE_e:
        se = JA_SE_BLUE_LUPY_GET, kind = GET;
        break;
    case dItemNo_RED_RUPEE_e:
    case dItemNo_PURPLE_RUPEE_e:
    case dItemNo_ORANGE_RUPEE_e:
    case dItemNo_SILVER_RUPEE_e:
        se = JA_SE_RED_LUPY_GET, kind = GET;
        break;
    case dItemNo_HEART_PIECE_e:     /* HEART_PIECE */
    case dItemNo_HEART_CONTAINER_e: /* HEART_CONTAINER */
        mDoAud_seStart_id(JA_SE_HEART_PIECE);
        mItemStatus = STATUS_INIT_GET_DEMO;
        clrFlag(FLAG_UNK04);
        mCyl.SetTgType(0);
        mCyl.OffCoSPrmBit(1);
        mCyl.ClrTgHit();
        itemClearCoHit(&mCyl); /* ClrCoHit */
        return;
    case dItemNo_SMALL_MAGIC_e: /* SMALL_MAGIC */
        se = JA_SE_MAGIC_POT_GET_S, kind = GET;
        break;
    case dItemNo_LARGE_MAGIC_e: /* LARGE_MAGIC */
        se = JA_SE_MAGIC_POT_GET_L, kind = GET;
        break;
    case dItemNo_BOMB_5_e:
    case dItemNo_BOMB_10_e:
    case dItemNo_BOMB_20_e:
    case dItemNo_BOMB_30_e: /* BOMB_* */
    case dItemNo_ARROW_10_e:
    case dItemNo_ARROW_20_e:
    case dItemNo_ARROW_30_e: /* ARROW_* */
        se = JA_SE_CONSUMP_ITEM_GET, kind = GET;
        break;
    case dItemNo_TINGLE_BOTTLE_e: /* HD */
        mDoAud_seStartName("SE_TNCL_BOTTLE_GET");
        kind = GET;
        break;
    case dItemNo_JOY_PENDANT_e:
        se = JA_SE_SPOILS_GET, kind = BEAST, idx = 7;
        break;
    case dItemNo_SWORD_e: {
        daItem_c *item = (daItem_c *)itemSearchByName(0xFF /* fpcNm_ITEM_e */);
        if (item && item->m_itemNo == dItemNo_SHIELD_e) {
            item->itemGetExecute();
        }
        kind = DEMO;
        break;
    }
    case dItemNo_SHIELD_e: {
        daItem_c *item = (daItem_c *)itemSearchByName(0xFF /* fpcNm_ITEM_e */);
        if (item && item->m_itemNo == dItemNo_SWORD_e) {
            item->itemGetExecute();
        }
        kind = DEMO;
        break;
    }
    case dItemNo_SMALL_KEY_e:
    case dItemNo_DEKU_LEAF_e: /* DEKU_LEAF */
    case dItemNo_DROPPED_SWORD_e:
        kind = DEMO;
        break;
    case dItemNo_SKULL_NECKLACE_e:
        se = JA_SE_SPOILS_GET, kind = BEAST, idx = 0;
        break;
    case dItemNo_BOKOBABA_SEED_e:
    case dItemNo_GOLDEN_FEATHER_e:
    case dItemNo_KNIGHTS_CREST_e:
    case dItemNo_RED_JELLY_e:
    case dItemNo_GREEN_JELLY_e:
    case dItemNo_BLUE_JELLY_e: /* the other spoils */
        se = JA_SE_SPOILS_GET, kind = BEAST, idx = m_itemNo - dItemNo_SKULL_NECKLACE_e;
        break;
    case dItemNo_BIRD_BAIT_5_e:
        se = JA_SE_ESA_GET, kind = BAIT, idx = 0;
        break;
    case dItemNo_HYOI_PEAR_e:
        se = JA_SE_ESA_GET, kind = BAIT, idx = 1;
        break;
    }

    switch (kind) {
    case GET:
        if (se != 0)
            mDoAud_seStart_id(se);
        execItemGet(m_itemNo);
        break;
    case DEMO:
        mItemStatus = STATUS_INIT_GET_DEMO;
        clrFlag(FLAG_UNK04);
        mCyl.SetTgType(0);
        mCyl.OffCoSPrmBit(1);
        mCyl.ClrTgHit();
        itemClearCoHit(&mCyl); /* ClrCoHit */
        return;
    case BEAST:
        mDoAud_seStart_id(se);
        if (!dComIfGs_isGetItemBeast(idx)) {
            mItemStatus = STATUS_INIT_GET_DEMO;
            dComIfGs_onGetItemBeast(idx);
        } else {
            execItemGet(m_itemNo);
        }
        break;
    case BAIT:
        mDoAud_seStart_id(se);
        if (!dComIfGs_isGetItemBait(idx)) {
            mItemStatus = STATUS_INIT_GET_DEMO;
            dComIfGs_onGetItemBait(idx);
        } else {
            execItemGet(m_itemNo);
        }
        break;
    }

    if (kind == SAVE || mItemStatus != STATUS_INIT_GET_DEMO) {
        /* fopAcM_onItemForIb(mItemBitNo, m_itemNo, current.roomNo) */
        u8 itemNo = m_itemNo;
        s32 roomNo = current.roomNo;
        ItemSave *save = gabi::at<ItemSave>(dComIfGs_ea());
        s32 bitNo = mItemBitNo;
        if (itemNo == dItemNo_BLUE_JELLY_e) {
            itemOnSwitch(&save->mMemory, bitNo); /* dSv_memBit_c::onSwitch */
        } else {
            itemOnItem(&save->mSave, bitNo, roomNo); /* dSv_info_c::onItem */
        }
    }

    clrFlag(FLAG_UNK04);
    mCyl.SetTgType(0);
    mCyl.OffCoSPrmBit(1);
    mCyl.ClrTgHit();
    itemClearCoHit(&mCyl); /* ClrCoHit */
}

/* 0217E6AC */
void daItem_c::itemDefaultRotateY() {

    s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
    fopAcM_addAngleY(this, current.angle.y + rotationSpeed, rotationSpeed);
}

// HD: Actors with the no-cull status bypass the event-running disappearance check.
/* 0217E938 */
BOOL daItem_c::checkItemDisappear() {

    BOOL disappearing = TRUE;
    if (mItemStatus == STATUS_BRING_NEZUMI) {
        disappearing = FALSE;
        show();
    }
    if (checkFlag(FLAG_UNK02)) {
        disappearing = FALSE;
    }
    if (checkFlag(FLAG_UNK10)) {
        disappearing = FALSE;
    }
    if (dItem_data::getFlag(m_itemNo) & 0x01) {
        disappearing = FALSE;
    }
    if (!(actor_status & fopAcStts_UNK4000_e)) {
        if (dComIfGp_event_runCheck()) { /* dComIfGp_event_runCheck() */
            disappearing = FALSE;
        }
    }
    if (mItemStatus == STATUS_UNK4) {
        disappearing = FALSE;
    }
    if (checkFlag(FLAG_BOOMERANG) || checkFlag(FLAG_HOOK) ||
        (actor_status & fopAcStts_HOOK_CARRY_e)) {
        disappearing = FALSE;
        show();
    }
    return disappearing;
}

/* 0218084C */
void daItem_c::setItemTimer(int timer) {

    if (timer == -1) {
        setFlag(FLAG_UNK10);
        return;
    }
    mWaitTimer = timer;
}

// HD: Never collect an item spawned with the no-collect action.
/* 0217DDDC */
BOOL daItem_c::checkPlayerGet() {

    if (mNoCollect != 0) {
        return FALSE;
    }
    if (m_get_timer < daItem_getData()->mNoGetTime) {
        return FALSE;
    }
    if (mItemStatus == STATUS_BRING_NEZUMI) {
        return FALSE;
    }
    return TRUE;
}

// HD: Thrown-to-player items adjust their horizontal speed; no-collect items stop waiting on landing.
/* 02180DE4 */
BOOL daItem_c::itemActionForRupee() {

    auto frame = GuestFrame(0xA8);
    mAcch.CrrPos(dComIfG_Bgsp());
    checkWall();

    if (mAcch.ChkGroundLanding()) {
        if (mNoCollect != 0) {
            mWaitTimer = 0;
        }
        f32 temp2 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp2 > gravity - 0.5f) {
            fopAcM_SetSpeedF(this, 0.0f);
        } else {
            speed.x = 0.0f;
            speed.y = -temp2;
            speed.z = 0.0f;
        }

        mOnGroundTimer = mOnGroundTimer + 1;
        if (mOnGroundTimer >= 2) {
            clrFlag(FLAG_UNK04);
        }

        set_bound_se();
    } else if (mAcch.ChkGroundHit()) {
        if (m_itemNo != dItemNo_TINGLE_BOTTLE_e) {
            itemDefaultRotateY();
        }
        fopAcM_SetSpeedF(this, 0.0f);
        clrFlag(FLAG_UNK04);
        mOnGroundTimer = 1;
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    s16 rotateSpeed = daItem_getData()->mRotateXSpeed;
    mRotateSpeed = rotateSpeed;

    s16 target = 0;
    if (mOnGroundTimer == 0) {
        target = current.angle.x + rotateSpeed;
    }
    mTargetAngleX = target;

    if (!checkFlag(FLAG_UNK02)) {
        cLib_chaseAngleS(&current.angle.x, target, mRotateSpeed);
    }

    if (mAction == daItemAct_THROW_TO_PLAYER_e) {
        throwToTarget(this, frame.sp);
    }

    checkTingleBottleDelete();
    return TRUE;
}

// HD: Thrown-to-player hearts are collected after thirty frames.
/* 021811F4 */
BOOL daItem_c::itemActionForHeart() {

    f32 origSpeedY = speed.y;
    if (origSpeedY < 0.0f) {
        gravity = 0.0f;
        speed.z = 0.0f;
        speed.y = daItem_getData()->mHeartFallSpeed;
        speed.x = 0.0f;
    }

    mAcch.CrrPos(dComIfG_Bgsp());

    if (mAcch.m_flags & (dBgS_Acch::GROUND_LANDING | dBgS_Acch::GROUND_HIT)) {
        speed.x = 0.0f;
        clrFlag(FLAG_UNK04);
        speed.z = 0.0f;
        speedF = 0.0f;
        mExtraZRot = 0;
        speed.y = -1.0f;
        itemDefaultRotateY();
    } else if (origSpeedY < 0.0f) {
        f32 amplitude = daItem_getData()->mHeartAmplitude;
        speedF = cM_ssin(m_timer * daItem_getData()->mHeartFallCycleTime) * amplitude;
    }

    if (mAction == daItemAct_THROW_TO_PLAYER_e && m_timer > 30) {
        itemGetExecute();
    }
    return TRUE;
}

/* 02181338 */
BOOL daItem_c::itemActionForKey() {

    mAcch.CrrPos(dComIfG_Bgsp());
    checkWall();

    if (mAcch.ChkGroundLanding()) {
        f32 temp2 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp2 > gravity - 0.5f) {
            speedF = 0.0f;
            mRotateSpeed = 0;
            current.angle.x = 0x4000;
            mTargetAngleX = 0x4000;
        } else {
            speed.z = 0.0f;
            speed.x = 0.0f;
            speed.y = -temp2;
        }

        mOnGroundTimer = mOnGroundTimer + 1;
        if (mOnGroundTimer >= 2) {
            clrFlag(FLAG_UNK04);
        }
    } else if (mAcch.ChkGroundHit()) {
        current.angle.x = 0;
        mOnGroundTimer = 1;
        mTargetAngleX = 0;
        clrFlag(FLAG_UNK04);
        itemDefaultRotateY();
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    s16 rotateSpeed = daItem_getData()->mRotateXSpeed;
    mRotateSpeed = rotateSpeed;

    s16 target = 0;
    if (mOnGroundTimer == 0) {
        target = current.angle.x + rotateSpeed;
    }
    s16 step = mRotateSpeed;
    mTargetAngleX = target;
    cLib_chaseAngleS(&current.angle.x, target, step);

    return TRUE;
}

/* 021814C0 */
BOOL daItem_c::itemActionForEmono() {

    mAcch.CrrPos(dComIfG_Bgsp());

    if (mAcch.ChkGroundLanding()) {
        f32 temp2 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp2 > gravity - 0.5f) {
            speedF = 0.0f;
        } else {
            speed.x = 0.0f;
            speed.z = 0.0f;
            speed.y = -temp2;
        }

        set_bound_se();
    } else if (mAcch.ChkGroundHit()) {
        s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
        rotationSpeed = rotationSpeed / 2;
        fopAcM_addAngleY(this, current.angle.y + rotationSpeed, rotationSpeed);
        speedF = 0.0f;
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    return TRUE;
}

/* 0218188C */
BOOL daItem_c::itemActionForSword() {

    mAcch.CrrPos(dComIfG_Bgsp());

    bool isQuake = itemCheckQuake(gabi::at<dDetect_c>(dComIfGp_ea() + PLAY_DETECT), &current.pos) !=
                   0; /* chk_quake */
    if (isQuake && !checkFlag(FLAG_QUAKE) && mAcch.ChkGroundHit()) {
        speed.x = 0.0f;
        speed.y = 21.0f;
        speed.z = 0.0f;
        gravity = -3.5f;
    }

    if (mAcch.ChkGroundLanding()) {
        f32 temp = field_0x650 * 0.9f;
        speed.x = 0.0f;
        speed.y = -temp;
        speed.z = 0.0f;

        if (m_itemNo == dItemNo_SWORD_e) {
            if (m_get_timer > 15) {
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(JA_SE_OBJ_LNK_SWORD_FALL /* JA_SE_OBJ_LNK_SWORD_FALL */, &eyePos, 0, reverb);
            }
        } else if (m_itemNo == dItemNo_SHIELD_e) {
            if (m_get_timer > 15) {
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(JA_SE_OBJ_LNK_SHIELD_FALL /* JA_SE_OBJ_LNK_SHIELD_FALL */, &eyePos, 0, reverb);
            }
        }
    }

    gabi::Local<dBgS_GndChk> gndChk; /* dBgS_ObjGndChk */
    dBgS_GndChk_ct(gndChk, item_tu.gndChk, true);
    gabi::Local<cXyz> bottomPos;
    gabi::Local<cXyz> topPos;
    bottomPos->x = 0.0f, bottomPos->y = 0.0f, bottomPos->z = 0.0f;
    topPos->x = 0.0f, topPos->y = 50.0f, topPos->z = 0.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    PSMTXMultVec(mDoMtx_stack_c::get(), bottomPos, bottomPos);
    PSMTXMultVec(mDoMtx_stack_c::get(), topPos, topPos);
    dBgS_GndChk_SetPos(gndChk, bottomPos);
    f32 groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    dBgS_GndChk_SetPos(gndChk, topPos);
    f32 temp3 = groundY - bottomPos->y;
    groundY = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
    f32 temp4 = groundY - topPos->y;

    s16 rot;
    if (field_0x666 == 1) {
        rot = 0x2000;
        field_0x660 = rot;
    } else {
        rot = field_0x660;
    }

    if (temp3 > 0.0f || temp4 > 0.0f) {
        rot = (s16)gabi::ftoi((f32)rot * -0.8f);
        u8 n = field_0x666;
        field_0x660 = rot;
        field_0x666 = n + 1;
    }

    if (rot == 0) {
        field_0x666 = 0;
    } else {
        cLib_addCalcAngleS2(&current.angle.x, rot + 0x4000, 10, 0x800);
    }

    if (isQuake) {
        setFlag(FLAG_QUAKE);
    } else {
        clrFlag(FLAG_QUAKE);
    }

    /* ~dBgS_ObjGndChk (inline): this TU's vtables, then cBgS_Chk::~cBgS_Chk */
    dBgS_GndChk *g = gndChk;
    g->dBgS_PolyPassChk::__vtbl = item_tu.gndChkDt[1];
    g->cBgS_PolyInfo::__vtbl = item_tu.gndChkDt[0];
    g->dBgS_GrpPassChk::__vtbl = item_tu.gndChkDt[2];
    itemDestroyCheck(g, 0);
    return TRUE;
}

// HD: Emitter versions seven and later negate the global translation Y component.
/* 021815E8 */
BOOL daItem_c::itemActionForArrow() {

    mAcch.CrrPos(dComIfG_Bgsp());

    if (mOnGroundTimer == 0 && mpParticleEmitter && itemSearchByName(fpcNm_BST_e)) { /* Gohdan */
        f32 z = current.pos.z;
        f32 y = current.pos.y;
        ItemEmitter *e = reinterpret_cast<ItemEmitter *>((JPABaseEmitter *)mpParticleEmitter);
        f32 x = current.pos.x;
        u8 version = e->mVersion;
        e->mGlobalTranslation.x = x;
        e->mGlobalTranslation.y = y;
        e->mGlobalTranslation.z = z;
        if (version >= 7) {
            e->mGlobalTranslation.y = -(f32)e->mGlobalTranslation.y;
        }
    }

    if (mAcch.ChkGroundLanding()) {
        f32 temp_f3 = field_0x650 * daItem_getData()->mGroundReflect;
        if (temp_f3 > gravity - 0.5f) {
            mOnGroundTimer = mOnGroundTimer + 1;
            speedF = 0.0f;
        } else {
            f32 spd = speedF;
            speed.z = 0.0f;
            speed.x = 0.0f;
            speed.y = -temp_f3;
            mOnGroundTimer = mOnGroundTimer + 1;
            speedF = spd * 0.5f;
        }

        if (mOnGroundTimer == 1 && itemSearchByName(fpcNm_BST_e)) { /* Gohdan */
            s8 roomNo = current.roomNo;
            JPABaseEmitter *emitter =
                dComIfGp_particle_set(0xA1E2 /* ID_AK_ST_BSTDROPARROWSMOKE00 */, &current.pos, NULL,
                                      NULL, 0xFF, (dPa_levelEcallBack *)mPtclSmokeCb, roomNo);
            if (emitter) {
                reinterpret_cast<ItemEmitter *>(emitter)->mMaxFrame = 1; /* setMaxFrame(1) */
            }
        }

        if (mpParticleEmitter) {
            invalidateItemEmitter(mpParticleEmitter);
            mpParticleEmitter = NULL;
        }

        set_bound_se();
    } else if (mAcch.ChkGroundHit()) {
        speedF = 0.0f;
        if (m_itemNo != dItemNo_HEART_CONTAINER_e) {
            itemDefaultRotateY();
        }
    }

    if (m_itemNo == dItemNo_HEART_CONTAINER_e) {
        if (mOnGroundTimer != 0) {
            s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
            cLib_addCalcAngleS(&mRotateSpeed, rotationSpeed, 10, 0x400, 0x100);
        }

        cLib_chaseAngleS(&shape_angle.y, shape_angle.y + mRotateSpeed, mRotateSpeed);
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }

    return TRUE;
}

/* 02180AA4 */
void daItem_c::checkWall() {

    if (!mAcch.ChkWallHit()) {
        return;
    }

    const cM3dGPla *pla = static_cast<const cM3dGPla *>(dBgS_GetTriPla(dComIfG_Bgsp(), &mAcchCir));
    gabi::Local<cXyz> wallNorm;
    wallNorm->x = pla->mNormal.x;
    wallNorm->y = pla->mNormal.y;
    wallNorm->z = pla->mNormal.z;

    gabi::Local<cXyz> vel;
    vel->x = speedF * cM_ssin(current.angle.y);
    vel->y = speed.y;
    vel->z = speedF * cM_scos(current.angle.y);

    const f32 eps = 3.8146973e-06f; /* G_CM3D_F_ABS_MIN */
    if (!(std::fabs((f32)vel->x) < eps) || !(std::fabs((f32)vel->z) < eps)) {
        /* Reflect(wallNorm, &vel, 1.0f, 1.0f) */
        f32 mag = std_sqrtf(PSVECSquareMag(vel));
        gabi::Local<cXyz> tmp;
        gabi::Local<cXyz> moveNorm;
        gabi::Local<cXyz> surfNorm;
        cXyz_normalize(vel, tmp);
        moveNorm->copy(*tmp);
        cXyz_normalize(wallNorm, tmp);
        surfNorm->copy(*tmp);
        cXyz_ml(moveNorm, tmp, -1.0f);
        moveNorm->copy(*tmp);
        f32 dot = PSVECDotProduct(surfNorm, moveNorm);
        dot = dot + dot;
        gabi::Local<cXyz> scaled;
        cXyz_ml(surfNorm, scaled, dot);
        cXyz_mi(scaled, tmp, moveNorm);
        gabi::Local<cXyz> reflectVec;
        reflectVec->copy(*tmp);
        PSVECScale(reflectVec, reflectVec, mag);
        f32 rz = reflectVec->z;
        f32 rx = reflectVec->x;
        f32 ry = reflectVec->y;
        vel->z = rz;
        vel->y = ry;
        vel->x = rx;
        if (!(std::fabs(rx) < eps)) {
            current.angle.y = cM_atan2s(rx, rz);
            if (m_itemNo == dItemNo_HD_THROWN_ITEM_e) {
                f32 speed = std_sqrtf(PSVECSquareMag(vel));
                u32 temp = f2u(speed + speed);
                if (temp > 100) {
                    temp = 100;
                }
                s32 reverb = dComIfGp_getReverb(current.roomNo);
                mDoAud_seStart(ITEM_THROWN_WALL_SOUND, &eyePos, temp, reverb);
            }
        }
    }
}

/* 0218086C */
void daItem_c::set_bound_se() {

    if (m_get_timer < 10) {
        return;
    }

    f32 a = std::fabs(field_0x650);
    u32 temp = f2u(a + a);
    if (temp > 100) {
        temp = 100;
    }

    u8 itemNo = m_itemNo;
    u32 id;
    if (itemNo >= 1 && itemNo <= 0xE) {
        /* rupees, magic, bombs: JA_SE_OBJ_LUPY_BOUND, JA_SE_OBJ_M_POT_BOUND, JA_SE_CM_BST_BOMB_BOUND... */
        id = l_boundSounds.mSound[itemNo];
    } else if (itemNo >= 0x10 && itemNo <= 0x13) {
        id = l_arrowBoundSounds.mSound[itemNo];
    } else if (itemNo >= 0x35 && itemNo <= 0x36) {
        id = JA_SE_CM_BST_ARROW_BOUND; /* JA_SE_CM_BST_ARROW_BOUND (magic/light arrows) */
    } else {
        return;
    }
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(id, &eyePos, temp, reverb);
}

/* 0217E898 */
BOOL daItem_c::checkGetItem() {

    dComIfGp_get();
    if (!checkPlayerGet()) {
        return FALSE;
    }

    if (mCyl.ChkTgHit()) {
        void *hitObj = mCyl.GetTgHitObj();
        if (hitObj) {
            if (cCcD_Obj_ChkAtType(hitObj, 0x2 /* AT_TYPE_SWORD */)) {
                itemGetExecute();
                return TRUE;
            } else if (cCcD_Obj_ChkAtType(hitObj, 0x40 /* AT_TYPE_BOOMERANG */)) {
                setFlag(FLAG_BOOMERANG);
            }
        }
    }

    return FALSE;
}

// HD: No-collect pickups continue counting down while the normal get checks fail.
/* 0217E608 */
BOOL daItem_c::timeCount() {

    m_timer = m_timer + 1;
    if (m_get_timer < 10000 /* m_timer_max */) {
        m_get_timer = m_get_timer + 1;
    }

    if ((checkPlayerGet() && !(dComIfGp_event_runCheck())) || mNoCollect != 0) {
        if (mWaitTimer > 0) {
            mWaitTimer = mWaitTimer - 1;
        } else if (mDisappearTimer > 0) {
            mDisappearTimer = mDisappearTimer - 1;
        }
    }

    return TRUE;
}

/* 02181C4C */
void daItem_c::mode_wait_init() {

    mMode = MODE_WAIT;
    gravity = daItem_getData()->mGravity;
    dPa_rippleEcallBack_end((dPa_rippleEcallBack *)mPtclRippleCb);
}

// HD: Bottles float vertically and can follow the ship; stop turning when ground is above water.
/* 02181C68 */
void daItem_c::waterFloat() {

    mGroundAboveWater = 0;
    f32 x = current.pos.x, z = current.pos.z;
    f32 waterY = acch_waterH(this);
    if (daSea_ChkArea(x, z)) {
        waterY = daSea_calcWave(current.pos.x, current.pos.z);
    }

    if (mAction == daItemAct_FOLLOW_SHIP_e) {
        fopAc_ac_c *player = dComIfGp_getPlayer(0);
        if (player != 0) {
            gabi::Local<cXyz> pos;
            pos->x = player->current.pos.x;
            pos->y = player->current.pos.y;
            pos->z = player->current.pos.z;
            if (!itemWaterPositionTest(l_waterState.mpState, pos.get())) {
                waterY = waterY - 50.0f;
            }
        }
    }

    if (mAcch.GetGroundH() > waterY) {
        speedF = 0.0f;
        mGroundAboveWater = 1;
        return;
    }

    fopAc_ac_c *ship = dComIfGp_getShipActor();
    if (ship != 0 && home.pos.y < 50.0f) {
        fopAc_ac_c *shipAc = ship;
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> xz;
        cXyz_mi(&shipAc->current.pos, d, &home.pos);
        xz->x = d->x, xz->y = 0.0f, xz->z = d->z;
        f32 d1 = PSVECSquareMag(xz);
        cXyz_mi(&current.pos, d, &home.pos);
        gabi::Local<cXyz> xz2;
        xz2->x = d->x, xz2->y = 0.0f, xz2->z = d->z;
        f32 d2 = PSVECSquareMag(xz2);
        if (d1 > 6250000.0f || d2 > 6250000.0f) {
            cLib_addCalc0(&speedF, 0.1f, 0.5f);
        } else {
            gabi::Local<dBgS_LinChk> linChk;
            dBgS_LinChk_ct(linChk, l_lineVtable, false);
            dBgS_LinChk_Set(linChk, &current.pos, &shipAc->current.pos, this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                cLib_addCalc2(&speedF, 0.075f * shipAc->speedF, 0.1f, 0.5f);
            } else {
                speedF = 0.0f;
            }
            /* ~dBgS_LinChk (inline) */
            dBgS_LinChk *c = linChk;
            c->dBgS_PolyPassChk::__vtbl = l_lineVtable.v58;
            c->dBgS_GrpPassChk::__vtbl = item_tu.gndChkDt[2];
            c->cBgS_PolyInfo::__vtbl = l_linePolyVtable;
            cBgS_LinChk_dt(linChk, 0);
        }
        s16 angleY = current.angle.y;
        current.angle.y = cLib_targetAngleY(&current.pos, &shipAc->current.pos);
        fopAcM_posMoveF(this, NULL);
        current.angle.y = angleY;
    }

    f32 g = gravity;
    f32 py = current.pos.y;
    f32 v = (std::fabs(g) * (waterY - py)) / 7.5f;
    mWaterAcceleration = v;
    if (mAction == daItemAct_FOLLOW_SHIP_e && v < 0.0f && waterY < py + 15.0f) {
        v = v + v;
        mWaterAcceleration = v;
    }

    f32 vy = speed.y + v;
    if (waterY < py) {
        if (vy < -10.0f) {
            vy = -10.0f;
        } else if (vy > 6.0f) {
            vy = 6.0f;
        }
    } else {
        if (vy < -6.0f) {
            vy = -6.0f;
        } else if (vy > 10.0f) {
            vy = 10.0f;
        }
    }
    if (waterY + 20.0f < py) {
        if (vy > 0.0f) {
            vy = 0.0f;
        }
    }
    if (py < waterY - 20.0f && vy < 0.0f) {
        speed.y = 0.0f;
    } else {
        speed.y = vy;
    }
}

// HD: Bottles grow and simulate a random number of floating frames on first water entry.
/* 0218214C */
void daItem_c::mode_water_init() {

    f32 z = current.pos.z, x = current.pos.x;
    if (mNoCollect != 0) {
        mWaitTimer = 0;
    }
    mMode = MODE_WATER;

    if (daSea_ChkArea(x, z)) {
        f32 seaH = daSea_calcWave(current.pos.x, current.pos.z);
        if (seaH > current.pos.y) {
            current.pos.y = seaH;
        }
    } else {
        if (!mAcch.ChkWaterHit() || acch_waterH(this) < current.pos.y) {
            mode_wait_init();
        }
        current.pos.y = acch_waterH(this);
    }

    mExtraZRot = 0;
    mRotateSpeed = 0;
    speed.x = 0.0f;
    speed.y = 0.0f;
    speed.z = 0.0f;
    speedF = 0.0f;
    current.angle.z = 0;
    current.angle.x = 0;
    clrFlag(FLAG_UNK04);

    if (m_itemNo == dItemNo_TINGLE_BOTTLE_e) {
        PSVECScale(&mScaleTarget, &mScaleTarget, 1.2f);
        f32 k = scale.x * 1.2f;
        f32 height = item_getH(m_itemNo) * k;
        f32 radius = (f32)dItem_data::getR(m_itemNo) * k;
        mCyl.SetR(radius);
        mCyl.SetH(height);
    }

    f32 sx = mScaleTarget.x;
    scale.x = sx;
    scale.y = mScaleTarget.y;
    scale.z = mScaleTarget.z;

    gabi::Local<cXyz> particleScale;
    f32 temp3 = (f32)l_itemInfo.mItems[(u8)m_itemNo].mShadowSize /
                (f32)l_itemInfo.mItems[dItemNo_GREEN_RUPEE_e].mShadowSize;
    temp3 *= sx;
    particleScale->x = temp3;
    particleScale->y = temp3;
    particleScale->z = temp3;

    /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, ...) */
    dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, NULL, particleScale, 0xFF,
                    (dPa_levelEcallBack *)mPtclRippleCb, -1, NULL, NULL, NULL);

    if (m_itemNo == dItemNo_TINGLE_BOTTLE_e) {
        gravity = -0.2f;
        if (mWaterFloatInitialized == 0) {
            s16 r = (s16)gabi::ftoi(cM_rndFX(80.0f));
            u8 action = mAction;
            current.angle.x = r + 0x400;
            if (action != daItemAct_FOLLOW_SHIP_e) {
                s32 n = gabi::ftoi(cM_rndF(20.0f));
                if (n > 0) {
                    do {
                        speed.y = speed.y + -0.2f;
                        waterFloat();
                        current.pos.y = current.pos.y + speed.y;
                    } while (--n != 0);
                }
            }
            mWaterFloatInitialized = 1;
        }
    }
    reinterpret_cast<ItemRipple *>(mPtclRippleCb)->mRate = 0.0f; /* mPtclRippleCb.mRate */
}

/* 02182C80 */
void daItem_c::mode_wait() {

    if (checkFlag(FLAG_UNK04) && itemCheckAppearEffect(m_itemNo) /* checkAppearEffect */) {
        u16 appearEffect = itemGetAppearEffect(m_itemNo); /* getAppearEffect */
        dComIfGp_particle_setSimple(appearEffect, &current.pos);
    }

    switch (m_itemNo) {
    case dItemNo_HEART_e:
    case dItemNo_TRIPLE_HEART_e:
        itemActionForHeart();
        break;
    case dItemNo_HEART_PIECE_e:
    case dItemNo_HEART_CONTAINER_e: /* HEART_PIECE, HEART_CONTAINER */
    case dItemNo_BOMB_5_e:
    case dItemNo_BOMB_10_e:
    case dItemNo_BOMB_20_e:
    case dItemNo_BOMB_30_e: /* BOMB_* */
    case dItemNo_ARROW_10_e:
    case dItemNo_ARROW_20_e:
    case dItemNo_ARROW_30_e: /* ARROW_* */
    case dItemNo_MAGIC_ARROW_e:
    case dItemNo_LIGHT_ARROW_e: /* MAGIC_ARROW, LIGHT_ARROW */
        itemActionForArrow();
        break;
    case dItemNo_SMALL_KEY_e:
        itemActionForKey();
        break;
    case dItemNo_SMALL_MAGIC_e:
    case dItemNo_LARGE_MAGIC_e: /* SMALL_MAGIC, LARGE_MAGIC */
    case dItemNo_JOY_PENDANT_e:
    case dItemNo_SKULL_NECKLACE_e:
    case dItemNo_BOKOBABA_SEED_e:
    case dItemNo_GOLDEN_FEATHER_e:
    case dItemNo_KNIGHTS_CREST_e:
    case dItemNo_RED_JELLY_e:
    case dItemNo_GREEN_JELLY_e:
    case dItemNo_BLUE_JELLY_e: /* spoils */
        itemActionForEmono();
        break;
    case dItemNo_SWORD_e:
    case dItemNo_SHIELD_e:
    case dItemNo_DROPPED_SWORD_e:
        itemActionForSword();
        break;
    case dItemNo_HD_THROWN_ITEM_e: /* HD */
        itemActionForThrownItem();
        return;
    default:
        itemActionForRupee();
        break;
    }

    if ((mAcch.ChkWaterHit() && acch_waterH(this) > current.pos.y) ||
        (daSea_ChkArea(current.pos.x, current.pos.z) &&
         daSea_calcWave(current.pos.x, current.pos.z) > current.pos.y)) {
        mode_water_init();
    }

    gabi::Local<dBgS_GndChk> lavaChk; /* dBgS_ObjGndChk_Yogan */
    yoganChk_ct(lavaChk);
    dBgS_GndChk *c = lavaChk;
    c->m_pos.z = old.pos.z;
    c->m_pos.y = old.pos.y;
    c->m_pos.x = old.pos.x;
    f32 lavaY = cBgS_GroundCross(dComIfG_Bgsp(), lavaChk);
    if (lavaY != -1e9f && lavaY > current.pos.y) {
        fopAcM_delete(this);
    }
    gndChk_dt(lavaChk);
}

// HD: Bottles bob independently of the retail water height snap.
/* 02182B70 */
void daItem_c::mode_water() {

    if (m_itemNo == dItemNo_TINGLE_BOTTLE_e) {
        waterFloat();
    }
    mAcch.CrrPos(dComIfG_Bgsp());

    if (m_itemNo != dItemNo_TINGLE_BOTTLE_e) {
        if (daSea_ChkArea(current.pos.x, current.pos.z)) {
            f32 seaH = daSea_calcWave(current.pos.x, current.pos.z);
            if (seaH < current.pos.y) {
                mode_wait_init();
            } else {
                current.pos.y = seaH;
            }
        } else if (!mAcch.ChkWaterHit() || acch_waterH(this) < current.pos.y) {
            mode_wait_init();
        } else {
            current.pos.y = acch_waterH(this);
        }
    }

    if (mGroundAboveWater == 0) {
        s16 rotationSpeed = 0xFFFF / daItem_getData()->mRotateYSpeed;
        fopAcM_addAngleY(this, current.angle.y + rotationSpeed, rotationSpeed);
    }
    checkTingleBottleDelete();
}

// HD: Initial vertical speed is zero; the thrown-to-player action computes its flight and target.
/* 0217F93C */
BOOL daItem_c::initAction() {

    const daItemBase_c_m_data *data = daItem_getData();
    f32 y_speed = 0.0f;

    if (checkFlag(FLAG_UNK02)) {
        u8 action = mAction;
        scale.x = mScaleTarget.x;
        f32 ty = mScaleTarget.y, tz = mScaleTarget.z;
        scale.y = ty;
        scale.z = tz;

        switch (action) {
        case daItemAct_4_e: {
            current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
            f32 temp = data->field_0x2C + cM_rndF(5.0f);
            speedF = cM_rndF(data->field_0x30);
            speed.x = 0.0f;
            speed.y = temp;
            speed.z = 0.0f;
            break;
        }
        case daItemAct_BOSS_DISAPPEAR_e:
            speed.x = 0.0f, speed.y = 0.0f;
            actor_status = actor_status | fopAcStts_UNK4000_e;
            speed.z = 0.0f;
            mRotateSpeed = 0x4A8;
            speedF = 0.0f;
            scale.x = 0.0f, scale.y = 0.0f, scale.z = 0.0f;
            mItemStatus = STATUS_WAIT_BOSS1;
            break;
        case daItemAct_BOSS_e:
            mRotateSpeed = 0x4A8;
            actor_status = actor_status | fopAcStts_UNK4000_e;
            scale.x = 1.0f, scale.y = 1.0f, scale.z = 1.0f;
            mItemStatus = STATUS_WAIT_BOSS2;
            break;
        }

        u8 flag = mFlag;
        mMode = MODE_WAIT;
        gravity = data->mGravity;
        mFlag = flag & ~FLAG_UNK04;
        return TRUE;
    }

    switch (mAction) {
    case daItemAct_1_e: {
        f32 r = cM_rndFX(5.0f);
        f32 speedH = data->mSpeedH;
        speedF = speedH / 10.0f;
        y_speed = data->mLaunchSpeed + r;
        if (itemStickValue(0) != 0.0f) { /* CPad_GET_STICK_VALUE(0) */
            speedF = speedH;
        }
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        break;
    }
    case daItemAct_3_e:
        y_speed = 25.0f;
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        speedF = data->mVelocityScale;
        break;
    case daItemAct_7_e:
        speedF = data->mVelocityScale * 1.5f;
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
        break;
    case daItemAct_2_e:
    case daItemAct_4_e:
    case daItemAct_9_e:
        speedF = 0.0f;
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
        break;
    case daItemAct_8_e: {
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        f32 r = cM_rndFX(5.0f);
        f32 velocity = data->mVelocityScale;
        y_speed = data->field_0x44 + r;
        speedF = velocity;
        break;
    }
    case daItemAct_A_e:
        scale.x = 0.0f;
        gravity = data->mGravity;
        mMode = MODE_WAIT;
        scale.y = 0.0f, scale.z = 0.0f;
        break;
    case daItemAct_B_e:
        current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
        speedF = 0.0f;
        y_speed = 0.0f;
        break;
    case daItemAct_6_e:
        y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
        break;
    case daItemAct_THROW_TO_PLAYER_e: /* HD: thrown to the player */
        if (isHeart(m_itemNo)) {
            speedF = 0.0f;
            current.angle.y = (s16)gabi::ftoi(cM_rndF(65535.0f));
            y_speed = data->mLaunchSpeed + cM_rndFX(5.0f);
            mHDTargetSpeedF = -1.0f;
        } else {
            mAcch.CrrPos(dComIfG_Bgsp());
            if (mAcch.ChkWaterHit() && acch_waterH(this) > current.pos.y) {
                current.pos.y = acch_waterH(this) + 30.0f;
            } else if (daSea_ChkArea(current.pos.x, current.pos.z) &&
                       daSea_calcWave(current.pos.x, current.pos.z) > current.pos.y) {
                current.pos.y = daSea_calcWave(current.pos.x, current.pos.z) + 30.0f;
            }
            gabi::Local<csXyz> unused;
            csXyz_ct(unused, 0, 0, 0);
            fopAc_ac_c *player = dComIfGp_getPlayer(0);
            gabi::Local<cXyz> target;
            f32 px = player->current.pos.x;
            target->x = px;
            f32 py = player->current.pos.y;
            target->y = py;
            f32 pz = player->current.pos.z;
            target->z = pz;
            f32 tx = gabi::fmadds(player->speed.x, 45.0f, px);
            f32 ty = py + 100.0f;
            target->x = tx;
            target->y = ty;
            f32 tz = gabi::fmadds(player->speed.z, 45.0f, pz);
            mHDTargetPos.x = tx;
            mHDTargetPos.y = ty;
            mHDTargetPos.z = tz;
            target->z = tz;
            current.angle.y =
                itemTargetAngleYHD(&current.pos, target.get()); /* cLib_targetAngleY */
            gabi::Local<cXyz> d;
            cXyz_mi(target, d, &current.pos);
            gabi::Local<cXyz> xz;
            xz->x = d->x, xz->y = 0.0f, xz->z = d->z;
            f32 dist = std_sqrtf(PSVECSquareMag(xz));
            s32 n = 45;
            f32 spd = dist / 45.0f;
            if (spd > 30.0f) {
                spd = 30.0f;
                n = gabi::ftoi(dist / 30.0f);
            }
            f32 dy = target->y - current.pos.y;
            u32 sum = 0; /* wraps like the console's 32-bit add */
            for (s32 i = 1; i <= n; i++) {
                sum += (u32)i;
            }
            f32 fn = (f32)n;
            f32 fsum = (f32)(s32)sum;
            mHDTargetSpeedF = -1.0f;
            speed.y = 30.0f;
            speedF = spd;
            gravity = gabi::fnmsubs(30.0f, fn, dy) / fsum;
        }
        break;
    case daItemAct_0_e:
    case daItemAct_BOSS_DISAPPEAR_e:
    default:
        break;
    }

    mExtraZRot = 0;

    if (isHeart(m_itemNo)) {
        speedF = speedF + speedF;
        mExtraZRot = (s16)gabi::ftoi(cM_rndFX((f32)data->mHeartTilt));
    }

    u8 action = mAction;
    if (m_itemNo == dItemNo_HD_THROWN_ITEM_e) {
        mThrownItemState = THROWN_WAIT;
        actor_status = actor_status | fopAcStts_UNK4000_e;
        mCyl.SetTgType(0);
        mCyl.OffCoSPrmBit(1);
    }
    if (action != daItemAct_THROW_TO_PLAYER_e || isHeart(m_itemNo)) {
        gravity = data->mGravity;
        speed.x = 0.0f;
        speed.y = y_speed;
        speed.z = 0.0f;
    }
    scale.x = 0.0f, scale.y = 0.0f;
    u8 flag = mFlag;
    scale.z = 0.0f;
    mMode = MODE_WAIT;
    mFlag = flag | FLAG_UNK04;

    return TRUE;
}

// HD: Tingle Bottles vanish in smoke when the bottle manager requests deletion.
/* 02180D2C */
void daItem_c::checkTingleBottleDelete() {

    if (m_itemNo == dItemNo_TINGLE_BOTTLE_e && l_bottleState.mpState->mDeleteBottle != 0) {
        dComIfGp_particle_set(0x1C, &current.pos, NULL, NULL, 0xFF, &l_smokeCallback);
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(ITEM_VANISH_SOUND, &current.pos, 0, reverb);
        fopAcM_delete(this);
    }
}

// HD: The thrown pickup has its own launch, bounce, splash and disappearance state machine.
/* 02182460 */
BOOL daItem_c::itemActionForThrownItem() {

    switch (mThrownItemState) {
    case THROWN_WAIT:
        scale.x = 1.0f, scale.y = 1.0f, scale.z = 1.0f;
        gravity = 0.0f;
        speed.x = 0.0f, speed.y = 0.0f, speed.z = 0.0f;
        speedF = 0.0f;
        break;
    case THROWN_LAUNCH: {
        scale.x = 1.0f;
        field_0x650 = 50.0f;
        scale.y = 1.0f, scale.z = 1.0f;
        speedF = 30.0f;
        fopAc_ac_c *player = dComIfGp_getPlayer(0);
        if (player != 0) {
            current.angle.y = player->shape_angle.y; /* shape_angle.y */
        }
        gravity = daItem_getData()->mGravity * 0.75f;
        speed.x = 0.0f, speed.y = 50.0f, speed.z = 0.0f;
        mRotateSpeed = (s16)gabi::ftoi((f32)daItem_getData()->mRotateXSpeed * 0.5f);
        player = dComIfGp_getPlayer(0);
        if (player != 0) {
            gabi::call_ptr(gabi::at<ItemPlayerVtable>(player->__vtbl)->mItemAction, player, 0x11);
        }
        mThrownItemState = THROWN_FLY;
        mThrownItemTimer = 90;
        break;
    }
    case THROWN_FLY: {
        mAcch.CrrPos(dComIfG_Bgsp());
        checkWall();
        s16 step = mRotateSpeed;
        s16 target = current.angle.x + step;
        mTargetAngleX = target;
        cLib_chaseAngleS(&current.angle.x, target, step);
        u8 onGround = mOnGroundTimer;
        scale.x = 1.0f, scale.y = 1.0f, scale.z = 1.0f;
        bool vanish = false;
        if (onGround != 0 && (!(field_0x650 > -gravity) || mAcch.ChkGroundHit())) {
            vanish = true;
        } else {
            mThrownItemTimer = mThrownItemTimer - 1;
            if (mThrownItemTimer == 0 || mAcch.ChkRoofHit()) {
                vanish = true;
            }
        }
        if (vanish) {
            mThrownItemState = THROWN_VANISH;
            break;
        }
        if ((mAcch.ChkWaterHit() && acch_waterH(this) > current.pos.y) ||
            (daSea_ChkArea(current.pos.x, current.pos.z) &&
             daSea_calcWave(current.pos.x, current.pos.z) > current.pos.y)) {
            mThrownItemState = THROWN_SPLASH;
            break;
        }
        gabi::Local<dBgS_GndChk> lavaChk;
        yoganChk_ct(lavaChk);
        dBgS_GndChk *c = lavaChk;
        c->m_pos.y = old.pos.y;
        c->m_pos.x = old.pos.x;
        c->m_pos.z = old.pos.z;
        f32 lavaY = cBgS_GroundCross(dComIfG_Bgsp(), lavaChk);
        if (lavaY != -1e9f && lavaY > current.pos.y) {
            c->dBgS_GrpPassChk::__vtbl = item_tu.gndChkDt[2];
            c->dBgS_PolyPassChk::__vtbl = item_tu.gndChkDt[1];
            c->cBgS_PolyInfo::__vtbl = item_tu.gndChkDt[0];
            mThrownItemState = THROWN_VANISH;
            itemDestroyCheck(c, 0);
            break;
        }
        if (mAcch.ChkGroundLanding()) {
            f32 f = field_0x650 * 0.75f;
            mOnGroundTimer = mOnGroundTimer + 1;
            speed.x = 0.0f;
            speed.y = -f;
            speed.z = 0.0f;
            set_bound_se();
        }
        gndChk_dt(lavaChk);
        break;
    }
    case THROWN_VANISH: {
        hide();
        gravity = 0.0f;
        speed.x = 0.0f, speed.y = 0.0f;
        s16 ax = current.angle.x;
        speed.z = 0.0f;
        mTargetAngleX = ax;
        mRotateSpeed = 0;
        speedF = 0.0f;
        dComIfGp_particle_set(0x1C, &current.pos, NULL, NULL, 0xFF, &l_smokeCallback);
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(ITEM_VANISH_SOUND, &current.pos, 0, reverb);
        mThrownItemTimer = 0x1C;
        mThrownItemState = THROWN_SMOKE;
        break;
    }
    case THROWN_SMOKE:
        mThrownItemTimer = mThrownItemTimer - 1;
        if (mThrownItemTimer == 0) {
            mThrownItemState = THROWN_DELETE;
        }
        break;
    case THROWN_DELETE:
        fopAcM_delete(this);
        break;
    case THROWN_SPLASH: {
        s16 angleY = current.angle.y;
        f32 spd = speedF;
        mode_water_init();
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(ITEM_SPLASH_SOUND, &eyePos, 0, reverb);
        fopKyM_createWpillar(&current.pos, 0.5f, 1.0f, 0);
        current.angle.y = angleY;
        f32 f = field_0x650;
        mHDRotY = 0;
        speedF = spd * 0.75f;
        speed.x = 0.0f;
        speed.y = f * 0.07f;
        speed.z = 0.0f;
        f32 r = cM_rndF(256.0f);
        mMode = MODE_WAIT;
        mThrownItemState = THROWN_FLOAT;
        s16 ax = (s16)gabi::ftoi(r) + 0x900;
        current.angle.x = ax;
        mTargetAngleX = ax;
        mThrownItemTimer = 0x2D;
        mRotateSpeed = 0;
        gravity = -0.5f;
        break;
    }
    case THROWN_FLOAT: {
        f32 x = current.pos.x, z = current.pos.z;
        f32 waterY = acch_waterH(this);
        f32 py = current.pos.y;
        if (daSea_ChkArea(x, z)) {
            waterY = daSea_calcWave(current.pos.x, current.pos.z);
        }
        f32 g = gravity;
        f32 spd = speedF;
        f32 v = (std::fabs(g) * (waterY - py)) / 7.5f;
        speedF = spd * 0.95f;
        mWaterAcceleration = v;
        speed.y = (speed.y + v) * 0.95f;
        mAcch.CrrPos(dComIfG_Bgsp());
        cLib_chaseAngleS(&mHDRotY, mHDRotY + 0x888, 0x888);
        u16 rs = (u16)mRotateSpeed;
        f32 sp = speedF;
        s16 k = (s16)gabi::ftoi(cM_ssin(rs));
        u8 timer = mThrownItemTimer;
        s16 target = mTargetAngleX;
        mRotateSpeed = rs + 0xCCC;
        timer = timer - 1;
        mThrownItemTimer = timer;
        current.angle.x = target * k;
        if (!(sp > 3.0f) || timer == 0) {
            mThrownItemState = THROWN_VANISH;
        }
        break;
    }
    }

    if (speed.y != 0.0f) {
        field_0x650 = speed.y;
    }
    return TRUE;
}

/* 0217E604 */
static BOOL daItem_Draw(daItem_c *i_this) { return i_this->_daItem_draw(); }

/* 0217F88C */
static BOOL daItem_Execute(daItem_c *i_this) { return i_this->_daItem_execute(); }

/* 0217F890 */
static BOOL daItem_IsDelete(daItem_c *i_this) { return TRUE; }

/* 0217F938 */
static BOOL daItem_Delete(daItem_c *i_this) { return i_this->_daItem_delete(); }

/* 02180848 */
static cPhs_State daItem_Create(fopAc_ac_c *i_this) {

    return ((daItem_c *)i_this)->_daItem_create();
}

// HD: compiler-generated and template code of this translation unit.

/* 02182FB0 */
static void __sinit_d_a_item_cpp() {

    sinit_header_statics(item_tu.headerStatics[0], item_tu.headerStatics[1]);
}

/* 02183044 */
static void daItem_c_dt(daItem_c *i_this, s32 flags) {

    if (i_this != nullptr) {
        itemBaseDestructor(i_this, 0); /* daItemBase_c::~daItemBase_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue
ext 0200F958 s16 itemTargetAngleYHD(cXyz* from, cXyz* to)
ext 025D677C void fopAcM_addAngleY(fopAc_ac_c* actor, s16 target, s16 step)
ext 025D7B98 void fopAcM_orderItemEvent(fopAc_ac_c* actor, u16 param)
ext 025D7DEC u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale)
ext 0254457C BOOL itemEndCheckOld(dEvent_manager_c* manager, const char* name)
ext 02551C18 BOOL checkSpecialEffect(u8 itemNo)
ext 02551C48 u16 getSpecialEffect(u8 itemNo)
ext 025E1988 void mDoAud_seStart_id(u32 id)
ext 02030B38 void mDoAud_seStartName(const char* name)
ext 0251641C void itemClearCoHit(dCcD_Cyl* cyl)
ext 025B8CE0 void itemOnSwitch(be<u8>* memory, s32 bit)
ext 025BA384 void itemOnItem(be<u8>* save, s32 bit, s32 room)
ext 0201B31C void cXyz_normalize(const cXyz* vector, cXyz* result)
ext 02529CE8 BOOL itemCheckQuake(dDetect_c* detect, cXyz* pos)
ext 02008DAC void itemDestroyCheck(dBgS_GndChk* check, s32 flags)
ext 02037F80 BOOL itemWaterPositionTest(be<u8>* state, cXyz* pos)
ext 02551B90 BOOL itemCheckAppearEffect(u8 itemNo)
ext 02551BBC u16 itemGetAppearEffect(u8 itemNo)
ext 020079B4 f32 itemStickValue(s32 port)
ext 025A9084 void itemRippleCtor(void* callback)
ext 025A5B18 void itemSmokeCtor(void* callback, s32 mode)
ext 027200D0 u32 itemGetGameMode(be<u8>* save)
ext 0271FC5C BOOL itemDisallowHearts(u32 mode)
ext 021832F4 void itemBaseDestructor(daItem_c* item, s32 flags)
data l_itemData 100121B0
data l_cylSource 10011FB4
data l_itemInfo 101E8674
data l_fieldResources 101E6A74
data l_bottleState 1018F4AC
data l_waterState 1018F450
data l_smokeCallback 1047B274
data l_boundSounds 10011F26
data l_arrowBoundSounds 10011F00
data l_modeProc 10011E98
str 10011E84 "SE_TNCL_BOTTLE_GET"
str 10011EB8 "DEFAULT_GETITEM"
*/
