/* hd_olv_0203B620: the HD Miiverse operation manager translation unit (cking::olv OliveOperationMgr,
 * 0203B620..0203E1CF). No GameCube source: HD-only code.
 *
 * A sead singleton (0x98: IDisposer +0, flags +0x10.., heaps +0x14..+0x20, app data +0x24.. (0x10 bytes),
 * worker thread +0x34, state +0x38, 64-bit start time +0x78, error override +0x84, network init flags
 * +0x88..+0x8C, vtable +0x90) that runs Miiverse operations on a sead::DelegateThread: initialise
 * (nn::ac connect, sockets, NSSL, curl, nn::olv Initialize), post a Tingle-bottle message with a
 * screenshot (UploadPostDataByPostApp), empathy ("Yeah"), open the Miiverse portal on a post or user,
 * download post lists for the bottle spots, error reporting with an error-code whitelist for the
 * error viewer, and the companions (deleting destructors, SafeString / WSafeString terminators).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_olv_0203B620 {

/* ---- imports ---- */
static constexpr u32 OSReport = 0xC0009EE8;
static constexpr u32 OSGetSystemTime = 0xC0009C98;
static constexpr u32 olv_Finalize = 0xC0004978;
static constexpr u32 olv_IsInitialized = 0xC0004CA8;
static constexpr u32 olv_Cancel = 0xC0004908;
static constexpr u32 olv_GetErrorCode = 0xC0004A70;
static constexpr u32 curl_global_cleanup = 0xC0005A20;
static constexpr u32 NSSLFinish = 0xC0005BA0;
static constexpr u32 socket_lib_finish = 0xC0006020;
static constexpr u32 ac_Close = 0xC0007C68;
static constexpr u32 ac_Initialize = 0xC0007E48;
static constexpr u32 ac_Connect = 0xC0007CA0;
static constexpr u32 ac_GetLastErrorCode = 0xC0007D70;
static constexpr u32 socket_lib_init = 0xC0006028;
static constexpr u32 NSSLInit = 0xC0005BD8;
static constexpr u32 curl_global_init_mem = 0xC0005A30;
static constexpr u32 OSBlockMove_ = 0xC0009988;
static constexpr u32 OSBlockSet_ = 0xC0009990;
static constexpr u32 OSTicksToCalendarTime = 0xC000A180;
static constexpr u32 ProcUIRegisterCallback = 0xC0007890;
static constexpr u32 GX2InitTextureRegs = 0xC0006498;
static constexpr u32 act_GetPrincipalId = 0xC0004640;
static constexpr u32 olv_Initialize = 0xC0004C98;
static constexpr u32 olv_SetReportTypes = 0xC0004E10;
static constexpr u32 olv_GetResultByPostApp = 0xC0004BA0;
static constexpr u32 olv_InitializeParam_ct = 0xC0004F00;
static constexpr u32 olv_InitializeParam_SetWork = 0xC0004E90;
static constexpr u32 olv_UploadPostParam_ct = 0xC0004F70;       /* UploadPostDataByPostAppParam */
static constexpr u32 olv_UploadPostParam_SetBodyTextMaxLength = 0xC0004CF0;
static constexpr u32 olv_UploadPostParam_SetFlags = 0xC0004DB0;
static constexpr u32 olv_UploadPostDataByPostApp = 0xC0004EF0;
static constexpr u32 olv_UploadParam_SetExternalImageData = 0xC0004D68;
static constexpr u32 olv_UploadParam_SetAppData = 0xC0004CD8;
static constexpr u32 olv_UploadParam_SetTopicTag = 0xC0004E70;
static constexpr u32 olv_UploadParam_SetSearchKey = 0xC0004E18;
static constexpr u32 olv_UploadParamBase_dt = 0xC0004FE8;
static constexpr u32 olv_EmpathyParam_ct = 0xC0004F68;          /* UploadEmpathyToPostDataParam */
static constexpr u32 olv_EmpathyParam_SetPostId = 0xC0004E08;
static constexpr u32 olv_UploadEmpathyToPostData = 0xC0004EE0;
static constexpr u32 olv_PortalParam_ct = 0xC0004F28;           /* StartPortalAppParam */
static constexpr u32 olv_PortalParam_SetPostId = 0xC0004E00;
static constexpr u32 olv_PortalParam_SetUserPid = 0xC0004E78;
static constexpr u32 olv_StartPortalApp = 0xC0004E98;
static constexpr u32 olv_TopicData_ct = 0xC0004F20;             /* DownloadedTopicData */
static constexpr u32 olv_ListParam_ct = 0xC0004F50;             /* DownloadPostDataListParam */
static constexpr u32 olv_ListParam_SetSearchKey = 0xC0004E20;
static constexpr u32 olv_ListParam_SetPostDataMaxNum = 0xC0004DE8;
static constexpr u32 olv_ListParam_SetFlags = 0xC0004DA8;
static constexpr u32 olv_DownloadPostDataList = 0xC0004960;
static constexpr u32 olv_Data_GetPostId = 0xC0004B58;
static constexpr u32 olv_Data_GetPostDate = 0xC0004B50;
static constexpr u32 olv_Data_GetMiiNickname = 0xC0004B00;
static constexpr u32 olv_Data_GetTopicTag = 0xC0004C28;
static constexpr u32 olv_Data_GetUserPid = 0xC0004C60;
static constexpr u32 olv_Data_TestFlags = 0xC0004EB0;
static constexpr u32 olv_Data_GetBodyText = 0xC00049C8;
static constexpr u32 olv_Data_GetBodyMemo = 0xC00049B8;
static constexpr u32 olv_Data_GetAppData = 0xC00049A0;
static constexpr u32 olv_Data_GetExternalImageDataSize = 0xC0004A98;
static constexpr u32 olv_Data_DownloadExternalImageData = 0xC0004958;
static constexpr u32 olv_Post_GetEmpathyCount = 0xC0004A68;
static constexpr u32 olv_Post_GetCommentCount = 0xC00049D8;

/* globals */
static constexpr u32 ERRVIEW = 0x1018F2BC;   /* ErrorViewerTask instance */
static constexpr u32 COMMENTMGR = 0x1018F4AC; /* OliveCommentMgr instance */
static constexpr u32 POST_OK = 0x1018F510, POST_DONE = 0x1018F511, POST_PENDING = 0x1018F512;
static constexpr u32 TOPIC = 0x10200C9C, LISTPARAM = 0x10201C9C, POSTS = 0x10202CA0; /* 50 x 0xC208 */
static constexpr u32 ac_Finalize = 0xC0007CB8;

static constexpr u32 SS_VT = 0x10005CF0; /* this TU's SafeString vtable */
static inline void sstr(u32 a, u32 s) { st(a + 4, SS_VT); st(a + 0, s); }
/* sead::Thread / heap virtuals through the vtable at +0xC */
static inline u32 v0c(u32 o, u32 slot) { return vfn(o, 0xC, slot); }
/* OSReport (varargs, crclr cr1eq): only the arguments the format uses are passed */
static inline void osr(u32 fmt) {
    gabi::cpu->cr[6] = 0;
    gabi::call(OSReport, fmt);
}
static inline void osr1(u32 fmt, u32 a4) {
    gabi::cpu->cr[6] = 0;
    gabi::call(OSReport, fmt, a4);
}
static inline void osr(u32 fmt, u32 a4, u32 a5) {
    gabi::cpu->cr[6] = 0;
    gabi::call(OSReport, fmt, a4, a5);
}

/* 0203B620: deleting destructor (vtable 1015E028 at +0x1C, base dtor 0275CFD0) */
static void Dt_0203B620(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B620, void, p, flags);
    if (p == 0) return;
    st(p + 0x1C, 0x1015E028);
    gabi::call(0x0275CFD0, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B620, Dt_0203B620);

