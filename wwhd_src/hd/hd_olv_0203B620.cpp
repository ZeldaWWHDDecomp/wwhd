/**
 * HD Miiverse operation manager: network setup, bottle posts, empathy and downloads.
 * No GameCube counterpart. Native ABI, call order and guest stack reservations are retained.
 * Verified against the original by tools/verify (hd_olv_0203B620). Private game-derived source.
 */
#include "hd/hd_olv_0203B620.h"

using namespace hd_olv_0203B620;
static OperationHeaderValues sHeaderValues;
static OperationSingletonData sSingleton;
static OperationTuning sTuning = {2.0f, 40.0f, 0.0f};
static OperationGlobalPointer sErrorViewer;
static OperationGlobalPointer sComments;
static OperationGlobalPointer sSave;
static OperationGlobalPointer sMenu;
static OperationGlobalPointer sSaveSystem;
static OperationGlobalPointer sJpegJob;
static OperationGlobalPointer sThreadPriority;
static OperationTick sTickRate;
static OperationTopicData sTopic;
static OperationListParam sListParam;
static OperationPostArray sPosts;
static OperationAddressLabel sWideStringVtable;
static OperationAddressLabel sFixedStringVtable;
static OperationAddressLabel sWideBufferVtable;
static OperationAddressLabel sFixedWideStringVtable;
static OperationAddressLabel sOperationDelegateVtable;
static OperationInitConstants sHeaderConstants;
static OperationAddressLabel sDisposerVtable;
static OperationAddressLabel sOperationVtable;
static OperationAddressLabel sWorkerVtable;
static OperationDestructorRecords sHeaderDestructors;
static OperationAddressLabel sPostArrayDestructor;
static OperationHeader sHeaderWords;
static OperationAddressLabel sSafeStringVtable;
static OperationAddressLabel sEmptyWideString;

static void destroyWorker(u32 p, u32 flags);
static void destroySafeStringBase();
static void SafeBuf_assureTerminate(u32 self);
static void deleteSafeString(u32 p, u32 flags);
static void destroyWideStringBase();
static void WSafeBuf_assureTerminate(u32 self);
static void deleteWideString(u32 p, u32 flags);
static void deleteFixedWideString(u32 p, u32 flags);
static u32 createWorker(u32 unused, u32 heap, u32 align);
static void destroyCommentStorage(u32 p, u32 flags);
static f32 getFirstOperationTuning();
static f32 getSecondOperationTuning();
static f32 getThirdOperationTuning();
static u32 OpMgr_ct(u32 self);
static u32 OpMgr_createInstance(u32 heap);
static void OpMgr_dt(u32 p, u32 flags);
static void OpMgr_createThread(u32 self);
static u32 OpMgr_start(u32 self);
static void OpMgr_cancel(u32 self);
static void OpMgr_wake(u32 self);
static u32 isListedError(u32 c);
static void OpMgr_reportError(u32 self, u32 res, u32 show);
static void Utf8ToUtf16(u32 dst, u32 src);
static void OpMgr_post(u32 self, u32 img);
static u32 OpMgr_requestDownload(u32 self);
static void OpMgr_requestEmpathy(u32 self, u32 postId);
static void OpMgr_openPost(u32 self, u32 postId);
static void OpMgr_openUser(u32 self);
static u32 OpMgr_onPostAppReturn();
static void OpMgr_abortEmpathy(u32 self);
static u32 Curl_malloc(u32 size);
static u32 Curl_free(u32 p);
static u32 Curl_realloc(u32 p, u32 size);
static u32 Curl_strdup(u32 s);
static u32 Curl_calloc(u32 n, u32 sz);
static void OpMgr_initNetwork(u32 self);
static void OpMgr_sendEmpathy(u32 self);
static void OpMgr_update(u32 self);
static void OpMgr_threadProc(u32 self, u32 thread, u32 msg);
static void OpMgr_disposer_dt(u32 p, u32 flags);
static void __sinit_olvOperation();
static void OpMgr_refreshCommentCounts(u32 self);
static void OpMgr_resetEntries(u32 self);
static u32 OpMgr_isKnownPost(u32 self, u32 postId);
static s32 OpMgr_findFreeTex(u32 self);
static u32 OpMgr_storePost(u32 self, u32 slot, u32 post, u32 flag);
static void OpMgr_download(u32 self);

static inline void sstr(u32 a, u32 s) {
    auto* text = gabi::at<OliveSafeString>(a);
    text->__vtbl = gabi::ea(&sSafeStringVtable);
    text->mpData.v = s;
}
/* OSReport (varargs, crclr cr1eq): only the arguments the format uses are passed */
static inline void osr(u32 fmt) {
    gabi::cpu->cr[6] = 0;
    bottleOp_OSReport(fmt);
}
static inline void osr1(u32 fmt, u32 a4) {
    gabi::cpu->cr[6] = 0;
    bottleOp_OSReport(fmt, a4);
}
static inline void osr(u32 fmt, u32 a4, u32 a5) {
    gabi::cpu->cr[6] = 0;
    bottleOp_OSReport(fmt, a4, a5);
}

/* 0203B6F8 */
static u32 createWorker(u32 unused, u32 heap, u32 align) {
    u32 p = bottleOp_allocateInHeap(0x24u, heap, align);
    if (p == 0)
        return 0;
    return bottleOp_constructWorker(p);
}

/* deleting destructor of a holder of two 10-element arrays (0x3EC each, element dtor
 * 02038FF0) at +0x274C and +0x14 */

/* 0203B7B4 */
static f32 getFirstOperationTuning() {
    return sTuning.mFirst;
}

/* 0203B7C0 */
static f32 getSecondOperationTuning() {
    return sTuning.mSecond;
}

/* 0203B7CC */
static f32 getThirdOperationTuning() {
    return sTuning.mThird;
}

/* 0203B7D8 */
static u32 OpMgr_ct(u32 self) {
    u32 t = self;
    if (t == 0) {
        t = bottleOp_allocateObject(0x98);
        if (t == 0)
            return 0;
    }
    manager(t)->mWorkBuffer = 0;
    manager(t)->__vtbl = gabi::ea(&sOperationVtable);
    manager(t)->mWorkSize = 0;
    manager(t)->mWorkHeap = 0;
    manager(t)->mCurlHeap = 0;
    manager(t)->mEmpathyQueued = 0;
    bottleOp_zeroMemory(gabi::ea(&manager(t)->mAppData), 0x10u);
    manager(t)->mState = STATE_STOPPED;
    manager(t)->mThread = 0;
    bottleOp_constructCriticalSection(gabi::ea(&manager(t)->mCriticalSection));
    bottleOp_constructTick(gabi::ea(&manager(t)->mStartTick));
    manager(t)->mCurlInitialized = 0;
    manager(t)->mAcInitialized = 0;
    manager(t)->mErrorOverride = 0xFDE7F;
    manager(t)->mPostID = 0;
    manager(t)->mAcConnected = 0;
    manager(t)->mNsslInitialized = 0;
    manager(t)->mSocketsInitialized = 0;
    return t;
}

/* 0203B88C */
static u32 OpMgr_createInstance(u32 heap) {
    u32 cur = sSingleton.mInstance;
    if (cur != 0)
        return cur;
    u32 d = bottleOp_allocateAlignedInHeap(0x98u, heap, 4u);
    if (d != 0) {
        bottleOp_constructDisposer(d, heap, 3u);
        gabi::at<OliveDisposer>(d)->__vtbl = gabi::ea(&sDisposerVtable);
    }
    sSingleton.mDisposer = d;
    u32 r = d;
    if (d != 0)
        r = OpMgr_ct(d);
    sSingleton.mInstance = r;
    return r;
}

/* destructor */
/* 0203B920 */
static void OpMgr_dt(u32 p, u32 flags) {
    if (p == 0)
        return;
    manager(p)->__vtbl = gabi::ea(&sOperationVtable);
    bottleOp_olv_Finalize(p);
    bottleOp_destroyCriticalSection(gabi::ea(&manager(p)->mCriticalSection), 2u);
    if (flags & 1)
        bottleOp_deleteObject(p);
}

/* 0203B984 */
static void OpMgr_createThread(u32 self) {
    gabi::Local<be<u32>[2]> nm;
    gabi::Local<be<u32>[2]> sa; /* outgoing stack-argument area */
    sstr(nm.a, gabi::guest_addr("OliveOperationMgr"));
    u32 parent = bottleOp_getParentHeap();
    u32 heap = bottleOp_createHeap(0u, nm.a, parent, 1u, 0u);
    u32 th = bottleOp_allocateInHeap(0x98u, heap, 4u);
    if (th != 0) {
        sstr(nm.a, gabi::guest_addr("OliveOperationMgrThread"));
        u32 dg = bottleOp_allocateInHeap(0x10u, heap, 4u);
        if (dg != 0) {
            /* sead::Delegate<OliveOperationMgr>: object, GHS member pointer {0, -1, fn} */
            gabi::at<OperationDelegate>(dg)->mObject = self;
            gabi::at<OperationDelegate>(dg)->mDelta = 0;
            gabi::at<OperationDelegate>(dg)->mFunction =
                gabi::guest_fn((const void*)OpMgr_threadProc);
            gabi::at<OperationDelegate>(dg)->mVtable = gabi::ea(&sOperationDelegateVtable);
            gabi::at<OperationDelegate>(dg)->mVtableIndex = 0xFFFF;
        }
        u32 prio = sThreadPriority.mAddress + 3;
        th = bottleOp_constructDelegateThread(th, nm.a, dg, heap, prio, 0u, 0x7FFFFFFFu, 0x9000u,
                                              0x20u);
    }
    manager(self)->mThread = th;
    hd_olv_0203B620::heap(heap)->adjust();
    u32 t = manager(self)->mThread;
    hd_olv_0203B620::thread(t)->start();
    manager(self)->mState = STATE_STOPPED;
    auto now = bottleOp_OSGetSystemTime();
    manager(self)->mStartTick.mLow = now.r4;
    manager(self)->mStartTick.mHigh = now.r3;
}

