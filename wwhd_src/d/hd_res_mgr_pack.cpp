/* hd_res_mgr_pack: HD ResMgr loader threads, file loading and pack scanning (unit hd_res_mgr), WWHD.
 * HD-only code written from the WWHD code; see
 * hd_res_mgr.cpp for the ResMgr layout. sead objects built on the stack here (layouts from the
 * code): FileDevice path buffer FixedSafeString<0x104>, ResourceMgr::LoadArg, a temporary
 * resource list object (destroyed by 0275CFD0), ArchiveFileDevice, DirectoryHandle, DirectoryEntry.
 */
#include "d/hd_res_mgr.h"
using namespace gabi;
using namespace ResourceManagerHD;
u32 resPackNative_decompressionArenaConstruct(u32, u32, u32);
u32 resPackNative_allocateWithHeap(u32, u32, u32);
u32 resPackNative_threadConstruct(u32, u32, u32, u32, u32, u32, u32, u32, u32);
void resPackNative_threadSetCoreMask(u32, u32);
void resPackNative_disposerConstruct(u32);
u32 resPackNative_findFileDevice(u32, u32, u32);
u32 resPackNative_loadResource(u32, u32);
void resPackNative_resourceListDestroy(u32, u32);
u32 resPackNative_loadResourceDecompressed(u32, u32, u32, u32);
void resPackNative_invalidateArchiveCache(u32, u32);
u32 resPackNative_constructPackHolder(u32, u32, u32, u32);
u32 resPackNative_hashCRC(u32, u32);
u32 resPackNative_insertPackNode(u32, u32, u32);
u32 resPackNative_findPackNode(u32, u32, u32);
void resPackNative_copyBytes(u32, u32, u32, u32);
void resPackNative_stringViewConstruct(u32);
void resPackNative_threadSleep(u32);
u32 resPackNative_findArchiveNode(u32, u32, u32);
u32 resPackNative_insertArchiveNode(u32, u32, u32);
void resPackNative_closeDirectory(u32, u32);
void resPackNative_disposerDestroy(u32, u32);
void resPackNative_archiveDeviceDestroy(u32, u32);
u32 resPackNative_createExpHeap(u32, u32, u32, u32, u32);
u32 resPackNative_loadArchiveFile(u32, u32, u32, u32);
void resPackNative_openDirectory(u32, u32, u32);
u32 resPackNative_readDirectory(u32, u32, u32);
u32 resPackNative_constructArchiveHolder(u32, u32, u32, u32);

static s32 packStrlen(u32 p) {
    s32 n = 0;
    if (!byteAt(p).get())
        return 0;
    for (;;) {
        n++;
        if (n > 0x40000)
            return 0;
        p++;
        if (!byteAt(p).get())
            return n;
    }
}

/* the RTTI type info of SharcArchiveRes-like resources, initialised on first use */
static u32 typeInfo(u32 guard, u32 info, u32 base) {
    if (!wordAt(guard).get()) {
        wordAt(guard) = 1;
        wordAt(info) = base;
    }
    return info;
}

/* 0260FC98 */
// HD: creates the arena and the two loader threads
void ResMgr_createThreads(u32 self, u32 heap) {

    const u32 buf = call_ptr<u32>(heapVtable(heap)->allocate.get(), heap, 0xA00000u, 0x40u);
    const u32 arena = resPackNative_decompressionArenaConstruct(0u, 0xA00000u, buf);
    const u32 r = self + 0x1123C;
    writeField<u32>(at<Manager>(self)->mArena, arena);
    Local<ThreadArguments> F;
    const u32 f = F.a; /* +0 core mask {4, 2}, +8 name SafeString */
    const u32 names[2] = {kArchiveLoaderThreadName, kPackLoaderThreadName};
    const u32 fns[2] = {0x02613394, 0x026134B4};
    for (u32 i = 0; i < 2; i++) {
        u32 t = resPackNative_allocateWithHeap(0x98u, heap, 4u);
        if (t) {
            writeField<u32>(at<ThreadArguments>(f)->mName.mVtable, kSafeStringVt);
            writeField<u32>(at<ThreadArguments>(f)->mName.mTop, names[i]);
            const u32 d = resPackNative_allocateWithHeap(0x10u, heap, 4u);
            if (d) {
                writeField<u32>(at<ThreadDelegate>(d)->mManager, self);
                writeField<u32>(at<ThreadDelegate>(d)->mVtable, kThreadDelegateVtable);
                writeField<u16>(at<ThreadDelegate>(d)->mOffset, 0);
                writeField<u16>(at<ThreadDelegate>(d)->mVirtualIndex, 0xFFFF);
                writeField<u32>(at<ThreadDelegate>(d)->mFunction, fns[i]);
            }
            const u32 prio = threadPrioritySlot().get() + (i ? 3u : u32(-2));
            t = resPackNative_threadConstruct(t, f + 8, d, heap, prio, 0u, 0x7FFFFFFFu, 0x1800u,
                                              0x20u);
        }
        wordAt(r + 4 + 4 * i) = t;
        writeField<u32>(at<ThreadArguments>(f)->mCoreMask[1], 2);
        writeField<u32>(at<ThreadArguments>(f)->mCoreMask[0], 4);
        resPackNative_threadSetCoreMask(t, f);
        const u32 th = wordAt(r + 4 + 4 * i).get();
        call_ptr<void>(threadVtable(th)->start.get(), th);
    }
    call_ptr<void>(heapVtable(heap)->adjust.get(), heap);
}

/* the temporary resource list object (o) and the FileDevice path buffer (pb) shared by the loaders */
static void tempListCtor(u32 o) {
    writeField<u32>(at<ResourceList>(o)->mCount, 0);
    writeField<u32>(at<ResourceList>(o)->mHead, o);
    writeField<u32>(at<ResourceList>(o)->mNext, 0);
    writeField<u32>(at<ResourceList>(o)->mPrevious, 0);
    resPackNative_disposerConstruct(o + 0x10);
}
static void pathBufCtor(u32 pb) {
    writeField<u32>(at<FixedString<0x104>>(pb)->mString.mTop, pb + 0xC);
    writeField<u8>(at<FixedString<0x104>>(pb)->mBuffer[0x103], 0);
    writeField<u8>(at<FixedString<0x104>>(pb)->mBuffer[0], 0);
    writeField<u32>(at<FixedString<0x104>>(pb)->mCapacity, 0x104);
    writeField<u32>(at<FixedString<0x104>>(pb)->mString.mVtable, kPathBufferVtable);
}

