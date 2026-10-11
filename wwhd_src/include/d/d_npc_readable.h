// NPC helper implementations.
#pragma once
#include "d/actor/d_a_player.h"
#include "d/d_item_2.h"
#include "d/d_npc.h"
#include "d/d_path.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_msg.h"

namespace npc_readable {
struct dStage_objectNameInf {
    /* 0x00 */ char name[8];
    /* 0x08 */ be<s16> procname;
    /* 0x0A */ be<s8> argument;
    /* 0x0B */ u8 pad;
};
WWHD_SIZE(dStage_objectNameInf, 12);
WWHD_OFFSET(dStage_objectNameInf, procname, 8);
WWHD_OFFSET(dStage_objectNameInf, argument, 10);
struct EventInteger {
    /* 0x00 */ be<s16> high;
    /* 0x02 */ be<s16> low;
};
WWHD_SIZE(EventInteger, 4);
WWHD_OFFSET(EventInteger, low, 2);
inline s16 eventLow(const void* value) { return static_cast<const EventInteger*>(value)->low; }

// The public header retains its call bindings for other translation units.
// These implementation views inherit the same HD storage without adding fields.
struct dNpc_PathRun_c : ::dNpc_PathRun_c {
    BOOL setNearPathIndx(cXyz* position, f32 weight);
    f32 setNearPathIndxMk(cXyz* position);
    BOOL setNearPathIndxMk2(cXyz* position, u32 current, s32 maxDifference);
    BOOL chkPointPass(cXyz* position, u32 forward);
    BOOL chkInside(cXyz* position);
    BOOL setInf(s32 id, s32 room, u32 reverse);
    BOOL setInfDrct(u32 path);
    u32 nextPath();
    void getPoint(cXyz* output, u32 index);
    BOOL nextIdx();
    BOOL nextIdxAuto();
    s32 absIdx(s32 first, s32 second);
    u32 maxPoint();
    u32 pointArg(u32 index);
    BOOL incIdx();
    BOOL incIdxLoop();
    BOOL incIdxAuto();
    BOOL decIdx();
    BOOL decIdxLoop();
    BOOL decIdxAuto();
};
WWHD_SIZE(dNpc_PathRun_c, 8);
struct dNpc_JntCtrl_c : ::dNpc_JntCtrl_c {
    void lookAtTarget_2(be<s16>* angle, cXyz* target, cXyz* origin, s32 yaw, u32 velocity,
                        u32 locked);
    BOOL move(s32 requested, u32 axis);
    BOOL follow(be<s16>* angle, s32 goal, s32 maxStep, u32 axis);
    void lookAtTarget(be<s16>* angle, cXyz* target, cXyz* eye, s32 yaw, s32 velocity, u32 headOnly);
    void turn_fromBackbone2Head(s32 requested, be<s16>* head, be<s16>* back, u32 direction);
    void turn_fromHead2Backbone(s32 requested, be<s16>* head, be<s16>* back);
    BOOL angCalcS(be<s16>* angle, s32 goal, s32 minimum, s32 maximum);
    s16 chkLim(s32 value, u32 joint, u32 axis);
    s16 follow_current(be<s16>* angle, s32 amount);
    void setParam(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 g);
    void limitter(be<s16>* angle, s32 upper, s32 lower);
};
WWHD_SIZE(dNpc_JntCtrl_c, 0x34);
// HD: direct function words stored in the GHS member-pointer representation.
enum HeadSwingProc : u32 {
    SWING_VERTICAL = 0x0259F448,
    SWING_HORIZONE = 0x0259F5A4,
};
struct dNpc_HeadAnm_c : ::dNpc_HeadAnm_c {
    void swing_vertical_init(s32 count, s32 step, s32 amplitude, u32 resume);
    void swing_horizone_init(s32 count, s32 step, s32 amplitude, u32 resume);
    void swing_vertical();
    void swing_horizone();
    void move();
};
WWHD_SIZE(dNpc_HeadAnm_c, 0x20);
struct dNpc_EventCut_c : ::dNpc_EventCut_c {
    using ProcFunc = void (dNpc_EventCut_c::*)();
    struct cut_entry_t {
        /* 0x00 */ gabi::ptmf<ProcFunc> start;
        /* 0x08 */ gabi::ptmf<ProcFunc> proc;
    };
    BOOL cutProc();
    void cutTurnToActorStart();
    void cutTurnToActorProc();
    void cutMoveToActorProc();
    void cutMoveToActorStart();
    void cutTurnToPosStart();
    void cutTurnToPosProc();
    void cutMoveToPosStart();
    void cutMoveToPosProc();
    void cutWaitStart();
    void cutWaitProc();
    void cutTalkMsgStart();
    void cutContinueTalkStart();
    void cutTalkMsgProc();
    void setActorInfo(const char* name, fopAc_ac_c* actor);
    void setActorInfo2(const char* name, ::fopNpc_npc_c* actor);
};
WWHD_SIZE(dNpc_EventCut_c, 0x6C);
WWHD_SIZE(dNpc_EventCut_c::cut_entry_t, 0x10);
WWHD_OFFSET(dNpc_EventCut_c::cut_entry_t, proc, 8);
enum {
    fopMsgStts_MSG_DISPLAYED_e = 14,
    fopMsgStts_MSG_DESTROYED_e = 19,
};
struct NpcVtable {
    /* 0x00 */ u8 pad[0x14];
    /* 0x14 */ gfn next_msgStatus;
    /* 0x18 */ u8 pad18[4];
    /* 0x1C */ gfn getMsg;
    /* 0x20 */ u8 pad20[4];
    /* 0x24 */ gfn anmAtr;
};
WWHD_SIZE(NpcVtable, 0x28);
WWHD_OFFSET(NpcVtable, next_msgStatus, 0x14);
WWHD_OFFSET(NpcVtable, getMsg, 0x1C);
WWHD_OFFSET(NpcVtable, anmAtr, 0x24);
struct fopNpc_npc_c : ::fopNpc_npc_c {
    u16 talk(s32 mode);
    void setCollision(f32 radius, f32 height);
};
WWHD_SIZE(fopNpc_npc_c, 0x7DC);
} // namespace npc_readable

