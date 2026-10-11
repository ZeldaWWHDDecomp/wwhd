/* Warp departure effect: tww actor methods with verified HD event and particle semantics. */
#include "d/actor/d_a_warpfout.h"

// HD: table storage is GHS's eight-byte member-pointer representation. Dispatch preserves linkage.
static WarpfoutAction event_init_tbl[] = {{&daWarpfout_c::initWarp1},
                                          {&daWarpfout_c::initWarp2},
                                          {&daWarpfout_c::initWarp3},
                                          {&daWarpfout_c::initWarp4},
                                          {&daWarpfout_c::initEnd}};
static WarpfoutAction event_action_tbl[] = {{&daWarpfout_c::actWarp1},
                                            {&daWarpfout_c::actWarp2},
                                            {&daWarpfout_c::actWarp3},
                                            {&daWarpfout_c::actWarp4},
                                            {&daWarpfout_c::actEnd}};
static const char* action_table[] = {"WARP1", "WARP2", "WARP3", "WARP4", "END"};
static headerZeroStorage headerZero;
static headerPiStorage headerPi;
static headerObjectsStorage headerObjects;
static headerRecordsStorage headerRecords;
/* 024D8198 */
void daWarpfout_c::eventOrder() {}
/* 024D819C */
void daWarpfout_c::CreateInit() { unknownInitFlag = -1; }
/* 024D81A8 */
s32 daWarpfout_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this)) {
            Warpfout_actorConstruct(this);
            __vtbl = 0x10042474;
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}
/* 024D8210 */
s32 daWarpfout_Create(daWarpfout_c* a) { return a->_create(); }
/* 024D8214 */
s32 daWarpfout_Delete(daWarpfout_c* a) { return 1; }
/* 024D821C */
s32 daWarpfout_Draw(daWarpfout_c* a) { return 1; }
/* 024D8224 */
void daWarpfout_c::demo_proc() {
    WarpfoutGameInfo* game = Warpfout_getGame();
    mStaffId = Warpfout_getMyStaffId(&game->eventManager, "Warpfo", 0, 0);
    game = Warpfout_getGame();
    if (!game->eventControl.mMode || eventInfo.mCommand == 1)
        return;
    s32 staff = mStaffId;
    if (staff == -1)
        return;
    game = Warpfout_getGame();
    s32 action = Warpfout_getMyActIdx(&game->eventManager, staff, action_table, 5, 0, 0);
    staff = mStaffId;
    if (action == -1) {
        game = Warpfout_getGame();
        Warpfout_cutEnd(&game->eventManager, staff);
        return;
    }
    game = Warpfout_getGame();
    s32 advance = Warpfout_getIsAddvance(&game->eventManager, staff);
    if (advance)
        warpfoutDispatch(&event_init_tbl[action], this, mStaffId);
    s32 done = warpfoutDispatch(&event_action_tbl[action], this, mStaffId);
    if (done) {
        staff = mStaffId;
        game = Warpfout_getGame();
        Warpfout_cutEnd(&game->eventManager, staff);
    }
}

