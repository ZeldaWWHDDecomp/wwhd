// Moving-background callbacks.
#pragma once
#include "bindings.h"
// HD: the GameCube poly-info reference is passed as a guest pointer.
/* 024EE658 [v d_a_bmd]: callback address stored by the actor's heap setup. */
void dBgS_MoveBGProc_Typical(dBgW*, void*, cBgS_PolyInfo*, bool, cXyz*, csXyz*, csXyz*);
// HD: ext glue defines the entry adapter but does not register external callback symbols.
inline gabi::HostFn dBgS_MoveBGProc_TypicalSymbol((const void*)&dBgS_MoveBGProc_Typical, 0x024EE658);
