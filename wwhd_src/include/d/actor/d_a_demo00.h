#pragma once
#include "f_op/f_op_actor.h"

struct Demo00ResourceIDs {
    be<u32> shape, bck, auxiliary, btp, btk, brk, plight;
};
WWHD_SIZE(Demo00ResourceIDs, 0x1C);

struct Demo00Model {
    Demo00ResourceIDs ids;
    gptr<void> morf, model, invisibleModel, auxiliary, btp, btk, brk;
    gptr<void> unused, plight, background;
};
WWHD_SIZE(Demo00Model, 0x44);
WWHD_OFFSET(Demo00Model, morf, 0x1C);
WWHD_OFFSET(Demo00Model, plight, 0x3C);

struct daDemo00_c : fopAc_ac_c {
    be<s16> actionAdjustment, actionIndex;
    be<u32> actionTarget;
    be<u8> drawMode;
    be<s8> previousDrawMode;
    be<u8> groundValid, flags;
    Demo00ResourceIDs nextIDs;
    Demo00Model demoModel;
};
WWHD_SIZE(daDemo00_c, 0x418);
WWHD_OFFSET(daDemo00_c, actionAdjustment, 0x3AC);
WWHD_OFFSET(daDemo00_c, drawMode, 0x3B4);
WWHD_OFFSET(daDemo00_c, nextIDs, 0x3B8);
WWHD_OFFSET(daDemo00_c, demoModel, 0x3D4);
