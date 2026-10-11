/**
 * d_a_ship_create.cpp (WWHD)
 * King of Red Lions: createHeap, create and the HD helpers they use.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ship.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_ship_create.h"

static u32 resourceControl;
static u32 arrayHeaderSize;
static ShipCreateResourceArc resourceArc;
static dCcD_SrcCyl cyl_src;
static dCcD_SrcSph sph_src;
static dCcD_SrcSph rope_sph_src;
static cXyz tiller_top_offset;

// HD: resource names use this ship translation unit's SafeString vtable.
static inline void* ship_getResName(const char* arc, const char* name) {
    gabi::Local<daShip_SafeString> a;
    gabi::Local<daShip_SafeString> n;
    a->mStr = gabi::guest_addr(arc);
    a->__vtbl = SAFESTRING_VTBL;
    n->mStr = gabi::guest_addr(name);
    n->__vtbl = SAFESTRING_VTBL;
    return ShipCreate_getResource(resourceControl, a.get(), n.get());
}
static inline void* ship_getResIdx(const char* arc, s32 idx) {
    return dComIfG_getObjectRes(arc, idx, SAFESTRING_VTBL);
}

/* PowerPC slw/srw: shift amounts 32..63 give 0 */
static inline u32 ppc_slw(u32 x, u32 n) {
    n &= 63;
    return n >= 32 ? 0 : x << n;
}
static inline u32 ppc_srw(u32 x, u32 n) {
    n &= 63;
    return n >= 32 ? 0 : x >> n;
}

// HD: replace the ending ship textures with their Demo46 versions and mark the entries.
/* 0247AF98 */
static void daShip_replaceSeaETextures(J3DModelData* data) {
    if (!ship_isStartStage(gabi::guest_addr("sea_E"))) {
        return;
    }
    ShipCreateTextureTable* tex = reinterpret_cast<ShipCreateModelTextures*>(data)->mTexture;
    u32 names = reinterpret_cast<ShipCreateModelTextures*>(data)->mTextureName;
    for (u16 i = 0; i < tex->mNum; i++) {
        gabi::Local<daShip_SafeString> name;
        name->mStr = gabi::guest_addr(ShipCreate_getTextureName(names, (u32)i));
        name->__vtbl = SAFESTRING_VTBL;
        gabi::Local<daShip_SafeString> b1;
        b1->mStr = gabi::guest_addr("fn_head1");
        b1->__vtbl = SAFESTRING_VTBL;
        /* the first comparison: name's first assureTermination is a direct call (this TU's copy) */
        bool hit;
        {
            ShipCreate_assureTermination(name.get());
            ship_ss_assure(name.get());
            u32 pa = name->mStr;
            ship_ss_assure(b1.get());
            u32 pb = b1->mStr;
            if (pa == pb) {
                hit = true;
            } else {
                hit = false;
                for (u32 n = 0; n < 0x40001; n++) {
                    u8 ca = gabi::at<be<u8>>(pa)[n];
                    u8 cb = gabi::at<be<u8>>(pb)[n];
                    if (ca != cb)
                        break;
                    if (ca == 0) {
                        hit = true;
                        break;
                    }
                }
            }
        }
        if (!hit) {
            gabi::Local<daShip_SafeString> b2;
            b2->mStr = gabi::guest_addr("fn_main1");
            b2->__vtbl = SAFESTRING_VTBL;
            hit = ship_ss_cmp(name.get(), b2.get());
            if (!hit) {
                gabi::Local<daShip_SafeString> b3;
                b3->mStr = gabi::guest_addr("new_fn_eye");
                b3->__vtbl = SAFESTRING_VTBL;
                hit = ship_ss_cmp(name.get(), b3.get());
            }
        }
        if (!hit) {
            continue;
        }
        ship_ss_assure(name.get());
        gabi::Local<ShipCreateFormattedString> buf;
        u32 bf = gabi::ea(buf.get());
        ShipCreate_formatTextureName(buf.get(), "%s_D46.ftxb", name->mStr);
        gabi::call_ptr(gabi::at<ShipCreateVirtualTable>(buf->__vtbl)->mMethod, bf);
        gabi::Local<daShip_SafeString> k1;
        gabi::Local<daShip_SafeString> k2;
        k1->mStr = gabi::ea(&resourceArc);
        k1->__vtbl = SAFESTRING_VTBL;
        k2->mStr = buf->mStr;
        k2->__vtbl = SAFESTRING_VTBL;
        u32 res = gabi::ea(ShipCreate_getResource(resourceControl, k1.get(), k2.get()));
        ShipCreateTextureEntry* entry = &tex->mEntries[i];
        be<u32>* src = gabi::at<be<u32>>(res);
        be<u32>* dst = reinterpret_cast<be<u32>*>(entry);
        for (int k = 0; k < 9; k++) {
            dst[k] = src[k];
        }
        entry = &tex->mEntries[i];
        entry->mImageOffset = entry->mImageOffset + res - gabi::ea(entry);
        entry = &tex->mEntries[i];
        entry->mPaletteOffset = entry->mPaletteOffset + res - gabi::ea(entry);
        entry = &tex->mEntries[i];
        entry->mHDImage = gabi::at<ShipCreateTextureEntry>(res)->mHDImage;
        /* mask |= (u64 at +0x18) << i, a 128-bit mask at +0x8 */
        u32 hi = tex->mMaskHigh;
        u32 lo = tex->mMaskLow;
        u32 n = i;
        u32 at = 0;
        if (n >= 0x40) {
            n -= 0x40;
            at = 2;
        }
        u32 shi = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
        u32 slo = ppc_slw(lo, n);
        tex->mReplacedMask[at] |= shi;
        tex->mReplacedMask[at + 1] |= slo;
    }
}

