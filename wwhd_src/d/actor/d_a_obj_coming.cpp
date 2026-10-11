/* Floating barrels around the ship, in the GameCube decompilation's layout.
 * WWHD addresses and guest-call boundaries are verified against the original. */
#include "d/actor/d_a_obj_coming.h"
using namespace daObjComing;
enum ComingProcessHD { fpcNm_SHIP_e = 0xA5, fpcNm_PLAYER_e = 0xA8, fpcNm_Obj_Barrel2_e = 0x1C9 };

const gc_tu_data gc_tu = {
    .gndChk = {0x10026C90, 0x10026CA0, 0x10026CC0, 0x10026CB0},
    .gndChkDt = {0x10026C60, 0x10026C80, 0x10026C40},
    .headerStatics = {0x104694AC, 0x101C89C0, 0x104694B8},
};
namespace {
struct ComingVtables {
    u32 actor;
    dBgS_LinChk_vt line;
    u32 lineDt[3], waterDt[3];
};
static const ComingVtables l_vtables = {
    0x10026DB0,
    {0x10026D70, 0x10026D80, 0x10026DA0, 0x10026D90},
    {0x10026D60, 0x10026C40, 0x10026C30},
    {0x10026CD0, 0x10026CF0, 0x10026C40},
};
struct Attr_c { be<s16> m00[16]; be<f32> L_Hio[6]; };
WWHD_SIZE(Attr_c, 0x38);
WWHD_OFFSET(Attr_c, L_Hio, 0x20);
static const Attr_c L_Attr = {
    {2,3,4,5,20,60,40,80,80,80,100,100,30,30,300,30},
    {25.0f,5000.0f,6000.0f,5000.0f,0.05f,0.05f}
};
static const u8 M_item_tbl[7][16] = {
    {
        0x03,
        0x03,
        0x03,
        0x0E,
        0x0E,
        0x0A,
        0x0A,
        0x04,
        0x12,
        0x12,
        0x05,
        0x06,
        0x07,
        0x07,
        0x07,
        0x07,
    },
    {
        0x03,
        0x03,
        0x03,
        0x03,
        0x04,
        0x04,
        0x0A,
        0x0A,
        0x04,
        0x04,
        0x05,
        0x05,
        0x05,
        0x06,
        0x07,
        0x07,
    },
    {
        0x02,
        0x02,
        0x02,
        0x03,
        0x03,
        0x03,
        0x03,
        0x04,
        0x04,
        0x04,
        0x04,
        0x05,
        0x05,
        0x06,
        0x07,
        0x07,
    },
    {
        0x02,
        0x02,
        0x02,
        0x02,
        0x02,
        0x02,
        0x02,
        0x02,
        0x01,
        0x01,
        0x01,
        0x01,
        0x01,
        0x05,
        0x07,
        0x07,
    },
    {
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0x0B,
        0x0B,
        0x0B,
        0x0D,
        0x07,
        0x07,
    },
    {
        0x0E,
        0x0E,
        0x0E,
        0x0E,
        0x0E,
        0x0E,
        0x0F,
        0x0F,
        0x0F,
        0x0F,
        0x0F,
        0x10,
        0x11,
        0x11,
        0x07,
        0x07,
    },
    {
        0x14,
        0x14,
        0x13,
        0x13,
        0x12,
        0x12,
        0x12,
        0x12,
        0x08,
        0x08,
        0x07,
        0x07,
        0x07,
        0x07,
        0x07,
        0x07,
    },
};
using ModeProc = void (Act_c::*)(Ctrl_c*);
struct ModeProc_c { gabi::ptmf<ModeProc> fn; };
WWHD_SIZE(ModeProc_c, 8);
static const ModeProc_c mode_proc[1][3] = {{
    {&Act_c::mode_barrel2_wait}, {&Act_c::mode_barrel2_appear}, {&Act_c::mode_barrel2_leave}
}};
static const u32 barrel_type[] = {0,1,2};
static const u8 type_data[] = {1,0,2,1,1,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,0};
static const u8 item_data[] = {255,255,255,1,2,3,4,0,0,11,12,13,14,14,16,17,18,18,9,10,10};
static const u8 buoy_data[] = {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// HD: BSS objects have no image initializers; their constructor stores are verified below.
static dBgS_ObjGndChk M_gnd_work;
static dBgS_ObjLinChk M_wall_work;
static ComingWaterChk l_water;
static u32 l_waterInitialized;
struct HeaderStatics { be<f32> negativePi, positivePi; be<u8> angle, flag; u8 padding[2]; be<u32> zero[4]; };
WWHD_SIZE(HeaderStatics, 0x1C);
WWHD_OFFSET(HeaderStatics, positivePi, 4);
WWHD_OFFSET(HeaderStatics, angle, 8);
WWHD_OFFSET(HeaderStatics, flag, 9);
WWHD_OFFSET(HeaderStatics, zero, 0x0C);
static HeaderStatics l_header;
// ABI addresses of static destructor records and common matrix/vector storage.
inline void* Coming_record(u32 offset) { return gabi::at<void>(0x101C89B4 + offset); }
inline Mtx34* Coming_matrix() { return gabi::at<Mtx34>(0x1048D0CC); }
inline const cXyz* Coming_zero() { return gabi::at<cXyz>(0x101FFBA8); }
inline void* Coming_saveInfo() { return gabi::at<void>(dComIfGs_ea() + 0x20); }
inline ComingBarrel2* Coming_barrel(u32 id) { return gabi::at<ComingBarrel2>(id); }

} // namespace

/* 02331A08 */
void Ctrl_c::init(const Act_c* actor) {
    Coming_initTimer(this, actor);
    m00 = MODE_WAIT;
    m08 = fpcM_ERROR_PROCESS_ID_e;
}

/* 0233194C */
void Ctrl_c::init_timer(const Act_c* actor) {
    u32 waitTime = Coming_PrmAbstract(actor, Act_c::PRM_WAIT_TIME_W, Act_c::PRM_WAIT_TIME_S);
    if (waitTime >= 4)
        Coming_assert("d_a_obj_coming.cpp", 0x104, "(n >= 0) && (n < L_wait_max)");
    s32 total = (s32)L_Attr.m00[4 + waitTime * 2] + (s32)L_Attr.m00[5 + waitTime * 2];
    m04 = gabi::ftoi(Coming_rndF((f32)total));
}

/* 02331A44 */
void Ctrl_c::set_wait_timer(const Act_c* actor) {
    u32 waitTime = Coming_PrmAbstract(actor, Act_c::PRM_WAIT_TIME_W, Act_c::PRM_WAIT_TIME_S);
    if (waitTime >= 4)
        Coming_assert("d_a_obj_coming.cpp", 0x111, "(n >= 0) && (n < L_wait_max)");
    f32 range = (f32)L_Attr.m00[5 + waitTime * 2];
    s32 base = L_Attr.m00[4 + waitTime * 2];
    s32 random = gabi::ftoi(Coming_rndF(range));
    m04 = (u32)base + (u32)random;
}

/* 02331B00 */
void Ctrl_c::set_retry_timer(const Act_c*) {
    s32 random = gabi::ftoi(Coming_rndF(30.0f));
    m04 = (u32)random + 30;
}

/* 02331B4C */
void Ctrl_c::set_appear_timer(const Act_c*) {
    s32 random = gabi::ftoi(Coming_rndF(30.0f));
    m04 = (u32)random + 300;
}

/* 02331B98 */
s32 Act_c::_create() {
    // HD: GHS constructs the base conditionally and installs this TU's vtable.
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            Coming_actorCtor(this);
            condition = actor_condition;
            __vtbl = l_vtables.actor;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    m290 = Coming_PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
    Coming_disableDraw(&draw_tag);
    for (int i = 0; i < 5; ++i)
        Coming_init(&m294[i], this);
    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
    y += 500.0f;
    M_gnd_work.m_pos.x = x;
    M_gnd_work.m_pos.z = z;
    M_gnd_work.m_pos.y = y;
    dComIfG_Bgsp()->GroundCross(&M_gnd_work);
    u32 room = Coming_getRoomId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&M_gnd_work));
    mRoomId = room;
    return cPhs_COMPLEATE_e;
}