/* start: work heap (0x380000) with an aligned block, a 1 MB heap (also published at
 * 1018F508), state 2 and wake the thread; refused when the state is not idle */
/* 0203BBB0 */
static u32 OpMgr_start(u32 self) {
    u32 state = manager(self)->mState;
    if (state != STATE_STOPPED) {
        osr(gabi::guest_addr(
            "\n\n\nOliveOperationMgrThread is Busy\n\n\n")); /* no format arguments (r6 holds the
                                                                state) */
        return 0;
    }
    u32 parent = bottleOp_getParentHeap();
    gabi::Local<be<u32>[2]> nm;
    sstr(nm.a, gabi::guest_addr("OliveHeap"));
    u32 h = bottleOp_createWorkHeap(0x380000u, nm.a, parent, 1u, 0u);
    manager(self)->mWorkHeap = h;
    u32 sz = hd_olv_0203B620::heap(h)->freeSize(0x40u);
    u32 h2 = manager(self)->mWorkHeap;
    manager(self)->mWorkSize = sz;
    manager(self)->mWorkBuffer = hd_olv_0203B620::heap(h2)->allocate(sz, 0x40u);
    sstr(nm.a, gabi::guest_addr("CurlHeap"));
    u32 h3 = bottleOp_createHeap(0x100000u, nm.a, parent, 1u, 0u);
    manager(self)->mCurlHeap = h3;
    sSingleton.mCurlHeap = h3;
    u32 t = manager(self)->mThread;
    manager(self)->mState = STATE_INITIALIZING;
    hd_olv_0203B620::thread(t)->send(1u, 1u);
    return 1;
}

/* cancel: state 6, stop the thread, and when it had initialised (thread state 3/4) shut
 * down nn::olv, curl, NSSL, sockets and nn::ac as far as they were set up */
/* 0203BCF4 */
static void OpMgr_cancel(u32 self) {
    manager(self)->mState = STATE_CANCELLED;
    bottleOp_olv_Cancel();
    u32 t = manager(self)->mThread;
    hd_olv_0203B620::thread(t)->cancel(1u);
    t = manager(self)->mThread;
    hd_olv_0203B620::thread(t)->join();
    u32 ts = hd_olv_0203B620::thread(manager(self)->mThread)->mState;
    if (ts != 3 && ts != 4)
        return;
    if (bottleOp_olv_IsInitialized() != 0) {
        bottleOp_olv_Cancel();
        bottleOp_olv_Finalize();
    }
    if (manager(self)->mCurlInitialized != 0)
        bottleOp_curl_global_cleanup();
    if (manager(self)->mNsslInitialized != 0)
        bottleOp_NSSLFinish();
    if (manager(self)->mSocketsInitialized != 0)
        bottleOp_socket_lib_finish();
    if (manager(self)->mAcConnected != 0)
        bottleOp_ac_Close();
    if (manager(self)->mAcInitialized != 0)
        bottleOp_ac_Finalize();
}

/* wake the thread while the state is 2 */
/* 0203BDD0 */
static void OpMgr_wake(u32 self) {
    if (manager(self)->mState != STATE_INITIALIZING)
        return;
    u32 t = manager(self)->mThread;
    hd_olv_0203B620::thread(t)->send(1u, 1u);
}

/* report a Miiverse error; with SHOW, hand the code (or the override +0x84) to the error
 * viewer (1018F2BC), marked fatal for a fixed list of codes */
static u32 isListedError(u32 c) {
    if (c >= 0xFBB58) {
        if (c >= 0xFC008) {
            if (c <= 0xFC00F)
                return 1;
            return c == 0xFC06B || c == 0x119019 || c == 0x119FB9;
        }
        if (c <= 0xFBB5E)
            return 1;
        if (c < 0xFBBBB)
            return 0;
        if (c <= 0xFBBC2)
            return 1;
        if (c < 0xFBC1F)
            return 0;
        if (c <= 0xFBC27)
            return 1;
        return c == 0xFBC83;
    }
    if (c >= 0xF9A93) {
        if (c == 0xF9A93)
            return 1;
        if (c < 0xF9AA7)
            return 0;
        if (c <= 0xF9AA8)
            return 1;
        if (c < 0xFB964)
            return 0;
        if (c <= 0xFB96B)
            return 1;
        return c == 0xFB9C7;
    }
    return c == 0xF945D || c == 0xF9899 || c == 0xF9A27 || c == 0xF9A2F;
}
/* 0203BDF8 */
static void OpMgr_reportError(u32 self, u32 res, u32 show) {
    u32 code = bottleOp_olv_GetErrorCode(res);
    osr(gabi::guest_addr("----------- ERROR INFO -----------\n"));
    u32 r = *gabi::at<be<u32>>(res);
    u32 mask = ((r >> 27) & 3) == 3 ? 0x3FFu : 0xFFFFFu;
    osr(gabi::guest_addr("description:%d errorCode:%d\n"), r & mask, code);
    if (show == 0)
        return;
    u32 ev = sErrorViewer.mAddress;
    if (ev == 0)
        return;
    u32 ov = manager(self)->mErrorOverride;
    if (ov != 0)
        code = ov;
    bottleOp_showError(ev, code, isListedError(code));
}

// HD: UTF-8 conversion keeps each intermediate output store, including overlapping input/output.
/* 0203C05C */
static void Utf8ToUtf16(u32 dst, u32 src) {
    u32 n = 0;
    u32 c = *gabi::at<be<u8>>(src);
    u32 d = dst;
    while (c != 0) {
        if ((c & 0x80) == 0) {
            src += 1;
            n += 1;
            *gabi::at<be<u16>>(d) = c & 0x7F;
            d += 2;
        } else if ((c & 0xE0) == 0xC0) {
            u32 v = (c << 6) & 0x7C0;
            *gabi::at<be<u16>>(d) = v;
            u32 c1 = *gabi::at<be<u8>>(src + 1);
            n += 1;
            v = (v & ~0x3Fu) | (c1 & 0x3F);
            src += 2;
            *gabi::at<be<u16>>(d) = v;
            d += 2;
        } else if ((c & 0xF0) == 0xE0) {
            u32 v = (c << 12) & 0xF000;
            *gabi::at<be<u16>>(d) = v;
            u32 c1 = *gabi::at<be<u8>>(src + 1);
            v = (v & ~0xFC0u) | ((c1 << 6) & 0xFC0);
            *gabi::at<be<u16>>(d) = v;
            u32 c2 = *gabi::at<be<u8>>(src + 2);
            n += 1;
            v = v | (c2 & 0x3F);
            src += 3;
            *gabi::at<be<u16>>(d) = v;
            d += 2;
        } else {
            break;
        }
        c = *gabi::at<be<u8>>(src);
    }
    *gabi::at<be<u16>>(dst + n * 2) = 0;
}

/* thread-status helpers of the Miiverse system UI hooks (02035D78 instance) */
static inline void sysui_a(u32 v) {
    u32 r = bottleOp_getSystemUI();
    bottleOp_setSystemUIReady(r, v);
}
static inline void sysui_b(u32 v) {
    u32 r = bottleOp_getSystemUI();
    bottleOp_setSystemUIBusy(r, v);
}
static inline void thread_send(u32 self, u32 msg) {
    u32 t = manager(self)->mThread;
    hd_olv_0203B620::thread(t)->send(msg, 1u);
}
static inline u32 thread_trysend(u32 self, u32 msg) {
    u32 t = manager(self)->mThread;
    return hd_olv_0203B620::thread(t)->send(msg, 1u);
}

/* post the bottle message (UploadPostDataByPostApp): optional screenshot IMG of the save's
 * picture album with its app data, topic tag from the comment manager, search key "from_game" */
