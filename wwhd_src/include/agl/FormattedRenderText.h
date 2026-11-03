// Fixed-256-byte formatted text constructor; the six trailing words are the EABI integer argument registers.
#pragma once
#include "sead/FileDevice.h"
namespace agl { namespace render {
sead::PathText* createFormattedRenderText(sead::PathText*,const char*,u32,u32,u32,u32,u32,u32);
} }