/* 02610B40 */
// HD: loads a file as an archive resource (no decompression)
u32 ResMgr_loadFile(u32 path, u32 heap, u32 align, u32 bufAlign) {

    Local<FileFrame>
        F; /* game test 2026-10-05: pathBufCtor(f+0x84) writes f+0x193; the original frame keeps sp+0..0x194 below its stmw */
    const u32 f = F.a; /* frame offsets as the original's sp offsets */
    const u32 o = f + 0x34, pb = f + 0x84, a = f + 8;
    tempListCtor(o);
    writeField<u8>(at<FileFrame>(f)->mList.mName.mBuffer[0], 0);
    writeField<u32>(at<FileFrame>(f)->mList.mName.mString.mTop, f + 0x60);
    writeField<u32>(at<FileFrame>(f)->mList.mName.mCapacity, 0x20);
    writeField<u8>(at<FileFrame>(f)->mList.mName.mBuffer[0x1F], 0);
    writeField<u32>(at<FileFrame>(f)->mList.mName.mString.mVtable, kFixedString32Vtable);
    writeField<u32>(at<FileFrame>(f)->mList.mDeviceVtable, kResourceListDeviceVtable);
    writeField<u32>(at<FileFrame>(f)->mList.mVtable, kResourceListVtable);
    pathBufCtor(pb);
    const u32 dev = resPackNative_findFileDevice(fileDeviceManagerSlot().get(), path, pb);
    u32 res = 0;
    if (dev) {
        writeField<u32>(at<LoadArg>(a)->mReserved, 0);
        writeField<u32>(at<LoadArg>(a)->mPath.mTop,
                        readField<u32>(at<FixedString<0x104>>(pb)->mString.mTop));
        writeField<u32>(at<LoadArg>(a)->mAlignment, bufAlign);
        writeField<u32>(at<LoadArg>(a)->mInstanceAlignment, bufAlign);
        writeField<u32>(at<LoadArg>(a)->mPath.mVtable, kSafeStringVt);
        writeField<u32>(at<LoadArg>(a)->mInstanceHeap, heap);
        writeField<u32>(at<LoadArg>(a)->mFileDevice, dev);
        writeField<u32>(at<LoadArg>(a)->mResourceList, o);
        writeField<u32>(at<LoadArg>(a)->mHeap, heap);
        writeField<u32>(at<LoadArg>(a)->mBufferAlignment, align);
        writeField<u32>(at<LoadArg>(a)->mFlags, 0);
        res = resPackNative_loadResource(resourceManagerSlot().get(), a);
        if (res) {
            const u32 info =
                typeInfo(kArchiveTypeInfoGuard, kArchiveTypeInfo, kArchiveResourceTypeInfo);
            if (!call_ptr<u32>(resourceVtable(res)->isDerived.get(), res, info))
                res = 0;
            writeField<u32>(at<FileFrame>(f)->mList.mVtable, kResourceListBaseVtable);
            resPackNative_resourceListDestroy(o, 0u);
            return res;
        }
    }
    writeField<u32>(at<FileFrame>(f)->mList.mVtable, kResourceListBaseVtable);
    resPackNative_resourceListDestroy(o, 0u);
    return 0;
}

/* 02612C88 */
// HD: loads a compressed file (factory "sarc", decompressor) as an archive resource
u32 ResMgr_loadCompressed(u32 path, u32 heap, u32 decomp, u32 align, u32 bufAlign) {

    Local<CompressedFrame> F;
    const u32 f = F.a;
    const u32 o = f + 0x3C, pb = f + 0x8C, a = f + 0x10, fac = f + 8;
    tempListCtor(o);
    writeField<u32>(at<CompressedFrame>(f)->mList.mName.mString.mTop, f + 0x68);
    writeField<u8>(at<CompressedFrame>(f)->mList.mName.mBuffer[0], 0);
    writeField<u32>(at<CompressedFrame>(f)->mList.mName.mCapacity, 0x20);
    writeField<u32>(at<CompressedFrame>(f)->mList.mName.mString.mVtable, kFixedString32Vtable);
    writeField<u8>(at<CompressedFrame>(f)->mList.mName.mBuffer[0x1F], 0);
    writeField<u32>(at<CompressedFrame>(f)->mList.mDeviceVtable, kResourceListDeviceVtable);
    writeField<u32>(at<CompressedFrame>(f)->mList.mVtable, kResourceListVtable);
    pathBufCtor(pb);
    const u32 dev = resPackNative_findFileDevice(fileDeviceManagerSlot().get(), path, pb);
    if (dev) {
        writeField<u32>(at<LoadArg>(a)->mFileDevice, dev);
        writeField<u32>(at<LoadArg>(a)->mPath.mTop,
                        readField<u32>(at<FixedString<0x104>>(pb)->mString.mTop));
        writeField<u32>(at<LoadArg>(a)->mReserved, 0);
        writeField<u32>(at<LoadArg>(a)->mInstanceAlignment, bufAlign);
        writeField<u32>(at<SafeString>(fac)->mTop, kArchiveFactory);
        writeField<u32>(at<SafeString>(fac)->mVtable, kSafeStringVt);
        writeField<u32>(at<LoadArg>(a)->mBufferAlignment, align);
        writeField<u32>(at<LoadArg>(a)->mInstanceHeap, heap);
        writeField<u32>(at<LoadArg>(a)->mResourceList, o);
        writeField<u32>(at<LoadArg>(a)->mFlags, 0);
        writeField<u32>(at<LoadArg>(a)->mPath.mVtable, kSafeStringVt);
        writeField<u32>(at<LoadArg>(a)->mAlignment, bufAlign);
        writeField<u32>(at<LoadArg>(a)->mHeap, heap);
        u32 res =
            resPackNative_loadResourceDecompressed(resourceManagerSlot().get(), a, fac, decomp);
        if (res) {
            const u32 info =
                typeInfo(kArchiveTypeInfoGuard, kArchiveTypeInfo, kArchiveResourceTypeInfo);
            if (!call_ptr<u32>(resourceVtable(res)->isDerived.get(), res, info))
                res = 0;
            resPackNative_invalidateArchiveCache(readField<u32>(at<ArchiveResource>(res)->mBuffer),
                                                 readField<u32>(at<ArchiveResource>(res)->mSize));
            writeField<u32>(at<CompressedFrame>(f)->mList.mVtable, kResourceListBaseVtable);
            resPackNative_resourceListDestroy(o, 0u);
            return res;
        }
    }
    writeField<u32>(at<CompressedFrame>(f)->mList.mVtable, kResourceListBaseVtable);
    resPackNative_resourceListDestroy(o, 0u);
    return 0;
}