/* 0203C118 */
static void OpMgr_post(u32 self, u32 img) {
    bottleOp_clearError(sErrorViewer.mAddress);
    sysui_b(0);
    if (manager(self)->mErrorOverride != 0) {
        gabi::Local<be<u32>> res;
        sSingleton.mPostSucceeded = 0;
        (*res) = 0xFFFFFFFF;
        sSingleton.mPostDone = 1;
        OpMgr_reportError(self, res.a, 1u);
        sysui_a(1);
        sysui_b(1);
        thread_send(self, MESSAGE_INITIALIZE);
        return;
    }
    gabi::Local<OperationUploadParam> param; /* UploadPostDataByPostAppParam (0x1400) */
    bottleOp_olv_UploadPostParam_ct(param.a);
    if (img != 0xFFFFFFFF) {
        u32 album =
            bottleOp_getReadAlbum(gabi::ea(&gabi::at<OperationSavedAlbum>(sSave.mAddress)->mData));
        u32 data = bottleOp_getAlbumImage(album, img);
        bottleOp_olv_UploadParam_SetExternalImageData(param.a, data, 0x50000u);
        u32 x = bottleOp_getAlbumData(album, img);
        manager(self)->mAppData.mType = bottleOp_getAlbumDataType(x);
        manager(self)->mAppData.mFlag = bottleOp_getAlbumFlag(album, img);
        manager(self)->mAppData.mID = bottleOp_getAlbumID(album, img);
        x = bottleOp_getAlbumData(album, img);
        manager(self)->mAppData.mSize = bottleOp_getAlbumDataSize(x);
        x = bottleOp_getAlbumData(album, img);
        manager(self)->mAppData.mValue = bottleOp_getAlbumDataValue(x);
        bottleOp_olv_UploadParam_SetAppData(param.a, gabi::ea(&manager(self)->mAppData), 0x10u);
    }
    /* WFixedSafeString<0x33> tag = WSafeString(L"") (copy with sead's capped strlen) */
    gabi::Local<OperationFixedWideString> tag;
    const u32 bufa = tag.a + 0xC;
    const u32 buffer = bufa;
    gabi::Local<be<u32>[2]> src;
    tag->mBuffer = buffer;
    tag->mCapacity = 0x33;
    gabi::at<OliveSafeString>(src.a)->__vtbl = gabi::ea(&sWideStringVtable);
    tag->mVtable = gabi::ea(&sWideBufferVtable);
    *gabi::at<be<u16>>(buffer + 0x64) = 0;
    gabi::at<OliveSafeString>(src.a)->mpData.v = gabi::ea(&sEmptyWideString);
    bottleOp_assureWideTermination(src.a);
    u32 p = gabi::at<OliveSafeString>(src.a)->mpData.v.get();
    s32 n = 0;
    if (*gabi::at<be<u16>>(p) != 0) {
        for (;;) {
            n += 1;
            p += 2;
            if (n > 0x40000) {
                n = 0;
                break;
            }
            if (*gabi::at<be<u16>>(p) == 0)
                break;
        }
    }
    s32 cap = (s32)tag->mCapacity;
    u32 vt = gabi::at<OliveSafeString>(src.a)->__vtbl;
    if (!(n < cap))
        n = cap - 1;
    gabi::call_ptr<u32>(gabi::at<OliveStringVtable>(vt)->mAssureTermination.get(), src.a);
    bottleOp_OSBlockMove_(buffer, gabi::at<OliveSafeString>(src.a)->mpData.v.get(), (u32)n * 2, 0u);
    *gabi::at<be<u16>>(buffer + (u32)n * 2) = 0;
    u32 cm = sComments.mAddress;
    tag->mVtable = gabi::ea(&sFixedWideStringVtable);
    bottleOp_getTopicTag(cm, tag.a);
    gabi::call_ptr<u32>(gabi::at<OliveStringVtable>(tag->mVtable)->mAssureTermination.get(), tag.a);
    bottleOp_olv_UploadParam_SetTopicTag(param.a, tag->mBuffer);
    gabi::Local<be<u16>[0x40]> key;
    Utf8ToUtf16(key.a, gabi::guest_addr("from_game"));
    bottleOp_olv_UploadParam_SetSearchKey(param.a, key.a, 0u);
    bottleOp_olv_UploadPostParam_SetBodyTextMaxLength(param.a, 0x64u);
    bottleOp_olv_UploadPostParam_SetFlags(param.a, 0u);
    osr(gabi::guest_addr("\nUploadPostDataByPostAppParam.\n"));
    gabi::Local<be<u32>> res;
    u32 r = bottleOp_olv_UploadPostDataByPostApp(param.a);
    (*res) = r;
    if (r & 0x80000000u) {
        sSingleton.mPostSucceeded = 0;
        sSingleton.mPostDone = 1;
        OpMgr_reportError(self, res.a, 1u);
        sysui_a(1);
        bottleOp_olv_UploadParamBase_dt(param.a, 0u);
        return;
    }
    sSingleton.mPostDone = 0;
    sSingleton.mPostPending = 1;
    bottleOp_olv_UploadParamBase_dt(param.a, 0u);
}

/* request the download job (idle state 1 -> 3) */
/* 0203C42C */
static u32 OpMgr_requestDownload(u32 self) {
    if (manager(self)->mState != STATE_IDLE)
        return 0;
    if (thread_trysend(self, MESSAGE_DOWNLOAD) == 0)
        return 0;
    manager(self)->mState = STATE_DOWNLOADING;
    return 1;
}

/* request an empathy ("Yeah!") for POSTID (state 4) */
/* 0203C4A8 */
static void OpMgr_requestEmpathy(u32 self, u32 postId) {
    manager(self)->mPostID = postId;
    manager(self)->mState = STATE_EMPATHY;
    manager(self)->mEmpathyQueued = 0;
    osr(gabi::guest_addr("\213\244\212\264\227\\\226\361\201I\n"));
    if (thread_trysend(self, MESSAGE_EMPATHY) != 0) {
        manager(self)->mEmpathyQueued = 1;
        osr(gabi::guest_addr("\213\244\212\264\227\\\226\361\220\254\214\367\201I\n"));
        return;
    }
    osr(gabi::guest_addr("\213\244\212\264\227\\\226\361\216\270\224s\202\265\202\304\202\314\202"
                         "\305\227\\\226\361\202\314\227\\\226\361\201I\n"));
    manager(self)->mEmpathyQueued = 0;
}

/* portal result handling shared by the two StartPortalApp users */
static inline void portal_result(u32 self, u32 r, u32 resloc) {
    *gabi::at<be<u32>>(resloc) = r;
    if (r & 0x80000000u) {
        sSingleton.mPostSucceeded = 0;
        sSingleton.mPostDone = 1;
        OpMgr_reportError(self, resloc, 1u);
        return;
    }
    sSingleton.mPostDone = 1;
    sSingleton.mPostSucceeded = 1;
}

/* open the Miiverse portal on a post */
/* 0203C570 */
static void OpMgr_openPost(u32 self, u32 postId) {
    if (postId == 0)
        return;
    gabi::Local<OperationPortalParam> prm; /* StartPortalAppParam (0x1000) */
    gabi::Local<be<u32>> res;
    bottleOp_olv_PortalParam_ct(prm.a);
    bottleOp_olv_PortalParam_SetPostId(prm.a, postId);
    u32 r = bottleOp_olv_StartPortalApp(prm.a);
    portal_result(self, r, res.a);
}

/* open the Miiverse portal on the current user */
/* 0203C61C */
static void OpMgr_openUser(u32 self) {
    gabi::Local<OperationPortalParam> prm;
    gabi::Local<be<u32>> res;
    bottleOp_olv_PortalParam_ct(prm.a);
    u32 pid = bottleOp_act_GetPrincipalId();
    if (pid == 0)
        return;
    bottleOp_olv_PortalParam_SetUserPid(prm.a, pid);
    u32 r = bottleOp_olv_StartPortalApp(prm.a);
    portal_result(self, r, res.a);
}

/* ProcUI callback after the post applet returns */
/* 0203C6BC */
static u32 OpMgr_onPostAppReturn() {
    u32 r = bottleOp_olv_GetResultByPostApp();
    u32 ok = (r >> 31) ^ 1;
    sSingleton.mPostDone = 1;
    u32 pending = sSingleton.mPostPending;
    sSingleton.mPostSucceeded = ok;
    if (pending != 0) {
        sysui_a(1);
        sysui_b(1);
        if (sSingleton.mPostSucceeded != 0) {
            u32 album = bottleOp_getWriteAlbum(
                gabi::ea(&gabi::at<OperationSavedAlbum>(sSave.mAddress)->mData));
            bottleOp_setAlbumPosted(album, 1u);
        }
    }
    u32 g = sMenu.mAddress;
    sSingleton.mPostPending = 0;
    if (bottleOp_checkMenuEvent(g) != 0)
        bottleOp_setMenuEvent(sMenu.mAddress, 0u);
    return 0;
}

/* abort a pending empathy request (state 4) */
/* 0203C780 */
static void OpMgr_abortEmpathy(u32 self) {
    if (manager(self)->mState != STATE_EMPATHY)
        return;
    if (manager(self)->mEmpathyQueued == 0) {
        osr(gabi::guest_addr(
            "\225\333\214\257\213\225\202\265\202\255\213\244\212\264\216\270\224s\201I\n"));
        gabi::Local<be<u32>> res;
        (*res) = 0xFFFFFFFF;
        manager(self)->mErrorOverride = 0x118D5D;
        OpMgr_reportError(self, res.a, 1u);
        manager(self)->mErrorOverride = 0;
        sSingleton.mPostSucceeded = 0;
        manager(self)->mState = STATE_IDLE;
        osr(gabi::guest_addr(
            "ErrorViewerTask\202\311\202\250\212\350\202\242\202\265\202\275\201B\n"));
    }
    bottleOp_olv_Cancel();
    osr(gabi::guest_addr("\213\244\212\264\203L\203\203\203\223\203Z\203\213\214\304\202\361\202"
                         "\276\202\346\201I\n"));
}

/* 0203C830 */
static u32 Curl_malloc(u32 size) {
    u32 h = sSingleton.mCurlHeap;
    return hd_olv_0203B620::heap(h)->allocate(size, 0x40u);
}

/* 0203C850 */
static u32 Curl_free(u32 p) {
    u32 h = sSingleton.mCurlHeap;
    return hd_olv_0203B620::heap(h)->free(p);
}

/* 0203C86C */
static u32 Curl_realloc(u32 p, u32 size) {
    if (p != 0) {
        if (size == 0) {
            Curl_free(p);
            return 0;
        }
        u32 old = bottleOp_getAllocationSize(sSingleton.mCurlHeap, p);
        if (old != 0) {
            u32 q = Curl_malloc(size);
            if (q != 0) {
                bottleOp_OSBlockMove_(q, p, size < old ? size : old, 0u);
                Curl_free(p);
            }
            return q;
        }
    }
    return Curl_malloc(size);
}

