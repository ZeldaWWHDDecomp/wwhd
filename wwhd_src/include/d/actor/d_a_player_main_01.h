/* Link model and joint-callback views for the main-01 range.
 * HD: animation heaps retain the GameCube 0x10 layout; the packet list is new.
 */
#pragma once
#include "d/actor/d_a_player_main_readable.h"
#include "JSystem/JParticle/JPA_emitter_calc.h"
#include "m_Do/m_Do_ext_animation.h"
#include "m_Do/m_Do_lib.h"
#include "d/d_drawlist.h"

struct PlayerPacketList {
    /* 0x00 */ be<s32> mCount;
    /* 0x04 */ be<s32> mCapacity;
    /* 0x08 */ gptr<be<u32>> mpPackets;
    /* 0x0C */ u8 _padding[0x80];
    /* 0x8C */ be<u8> mEnabled;
    /* 0x8D */ u8 _tail[3];
};
WWHD_OFFSET(PlayerPacketList, mCount, 0);
WWHD_OFFSET(PlayerPacketList, mCapacity, 4);
WWHD_OFFSET(PlayerPacketList, mpPackets, 8);
WWHD_OFFSET(PlayerPacketList, mEnabled, 0x8C);
WWHD_SIZE(PlayerPacketList, 0x90);

// HD: aura entries retain the model/frame pair and eight-byte stride.
struct daPy_aura_c {
    gptr<J3DModel> mpModel;
    be<f32> mFrame;
};
WWHD_OFFSET(daPy_aura_c, mFrame, 4);
WWHD_SIZE(daPy_aura_c, 8);

struct PlayerModelState {
    // HD: resource animation pointers became inline animation objects.
    /* 0x0000 */ mDoExt_brkAnm mTswgripmsabBrk;
    /* 0x0078 */ mDoExt_btkAnm mTswgripmsBtk;
    /* 0x00EC */ gptr<J3DModel> mpPodmsModel;
    /* 0x00F0 */ gptr<J3DModel> mpEquippedShieldModel;
    /* 0x00F4 */ gptr<J3DModel> mpShaModel;
    /* 0x00F8 */ gptr<J3DModel> mpShmsModel;
    /* 0x00FC */ mDoExt_bckAnm mAtngshaBck;
    /* 0x0188 */ mDoExt_btkAnm mTshmsBtk;
    /* 0x01FC */ u8 mMirrorPacket[0x43B4 - 0xF8C];
    /* 0x3624 */ gptr<J3DModel> mpYmsls00Model;
    /* 0x3628 */ mDoExt_btkAnm mYmsls00Btk;
    /* 0x369C */ gptr<J3DModel> mpHbootsModels[2];
    /* 0x36A4 */ gptr<J3DModel> mpPringModel;
    /* 0x36A8 */ gptr<void> mpItemHeaps[2];
    /* 0x36B0 */ gptr<J3DModel> mpEquipItemModel;
    /* 0x36B4 */ mDoExt_bckAnm mSwordAnim;
    /* 0x3740 */ gptr<mDoExt_McaMorf> mpParachuteFanMorf;
    /* 0x3744 */ u8 _itemEffects[0x4808 - 0x44D4];
    /* 0x3A78 */ be<u32> mpItemAnimeHeap;
    /* 0x3A7C */ gptr<void> m_item_bck_buffer;
    /* 0x3A80 */ mDoExt_btkAnm mpEquipItemBtk;
    /* 0x3AF4 */ mDoExt_btkAnm mpSwordBtk;
    /* 0x3B68 */ mDoExt_brkAnm mpEquipItemBrk;
    /* 0x3BE0 */ gptr<J3DModel> mpBottleContentsModel;
    /* 0x3BE4 */ gptr<J3DModel> mpBottleCapModel;
    /* 0x3BE8 */ gptr<J3DModel> mpSwordModel1;
    /* 0x3BEC */ gptr<J3DModel> mpSwordTipStabModel;
    /* 0x3BF0 */ mDoExt_bpkAnm mpCutfBpk;
    /* 0x3C64 */ mDoExt_btkAnm mpCutfBtk;
    /* 0x3CD8 */ mDoExt_brkAnm mpCutfBrk;
    u8 _swordEffects[0x4B58 - 0x4AE0];
    /* 0x3DC8 */ PlayerPacketList mPacketList;
    /* 0x3E58 */ u8 _effects[0x5378 - 0x4BE8];
    /* 0x45E8 */ gptr<J3DModel> mpSuimenMunyaModel;
    /* 0x45EC */ mDoExt_btkAnm mSuimenMunyaBtk;
    /* 0x4660 */ gptr<J3DModel> mpYuchw00Model;
    /* 0x4664 */ mDoExt_bckAnm mYuchw00Bck;
    /* 0x46F0 */ mDoExt_btkAnm mYuchw00Btk;
    /* 0x4764 */ mDoExt_brkAnm mYuchw00Brk;
    /* 0x47DC */ gptr<J3DModel> mpYbafo00Model;
    /* 0x47E0 */ mDoExt_btkAnm mYbafo00Btk;
    /* 0x4854 */ daPy_aura_c mMagicArmorAuraEntries[6];
    /* 0x4884 */ mDoExt_brkAnm mYaura00rBrk;
    /* 0x48FC */ mDoExt_btkAnm mYaura00Btk;
    /* 0x4970 */ gptr<J3DModel> mpYmgcs00Model;
    /* 0x4974 */ mDoExt_brkAnm mYmgcs00Brk;
    /* 0x49EC */ mDoExt_btkAnm mYmgcs00Btk;
};
WWHD_OFFSET(PlayerModelState, mPacketList, 0x3DC8);
WWHD_OFFSET(PlayerModelState, mYuchw00Brk, 0x4764);
WWHD_OFFSET(PlayerModelState, mpSuimenMunyaModel, 0x45E8);
WWHD_OFFSET(PlayerModelState, mSuimenMunyaBtk, 0x45EC);
WWHD_OFFSET(PlayerModelState, mpYuchw00Model, 0x4660);
WWHD_OFFSET(PlayerModelState, mYuchw00Bck, 0x4664);
WWHD_OFFSET(PlayerModelState, mYuchw00Btk, 0x46F0);
WWHD_OFFSET(PlayerModelState, mpYbafo00Model, 0x47DC);
WWHD_OFFSET(PlayerModelState, mYbafo00Btk, 0x47E0);
WWHD_OFFSET(PlayerModelState, mMagicArmorAuraEntries, 0x4854);
WWHD_OFFSET(PlayerModelState, mYaura00rBrk, 0x4884);
WWHD_OFFSET(PlayerModelState, mYaura00Btk, 0x48FC);
WWHD_OFFSET(PlayerModelState, mpYmgcs00Model, 0x4970);
WWHD_OFFSET(PlayerModelState, mYmgcs00Brk, 0x4974);
WWHD_OFFSET(PlayerModelState, mYmgcs00Btk, 0x49EC);
WWHD_SIZE(PlayerModelState, 0x4A60);

struct PlayerDrawPacket {
    /* 0x00 */ u8 _packet[0x98];
    /* 0x98 */ be<u32> mpModel;
    /* 0x9C */ u8 _drawState[0x10];
    /* 0xAC */ be<u8> mEnabled;
    /* 0xAD */ be<u8> field_0xad;
    /* 0xAE */ be<u8> field_0xae;
    /* 0xAF */ be<u8> field_0xaf;
};
WWHD_OFFSET(PlayerDrawPacket, mpModel, 0x98);
WWHD_OFFSET(PlayerDrawPacket, mEnabled, 0xAC);
WWHD_SIZE(PlayerDrawPacket, 0xB0);

