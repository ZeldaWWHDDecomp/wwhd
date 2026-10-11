/* hd_res_mgr: HD resource manager (ResMgr singleton 101F4F7C: archives by name, pack loading,
 * loader threads), WWHD.
 * HD-only code (no GameCube source): written from the
 * WWHD code. Range 0260F9E4..02614BDF (sinit 02613710; two initialiser-only TUs 02614AB8 and
 * 02614B4C). The pack scanners are in hd_res_mgr_pack.cpp. Not verified here (sead library
 * template instances emitted in this TU): 02613884 02613898 026138AC 0261390C 0261396C 02613970
 * 02613988 0261399C 02613BBC..02613C0C 02613C20 02613C94 02613C98 02613CA0 02613CA4 02613D60
 * 02613DB4 02613DE4 02613E38 02613E90..02613ECC 02613EE0 02613F00 02613F20 (TreeMap find)
 * 0261401C (TreeMap insert) 026142C4 (TreeMap erase).
 *
 * ResMgr (0x1131C): +0 vtable 100E222C, +0x14 archive map (sead TreeMap keyed by name, 0x280 nodes
 * of 0x54 at +0x28; node +0xC key {top, vtable} with a 0x30-char buffer at +0x20, +0x18 value),
 * +0xD228 pack entry map (hash keys, 0x200 nodes of 0x20; find 0260ACC8, insert 0260AD08,
 * erase 0260AEFC), +0x1123C arena, +0x11240/+0x11244 loader threads, +0x11248 request state,
 * +0x1124C request name (FixedSafeString<48>) and +0x11288 path (FixedSafeString<64>),
 * +0x112D4 heap, +0x112D8 u8 "from pack", +0x112DC critical section, +0x11318 load alignment.
 */
#include "d/hd_res_mgr.h"
using namespace gabi;
using namespace ResourceManagerHD;
void resNative_copyBytes(u32, u32, u32, u32);
u32 resNative_hashCRC(u32, u32);
u32 resNative_allocate(u32);
void resNative_clearRequestBuffer(u32, u32);
void resNative_criticalSectionConstruct(u32);
u32 resNative_allocateWithHeapAligned(u32, u32, u32);
void resNative_singletonDisposerConstruct(u32, u32, u32);
void resNative_formatObjectPath(u32, u32, u32);
void resNative_disposerConstruct(u32);
u32 resNative_currentHeap(u32);
u32 resNative_findArchiveNode(u32, u32, u32);
u32 resNative_insertArchiveNode(u32, u32, u32);
u32 resNative_archiveResourceSize(u32);
void resNative_archiveResourceRead(u32, u32, u32);
u32 resNative_allocateWithHeap(u32, u32, u32);
u32 resNative_archiveResourceConstruct(u32);
void resNative_archiveResourceSetBuffer(u32, u32, u32, u32, u32, u32);
void resNative_invalidateArchiveCache(u32, u32);
u32 resNative_findPackNode(u32, u32, u32);
void resNative_formatStageName(u32, u32, u32, u32);
u32 resNative_loadCompressedArchive(u32, u32, u32, u32, u32);
u32 resNative_loadArchiveFile(u32, u32, u32, u32);
u32 resNative_eraseArchiveNode(u32, u32, u32);
u32 resNative_erasePackNode(u32, u32, u32);
void resNative_criticalSectionEnter(u32);
void resNative_criticalSectionLeave(u32);
void resNative_scanStagePack(u32, u32, u32, u32, u32);
void resNative_scanPermanentPack(u32, u32, u32);
void resNative_criticalSectionDestroy(u32, u32);
void resNative_disposerDestroy(u32, u32);
void resNative_deallocate(u32);
void resNative_registerStaticObject(u32);
void resNative_initializeStaticObjectFlag(u32);
void resNative_initializeStaticObjectByte(u32);