u32 npc_pathGetRoom(s32 id, s32 room);
u32 npc_pathGetNext(u32 path);
cXyz* npc_allocatePoint(u32 size);

void npc_addCalcAngle(be<s16>* angle, s32 goal, s32 scale, s32 maximum, s32 minimum);

void npc_addCalcAngleL(be<s32>* angle, s32 goal, s32 scale, s32 maximum, s32 minimum);
s32 npc_angleDistance(s32 a, s32 b);
s32 npc_jointLimit(npc_readable::dNpc_JntCtrl_c* self, s32 value, u32 joint, u32 axis);
f32 npc_sqrt(f32 value);
s32 npc_atan2s(f32 x, f32 z);

s32 npc_targetAngleY(cXyz* origin, cXyz* target);
s32 npc_targetAngleX(cXyz* origin, cXyz* target);
s32 npc_jointConsume(npc_readable::dNpc_JntCtrl_c* self, be<s16>* angle, s32 amount);

void npc_clearMemory(void* memory, u32 size);

void npc_vectorSubtract(cXyz* a, cXyz* result, cXyz* b);
f32 npc_vectorSquareMag(cXyz* vector);

u32 npc_calcTimer(be<s32>* timer);
u32 npc_talk(fopNpc_npc_c* actor, s32 mode);

void npc_addCalc2(be<f32>* value, f32 goal, f32 factor, f32 speed);

using NpcSearchCallback = u32 (*)(fopAc_ac_c*, npc_readable::dNpc_EventCut_c*);
npc_readable::dStage_objectNameInf* npc_searchName(const char* name);
u32 npc_searchActors(NpcSearchCallback callback, npc_readable::dNpc_EventCut_c* cut);
void npc_vectorAdd(cXyz* a, cXyz* result, cXyz* b);
u32 npc_playerEye(cXyz* result, f32 offset);
u32 npc_searchActorAngleY(fopAc_ac_c* actor, fopAc_ac_c* target);

struct NpcString {
    /* 0x00 */ be<u32> data;
    /* 0x04 */ be<u32> vt;
};
struct NpcSafeStringVtable {
    /* 0x00 */ u8 pad[0x14];
    /* 0x14 */ gfn assure;
};
WWHD_SIZE(NpcString, 8);
WWHD_OFFSET(NpcString, vt, 4);
WWHD_OFFSET(NpcSafeStringVtable, assure, 0x14);
void npc_assureString(NpcString* string);

using NpcResourceLookup = void* (*)(dRes_control_c*, NpcString*, s32);
void* npc_getObjectIDRes(dRes_control_c* manager, NpcString* arc, s32 id);
void* npc_getObjectRes(dRes_control_c* manager, NpcString* arc, s32 id);
void* npc_getObjectNamedRes(dRes_control_c* manager, NpcString* arc, NpcString* name);
void npc_setMorph(mDoExt_McaMorf* morf, void* anim, u32 loop, void* sound, f32 blend, f32 speed,
                  f32 start, f32 end);

u32 npc_messageSet(msg_class* message, u32 number, cXyz* eye);
u32 npc_messageStatus(msg_class* message);
void npc_messageSetStatus(msg_class* message, u32 state);

namespace npc_readable {
struct ShapeAttr {
    /* 0x00 */ u8 pad[0x18];
    /* 0x18 */ be<u32> aabVtable;
    /* 0x1C */ be<u32> shapeVtable;
};
WWHD_SIZE(ShapeAttr, 0x20);
WWHD_OFFSET(ShapeAttr, aabVtable, 0x18);
WWHD_OFFSET(ShapeAttr, shapeVtable, 0x1C);
struct GlobalObjectRecord {
    /* 0x00 */ u8 bytes[12];
};
WWHD_SIZE(GlobalObjectRecord, 12);
} // namespace npc_readable
void npc_actorCtor(fopAc_ac_c* actor);
void npc_jointCtor(dNpc_JntCtrl_c* joint);
void npc_eventCutCtor(dNpc_EventCut_c* cut);
void npc_objAcchCtor(dBgS_ObjAcch* acch);
void npc_acchCirCtor(dBgS_AcchCir* circle);
void npc_ccSttsCtor(dCcD_Stts* status);
void npc_gSttsCtor(be<u32>* status);
void npc_gObjInfCtor(dCcD_GObjInf* collider);
void npc_cylinderCtor(cM3dGCyl* cylinder);
void npc_colliderDt(dCcD_Cyl* collider, u32 flags);
void npc_sttsDt(dCcD_Stts* status, u32 flags);
void npc_circleDt(cM3dGCir_l* circle, u32 flags);
void npc_objAcchDt(dBgS_ObjAcch* acch, u32 flags);
void npc_actorDt(fopAc_ac_c* actor, u32 flags);
void npc_delete(void* object);
void npc_registerGlobal(npc_readable::GlobalObjectRecord* record);
void npc_initializeHeaderByte(be<u8>* object);
void npc_initializeHeaderFlag(be<u8>* object);