static void Empty_0203B680() {
    WWHD_FUNC(0x0203B680, void);
}
VERIFY(0x0203B680, Empty_0203B680);

static void SafeBuf_assureTerminate(u32 self) {
    WWHD_FUNC(0x0203B684, void, self);
    stb(ld(self) + ld(self + 8) - 1, 0);
}
VERIFY(0x0203B684, SafeBuf_assureTerminate);

static void Dt_0203B69C(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B69C, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B69C, Dt_0203B69C);

static void Empty_0203B6B0() {
    WWHD_FUNC(0x0203B6B0, void);
}
VERIFY(0x0203B6B0, Empty_0203B6B0);

static void WSafeBuf_assureTerminate(u32 self) {
    WWHD_FUNC(0x0203B6B4, void, self);
    sth(ld(self) + ld(self + 8) * 2 - 2, 0);
}
VERIFY(0x0203B6B4, WSafeBuf_assureTerminate);

static void Dt_0203B6D0(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B6D0, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B6D0, Dt_0203B6D0);

static void Dt_0203B6E4(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B6E4, void, p, flags);
    if (p == 0) return;
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B6E4, Dt_0203B6E4);

/* 0203B6F8: new (heap, align) T() with a 0x24-byte T (ctor 0275D12C) */
static u32 Create_0203B6F8(u32 unused, u32 heap, u32 align) {
    WWHD_FUNC(0x0203B6F8, u32, unused, heap, align);
    u32 p = gabi::call<u32>(0x0273B050, 0x24u, heap, align);
    if (p == 0) return 0;
    return gabi::call<u32>(0x0275D12C, p);
}
VERIFY(0x0203B6F8, Create_0203B6F8);

/* 0203B728: deleting destructor of a holder of two 10-element arrays (0x3EC each, element dtor
 * 02038FF0) at +0x274C and +0x14 */
static void Dt_0203B728(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B728, void, p, flags);
    if (p == 0) return;
    gabi::call(0x028F0164, p + 0x274C, 0xAu, 0x3ECu, 0x02038FF0, 0u, 0u);
    gabi::call(0x028F0164, p + 0x14, 0xAu, 0x3ECu, 0x02038FF0, 0u, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B728, Dt_0203B728);

/* 0203B7B4..0203B7CC: float parameters (data words 1018F4C8.. : 2.0, 40.0, 0.0) */
static f32 Param_0203B7B4() {
    WWHD_FUNC(0x0203B7B4, f32);
    return ldf(0x1018F4C8);
}
VERIFY(0x0203B7B4, Param_0203B7B4);

static f32 Param_0203B7C0() {
    WWHD_FUNC(0x0203B7C0, f32);
    return ldf(0x1018F4CC);
}
VERIFY(0x0203B7C0, Param_0203B7C0);

static f32 Param_0203B7CC() {
    WWHD_FUNC(0x0203B7CC, f32);
    return ldf(0x1018F4D0);
}
VERIFY(0x0203B7CC, Param_0203B7CC);

/* 0203B7D8: OliveOperationMgr constructor (0x98; the IDisposer part at +0 is built by createInstance) */
static u32 OpMgr_ct(u32 self) {
    WWHD_FUNC(0x0203B7D8, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x98);
        if (t == 0) return 0;
    }
    st(t + 0x18, 0);
    st(t + 0x90, 0x100061E0);
    st(t + 0x1C, 0);
    st(t + 0x14, 0);
    st(t + 0x20, 0);
    stb(t + 0x10, 0);
    gabi::call(0x028F521C, t + 0x24, 0x10u);
    st(t + 0x38, 0);
    st(t + 0x34, 0);
    gabi::call(0x02760084, t + 0x3C);
    gabi::call(0x02760DE8, t + 0x78);
    stb(t + 0x88, 0);
    stb(t + 0x8C, 0);
    st(t + 0x84, 0xFDE7F);
    st(t + 0x80, 0);
    stb(t + 0x8B, 0);
    stb(t + 0x89, 0);
    stb(t + 0x8A, 0);
    return t;
}
VERIFY(0x0203B7D8, OpMgr_ct);

/* 0203B88C: sead singleton createInstance(heap) (instance 1018F504, disposer 1018F50C) */
static u32 OpMgr_createInstance(u32 heap) {
    WWHD_FUNC(0x0203B88C, u32, heap);
    u32 cur = ld(0x1018F504);
    if (cur != 0) return cur;
    u32 d = gabi::call<u32>(0x0273B0D4, 0x98u, heap, 4u);
    if (d != 0) {
        gabi::call(0x02752B0C, d, heap, 3u);
        st(d + 0xC, 0x100061D0);
    }
    st(0x1018F50C, d);
    u32 r = d;
    if (d != 0) r = gabi::call<u32>(0x0203B7D8, d);
    st(0x1018F504, r);
    return r;
}
VERIFY(0x0203B88C, OpMgr_createInstance);

/* 0203B920: destructor */
static void OpMgr_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203B920, void, p, flags);
    if (p == 0) return;
    st(p + 0x90, 0x100061E0);
    gabi::call(olv_Finalize, p);
    gabi::call(0x02760168, p + 0x3C, 2u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203B920, OpMgr_dt);

/* 0203B984: create the worker thread (sead::DelegateThread, method 0203DEEC) on its own heap */
static void OpMgr_createThread(u32 self) {
    WWHD_FUNC(0x0203B984, void, self);
    gabi::Local<be<u32>[2]> nm;
    gabi::Local<be<u32>[2]> sa; /* outgoing stack-argument area */
    sstr(nm.a, 0x10005DC0);
    u32 parent = gabi::call<u32>(0x0203E8C4);
    u32 heap = gabi::call<u32>(0x02753004, 0u, nm.a, parent, 1u, 0u);
    u32 th = gabi::call<u32>(0x0273B050, 0x98u, heap, 4u);
    if (th != 0) {
        sstr(nm.a, 0x10005DA8);
        u32 dg = gabi::call<u32>(0x0273B050, 0x10u, heap, 4u);
        if (dg != 0) {
            /* sead::Delegate<OliveOperationMgr>: object, GHS member pointer {0, -1, fn} */
            st(dg + 4, self);
            sth(dg + 8, 0);
            st(dg + 0xC, 0x0203DEEC);
            st(dg + 0, 0x10005D98);
            sth(dg + 0xA, 0xFFFF);
        }
        u32 prio = ld(0x101F8B9C) + 3;
        th = gabi::call<u32>(0x0275FBD8, th, nm.a, dg, heap, prio, 0u, 0x7FFFFFFFu, 0x9000u, 0x20u);
    }
    st(self + 0x34, th);
    gabi::call_ptr<u32>(v0c(heap, 0x2C), heap);
    u32 t = ld(self + 0x34);
    gabi::call_ptr<u32>(v0c(t, 0x2C), t);
    st(self + 0x38, 0);
    Pair32 now = gabi::call<Pair32>(OSGetSystemTime);
    st(self + 0x7C, now.r4);
    st(self + 0x78, now.r3);
}
VERIFY(0x0203B984, OpMgr_createThread);

/* 0203BBB0: start: work heap (0x380000) with an aligned block, a 1 MB heap (also published at
 * 1018F508), state 2 and wake the thread; refused when the state is not idle */