/* sead strlen bound */
static s32 cstrLength(u32 p) {
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

/* copies the string object src into the buffer dst (capacity read from cap) */
static void copyInto(u32 dst, u32 capAddr, u32 src) {
    call_ptr<void>(stringVtable(src)->assureTermination.get(), src);
    s32 n = cstrLength(readField<u32>(at<FixedString<1>>(src)->mString.mTop));
    const s32 size = s32(wordAt(capAddr).get());
    const u32 fn = stringVtable(src)->assureTermination.get();
    if (n >= size)
        n = size - 1;
    call_ptr<void>(fn, src);
    resNative_copyBytes(dst, readField<u32>(at<FixedString<1>>(src)->mString.mTop), u32(n), 0u);
    byteAt(dst + n) = 0;
}

/* hash of a SafeString's text: sead::HashCRC32::calcHash (0273B264, r3/r4 only; matcher name
 * dRes_control_c::getResInfoLoaded corrected in matcher_errors.tsv) */
static u32 hashString(u32 s) {
    call_ptr<void>(stringVtable(s)->assureTermination.get(), s);
    const u32 fn = stringVtable(s)->assureTermination.get();
    const u32 top = readField<u32>(at<FixedString<1>>(s)->mString.mTop);
    call_ptr<void>(fn, s);
    const s32 len = cstrLength(readField<u32>(at<FixedString<1>>(s)->mString.mTop));
    return resNative_hashCRC(top, len);
}

/* FixedSafeString<N> constructor, inlined (placement-new null checks as GHS emits them) */
static void fixedCtor(u32 s, u32 n, u32 vt1, u32 vt2, u32 vt3, u32 allocSize) {
    u32 q = s;
    if (!q)
        q = resNative_allocate(allocSize);
    if (!q)
        return;
    u32 r = q;
    if (!r)
        r = resNative_allocate(0xCu);
    if (r) {
        wordAt(r) = q + 0xC;
        wordAt(r + 4) = vt1;
        writeField<u32>(at<FixedString<1>>(r)->mCapacity, n);
        byteAt(q + 0xC + n - 1) = 0;
    }
    const u32 top = readField<u32>(at<FixedString<1>>(q)->mString.mTop);
    writeField<u32>(at<FixedString<1>>(q)->mString.mVtable, vt2);
    byteAt(top) = 0;
    writeField<u32>(at<FixedString<1>>(q)->mString.mVtable, vt3);
}

/* 0260F9E4 */
// HD: ResMgr constructor
u32 ResMgr_ctor(u32 p) {

    if (!p) {
        p = resNative_allocate(0x1131Cu);
        if (!p)
            return 0;
    }
    writeField<u32>(at<Manager>(p)->mVtable, kManagerVtable);
    u32 t = p + 0x14;
    if (!t)
        t = resNative_allocate(0xD214u);
    if (t) {
        const u32 nodes = t + 0x14;
        writeField<u32>(at<TreeHeader>(t)->mRoot, 0);
        writeField<u32>(at<TreeHeader>(t)->mCount, 0);
        writeField<u32>(at<TreeHeader>(t)->mCapacity, 0x280);
        writeField<u32>(at<TreeHeader>(t)->mFreeHead, nodes);
        for (u32 i = 0; i < 0x27F; i++)
            at<ArchiveNode>(nodes)[i].mLeft = nodes + sizeof(ArchiveNode) * (i + 1);
        at<ArchiveNode>(nodes)[0x27F].mLeft = 0;
        writeField<u32>(at<TreeHeader>(t)->mNodes, nodes);
    }
    const u32 m = p + 0xD228;
    u32 q = m;
    if (!q)
        q = resNative_allocate(0x4014u);
    if (q) {
        const u32 nodes = q + 0x14;
        writeField<u32>(at<TreeHeader>(q)->mRoot, 0);
        writeField<u32>(at<TreeHeader>(q)->mCount, 0);
        writeField<u32>(at<TreeHeader>(q)->mFreeHead, nodes);
        writeField<u32>(at<TreeHeader>(q)->mCapacity, 0x200);
        for (u32 i = 0; i < 0x1FF; i++)
            at<PackNode>(nodes)[i].mLeft = nodes + sizeof(PackNode) * (i + 1);
        at<PackNode>(nodes)[0x1FF].mLeft = 0;
        writeField<u32>(at<TreeHeader>(q)->mNodes, nodes);
    }
    writeField<u32>(managerFromPackTree(m)->mArena, 0);
    writeField<u32>(managerFromPackTree(m)->mRequestState, 0);
    writeField<u32>(managerFromPackTree(m)->mPackThread, 0);
    writeField<u32>(managerFromPackTree(m)->mLoaderThread, 0);
    resNative_clearRequestBuffer(m + 0x4024, 0x90u);
    fixedCtor(m + 0x4024, 0x30, kFixedStringBaseVtable, kFixedString48BaseVtable,
              kFixedString48Vtable, 0x3C);
    fixedCtor(m + 0x4060, 0x40, kFixedStringBaseVtable, kFixedString64BaseVtable,
              kFixedString64Vtable, 0x4C);
    resNative_criticalSectionConstruct(m + 0x40B4);
    writeField<u32>(managerFromPackTree(m)->mLoadAlignment, 0x40000);
    return p;
}

/* 0260FBF4 */
// HD: creates the singleton with its disposer
void ResMgr_createInstance(u32 heap) {

    if (readField<u32>(singletonSlot()))
        return;
    const u32 p = resNative_allocateWithHeapAligned(0x1131Cu, heap, 4u);
    const u32 d = p + 4;
    if (d) {
        resNative_singletonDisposerConstruct(d, heap, 3u);
        writeField<u32>(at<Disposer>(d)->mVtable, kSingletonDisposerVtable);
    }
    disposerSlot() = d;
    writeField<u32>(singletonSlot(), p ? ResMgr_ctor(p) : 0u);
}

/* 02610070 */
// HD: "Object/<name>.szs"
void ResMgr_objectPath(u32 self, u32 out, u32 name) {

    call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
    resNative_formatObjectPath(out, kObjectPathFormat,
                               readField<u32>(at<FixedString<1>>(name)->mString.mTop));
}

/* 026100D0 */
// HD: archive holder constructor (name, archive resource, heap; default heap: current)
u32 ResMgr_holderCtor(u32 p, u32 name, u32 res, u32 heap) {

    if (!p) {
        p = resNative_allocate(0x54u);
        if (!p)
            return 0;
    }
    resNative_disposerConstruct(p);
    writeField<u32>(at<Holder>(p)->mVtable, kArchiveHolderVtable);
    fixedCtor(p + 0x10, 0x30, kFixedStringBaseVtable, kFixedString48BaseVtable,
              kFixedString48Vtable, 0x3C);
    writeField<u32>(at<Holder>(p)->mResource, res);
    const u32 dst = readField<u32>(at<Holder>(p)->mName.mString.mTop);
    copyInto(dst, p + 0x18, name);
    writeField<u32>(at<Holder>(p)->mHeap,
                    heap ? heap : resNative_currentHeap(heapManagerSlot().get()));
    return p;
}

/* inserts holder under name into the archive map (map full: replaces an existing entry) */
static void insertNamed(u32 self, u32 name, u32 holder) {
    const u32 tree = self + 0x14;
    if (s32(readField<u32>(at<TreeHeader>(tree)->mCount)) >=
        s32(readField<u32>(at<TreeHeader>(tree)->mCapacity))) {
        const u32 n =
            resNative_findArchiveNode(tree, readField<u32>(at<TreeHeader>(tree)->mRoot), name);
        if (n && n + 0x18)
            writeField<u32>(at<ArchiveNode>(n)->mHolder, holder);
        return;
    }
    const u32 node = readField<u32>(at<TreeHeader>(tree)->mFreeHead);
    if (node)
        writeField<u32>(at<TreeHeader>(tree)->mFreeHead,
                        readField<u32>(at<ArchiveNode>(node)->mLeft));
    if (node) {
        writeField<u32>(at<ArchiveNode>(node)->mHolder, holder);
        writeField<u32>(at<ArchiveNode>(node)->mLeft, 0);
        writeField<u32>(at<ArchiveNode>(node)->mTree, tree);
        writeField<u32>(at<ArchiveNode>(node)->mValueVtable, kArchiveMapValueVtable);
        writeField<u8>(at<ArchiveNode>(node)->mBufferFlag, 0);
        writeField<u32>(at<ArchiveNode>(node)->mKey.mTop, kEmptyText);
        writeField<u8>(at<ArchiveNode>(node)->mColor, 1);
        writeField<u32>(at<ArchiveNode>(node)->mKey.mVtable, kSafeStringVt);
        writeField<u32>(at<ArchiveNode>(node)->mRight, 0);
        const u32 buf = node + 0x20;
        call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
        const u32 text = readField<u32>(at<FixedString<1>>(name)->mString.mTop);
        s32 n = 0;
        bool over = false;
        if (byteAt(text).get()) {
            for (;;) {
                n++;
                if (n > 0x40000) {
                    over = true;
                    break;
                }
                if (!byteAt(text + n).get())
                    break;
            }
        }
        if (over) {
            call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
            resNative_copyBytes(buf, readField<u32>(at<FixedString<1>>(name)->mString.mTop), 0u,
                                0u);
            byteAt(buf) = 0;
        } else {
            if (n >= 0x31)
                n = 0x30;
            call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
            resNative_copyBytes(buf, readField<u32>(at<FixedString<1>>(name)->mString.mTop), u32(n),
                                0u);
            byteAt(buf + n) = 0;
        }
        writeField<u32>(at<ArchiveNode>(node)->mKey.mTop, buf);
    }
    const u32 cnt = readField<u32>(at<TreeHeader>(tree)->mCount) + 1;
    const u32 root = readField<u32>(at<TreeHeader>(tree)->mRoot);
    writeField<u32>(at<TreeHeader>(tree)->mCount, cnt);
    const u32 r = resNative_insertArchiveNode(tree, root, node);
    writeField<u32>(at<TreeHeader>(tree)->mRoot, r);
    writeField<u8>(at<ArchiveNode>(r)->mColor, 0);
}

/* reads a pack entry into a new SharcArchiveRes */
static u32 readPackEntry(u32 entry, u32 heap) {
    const u32 size =
        resNative_archiveResourceSize(at<Holder>(wordAt(entry).get())->mResource.get());
    const u32 aligned = (size + 0x1F) & ~0x1Fu;
    const u32 buf = resNative_allocateWithHeapAligned(aligned, heap, 0x2000u);
    resNative_archiveResourceRead(at<Holder>(wordAt(entry).get())->mResource.get(), buf, aligned);
    u32 arc = resNative_allocateWithHeap(0x44u, heap, 0x2000u);
    if (arc)
        arc = resNative_archiveResourceConstruct(arc);
    resNative_archiveResourceSetBuffer(arc, buf, size, aligned, 0u, heap);
    resNative_invalidateArchiveCache(readField<u32>(at<ArchiveResource>(arc)->mBuffer),
                                     readField<u32>(at<ArchiveResource>(arc)->mSize));
    return arc;
}

/* 02610258 */
// HD: makes an archive from the loaded packs available under its name
u32 ResMgr_loadFromPack(u32 self, u32 name, u32 path, u32 heap) {

    const u32 n = resNative_findArchiveNode(
        self + 0x14, readField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot), name);
    if (n && n + 0x18)
        return 1;
    Local<be<u32>> key;
    *key = hashString(name);
    const u32 e = resNative_findPackNode(
        self + 0xD228, readField<u32>(at<Manager>(self)->mPacks.mHeader.mRoot), key.a);
    if (!e || !(e + 0x18))
        return 0;
    const u32 arc = readPackEntry(e + 0x18, heap);
    if (!arc)
        return 0;
    u32 holder = resNative_allocateWithHeap(0x54u, heap, 4u);
    if (holder)
        holder = ResMgr_holderCtor(holder, name, arc, heap);
    insertNamed(self, name, holder);
    return 1;
}