// Resource indices retain the GameCube LkAnm archive numbering.
enum {
    dRes_INDEX_LKANM_BCK_ARROWRELORD_e = 0xC,
    dRes_INDEX_LKANM_BCK_ARROWSHOOT_e = 0xE,
    dRes_INDEX_LKANM_BCK_BOOMTHROW_e = 0x34,
    dRes_INDEX_LKANM_BCK_BOOMWAIT_e = 0x35,
    dRes_INDEX_LKANM_BCK_BOWWAIT_e = 0x36,
    dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e = 0xA7,
    dRes_INDEX_LKANM_BCK_ROPETHROW_e = 0xE2,
    dRes_INDEX_LKANM_BCK_SHIP_JUMP1_e = 0xF3,
    dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e = 0xF4,
    dRes_INDEX_LKANM_BCK_TAKE_e = 0x103,
    dRes_INDEX_LKANM_BCK_TAKEBOTH_e = 0x104,
    dRes_INDEX_LKANM_BCK_VOYAGE1_e = 0x11B,
};

// HD: the material animation vtable follows its inline base state. The two
// current eye-offset components are stored in the reverse order from GameCube.
struct PlayerOldEyeOffset { be<f32> x, y; };
struct PlayerEyeOffset { be<f32> y, x; };
WWHD_OFFSET(PlayerOldEyeOffset, x, 0);
WWHD_OFFSET(PlayerOldEyeOffset, y, 4);
WWHD_SIZE(PlayerOldEyeOffset, 8);
WWHD_OFFSET(PlayerEyeOffset, y, 0);
WWHD_OFFSET(PlayerEyeOffset, x, 4);
WWHD_SIZE(PlayerEyeOffset, 8);
struct daPy_matAnm_c {
    enum : u32 { __vt = 0x10037AE0 };
    /* 0x00 */ u8 _materialAnm[0x68];
    /* 0x68 */ be<u32> __vtbl;
    /* 0x6C */ PlayerOldEyeOffset mOldOffset;
    /* 0x74 */ PlayerEyeOffset mNowOffset;
};
WWHD_OFFSET(daPy_matAnm_c, __vtbl, 0x68);
WWHD_OFFSET(daPy_matAnm_c, mOldOffset, 0x6C);
WWHD_OFFSET(daPy_matAnm_c, mNowOffset, 0x74);
WWHD_SIZE(daPy_matAnm_c, 0x7C);

// HD: keep the caller's full integer registers for the byte mode and null
// invisible-model argument when crossing this independently verified range.
struct dMat_control_c {
    static void iceUpdateDL(J3DModel*, s32, s32);
    static void iceEntryDL(J3DModel*, s32, s32);
};

// GameCube animation-heap member order and size are retained in HD.
struct daPy_anmHeap_c {
    /* 0x00 */ be<u16> mIdx;
    /* 0x02 */ be<u16> field_0x2;
    /* 0x04 */ be<u16> field_0x4;
    /* 0x06 */ be<u16> field_0x6;
    /* 0x08 */ gptr<void> m_buffer;
    /* 0x0C */ gptr<void> mpAnimeHeap;
};
WWHD_OFFSET(daPy_anmHeap_c, mIdx, 0);
WWHD_OFFSET(daPy_anmHeap_c, field_0x2, 2);
WWHD_OFFSET(daPy_anmHeap_c, field_0x4, 4);
WWHD_OFFSET(daPy_anmHeap_c, field_0x6, 6);
WWHD_OFFSET(daPy_anmHeap_c, m_buffer, 8);
WWHD_OFFSET(daPy_anmHeap_c, mpAnimeHeap, 0xC);
WWHD_SIZE(daPy_anmHeap_c, 0x10);

// HD: native range bindings preserve the original full-width guest registers.
u32 mDoExt_setCurrentHeap(u32 heap);
u32 operator_new_align(u32 size, s32 align);
u32 operator_new_array_align(u32 size, s32 align);
u32 PlayerModelData_getResource(u32 data);
void PlayerTexScroll_setAnm(u32 obj, u32 anm);
void PlayerTexScroll_setModelData(u32 obj, u32 resource);

// HD: shader colour values are stored as four normalized floating components.
struct PlayerColorF4 { be<f32> r, g, b, a; };
WWHD_OFFSET(PlayerColorF4, r, 0);
WWHD_OFFSET(PlayerColorF4, g, 4);
WWHD_OFFSET(PlayerColorF4, b, 8);
WWHD_OFFSET(PlayerColorF4, a, 0xC);
WWHD_SIZE(PlayerColorF4, 0x10);

// HD: node-to-joint access returns the joint resource header used by callbacks.
struct PlayerJointData {
    /* 0x00 */ be<u32> mFlags;
    /* 0x04 */ be<u16> mJntNo;
};
WWHD_OFFSET(PlayerJointData, mJntNo, 4);
WWHD_SIZE(PlayerJointData, 6);

WWHD_OFFSET(PlayerModelState, mTswgripmsabBrk, 0x0);
WWHD_OFFSET(PlayerModelState, mTswgripmsBtk, 0x78);
WWHD_OFFSET(PlayerModelState, mpPodmsModel, 0xEC);
WWHD_OFFSET(PlayerModelState, mpEquippedShieldModel, 0xF0);
WWHD_OFFSET(PlayerModelState, mpShaModel, 0xF4);
WWHD_OFFSET(PlayerModelState, mpShmsModel, 0xF8);
WWHD_OFFSET(PlayerModelState, mAtngshaBck, 0xFC);
WWHD_OFFSET(PlayerModelState, mTshmsBtk, 0x188);
WWHD_OFFSET(PlayerModelState, mMirrorPacket, 0x1FC);
WWHD_OFFSET(PlayerModelState, mpYmsls00Model, 0x3624);
WWHD_OFFSET(PlayerModelState, mYmsls00Btk, 0x3628);
WWHD_OFFSET(PlayerModelState, mpHbootsModels, 0x369C);
WWHD_OFFSET(PlayerModelState, mpPringModel, 0x36A4);
WWHD_OFFSET(PlayerModelState, mpItemHeaps, 0x36A8);
WWHD_OFFSET(PlayerModelState, mpEquipItemModel, 0x36B0);
WWHD_OFFSET(PlayerModelState, mSwordAnim, 0x36B4);
WWHD_OFFSET(PlayerModelState, mpParachuteFanMorf, 0x3740);

// HD: animation setup retains full-width mode and frame registers.
BOOL Player_initBckAnimation(u32 anm, u32 data, u32 animation, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify);
BOOL Player_initBrkAnimation(u32 anm, u32 data, u32 animation, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify, s32 entry);
BOOL Player_initBtkAnimation(u32 anm, u32 data, u32 animation, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify, s32 entry);

