/* NPC - Tetra drawing and effects. HD layout, verified against cking.rpx. */
#include "d/actor/d_a_npc_zl1_draw.h"

static u8 sLineCheckBaseVtable;
static u8 sBackgroundCheckVtable;
static u8 sRoofCheckBaseVtable;
static u8 sRoofObjectBaseVtable;
static u8 sRoofVtable;
static u8 sRoofCheckVtable;
static u8 sRoofGroupVtable;
static u8 sRoofObjectVtable;
static u8 sLineVtable;
static u8 sLineCheckVtable;
static u8 sLineGroupVtable;
static u8 sLinePolyVtable;
static cXyz sEyeOffset;
static u32 sEyeOffsetInitialized;
static daNpc_Zl1_HIO_c sDrawHio;
static Mtx34 sCurrentMatrix;
static Zl1Draw::ModelPointer sCurrentModel;
static Zl1Draw::BufferPointer sOpaqueBuffer;
static Zl1Draw::BufferPointer sTranslucentBuffer;
static Zl1Draw::SafeVtable sSafeVtable;
static Zl1Draw::Word sStageName;
static Zl1Draw::Word sDemoName;
static u32 sColorGuardA;
static u32 sColorGuardB;
static u32 sColorGuardC;
static u32 sColorGuardD;
static Zl1Draw::Word sColorA;
static Zl1Draw::Word sColorB;
static Zl1Draw::Word sColorC;
static Zl1Draw::Word sColorD;
static Zl1Draw::Word sColorSourceA;
static Zl1Draw::Word sColorSourceB;
static Zl1Draw::Word sColorSourceC;
static Zl1Draw::Word sColorSourceD;
static Zl1Draw::Play* getDrawPlay();
static Zl1Draw::Emitter* createLandEmitter(void*, cXyz*, csXyz*, f32, f32, f32, dKy_tevstr_c*,
                                           be<s32>*, s32);
static s32 getPolyAttribute(dBgS*, void*);
static csXyz* constructAngles(csXyz*, s16, s16, s16);
static BOOL isCreating(u32);
static s32 searchActor(u32, gptr<fopAc_ac_c>*);
static Mtx34* getBranchMatrix(fopAc_ac_c*, const char*);
static void assureString(Zl1Draw::String*);
static void constructRoof(Zl1DrawRoofCheck*);
static f32 checkRoof(dBgS*, Zl1DrawRoofCheck*);
static void destroyBackgroundCheck(void*, int);
static void enterPacket(void*, void*, int);
static void enterJoint(J3DModel*, J3DJoint*);
static void enterBtp(void*, J3DModel*, f32);
static void registerFigure(s32, fopAc_ac_c*, cXyz*, s16, f32, f32, f32);
static void enterPositionPacket(Zl1Draw::PositionPacket*);
static void enterMorf(mDoExt_McaMorf*);
static void* createLandParticle(dPa_control_c*, void*, cXyz*, csXyz*, f32, f32, f32, dKy_tevstr_c*,
                                be<s32>*, s32);
static Mtx34* drawAnimationMatrix(J3DModel* model, s32 joint) {
    Zl1Draw::Model* view = reinterpret_cast<Zl1Draw::Model*>(model);
    Zl1Draw::MatrixBlock* block = view->mpMatrixBlock;
    block->mFlags |= 0x10;
    return &block->mpMtx.get()[joint];
}
static u32 drawJointNumber(J3DNode* node) {
    return reinterpret_cast<Zl1Draw::Joint*>(J3DNode_toJoint(node))->mJointNo;
}
static s32 animationLastFrame(Zl1Draw::Animation* animation) {
    return gabi::call_ptr<s32>(animation->mpVtable->mGetFrameMax, animation);
}

static inline f32 fsel(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }

static Zl1Draw::Emitter* createLandEmitter(void* poly, cXyz* pos, csXyz* angle, f32 size, f32 scale,
                                           f32 rate, dKy_tevstr_c* tev, be<s32>* attr, s32 type) {
    return reinterpret_cast<Zl1Draw::Emitter*>(createLandParticle(
        dComIfGp_getParticle(), poly, pos, angle, size, scale, rate, tev, attr, type));
}