/* 026105C4 */
// HD: request by name: stores the name and its "Object/<name>.szs" path, then loads it
u32 ResMgr_requestObject(u32 self, u32 name, u32 heap) {

    const u32 r = self + 0x1124C;
    const u32 dst = readField<u32>(at<Request>(r)->mName.mString.mTop);
    call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
    s32 n = cstrLength(readField<u32>(at<FixedString<1>>(name)->mString.mTop));
    const s32 size = s32(readField<u32>(at<Request>(r)->mName.mCapacity));
    const u32 fn = stringVtable(name)->assureTermination.get();
    if (n >= size)
        n = size - 1;
    call_ptr<void>(fn, name);
    resNative_copyBytes(dst, readField<u32>(at<FixedString<1>>(name)->mString.mTop), u32(n), 0u);
    byteAt(dst + n) = 0;
    ResMgr_objectPath(self, r + 0x3C, name);
    writeField<u32>(at<Request>(r)->mHeap, heap);
    return ResMgr_loadFromPack(self, r, r + 0x3C, heap);
}

/* 026106B4 */
// HD: second holder type constructor (pack-scanned archives)
u32 ResMgr_holder2Ctor(u32 p, u32 name, u32 res, u32 heap) {

    if (!p) {
        p = resNative_allocate(0x54u);
        if (!p)
            return 0;
    }
    resNative_disposerConstruct(p);
    writeField<u32>(at<Holder>(p)->mVtable, kPackHolderVtable);
    fixedCtor(p + 0x10, 0x30, kFixedStringBaseVtable, kFixedString48BaseVtable,
              kFixedString48Vtable, 0x3C);
    writeField<u32>(at<Holder>(p)->mHeap, heap);
    writeField<u32>(at<Holder>(p)->mResource, res);
    const u32 dst = readField<u32>(at<Holder>(p)->mName.mString.mTop);
    copyInto(dst, p + 0x18, name);
    return p;
}

