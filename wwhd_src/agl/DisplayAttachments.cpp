// Releases the attachments the view acquired from the shared pool of the dynamic texture cache ( flag 4 set by
// configureAttachments); attachments borrowed from the other view are only cleared.
#include "agl/DisplayFrameState.h"
#include "agl/DynamicTextureCache.h"
using namespace agl::render;
static agl::TextureCacheEntry* acquiredAttachment(agl::TextureData* texture) {
    return static_cast<agl::TextureCacheEntry*>(texture);
}
/* 027D5C48 */
void agl::render::DisplayFrameState::releaseAttachments() {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)mFlags & 4u) {
        agl::TextureData* first = mAttachment;
        agl::DynamicTextureCache* cache = sDynamicTextureCache;
        if (first) {
            cache->releaseShared(acquiredAttachment(first));
            mAttachment = NULL;
        }
        agl::TextureData* second = mSecondaryAttachment;
        if (second) {
            cache->releaseShared(acquiredAttachment(second));
            mSecondaryAttachment = NULL;
        }
        agl::TextureData* extra = mExtraAttachment;
        if (extra) {
            cache->releaseShared(acquiredAttachment(extra));
            mExtraAttachment = NULL;
        }
        mFlags = (u32)mFlags & ~4u;
    }
    mAttachment = NULL;
    mSecondaryAttachment = NULL;
}
/* WWHD glue
ext 027A0114 void agl::DynamicTextureCache::releaseShared(agl::TextureCacheEntry*)
*/
