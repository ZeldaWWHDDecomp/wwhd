/* Actor process lifecycle, tww names and verified WWHD behavior. Local only.
 */
// HD: keep the pre-existing generic binding separate from this unit's typed definition.
#define fopAc_IsActor fopAc_IsActorBindingHD
#include "f_op/f_op_actor_readable.h"
#undef fopAc_IsActor

using ActorType = be<s32>;
using ActorStatus = be<u32>;
using StayRoom = be<s8>;
using DemoObject = gptr<dDemo_object_c>;
static ActorType g_fopAc_type;
// HD: a TU view of the GameCube fopAc_ac_c::stopStatus static, without changing the shared class.
static ActorStatus stopStatus;
static StayRoom mStayNo;
static DemoObject m_object;
static const fopAc_lightBlockHD l_defaultLight;
static fopAc_vtableHD l_actorVtable;

/* 025D4604 */
BOOL fopAc_IsActor(fopAc_ac_c* pProc) {
    // HD: unlike the GameCube release, the actor check accepts a null process.
    return pProc && fpcBs_Is_JustOfType(g_fopAc_type, pProc->actor_type) ? TRUE : FALSE;
}

/* 025D4654 */
s32 fopAc_Draw(fopAc_ac_c* actor) {
    s32 ret = TRUE;
    if (!dMenu_flag()) {
        s32 moveApproval = dComIfGp_event_moveApproval(actor);
        bool draw;
        if (moveApproval == dEvtMove_FORCE_e) {
            draw = !(actor->actor_status & fopAcStts_NODRAW_e);
        } else {
            u32 status = actor->actor_status;
            if (status & stopStatus)
                draw = false;
            else if ((status & fopAcStts_CULL_e) && fopAcM_cullingCheck(actor))
                draw = false;
            else
                draw = !(actor->actor_status & fopAcStts_NODRAW_e);
        }
        u32 status;
        if (draw) {
            actor->actor_condition &= ~u32(fopAcCnd_NODRAW_e);
            ret = fpcLf_DrawMethod(gabi::at<leafdraw_method_class>(actor->sub_method), actor);
            status = actor->actor_status & ~u32(fopAcStts_NODRAW_e);
            actor->actor_status = status;
        } else {
            u32 condition = actor->actor_condition;
            status = actor->actor_status & ~u32(fopAcStts_NODRAW_e);
            actor->actor_condition = condition | fopAcCnd_NODRAW_e;
            actor->actor_status = status;
        }
        if (mStayNo >= 0 && (status & fopAcStts_SHOWMAP_e))
            dMap_drawActorPointMiniMap(actor);
    }
    return ret;
}

static bool positionIsNaN(u32 bits) { return u32(bits << 1) > 0xFF000000u; }

/* inlined into fopAc_Execute */
static void checkPosition(fopAc_ac_c* actor, s32 line, s32 rangeLine) {
    // HD: the assertion uses integer NaN tests before its ordered float range checks.
    auto* words = reinterpret_cast<fopAc_vectorWordsHD*>(&actor->current.pos);
    if (positionIsNaN(words->position[0]))
        JUTAssertion_showAssert("f_op_actor.cpp", line, "!isnan(actor->current.pos.x)");
    if (positionIsNaN(words->position[1]))
        JUTAssertion_showAssert("f_op_actor.cpp", line + 1, "!isnan(actor->current.pos.y)");
    if (positionIsNaN(words->position[2]))
        JUTAssertion_showAssert("f_op_actor.cpp", line + 2, "!isnan(actor->current.pos.z)");
    const f32 lo = -1.0e32f, hi = 1.0e32f;
    f32 x = actor->current.pos.x;
    bool ok = lo < x && x < hi;
    if (ok) {
        f32 y = actor->current.pos.y;
        ok = lo < y && y < hi;
        if (ok) {
            f32 z = actor->current.pos.z;
            ok = lo < z && z < hi;
        }
    }
    if (!ok)
        JUTAssertion_showAssert(
            "f_op_actor.cpp", rangeLine,
            "-1.0e32f < actor->current.pos.x && actor->current.pos.x < 1.0e32f && -1.0e32f < "
            "actor->current.pos.y && actor->current.pos.y < 1.0e32f && -1.0e32f < "
            "actor->current.pos.z && actor->current.pos.z < 1.0e32f");
}