/* 02612374 */
// HD: "Stage/<arc>_<name>.szs"
void ResMgr_stagePath(u32 self, u32 out, u32 arc, u32 name) {

    call_ptr<void>(stringVtable(arc)->assureTermination.get(), arc);
    const u32 fn = stringVtable(name)->assureTermination.get();
    const u32 arcTop = readField<u32>(at<FixedString<1>>(arc)->mString.mTop);
    call_ptr<void>(fn, name);
    resNative_formatStageName(out, kStagePathFormat, arcTop,
                              readField<u32>(at<FixedString<1>>(name)->mString.mTop));
}

/* 026123FC */
// HD: the archive resource of a loaded name (0 when not loaded)
u32 ResMgr_getArchive(u32 self, u32 name) {

    const u32 n = resNative_findArchiveNode(
        self + 0x14, readField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot), name);
    if (!n || !(n + 0x18))
        return 0;
    return at<Holder>(readField<u32>(at<ArchiveNode>(n)->mHolder))->mResource.get();
}

/* 02612450 */
// HD: file of an archive resource (size through outSize)
u32 ResMgr_getFile(u32 res, u32 file, u32 outSize) {

    Local<FileInfo> info;
    info->mStart = 0;
    info->mSize = 0;
    const u32 r = call_ptr<u32>(resourceVtable(res)->getFile.get(), res, file, info.a);
    if (r && outSize)
        wordAt(outSize) = info->mSize.get();
    return r;
}

/* 026124B0 */
// HD: file of a named archive
u32 ResMgr_getArchiveFile(u32 self, u32 name, u32 file, u32 outSize) {

    return ResMgr_getFile(ResMgr_getArchive(self, name), file, outSize);
}

