/* Nintendo Gallery figurines and pedestal. */
#include "d/actor/d_a_obj_figure.h"
#include <cmath>
using gabi::call;
const gc_tu_data gc_tu = {.safeString = 0x100282AC};
static FigureLightParametersHD l_lightDefaults;
static FigureDataTableHD l_figure_dat_tbl;
static FigureArchiveTableHD l_arcname_tbl;
static FigureCompletionTableHD l_figure_comp;
static Mtx34 l_mtxStack;
static Mtx34 l_identityMtx;
static dCcD_SrcCyl l_cyl_src;
static FigurePhaseTableHD l_method;
static FigureCutNameTableHD cut_name_tbl;
static FigureMoveTableHD moveProc;
static FigureMessagePointerHD l_messageManager;
static FigureJ3DSysHD l_renderState;
static FigureDrawPacketHD l_offCupOnAupPacket1;
static u32 l_offCupOnAupPacket1Guard;
static FigureDtorRecordHD l_offCupOnAupPacket1Dtor;
static FigureDrawPacketHD l_offCupOnAupPacket2;
static u32 l_offCupOnAupPacket2Guard;
static FigureDtorRecordHD l_offCupOnAupPacket2Dtor;
static FigureDrawPacketHD l_onCupOffAupPacket1;
static u32 l_onCupOffAupPacket1Guard;
static FigureDtorRecordHD l_onCupOffAupPacket1Dtor;
static FigureDrawPacketHD l_onCupOffAupPacket2;
static u32 l_onCupOffAupPacket2Guard;
static FigureDtorRecordHD l_onCupOffAupPacket2Dtor;

// HD: preserve this translation unit's common-header initialization order.
static FigureHeaderStaticsHD l_headerStatics;
static FigureHeaderZerosHD l_headerZeros;
static FigureDtorRecordHD l_headerDtor;
static FigureDtorRecordHD l_firstHeaderDtor;
static FigureDtorRecordHD l_secondHeaderDtor;
u32 PrmAbstract(u32 a, s32 width, s32 shift);
static daPy_py_c* figureLinkPlayerHD() {
    return reinterpret_cast<daPy_py_c*>(
        reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mpPlayerPtr[0].get());
}
// HD: the resource name occupies the same eight-byte guest temporary as the original.
static u32 resource(const char* name, s32 index) { return gabi::ea(dComIfG_getObjectIDRes(name, index)); }
static void copyMatrix(const Mtx34* src, Mtx34* dst) { J3DModel::mtx_assign(dst, src); }
static s32 truncWord(f64 value) {
    if (!std::isfinite(value) || value >= 2147483648.0 || value < -2147483648.0)
        return (s32)0x80000000u;
    return (s32)value;
}
static bool equalText(u32 left, u32 right) {
    if (left == right)
        return true;
    // HD: SafeString compares at most 0x40001 bytes, preserving wrapped guest addresses.
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = *gabi::at<const be<u8>>(left + i), b = *gabi::at<const be<u8>>(right + i);
        if (a != b)
            return false;
        if (!a)
            return true;
    }
    return false;
}
static u32 assureTerminationTarget(SafeString* string) {
    return gabi::at<FigureSafeStringVtableHD>(string->__vtbl)->mAssureTermination.get();
}
static bool rootNameMatches(SafeString* strings, u32 name, u32 incoming) {
    u32 slot = incoming / sizeof(SafeString);
    strings[slot].mStringTop = name;
    strings[slot].__vtbl = gc_tu.safeString;
    u32 target = assureTerminationTarget(&strings[0]);
    call<void>(target, &strings[0], 0);
    target = assureTerminationTarget(&strings[0]);
    call<void>(target, &strings[0]);
    target = assureTerminationTarget(&strings[slot]);
    u32 left = strings[0].mStringTop;
    call<void>(target, &strings[slot]);
    if (equalText(left, strings[slot].mStringTop))
        return true;
    strings[slot + 2].mStringTop = name;
    strings[slot + 2].__vtbl = gc_tu.safeString;
    target = assureTerminationTarget(&strings[1]);
    call<void>(target, &strings[1], 1);
    target = assureTerminationTarget(&strings[1]);
    call<void>(target, &strings[1]);
    target = assureTerminationTarget(&strings[slot + 2]);
    left = strings[1].mStringTop;
    call<void>(target, &strings[slot + 2]);
    return equalText(left, strings[slot + 2].mStringTop);
}
static FigureDrawListHD* figureDrawList() {
    // HD: these Gallery lists precede the normal lists in dDlst_list_c.
    return reinterpret_cast<FigureDrawListHD*>(&reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->drawlist);
}