/* 025D475C */
BOOL fopAc_Execute(fopAc_ac_c* actor) {
    BOOL ret = TRUE;
    checkPosition(actor, 0x2A4, 0x2A8);
    if (!(actor->actor_status & fopAcStts_NOPAUSE_e)) {
        if (dMenu_flag())
            return ret;
        if (dScnPly_ply_c::isPause())
            return ret;
    }
    actor->eventInfo.mCondition = 0;
    s32 moveApproval = dComIfGp_event_moveApproval(actor);
    bool exec;
    if (moveApproval == dEvtMove_FORCE_e)
        exec = true;
    else if (moveApproval == dEvtMove_NOMOVE_e || (actor->actor_status & stopStatus))
        exec = false;
    else if (actor->actor_status & fopAcStts_NOCULLEXEC_e)
        exec = !(actor->actor_condition & fopAcCnd_NODRAW_e);
    else
        exec = true;
    if (exec) {
        // HD: old = current is a five-word copy, including the room byte and final padding.
        auto* old = reinterpret_cast<fopAc_placeWordsHD*>(&actor->old);
        auto* current = reinterpret_cast<fopAc_placeWordsHD*>(&actor->current);
        for (u32 i = 0; i < 3; ++i)
            old->position[i] = current->position[i];
        for (u32 i = 0; i < 2; ++i)
            old->angleAndRoom[i] = current->angleAndRoom[i];
        actor->actor_condition &= ~u32(fopAcCnd_NOEXEC_e);
        ret = fpcMtd_Execute(gabi::at<process_method_class>(actor->sub_method), actor);
        // HD: enemies below the player by 15000 are deleted, unless the BOSS status suppresses it.
        if (actor->group == fopAc_ENEMY_e && !(actor->actor_status & fopAcStts_BOSS_e)) {
            auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            fopAc_ac_c* player = game->play.mpPlayerPtr[0];
            if (player && actor->current.pos.y < player->current.pos.y - 15000.0f)
                fopAcM_delete(actor);
        }
    } else {
        actor->actor_condition |= fopAcCnd_NOEXEC_e;
    }
    checkPosition(actor, 0x2EA, 0x2EE);
    return ret;
}

/* 025D4AF8 */
s32 fopAc_IsDelete(fopAc_ac_c* actor) {
    s32 ret = fpcMtd_IsDelete(gabi::at<process_method_class>(actor->sub_method), actor);
    if (ret == TRUE)
        fopDwTg_DrawQTo(&actor->draw_tag);
    return ret;
}

/* 025D4B4C */
s32 fopAc_Delete(fopAc_ac_c* actor) {
    // HD: constructed actors run their virtual destructor before removal from the queues.
    JUT_ASSERT(0x376, actor->mHdConstructState == CONSTRUCTED_e);
    s32 ret = fpcMtd_Delete(gabi::at<process_method_class>(actor->sub_method), actor);
    if (ret == TRUE) {
        if (actor->mHdConstructState == CONSTRUCTED_e) {
            auto* vtable = gabi::at<fopAc_vtableHD>(actor->__vtbl);
            gabi::call_ptr(vtable->mDestructor, actor, 2);
        }
        fopAcTg_ActorQTo(&actor->actor_tag);
        fopDwTg_DrawQTo(&actor->draw_tag);
        fopAcM_DeleteHeap(actor);
        u8 id = actor->demoActorID;
        if (id != 0 && id <= 0x20) {
            dDemo_object_c* demo = m_object;
            if (!demo) {
                JUTAssertion_showAssert("d_demo.h", 0x23A, "m_object != (0)");
                demo = m_object;
            }
            dDemo_actor_c* pDemoActor = demo->getActor(id);
            if (pDemoActor)
                pDemoActor->setActor(nullptr);
        }
        mDoAud_seDeleteObject(&actor->eyePos);
        mDoAud_seDeleteObject(&actor->current.pos);
    }
    return ret;
}