// HD: each joint has independent saved transform and quaternion records.
struct J3DTransformInfo {
    /* 0x00 */ cXyz mScale;
    /* 0x0C */ csXyz mRotation;
    /* 0x12 */ u8 _padding[2];
    /* 0x14 */ cXyz mTranslate;
};
WWHD_OFFSET(J3DTransformInfo, mScale, 0);
WWHD_OFFSET(J3DTransformInfo, mRotation, 0xC);
WWHD_OFFSET(J3DTransformInfo, mTranslate, 0x14);
WWHD_SIZE(J3DTransformInfo, 0x20);
// Copies use interleaved word loads/stores, including signalling NaN payloads.
struct PlayerSavedQuaternion { be<u32> mWords[4]; };
WWHD_SIZE(PlayerSavedQuaternion, 0x10);
struct PlayerJointState {
    /* 0x000 */ PlayerSavedQuaternion mSavedQuaternion[42];
    /* 0x2A0 */ J3DTransformInfo mSavedTransform[42];
};
WWHD_OFFSET(PlayerJointState, mSavedQuaternion, 0);
WWHD_OFFSET(PlayerJointState, mSavedTransform, 0x2A0);
WWHD_SIZE(PlayerJointState, 0x7E0);

// GameCube foot records retain their size and member offsets in HD.
struct daPy_footData_c {
    /* 0x000 */ be<u8> field_0x000;
    /* 0x001 */ be<u8> field_0x001;
    /* 0x002 */ be<s16> field_0x002;
    /* 0x004 */ be<s16> field_0x004;
    /* 0x006 */ be<s16> field_0x006;
    /* 0x008 */ be<s16> field_0x008;
    /* 0x00A */ be<s16> field_0x00A;
    /* 0x00C */ cXyz field_0x00C;
    /* 0x018 */ cXyz field_0x018;
    /* 0x024 */ cXyz field_0x024;
    /* 0x030 */ be<f32> field_0x030;
    /* 0x034 */ u8 field_0x034[0x54];
    /* 0x088 */ Mtx34 field_0x088[3];
};
WWHD_OFFSET(daPy_footData_c, field_0x030, 0x30);
WWHD_OFFSET(daPy_footData_c, field_0x034, 0x34);
WWHD_OFFSET(daPy_footData_c, field_0x088, 0x88);
WWHD_SIZE(daPy_footData_c, 0x118);

// HD: keep full native argument registers at the quaternion call boundary.
// GameCube names: JMAEulerToQuat and mDoMtx_QuatConcat.
void PlayerEulerToQuat(s32 x, s32 y, s32 z, u32 out);
void PlayerQuatConcat(u32 first, u32 second, u32 out);

// HD: preserve double FPR values and full integer registers at native calls.
// GameCube names: PSVECSquareMag, sqrtf, cM_atan2s, mDoMtx_ZrotM, mDoMtx_XrotM.
f64 Player_squareMagnitudeNative(const cXyz* vector);
f64 Player_sqrtNative(f64 value);
u32 Player_atan2Native(f64 y, f64 x);
void Player_ZrotNative(u32 matrix, u32 angle);
void Player_XrotNative(u32 matrix, u32 angle);

// HD: animation setup has no GameCube counterpart. Keep unknown words opaque,
// including the untouched tail, so the native initializer sees the same bytes.
struct PlayerTextureAnimationSetup {
    /* 0x00 */ be<s32> mParameters[4];
    /* 0x10 */ be<u8> mEnabled;
    /* 0x11 */ be<u8> field_0x11;
    /* 0x12 */ u8 _padding[2];
    /* 0x14 */ be<u32> field_0x14;
    /* 0x18 */ be<u32> field_0x18;
    /* 0x1C */ u8 _nativeState[0x20];
    /* 0x3C */ be<u32> mModelResourceValue;
};
WWHD_OFFSET(PlayerTextureAnimationSetup, mEnabled, 0x10);
WWHD_OFFSET(PlayerTextureAnimationSetup, field_0x14, 0x14);
WWHD_OFFSET(PlayerTextureAnimationSetup, field_0x18, 0x18);
WWHD_OFFSET(PlayerTextureAnimationSetup, mModelResourceValue, 0x3C);
WWHD_SIZE(PlayerTextureAnimationSetup, 0x40);

u32 Player_getArchiveResource(dRes_control_c* control, SafeString* archive, u32 index);
// HD: texture-pattern and texture-scroll animation objects use native setup records.
void PlayerTexPattern_init(u32 obj, PlayerTextureAnimationSetup* setup, u32 buffer, u32 size);
void PlayerTexScroll_init(u32 obj, PlayerTextureAnimationSetup* setup, u32 buffer, u32 size);
void PlayerTexPattern_setAnm(u32 obj, u32 animation);
void PlayerTexPattern_setModelData(u32 obj, u32 modelData);


// HD: resource formats differ in flag width and the setup count selected.
struct PlayerTexPatternData {
    u8 _header[0xC];
    /* 0x0C */ be<u16> mFlags;
    u8 _paddingE[2];
    u8 _tables[4];
    /* 0x14 */ be<u16> mMaterialNum;
    /* 0x16 */ be<u16> field_0x16;
    /* 0x18 */ be<s32> field_0x18;
    /* 0x1C */ be<s32> field_0x1C;
};
WWHD_OFFSET(PlayerTexPatternData, mFlags, 0xC);
WWHD_OFFSET(PlayerTexPatternData, mMaterialNum, 0x14);
WWHD_OFFSET(PlayerTexPatternData, field_0x16, 0x16);
WWHD_OFFSET(PlayerTexPatternData, field_0x18, 0x18);
WWHD_OFFSET(PlayerTexPatternData, field_0x1C, 0x1C);
WWHD_SIZE(PlayerTexPatternData, 0x20);
struct PlayerTexScrollData {
    u8 _header[0xC];
    /* 0x0C */ be<u32> mFlags;
    u8 _tables[4];
    /* 0x14 */ be<u16> mMaterialNum;
    /* 0x16 */ be<u16> field_0x16;
    /* 0x18 */ be<s32> field_0x18;
    /* 0x1C */ be<s32> field_0x1C;
};
WWHD_OFFSET(PlayerTexScrollData, mFlags, 0xC);
WWHD_OFFSET(PlayerTexScrollData, mMaterialNum, 0x14);
WWHD_OFFSET(PlayerTexScrollData, field_0x16, 0x16);
WWHD_OFFSET(PlayerTexScrollData, field_0x18, 0x18);
WWHD_OFFSET(PlayerTexScrollData, field_0x1C, 0x1C);
WWHD_SIZE(PlayerTexScrollData, 0x20);
struct PlayerTextureResource {
    u8 _header[0xC];
    /* 0x0C */ be<u32> mpData;
};
WWHD_OFFSET(PlayerTextureResource, mpData, 0xC);
WWHD_SIZE(PlayerTextureResource, 0x10);

// HD: material records are contiguous (stride 0x39C); animation pointer at +0x24.
struct PlayerMaterialAnimationView {
    /* 0x00 */ be<u32> mpData;
    /* 0x04 */ gptr<PlayerMaterialAnimationView> mNext;
    /* 0x08 */ gptr<J3DShape> mShape;
    /* 0x0C */ u8 _material[0xC];
    /* 0x18 */ be<u32> mpTevBlock;
    /* 0x1C */ u8 _animation[8];
    /* 0x24 */ gptr<daPy_matAnm_c> mMaterialAnm;
    /* 0x28 */ u8 _parameters[0x78];
    /* 0xA0 */ be<u32> mParameterFlags;
};
WWHD_OFFSET(PlayerMaterialAnimationView, mNext, 4);
WWHD_OFFSET(PlayerMaterialAnimationView, mShape, 8);
WWHD_OFFSET(PlayerMaterialAnimationView, mMaterialAnm, 0x24);

