/* Play scene part 2. */
// HD: native scene, wind and resource systems extend the GameCube play scene.
#include "d/d_s_play.h"
#include <cstring>

// HD: wait for the native resource system and start it once.
/* 025B1F54 */
static s32 phase_3(dScnPly::dScnPly_ply_c *i_this) {
    if (dScnPlyAPI_checkSystemLoad(l_resourceLoader.mValue) == 0)
        return cPhs_INIT_e;
    u32 o = l_resourceManager.mValue;
    if (gabi::at<dScnPly_ResourceManager>(o)->mIsStarted == 0) {
        gabi::at<dScnPly_ResourceManager>(o)->mIsStarted = 1;
        dScnPlyAPI_startResourceSystem(l_resourceSystem.mValue);
    }
    u32 cmd = i_this->sceneCommand.v; /* sceneCommand */
    if (cmd != 0 && gabi::at<dScnPly_SceneCommand>(cmd)->mIsComplete == 0)
        return cPhs_INIT_e; /* !sync() */
    if (dScnPlyAPI_checkFirstDynamicWave() != 0)
        return cPhs_INIT_e; /* check1stDynamicWave */
    return cPhs_NEXT_e;
}

/* 025B2F90 */
static s32 phase_5(dScnPly::dScnPly_ply_c *i_this) {
    s8 preLoadNo = (s8)l_preLoad.preLoadNo;
    if (preLoadNo < 0)
        return cPhs_NEXT_e;
    const dScnPly_PreLoadInfo *info = &PreLoadInfoT.mEntries[preLoadNo];
    u32 resName = info->resName.v;
    s32 rt = cPhs_NEXT_e;
    u32 num = info->resNum;
    if (resName != 0 && (u32)*gabi::at<be<u32>>(resName) != 0) {
        if (num > 0x23)
            dScnPlyAPI_showAssert(
                gabi::guest_addr("d_s_play.cpp"), 0x127C,
                gabi::guest_addr("resNameNum <= (sizeof(resPhase) / sizeof(resPhase[0]))"));
        if ((s32)num > 0) {
            u32 p = resName;
            u32 off = 0;
            do {
                if (dScnPlyAPI_loadResource(
                        gabi::ea(&resPhase.mPhases[off / sizeof(request_of_phase_process_class)]),
                        (u32)*gabi::at<be<u32>>(p)) != cPhs_COMPLEATE_e)
                    rt = cPhs_INIT_e;
                p += 4;
                off += 8;
            } while (--num != 0);
        }
    }
    return rt;
}

// HD: HD-only: creates the ocean heap (sOceanHeap_, "LOD48" unit heap)
/* 025B306C */
static void dScnPly_createOceanHeap() {
    u32 size = dScnPlyAPI_getUnitHeapOverhead(0x20) + 0x5000;
    gabi::Local<dScnPly_SafeString> name;
    name->mVtable = gc_tu.safeString;
    name->mString = gabi::guest_addr("Darkness Spot");
    u32 parent = dScnPlyAPI_getGameHeap();
    if (l_oceanHeap.mpHeap != 0)
        dScnPlyAPI_showAssert(gabi::guest_addr("d_s_play.cpp"), 0x110,
                              gabi::guest_addr("sOceanHeap_ == (0)"));
    l_oceanHeap.mpHeap =
        dScnPlyAPI_createOceanUnitHeap(size, 0x30, gabi::ea(name.get()), 0x100, parent, 0);
}

// HD: on the "sea" stage the ocean heap is created; returns NEXT (025B3764 completes)
/* 025B3114 */
static s32 phase_compleate(dScnPly::dScnPly_ply_c *i_this) {
    u32 play = dComIfGp_ea();
    gabi::Local<dScnPly_SafeString> stage, sea;
    sea->mVtable = gc_tu.safeString;
    sea->mString = gabi::guest_addr("sea"); /* "sea" */
    stage->mString = gabi::ea(&gabi::at<dScnPly_GameInfo>(play)->play.mCurStage);
    stage->mVtable = gc_tu.safeString;
    dScnPlyAPI_assureSafeString(gabi::ea(stage.get()));
    gabi::call_ptr(gabi::at<dScnPly_StringVtable>(stage->mVtable)->assureTermination,
                   gabi::ea(stage.get()));
    u32 s1 = stage->mString;
    gabi::call_ptr(gabi::at<dScnPly_StringVtable>(sea->mVtable)->assureTermination,
                   gabi::ea(sea.get()));
    u32 s2 = sea->mString;
    bool eq = s1 == s2;
    if (!eq) {
        for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
            u8 c = (u8)*gabi::at<be<u8>>(s1);
            if (c != (u8)*gabi::at<be<u8>>(s2))
                break;
            if (c == 0) {
                eq = true;
                break;
            }
        }
    }
    if (eq)
        dScnPly_createOceanHeap();
    return cPhs_NEXT_e;
}

