/**
 * d_a_ship_create.cpp (WWHD)
 * King of Red Lions: createHeap, create and the HD helpers they use.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ship.cpp) to the WWHD layout and verified against cking.rpx.
 */
#include "d/actor/d_a_ship.h"

/* 02606900 dRes_control_c::getRes(const SafeString& arc, const SafeString& name) */
static inline void* ship_getResName(u32 arc, u32 name) {
    gabi::Local<daShip_SafeString> a;
    gabi::Local<daShip_SafeString> n;
    a->mStr = arc;
    a->__vtbl = SAFESTRING_VTBL;
    n->mStr = name;
    n->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x02606900, gabi::load<u32>(0x101F4F28), a.get(), n.get());
}
static inline void* ship_getResIdx(u32 arc, s32 idx) {
    return dComIfG_getObjectRes(STR(arc), idx, SAFESTRING_VTBL);
}

/* PowerPC slw/srw: shift amounts 32..63 give 0 */
static inline u32 ppc_slw(u32 x, u32 n) { n &= 63; return n >= 32 ? 0 : x << n; }
static inline u32 ppc_srw(u32 x, u32 n) { n &= 63; return n >= 32 ? 0 : x >> n; }

/* 0247AF98: HD-only. In the stage "sea_E", the textures fn_head1, fn_main1 and new_fn_eye of a model
 * data are replaced with their "<name>_D46.ftxb" versions from the resources (the texture header is
 * copied over the entry and its offsets rebased), and the entry's bit is set in the data's
 * replaced-texture mask. Probably the boat's look in the ending sea. */
static void daShip_replaceSeaETextures(J3DModelData* data) {
    WWHD_FUNC(0x0247AF98, void, data);
    if (!ship_isStartStage(0x1003A5D4 /* "sea_E" */)) {
        return;
    }
    u32 tex = gabi::load<u32>(gabi::ea(data) + 0x30);
    u32 names = gabi::load<u32>(gabi::ea(data) + 0x34);
    for (u16 i = 0; i < gabi::load<u16>(tex); i++) {
        gabi::Local<daShip_SafeString> name;
        name->mStr = gabi::call<u32>(0x027ED1F0 /* JUTNameTab::getName */, names, (u32)i);
        name->__vtbl = SAFESTRING_VTBL;
        gabi::Local<daShip_SafeString> b1;
        b1->mStr = 0x1003A5E8; /* "fn_head1" */
        b1->__vtbl = SAFESTRING_VTBL;
        /* the first comparison: name's first assureTermination is a direct call (this TU's copy) */
        bool hit;
        {
            gabi::call(0x024832A0, name.get());
            ship_ss_assure(name.get());
            u32 pa = name->mStr;
            ship_ss_assure(b1.get());
            u32 pb = b1->mStr;
            if (pa == pb) {
                hit = true;
            } else {
                hit = false;
                for (u32 n = 0; n < 0x40001; n++) {
                    u8 ca = gabi::load<u8>(pa + n);
                    u8 cb = gabi::load<u8>(pb + n);
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
            b2->mStr = 0x1003A5F4; /* "fn_main1" */
            b2->__vtbl = SAFESTRING_VTBL;
            hit = ship_ss_cmp(name.get(), b2.get());
            if (!hit) {
                gabi::Local<daShip_SafeString> b3;
                b3->mStr = 0x1003A600; /* "new_fn_eye" */
                b3->__vtbl = SAFESTRING_VTBL;
                hit = ship_ss_cmp(name.get(), b3.get());
            }
        }
        if (!hit) {
            continue;
        }
        ship_ss_assure(name.get());
        gabi::Local<u8[0x2C]> buf;
        u32 bf = gabi::ea(buf.get());
        gabi::call(0x02482CFC /* FormatFixedSafeString<32>(fmt, ...) */, bf, 0x1003A5DC /* "%s_D46.ftxb" */, (u32)name->mStr);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(bf + 4) + 0x14), bf);
        gabi::Local<daShip_SafeString> k1;
        gabi::Local<daShip_SafeString> k2;
        k1->mStr = 0x1047E6B8;
        k1->__vtbl = SAFESTRING_VTBL;
        k2->mStr = gabi::load<u32>(bf);
        k2->__vtbl = SAFESTRING_VTBL;
        u32 res = gabi::call<u32>(0x02606900, gabi::load<u32>(0x101F4F28), k1.get(), k2.get());
        u32 e = gabi::load<u32>(tex + 4) + i * 0x24;
        for (int k = 0; k < 0x24; k += 4) {
            gabi::store<u32>(e + k, gabi::load<u32>(res + k));
        }
        e = gabi::load<u32>(tex + 4) + i * 0x24;
        gabi::store<u32>(e + 0x1C, gabi::load<u32>(e + 0x1C) + res - e);
        e = gabi::load<u32>(tex + 4) + i * 0x24;
        gabi::store<u32>(e + 0xC, gabi::load<u32>(e + 0xC) + res - e);
        e = gabi::load<u32>(tex + 4) + i * 0x24;
        gabi::store<u32>(e + 0x20, gabi::load<u32>(res + 0x20));
        /* mask |= (u64 at +0x18) << i, a 128-bit mask at +0x8 */
        u32 hi = gabi::load<u32>(tex + 0x18);
        u32 lo = gabi::load<u32>(tex + 0x1C);
        u32 n = i;
        u32 at = 8;
        if (n >= 0x40) {
            n -= 0x40;
            at = 0x10;
        }
        u32 shi = ppc_slw(lo, n + 0x20) | (ppc_slw(hi, n) | ppc_srw(lo, 0x20 - n));
        u32 slo = ppc_slw(lo, n);
        gabi::store<u32>(tex + at, gabi::load<u32>(tex + at) | shi);
        gabi::store<u32>(tex + at + 4, gabi::load<u32>(tex + at + 4) | slo);
    }
}
VERIFY(0x0247AF98, daShip_replaceSeaETextures);

