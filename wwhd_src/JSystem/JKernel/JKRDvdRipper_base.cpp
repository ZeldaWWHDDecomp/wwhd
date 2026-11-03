/** JKRDvdRipper_base platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRDvdRipper_base.h"
#include "JSystem/JSupport/JSUList.h"
static JKRDvdRipper_baseStaticBlock sStaticBlock;
static u32 sStaticRegister;
static JSUPtrList sFileList;
static u32 sFileListRegister;

/* 027EBC00 */
static void __sinit_JKRDvdRipper_base_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRDvdRipper_baseConstructRegister(&sStaticRegister);
    JKRDvdRipper_baseInitiateList(&sFileList);
    JKRDvdRipper_baseConstructRegister(&sFileListRegister);
}


/* 027EAE14 */
static u32 JKRDvdRipperGetFileSize(JKRDvdFile* file) {
    return JKRDvdFileGetSize(file->mFileInfo);
}
/* 027EAE1C */
static void JKRDvdRipperList__dt(JSUPtrList* list,u32 flags) {
    if (list == NULL) return;
    JKRDvdRipperListDestroy(list,0);
    if (flags & 1) operator_delete(list);
}


/* 027EBB2C */
static void* JKRDvdRipperLoadToMainRAM(const void* name,void* destination,u32 expandSwitch,u32 destinationLength,void* heap,u32 allocationDirection,u32 offset,void* compression,void* decompressedSize) {
    gabi::Local<JKRDvdFile> file;
    JKRDvdRipperFileConstruct(file.get());
    gabi::Local<JKRDvdPath> path;
    JKRDvdRipperCopyName(path.get(),name,0x101);
    if (file->open(path.get()) == 0) {
        JKRDvdRipperFileDestroy(file.get(),2);
        return NULL;
    }
    void* result = JKRDvdRipperLoadFile(file.get(),destination,expandSwitch,destinationLength,heap,allocationDirection,offset,compression,decompressedSize);
    JKRDvdRipperFileDestroy(file.get(),2);
    return result;
}

/* WWHD glue
 ext 027EABB8 JKRDvdFile* JKRDvdRipperFileConstruct(JKRDvdFile*)
 ext 027EAD44 void JKRDvdRipperFileDestroy(JKRDvdFile*,u32)
 ext 027EB5F4 void* JKRDvdRipperLoadFile(JKRDvdFile*,void*,u32,u32,void*,u32,u32,void*,void*)
 ext C000A850 void JKRDvdRipperCopyName(void*,const void*,u32)
 ext 0284B690 u32 JKRDvdFileGetSize(void*)
 ext 027EC68C void JKRDvdRipperListDestroy(JSUPtrList*,u32)
 data sStaticBlock 104A23B8
 data sStaticRegister 101F9734
 data sFileList 104A23F4
 data sFileListRegister 101F9740
 ext 027EC678 void JKRDvdRipper_baseInitiateList(JSUPtrList*)
 ext 028F026C void JKRDvdRipper_baseConstructRegister(void*)
*/