/* 025D4C70 */
s32 fopAc_Create(fopAc_ac_c* actor) {
    if (actor->mInitState == 0) {
        auto* profile = reinterpret_cast<actor_process_profile_definition*>(actor->mpProf.get());
        actor->actor_type = fpcBs_MakeOfType(&g_fopAc_type);
        actor->sub_method = profile->sub_method;
        fopAcTg_Init(&actor->actor_tag, actor);
        fopAcTg_ToActorQ(&actor->actor_tag);
        fopDwTg_Init(&actor->draw_tag, actor);
        actor->actor_status = profile->status;
        actor->group = profile->group;
        actor->cullType = profile->cullType;
        auto* prm = static_cast<fopAcM_prm_class*>(actor->mpUserData.get());
        if (prm) {
            actor->mParameters = prm->base.parameters;
            actor->home.pos = prm->base.position;
            actor->home.angle = prm->base.angle;
            actor->shape_angle = prm->base.angle;
            actor->parentActorID = prm->parent_id;
            actor->argument = prm->argument;
            actor->gbaName = prm->gbaName;
            f32 sx = prm->scale.x, sy = prm->scale.y, sz = prm->scale.z;
            actor->scale.set(sx * 0.1f, sy * 0.1f, sz * 0.1f);
            actor->setID = prm->base.setID;
            actor->home.roomNo = prm->room_no;
        }
        // HD: snapshot all five home words before the old/current/eye/attention copies.
        auto* home = reinterpret_cast<fopAc_placeWordsHD*>(&actor->home);
        u32 px = home->position[0], py = home->position[1], pz = home->position[2];
        u32 angleXY = home->angleAndRoom[0], angleZRoom = home->angleAndRoom[1];
        auto* old = reinterpret_cast<fopAc_placeWordsHD*>(&actor->old);
        old->position[0] = px;
        old->position[1] = py;
        old->position[2] = pz;
        old->angleAndRoom[0] = angleXY;
        old->angleAndRoom[1] = angleZRoom;
        auto* current = reinterpret_cast<fopAc_placeWordsHD*>(&actor->current);
        current->position[0] = px;
        current->position[1] = py;
        current->position[2] = pz;
        current->angleAndRoom[0] = angleXY;
        current->angleAndRoom[1] = angleZRoom;
        auto* eye = reinterpret_cast<fopAc_vectorWordsHD*>(&actor->eyePos);
        eye->position[0] = px;
        eye->position[1] = py;
        eye->position[2] = pz;
        actor->maxFallSpeed = -100.0f;
        actor->attention_info.distances[fopAc_Attn_TYPE_MISC_e] = 1;
        actor->attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 2;
        actor->attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 3;
        actor->attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 7;
        actor->attention_info.distances[fopAc_Attn_TYPE_CARRY_e] = 8;
        actor->attention_info.distances[fopAc_Attn_TYPE_DOOR_e] = 16;
        actor->attention_info.distances[fopAc_Attn_TYPE_TREASURE_e] = 16;
        actor->attention_info.distances[fopAc_Attn_TYPE_SHIP_e] = 15;
        auto* attention = reinterpret_cast<fopAc_vectorWordsHD*>(&actor->attention_info.position);
        attention->position[0] = px;
        attention->position[1] = py;
        attention->position[2] = pz;
        dKy_tevstr_init(&actor->tevStr, actor->home.roomNo, 0xFF);
    }
    s32 status = fpcMtd_Create(gabi::at<process_method_class>(actor->sub_method), actor);
    if (status == cPhs_COMPLEATE_e) {
        s32 priority = fpcM_DrawPriority(actor);
        fopDwTg_ToDrawQ(&actor->draw_tag, priority);
    }
    return status;
}

// HD: compiler-generated constructor/destructor and common-header initialization.
/* 025D4ED0 */
u32 fopAc_ac_c__ct(fopAc_ac_c* self) {
    if (!self) {
        self = static_cast<fopAc_ac_c*>(operator_new(sizeof(fopAc_ac_c)));
        if (!self)
            return 0;
    }
    fopAc_baseProcessCtorHD(self);
    self->__vtbl = gabi::ea(&l_actorVtable);
    dEvt_info_c_ctorHD(&self->eventInfo);
    // HD: read the entire default block before writing any light, including possible aliases.
    f32 f[6];
    for (u32 i = 0; i < 6; ++i)
        f[i] = l_defaultLight.positionAndDirection[i];
    u8 b[4];
    for (u32 i = 0; i < 4; ++i)
        b[i] = l_defaultLight.color[i];
    u16 h[4];
    for (u32 i = 0; i < 4; ++i)
        h[i] = l_defaultLight.packedParameters[i];
    f32 g[8];
    for (u32 i = 0; i < 8; ++i)
        g[i] = l_defaultLight.factors[i];
    auto* tev = reinterpret_cast<fopAc_tevInitHD*>(&self->tevStr);
    for (u32 k = 0; k < 3; ++k) {
        auto* light = k == 0 ? &tev->mLightObj.mDefault : &tev->mHdLightParam[k - 1].mDefault;
        for (u32 i = 0; i < 6; ++i)
            light->positionAndDirection[i] = f[i];
        for (u32 i = 0; i < 4; ++i)
            light->color[i] = b[i];
        for (u32 i = 0; i < 4; ++i)
            light->packedParameters[i] = h[i];
        for (u32 i = 0; i < 8; ++i)
            light->factors[i] = g[i];
    }
    JUT_ASSERT(0x14F, self->mHdConstructState == CLEARED_e);
    self->mHdConstructState = CONSTRUCTED_e;
    return gabi::ea(self);
}

