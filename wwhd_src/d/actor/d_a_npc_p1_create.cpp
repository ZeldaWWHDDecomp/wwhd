/**
 * d_a_npc_p1_create.cpp (WWHD)
 * NPC - Gonzo, Senza, & Nudge (Tetra's pirates): CreateHeap, _create.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_p1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_p1.h"

using namespace p1_cpp;

/* J3DModelData (HD) joint names: 027F68FC returns the joint name table header */
static inline u32 p1c_getJointName(J3DModelData* d) {
    u32 h = p1JointNameHeader(d);
    u32 off = gabi::at<p1_cpp::JointNameHeader>(h)->mNameOffset;
    return off != 0 ? h + 0x10 + off : 0;
}

static inline u16 p1c_getJointNum(J3DModelData* d) {
    return gabi::at<p1_cpp::JointRegistry>(p1JointRegistry(d))->mJointCount;
}
/* getJointNodePointer(i)->setCallBack(cb): HD joint nodes are 0x1C-byte records at *(data + 8), the
 * count at data + 4; an index out of range falls back to the first node (inline bound check) */
static inline void p1c_setJointCallBack(J3DModelData* d, u32 i, u32 cb) {
    auto* data = reinterpret_cast<p1_cpp::ModelDataView*>(d);
    u32 n = data->mJointCount;
    auto* p = data->mpJoints.get();
    if (i < n)
        p = &p1_cpp::entry(p, i);
    p->mCallback = cb;
}
/* 0259F7D4 dNpc_EventCut_c::setActorInfo(const char*, fopAc_ac_c*) */

/* HD: strcmp(dComIfGp_getStartStageName() (play + 0x5134), str) == 0 through two sead::SafeString
 * temporaries (the first one's assureTermination is called twice through the vtable) */

/* the Outset Island sea room with the stage layer 0xA (Senza holding the dora stick) */