// HD: sead::SafeString equality of two C strings (both through the per-TU SafeString vtable; * at
// most 0x40001 characters compared)

static bool safestring_equal(be<u8>* str1, be<u8>* str2) {
    gabi::Local<Zl1Draw::String> a;
    gabi::Local<Zl1Draw::String> b;
    a->mpVtable = &sSafeVtable;
    b->mpVtable = &sSafeVtable;
    a->mpString = str1;
    b->mpString = str2;
    assureString(a.get());
    gabi::call_ptr(a->mpVtable->mAssureTermination, a.get());
    be<u8>* s1 = a->mpString;
    gabi::call_ptr(b->mpVtable->mAssureTermination, b.get());
    be<u8>* s2 = b->mpString;
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = s1[i], c2 = s2[i];
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

// HD: each draw-list switch retains two play accessor calls.
static void setListP0() {
    sOpaqueBuffer.mpBuffer = getDrawPlay()->mpListP0;
    sTranslucentBuffer.mpBuffer = getDrawPlay()->mpListP0;
}
static void setListP1() {
    sOpaqueBuffer.mpBuffer = getDrawPlay()->mpListP1;
    sTranslucentBuffer.mpBuffer = getDrawPlay()->mpListP1;
}
static void setListHd() {
    sOpaqueBuffer.mpBuffer = getDrawPlay()->mpListOpa;
    sTranslucentBuffer.mpBuffer = getDrawPlay()->mpListXlu;
}
static void packet_entryOpa(void* packet) { enterPacket(sOpaqueBuffer.mpBuffer.get(), packet, 0); }
static void shape_vis(u32 shape, u8 visible) {
    gabi::at<Zl1Draw::Shape>(shape)->mVisible = visible;
}
static void local_static_init(be<u32>* guard, Zl1Draw::Word* dst, Zl1Draw::Word* src) {
    if (*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, sizeof(Zl1Draw::Word));
    }
}
// HD: select shapes by material index rather than the first two materials.
static void mesh_vis(daNpc_Zl1_c* t, u8 v) {
    Zl1Draw::Mesh* mesh = reinterpret_cast<Zl1Draw::MeshJoint*>(t->mJoint1.get())->mpMesh;
    while (mesh != nullptr) {
        u32 index = mesh->mpMaterial->mIndex;
        if (index == (u32)(s32)t->mB0C || index == (u32)(s32)t->mB10) {
            mesh->mpShape->mVisible = v;
        } else {
            mesh->mpShape->mVisible = !v;
        }
        mesh = mesh->mpNext;
    }
}

