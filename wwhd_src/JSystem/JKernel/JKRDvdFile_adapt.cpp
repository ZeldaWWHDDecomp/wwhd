// HD DVD lifetime adapter; reconstructed from the game's instructions.
#include "bindings.h"
#include "JSystem/JKernel/JKRDvdFile_adapt.h"

static JKRDvdFileVtable sDvdFileTable;

/* 027EAD44 */
void JKRDvdFileLifetime::destroy(u32 flags) {
    if (this != NULL) {
        __vtbl = &sDvdFileTable;
        close();
        destroyNativeDvdLink(&mDvdLink,0);
        destroyNativeDisposer(reinterpret_cast<sead::Disposer*>(this),0);
        if (flags & 1) deleteNativeDvdFile(this);
    }
}

/* WWHD glue
 data sDvdFileTable 1016DB24
 ext 027EACBC void JKRDvdFile::close()
 ext 027EC62C void destroyNativeDvdLink(JSUPtrLink*,u32)
 ext 02752BEC void destroyNativeDisposer(sead::Disposer*,u32)
 ext 0273AF40 void deleteNativeDvdFile(void*)
*/
