/* Compiler companion for the merged JGadget pointer-vector translation unit. */
#pragma once
#include "JSystem/JGadget/vector.h"
struct VectorDestructorRecord { be<u32> mWords[3]; };
WWHD_SIZE(VectorDestructorRecord,12);
void registerVectorCompanion(VectorDestructorRecord* record);