/* 023020F8 */
void daNpc_Zl1_c::_nodeCB_Head(J3DNode* i_node, J3DModel* i_pModel) {
    cXyz* a_eye_pos_off = &sEyeOffset;
    if (sEyeOffsetInitialized == 0) {
        sEyeOffsetInitialized = 1;
        a_eye_pos_off->z = 0.0f;
        a_eye_pos_off->x = 20.0f;
        a_eye_pos_off->y = -16.0f;
    }
    u32 jointIdx = drawJointNumber(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(drawAnimationMatrix(i_pModel, jointIdx), stk);
    field_0x770.x = stk->m[0][3];
    field_0x770.y = stk->m[1][3];
    field_0x770.z = stk->m[2][3];
    mDoMtx_YrotM(stk, -field_0x83C);
    mDoMtx_ZrotM(stk, -field_0x83E);
    PSMTXMultVec(stk, a_eye_pos_off, &field_0x74C);
    PSMTXCopy(stk, &sCurrentMatrix);
    mtx_copy(drawAnimationMatrix(i_pModel, jointIdx), stk);
}

/* 02302278 */
static BOOL nodeCB_Head(J3DNode* i_node, int i_param) {
    if (i_param == 0) {
        J3DModel* model = sCurrentModel.mpModel;
        daNpc_Zl1_c* actor = reinterpret_cast<Zl1Draw::Model*>(model)->mpActor;
        if (actor != nullptr)
            actor->_nodeCB_Head(i_node, model);
    }
    return TRUE;
}

/* 023022C0 */
void daNpc_Zl1_c::_nodeCB_BackBone(J3DNode* i_node, J3DModel* i_pModel) {
    u32 jointIdx = drawJointNumber(i_node);
    Mtx34* stk = mDoMtx_stack_c::get();
    PSMTXCopy(drawAnimationMatrix(i_pModel, jointIdx), stk);
    mDoMtx_XrotM(stk, m_jnt.mAngles[1][1]);
    mDoMtx_ZrotM(stk, -m_jnt.mAngles[1][0]);
    PSMTXCopy(stk, &sCurrentMatrix);
    mtx_copy(drawAnimationMatrix(i_pModel, jointIdx), stk);
}

/* 023023E4 */
static BOOL nodeCB_BackBone(J3DNode* i_node, int i_param) {
    if (i_param == 0) {
        J3DModel* model = sCurrentModel.mpModel;
        daNpc_Zl1_c* actor = reinterpret_cast<Zl1Draw::Model*>(model)->mpActor;
        if (actor != nullptr)
            actor->_nodeCB_BackBone(i_node, model);
    }
    return TRUE;
}

/* 023045E0 */
void daNpc_Zl1_c::play_animation() {
    u32 mtrlSnd = 0;
    if (field_0x848 == 1 && field_0x7D0 != 0) {
        setEyeCtrl();
    } else {
        clrEyeCtrl();
    }
    play_btp_anm();
    play_btk_anm();
    if (mObjAcch.ChkGroundHit()) {
        mtrlSnd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), daNpc_gndPoly(this));
    }
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
    field_0x7C3 = (s8)mpMorf->play(&eyePos, mtrlSnd, (s8)reverb);
    if (mpMorf->getFrame() < mFrame) {
        field_0x7C3 = 1;
    }
    mFrame = mpMorf->getFrame();
    switch ((u32)(s32)field_0x847) {
    case 0xD:
    case 0xE:
        mBtpAnmFrame = (u8)gabi::ftoi(mpMorf->getFrame());
        break;
    }
    switch ((u32)(s32)field_0x848) {
    case 6:
    case 7:
    case 8:
        mBtkAnmFrame = (u8)gabi::ftoi(mpMorf->getFrame());
        break;
    }
}

/* 0230473C */
bool daNpc_Zl1_c::swoon_OnShip() {
    Zl1Draw::Ship* pShip = getDrawPlay()->mpShip;
    if (pShip != nullptr) {

        J3DModel* body = pShip->mpMorf->getModel();
        Mtx34* stk = mDoMtx_stack_c::get();
        PSMTXCopy(J3DModel_getBaseTRMtx(body), stk);
        mDoMtx_stack_c::transM(-23.88f, 58.66f, -12.44f);
        mDoMtx_XYZrotM(stk, 0x471C, 0x156, 0x4147);
        current.pos.x = stk->m[0][3];
        current.pos.z = stk->m[2][3];
        current.pos.y = stk->m[1][3];
        current.angle.y = (s16)(pShip->shape_angle.y - 0x8000);
        return true;
    }
    return false;
}