/* 025B31E4 */
static s32 dScnPly_Create(dScnPly::dScnPly_ply_c *i_this) {
    return dScnPlyAPI_phaseHandler(gabi::ea(&i_this->mLoadPhs), gabi::ea(&l_method),
                                   gabi::ea(i_this)); /* dComLbG_PhaseHandler */
}

/* 025B31F8 */
static dScnPly_reg_childHIO_c *dScnPly_reg_childHIO_c_ct(dScnPly_reg_childHIO_c *self) {
    if (self == 0) {
        self = static_cast<dScnPly_reg_childHIO_c *>(operator_new(0x90));
        if (self == 0)
            return self;
    }
    self->mVtable = play_tu.mRegisterVtable;
    for (int i = 0; i < 30; i++)
        self->mFloatRegs[i] = 0.0f;
    for (int i = 0; i < 10; i++)
        self->mShortRegs[i] = 0;
    return self;
}

/* 025B326C */
static dScnPly_dark_HIO_c *dScnPly_dark_HIO_c_ct(dScnPly_dark_HIO_c *self) {
    if (self == 0) {
        self = static_cast<dScnPly_dark_HIO_c *>(operator_new(0x28));
        if (self == 0)
            return self;
    }
    self->field_0x05 = 0;
    self->mVtable = play_tu.mDarkVtable;
    self->field_0x06 = 0;
    for (int i = 0; i < 8; i++)
        self->mDarkStatus[i] =
            l_defaultDarkStatus.mWords[i]; /* dStage_roomControl_c::getDarkStatus(0) */
    return self;
}

/* 025B32FC */
static dScnPly_env_otherHIO_c *dScnPly_env_otherHIO_c_ct(dScnPly_env_otherHIO_c *self) {
    if (self == 0) {
        self = static_cast<dScnPly_env_otherHIO_c *>(operator_new(0x48));
        if (self == 0)
            return self;
    }
    self->mVtable = play_tu.mEnvOtherVtable;
    self->field_0x05 = 1;
    self->field_0x06 = 1;
    self->field_0x07 = 0;
    self->field_0x08 = 0;
    self->field_0x09 = 0;
    self->field_0x0a = 0;
    self->field_0x0b = 0;
    self->field_0x0c = 0;
    self->field_0x0d = 0x80;
    self->field_0x0e = 0x10;
    self->field_0x0f = 0;
    self->field_0x10 = 0;
    self->field_0x11 = 0;
    self->field_0x12 = 0;
    self->field_0x40 = 0;
    self->field_0x44 = 4000.0f;
    self->field_0x15 = 0;
    for (int i = 0; i < 20; i++)
        self->field_0x18[i] = 0xFFFF;
    self->field_0x16 = 0;
    return self;
}

/* 025B33B8 */
static dScnPly_env_debugHIO_c *dScnPly_env_debugHIO_c_ct(dScnPly_env_debugHIO_c *self) {
    if (self == 0) {
        self = static_cast<dScnPly_env_debugHIO_c *>(operator_new(0x2C));
        if (self == 0)
            return self;
    }
    self->field_0x08 = -100.0f;
    self->field_0x0c = -100.0f;
    self->field_0x10 = -100.0f;
    self->field_0x14 = 100.0f;
    self->field_0x18 = 100.0f;
    self->field_0x1c = 100.0f;
    self->field_0x20 = 0.0f;
    self->field_0x24 = 0.0f;
    self->field_0x28 = 0.0f;
    self->field_0x2c = 100.0f;
    self->mVtable = play_tu.mEnvDebugVtable;
    return self;
}