/* 026124F4 */
// HD: ResMgr_getFile with a leading unused argument
u32 ResMgr_getFile2(u32 unused, u32 res, u32 file, u32 outSize) {

    return ResMgr_getFile(res, file, outSize);
}

/* 02612504 */
// HD: is the archive loaded?
u32 ResMgr_isLoaded(u32 self, u32 name) {

    const u32 n = resNative_findArchiveNode(
        self + 0x14, readField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot), name);
    return (n ? n + 0x18 : 0) != 0;
}

/* 02612540 */
// HD: is the archive in a loaded pack?
u32 ResMgr_isInPack(u32 self, u32 name) {

    Local<be<u32>> key;
    *key = hashString(name);
    const u32 n = resNative_findPackNode(
        self + 0xD228, readField<u32>(at<Manager>(self)->mPacks.mHeader.mRoot), key.a);
    return (n ? n + 0x18 : 0) != 0;
}

/* 0261261C */
// HD: deletes a loaded archive's resource and holder
u32 ResMgr_unloadArchive(u32 self, u32 name) {

    const u32 n = resNative_findArchiveNode(
        self + 0x14, readField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot), name);
    if (!n || !(n + 0x18))
        return 0;
    const u32 v = n + 0x18;
    u32 holder = wordAt(v).get();
    const u32 res = readField<u32>(at<Holder>(holder)->mResource);
    if (res) {
        call_ptr<void>(resourceVtable(res)->destroy.get(), res, 3u);
        holder = wordAt(v).get();
    }
    if (holder)
        call_ptr<void>(holderVtable(holder)->destroy.get(), holder, 3u);
    return 1;
}

/* 026126C0 */
// HD: destroys the heap of a pack entry
u32 ResMgr_freePackEntry(u32 self, u32 name) {

    Local<be<u32>> key;
    *key = hashString(name);
    const u32 n = resNative_findPackNode(
        self + 0xD228, readField<u32>(at<Manager>(self)->mPacks.mHeader.mRoot), key.a);
    if (!n || !(n + 0x18))
        return 0;
    const u32 heap = at<Holder>(readField<u32>(at<PackNode>(n)->mHolder))->mHeap.get();
    call_ptr<void>(heapVtable(heap)->destroy.get(), heap);
    return 1;
}

/* 026127D4 */
// HD: asynchronous object request (loader thread message 1)
u32 ResMgr_requestObjectAsync(u32 self, u32 name, u32 heap) {

    const u32 busy = readField<u32>(at<Manager>(self)->mRequestState);
    u32 fromPack = 0;
    if (busy)
        fromPack = 1;
    else
        writeField<u32>(at<Manager>(self)->mRequestState, 1);
    const u32 r = self + 0x1124C;
    const u32 dst = readField<u32>(at<Request>(r)->mName.mString.mTop);
    call_ptr<void>(stringVtable(name)->assureTermination.get(), name);
    s32 n = cstrLength(readField<u32>(at<FixedString<1>>(name)->mString.mTop));
    const s32 size = s32(readField<u32>(at<Request>(r)->mName.mCapacity));
    const u32 fn = stringVtable(name)->assureTermination.get();
    if (n >= size)
        n = size - 1;
    call_ptr<void>(fn, name);
    resNative_copyBytes(dst, readField<u32>(at<FixedString<1>>(name)->mString.mTop), u32(n), 0u);
    byteAt(dst + n) = 0;
    ResMgr_objectPath(self, r + 0x3C, name);
    writeField<u8>(at<Request>(r)->mFromPack, u8(fromPack));
    writeField<u32>(at<Request>(r)->mHeap, heap);
    const u32 t = readField<u32>(at<Manager>(self)->mLoaderThread);
    call_ptr<void>(threadVtable(t)->sendMessage.get(), t, 1u, 0u);
    return 1;
}

/* 02612908 */
// HD: asynchronous stage request "<arc>_<name>" (loader thread message 1)
u32 ResMgr_requestStageAsync(u32 self, u32 arc, u32 name, u32 heap) {

    const u32 busy = readField<u32>(at<Manager>(self)->mRequestState);
    u32 fromPack = 0;
    if (busy)
        fromPack = 1;
    else
        writeField<u32>(at<Manager>(self)->mRequestState, 1);
    call_ptr<void>(stringVtable(arc)->assureTermination.get(), arc);
    const u32 fn = stringVtable(name)->assureTermination.get();
    const u32 arcTop = readField<u32>(at<FixedString<1>>(arc)->mString.mTop);
    call_ptr<void>(fn, name);
    const u32 r = self + 0x1124C;
    resNative_formatStageName(r, kStageRequestFormat, arcTop,
                              readField<u32>(at<FixedString<1>>(name)->mString.mTop));
    ResMgr_stagePath(self, r + 0x3C, arc, name);
    writeField<u8>(at<Request>(r)->mFromPack, u8(fromPack));
    writeField<u32>(at<Request>(r)->mHeap, heap);
    const u32 t = readField<u32>(at<Manager>(self)->mLoaderThread);
    call_ptr<void>(threadVtable(t)->sendMessage.get(), t, 1u, 0u);
    return 1;
}