/* 02304C98 */
void daNpc_Zl1_c::setMtx(u32 param_1) {
    bool temp = false;
    J3DModel_setBaseScale(mpMorf->getModel(), &scale);
    Mtx34* stk = mDoMtx_stack_c::get();
    if (field_0x7CE) {
        if (!isCreating((u32)mProcId1) && mProcId1 != 0xFFFFFFFF) {
            gabi::Local<gptr<fopAc_ac_c>> actor;
            if (searchActor((u32)mProcId1, actor.get()) == 1 && actor->get() != nullptr) {
                temp = true;

                PSMTXCopy(getBranchMatrix(actor->get(), "wood2"), stk);
                mDoMtx_stack_c::transM(120.28999f, -88.36f, -19.130001f);
            }
        }
    } else if (field_0x7CA) {
        temp = swoon_OnShip();
    }
    if (!temp) {
        PSMTXTrans(stk, current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(stk, field_0x73E.x, field_0x73E.y, field_0x73E.z);
    }
    J3DModel_setBaseTRMtx(mpMorf->getModel(), stk);
    mpMorf->calc();
    if (mpModel.get() != nullptr) {
        J3DModel_setBaseTRMtx(mpModel, drawAnimationMatrix(mpMorf->getModel(), m_bbone_jnt_num));
        J3DModel_calc(mpModel);
    }
    if (!field_0x7D9) {
        setWaterRipple();
        setEff();
    } else if (safestring_equal(reinterpret_cast<be<u8>*>(&sDemoName),
                                reinterpret_cast<be<u8>*>(&sStageName))) {
        // HD: in the demo stage "Demo37" the ripple runs with the water flag set

        field_0x7C8 = 1;
        setWaterRipple();
    }
    setAttention(param_1);
}

/* 02304268 */
void daNpc_Zl1_c::play_btp_anm() {
    u8 frame =
        (u8)animationLastFrame(reinterpret_cast<Zl1Draw::BtpAnimation*>(mBtpAnm)->mpAnimation);
    if (field_0x847 != 1 || cLib_calcTimer(&mTimer) == 0) {
        u8 f = (u8)(mBtpAnmFrame + 1);
        mBtpAnmFrame = f;
        if (f >= frame) {
            if (field_0x847 != 1) {
                mBtpAnmFrame = frame;
            } else {
                mTimer = (s16)cLib_getRndValue(0x3C, 0x5A);
                mBtpAnmFrame = 0;
            }
        }
    }
}

/* 02304310 */
void daNpc_Zl1_c::eye_ctrl() {
    f32 temp, temp2;
    if (field_0x7D1) {
        f32 div = (f32)(s32)sDrawHio.mPrmTbl.field_5E;
        temp = ((f32)(s32)field_0x842 / div) * 0.1f;
        temp2 = ((f32)(s32)field_0x840 / div) * 0.1f;

        temp = temp < -1.0f ? -1.0f : fsel(temp - 1.0f, 1.0f, temp);
        temp2 = temp2 < -1.0f ? -1.0f : fsel(temp2 - 1.0f, 1.0f, temp2);
    } else {
        temp = 0.0f;
        temp2 = temp;
    }
    if (field_0x6D4[0].get() != nullptr) {
        cLib_addCalc(&field_0x6D4[0]->mOffsetX, temp2, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&field_0x6D4[0]->mOffsetY, temp, 0.5f, 0.1f, 0.03f);
    }
    temp2 = -temp2;
    if (field_0x6D4[1].get() != nullptr) {
        cLib_addCalc(&field_0x6D4[1]->mOffsetX, temp2, 0.5f, 0.1f, 0.03f);
        cLib_addCalc(&field_0x6D4[1]->mOffsetY, temp, 0.5f, 0.1f, 0.03f);
    }
}

/* 02304548 */
void daNpc_Zl1_c::play_btk_anm() {
    u8 frame =
        (u8)animationLastFrame(reinterpret_cast<Zl1Draw::BtkAnimation*>(mBtkAnm)->mpAnimation);
    if (field_0x7D0) {
        eye_ctrl();
    } else {
        u8 f = (u8)(mBtkAnmFrame + 1);
        if (f >= frame) {
            mBtkAnmFrame = frame;
        } else {
            mBtkAnmFrame = f;
        }
    }
}

/* 02304824 */
void daNpc_Zl1_c::setWaterRipple() {
    if (!field_0x7CA) {
        // HD: tests field_0x7C8 (the water flag _execute stored) instead of mObjAcch.ChkWaterIn()

        if (field_0x7C8) {
            if (reinterpret_cast<Zl1Draw::Ripple*>(mRippleCallBack)->mpEmitter.get() == 0) {

                dPa_control_set(dComIfGp_getParticle(), Zl1Draw::PARTICLE_SHIP_TAIL,
                                Zl1Draw::ID_AK_JN_HAMON00, &current.pos, nullptr, nullptr, 0xFF,
                                (dPa_levelEcallBack*)(void*)mRippleCallBack, -1, nullptr, nullptr,
                                nullptr);
            }
            f32 temp = std::fabs((f32)speedF) * 0.1f;
            f32 temp2 = temp * temp;
            temp2 = (temp2 - 1.0f >= 0.0f) ? 1.0f : temp2;
            reinterpret_cast<Zl1Draw::Ripple*>(mRippleCallBack)->mRate = temp2;
        } else {
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)(void*)mRippleCallBack);
        }
    }
}

