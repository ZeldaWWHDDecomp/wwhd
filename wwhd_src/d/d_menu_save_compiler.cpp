// HD SaveMgr compiler/vtable helpers.
#include "d/d_menu_save.h"
using namespace hdSave;
/* 027253A0 */
void saveHelperDelete0(u32 p, u32 flags) {
    if (p && (flags & 1))
        saveCompilerApi_deleteSaveObject(p);
}
/* 027253B4 */
void saveHelperEmpty() {}
/* 027253B8 */
void saveHelperTerminate(u32 p) {
    gabi::store<u8>((u32)gabi::at<FormatString>(p)->mpText + (u32)gabi::at<FormatString>(p)->mCapacity - 1,
                    0);
}
/* 027253D0 */
void saveHelperDelete1(u32 p, u32 flags) {
    if (p && (flags & 1))
        saveCompilerApi_deleteSaveObject(p);
}
/* 027253E4 */
void saveHelperDelete2(u32 p, u32 flags) {
    if (p && (flags & 1))
        saveCompilerApi_deleteSaveObject(p);
}
/* 027253F8 */
void saveHelperDelete3(u32 p, u32 flags) {
    if (p && (flags & 1))
        saveCompilerApi_deleteSaveObject(p);
}
/* 0272540C */
void Worker::destroy(u32 flags) {
    [[maybe_unused]] u32 p = gabi::ea(this);
    if (!p)
        return;
    u32 heap = (u32)gabi::at<Worker>(p)->mpHeap;
    gabi::store<u32>(p + 12, FileDisposerVtableAddress);
    if (heap)
        saveCompilerApi_unlinkFileDisposer(heap, p);
    saveCompilerApi_destroySaveDisposer(p, 0u);
    if (flags & 1)
        saveCompilerApi_deleteSaveObject(p);
}
/* 02725480 */
u32 saveMemberDispatch(u32 p) {
    u32 obj = (u32)gabi::at<WorkerCallback>(p)->mpObject;
    if (!obj)
        return p;
    s32 index = (s16)gabi::at<WorkerCallback>(p)->mCallback.mVirtualIndex;
    if (!index)
        return p;
    s32 adjust = (s16)gabi::at<WorkerCallback>(p)->mCallback.mThisAdjustment;
    u32 self = obj + (u32)adjust, target, offset = cpu->r[8];
    if (index < 0)
        target = (u32)gabi::at<WorkerCallback>(p)->mCallback.mTarget;
    else {
        offset = (u32)(s32)gabi::load<s16>(p + 14);
        u32 table = readWord(self + offset);
        target = readWord(table + ((u32)index << 3) + 4);
    }
    // The dispatcher establishes r6/r7/r9 and, on virtual dispatch, r8; forward all other live incoming
    // arguments.
    return gabi::call_ptr<u32>(target, self, cpu->r[4], cpu->r[5], (u32)adjust, (u32)index, offset, obj,
                               cpu->r[10], cpu->f[1].ps0, cpu->f[2].ps0, cpu->f[3].ps0, cpu->f[4].ps0,
                               cpu->f[5].ps0, cpu->f[6].ps0, cpu->f[7].ps0, cpu->f[8].ps0);
}
/* WWHD glue

ext 0273AF40 void saveCompilerApi_deleteSaveObject(u32)
ext 02740BE0 void saveCompilerApi_unlinkFileDisposer(u32, u32)
ext 02752BEC void saveCompilerApi_destroySaveDisposer(u32, u32)
*/
