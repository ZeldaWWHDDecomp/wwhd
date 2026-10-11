#pragma once
#include "bindings.h"
struct ResTIMG;
enum {
    dRes_INDEX_AYUSH_BDL_AYUSH_e = 5,
    dRes_INDEX_AYUSH_BTK_AYUSH_e = 8,
    dRes_INDEX_AYUSH_BTI_B_GSHIP_HATA_e = 0xB,
    dRes_INDEX_AYUSH_BTI_B_GSHIP_HO_e = 0xC,
    dRes_INDEX_CLOTH_BTI_CLOTHTOON_e = 3,
};
#include "d/d_lib.h"
namespace ghostshipHD {
struct HeaderZeros {
    be<u32> zero0, zero1, zero2, zero3;
};
struct DestructorRecord {
    u8 record[12];
};
struct Cloth;
} // namespace ghostshipHD

struct daGhostship_c : fopAc_ac_c {
    enum Mode { MODE_WAIT, MODE_REALIZE, MODE_PATH_MOVE };
    using ModeProcFunc = void (daGhostship_c::*)();

    BOOL _createHeap();
    BOOL _pathMove(cXyz* position, cXyz* current, cXyz* next);
    void createInit();
    void modeProcCall();
    BOOL _execute();
    BOOL _draw();
    void pathMove();
    void modePathMove();
    void getArg();
    void modePathMoveInit();
    void modeWaitInit();
    void modeWait();
    void modeRealize();

    be<u32> mMode;
    be<s32> mPathNo;
    be<u8> moonPhase;
    u8 pad3B5[3];
    cXyz mCurPntPos;
    be<s8> mPathPntNo;
    u8 pad3C5[3];
    gptr<u8> mPath;
    be<u32> field_0x2B0;
    cXyz mNextPntPos;
    be<u32> field_0x2C0, field_0x2C4, field_0x2C8;
    request_of_phase_process_class mPhs, mClothPhs;
    gptr<ghostshipHD::Cloth> mpCloth, mpCloth2;
    gptr<J3DModel> mpModel;
    mDoExt_btkAnm mBtk;
    be<f32> mAlpha;
    dBgS_ObjAcch mAcch;
    dBgS_AcchCir mCir;
    dLib_wave_c mWave;
    be<u8> mbEnteredShip;
    u8 pad68D[3];
    dLib_circle_path_c mPaths[12];
    be<f32> field_0x6C4[12];
    cXyz mPathPos;
    be<u32> field_0x700;
    be<f32> mPathSpeed;
    be<u8> mbCanEnterShip;
    u8 pad885[3];
};
WWHD_OFFSET(daGhostship_c, mMode, 0x3AC);
WWHD_OFFSET(daGhostship_c, mCurPntPos, 0x3B8);
WWHD_OFFSET(daGhostship_c, mNextPntPos, 0x3D0);
WWHD_OFFSET(daGhostship_c, mPhs, 0x3E8);
WWHD_OFFSET(daGhostship_c, mpCloth, 0x3F8);
WWHD_OFFSET(daGhostship_c, mpModel, 0x400);
WWHD_OFFSET(daGhostship_c, mBtk, 0x404);
WWHD_OFFSET(daGhostship_c, mAlpha, 0x478);
WWHD_OFFSET(daGhostship_c, mAcch, 0x47C);
WWHD_OFFSET(daGhostship_c, mCir, 0x640);
WWHD_OFFSET(daGhostship_c, mWave, 0x680);
WWHD_OFFSET(daGhostship_c, mPaths, 0x690);
WWHD_OFFSET(daGhostship_c, field_0x6C4, 0x840);
WWHD_OFFSET(daGhostship_c, mPathPos, 0x870);
WWHD_OFFSET(daGhostship_c, mbCanEnterShip, 0x884);
WWHD_SIZE(daGhostship_c, 0x888);