/* 023048F4 */
void daNpc_Zl1_c::setWaterSplash() {
    gabi::Local<be<s32>> temp;
    Zl1Draw::Emitter* pEmitter = createLandEmitter(daNpc_gndPoly(this), &field_0x77C, &field_0x744,
                                                   1.25f, 0.65f, 1.0f, &tevStr, temp, 6);
    if (pEmitter != 0) {
        pEmitter->mLifeTime = 0xF;
        pEmitter->mDirectionalSpeed = 11.0f;
        pEmitter->mRate = 4.0f;
        pEmitter->mSpread = 0.6f;
    }
}

/* 0230498C */
void daNpc_Zl1_c::set_simpleLand(u32 param_1) {
    gabi::Local<be<s32>> temp;
    s32 temp2 = 6;
    if ((u8)param_1) {
        temp2 = 7;
    }
    Zl1Draw::Emitter* pEmitter = createLandEmitter(daNpc_gndPoly(this), &field_0x77C, &field_0x744,
                                                   1.25f, 0.65f, 0.6f, &tevStr, temp, temp2);
    if (pEmitter != 0) {
        switch ((u32)*temp) {
        case 0x24:
            pEmitter->mSpread = 1.0f;
            pEmitter->mLifeTime = 0xF;
            pEmitter->mRate = 10.0f;
            break;
        case 0x23:
            pEmitter->mSpread = 1.0f;
            pEmitter->mRate = 18.0f;
            break;
        }
    }
}

/* 02304A6C */
void daNpc_Zl1_c::setEff() {
    if (field_0x849 == 0xB || field_0x7C6) {
        // HD: tests field_0x7C8 (the water flag _execute stored) instead of mObjAcch.ChkWaterIn()

        if (field_0x7C8) {
            if (!field_0x7C6) {
                s16 ang = shape_angle.y;
                field_0x77C.x = gabi::fmadds(40.0f, cM_ssin(ang), current.pos.x);
                field_0x77C.z = gabi::fmadds(40.0f, cM_scos(ang), current.pos.z);
                field_0x744.z = 0;
                field_0x744.x = 0;
                field_0x744.y = (s16)(ang - 0x8000);
            }
            field_0x77C.y = mObjAcch.m_wtr.m_height;
            if (getPolyAttribute(dComIfG_Bgsp(), daNpc_gndPoly(this)) == 0x13) {
                f32 y = current.pos.y + 40.0f;
                if (mObjAcch.m_wtr.m_height > y) {
                    setWaterSplash();
                }
            }
        } else {
            if (!field_0x7C6) {
                gabi::Local<csXyz> temp;
                csXyz* t = constructAngles(temp.get(), (s16)current.angle.y, (s16)0, (s16)0);
                field_0x744.x = t->x;
                field_0x744.y = t->y;
                field_0x744.z = t->z;
                field_0x77C.copy(current.pos);
            }
        }
        if (mObjAcch.ChkGroundHit() && (mpMorf->mFrameCtrl.checkPass(13.0f) ||
                                        mpMorf->mFrameCtrl.checkPass(27.0f) || field_0x7C6)) {
            set_simpleLand(field_0x7C6);
            field_0x7C6 = false;
        }
    }
}