/* 0203C964 */
static u32 Curl_strdup(u32 s) {
    if (s == 0)
        return 0;
    u32 e = s;
    while (*gabi::at<be<u8>>(e) != 0)
        e++;
    u32 len = e - s + 1;
    u32 q = Curl_malloc(len);
    bottleOp_OSBlockMove_(q, s, len, 0u);
    *gabi::at<be<u8>>(q + len - 1) = 0;
    return q;
}

/* 0203CA04 */
static u32 Curl_calloc(u32 n, u32 sz) {
    if (n == 0)
        n = 1;
    if (sz == 0)
        sz = 1;
    u32 t = n * sz;
    u32 q = Curl_malloc(t);
    if (q != 0)
        bottleOp_OSBlockSet_(q, 0u, t);
    return q;
}

/* network / Miiverse initialisation on the thread (state 2 -> 1, or back to 2 on error) */
/* 0203CA70 */
static void OpMgr_initNetwork(u32 self) {
    if (manager(self)->mState != STATE_INITIALIZING)
        return;
    gabi::Local<be<u32>> res;
    (*res) = 0xFFFFFFFF;
    u32 r = bottleOp_ac_Initialize();
    (*res) = r;
    if (r & 0x80000000u) {
        bottleOp_ac_GetLastErrorCode(gabi::ea(&manager(self)->mErrorOverride));
        u32 w = (*res);
        osr1(gabi::guest_addr("nn::ac::Initialize FAILED !!! reason:%d\n"),
             w & (((w >> 27) & 3) == 3 ? 0x3FFu : 0xFFFFFu));
        manager(self)->mState = STATE_INITIALIZING;
        return;
    }
    manager(self)->mAcInitialized = 1;
    r = bottleOp_ac_Connect();
    (*res) = r;
    if (r & 0x80000000u) {
        bottleOp_ac_GetLastErrorCode(gabi::ea(&manager(self)->mErrorOverride));
        u32 w = (*res);
        osr1(gabi::guest_addr("nn::ac::Connect FAILED !!! reason:%d\n"),
             w & (((w >> 27) & 3) == 3 ? 0x3FFu : 0xFFFFFu));
        manager(self)->mState = STATE_INITIALIZING;
        return;
    }
    manager(self)->mAcConnected = 1;
    osr(gabi::guest_addr("nn::ac::Connect SUCCESS !!!\n"));
    s32 sr = (s32)bottleOp_socket_lib_init();
    if (sr < 0) {
        manager(self)->mErrorOverride = 0xFDE7F;
        osr1(gabi::guest_addr("ERROR: SOinit failed with rval %d\n"), (u32)sr);
        manager(self)->mState = STATE_INITIALIZING;
        return;
    }
    manager(self)->mSocketsInitialized = 1;
    osr(gabi::guest_addr("Socket library initialized.\n"));
    u32 nr = bottleOp_NSSLInit();
    if (nr != 0) {
        manager(self)->mErrorOverride = 0xFDE7F;
        osr1(gabi::guest_addr("ERROR: NSSLinit failed with NSSL_RVAL %d\n"), nr);
        manager(self)->mState = STATE_INITIALIZING;
        return;
    }
    manager(self)->mNsslInitialized = 1;
    if (bottleOp_curl_global_init_mem(
            3u, gabi::guest_fn((const void*)Curl_malloc), gabi::guest_fn((const void*)Curl_free),
            gabi::guest_fn((const void*)Curl_realloc), gabi::guest_fn((const void*)Curl_strdup),
            gabi::guest_fn((const void*)Curl_calloc)) != 0) {
        manager(self)->mErrorOverride = 0xFDE7F;
        manager(self)->mState = STATE_INITIALIZING;
        return;
    }
    manager(self)->mCurlInitialized = 1;
    gabi::Local<OperationInitializeParam> ip; /* nn::olv::InitializeParam */
    bottleOp_olv_InitializeParam_ct(ip.a);
    bottleOp_OSBlockSet_(manager(self)->mWorkBuffer, 0u, manager(self)->mWorkSize);
    r = bottleOp_olv_InitializeParam_SetWork(ip.a, manager(self)->mWorkBuffer,
                                             manager(self)->mWorkSize);
    (*res) = r;
    if (r & 0x80000000u) {
        manager(self)->mState = STATE_INITIALIZING;
        return;
    }
    r = bottleOp_olv_Initialize(ip.a);
    (*res) = r;
    if (r & 0x80000000u) {
        manager(self)->mErrorOverride = bottleOp_olv_GetErrorCode(res.a);
        if (bottleOp_olv_IsInitialized() != 0)
            bottleOp_olv_Finalize();
        manager(self)->mState = STATE_INITIALIZING;
        return;
    }
    bottleOp_olv_SetReportTypes(0x800u);
    bottleOp_ProcUIRegisterCallback(0u, gabi::guest_fn((const void*)OpMgr_onPostAppReturn), 0u, 1u);
    osr(gabi::guest_addr("\n\nOLV Init End\n\n"));
    auto now = bottleOp_OSGetSystemTime();
    manager(self)->mStartTick.mHigh = now.r3;
    manager(self)->mErrorOverride = 0;
    manager(self)->mStartTick.mLow = now.r4;
    manager(self)->mState = STATE_IDLE;
}

/* send the requested empathy (thread side) */
/* 0203CD18 */
static void OpMgr_sendEmpathy(u32 self) {
    osr(gabi::guest_addr("\202\320\202\306\202\334\202\270\213\244\212\264\214\304\202\324\212\326"
                         "\220\224\202\311\223\374\202\301\202\275\n"));
    if (manager(self)->mPostID == 0)
        return;
    bottleOp_olv_Cancel();
    gabi::Local<OperationEmpathyParam> prm; /* UploadEmpathyToPostDataParam */
    gabi::Local<be<u32>> res;
    bottleOp_olv_EmpathyParam_ct(prm.a);
    bottleOp_olv_EmpathyParam_SetPostId(prm.a, manager(self)->mPostID);
    osr(gabi::guest_addr("\213\244\212\264\214\304\202\324\222\274\221O\n"));
    u32 r = bottleOp_olv_UploadEmpathyToPostData(prm.a);
    (*res) = r;
    osr(gabi::guest_addr("\213\244\212\264\214\304\202\361\202\276\222\274\214\343\n"));
    if ((*res) & 0x80000000u) {
        osr(gabi::guest_addr("\213\244\212\264\216\270\224s\201I\n"));
        sSingleton.mPostSucceeded = 0;
        OpMgr_reportError(self, res.a, 1u);
        osr(gabi::guest_addr(
            "ErrorViewerTask\202\311\202\250\212\350\202\242\202\265\202\275\201B\n"));
        return;
    }
    osr(gabi::guest_addr("\213\244\212\264\202\265\202\275\201I\n"));
    sSingleton.mPostSucceeded = 1;
}

/* periodic update: idle for 300 s -> refresh the downloads (state 3); retry a pending
 * empathy request */
/* 0203DDC0 */
static void OpMgr_update(u32 self) {
    u32 s = manager(self)->mState;
    if (s == STATE_IDLE) {
        auto el = bottleOp_getElapsedTicks(gabi::ea(&manager(self)->mStartTick));
        auto sec = bottleOp_divideTicks(el.r3, el.r4, sTickRate.mHigh, sTickRate.mLow);
        if ((s32)sec.r3 < 0)
            return;
        if ((s32)sec.r3 == 0 && sec.r4 < 0x12C)
            return;
        if (thread_trysend(self, MESSAGE_DOWNLOAD) == 0)
            return;
        manager(self)->mState = STATE_DOWNLOADING;
        auto now = bottleOp_OSGetSystemTime();
        manager(self)->mStartTick.mHigh = now.r3;
        manager(self)->mStartTick.mLow = now.r4;
        return;
    }
    if (s != STATE_EMPATHY)
        return;
    if (manager(self)->mEmpathyQueued != 0)
        return;
    if (thread_trysend(self, MESSAGE_EMPATHY) != 0) {
        manager(self)->mEmpathyQueued = 1;
        osr(gabi::guest_addr(
            "\213\244\212\264\225\333\214\257\227\\\226\361\220\254\214\367\201I\n"));
        return;
    }
    osr(gabi::guest_addr("\213\244\212\264\225\333\214\257\227\\\226\361\216\270\224s\201I\n"));
}

/* worker thread message handler (sead::DelegateThread callback) */
/* 0203DEEC */
static void OpMgr_threadProc(u32 self, u32 thread, u32 msg) {
    u32 cs = gabi::ea(&manager(self)->mCriticalSection);
    if (msg == MESSAGE_INITIALIZE) {
        bottleOp_enterCriticalSection(cs);
        OpMgr_initNetwork(self);
        bottleOp_leaveCriticalSection(cs);
    } else if (msg == MESSAGE_DOWNLOAD) {
        bottleOp_enterCriticalSection(cs);
        OpMgr_download(self);
        if (manager(self)->mState != STATE_EMPATHY)
            (manager(self)->mState = STATE_IDLE);
        bottleOp_leaveCriticalSection(cs);
    } else if (msg == MESSAGE_EMPATHY) {
        bottleOp_enterCriticalSection(cs);
        OpMgr_sendEmpathy(self);
        manager(self)->mState = STATE_IDLE;
        bottleOp_leaveCriticalSection(cs);
    } else if (msg == MESSAGE_UPDATE) {
        OpMgr_update(self);
    }
    thread_send(self, MESSAGE_UPDATE);
}

/* the singleton disposer's destructor */

/* __sinit: header statics, the 50 downloaded posts, the topic and the list parameter */

/* update the saved comment counts of the known posts from a fresh post list; flags new
 * comments in the comment manager */