// GameCube LkAnm resource indices (tww assets/GZLE01/res/Object/LkAnm.h).
enum {
    dRes_INDEX_LKANM_BRK_TBINHO_e = 0x152,
    dRes_INDEX_LKANM_BRK_TBINLIQUID_e = 0x153,
    dRes_INDEX_LKANM_BRK_TTAKT_e = 0x154,
    dRes_INDEX_LKANM_BTK_TBOTTLE_e = 0x16B,
    dRes_INDEX_LKANM_BTK_TMABA_e = 0x1A7,
};

WWHD_OFFSET(PlayerModelState, mpBottleContentsModel, 0x3BE0);
WWHD_OFFSET(PlayerModelState, mpBottleCapModel, 0x3BE4);
WWHD_OFFSET(PlayerModelState, mpSwordModel1, 0x3BE8);
WWHD_OFFSET(PlayerModelState, mpSwordTipStabModel, 0x3BEC);

u32 Player_getJointTree(u32 modelData);
s32 Player_jointSubtreeEnd(u32 tree, s32 joint);

void Player_convertColorToRGB(cXyz* out, PlayerColorF4* color, f32 scale);
u32 Player_materialParameter(u32 block, u32 index);

void Player_constructGroundCheck(cBgS_GndChk* check);
void Player_destroyGroundCheck(cBgS_Chk* check, s32 flag);
void Player_offsetPosition(cXyz* out, cXyz* position, s32 angle, cXyz* offset);
// HD: retain the full FPR result from cBgS::GroundCross.
f64 Player_groundCrossNative(dBgS* background, dBgS_GndChk* check);
BOOL Player_checkPolySafe(dBgS* background, cBgS_PolyInfo* poly);

WWHD_OFFSET(PlayerMaterialAnimationView, mpTevBlock, 0x18);
WWHD_OFFSET(PlayerMaterialAnimationView, mParameterFlags, 0xA0);
// HD: shadow model occupies the base player's previously unnamed tail word.
inline J3DModel* Player_getShadowModel(daPy_lk_c* player) {
    return reinterpret_cast<gptr<J3DModel>*>(player->_438)->get();
}

// HD: explicit native vtable layout for J3DTevBlock::setTevColor.
struct PlayerTevBlockVtable {
    u8 _slots[0x3C];
    be<u32> mSetTevColor;
};
struct PlayerTevBlockNative {
    u8 _header[4];
    gptr<PlayerTevBlockVtable> mpVtable;
    void setTevColor(s32 index, void* color) {
        gabi::call_ptr(mpVtable->mSetTevColor, this, index, color);
    }
};
WWHD_OFFSET(PlayerTevBlockVtable, mSetTevColor, 0x3C);
WWHD_OFFSET(PlayerTevBlockNative, mpVtable, 4);

struct PlayerSightImage {
    u8 _format[2];
    be<u16> width, height;
};
// HD: the sight packet retains its GameCube prefix and adds native tail state.
struct daPy_sightPacket_c {
    be<u32> __vtbl;
    be<u8> mDrawFlag, mLockFlag, mFrame, mLockAlpha;
    cXyz mPos;
    Mtx34 mMtx;
    gptr<PlayerSightImage> mpImg;
    u8 _nativeState[0xC08 - 0x48];
};
WWHD_OFFSET(PlayerSightImage, width, 2);
WWHD_OFFSET(PlayerSightImage, height, 4);
WWHD_OFFSET(daPy_sightPacket_c, mLockFlag, 5);
WWHD_OFFSET(daPy_sightPacket_c, mFrame, 6);
WWHD_OFFSET(daPy_sightPacket_c, mLockAlpha, 7);
WWHD_OFFSET(daPy_sightPacket_c, mPos, 8);
WWHD_OFFSET(daPy_sightPacket_c, mMtx, 0x14);
WWHD_OFFSET(daPy_sightPacket_c, mpImg, 0x44);
WWHD_SIZE(daPy_sightPacket_c, 0xC08);

void Player_getViewport(view_port_class* out);
void Player_project(cXyz* position, cXyz* out, view_port_class* viewport);
void Player_listSet(u32 list, u32 first, u32 last, u32 packet);
// HD: 2D translucent list anchors inside dDlst_list_c.
struct PlayerDrawList2D {
    u8 _lists[0x264];
    be<u32> mpFirst, mpLast;
};
WWHD_OFFSET(PlayerDrawList2D, mpFirst, 0x264);
WWHD_OFFSET(PlayerDrawList2D, mpLast, 0x268);

// HD: runtime model/material state used for color and alpha update masks.
struct PlayerRuntimeMaterial {
    u8 _state[0x30];
    be<u32> mFlags;
    u8 _tail[4];
};
struct PlayerModelDrawState {
    u8 _model[0x74];
    be<u32> mDrawFlags;
    u8 _state[0xC0];
    be<u32> mMaterialNum;
    gptr<PlayerRuntimeMaterial> mpMaterials;
};
WWHD_OFFSET(PlayerRuntimeMaterial, mFlags, 0x30);
WWHD_SIZE(PlayerRuntimeMaterial, 0x38);
WWHD_OFFSET(PlayerModelDrawState, mDrawFlags, 0x74);
WWHD_OFFSET(PlayerModelDrawState, mMaterialNum, 0x138);
WWHD_OFFSET(PlayerModelDrawState, mpMaterials, 0x13C);

// HD: joint nodes link the material mesh list independently of resource joint data.
struct PlayerJointNodeView {
    be<u32> mpData;
    u8 _joint[0xC];
    gptr<PlayerMaterialAnimationView> mMesh;
};
struct PlayerMaterialResource {
    u8 _header[0xC];
    be<u16> mIndex;
};
WWHD_OFFSET(PlayerJointNodeView, mMesh, 0x10);
WWHD_OFFSET(PlayerMaterialResource, mIndex, 0xC);

u32 Player_newArray(u32 size);

void Player_getSeaColor(GXColor* primary, GXColor* secondary);

// HD: the surface model's TEV implementation uses slot 0x24; the shadow
// implementation above uses slot 0x3C.
struct PlayerSurfaceTevVtable {
    u8 _slots[0x24];
    be<u32> mSetTevColor;
};
struct PlayerSurfaceTevBlock {
    u8 _header[4];
    gptr<PlayerSurfaceTevVtable> mpVtable;
};
WWHD_OFFSET(PlayerSurfaceTevVtable, mSetTevColor, 0x24);
WWHD_OFFSET(PlayerSurfaceTevBlock, mpVtable, 4);

void Player_initializeDrawPacket(PlayerDrawPacket* packet, u32 mode);

// HD: actor virtual dispatch retains these measured slots.
struct PlayerActorVtable {
    u8 _beforeGuard[0x3C];
    be<u32> mCheckPlayerGuard;
    u8 _beforeNoDraw[0x8C];
    be<u32> mCheckPlayerNoDraw;
};
struct PlayerActorDispatch {
    u8 _actor[0xB4];
    gptr<PlayerActorVtable> mpVtable;
};
WWHD_OFFSET(PlayerActorVtable, mCheckPlayerGuard, 0x3C);
WWHD_OFFSET(PlayerActorVtable, mCheckPlayerNoDraw, 0xCC);
WWHD_OFFSET(PlayerActorDispatch, mpVtable, 0xB4);

void Player_updateMirrorPacket(u32 packet, Mtx34* matrix, u32 alpha, f32 size);
void Player_drawBufferEntryImmediate(u32 buffer, u32 packet, u32 index);

struct PlayerSafeStringVtable {
    u8 _slots[0x14];
    be<u32> mAssureTermination;
};
WWHD_OFFSET(PlayerSafeStringVtable, mAssureTermination, 0x14);

