/**
 * d_a_npc_p1_create.cpp (WWHD)
 * NPC - Gonzo, Senza, & Nudge (Tetra's pirates): CreateHeap, _create.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_p1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E789C mDoExt_btpAnm::init(modelData, anm, anmPlay, attr, rate, start, end, modify, entry (stack)) */
static inline s32 p1c_btpInit(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end,
                              u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
/* J3DModelData (HD) joint names: 027F68FC returns the joint name table header */
static inline u32 p1c_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
/* 027DF9B0 JUTNameTab::getIndex */
static inline s8 p1c_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* 027F3F94 (the matcher names it __nw): the model data's joint tree; joint count (u16) at +8 */
static inline u16 p1c_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* getJointNodePointer(i)->setCallBack(cb): HD joint nodes are 0x1C-byte records at *(data + 8), the
 * count at data + 4; an index out of range falls back to the first node (inline bound check) */
static inline void p1c_setJointCallBack(J3DModelData* d, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* 0259F7D4 dNpc_EventCut_c::setActorInfo(const char*, fopAc_ac_c*) */
static inline void p1c_setActorInfo(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a) { gabi::call(0x0259F7D4, c, name, a); }
/* 0259DAA0 dNpc_JntCtrl_c ctor, 0259F740 dNpc_EventCut_c ctor, 025E7820 mDoExt_btpAnm ctor */
static inline void p1c_JntCtrl_ct(void* p) { gabi::call(0x0259DAA0, p); }
static inline void p1c_EventCut_ct(void* p) { gabi::call(0x0259F740, p); }
static inline void p1c_btpAnm_ct(void* p) { gabi::call(0x025E7820, p); }

/* HD: strcmp(dComIfGp_getStartStageName() (play + 0x5134), str) == 0 through two sead::SafeString
 * temporaries (the first one's assureTermination is called twice through the vtable) */
static inline bool p1c_isStartStage(u32 str) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->mStringTop = str;
    a->__vtbl = P1_SAFESTRING_VTBL;
    u32 play = dComIfGp_ea();
    b->__vtbl = P1_SAFESTRING_VTBL;
    b->mStringTop = play + 0x5134;
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2) return false;
        if (c1 == 0) return true;
    }
    return false;
}
/* the Outset Island sea room with the stage layer 0xA (Senza holding the dora stick) */
static inline bool p1c_isOutsetLayerA(daNpc_P1_c* a) {
    if (!p1c_isStartStage(0x1001F92C /* "sea" */)) return false;
    if (a->current.roomNo != 0x2C /* dIsleRoom_OutsetIsland_e */) return false;
    return p1_getStartStageLayer() == 0xA;
}