static u32 OpMgr_start(u32 self) {
    WWHD_FUNC(0x0203BBB0, u32, self);
    u32 state = ld(self + 0x38);
    if (state != 0) {
        osr(0x10005DD4); /* no format arguments (r6 holds the state) */
        return 0;
    }
    u32 parent = gabi::call<u32>(0x0203E8C4);
    gabi::Local<be<u32>[2]> nm;
    sstr(nm.a, 0x10005DFC);
    u32 h = gabi::call<u32>(0x02754B38, 0x380000u, nm.a, parent, 1u, 0u);
    st(self + 0x14, h);
    u32 sz = gabi::call_ptr<u32>(v0c(h, 0x7C), h, 0x40u);
    u32 h2 = ld(self + 0x14);
    st(self + 0x1C, sz);
    st(self + 0x18, gabi::call_ptr<u32>(v0c(h2, 0x34), h2, sz, 0x40u));
    sstr(nm.a, 0x10005E08);
    u32 h3 = gabi::call<u32>(0x02753004, 0x100000u, nm.a, parent, 1u, 0u);
    st(self + 0x20, h3);
    st(0x1018F508, h3);
    u32 t = ld(self + 0x34);
    st(self + 0x38, 2);
    gabi::call_ptr<u32>(v0c(t, 0x1C), t, 1u, 1u);
    return 1;
}
VERIFY(0x0203BBB0, OpMgr_start);

/* 0203BCF4: cancel: state 6, stop the thread, and when it had initialised (thread state 3/4) shut
 * down nn::olv, curl, NSSL, sockets and nn::ac as far as they were set up */
static void OpMgr_cancel(u32 self) {
    WWHD_FUNC(0x0203BCF4, void, self);
    st(self + 0x38, 6);
    gabi::call(olv_Cancel);
    u32 t = ld(self + 0x34);
    gabi::call_ptr<u32>(v0c(t, 0x34), t, 1u);
    t = ld(self + 0x34);
    gabi::call_ptr<u32>(v0c(t, 0x3C), t);
    u32 ts = ld(ld(self + 0x34) + 0x80);
    if (ts != 3 && ts != 4) return;
    if (gabi::call<u32>(olv_IsInitialized) != 0) {
        gabi::call(olv_Cancel);
        gabi::call(olv_Finalize);
    }
    if (lbz(self + 0x88) != 0) gabi::call(curl_global_cleanup);
    if (lbz(self + 0x89) != 0) gabi::call(NSSLFinish);
    if (lbz(self + 0x8A) != 0) gabi::call(socket_lib_finish);
    if (lbz(self + 0x8B) != 0) gabi::call(ac_Close);
    if (lbz(self + 0x8C) != 0) gabi::call(ac_Finalize);
}
VERIFY(0x0203BCF4, OpMgr_cancel);

/* 0203BDD0: wake the thread while the state is 2 */
static void OpMgr_wake(u32 self) {
    WWHD_FUNC(0x0203BDD0, void, self);
    if (ld(self + 0x38) != 2) return;
    u32 t = ld(self + 0x34);
    gabi::call_ptr<u32>(v0c(t, 0x1C), t, 1u, 1u);
}
VERIFY(0x0203BDD0, OpMgr_wake);

/* 0203BDF8: report a Miiverse error; with SHOW, hand the code (or the override +0x84) to the error
 * viewer (1018F2BC), marked fatal for a fixed list of codes */
static u32 isListedError(u32 c) {
    if (c >= 0xFBB58) {
        if (c >= 0xFC008) {
            if (c <= 0xFC00F) return 1;
            return c == 0xFC06B || c == 0x119019 || c == 0x119FB9;
        }
        if (c <= 0xFBB5E) return 1;
        if (c < 0xFBBBB) return 0;
        if (c <= 0xFBBC2) return 1;
        if (c < 0xFBC1F) return 0;
        if (c <= 0xFBC27) return 1;
        return c == 0xFBC83;
    }
    if (c >= 0xF9A93) {
        if (c == 0xF9A93) return 1;
        if (c < 0xF9AA7) return 0;
        if (c <= 0xF9AA8) return 1;
        if (c < 0xFB964) return 0;
        if (c <= 0xFB96B) return 1;
        return c == 0xFB9C7;
    }
    return c == 0xF945D || c == 0xF9899 || c == 0xF9A27 || c == 0xF9A2F;
}
static void OpMgr_reportError(u32 self, u32 res, u32 show) {
    WWHD_FUNC(0x0203BDF8, void, self, res, show);
    u32 code = gabi::call<u32>(olv_GetErrorCode, res);
    osr(0x10005E14);
    u32 r = ld(res);
    u32 mask = ((r >> 27) & 3) == 3 ? 0x3FFu : 0xFFFFFu;
    osr(0x10005E38, r & mask, code);
    if (show == 0) return;
    u32 ev = ld(0x1018F2BC);
    if (ev == 0) return;
    u32 ov = ld(self + 0x84);
    if (ov != 0) code = ov;
    gabi::call(0x0203300C, ev, code, isListedError(code));
}
VERIFY(0x0203BDF8, OpMgr_reportError);

/* 0203C05C: UTF-8 -> UTF-16 (1..3-byte sequences; stops at the first other lead byte) */
static void Utf8ToUtf16(u32 dst, u32 src) {
    WWHD_FUNC(0x0203C05C, void, dst, src);
    u32 n = 0;
    u32 c = lbz(src);
    u32 d = dst;
    while (c != 0) {
        if ((c & 0x80) == 0) {
            src += 1;
            n += 1;
            sth(d, c & 0x7F);
            d += 2;
        } else if ((c & 0xE0) == 0xC0) {
            u32 v = (c << 6) & 0x7C0;
            sth(d, v);
            u32 c1 = lbz(src + 1);
            n += 1;
            v = (v & ~0x3Fu) | (c1 & 0x3F);
            src += 2;
            sth(d, v);
            d += 2;
        } else if ((c & 0xF0) == 0xE0) {
            u32 v = (c << 12) & 0xF000;
            sth(d, v);
            u32 c1 = lbz(src + 1);
            v = (v & ~0xFC0u) | ((c1 << 6) & 0xFC0);
            sth(d, v);
            u32 c2 = lbz(src + 2);
            n += 1;
            v = v | (c2 & 0x3F);
            src += 3;
            sth(d, v);
            d += 2;
        } else {
            break;
        }
        c = lbz(src);
    }
    sth(dst + n * 2, 0);
}
VERIFY(0x0203C05C, Utf8ToUtf16);

/* thread-status helpers of the Miiverse system UI hooks (02035D78 instance) */
static inline void sysui_a(u32 v) { u32 r = gabi::call<u32>(0x02035D78); gabi::call(0x02035ED4, r, v); }
static inline void sysui_b(u32 v) { u32 r = gabi::call<u32>(0x02035D78); gabi::call(0x02035F3C, r, v); }
static inline void thread_send(u32 self, u32 msg) {
    u32 t = ld(self + 0x34);
    gabi::call_ptr<u32>(v0c(t, 0x1C), t, msg, 1u);
}
static inline u32 thread_trysend(u32 self, u32 msg) {
    u32 t = ld(self + 0x34);
    return gabi::call_ptr<u32>(v0c(t, 0x1C), t, msg, 1u);
}

/* 0203C118: post the bottle message (UploadPostDataByPostApp): optional screenshot IMG of the save's
 * picture album with its app data, topic tag from the comment manager, search key "from_game" */