// HD: texture controllers own an out-of-line frame calculation record.
struct PlayerTextureFrameControl {
    be<f32> mFrame, mStartFrame, mEndFrame;
    u8 _padding[4];
    be<u32> mCalculateFrame;
    be<u32> mContext;
};
struct PlayerTextureController {
    gptr<PlayerTextureFrameControl> mpFrameControl;
    u8 _state[0x54];
};
struct PlayerModelTextureControllers {
    u8 _modelData[0x38];
    gptr<PlayerTextureController> mpTexturePatterns[2];
    u8 _padding[4];
    gptr<PlayerTextureController> mpTextureScroll;
};
WWHD_OFFSET(PlayerTextureFrameControl, mCalculateFrame, 0x10);
WWHD_OFFSET(PlayerTextureFrameControl, mContext, 0x14);
WWHD_SIZE(PlayerTextureFrameControl, 0x18);
WWHD_SIZE(PlayerTextureController, 0x58);
WWHD_OFFSET(PlayerModelTextureControllers, mpTexturePatterns, 0x38);
WWHD_OFFSET(PlayerModelTextureControllers, mpTextureScroll, 0x44);
void Player_prepareTexturePattern(PlayerTextureController* controller);
void Player_prepareTextureScroll(PlayerTextureController* controller);

// HD: process and joint indices used by this range retain their verified values.
enum {
    CL_JNT_BODY_CHN_e = 2,
    CL_JNT_CL_BACK_e = 0x29,
    CL_JNT_CL_EYE_e = 0x13,
    CL_JNT_CL_HANA_e = 0x14,
    CL_JNT_CL_LHANDA_e = 8,
    CL_JNT_CL_MAYU_e = 0x15,
    CL_JNT_CL_PODA_e = 0xD,
    CL_JNT_CL_RHANDA_e = 0xC,
    CL_JNT_HATA_JNT_e = 0x1A,
    CL_JNT_HATB_JNT_e = 0x1B,
    CL_JNT_HATC_JNT_e = 0x1C,
    CL_JNT_HEAD_JNT_e = 0xF,
    CL_JNT_LCLOTCH_JNT_e = 0x1F,
    CL_JNT_LFOOT_JNT_e = 0x22,
    CL_JNT_LINK_ROOT_e = 0,
    CL_JNT_LLEGA_JNT_e = 0x20,
    CL_JNT_LLEGB_JNT_e = 0x21,
    CL_JNT_LMOMI_JNT_e = 0x10,
    CL_JNT_RCLOTCH_JNT_e = 0x24,
    CL_JNT_RFOOT_JNT_e = 0x27,
    CL_JNT_RLEGA_JNT_e = 0x25,
    CL_JNT_RLEGB_JNT_e = 0x26,
    CL_JNT_RMOMI_JNT_e = 0x11,
    CL_JNT_WAIST_JNT_e = 0x1E,
    HANDS_JNT_WORLD_ROOT_e = 0,
    dItemNo_HOOKSHOT_e = 0x2F,
    daPyProc_BACK_JUMP_e = 0x22,
    daPyProc_BT_VERTICAL_JUMP_CUT_e = 0x63,
    daPyProc_CUT_F_e = 0x42,
    daPyProc_DEMO_CAUGHT_e = 0xBD,
    daPyProc_DEMO_DEAD_e = 0xB2,
    daPyProc_DEMO_TOOL_e = 0xA9,
    daPyProc_ELEC_DAMAGE_e = 0x6C,
    daPyProc_FAN_GLIDE_e = 0x93,
    daPyProc_GRAB_WAIT_e = 0x73,
    daPyProc_GUARD_CRASH_e = 0x65,
    daPyProc_TACT_PLAY_e = 0x9B,
    fpcNm_KB_e = 0xDC,
};

// HD: native calls preserve full register widths across independently verified ranges.
u32 Player_demoGetActor(u32 object, u32 id);
void Player_ZrotS(u32 matrix, s32 angle);
void Player_YrotS(u32 matrix, s32 angle);
BOOL Player_isCreating(u32 processId);
void Player_blendQuaternion(u32 first, u32 second, u32 out, f32 ratio);
void Player_subtractPosition(u32 first, cXyz* out, u32 second);
u32 Player_materialNameIndex(u32 table, u32 name);
u32 Player_cameraBody();
void Player_posToCamera(cXyz* position, cXyz* out);
void Player_setDrawListPosition(u32 position);
void Player_entryZSortXlu(u32 list, u32 buffer, u32 packet, u32 position);

struct PlayerEyeMaterialNames { SafeString mNames[2]; };
WWHD_SIZE(PlayerEyeMaterialNames, 0x10);

WWHD_OFFSET(PlayerModelState, m_item_bck_buffer, 0x3A7C);
// HD: the appended timer suppresses item buttons until it counts down to zero.
struct PlayerTailRuntime {
    be<u32> mItemButtonLockTimer;
    u8 _state[0x20];
};
WWHD_SIZE(PlayerTailRuntime, 0x24);

// HD: native resource records begin with the current animation frame.
struct PlayerAnimationFrame { be<f32> mFrame; };
struct PlayerModelResource {
    u8 _header[0x24];
    be<u16> mMaterialNum;
};
struct PlayerBtkRuntime {
    u8 _animation[0x68];
    be<u32> mAnimationResource;
};
WWHD_OFFSET(PlayerModelResource, mMaterialNum, 0x24);
WWHD_OFFSET(PlayerBtkRuntime, mAnimationResource, 0x68);

// HD: camera flags gate the first-person ship-bow hand material update.
struct PlayerCameraHandState {
    u8 _camera[0x100];
    be<u8> mFirstPersonFlags[3];
};
struct PlayerModelNameView {
    u8 _model[0x14];
    be<u32> mpNameData;
};
struct PlayerSwordBlurView {
    u8 _packet[0x9C];
    be<s32> mCount;
    u8 _trailHeader[0xC];
    /* 0xAC */ be<u32> mpPosBuffer;
    u8 _trail[0x38C - 0xB0];
    cXyz mPosition;
};
WWHD_OFFSET(PlayerCameraHandState, mFirstPersonFlags, 0x100);
WWHD_OFFSET(PlayerModelNameView, mpNameData, 0x14);
WWHD_OFFSET(PlayerSwordBlurView, mCount, 0x9C);
WWHD_OFFSET(PlayerSwordBlurView, mPosition, 0x38C);

struct PlayerFrameCounter { be<u32> mTimer; };
struct dDemo_object_c;
struct PlayerDemoObjectState { gptr<dDemo_object_c> mpObject; };

struct PlayerJ3DSysState {
    u8 _state[0x6C];
    gptr<J3DModel> mpModel;
    u8 _padding[4];
    gptr<J3DDrawBuffer> mpOpaDrawBuffer, mpXluDrawBuffer;
};
struct PlayerStageInfo {
    u8 _info[0xC];
    be<u32> mStageFlags;
};
WWHD_OFFSET(PlayerJ3DSysState, mpModel, 0x6C);
WWHD_OFFSET(PlayerJ3DSysState, mpOpaDrawBuffer, 0x74);
WWHD_OFFSET(PlayerJ3DSysState, mpXluDrawBuffer, 0x78);
WWHD_OFFSET(PlayerStageInfo, mStageFlags, 0xC);
enum { dStageType_FF1_e = 5 };