/* 02610820 */
// HD: loads a stage pack file and registers it in the pack entry map
u32 ResMgr_loadStagePack(u32 self, u32 name, u32 path, u32 heap, u32 align) {

    Local<StageFrame> F;
    const u32 f = F.a;
    const u32 o = f + 0x38, pb = f + 0x88, a = f + 0xC;
    pathBufCtor(pb);
    const u32 dev = resPackNative_findFileDevice(fileDeviceManagerSlot().get(), path, pb);
    if (!dev)
        return 0;
    writeField<u32>(at<ResourceList>(o)->mNext, 0);
    writeField<u32>(at<ResourceList>(o)->mPrevious, 0);
    writeField<u32>(at<ResourceList>(o)->mHead, o);
    writeField<u32>(at<ResourceList>(o)->mCount, 0);
    resPackNative_disposerConstruct(o + 0x10);
    writeField<u8>(at<StageFrame>(f)->mList.mName.mBuffer[0], 0);
    writeField<u32>(at<LoadArg>(a)->mFlags, 0);
    writeField<u32>(at<LoadArg>(a)->mReserved, 0);
    writeField<u32>(at<StageFrame>(f)->mList.mDeviceVtable, kPackResourceListDeviceVtable);
    writeField<u32>(at<LoadArg>(a)->mPath.mTop,
                    readField<u32>(at<FixedString<0x104>>(pb)->mString.mTop));
    writeField<u32>(at<LoadArg>(a)->mHeap, heap);
    writeField<u32>(at<LoadArg>(a)->mBufferAlignment, align);
    writeField<u32>(at<LoadArg>(a)->mPath.mVtable, kSafeStringVt);
    writeField<u32>(at<LoadArg>(a)->mFileDevice, dev);
    writeField<u32>(at<StageFrame>(f)->mList.mName.mString.mVtable, kFixedString32Vtable);
    writeField<u32>(at<StageFrame>(f)->mList.mName.mString.mTop, f + 0x64);
    writeField<u32>(at<LoadArg>(a)->mAlignment, 0x2000);
    writeField<u32>(at<LoadArg>(a)->mInstanceHeap, heap);
    writeField<u32>(at<StageFrame>(f)->mList.mVtable, kPackResourceListVtable);
    writeField<u8>(at<StageFrame>(f)->mList.mName.mBuffer[0x1F], 0);
    writeField<u32>(at<LoadArg>(a)->mInstanceAlignment, 0x2000);
    writeField<u32>(at<LoadArg>(a)->mResourceList, o);
    writeField<u32>(at<StageFrame>(f)->mList.mName.mCapacity, 0x20);
    u32 res = resPackNative_loadResource(resourceManagerSlot().get(), a);
    const u32 info = typeInfo(kPackTypeInfoGuard, kPackTypeInfo, kArchiveResourceTypeInfo);
    if (!res || !call_ptr<u32>(resourceVtable(res)->isDerived.get(), res, info) || !res) {
        writeField<u32>(at<StageFrame>(f)->mList.mVtable, kResourceListBaseVtable);
        resPackNative_resourceListDestroy(o, 0u);
        return 0;
    }
    u32 holder = resPackNative_allocateWithHeap(0x54u, heap, 4u);
    if (holder)
        holder = resPackNative_constructPackHolder(holder, name, res, heap);
    call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
    const u32 fn = stringVtable(name)->assureTermination.get();
    const u32 top = readField<u32>(at<FixedString<1>>(name)->mString.mTop);
    call_ptr<void>(fn, name);
    const s32 len = packStrlen(readField<u32>(at<FixedString<1>>(name)->mString.mTop));
    const u32 h = resPackNative_hashCRC(top, len);
    const u32 tree = self + 0xD228;
    if (s32(readField<u32>(at<TreeHeader>(tree)->mCount)) <
        s32(readField<u32>(at<TreeHeader>(tree)->mCapacity))) {
        const u32 node = readField<u32>(at<TreeHeader>(tree)->mFreeHead);
        if (node)
            writeField<u32>(at<TreeHeader>(tree)->mFreeHead,
                            readField<u32>(at<PackNode>(node)->mLeft));
        if (node) {
            writeField<u32>(at<PackNode>(node)->mHash, h);
            writeField<u32>(at<PackNode>(node)->mTree, tree);
            writeField<u32>(at<PackNode>(node)->mLeft, 0);
            writeField<u8>(at<PackNode>(node)->mColor, 1);
            writeField<u32>(at<PackNode>(node)->mHashCopy, h);
            writeField<u32>(at<PackNode>(node)->mHolder, holder);
            writeField<u32>(at<PackNode>(node)->mValueVtable, kPackMapValueVtable);
            writeField<u32>(at<PackNode>(node)->mRight, 0);
        }
        const u32 cnt = readField<u32>(at<TreeHeader>(tree)->mCount) + 1;
        const u32 root = readField<u32>(at<TreeHeader>(tree)->mRoot);
        writeField<u32>(at<TreeHeader>(tree)->mCount, cnt);
        const u32 r = resPackNative_insertPackNode(tree, root, node);
        writeField<u32>(at<TreeHeader>(tree)->mRoot, r);
        writeField<u8>(at<ArchiveNode>(r)->mColor, 0);
    } else {
        Local<be<u32>> key;
        *key = h;
        const u32 n =
            resPackNative_findPackNode(tree, readField<u32>(at<TreeHeader>(tree)->mRoot), key.a);
        if (n && n + 0x18)
            writeField<u32>(at<PackNode>(n)->mHolder, holder);
    }
    writeField<u32>(at<StageFrame>(f)->mList.mVtable, kResourceListBaseVtable);
    resPackNative_resourceListDestroy(o, 0u);
    return 1;
}

