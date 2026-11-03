/* Initializer at the WWHD JFWSystem/JAudio boundary. */
#pragma once
#include "bindings.h"
struct JFWBoundaryStatics { be<u32> mWords[4]; };
struct JFWBoundaryDestructorRecord { be<u32> mWords[3]; };
WWHD_SIZE(JFWBoundaryStatics, 16);
WWHD_SIZE(JFWBoundaryDestructorRecord, 12);
void registerJFWBoundaryStatics(JFWBoundaryDestructorRecord*);