/* 0233F6DC */
u32 daObjFigure_constructHD(u32 a) {
    if (!a) {
        a = gabi::ea(operator_new(sizeof(daObjFigure_c)));
        if (!a)
            return 0;
    }
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    fopAc_ac_c_ct(actor);
    actor->__vtbl = daObjFigure_c::__vt;
    mDoExt_btpAnm::ct(&actor->mBtpAnm1);
    mDoExt_btpAnm::ct(&actor->mBtpAnm2);
    auto* light = reinterpret_cast<FigureLightParametersHD*>(&actor->mTevStrBG2.mLightObj);
    auto* lightCopy1 = reinterpret_cast<FigureLightParametersHD*>(&actor->mTevStrBG2.mHdLightParam[0]);
    auto* lightCopy2 = reinterpret_cast<FigureLightParametersHD*>(&actor->mTevStrBG2.mHdLightParam[0x84]);
    f32 defaultPositionX = (f32)l_lightDefaults.mPosition.x;
    f32 defaultPositionY = (f32)l_lightDefaults.mPosition.y;
    f32 defaultPositionZ = (f32)l_lightDefaults.mPosition.z;
    f32 defaultDirectionX = (f32)l_lightDefaults.mDirection.x;
    f32 defaultDirectionY = (f32)l_lightDefaults.mDirection.y;
    f32 defaultDirectionZ = (f32)l_lightDefaults.mDirection.z;
    u8 defaultColorR = (u8)l_lightDefaults.mColor.r;
    u8 defaultColorG = (u8)l_lightDefaults.mColor.g;
    u8 defaultColorB = (u8)l_lightDefaults.mColor.b;
    u8 defaultColorA = (u8)l_lightDefaults.mColor.a;
    s16 defaultColorS10R = (s16)l_lightDefaults.mColorS10.r;
    s16 defaultColorS10G = (s16)l_lightDefaults.mColorS10.g;
    s16 defaultColorS10B = (s16)l_lightDefaults.mColorS10.b;
    s16 defaultColorS10A = (s16)l_lightDefaults.mColorS10.a;
    f32 defaultCosAtten0 = (f32)l_lightDefaults.mCosAtten[0];
    f32 defaultCosAtten1 = (f32)l_lightDefaults.mCosAtten[1];
    f32 defaultCosAtten2 = (f32)l_lightDefaults.mCosAtten[2];
    f32 defaultDistAtten0 = (f32)l_lightDefaults.mDistAtten[0];
    f32 defaultDistAtten1 = (f32)l_lightDefaults.mDistAtten[1];
    f32 defaultDistAtten2 = (f32)l_lightDefaults.mDistAtten[2];
    f32 defaultCutoff = (f32)l_lightDefaults.mCutoff;
    f32 defaultReferenceBrightness = (f32)l_lightDefaults.mReferenceBrightness;
    light->mPosition.x = defaultPositionX;
    light->mPosition.y = defaultPositionY;
    light->mPosition.z = defaultPositionZ;
    light->mDirection.x = defaultDirectionX;
    light->mDirection.y = defaultDirectionY;
    light->mDirection.z = defaultDirectionZ;
    light->mColor.r = defaultColorR;
    light->mColor.g = defaultColorG;
    light->mColor.b = defaultColorB;
    light->mColor.a = defaultColorA;
    light->mColorS10.r = defaultColorS10R;
    light->mColorS10.g = defaultColorS10G;
    light->mColorS10.b = defaultColorS10B;
    light->mColorS10.a = defaultColorS10A;
    light->mCosAtten[0] = defaultCosAtten0;
    light->mCosAtten[1] = defaultCosAtten1;
    light->mCosAtten[2] = defaultCosAtten2;
    light->mDistAtten[0] = defaultDistAtten0;
    light->mDistAtten[1] = defaultDistAtten1;
    light->mDistAtten[2] = defaultDistAtten2;
    light->mCutoff = defaultCutoff;
    lightCopy1->mColor.b = defaultColorB;
    lightCopy1->mReferenceBrightness = defaultReferenceBrightness;
    lightCopy1->mDirection.z = defaultDirectionZ;
    light->mReferenceBrightness = defaultReferenceBrightness;
    lightCopy2->mColor.r = defaultColorR;
    lightCopy1->mPosition.x = defaultPositionX;
    lightCopy2->mColorS10.a = defaultColorS10A;
    lightCopy1->mDistAtten[0] = defaultDistAtten0;
    lightCopy1->mColorS10.g = defaultColorS10G;
    lightCopy1->mDirection.x = defaultDirectionX;
    lightCopy1->mCosAtten[0] = defaultCosAtten0;
    lightCopy1->mDistAtten[2] = defaultDistAtten2;
    lightCopy2->mDirection.x = defaultDirectionX;
    lightCopy2->mCosAtten[2] = defaultCosAtten2;
    lightCopy1->mDirection.y = defaultDirectionY;
    lightCopy2->mColor.b = defaultColorB;
    lightCopy1->mCosAtten[2] = defaultCosAtten2;
    lightCopy1->mColor.a = defaultColorA;
    lightCopy1->mPosition.z = defaultPositionZ;
    lightCopy2->mColorS10.r = defaultColorS10R;
    lightCopy2->mPosition.y = defaultPositionY;
    lightCopy1->mPosition.y = defaultPositionY;
    lightCopy1->mColor.g = defaultColorG;
    lightCopy2->mColorS10.g = defaultColorS10G;
    lightCopy2->mDistAtten[0] = defaultDistAtten0;
    lightCopy2->mDirection.y = defaultDirectionY;
    lightCopy1->mCutoff = defaultCutoff;
    lightCopy2->mColor.a = defaultColorA;
    lightCopy2->mPosition.z = defaultPositionZ;
    lightCopy2->mCosAtten[1] = defaultCosAtten1;
    lightCopy2->mColorS10.b = defaultColorS10B;
    lightCopy1->mColorS10.b = defaultColorS10B;
    lightCopy1->mColor.r = defaultColorR;
    lightCopy1->mColorS10.r = defaultColorS10R;
    lightCopy1->mDistAtten[1] = defaultDistAtten1;
    lightCopy1->mColorS10.a = defaultColorS10A;
    lightCopy2->mDistAtten[1] = defaultDistAtten1;
    lightCopy1->mCosAtten[1] = defaultCosAtten1;
    lightCopy2->mColor.g = defaultColorG;
    lightCopy2->mCosAtten[0] = defaultCosAtten0;
    lightCopy2->mDistAtten[2] = defaultDistAtten2;
    lightCopy2->mDirection.z = defaultDirectionZ;
    lightCopy2->mPosition.x = defaultPositionX;
    lightCopy2->mCutoff = defaultCutoff;
    lightCopy2->mReferenceBrightness = defaultReferenceBrightness;
    dCcD_Stts_ct(&actor->mStts);
    dCcD_Cyl_ct(&actor->mCyl1, daObjFigure_c::CYL_AAB_VTABLE_HD);
    Figure_secondCyl_ctorHD(&actor->mCyl2);
    Figure_eventCut_ctorHD(&actor->mEventCut);
    u32 no = actor->getPrmFigureNo();
    if (no != 0xFF && no < 0x86) {
        s16 angle = (s16)actor->current.angle.y;
        actor->mFigureNo = no;
        actor->m734 = angle;
        u8 display = actor->isFigureGet((u8)no);
        actor->m734 = 0;
        actor->mbDisplay = display;
        actor->m732 = 0;
        angle = (s16)actor->current.angle.y;
        actor->m73C = 0;
        actor->m736 = angle;
    } else {
        s16 angle = (s16)actor->current.angle.y;
        actor->mFigureNo = 0;
        actor->m734 = angle;
        u8 display = actor->isFigureGet(0);
        angle = (s16)actor->current.angle.y;
        actor->mbDisplay = display;
        actor->m736 = angle;
        actor->m734 = 0;
        actor->m732 = 0;
        actor->m73C = 0;
    }
    return a;
}
/* 0233F630 */
bool CheckCreateHeap(u32 a) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    return actor->createHeap();
}
/* 0233F9C4 */
s32 phase_1(u32 a) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    u32 flags = (u32)actor->actor_condition;
    if (!(flags & 8)) {
        if (a) {
            daObjFigure_constructHD(a);
            flags = (u32)actor->actor_condition;
        }
        actor->actor_condition = flags | 8;
    }
    actor->mLoadFlags = (u8)actor->mLoadFlags | 1;
    s32 phase = dComIfG_resLoad(&actor->mPhsFigure, "Figure");
    return phase == 4 ? 2 : phase;
}
/* 0233FDD8 */
s32 phase_2(u32 a) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    u8 flags = (u8)actor->mLoadFlags, no = (u8)actor->mFigureNo;
    actor->mLoadFlags = flags | 2;
    s32 room = Figure_getFigureRoomHD(no);
    s32 overrideRoom = (s32)l_figure_dat_tbl.mData[(u32)no].mRoomId;
    if (overrideRoom >= 0)
        room = overrideRoom;
    s32 phase = dComIfG_resLoad(&actor->mPhsRoomArc, l_arcname_tbl.mNames[(u32)room]);
    if (phase == 4) {
        if (!fopAcM_entrySolidHeap(actor, reinterpret_cast<BOOL (*)(fopAc_ac_c*)>(CheckCreateHeap),
                                   no == 0x40 ? 0x25000 : 0xCD90))
            return 5;
        return actor->createInit();
    }
    return phase;
}
/* 0233FEB0 */
s32 daObjFigure_c::_create() { return Figure_phaseHandlerHD(&mPhsLoad, &l_method, this); }
/* 0233F37C */
bool daObjFigure_c::createHeap() {
    s32 room = Figure_getFigureRoomHD((u8)mFigureNo);
    u32 no = (u8)mFigureNo;
    s32 overrideRoom = (s32)l_figure_dat_tbl.mData[no].mRoomId;
    if (overrideRoom >= 0)
        room = overrideRoom;
    const char* arc = l_arcname_tbl.mNames[(u32)room];
    s32 index = getFigureBmd(no);
    u32 data = resource(arc, index);
    if (!data)
        return false;
    no = (u8)mFigureNo;
    u32 model = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(data), 0x80000,
                                                 (u32)l_figure_dat_tbl.mData[no].mModelAttr));
    mpModel.v = model;
    if (!model)
        return false;
    no = (u8)mFigureNo;
    if (no == 0x3D) {
        u32 animation = resource(arc, 0x1A);
        if (!animation)
            return false;
        u32 controller = gabi::ea(Figure_newBrkHD(nullptr));
        mpBrkAnm.v = controller;
        if (!controller)
            return false;
        if (!gabi::at<mDoExt_brkAnm>(controller)
                 ->init(gabi::at<J3DModelData>(data), gabi::at<J3DAnmTevRegKey>(animation), true,
                        J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0))
            return false;
        no = (u8)mFigureNo;
    }
    if (no == 0x40) {
        u32 pedestal = resource(arc, 0x15);
        u32 animation = resource(arc, 0);
        u32 morf = gabi::ea(mDoExt_McaMorf::create(
            nullptr, gabi::at<J3DModelData>(pedestal), nullptr, nullptr, gabi::at<J3DAnmTransform>(animation),
            J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1, nullptr, 0x80000, 0x11001222));
        mpMorf.v = morf;
        if (!morf || !gabi::at<mDoExt_McaMorf>(morf)->mpModel.v)
            return false;
    }
    u32 pedestal = resource("Figure", 1);
    if (!pedestal)
        return false;
    model = gabi::ea(mDoExt_J3DModel__create(gabi::at<J3DModelData>(pedestal), 0x80000, 0x11020022));
    mpPedestalModel.v = model;
    if (!model)
        return false;
    u32 pattern = resource("Figure", 2);
    mpPedestalBtp.v = pattern;
    if (!pattern)
        return false;
    return mBtpAnm1.init(gabi::at<J3DModelData>(pedestal), gabi::at<J3DAnmTexPattern>(pattern), true,
                         J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0) != 0;
}
/* 0233FC60 */
s32 daObjFigure_c::createInit() {
    mStts.Init(0xFF, 0xFF, this);
    mCyl1.Set(&l_cyl_src);
    mCyl1.SetStts(&mStts);
    mCyl2.Set(&l_cyl_src);
    mCyl2.SetStts(&mStts);
    mCyl2.mObjCo.mSPrm = 0x129;
    mCyl2.SetR(200.0f);
    mCyl2.SetH(100.0f);
    u32 matrix = (u32)mpPedestalModel.v;
    if (matrix)
        matrix += 0xC8;
    cullMtx = matrix;
    // HD: the cull box extends to 450 rather than the GC height of 200.
    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 450.0f, 50.0f);
    s32 event = dComIfGp_evmng_getEventIdx("FIGURE_CHECK", 0xFF);
    mFigureCheckEvtIdx = event;
    mEventCut.setActorInfo("Figure", this);
    s16 id = (s16)mFigureCheckEvtIdx;
    s8 room = (s8)home.roomNo;
    eventInfo.mEventId = id;
    attention_info.flags = 0x2000000A;
    u16 rz = (u16)current.angle.z, rx = (u16)current.angle.x, ry = (u16)current.angle.y;
    attention_info.distances[3] = 0xA8;
    attention_info.distances[1] = 0xA8;
    shape_angle.x = rx;
    shape_angle.z = rz;
    shape_angle.y = ry;
    Figure_tevInitHD(&mTevStrBG2, room, 0xFF);
    MTXCopy(&l_identityMtx, &mMtx);
    setMtx();
    return 4;
}
/* 0233FEC8 */
bool daObjFigure_c::_delete() {
    dComIfG_resDelete(&mPhsFigure, "Figure");
    s32 room = Figure_getFigureRoomHD((u8)mFigureNo);
    s32 overrideRoom = (s32)l_figure_dat_tbl.mData[(u32)(u8)mFigureNo].mRoomId;
    if (overrideRoom >= 0)
        room = overrideRoom;
    dComIfG_resDelete(&mPhsRoomArc, l_arcname_tbl.mNames[(u32)room]);
    return true;
}
/* 023409C0 */
bool daObjFigure_c::_draw() {
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, 3, &current.pos, &mTevStrBG2);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &mTevStrBG2);
    env = dKy_getEnvlight();
    settingTevStruct(env, TEV_TYPE_BG0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpPedestalModel, &tevStr);
    J3DModel* model = mpPedestalModel;
    u8 display = mbDisplay;
    auto* data = reinterpret_cast<FigureModelDataHD*>(model->mModelData.get());
    if (!display) {
        // HD: only the Link figurine suppresses its pedestal here.
        if ((u8)mFigureNo != 0x40) {
            mBtpAnm1.entry(data, 0);
            mDoExt_modelUpdateDL(mpPedestalModel, 0);
            data->mpBtp = 0;
        }
        return true;
    }
    mDoExt_brkAnm* anim = mpBrkAnm;
    if (anim) {
        model = mpModel;
        f32 frame = anim->mFrameCtrl.mFrame;
        anim->entry(model->mModelData, frame);
    }
    mBtpAnm1.entry(data, 1);
    mDoExt_modelUpdateDL(mpPedestalModel, 0);
    mDoExt_modelUpdateDL(mpModel, 0);
    data->mpBtp = 0;
    if (mpBrkAnm.v)
        reinterpret_cast<FigureModelDataHD*>(mpModel->mModelData.get())->mpBrk = 0;
    mDoExt_McaMorf* morf = mpMorf;
    if (!morf)
        return true;
    env = dKy_getEnvlight();
    setLightTevColorType(env, morf->mpModel, &mTevStrBG2);
    morf = mpMorf;
    morf->calc();
    model = morf->mpModel;
    u32 flag = l_offCupOnAupPacket1Guard;
    data = reinterpret_cast<FigureModelDataHD*>(model->mModelData.get());
    l_renderState.mpModel = model;
    if (!flag) {
        l_offCupOnAupPacket1Guard = 1;
        Figure_packetConstructHD(&l_offCupOnAupPacket1);
        l_offCupOnAupPacket1.mVtable = FigureDrawPacketHD::OFF_CUP_ON_AUP_VTABLE;
        Figure_registerDtorHD(&l_offCupOnAupPacket1Dtor);
    }
    if (!l_offCupOnAupPacket2Guard) {
        l_offCupOnAupPacket2Guard = 1;
        Figure_packetConstructHD(&l_offCupOnAupPacket2);
        l_offCupOnAupPacket2.mVtable = FigureDrawPacketHD::OFF_CUP_ON_AUP_VTABLE;
        Figure_registerDtorHD(&l_offCupOnAupPacket2Dtor);
    }
    if (!l_onCupOffAupPacket1Guard) {
        l_onCupOffAupPacket1Guard = 1;
        Figure_packetConstructHD(&l_onCupOffAupPacket1);
        l_onCupOffAupPacket1.mVtable = FigureDrawPacketHD::ON_CUP_OFF_AUP_VTABLE;
        Figure_registerDtorHD(&l_onCupOffAupPacket1Dtor);
    }
    if (!l_onCupOffAupPacket2Guard) {
        l_onCupOffAupPacket2Guard = 1;
        Figure_packetConstructHD(&l_onCupOffAupPacket2);
        l_onCupOffAupPacket2.mVtable = FigureDrawPacketHD::ON_CUP_OFF_AUP_VTABLE;
        Figure_registerDtorHD(&l_onCupOffAupPacket2Dtor);
    }
    u32 count = data->mJointNum;
    auto* joints = gabi::at<FigureJointHD>(data->mJointNodes);
    FigureJointHD* root = joints;
    FigureJointHD* eye = count > 0x13 ? &joints[0x13] : joints;
    FigureJointHD* eyebrow = count > 0x15 ? &joints[0x15] : joints;
    FigureMaterialHD* material = (count > 0x13 ? &joints[0x13] : joints)->mpMesh;
    // HD: retain the original 0xB0 guest frame, with adjacent SafeStrings and shape lists.
    gabi::Local<FigureDrawFrameHD> guestFrame;
    guestFrame->mpMorf = morf;
    gptr<J3DShape>* shapes = guestFrame->mShapes;
    for (u32 i = 0; i < 12; ++i)
        shapes[i].v = 0;
    u32 zoff_blend_cnt = 0, zoff_none_cnt = 0, zon_cnt = 0;
    for (int pass = 0; pass < 2; ++pass) {
        while (material) {
            FigurePEBlockHD* block = material->mpPEBlock;
            const be<u8>* mode = call<const be<u8>*>(block->mVtable->mGetZMode.get(), block);
            if (!*mode) {
                block = material->mpPEBlock;
                mode = call<const be<u8>*>(block->mVtable->mGetBlend.get(), block);
                u8 blend = *mode;
                J3DShape* shape = material->mpShape;
                if (blend == 1) {
                    shapes[zoff_blend_cnt] = shape;
                    ++zoff_blend_cnt;
                    if ((s32)zoff_blend_cnt > 4)
                        JUTAssertion_showAssert("d_a_obj_figure.cpp", 0x748, "zoff_blend_cnt <= 4");
                } else {
                    shapes[4 + zoff_none_cnt] = shape;
                    ++zoff_none_cnt;
                    if ((s32)zoff_none_cnt > 4)
                        JUTAssertion_showAssert("d_a_obj_figure.cpp", 0x74B, "zoff_none_cnt <= 4");
                }
            } else {
                shapes[8 + zon_cnt] = material->mpShape;
                ++zon_cnt;
                if ((s32)zon_cnt > 4)
                    JUTAssertion_showAssert("d_a_obj_figure.cpp", 0x74F, "zon_cnt <= 4");
            }
            material = material->mpNext;
        }
        count = data->mJointNum;
        joints = gabi::at<FigureJointHD>(data->mJointNodes);
        material = (count > 0x15 ? &joints[0x15] : joints)->mpMesh;
    }
    l_renderState.mpOpaBuffer = figureDrawList()->mpListP0;
    J3DDrawBuffer* buffer = figureDrawList()->mpListP0;
    J3DDrawBuffer* opaque = l_renderState.mpOpaBuffer;
    l_renderState.mpXluBuffer = buffer;
    opaque->entryImm(reinterpret_cast<J3DPacket*>(&l_onCupOffAupPacket2), 0);
    for (u32 i = 0; i < 4; ++i) {
        J3DShape* shape = shapes[i];
        if (shape)
            shape->hide();
        shape = shapes[8 + i];
        if (shape)
            shape->hide();
        shape = shapes[4 + i];
        if (shape)
            shape->show();
    }
    reinterpret_cast<J3DJoint*>(eye)->entryIn(model);
    reinterpret_cast<J3DJoint*>(eyebrow)->entryIn(model);
    l_renderState.mpOpaBuffer->entryImm(reinterpret_cast<J3DPacket*>(&l_offCupOnAupPacket2), 0);
    for (u32 i = 0; i < 4; ++i) {
        J3DShape* shape = shapes[i];
        if (shape)
            shape->show();
        shape = shapes[4 + i];
        if (shape)
            shape->hide();
    }
    reinterpret_cast<J3DJoint*>(eye)->entryIn(model);
    reinterpret_cast<J3DJoint*>(eyebrow)->entryIn(model);
    material = root->mpMesh;
    SafeString* strings = guestFrame->mStrings;
    strings[0].mStringTop = gabi::guest_str("face");
    strings[0].__vtbl = gc_tu.safeString;
    strings[1].mStringTop = gabi::guest_str("ear_2_");
    strings[1].__vtbl = gc_tu.safeString;
    while (material) {
        FigureMaterialRecordHD* record = material->mpData;
        u32 offset = record->mNameOffset;
        u32 name = offset ? gabi::ea(&record->mNameOffset) + offset : 0;
        if (!rootNameMatches(strings, name, 16))
            material->mpShape->hide();
        material = material->mpNext;
    }
    reinterpret_cast<J3DJoint*>(root)->entryIn(model);
    material = root->mpMesh;
    while (material) {
        FigureMaterialRecordHD* record = material->mpData;
        u32 offset = record->mNameOffset;
        u32 name = offset ? gabi::ea(&record->mNameOffset) + offset : 0;
        bool match = rootNameMatches(strings, name, 24);
        material->mpShape->mVisible = match ? 0 : 1;
        material = material->mpNext;
    }
    l_renderState.mpOpaBuffer = figureDrawList()->mpListP1;
    buffer = figureDrawList()->mpListP1;
    opaque = l_renderState.mpOpaBuffer;
    l_renderState.mpXluBuffer = buffer;
    opaque->entryImm(reinterpret_cast<J3DPacket*>(&l_onCupOffAupPacket1), 0);
    for (u32 i = 0; i < 4; ++i) {
        J3DShape* shape = shapes[i];
        if (shape)
            shape->hide();
        shape = shapes[8 + i];
        if (shape)
            shape->show();
        shape = shapes[4 + i];
        if (shape)
            shape->hide();
    }
    reinterpret_cast<J3DJoint*>(eye)->entryIn(model);
    reinterpret_cast<J3DJoint*>(eyebrow)->entryIn(model);
    l_renderState.mpOpaBuffer->entryImm(reinterpret_cast<J3DPacket*>(&l_offCupOnAupPacket1), 0);
    for (u32 i = 0; i < 4; ++i) {
        J3DShape* shape = shapes[8 + i];
        if (shape)
            shape->hide();
    }
    l_renderState.mpOpaBuffer = figureDrawList()->mpOpa;
    l_renderState.mpXluBuffer = figureDrawList()->mpXlu;
    mDoExt_modelEntryDL(guestFrame->mpMorf->mpModel);
    material = root->mpMesh;
    while (material) {
        material->mpShape->show();
        material = material->mpNext;
    }
    count = data->mJointNum;
    joints = gabi::at<FigureJointHD>(data->mJointNodes);
    (count > 0x14 ? &joints[0x14] : joints)->mpMesh->mpShape->show();
    count = data->mJointNum;
    joints = gabi::at<FigureJointHD>(data->mJointNodes);
    (count > 0x29 ? &joints[0x29] : joints)->mpMesh->mpShape->show();
    return true;
}
/* 02340858 */
bool daObjFigure_c::_execute() {
    if (!dComIfGp_event_runCheck()) {
        (this->*moveProc.mProc)();
    } else
        eventMove();
    if ((u8)mbDisplay || (u8)mFigureNo != 0x40) {
        mCyl1.SetC(&current.pos);
        mCyl2.SetC(&current.pos);
        cCcS_Set(dComIfG_Ccsp(), &mCyl1);
        cCcS_Set(dComIfG_Ccsp(), &mCyl2);
    }
    f32 y = (f32)current.pos.y, x = (f32)current.pos.x, z = (f32)current.pos.z;
    u32 animation = (u32)mpBrkAnm.v;
    eyePos.x = x;
    eyePos.y = (f32)(y + 150.0f);
    attention_info.position.x = x;
    eyePos.z = z;
    attention_info.position.z = z;
    attention_info.position.y = (f32)(y + 200.0f);
    if (animation)
        gabi::at<mDoExt_brkAnm>(animation)->play();
    setMtx();
    return true;
}
/* 02341360 */
void daObjFigure_c::executeNormal() {
    if (!(u8)mbDisplay)
        return;
    auto* info = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    f32 x = current.pos.x;
    fopAc_ac_c* player = info->play.mpPlayerPtr[0];
    f32 y = current.pos.y, z = current.pos.z;
    gabi::Local<cXyz> source, target;
    source->x = x;
    source->y = y;
    source->z = z;
    x = player->current.pos.x;
    y = player->current.pos.y;
    z = player->current.pos.z;
    target->x = x;
    target->y = y;
    target->z = z;
    gabi::Local<be<f32>> distance;
    gabi::Local<be<s16>> angle;
    dNpc_calc_DisXZ_AngY(source.get(), target.get(), distance.get(), angle.get());
    s16 facing = current.angle.y, heading = *angle.get();
    s16 delta = (s16)(heading - facing);
    f32 length = *distance.get();
    if (150.0f > length) {
        s32 magnitude = delta < 0 ? -(s32)delta : (s32)delta;
        if (magnitude < 13000)
            eventInfo.onCondition(dEvtCnd_CANTALK_e);
    }
}
/* 023407A0 */
void daObjFigure_c::eventMove() {
    s16 event = (s16)mFigureCheckEvtIdx;
    if (dComIfGp_evmng_endCheck(event)) {
        dComIfGp_event_reset();
        return;
    }
    u8 attention = (u8)mEventCut.mbAttention;
    if (mEventCut.cutProc()) {
        if (!(u8)mEventCut.mbAttention)
            mEventCut.mbAttention = attention;
    } else
        privateCut();
}
/* 02340664 */
void daObjFigure_c::privateCut() {
    s32 staff = dComIfGp_evmng_getMyStaffId("Figure", nullptr, 0);
    if (staff == -1)
        return;
    s8 action = (s8)dComIfGp_evmng_getMyActIdx(staff, cut_name_tbl.mNames, 3, TRUE, 0);
    m741 = action;
    dEvent_manager_c* manager = dComIfGp_getPEvtManager();
    if (action == -1) {
        Figure_cutEndHD(manager, staff);
        return;
    }
    if (Figure_getIsAdvanceHD(manager, staff)) {
        action = (s8)m741;
        if (action == 0)
            eventMesSetInit(staff);
        else if (action == 1)
            eventOnPlrInit();
        else if (action == 2)
            eventOffPlrInit();
    }
    action = (s8)m741;
    if (action == 0 && !eventMesSet())
        return;
    dComIfGp_evmng_cutEnd(staff);
}
/* 0233FF68 */
void daObjFigure_c::eventMesSetInit(s32 staff) {
    auto* message = reinterpret_cast<be<u32>*>(dComIfGp_evmng_getMySubstanceP(staff, "MsgNo", 3));
    if (message) {
        m72C.v = 0;
        u32 value = *message;
        if (value != 1) {
            if (value == 0)
                value = getMsg();
            setMessage(value);
            be<u32>* list = m72C;
            if (list)
                setMessage((u32)*list);
        }
    } else {
        be<u32>* list = Figure_nextMessageHD(m72C);
        m72C = list;
        setMessage((u32)*list);
    }
    m73A = 10;
    m724 = 50.0f;
    m738 = 0;
    m73F = 0;
    m728 = 118.0f;
}
/* 0234025C */
bool daObjFigure_c::eventMesSet() {
    s16 timer = (s16)m73A;
    if (timer)
        m73A = (u16)(timer - 1);
    else {
        bool input = Figure_stickXHD(0) != 0.0f;
        if (!input)
            input = Figure_stickYHD(0) != 0.0f;
        if (!input)
            input = Figure_substickYHD(0) != 0.0f;
        if (input) {
            m738 = 0;
            f32 stick = Figure_stickXHD(0);
            s16 delta = (s16)truncWord((f32)(stick * 512.0f));
            m734 = (u16)((s16)m734 + delta);
            stick = Figure_stickYHD(0);
            delta = (s16)truncWord((f32)(stick * 5.0f));
            f32 height = (f32)((f32)m728 + (f32)delta);
            if (height < 80.0f)
                height = 80.0f;
            else if (height > 150.0f)
                height = 150.0f;
            m728 = height;
            stick = Figure_substickYHD(0);
            f32 twice = (f32)(stick + stick);
            f32 zoom = (f32)((f32)m724 - twice);
            if (zoom < 20.0f)
                zoom = 20.0f;
            else if (zoom > 60.0f)
                zoom = 60.0f;
            m724 = zoom;
        } else {
            s16 wait = (s16)m738;
            if (wait)
                m738 = (u16)(wait - 1);
        }
    }
    u8 state = (u8)m73F;
    if (state == 0) {
        dComIfGp_setDoStatusForce(0x21);
        dComIfGp_setAStatusForce(0x27);
        if ((s16)m73A == 0) {
            dComIfGp_setDoStatusForce(0x21);
            dComIfGp_setAStatusForce(0x27);
            if (Figure_triggerBHD(0)) {
                s16 angle = (s16)current.angle.y;
                m732 = 0;
                m734 = 0;
                m736 = angle;
                return true;
            }
            if (Figure_triggerAHD(0))
                m73F = 1;
        }
    } else if (state == 1) {
        talk(0);
        if ((u32)m694 != 0xFFFFFFFF)
            m73F = 2;
    } else if (state == 2) {
        if (talk(0) == 0x12)
            m73F = 0;
    }
    f32 height = (f32)m728;
    s16 angle = (s16)current.angle.y;
    f32 cameraHeight = (f32)(height + 30.0f);
    m70C.x = 0.0f;
    s16 rotation = (s16)m734;
    m70C.y = cameraHeight;
    m70C.z = 0.0f;
    m718.x = 0.0f;
    m718.y = cameraHeight;
    m718.z = 200.0f;
    mDoMtx_stack_c::YrotS((s16)(angle + rotation));
    PSMTXMultVec(&l_mtxStack, &m718, &m718);
    PSVECAdd(&m70C, &current.pos, &m70C);
    PSVECAdd(&m718, &current.pos, &m718);
    f32 tx = (f32)m70C.x, ex = (f32)m718.x;
    f32 tz = (f32)m70C.z, ez = (f32)m718.z;
    f32 ty = (f32)m70C.y, ey = (f32)m718.y;
    gabi::Local<cXyz> target, eye;
    target->x = tx;
    target->y = ty;
    target->z = tz;
    eye->x = ex;
    eye->y = ey;
    eye->z = ez;
    FigureCameraHD* camera = Figure_getCameraHD();
    Figure_cameraSetHD(camera, target.get(), eye.get(), (f32)m724, 0);
    return false;
}
/* 02340040 */
void daObjFigure_c::eventOnPlrInit() {
    auto* player = figureLinkPlayerHD();
    player->offNoResetFlg0(daPy_py_c::daPyFlg0_NO_DRAW);
    Figure_cameraResetHD(Figure_getCameraHD());
    Figure_cameraStartHD(Figure_getCameraHD());
    dComIfGs_offTmpBit(0x408);
}
/* 02340094 */
void daObjFigure_c::eventOffPlrInit() {
    auto* player = figureLinkPlayerHD();
    player->onNoResetFlg0(daPy_py_c::daPyFlg0_NO_DRAW);
    Figure_cameraStopHD(Figure_getCameraHD());
    dComIfGs_onTmpBit(0x408);
}
/* 02340128 */
u16 daObjFigure_c::talk(s32 param) {
    u32 id = m694;
    FigureMessageManagerHD* message = l_messageManager.mpManager;
    u16 status = 0xFF;
    if (id == 0xFFFFFFFF) {
        u32 text;
        if (param == 1) {
            text = getMsg();
            m69C = text;
        } else
            text = (u32)m69C;
        u32 newId = Figure_messageSetHD(message, text, &eyePos);
        m694 = newId;
        if (newId != 0xFFFFFFFF)
            mHasMessageHD = 0;
    } else if ((u8)mHasMessageHD) {
        status = (u16)Figure_messageGetStatusHD(message);
        if (status == 0xE) {
            u16 next = next_msgStatus(&m69C);
            Figure_messageSetStatusHD(message, next);
            if (Figure_messageGetStatusHD(message) == 0xF)
                Figure_messageSetHD(message, (u32)m69C, nullptr);
        } else if (status == 0x12) {
            Figure_messageSetStatusHD(message, 0x13);
            m694 = 0xFFFFFFFF;
        }
    } else
        mHasMessageHD = 1;
    return status;
}
/* 023400E0 */
u16 daObjFigure_c::next_msgStatus(be<u32>* message) {
    be<u32>* list = m72C;
    if (!list)
        return 0x10;
    list = Figure_nextMessageHD(list);
    m72C = list;
    u32 value = (u32)*list;
    if (!value) {
        m72C.v = 0;
        return 0x10;
    }
    *message = value;
    return 0xF;
}
/* 0233FF54 */
u32 daObjFigure_c::getMsg() {
    u8 no = (u8)mFigureNo;
    m72C.v = 0;
    return 0x37DD + no;
}
/* 0233FF44 */
void daObjFigure_c::setMessage(u32 message) {
    m69C = message;
    m694 = 0xFFFFFFFF;
}
/* 0233F634 */
u8 daObjFigure_c::getPrmFigureNo() {
    u32 a = gabi::ea(this);
    return (u8)PrmAbstract(a, PRM_FIGURE_NO_W, PRM_FIGURE_NO_S);
}
/* 0233FA44 */
void daObjFigure_c::setMtx() {
    f32 x = scale.x, y = scale.y;
    J3DModel* model = mpPedestalModel;
    f32 z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    PSMTXTrans(&l_mtxStack, x, y, z);
    mDoMtx_YrotM(&l_mtxStack, (s16)current.angle.y);
    copyMatrix(&l_mtxStack, &mpPedestalModel->mBaseTransformMtx);
    model = mpModel;
    model->mBaseScale.z = 1.0f;
    model->mBaseScale.y = 1.0f;
    model->mBaseScale.x = 1.0f;
    y = current.pos.y;
    x = current.pos.x;
    z = current.pos.z;
    PSMTXTrans(&l_mtxStack, x, (f32)(y + 100.0f), z);
    mDoMtx_YrotM(&l_mtxStack, (s16)m736);
    copyMatrix(&l_mtxStack, &mpModel->mBaseTransformMtx);
    mDoExt_McaMorf* morf = mpMorf;
    if (morf) {
        model = morf->mpModel;
        model->mBaseScale.x = 1.0f;
        model->mBaseScale.y = 1.0f;
        model->mBaseScale.z = 1.0f;
        morf = mpMorf;
        model = morf->mpModel;
        copyMatrix(&l_mtxStack, &model->mBaseTransformMtx);
    }
}
/* 0233F660 */
u8 daObjFigure_c::isFigureGet(u32 no) {
    u32 byte = no >> 3;
    if ((s32)byte >= 17)
        return 0;
    u16 event = (u16)l_figure_comp.mEvents[byte];
    return (u8)(dComIfGs_getEventRegHD(event) & (1u << (no & 7)));
}
/* 0233F2F0 */
s32 daObjFigure_c::getFigureBmd(u32 no) {
    s32 index = (s32)l_figure_dat_tbl.mData[no].mBmdId;
    if (no == 0x10 || no == 0x12) {
        if (dComIfGs_isEventBit(0x2D01))
            index = no == 0x10 ? 3 : 6;
    }
    return index;
}
/* 0233FEC4 */
s32 daSampleCreate(u32 a) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    return actor->_create();
}
/* 0233FF40 */
bool daSampleDelete(u32 a) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    return actor->_delete();
}
/* 023409BC */
bool daSampleExecute(u32 a) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    return actor->_execute();
}
/* 0234135C */
bool daSampleDraw(u32 a) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    return actor->_draw();
}
/* 023414CC */
bool daSampleIsDelete(u32 a) { return true; }
// HD: compiler-generated and template code of this translation unit.
/* 02341424 */
void __sinit_d_a_obj_figure_cpp() {
    l_headerZeros.mWords[2] = 0;
    l_headerZeros.mWords[0] = 0;
    l_headerZeros.mWords[3] = 0;
    l_headerZeros.mWords[1] = 0;
    Figure_registerDtorHD(&l_headerDtor);
    l_headerStatics.mLower = -3.1415927410125732f;
    l_headerStatics.mUpper = 3.1415927410125732f;
    Figure_headerConstruct1HD(&l_headerStatics.mFirst);
    Figure_registerDtorHD(&l_firstHeaderDtor);
    Figure_headerConstruct2HD(&l_headerStatics.mSecond);
    Figure_registerDtorHD(&l_secondHeaderDtor);
}
/* 023414B8 */
void SafeString_dtor(u32 a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(gabi::at<void>(a));
}
/* 023414D4 */
void offCupOnAupPacket_dtor(u32 a, s32 flags) {
    if (a) {
        Figure_packetDestructHD(gabi::at<FigureDrawPacketHD>(a), 0);
        if (flags & 1)
            operator_delete(gabi::at<void>(a));
    }
}
/* 02341528 */
void onCupOffAupPacket_dtor(u32 a, s32 flags) {
    if (a) {
        Figure_packetDestructHD(gabi::at<FigureDrawPacketHD>(a), 0);
        if (flags & 1)
            operator_delete(gabi::at<void>(a));
    }
}
/* 0234157C */
void daObjFigure_dtor(u32 a, s32 flags) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    if (a) {
        dCcD_Cyl_dt(&actor->mCyl2, 2);
        dCcD_Cyl_dt(&actor->mCyl1, 2);
        dCcD_Stts_dt(&actor->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operator_delete(gabi::at<void>(a));
    }
}
/* 023415F4 */
void SafeString_assureTerminationImpl(u32 a) {}

