// Canonical SEAD projection and viewport types used by the local stack adapters.
#pragma once
#include "sead/PerspectiveState.h"
#include "sead/ProjectionState.h"
#include "sead/Viewport.h"
WWHD_SIZE(sead::PerspectiveState,0xB8);
WWHD_SIZE(sead::ProjectionState,0xAC);
WWHD_SIZE(sead::Viewport,0x18);
