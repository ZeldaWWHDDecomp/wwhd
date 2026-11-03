/** JKRDvdFile platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRDvdFile.h"
#include "JSystem/JSupport/JSUList.h"
static JKRDvdFileStaticBlock sStaticBlock;
static u32 sStaticRegister;
static JSUPtrList sFileList;
static u32 sFileListRegister;

/* 027EADB8 */
static void __sinit_JKRDvdFile_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRDvdFileConstructRegister(&sStaticRegister);
    JKRDvdFileInitiateList(&sFileList);
    JKRDvdFileConstructRegister(&sFileListRegister);
}


/* 027EAC44 */
u32 JKRDvdFile::open(const void* name) {
    u32 available = mIsAvailable;
    if (available == 0) {
        gabi::Local<JKRDvdPath> path;
        JKRDvdCopyPath(path.get(),name,0x101);
        available = JKRDvdOpenPath(path.get(),mFileInfo) != 0;
        mIsAvailable = available;
        if (available != 0) {
            JKRDvdAppend(&sFileList,&mDvdLink);
            JKRDvdGetStatus(this);
            available = mIsAvailable;
        }
    }
    return available;
}
/* 027EACBC */
void JKRDvdFile::close() {
    if ((u8)mIsAvailable == 0) return;
    if (JKRDvdCloseInfo(mFileInfo) != 0) {
        mIsAvailable = 0;
        JKRDvdRemove(&sFileList,&mDvdLink);
    } else {
        JKRDvdCloseAssert("JKRDvdFile.cpp",216,"cannot close DVD file\n");
    }
}

/* WWHD glue
 ext C000A850 void JKRDvdCopyPath(void*,const void*,u32)
 ext 0284B6E8 u32 JKRDvdOpenPath(void*,void*)
 ext 027EAC3C u32 JKRDvdGetStatus(JKRDvdFile*)
 ext 0284B780 u32 JKRDvdCloseInfo(void*)
 ext 027EC6F4 u32 JKRDvdAppend(JSUPtrList*,JSUPtrLink*)
 ext 027EC560 u32 JKRDvdRemove(JSUPtrList*,JSUPtrLink*)
 ext 0273AA24 void JKRDvdCloseAssert(const char*,s32,const char*)
 str 1016DAF8 "JKRDvdFile.cpp"
 str 1016DB08 "cannot close DVD file\n"
 data sStaticBlock 104A2364
 data sStaticRegister 101F9718
 data sFileList 104A2374
 data sFileListRegister 101F9724
 ext 027EC678 void JKRDvdFileInitiateList(JSUPtrList*)
 ext 028F026C void JKRDvdFileConstructRegister(void*)
*/
