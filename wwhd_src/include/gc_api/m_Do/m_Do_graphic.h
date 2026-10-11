// Graphics controls for verified HD callers.
#pragma once
#include "m_Do/m_Do_graphic.h"
/* 101F4808 [v d_a_bmd]: process-global renderer state. */
inline mDoGph_StateHD* mDoGph_getState() { return gabi::at<mDoGph_StateHD>(0x101F4808); }
/* 025F064C [v d_a_bmd] */
inline void mDoGph_gInf_c::onBlure() { gabi::call(0x025F064C); }
