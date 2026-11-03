// The shader archive manager singleton (101F8A94) and its disposer holder (101F8A98), shared with the units that
// read or clear them (AGL#1's manager destructor); declared in hd/hd_shader_archive.h.
#include "hd/hd_shader_archive.h"
static hdShaderArchiveMgr_c* sShaderArchiveShaderManager;
static hdShaderArchiveMgr_c* sShaderArchiveShaderManagerDisposer;
/* WWHD glue
 data sShaderArchiveShaderManager 101F8A94
 data sShaderArchiveShaderManagerDisposer 101F8A98
*/