/* 0247B400: HD-only. Creates the cannon-sight packet's model (+0x9C) and its 25 segments (+0xA0) */
static void daShip_sightPacket_create(u8* pkt, u32 heap) {
    WWHD_FUNC(0x0247B400, void, pkt, heap);
    u32 p = gabi::ea(pkt);
    if (gabi::load<u32>(p + 0x9C) == 0) {
        u32 m = gabi::call<u32>(0x0273B050 /* operator new(size, heap, align) */, 0xC, heap, 4);
        if (m != 0) {
            m = gabi::call<u32>(0x027FD6F4, m);
        }
        gabi::store<u32>(p + 0x9C, m);
        gabi::call(0x027FD838, m, 1, heap);
    }
    if (gabi::load<u32>(p + 0xA0) == 0) {
        u32 a = gabi::call<u32>(0x0273B0D4 /* operator new[](size, heap, align) */, gabi::load<u32>(0x101FCD08) + 0x1068, heap, 4);
        if (a != 0) {
            a = gabi::call<u32>(0x028EFF8C /* __construct_new_array */, a + gabi::load<u32>(0x101FCD08), 0x19, 0xA8, 0x02482E30, 0);
        } else {
            a = 0;
        }
        gabi::store<u32>(p + 0xA0, a);
        for (int i = 0; i < 0x19; i++) {
            gabi::call(0x027FB5D4, gabi::load<u32>(p + 0xA0) + i * 0xA8, heap);
        }
    }
}
VERIFY(0x0247B400, daShip_sightPacket_create);

