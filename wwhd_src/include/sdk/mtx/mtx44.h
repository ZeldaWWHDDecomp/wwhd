// SDK 4x4 matrix routines (the Mtx44 layout is in wwhd.h, shared with the MTX unit).
#pragma once
#include "wwhd.h"
void PSMTX44Identity(Mtx44* matrix);
void PSMTX44Copy(const Mtx44* source, Mtx44* dest);
void PSMTX44Concat(const Mtx44* a, const Mtx44* b, Mtx44* ab);
void PSMTX44Trans(Mtx44* matrix, f32 x, f32 y, f32 z);
