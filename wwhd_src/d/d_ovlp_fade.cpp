/** Overlap fade controller, with HD stage-name modes and display calls.
 * Verified WWHD unit d_ovlp_fade. */
#include "d/d_ovlp_fade.h"

static gfn dOvlpFd_execute_f;
#define l_display (*gabi::at<gptr<void>>(0x101F86B4))
#define l_stageName (gabi::at<const char>(0x1047E6B8))

static BOOL dOvlpFd_FadeOut(overlap1_class*);
static BOOL dOvlpFd_Wait(overlap1_class*);
static BOOL dOvlpFd_FadeIn(overlap1_class*);
static void seadSafeString_assureTerminationImpl();

// HD: SafeString equality is inlined, including its bounded unsigned-byte comparison.
/* inlined into dOvlpFd_FadeOut */
static bool sameStageName(const be<u8>* a, const be<u8>* b) {
    if (a == b) return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 x = a[i];
        u8 y = b[i];
        if (x != y) return false;
        if (x == 0) return true;
    }
    return false;
}

/* inlined into dOvlpFd_FadeOut */
static bool stageNameEquals(const char* name) {
    gabi::Local<SafeString> left, right;
    right->__vtbl = 0x10051884;
    left->__vtbl = 0x10051884;
    left->mStringTop = gabi::guest_addr(name);
    right->mStringTop = gabi::ea(l_stageName);
    // HD: keep the direct assure call and both virtual calls in their original order.
    gabi::call_ptr<void>(gabi::guest_fn((const void*)seadSafeString_assureTerminationImpl), left.get());
    auto* leftVtable = gabi::at<dOvlpFd_SafeStringVtable>(left->__vtbl);
    gabi::call_ptr<void>(leftVtable->assureTerminationImpl.get(), left.get());
    u32 saved = left->mStringTop;
    auto* rightVtable = gabi::at<dOvlpFd_SafeStringVtable>(right->__vtbl);
    gabi::call_ptr<void>(rightVtable->assureTerminationImpl.get(), right.get());
    return sameStageName(gabi::at<const be<u8>>(saved), gabi::at<const be<u8>>(right->mStringTop));
}

/* 025A1798 */
static BOOL dOvlpFd_Draw() {
    return TRUE;
}

/* 025A17A0 */
static BOOL dOvlpFd_FadeOut(overlap1_class* i_this) {
    if (i_this->mFadeOutTime == 0) {
        if (i_this && i_this->base.mProfName == fpcNm_OVERLAP6_e) {
            if (!dOvlpFd_startFadeInHD(l_display, 1, 0)) return TRUE;
            i_this->mFadeOutTime = 1;
        } else {
            // HD: stage-specific modes and the normal path's return test differ from GameCube.
            u32 mode = stageNameEquals("Demo01") ? 1 : (stageNameEquals("Demo37") ? 2 : 0);
            if (dOvlpFd_startFadeInHD(l_display, 26, mode)) return TRUE;
            i_this->mFadeOutTime = 26;
        }
    }
    fopOvlpM_SceneIsStart();
    u32 count = (u32)i_this->mFadeOutTime - 1;
    i_this->mFadeOutTime = count;
    if (count == 0) {
        fopOvlpM_Done(i_this);
        i_this->mFadeOutTime = (u32)i_this->mFadeOutTime + 1;
    }
    return TRUE;
}

/* 025A19E0 */
static BOOL dOvlpFd_Wait(overlap1_class* i_this) {
    if (fopOvlpM_IsOutReq(i_this)) {
        dOvlpFd_execute_f.v = gabi::guest_fn((const void*)dOvlpFd_FadeOut);
        // HD: present in the retail HD wait path.
        fopOvlpM_SceneIsStart();
    }
    return TRUE;
}

/* 025A1A20 */
static BOOL dOvlpFd_FadeIn(overlap1_class* i_this) {
    u32 count = i_this->mFadeInTime;
    if (count == 0) {
        count = 26;
        i_this->mFadeInTime = count;
        if (i_this && (i_this->base.mProfName == fpcNm_OVERLAP0_e || i_this->base.mProfName == fpcNm_OVERLAP7_e)) {
            mDoGph_startFadeOutHD(26);
            count = i_this->mFadeInTime;
        }
    }
    i_this->mFadeInTime = count - 1;
    if (count == 1) {
        dOvlpFd_execute_f.v = gabi::guest_fn((const void*)dOvlpFd_Wait);
        // HD: fade completion uses 1 and finishes the display fade before reporting Done.
        mDoGph_startFadeOutHD(1);
        dOvlpFd_finishFadeHD(l_display);
        fopOvlpM_Done(i_this);
    }
    return TRUE;
}

/* 025A1ACC */
static BOOL dOvlpFd_Execute(overlap1_class* i_this) {
    gabi::call_ptr<void>(dOvlpFd_execute_f.get(), i_this);
    return TRUE;
}

/* 025A1AFC */
static BOOL dOvlpFd_IsDelete() { return TRUE; }

/* 025A1B04 */
static BOOL dOvlpFd_Delete() { return TRUE; }

/* 025A1B0C */
static u32 dOvlpFd_Create() {
    dOvlpFd_execute_f.v = gabi::guest_fn((const void*)dOvlpFd_FadeIn);
    return cPhs_COMPLEATE_e;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025A1B24 */
static void __sinit_d_ovlp_fade_cpp() {
    sinit_header_statics_rodata(0x1047B0E4, 0x101EA1EC, 0x1047B0F0, 0x100518B4);
}

/* 025A1BB8 */
static void seadSafeString__dt(SafeString* self, u32 flags) {
    if (self && (flags & 1)) operator_delete(self);
}

/* 025A1BCC */
static void seadSafeString_assureTerminationImpl() {}

/* WWHD glue (tools/verify/autoglue.py): dispatch pointer, HD stage names and free call adapters
data dOvlpFd_execute_f 101EA1D4
str 1005189C "Demo01"
str 100518A4 "Demo37"
ext 02728CB8 u32 dOvlpFd_startFadeInHD(void*, u32, u32)
ext 02728D5C void dOvlpFd_finishFadeHD(void*)
ext 025F05E8 void mDoGph_startFadeOutHD(u32)
ext 025DBD74 void fopOvlpM_SceneIsStart()
ext 025DBDC4 u32 fopOvlpM_IsOutReq(overlap1_class*)
ext 025DBDDC void fopOvlpM_Done(overlap1_class*)
*/