/* 0247B508 */
BOOL daShip_c::createHeap() {
    WWHD_FUNC(0x0247B508, BOOL, this);
    if (checkStateFlg((daSHIP_SFLG)0x200)) {
        m0392 = dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e + 1;
    } else {
        m0392 = dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e;
    }
    J3DModelData* modelData = (J3DModelData*)ship_getResIdx(0x101D034C, 0x11 /* fn_body.bdl */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1003A664), 0x216A, STR(0x1003A674));
    }
    daShip_replaceSeaETextures(modelData);
    J3DAnmTransform* anm = (J3DAnmTransform*)ship_getResIdx(0x101D034C, (u16)m0392);
    mpBodyAnm = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 0, 1.0f, 0, -1, 0, nullptr, 0x80000, 0x11200202);
    if (mpBodyAnm == nullptr || mpBodyAnm->getModel() == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)ship_getResIdx(0x101D034C, 0x13 /* vfncn.bdl */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1003A664), 0x2195, STR(0x1003A674));
    }
    mpCannonModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (mpCannonModel == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)ship_getResIdx(0x101D034C, 0x14 /* vfncr.bdl */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1003A664), 0x21A2, STR(0x1003A674));
    }
    mpSalvageArmModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (mpSalvageArmModel == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)ship_getResIdx(0x1003A60C /* "Link" */, 0x2E);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1003A664), 0x21AF, STR(0x1003A674));
    }
    mpLinkModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    if (mpLinkModel == nullptr) {
        return FALSE;
    }

    modelData = (J3DModelData*)ship_getResIdx(0x101D034C, 0x12 /* fn_head_h.bdl */);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x1003A664), 0x21BC, STR(0x1003A674));
    }
    daShip_replaceSeaETextures(modelData);
    m03B4 = 7;
    anm = (J3DAnmTransform*)ship_getResIdx(0x101D034C, 7);
    mpHeadAnm = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 0, 0.0f, 0, -1, 0, nullptr, 0x80000, 0x11000002);
    if (mpHeadAnm == nullptr || mpHeadAnm->getModel() == nullptr) {
        return FALSE;
    }
    mpHeadAnm->setMorf(3.0f);

    /* HD: 125 rope segments (GameCube 250) */
    u32 ropeTex = gabi::ea(ship_getResIdx(0x1003A614 /* "Always" */, 0x7E));
    if (!gabi::call<BOOL>(0x025EBA58 /* mDoExt_3DlineMat1_c::init */, &mRopeLine, 1, 0x7D, ropeTex, 0)) {
        return FALSE;
    }

    /* HD: the beacon model */
    J3DModelData* beacon = (J3DModelData*)ship_getResName(0x101D034C, 0x1003A688 /* "beacon.bdl" */);
    mpHD5AC = mDoExt_J3DModel__create(beacon, 0, 0x11020203);
    if (mpHD5AC == nullptr) {
        return FALSE;
    }
    {
        u32 m = gabi::ea(mpHD5AC.get());
        u32 fl = gabi::load<u32>(m + 0x74) & ~1u;
        gabi::store<u32>(m + 0x74, fl);
        gabi::call(0x027F596C, m, fl); /* (r4: the flags, left over) */
    }

    /* HD: the cannon-sight impact marker */
    J3DModelData* fallData = (J3DModelData*)ship_getResName(0x101D034C, 0x1003A61C /* "fallpoint.bdl" */);
    J3DAnmTransform* fallAnm = (J3DAnmTransform*)ship_getResName(0x101D034C, 0x1003A62C /* "fallpoint.bck" */);
    mpHD5B4 = mDoExt_McaMorf::create(nullptr, fallData, nullptr, nullptr, fallAnm, 2, 1.0f, 0, -1, 0, nullptr, 0, 0x11020203);
    mpHD5B0 = mpHD5B4->getModel();
    mpHD5B4->setMorf(0.0f);
    if (mpHD5B0 != nullptr) {
        u32 m = gabi::ea(mpHD5B0.get());
        u32 fl = gabi::load<u32>(m + 0x74) & ~1u;
        gabi::store<u32>(m + 0x74, fl);
        gabi::call(0x027F596C, m, fl); /* (r4: the flags, left over) */
    }

    /* HD: the cannon-sight packet, drawn with the beacon model */
    daShip_sightPacket_create(mHDPacket, 0);
    gabi::store<u32>(gabi::ea(this) + 0x1BEC, gabi::ea(mpHD5AC.get()));

    /* HD: the shadow on the water and its texture animation */
    J3DModelData* shadowData = (J3DModelData*)ship_getResName(0x1003A614, 0x1003A63C /* "ship_minamo00.bdl" */);
    mpShadowModel = mDoExt_J3DModel__create(shadowData, 0, 0x11020203);
    if (mpShadowModel == nullptr) {
        return FALSE;
    }
    u32 btk = gabi::ea(ship_getResName(0x1003A614, 0x1003A650 /* "ship_minamo00.btk" */));
    if (gabi::call<BOOL>(0x025E7CE0 /* mDoExt_btkAnm::init */, &mShadowBtk, shadowData, btk, 1, 2, 1.0f, 0, -1, 0, 0) == 0) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0247B508, &daShip_c::createHeap);

