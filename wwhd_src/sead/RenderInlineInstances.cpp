// WWHD renderer-local inline instances, reconstructed from the local game.
#include "sead/RenderInlineInstances.h"
#include "sead/RenderState.h"
#include "sead/RuntimeType.h"

static sead::RuntimeType sInputType;
static sead::RuntimeType sOutputType;
static u32 sInputGuard;
static u32 sOutputGuard;
static u32 sDefaultRootTable;
static u32 sDefaultStateTable;
static u32 sDefaultRoot;
static sead::CompactRenderState sDefaultState;
void releaseRenderObject(void* object);
void destroyRenderTaskBase(void* object, u32 flags);

/* 0274ECD4 */
static u32 matchesRenderInput(void* object, const sead::RuntimeType* type) {
    if (sInputGuard == 0) {
        sInputGuard = 1;
        sInputType.mTable = &sStreamTable;
    }
    if (type == &sInputType) return 1;
    if (sStreamBaseGuard == 0) {
        sStreamBaseGuard = 1;
        sStreamBaseType.mTable = &sStreamBaseTable;
    }
    return type == &sStreamBaseType;
}

/* 0274ED4C */
static u32 renderInputMode() { return 0; }

/* 0274ED54 */
static u32 matchesRenderOutput(void* object, const sead::RuntimeType* type) {
    if (sOutputGuard == 0) {
        sOutputGuard = 1;
        sOutputType.mTable = &sStreamTable;
    }
    if (type == &sOutputType) return 1;
    if (sStreamBaseGuard == 0) {
        sStreamBaseGuard = 1;
        sStreamBaseType.mTable = &sStreamBaseTable;
    }
    return type == &sStreamBaseType;
}

/* 0274EDCC */
static u32 renderOutputMode() { return 2; }

/* 02750D0C */
static void initializeDefaultRenderState() {
    sDefaultRoot = gabi::ea(&sDefaultRootTable);
    sead::CompactRenderState::create(&sDefaultState);
    sDefaultState.mCullMode = 2;
    sDefaultState.mDepthTest = 0;
    sDefaultState.mDepthWrite = 0;
    sDefaultState.mTable = &sDefaultStateTable;
}

/* 02750D70 */
static void destroyRenderVector(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) releaseRenderObject(object);
}

/* 02750D84 */
static void renderVectorNoop() {}

/* 02750D88 */
static void destroyRenderColor(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) releaseRenderObject(object);
}

/* 02750D9C */
static void terminateRenderText(sead::SafeTextBuffer* text) {
    be<u8>* end = gabi::at<be<u8>>((u32)text->mText.v + (u32)text->mCapacity - 1);
    *end = 0;
}

/* 02750DB4 */
static void renderColorNoop() {}

/* 02750DB8 */
static void destroyRenderText(void* object, u32 flags) {
    if (object != NULL && (flags & 1)) releaseRenderObject(object);
}

/* 02750DCC */
static void renderTextNoop() {}

/* 02750DD0 */
static void destroyRenderTask(void* object, u32 flags) {
    if (object == NULL) return;
    destroyRenderTaskBase(object, 0);
    if (flags & 1) releaseRenderObject(object);
}

/* WWHD glue
 data sInputType 101FDF84
 data sInputGuard 101FD6A8
 data sOutputType 101FDF88
 data sOutputGuard 101FD6AC
 data sDefaultRootTable 101455A4
 data sDefaultStateTable 10145594
 data sDefaultRoot 104A0280
 data sDefaultState 104A0284
 ext 0273AF40 void releaseRenderObject(void*)
 ext 0274D67C void destroyRenderTaskBase(void*,u32)
*/