/* 02331DD8 */
u32 Act_c::chk_switch() {
    u32 sw = Coming_PrmAbstract(this, PRM_SAVE_W, PRM_SAVE_S);
    if (sw == 255)
        return 1;
    s8 room = home.roomNo; // HD: reads the room in the home placement.
    return Coming_isSwitch(Coming_saveInfo(), sw, room) != 0;
}

// HD: GC static helpers are free functions here, retaining their guest argument lists.
/* 02331E50 */
static f32 get_aqua_y(const cXyz* pos) {
    f32 z = pos->z, x = pos->x;
    f32 water = -1000000000.0f;
    if (Coming_seaCheck(x, z)) {
        x = pos->x;
        z = pos->z;
        water = Coming_waveY(x, z);
    } else {
        if (!l_waterInitialized) {
            // HD: the guard is set before construction, as in the original.
            l_waterInitialized = 1;
            Coming_waterCtor(&l_water);
            Coming_registerDestructor(Coming_record(0));
        }
        x = pos->x;
        f32 y = pos->y;
        z = pos->z;
        f32 lower = y - 1000.0f;
        l_water.mPos.x = x;
        y += 1000.0f;
        l_water.mPos.z = z;
        l_water.mPos.y = lower;
        l_water.mUpperY = y;
        if (Coming_waterCheck(dComIfG_Bgsp(), &l_water))
            water = l_water.mHeight;
    }
    return water;
}

