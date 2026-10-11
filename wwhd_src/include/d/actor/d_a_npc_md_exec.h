/**
 * Medli execution bindings and private WWHD layout projections.
 * HD: emitter, mirror packet and morph fields differ from the GameCube layouts;
 * the shared actor layout remains in d_a_npc_md.h.
 */
#pragma once
#include "d/actor/d_a_npc_md.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
bool mdExec_setAnmFNDirect(mDoExt_McaMorf* m, int loop, f32 morf, f32 speed, const char* fn, const char* snd, const char* arc);
void mdExec_changePlayer(fopAc_ac_c* self, fopAc_ac_c* player);
BOOL mdExec_padTrigRorStart(s32 port);
void mdExec_getXYZRotateMtx(s16 x, s16 y, s16 z, u32 mtx);
void* mdExec_getObjectRes(dRes_control_c* control, SafeString* arc, SafeString* name);
void mdExec_setMorfAnm(mDoExt_McaMorf2* m, void* a1, void* a2, f32 blend, s32 loop, f32 morf, f32 speed, f32 start, f32 end, void* snd);
u8 mdExec_calcTimerU8(be<u8>* timer);
void mdExec_initAnmPlay(void* anm, s16 frameMax, s32 mode, f32 speed, s16 start, s16 end, bool modify);
void mdExec_orderEvent(dEvt_control_c* event, u16 type, u16 prio, u16 flag, u16 hind, fopAc_ac_c* a1, fopAc_ac_c* a2, s16 eventId, u8 infoIdx);
void mdExec_normalizeZP(cXyz* src, cXyz* out);
void mdExec_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly);
s16 mdExec_getPriority(void* actor);
void mdExec_toDrawQueue(u32 tag, s16 priority);
s16 mdExec_calcTimerS16(be<s16>* timer);
fpc_ProcID mdExec_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 argFlag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale);
void mdExec_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe);
u32 mdExec_setTactMessage(u32 manager);
u32 mdExec_setMessage(u32 manager, u32 message, cXyz* eye);
u32 mdExec_getRunEventName(dEvent_manager_c* manager);
BOOL mdExec_checkPolySafe(dBgS* bgs, u32 poly);
u8 mdExec_playMorf(mDoExt_McaMorf2* m, cXyz* pos, u32 sound, s32 reverb);
BOOL mdExec_checkFrame(u32 control, f32 frame);
BOOL mdExec_startCheckOld(dEvent_manager_c* manager, u32 name);
BOOL mdExec_endCheckOld(dEvent_manager_c* manager, u32 name);
s32 mdExec_getRndValue(s32 base, s32 range);
void mdExec_monsSeStart(u32 sound, cXyz* pos, u32 process, s32 reverb);
BOOL mdExec_checkLight(dDetect_c* detect, cXyz* pos);
void mdExec_lineVsPosSuisenCross(u32 start, u32 end, u32 pos, cXyz* out);
void mdExec_makeEmitter(daPy_mtxFollowEcallBack_l* cb, u16 id, Mtx34* mtx, cXyz* pos, cXyz* scale);
void mdExec_endMtxFollow(daPy_mtxFollowEcallBack_l* cb);
void mdExec_XYZrotM(Mtx34* mtx, s16 x, s16 y, s16 z);
void mdExec_setStartEnd(u32 line, cXyz* start, cXyz* end);
void mdExec_setMass(u8* mass, void* obj, u8 param);
void mdExec_clearCoHit(dCcD_Cyl* cyl);
void mdExec_resetAtHit(dCcD_Cps* capsule);
s32 mdExec_getMyActIdx(dEvent_manager_c* manager, s32 staffId, u32 table, s32 count, s32 force, s32 nameType);
u32 mdExec_getPartnerId(u32 event, fopAc_ac_c* actor);
void mdExec_startControlSound(s32 sound);
void mdExec_stopZelAnime(mDoExt_McaMorf2* m);
void mdExec_endRipple(dPa_rippleEcallBack_md* cb);
void mdExec_destroyLineCheck(u32 check, s32 flags);
void mdExec_destroyColorAnm(u32 anm, s32 flags);
void mdExec_destroyTevRegAnm(u32 anm, s32 flags);
void mdExec_destroyModelDataArray(u32 data);
u32 mdExec_findHeap(u32 heap, u32 block);
void mdExec_destroyArray(u32 array, s32 count, s32 stride, u32 destructor, s32 reserved1, s32 reserved2);
void mdExec_destroyModel(u32 model, s32 flags);
void mdExec_destroyPacket(u32 packet, s32 flags);
void mdExec_destroyCapsule(dCcD_Cps* capsule, s32 flags);
void mdExec_destroyCylinder(dCcD_Cyl* cylinder, s32 flags);
void mdExec_destroyCollisionStatus(dCcD_Stts* status, s32 flags);
void mdExec_destroyPlayerNpc(daNpc_Md_c* actor, s32 flags);
void mdExec_entryBtp(void* anm, J3DModelData* data, s16 frame);
void mdExec_entryMorfDL(mDoExt_McaMorf2* m);
void mdExec_drawDamageFog(void* actor);
void mdExec_concatMtx(const Mtx34* a, const Mtx34* b, Mtx34* out);
void mdExec_updateMirror(u32 packet, Mtx34* mtx, u8 alpha, f32 angle);
void mdExec_entryImmediate(u32 list, u32 packet, u16 index);
BOOL mdExec_checkInitialRoom(void* actor);
BOOL mdExec_initialRestartOption(void* actor, s8 option, BOOL isDungeon);
BOOL mdExec_checkMoveStop(void* actor);
BOOL mdExec_checkNowPosMove(void* actor, u32 name);
s8 mdExec_checkMoveBlock(void* actor, cXyz* pos);
void mdExec_objWindHitCheck(void* actor, dCcD_Cyl* cylinder);
BOOL mdExec_checkMoveBg(dBgS* bgs, u32 poly);
void mdExec_moveBgCrrPos(dBgS* bgs, u32 poly, bool moving, cXyz* pos, csXyz* angle1, csXyz* angle2);
s8 mdExec_getRoomId(dBgS* bgs, u32 poly);
u8 mdExec_getPolyColor(dBgS* bgs, u32 poly);
s32 mdExec_getGroundCode(dBgS* bgs, u32 poly);
void mdExec_setSimpleLand(dPa_control_c* particle, u32 poly, cXyz* pos, csXyz* angle, f32 a, f32 b, f32 c, dKy_tevstr_c* tev, s32* out, s32 count);
void mdExec_startRestartRoom(fopAc_ac_c* link, u32 mode, s32 index, f32 speed, s32 option);