// HD: ship actor fields used by the context-sensitive A button action.
struct PlayerShipActionState {
    u8 _actorAndShipState[0x704];
    gptr<fopAc_ac_c> mTornadoActor;
    be<u32> mWhirlID;
    gptr<fopAc_ac_c> mWhirlActor;
};
WWHD_OFFSET(PlayerShipActionState, mTornadoActor, 0x704);
WWHD_OFFSET(PlayerShipActionState, mWhirlActor, 0x70C);

// HD: Link archive model indices differ from the GameCube archive.
enum {
    dRes_INDEX_LINK_BDL_CL_e = 0x18,
    dRes_INDEX_LINK_BDL_HANDS_e = 0x1D,
    dRes_INDEX_LINK_BDL_KATSURA_e = 0x20,
    dRes_INDEX_LINK_BDL_YAMU_e = 0x29,
    dRes_INDEX_LINK_BDL_PRING_e = 0x23,
    dRes_INDEX_LINK_BDL_SWGRIPA_e = 0x26,
    dRes_INDEX_LINK_BDL_SWGRIPMS_e = 0x45,
    dRes_INDEX_LINK_BDL_PODMS_e = 0x22,
    dRes_INDEX_LINK_BDL_SHA_e = 0x24,
    dRes_INDEX_LINK_BDL_SHMS_e = 0x43,
    dRes_INDEX_LINK_BDL_YMSLS00_e = 0x4D,
    dRes_INDEX_LINK_BDL_HBOOTS_e = 0x2D,
    dRes_INDEX_LINK_BDL_SUIMEN_MUNYA_e = 0x44,
    dRes_INDEX_LINK_BDL_YUCHW00_e = 0x4E,
    dRes_INDEX_LINK_BDL_YBAFO00_e = 0x49,
    dRes_INDEX_LINK_BDL_YMGCS00_e = 0x4B,
};

// HD: native range calls preserve the original register widths and call sequence.
void Player01_clearActorKeep(daPy_actorKeep_l* keep);
void Player01_setActorKeep(daPy_actorKeep_l* keep, fopAc_ac_c* actor);
void Player01_deleteEquipItem(daPy_lk_c* player, BOOL sound);
void Player01_setSwordModel(daPy_lk_c* player, BOOL drawn);
void Player01_makeItemType(daPy_lk_c* player);
void Player01_resetPriTextureAnime(daPy_lk_c* player);
void Player01_setBowModel(daPy_lk_c* player);
void Player01_setBottleModel(daPy_lk_c* player, u32 item);
void Player01_setTextureAnime(daPy_lk_c* player, u32 face, u32 mode);
void Player01_setTextureAnimeResource(daPy_lk_c* player, u32 resource, u32 mode);

// HD: each animation ratio pack owns one weight per joint.
struct Player01AnmRatioPack {
    /* 0x00 */ be<f32> mRatio;
    /* 0x04 */ gptr<PlayerAnimationFrame> mpAnmTransform;
    /* 0x08 */ be<s32> mJointCount;
    /* 0x0C */ gptr<be<f32>> mpJointWeights;
    void setRatio(f32 ratio) {
        for (s32 joint = 0; joint < mJointCount; joint++) {
            mpJointWeights[joint] = ratio;
        }
    }
};
WWHD_OFFSET(Player01AnmRatioPack, mpAnmTransform, 4);
WWHD_OFFSET(Player01AnmRatioPack, mJointCount, 8);
WWHD_OFFSET(Player01AnmRatioPack, mpJointWeights, 0xC);
WWHD_SIZE(Player01AnmRatioPack, 0x10);
struct Player01HeapVtable { u8 _slots[0x54]; be<u32> mFreeAll; };
struct Player01Heap { u8 _heap[0xC]; gptr<Player01HeapVtable> mpVtable; };
WWHD_OFFSET(Player01HeapVtable, mFreeAll, 0x54);
WWHD_OFFSET(Player01Heap, mpVtable, 0xC);
struct Player01SwimDefaults { be<f32> mSwimWallRadius; };
void Player01_setWallR(dBgS_AcchCir* circle, f32 radius);

WWHD_OFFSET(PlayerModelState, mpItemAnimeHeap, 0x3A78);
WWHD_OFFSET(PlayerModelState, mpEquipItemBtk, 0x3A80);
WWHD_OFFSET(PlayerModelState, mpSwordBtk, 0x3AF4);
WWHD_OFFSET(PlayerModelState, mpEquipItemBrk, 0x3B68);
WWHD_OFFSET(PlayerModelState, mpCutfBpk, 0x3BF0);
WWHD_OFFSET(PlayerModelState, mpCutfBtk, 0x3C64);
WWHD_OFFSET(PlayerModelState, mpCutfBrk, 0x3CD8);
WWHD_OFFSET(PlayerSwordBlurView, mpPosBuffer, 0xAC);
// HD: the native BCK object retains its selected resource in the tail.
struct Player01BckRuntime { u8 _animation[0x88]; be<u32> mpBck; };
WWHD_OFFSET(Player01BckRuntime, mpBck, 0x88);
struct Player01FanSwingCallback { be<u32> mVtable, mAlphaOutFlag; u8 _state[4]; };
WWHD_OFFSET(Player01FanSwingCallback, mAlphaOutFlag, 4);
struct Player01LightInfluence { u8 _positionAndColor[0x14]; be<f32> mPower; u8 _state[8]; };
WWHD_OFFSET(Player01LightInfluence, mPower, 0x14);
WWHD_SIZE(Player01LightInfluence, 0x20);

// HD: native calls retain unextended register values at range boundaries.
void Player01_endMtxEmitter(u32 callback);
void Player01_initFrameCtrl(u32 frameCtrl, s32 end);
void Player01_updateFrameCtrl(J3DFrameCtrl* frameCtrl);
void Player01_initOldFrameMorf(u32 oldFrame, f32 duration, u32 start, u32 end);
void Player01_makeMtxEmitter(u32 callback, u32 effect, u32 matrix, cXyz* position, u32 flags);
void Player01_playMorf(u32 morf, u32 flags, u32 reverb, u32 sound);
void Player01_entryBrk(u32 animation, u32 modelData, f32 frame);
void Player01_entryBtk(u32 animation, u32 modelData, f32 frame);
void Player01_entryBpk(u32 animation, u32 modelData, f32 frame);
u32 Player01_checkMainBgm();
u32 Player01_checkSubBgm();
void Player01_initActorAnimSound(u32 sound, u32 buffer, s32 direction, f32 loop);
void Player01_setPlayPosition(u32 sound, f32 position);
BOOL Player01_lockonTruth(u32 attention);
u32 Player01_getLockonList(u32 attention, u32 index);
fopAc_ac_c* Player01_getAttentionActor(u32 entry);
u32 Player01_getActionBtnB(u32 attention);
u32 Player01_getActionBtnX(u32 attention);
u32 Player01_getActionBtnY(u32 attention);
u32 Player01_getActionBtnZ(u32 attention);
u32 Player01_getActionBtnHD(u32 attention);
void Player01_changeBckOnly(u32 animation, u32 resource);
u32 Player01_getAnimeResource(daPy_lk_c* player, u32 heap, u32 index, u32 size);
void Player01_setFrameCtrl(daPy_lk_c* player, u32 control, u32 attribute, s32 start, s32 end, f32 rate, f32 frame);
u32 Player01_getSoundAnimeResource(dRes_control_c* control, SafeString* archive, u32 metadata);
fopAc_ac_c* Player01_fastCreate(s32 name, u32 parameters, cXyz* position, s32 room, u32 angle, u32 scale, s32 subtype, u32 callback, u32 user);
u32 Player01_makeBombParameters(u32 type, u32 flag, u32 fuse);
void Player01_setCarryNow(fopAc_ac_c* actor, u32 flags);
BOOL Player01_startCheckOld(dEvent_manager_c* manager, const char* name);
u32 Player01_getMyNowCutName(dEvent_manager_c* manager, s32 staff);
fopAc_ac_c* Player01_getItemEventPartner(daPy_lk_c* player);
void Player01_hideItem(fopAc_ac_c* item);
u32 Player01_getItemNo(fopAc_ac_c* item);
void Player01_deadItem(fopAc_ac_c* item);
s32 Player01_messageSet(u32 manager, u32 message, u32 flags);
u8 Player01_getMessageStatus();
void Player01_setMessageStatus(u32 manager, u32 status);
void Player01_demoMsgFlagOn();
void Player01_assureMaterialName(u32 string);
f64 Player01_random();
void Player01_setBottleModelFlag(u32 model, u32 flag);
void Player01_initBpk(u32 animation, u32 data, u32 resource, s32 play, s32 mode, f32 speed, s32 start, s32 end, s32 modify, s32 entry);
void Player01_stopSound(u32 sound);
void Player01_itemGetColorOff();
void Player01_endSmoke(u32 callback);