// HD: create the cannon-sight model and its 25 segments.
/* 0247B400 */
static void daShip_sightPacket_create(ShipCreateSightPacket* pkt, u32 heap) {
    if (pkt->mModel == 0) {
        u32 m = ShipCreate_new(0xC, heap, 4);
        if (m != 0) {
            m = ShipCreate_modelConstruct(m);
        }
        pkt->mModel = m;
        ShipCreate_modelCreate(m, 1, heap);
    }
    if (pkt->mSegments == 0) {
        u32 a = ShipCreate_newArray(arrayHeaderSize + 0x1068, heap, 4);
        if (a != 0) {
            a = ShipCreate_constructNewArray(a + arrayHeaderSize, 0x19, 0xA8,
                                             ShipCreateTU::segmentConstructor, 0);
        } else {
            a = 0;
        }
        pkt->mSegments = a;
        for (int i = 0; i < 0x19; i++) {
            ShipCreate_segmentCreate(pkt->mSegments + i * 0xA8, heap);
        }
    }
}

/* 0247B508 */
BOOL daShip_c::createHeap() {
    ShipCreateEffectModels* hd = reinterpret_cast<ShipCreateEffectModels*>(this);
    if (checkStateFlg(daSFLG_SAIL_ON_e)) {
        m0392 = dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e;
    } else {
        m0392 = dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e;
    }
    J3DModelData* modelData = (J3DModelData*)ship_getResIdx("Ship", dRes_INDEX_SHIP_BDL_FN_BODY_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_ship.cpp", 0x216A, "modelData != (0)");
    }
    daShip_replaceSeaETextures(modelData);
    J3DAnmTransform* anm = (J3DAnmTransform*)ship_getResIdx("Ship", (u16)m0392);
    mpBodyAnm = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 0, 1.0f, 0, -1, 0,
                                       nullptr, 0x80000, 0x11200202);
    if (mpBodyAnm == nullptr || mpBodyAnm->getModel() == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)ship_getResIdx("Ship", dRes_INDEX_SHIP_BDL_VFNCN_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_ship.cpp", 0x2195, "modelData != (0)");
    }
    mpCannonModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (mpCannonModel == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)ship_getResIdx("Ship", dRes_INDEX_SHIP_BDL_VFNCR_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_ship.cpp", 0x21A2, "modelData != (0)");
    }
    mpSalvageArmModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (mpSalvageArmModel == nullptr) {
        return FALSE;
    }

    modelData =
        (J3DModelData*)ship_getResIdx("Link", ShipCreateResource::dRes_INDEX_LINK_BDL_ROPEEND_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_ship.cpp", 0x21AF, "modelData != (0)");
    }
    mpLinkModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (mpLinkModel == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)ship_getResIdx("Ship", dRes_INDEX_SHIP_BDL_FN_HEAD_H_e);
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_ship.cpp", 0x21BC, "modelData != (0)");
    }
    daShip_replaceSeaETextures(modelData);
    m03B4 = dRes_INDEX_SHIP_BCK_FN_LOOK_L_e;
    anm = (J3DAnmTransform*)ship_getResIdx("Ship", dRes_INDEX_SHIP_BCK_FN_LOOK_L_e);
    mpHeadAnm = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 0, 0.0f, 0, -1, 0,
                                       nullptr, 0x80000, 0x11000002);
    if (mpHeadAnm == nullptr || mpHeadAnm->getModel() == nullptr) {
        return FALSE;
    }
    mpHeadAnm->setMorf(3.0f);

    // HD: 125 rope segments (GameCube 250).
    u32 ropeTex =
        gabi::ea(ship_getResIdx("Always", ShipCreateResource::dRes_INDEX_ALWAYS_BTI_ROPE_e));
    if (!ShipCreate_ropeInit(&mRopeLine, 1, 0x7D, ropeTex, 0)) {
        return FALSE;
    }

    // HD: the beacon model.
    J3DModelData* beacon = (J3DModelData*)ship_getResName("Ship", "beacon.bdl");
    hd->mpBeaconModel = mDoExt_J3DModel__create(beacon, 0, 0x11020203);
    if (hd->mpBeaconModel == nullptr) {
        return FALSE;
    }
    {
        u32 m = gabi::ea(hd->mpBeaconModel.get());
        u32 fl = gabi::at<ShipCreateModelFlags>(m)->mFlags & ~1u;
        gabi::at<ShipCreateModelFlags>(m)->mFlags = fl;
        ShipCreate_modelUnlock(m, fl); /* (r4: the flags, left over) */
    }

    // HD: the cannon-sight impact marker.
    J3DModelData* fallData = (J3DModelData*)ship_getResName("Ship", "fallpoint.bdl");
    J3DAnmTransform* fallAnm = (J3DAnmTransform*)ship_getResName("Ship", "fallpoint.bck");
    hd->mpFallPointAnm = mDoExt_McaMorf::create(nullptr, fallData, nullptr, nullptr, fallAnm, 2,
                                                1.0f, 0, -1, 0, nullptr, 0, 0x11020203);
    hd->mpFallPointModel = hd->mpFallPointAnm->getModel();
    hd->mpFallPointAnm->setMorf(0.0f);
    if (hd->mpFallPointModel != nullptr) {
        u32 m = gabi::ea(hd->mpFallPointModel.get());
        u32 fl = gabi::at<ShipCreateModelFlags>(m)->mFlags & ~1u;
        gabi::at<ShipCreateModelFlags>(m)->mFlags = fl;
        ShipCreate_modelUnlock(m, fl); /* (r4: the flags, left over) */
    }

    // HD: the cannon-sight packet, drawn with the beacon model.
    daShip_sightPacket_create(reinterpret_cast<ShipCreateSightPacket*>(mHDPacket), 0);
    reinterpret_cast<ShipCreateSightPacket*>(mHDPacket)->mBeacon = hd->mpBeaconModel;

    // HD: the shadow on the water and its texture animation.
    J3DModelData* shadowData = (J3DModelData*)ship_getResName("Always", "ship_minamo00.bdl");
    mpShadowModel = mDoExt_J3DModel__create(shadowData, 0, 0x11020203);
    if (mpShadowModel == nullptr) {
        return FALSE;
    }
    u32 btk = gabi::ea(ship_getResName("Always", "ship_minamo00.btk"));
    if (ShipCreate_shadowInit(&mShadowBtk, shadowData, btk, 1, 2, 1.0f, 0, -1, 0, 0) == 0) {
        return FALSE;
    }
    return TRUE;
}