/* ---- pack scanning (02610CF8 arc packs, 02611864 SZS packs) ---- */

/* copies the string object src into a buffer whose capacity is at capAddr (dst read before) */
static void packCopy(u32 dst, u32 capAddr, u32 src) {
    call_ptr<void>(stringVtable(src)->assureTermination.get(), src);
    s32 n = packStrlen(readField<u32>(at<FixedString<1>>(src)->mString.mTop));
    const s32 size = s32(wordAt(capAddr).get());
    const u32 fn = stringVtable(src)->assureTermination.get();
    if (n >= size)
        n = size - 1;
    call_ptr<void>(fn, src);
    resPackNative_copyBytes(dst, readField<u32>(at<FixedString<1>>(src)->mString.mTop), u32(n), 0u);
    byteAt(dst + n) = 0;
}

/* sead::SafeString strlen with the overflow flag */
static s32 packStrlenOver(u32 p, bool& over) {
    over = false;
    s32 n = 0;
    if (!byteAt(p).get())
        return 0;
    for (;;) {
        n++;
        if (n > 0x40000) {
            over = true;
            return 0;
        }
        p++;
        if (!byteAt(p).get())
            return n;
    }
}

/* rfind of the "." string object d (at f+0x14) in the entry name (object at ent); T at f+0x1C */
static s32 findDot(u32 f, u32 ent, u32 dotStr) {
    const u32 d = f + 0x14, tt = f + 0x1C;
    writeField<u32>(at<FixedString<1>>(d)->mString.mVtable, kSafeStringVt);
    writeField<u32>(at<FixedString<1>>(d)->mString.mTop, dotStr);
    call_ptr<void>(stringVtable(ent)->assureTermination.get(), ent);
    bool over;
    const s32 len = packStrlenOver(readField<u32>(at<FixedString<1>>(ent)->mString.mTop), over);
    call_ptr<void>(stringVtable(d)->assureTermination.get(), d);
    bool dOver;
    s32 dlen = packStrlenOver(readField<u32>(at<FixedString<1>>(d)->mString.mTop), dOver);
    s32 pos;
    if (dOver) {
        dlen = 0;
        pos = len;
    } else
        pos = len - dlen;
    if (pos < 0)
        return -1;
    s32 count = pos + 1;
    for (;;) {
        writeField<u32>(at<FixedString<1>>(tt)->mString.mVtable, kSafeStringVt);
        writeField<u32>(at<FixedString<1>>(tt)->mString.mTop,
                        readField<u32>(at<FixedString<1>>(ent)->mString.mTop) + u32(pos));
        resPackNative_stringViewConstruct(tt);
        call_ptr<void>(stringVtable(tt)->assureTermination.get(), tt);
        const u32 fn = stringVtable(d)->assureTermination.get();
        const u32 ttop = readField<u32>(at<FixedString<1>>(tt)->mString.mTop);
        call_ptr<void>(fn, d);
        if (ttop == readField<u32>(at<FixedString<1>>(d)->mString.mTop) || dlen <= 0)
            return pos;
        u32 a = readField<u32>(at<FixedString<1>>(d)->mString.mTop),
            b = readField<u32>(at<FixedString<1>>(tt)->mString.mTop);
        bool match = false;
        for (s32 k = dlen;;) {
            const u32 tb = byteAt(b).get();
            if (!tb) {
                match = !readField<u8>(at<LoadArg>(a)->mPath.mTop);
                break;
            }
            const u32 ta = readField<u8>(at<LoadArg>(a)->mPath.mTop);
            if (!ta || ta != tb)
                break;
            a++;
            b++;
            if (--k == 0) {
                match = true;
                break;
            }
        }
        if (match)
            return pos;
        count--;
        pos--;
        if (!count)
            return pos;
    }
}

/* is the extension after pos the string extStr? (E at f+0x14, the extension string at f+0x24) */
static bool isExtension(u32 f, u32 ent, s32 pos, u32 extStr) {
    const u32 e = f + 0x14, x = f + 0x24;
    call_ptr<void>(stringVtable(ent)->assureTermination.get(), ent);
    const u32 top = readField<u32>(at<FixedString<1>>(ent)->mString.mTop);
    const s32 nlen = packStrlen(top);
    const s32 p1 = pos + 1;
    const u32 et = (p1 < 0 || p1 > nlen) ? wordAt(kEmptyStringTopSlot).get() : top + u32(p1);
    writeField<u32>(at<FixedString<1>>(e)->mString.mVtable, kSafeStringVt);
    writeField<u32>(at<FixedString<1>>(x)->mString.mVtable, kSafeStringVt);
    writeField<u32>(at<FixedString<1>>(e)->mString.mTop, et);
    writeField<u32>(at<FixedString<1>>(x)->mString.mTop, extStr);
    resPackNative_stringViewConstruct(e);
    call_ptr<void>(stringVtable(e)->assureTermination.get(), e);
    const u32 fn = stringVtable(x)->assureTermination.get();
    const u32 etop = readField<u32>(at<FixedString<1>>(e)->mString.mTop);
    call_ptr<void>(fn, x);
    const u32 xs = readField<u32>(at<FixedString<1>>(x)->mString.mTop);
    if (etop == xs)
        return true;
    const u32 a = readField<u32>(at<FixedString<1>>(e)->mString.mTop);
    for (u32 i = 0; i < 0x40001; i++) {
        const u32 c = byteAt(a + i).get(), y = byteAt(xs + i).get();
        if (c != y)
            return false;
        if (!c)
            return true;
    }
    return false;
}

/* cuts the entry name at pos (when inside its buffer) */
static void cutAt(u32 ent, s32 pos) {
    if (pos < s32(readField<u32>(at<FixedString<1>>(ent)->mCapacity))) {
        const u32 top = readField<u32>(at<FixedString<1>>(ent)->mString.mTop);
        byteAt(top + u32(pos < 0 ? 0 : pos)) = 0;
    } else {
        call_ptr<void>(stringVtable(ent)->assureTermination.get(), ent);
        packStrlen(readField<u32>(at<FixedString<1>>(ent)->mString.mTop));
    }
}

