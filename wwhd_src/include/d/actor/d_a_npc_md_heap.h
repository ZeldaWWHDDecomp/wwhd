// Medli heap setup. Non-owning WWHD projections; GameCube names and HD layout deltas.
#pragma once
#include "d/actor/d_a_npc_md.h"
#include "gc_api/f_op/f_op_actor.h"
#include "SSystem/SComponent/c_m3d_g_tri.h"
#include "SSystem/SComponent/c_m3d_g_lin.h"
#include "gc_api/d/d_com_inf_game.h"
namespace MdHeap {
inline JUTNameTab* nameTable(u32 address) { return gabi::at<JUTNameTab>(address); }
inline const char* nameString(u32 address) { return gabi::at<const char>(address); }
enum Type { TYPE_SEA = 3, TYPE_EDAICHI = 4, TYPE_M_DAI = 5, TYPE_M_DAIB = 6, TYPE_SHIP_RIDE = 7 };
enum Item { dItemNo_WIND_WAKER_e = 0x22, dItemNo_GOLDEN_FEATHER_e = 0x47 };
enum EventFlag { UNK_2E40 = 0x2E40, UNK_3B80 = 0x3B80, UNK_2C08 = 0x2C08, UNK_1620 = 0x1620, UNK_2E04 = 0x2E04, UNK_1820 = 0x1820, UNK_1101 = 0x1101, UNK_2920 = 0x2920, UNK_1608 = 0x1608 };
constexpr u32 JA_SE_CM_MD_HARP_CN4 = 0x5940;
constexpr u32 AT_TYPE_LIGHT = 0x800000;
constexpr int STAGE_DRC = 3;
constexpr u32 ACCH_CIRCLE_CONSTRUCTOR = 0x024EFE94;
constexpr u32 LIN_CHECK_VTABLE = 0x1001D798;
constexpr u32 MIRROR_LINE_VTABLE = 0x1001D7A8;
constexpr u32 MIRROR_GROUP_PASS_VTABLE = 0x1001D7B8;
constexpr u32 MIRROR_POLY_PASS_VTABLE = 0x1001D7C8;
constexpr u32 AABB_VTABLE = 0x1001D6D8;
constexpr u32 SHAPE_VTABLE = 0x100015A8;
constexpr u32 CAPSULE_VTABLE = 0x1004AF18;
constexpr u32 CAPSULE_GEOMETRY_VTABLE = 0x1004AF60;
constexpr u32 CAPSULE_SHAPE_VTABLE = 0x1004AF70;
struct Morf { u8 mPadding[0x90]; gptr<J3DModel> mpModel; u8 mPad94[0xC0-0x94]; be<f32> mAnmRate; };
WWHD_OFFSET(Morf, mpModel, 0x90);
WWHD_OFFSET(Morf, mAnmRate, 0xC0);
WWHD_SIZE(Morf, 0xC4);
struct AnimationVtable { u8 mPadding[0x14]; gfn getFrameMax; };
WWHD_OFFSET(AnimationVtable, getFrameMax, 0x14);
WWHD_SIZE(AnimationVtable, 0x18);
struct Animation { u8 mPadding[4]; gptr<AnimationVtable> mpVtable;
    s32 getFrameMax() { return gabi::call_ptr<s32>(mpVtable->getFrameMax.get(), this); }
};
WWHD_OFFSET(Animation, mpVtable, 4);
WWHD_SIZE(Animation, 8);
inline be<u8>* bytes(u32 address) { return gabi::at<be<u8>>(address); }
struct SafeStringVtable { u8 mPadding[0x14]; gfn cstr; };
WWHD_OFFSET(SafeStringVtable, cstr, 0x14);
WWHD_SIZE(SafeStringVtable, 0x18);
inline void safeStringCstr(SafeString* value) { gabi::call_ptr(gabi::at<SafeStringVtable>(value->__vtbl)->cstr.get(), value); }

struct LinCheck {
    gptr<be<u32>> mpPolyPass;
    gptr<be<u32>> mpGroupPass;
    u8 mPad08[8];
    be<u32> mVtable;
    u8 mPad14[0x20-0x14];
    be<u32> mLineVtable;
    u8 mPad24[0x58-0x24];
    be<u32> mPolyPassVtable;
    be<u8> mFlags[8];
    be<u32> mGroupPassVtable;
    be<u32> mGroup;
};
WWHD_OFFSET(LinCheck, mVtable, 0x10);
WWHD_OFFSET(LinCheck, mLineVtable, 0x20);
WWHD_OFFSET(LinCheck, mPolyPassVtable, 0x58);
WWHD_OFFSET(LinCheck, mFlags, 0x5C);
WWHD_OFFSET(LinCheck, mGroupPassVtable, 0x64);
WWHD_OFFSET(LinCheck, mGroup, 0x68);
WWHD_SIZE(LinCheck, 0x6C);
struct CapsuleGeometry { cXyz mStart; cXyz mEnd; be<u32> __vtbl; be<f32> mRadius; };
WWHD_OFFSET(CapsuleGeometry, __vtbl, 0x18);
WWHD_OFFSET(CapsuleGeometry, mRadius, 0x1C);
WWHD_SIZE(CapsuleGeometry, 0x20);
struct Capsule {
    u8 mPad00[0x3C]; be<u32> mHitVtable;
    u8 mPad40[0x110-0x40]; be<u32> mAabbVtable;
    be<u32> mShapeVtable; CapsuleGeometry mGeometry;
};
WWHD_OFFSET(Capsule, mHitVtable, 0x3C);
WWHD_OFFSET(Capsule, mAabbVtable, 0x110);
WWHD_OFFSET(Capsule, mShapeVtable, 0x114);
WWHD_OFFSET(Capsule, mGeometry, 0x118);
WWHD_SIZE(Capsule, 0x138);
struct Ship : fopAc_ac_c { u8 mPad3AC[8]; gptr<Morf> mpMorf; };
WWHD_OFFSET(Ship, mpMorf, 0x3B4);
WWHD_SIZE(Ship, 0x3B8);
struct Priest { cXyz mPos; be<s16> mRotate; be<s8> mRoomNo; be<u8> mFlag; };
WWHD_OFFSET(Priest, mRotate, 0xC);
WWHD_OFFSET(Priest, mRoomNo, 0xE);
WWHD_OFFSET(Priest, mFlag, 0xF);
WWHD_SIZE(Priest, 0x10);
struct SaveInfo {
    u8 mPad00[0x1CC]; Priest mPriest;
    u8 mPad1DC[0x644-0x1DC]; u8 mEvent;
    u8 mPad645[0x1148-0x645]; u8 mRestart;
};
WWHD_OFFSET(SaveInfo, mPriest, 0x1CC);
WWHD_OFFSET(SaveInfo, mEvent, 0x644);
WWHD_OFFSET(SaveInfo, mRestart, 0x1148);
WWHD_SIZE(SaveInfo, 0x1149);
struct SavePointer { gptr<SaveInfo> mpInfo; };
WWHD_SIZE(SavePointer, 4);
struct ActorPointer { gptr<daNpc_Md_c> mpActor; };
WWHD_SIZE(ActorPointer, 4);
struct ActionWords { union { gabi::ptmf<BOOL (daNpc_Md_c::*)(void*)> fn; be<u32> mWords[2]; }; };
WWHD_OFFSET(ActionWords, fn, 0);
WWHD_SIZE(ActionWords, 8);
struct VectorTable { cXyz values[4]; };
WWHD_SIZE(VectorTable, 0x30);
struct MaskTable { be<u8> values[8]; };
WWHD_SIZE(MaskTable, 8);
struct HairNames { gptr<const char> values[8]; };
WWHD_SIZE(HairNames, 0x20);
struct EventNames { gptr<const char> values[10]; };
WWHD_SIZE(EventNames, 0x28);
struct NamePointer { gptr<const char> mName; };
WWHD_SIZE(NamePointer, 4);
struct HeapSizes { be<u32> values[2]; };
WWHD_SIZE(HeapSizes, 8);
struct BtpEntry { be<u8> m00[0x20]; be<u8> m20; };
WWHD_OFFSET(BtpEntry, m20, 0x20);
WWHD_SIZE(BtpEntry, 0x21);
struct BtpTable { BtpEntry values[16]; };
WWHD_SIZE(BtpTable, 0x210);
// HD: the resource name buffers remain 0x68 above their matching SafeStrings.
struct HeapFrame {
    u8 mPad00[0x2C]; SafeString mBodyArc; SafeString mBodyName;
    u8 mPad3C[0x4C-0x3C]; SafeString mArmArc; SafeString mArmName;
    u8 mPad5C[0x9C-0x5C]; be<u8> wait_anim_name[0x20]; be<u8> arm_wait_anim_name[0x14];
};
WWHD_OFFSET(HeapFrame, mBodyArc, 0x2C);
WWHD_OFFSET(HeapFrame, mBodyName, 0x34);
WWHD_OFFSET(HeapFrame, mArmArc, 0x4C);
WWHD_OFFSET(HeapFrame, mArmName, 0x54);
WWHD_OFFSET(HeapFrame, wait_anim_name, 0x9C);
WWHD_OFFSET(HeapFrame, arm_wait_anim_name, 0xBC);
WWHD_SIZE(HeapFrame, 0xD0);
constexpr u32 SAFE_STRING_VTABLE = 0x1001D6A0;
constexpr u32 POLY_INFO_VTABLE = 0x1001D6E8;
constexpr u32 LIGHT_BTK_ASSERT = 0x1001D928;
constexpr u32 LIGHT_BTK_ASSERT_FILE = 0x1001D93C;
constexpr u32 EYE_PATTERN_ASSERT = 0x1001D94C;
constexpr u32 EYE_PATTERN_ASSERT_FILE = 0x1001D960;
constexpr u32 WING_RIGHT_LOCATION_ASSERT = 0x1001D9EC;
constexpr u32 WING_LEFT_LOCATION_ASSERT = 0x1001DA04;
constexpr u32 ARM_RIGHT_ASSERT = 0x1001DA1C;
constexpr u32 ARM_LEFT_ASSERT = 0x1001DA30;
constexpr u32 NECK_ASSERT = 0x1001DA44;
constexpr u32 WING_RIGHT_SECOND_ASSERT = 0x1001DAB8;
constexpr u32 WING_LEFT_SECOND_ASSERT = 0x1001DAD0;
constexpr u32 WING_RIGHT_THIRD_ASSERT = 0x1001DAE8;
constexpr u32 WING_LEFT_THIRD_ASSERT = 0x1001DB00;
constexpr u32 HEAP_ASSERT_FILE = 0x1001DB40;
constexpr u32 MODEL_DATA_ASSERT = 0x1001DB50;
constexpr u32 BACKBONE_FIRST_ASSERT = 0x1001DB64;
constexpr u32 BACKBONE_SECOND_ASSERT = 0x1001DB80;
constexpr u32 ARM_RIGHT_LOCATION_ASSERT = 0x1001DB9C;
constexpr u32 ARM_LEFT_LOCATION_ASSERT = 0x1001DBB4;
constexpr u32 HAND_LEFT_ASSERT = 0x1001DBCC;
constexpr u32 FOLLOW_CALLBACK_VTABLE = 0x1001E048;
constexpr u32 MTX_FOLLOW_CALLBACK_VTABLE = 0x100347F0;
}
void* mdHeap_getResource(void* control, SafeString* arc, SafeString* name);
void mdHeap_multVecArray(const Mtx34* matrix, const cXyz* src, cXyz* dst, u32 count);
void mdHeap_normZP(const cXyz* src, cXyz* result);
void mdHeap_norm(const cXyz* src, cXyz* result);
void mdHeap_outprod(const cXyz* src, cXyz* result, const cXyz* other);
void mdHeap_normalizeZP(cXyz* src, cXyz* result);
void mdHeap_getAllWind(cXyz* pos, cXyz* dir, be<f32>* power);
void mdHeap_rotAxisRad(Mtx34* matrix, const cXyz* axis, f32 radians);
void mdHeap_concat(const Mtx34* first, const Mtx34* second, Mtx34* result);
f32 mdHeap_dotProduct(const cXyz* first, const cXyz* second);
bool mdHeap_upMatrix(const cXyz* first, const cXyz* second, Mtx34* matrix);
void mdHeap_constructTriangle(cM3dGTri* triangle, const cXyz* a, const cXyz* b, const cXyz* c);
void mdHeap_constructLine(cM3dGLin* line, const cXyz* start, const cXyz* end);
bool mdHeap_crossLineTriangle(const cM3dGLin* line, const cM3dGTri* triangle, cXyz* result, bool front, bool back);
void mdHeap_upTriangle(cM3dGTri* triangle, f32 distance);
f32 mdHeap_signedDistance(const cM3dGTri* triangle, const cXyz* pos);
void mdHeap_xyzRotM(Mtx34* matrix, s16 x, s16 y, s16 z);
void mdHeap_calcMorf2(mDoExt_McaMorf2* morf);
s32 mdHeap_initBtp(void* animation, J3DModelData* data, void* resource, s32 play, s32 mode, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
s32 mdHeap_initBtk(void* animation, J3DModelData* data, void* resource, s32 play, s32 mode, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
mDoExt_McaMorf2* mdHeap_newMorf2(u32 object, J3DModelData* data, u32 callback1, u32 callback2, void* animation, u32 animation2, s32 mode, f32 rate, s32 start, s32 end, s32 sound, u32 info, u32 flags, u32 materials);
mDoExt_McaMorf* mdHeap_newMorf(u32 object, J3DModelData* data, u32 callback1, u32 callback2, void* animation, s32 mode, f32 rate, s32 start, s32 end, s32 sound, u32 info, u32 flags, u32 materials);
void mdHeap_setMorf(mDoExt_McaMorf2* morf, f32 value);
void mdHeap_setCapsule(dCcD_Cps* capsule, const dCcD_SrcCps* source);
void mdHeap_initMirror(void* mirror, void* texture);
void mdHeap_copyString(char* dst, const char* src);
void mdHeap_constructNpc(daNpc_Md_c* actor);
void mdHeap_constructBtp(void* animation);
void mdHeap_constructArray(void* array, u32 count, u32 stride, u32 constructor);
void mdHeap_constructLinCheck(void* check);
void mdHeap_constructObjectInfo(void* object);
void mdHeap_constructCapsule(void* capsule);
void mdHeap_constructJointControl(dNpc_JntCtrl_c* control);
void mdHeap_constructMirror(void* mirror);
void mdHeap_constructRipple(dPa_rippleEcallBack_md* callback);
BOOL mdHeap_checkLetterPassed();
BOOL mdHeap_entrySolidHeap(fopAc_ac_c* actor, u32 callback, u32 size);
void mdHeap_setRestartOption(void* restart, s32 flag, const cXyz* pos, s16 rotation, s8 room);
void mdHeap_setPriest(void* priest, s32 flag, const cXyz* pos, s16 rotation, s8 room);
void mdHeap_checkRestart(daNpc_Md_c* actor, s32 flag);