// HD: cloth packets include renderer storage before the GameCube simulation fields.
namespace ghostshipHD {
// HD: the save singleton has a 0x20-byte prefix before dSv_info_c.
struct SaveBlock {
    u8 prefix[0x20];
    dSv_info_c save;
};
struct SceneList {
    char name[8];
    be<u8> start;
    be<u8> room;
    u8 tail[2];
};

struct FloatColor {
    be<f32> r, g, b, a;
};
struct TevVtable {
    u8 precedingSlots[0x3C];
    be<u32> setTevKColor;
    u8 interveningSlots[0xC];
    be<u32> getTevKColor;
};
struct TevBlock {
    u8 rendererHeader[4];
    gptr<TevVtable> vtable;
    GXColor* getTevKColor(u32 index) {
        return gabi::call_ptr<GXColor*>(vtable->getTevKColor, this, index);
    }
    void setTevKColor(u32 index, GXColor* color) {
        gabi::call_ptr<void>(vtable->setTevKColor, this, index, color);
    }
};
struct UniformBlock {
    be<u32> flags;
};
struct Material {
    u8 precedingBlocks[0x18];
    gptr<TevBlock> mTevBlock;
    u8 otherBlocks[0x84];
    UniformBlock uniforms;
    u8 rendererTail[0x2F8];
};
WWHD_OFFSET(Material, uniforms, 0xA0);
WWHD_SIZE(Material, 0x39C);
struct MaterialTable {
    u8 precedingData[0x24];
    be<u16> mMaterialNum;
};
struct ModelDataTail {
    u8 precedingData[0x44];
    be<u32> mTextureAnimation;
};

struct Hio {
    be<u32> vtable;
    be<u8> field_0x04;
    be<u8> hideShip;
    u8 padding[2];
    be<f32> shipAlpha;
    be<f32> field_0x0C;
    be<f32> shipHideDist;
    be<f32> shipEnterDist;
};
WWHD_SIZE(Hio, 0x18);

struct DebugWord {
    be<u32> value;
};

struct ModeProc {
    gabi::ptmf<daGhostship_c::ModeProcFunc> fn;
};
WWHD_SIZE(ModeProc, 8);

struct ClothVtable {
    u8 precedingSlots[0xC];
    be<u32> destroy;
    u8 packetSlots[0x2C];
    be<u32> cloth_move;
    u8 interveningSlot[4];
    be<u32> cloth_draw;
};
struct Cloth {
    u8 packetHeader[0xC];
    gptr<ClothVtable> vtable;
    u8 rendererAndGeometry[0x9C];
    be<u32> mpFactorCheckCB;
    u8 geometryAndWind[0x34];
    be<f32> mWindSpeed;
    be<f32> mWindSpeedWave;
    u8 rendererMatrices[0xB4];
    be<f32> mSpring;
    be<f32> mGravity;
    be<f32> mDrag;
    be<f32> mFlyFlex;
    be<f32> mHoistFlex;
    be<s16> mWave;
    be<u16> mWaveSpeed;
    be<s16> field_0xF0;
    be<u16> field_0xF2;
    be<u16> mRipple;
    be<u16> mRotateY;
    void destroy(s32 flags) {
        gabi::call_ptr<void>(vtable->destroy, this, flags);
    }
    void cloth_move() {
        gabi::call_ptr<void>(vtable->cloth_move, this);
    }
    void cloth_draw() {
        gabi::call_ptr<void>(vtable->cloth_draw, this);
    }
};
WWHD_OFFSET(Cloth, mpFactorCheckCB, 0xAC);
WWHD_OFFSET(Cloth, mSpring, 0x1A0);
WWHD_OFFSET(Cloth, mWaveSpeed, 0x1B6);
WWHD_OFFSET(Cloth, mRotateY, 0x1BE);
WWHD_SIZE(Cloth, 0x1C0);

// Integer transport preserves signaling NaNs in the HD point-copy instructions.
struct FloatWord {
    be<u32> bits;
};
} // namespace ghostshipHD