static void OpMgr_post(u32 self, u32 img) {
    WWHD_FUNC(0x0203C118, void, self, img);
    gabi::call(0x020330E8, ld(ERRVIEW));
    sysui_b(0);
    if (ld(self + 0x84) != 0) {
        gabi::Local<be<u32>[1]> res;
        stb(POST_OK, 0);
        st(res.a, 0xFFFFFFFF);
        stb(POST_DONE, 1);
        gabi::call(0x0203BDF8, self, res.a, 1u);
        sysui_a(1);
        sysui_b(1);
        thread_send(self, 1);
        return;
    }
    gabi::Local<be<u32>[0x500]> param; /* UploadPostDataByPostAppParam (0x1400) */
    gabi::call(olv_UploadPostParam_ct, param.a);
    if (img != 0xFFFFFFFF) {
        u32 album = gabi::call<u32>(0x02720144, ld(0x101F84DC) + 0x12C0);
        u32 data = gabi::call<u32>(0x02726188, album, img);
        gabi::call(olv_UploadParam_SetExternalImageData, param.a, data, 0x50000u);
        u32 x = gabi::call<u32>(0x027260BC, album, img);
        stb(self + 0x24, gabi::call<u32>(0x02725A00, x));
        stb(self + 0x25, gabi::call<u32>(0x02726120, album, img));
        st(self + 0x28, gabi::call<u32>(0x027260DC, album, img));
        x = gabi::call<u32>(0x027260BC, album, img);
        st(self + 0x2C, gabi::call<u32>(0x027259F0, x));
        x = gabi::call<u32>(0x027260BC, album, img);
        st(self + 0x30, gabi::call<u32>(0x02725B44, x));
        gabi::call(olv_UploadParam_SetAppData, param.a, self + 0x24, 0x10u);
    }
    /* WFixedSafeString<0x33> tag = WSafeString(L"") (copy with sead's capped strlen) */
    struct wfixed_l { be<u32> h[3]; be<u16> b[0x34]; }; /* {buffer, vtable, capacity} + inline buffer */
    gabi::Local<wfixed_l> tag;
    const u32 bufa = tag.a + 0xC;
    struct bufref_l { u32 a; };
    bufref_l buf{bufa};
    gabi::Local<be<u32>[2]> src;
    st(tag.a + 0, buf.a);
    st(tag.a + 8, 0x33);
    st(src.a + 4, 0x10005D20);
    st(tag.a + 4, 0x10005D68);
    sth(buf.a + 0x64, 0);
    st(src.a + 0, 0x10005E58);
    gabi::call(0x0203E250, src.a);
    u32 p = ld(src.a);
    s32 n = 0;
    if (lhz(p) != 0) {
        for (;;) {
            n += 1;
            p += 2;
            if (n > 0x40000) { n = 0; break; }
            if (lhz(p) == 0) break;
        }
    }
    s32 cap = (s32)ld(tag.a + 8);
    u32 vt = ld(src.a + 4);
    if (!(n < cap)) n = cap - 1;
    gabi::call_ptr<u32>(ld(vt + 0x14), src.a);
    gabi::call(OSBlockMove_, buf.a, ld(src.a), (u32)n * 2, 0u);
    sth(buf.a + (u32)n * 2, 0);
    u32 cm = ld(COMMENTMGR);
    st(tag.a + 4, 0x10005D80);
    gabi::call(0x0203A984, cm, tag.a);
    gabi::call_ptr<u32>(ld(ld(tag.a + 4) + 0x14), tag.a);
    gabi::call(olv_UploadParam_SetTopicTag, param.a, ld(tag.a));
    gabi::Local<be<u16>[0x40]> key;
    gabi::call(0x0203C05C, key.a, 0x10005E7C);
    gabi::call(olv_UploadParam_SetSearchKey, param.a, key.a, 0u);
    gabi::call(olv_UploadPostParam_SetBodyTextMaxLength, param.a, 0x64u);
    gabi::call(olv_UploadPostParam_SetFlags, param.a, 0u);
    osr(0x10005E5C);
    gabi::Local<be<u32>[1]> res;
    u32 r = gabi::call<u32>(olv_UploadPostDataByPostApp, param.a);
    st(res.a, r);
    if (r & 0x80000000u) {
        stb(POST_OK, 0);
        stb(POST_DONE, 1);
        gabi::call(0x0203BDF8, self, res.a, 1u);
        sysui_a(1);
        gabi::call(olv_UploadParamBase_dt, param.a, 0u);
        return;
    }
    stb(POST_DONE, 0);
    stb(POST_PENDING, 1);
    gabi::call(olv_UploadParamBase_dt, param.a, 0u);
}
VERIFY(0x0203C118, OpMgr_post);

/* 0203C42C: request the download job (idle state 1 -> 3) */
static u32 OpMgr_requestDownload(u32 self) {
    WWHD_FUNC(0x0203C42C, u32, self);
    if (ld(self + 0x38) != 1) return 0;
    if (thread_trysend(self, 3) == 0) return 0;
    st(self + 0x38, 3);
    return 1;
}
VERIFY(0x0203C42C, OpMgr_requestDownload);

/* 0203C4A8: request an empathy ("Yeah!") for POSTID (state 4) */
static void OpMgr_requestEmpathy(u32 self, u32 postId) {
    WWHD_FUNC(0x0203C4A8, void, self, postId);
    st(self + 0x80, postId);
    st(self + 0x38, 4);
    stb(self + 0x10, 0);
    osr(0x10005E98);
    if (thread_trysend(self, 4) != 0) {
        stb(self + 0x10, 1);
        osr(0x10005E88);
        return;
    }
    osr(0x10005EA4);
    stb(self + 0x10, 0);
}
VERIFY(0x0203C4A8, OpMgr_requestEmpathy);

/* portal result handling shared by the two StartPortalApp users */
static inline void portal_result(u32 self, u32 r, u32 resloc) {
    st(resloc, r);
    if (r & 0x80000000u) {
        stb(POST_OK, 0);
        stb(POST_DONE, 1);
        gabi::call(0x0203BDF8, self, resloc, 1u);
        return;
    }
    stb(POST_DONE, 1);
    stb(POST_OK, 1);
}

/* 0203C570: open the Miiverse portal on a post */
static void OpMgr_openPost(u32 self, u32 postId) {
    WWHD_FUNC(0x0203C570, void, self, postId);
    if (postId == 0) return;
    gabi::Local<be<u32>[0x400]> prm; /* StartPortalAppParam (0x1000) */
    gabi::Local<be<u32>[1]> res;
    gabi::call(olv_PortalParam_ct, prm.a);
    gabi::call(olv_PortalParam_SetPostId, prm.a, postId);
    u32 r = gabi::call<u32>(olv_StartPortalApp, prm.a);
    portal_result(self, r, res.a);
}
VERIFY(0x0203C570, OpMgr_openPost);

/* 0203C61C: open the Miiverse portal on the current user */
static void OpMgr_openUser(u32 self) {
    WWHD_FUNC(0x0203C61C, void, self);
    gabi::Local<be<u32>[0x400]> prm;
    gabi::Local<be<u32>[1]> res;
    gabi::call(olv_PortalParam_ct, prm.a);
    u32 pid = gabi::call<u32>(act_GetPrincipalId);
    if (pid == 0) return;
    gabi::call(olv_PortalParam_SetUserPid, prm.a, pid);
    u32 r = gabi::call<u32>(olv_StartPortalApp, prm.a);
    portal_result(self, r, res.a);
}
VERIFY(0x0203C61C, OpMgr_openUser);

/* 0203C6BC: ProcUI callback after the post applet returns */
static u32 OpMgr_onPostAppReturn() {
    WWHD_FUNC(0x0203C6BC, u32);
    u32 r = gabi::call<u32>(olv_GetResultByPostApp);
    u32 ok = (r >> 31) ^ 1;
    stb(POST_DONE, 1);
    u32 pending = lbz(POST_PENDING);
    stb(POST_OK, ok);
    if (pending != 0) {
        sysui_a(1);
        sysui_b(1);
        if (lbz(POST_OK) != 0) {
            u32 album = gabi::call<u32>(0x027201DC, ld(0x101F84DC) + 0x12C0);
            gabi::call(0x02726DAC, album, 1u);
        }
    }
    u32 g = ld(0x101F5088);
    stb(POST_PENDING, 0);
    if (gabi::call<u32>(0x02617AE4, g) != 0) gabi::call(0x02618720, ld(0x101F5088), 0u);
    return 0;
}
VERIFY(0x0203C6BC, OpMgr_onPostAppReturn);

