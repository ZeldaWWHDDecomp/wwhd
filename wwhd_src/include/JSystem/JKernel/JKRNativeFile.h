// HD file entry adapters, using the canonical sead file interfaces.
#pragma once
#include "sead/FileDevice.h"
struct NativeFilePath { be<u8> mBytes[257]; };
WWHD_SIZE(NativeFilePath,257);
u32 queryNativeFileSize(sead::FileHandle*);
u32 openNativeFileAlias(const be<u8>*,sead::FileHandle*);
void* copyNativeFilePath(void*,const void*,u32);