/* 023415F8 */
u32 PrmAbstract(u32 a, s32 width, s32 shift) {
    daObjFigure_c* actor = gabi::at<daObjFigure_c>(a);
    u32 w = (u32)width & 63, s = (u32)shift & 63;
    return (s < 32 ? (u32)actor->mParameters >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_headerStatics 104698F4
data l_headerZeros 10469910
data l_headerDtor 101C9360
data l_firstHeaderDtor 101C936C
data l_secondHeaderDtor 101C9378
ext 028ED6F8 void Figure_headerConstruct1HD(be<u8>* object)
ext 028EAB2C void Figure_headerConstruct2HD(be<u8>* object)
data l_lightDefaults 1016E414
data l_figure_dat_tbl 10028330
data l_arcname_tbl 101C927C
data l_figure_comp 101C92E0
data l_mtxStack 1048D0CC
data l_identityMtx 101F48F0
data l_cyl_src 101C9304
data l_method 101C9348
data cut_name_tbl 101C9354
data moveProc 101C9254
str 1002897C "Figure" in 0233F37C
str 10028984 "Figure" in 0233F9C4
str 100289A4 "Figure" in 0233FC60
str 100289BC "Figure" in 0233FEC8
str 100289F4 "Figure" in 02340664
str 100289AC "FIGURE_CHECK"
ext 025BD64C s32 Figure_getFigureRoomHD(u32 figureNo)
ext 025E80D0 mDoExt_brkAnm* Figure_newBrkHD(mDoExt_brkAnm* self)
ext 02515FB8 void Figure_GObjInf_ctorHD(dCcD_GObjInf* object)
ext 02018590 void Figure_cylShape_ctorHD(cM3dGCyl* cylinder)
ext 0259F740 void Figure_eventCut_ctorHD(dNpc_EventCut_c* cut)
ext 0255FFF4 void Figure_tevInitHD(dKy_tevstr_c* tev, s32 room, u32 type)
ext 02525FE4 s32 Figure_phaseHandlerHD(request_of_phase_process_class* phase, FigurePhaseTableHD* table, daObjFigure_c* actor)
ext 02543280 void Figure_cutEndHD(dEvent_manager_c* manager, s32 staff)
ext 025447C8 s32 Figure_getIsAdvanceHD(dEvent_manager_c* manager, s32 staff)
ext 027F13DC void Figure_packetDestructHD(FigureDrawPacketHD* packet, s32 flags)
data l_messageManager 101F4B5C
str 100289C8 "MsgNo"
ext 025F795C u32 Figure_messageGetStatusHD(FigureMessageManagerHD* manager)
ext 025F74D0 void Figure_messageSetStatusHD(FigureMessageManagerHD* manager, u32 status)
ext 025F7DB0 u32 Figure_messageSetHD(FigureMessageManagerHD* manager, u32 message, cXyz* pos)
ext 024F8044 FigureCameraHD* Figure_getCameraHD()
ext 02515048 void Figure_cameraResetHD(FigureCameraHD* camera)
ext 02514F38 void Figure_cameraStartHD(FigureCameraHD* camera)
ext 02514F2C void Figure_cameraStopHD(FigureCameraHD* camera)
ext 02514F88 void Figure_cameraSetHD(FigureCameraHD* camera, cXyz* target, cXyz* eye, f32 zoom, s32 flags)
ext 0200796C f32 Figure_stickXHD(s32 port)
ext 02007990 f32 Figure_stickYHD(s32 port)
ext 02007B20 f32 Figure_substickYHD(s32 port)
ext 02007898 s32 Figure_triggerAHD(s32 port)
ext 020078BC s32 Figure_triggerBHD(s32 port)
data l_renderState 104B462C
data l_offCupOnAupPacket1 10469920
data l_offCupOnAupPacket1Guard 104698FC
data l_offCupOnAupPacket1Dtor 101C92B0
data l_offCupOnAupPacket2 104699B8
data l_offCupOnAupPacket2Guard 10469900
data l_offCupOnAupPacket2Dtor 101C92BC
data l_onCupOffAupPacket1 10469A50
data l_onCupOffAupPacket1Guard 10469904
data l_onCupOffAupPacket1Dtor 101C92C8
data l_onCupOffAupPacket2 10469AE8
data l_onCupOffAupPacket2Guard 10469908
data l_onCupOffAupPacket2Dtor 101C92D4
str 1002829C "face"
str 100282A4 "ear_2_"
str 100282F8 "d_a_obj_figure.cpp"
str 100282E4 "zoff_blend_cnt <= 4"
str 1002830C "zoff_none_cnt <= 4"
str 10028320 "zon_cnt <= 4"
ext 027F1278 void Figure_packetConstructHD(FigureDrawPacketHD* packet)
ext 028F026C void Figure_registerDtorHD(FigureDtorRecordHD* record)
*/
// clang-format on
