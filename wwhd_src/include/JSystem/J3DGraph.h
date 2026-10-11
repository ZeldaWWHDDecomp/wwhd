/* J3D (JSystem 3D graph), WWHD layouts and GameCube-named accessors.
 *
 * HD (measured in kamome, kb, mo2, am, bb, nz): j3dSys's current model at 0x104B462C and
 * J3DSys::mCurrentMtx at 0x104B4868; a model's joint matrices live in a buffer at +0x2C (+0x4
 * flags, +0x10 matrices; getAnmMtx marks them dirty), model data +0xAC, user area +0xB8, base
 * scale +0xBC, base matrix +0xC8. Model data: resource header at +0, joint nodes (0x1C each)
 * +8 with their count at +4, materials (0x39C each) +0x10 with their count at +0xC.
 * [v] = used by a verified function. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "wwhd.h"

struct J3DModel;
struct J3DJoint;
typedef s32 (*J3DJointCallBack)(J3DNode*, int);  /* GameCube: int (*)(J3DNode*, int) */
enum { J3DNodeCBCalcTiming_In = 0, J3DNodeCBCalcTiming_Out = 1 };

/* J3DNode: HD reaches the joint data through an out-of-line accessor (027F7878 returns *this) */
struct J3DNode {
    /* 0x00 */ be<u32> mpData;
};

struct J3DJoint {
    /* 0x00 */ be<u32> mpData;
    /* 0x04 */ u8 _04[4];
    /* 0x08 */ be<u32> mCallBack; /* [v nz] */
    /* GameCube inline; HD: through 027F7878 [v nz] */
    u16 getJntNo() { return gabi::load<u16>(gabi::call<u32>(0x027F7878, this) + 4); }
    void setCallBack(J3DJointCallBack cb) { mCallBack = gabi::guest_fn((const void*)cb); } /* [v nz] */
    /* 027F583C: HD entryIn takes the model (r3) and the joint (r4) [v nz] */
    void entryIn(J3DModel* model) { gabi::call(0x027F583C, model, this); }
};

struct J3DShape {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u8> mVisible; /* HD: a flag byte (GameCube: mFlags bit) [v nz] */
    void hide() { mVisible = 0; }
    void show() { mVisible = 1; }
};

struct J3DMaterial {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ gptr<J3DShape> mShape;
    J3DShape* getShape() { return mShape; }
};

/* JUTNameTab::getIndex (HD out of line) */
struct JUTNameTab {
    s32 getIndex(const char* name) { return gabi::call<s32>(0x027DF9B0, this, name); }
};

struct J3DModelData {
    /* 0x00 */ be<u32> mpResHeader;
    /* 0x04 */ be<u32> mJointNum;
    /* 0x08 */ be<u32> mJointNodes;
    /* 0x0C */ be<u32> mMaterialNum;
    /* 0x10 */ be<u32> mMaterials;
    /* HD: 027F3F94 returns the joint tree (+8: joint count) [v nz] */
    u16 getJointNum() { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, this) + 8); }
    /* HD inline, index checked against the count [v nz] */
    J3DJoint* getJointNodePointer(u16 i) {
        u32 n = mJointNum;
        u32 p = mJointNodes;
        return gabi::at<J3DJoint>(i < n ? p + (u32)i * 0x1C : p);
    }
    /* HD: 027F68FC returns the name-table block; the table is at +0x10 + offset [v nz] */
    JUTNameTab* getJointName() {
        u32 b = gabi::call<u32>(0x027F68FC, this);
        u32 off = gabi::load<u32>(b + 0x10);
        return gabi::at<JUTNameTab>(off ? b + 0x10 + off : 0);
    }
    /* HD: materials are looked up by name (inline; defined in gc_api/JSystem/J3DModelData.h) */
    J3DMaterial* getMaterialNodePointer(const char* name);
    J3DMaterial* getMaterialNodePointer(u16 i) {
        u32 n = mMaterialNum;
        u32 p = mMaterials;
        return gabi::at<J3DMaterial>(i < n ? p + (u32)i * 0x39C : p);
    }
};

struct J3DMtxBuffer_hd {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ be<u32> mpAnmMtx;
};

struct J3DModel {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBuffer_hd> mpMtxBuffer;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
    /* 0xBC */ cXyz mBaseScale;
    /* 0xC8 */ Mtx34 mBaseTransformMtx;

    J3DModelData* getModelData() { return mModelData; }
    /* 027F4D5C J3DModel::calc [g] */
    void calc() { gabi::call(0x027F4D5C, this); }
    /* the user area holds a guest address; GameCube code casts it to the owner */
    void* getUserArea() { return gabi::at<void>(mUserArea); }
    void setUserArea(uintptr_t v) { mUserArea = gabi::ea((const void*)v); }
    void setBaseScale(const cXyz& s) { mBaseScale.x = s.x; mBaseScale.y = s.y; mBaseScale.z = s.z; }
    /* HD: null-preserving (`model ? &model->mBaseTransformMtx : NULL`) [v nz] */
    Mtx34* getBaseTRMtx() { return gabi::ea(this) ? &mBaseTransformMtx : nullptr; }
    /* GHS matrix assignment: twelve loads, then twelve stores */
    void setBaseTRMtx(const Mtx34* m) { mtx_assign(&mBaseTransformMtx, m); }
    /* HD inline: marks the joint matrices dirty [v nz, kamome] */
    Mtx34* getAnmMtx(int jnt) {
        J3DMtxBuffer_hd* b = mpMtxBuffer;
        b->mFlags |= 0x10;
        return gabi::at<Mtx34>(b->mpAnmMtx + (u32)jnt * 0x30);
    }
    void setAnmMtx(int jnt, const Mtx34* m) { mtx_assign(getAnmMtx(jnt), m); }

    static void mtx_assign(Mtx34* dst, const Mtx34* src) {
        f32 t[12];
        for (int i = 0; i < 12; i++) t[i] = src->m[i / 4][i % 4];
        for (int i = 0; i < 12; i++) dst->m[i / 4][i % 4] = t[i];
    }
};
WWHD_OFFSET(J3DModel, mModelData, 0xAC);
WWHD_OFFSET(J3DModel, mUserArea, 0xB8);
WWHD_OFFSET(J3DModel, mBaseTransformMtx, 0xC8);

/* J3DDrawBuffer (the j3dSys opa/xlu buffers, the draw list's buffers); layout not needed */
struct J3DPacket;
struct J3DDrawBuffer {
    u8 _opaque;
    /* 027F0E04 entryImm(J3DPacket*, u16 index) [v: dComIfGd_getOpaListP1()->entryImm in several actors] */
    int entryImm(J3DPacket* packet, u16 index) { return gabi::call<int>(0x027F0E04, this, packet, index); }
};

struct J3DSys {
    J3DModel* getModel() const { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
    static Mtx34* const mCurrentMtx;
};
inline Mtx34* const J3DSys::mCurrentMtx = gabi::at<Mtx34>(0x104B4868);
inline J3DSys j3dSys;