/* 0203CE24 */
static void OpMgr_refreshCommentCounts(u32 self) {
    if (comments(sComments.mAddress)->mNotification != 0)
        return;
    u32 store = gabi::ea(&gabi::at<OperationSavedPosts>(sSave.mAddress)
                              ->mData); /* saved post ids / comment counts */
    gabi::Local<be<u32>> res;
    gabi::Local<be<u32>> cnt;
    u32 changed = 0;
    (*res) = 0xFFFFFFFF;
    bottleOp_olv_ListParam_SetPostDataMaxNum(gabi::ea(&sListParam), 0x32u);
    bottleOp_olv_ListParam_SetFlags(gabi::ea(&sListParam), 0x104u);
    u32 s = manager(self)->mState;
    if (s == STATE_EMPATHY || s == STATE_CANCELLED)
        return;
    u32 r = bottleOp_olv_DownloadPostDataList(gabi::ea(&sTopic), gabi::ea(&sPosts), cnt.a, 0x32u,
                                              gabi::ea(&sListParam));
    (*res) = r;
    if (r & 0x80000000u) {
        OpMgr_reportError(self, res.a, 0u);
        return;
    }
    /* FixedSafeString<0x20> {buffer, vtable, capacity} + buffer */
    gabi::Local<OperationFixedString> id;
    const u32 buf = id.a + 0xC;
    u32 post = gabi::ea(&sPosts);
    for (u32 i = 0; i < (*cnt); i++, post += (u32)sizeof(OperationPostData)) {
        for (u32 j = 0; j < MAX_POSTS; j++) {
            id->mCharacters[0] = 0;
            id->mCapacity = 0x20;
            id->mCharacters[0x1F] = 0;
            id->mBuffer = buf;
            id->mVtable = gabi::ea(&sFixedStringVtable);
            bottleOp_getSavedPostID(store, id.a, j & 0xFFFF);
            u32 cs = (s32)id->mCapacity - 1 >= 0 ? id->mBuffer : gabi::guest_addr("");
            u32 pid = bottleOp_olv_Data_GetPostId(post);
            u32 a, b;
            u32 k = 0;
            do {
                a = *gabi::at<be<u8>>(cs + k);
                b = *gabi::at<be<u8>>(pid + k);
                k++;
            } while (a == b && a != 0);
            if (a != b)
                continue;
            u32 saved = bottleOp_getSavedCommentCount(store, j & 0xFFFF);
            if (saved < bottleOp_olv_Post_GetCommentCount(post))
                changed = 1;
        }
    }
    post = gabi::ea(&sPosts);
    for (u32 k = 0; k < (*cnt) && (s32)k < MAX_POSTS; k++, post += (u32)sizeof(OperationPostData)) {
        id->mCapacity = 0x20;
        id->mBuffer = buf;
        id->mVtable = gabi::ea(&sFixedStringVtable);
        id->mCharacters[0x1F] = 0;
        id->mCharacters[0] = 0;
        u32 pid = bottleOp_olv_Data_GetPostId(post);
        bottleOp_setSavedPostID(store, pid, k & 0xFFFF);
        u32 cc = bottleOp_olv_Post_GetCommentCount(post);
        bottleOp_setSavedCommentCount(store, k & 0xFFFF, cc & 0xFFFF);
    }
    u32 cm = sComments.mAddress;
    comments(cm)->mNotification = changed;
    if (changed)
        (comments(cm)->mNotificationRead = 0);
    bottleOp_flushSave(sSaveSystem.mAddress);
}

/* clear one bottle entry */
static inline void clear_entry(u32 e) {
    bottle(e)->mName.mpData.get()[0] = 0;
    bottle(e)->mTopic.mpData.get()[0] = 0;
    bottle(e)->mBody.mpData.get()[0] = 0;
    bottle(e)->mSpoiler = 0;
    bottle(e)->mHasMemo = 0;
    bottle(e)->mScreenshot.mpSurface = 0;
    bottle(e)->mApplicationSize = 0;
    bottle(e)->mMemo.mpSurface = 0;
    bottle(e)->mApplicationValue = 0;
    bottle(e)->mEmpathy = 0;
    bottle(e)->mTextureSlot = 0xFF;
    bottle(e)->mHasScreenshot = 0;
    bottle(e)->mpScreenshotBuffer = 0;
    bottle(e)->mEmpathyCount = 0;
    bottle(e)->mApplicationFlag = 0;
    bottle(e)->mCommentCount = 0;
    bottle(e)->mApplicationID = 0;
    bottle(e)->mValid = 0;
    bottle(e)->mApplicationType = 0;
    bottle(e)->mRead = 0;
    bottleOp_OSBlockSet_(gabi::ea(bottle(e)->mPostID), 0u, 0x20u);
}

/* release the texture slots in state 4 and clear the bottle entries whose texture slot
 * became free */
/* 0203D0B0 */
static void OpMgr_resetEntries(u32 self) {
    u32 cm = sComments.mAddress;
    u32 tex = gabi::ea(comments(cm)->mTextures);
    for (u32 i = 0; i < MAX_TEXTURES; i++) {
        u32 t = tex + i * (u32)sizeof(OliveTextureSlot);
        if (textureSlot(t)->mState == TEXTURE_RELEASED) {
            if (!(i < MAX_TEXTURES))
                t = tex;
            textureSlot(t)->mState = TEXTURE_EMPTY;
        }
    }
    u32 base = gabi::ea(comments(cm)->mDownloaded);
    for (u32 k = 0; k < MAX_BOTTLES; k++) {
        u32 e = k < MAX_BOTTLES ? base + k * (u32)sizeof(OliveBottle) : base;
        s32 b = (s8)bottle(e)->mTextureSlot;
        if (b < 0)
            continue;
        u32 t = (u32)b < MAX_TEXTURES ? tex + (u32)b * (u32)sizeof(OliveTextureSlot) : tex;
        if (textureSlot(t)->mState != TEXTURE_EMPTY)
            continue;
        clear_entry(k < MAX_BOTTLES ? base + k * (u32)sizeof(OliveBottle) : base);
    }
}

static inline u32 str_eq(u32 a, u32 b) {
    u32 x, y;
    do {
        x = *gabi::at<be<u8>>(a++);
        y = *gabi::at<be<u8>>(b++);
    } while (x == y && x != 0);
    return x == y;
}

/* is POSTID already one of the current or the other bottle entries? */
/* 0203D284 */
static u32 OpMgr_isKnownPost(u32 self, u32 postId) {
    for (u32 k = 0; k < MAX_BOTTLES; k++) {
        u32 e = gabi::ea(comments(sComments.mAddress)->mDownloaded) + k * (u32)sizeof(OliveBottle);
        if (str_eq(gabi::ea(bottle(e)->mPostID), postId))
            return 1;
    }
    for (u32 k = 0; k < MAX_BOTTLES; k++) {
        u32 e = gabi::ea(comments(sComments.mAddress)->mShown) + k * (u32)sizeof(OliveBottle);
        if (str_eq(gabi::ea(bottle(e)->mPostID), postId))
            return 1;
    }
    return 0;
}

/* first free texture slot, or -1 */
/* 0203D394 */
static s32 OpMgr_findFreeTex(u32 self) {
    u32 t = gabi::ea(comments(sComments.mAddress)->mTextures);
    for (s32 i = 0; i < MAX_TEXTURES; i++, t += (u32)sizeof(OliveTextureSlot))
        if (textureSlot(t)->mState == TEXTURE_EMPTY)
            return (s8)i;
    return -1;
}

// HD: capped SafeString copy reloads the capacity and buffer after the native copy call.
static inline void wcopy(u32 e, u32 srcLocal, u32 maxn, u32 termOff) {
    s32 cap = (s32)gabi::at<OliveWideString>(e)->mCapacity;
    u32 n = maxn;
    u32 dst = gabi::at<OliveWideString>(e)->mpData.v.get();
    if (!(cap > (s32)maxn))
        n = cap - 1;
    bottleOp_assureWideTermination(srcLocal);
    bottleOp_OSBlockMove_(dst, gabi::at<OliveSafeWideString>(srcLocal)->mpData.v.get(), n * 2, 0u);
    *gabi::at<be<u16>>(dst + n * 2) = 0;
    if (!((s32)gabi::at<OliveWideString>(e)->mCapacity > (s32)maxn)) {
        gabi::at<OliveWideString>(e)->assureTermination();
        /* (sead's capped strlen of the result follows; its value is unused) */
    } else {
        *gabi::at<be<u16>>(gabi::at<OliveWideString>(e)->mpData.v.get() + termOff) = 0;
    }
}

/* store a downloaded post into bottle entry SLOT (texture slot, name, date, topic, body
 * text or handwritten memo texture, counts, flags, screenshot and app data); FLAG: from the "from
 * game" list. Returns 1 when the entry is complete. */