/* the placement constructor (fopAcM_SetupActor(this, daShip_c)) */
static void daShip_construct(daShip_c* p) {
    fopAc_ac_c_ct(p);
    p->__vtbl = ShipCreateTU::shipVtable;
    ShipCreate_frameInit(&p->mFrameCtrl, 0);
    ShipCreate_lineConstruct(&p->mRopeLine);
    ShipCreate_btkConstruct(&p->mShadowBtk);
    ShipCreate_constructArray(p->mAcchCir, 4, 0x40, ShipCreateTU::acchCirConstructor);
    {
        /* mAcch: dBgS_ObjAcch */
        ShipCreateAcchBase* b = reinterpret_cast<ShipCreateAcchBase*>(&p->mAcch);
        ShipCreate_acchConstruct(b);
        b->mObject = 1;
        b->mPolyVtable = ShipCreateTU::polyVtable;
        b->mObjectVtable = ShipCreateTU::objectVtable;
        b->mCheckVtable = ShipCreateTU::checkVtable;
    }
    dCcD_Stts_ct(&p->mStts);
    ShipCreate_constructArray(p->mCyl, 3, 0x130, ShipCreateTU::cylinderConstructor);
    ShipCreate_sphereConstruct(&p->mSph);
    // HD: the sight packet.
    ShipCreate_packetConstruct(p->mHDPacket);
    reinterpret_cast<ShipCreateSightPacket*>(p->mHDPacket)->mBeacon = nullptr;
    reinterpret_cast<ShipCreateSightPacket*>(p->mHDPacket)->mModel = 0;
    reinterpret_cast<ShipCreateSightPacket*>(p->mHDPacket)->mSegments = 0;
    reinterpret_cast<ShipCreateSightPacket*>(p->mHDPacket)->__vtbl = ShipCreateTU::sightVtable;
    // HD: the 128 sight colliders.
    ShipCreate_constructArray(p->mRopeStts, 0x80, 0x3C, ShipCreateTU::statusConstructor);
    ShipCreate_constructArray(p->mRopeSph, 0x80, 0x12C, ShipCreateTU::sphereConstructor);
    /* the wave callbacks (their cXyz members' constructors allocate only for a null this) */
    for (int i = 0; i < 2; i++) {
        ShipCreateWave* wave = reinterpret_cast<ShipCreateWave*>(i == 0 ? p->mWaveR : p->mWaveL);
        wave->__vtbl = ShipCreateTU::waveVtable;
        // HD: retain the GHS null-subobject allocation checks, including 32-bit wraparound.
        for (int j = 0; j < 3; j++) {
            if (gabi::ea(&wave->mPosition[j]) == 0) {
                operator_new(sizeof(cXyz));
            }
        }
    }
    reinterpret_cast<ShipCreateVtable*>(p->mSplash)->__vtbl = ShipCreateTU::splashVtable;
    reinterpret_cast<ShipCreateTrack*>(p->mTrack)->__vtbl = ShipCreateTU::trackVtable;
    ShipCreate_constructArray(reinterpret_cast<ShipCreateTrack*>(p->mTrack)->mPosition, 3, 0xC,
                              ShipCreateTU::positionConstructor);
    ShipCreate_rippleConstruct(p->mRipple);
    ShipCreate_followConstruct(&p->m1984, 0, 0);
    ShipCreate_followConstruct(&p->m1998, 0, 0);
    ShipCreate_followConstruct(&p->m19AC, 0, 0);
    ShipCreate_rippleConstruct(p->m19C0);
}

