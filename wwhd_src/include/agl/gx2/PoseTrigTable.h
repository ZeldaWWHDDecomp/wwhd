/* The pose sine/cosine table (1016C7E0, .rodata): 256 records of sine, cosine and their slopes, indexed by the top
 * byte of an encoded angle. Shared by the AGL GX2 pose units (AGL-GX2#1 owns the users, AGL-GX2#3 reads it too);
 * owned by agl/gx2/PoseTrigTable.cpp. Layout from the original's indexed loads (16-byte records). */
#pragma once
#include "wwhd.h"
namespace agl { namespace gx2util {
struct PoseTrigEntry { be<f32> mSin, mCos, mSinSlope, mCosSlope; };
struct PoseTrigTable { PoseTrigEntry mEntries[256]; };
WWHD_SIZE(PoseTrigEntry, 16);
WWHD_SIZE(PoseTrigTable, 0x1000);
} }
extern agl::gx2util::PoseTrigTable& sPoseTrigTable;