/* 0203D3D4 */
static u32 OpMgr_storePost(u32 self, u32 slot, u32 post, u32 flag) {
    u32 me = bottleOp_act_GetPrincipalId();
    if (me == bottleOp_olv_Data_GetUserPid(post))
        return 0;
    s32 ti = (s32)OpMgr_findFreeTex(self);
    if (ti < 0) {
        osr(gabi::guest_addr("\203e\203N\203X\203`\203\203\203o\203b\203t\203@"
                             "\202\314\213\363\202\253\202\252\226\263\202\242\n"));
        return 0;
    }
    u32 s = manager(self)->mState;
    if (s == STATE_EMPATHY || s == STATE_CANCELLED)
        return 0;
    u32 cm = sComments.mAddress;
    u32 e = gabi::ea(comments(cm)->mDownloaded);
    if (slot < MAX_BOTTLES)
        e += slot * (u32)sizeof(OliveBottle);
    u32 tex = gabi::ea(comments(cm)->mTextures);
    if ((u32)ti < MAX_TEXTURES)
        tex += (u32)ti * (u32)sizeof(OliveTextureSlot);
    bottleOp_OSBlockMove_(gabi::ea(bottle(e)->mPostID), bottleOp_olv_Data_GetPostId(post), 0x20u,
                          0u);
    u32 heap = bottleOp_getImageHeap();
    gabi::Local<be<u32>[2]> ws;
    gabi::Local<be<u32>[2]> ws2;
    gabi::Local<be<u32>> memoSz;
    gabi::Local<be<u32>> imgSz;
    gabi::Local<be<u32>> appSz;
    gabi::Local<OperationAppData> app;
    gabi::Local<be<u32>[2]> l1c;
    gabi::Local<OperationDimensions> wh;
    gabi::Local<be<u32>[10]> cal;
    gabi::Local<be<u16>[0x66]> body;
    auto date = bottleOp_olv_Data_GetPostDate(post);
    bottleOp_OSTicksToCalendarTime(date.r3, date.r4, cal.a);
    gabi::at<OliveSafeString>(ws.a)->mpData.v = bottleOp_olv_Data_GetMiiNickname(post);
    gabi::at<OliveSafeString>(ws.a)->__vtbl = gabi::ea(&sWideStringVtable);
    wcopy(e, ws.a, 10, 0x14);
    gabi::at<OliveSafeString>(ws.a)->mpData.v = bottleOp_olv_Data_GetTopicTag(post);
    gabi::at<OliveSafeString>(ws.a)->__vtbl = gabi::ea(&sWideStringVtable);
    bottleOp_setBottleTopic(e, ws.a);
    if (bottleOp_olv_Data_TestFlags(post, 1u) != 0) {
        if (bottleOp_olv_Data_GetBodyText(post, body.a, 0x65u) & 0x80000000u)
            return 0;
        gabi::at<OliveSafeString>(ws2.a)->__vtbl = gabi::ea(&sWideStringVtable);
        gabi::at<OliveSafeString>(ws2.a)->mpData.v = body.a;
        wcopy(e + 0x98, ws2.a, 200, 0x190);
    }
    bottle(e)->mSpoiler = bottleOp_olv_Data_TestFlags(post, 0x80u);
    bottle(e)->mEmpathyCount = bottleOp_olv_Post_GetEmpathyCount(post);
    u32 cc = bottleOp_olv_Post_GetCommentCount(post);
    bottle(e)->mRead = 0;
    bottle(e)->mCommentCount = cc;
    bottle(e)->mEmpathy = flag;
    u32 spo = bottleOp_olv_Data_TestFlags(post, 0x200u);
    bottle(e)->mFlag = spo;
    bottle(e)->mTextureSlot = (u32)ti;
    bool hasImage;
    if (bottleOp_olv_Data_TestFlags(post, 2u) != 0) {
        /* handwritten memo: decode the TGA into a texture */
        (*memoSz) = 0;
        if (bottleOp_olv_Data_GetBodyMemo(post, textureSlot(tex)->mpImage.v.get(), memoSz.a,
                                          0x2582Cu) &
            0x80000000u) {
            bottle(e)->mHasMemo = 0;
            return 0;
        }
        bottle(e)->mHasMemo = 1;
        u32 img = bottleOp_allocateInHeap(0x90u, heap, 4u);
        if (img != 0)
            img = bottleOp_constructTgaImage(img);
        bottleOp_decodeTgaImage(img, textureSlot(tex)->mpImage.v.get(), 0x28000u, heap);
        u32 gt = gabi::ea(&memo(bottle(e))->mTexture); /* GX2Texture */
        bottleOp_OSBlockMove_(gt, img + 4, 0x74u, 0u);
        u32 a338 = memo(bottle(e))->mTexture.mSurface.mMipLevels;
        memo(bottle(e))->mTexture.mFirstMip = 0;
        memo(bottle(e))->mTexture.mComponents = 0x10203;
        u32 a334 = memo(bottle(e))->mTexture.mSurface.mDepth;
        memo(bottle(e))->mTexture.mFirstSlice = 0;
        memo(bottle(e))->mTexture.mSliceCount = a334;
        memo(bottle(e))->mTexture.mMipCount = a338;
        bottleOp_GX2InitTextureRegs(gt);
        bottleOp_getTextureFormat(l1c.a, 0x1Au, 1u);
        s32 w = (s32)gabi::at<OperationTgaImage>(img)->mSurface.mWidth;
        s32 mw = (s32)gabi::at<OperationTgaImage>(img)->mMinimumWidth;
        if (w > mw)
            mw = w;
        s32 h = (s32)gabi::at<OperationTgaImage>(img)->mSurface.mHeight;
        s32 mh = (s32)gabi::at<OperationTgaImage>(img)->mMinimumHeight;
        if (h > mh)
            mh = h;
        u32 fmt = *gabi::at<be<u32>>(l1c.a);
        memo(bottle(e))->mPresent = 1;
        wh->mWord = ((u32)mw << 16) | ((u32)mh & 0xFFFF);
        memo(bottle(e))->mWidth = wh->mWidth;
        memo(bottle(e))->mHeight = wh->mHeight;
        memo(bottle(e))->mFormat = fmt;
        gabi::call_ptr<u32>(
            gabi::at<OperationManagerVtable>(gabi::at<OperationTgaImage>(img)->__vtbl)
                ->mDestroy.get(),
            img, 3u);
        hasImage = bottleOp_olv_Data_TestFlags(post, 4u) != 0;
    } else {
        bottle(e)->mHasMemo = 0;
        hasImage = bottleOp_olv_Data_TestFlags(post, 4u) != 0;
    }
    if (hasImage) {
        if (bottleOp_olv_Data_GetExternalImageDataSize(post) > 0x50000)
            return 0;
        (*imgSz) = 0;
        bottle(e)->mpScreenshotBuffer.v = textureSlot(tex)->mpScreenshot.v.get();
        s = manager(self)->mState;
        if (s == STATE_EMPATHY || s == STATE_CANCELLED)
            return 0;
        if (bottleOp_olv_Data_DownloadExternalImageData(
                post, textureSlot(tex)->mpScreenshot.v.get(), imgSz.a, 0x50000u) &
            0x80000000u) {
            bottle(e)->mHasScreenshot = 0;
            return 0;
        }
        (*appSz) = 0;
        if (bottleOp_olv_Data_GetAppData(post, app.a, appSz.a, 0x10u) & 0x80000000u) {
            bottle(e)->mHasScreenshot = 0;
            return 0;
        }
        while (bottleOp_startJpegJob(sJpegJob.mAddress, bottle(e)->mpScreenshotBuffer.v.get(),
                                     gabi::ea(&bottle(e)->mScreenshot),
                                     textureSlot(tex)->mpMemo.v.get(), 8u) == 0) {
        }
        u32 jd = sJpegJob.mAddress;
        if (gabi::at<OperationJpegJob>(jd)->mDone == 0)
            for (;;) {
            } /* the original spins here forever (the flag is not re-read) */
        if (gabi::at<OperationJpegJob>(jd)->mSucceeded == 0) {
            bottle(e)->mHasScreenshot = 0;
            return 0;
        }
        bottle(e)->mHasScreenshot = 1;
        bottle(e)->mApplicationType = app->mType;
        bottle(e)->mApplicationFlag = app->mFlag;
        bottle(e)->mApplicationID = app->mID;
        bottle(e)->mApplicationSize = app->mSize;
        bottle(e)->mApplicationValue = app->mValue;
    } else {
        bottle(e)->mHasScreenshot = 0;
    }
    u32 v = *gabi::at<be<u8>>(
        bottleOp_getBottleMode(gabi::ea(&gabi::at<OperationSavedAlbum>(sSave.mAddress)->mData)));
    bool ok = flag != 0 ? v == 1 : (v != 1 && v != 2);
    if (!ok) {
        osr(gabi::guest_addr(
            "\216\363\220Mj\212J\216n\216\236\202\306\220\335\222\350\202\252\210\341\202\244\n"));
        return 0;
    }
    bottleOp_commitBottleDownload(sComments.mAddress);
    bottle(e)->mValid = 1;
    textureSlot(tex)->mState = TEXTURE_READY;
    return 1;
}

/* collect up to 10 new posts of the list into the bottle entries */
static inline void collect(u32 self, u32 cnt, u32& n, u32 flag) {
    u32 post = gabi::ea(&sPosts);
    for (u32 k = 0; k < *gabi::at<be<u32>>(cnt) && (s32)n < MAX_BOTTLES;
         k++, post += (u32)sizeof(OperationPostData)) {
        u32 pid = bottleOp_olv_Data_GetPostId(post);
        if (OpMgr_isKnownPost(self, pid) != 0)
            continue;
        if (OpMgr_storePost(self, n, post, flag) != 0)
            n++;
    }
}

/* download job: (needs the Tingle Bottle, item 0x21) refresh counts, reset entries, then fill
 * the free bottle entries from the "from_game" post list of the current mode */
