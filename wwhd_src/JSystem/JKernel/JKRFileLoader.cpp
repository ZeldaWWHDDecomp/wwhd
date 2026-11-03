/** JKRFileLoader platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JSupport/JSUList.h"
static JKRFileLoaderStaticBlock sStaticBlock;
static u32 sStaticRegister;
static u32 sFileListRegister;

/* 027EBFFC */
static void __sinit_JKRFileLoader_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRFileLoaderConstructRegister(&sStaticRegister);
    JKRFileLoaderInitiateList(&sVolumeList);
    JKRFileLoaderConstructRegister(&sFileListRegister);
}


/* 027EC058 */
static void JKRFileLoaderList__dt(JSUPtrList* list,u32 flags) {
    if (list == NULL) return;
    JKRFileLoaderListDestroy(list,0);
    if (flags & 1) operator_delete(list);
}


static JKRFileLoaderRootPath sRootPath;
static JKRFileLoader* sCurrentVolume;
/* 027EBD6C */
const be<u8>* JKRFileLoader::fetchVolumeName(be<u8>* buffer,s32 bufferSize,const be<u8>* path) {
    // HD: the inline comparison and lowercase conversion only recognize ASCII.
    const be<u8>* compare = path;
    const char* root = "/";
    u32 ch, expected;
    do { ch = *compare++; expected = (u8)*root++; } while (ch == expected && ch != 0);
    if (ch == expected) {
        JKRFileLoaderCopyString(buffer,&sRootPath);
        return (const be<u8>*)&sRootPath;
    }
    ++path;
    while ((u8)*path != 0 && (u8)*path != '/') {
        if (bufferSize > 1) {
            u32 character = *path;
            if (character >= 'A' && character <= 'Z') character += 32;
            *buffer++ = character;
            --bufferSize;
        }
        ++path;
    }
    *buffer = 0;
    if ((u8)*path == 0) path = (const be<u8>*)&sRootPath;
    return path;
}
/* 027EBE3C */
JKRFileLoader* JKRFileLoader::findVolume(gptr<const be<u8>>* name) {
    const be<u8>* path = *name;
    if ((u8)*path != '/') return sCurrentVolume;
    gabi::Local<JKRVolumeNameBuffer> buffer;
    *name = fetchVolumeName(buffer->mBytes,0x101,path);
    JSUPtrLink* link = sVolumeList.mHead;
    while (link != NULL) {
        JKRFileLoader* volume = (JKRFileLoader*)(void*)link->mObject;
        const be<u8>* volumeName = volume->mVolumeName;
        const be<u8>* text = buffer->mBytes;
        u32 a,b;
        do { a = *text++; b = *volumeName++; } while (a == b && a != 0);
        if (a == b) return (JKRFileLoader*)(void*)link->mObject;
        link = link->mNext;
    }
    return NULL;
}
/* 027EBF08 */
void* JKRFileLoader::getGlbResource(const be<u8>* name) {
    gabi::Local<gptr<const be<u8>>> remaining;
    *remaining = name;
    JKRFileLoader* volume = findVolume(remaining.get());
    if (volume == NULL) return NULL;
    return gabi::call_ptr<void*>(volume->mVtable->mGetResource,volume,(const be<u8>*)*remaining);
}
/* 027EBF60 */
void* JKRFileLoader::getGlbResourceFromVolume(const be<u8>* name,JKRFileLoader* volume) {
    if (volume != NULL) return gabi::call_ptr<void*>(volume->mVtable->mGetTypedResource,volume,0,name);
    void* resource = NULL;
    JSUPtrLink* link = sVolumeList.mHead;
    while (link != NULL) {
        volume = (JKRFileLoader*)(void*)link->mObject;
        resource = gabi::call_ptr<void*>(volume->mVtable->mGetTypedResource,volume,0,name);
        if (resource != NULL) break;
        link = link->mNext;
    }
    return resource;
}

/* WWHD glue
 str 1016DC58 "/"
 data sRootPath 101F976C
 data sCurrentVolume 101F9788
 ext 028F040C void* JKRFileLoaderCopyString(void*,const void*)
 ext 027EC68C void JKRFileLoaderListDestroy(JSUPtrList*,u32)
 data sStaticBlock 104A2420
 data sStaticRegister 101F9770
 data sFileListRegister 101F977C
 ext 027EC678 void JKRFileLoaderInitiateList(JSUPtrList*)
 ext 028F026C void JKRFileLoaderConstructRegister(void*)
*/
