/* daNpc_Aj1_c (Sturgeon, Outset), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_aj1 and named after their use. The class
 * derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h). Members are read up to 0x931 (the
 * profile's size word 0x8C8 is, as for ls1 (0x950 for 0xA28), not the HD object size). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
struct Aj1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(Aj1ActionFunc, 8);

struct daNpc_Aj1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    u32 _XyCheckCB(int);
    bool init_AJ1_0();
    bool init_AJ1_1();
    bool init_AJ1_2();
    bool createInit();
    void play_animation();
    void ctrl_WAITanm();
    void ctrl_TIREanm();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    u32 init_texPttrnAnm(s8, u32);
    void play_texPttrnAnm();
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void setAnm();
    void chngAnmAtr(u8);
    void ctrlAnmAtr();
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_AJ1_0();
    u32 getMsg_AJ1_1();
    u32 getMsg_AJ1_2();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    void cut_init_AJ1_TLK();
    u32 cut_move_AJ1_TLK();
    void cut_init_INI_ANGRY();
    u32 cut_move_VIVRATE();
    u32 cut_move_JMP();
    u32 cut_move_SPPRISE();
    u32 cut_move_LOK();
    u32 cut_move_DAN();
    void cut_init_INVIT();
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    void set_pa_pun();
    void set_pa_aka();
    void set_pa_don();
    void set_pa_smk();
    void flw_pa_pun();
    void flw_pa_aka();
    void del_pa_aka();
    void del_pa(gptr<JPABaseEmitter>*);
    void setSmoke();
    BOOL set_action(Aj1ActionFunc*, void*);
    void setStt(s8);
    u8 chk_areaIN(f32, s16, cXyz*);
    BOOL FARwai();
    BOOL call_1();
    BOOL wait_1();
    BOOL talk_1();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL itemCreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hnd_L_jnt_num;
    /* 0x7E7 */ be<s8> m_foot_L_jnt_num;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel; /* "Aj" res 9, carried on handL */
    /* 0x7EC */ u8 mBtpAnm[0x74];           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ Aj1ActionFunc mCurrProcFunc;
    /* 0x86C */ cXyz mHomePos;
    /* 0x878 */ csXyz mHomeAngle;
    /* 0x87E */ csXyz mAngle;
    /* 0x884 */ cXyz mEyePos; /* head joint * (24, -16, 0) */
    /* 0x890 */ cXyz mLookPos;
    /* 0x89C */ cXyz mHeadPos;
    /* 0x8A8 */ be<s16> mActorAngleY;
    /* 0x8AA */ be<s16> mJointHeadY;
    /* 0x8AC */ be<s16> mJointBackboneY;
    /* 0x8AE */ u8 _8AE[2];
    /* 0x8B0 */ be<f32> mPrevMorfFrame;
    /* 0x8B4 */ be<s16> mEventIDTbl[1];
    /* 0x8B6 */ be<s16> mEventIndex;
    /* 0x8B8 */ be<s16> mWaitTimer;
    /* 0x8BA */ be<s16> mPunTimer;
    /* 0x8BC */ be<s16> mCutTimer;
    /* 0x8BE */ be<s16> mTalkEndTimer;
    /* 0x8C0 */ be<s16> mFarTimer;
    /* 0x8C2 */ u8 _8C2[2];
    /* 0x8C4 */ be<s16> mLookVel;
    /* 0x8C6 */ be<s16> mLookAngleY;
    /* 0x8C8 */ be<s8> mbMorfAnimStopped;
    /* 0x8C9 */ be<u8> mLoopCnt;
    /* 0x8CA */ be<u8> mItemNo;
    /* 0x8CB */ be<u8> mSwitchNo;
    /* 0x8CC */ be<u8> mTireTimer;
    /* 0x8CD */ u8 _8CD;
    /* 0x8CE */ be<u8> mbHidden;
    /* 0x8CF */ be<u8> mbNoShapeAngle;
    /* 0x8D0 */ be<u8> mbNoDraw;
    /* 0x8D1 */ be<u8> mbHomeSet;
    /* 0x8D2 */ u8 _8D2[2];
    /* 0x8D4 */ be<s32> mbSetEyePos;
    /* 0x8D8 */ be<u8> mbAttention;
    /* 0x8D9 */ be<u8> mbTalk;
    /* 0x8DA */ be<u8> mbHeadOnly;
    /* 0x8DB */ be<u8> mbDemo;
    /* 0x8DC */ u8 mSmokeCB[0x20]; /* dPa_smokeEcallBack (constructor 025A5B18(this, 1)) */
    /* 0x8FC */ gptr<JPABaseEmitter> mpSmokeEmitter;
    /* 0x900 */ gptr<JPABaseEmitter> mpPunEmitter;
    /* 0x904 */ gptr<JPABaseEmitter> mpAkaEmitter;
    /* 0x908 */ gptr<JPABaseEmitter> mpDonEmitter;
    /* 0x90C */ cXyz mSmokePos;
    /* 0x918 */ cXyz mDonPos;
    /* 0x924 */ be<u8> mbAkaStop;
    /* 0x925 */ be<s8> mActionIndex;
    /* 0x926 */ be<u8> mAnmAtr;
    /* 0x927 */ be<u8> mMesgAnimeTag;
    /* 0x928 */ be<s8> mBtpNum;
    /* 0x929 */ be<s8> mBckNum;
    /* 0x92A */ be<s8> mEvtState;
    /* 0x92B */ be<s8> mStt;
    /* 0x92C */ be<s8> mPrevStt;
    /* 0x92D */ be<s8> mLookMode;
    /* 0x92E */ be<s8> mSubtype;
    /* 0x92F */ be<s8> mType;
    /* 0x930 */ be<s8> mActState;
    /* 0x931 */ be<s8> mAtrSet;
    /* 0x932 */ u8 _932[2];
};
WWHD_OFFSET(daNpc_Aj1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Aj1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Aj1_c, mEyePos, 0x884);
WWHD_OFFSET(daNpc_Aj1_c, mPrevMorfFrame, 0x8B0);
WWHD_OFFSET(daNpc_Aj1_c, mbSetEyePos, 0x8D4);
WWHD_OFFSET(daNpc_Aj1_c, mpSmokeEmitter, 0x8FC);
WWHD_OFFSET(daNpc_Aj1_c, mbAkaStop, 0x924);
WWHD_OFFSET(daNpc_Aj1_c, mAtrSet, 0x931);