/* 025B3438 */
static dScnPly_msg_HIO_c *dScnPly_msg_HIO_c_ct(dScnPly_msg_HIO_c *self) {
    if (self == 0) {
        self = static_cast<dScnPly_msg_HIO_c *>(operator_new(0x14));
        if (self == 0)
            return self;
    }
    self->field_0x06 = 0;
    self->field_0x08 = 0;
    self->mID = 1; /* mID */
    self->mVtable = play_tu.mMessageVtable;
    self->field_0x10 = (u32)-1;
    self->mIsUpdate = 0; /* mIsUpdate */
    self->field_0x09 = 0;
    self->mGroup = 0; /* mGroup */
    self->field_0x07 = 0;
    return self;
}

// HD: compiler-generated initialization and template code.
/* 025B34A4 */
static void __sinit_d_s_play_cpp() {
    for (int i = 0; i < 4; i++)
        l_headerStatics.mZeroWords[i] = 0;
    dScnPlyAPI_registerGlobalObject(play_tu.mStaticDestructors[0]);
    l_headerStatics.mAngles[0] = -3.1415927f;
    l_headerStatics.mAngles[1] = 3.1415927f;
    dScnPlyAPI_constructHeaderStaticFlag(gabi::ea(&l_headerStatics.mFlag));
    dScnPlyAPI_registerGlobalObject(play_tu.mStaticDestructors[1]);
    dScnPlyAPI_constructHeaderStaticByte(gabi::ea(&l_headerStatics.mByte));
    dScnPlyAPI_registerGlobalObject(play_tu.mStaticDestructors[2]);
    l_headerStatics.mFloatDefaults[0] = 50000.0f;
    l_headerStatics.mFloatDefaults[2] = 10000.0f;
    l_headerStatics.mFloatDefaults[1] = 50000.0f;
    l_headerStatics.mFloatDefaults[3] = 10000.0f;
    dScnPly_dark_HIO_c_ct(&g_darkHIO);     /* g_darkHIO */
    g_regHIO.mVtable = play_tu.mRegVtable; /* g_regHIO */
    dScnPlyAPI_constructArray(
        gabi::ea(g_regHIO.mChildren), 33, sizeof(dScnPly_reg_childHIO_c),
        gabi::guest_fn(
            (const void *)&dScnPly_reg_childHIO_c_ct)); /* __construct_array of the children */
    g_envHIO.mVtable = play_tu.mEnvVtable;              /* g_envHIO */
    dScnPly_env_otherHIO_c_ct(&g_envHIO.mOther);
    dScnPly_env_debugHIO_c_ct(&g_envHIO.mEnvDebug);
    dScnPly_msg_HIO_c_ct(&g_msgDHIO); /* g_msgDHIO */
    l_preLoadHIO.mValue = play_tu.mPreLoadVtable;
}

// HD: compiler-generated and template code of this translation unit.
// Identical deleting-destructor bodies retain their distinct verified entry points.
/* 025B35E8 */
static void dScnPly_deletingDestructorFirst(u32 self, u32 flags) {
    if (self == 0)
        return;
    if (!(flags & 1))
        return;
    dScnPlyAPI_deallocate(self);
}

/* 025B3734 */
static void dScnPly_deletingDestructorSecond(u32 self, u32 flags) {
    if (self == 0)
        return;
    if (!(flags & 1))
        return;
    dScnPlyAPI_deallocate(self);
}

/* 025B3748 */
static void dScnPly_deletingDestructorThird(u32 self, u32 flags) {
    if (self == 0)
        return;
    if (!(flags & 1))
        return;
    dScnPlyAPI_deallocate(self);
}

/* 025B3770 */
static void dScnPly_deletingDestructorFourth(u32 self, u32 flags) {
    if (self == 0)
        return;
    if (!(flags & 1))
        return;
    dScnPlyAPI_deallocate(self);
}

/* 025B3784 */
static void dScnPly_deletingDestructorFifth(u32 self, u32 flags) {
    if (self == 0)
        return;
    if (!(flags & 1))
        return;
    dScnPlyAPI_deallocate(self);
}