/* 0203C780: abort a pending empathy request (state 4) */
static void OpMgr_abortEmpathy(u32 self) {
    WWHD_FUNC(0x0203C780, void, self);
    if (ld(self + 0x38) != 4) return;
    if (lbz(self + 0x10) == 0) {
        osr(0x10005EC8);
        gabi::Local<be<u32>[1]> res;
        st(res.a, 0xFFFFFFFF);
        st(self + 0x84, 0x118D5D);
        gabi::call(0x0203BDF8, self, res.a, 1u);
        st(self + 0x84, 0);
        stb(POST_OK, 0);
        st(self + 0x38, 1);
        osr(0x10005EFC);
    }
    gabi::call(olv_Cancel);
    osr(0x10005EE0);
}
VERIFY(0x0203C780, OpMgr_abortEmpathy);

/* 0203C830..0203CA04: curl memory callbacks on the curl heap (1018F508) */
static u32 Curl_malloc(u32 size) {
    WWHD_FUNC(0x0203C830, u32, size);
    u32 h = ld(0x1018F508);
    return gabi::call_ptr<u32>(v0c(h, 0x34), h, size, 0x40u);
}
VERIFY(0x0203C830, Curl_malloc);

static u32 Curl_free(u32 p) {
    WWHD_FUNC(0x0203C850, u32, p);
    u32 h = ld(0x1018F508);
    return gabi::call_ptr<u32>(v0c(h, 0x3C), h, p);
}
VERIFY(0x0203C850, Curl_free);

static u32 Curl_realloc(u32 p, u32 size) {
    WWHD_FUNC(0x0203C86C, u32, p, size);
    if (p != 0) {
        if (size == 0) {
            gabi::call(0x0203C850, p);
            return 0;
        }
        u32 old = gabi::call<u32>(0x02754814, ld(0x1018F508), p);
        if (old != 0) {
            u32 q = gabi::call<u32>(0x0203C830, size);
            if (q != 0) {
                gabi::call(OSBlockMove_, q, p, size < old ? size : old, 0u);
                gabi::call(0x0203C850, p);
            }
            return q;
        }
    }
    return gabi::call<u32>(0x0203C830, size);
}
VERIFY(0x0203C86C, Curl_realloc);

static u32 Curl_strdup(u32 s) {
    WWHD_FUNC(0x0203C964, u32, s);
    if (s == 0) return 0;
    u32 e = s;
    while (lbz(e) != 0) e++;
    u32 len = e - s + 1;
    u32 q = gabi::call<u32>(0x0203C830, len);
    gabi::call(OSBlockMove_, q, s, len, 0u);
    stb(q + len - 1, 0);
    return q;
}
VERIFY(0x0203C964, Curl_strdup);

static u32 Curl_calloc(u32 n, u32 sz) {
    WWHD_FUNC(0x0203CA04, u32, n, sz);
    if (n == 0) n = 1;
    if (sz == 0) sz = 1;
    u32 t = n * sz;
    u32 q = gabi::call<u32>(0x0203C830, t);
    if (q != 0) gabi::call(OSBlockSet_, q, 0u, t);
    return q;
}
VERIFY(0x0203CA04, Curl_calloc);

/* 0203CA70: network / Miiverse initialisation on the thread (state 2 -> 1, or back to 2 on error) */
static void OpMgr_initNetwork(u32 self) {
    WWHD_FUNC(0x0203CA70, void, self);
    if (ld(self + 0x38) != 2) return;
    gabi::Local<be<u32>[1]> res;
    st(res.a, 0xFFFFFFFF);
    u32 r = gabi::call<u32>(ac_Initialize);
    st(res.a, r);
    if (r & 0x80000000u) {
        gabi::call(ac_GetLastErrorCode, self + 0x84);
        u32 w = ld(res.a);
        osr1(0x10005F70, w & (((w >> 27) & 3) == 3 ? 0x3FFu : 0xFFFFFu));
        st(self + 0x38, 2);
        return;
    }
    stb(self + 0x8C, 1);
    r = gabi::call<u32>(ac_Connect);
    st(res.a, r);
    if (r & 0x80000000u) {
        gabi::call(ac_GetLastErrorCode, self + 0x84);
        u32 w = ld(res.a);
        osr1(0x10005F1C, w & (((w >> 27) & 3) == 3 ? 0x3FFu : 0xFFFFFu));
        st(self + 0x38, 2);
        return;
    }
    stb(self + 0x8B, 1);
    osr(0x10005F9C);
    s32 sr = (s32)gabi::call<u32>(socket_lib_init);
    if (sr < 0) {
        st(self + 0x84, 0xFDE7F);
        osr1(0x10005FBC, (u32)sr);
        st(self + 0x38, 2);
        return;
    }
    stb(self + 0x8A, 1);
    osr(0x10005FE0);
    u32 nr = gabi::call<u32>(NSSLInit);
    if (nr != 0) {
        st(self + 0x84, 0xFDE7F);
        osr1(0x10005F44, nr);
        st(self + 0x38, 2);
        return;
    }
    stb(self + 0x89, 1);
    if (gabi::call<u32>(curl_global_init_mem, 3u, 0x0203C830, 0x0203C850, 0x0203C86C, 0x0203C964, 0x0203CA04) != 0) {
        st(self + 0x84, 0xFDE7F);
        st(self + 0x38, 2);
        return;
    }
    stb(self + 0x88, 1);
    gabi::Local<be<u32>[0x11]> ip; /* nn::olv::InitializeParam */
    gabi::call(olv_InitializeParam_ct, ip.a);
    gabi::call(OSBlockSet_, ld(self + 0x18), 0u, ld(self + 0x1C));
    r = gabi::call<u32>(olv_InitializeParam_SetWork, ip.a, ld(self + 0x18), ld(self + 0x1C));
    st(res.a, r);
    if (r & 0x80000000u) {
        st(self + 0x38, 2);
        return;
    }
    r = gabi::call<u32>(olv_Initialize, ip.a);
    st(res.a, r);
    if (r & 0x80000000u) {
        st(self + 0x84, gabi::call<u32>(olv_GetErrorCode, res.a));
        if (gabi::call<u32>(olv_IsInitialized) != 0) gabi::call(olv_Finalize);
        st(self + 0x38, 2);
        return;
    }
    gabi::call(olv_SetReportTypes, 0x800u);
    gabi::call(ProcUIRegisterCallback, 0u, 0x0203C6BC, 0u, 1u);
    osr(0x10006000);
    Pair32 now = gabi::call<Pair32>(OSGetSystemTime);
    st(self + 0x78, now.r3);
    st(self + 0x84, 0);
    st(self + 0x7C, now.r4);
    st(self + 0x38, 1);
}
VERIFY(0x0203CA70, OpMgr_initNetwork);

/* 0203CD18: send the requested empathy (thread side) */
static void OpMgr_sendEmpathy(u32 self) {
    WWHD_FUNC(0x0203CD18, void, self);
    osr(0x1000603C);
    if (ld(self + 0x80) == 0) return;
    gabi::call(olv_Cancel);
    gabi::Local<be<u32>[0x12]> prm; /* UploadEmpathyToPostDataParam */
    gabi::Local<be<u32>[1]> res;
    gabi::call(olv_EmpathyParam_ct, prm.a);
    gabi::call(olv_EmpathyParam_SetPostId, prm.a, ld(self + 0x80));
    osr(0x1000605C);
    u32 r = gabi::call<u32>(olv_UploadEmpathyToPostData, prm.a);
    st(res.a, r);
    osr(0x10006014);
    if (ld(res.a) & 0x80000000u) {
        osr(0x10006024);
        stb(POST_OK, 0);
        gabi::call(0x0203BDF8, self, res.a, 1u);
        osr(0x1000606C);
        return;
    }
    osr(0x10006030);
    stb(POST_OK, 1);
}
VERIFY(0x0203CD18, OpMgr_sendEmpathy);