void Player01_stopSwordBgm(u32 usingSword, u32 item);
void Player01_startSwordBgm(u32 usingSword);

// HD: the demo archive remains a native SafeString in the game-info block.
struct Player01DemoArchive { u8 _state[0x5A50]; SafeString mLkDemoAnmArchive; };
WWHD_OFFSET(Player01DemoArchive, mLkDemoAnmArchive, 0x5A50);
struct Player01Attention { u8 _state[0x20]; be<u32> mLockFlags; };
WWHD_OFFSET(Player01Attention, mLockFlags, 0x20);
struct Player01EmitterAlpha { u8 _state[0x247]; be<u8> mGlobalAlpha; };
WWHD_OFFSET(Player01EmitterAlpha, mGlobalAlpha, 0x247);
struct Player01SwimTailCallback {
    be<u32> mVtable;
    be<u8> mEnd;
    u8 _state[0x1B];
    be<u32> mTimer;
    u8 _tail[4];
};
WWHD_OFFSET(Player01SwimTailCallback, mEnd, 4);
WWHD_OFFSET(Player01SwimTailCallback, mTimer, 0x20);
WWHD_SIZE(Player01SwimTailCallback, 0x28);

struct Player01EmitterState {
    u8 _dynamics[0x5C];
    be<s32> mMaxFrame;
    u8 _particleState[0x1E4 - 0x60];
    be<u32> mpEmitterCallBack;
    u8 _drawState[0x254 - 0x1E8];
    be<u32> mFlags;
};
WWHD_OFFSET(Player01EmitterState, mMaxFrame, 0x5C);
WWHD_OFFSET(Player01EmitterState, mpEmitterCallBack, 0x1E4);
WWHD_OFFSET(Player01EmitterState, mFlags, 0x254);

// HD: the animation resource begins with frame, vtable and native metadata.
struct Player01AnimationVtable {
    u8 _slots[0xC];
    be<u32> mGetAttribute;
    u8 _slot10[4];
    be<u32> mGetFrameMax;
};
struct Player01AnimationResource {
    be<f32> mFrame;
    gptr<Player01AnimationVtable> mpVtable;
    be<u32> mpMetadata, mSoundMetadata;
    u32 getAttribute() { return gabi::call_ptr<u32>(mpVtable->mGetAttribute, this); }
    s32 getFrameMax() { return gabi::call_ptr<s32>(mpVtable->mGetFrameMax, this); }
};
WWHD_OFFSET(Player01AnimationVtable, mGetAttribute, 0xC);
WWHD_OFFSET(Player01AnimationVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Player01AnimationResource, mpVtable, 4);
WWHD_OFFSET(Player01AnimationResource, mpMetadata, 8);
WWHD_OFFSET(Player01AnimationResource, mSoundMetadata, 0xC);
WWHD_SIZE(Player01AnimationResource, 0x10);
struct Player01AnmIndex { be<u16> mUnderBckIdx, mUpperBckIdx; };
struct Player01TexAnmEntry { be<u16> mBtpIdx, mBtkIdx; };
struct Player01ProcFuncWords { be<u32> mWords[2]; };
struct Player01ProcInitEntry { Player01ProcFuncWords mProcFunc; be<u32> mProcFlags; };
WWHD_SIZE(Player01AnmIndex, 4);
WWHD_SIZE(Player01TexAnmEntry, 4);
WWHD_SIZE(Player01ProcFuncWords, 8);
WWHD_OFFSET(Player01ProcInitEntry, mProcFlags, 8);
WWHD_SIZE(Player01ProcInitEntry, 0xC);

struct Player01SwordAnmIndexTableStorage { Player01AnmIndex mEntries[27]; };
WWHD_SIZE(Player01SwordAnmIndexTableStorage, sizeof(Player01AnmIndex) * 27);

struct Player01BokoAnmIndexTableStorage { Player01AnmIndex mEntries[27]; };
WWHD_SIZE(Player01BokoAnmIndexTableStorage, sizeof(Player01AnmIndex) * 27);

struct Player01HammerAnmIndexTableStorage { Player01AnmIndex mEntries[27]; };
WWHD_SIZE(Player01HammerAnmIndexTableStorage, sizeof(Player01AnmIndex) * 27);

struct Player01AnmDataTableStorage { PlayerAnmData mEntries[234]; };
WWHD_SIZE(Player01AnmDataTableStorage, sizeof(PlayerAnmData) * 234);

struct Player01TexAnmIndexTableStorage { Player01TexAnmEntry mEntries[173]; };
WWHD_SIZE(Player01TexAnmIndexTableStorage, sizeof(Player01TexAnmEntry) * 173);

struct Player01ProcInitTableStorage { Player01ProcInitEntry mEntries[209]; };
WWHD_SIZE(Player01ProcInitTableStorage, sizeof(Player01ProcInitEntry) * 209);

// The base Link source owns this mapped table; autoglue emits an array reference.
extern PlayerAnmData (&mAnmDataTable)[234];

// HD: both foot emitters dispatch remove through the native callback vtable.
struct Player01FootCallbackVtable { u8 _slots[0x44]; be<u32> mRemove; };
struct Player01FootCallback {
    gptr<Player01FootCallbackVtable> mpVtable;
    u8 _state[0x1C];
    void remove() { gabi::call_ptr(mpVtable->mRemove, this); }
};
struct Player01FootEffect {
    Player01FootCallback mSmokeCallback, mOtherCallback;
    u8 _state[8];
    be<s32> mID;
};
WWHD_OFFSET(Player01FootCallbackVtable, mRemove, 0x44);
WWHD_SIZE(Player01FootCallback, 0x20);
WWHD_OFFSET(Player01FootEffect, mOtherCallback, 0x20);
WWHD_OFFSET(Player01FootEffect, mID, 0x48);
WWHD_SIZE(Player01FootEffect, 0x4C);
struct Player01BarrelState { u8 _actor[0x738]; be<s16> m61C; };
WWHD_OFFSET(Player01BarrelState, m61C, 0x738);
// HD: the sword-tip model exposes native texture/material controller words.
struct Player01SwordTipModelData {
    u8 _modelData[0x40];
    be<u32> mTexturePattern, mTextureScroll, mMaterialAnimation;
};
WWHD_OFFSET(Player01SwordTipModelData, mTexturePattern, 0x40);
WWHD_OFFSET(Player01SwordTipModelData, mTextureScroll, 0x44);
WWHD_OFFSET(Player01SwordTipModelData, mMaterialAnimation, 0x48);