/* 02331F48 */
u32 Act_c::chk_ship_spd(const fopAc_ac_c* ship) {
    return ship->speedF > 25.0f;
}

/* 02331F64 */
u32 Act_c::chk_ship_range(const fopAc_ac_c* ship) {
    if (Coming_PrmAbstract(this, PRM_RANGE_W, PRM_RANGE_S))
        return 1;
    cXyz shipPos;
    cXyz actorPos;
    f32 sz = ship->current.pos.z, az = current.pos.z, ax = current.pos.x;
    shipPos.z = sz;
    actorPos.z = az;
    actorPos.y = 0;
    actorPos.x = ax;
    f32 sx = ship->current.pos.x;
    shipPos.y = 0;
    shipPos.x = sx;
    f32 distance = Coming_squareDistance(&shipPos, &actorPos);
    distance = Coming_sqrt(distance);
    f32 limit = scale.x * 10000.0f;
    return distance < limit;
}

/* 02332024 */
void Act_c::get_make_pos(const fopAc_ac_c* ship, cXyz* out) {
    f32 x = ship->current.pos.x, y = ship->current.pos.y, z = ship->current.pos.z;
    Coming_mtxTrans(Coming_matrix(), x, y, z);
    Coming_mtxYrot(Coming_matrix(), ship->shape_angle.y);
    f32 forward = Coming_rndF(6000.0f) + 5000.0f;
    f32 lateral = (Coming_rnd() - 0.5f) * 5000.0f;
    Coming_transM(lateral, 0, forward);
    cXyz pos;
    Coming_multVec(Coming_matrix(), Coming_zero(), &pos);
    x = pos.x;
    z = pos.z;
    out->x = x;
    y = Coming_waveY(x, z);
    z = pos.z;
    out->y = y;
    out->z = z;
}

/* 0233211C */
u32 Act_c::chk_make_pos_gnd(const MakeTmp_c* tmp) {
    f32 end = tmp->m18 + 100.0f, distance = 0;
    s16 angle = 0;
    if (!(distance < end))
        return 1;
    do {
        f32 x = tmp->m0C.x, y = tmp->m0C.y, z = tmp->m0C.z;
        Coming_mtxTrans(Coming_matrix(), x, y, z);
        Coming_mtxYrot(Coming_matrix(), angle);
        Coming_transM(0, 0, distance);
        cXyz pos;
        Coming_multVec(Coming_matrix(), Coming_zero(), &pos);
        f32 water = Coming_waterY(&pos);
        if (!(water > -1000000000.0f))
            return 0;
        y = pos.y;
        z = pos.z;
        x = pos.x;
        y += 20000.0f;
        M_gnd_work.m_pos.z = z;
        M_gnd_work.m_pos.x = x;
        M_gnd_work.m_pos.y = y;
        pos.y = y;
        f32 floor = dComIfG_Bgsp()->GroundCross(&M_gnd_work);
        if (floor > water - 100.0f)
            return 0;
        distance += 20.0f;
        angle = (s16)(angle + 20000);
    } while (distance < end);
    return 1;
}

