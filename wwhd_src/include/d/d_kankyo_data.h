#pragma once
#include "gc_api/gc_api.h"

struct GXFogAdjTable {
    be<u16> fogVals[10];
};
WWHD_SIZE(GXFogAdjTable, 20);

// HD: Only this environment prefix is accessed by the admitted fog-table copy.
struct dKyd_environment_c {
    u8 padding[0xB50];
    GXFogAdjTable mFogAdjTable;
};
WWHD_OFFSET(dKyd_environment_c, mFogAdjTable, 0xB50);
WWHD_SIZE(dKyd_environment_c, 0xB64);

// Palette/skybox getters expose addresses only; their HD layouts remain opaque.
WWHD_OPAQUE(stage_palet_info_class);
struct stage_pselect_info_class {
    be<u8> palette_id[8];
    be<f32> change_rate;
};
WWHD_OFFSET(stage_pselect_info_class, change_rate, 8);
WWHD_SIZE(stage_pselect_info_class, 12);
struct stage_envr_info_class {
    be<u8> pselect_id[8];
};
WWHD_SIZE(stage_envr_info_class, 8);
WWHD_OPAQUE(stage_vrbox_info_class);
struct dKyd_Schedule {
    be<f32> mTimeEnd;
    be<f32> mTimeBegin;
    be<u8> mPalIdx0;
    be<u8> mPalIdx1;
    u8 padding[2];
};
WWHD_OFFSET(dKyd_Schedule, mTimeBegin, 4);
WWHD_OFFSET(dKyd_Schedule, mPalIdx0, 8);
WWHD_SIZE(dKyd_Schedule, 12);