static void sleepTicks(u32 t, u32 ticks) {
    wordAt(t + 8) = 0;
    wordAt(t + 0xC) = ticks;
    wordAt(t) = 0;
    wordAt(t + 4) = ticks;
    resPackNative_threadSleep(t);
}

/* the archive map insertion of hd_res_mgr.cpp (key: the string object name) */
static void packInsertNamed(u32 self, u32 name, u32 holder) {
    const u32 tree = self + 0x14;
    if (s32(readField<u32>(at<TreeHeader>(tree)->mCount)) >=
        s32(readField<u32>(at<TreeHeader>(tree)->mCapacity))) {
        const u32 n =
            resPackNative_findArchiveNode(tree, readField<u32>(at<TreeHeader>(tree)->mRoot), name);
        if (n && n + 0x18)
            writeField<u32>(at<ArchiveNode>(n)->mHolder, holder);
        return;
    }
    const u32 node = readField<u32>(at<TreeHeader>(tree)->mFreeHead);
    if (node)
        writeField<u32>(at<TreeHeader>(tree)->mFreeHead,
                        readField<u32>(at<ArchiveNode>(node)->mLeft));
    if (node) {
        writeField<u32>(at<ArchiveNode>(node)->mKey.mVtable, kSafeStringVt);
        writeField<u8>(at<ArchiveNode>(node)->mColor, 1);
        writeField<u32>(at<ArchiveNode>(node)->mValueVtable, kArchiveMapValueVtable);
        writeField<u32>(at<ArchiveNode>(node)->mKey.mTop, kEmptyText);
        writeField<u32>(at<ArchiveNode>(node)->mTree, tree);
        writeField<u32>(at<ArchiveNode>(node)->mHolder, holder);
        writeField<u8>(at<ArchiveNode>(node)->mBufferFlag, 0);
        writeField<u32>(at<ArchiveNode>(node)->mLeft, 0);
        writeField<u32>(at<ArchiveNode>(node)->mRight, 0);
        const u32 buf = node + 0x20;
        call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
        bool over;
        s32 n = packStrlenOver(readField<u32>(at<FixedString<1>>(name)->mString.mTop), over);
        if (over) {
            call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
            resPackNative_copyBytes(buf, readField<u32>(at<FixedString<1>>(name)->mString.mTop), 0u,
                                    0u);
            byteAt(buf) = 0;
        } else {
            if (n >= 0x31)
                n = 0x30;
            call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
            resPackNative_copyBytes(buf, readField<u32>(at<FixedString<1>>(name)->mString.mTop),
                                    u32(n), 0u);
            byteAt(buf + n) = 0;
        }
        writeField<u32>(at<ArchiveNode>(node)->mKey.mTop, buf);
    }
    const u32 cnt = readField<u32>(at<TreeHeader>(tree)->mCount) + 1;
    const u32 root = readField<u32>(at<TreeHeader>(tree)->mRoot);
    writeField<u32>(at<TreeHeader>(tree)->mCount, cnt);
    const u32 r = resPackNative_insertArchiveNode(tree, root, node);
    writeField<u32>(at<TreeHeader>(tree)->mRoot, r);
    writeField<u8>(at<ArchiveNode>(r)->mColor, 0);
}

/* the stack objects of both scanners: ArchiveFileDevice dev over arc (device name from devName),
 * DirectoryHandle h opened at the root path */
static void deviceCtor(u32 f, u32 dev, u32 nameObj, u32 devName, u32 arc, u32 saveVt) {
    writeField<u32>(at<FixedString<1>>(nameObj)->mString.mVtable, kSafeStringVt);
    writeField<u32>(at<FixedString<1>>(nameObj)->mString.mTop, devName);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mPrevious, 0);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mNext, 0);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mCount, 0);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mHead, dev);
    resPackNative_disposerConstruct(dev + 0x10);
    writeField<u8>(at<ArchiveFileDevice>(dev)->mName.mBuffer[0x1F], 0);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mDeviceVtable, kFileDeviceVtable);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mName.mString.mTop, dev + 0x2C);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mVtable, kFileDeviceDisposerVtable);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mName.mCapacity, 0x20);
    writeField<u8>(at<ArchiveFileDevice>(dev)->mEnabled, 1);
    writeField<u8>(at<ArchiveFileDevice>(dev)->mName.mBuffer[0], 0);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mName.mString.mVtable, kFixedString32Vtable);
    packCopy(dev + 0x2C, dev + 0x28, nameObj);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mArchive, arc);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mDeviceVtable, kArchiveFileDeviceVtable);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mVtable, kArchiveFileDeviceDisposerVtable);
    wordAt(saveVt) = kArchiveFileDeviceDisposerVtable;
}

static void closeHandle(u32 f, u32 h, u32 dev, u32 saveHVt, u32 saveDVt) {
    const u32 inner = readField<u32>(at<DirectoryHandle>(h)->mDevice);
    writeField<u32>(at<DirectoryHandle>(h)->mVtable, wordAt(saveHVt).get());
    if (inner)
        resPackNative_closeDirectory(inner, h);
    resPackNative_disposerDestroy(h, 0u);
    writeField<u32>(at<ArchiveFileDevice>(dev)->mVtable, wordAt(saveDVt).get());
    resPackNative_archiveDeviceDestroy(dev, 0u);
}