/* 02308EC4 */
BOOL daNpc_Zl1_c::setFrontWallType() {
    s16 ang = shape_angle.y;
    f32 sin = cM_ssin(ang);
    f32 cos = cM_scos(ang);

    gabi::Local<Zl1DrawLineCheck> linChk;
    cBgS_LinChk_ct(linChk);
    for (int i = 0; i < 7; i++)
        linChk->mPass[i] = 0;
    linChk->mPolyPassVtable = gabi::ea(&sLinePolyVtable);
    linChk->mLineVtable = gabi::ea(&sLineVtable);
    linChk->mpPolyPassChk = gabi::ea(&linChk->mPolyPassVtable);
    linChk->mpGrpPassChk = gabi::ea(&linChk->mGroupPassVtable);
    linChk->mCheckVtable = gabi::ea(&sLineCheckVtable);
    linChk->mGroupPassVtable = gabi::ea(&sLineGroupVtable);
    linChk->mGrp = 1;

    gabi::Local<Zl1DrawRoofCheck> roofChk;
    constructRoof(roofChk.get());
    roofChk->mRoofVtable = gabi::ea(&sRoofVtable);
    roofChk->mFlags = 1;
    roofChk->mGroupPassVtable = gabi::ea(&sRoofGroupVtable);
    roofChk->mObjectVtable = gabi::ea(&sRoofObjectVtable);
    roofChk->mCheckVtable = gabi::ea(&sRoofCheckVtable);

    f32 wallH = mAcchCir.GetWallH();
    f32 r = mAcchCir.GetWallR() + 25.0f;
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    f32 x = current.pos.x, z = current.pos.z;
    f32 y = current.pos.y + wallH;
    sp14->x = x;
    sp08->x = gabi::fmadds(sin, r, x);
    sp14->z = z;
    sp08->z = gabi::fmadds(cos, r, z);
    sp08->y = y;
    sp14->y = y;
    dBgS_LinChk_Set(linChk, sp14, sp08, this);

    BOOL ret;
    if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        ret = TRUE;
    } else {
        // HD: the plane is checked for NULL

        cM3dGPla* plane = (cM3dGPla*)dBgS_GetTriPla(dComIfG_Bgsp(), linChk->mPolyInfo);
        if (plane == nullptr || std::fabs(plane->mNormal.y) > 0.05f) {
            ret = TRUE;
        } else {
            f32 r2 = mAcchCir.GetWallR() + 25.0f;
            f32 x2 = current.pos.x, z2 = current.pos.z;
            f32 y2 = current.pos.y + 70.1f;
            sp14->x = x2;
            sp08->y = y2;
            sp14->z = z2;
            sp14->y = y2;
            sp08->z = gabi::fmadds(cos, r2, z2);
            sp08->x = gabi::fmadds(sin, r2, x2);
            dBgS_LinChk_Set(linChk, sp14, sp08, this);
            ret = cBgS_LineCross(dComIfG_Bgsp(), linChk);
            if (!ret) {
                roofChk->mPos.copy(current.pos);
                f32 h = checkRoof(dComIfG_Bgsp(), roofChk.get());
                ret = (h - current.pos.y > 80.0f) ? FALSE : TRUE;
            }
        }
    }

    roofChk->mCheckVtable = gabi::ea(&sRoofCheckBaseVtable);
    roofChk->mObjectVtable = gabi::ea(&sRoofObjectBaseVtable);
    roofChk->mGroupPassVtable = gabi::ea(&sBackgroundCheckVtable);
    destroyBackgroundCheck(roofChk->mCheckBase, 0);
    linChk->mCheckVtable = gabi::ea(&sLineCheckBaseVtable);
    linChk->mGroupPassVtable = gabi::ea(&sBackgroundCheckVtable);
    linChk->mPolyPassVtable = gabi::ea(&sLinePolyVtable);
    cBgS_LinChk_dt(linChk, 0);
    return ret == FALSE;
}