/* 02612A80 */
// HD: starts the permanent pack thread (message 2) when idle
u32 ResMgr_startPackLoad(u32 self) {

    if (readField<u32>(at<Manager>(self)->mRequestState))
        return 0;
    const u32 t = readField<u32>(at<Manager>(self)->mPackThread);
    writeField<u32>(at<Manager>(self)->mRequestState, 2);
    call_ptr<void>(threadVtable(t)->sendMessage.get(), t, 2u, 0u);
    return 1;
}

/* 02612AE8 */
// HD: asynchronous stage pack scan (loader thread message 3) when idle
u32 ResMgr_requestStagePackAsync(u32 self, u32 arc, u32 name, u32 heap) {

    if (readField<u32>(at<Manager>(self)->mRequestState))
        return 0;
    writeField<u32>(at<Manager>(self)->mRequestState, 1);
    call_ptr<void>(stringVtable(arc)->assureTermination.get(), arc);
    const u32 fn = stringVtable(name)->assureTermination.get();
    const u32 arcTop = readField<u32>(at<FixedString<1>>(arc)->mString.mTop);
    call_ptr<void>(fn, name);
    const u32 r = self + 0x1124C;
    resNative_formatStageName(r, kStagePackRequestFormat, arcTop,
                              readField<u32>(at<FixedString<1>>(name)->mString.mTop));
    ResMgr_stagePath(self, r + 0x3C, arc, name);
    writeField<u32>(at<Request>(r)->mHeap, heap);
    writeField<u8>(at<Request>(r)->mFromPack, 0);
    const u32 t = readField<u32>(at<Manager>(self)->mLoaderThread);
    call_ptr<void>(threadVtable(t)->sendMessage.get(), t, 3u, 0u);
    return 1;
}

/* 02612BE0 */
// HD: the 2D permanent pack name of the console language
u32 ResMgr_languagePack() {

    const u32 cfg = configurationSlot().get();
    const u32 region = readField<u32>(at<Config>(cfg)->mRegion);
    if (region == 1)
        return ea(&languagePackObjects()[0]);
    if (region == 2) {
        const u32 lang = readField<u32>(at<Config>(cfg)->mLanguage);
        if (lang == 2)
            return ea(&languagePackObjects()[2]);
        if (lang == 5)
            return ea(&languagePackObjects()[3]);
        return ea(&languagePackObjects()[1]);
    }
    if (region == 4) {
        const u32 lang = readField<u32>(at<Config>(cfg)->mLanguage);
        if (lang >= 1 && lang <= 5)
            return languagePackTable()[lang].get();
    }
    return ea(&languagePackObjects()[1]);
}

/* 02612E64 */
// HD: loads an archive (from a loaded pack, else from the file system) under its name
u32 ResMgr_loadArchive(u32 self, u32 name, u32 path, u32 heap, u32 compressed, u32 align,
                       u32 bufAlign) {

    const u32 n = resNative_findArchiveNode(
        self + 0x14, readField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot), name);
    if (n && n + 0x18)
        return 1;
    Local<be<u32>> key;
    *key = hashString(name);
    const u32 m = self + 0xD228;
    const u32 e = resNative_findPackNode(m, readField<u32>(at<TreeHeader>(m)->mRoot), key.a);
    u32 arc;
    if (e && e + 0x18) {
        arc = readPackEntry(e + 0x18, heap);
    } else if (compressed) {
        arc = resNative_loadCompressedArchive(
            path, heap, readField<u32>(managerFromPackTree(m)->mArena), align, bufAlign);
    } else {
        arc = resNative_loadArchiveFile(path, heap, align, bufAlign);
    }
    if (!arc)
        return 0;
    u32 holder = resNative_allocateWithHeap(0x54u, heap, 4u);
    if (holder)
        holder = ResMgr_holderCtor(holder, name, arc, heap);
    insertNamed(self, name, holder);
    return 1;
}

/* 0261321C */
// HD: removes a name from the archive map
void ResMgr_eraseArchive(u32 self, u32 name) {

    const u32 n = resNative_findArchiveNode(
        self + 0x14, readField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot), name);
    if (!n || !(n + 0x18))
        return;
    const u32 r = resNative_eraseArchiveNode(
        self + 0x14, readField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot), name);
    writeField<u32>(at<Manager>(self)->mArchives.mHeader.mRoot, r);
    if (r)
        writeField<u8>(at<ArchiveNode>(r)->mColor, 0);
}

