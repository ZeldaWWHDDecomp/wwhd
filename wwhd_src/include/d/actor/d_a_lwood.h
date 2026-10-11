/** Normal tree, WWHD layout.
 * HD: the larger actor base shifts tww members by 0x11C. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_LWOOD_BDL_ALWD_e = 4, dRes_INDEX_LWOOD_DZB_ALWD_e = 7 };
struct daLwood_c : fopAc_ac_c {
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    f32 getYureScale() const { return mScale; }
    s16 getYureTimer() const { return mTimer; }
    void setMoveBGMtx();
    void set_mtx();

    static const char m_arcname[];
    BOOL CreateHeap();
    void CreateInit();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ Mtx34 mtx;
    /* 0x3EC */ be<s16> mTimer;
    /* 0x3EE */ u8 _3EE[2];
    /* 0x3F0 */ be<f32> mScale;
};
WWHD_OFFSET(daLwood_c, mtx, 0x3BC);
WWHD_OFFSET(daLwood_c, mScale, 0x3F0);

WWHD_SIZE(daLwood_c, 0x3F4);
// HD: private views of the model's matrix storage and joint resource table.
namespace lwood_layout {
struct MatrixBlock { u8 pad[4]; be<u16> flags; u8 padFlags[10]; gptr<Mtx34> matrices; };
struct Model {
    u8 pad[0x2C]; gptr<MatrixBlock> matrixBlock;
    u8 padData[0xAC-0x30]; gptr<J3DModelData> modelData;
    u8 padUser[8]; gptr<daLwood_c> userArea;
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
