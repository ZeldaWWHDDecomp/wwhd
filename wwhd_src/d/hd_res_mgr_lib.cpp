/* sead instances emitted in the hd_res_mgr translation unit: the tree operations of the
 * SafeString-keyed maps (find, insert, erase, eraseMin; every such map of the game calls this one
 * copy), the node erase of the archive and pack maps, the pack resource (deleting destructor, load
 * alignment), the resource factories' newResource_ and the directory handle's deleting destructor.
 * Reconstructed from the game's instructions. */
#include "bindings.h"
#include "gc_api/MSL_C/string.h"
#include "sead/HdTuInstances.h"
#include "d/hd_res_mgr.h"

using namespace sead::instances;

void* newFromHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
static sead::DisposerVtable sDirectoryHandleVtable;
static sead::ResourceVtable sPackResourceVtable;

/* ---- balancing of the maps (a red-black tree whose red links lean to the lower side; GHS inlines every helper below) ----
 * A node's mRedLink byte marks the link from its parent as red. mLower holds the smaller keys,
 * mUpper the larger ones. */

/* the link to child is red */
static bool redLink(StrTreeNode* child) {
    return child != NULL && child->mRedLink != 0;
}

/* the upper child takes top's place and keeps top's link colour; top hangs below it on a red link */
static StrTreeNode* raiseUpper(StrTreeNode* top) {
    StrTreeNode* upper = top->mUpper;
    top->mUpper = upper->mLower;
    upper->mLower = top;
    upper->mRedLink = top->mRedLink;
    top->mRedLink = StrTreeNode::cRedLink;
    return upper;
}

/* the mirror image: the lower child takes top's place */
static StrTreeNode* raiseLower(StrTreeNode* top) {
    StrTreeNode* lower = top->mLower;
    top->mLower = lower->mUpper;
    lower->mUpper = top;
    lower->mRedLink = top->mRedLink;
    top->mRedLink = StrTreeNode::cRedLink;
    return lower;
}

/* toggles the link bytes of top and both children (an XOR with 1 in the original, not a negation) */
static void toggleLinks(StrTreeNode* top) {
    top->mRedLink ^= 1;
    top->mLower->mRedLink ^= 1;
    top->mUpper->mRedLink ^= 1;
}

/* the lower child and its lower child are both on black links (reads the grandchild without a null check) */
static bool lowerPathBlack(StrTreeNode* top) {
    StrTreeNode* lower = top->mLower;
    return !redLink(lower) && !redLink(lower->mLower);
}

/* the same on the upper side, through the upper child's lower child */
static bool upperPathBlack(StrTreeNode* top) {
    StrTreeNode* upper = top->mUpper;
    return !redLink(upper) && !redLink(upper->mLower);
}

/* two red links in a row on the lower side */
static bool lowerPathDoubleRed(StrTreeNode* top) {
    StrTreeNode* lower = top->mLower;
    return redLink(lower) && redLink(lower->mLower);
}

/* inlined into erase and eraseMin: makes a red link available on the lower side before descending there */
static StrTreeNode* lendRedDownLower(StrTreeNode* top) {
    toggleLinks(top);
    StrTreeNode* upper = top->mUpper;
    if (!redLink(upper->mLower)) return top;
    top->mUpper = raiseLower(upper);
    StrTreeNode* raised = raiseUpper(top);
    toggleLinks(raised);
    return raised;
}

/* inlined into erase: the same before descending on the upper side */
static StrTreeNode* lendRedDownUpper(StrTreeNode* top) {
    toggleLinks(top);
    if (!redLink(top->mLower->mLower)) return top;
    StrTreeNode* raised = raiseLower(top);
    toggleLinks(raised);
    return raised;
}

/* inlined into erase and eraseMin: the balance repair on the way back up */
static StrTreeNode* restoreBalance(StrTreeNode* top) {
    if (redLink(top->mUpper)) top = raiseUpper(top);
    if (lowerPathDoubleRed(top)) top = raiseLower(top);
    if (redLink(top->mLower) && redLink(top->mUpper)) toggleLinks(top);
    return top;
}

/* ---- SafeString comparisons (SafeStringBase::compare, inlined three ways) ---- */