/* the placement constructor (fopAcM_SetupActor(this, daShip_c)) */
static void daShip_construct(daShip_c* p) {
    u32 s = gabi::ea(p);
    fopAc_ac_c_ct(p);
    gabi::store<u32>(s + 0xB4, 0x1003A2EC);
    gabi::call(0x027F2BC0 /* J3DFrameCtrl::init */, s + 0x408, 0);
    gabi::call(0x025EB82C /* mDoExt_3DlineMat1_c ctor */, s + 0x424);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm ctor */, s + 0x5BC);
    gabi::call(0x028EFFD0 /* __construct_array */, s + 0x1368, 4, 0x40, 0x024EFE94 /* dBgS_AcchCir ctor */);
    {
        /* mAcch: dBgS_ObjAcch */
        u32 b = s + 0x1468;
        gabi::call(0x024F0474, b);
        gabi::store<u8>(b + 0x18, 1);
        gabi::store<u32>(b + 0x10, 0x1003A27C);
        gabi::store<u32>(b + 0x20, 0x1003A28C);
        gabi::store<u32>(b + 0x14, 0x1003A29C);
    }
    dCcD_Stts_ct(&p->mStts);
    gabi::call(0x028EFFD0, s + 0x1668, 3, 0x130, 0x02482EA0 /* dCcD_Cyl ctor */);
    gabi::call(0x025166F0 /* dCcD_Sph ctor */, s + 0x19F8);
    /* HD: the sight packet */
    gabi::call(0x027F1278 /* J3DPacket ctor */, s + 0x1B54);
    gabi::store<u32>(s + 0x1BEC, 0);
    gabi::store<u32>(s + 0x1BF0, 0);
    gabi::store<u32>(s + 0x1BF4, 0);
    gabi::store<u32>(s + 0x1B60, 0x1003A7F8);
    /* HD: the 128 sight colliders */
    gabi::call(0x028EFFD0, s + 0x1BF8, 0x80, 0x3C, 0x02482F2C /* dCcD_Stts ctor */);
    gabi::call(0x028EFFD0, s + 0x39F8, 0x80, 0x12C, 0x025166F0 /* dCcD_Sph ctor */);
    /* the wave callbacks (their cXyz members' constructors allocate only for a null this) */
    for (u32 w = 0xCFF8; w <= 0xD05C; w += 0x64) {
        gabi::store<u32>(s + w, 0x100521A8);
        if (s + w + 0x3C == 0) operator_new(0xC);
        if (s + w + 0x48 == 0) operator_new(0xC);
        if (s + w + 0x54 == 0) operator_new(0xC);
    }
    gabi::store<u32>(s + 0xD0C0, 0x100521E8); /* mSplash */
    gabi::store<u32>(s + 0xD0DC, 0x10052268); /* mTrack */
    gabi::call(0x028EFFD0, s + 0xD0EC, 3, 0xC, 0x02482F94);
    gabi::call(0x025A9084 /* dPa_rippleEcallBack ctor */, s + 0xD12C);
    gabi::call(0x025A5894 /* dPa_followEcallBack ctor */, s + 0xD140, 0, 0);
    gabi::call(0x025A5894, s + 0xD154, 0, 0);
    gabi::call(0x025A5894, s + 0xD168, 0, 0);
    gabi::call(0x025A9084, s + 0xD17C);
}

/* a model's joint callback (joint i: i < count ? &joints[i] : joints) */
static inline void ship_setJointCallBack(u32 data, u32 i, u32 cb) {
    u32 j = gabi::load<u32>(data + 8);
    if (i < gabi::load<u32>(data + 4)) {
        j += i * 0x1C;
    }
    gabi::store<u32>(j + 8, cb);
}