/* 0203DA00 */
static void OpMgr_download(u32 self) {
    if (manager(self)->mState == STATE_DEFERRED) {
        osr(gabi::guest_addr("OliveOperationMgr::\213\331\213}\216\226\221\324\n"));
        return;
    }
    if (bottleOp_checkItem(0x21u, 1u) == 0) {
        osr(gabi::guest_addr("OliveOperationMgr::\203{\203g\203\213\226\242\223\374\216\350\n"));
        return;
    }
    if (manager(self)->mState == STATE_CANCELLED)
        return;
    u32 v = *gabi::at<be<u8>>(
        bottleOp_getBottleMode(gabi::ea(&gabi::at<OperationSavedAlbum>(sSave.mAddress)->mData)));
    if (v != 2)
        OpMgr_refreshCommentCounts(self);
    OpMgr_resetEntries(self);
    u32 n = bottleOp_countDownloadedBottles(sComments.mAddress);
    if ((u32)(9 - n) >= 10) {
        osr(gabi::guest_addr("OliveOperationMgr::\203{"
                             "\203g\203\213\202\360\225\372\202\350\215\236\202\336\202\306\202\261"
                             "\202\353\202\252\226\263\202\242\201I\n"));
        u32 cm = sComments.mAddress;
        gabi::cpu->cr[6] = 0;
        bottleOp_OSReport(gabi::guest_addr("IsValid : %d, PostId : %s, TexIndex : %d, "),
                          comments(cm)->mDownloaded[9].mValid,
                          gabi::ea(comments(cm)->mDownloaded[9].mPostID),
                          (u32)(s32)(s8)comments(cm)->mDownloaded[9].mTextureSlot);
        s32 b = (s8)comments(cm)->mDownloaded[9].mTextureSlot;
        if (b < 0) {
            osr(gabi::guest_addr("TexState : Invalid\n"));
            return;
        }
        u32 t = gabi::ea(comments(cm)->mTextures);
        if ((u32)b < MAX_TEXTURES)
            t += (u32)b * (u32)sizeof(OliveTextureSlot);
        osr1(gabi::guest_addr("TexState : %d\n"), textureSlot(t)->mState);
        return;
    }
    gabi::Local<be<u32>> cnt;
    gabi::Local<be<u32>> res;
    gabi::Local<be<u16>[0x40]> key;
    (*res) = 0xFFFFFFFF;
    Utf8ToUtf16(key.a, gabi::guest_addr("from_game"));
    if (v != 1 && v != 0)
        return;
    bottleOp_olv_ListParam_SetSearchKey(gabi::ea(&sListParam), key.a, 0u);
    bottleOp_olv_ListParam_SetPostDataMaxNum(gabi::ea(&sListParam), 0x32u);
    u32 fl = v == 1 ? 0x81 : 0x80;
    if (*gabi::at<be<u8>>(bottleOp_getBottleMode(
                              gabi::ea(&gabi::at<OperationSavedAlbum>(sSave.mAddress)->mData)) +
                          1) == 0)
        fl |= 0x100;
    bottleOp_olv_ListParam_SetFlags(gabi::ea(&sListParam), fl);
    u32 s = manager(self)->mState;
    if (s == STATE_EMPATHY || s == STATE_CANCELLED)
        return;
    u32 r = bottleOp_olv_DownloadPostDataList(gabi::ea(&sTopic), gabi::ea(&sPosts), cnt.a, 0x32u,
                                              gabi::ea(&sListParam));
    (*res) = r;
    if (r & 0x80000000u) {
        OpMgr_reportError(self, res.a, 0u);
        return;
    }
    collect(self, cnt.a, n, v == 1 ? 1 : 0);
}

// HD: compiler-generated and template code of this translation unit.

/* 0203B620 */
static void destroyWorker(u32 p, u32 flags) {
    if (p == 0)
        return;
    gabi::at<OperationWorker>(p)->__vtbl = gabi::ea(&sWorkerVtable);
    bottleOp_destroyWorkerBase(p, 0u);
    if (flags & 1)
        bottleOp_deleteObject(p);
}

/* 0203B680 */
static void destroySafeStringBase() {}

/* 0203B684 */
static void SafeBuf_assureTerminate(u32 self) {
    *gabi::at<be<u8>>(gabi::at<OliveSafeString>(self)->mpData.v.get() +
                      gabi::at<OliveWideString>(self)->mCapacity - 1) = 0;
}

/* 0203B69C */
static void deleteSafeString(u32 p, u32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        bottleOp_deleteObject(p);
}

/* 0203B6B0 */
static void destroyWideStringBase() {}

/* 0203B6B4 */
static void WSafeBuf_assureTerminate(u32 self) {
    *gabi::at<be<u16>>(gabi::at<OliveSafeString>(self)->mpData.v.get() +
                       gabi::at<OliveWideString>(self)->mCapacity * 2 - 2) = 0;
}

/* 0203B6D0 */
static void deleteWideString(u32 p, u32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        bottleOp_deleteObject(p);
}

/* 0203B6E4 */
static void deleteFixedWideString(u32 p, u32 flags) {
    if (p == 0)
        return;
    if (flags & 1)
        bottleOp_deleteObject(p);
}

/* 0203B728 */
static void destroyCommentStorage(u32 p, u32 flags) {
    if (p == 0)
        return;
    bottleOp_destroyArray(gabi::ea(comments(p)->mDownloaded), 0xAu, 0x3ECu, BOTTLE_DESTRUCTOR, 0u,
                          0u);
    bottleOp_destroyArray(gabi::ea(comments(p)->mShown), 0xAu, 0x3ECu, BOTTLE_DESTRUCTOR, 0u, 0u);
    if (flags & 1)
        bottleOp_deleteObject(p);
}

/* 0203E050 */
static void OpMgr_disposer_dt(u32 p, u32 flags) {
    if (p == 0)
        return;
    gabi::at<OliveDisposer>(p)->__vtbl = gabi::ea(&sDisposerVtable);
    if (p == sSingleton.mDisposer) {
        u32 inst = sSingleton.mInstance;
        sSingleton.mDisposer = 0;
        gabi::call_ptr<u32>(gabi::at<OperationManagerVtable>(manager(inst)->__vtbl)->mDestroy.get(),
                            inst, 2u);
        sSingleton.mInstance = 0;
    }
    bottleOp_destroyDisposer(p, 0u);
    if (flags & 1)
        bottleOp_deleteObject(p);
}

/* 0203E0F8 */
static void __sinit_olvOperation() {
    sHeaderWords.mWords[2] = 0;
    sHeaderWords.mWords[0] = 0;
    sHeaderWords.mWords[3] = 0;
    sHeaderWords.mWords[1] = 0;
    bottleOp_registerGlobal(gabi::ea(&sHeaderDestructors));
    f32 minusPi = sHeaderConstants.mMinusPi;
    f32 pi = sHeaderConstants.mPi;
    sHeaderValues.mMinusPi = minusPi;
    sHeaderValues.mPi = pi;
    bottleOp_constructByte(gabi::ea(&sHeaderValues.mByte));
    bottleOp_registerGlobal(gabi::ea(&sHeaderDestructors.mRecords[1]));
    bottleOp_constructFlag(gabi::ea(&sHeaderValues.mFlag));
    bottleOp_registerGlobal(gabi::ea(&sHeaderDestructors.mRecords[2]));
    bottleOp_constructArray(
        gabi::ea(&sPosts), 0x32u, 0xC208u,
        POST_DATA_CONSTRUCTOR); /* nn::olv::DownloadedPostData::DownloadedPostData */
    bottleOp_registerGlobal(gabi::ea(&sPostArrayDestructor));
    bottleOp_olv_TopicData_ct(gabi::ea(&sTopic));
    bottleOp_olv_ListParam_ct(gabi::ea(&sListParam));
}