/* 023322EC */
u32 Act_c::chk_make_pos_wall(const MakeTmp_c* tmp) {
    Coming_setLine(&M_wall_work, &tmp->m00, &tmp->m0C, 0);
    // HD: the full callee register is XORed before truncation, rather than boolean negation.
    return (u8)(Coming_lineCross(dComIfG_Bgsp(), &M_wall_work) ^ 1);
}

/* 0233233C */
static void* chk_make_pos_act(void* i_actor, void* i_tmp) {
    fopAc_ac_c* actor = static_cast<fopAc_ac_c*>(i_actor);
    const Act_c::MakeTmp_c* tmp = static_cast<const Act_c::MakeTmp_c*>(i_tmp);
    if (!Coming_isActor(actor))
        return nullptr;
    if (actor && actor->mProcName == fpcNm_SHIP_e)
        return nullptr;
    if (actor && actor->mProcName == fpcNm_PLAYER_e)
        return nullptr;
    f32 y = actor->current.pos.y, ty = tmp->m0C.y, height = tmp->m1C;
    f32 difference = __builtin_fabsf(y - ty);
    height += 100.0f;
    if (!(difference < height))
        return nullptr;
    cXyz actorPos;
    cXyz tmpPos;
    actorPos.x = actor->current.pos.x;
    actorPos.z = actor->current.pos.z;
    actorPos.y = 0;
    tmpPos.z = tmp->m0C.z;
    tmpPos.x = tmp->m0C.x;
    tmpPos.y = 0;
    f32 distance = Coming_squareDistance(&actorPos, &tmpPos);
    distance = Coming_sqrt(distance);
    f32 radius = tmp->m18 + 100.0f;
    if (!(distance < radius))
        return nullptr;
    return actor;
}

/* 02332468 */
u32 Act_c::chk_make_pos(const MakeTmp_c* tmp) {
    if (!Coming_checkGround(this, tmp))
        return 0;
    if (!Coming_checkWall(this, tmp))
        return 0;
    return Coming_searchActor(gabi::guest_fn((const void*)&chk_make_pos_act), const_cast<MakeTmp_c*>(tmp)) == 0;
}

/* 023324E8 */
u32 Act_c::barrel2_get_ptn() {
    f32 random = Coming_rnd();
    if (random < 0.05f)
        return PTN_01_e;
    if (random < 0.1f)
        return PTN_00_e;
    s32 pick = gabi::ftoi(Coming_rndF(16.0f));
    // HD: signed table selection retains the full guest return before the upper-bound check.
    s32 table = (s32)Coming_PrmAbstract(this, PRM_TBL_NUM_W, PRM_TBL_NUM_S);
    if (table >= 7)
        table = 0;
    return M_item_tbl[(u32)table][(u32)pick];
}

/* 02332590 */
static u32 barrel2_get_type(u32 pattern) {
    u32 index = type_data[pattern];
    return barrel_type[index];
}

/* 023325A8 */
static u32 barrel2_get_item(u32 pattern) {
    return item_data[pattern] & 63;
}

/* 023325B8 */
static u32 barrel2_get_buoy(u32 pattern) {
    return buoy_data[pattern];
}