/* 024D83E8 */
s32 daWarpfout_c::_execute() {
    // HD: checkOrder/eventOrder were folded away; only demo_proc remains.
    demo_proc();
    return 1;
}
/* 024D840C */
s32 daWarpfout_Execute(daWarpfout_c* a) { return a->_execute(); }
/* 024D8410 */
u32 daWarpfout_c::initWarp1(s32 staff) {
    // HD: keep the sound call's full return register; tww declares initWarp1 void.
    mTimer = 5;
    return Warpfout_seStart(JA_SE_LK_BOSS_WARP_EFF_ED);
}
// HD: the small-structure ABI returns x/y in r3 and z/padding in r4.
/* 024D8420 */
u32 daWarpfout_c::get_effect_angle() {
    WarpfoutGameInfo* game = Warpfout_getGame();
    void* camera = game->camera;
    s16 x = Warpfout_cameraAngleX(camera);
    game = Warpfout_getGame();
    camera = game->camera;
    s16 y = Warpfout_cameraAngleY(camera);
    gabi::Local<WarpfoutAngle> angle;
    angle->components[2] = 0;
    u32 second = angle->words[1];
    angle->components[0] = u16(x);
    angle->components[1] = u16(u32(s32(y)) + 0x8000);
    warpfoutSetSecondResult(second);
    return angle->words[0];
}
static void warpfoutParticle(void* particle, u32 id, cXyz* pos, csXyz* angle) {
    Warpfout_particleSet(particle, 0, id, pos, angle, 0, 0xFF, 0, -1, 0, 0, 0);
}
/* 024D8480 */
void daWarpfout_c::set_effect_wind01(cXyz* position, s16 offset) {
    u32 xy = get_effect_angle();
    u32 second = warpfoutSecondResult();
    gabi::Local<WarpfoutAngle> angle;
    angle->words[1] = second;
    s16 z = angle->components[2];
    angle->words[0] = xy;
    angle->components[2] = u16(s32(z) + s32(offset));
    WarpfoutGameInfo* game = Warpfout_getGame();
    void* particle = game->particle;
    warpfoutParticle(particle, ID_AK_SN_DEMOWARPWIND01, position, &angle->rotation);
}
/* inlined into initWarp2, initWarp3 and actWarp1 */
static void warpfoutRaisedEffect(daWarpfout_c* a, f32 height, s16 angle) {
    WarpfoutGameInfo* game = Warpfout_getGame();
    fopAc_ac_c* player = game->link;
    f32 y = player->current.pos.y;
    f32 x = player->current.pos.x;
    y = y + height;
    f32 z = player->current.pos.z;
    gabi::Local<cXyz> position;
    position->x = x;
    position->y = y;
    position->z = z;
    a->set_effect_wind01(position, angle);
}
/* 024D850C */
void daWarpfout_c::initWarp2(s32 staff) {
    warpfoutRaisedEffect(this, 60.0f, 0x38E);
    mTimer = 5;
}
/* 024D8578 */
void daWarpfout_c::initWarp3(s32 staff) {
    warpfoutRaisedEffect(this, 80.0f, 0x71C);
    mTimer = 5;
}
/* 024D85E4 */
void daWarpfout_c::initWarp4(s32 staff) {
    WarpfoutGameInfo* game = Warpfout_getGame();
    fopAc_ac_c* player = game->link;
    for (s32 count = 6; count; --count) {
        game = Warpfout_getGame();
        void* particle = game->particle;
        warpfoutParticle(particle, ID_AK_SN_DEMOWARPWIND00, &player->current.pos, nullptr);
    }
}
/* 024D8664 */
s32 daWarpfout_c::actWarp1(s32 staff) {
    if (s32(mTimer) == 4)
        warpfoutRaisedEffect(this, 40.0f, 0);
    return Warpfout_calcTimer(&mTimer) == 0;
}
/* 024D86E4 */
s32 daWarpfout_c::actWarp2(s32 staff) { return Warpfout_calcTimer(&mTimer) == 0; }
/* 024D8710 */
s32 daWarpfout_c::actWarp3(s32 staff) { return Warpfout_calcTimer(&mTimer) == 0; }
/* 024D873C */
s32 daWarpfout_c::actEnd(s32 staff) {
    Warpfout_deleteActor(this);
    return 1;
}
// HD: compiler-generated header statics, with GHS's zero-before-construction order.
/* 024D8760 */
void __sinit_d_a_warpfout_cpp() {
    headerZero[2] = 0;
    headerZero[0] = 0;
    headerZero[3] = 0;
    headerZero[1] = 0;
    Warpfout_registerDestructor(&headerRecords[0]);
    headerPi[0] = -3.1415927410125732f;
    headerPi[1] = 3.1415927410125732f;
    Warpfout_headerConstructA(&headerObjects[0]);
    Warpfout_registerDestructor(&headerRecords[1]);
    Warpfout_headerConstructB(&headerObjects[1]);
    Warpfout_registerDestructor(&headerRecords[2]);
}
/* 024D87F4 */
s32 daWarpfout_IsDelete(daWarpfout_c* a) { return 1; }
/* 024D87FC */
s32 daWarpfout_c::actWarp4(s32 staff) { return 1; }
/* 024D8804 */
void daWarpfout_c::initEnd(s32 staff) {}
/* 024D8808 */
void daWarpfout_c_dtor(daWarpfout_c* a, s32 flags) {
    if (a) {
        Warpfout_actorDestroy(a, 0);
        if (u32(flags) & 1)
            Warpfout_free(a);
    }
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
str 10042484
data event_init_tbl 101D2F44
data event_action_tbl 101D2F6C
data action_table 101D2FB4
data headerZero 1046EB38
data headerPi 1046EB2C
data headerObjects 1046EB34
data headerRecords 101D2FC8
*/
// clang-format on