/* 022B0AE0 */
BOOL daNpc_P1_c::CreateHeap() {
    WWHD_FUNC(0x022B0AE0, BOOL, this);
    /* HD: one body model per type (dRes indices 0x11, 7, 8): GameCube shared one body and swapped
     * its materials in _draw */
    static const s32 body_idx[3] = {0x11, 7, 8};
    const char* arc = STR(0x1001F938); /* "P1" */
    J3DModelData* model_data_p = (J3DModelData*)dComIfG_getObjectRes(arc, body_idx[(u8)mType], P1_SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(arc, 0x28 /* BCK_WAIT */, P1_SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, model_data_p, nullptr, nullptr, bck, 2, 1.0f, 0, -1, 1,
                                                  nullptr, 0x80000, 0x11020002);
    mpMorf = morf;
    if (!morf || !morf->getModel()) {
        return FALSE;
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    p1_mtx_copy(gabi::ea(mpMorf->getModel()) + 0xC8, gabi::ea(mDoMtx_stack_c::get())); /* setBaseTRMtx */
    s8 jnt = p1c_getIndex(p1c_getJointName(model_data_p), STR(0x1001F93C) /* "head" */);
    m_jnt.mHeadJntNum = jnt;
    if (jnt < 0) JUT_ASSERT_fail(STR(0x1001F944), 0x650, STR(0x1001F954));
    jnt = p1c_getIndex(p1c_getJointName(model_data_p), STR(0x1001F970) /* "backbone" */);
    m_jnt.mBackboneJntNum = jnt;
    if (jnt < 0) JUT_ASSERT_fail(STR(0x1001F944), 0x655, STR(0x1001F97C));
    jnt = p1c_getIndex(p1c_getJointName(model_data_p), STR(0x1001F930) /* "handR" */);
    m_handR_jnt_num = jnt;
    if (jnt < 0) JUT_ASSERT_fail(STR(0x1001F944), 0x658, STR(0x1001F99C));
    J3DModelData* head_model_data_p;
    if (mType == TYPE_P1A_e) {
        head_model_data_p = (J3DModelData*)dComIfG_getObjectRes(arc, 0x15, P1_SAFESTRING_VTBL);
        mpTexture = (J3DAnmTexPattern*)dComIfG_getObjectRes(arc, 0xB, P1_SAFESTRING_VTBL);
    } else if (mType == TYPE_P1B_e) {
        head_model_data_p = (J3DModelData*)dComIfG_getObjectRes(arc, 0x16, P1_SAFESTRING_VTBL);
        mpTexture = (J3DAnmTexPattern*)dComIfG_getObjectRes(arc, 0xC, P1_SAFESTRING_VTBL);
    } else {
        head_model_data_p = (J3DModelData*)dComIfG_getObjectRes(arc, 0x17, P1_SAFESTRING_VTBL);
        mpTexture = (J3DAnmTexPattern*)dComIfG_getObjectRes(arc, 0xD, P1_SAFESTRING_VTBL);
    }
    void* dummy_tex_all_p = dComIfG_getObjectRes(arc, 0xE, P1_SAFESTRING_VTBL);
    mpHeadModel = mDoExt_J3DModel__create(head_model_data_p, 0x80000, 0x11020002);
    if (mpTexture == nullptr) {
        return FALSE;
    }
    if (!p1c_btpInit(mBtp, head_model_data_p, dummy_tex_all_p, 1, 2, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    p1c_btpInit(mBtp, head_model_data_p, mpTexture.get(), 1, 2, 1.0f, 0, -1, 1, 0);
    if (mType == TYPE_P1B_e && p1c_isOutsetLayerA(this)) {
        mpDoraModel = mDoExt_J3DModel__create((J3DModelData*)dComIfG_getObjectRes(arc, 0x12, P1_SAFESTRING_VTBL), 0x80000,
                                              0x11000002);
        if (mpDoraModel == nullptr) {
            return FALSE;
        }
    } else {
        mpDoraModel = nullptr;
    }
    for (u16 i = 0; i < p1c_getJointNum(model_data_p); i++) {
        if (i == (u32)(s32)m_jnt.mHeadJntNum || i == (u32)(s32)m_jnt.mBackboneJntNum) {
            p1c_setJointCallBack(model_data_p, i, 0x022B0674 /* nodeCallBack1 */);
        }
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    if (mType == TYPE_P1B_e) {
        if (mParam == 3) {
            mObjAcch.m_flags |= 4; /* SetWallNone */
        }
    }
    return TRUE;
}
VERIFY(0x022B0AE0, &daNpc_P1_c::CreateHeap);

/* 022B36A8 */
cPhs_State daNpc_P1_c::_create() {
    WWHD_FUNC(0x022B36A8, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_P1_c): the inline constructor (HD virtual destructor) */
    u32 cond = actor_condition;
    if (!(cond & 8)) {
        if (gabi::ea(this) != 0) { /* GHS: a constructor allocates when this == NULL (not here: fopAcM_ct) */
            fopAc_ac_c_ct(this);
            gabi::store<u32>(gabi::ea(this) + 0xB4, P1_VTBL);
            p1c_btpAnm_ct(mBtp);
            dBgS_ObjAcch_ct(&mObjAcch, dBgS_ObjAcch_vt{0x1001F8A4, 0x1001F8C4, 0x1001F8B4});
            dBgS_AcchCir_ct(&mAcchCir);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, P1_AAB_VTBL);
            p1c_JntCtrl_ct(&m_jnt);
            p1c_EventCut_ct(&mEventCut6B0);
            /* dNpc_HeadAnm_c inline constructor */
            mHeadAnm.field_0x1C = 0;
            mHeadAnm.field_0x14 = 0.0f;
            mHeadAnm.field_0x00.y = 0;
            mHeadAnm.field_0x20 = 0;
            mHeadAnm.field_0x00.z = 0;
            mHeadAnm.field_0x18 = 0.0f;
            mHeadAnm.field_0x00.x = 0;
            cond = actor_condition;
            mHeadAnm.field_0x1E = 0;
        }
        gabi::store<u32>(gabi::ea(this) + 0x2E4, cond | 8);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x1001FA9C) /* "P1" */);
    if (state == cPhs_COMPLEATE_e) {
        u32 param = mParameters;
        mParam = (u8)param;
        u32 param_bit = (param >> 20) & 0xF;
        if (param_bit == 0) {
            mType = TYPE_P1A_e;
            if (mParam == 2) {
                m_jnt.mbBackBoneLock = 1; /* onBackBoneLock */
                m671 = 1;
                mKajiId = getKajiID();
            } else {
                m671 = 0;
            }
            p1c_setActorInfo(&mEventCut6B0, STR(0x1001FA8C) /* "P1a" */, this);
        } else if (param_bit == 1) {
            m671 = 0;
            mType = TYPE_P1B_e;
            p1c_setActorInfo(&mEventCut6B0, STR(0x1001FA90) /* "P1b" */, this);
        } else {
            m671 = 0;
            mType = TYPE_P1C_e;
            p1c_setActorInfo(&mEventCut6B0, STR(0x1001FA94) /* "P1c" */, this);
        }
        mEventCut6B0.mpJntCtrl = &m_jnt; /* setJntCtrlPtr */
        u32 max_heap_size;
        switch (mType) {
        case TYPE_P1A_e:
            max_heap_size = 0x25C0;
            break;
        case TYPE_P1B_e:
            if (p1c_isOutsetLayerA(this)) {
                max_heap_size = 0x25C0;
            } else {
                max_heap_size = 0x2120;
            }
            break;
        case TYPE_P1C_e:
            max_heap_size = 0x2120;
            break;
        default:
            max_heap_size = 0x15000;
            break;
        }
        if (!fopAcM_entrySolidHeap(this, 0x022B1124 /* CheckCreateHeap */, max_heap_size)) {
            return cPhs_ERROR_e;
        }
        u32 model = gabi::ea(mpMorf->getModel());
        cullMtx = model != 0 ? model + 0xC8 : 0; /* getBaseTRMtx */
        /* HD: a cull box instead of the profile's cull size */
        fopAcM_setCullSizeBox(this, -120.0f, 0.0f, -120.0f, 120.0f, 250.0f, 120.0f);
        gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA); /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
        gravity = -9.0f;
        daNpc_P1_HIO_c* hio = p1_HIO();
        s32 n = hio->m8;
        if (n < 0) {
            s8 no = mDoHIO_createChild(STR(0x1001FAA0) /* "海賊下っ端" */, hio);
            n = hio->m8;
            hio->mNo = no;
        }
        hio->m8 = n + 1;
        mStts.Init(0xFF, 0xFF, this);
        mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101C2BA0)); /* l_cyl_src */
        m670 = 0;
        m66C = 0;
        mKajiTimer = 300;
        mAnmNum = -1;
        mCyl.SetStts(&mStts);
        u32 attn = gabi::ea(this) + 0x388; /* attention_info.distances[] */
        if (mType == TYPE_P1B_e) {
            gabi::store<u8>(attn + 1, 0xAA); /* fopAc_Attn_TYPE_TALK_e */
            gabi::store<u8>(attn + 3, 0xAA); /* fopAc_Attn_TYPE_SPEAK_e */
            u8 type = mParam;
            if (type == 1) {
                if (p1_getStartStagePoint() == 0 || p1_getStartStagePoint() == 2) {
                    p1_setAction(this, P1_speakAction);
                }
            } else if (type == 0) {
                p1_setAction(this, P1_confuseAction);
            } else {
                if (type == 2) {
                    m_jnt.mbBackBoneLock = 1;
                    m_jnt.mbHeadLock = 1;
                    mCyl.SetR(90.0f);
                } else if (type == 3) {
                    m_jnt.mbHeadLock = 1;
                    m_jnt.mbBackBoneLock = 1;
                }
                p1_setAction(this, P1_normalAction);
            }
        } else if (mType == TYPE_P1C_e) {
            gabi::store<u8>(attn + 1, 0xAA);
            gabi::store<u8>(attn + 3, 0xAA);
            if (!p1_isEventBit(0x820) && !p1_isEventBit(0x808)) {
                current.pos.x = gabi::fnmsubs(40.0f, cM_scos((u16)current.angle.y), current.pos.x);
                current.pos.z = gabi::fmadds(40.0f, cM_ssin((u16)current.angle.y), current.pos.z);
                setAnm(4, -1.0f);
                p1_setAction(this, P1_p1c_speakAction);
            } else {
                p1_setAction(this, P1_normalAction);
            }
            mCyl.SetR(100.0f);
        } else {
            gabi::store<u8>(attn + 1, 0xAB);
            gabi::store<u8>(attn + 3, 0xAB);
            if (m671 != 0) {
                setAnm(9, 0.0f);
                mCyl.SetR(gabi::load<f32>(0x1047BBC4) /* REG10_F(5) */ + 90.0f);
            }
            p1_setAction(this, P1_normalAction);
        }
        if (mType == TYPE_P1B_e && mParam == 1 && p1_getStartStagePoint() == 1) {
            return cPhs_ERROR_e;
        }
    }
    return state;
}
VERIFY(0x022B36A8, &daNpc_P1_c::_create);