/* 025B3798 */
static void dScnPly_deletingDestructorSixth(u32 self, u32 flags) {
    if (self == 0)
        return;
    if (!(flags & 1))
        return;
    dScnPlyAPI_deallocate(self);
}

/* 025B37AC */
static void dScnPly_deletingDestructorSeventh(u32 self, u32 flags) {
    if (self == 0)
        return;
    if (!(flags & 1))
        return;
    dScnPlyAPI_deallocate(self);
}

// HD: sead::SafeString assureTerminationImpl_ (this unit's copy): buffer[size - 1] = 0
/* 025B35FC */
static void dScnPly_FixedString_assureTermination(dScnPly_FixedString8 *self) {
    u32 str = self->mString;
    u32 size = self->mCapacity;
    gabi::at<be<u8>>(str)[size - 1] = 0;
}

/* va_list of a variadic function: the GHS/EABI layout as the original places it at sp+8 (register
 * save area at +0x10, the caller's stack arguments at +0x88 = entry sp + 8) */

// HD: sead::FixedSafeString<8>::FixedSafeString(fmt, ...) (this unit's copy; formats with formatV 02759C10)
/* 025B3614 */
static u32 FixedSafeString8_ct_format(u32 self, u32 fmt, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9,
                                      u32 a10) {
    bool floats = gabi::cpu->cr[6] != 0; /* cr1eq: the caller passed floating-point arguments */
    gabi::Local<dScnPly_VaList> va;
    const u32 regs[8] = {self, fmt, a5, a6, a7, a8, a9, a10};
    for (int i = 0; i < 8; i++)
        va->gprs[i] = regs[i];
    if (floats) {
        for (int i = 0; i < 8; i++) {
            double d = gabi::cpu->f[1 + i].ps0;
            u64 bits;
            memcpy(&bits, &d, 8);
            va->fprs[2 * i] = (u32)(bits >> 32);
            va->fprs[2 * i + 1] = (u32)bits;
        }
    }
    if (self == 0) {
        self = dScnPlyAPI_allocate(0x14);
        if (self == 0)
            return self;
    }
    auto *string = gabi::at<dScnPly_FixedString8>(self);
    string->mString = gabi::ea(string->mBuffer);
    string->mVtable = play_tu.mFixedStringBaseVtable;
    string->mCapacity = 8;
    string->mBuffer[7] = 0;
    u32 buf = string->mString;
    string->mVtable = play_tu.mBufferedStringVtable;
    *gabi::at<be<u8>>(buf) = 0;
    string->mVtable = play_tu.mFixedString8Vtable;
    u32 v = gabi::ea(va.get());
    va->gpr = 2;
    va->overflow_arg_area = v + 0x88;
    va->reg_save_area = gabi::ea(va->gprs);
    va->fpr = 0;
    dScnPlyAPI_formatStringV(self, fmt, v); /* formatV(fmt, va) */
    return self;
}

// HD: phase_6 (HD: no REL preloading)
/* 025B375C */
static s32 phase_6(dScnPly::dScnPly_ply_c *i_this) { return cPhs_NEXT_e; }

// HD: the last phase (complete)
/* 025B3764 */
static s32 phase_end(dScnPly::dScnPly_ply_c *i_this) { return cPhs_COMPLEATE_e; }

// HD: an empty per-TU inline (called on SafeString temporaries)
/* 025B376C */
static void dScnPly_SafeString_assureTerminationImpl(u32 self) { (void)self; }

/* 025B37C0 */
static s32 dScnPly_processIsDeletable(u32 self) { return 1; }

// HD: deletes the process and returns 0
/* 025B37C8 */
static s32 dScnPly_requestProcessDeletion(u32 p) {
    dScnPlyAPI_deleteProcess(p); /* fpcM_Delete */
    return 0;
}
/* WWHD glue
str 100533FC "d_s_play.cpp" in 025B2F90
str 1005340C "resNameNum <= (sizeof(resPhase) / sizeof(resPhase[0]))" in 025B2F90
str 10053444 "Darkness Spot" renamed in 025B306C
str 10053458 "d_s_play.cpp" in 025B306C
str 10053468 "sOceanHeap_ == (0)" in 025B306C
str 1005347C "sea" in 025B3114
*/