/* 0203DDC0: periodic update: idle for 300 s -> refresh the downloads (state 3); retry a pending
 * empathy request */
static void OpMgr_update(u32 self) {
    WWHD_FUNC(0x0203DDC0, void, self);
    u32 s = ld(self + 0x38);
    if (s == 1) {
        Pair32 el = gabi::call<Pair32>(0x02760E58, self + 0x78);
        Pair32 sec = gabi::call<Pair32>(0x028F5B8C, el.r3, el.r4, ld(0x104A11C8), ld(0x104A11CC));
        if ((s32)sec.r3 < 0) return;
        if ((s32)sec.r3 == 0 && sec.r4 < 0x12C) return;
        if (thread_trysend(self, 3) == 0) return;
        st(self + 0x38, 3);
        Pair32 now = gabi::call<Pair32>(OSGetSystemTime);
        st(self + 0x78, now.r3);
        st(self + 0x7C, now.r4);
        return;
    }
    if (s != 4) return;
    if (lbz(self + 0x10) != 0) return;
    if (thread_trysend(self, 4) != 0) {
        stb(self + 0x10, 1);
        osr(0x1000619C);
        return;
    }
    osr(0x100061B0);
}
VERIFY(0x0203DDC0, OpMgr_update);

/* 0203DEEC: worker thread message handler (sead::DelegateThread callback) */
static void OpMgr_threadProc(u32 self, u32 thread, u32 msg) {
    WWHD_FUNC(0x0203DEEC, void, self, thread, msg);
    u32 cs = self + 0x3C;
    if (msg == 1) {
        gabi::call(0x027601BC, cs);
        gabi::call(0x0203CA70, self);
        gabi::call(0x027601F0, cs);
    } else if (msg == 3) {
        gabi::call(0x027601BC, cs);
        gabi::call(0x0203DA00, self);
        if (ld(self + 0x38) != 4) st(self + 0x38, 1);
        gabi::call(0x027601F0, cs);
    } else if (msg == 4) {
        gabi::call(0x027601BC, cs);
        gabi::call(0x0203CD18, self);
        st(self + 0x38, 1);
        gabi::call(0x027601F0, cs);
    } else if (msg == 5) {
        gabi::call(0x0203DDC0, self);
    }
    thread_send(self, 5);
}
VERIFY(0x0203DEEC, OpMgr_threadProc);

/* 0203E050: the singleton disposer's destructor */
static void OpMgr_disposer_dt(u32 p, u32 flags) {
    WWHD_FUNC(0x0203E050, void, p, flags);
    if (p == 0) return;
    st(p + 0xC, 0x100061D0);
    if (p == ld(0x1018F50C)) {
        u32 inst = ld(0x1018F504);
        st(0x1018F50C, 0);
        gabi::call_ptr<u32>(vfn(inst, 0x90, 0xC), inst, 2u);
        st(0x1018F504, 0);
    }
    gabi::call(0x02752BEC, p, 0u);
    if (flags & 1) op_delete(p);
}
VERIFY(0x0203E050, OpMgr_disposer_dt);

/* 0203E0F8: __sinit: header statics, the 50 downloaded posts, the topic and the list parameter */
static void sinit_0203E0F8() {
    WWHD_FUNC(0x0203E0F8, void);
    header_sinit(0x10200C8C, 0x1018F4D4, 0x100061C4);
    gabi::call(0x028EFFD0, POSTS, 0x32u, 0xC208u, 0xC0004F18); /* nn::olv::DownloadedPostData::DownloadedPostData */
    gabi::call(0x028F026C, 0x1018F4F8);
    gabi::call(olv_TopicData_ct, TOPIC);
    gabi::call(olv_ListParam_ct, LISTPARAM);
}
VERIFY(0x0203E0F8, sinit_0203E0F8);

/* the comment manager's tables: 10 bottle entries (0x3EC) at +0x274C (current) and +0x14 (other), 20
 * texture slots (0x10: ?, buffer, image buffer, in-use) at +0x4E84 */
static constexpr u32 CM_ENTRIES = 0x274C, CM_ENTRIES2 = 0x14, CM_TEX = 0x4E84;

/* 0203CE24: update the saved comment counts of the known posts from a fresh post list; flags new
 * comments in the comment manager */
static void OpMgr_refreshCommentCounts(u32 self) {
    WWHD_FUNC(0x0203CE24, void, self);
    if (lbz(ld(COMMENTMGR) + 0x4FD5) != 0) return;
    u32 store = ld(0x101F84DC) + 0xF026C0; /* saved post ids / comment counts */
    gabi::Local<be<u32>[1]> res;
    gabi::Local<be<u32>[1]> cnt;
    u32 changed = 0;
    st(res.a, 0xFFFFFFFF);
    gabi::call(olv_ListParam_SetPostDataMaxNum, LISTPARAM, 0x32u);
    gabi::call(olv_ListParam_SetFlags, LISTPARAM, 0x104u);
    u32 s = ld(self + 0x38);
    if (s == 4 || s == 6) return;
    u32 r = gabi::call<u32>(olv_DownloadPostDataList, TOPIC, POSTS, cnt.a, 0x32u, LISTPARAM);
    st(res.a, r);
    if (r & 0x80000000u) {
        gabi::call(0x0203BDF8, self, res.a, 0u);
        return;
    }
    /* FixedSafeString<0x20> {buffer, vtable, capacity} + buffer */
    struct fixed_l { be<u32> h[3]; u8 b[0x20]; };
    gabi::Local<fixed_l> id;
    const u32 buf = id.a + 0xC;
    u32 post = POSTS;
    for (u32 i = 0; i < ld(cnt.a); i++, post += 0xC208) {
        for (u32 j = 0; j < 0x32; j++) {
            stb(buf + 0, 0);
            st(id.a + 8, 0x20);
            stb(buf + 0x1F, 0);
            st(id.a + 0, buf);
            st(id.a + 4, 0x10005D50);
            gabi::call(0x02725578, store, id.a, j & 0xFFFF);
            u32 cs = (s32)ld(id.a + 8) - 1 >= 0 ? ld(id.a) : 0x10000160;
            u32 pid = gabi::call<u32>(olv_Data_GetPostId, post);
            u32 a, b;
            u32 k = 0;
            do {
                a = lbz(cs + k);
                b = lbz(pid + k);
                k++;
            } while (a == b && a != 0);
            if (a != b) continue;
            u32 saved = gabi::call<u32>(0x027255C4, store, j & 0xFFFF);
            if (saved < gabi::call<u32>(olv_Post_GetCommentCount, post)) changed = 1;
        }
    }
    post = POSTS;
    for (u32 k = 0; k < ld(cnt.a) && (s32)k < 0x32; k++, post += 0xC208) {
        st(id.a + 8, 0x20);
        st(id.a + 0, buf);
        st(id.a + 4, 0x10005D50);
        stb(buf + 0x1F, 0);
        stb(buf + 0, 0);
        u32 pid = gabi::call<u32>(olv_Data_GetPostId, post);
        gabi::call(0x027255A4, store, pid, k & 0xFFFF);
        u32 cc = gabi::call<u32>(olv_Post_GetCommentCount, post);
        gabi::call(0x027255E4, store, k & 0xFFFF, cc & 0xFFFF);
    }
    u32 cm = ld(COMMENTMGR);
    stb(cm + 0x4FD5, changed);
    if (changed) stb(cm + 0x4FD6, 0);
    gabi::call(0x0272294C, ld(0x101F852C));
}
VERIFY(0x0203CE24, OpMgr_refreshCommentCounts);