/* 023325C4 */
u32 Act_c::barrel2_get_param(PrmBarrel2_c* out) {
    fopAc_ac_c* ship = dComIfGp_getShipActor();
    if (!ship)
        return 0;
    if (!Coming_shipSpeed(this, ship))
        return 0;
    if (!Coming_shipRange(this, ship))
        return 0;
    cXyz origin;
    origin.x = ship->current.pos.x;
    origin.y = ship->current.pos.y;
    origin.z = ship->current.pos.z;
    cXyz pos;
    Coming_makePosition(this, ship, &pos);
    MakeTmp_c tmp;
    // HD: these are float copies, with separate origin/position guest stack slots.
    tmp.m00.x = origin.x;
    tmp.m00.y = origin.y;
    tmp.m00.z = origin.z;
    tmp.m0C.x = pos.x;
    tmp.m0C.y = pos.y;
    tmp.m0C.z = pos.z;
    tmp.m18 = 200.0f;
    tmp.m1C = 400.0f;
    if (!Coming_checkPosition(this, &tmp))
        return 0;
    f32 x = pos.x, y = pos.y, z = pos.z;
    out->mPos.y = y;
    out->mPos.z = z;
    out->mPos.x = x;
    u32 pattern = Coming_pattern(this);
    out->mType = Coming_type(pattern);
    out->mItem = Coming_item(pattern);
    out->m14 = Coming_buoy(pattern);
    // HD: targetAngleY's entire r3 participates in the addition.
    s32 angle = Coming_targetAngle(&ship->current.pos, &pos);
    s32 random = gabi::ftoi(Coming_rndFX(8192.0f));
    out->mAngleY = (u32)angle + (u32)(s16)random;
    return 1;
}

/* 02332728 */
void Act_c::mode_barrel2_wait(Ctrl_c* ctrl) {
    s8 room = dComIfGp_roomControl_getStayNo();
    if (!Coming_switch(this))
        return;
    if (home.roomNo != room && mRoomId != room && !Coming_PrmAbstract(this, PRM_ALWAYS_W, PRM_ALWAYS_S))
        return;
    if (ctrl->m04 > 0)
        return;
    PrmBarrel2_c param;
    if (Coming_parameters(this, &param)) {
        s8 createRoom = home.roomNo;
        u32 item = param.mItem;
        s16 angle = param.mAngleY;
        u32 type = param.mType;
        u8 buoy = param.m14;
        csXyz rot;
        Coming_angleCtor(&rot, 0, angle, 0);
        u32 parameters = (item & 63) | 0x007F0000 | (type << 24) |
                         ((u32)(buoy ^ 1) << 8) | 0x10000000;
        ctrl->m08 = Coming_createBarrel(fpcNm_Obj_Barrel2_e, parameters, &param.mPos, createRoom, &rot, 0, -1, 0);
        Coming_appearTimer(ctrl, this);
        ctrl->m00 = Ctrl_c::MODE_APPEAR;
    } else {
        Coming_retryTimer(ctrl, this);
    }
}

/* 02332868 */
void Act_c::mode_barrel2_appear(Ctrl_c* ctrl) {
    if (ctrl->m04 > 0)
        return;
    u32 id = ctrl->m08;
    if (id == fpcM_ERROR_PROCESS_ID_e)
        return;
    gabi::Local<be<u32>> found;
    if (!Coming_searchById(id, found.get())) {
        ctrl->m08 = fpcM_ERROR_PROCESS_ID_e;
        return;
    }
    ComingBarrel2* barrel = Coming_barrel(*found);
    if (barrel) {
        if (barrel->mProcName != fpcNm_Obj_Barrel2_e)
            // HD: assertion spells the profile tk_Obj_Barrel2_e and the pointer barrel2_act.
            Coming_assert("d_a_obj_coming.cpp", 0x465, "fopAcM_GetName(barrel) == fpcNm_Obj_Barrel2_e");
        barrel->mExit = 1;
    }
    if (ctrl->m08 != fpcM_ERROR_PROCESS_ID_e) {
        Coming_waitTimer(ctrl, this);
        ctrl->m00 = Ctrl_c::MODE_WAIT;
    }
}