/* 0247C358 */
cPhs_State daShip_c::create() {
    WWHD_FUNC(0x0247C358, cPhs_State, this);
    u32 s = gabi::ea(this);
    if (!(gabi::load<u32>(s + 0x2E4) & 8)) {
        daShip_construct(this);
        gabi::store<u32>(s + 0x2E4, gabi::load<u32>(s + 0x2E4) | 8);
    }
    if (!dComIfGs_isEventBit(0x0F80)) {
        return cPhs_ERROR_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, STR(l_arcName));
    if (((gabi::load<u32>(s + 0xB0) >> 8) & 0xFF) == 1) {
        onStateFlg((daSHIP_SFLG)0x200);
        m03E8 = 1.0f;
    } else {
        m03E8 = 0.001f;
        offStateFlg((daSHIP_SFLG)0x200);
    }
    if (phase != cPhs_COMPLEATE_e) {
        return phase;
    }

    /* HD: the ending sea waits for the "Demo46" resources */
    if (ship_isStartStage(0x1003A6C4 /* "sea_E" */)) {
        gabi::Local<daShip_SafeString> arc;
        arc->mStr = 0x1003A6D4; /* "Demo46" */
        arc->__vtbl = SAFESTRING_VTBL;
        if (gabi::call<u32>(0x026065A8, gabi::load<u32>(0x101F4F28), arc.get(), 0, 1) == 0) {
            return (cPhs_State)0;
        }
    }

    if (!fopAcM_entrySolidHeap(this, (heapCallbackFunc)0x0247BA34, 0x20000)) {
        return cPhs_ERROR_e;
    }

    /* mpBodyAnm->setFrame(mpBodyAnm->getEndFrame()) */
    {
        u32 b = gabi::ea(mpBodyAnm.get());
        f32 end = (f32)gabi::load<s16>(b + 0xA2);
        s16 f = (s16)gabi::ftoi(end);
        gabi::store<f32>(b + 0x9C, (f32)f);
    }
    u32 model = gabi::load<u32>(gabi::ea(mpBodyAnm.get()) + 0x90);
    u32 data = gabi::load<u32>(model + 0xAC);
    gabi::store<u32>(model + 0xB8, s);
    gabi::store<u32>(s + 0x348, model != 0 ? model + 0xC8 : 0);
    for (u16 i = 0; i < gabi::load<u16>(gabi::call<u32>(0x027F3F94, data) + 8); i++) {
        if ((i >= 5 && i <= 7) || i == 10) {
            ship_setJointCallBack(data, i, 0x0247187C);
        }
    }
    /* HD: the texture matrix of the material "m_fn_main_hashi" */
    {
        gabi::Local<daShip_SafeString> mat;
        mat->mStr = 0x1003A6E4;
        mat->__vtbl = SAFESTRING_VTBL;
        u32 d0 = gabi::load<u32>(data);
        gabi::call(0x024832A0, mat.get());
        u32 off = gabi::load<u32>(d0 + 0x18);
        u32 tab = off != 0 ? d0 + 0x18 + off : 0;
        s32 idx = JUTNameTab_getIndex_s(tab, mat->mStr);
        if (idx >= 0) {
            u32 m = gabi::load<u32>(data + 0x10);
            if ((u32)idx < gabi::load<u32>(data + 0xC)) {
                m += idx * 0x39C;
            }
            if (m != 0) {
                u32 o = gabi::load<u32>(m + 0x14);
                u32 r = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(o) + 0x14), o, 1);
                m02A0 = r;
                m02A4 = r;
            }
        }
    }
    gabi::call(0x028E9A70 /* C_MTXOrtho */, &m02A8, 50.0f, -50.0f, -50.0f, 50.0f, 0.1f, 100000.0f);

    model = gabi::load<u32>(gabi::ea(mpHeadAnm.get()) + 0x90);
    data = gabi::load<u32>(model + 0xAC);
    gabi::store<u32>(model + 0xB8, s);
    for (u16 i = 0; i < gabi::load<u16>(gabi::call<u32>(0x027F3F94, data) + 8); i++) {
        if (i == 8 || i == 10) {
            ship_setJointCallBack(data, i, 0x02471C50);
        } else if ((u32)(i - 2) <= 5) {
            ship_setJointCallBack(data, i, 0x02471E54);
        }
    }

    gabi::store<u32>(gabi::ea(mpCannonModel.get()) + 0xB8, s);
    data = gabi::load<u32>(gabi::ea(mpCannonModel.get()) + 0xAC);
    ship_setJointCallBack(data, 1, 0x02471AAC);
    ship_setJointCallBack(data, 2, 0x02471AAC);
    gabi::store<u32>(gabi::ea(mpSalvageArmModel.get()) + 0xB8, s);
    data = gabi::load<u32>(gabi::ea(mpSalvageArmModel.get()) + 0xAC);
    ship_setJointCallBack(data, 1, 0x02471BF4);

    mTornadoID = fpcM_ERROR_PROCESS_ID_e;
    mTornadoActor = nullptr;
    mWhirlID = fpcM_ERROR_PROCESS_ID_e;
    m034B = (u8)gabi::load<u32>(s + 0xB0);
    mWhirlActor = nullptr;
    mPart = PART_WAIT_e;
    if (mNextMode != MODE_START_MODE_WARP_e) {
        mTactWarpID = fpcM_ERROR_PROCESS_ID_e;
    }
    if (checkStateFlg((daSHIP_SFLG)0x200)) {
        mPart = PART_STEER_e;
        procSteerMove_init();
    } else if (mNextMode == 2 /* MODE_PADDLE_MOVE_e */) {
        procPaddleMove_init();
    } else if (mNextMode == MODE_START_MODE_WARP_e) {
        procStartModeWarp_init();
    } else if (mNextMode == MODE_START_MODE_THROW_e) {
        procStartModeThrow_init();
    } else {
        procWait_init();
    }
    BOOL wide;
    if (ship_getStageType() == 7 /* SEA */) {
        wide = TRUE;
    } else if (ship_isStartStage(0x1003A6DC /* "Hyrule" */)) {
        wide = TRUE;
    } else if (ship_isStartStage(0x1003A6CC /* "Ocean" */)) {
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
    gabi::store<f32>(s + 0x390, current.pos.x);
    gabi::store<f32>(s + 0x394, current.pos.y);
    gabi::store<f32>(s + 0x1530, 10000.0f);
    gabi::store<f32>(s + 0x398, current.pos.z);
    gabi::store<u32>(s + 0x39C, 0);
    gabi::store<u32>(s + 0x1490, gabi::load<u32>(s + 0x1490) & ~0x400u);
    gravity = -2.5f;
    maxFallSpeed = -150.0f;
    mGridID = fopAcM_create(0xAB /* PROC_SAIL */, 1, &current.pos, -1, &current.angle, nullptr, -1, 0);
    if (mGridID == fpcM_ERROR_PROCESS_ID_e) {
        return cPhs_ERROR_e;
    }
    mTactWarpPosNum = -1;
    m03E0 = 10000.0f;
    mStts.Init(0xF0, 0, this);
    for (int i = 0; i < 3; i++) {
        mCyl[i].Set(gabi::at<dCcD_SrcCyl>(0x101D0440));
        gabi::store<u32>(gabi::ea(&mCyl[i]) + 0x44, gabi::ea(&mStts));
    }
    mCyl[1].SetR(95.0f);
    mSph.Set(gabi::at<dCcD_SrcSph>(0x101D0400));
    gabi::store<u32>(gabi::ea(&mSph) + 0x44, gabi::ea(&mStts));
    gabi::call(0x025D672C /* fopAcM_SetMin */, this, -325.0f, -50.0f, -325.0f);
    gabi::call(0x025D673C /* fopAcM_SetMax */, this, 325.0f, 570.0f, 240.0f);
    gabi::call(0x025DADA4 /* fopKyM_create */, 0x16, this, 0, 0, 0);
    offStateFlg((daSHIP_SFLG)0x2);
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
    PSMTXMultVec(model_getAnmMtx(mpBodyAnm->getModel(), FN_BODY_JNT_J_FN_STEER1_e), gabi::at<cXyz>(0x101D03C0 /* l_tiller_top_offset */),
                 &mTillerTopPos);
    mpHeadAnm->play(nullptr, 0, 0);
    J3DModel_setBaseTRMtx(mpHeadAnm->getModel(), model_getAnmMtx(mpBodyAnm->getModel(), FN_BODY_JNT_J_FN_GATTAI_e));
    mpHeadAnm->calc();
    gabi::store<u32>(dComIfGp_ea() + 0x5B3C, s); /* dComIfGp_setShip */
    gabi::store<f32>(s + 0xD124, 3.0f);
    gabi::store<f32>(s + 0xD074, 40.0f); /* mWaveL max speed */
    gabi::store<f32>(s + 0xD18C, 0.0f);
    gabi::store<f32>(s + 0xD010, 40.0f); /* mWaveR max speed */
    if (ship_getStageType() == 7 /* SEA */) {
        gabi::call(0x025D5A20 /* fopAcM_createChild */, 0x10E, gabi::load<u32>(s + 4), 0, &current.pos, -1, 0, 0, -1, 0);
    }
    /* HD: the cannon sight */
    u32 b = s + 0x1B24;
    gabi::store<f32>(b + 0x8, 0.0f);
    gabi::store<u16>(b + 0, 0);
    gabi::store<u16>(b + 2, 0);
    gabi::store<u16>(b + 4, 0);
    for (u32 k = 0xC; k <= 0x24; k += 4) {
        gabi::store<f32>(b + k, 0.0f);
    }
    gabi::store<u16>(b + 0x28, 0);
    gabi::store<f32>(b + 0x2C, -100.0f);
    for (int i = 0; i < 0x80; i++) {
        mRopeStts[i].Init(0, 0xFF, this);
        mRopeSph[i].Set(gabi::at<dCcD_SrcSph>(0x101D0354));
        gabi::store<u32>(gabi::ea(&mRopeSph[i]) + 0x44, gabi::ea(&mRopeStts[i]));
    }
    return phase;
}
VERIFY(0x0247C358, &daShip_c::create);