/* clear one bottle entry */
static inline void clear_entry(u32 e) {
    sth(ld(e + 0), 0);
    sth(ld(e + 0x24), 0);
    sth(ld(e + 0x98), 0);
    stb(e + 0x24D, 0);
    stb(e + 0x24C, 0);
    st(e + 0x2A4, 0);
    st(e + 0x26C, 0);
    st(e + 0x34C, 0);
    st(e + 0x268, 0);
    stb(e + 0x258, 0);
    stb(e + 0x3E4, 0xFF);
    stb(e + 0x259, 0);
    st(e + 0x270, 0);
    st(e + 0x250, 0);
    stb(e + 0x25C, 0);
    st(e + 0x254, 0);
    st(e + 0x260, 0);
    stb(e + 0x25B, 0);
    stb(e + 0x264, 0);
    stb(e + 0x25A, 0);
    gabi::call(OSBlockSet_, e + 0x3C4, 0u, 0x20u);
}

/* 0203D0B0: release the texture slots in state 4 and clear the bottle entries whose texture slot
 * became free */
static void OpMgr_resetEntries(u32 self) {
    WWHD_FUNC(0x0203D0B0, void, self);
    u32 cm = ld(COMMENTMGR);
    u32 tex = cm + CM_TEX;
    for (u32 i = 0; i < 0x14; i++) {
        u32 t = tex + i * 0x10;
        if (ld(t + 0xC) == 4) {
            if (!(i < 0x14)) t = tex;
            st(t + 0xC, 0);
        }
    }
    u32 base = cm + CM_ENTRIES;
    for (u32 k = 0; k < 10; k++) {
        u32 e = k < 10 ? base + k * 0x3EC : base;
        s32 b = (s8)lbz(e + 0x3E4);
        if (b < 0) continue;
        u32 t = (u32)b < 0x14 ? tex + (u32)b * 0x10 : tex;
        if (ld(t + 0xC) != 0) continue;
        clear_entry(k < 10 ? base + k * 0x3EC : base);
    }
}
VERIFY(0x0203D0B0, OpMgr_resetEntries);

static inline u32 str_eq(u32 a, u32 b) {
    u32 x, y;
    do {
        x = lbz(a++);
        y = lbz(b++);
    } while (x == y && x != 0);
    return x == y;
}

/* 0203D284: is POSTID already one of the current or the other bottle entries? */
static u32 OpMgr_isKnownPost(u32 self, u32 postId) {
    WWHD_FUNC(0x0203D284, u32, self, postId);
    for (u32 k = 0; k < 10; k++) {
        u32 e = ld(COMMENTMGR) + CM_ENTRIES + k * 0x3EC;
        if (str_eq(e + 0x3C4, postId)) return 1;
    }
    for (u32 k = 0; k < 10; k++) {
        u32 e = ld(COMMENTMGR) + CM_ENTRIES2 + k * 0x3EC;
        if (str_eq(e + 0x3C4, postId)) return 1;
    }
    return 0;
}
VERIFY(0x0203D284, OpMgr_isKnownPost);

/* 0203D394: first free texture slot, or -1 */
static s32 OpMgr_findFreeTex(u32 self) {
    WWHD_FUNC(0x0203D394, s32, self);
    u32 t = ld(COMMENTMGR) + CM_TEX;
    for (s32 i = 0; i < 0x14; i++, t += 0x10)
        if (ld(t + 0xC) == 0) return (s8)i;
    return -1;
}
VERIFY(0x0203D394, OpMgr_findFreeTex);

/* copy a wide SafeString into the WFixedSafeString at E (buffer +0, vtable +4, capacity +8) with at
 * most MAXN characters; sets the terminator */
static inline void wcopy(u32 e, u32 srcLocal, u32 maxn, u32 termOff) {
    s32 cap = (s32)ld(e + 8);
    u32 n = maxn;
    u32 dst = ld(e + 0);
    if (!(cap > (s32)maxn)) n = cap - 1;
    gabi::call(0x0203E250, srcLocal);
    gabi::call(OSBlockMove_, dst, ld(srcLocal), n * 2, 0u);
    sth(dst + n * 2, 0);
    if (!((s32)ld(e + 8) > (s32)maxn)) {
        gabi::call_ptr<u32>(ld(ld(e + 4) + 0x14), e);
        /* (sead's capped strlen of the result follows; its value is unused) */
    } else {
        sth(ld(e + 0) + termOff, 0);
    }
}

/* 0203D3D4: store a downloaded post into bottle entry SLOT (texture slot, name, date, topic, body
 * text or handwritten memo texture, counts, flags, screenshot and app data); FLAG: from the "from
 * game" list. Returns 1 when the entry is complete. */
