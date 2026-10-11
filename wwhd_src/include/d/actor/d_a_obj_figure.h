/* Nintendo Gallery figurines, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"

// HD: two expanded BTP controllers and the larger lighting block move the tail by 0x2F4.
class daObjFigure_c : public fopAc_ac_c {
  public:
    // HD: public ABI widths are retained where HD uses a wider figure or message argument.
    s32 getFigureBmd(u32 no);
    bool createHeap();
    u8 getPrmFigureNo();
    u8 isFigureGet(u32 no);
    void setMtx();
    s32 createInit();
    s32 _create();
    bool _delete();
    void setMessage(u32 message);
    u32 getMsg();
    void eventMesSetInit(s32 staff);
    void eventOnPlrInit();
    void eventOffPlrInit();
    u16 next_msgStatus(be<u32>* message);
    u16 talk(s32 param);
    bool eventMesSet();
    void privateCut();
    void eventMove();
    bool _execute();
    bool _draw();
    void executeNormal();
    enum : u32 { __vt = 0x100282D4, CYL_AAB_VTABLE_HD = 0x100282C4 };
    enum Prm_e { PRM_FIGURE_NO_W = 8, PRM_FIGURE_NO_S = 0 };
    request_of_phase_process_class mPhsLoad;    // 0x3AC
    request_of_phase_process_class mPhsFigure;  // 0x3B4
    request_of_phase_process_class mPhsRoomArc; // 0x3BC
    gptr<J3DModel> mpModel;                     // 0x3C4
    gptr<J3DModel> mpPedestalModel;             // 0x3C8
    gptr<J3DAnmTexPattern> mpPedestalBtp;       // 0x3CC
    mDoExt_btpAnm mBtpAnm1;                     // 0x3D0
    u8 m2C8[4];
    mDoExt_btpAnm mBtpAnm2;       // 0x448
    gptr<mDoExt_McaMorf> mpMorf;  // 0x4BC
    gptr<mDoExt_brkAnm> mpBrkAnm; // 0x4C0
    dKy_tevstr_c mTevStrBG2;      // 0x4C4
    u8 m398[0x30];
    Mtx34 mMtx;           // 0x6BC
    dCcD_Stts mStts;      // 0x6EC
    dCcD_Cyl mCyl1;       // 0x728
    dCcD_Cyl mCyl2;       // 0x858
    be<u32> m694;         // 0x988
    be<u8> mHasMessageHD; // HD: manager handle replaces mpCurrMsg.
    u8 mMessagePaddingHD[3];
    be<u32> m69C;              // 0x990
    dNpc_EventCut_c mEventCut; // 0x994
    cXyz m70C;                 // 0xA00
    cXyz m718;                 // 0xA0C
    be<f32> m724;
    be<f32> m728;
    gptr<be<u32>> m72C;
    be<s16> mFigureCheckEvtIdx;
    be<s16> m732;
    be<s16> m734;
    be<s16> m736;
    be<s16> m738;
    be<s16> m73A;
    be<u8> m73C;
    be<u8> mFigureNo;
    be<u8> mbDisplay;
    be<u8> m73F;
    be<u8> m740;
    be<s8> m741;
    be<u8> mLoadFlags;
    u8 m743;
};
WWHD_OFFSET(daObjFigure_c, mPhsLoad, 0x3AC);
WWHD_OFFSET(daObjFigure_c, mPhsFigure, 0x3B4);
WWHD_OFFSET(daObjFigure_c, mPhsRoomArc, 0x3BC);
WWHD_OFFSET(daObjFigure_c, mpModel, 0x3C4);
WWHD_OFFSET(daObjFigure_c, mpPedestalModel, 0x3C8);
WWHD_OFFSET(daObjFigure_c, mpPedestalBtp, 0x3CC);
WWHD_OFFSET(daObjFigure_c, mBtpAnm1, 0x3D0);
WWHD_OFFSET(daObjFigure_c, mBtpAnm2, 0x448);
WWHD_OFFSET(daObjFigure_c, mpMorf, 0x4BC);
WWHD_OFFSET(daObjFigure_c, mpBrkAnm, 0x4C0);
WWHD_OFFSET(daObjFigure_c, mTevStrBG2, 0x4C4);
WWHD_OFFSET(daObjFigure_c, mMtx, 0x6BC);
WWHD_OFFSET(daObjFigure_c, mStts, 0x6EC);
WWHD_OFFSET(daObjFigure_c, mCyl1, 0x728);
WWHD_OFFSET(daObjFigure_c, mCyl2, 0x858);
WWHD_OFFSET(daObjFigure_c, m694, 0x988);
WWHD_OFFSET(daObjFigure_c, mHasMessageHD, 0x98C);
WWHD_OFFSET(daObjFigure_c, m69C, 0x990);
WWHD_OFFSET(daObjFigure_c, mEventCut, 0x994);
WWHD_OFFSET(daObjFigure_c, m70C, 0xA00);
WWHD_OFFSET(daObjFigure_c, m718, 0xA0C);
WWHD_OFFSET(daObjFigure_c, m724, 0xA18);
WWHD_OFFSET(daObjFigure_c, m728, 0xA1C);
WWHD_OFFSET(daObjFigure_c, m72C, 0xA20);
WWHD_OFFSET(daObjFigure_c, mFigureCheckEvtIdx, 0xA24);
WWHD_OFFSET(daObjFigure_c, m732, 0xA26);
WWHD_OFFSET(daObjFigure_c, m734, 0xA28);
WWHD_OFFSET(daObjFigure_c, m736, 0xA2A);
WWHD_OFFSET(daObjFigure_c, m738, 0xA2C);
WWHD_OFFSET(daObjFigure_c, m73A, 0xA2E);
WWHD_OFFSET(daObjFigure_c, m73C, 0xA30);
WWHD_OFFSET(daObjFigure_c, mFigureNo, 0xA31);
WWHD_OFFSET(daObjFigure_c, mbDisplay, 0xA32);
WWHD_OFFSET(daObjFigure_c, m73F, 0xA33);
WWHD_OFFSET(daObjFigure_c, m740, 0xA34);
WWHD_OFFSET(daObjFigure_c, m741, 0xA35);
WWHD_OFFSET(daObjFigure_c, mLoadFlags, 0xA36);
WWHD_SIZE(daObjFigure_c, 0xA38);

// HD: GHS copies the same packed light parameters into the light object and two cached blocks.
struct FigureLightParametersHD {
    cXyz mPosition;
    cXyz mDirection;
    GXColor mColor;
    GXColorS10 mColorS10;
    be<f32> mCosAtten[3];
    be<f32> mDistAtten[3];
    be<f32> mCutoff;
    be<f32> mReferenceBrightness;
};
WWHD_SIZE(FigureLightParametersHD, 0x44);
struct FigureShapeAttrHD {
    cXyz mMin;
    cXyz mMax;
    be<u32> mAabVtable;
    be<u32> mShapeVtable;
};
WWHD_SIZE(FigureShapeAttrHD, 0x20);

struct FigureData {
    be<s32> mBmdId;
    be<u32> mModelAttr;
    be<s32> mRoomId;
};
WWHD_SIZE(FigureData, 0x0C);
struct FigureArchiveTableHD {
    gptr<char> mNames[13];
};
struct FigureCompletionTableHD {
    be<u16> mEvents[17];
};

struct FigureDataTableHD {
    FigureData mData[134];
};

// HD: material records use self-relative resource names and compact virtual dispatch.
struct FigureMaterialRecordHD {
    be<u32> mTag;
    be<u32> mNameOffset;
};
struct FigurePEVtableHD {
    u8 mPrefix[0x2C];
    gfn mGetBlend;
    u8 mMiddle[0x14];
    gfn mGetZMode;
};
WWHD_OFFSET(FigurePEVtableHD, mGetBlend, 0x2C);
WWHD_OFFSET(FigurePEVtableHD, mGetZMode, 0x44);
struct FigurePEBlockHD {
    gptr<FigurePEVtableHD> mVtable;
};
struct FigureMaterialHD {
    gptr<FigureMaterialRecordHD> mpData;
    gptr<FigureMaterialHD> mpNext;
    gptr<J3DShape> mpShape;
    u8 mMiddle[0x14];
    gptr<FigurePEBlockHD> mpPEBlock;
};
WWHD_OFFSET(FigureMaterialHD, mpPEBlock, 0x20);
struct FigureJointHD {
    u8 mJointPrefix[0x10];
    gptr<FigureMaterialHD> mpMesh;
    u8 mJointTail[8];
};
WWHD_SIZE(FigureJointHD, 0x1C);
struct FigureModelDataHD : J3DModelData {
    u8 mAnimationPrefix[0x38 - sizeof(J3DModelData)];
    be<u32> mpBtp;
    u8 mAnimationMiddle[0x0C];
    be<u32> mpBrk;
};
WWHD_OFFSET(FigureModelDataHD, mpBtp, 0x38);
WWHD_OFFSET(FigureModelDataHD, mpBrk, 0x48);
struct FigureDrawPacketHD {
    enum : u32 { OFF_CUP_ON_AUP_VTABLE = 0x10058D10, ON_CUP_OFF_AUP_VTABLE = 0x10058D40 };
    u8 mPacketPrefix[0x0C];
    be<u32> mVtable;
    u8 mPacketTail[0x98 - 0x10];
};
WWHD_SIZE(FigureDrawPacketHD, 0x98);
struct FigureDtorRecordHD {
    be<u32> mWords[3];
};
struct FigureJ3DSysHD {
    gptr<J3DModel> mpModel;
    u8 mMiddle[4];
    gptr<J3DDrawBuffer> mpOpaBuffer;
    gptr<J3DDrawBuffer> mpXluBuffer;
};
WWHD_OFFSET(FigureJ3DSysHD, mpOpaBuffer, 8);
struct FigureDrawListHD {
    u8 mPrefix[0x24];
    gptr<J3DDrawBuffer> mpListP0;
    u8 mMiddle[4];
    gptr<J3DDrawBuffer> mpListP1;
    u8 mBeforeNormal[0x18];
    gptr<J3DDrawBuffer> mpOpa;
    gptr<J3DDrawBuffer> mpXlu;
};
WWHD_OFFSET(FigureDrawListHD, mpListP0, 0x24);
WWHD_OFFSET(FigureDrawListHD, mpListP1, 0x2C);
WWHD_OFFSET(FigureDrawListHD, mpOpa, 0x48);
WWHD_OFFSET(FigureDrawListHD, mpXlu, 0x4C);
struct FigureDrawFrameHD {
    gptr<mDoExt_McaMorf> mpMorf;
    SafeString mStrings[6];
    gptr<J3DShape> mShapes[12];
    u8 mUnused[0x4C];
};
WWHD_OFFSET(FigureDrawFrameHD, mStrings, 4);
WWHD_OFFSET(FigureDrawFrameHD, mShapes, 0x34);
WWHD_SIZE(FigureDrawFrameHD, 0xB0);
struct FigureSafeStringVtableHD {
    u8 mPrefix[0x14];
    gfn mAssureTermination;
};
void Figure_packetConstructHD(FigureDrawPacketHD* packet);
void Figure_registerDtorHD(FigureDtorRecordHD* record);

// HD: messages use a cached manager and full-width status, rather than GC msg_class storage.
struct FigureMessageManagerHD;
struct FigureMessagePointerHD {
    gptr<FigureMessageManagerHD> mpManager;
};
u32 Figure_messageGetStatusHD(FigureMessageManagerHD* manager);
void Figure_messageSetStatusHD(FigureMessageManagerHD* manager, u32 status);
u32 Figure_messageSetHD(FigureMessageManagerHD* manager, u32 message, cXyz* pos);
struct FigureCameraHD;
FigureCameraHD* Figure_getCameraHD();
void Figure_cameraResetHD(FigureCameraHD* camera);
void Figure_cameraStartHD(FigureCameraHD* camera);
void Figure_cameraStopHD(FigureCameraHD* camera);
void Figure_cameraSetHD(FigureCameraHD* camera, cXyz* target, cXyz* eye, f32 zoom, s32 flags);
f32 Figure_stickXHD(s32 port);
f32 Figure_stickYHD(s32 port);
f32 Figure_substickYHD(s32 port);
s32 Figure_triggerAHD(s32 port);
s32 Figure_triggerBHD(s32 port);
inline be<u32>* Figure_nextMessageHD(be<u32>* message) {
    // HD: a null or overflowing message pointer still increments modulo 2^32.
    return gabi::at<be<u32>>(gabi::ea(message) + 4u);
}

struct FigurePhaseTableHD {
    be<u32> mHandlers[3];
};
struct FigureCutNameTableHD {
    gptr<char> mNames[3];
};
struct FigureMoveTableHD {
    gabi::ptmf<void (daObjFigure_c::*)()> mProc;
};
s32 Figure_getFigureRoomHD(u32 figureNo);
mDoExt_brkAnm* Figure_newBrkHD(mDoExt_brkAnm* self);
void Figure_GObjInf_ctorHD(dCcD_GObjInf* object);
void Figure_cylShape_ctorHD(cM3dGCyl* cylinder);
void Figure_eventCut_ctorHD(dNpc_EventCut_c* cut);
void Figure_tevInitHD(dKy_tevstr_c* tev, s32 room, u32 type);
s32 Figure_phaseHandlerHD(request_of_phase_process_class* phase, FigurePhaseTableHD* table,
                          daObjFigure_c* actor);
void Figure_cutEndHD(dEvent_manager_c* manager, s32 staff);
s32 Figure_getIsAdvanceHD(dEvent_manager_c* manager, s32 staff);
void Figure_packetDestructHD(FigureDrawPacketHD* packet, s32 flags);
inline void Figure_secondCyl_ctorHD(dCcD_Cyl* cylinder) {
    // HD: the second cylinder stores its final cylinder vtable before its shape vtable.
    Figure_GObjInf_ctorHD(cylinder);
    auto* shape = reinterpret_cast<FigureShapeAttrHD*>(&cylinder->mShapeAttr);
    shape->mShapeVtable = 0x100015A8;
    shape->mAabVtable = daObjFigure_c::CYL_AAB_VTABLE_HD;
    Figure_cylShape_ctorHD(&cylinder->mCyl);
    cylinder->__vtbl_hitinf = 0x1004B108;
    cylinder->mCyl.__vtbl = 0x1004B150;
    shape->mShapeVtable = 0x1004B160;
}

void Figure_headerConstruct1HD(be<u8>* object);
void Figure_headerConstruct2HD(be<u8>* object);

struct FigureHeaderStaticsHD {
    be<f32> mLower, mUpper;
    u8 mReserved[16];
    be<u8> mFirst, mSecond;
};
struct FigureHeaderZerosHD {
    be<u32> mWords[4];
};
WWHD_OFFSET(FigureHeaderStaticsHD, mFirst, 0x18);
WWHD_OFFSET(FigureHeaderStaticsHD, mSecond, 0x19);
WWHD_SIZE(FigureHeaderStaticsHD, 0x1A);
WWHD_SIZE(FigureHeaderZerosHD, 0x10);