/* 02610CF8 */
// HD: loads a pack archive and registers its ".szs" members as sarc archives by name
u32 ResMgr_loadPack(u32 self, u32 name, u32 heap) {

    Local<PackFrame> F;
    const u32 f = F.a;
    const u32 s = f + 0x48, dev = f + 0x218, h = f + 0x1E0, ent = f + 0xD0, la = f + 0xA4,
              lst = f + 0x270;
    writeField<u32>(at<FixedString<1>>(s)->mString.mTop, kArchiveTemporaryHeapName);
    writeField<u32>(at<FixedString<1>>(s)->mString.mVtable, kSafeStringVt);
    const u32 tmpHeap =
        resPackNative_createExpHeap(0u, s, gameHeaps()->mPackHeap.get(), 0xFFFFFFFFu, 0u);
    const u32 arc = resPackNative_loadArchiveFile(
        name, tmpHeap, readField<u32>(at<Manager>(self)->mLoadAlignment), 0x2000u);
    call_ptr<void>(heapVtable(tmpHeap)->adjust.get(), tmpHeap);
    writeField<u32>(at<FixedString<1>>(s)->mString.mVtable, kSafeStringVt);
    writeField<u32>(at<FixedString<1>>(s)->mString.mTop, kArchiveResourceHeapName);
    const u32 resHeap = resPackNative_createExpHeap(0u, s, heap, 1u, 0u);
    deviceCtor(f, dev, f + 0x9C, kArchiveDeviceName, arc, f + 0x10);
    resPackNative_disposerConstruct(h);
    writeField<u32>(at<DirectoryHandle>(h)->mDevice, 0);
    writeField<u32>(at<FixedString<1>>(s)->mString.mVtable, kSafeStringVt);
    writeField<u32>(at<PackFrame>(f)->mHandleVtable, kDirectoryHandleVtable);
    writeField<u32>(at<DirectoryHandle>(h)->mIsOpen, 0);
    writeField<u32>(at<DirectoryHandle>(h)->mVtable, kDirectoryHandleVtable);
    writeField<u32>(at<FixedString<1>>(s)->mString.mTop, kArchiveRootPath);
    resPackNative_openDirectory(dev, h, s);
    if (!readField<u32>(at<DirectoryHandle>(h)->mIsOpen)) {
        call_ptr<void>(heapVtable(tmpHeap)->destroy.get(), tmpHeap);
        closeHandle(f, h, dev, f + 8, f + 0x10);
        return 0;
    }
    writeField<u32>(at<ResourceList>(lst)->mNext, 0);
    writeField<u32>(at<ResourceList>(lst)->mPrevious, 0);
    writeField<u8>(at<DirectoryEntry>(ent)->mName.mBuffer[0xFF], 0);
    writeField<u8>(at<DirectoryEntry>(ent)->mName.mBuffer[0], 0);
    writeField<u32>(at<DirectoryEntry>(ent)->mName.mString.mVtable, kDirectoryEntryStringVtable);
    writeField<u8>(at<DirectoryEntry>(ent)->mIsDirectory, 0);
    writeField<u32>(at<DirectoryEntry>(ent)->mName.mString.mTop, ent + 0xC);
    writeField<u32>(at<ResourceList>(lst)->mHead, lst);
    writeField<u32>(at<ResourceList>(lst)->mCount, 0);
    writeField<u32>(at<DirectoryEntry>(ent)->mName.mCapacity, 0x100);
    resPackNative_disposerConstruct(lst + 0x10);
    writeField<u8>(at<ResourceList>(lst)->mName.mBuffer[0x1F], 0);
    writeField<u32>(at<ResourceList>(lst)->mName.mString.mTop, lst + 0x2C);
    wordAt(la + 4) = kSafeStringVt;
    wordAt(la + 0x1C) = 0;
    wordAt(la + 0xC) = resHeap;
    writeField<u8>(at<ResourceList>(lst)->mName.mBuffer[0], 0);
    writeField<u32>(at<PackFrame>(f)->mListVtable, kResourceListBaseVtable);
    wordAt(la + 0x20) = lst;
    wordAt(la + 0x14) = 0x2000;
    writeField<u32>(at<ResourceList>(lst)->mVtable, kResourceListVtable);
    writeField<u32>(at<ResourceList>(lst)->mName.mString.mVtable, kFixedString32Vtable);
    wordAt(la + 0x18) = 0;
    writeField<u32>(at<ResourceList>(lst)->mDeviceVtable, kResourceListDeviceVtable);
    wordAt(la + 8) = resHeap;
    wordAt(la + 0x10) = 0x2000;
    wordAt(la + 0x24) = dev;
    wordAt(la) = kEmptyText;
    wordAt(la + 0x28) = 0x40000;
    writeField<u32>(at<ResourceList>(lst)->mName.mCapacity, 0x20);
    while (resPackNative_readDirectory(h, ent, 1u)) {
        if (readField<u8>(at<DirectoryEntry>(ent)->mIsDirectory))
            continue;
        const s32 pos = findDot(f, ent, kArchiveDot);
        const u32 k = f + 0x50;
        byteAt(k + 0x4B) = 0;
        writeField<u32>(at<FixedString<1>>(k)->mString.mTop, k + 0xC);
        writeField<u32>(at<FixedString<1>>(k)->mCapacity, 0x40);
        byteAt(k + 0xC) = 0;
        writeField<u32>(at<FixedString<1>>(k)->mString.mVtable, kFixedString64Vtable);
        bool szs = false;
        if (pos >= 0)
            szs = isExtension(f, ent, pos, kArchiveExtension);
        if (szs) {
            const u32 arena = readField<u32>(at<Manager>(self)->mArena);
            wordAt(la) = readField<u32>(at<DirectoryEntry>(ent)->mName.mString.mTop);
            writeField<u32>(at<PackFrame>(f)->mFactory.mTop, kPackArchiveFactory);
            writeField<u32>(at<PackFrame>(f)->mFactory.mVtable, kSafeStringVt);
            u32 res = resPackNative_loadResourceDecompressed(resourceManagerSlot().get(), la,
                                                             f + 0x2C, arena);
            const u32 info =
                typeInfo(kArchiveTypeInfoGuard, kArchiveTypeInfo, kArchiveResourceTypeInfo);
            if (!res || !call_ptr<u32>(resourceVtable(res)->isDerived.get(), res, info) || !res)
                continue;
            cutAt(ent, pos);
            const u32 dst = readField<u32>(at<FixedString<1>>(k)->mString.mTop);
            packCopy(dst, k + 8, ent);
            resPackNative_invalidateArchiveCache(readField<u32>(at<ArchiveResource>(res)->mBuffer),
                                                 readField<u32>(at<ArchiveResource>(res)->mSize));
            u32 holder = resPackNative_allocateWithHeap(0x54u, resHeap, 4u);
            if (holder)
                holder = resPackNative_constructArchiveHolder(holder, k, res, tmpHeap);
            packInsertNamed(self, k, holder);
        }
        sleepTicks(f + 0x38, 0x1E);
    }
    resPackNative_closeDirectory(dev, h);
    call_ptr<void>(heapVtable(resHeap)->adjust.get(), resHeap);
    call_ptr<void>(heapVtable(tmpHeap)->destroy.get(), tmpHeap);
    writeField<u32>(at<ResourceList>(lst)->mVtable, readField<u32>(at<PackFrame>(f)->mListVtable));
    resPackNative_resourceListDestroy(lst, 0u);
    closeHandle(f, h, dev, f + 8, f + 0x10);
    return 1;
}