/* 02332960 */
void Act_c::mode_barrel2_leave(Ctrl_c* ctrl) {
    u32 id = ctrl->m08;
    if (id == fpcM_ERROR_PROCESS_ID_e)
        return;
    gabi::Local<be<u32>> found;
    if (Coming_searchById(id, found.get())) {
        ComingBarrel2* barrel = Coming_barrel(*found);
        if (barrel) {
            if (barrel->mProcName != fpcNm_Obj_Barrel2_e)
                // HD: assertion spells the profile tk_Obj_Barrel2_e and the pointer barrel2_act.
                Coming_assert("d_a_obj_coming.cpp", 0x480, "fopAcM_GetName(barrel) == fpcNm_Obj_Barrel2_e");
            barrel->mExit = 1;
        }
    } else {
        ctrl->m08 = fpcM_ERROR_PROCESS_ID_e;
    }
}

/* 02331C94 */
s32 Act_c::_execute() {
    u32 makeNum = Coming_PrmAbstract(this, PRM_MAKENUM_W, PRM_MAKENUM_S);
    if (makeNum >= 4) {
        Coming_assert("d_a_obj_coming.cpp", 0x497, "(prm >= 0) && (prm < L_ctrl_max)");
        if (makeNum >= 4)
            return 1;
    }
    s32 count = L_Attr.m00[makeNum];
    bool done = true;
    for (s32 i = 0; i < count; ++i) {
        Ctrl_c* ctrl = &m294[i];
        s32 timer = ctrl->m04;
        if (timer > 0)
            ctrl->m04 = (u32)timer - 1;
        u32 type = m290, mode = ctrl->m00;
        // HD: read the guest PTMF table, including the virtual form, on every dispatch.
        (this->*mode_proc[type][mode].fn)(ctrl);
        if (ctrl->m00 != Ctrl_c::MODE_LEAVE || ctrl->m08 != fpcM_ERROR_PROCESS_ID_e)
            done = false;
    }
    if (done)
        Coming_deleteActor(this);
    return 1;
}

/* 02332A00 */
static s32 Mthd_Create(void* actor) { return static_cast<Act_c*>(actor)->_create(); }
/* 02332A04 */
static s32 Mthd_Delete(void*) { return 1; }
/* 02332A0C */
static s32 Mthd_Execute(void* actor) { return static_cast<Act_c*>(actor)->_execute(); }
/* 02332A10 */
static s32 Mthd_Draw(void*) { return 1; }

// HD: compiler-generated and template code of this translation unit.
/* 02332A18 */
static void __sinit_d_a_obj_coming_cpp() {
    // HD: preserve this TU's constructor store order, which differs from the shared helper.
    l_header.zero[2] = 0;
    l_header.zero[0] = 0;
    l_header.zero[3] = 0;
    l_header.zero[1] = 0;
    Coming_registerDestructor(Coming_record(0x0C));
    l_header.positivePi = 3.1415927410125732f;
    l_header.negativePi = -3.1415927410125732f;
    Coming_headerByteCtor(&l_header.angle);
    Coming_registerDestructor(Coming_record(0x18));
    Coming_headerFlagCtor(&l_header.flag);
    Coming_registerDestructor(Coming_record(0x24));
    Coming_groundCtor(&M_gnd_work);
    dBgS_ObjGndChk& ground = M_gnd_work;
    ground.mbCamThrough = 0;
    ground.mbLinkThrough = 0;
    ground.mbArrowThrough = 0;
    ground.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&ground));
    ground.mbBombThrough = 0;
    ground.mbBoomerangThrough = 0;
    ground.mbRopeThrough = 0;
    ground.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&ground));
    ground.mGrp = 1;
    ground.mbObjThrough = 1;
    ground.cBgS_Chk::__vtbl = gc_tu.gndChk.v10;
    ground.cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
    ground.dBgS_GrpPassChk::__vtbl = gc_tu.gndChk.v4C;
    ground.dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
    Coming_registerDestructor(Coming_record(0x30));
    Coming_lineCtor(&M_wall_work);
    dBgS_ObjLinChk& line = M_wall_work;
    line.mbCamThrough = 0;
    line.mGrp = 1;
    line.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&line));
    line.mbLinkThrough = 0;
    line.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&line));
    line.mbArrowThrough = 0;
    line.cBgS_Chk::__vtbl = l_vtables.line.v10;
    line.mbBombThrough = 0;
    line.cBgS_PolyInfo::__vtbl = l_vtables.line.v20;
    line.dBgS_GrpPassChk::__vtbl = l_vtables.line.v64;
    line.mbBoomerangThrough = 0;
    line.mbRopeThrough = 0;
    line.dBgS_PolyPassChk::__vtbl = l_vtables.line.v58;
    line.mbObjThrough = 1;
    Coming_registerDestructor(Coming_record(0x3C));
}

