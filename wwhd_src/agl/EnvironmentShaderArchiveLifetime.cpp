// Clear the canonical singleton holders only when this is their registered disposer.
#include "agl/EnvironmentShaderArchiveLifetime.h"
using namespace agl::env;
static u32 sEnvironmentShaderArchiveManagerMethods;
void destroyEnvironmentShaderArchiveDisposer(hdShaderArchiveMgr_c*,u32);
void releaseEnvironmentShaderArchiveManager(void*);

/* 0273926C */
void EnvironmentShaderArchiveLifetime_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = gabi::ea(&sEnvironmentShaderArchiveManagerMethods);
    if (gabi::ea(this) == gabi::ea(sShaderArchiveShaderManagerDisposer.get())) {
        sShaderArchiveShaderManagerDisposer = NULL;
        sShaderArchiveShaderManager = NULL;
    }
    destroyEnvironmentShaderArchiveDisposer(this,0);
    if (flags & 1) releaseEnvironmentShaderArchiveManager(this);
}
/* WWHD glue
 data sEnvironmentShaderArchiveManagerMethods 10142E28
 ext 02752BEC void destroyEnvironmentShaderArchiveDisposer(hdShaderArchiveMgr_c*,u32)
 ext 0273AF40 void releaseEnvironmentShaderArchiveManager(void*)
*/