/* 02611864 */
// HD: loads an SZS permanent pack and registers its ".szs" members (still compressed) in the pack entry map by name hash; the pack heap creation is retried every 30000000 ticks
u32 ResMgr_loadSzsPack(u32 self, u32 name, u32 heap) {

    Local<SzsPackFrame> F;
    const u32 f = F.a;
    const u32 dev = f + 0x228, h = f + 0x1F0, ent = f + 0xE0, la = f + 0xB4, lst = f + 0x280,
              k = f + 0x60;
    const u32 parent = gameHeaps()->mPackHeap.get();
    u32 packHeap;
    for (;;) {
        writeField<u32>(at<SzsPackFrame>(f)->mHeapName.mVtable, kSafeStringVt);
        writeField<u32>(at<SzsPackFrame>(f)->mHeapName.mTop, kSzsPackHeapName);
        packHeap = resPackNative_createExpHeap(0x3200000u, f + 0x2C, parent, 0xFFFFFFFFu, 0u);
        if (packHeap)
            break;
        writeField<u32>(at<SzsPackFrame>(f)->mRetrySleep.mLow, 0x1C9C380);
        wordAt(f + 0x4C) = 0x1C9C380;
        writeField<u32>(at<SzsPackFrame>(f)->mRetrySleep.mHigh, 0);
        wordAt(f + 0x48) = 0;
        resPackNative_threadSleep(f + 0x48);
    }
    const u32 arc = resPackNative_loadArchiveFile(
        name, packHeap, readField<u32>(at<Manager>(self)->mLoadAlignment), 0x2000u);
    call_ptr<void>(heapVtable(packHeap)->adjust.get(), packHeap);
    writeField<u32>(at<SzsPackFrame>(f)->mPath.mVtable, kSafeStringVt);
    writeField<u32>(at<SzsPackFrame>(f)->mPath.mTop, kSzsResourceHeapName);
    const u32 resHeap = resPackNative_createExpHeap(0u, f + 0xAC, heap, 1u, 0u);
    deviceCtor(f, dev, f + 0x58, kSzsDeviceName, arc, f + 0xC);
    resPackNative_disposerConstruct(h);
    writeField<u32>(at<DirectoryHandle>(h)->mDevice, 0);
    writeField<u32>(at<SzsPackFrame>(f)->mPath.mVtable, kSafeStringVt);
    writeField<u32>(at<SzsPackFrame>(f)->mHandleVtable, kDirectoryHandleVtable);
    writeField<u32>(at<DirectoryHandle>(h)->mIsOpen, 0);
    writeField<u32>(at<DirectoryHandle>(h)->mVtable, kDirectoryHandleVtable);
    writeField<u32>(at<SzsPackFrame>(f)->mPath.mTop, kSzsRootPath);
    resPackNative_openDirectory(dev, h, f + 0xAC);
    if (!readField<u32>(at<DirectoryHandle>(h)->mIsOpen)) {
        call_ptr<void>(heapVtable(packHeap)->destroy.get(), packHeap);
        closeHandle(f, h, dev, f + 0x10, f + 0xC);
        return 0;
    }
    writeField<u32>(at<ResourceList>(lst)->mNext, 0);
    writeField<u8>(at<DirectoryEntry>(ent)->mName.mBuffer[0xFF], 0);
    writeField<u32>(at<ResourceList>(lst)->mPrevious, 0);
    writeField<u8>(at<DirectoryEntry>(ent)->mName.mBuffer[0], 0);
    writeField<u32>(at<DirectoryEntry>(ent)->mName.mString.mVtable, kDirectoryEntryStringVtable);
    writeField<u8>(at<DirectoryEntry>(ent)->mIsDirectory, 0);
    writeField<u32>(at<DirectoryEntry>(ent)->mName.mString.mTop, ent + 0xC);
    writeField<u32>(at<ResourceList>(lst)->mHead, lst);
    writeField<u32>(at<ResourceList>(lst)->mCount, 0);
    writeField<u32>(at<DirectoryEntry>(ent)->mName.mCapacity, 0x100);
    resPackNative_disposerConstruct(lst + 0x10);
    writeField<u8>(at<ResourceList>(lst)->mName.mBuffer[0x1F], 0);
    writeField<u32>(at<ResourceList>(lst)->mName.mString.mTop, lst + 0x2C);
    wordAt(la + 0xC) = resHeap;
    wordAt(la + 0x18) = 0;
    wordAt(la + 0x28) = 0;
    writeField<u8>(at<ResourceList>(lst)->mName.mBuffer[0], 0);
    writeField<u32>(at<SzsPackFrame>(f)->mListVtable, kResourceListBaseVtable);
    writeField<u32>(at<ResourceList>(lst)->mName.mCapacity, 0x20);
    wordAt(la + 8) = resHeap;
    wordAt(la) = kEmptyText;
    wordAt(la + 4) = kSafeStringVt;
    wordAt(la + 0x20) = lst;
    wordAt(la + 0x10) = 0x2000;
    writeField<u32>(at<ResourceList>(lst)->mName.mString.mVtable, kFixedString32Vtable);
    wordAt(la + 0x24) = dev;
    wordAt(la + 0x1C) = 0;
    writeField<u32>(at<ResourceList>(lst)->mDeviceVtable, kPackResourceListDeviceVtable);
    wordAt(la + 0x14) = 0x2000;
    writeField<u32>(at<ResourceList>(lst)->mVtable, kPackResourceListVtable);
    while (resPackNative_readDirectory(h, ent, 1u)) {
        if (readField<u8>(at<DirectoryEntry>(ent)->mIsDirectory))
            continue;
        const s32 pos = findDot(f, ent, kSzsDot);
        byteAt(k + 0x4B) = 0;
        byteAt(k + 0xC) = 0;
        writeField<u32>(at<FixedString<1>>(k)->mCapacity, 0x40);
        writeField<u32>(at<FixedString<1>>(k)->mString.mTop, k + 0xC);
        writeField<u32>(at<FixedString<1>>(k)->mString.mVtable, kFixedString64Vtable);
        if (pos < 0 || !isExtension(f, ent, pos, kSzsExtension))
            continue;
        wordAt(la) = readField<u32>(at<DirectoryEntry>(ent)->mName.mString.mTop);
        const u32 res = resPackNative_loadResource(resourceManagerSlot().get(), la);
        const u32 info = typeInfo(kPackTypeInfoGuard, kPackTypeInfo, kArchiveResourceTypeInfo);
        if (!res || !call_ptr<u32>(resourceVtable(res)->isDerived.get(), res, info) || !res)
            continue;
        cutAt(ent, pos);
        const u32 dst = readField<u32>(at<FixedString<1>>(k)->mString.mTop);
        packCopy(dst, k + 8, ent);
        u32 holder = resPackNative_allocateWithHeap(0x54u, resHeap, 4u);
        if (holder)
            holder = resPackNative_constructPackHolder(holder, k, res, 0u);
        call_ptr<void>(stringVtable(k)->assureTermination.get(), k);
        const u32 fn = stringVtable(k)->assureTermination.get();
        const u32 top = readField<u32>(at<FixedString<1>>(k)->mString.mTop);
        call_ptr<void>(fn, k);
        const s32 klen = packStrlen(readField<u32>(at<FixedString<1>>(k)->mString.mTop));
        const u32 hh = resPackNative_hashCRC(top, klen);
        const u32 tree = self + 0xD228;
        if (s32(readField<u32>(at<TreeHeader>(tree)->mCount)) <
            s32(readField<u32>(at<TreeHeader>(tree)->mCapacity))) {
            const u32 node = readField<u32>(at<TreeHeader>(tree)->mFreeHead);
            if (node)
                writeField<u32>(at<TreeHeader>(tree)->mFreeHead,
                                readField<u32>(at<PackNode>(node)->mLeft));
            if (node) {
                writeField<u32>(at<PackNode>(node)->mLeft, 0);
                writeField<u32>(at<PackNode>(node)->mHash, hh);
                writeField<u32>(at<PackNode>(node)->mValueVtable, kPackMapValueVtable);
                writeField<u32>(at<PackNode>(node)->mHolder, holder);
                writeField<u32>(at<PackNode>(node)->mTree, tree);
                writeField<u32>(at<PackNode>(node)->mRight, 0);
                writeField<u8>(at<PackNode>(node)->mColor, 1);
                writeField<u32>(at<PackNode>(node)->mHashCopy, hh);
            }
            const u32 cnt = readField<u32>(at<TreeHeader>(tree)->mCount) + 1;
            const u32 root = readField<u32>(at<TreeHeader>(tree)->mRoot);
            writeField<u32>(at<TreeHeader>(tree)->mCount, cnt);
            const u32 r = resPackNative_insertPackNode(tree, root, node);
            writeField<u32>(at<TreeHeader>(tree)->mRoot, r);
            writeField<u8>(at<ArchiveNode>(r)->mColor, 0);
        } else {
            writeField<u32>(at<SzsPackFrame>(f)->mHash, hh);
            const u32 n = resPackNative_findPackNode(
                tree, readField<u32>(at<TreeHeader>(tree)->mRoot), f + 0x34);
            if (n && n + 0x18)
                writeField<u32>(at<PackNode>(n)->mHolder, holder);
        }
        sleepTicks(f + 0x50, 1);
    }
    resPackNative_closeDirectory(dev, h);
    call_ptr<void>(heapVtable(resHeap)->adjust.get(), resHeap);
    call_ptr<void>(heapVtable(packHeap)->destroy.get(), packHeap);
    writeField<u32>(at<ResourceList>(lst)->mVtable,
                    readField<u32>(at<SzsPackFrame>(f)->mListVtable));
    resPackNative_resourceListDestroy(lst, 0u);
    closeHandle(f, h, dev, f + 0x10, f + 0xC);
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0275E9D0 u32 resPackNative_decompressionArenaConstruct(u32, u32, u32)
ext 0273B050 u32 resPackNative_allocateWithHeap(u32, u32, u32)
ext 0275FBD8 u32 resPackNative_threadConstruct(u32, u32, u32, u32, u32, u32, u32, u32, u32)
ext 02760C4C void resPackNative_threadSetCoreMask(u32, u32)
ext 02752A84 void resPackNative_disposerConstruct(u32)
ext 02741C24 u32 resPackNative_findFileDevice(u32, u32, u32)
ext 0275DB94 u32 resPackNative_loadResource(u32, u32)
ext 0275CFD0 void resPackNative_resourceListDestroy(u32, u32)
ext 0275DD38 u32 resPackNative_loadResourceDecompressed(u32, u32, u32, u32)
ext C00088A0 void resPackNative_invalidateArchiveCache(u32, u32)
ext 026106B4 u32 resPackNative_constructPackHolder(u32, u32, u32, u32)
ext 0273B264 u32 resPackNative_hashCRC(u32, u32)
ext 0260AD08 u32 resPackNative_insertPackNode(u32, u32, u32)
ext 0260ACC8 u32 resPackNative_findPackNode(u32, u32, u32)
ext C0009988 void resPackNative_copyBytes(u32, u32, u32, u32)
ext 0261396C void resPackNative_stringViewConstruct(u32)
ext 02760C3C void resPackNative_threadSleep(u32)
ext 02613F20 u32 resPackNative_findArchiveNode(u32, u32, u32)
ext 0261401C u32 resPackNative_insertArchiveNode(u32, u32, u32)
ext 02741088 void resPackNative_closeDirectory(u32, u32)
ext 02752BEC void resPackNative_disposerDestroy(u32, u32)
ext 02740A38 void resPackNative_archiveDeviceDestroy(u32, u32)
ext 02754B38 u32 resPackNative_createExpHeap(u32, u32, u32, u32, u32)
ext 02610B40 u32 resPackNative_loadArchiveFile(u32, u32, u32, u32)
ext 02740FDC void resPackNative_openDirectory(u32, u32, u32)
ext 02741604 u32 resPackNative_readDirectory(u32, u32, u32)
ext 026100D0 u32 resPackNative_constructArchiveHolder(u32, u32, u32, u32)
*/
