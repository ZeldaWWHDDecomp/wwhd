/* Actor lifecycle helpers. Shared fopAc_ac_c layout stays unchanged; HD views are private here. */
#pragma once
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_pc/f_pc.h"
#include "gc_api/JSystem/JUTAssert.h"

struct fopAcM_prmBase_class {
    be<u32> parameters;
    cXyz position;
    csXyz angle;
    be<u16> setID;
};
WWHD_SIZE(fopAcM_prmBase_class, 0x18);
struct fopAcM_prmScale_class {
    be<u8> x, y, z;
};
WWHD_SIZE(fopAcM_prmScale_class, 3);
struct fopAcM_prm_class {
    fopAcM_prmBase_class base;
    fopAcM_prmScale_class scale;
    be<u8> gbaName;
    be<u32> parent_id;
    be<s8> argument;
    be<s8> room_no;
    u8 pad22[2];
};
WWHD_OFFSET(fopAcM_prm_class, scale, 0x18);
WWHD_OFFSET(fopAcM_prm_class, gbaName, 0x1B);
WWHD_OFFSET(fopAcM_prm_class, parent_id, 0x1C);
WWHD_OFFSET(fopAcM_prm_class, argument, 0x20);
WWHD_OFFSET(fopAcM_prm_class, room_no, 0x21);
WWHD_SIZE(fopAcM_prm_class, 0x24);

// HD: integer views preserve the GHS copies and NaN checks without loading positions as floats.
struct fopAc_placeWordsHD {
    be<u32> position[3];
    be<u32> angleAndRoom[2];
};
WWHD_OFFSET(fopAc_placeWordsHD, angleAndRoom, 0xC);
WWHD_SIZE(fopAc_placeWordsHD, sizeof(actor_place));
struct fopAc_vectorWordsHD {
    be<u32> position[3];
};
WWHD_SIZE(fopAc_vectorWordsHD, sizeof(cXyz));

// HD: the constructor copies this 0x44-byte default into three 0x84-byte light slots.
struct fopAc_lightBlockHD {
    be<f32> positionAndDirection[6];
    be<u8> color[4];
    be<u16> packedParameters[4];
    be<f32> factors[8];
};
WWHD_OFFSET(fopAc_lightBlockHD, color, 0x18);
WWHD_OFFSET(fopAc_lightBlockHD, packedParameters, 0x1C);
WWHD_OFFSET(fopAc_lightBlockHD, factors, 0x24);
WWHD_SIZE(fopAc_lightBlockHD, 0x44);
struct fopAc_lightSlotHD {
    fopAc_lightBlockHD mDefault;
    u8 pad44[0x84 - 0x44];
};
WWHD_SIZE(fopAc_lightSlotHD, 0x84);
struct fopAc_tevInitHD {
    fopAc_lightSlotHD mLightObj;
    u8 pad84[0xC0 - 0x84];
    fopAc_lightSlotHD mHdLightParam[2];
};
WWHD_OFFSET(fopAc_tevInitHD, mHdLightParam, 0xC0);
WWHD_SIZE(fopAc_tevInitHD, sizeof(dKy_tevstr_c));
struct fopAc_vtableHD {
    u8 pad0[0xC];
    be<u32> mDestructor;
};
WWHD_OFFSET(fopAc_vtableHD, mDestructor, 0xC);
WWHD_SIZE(fopAc_vtableHD, 0x10);

enum dEvt_Move_e { dEvtMove_NOMOVE_e = 0, dEvtMove_FORCE_e = 2 };
enum fopAc_constructStateHD { CLEARED_e, CONSTRUCTED_e, DESTROYED_e };
struct leafdraw_method_class;
/* fpcBs_Is_JustOfType: f_pc/f_pc_base.h */
s32 fpcBs_MakeOfType(be<s32>*);
BOOL dMenu_flag();
BOOL dScnPly_isPauseHD();
struct dScnPly_ply_c {
    // HD: free adapter for a static method, with no implicit this register.
    static BOOL isPause() { return dScnPly_isPauseHD(); }
};
s32 dEvt_control_moveApprovalHD(dEvt_control_c*, fopAc_ac_c*);
inline s32 dComIfGp_event_moveApproval(fopAc_ac_c* actor) {
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    return dEvt_control_moveApprovalHD(&game->play.mEvtCtrl, actor);
}
BOOL fopAcM_cullingCheck(fopAc_ac_c*);
s32 fpcLf_DrawMethod(leafdraw_method_class*, fopAc_ac_c*);
void dMap_drawActorPointMiniMap(fopAc_ac_c*);
u32 fopAcTg_Init(create_tag_class*, void*);
u32 fopAcTg_ToActorQ(create_tag_class*);
u32 fopAcTg_ActorQTo(create_tag_class*);
bool fopDwTg_Init(create_tag_class*, void*);
u32 fopDwTg_ToDrawQ(create_tag_class*, s32);
void fopDwTg_DrawQTo(create_tag_class*);
s32 fpcM_DrawPriority(fopAc_ac_c*);
void fopAcM_DeleteHeap(fopAc_ac_c*);
void dKy_tevstr_init(dKy_tevstr_c*, s8, u8);
void fopAc_baseProcessCtorHD(base_process_class*);
void fopAc_baseProcessDtorHD(base_process_class*, s32);
void dEvt_info_c_ctorHD(dEvt_info_c*);

struct dDemo_actor_c;
struct dDemo_object_c;
dDemo_actor_c* fopAc_demoGetActorHD(dDemo_object_c*, u8);
void fopAc_demoSetActorHD(dDemo_actor_c*, fopAc_ac_c*);
struct dDemo_object_c {
    dDemo_actor_c* getActor(u8 id) { return fopAc_demoGetActorHD(this, id); }
};
struct dDemo_actor_c {
    void setActor(fopAc_ac_c* actor) { fopAc_demoSetActorHD(this, actor); }
};