// GameCube names with the verified HD process, resource and sound values.
namespace player01 {
enum : u32 {
    JA_BGM_GET_HEART = 0x80000024,
    JA_BGM_ITEM_GET = 0x80000002,
    JA_BGM_ITEM_GET_S = 0x80000025,
    JA_SE_LK_ITEM_TAKEOUT = 0x2810,
    JA_SE_LK_OUTOF_WATER = 0x3809,
    JA_SE_LK_RUN_SLIP = 0x280D,
    JA_SE_ME_ITEM_GET_S = 0x8F5,
    ModeFlg_CRAWL = 0x1000000,
    ModeFlg_GRAB = 0x100000,
    ModeFlg_HANG = 0x20,
    ModeFlg_HOOKSHOT = 0x200,
    ModeFlg_MIDAIR = 2,
    ModeFlg_PUSHPULL = 0x200000,
    ModeFlg_ROPE = 0x800,
    ModeFlg_SWIM = 0x40000,
    ModeFlg_WHIDE = 0x10,
    dItemNo_BOMB_BAG_e = 0x31,
    dItemNo_BOOMERANG_e = 0x2D,
    dItemNo_DEKU_LEAF_e = 0x34,
    dItemNo_FAIRY_BOTTLE_e = 0x57,
    dItemNo_FIREFLY_BOTTLE_e = 0x58,
    dItemNo_FOREST_WATER_e = 0x59,
    dItemNo_GRAPPLING_HOOK_e = 0x25,
    dItemNo_HALF_SOUP_BOTTLE_e = 0x54,
    dItemNo_HEART_PIECE_e = 7,
    dItemNo_NONE_e = 0xFF,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dItemNo_TELESCOPE_e = 0x20,
    dItemNo_TINGLE_TUNER_e = 0x21,
    dItemNo_WIND_WAKER_e = 0x22,
    dRes_INDEX_LINK_BDL_BINFAIRY_e = 0x13,
    dRes_INDEX_LINK_BDL_BINHO_e = 0x39,
    dRes_INDEX_LINK_BDL_BINLIQUIDH_e = 0x3B,
    dRes_INDEX_LINK_BDL_BINLIQUID_e = 0x3A,
    dRes_INDEX_LINK_BDL_BOTTLECAP_e = 0x15,
    dRes_INDEX_LINK_BDL_BOTTLEEMP_e = 0x3D,
    dRes_INDEX_LINK_BDL_BOW_e = 0x16,
    dRes_INDEX_LINK_BDL_CAMERA_e = 0x17,
    dRes_INDEX_LINK_BDL_FANSMALL_e = 0x1B,
    dRes_INDEX_LINK_BDL_HAMMER_e = 0x1C,
    dRes_INDEX_LINK_BDL_HOOKSHOT_e = 0x1E,
    dRes_INDEX_LINK_BDL_TAKT_e = 0x47,
    dRes_INDEX_LINK_BDL_TCEIVER_e = 0x27,
    dRes_INDEX_LINK_BDL_TELESCOPE_e = 0x2F,
    dRes_INDEX_LKANM_BCK_ARROWRELORDA_e = 0xD,
    dRes_INDEX_LKANM_BCK_CUTAA_e = 0x40,
    dRes_INDEX_LKANM_BCK_CUTAMS_e = 0x41,
    dRes_INDEX_LKANM_BCK_FANWAIT_e = 0x85,
    dRes_INDEX_LKANM_BCK_GRABWAIT_e = 0x95,
    dRes_INDEX_LKANM_BCK_HAMMERDAM_e = 0x97,
    dRes_INDEX_LKANM_BCK_HOOKSHOTA_e = 0xA5,
    dRes_INDEX_LKANM_BTK_TSWMS_e = 0x1C3,
    daPyFace_TDAMDASH = 0x40,
    daPyFace_TDASHKAZE = 0xA2,
    daPyFace_TMABAA = 0,
    daPyFace_TMABAB = 2,
    daPyFace_TMABAC = 3,
    daPyFace_TMABACB = 4,
    daPyFace_TMABACC = 6,
    daPyFace_TMABAE = 9,
    daPyFace_TMABAF = 0xE,
    daPyFace_TMABAG = 0xF,
    daPyFace_TMABAH = 0x10,
    daPyFace_TMABAH_TABEKOBE = 0x92,
    daPyFace_TMABAJ_TEYORIME = 0x12,
    daPyItem_BOKO_e = 0x101,
    daPyItem_NONE_e = 0x100,
    daPyItem_SWORD_e = 0x103,
    daPyItem_UNK10B_e = 0x10B,
    daPyProc_CRAWL_END_e = 0x12,
    daPyProc_CUT_ROLL_END_e = 0x57,
    daPyProc_DEMO_GET_DANCE_e = 0xD0,
    daPyProc_DEMO_GET_ITEM_e = 0xAE,
    daPyProc_DEMO_LETTER_READ_e = 0xCD,
    daPyProc_DEMO_PRESENT_e = 0xC3,
    daPyProc_DEMO_TALISMAN_WAIT_e = 0xB7,
    daPyProc_FAN_SWING_e = 0x92,
    daPyProc_GRAB_MISS_e = 0x70,
    daPyProc_GRAB_READY_e = 0x6E,
    daPyProc_LARGE_DAMAGE_WALL_e = 0x6A,
    daPyProc_MOVE_e = 6,
    daPyProc_NOT_USE_e = 0xA2,
    daPyProc_ROPE_MOVE_e = 0x7D,
    daPyProc_ROPE_SUBJECT_e = 0x76,
    daPyProc_ROPE_SWING_START_e = 0x7C,
    daPyProc_SHIP_BOW_e = 0x8D,
    daPyProc_SHIP_CRANE_e = 0x8F,
    daPyProc_SLIP_e = 0x19,
    daPyProc_SLOW_FALL_e = 0x28,
    daPyProc_SWIM_MOVE_e = 0x37,
    daPyProc_WAIT_e = 4,
    fpcNm_ARROW_e = 0x1D8,
    fpcNm_BOMB_e = 0x126,
    fpcNm_BOOMERANG_e = 0x1B0,
    fpcNm_Demo_Item_e = 0x101,
    fpcNm_HIMO2_e = 0x1BE,
    fpcNm_HIMO3_e = 0x1BF,
    fpcNm_HOOKSHOT_e = 0xA9,
    fpcNm_ITEM_e = 0xFF,
    fpcNm_Obj_Barrel_e = 0x1C8,
};
}

// HD: the two sword resource rows retain the native eight-halfword layout.
struct Player01SwordModelResources {
    be<u16> mBladeModel;
    be<u16> mGlowModel;
    be<u16> mGlowBtk;
    be<u16> mGlowBrk;
    be<u16> mTipStabModel;
    be<u16> mTipStabBpk;
    be<u16> mTipStabBtk;
    be<u16> mTipStabBrk;
};
WWHD_SIZE(Player01SwordModelResources, 0x10);
struct Player01SwordModelTable { Player01SwordModelResources mEntries[2]; };
WWHD_SIZE(Player01SwordModelTable, 0x20);
