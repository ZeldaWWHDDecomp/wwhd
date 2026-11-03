// Destructor view of the canonical HD shader archive manager; no additional fields.
#pragma once
#include "hd/hd_shader_archive.h"
namespace agl { namespace env {
struct EnvironmentShaderArchiveLifetime_l : hdShaderArchiveMgr_c {
    void destroy(u32 flags);
};
}}
WWHD_SIZE(agl::env::EnvironmentShaderArchiveLifetime_l,0x20);