static u32 OpMgr_storePost(u32 self, u32 slot, u32 post, u32 flag) {
    WWHD_FUNC(0x0203D3D4, u32, self, slot, post, flag);
    u32 me = gabi::call<u32>(act_GetPrincipalId);
    if (me == gabi::call<u32>(olv_Data_GetUserPid, post)) return 0;
    s32 ti = (s32)gabi::call<u32>(0x0203D394, self);
    if (ti < 0) {
        osr(0x1000608C);
        return 0;
    }
    u32 s = ld(self + 0x38);
    if (s == 4 || s == 6) return 0;
    u32 cm = ld(COMMENTMGR);
    u32 e = cm + CM_ENTRIES;
    if (slot < 10) e += slot * 0x3EC;
    u32 tex = cm + CM_TEX;
    if ((u32)ti < 0x14) tex += (u32)ti * 0x10;
    gabi::call(OSBlockMove_, e + 0x3C4, gabi::call<u32>(olv_Data_GetPostId, post), 0x20u, 0u);
    u32 heap = gabi::call<u32>(0x0203E90C);
    gabi::Local<be<u32>[2]> ws;
    gabi::Local<be<u32>[2]> ws2;
    gabi::Local<be<u32>[1]> memoSz;
    gabi::Local<be<u32>[1]> imgSz;
    gabi::Local<be<u32>[1]> appSz;
    gabi::Local<be<u32>[4]> app;
    gabi::Local<be<u32>[2]> l1c;
    gabi::Local<be<u32>[1]> wh;
    gabi::Local<be<u32>[10]> cal;
    gabi::Local<be<u16>[0x66]> body;
    Pair32 date = gabi::call<Pair32>(olv_Data_GetPostDate, post);
    gabi::call(OSTicksToCalendarTime, date.r3, date.r4, cal.a);
    st(ws.a + 0, gabi::call<u32>(olv_Data_GetMiiNickname, post));
    st(ws.a + 4, 0x10005D20);
    wcopy(e, ws.a, 10, 0x14);
    st(ws.a + 0, gabi::call<u32>(olv_Data_GetTopicTag, post));
    st(ws.a + 4, 0x10005D20);
    gabi::call(0x02039240, e, ws.a);
    if (gabi::call<u32>(olv_Data_TestFlags, post, 1u) != 0) {
        if (gabi::call<u32>(olv_Data_GetBodyText, post, body.a, 0x65u) & 0x80000000u) return 0;
        st(ws2.a + 4, 0x10005D20);
        st(ws2.a + 0, body.a);
        wcopy(e + 0x98, ws2.a, 200, 0x190);
    }
    stb(e + 0x24D, gabi::call<u32>(olv_Data_TestFlags, post, 0x80u));
    st(e + 0x250, gabi::call<u32>(olv_Post_GetEmpathyCount, post));
    u32 cc = gabi::call<u32>(olv_Post_GetCommentCount, post);
    stb(e + 0x25A, 0);
    st(e + 0x254, cc);
    stb(e + 0x258, flag);
    u32 spo = gabi::call<u32>(olv_Data_TestFlags, post, 0x200u);
    stb(e + 0x3E5, spo);
    stb(e + 0x3E4, (u32)ti);
    bool hasImage;
    if (gabi::call<u32>(olv_Data_TestFlags, post, 2u) != 0) {
        /* handwritten memo: decode the TGA into a texture */
        st(memoSz.a, 0);
        if (gabi::call<u32>(olv_Data_GetBodyMemo, post, ld(tex + 4), memoSz.a, 0x2582Cu) & 0x80000000u) {
            stb(e + 0x24C, 0);
            return 0;
        }
        stb(e + 0x24C, 1);
        u32 img = gabi::call<u32>(0x0273B050, 0x90u, heap, 4u);
        if (img != 0) img = gabi::call<u32>(0x027BE6B8, img);
        gabi::call(0x027B69A8, img, ld(tex + 4), 0x28000u, heap);
        u32 gt = e + 0x328; /* GX2Texture */
        gabi::call(OSBlockMove_, gt, img + 4, 0x74u, 0u);
        u32 a338 = ld(e + 0x338);
        st(e + 0x39C, 0);
        st(e + 0x3AC, 0x10203);
        u32 a334 = ld(e + 0x334);
        st(e + 0x3A4, 0);
        st(e + 0x3A8, a334);
        st(e + 0x3A0, a338);
        gabi::call(GX2InitTextureRegs, gt);
        gabi::call(0x0287F1EC, l1c.a, 0x1Au, 1u);
        s32 w = (s32)ld(img + 8);
        s32 mw = (s32)lbz(img + 0x78);
        if (w > mw) mw = w;
        s32 h = (s32)ld(img + 0xC);
        s32 mh = (s32)lbz(img + 0x79);
        if (h > mh) mh = h;
        u32 fmt = ld(l1c.a);
        st(e + 0x31C, 1);
        st(wh.a, ((u32)mw << 16) | ((u32)mh & 0xFFFF));
        sth(e + 0x320, lhz(wh.a));
        sth(e + 0x322, lhz(wh.a + 2));
        stb(e + 0x324, fmt);
        gabi::call_ptr<u32>(vfn(img, 0x8C, 0xC), img, 3u);
        hasImage = gabi::call<u32>(olv_Data_TestFlags, post, 4u) != 0;
    } else {
        stb(e + 0x24C, 0);
        hasImage = gabi::call<u32>(olv_Data_TestFlags, post, 4u) != 0;
    }
    if (hasImage) {
        if (gabi::call<u32>(olv_Data_GetExternalImageDataSize, post) > 0x50000) return 0;
        st(imgSz.a, 0);
        st(e + 0x270, ld(tex + 8));
        s = ld(self + 0x38);
        if (s == 4 || s == 6) return 0;
        if (gabi::call<u32>(olv_Data_DownloadExternalImageData, post, ld(tex + 8), imgSz.a, 0x50000u) & 0x80000000u) {
            stb(e + 0x259, 0);
            return 0;
        }
        st(appSz.a, 0);
        if (gabi::call<u32>(olv_Data_GetAppData, post, app.a, appSz.a, 0x10u) & 0x80000000u) {
            stb(e + 0x259, 0);
            return 0;
        }
        while (gabi::call<u32>(0x02036474, ld(0x1018F3F4), ld(e + 0x270), e + 0x274, ld(tex + 0), 8u) == 0) {
        }
        u32 jd = ld(0x1018F3F4);
        if (lbz(jd + 0x18) == 0)
            for (;;) {
            } /* the original spins here forever (the flag is not re-read) */
        if (lbz(jd + 0x19) == 0) {
            stb(e + 0x259, 0);
            return 0;
        }
        stb(e + 0x259, 1);
        stb(e + 0x264, lbz(app.a + 0));
        stb(e + 0x25C, lbz(app.a + 1));
        st(e + 0x260, ld(app.a + 4));
        st(e + 0x26C, ld(app.a + 8));
        st(e + 0x268, ld(app.a + 0xC));
    } else {
        stb(e + 0x259, 0);
    }
    u32 v = lbz(gabi::call<u32>(0x027200F4, ld(0x101F84DC) + 0x12C0));
    bool ok = flag != 0 ? v == 1 : (v != 1 && v != 2);
    if (!ok) {
        osr(0x100060AC);
        return 0;
    }
    gabi::call(0x0203AF50, ld(COMMENTMGR));
    stb(e + 0x25B, 1);
    st(tex + 0xC, 1);
    return 1;
}
VERIFY(0x0203D3D4, OpMgr_storePost);

/* collect up to 10 new posts of the list into the bottle entries */
static inline void collect(u32 self, u32 cnt, u32& n, u32 flag) {
    u32 post = POSTS;
    for (u32 k = 0; k < ld(cnt) && (s32)n < 10; k++, post += 0xC208) {
        u32 pid = gabi::call<u32>(olv_Data_GetPostId, post);
        if (gabi::call<u32>(0x0203D284, self, pid) != 0) continue;
        if (gabi::call<u32>(0x0203D3D4, self, n, post, flag) != 0) n++;
    }
}

/* 0203DA00: download job: (needs the Tingle Bottle, item 0x21) refresh counts, reset entries, then fill
 * the free bottle entries from the "from_game" post list of the current mode */
static void OpMgr_download(u32 self) {
    WWHD_FUNC(0x0203DA00, void, self);
    if (ld(self + 0x38) == 5) {
        osr(0x100060E8);
        return;
    }
    if (gabi::call<u32>(0x0254DA50, 0x21u, 1u) == 0) {
        osr(0x10006108);
        return;
    }
    if (ld(self + 0x38) == 6) return;
    u32 v = lbz(gabi::call<u32>(0x027200F4, ld(0x101F84DC) + 0x12C0));
    if (v != 2) gabi::call(0x0203CE24, self);
    gabi::call(0x0203D0B0, self);
    u32 n = gabi::call<u32>(0x02039B80, ld(COMMENTMGR));
    if ((u32)(9 - n) >= 10) {
        osr(0x1000612C);
        u32 cm = ld(COMMENTMGR);
        gabi::cpu->cr[6] = 0;
        gabi::call(OSReport, 0x10006160, lbz(cm + 0x4CF3), cm + 0x4E5C, (u32)(s32)(s8)lbz(cm + 0x4E7C));
        s32 b = (s8)lbz(cm + 0x4E7C);
        if (b < 0) {
            osr(0x100060C8);
            return;
        }
        u32 t = cm + CM_TEX;
        if ((u32)b < 0x14) t += (u32)b * 0x10;
        osr1(0x1000618C, ld(t + 0xC));
        return;
    }
    gabi::Local<be<u32>[1]> cnt;
    gabi::Local<be<u32>[1]> res;
    gabi::Local<be<u16>[0x40]> key;
    st(res.a, 0xFFFFFFFF);
    gabi::call(0x0203C05C, key.a, 0x100060DC);
    if (v != 1 && v != 0) return;
    gabi::call(olv_ListParam_SetSearchKey, LISTPARAM, key.a, 0u);
    gabi::call(olv_ListParam_SetPostDataMaxNum, LISTPARAM, 0x32u);
    u32 fl = v == 1 ? 0x81 : 0x80;
    if (lbz(gabi::call<u32>(0x027200F4, ld(0x101F84DC) + 0x12C0) + 1) == 0) fl |= 0x100;
    gabi::call(olv_ListParam_SetFlags, LISTPARAM, fl);
    u32 s = ld(self + 0x38);
    if (s == 4 || s == 6) return;
    u32 r = gabi::call<u32>(olv_DownloadPostDataList, TOPIC, POSTS, cnt.a, 0x32u, LISTPARAM);
    st(res.a, r);
    if (r & 0x80000000u) {
        gabi::call(0x0203BDF8, self, res.a, 0u);
        return;
    }
    collect(self, cnt.a, n, v == 1 ? 1 : 0);
}
VERIFY(0x0203DA00, OpMgr_download);

}  // namespace hd_olv_0203B620
