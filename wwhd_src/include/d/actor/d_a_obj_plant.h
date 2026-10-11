/** Plant actor, WWHD layout.
 * HD: actor base and collision objects enlarge the GameCube layout; tww member names remain. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_PLANT_BDL_YRMWD_e = 3, JA_SE_OBJ_TREE_SWING_S = 0x69E6 };
struct daObjPlant_c : fopAc_ac_c {
    cPhs_State _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();

    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ be<s16> field_0x408;
    /* 0x526 */ be<s16> field_0x40A;
    /* 0x528 */ be<s16> mHitTimer;
    /* 0x52A */ be<s16> field_0x40E;
    /* 0x52C */ be<u8> field_0x410;
    /* 0x52D */ u8 _52D[3];
};
WWHD_OFFSET(daObjPlant_c, mCyl, 0x3F4);
WWHD_OFFSET(daObjPlant_c, field_0x410, 0x52C);
WWHD_SIZE(daObjPlant_c, 0x530);


// HD: private views of the model's matrix storage and joint resource table.
namespace plant_layout {
struct MatrixBlock { u8 pad[4]; be<u16> flags; u8 padFlags[10]; gptr<Mtx34> matrices; };
struct Model {
    u8 pad[0x2C]; gptr<MatrixBlock> matrixBlock;
    u8 padData[0xAC-0x30]; gptr<J3DModelData> modelData;
    u8 padUser[8]; gptr<daObjPlant_c> userArea;
};
struct Joint { u8 pad[4]; be<u16> jointNo; u8 padNo[2]; be<u32> callback; u8 padCallback[0x10]; };
struct ModelData { u8 pad[4]; be<u32> jointCount; gptr<Joint> joints; };
struct ResourceHeader { u8 pad[0x10]; be<u32> jointNameOffset; };
WWHD_OFFSET(MatrixBlock, matrices, 0x10);
WWHD_SIZE(MatrixBlock, 0x14);
WWHD_OFFSET(Model, modelData, 0xAC);
WWHD_OFFSET(Model, userArea, 0xB8);
WWHD_SIZE(Model, 0xBC);
WWHD_OFFSET(Joint, jointNo, 4);
WWHD_OFFSET(Joint, callback, 8);
WWHD_SIZE(Joint, 0x1C);
WWHD_SIZE(ModelData, 0xC);
WWHD_SIZE(ResourceHeader, 0x14);
}