/* 02613294 */
// HD: removes a name from the pack entry map
void ResMgr_erasePackEntry(u32 self, u32 name) {

    const u32 h = hashString(name);
    Local<HashKeys> keys;
    keys->mFirst = h;
    const u32 m = self + 0xD228;
    const u32 root = readField<u32>(at<TreeHeader>(m)->mRoot);
    keys->mSecond = h;
    const u32 n = resNative_findPackNode(m, root, keys.a + 4);
    if (!n || !(n + 0x18))
        return;
    const u32 r = resNative_erasePackNode(m, root, keys.a);
    writeField<u32>(at<TreeHeader>(m)->mRoot, r);
    if (r)
        writeField<u8>(at<ArchiveNode>(r)->mColor, 0);
}

/* 02613394 */
// HD: ResMgrThread message handler (1: archive request, 3: stage pack scan)
void ResMgr_threadMain(u32 self, u32 thread, u32 msg) {

    const u32 st = self + 0x11248, r = self + 0x1124C, cs = self + 0x112DC;
    if (msg == 1) {
        if (readField<u8>(at<Manager>(self)->mFromPack)) {
            ResMgr_loadFromPack(self, r, r + 0x3C, readField<u32>(at<Request>(r)->mHeap));
            return;
        }
        resNative_criticalSectionEnter(cs);
        ResMgr_loadArchive(self, r, r + 0x3C, readField<u32>(at<Request>(r)->mHeap), 1u,
                           readField<u32>(at<Manager>(self)->mLoadAlignment), 0x2000u);
        wordAt(st) = 0;
        resNative_criticalSectionLeave(cs);
    } else if (msg == 3) {
        resNative_criticalSectionEnter(cs);
        resNative_scanStagePack(self, r, self + 0x11288,
                                readField<u32>(at<Manager>(self)->mRequestHeap),
                                readField<u32>(at<Manager>(self)->mLoadAlignment));
        wordAt(st) = 0;
        resNative_criticalSectionLeave(cs);
    }
}

/* 026134B4 */
// HD: ResMgrPackLoadThread message handler (2: the three SZS permanent packs)
void ResMgr_packThreadMain(u32 self, u32 thread, u32 msg) {

    if (msg != 2)
        return;
    const u32 st = self + 0x11248, cs = st + 0x94;
    resNative_criticalSectionEnter(cs);
    Local<SafeString> name;
    const u32 packs[3] = {kFirstPermanentPackPath, kSecondPermanentPackPath,
                          kThirdPermanentPackPath};
    for (u32 i = 0; i < 3; i++) {
        name->mTop = packs[i];
        name->mVtable = kSafeStringVt;
        resNative_scanPermanentPack(self, name.a, gameHeaps()->mPermanentPackHeap.get());
    }
    wordAt(st) = 0;
    resNative_criticalSectionLeave(cs);
}

/* 02613580 */
// HD: SingletonDisposer deleting destructor (destroys the instance)
void ResMgr_disposerDtor(u32 p, u32 flags) {

    if (!p)
        return;
    writeField<u32>(at<Disposer>(p)->mVtable, kSingletonDisposerVtable);
    if (p == disposerSlot().get()) {
        const u32 inst = readField<u32>(singletonSlot());
        disposerSlot() = 0;
        writeField<u32>(at<Manager>(inst)->mVtable, kManagerVtable);
        resNative_criticalSectionDestroy(inst + 0x112DC, 2u);
        writeField<u32>(singletonSlot(), 0);
    }
    resNative_disposerDestroy(p, 0u);
    if (flags & 1)
        resNative_deallocate(p);
}

/* 02613630 */
// HD: archive holder destructor (removes its map entry)
void ResMgr_holderDtor(u32 p, u32 flags) {

    if (!p)
        return;
    writeField<u32>(at<Holder>(p)->mVtable, kArchiveHolderVtable);
    ResMgr_eraseArchive(readField<u32>(singletonSlot()), p + 0x10);
    resNative_disposerDestroy(p, 0u);
    if (flags & 1)
        resNative_deallocate(p);
}

/* 026136A0 */
// HD: pack holder destructor (removes its pack map entry)
void ResMgr_holder2Dtor(u32 p, u32 flags) {

    if (!p)
        return;
    writeField<u32>(at<Holder>(p)->mVtable, kPackHolderVtable);
    ResMgr_erasePackEntry(readField<u32>(singletonSlot()), p + 0x10);
    resNative_disposerDestroy(p, 0u);
    if (flags & 1)
        resNative_deallocate(p);
}

/* the standard initialiser of these TUs: the shared static objects */
static void stdInit(u32 b, u32 r1, u32 lo, u32 hi, u32 f, u32 r2, u32 r3) {
    writeField<u32>(at<InitLists>(b)->mThird, 0);
    writeField<u32>(at<InitLists>(b)->mFirst, 0);
    writeField<u32>(at<InitLists>(b)->mFourth, 0);
    writeField<u32>(at<InitLists>(b)->mSecond, 0);
    resNative_registerStaticObject(r1);
    const f32 a = floatAt(lo).get(), c = floatAt(hi).get();
    writeField<f32>(at<InitAngles>(f)->mMinimum, a);
    writeField<f32>(at<InitAngles>(f)->mMaximum, c);
    resNative_initializeStaticObjectFlag(f + 8);
    resNative_registerStaticObject(r2);
    resNative_initializeStaticObjectByte(f + 9);
    resNative_registerStaticObject(r3);
}