/* 02332BC0 */
static void ObjGndChk_dtor(dBgS_ObjGndChk* obj, u32 flags) {
    if (obj) {
        obj->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
        obj->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
        obj->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
        Coming_checkDtor(obj, 0);
        if (flags & 1)
            Coming_delete(obj);
    }
}

/* 02332C38 */
static void WtrChk_dtor(ComingWaterChk* obj, u32 flags) {
    if (obj) {
        obj->mPolyVtable = l_vtables.waterDt[0];
        obj->mPassVtable = l_vtables.waterDt[1];
        obj->mGroupVtable = l_vtables.waterDt[2];
        // HD: the polygon base starts at the final vtable word of cBgS_Chk.
        Coming_polyDtor(&obj->mCheck.__vtbl, 0);
        if (flags & 1)
            Coming_delete(obj);
    }
}

/* 02332CB0 */
static void ObjLinChk_dtor(dBgS_ObjLinChk* obj, u32 flags) {
    if (obj) {
        obj->dBgS_PolyPassChk::__vtbl = l_vtables.lineDt[0];
        obj->dBgS_GrpPassChk::__vtbl = l_vtables.lineDt[1];
        obj->cBgS_PolyInfo::__vtbl = l_vtables.lineDt[2];
        Coming_polyDtor(obj, 0);
        if (flags & 1)
            Coming_delete(obj);
    }
}

/* 02332D28 */
static void Act_c_dtor(Act_c* obj, u32 flags) {
    if (obj) {
        Coming_actorDtor(obj, 0);
        if (flags & 1)
            Coming_delete(obj);
    }
}

/* 02332D7C */
static s32 Mthd_IsDelete(void*) { return 1; }

/* 02332D84 */
static u32 PrmAbstract(const Act_c* actor, u32 width, u32 shift) {
    // HD: GHS uses PPC shifts; a count with bit 5 set produces zero.
    u32 value = actor->mParameters;
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 bits = (shift & 32) ? 0 : (value >> (shift & 31));
    return bits & (mask - 1);
}