/* 02308090 */
BOOL daNpc_Zl1_c::_draw() {
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* modelData = pModel->getModelData();
    if (field_0x7D2 || field_0x7D4) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call

    mpMorf->calc();
    // HD: no pModel->unlock() and no material animation sharing (field_0x860/878)

    setListP0();
    packet_entryOpa(mOnCupOffAupPacket2);

    shape_vis(field_0x890[2], 0);
    shape_vis(field_0x890[5], 0);
    shape_vis(field_0x890[1], 0);
    shape_vis(field_0x890[4], 0);
    shape_vis(field_0x890[0], 1);
    shape_vis(field_0x890[3], 1);
    enterJoint(pModel, mJoint2);
    shape_vis(field_0x8A8[2], 0);
    shape_vis(field_0x8A8[5], 0);
    shape_vis(field_0x8A8[1], 0);
    shape_vis(field_0x8A8[4], 0);
    shape_vis(field_0x8A8[0], 1);
    shape_vis(field_0x8A8[3], 1);
    enterJoint(pModel, mJoint3);
    packet_entryOpa(mOffCupOnAupPacket2);

    mDoExt_btkAnm_entry((mDoExt_btkAnm*)(void*)mBtkAnm, modelData, (f32)(u32)mBtkAnmFrame);
    shape_vis(field_0x890[0], 0);
    shape_vis(field_0x890[3], 0);
    shape_vis(field_0x890[2], 1);
    shape_vis(field_0x890[5], 1);
    enterJoint(pModel, mJoint2);
    shape_vis(field_0x8A8[0], 0);
    shape_vis(field_0x8A8[3], 0);
    shape_vis(field_0x8A8[2], 1);
    shape_vis(field_0x8A8[5], 1);
    enterJoint(pModel, mJoint3);
    // HD: mBtkAnm.remove() moved to the end

    mesh_vis(this, 1);
    enterJoint(pModel, mJoint1);
    mesh_vis(this, 0);

    setListP1();
    packet_entryOpa(mOnCupOffAupPacket1);
    shape_vis(field_0x890[1], 1);
    shape_vis(field_0x890[4], 1);
    shape_vis(field_0x890[2], 0);
    shape_vis(field_0x890[5], 0);
    enterJoint(pModel, mJoint2);
    shape_vis(field_0x8A8[1], 1);
    shape_vis(field_0x8A8[4], 1);
    shape_vis(field_0x8A8[2], 0);
    shape_vis(field_0x8A8[5], 0);
    enterJoint(pModel, mJoint3);
    packet_entryOpa(mOffCupOnAupPacket1);
    shape_vis(field_0x890[1], 0);
    shape_vis(field_0x890[4], 0);
    shape_vis(field_0x8A8[1], 0);
    shape_vis(field_0x8A8[4], 0);

    setListHd(); // HD: another draw list pair

    {
        Zl1Draw::Play* play = getDrawPlay();
        play->mPositionPacket.mPosition.copy(current.pos);
        enterPositionPacket(&play->mPositionPacket);
    }
    enterMorf(mpMorf.get());
    dComIfGd_setList();
    // HD: the btp animations (two) are entered last, with the model

    enterBtp(mBtpAnm, pModel, (f32)(u32)mBtpAnmFrame);
    enterBtp(mBtpAnm2, pModel, (f32)(u32)mBtpAnmFrame);
    reinterpret_cast<Zl1Draw::ModelData*>(modelData)->mpBtk = nullptr;
    reinterpret_cast<Zl1Draw::ModelData*>(modelData)->mpBtp = nullptr;
    if (mpModel.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
        mDoExt_modelEntryDL(mpModel);
    }
    // HD: no shadowDraw(); the snap figure is registered at the branch joint while hanging

    gabi::Local<cXyz> pos;
    if (field_0x7CE) {
        gabi::Local<Mtx34> mtx;
        PSMTXCopy(J3DModel_getBaseTRMtx(mpMorf->getModel()), mtx);
        pos->x = mtx->m[0][3];
        pos->y = mtx->m[1][3];
        pos->z = mtx->m[2][3];
        registerFigure(Zl1Draw::DSNAP_TYPE_NPC_ZL1, this, pos, current.angle.y, 0.5f, 1.2f, 1.0f);
    } else {
        pos->copy(current.pos);
        registerFigure(Zl1Draw::DSNAP_TYPE_NPC_ZL1, this, pos, current.angle.y, 1.0f, 1.0f, 1.0f);
    }

    if (sDrawHio.mPrmTbl.field_20 != 0) {
        if (field_0x84F == 3) {
            local_static_init(&sColorGuardA, &sColorA, &sColorSourceA);
            dComIfGp_get();
            local_static_init(&sColorGuardB, &sColorB, &sColorSourceB);
            local_static_init(&sColorGuardA, &sColorA, &sColorSourceA);
        }
        local_static_init(&sColorGuardC, &sColorC, &sColorSourceC);
        local_static_init(&sColorGuardD, &sColorD, &sColorSourceD);
        local_static_init(&sColorGuardB, &sColorB, &sColorSourceB);
        local_static_init(&sColorGuardC, &sColorC, &sColorSourceC);
    }
    return TRUE;
}