/* inlined into find and insert: compare(key, other), reading the key's character first */
static s32 compareKeys(sead::SafeText* key, sead::SafeText* other) {
    key->assureTermination();
    key->assureTermination();
    be<u8>* keyText = key->mText;
    other->assureTermination();
    const be<u8>* otherText = other->mText;
    if (keyText == otherText) return 0;
    const be<u8>* text = key->mText;
    for (s32 i = 0; i < sead::kMaxTextLength; ++i) {
        u8 c = text[i];
        u8 o = otherText[i];
        if (c == 0) return o == 0 ? 0 : -1;
        if (o == 0) return 1;
        if (c < o) return -1;
        if (c > o) return 1;
    }
    return 0;
}

/* inlined into erase: compare(key, other) < 0, reading the other's character first */
static bool keyLessThan(sead::SafeText* key, sead::SafeText* other) {
    key->assureTermination();
    key->assureTermination();
    be<u8>* keyText = key->mText;
    other->assureTermination();
    const be<u8>* otherText = other->mText;
    if (keyText == otherText) return false;
    for (s32 i = 0; i < sead::kMaxTextLength; ++i) {
        u8 o = otherText[i];
        if (o == 0) return false;
        u8 c = key->mText[i];
        if (c == 0) return true;
        if (o > c) return true;
        if (o < c) return false;
    }
    return false;
}

/* inlined into erase: compare(key, other) == 0 */
static bool keysEqual(sead::SafeText* key, sead::SafeText* other) {
    key->assureTermination();
    key->assureTermination();
    be<u8>* keyText = key->mText;
    other->assureTermination();
    const be<u8>* otherText = other->mText;
    if (keyText == otherText) return true;
    const be<u8>* text = key->mText;
    for (s32 i = 0; i < sead::kMaxTextLength; ++i) {
        u8 c = text[i];
        u8 o = otherText[i];
        if (c == 0) return o == 0;
        if (o == 0 || o != c) return false;
    }
    return true;
}

/* ---- tree operations ---- */

// unlinks the node with the smallest key below top (the node itself stays allocated)
/* 0261399C */
StrTreeNode* sead::instances::StrTreeMap::eraseMin(StrTreeNode* top) {
    if (top->mLower == NULL) return NULL;
    if (lowerPathBlack(top)) top = lendRedDownLower(top);
    top->mLower = eraseMin(top->mLower);
    return restoreBalance(top);
}

/* inlined into erase: the first level of eraseMin, which then calls the function above */
static StrTreeNode* unlinkLowest(StrTreeNode* top) {
    if (top->mLower == NULL) return NULL;
    if (lowerPathBlack(top)) top = lendRedDownLower(top);
    top->mLower = StrTreeMap::eraseMin(top->mLower);
    return restoreBalance(top);
}

/* 02613F20 */
StrTreeNode* sead::instances::StrTreeMap::find(StrTreeNode* node, SafeText* key) {
    while (node != NULL) {
        s32 order = compareKeys(key, &node->mKey);
        if (order < 0) {
            node = node->mLower;
        } else if (order > 0) {
            node = node->mUpper;
        } else {
            return node;
        }
    }
    return NULL;
}

// adds node below top; a node with the same key is replaced and goes back to its pool
/* 0261401C */
StrTreeNode* sead::instances::StrTreeMap::insert(StrTreeNode* top, StrTreeNode* node) {
    if (top == NULL) {
        node->mRedLink = StrTreeNode::cRedLink;
        node->mUpper = NULL;
        node->mLower = NULL;
        return node;
    }
    s32 order = compareKeys(&node->mKey, &top->mKey);
    if (order < 0) {
        top->mLower = insert(top->mLower, node);
    } else if (order > 0) {
        top->mUpper = insert(top->mUpper, node);
    } else {
        if (top != node) {
            node->mUpper = top->mUpper;
            node->mLower = top->mLower;
            node->mRedLink = top->mRedLink;
            top->erase();
        }
        top = node;
    }
    bool leansUpper = redLink(top->mUpper) && !redLink(top->mLower);
    if (leansUpper) top = raiseUpper(top);
    if (lowerPathDoubleRed(top)) top = raiseLower(top);
    if (redLink(top->mLower) && redLink(top->mUpper)) toggleLinks(top);
    return top;
}