/* 022B0AE0 */
BOOL daNpc_P1_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* HD: one body model per type (dRes indices 0x11, 7, 8): GameCube shared one body and swapped
     * its materials in _draw */
    const s32 body_idx[3] = {dRes_INDEX_P1_BDL_P1_e, dRes_INDEX_P1_BDL_P1B_HD_e, dRes_INDEX_P1_BDL_P1C_HD_e};
    const char* arc = "P1";
    J3DModelData* model_data_p =
        (J3DModelData*)dComIfG_getObjectRes(arc, body_idx[(u8)mType], P1_SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(
        arc, dRes_INDEX_P1_BCK_WAIT_e /* BCK_WAIT */, P1_SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, model_data_p, nullptr, nullptr, bck, 2, 1.0f, 0,
                                                  -1, 1, nullptr, 0x80000, 0x11020002);
    mpMorf = morf;
    if (!morf || !morf->getModel()) {
        return FALSE;
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpMorf->getModel()->setBaseTRMtx(mDoMtx_stack_c::get()); /* setBaseTRMtx */
    s8 jnt = p1c_getIndex(p1c_getJointName(model_data_p), "head");
    m_jnt.mHeadJntNum = jnt;
    if (jnt < 0)
        JUT_ASSERT_fail("d_a_npc_p1.cpp", 0x650, "m_jnt.getHeadJntNum() >= 0");
    jnt = p1c_getIndex(p1c_getJointName(model_data_p), "backbone");
    m_jnt.mBackboneJntNum = jnt;
    if (jnt < 0)
        JUT_ASSERT_fail("d_a_npc_p1.cpp", 0x655, "m_jnt.getBackboneJntNum() >= 0");
    jnt = p1c_getIndex(p1c_getJointName(model_data_p), "handR");
    m_handR_jnt_num = jnt;
    if (jnt < 0)
        JUT_ASSERT_fail("d_a_npc_p1.cpp", 0x658, "m_handR_jnt_num >= 0");
    J3DModelData* head_model_data_p;
    if (mType == TYPE_P1A_e) {
        head_model_data_p =
            (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_P1_BDL_P1A_HEAD_e, P1_SAFESTRING_VTBL);
        mpTexture =
            (J3DAnmTexPattern*)dComIfG_getObjectRes(arc, dRes_INDEX_P1_BTP_MABA_A_e, P1_SAFESTRING_VTBL);
    } else if (mType == TYPE_P1B_e) {
        head_model_data_p =
            (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_P1_BDL_P1B_HEAD_e, P1_SAFESTRING_VTBL);
        mpTexture =
            (J3DAnmTexPattern*)dComIfG_getObjectRes(arc, dRes_INDEX_P1_BTP_MABA_B_e, P1_SAFESTRING_VTBL);
    } else {
        head_model_data_p =
            (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_P1_BDL_P1C_HEAD_e, P1_SAFESTRING_VTBL);
        mpTexture =
            (J3DAnmTexPattern*)dComIfG_getObjectRes(arc, dRes_INDEX_P1_BTP_MABA_C_e, P1_SAFESTRING_VTBL);
    }
    void* dummy_tex_all_p =
        dComIfG_getObjectRes(arc, dRes_INDEX_P1_BTP_P1A_DUMMY_TEX_ALL_e, P1_SAFESTRING_VTBL);
    mpHeadModel = mDoExt_J3DModel__create(head_model_data_p, 0x80000, 0x11020002);
    if (mpTexture == nullptr) {
        return FALSE;
    }
    if (!p1c_btpInit(&mBtp, head_model_data_p, dummy_tex_all_p, 1, 2, 1.0f, 0, -1, 0, 0)) {
        return FALSE;
    }
    p1c_btpInit(&mBtp, head_model_data_p, mpTexture.get(), 1, 2, 1.0f, 0, -1, 1, 0);
    if (mType == TYPE_P1B_e && p1c_isOutsetLayerA(this)) {
        mpDoraModel = mDoExt_J3DModel__create(
            (J3DModelData*)dComIfG_getObjectRes(arc, dRes_INDEX_P1_BDL_DORA_STICK_e, P1_SAFESTRING_VTBL),
            0x80000, 0x11000002);
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
    mpMorf->getModel()->mUserArea = gabi::ea(this); /* setUserArea(this) */
    mAcchCir.SetWall(30.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    if (mType == TYPE_P1B_e) {
        if (mParam == 3) {
            mObjAcch.m_flags |= 4; /* SetWallNone */
        }
    }
    return TRUE;
}

/* 022B36A8 */
cPhs_State daNpc_P1_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    /* fopAcM_ct(this, daNpc_P1_c): the inline constructor (HD virtual destructor) */
    u32 cond = actor_condition;
    if (!(cond & 8)) {
        if (gabi::ea(this) != 0) { /* GHS: a constructor allocates when this == NULL (not here: fopAcM_ct) */
            fopAc_ac_c_ct(this);
            __vtbl = P1_VTBL;
            p1c_btpAnm_ct(&mBtp);
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
        actor_condition = cond | 8;
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, "P1");
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
            p1c_setActorInfo(&mEventCut6B0, "P1a", this);
        } else if (param_bit == 1) {
            m671 = 0;
            mType = TYPE_P1B_e;
            p1c_setActorInfo(&mEventCut6B0, "P1b", this);
        } else {
            m671 = 0;
            mType = TYPE_P1C_e;
            p1c_setActorInfo(&mEventCut6B0, "P1c", this);
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
        J3DModel* model = mpMorf->getModel();
        cullMtx = gabi::ea(model->getBaseTRMtx()); /* getBaseTRMtx */
        /* HD: a cull box instead of the profile's cull size */
        fopAcM_setCullSizeBox(this, -120.0f, 0.0f, -120.0f, 120.0f, 250.0f, 120.0f);
        attention_info.flags =
            fopAc_Attn_LOCKON_TALK_e |
            fopAc_Attn_ACTION_SPEAK_e; /* attention_info.flags = LOCKON_TALK | ACTION_SPEAK */
        gravity = -9.0f;
        daNpc_P1_HIO_c* hio = (&l_HIO);
        s32 n = hio->m8;
        if (n < 0) {
            s8 no = mDoHIO_createChild("C¯ºÁ[", hio);
            n = hio->m8;
            hio->mNo = no;
        }
        hio->m8 = n + 1;
        mStts.Init(0xFF, 0xFF, this);
        mCyl.Set(reinterpret_cast<dCcD_SrcCyl*>(&l_cyl_src)); /* l_cyl_src */
        m670 = 0;
        m66C = 0;
        mKajiTimer = 300;
        mAnmNum = -1;
        mCyl.SetStts(&mStts);
        /* attention_info.distances[] */
        if (mType == TYPE_P1B_e) {
            attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAA;  /* fopAc_Attn_TYPE_TALK_e */
            attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAA; /* fopAc_Attn_TYPE_SPEAK_e */
            u8 type = mParam;
            if (type == 1) {
                if (dComIfGp_getStartStagePoint() == 0 || dComIfGp_getStartStagePoint() == 2) {
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
            attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAA;
            attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAA;
            if (!dComIfGs_isEventBit(0x820) && !dComIfGs_isEventBit(0x808)) {
                current.pos.x = gabi::fnmsubs(40.0f, cM_scos((u16)current.angle.y), current.pos.x);
                current.pos.z = gabi::fmadds(40.0f, cM_ssin((u16)current.angle.y), current.pos.z);
                setAnm(4, -1.0f);
                p1_setAction(this, P1_p1c_speakAction);
            } else {
                p1_setAction(this, P1_normalAction);
            }
            mCyl.SetR(100.0f);
        } else {
            attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xAB;
            attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xAB;
            if (m671 != 0) {
                setAnm(9, 0.0f);
                mCyl.SetR(REG_F(10, 5) /* REG10_F(5) */ + 90.0f);
            }
            p1_setAction(this, P1_normalAction);
        }
        if (mType == TYPE_P1B_e && mParam == 1 && dComIfGp_getStartStagePoint() == 1) {
            return cPhs_ERROR_e;
        }
    }
    return state;
}

// clang-format off
/* WWHD glue
str 1001F938 "P1" in 022B0AE0
str 1001F93C "head" in 022B0AE0
str 1001F944 "d_a_npc_p1.cpp" in 022B0AE0
str 1001F954 "m_jnt.getHeadJntNum() >= 0" in 022B0AE0
str 1001F970 "backbone" in 022B0AE0
str 1001F97C "m_jnt.getBackboneJntNum() >= 0" in 022B0AE0
str 1001F930 "handR" in 022B0AE0
str 1001F99C "m_handR_jnt_num >= 0" in 022B0AE0
str 1001FA9C "P1" in 022B36A8
str 1001FA8C "P1a" in 022B36A8
str 1001FA90 "P1b" in 022B36A8
str 1001FA94 "P1c" in 022B36A8
str 1001FAA0 "C¯ºÁ[" in 022B36A8
*/
// clang-format on