/* WWHD glue
data L_Attr 10026DC0
data mode_proc 10026E70
data barrel_type 10026EF8
data type_data 10026F04
data item_data 10026F1C
data buoy_data 10026F34
data M_item_tbl 10026FE8
data M_gnd_work 1046951C nocheck
data M_wall_work 10469570 nocheck
data l_water 104694C8 nocheck
data l_waterInitialized 10469518 nocheck
data l_header 104694AC nocheck
str 10026E00 in 0233194C
str 10026E14 "(n >= 0) && (n < L_wait_max)" in 0233194C
str 10026E34 in 02331A44
str 10026E48 "(n >= 0) && (n < L_wait_max)" in 02331A44
str 10026E88 in 02331C94
str 10026E9C "(prm >= 0) && (prm < L_ctrl_max)"
str 10026F88 in 02332868
str 10026F58 "fopAcM_GetName(barrel) == fpcNm_Obj_Barrel2_e" renamed in 02332868
str 10026FCC in 02332960
str 10026F9C "fopAcM_GetName(barrel) == fpcNm_Obj_Barrel2_e" renamed in 02332960
ext 02332D84 u32 Coming_PrmAbstract(const daObjComing::Act_c*, u32, u32)
ext 0273AA24 void Coming_assert(const char*, s32, const char*)
ext 020198D8 f32 Coming_rndF(f32)
ext 02019788 f32 Coming_rnd()
ext 02019918 f32 Coming_rndFX(f32)
ext 0233194C void Coming_initTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*)
ext 02331A08 void Coming_init(daObjComing::Ctrl_c*, const daObjComing::Act_c*)
ext 02331A44 void Coming_waitTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*)
ext 02331B00 void Coming_retryTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*)
ext 02331B4C void Coming_appearTimer(daObjComing::Ctrl_c*, const daObjComing::Act_c*)
ext 025D4ED0 void Coming_actorCtor(fopAc_ac_c*)
ext 025DA884 void Coming_disableDraw(create_tag_class*)
ext 024EF130 u32 Coming_getRoomId(dBgS*, cBgS_PolyInfo*)
ext 025D57E0 void Coming_deleteActor(fopAc_ac_c*)
ext 025BA0C0 u32 Coming_isSwitch(void*, u32, s32)
ext 0246B6A4 u32 Coming_seaCheck(f32, f32)
ext 0246BA0C f32 Coming_waveY(f32, f32)
ext 024F22DC void Coming_waterCtor(ComingWaterChk*)
ext 028F026C void Coming_registerDestructor(void*)
ext 024EF7C0 u32 Coming_waterCheck(dBgS*, ComingWaterChk*)
ext 028E8DE8 f32 Coming_squareDistance(const cXyz*, const cXyz*)
ext 028F4384 f32 Coming_sqrt(f32)
ext 028E93CC void Coming_mtxTrans(Mtx34*, f32, f32, f32)
ext 025F1C28 void Coming_mtxYrot(Mtx34*, s16)
ext 025F24E0 void Coming_transM(f32, f32, f32)
ext 028E8F64 void Coming_multVec(const Mtx34*, const cXyz*, cXyz*)
ext 02331E50 f32 Coming_waterY(const cXyz*)
ext 024F1AFC void Coming_setLine(dBgS_ObjLinChk*, const cXyz*, const cXyz*, u32)
ext 02008860 u32 Coming_lineCross(dBgS*, dBgS_ObjLinChk*)
ext 025D4604 u32 Coming_isActor(void*)
ext 0233211C u32 Coming_checkGround(const daObjComing::Act_c*, const daObjComing::Act_c::MakeTmp_c*)
ext 023322EC u32 Coming_checkWall(const daObjComing::Act_c*, const daObjComing::Act_c::MakeTmp_c*)
ext 025D5218 u32 Coming_searchActor(u32, void*)
ext 02331DD8 u32 Coming_switch(daObjComing::Act_c*)
ext 02331F48 u32 Coming_shipSpeed(const daObjComing::Act_c*, const fopAc_ac_c*)
ext 02331F64 u32 Coming_shipRange(const daObjComing::Act_c*, const fopAc_ac_c*)
ext 02332024 void Coming_makePosition(const daObjComing::Act_c*, const fopAc_ac_c*, cXyz*)
ext 02332468 u32 Coming_checkPosition(const daObjComing::Act_c*, const daObjComing::Act_c::MakeTmp_c*)
ext 023324E8 u32 Coming_pattern(const daObjComing::Act_c*)
ext 02332590 u32 Coming_type(u32)
ext 023325A8 u32 Coming_item(u32)
ext 023325B8 u32 Coming_buoy(u32)
ext 0200F93C s32 Coming_targetAngle(const cXyz*, const cXyz*)
ext 023325C4 u32 Coming_parameters(daObjComing::Act_c*, daObjComing::PrmBarrel2_c*)
ext 0201A478 void Coming_angleCtor(csXyz*, s32, s32, s32)
ext 025D5834 u32 Coming_createBarrel(s32, u32, const cXyz*, s32, const csXyz*, u32, s32, u32)
ext 025D54C4 u32 Coming_searchById(u32, be<u32>*)
ext 028ED6F8 void Coming_headerByteCtor(be<u8>*)
ext 028EAB2C void Coming_headerFlagCtor(be<u8>*)
ext 02008E0C void Coming_groundCtor(cBgS_GndChk*)
ext 02008FEC void Coming_lineCtor(cBgS_LinChk*)
ext 02008DAC void Coming_checkDtor(cBgS_Chk*, u32)
ext 02008B4C void Coming_polyDtor(void*, u32)
ext 0273AF40 void Coming_delete(void*)
ext 025D50BC void Coming_actorDtor(fopAc_ac_c*, u32)
*/