// removes the node with key below top (the removed node goes back to its pool)
/* 026142C4 */
StrTreeNode* sead::instances::StrTreeMap::erase(StrTreeNode* top, SafeText* key) {
    if (keyLessThan(key, &top->mKey)) {
        if (lowerPathBlack(top)) top = lendRedDownLower(top);
        top->mLower = erase(top->mLower, key);
        return restoreBalance(top);
    }
    if (redLink(top->mLower)) top = raiseLower(top);
    if (keysEqual(key, &top->mKey) && top->mUpper == NULL) {
        top->erase();
        return NULL;
    }
    if (upperPathBlack(top)) top = lendRedDownUpper(top);
    if (!keysEqual(key, &top->mKey)) {
        top->mUpper = erase(top->mUpper, key);
        return restoreBalance(top);
    }
    // top goes: the smallest node of its upper subtree takes its place
    StrTreeNode* smallest = top->mUpper;
    while (smallest->mLower != NULL) smallest = smallest->mLower;
    StrTreeNode* replacement = find(top->mUpper, &smallest->mKey);
    replacement->mUpper = unlinkLowest(top->mUpper);
    replacement->mLower = top->mLower;
    replacement->mRedLink = top->mRedLink;
    top->erase();
    return restoreBalance(replacement);
}

/* ---- map nodes ---- */

/* 02613EE0 */
void ResourceManagerHD::ArchiveNode::erase() {
    sead::TreeMapPool<ArchiveNode>* pool = mTree;
    mLeft = pool->mFreeHead;
    s32 count = pool->mCount;
    pool->mFreeHead = this;
    pool->mCount = count - 1;
}

/* 02613F00 */
void ResourceManagerHD::PackNode::erase() {
    sead::TreeMapPool<PackNode>* pool = mTree;
    mLeft = pool->mFreeHead;
    s32 count = pool->mCount;
    pool->mFreeHead = this;
    pool->mCount = count - 1;
}

/* ---- resources ---- */

/* 02613C20 */
void sead::instances::res_mgr::DirectoryHandle::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    FileDevice* device = mSourceDevice;
    mDisposer.mVtable = &sDirectoryHandleVtable;
    if (device != NULL) device->tryCloseDirectory(this);
    static_cast<RegisteredObject*>(&mDisposer)->destroy(0);
    if (flags & 1) operator_delete(this);
}

/* 02613C98 */
u32 sead::instances::res_mgr::PackResource::getLoadDataAlignment() {
    return 4;
}

/* 02613D60 */
void sead::instances::res_mgr::PackResource::destroy(u32 flags) {
    if (gabi::ea(this) == 0) return;
    DirectResource::destroy(0);
    if (flags & 1) operator_delete(this);
}

/* 02613DB4 */
sead::DirectResource* sead::instances::res_mgr::ArchiveFactory::newResource(RegistryHeap* heap, s32 alignment) {
    void* storage = newFromHeap(sizeof(SharcArchiveRes), heap, alignment);
    if (storage == NULL) return NULL;
    return SharcArchiveRes::construct(static_cast<SharcArchiveRes*>(storage));
}

/* 02613E38 */
sead::DirectResource* sead::instances::res_mgr::PackFactory::newResource(RegistryHeap* heap, s32 alignment) {
    PackResource* resource = static_cast<PackResource*>(newFromHeap(sizeof(PackResource), heap, alignment));
    if (resource != NULL) {
        // `new (heap, alignment) PackResource()`: value-initialised, then the implicit constructor
        bzero(resource, u32(sizeof(PackResource)));  // u32: the guest bzero, not the host one
        DirectResource::construct(resource);
        resource->mVtable = &sPackResourceVtable;
    }
    return resource;
}

/* WWHD glue
data sDirectoryHandleVtable 100E2194 nocheck
data sPackResourceVtable 100E21C4 nocheck
ext 0273B050 void* newFromHeap(u32, sead::RegistryHeap*, s32)
ext 02741088 u32 sead::FileDevice::tryCloseDirectory(sead::DirectoryHandle*)
ext 02752BEC void sead::RegisteredObject::destroy(u32)
ext 0275D12C sead::DirectResource* sead::DirectResource::construct(sead::DirectResource*)
ext 0275D1B4 void sead::DirectResource::destroy(u32)
ext 0275E11C sead::SharcArchiveRes* sead::SharcArchiveRes::construct(sead::SharcArchiveRes*)
*/