WWHD_SIZE(daNpc_Aj1_c, 0x934);
// HD: measured HIO layout, with vtable first.
struct daNpc_Aj1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackBoneX;
        /* 0x0A */ be<s16> mMaxBackBoneY;
        /* 0x0C */ be<s16> mMinBackBoneX;
        /* 0x0E */ be<s16> mMinBackBoneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mLookVelMax;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebugDraw;
        /* 0x19 */ u8 _19[3];
        /* 0x1C */ be<f32> mFarDist;
        /* 0x20 */ be<s16> mFarAngle;
        /* 0x22 */ u8 _22[2];
        /* 0x24 */ be<f32> mCallDist;
        /* 0x28 */ be<s16> mCallAngle;
        /* 0x2A */ be<s16> mSpprTime;
        /* 0x2C */ be<s16> mLokTime;
        /* 0x2E */ u8 _2E[2];
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mEntryID;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Aj1_HIO_c, 0x3C);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Aj1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Aj1Anm {
    u8 _0[4];
    gptr<Aj1AnmVtable> mpVtable;
};
struct Aj1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct Aj1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Aj1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Aj1Anm, mpVtable, 4);
WWHD_OFFSET(Aj1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Aj1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Aj1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Aj1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Aj1_frameMax(J3DAnmTexPattern* p) {
    auto* a = Aj1_view<Aj1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Aj1_loadAction(Aj1ActionFunc* dst, Aj1ActionFunc* src) {
    auto* d = Aj1_view<be<u32>>(dst);
    auto* s = Aj1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Aj1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Aj1_callAction(void* self, Aj1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Aj1_view<Aj1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Aj1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

struct Aj1Btp {
    u8 _0[0x10];
    gptr<J3DAnmTexPattern> mpAnm;
};
struct Aj1MorfVtable {
    u8 _0[0xC];
    be<u32> mDestructor;
};
struct Aj1Morf {
    gptr<Aj1MorfVtable> mpVtable;
};
struct Aj1Emitter {
    u8 _0[8];
    cXyz mLocalScale;
    u8 _14[0x34 - 0x14];
    be<f32> mInitialVelocity;
    u8 _38[0x5C - 0x38];
    be<u32> mMaxFrame;
    be<u16> mLifeTime;
    u8 _62[0x70 - 0x62];
    be<f32> mRate;
    u8 _74[0x1B4 - 0x74];
    be<u32> mAliveCount;
    u8 _1B8[8];
    be<u32> mFreeCount;
    u8 _1C4[0x1F0 - 0x1C4];
    u8 mGlobalRotation[0x30];
    cXyz mGlobalScale;
    cXyz mGlobalTranslation;
    cXyz mGlobalParticleScale;
    u8 _244[0x10];
    be<u32> mStatus;
};
struct Aj1SmokeColour {
    u8 _0[0x16];
    be<u8> r, g, b, a;
};
inline void Aj1_stopEmitter(JPABaseEmitter* p) {
    auto* e = Aj1_view<Aj1Emitter>(p);
    e->mMaxFrame = 0xFFFFFFFF;
    e->mStatus |= 1;
}
inline const char* Aj1_staffName(dNpc_EventCut_c* cut) {
    return gabi::at<const char>(cut->mpEvtStaffName);
}
inline be<u8>* Aj1_bytes(void* p) { return Aj1_view<be<u8>>(p); }
inline bool Aj1_equal(SafeString* a, SafeString* b) {
    auto assure = [](SafeString* s) {
        auto* v = gabi::at<be<u32>>(s->__vtbl);
        gabi::call_ptr(v[5], s);
    };
    assure(a);
    assure(a);
    assure(b);
    u32 s1 = a->mStringTop, s2 = b->mStringTop;
    if (s1 == s2)
        return true;
    auto* x = gabi::at<be<u8>>(s1);
    auto* y = gabi::at<be<u8>>(s2);
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = x[i], c2 = y[i];
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}
WWHD_OFFSET(Aj1Btp, mpAnm, 0x10);
WWHD_SIZE(Aj1Btp, 0x14);
WWHD_OFFSET(Aj1MorfVtable, mDestructor, 0xC);
WWHD_SIZE(Aj1MorfVtable, 0x10);
WWHD_SIZE(Aj1Morf, 4);
WWHD_OFFSET(Aj1Emitter, mLocalScale, 8);
WWHD_OFFSET(Aj1Emitter, mInitialVelocity, 0x34);
WWHD_OFFSET(Aj1Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Aj1Emitter, mLifeTime, 0x60);
WWHD_OFFSET(Aj1Emitter, mRate, 0x70);
WWHD_OFFSET(Aj1Emitter, mAliveCount, 0x1B4);
WWHD_OFFSET(Aj1Emitter, mFreeCount, 0x1C0);
WWHD_OFFSET(Aj1Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Aj1Emitter, mGlobalScale, 0x220);
WWHD_OFFSET(Aj1Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Aj1Emitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(Aj1Emitter, mStatus, 0x254);
WWHD_SIZE(Aj1Emitter, 0x258);
WWHD_SIZE(Aj1SmokeColour, 0x1A);

s8 Aj1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Aj1_polyColour(dBgS*, cBgS_PolyInfo*);
u32 Aj1_materialSound(dBgS*, cBgS_PolyInfo*);
s16 Aj1_calcTimer(be<s16>*);
u8 Aj1_calcTimer(be<u8>*);
s32 Aj1_random(s32, s32);
s32 Aj1_btpInit(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32);
bool Aj1_lockonTruth(dAttention_c*);
fopAc_ac_c* Aj1_lockonTarget(dAttention_c*, s32);
fopAc_ac_c* Aj1_actionTarget(dAttention_c*, s32);
void Aj1_playerEyePos(cXyz*, f32);
void Aj1_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
void Aj1_emitterMatrix(Mtx34*, void*, cXyz*);
void Aj1_quake(void*, u32);
void Aj1_smokeCtor(void*, u8);
void Aj1_npcCtor(daNpc_Aj1_c*);
void Aj1_btpCtor(void*);
void* Aj1_demoGetActor(u32, u8);
J3DAnmTexPattern* Aj1_demoBtp(void*, const char*);
void Aj1_demoData(daNpc_Aj1_c*, u32, mDoExt_McaMorf*, const char*, s32, u32, u32, s32);
void Aj1_circleDestroy(void*, s32);
void Aj1_acchDestroy(dBgS_Acch*, s32);
void Aj1_actorDestroy(daNpc_Aj1_c*, s32);
u32 Aj1_messageStatus(void*);
WWHD_OFFSET(daNpc_Aj1_c, mbHidden, 0x8CE);
WWHD_OFFSET(daNpc_Aj1_c, mSubtype, 0x92E);
WWHD_OFFSET(daNpc_Aj1_HIO_c, mPrm, 0xC);
WWHD_SIZE(daNpc_Aj1_HIO_c::hio_prm_c, 0x30);

// HD: item button callbacks return a full register rather than the GC s16 type.
enum Aj1Resource_e { dRes_INDEX_AJ_BDL_ITEM_e = 9, dRes_INDEX_AJ_BDL_BODY_e = 10 };
WWHD_SIZE(Aj1AnmVtable, 0x18);
WWHD_SIZE(Aj1Anm, 8);
WWHD_SIZE(Aj1ModelAnimation, 0x3C);
WWHD_SIZE(Aj1AcchVtables, 0x24);
WWHD_OFFSET(Aj1VirtualAction, mVtableOffset, 6);
WWHD_SIZE(Aj1VirtualAction, 8);
WWHD_OFFSET(Aj1ActionFunc, d, 0);
WWHD_OFFSET(Aj1ActionFunc, i, 2);
WWHD_OFFSET(Aj1ActionFunc, f, 4);
WWHD_OFFSET(Aj1SmokeColour, r, 0x16);
WWHD_OFFSET(Aj1SmokeColour, a, 0x19);
WWHD_OFFSET(daNpc_Aj1_c::anm_prm_c, bckNum, 0);
WWHD_OFFSET(daNpc_Aj1_c::anm_prm_c, btpNum, 1);
WWHD_OFFSET(daNpc_Aj1_c::anm_prm_c, morf, 4);
WWHD_OFFSET(daNpc_Aj1_c::anm_prm_c, speed, 8);
WWHD_OFFSET(daNpc_Aj1_c::anm_prm_c, loopMode, 0xC);
WWHD_SIZE(daNpc_Aj1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_Aj1_HIO_c, mNo, 4);
WWHD_OFFSET(daNpc_Aj1_HIO_c, mEntryID, 8);
WWHD_OFFSET(daNpc_Aj1_HIO_c::hio_prm_c, mFarDist, 0x1C);
WWHD_OFFSET(daNpc_Aj1_HIO_c::hio_prm_c, mCallDist, 0x24);
WWHD_OFFSET(daNpc_Aj1_HIO_c::hio_prm_c, mLokTime, 0x2C);
inline void Aj1_destroyMorf(mDoExt_McaMorf* morf) {
    auto* vt = Aj1_view<Aj1Morf>(morf)->mpVtable.get();
    gabi::call_ptr(vt->mDestructor, morf, 3);
}
