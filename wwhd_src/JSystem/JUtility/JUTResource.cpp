/** JUTResource platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JUtility/JUTResource.h"
static JUTResourceStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02843198 */
static void __sinit_JUTResource_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JUTResourceConstructRegister(&sStaticRegister);
}


/* 02843098 */
void* JUTResReference::getResource(u32 type,void* archive) {
    switch ((u8)mType) {
    case 2: return JUTArchiveGetResource(type,mName,archive);
    case 3: return JUTFileGetResource(mName,archive);
    case 4: return JUTFileGetGlobalResource(mName);
    default: return NULL;
    }
}
/* 02843104 */
void* JUTResReference::getResourceFromStream(JSUInputStream* stream,u32 type,void* archive) {
    stream->read(&mType,1);
    stream->read(&mNameLength,1);
    stream->read(mName,mNameLength);
    if ((u32)((u8)mType - 2) <= 2) mName[(u8)mNameLength] = 0;
    return getResource(type,archive);
}

/* WWHD glue
 ext 027EA948 void* JUTArchiveGetResource(u32,const void*,void*)
 ext 027EBF60 void* JUTFileGetResource(const void*,void*)
 ext 027EBF08 void* JUTFileGetGlobalResource(const void*)
 data sStaticBlock 104B6534
 data sStaticRegister 101FAFD8
 ext 028F026C void JUTResourceConstructRegister(void*)
*/