// clang-format off
/* WWHD glue
data sHeaderValues 10200C80
ext 028ED6F8 void bottleOp_constructByte(u32)
ext 028EAB2C void bottleOp_constructFlag(u32)
data sSingleton 1018F504
data sTuning 1018F4C8
data sErrorViewer 1018F2BC
data sComments 1018F4AC
data sSave 101F84DC
data sMenu 101F5088
data sSaveSystem 101F852C
data sJpegJob 1018F3F4
data sThreadPriority 101F8B9C
data sTickRate 104A11C8
data sTopic 10200C9C
data sListParam 10201C9C
data sPosts 10202CA0
str 10000160
data sWideStringVtable 10005D20
data sFixedStringVtable 10005D50
data sWideBufferVtable 10005D68
data sFixedWideStringVtable 10005D80
data sOperationDelegateVtable 10005D98
str 10005DA8
str 10005DC0
str 10005DD4
str 10005DFC
str 10005E08
str 10005E14
str 10005E38
data sEmptyWideString 10005E58
str 10005E5C
str 10005E7C in 0203C118
str 10005E88
str 10005E98
str 10005EA4
str 10005EC8
str 10005EE0
str 10005EFC in 0203C780
str 10005F1C
str 10005F44
str 10005F70
str 10005F9C
str 10005FBC
str 10005FE0
str 10006000
str 10006014
str 10006024
str 10006030
str 1000603C
str 1000605C
str 1000606C in 0203CD18
str 1000608C
str 100060AC
str 100060C8
str 100060DC in 0203DA00
str 100060E8
str 10006108
str 1000612C
str 10006160
str 1000618C
str 1000619C
str 100061B0
data sHeaderConstants 100061C4
data sDisposerVtable 100061D0
data sOperationVtable 100061E0
data sWorkerVtable 1015E028
data sHeaderDestructors 1018F4D4
data sPostArrayDestructor 1018F4F8
data sHeaderWords 10200C8C
data sSafeStringVtable 10005CF0
ext C0009EE8 void bottleOp_OSReport(u32)
ext C0009EE8 void bottleOp_OSReport(u32,u32)
ext C0009EE8 void bottleOp_OSReport(u32,u32,u32)
ext 0275CFD0 void bottleOp_destroyWorkerBase(u32,u32)
ext 0273B050 u32 bottleOp_allocateInHeap(u32,u32,u32)
ext 0275D12C u32 bottleOp_constructWorker(u32)
ext 028F0164 void bottleOp_destroyArray(u32,u32,u32,u32,u32,u32)
ext 028F521C void bottleOp_zeroMemory(u32,u32)
ext 02760084 void bottleOp_constructCriticalSection(u32)
ext 02760DE8 void bottleOp_constructTick(u32)
ext 0273B0D4 u32 bottleOp_allocateAlignedInHeap(u32,u32,u32)
ext 02752B0C void bottleOp_constructDisposer(u32,u32,u32)
ext C0004978 void bottleOp_olv_Finalize(u32)
ext 02760168 void bottleOp_destroyCriticalSection(u32,u32)
ext 0203E8C4 u32 bottleOp_getParentHeap()
ext 02753004 u32 bottleOp_createHeap(u32,u32,u32,u32,u32)
ext 0275FBD8 u32 bottleOp_constructDelegateThread(u32,u32,u32,u32,u32,u32,u32,u32,u32)
ext C0009C98 Pair32 bottleOp_OSGetSystemTime()
ext 02754B38 u32 bottleOp_createWorkHeap(u32,u32,u32,u32,u32)
ext C0004908 void bottleOp_olv_Cancel()
ext C0004CA8 u32 bottleOp_olv_IsInitialized()
ext C0004978 void bottleOp_olv_Finalize()
ext C0005A20 void bottleOp_curl_global_cleanup()
ext C0005BA0 void bottleOp_NSSLFinish()
ext C0006020 void bottleOp_socket_lib_finish()
ext C0007C68 void bottleOp_ac_Close()
ext C0007CB8 void bottleOp_ac_Finalize()
ext C0004A70 u32 bottleOp_olv_GetErrorCode(u32)
ext 0203300C void bottleOp_showError(u32,u32,u32)
ext 02035D78 u32 bottleOp_getSystemUI()
ext 02035ED4 void bottleOp_setSystemUIReady(u32,u32)
ext 02035F3C void bottleOp_setSystemUIBusy(u32,u32)
ext 020330E8 void bottleOp_clearError(u32)
ext C0004F70 void bottleOp_olv_UploadPostParam_ct(u32)
ext 02720144 u32 bottleOp_getReadAlbum(u32)
ext 02726188 u32 bottleOp_getAlbumImage(u32,u32)
ext C0004D68 void bottleOp_olv_UploadParam_SetExternalImageData(u32,u32,u32)
ext 027260BC u32 bottleOp_getAlbumData(u32,u32)
ext 02725A00 u32 bottleOp_getAlbumDataType(u32)
ext 02726120 u32 bottleOp_getAlbumFlag(u32,u32)
ext 027260DC u32 bottleOp_getAlbumID(u32,u32)
ext 027259F0 u32 bottleOp_getAlbumDataSize(u32)
ext 02725B44 u32 bottleOp_getAlbumDataValue(u32)
ext C0004CD8 void bottleOp_olv_UploadParam_SetAppData(u32,u32,u32)
ext 0203E250 void bottleOp_assureWideTermination(u32)
ext C0009988 void bottleOp_OSBlockMove_(u32,u32,u32,u32)
ext 0203A984 void bottleOp_getTopicTag(u32,u32)
ext C0004E70 void bottleOp_olv_UploadParam_SetTopicTag(u32,u32)
ext C0004E18 void bottleOp_olv_UploadParam_SetSearchKey(u32,u32,u32)
ext C0004CF0 void bottleOp_olv_UploadPostParam_SetBodyTextMaxLength(u32,u32)
ext C0004DB0 void bottleOp_olv_UploadPostParam_SetFlags(u32,u32)
ext C0004EF0 u32 bottleOp_olv_UploadPostDataByPostApp(u32)
ext C0004FE8 void bottleOp_olv_UploadParamBase_dt(u32,u32)
ext C0004F28 void bottleOp_olv_PortalParam_ct(u32)
ext C0004E00 void bottleOp_olv_PortalParam_SetPostId(u32,u32)
ext C0004E98 u32 bottleOp_olv_StartPortalApp(u32)
ext C0004640 u32 bottleOp_act_GetPrincipalId()
ext C0004E78 void bottleOp_olv_PortalParam_SetUserPid(u32,u32)
ext C0004BA0 u32 bottleOp_olv_GetResultByPostApp()
ext 027201DC u32 bottleOp_getWriteAlbum(u32)
ext 02726DAC void bottleOp_setAlbumPosted(u32,u32)
ext 02617AE4 u32 bottleOp_checkMenuEvent(u32)
ext 02618720 void bottleOp_setMenuEvent(u32,u32)
ext 02754814 u32 bottleOp_getAllocationSize(u32,u32)
ext C0009990 void bottleOp_OSBlockSet_(u32,u32,u32)
ext C0007E48 u32 bottleOp_ac_Initialize()
ext C0007D70 void bottleOp_ac_GetLastErrorCode(u32)
ext C0007CA0 u32 bottleOp_ac_Connect()
ext C0006028 u32 bottleOp_socket_lib_init()
ext C0005BD8 u32 bottleOp_NSSLInit()
ext C0005A30 u32 bottleOp_curl_global_init_mem(u32,u32,u32,u32,u32,u32)
ext C0004F00 void bottleOp_olv_InitializeParam_ct(u32)
ext C0004E90 u32 bottleOp_olv_InitializeParam_SetWork(u32,u32,u32)
ext C0004C98 u32 bottleOp_olv_Initialize(u32)
ext C0004E10 void bottleOp_olv_SetReportTypes(u32)
ext C0007890 void bottleOp_ProcUIRegisterCallback(u32,u32,u32,u32)
ext C0004F68 void bottleOp_olv_EmpathyParam_ct(u32)
ext C0004E08 void bottleOp_olv_EmpathyParam_SetPostId(u32,u32)
ext C0004EE0 u32 bottleOp_olv_UploadEmpathyToPostData(u32)
ext 02760E58 Pair32 bottleOp_getElapsedTicks(u32)
ext 028F5B8C Pair32 bottleOp_divideTicks(u32,u32,u32,u32)
ext 027601BC void bottleOp_enterCriticalSection(u32)
ext 027601F0 void bottleOp_leaveCriticalSection(u32)
ext 02752BEC void bottleOp_destroyDisposer(u32,u32)
ext 028EFFD0 void bottleOp_constructArray(u32,u32,u32,u32)
ext 028F026C void bottleOp_registerGlobal(u32)
ext C0004F20 void bottleOp_olv_TopicData_ct(u32)
ext C0004F50 void bottleOp_olv_ListParam_ct(u32)
ext C0004DE8 void bottleOp_olv_ListParam_SetPostDataMaxNum(u32,u32)
ext C0004DA8 void bottleOp_olv_ListParam_SetFlags(u32,u32)
ext C0004960 u32 bottleOp_olv_DownloadPostDataList(u32,u32,u32,u32,u32)
ext 02725578 void bottleOp_getSavedPostID(u32,u32,u32)
ext C0004B58 u32 bottleOp_olv_Data_GetPostId(u32)
ext 027255C4 u32 bottleOp_getSavedCommentCount(u32,u32)
ext C00049D8 u32 bottleOp_olv_Post_GetCommentCount(u32)
ext 027255A4 void bottleOp_setSavedPostID(u32,u32,u32)
ext 027255E4 void bottleOp_setSavedCommentCount(u32,u32,u32)
ext 0272294C void bottleOp_flushSave(u32)
ext C0004C60 u32 bottleOp_olv_Data_GetUserPid(u32)
ext 0203E90C u32 bottleOp_getImageHeap()
ext C0004B50 Pair32 bottleOp_olv_Data_GetPostDate(u32)
ext C000A180 void bottleOp_OSTicksToCalendarTime(u32,u32,u32)
ext C0004B00 u32 bottleOp_olv_Data_GetMiiNickname(u32)
ext C0004C28 u32 bottleOp_olv_Data_GetTopicTag(u32)
ext 02039240 void bottleOp_setBottleTopic(u32,u32)
ext C0004EB0 u32 bottleOp_olv_Data_TestFlags(u32,u32)
ext C00049C8 u32 bottleOp_olv_Data_GetBodyText(u32,u32,u32)
ext C0004A68 u32 bottleOp_olv_Post_GetEmpathyCount(u32)
ext C00049B8 u32 bottleOp_olv_Data_GetBodyMemo(u32,u32,u32,u32)
ext 027BE6B8 u32 bottleOp_constructTgaImage(u32)
ext 027B69A8 void bottleOp_decodeTgaImage(u32,u32,u32,u32)
ext C0006498 void bottleOp_GX2InitTextureRegs(u32)
ext 0287F1EC void bottleOp_getTextureFormat(u32,u32,u32)
ext C0004A98 u32 bottleOp_olv_Data_GetExternalImageDataSize(u32)
ext C0004958 u32 bottleOp_olv_Data_DownloadExternalImageData(u32,u32,u32,u32)
ext C00049A0 u32 bottleOp_olv_Data_GetAppData(u32,u32,u32,u32)
ext 02036474 u32 bottleOp_startJpegJob(u32,u32,u32,u32,u32)
ext 027200F4 u32 bottleOp_getBottleMode(u32)
ext 0203AF50 void bottleOp_commitBottleDownload(u32)
ext 0254DA50 u32 bottleOp_checkItem(u32,u32)
ext 02039B80 u32 bottleOp_countDownloadedBottles(u32)
ext C0009EE8 void bottleOp_OSReport(u32,u32,u32,u32)
ext C0004E20 void bottleOp_olv_ListParam_SetSearchKey(u32,u32,u32)
ext 0273AD10 u32 bottleOp_allocateObject(u32)
ext 0273AF40 void bottleOp_deleteObject(u32)
*/