/* a model's joint callback (joint i: i < count ? &joints[i] : joints) */
static inline void ship_setJointCallBack(J3DModelData* modelData, u32 i, u32 cb) {
    u32 joint = modelData->mJointNodes;
    if (i < modelData->mJointNum) {
        joint += i * 0x1C;
    }
    gabi::at<J3DJoint>(joint)->mCallBack = cb;
}

/* 0247C358 */
cPhs_State daShip_c::create() {
    u32 s = gabi::ea(this);
    if (!(actor_condition & ShipCreateActorInitialized)) {
        daShip_construct(this);
        actor_condition |= ShipCreateActorInitialized;
    }
    if (!dComIfGs_isEventBit(0x0F80)) {
        return cPhs_ERROR_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, "Ship");
    if (((mParameters >> 8) & 0xFF) == 1) {
        onStateFlg(daSFLG_SAIL_ON_e);
        m03E8 = 1.0f;
    } else {
        m03E8 = 0.001f;
        offStateFlg(daSFLG_SAIL_ON_e);
    }
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }

    // HD: the ending sea waits for the "Demo46" resources.
    if (ship_isStartStage(gabi::guest_addr("sea_E"))) {
        gabi::Local<daShip_SafeString> arc;
        arc->mStr = gabi::guest_addr("Demo46");
        arc->__vtbl = SAFESTRING_VTBL;
        if (ShipCreate_resourceReady(resourceControl, arc.get(), 0, 1) == 0) {
            return (cPhs_State)0;
        }
    }

    if (!fopAcM_entrySolidHeap(this, daShip_createHeap, 0x20000)) {
        return cPhs_ERROR_e;
    }

    mpBodyAnm->setFrame(mpBodyAnm->getEndFrame());
    // HD: preserve guest-zero views for unchecked model/data accesses.
    J3DModel* pModel = gabi::at<J3DModel>(gabi::ea(mpBodyAnm->getModel()));
    J3DModelData* pModelData = gabi::at<J3DModelData>(gabi::ea(pModel->getModelData()));
    pModel->mUserArea = s;
    cullMtx = gabi::ea(pModel->getBaseTRMtx());
    for (u16 i = 0; i < pModelData->getJointNum(); i++) {
        if ((i >= 5 && i <= 7) || i == 10) {
            ship_setJointCallBack(pModelData, i, ShipCreateTU::bodyJointCallback);
        }
    }
    // HD: the texture matrix of the material "m_fn_main_hashi".
    {
        gabi::Local<daShip_SafeString> mat;
        mat->mStr = gabi::guest_addr("m_fn_main_hashi");
        mat->__vtbl = SAFESTRING_VTBL;
        u32 d0 = pModelData->mpResHeader;
        ShipCreate_assureTermination(mat.get());
        u32 off = gabi::at<ShipCreateResourceHeader>(d0)->mMaterialNameOffset;
        u32 tab = off != 0 ? d0 + 0x18 + off : 0;
        s32 idx = JUTNameTab_getIndex_s(tab, mat->mStr);
        if (idx >= 0) {
            u32 m = pModelData->mMaterials;
            if ((u32)idx < pModelData->mMaterialNum) {
                m += idx * 0x39C;
            }
            if (m != 0) {
                ShipCreateTexGenBlock* texGen = gabi::at<ShipCreateMaterial>(m)->mTexGenBlock;
                u32 r = texGen->getTexMtx(1);
                m02A0 = r;
                m02A4 = r;
            }
        }
    }
    ShipCreate_ortho(&m02A8, 50.0f, -50.0f, -50.0f, 50.0f, 0.1f, 100000.0f);

    pModel = gabi::at<J3DModel>(gabi::ea(mpHeadAnm->getModel()));
    pModelData = gabi::at<J3DModelData>(gabi::ea(pModel->getModelData()));
    pModel->mUserArea = s;
    for (u16 i = 0; i < pModelData->getJointNum(); i++) {
        if (i == 8 || i == 10) {
            ship_setJointCallBack(pModelData, i, ShipCreateTU::headJointCallback0);
        } else if ((u32)(i - 2) <= 5) {
            ship_setJointCallBack(pModelData, i, ShipCreateTU::headJointCallback1);
        }
    }

    mpCannonModel->mUserArea = s;
    pModelData = gabi::at<J3DModelData>(gabi::ea(mpCannonModel->getModelData()));
    ship_setJointCallBack(pModelData, 1, ShipCreateTU::cannonJointCallback);
    ship_setJointCallBack(pModelData, 2, ShipCreateTU::cannonJointCallback);
    mpSalvageArmModel->mUserArea = s;
    pModelData = gabi::at<J3DModelData>(gabi::ea(mpSalvageArmModel->getModelData()));
    ship_setJointCallBack(pModelData, 1, ShipCreateTU::craneJointCallback);

    mTornadoID = fpcM_ERROR_PROCESS_ID_e;
    mTornadoActor = nullptr;
    mWhirlID = fpcM_ERROR_PROCESS_ID_e;
    m034B = (u8)mParameters;
    mWhirlActor = nullptr;
    mPart = PART_WAIT_e;
    if (mNextMode != MODE_START_MODE_WARP_e) {
        mTactWarpID = fpcM_ERROR_PROCESS_ID_e;
    }
    if (checkStateFlg(daSFLG_SAIL_ON_e)) {
        mPart = PART_STEER_e;
        procSteerMove_init();
    } else if (mNextMode == MODE_PADDLE_MOVE_e) {
        procPaddleMove_init();
    } else if (mNextMode == MODE_START_MODE_WARP_e) {
        procStartModeWarp_init();
    } else if (mNextMode == MODE_START_MODE_THROW_e) {
        procStartModeThrow_init();
    } else {
        procWait_init();
    }
    BOOL wide;
    if (ship_getStageType() == ShipCreateStageSea) {
        wide = TRUE;
    } else if (ship_isStartStage(gabi::guest_addr("Hyrule"))) {
        wide = TRUE;
    } else if (ship_isStartStage(gabi::guest_addr("Ocean"))) {
        wide = TRUE;
    } else {
        wide = FALSE;
    }
    m03CC = wide ? 4 : 3;
    mAcch.Set(&current.pos, &old.pos, this, m03CC, mAcchCir, &speed, &current.angle, &shape_angle);
    mAcchCir[0].SetWall(0.0f, 250.0f);
    mAcchCir[1].SetWall(75.0f, 250.0f);
    mAcchCir[2].SetWall(150.0f, 250.0f);
    mAcchCir[3].SetWall(-600.0f - current.pos.y, 250.0f);
    attention_info.position.x = current.pos.x;
    attention_info.position.y = current.pos.y;
    mAcch.SetWaterCheckOffset(10000.0f);
    attention_info.position.z = current.pos.z;
    attention_info.flags = 0;
    mAcch.m_flags &= ~0x400u;
    gravity = -2.5f;
    maxFallSpeed = -150.0f;
    mGridID = fopAcM_create(ShipCreateProcess::fpcNm_GRID_e, 1, &current.pos, -1, &current.angle,
                            nullptr, -1, 0);
    if (mGridID == fpcM_ERROR_PROCESS_ID_e) {
        return cPhs_ERROR_e;
    }
    mTactWarpPosNum = -1;
    m03E0 = 10000.0f;
    mStts.Init(0xF0, 0, this);
    for (int i = 0; i < 3; i++) {
        mCyl[i].Set(&cyl_src);
        mCyl[i].SetStts(&mStts);
    }
    mCyl[1].SetR(95.0f);
    mSph.Set(&sph_src);
    mSph.SetStts(&mStts);
    ShipCreate_setMin(this, -325.0f, -50.0f, -325.0f);
    ShipCreate_setMax(this, 325.0f, 570.0f, 240.0f);
    ShipCreate_createWindArrow(ShipCreateProcess::fpcNm_WIND_ARROW_e, this, 0, 0, 0);
    offStateFlg(daSFLG_UNK2_e);
    mAcch.CrrPos(dComIfG_Bgsp());
    setRoomInfo();
    m03F4 = getWaterY();
    f32 y;
    if (mCurMode == MODE_START_MODE_WARP_e) {
        y = m03F4 + 5000.0f;
        current.pos.y = y;
    } else if (mCurMode == MODE_START_MODE_THROW_e) {
        y = m03F4 + 2500.0f;
        current.pos.y = y;
    } else {
        y = current.pos.y;
    }
    mDoMtx_stack_c::transS(current.pos.x, y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_now(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpBodyAnm->getModel(), mDoMtx_now());
    mpBodyAnm->play(nullptr, 0, 0);
    mpBodyAnm->calc();
    PSMTXMultVec(model_getAnmMtx(mpBodyAnm->getModel(), FN_BODY_JNT_J_FN_STEER1_e),
                 &tiller_top_offset, &mTillerTopPos);
    mpHeadAnm->play(nullptr, 0, 0);
    J3DModel_setBaseTRMtx(mpHeadAnm->getModel(),
                          model_getAnmMtx(mpBodyAnm->getModel(), FN_BODY_JNT_J_FN_GATTAI_e));
    mpHeadAnm->calc();
    gabi::at<ShipCreatePlay>(dComIfGp_ea())->mShipActor = this;
    reinterpret_cast<ShipCreateTrack*>(mTrack)->mLimitSpeed = 3.0f;
    reinterpret_cast<ShipCreateWave*>(mWaveL)->mMaxSpeed = 40.0f; /* mWaveL max speed */
    reinterpret_cast<ShipCreateFollow*>(m19C0)->mRate = 0.0f;
    reinterpret_cast<ShipCreateWave*>(mWaveR)->mMaxSpeed = 40.0f; /* mWaveR max speed */
    if (ship_getStageType() == ShipCreateStageSea) {
        ShipCreate_createChild(ShipCreateProcess::fpcNm_Coming2_e, mBsPcId, 0, &current.pos, -1, 0,
                               0, -1, 0);
    }
    // HD: the cannon sight.
    ShipCreateSight* sight = &reinterpret_cast<ShipCreateSightActor*>(this)->mSight;
    sight->mTime = 0.0f;
    sight->mAngle[0] = 0;
    sight->mAngle[1] = 0;
    sight->mAngle[2] = 0;
    for (int i = 0; i < 7; i++) {
        sight->mTrajectory[i] = 0.0f;
    }
    sight->mSegment = 0;
    sight->mHeight = -100.0f;
    for (int i = 0; i < 0x80; i++) {
        mRopeStts[i].Init(0, 0xFF, this);
        mRopeSph[i].Set(&rope_sph_src);
        mRopeSph[i].SetStts(&mRopeStts[i]);
    }
    return phase;
}