/* 025D50BC */
void fopAc_ac_c__dt(fopAc_ac_c* self, s32 flags) {
    if (!self)
        return;
    u8 state = self->mHdConstructState;
    self->__vtbl = gabi::ea(&l_actorVtable);
    JUT_ASSERT(0x157, state == CONSTRUCTED_e);
    self->mHdConstructState = DESTROYED_e;
    fopAc_baseProcessDtorHD(self, 0);
    if (flags & 1)
        operator_delete(self);
}

/* 025D5148 */
static void __sinit_f_op_actor_cpp() { sinit_header_statics(0x10487424, 0x101F305C); }

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data g_fopAc_type 101F3080
data stopStatus 101F3084
data mStayNo 1047E6C8
data m_object 101D5FFC
data l_defaultLight 1016E414
data l_actorVtable 10057138
str 10056E7C "f_op_actor.cpp" in 025D475C
str 1005706C "f_op_actor.cpp" in 025D4B4C
str 100570E4 "f_op_actor.cpp" in 025D4ED0
str 10057118 "f_op_actor.cpp" in 025D50BC
str 10056E8C "!isnan(actor->current.pos.x)" renamed
str 10056EE8 "!isnan(actor->current.pos.y)" renamed
str 10056F44 "!isnan(actor->current.pos.z)" renamed
str 10056FA0
str 1005707C "actor->mHdConstructState == CONSTRUCTED_e" renamed
str 10056E64 "d_demo.h"
str 10056E54 "m_object != (0)"
str 100570C4 "self->mHdConstructState == CLEARED_e" renamed
str 100570F4 "state == CONSTRUCTED_e" renamed
ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
ext 025DD268 s32 fpcBs_MakeOfType(be<s32>*)
ext 025986BC BOOL dMenu_flag()
ext 025AF2A4 BOOL dScnPly_isPauseHD()
ext 02540158 s32 dEvt_control_moveApprovalHD(dEvt_control_c*, fopAc_ac_c*)
ext 025D6CE8 BOOL fopAcM_cullingCheck(fopAc_ac_c*)
ext 025DF2C0 s32 fpcLf_DrawMethod(leafdraw_method_class*, fopAc_ac_c*)
ext 0259029C void dMap_drawActorPointMiniMap(fopAc_ac_c*)
ext 025DA30C u32 fopAcTg_Init(create_tag_class*, void*)
ext 025DA2F8 u32 fopAcTg_ToActorQ(create_tag_class*)
ext 025DA308 u32 fopAcTg_ActorQTo(create_tag_class*)
ext 025DA888 bool fopDwTg_Init(create_tag_class*, void*)
ext 025DA874 u32 fopDwTg_ToDrawQ(create_tag_class*, s32)
ext 025DA884 void fopDwTg_DrawQTo(create_tag_class*)
ext 025DF2B8 s32 fpcM_DrawPriority(fopAc_ac_c*)
ext 025D6134 void fopAcM_DeleteHeap(fopAc_ac_c*)
ext 02526E70 dDemo_actor_c* fopAc_demoGetActorHD(dDemo_object_c*, u8)
ext 025277D8 void fopAc_demoSetActorHD(dDemo_actor_c*, fopAc_ac_c*)
ext 0255FFF4 void dKy_tevstr_init(dKy_tevstr_c*, s8, u8)
ext 025DD5F0 void fopAc_baseProcessCtorHD(base_process_class*)
ext 025DD630 void fopAc_baseProcessDtorHD(base_process_class*, s32)
ext 0253E948 void dEvt_info_c_ctorHD(dEvt_info_c*)
*/
// clang-format on
