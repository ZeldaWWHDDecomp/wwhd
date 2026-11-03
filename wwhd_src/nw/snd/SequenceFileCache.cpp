/* Nine-entry file address cache and archive loader lifecycle, reconstructed from WWHD. */
#include "nw/snd/SequenceFileCache.h"
using namespace nw::snd;
void* allocateSequenceCache(u32 size);
void releaseSequenceCache(void* storage);
ArchiveLoader* constructSequenceLoader(ArchiveLoader* loader);
void destroySequenceLoader(ArchiveLoader* loader,u32 flags);
void attachSequenceArchive(ArchiveLoader* loader,SoundArchive* archive);
/* 028AD988 */
void SequenceFileCache::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mReleased!=0) return;
    mActive=0;
    mReleased=1;
    attachSequenceArchive(&mLoader,nullptr);
}
/* 028AD9B0 */
void SequenceFileCache::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this==nullptr) return;
    mVtable=gabi::at<SequenceFileCacheVtable>(0x10187F18u);
    mLoader.vtable=gabi::at<ArchiveLoaderVtable>(0x10187F48u);
    finalize();
    destroySequenceLoader(&mLoader,0);
    if (flags&1) releaseSequenceCache(this);
}
/* 028ADA24 */
void* SequenceFileCache::exchange(u32 id,void* address) {
    for (u32 index=0;index<9;++index) {
        SequenceFileCacheEntry& entry=mEntries[index];
        if (entry.mId==id) {
            void* old=entry.mAddress;
            entry.mAddress=address;
            return old;
        }
    }
    for (u32 index=0;index<9;++index) {
        SequenceFileCacheEntry& entry=mEntries[index];
        if (entry.mId==0xFFFFFFFFu) {
            entry.mId=id;
            entry.mAddress=address;
            return nullptr;
        }
    }
    return nullptr;
}
/* 028ADA94 */
void* SequenceFileCache::exchangeAdjusted(ArchiveLoader* loader,u32 id,void* address) {
    gabi::AutoFrame frame{gabi::current_function()};
    return gabi::at<SequenceFileCache>(gabi::ea(loader)-0xC)->exchange(id,address);
}
/* 028ADA9C */
void* SequenceFileCache::find(u32 id) const {
    for (u32 index=0;index<9;++index) {
        const SequenceFileCacheEntry& entry=mEntries[index];
        if (entry.mId==id) return entry.mAddress;
    }
    return nullptr;
}
/* 028ADAD4 */
void* SequenceFileCache::findAdjusted(const ArchiveLoader* loader,u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    return gabi::at<const SequenceFileCache>(gabi::ea(loader)-0xC)->find(id);
}
/* 028ADADC */
void* SequenceFileCache::findVirtual(u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    return gabi::tail_ptr<void*>(mVtable->find.function,this,id);
}
/* 028ADAEC */
void* SequenceFileCache::findVirtualAdjusted(ArchiveLoader* loader,u32 id) {
    gabi::AutoFrame frame{gabi::current_function()};
    return gabi::at<SequenceFileCache>(gabi::ea(loader)-0xC)->findVirtual(id);
}
/* 028ADAF4 */
SequenceFileCache* SequenceFileCache::construct(SequenceFileCache* cache) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (cache==nullptr) cache=static_cast<SequenceFileCache*>(allocateSequenceCache(sizeof(SequenceFileCache)));
    if (cache==nullptr) return cache;
    cache->mReserved[0]=0;
    cache->mReserved[1]=0;
    cache->mVtable=gabi::at<SequenceFileCacheVtable>(0x10187F00u);
    constructSequenceLoader(&cache->mLoader);
    cache->mActive=0;
    cache->mVtable=gabi::at<SequenceFileCacheVtable>(0x10187F18u);
    cache->mReleased=1;
    cache->mLoader.vtable=gabi::at<ArchiveLoaderVtable>(0x10187F48u);
    return cache;
}
/* 028ADB80 */
void SequenceFileCache::initialize(SoundArchive* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mActive!=0) return;
    mActive=1;
    mReleased=0;
    for (u32 index=0;index<9;++index) {
        mEntries[index].mId=0xFFFFFFFFu;
        mEntries[index].mAddress=nullptr;
    }
    attachSequenceArchive(&mLoader,archive);
}
/* 028ADBC0 */
void SequenceFileCache::clear() {
    for (u32 index=0;index<9;++index) {
        mEntries[index].mId=0xFFFFFFFFu;
        mEntries[index].mAddress=nullptr;
    }
}
/* WWHD glue
ext 0273AD10 void* allocateSequenceCache(u32)
ext 0273AF40 void releaseSequenceCache(void*)
ext 02890F88 nw::snd::ArchiveLoader* constructSequenceLoader(nw::snd::ArchiveLoader*)
ext 02890FD0 void destroySequenceLoader(nw::snd::ArchiveLoader*,u32)
ext 02892980 void attachSequenceArchive(nw::snd::ArchiveLoader*,nw::snd::SoundArchive*)
*/