/* 02613710 */
// HD: static initialiser (shared objects, the language pack names)
void ResMgr_staticInit() {

    stdInit(0x1048DDA0, 0x101F4F58, 0x100E247C, 0x100E2480, 0x1048DD94, 0x101F4F64, 0x101F4F70);
    const u32 vt = kSafeStringVt;
    languagePackObjects()[0].mVtable = vt;
    languagePackObjects()[0].mTop = 0x100E24E4;
    languagePackObjects()[1].mVtable = vt;
    languagePackObjects()[1].mTop = 0x100E2508;
    languagePackObjects()[2].mVtable = vt;
    languagePackObjects()[3].mVtable = vt;
    languagePackObjects()[4].mVtable = vt;
    languagePackObjects()[5].mVtable = vt;
    languagePackObjects()[6].mVtable = vt;
    languagePackObjects()[6].mTop = 0x100E24C4;
    languagePackObjects()[3].mTop = 0x100E252C;
    languagePackObjects()[5].mTop = 0x100E24A4;
    languagePackObjects()[7].mVtable = vt;
    languagePackObjects()[7].mTop = 0x100E2574;
    languagePackObjects()[4].mTop = 0x100E2550;
    languagePackObjects()[8].mVtable = vt;
    languagePackObjects()[2].mTop = 0x100E2484;
    languagePackObjects()[8].mTop = 0x100E2598;
}

/* 02614AB8 */
// HD: / 02614B4C: initialisers of two TUs without code of their own
void ResMgr_staticInit2() {

    stdInit(0x1048DDBC, 0x101F4F84, 0x100E25F0, 0x100E25F4, 0x1048DDB0, 0x101F4F90, 0x101F4F9C);
}
/* 02614B4C */
void ResMgr_staticInit3() {

    stdInit(0x1048DDD8, 0x101F4FA8, 0x100E25F8, 0x100E25FC, 0x1048DDCC, 0x101F4FB4, 0x101F4FC0);
}

/* WWHD glue (tools/verify/autoglue.py)
ext C0009988 void resNative_copyBytes(u32, u32, u32, u32)
ext 0273B264 u32 resNative_hashCRC(u32, u32)
ext 0273AD10 u32 resNative_allocate(u32)
ext 028F521C void resNative_clearRequestBuffer(u32, u32)
ext 02760084 void resNative_criticalSectionConstruct(u32)
ext 0273B0D4 u32 resNative_allocateWithHeapAligned(u32, u32, u32)
ext 02752B0C void resNative_singletonDisposerConstruct(u32, u32, u32)
ext 02759C28 void resNative_formatObjectPath(u32, u32, u32)
ext 02752A84 void resNative_disposerConstruct(u32)
ext 02756140 u32 resNative_currentHeap(u32)
ext 02613F20 u32 resNative_findArchiveNode(u32, u32, u32)
ext 0261401C u32 resNative_insertArchiveNode(u32, u32, u32)
ext 0275F800 u32 resNative_archiveResourceSize(u32)
ext 0275F808 void resNative_archiveResourceRead(u32, u32, u32)
ext 0273B050 u32 resNative_allocateWithHeap(u32, u32, u32)
ext 0275E11C u32 resNative_archiveResourceConstruct(u32)
ext 0275D21C void resNative_archiveResourceSetBuffer(u32, u32, u32, u32, u32, u32)
ext C00088A0 void resNative_invalidateArchiveCache(u32, u32)
ext 0260ACC8 u32 resNative_findPackNode(u32, u32, u32)
ext 02759C28 void resNative_formatStageName(u32, u32, u32, u32)
ext 02612C88 u32 resNative_loadCompressedArchive(u32, u32, u32, u32, u32)
ext 02610B40 u32 resNative_loadArchiveFile(u32, u32, u32, u32)
ext 026142C4 u32 resNative_eraseArchiveNode(u32, u32, u32)
ext 0260AEFC u32 resNative_erasePackNode(u32, u32, u32)
ext 027601BC void resNative_criticalSectionEnter(u32)
ext 027601F0 void resNative_criticalSectionLeave(u32)
ext 02610820 void resNative_scanStagePack(u32, u32, u32, u32, u32)
ext 02611864 void resNative_scanPermanentPack(u32, u32, u32)
ext 02760168 void resNative_criticalSectionDestroy(u32, u32)
ext 02752BEC void resNative_disposerDestroy(u32, u32)
ext 0273AF40 void resNative_deallocate(u32)
ext 028F026C void resNative_registerStaticObject(u32)
ext 028ED6F8 void resNative_initializeStaticObjectFlag(u32)
ext 028EAB2C void resNative_initializeStaticObjectByte(u32)
*/