// clang-format off
/* WWHD glue
 data resourceControl 101F4F28
 data arrayHeaderSize 101FCD08
 data resourceArc 1047E6B8
 data cyl_src 101D0440
 data sph_src 101D0400
 data rope_sph_src 101D0354
 data tiller_top_offset 101D03C0
 str 1003A5D4 "sea_E" in 0247AF98
 str 1003A5E8 "fn_head1"
 str 1003A5F4 "fn_main1"
 str 1003A600 "new_fn_eye"
 str 101D034C "Ship"
 str 1003A60C "Link"
 str 1003A614 "Always"
 str 1003A688 "beacon.bdl"
 str 1003A61C "fallpoint.bdl"
 str 1003A62C "fallpoint.bck"
 str 1003A63C "ship_minamo00.bdl"
 str 1003A650 "ship_minamo00.btk"
 str 1003A6C4 "sea_E" in 0247C358
 str 1003A6D4 "Demo46"
 str 1003A6E4 "m_fn_main_hashi"
 str 1003A6DC "Hyrule"
 str 1003A6CC "Ocean"
 str 1003A664 "d_a_ship.cpp"
 str 1003A674 "modelData != (0)"
 ext 027ED1F0 const char* ShipCreate_getTextureName(u32, u32)
 ext 024832A0 void ShipCreate_assureTermination(daShip_SafeString*)
 ext 02482CFC void ShipCreate_formatTextureName(ShipCreateFormattedString*, const char*, u32)
 ext 02606900 void* ShipCreate_getResource(u32, daShip_SafeString*, daShip_SafeString*)
 ext 026065A8 u32 ShipCreate_resourceReady(u32, daShip_SafeString*, u32, u32)
 ext 0273B050 u32 ShipCreate_new(u32, u32, u32)
 ext 0273B0D4 u32 ShipCreate_newArray(u32, u32, u32)
 ext 027FD6F4 u32 ShipCreate_modelConstruct(u32)
 ext 027FD838 void ShipCreate_modelCreate(u32, u32, u32)
 ext 028EFF8C u32 ShipCreate_constructNewArray(u32, u32, u32, u32, u32)
 ext 027FB5D4 void ShipCreate_segmentCreate(u32, u32)
 ext 025EBA58 BOOL ShipCreate_ropeInit(void*, u32, u32, u32, u32)
 ext 027F596C void ShipCreate_modelUnlock(u32, u32)
 ext 025E7CE0 BOOL ShipCreate_shadowInit(void*, J3DModelData*, u32, u32, u32, f32, u32, s32, u32, u32)
 ext 028E9A70 void ShipCreate_ortho(Mtx34*, f32, f32, f32, f32, f32, f32)
 ext 025D672C void ShipCreate_setMin(daShip_c*, f32, f32, f32)
 ext 025D673C void ShipCreate_setMax(daShip_c*, f32, f32, f32)
 ext 025DADA4 void ShipCreate_createWindArrow(u32, daShip_c*, u32, u32, u32)
 ext 025D5A20 void ShipCreate_createChild(u32, u32, u32, cXyz*, s32, u32, u32, s32, u32)
 ext 027F2BC0 void ShipCreate_frameInit(void*, u32)
 ext 025EB82C void ShipCreate_lineConstruct(void*)
 ext 025E7C6C void ShipCreate_btkConstruct(void*)
 ext 028EFFD0 void ShipCreate_constructArray(void*, u32, u32, u32)
 ext 024F0474 void ShipCreate_acchConstruct(void*)
 ext 025166F0 void ShipCreate_sphereConstruct(void*)
 ext 027F1278 void ShipCreate_packetConstruct(void*)
 ext 025A9084 void ShipCreate_rippleConstruct(void*)
 ext 025A5894 void ShipCreate_followConstruct(void*, u32, u32)
 str 1003A5DC "%s_D46.ftxb"
*/