// Private projections disambiguate the TU's three verified overloads.
struct MdBodyAnimationActor : daNpc_Md_c {
    bool dNpc_Md_setAnm(mDoExt_McaMorf2*, f32, int, f32, f32, const char*, const char*, const char*);
};
struct MdWingAnimationActor : daNpc_Md_c {
    bool dNpc_Md_setAnm(mDoExt_McaMorf*, int, f32, f32, const char*, const char*);
};
struct MdLookBackActor : daNpc_Md_c { void lookBack(int, int, int); };
WWHD_SIZE(MdBodyAnimationActor, 0x43DC);
WWHD_SIZE(MdWingAnimationActor, 0x43DC);
WWHD_SIZE(MdLookBackActor, 0x43DC);

namespace MdExec {
struct Emitter {
    u8 padding0[0x8];
    /* 0x008 */ cXyz mEmitterScale;
    /* 0x014 */ cXyz mEmitterTranslation;
    u8 padding20[0x3C];
    /* 0x05C */ be<s32> mMaxFrame;
    u8 padding60[0x14C];
    /* 0x1AC */ be<u32> mpFirstParticle;
    u8 padding1B0[0x34];
    /* 0x1E4 */ be<u32> mpEmitterCallBack;
    u8 padding1E8[0x8];
    /* 0x1F0 */ Mtx34 mGlobalRotation;
    u8 padding220[0xC];
    /* 0x22C */ cXyz mGlobalTranslation;
    u8 padding238[0xF];
    /* 0x247 */ be<u8> mGlobalAlpha;
    u8 padding248[0xC];
    /* 0x254 */ be<u32> mFlags;
    u8 padding258[0xA];
    /* 0x262 */ be<u8> mScaleFlag;
    u8 padding263[0x1];
};
WWHD_OFFSET(Emitter, mEmitterScale, 0x8);
WWHD_OFFSET(Emitter, mEmitterTranslation, 0x14);
WWHD_OFFSET(Emitter, mMaxFrame, 0x5C);
WWHD_OFFSET(Emitter, mpFirstParticle, 0x1AC);
WWHD_OFFSET(Emitter, mpEmitterCallBack, 0x1E4);
WWHD_OFFSET(Emitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(Emitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(Emitter, mGlobalAlpha, 0x247);
WWHD_OFFSET(Emitter, mFlags, 0x254);
WWHD_OFFSET(Emitter, mScaleFlag, 0x262);
WWHD_SIZE(Emitter, 0x264);
struct ParticleLink {
    /* 0x000 */ be<u32> mpObject;
    u8 padding4[0x8];
    /* 0x00C */ be<u32> mpNext;
};
WWHD_OFFSET(ParticleLink, mpObject, 0x0);
WWHD_OFFSET(ParticleLink, mpNext, 0xC);
WWHD_SIZE(ParticleLink, 0x10);
struct Particle {
    u8 padding0[0xCC];
    /* 0x0CC */ be<u32> mFlags;
};
WWHD_OFFSET(Particle, mFlags, 0xCC);
WWHD_SIZE(Particle, 0xD0);
struct Morf2 {
    u8 padding0[0x90];
    /* 0x090 */ gptr<J3DModel> mpModel;
    u8 padding94[0x10];
    /* 0x0A4 */ be<f32> mPlaySpeed;
    /* 0x0A8 */ be<f32> mFrame;
};
WWHD_OFFSET(Morf2, mpModel, 0x90);
WWHD_OFFSET(Morf2, mPlaySpeed, 0xA4);
WWHD_OFFSET(Morf2, mFrame, 0xA8);
WWHD_SIZE(Morf2, 0xAC);
struct TexturePatternAnm {
    u8 padding0[0x4];
    /* 0x004 */ be<f32> mFrame;
    u8 padding8[0x6C];
};
WWHD_OFFSET(TexturePatternAnm, mFrame, 0x4);
WWHD_SIZE(TexturePatternAnm, 0x74);
struct AnmPrm {
    /* 0x000 */ be<s8> anmTblIdx;
    /* 0x001 */ be<u8> armAnmTblIdx;
    /* 0x002 */ be<u8> btpAnmTblIdx;
    u8 padding3[0x1];
    /* 0x004 */ be<s32> loopMode;
    /* 0x008 */ be<f32> morf;
    /* 0x00C */ be<f32> speed;
};
WWHD_OFFSET(AnmPrm, anmTblIdx, 0x0);
WWHD_OFFSET(AnmPrm, armAnmTblIdx, 0x1);
WWHD_OFFSET(AnmPrm, btpAnmTblIdx, 0x2);
WWHD_OFFSET(AnmPrm, loopMode, 0x4);
WWHD_OFFSET(AnmPrm, morf, 0x8);
WWHD_OFFSET(AnmPrm, speed, 0xC);
WWHD_SIZE(AnmPrm, 0x10);
struct AnmTbl {
    /* 0x000 */ char m00[32];
    /* 0x020 */ char m20[32];
};
WWHD_OFFSET(AnmTbl, m00, 0x0);
WWHD_OFFSET(AnmTbl, m20, 0x20);
WWHD_SIZE(AnmTbl, 0x40);
inline Emitter* emitter(JPABaseEmitter* value) { return reinterpret_cast<Emitter*>(value); }
inline Emitter* emitter(u32 value) { return gabi::at<Emitter>(value); }
inline Morf2* morf(mDoExt_McaMorf2* value) { return reinterpret_cast<Morf2*>(value); }
inline be<u32>& word(be<f32>& field) { return *reinterpret_cast<be<u32>*>(&field); }
inline const be<u32>& word(const be<f32>& field) { return *reinterpret_cast<const be<u32>*>(&field); }
inline void becomeInvalidEmitter(Emitter* value) { value->mMaxFrame = -1; value->mFlags = value->mFlags | 1; }
}

namespace MdExec {
struct ActionWords { union { gabi::ptmf<BOOL(daNpc_Md_c::*)(void*)> fn; be<u32> mWords[2]; }; };
WWHD_OFFSET(ActionWords, fn, 0); WWHD_SIZE(ActionWords, 8);
struct SaveInfo { u8 padding[0x644]; be<u8> mEvent; };
WWHD_OFFSET(SaveInfo, mEvent, 0x644); WWHD_SIZE(SaveInfo, 0x645);
struct SavePointer { gptr<SaveInfo> mpInfo; };
WWHD_SIZE(SavePointer, 4);
struct MessagePointer { be<u32> mpManager; };
WWHD_SIZE(MessagePointer, 4);
inline void copyAction(ProcFunc_l* dst, const ActionWords& src) {
    auto* out = reinterpret_cast<ActionWords*>(dst);
    out->mWords[0] = src.mWords[0]; out->mWords[1] = src.mWords[1];
}
}

namespace MdExec {
struct anmTypeData_TalkTable { be<s32> values[44]; };
WWHD_SIZE(anmTypeData_TalkTable, sizeof(be<s32>) * (44));
struct eventModesTable { be<s8> values[13]; };
WWHD_SIZE(eventModesTable, sizeof(be<s8>) * (13));
struct l_anmPrmTable { AnmPrm values[0x31]; };
WWHD_SIZE(l_anmPrmTable, sizeof(AnmPrm) * (0x31));
struct l_anmTblTable { AnmTbl values[39]; };
WWHD_SIZE(l_anmTblTable, sizeof(AnmTbl) * (39));
struct armAnmTblTable { AnmTbl values[32]; };
WWHD_SIZE(armAnmTblTable, sizeof(AnmTbl) * (32));
struct wingAnmTblTable { AnmTbl values[8]; };
WWHD_SIZE(wingAnmTblTable, sizeof(AnmTbl) * (8));
struct event_name_tblTable { be<u32> values[10]; };
WWHD_SIZE(event_name_tblTable, sizeof(be<u32>) * (10));
struct l_harp_play_seTable { be<u32> values[12]; };
WWHD_SIZE(l_harp_play_seTable, sizeof(be<u32>) * (12));
}

namespace MdExec {
struct MirrorModel {
    be<u32> mState;
    u8 mModel[0x154];
    u8 mModelData[0xF4];
    be<u32> mBufferState;
    be<u32> mpBuffer;
};
WWHD_OFFSET(MirrorModel, mModel, 4);
WWHD_OFFSET(MirrorModel, mModelData, 0x158);
WWHD_OFFSET(MirrorModel, mBufferState, 0x24C);
WWHD_OFFSET(MirrorModel, mpBuffer, 0x250);
WWHD_SIZE(MirrorModel, 0x254);
// HD: the expanded mirror packet embeds models, animations and a line check.
struct MirrorPacket {
    u8 padding[0x2614];
    u8 mModels[3][0x198];
    u8 padding2ADC[4];
    MirrorModel mModelData[2];
    u8 padding2F88[0x10];
    be<u32> mControl;
    u8 padding2F9C[4];
    u8 mTevRegAnm[0xC];
    u8 mColorAnm[2][0xA8];
    u8 padding30FC[0x2C0];
    dBgS_LinChk mLinChk;
};
WWHD_OFFSET(MirrorPacket, mModels, 0x2614);
WWHD_OFFSET(MirrorPacket, mModelData, 0x2AE0);
WWHD_OFFSET(MirrorPacket, mControl, 0x2F98);
WWHD_OFFSET(MirrorPacket, mTevRegAnm, 0x2FA0);
WWHD_OFFSET(MirrorPacket, mColorAnm, 0x2FAC);
WWHD_OFFSET(MirrorPacket, mLinChk, 0x33BC);
WWHD_SIZE(MirrorPacket, 0x3428);
struct HeapVtable { u8 padding[0x3C]; gfn free; };
WWHD_OFFSET(HeapVtable, free, 0x3C); WWHD_SIZE(HeapVtable, 0x40);
struct Heap { u8 padding[0xC]; gptr<HeapVtable> mpVtable; };
WWHD_OFFSET(Heap, mpVtable, 0xC); WWHD_SIZE(Heap, 0x10);
struct ActorPointer { gptr<daNpc_Md_c> mpActor; }; WWHD_SIZE(ActorPointer, 4);
struct HeapPointer { be<u32> mpHeap; }; WWHD_SIZE(HeapPointer, 4);
inline void freeHeapBlock(u32 address, u32 block) {
    auto* heap = gabi::at<Heap>(address);
    gabi::call_ptr<void>(heap->mpVtable->free.get(), heap, block);
}
constexpr u32 LINE_POLY_PASS_VTABLE = 0x1001D788;
constexpr u32 GROUP_PASS_VTABLE = 0x1001D708;
constexpr u32 LINE_POLY_INFO_VTABLE = 0x1001D6F8;
constexpr u32 MODEL_MEMBER_DESTRUCTOR = 0x02295490;
constexpr u32 CIRCLE_MEMBER_DESTRUCTOR = 0x0229543C;
}

namespace MdExec {
struct SafeStringVtable { u8 padding[0x14]; gfn assureTermination; };
WWHD_OFFSET(SafeStringVtable, assureTermination, 0x14); WWHD_SIZE(SafeStringVtable, 0x18);
inline void assureString(SafeString* value) { gabi::call_ptr<void>(gabi::at<SafeStringVtable>(value->__vtbl)->assureTermination.get(), value); }
inline be<u8>* bytes(u32 value) { return gabi::at<be<u8>>(value); }
struct RoomStatus { be<u8> mStatus; u8 padding[0x22B]; };
WWHD_OFFSET(RoomStatus, mStatus, 0); WWHD_SIZE(RoomStatus, 0x22C);
struct RoomTable { RoomStatus mRooms[64]; }; WWHD_SIZE(RoomTable, 0x8B00);
struct ShipState { u8 padding[0x644]; be<u32> mHeadFlags; };
WWHD_OFFSET(ShipState, mHeadFlags, 0x644); WWHD_SIZE(ShipState, 0x648);
struct ModelAnmState { u8 padding[0x38]; be<u32> mpBtp; u8 padding3C[8]; be<u32> mpBtk; };
WWHD_OFFSET(ModelAnmState, mpBtp, 0x38); WWHD_OFFSET(ModelAnmState, mpBtk, 0x44); WWHD_SIZE(ModelAnmState, 0x48);
struct HitObject { u8 padding[0x10]; be<u32> mAtType; };
WWHD_OFFSET(HitObject, mAtType, 0x10); WWHD_SIZE(HitObject, 0x14);
struct CapsuleGeometry { cXyz mStart; cXyz mEnd; };
WWHD_OFFSET(CapsuleGeometry, mEnd, 0xC); WWHD_SIZE(CapsuleGeometry, 0x18);
struct Capsule { u8 padding[0x118]; CapsuleGeometry mGeometry; };
WWHD_OFFSET(Capsule, mGeometry, 0x118); WWHD_SIZE(Capsule, 0x130);
struct PlaceWords { be<u32> mX, mY, mZ, mAngleXY, mAngleZRoom; };
WWHD_OFFSET(PlaceWords, mAngleXY, 0xC); WWHD_OFFSET(PlaceWords, mAngleZRoom, 0x10); WWHD_SIZE(PlaceWords, 0x14);
inline PlaceWords* placeWords(actor_place* place) { return reinterpret_cast<PlaceWords*>(place); }
struct NamePointer { gptr<const char> mpName; }; WWHD_SIZE(NamePointer, 4);
struct EventNames { be<u32> values[22]; }; WWHD_SIZE(EventNames, 0x58);
struct EventActions { ProcFunc_l values[22]; }; WWHD_SIZE(EventActions, 0xB0);
struct Play {
    u8 padding[0x5B38]; gptr<fopAc_ac_c> mpCb1Player;
    u8 padding5B3C[0x9E]; be<u8> mTactMessageMode;
    u8 padding5BDB[0x1A1]; be<u32> mpXluList;
};
WWHD_OFFSET(Play, mpCb1Player, 0x5B38); WWHD_OFFSET(Play, mTactMessageMode, 0x5BDA);
WWHD_OFFSET(Play, mpXluList, 0x5D7C); WWHD_SIZE(Play, 0x5D80);
inline Play* play() { return reinterpret_cast<Play*>(dComIfGp_get()); }
constexpr u32 SAFE_STRING_VTABLE = 0x1001D6A0;
constexpr u32 ARRIVAL_GND_NAME = 0x1001DC8C;
constexpr u32 OPTION_CHAR_END_NAME = 0x1001DCE0;
constexpr u32 DRAGON_ROOST_SPRING_NAME = 0x1001DD10;
}

namespace MdExec {
constexpr u32 JUMP_PLAYER_ACTION = 0x02293FA8;
constexpr u32 FALL_NPC_ACTION = 0x02291644;
constexpr u32 FALL02_NPC_ACTION = 0x02291764;
constexpr u32 MKAMAE_PLAYER_ACTION = 0x02294984;
constexpr u32 FLY_PLAYER_ACTION = 0x02294160;
constexpr u32 WALK_PLAYER_ACTION = 0x02293B5C;
constexpr u32 SEARCH_NPC_ACTION = 0x02292C48;
constexpr u32 WAIT_NPC_ACTION = 0x0228EB88;
constexpr u32 RETURN_TO_PLAYER_NPC_ACTION = 0x0229310C;
}

namespace MdExec {
inline ParticleLink* particleLink(u32 address) { return gabi::at<ParticleLink>(address); }
inline Particle* particle(u32 address) { return gabi::at<Particle>(address); }
inline Morf2* morfState(u32 address) { return gabi::at<Morf2>(address); }
inline ShipState* shipState(u32 address) { return gabi::at<ShipState>(address); }
inline J3DModel* modelAt(u32 address) { return gabi::at<J3DModel>(address); }
inline Mtx34* matrixAt(u32 address) { return gabi::at<Mtx34>(address); }
inline cXyz* normalAt(u32 address) { return gabi::at<cXyz>(address); }
inline JPABaseEmitter* nativeEmitter(u32 address) { return gabi::at<JPABaseEmitter>(address); }
inline dDetect_c* detect() { return gabi::at<dDetect_c>(dComIfGp_ea() + PLAY_DETECT); }
inline u8* mass() { return gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS); }
}
