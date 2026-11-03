// J3D HD renderer: the 15 texture slots of a material (owned or borrowed textures) and their binding.
// Names are our own; written from the WWHD binary.
#include "JSystem/J3DGraph/RenderTextureBindings.h"

/* 027FE344 */
j3d::RenderTextureBindings* j3d::RenderTextureBindings::construct(RenderTextureBindings* storage) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage == NULL)
        storage = static_cast<RenderTextureBindings*>(allocateRenderTextureObject(sizeof(RenderTextureBindings)));
    if (storage != NULL) {
        prepareRenderTextureStorage(storage->mTextures, sizeof(storage->mTextures));
        prepareRenderTextureStorage(storage->mOwned, sizeof(storage->mOwned));
        storage->mReady = 1;
        for (u32 index = 0; index < 15; ++index) {
            storage->mTextures[index] = NULL;
            storage->mOwned[index] = 0;
        }
    }
    return storage;
}

/* 027FE3D4 */
void j3d::RenderTextureBindings::clear(u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (mOwned[index] != 0) {
        RenderTexture_l* texture = mTextures[index];
        if (texture != NULL)
            gabi::call_ptr<void>(texture->mMethods->mDestroy.mFunction, texture, 3u);
        mTextures[index] = NULL;
        mOwned[index] = 0;
    }
}

/* 027FE458 */
void j3d::RenderTextureBindings::clearAll() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < 15; ++index) clear(index);
}

/* 027FE4B0 */
void j3d::RenderTextureBindings::destroy(RenderTextureBindings* storage, u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (storage != NULL) {
        storage->clearAll();
        if (flags & 1) releaseRenderTextureObject(storage);
    }
}

/* 027FE500 */
u32 j3d::RenderTextureBindings::allocate(u32 index, void* resource, void* heap) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (mOwned[index] != 0) return 0;
    auto* texture = static_cast<RenderTexture_l*>(allocateRenderTextureObjectAligned(sizeof(RenderTexture_l), heap, 4));
    if (texture != NULL) texture = constructRenderTexture(texture, resource);
    mTextures[index] = texture;
    if (texture == NULL) return 0;
    mOwned[index] = 1;
    return 1;
}

/* 027FE5B8 */
void j3d::RenderTextureBindings::bindOwned(const RenderTextureSlots_l* slots) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    for (u32 index = 0; index < 15; ++index) {
        if (mOwned[index] != 0) {
            const RenderTextureSlot_l* slot = guestNull<RenderTextureSlot_l>();
            if (index < (u32)slots->mCount) slot = &slots->mSlots.get()[index];
            bindRenderTexture(mTextures[index], slot->mSampler, index, 0);
        }
    }
}

struct RenderFallbackTextures_l {
    u8 mPrefix[0x14];
    gptr<j3d::RenderTexture_l> mPrimary;
    u8 mGap[4];
    gptr<j3d::RenderTexture_l> mSecondary;
};
static RenderFallbackTextures_l* sDefaultTextures;
static j3d::RenderTexture_l* sSystemTextureA;
static j3d::RenderTexture_l* sSystemTextureB;

/* 027FFE54 */
void j3d::RenderTextureBindings::bindExtra(const RenderTextureSlots_l* slots) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    RenderTexture_l* primary = sDefaultTextures->mPrimary;
    RenderTexture_l* secondary = sDefaultTextures->mSecondary;
    const u32 sourceIndices[5] = {6, 8, 9, 10, 11};
    const u32 outputSlots[5] = {8, 10, 11, 9, 12};
    for (u32 index = 0; index < 5; ++index) {
        RenderTexture_l* texture = mTextures[sourceIndices[index]];
        if (texture == NULL) texture = index == 3 ? secondary : primary;
        const RenderTextureSlot_l* slot = guestNull<RenderTextureSlot_l>();
        if (outputSlots[index] < (u32)slots->mCount) slot = &slots->mSlots.get()[outputSlots[index]];
        bindRenderTexture(texture, slot->mSampler, outputSlots[index], 0);
    }
    RenderTexture_l* extraA = sSystemTextureA;
    RenderTexture_l* extraB = sSystemTextureB;
    if (extraA == NULL) extraA = secondary;
    const RenderTextureSlot_l* slotA = guestNull<RenderTextureSlot_l>();
    if ((u32)slots->mCount > 13) slotA = &slots->mSlots.get()[13];
    bindRenderTexture(extraA, slotA->mSampler, 13, 0);
    if (extraB == NULL) extraB = secondary;
    const RenderTextureSlot_l* slotB = guestNull<RenderTextureSlot_l>();
    if ((u32)slots->mCount > 14) slotB = &slots->mSlots.get()[14];
    bindRenderTexture(extraB, slotB->mSampler, 14, 0);
}

/* WWHD glue
 data sDefaultTextures 101F8DEC
 data sSystemTextureA 104B45C0
 data sSystemTextureB 104B45C4
 ext 027BE53C void bindRenderTexture(j3d::RenderTexture_l*,const void*,u32,u32)
 ext 0273AD10 void* allocateRenderTextureObject(u32)
 ext 0273B050 void* allocateRenderTextureObjectAligned(u32,void*,s32)
 ext 028F521C void prepareRenderTextureStorage(void*,u32)
 ext 0273AF40 void releaseRenderTextureObject(void*)
 ext 027BE114 j3d::RenderTexture_l* constructRenderTexture(j3d::RenderTexture_l*,void*)
*/