// clang-format off
/* WWHD glue
str 10024008 "wood2"
data sDemoName 10024010
data sEyeOffset 10468BB4
data sEyeOffsetInitialized 10468CD4
data sDrawHio 10468C20
data sCurrentMatrix 104B4868
data sCurrentModel 104B462C
data sOpaqueBuffer 104B4634
data sTranslucentBuffer 104B4638
data sSafeVtable 10023B10
data sStageName 1047E6B8
data sColorGuardA 101FDA48
data sColorGuardB 101FDA44
data sColorGuardC 101FDA50
data sColorGuardD 101FDAC0
data sColorA 101FEBEC
data sColorB 101FEBE8
data sColorC 101FEBF4
data sColorD 101FEBF8
data sColorSourceA 10023AF8
data sColorSourceB 10023AFC
data sColorSourceC 10023B00
data sColorSourceD 10023B04
ext 025200D4 Zl1Draw::Play* getDrawPlay()
ext 025A8BFC void* createLandParticle(dPa_control_c*, void*, cXyz*, csXyz*, f32, f32, f32, dKy_tevstr_c*, be<s32>*, s32)
ext 024EF0F4 s32 getPolyAttribute(dBgS*, void*)
ext 0201A478 csXyz* constructAngles(csXyz*, s16, s16, s16)
ext 025DD868 BOOL isCreating(u32)
ext 025D54C4 s32 searchActor(u32, gptr<fopAc_ac_c>*)
ext 020DF7A8 Mtx34* getBranchMatrix(fopAc_ac_c*, const char*)
ext 0230A308 void assureString(Zl1Draw::String*)
ext 024EE7AC void constructRoof(Zl1DrawRoofCheck*)
ext 024EF6E8 f32 checkRoof(dBgS*, Zl1DrawRoofCheck*)
ext 02008B4C void destroyBackgroundCheck(void*, int)
ext 027F0E04 void enterPacket(void*, void*, int)
ext 027F583C void enterJoint(J3DModel*, J3DJoint*)
ext 025E7BE4 void enterBtp(void*, J3DModel*, f32)
ext 025BEBB8 void registerFigure(s32, fopAc_ac_c*, cXyz*, s16, f32, f32, f32)
ext 0252F4E0 void enterPositionPacket(Zl1Draw::PositionPacket*)
ext 025E5580 void enterMorf(mDoExt_McaMorf*)
data sLineCheckBaseVtable 10023B28
data sBackgroundCheckVtable 10023B38
data sRoofCheckBaseVtable 10023B48
data sRoofObjectBaseVtable 10023B68
data sRoofVtable 10023B78
data sRoofCheckVtable 10023B88
data sRoofGroupVtable 10023B98
data sRoofObjectVtable 10023BA8
data sLineVtable 10023BD8
data sLineCheckVtable 10023BE8
data sLineGroupVtable 10023BF8
data sLinePolyVtable 10023C08
*/
