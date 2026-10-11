// Sea barrel course actor, with audited HD behaviour preserved.
#include "d/actor/d_a_coming2.h"
using daComing2::Act_c;
// HD: the glue table validator requires numeric initializers: five green, five blue, five yellow
// rupees.
static s32 coming_make_item_no_table[15] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3};

namespace {
void *ptr(u32 address, u32 offset = 0) { return gabi::at<void>(address + offset); }
u32 play() { return dComIfGp_ea(); }
u32 player() { return (gabi::at<Coming2PlayHD>(play())->mpPlayer.v); }
// HD: eight-byte GHS member descriptors; preserve the virtual tail and explicit this adjustment.
u32 dispatch(Act_c *actor, u32 descriptor) {
    auto *method = gabi::at<Coming2MethodHD>(descriptor);
    s32 delta = method->adjustment;
    s32 index = method->slot;
    u32 object = gabi::ea(actor) + delta;
    u32 target;
    if (index < 0)
        target = method->target;
    else {
        s32 vptrOffset = *gabi::at<be<s16>>(descriptor + 6);
        u32 table = *gabi::at<be<u32>>(object + vptrOffset);
        target = gabi::at<Coming2MethodHD>(table)[index].target;
    }
    return gabi::call<u32>(target, ptr(object));
}
} // namespace

/* 02113478 */
void daComing2::Act_c::chase_ship() {
    gabi::Local<be<u32>> id;
    *id = parentActorID;
    if (*id != fpcM_ERROR_PROCESS_ID_e) {
        u32 ship = coming2ActorSearch(ptr(COMING2_SEARCH_BY_ID), id.get());
        if (ship) {
            auto *parent = gabi::at<fopAc_ac_c>(ship);
            current.pos.x = parent->current.pos.x;
            current.pos.y = (parent->current.pos.y);
            current.pos.z = (parent->current.pos.z);
            shape_angle.x = (parent->shape_angle.x);
            shape_angle.y = (parent->shape_angle.y);
            shape_angle.z = (parent->shape_angle.z);
        }
    }
}

/* 021134F8 */
void daComing2::Act_c::renew_scope_info() {
    f32 z = (f32)current.pos.z - (f32)old.pos.z;
    f32 x = (f32)current.pos.x - (f32)old.pos.x;
    f32 speed = coming2SquareRoot(gabi::fmadds(x, x, z * z));
    // HD: keep the z-before-x loads, fused square sum and original NaN-sensitive clamp.
    unk_2A0 = speed;
    f32 clamped = speed - 35.0f >= 0.0f ? speed : 35.0f;
    gabi::Local<cXyz> ahead;
    ahead->set(0.0f, 100.0f, clamped * 135.0f);
    s32 index = unk_2A4;
    s32 angle = ((index + (s16)unk_29C) & 1) ? -0x546 : 0x546;
    s32 spread = (((u32)index >> 2) & 15) * 360;
    angle = (s16)((s16)mStartStat.unk_00 + (angle > 0 ? angle + spread : angle - spread));
    const cXyz *origin = index == 0 ? &current.pos : &mBarrelInfo[index - 1].unk_08;
    coming2MatrixTrans(ptr(0x1048D0CC), (f32)origin->x, (f32)origin->y, (f32)origin->z);
    coming2MatrixYRotate(ptr(0x1048D0CC), angle);
    gabi::Local<Mtx34> matrix;
    coming2MatrixCopy(ptr(0x1048D0CC), matrix.get());
    coming2MatrixVector(matrix.get(), ahead.get(), &unk_4A8);
}

/* 021139AC */
u32 daComing2::Act_c::no_ship_obstacle(cXyz *position) {
    constexpr u32 checker = 0x10462D54;
    auto *M_wall_work = gabi::at<Coming2LineHD>(checker);
    if ((coming2Guards()->line) == 0) {
        coming2Guards()->line = 1;
        coming2LineConstruct(ptr(checker));
        M_wall_work->mFlags = 1;
        M_wall_work->mGroupVtable = COMING2_LINE_GROUP_VTABLE;
        M_wall_work->mPoly = checker + 0x58;
        M_wall_work->mVtable = COMING2_LINE_BASE_VTABLE;
        M_wall_work->mGroup = checker + 0x64;
        M_wall_work->mCheckVtable = COMING2_LINE_CHECK_VTABLE;
        M_wall_work->mPass[1] = 0;
        M_wall_work->mPass[6] = 0;
        M_wall_work->mPass[0] = 1;
        M_wall_work->mPass[4] = 0;
        M_wall_work->mPass[5] = 0;
        M_wall_work->mPass[3] = 0;
        M_wall_work->mPolyVtable = COMING2_LINE_POLY_VTABLE;
        M_wall_work->mPass[2] = 0;
        coming2RegisterStatic(ptr(0x101B3C58));
    }
    coming2LineSet(ptr(checker), &current.pos, position, 0);
    return coming2LineCross(ptr(play(), 0x12A0), ptr(checker)) ^ 1;
}

/* 021141AC */
void *coming2_position_is_safety_call_back(fopAc_ac_c *actor, SafetyCallback *callback) {
    if (coming2IsActor(actor) && actor->group == fopAc_ENEMY_e) {
        u32 position = callback->pos.v;
        f32 z = (f32)actor->current.pos.z - (gabi::at<cXyz>(position)->z);
        f32 x = (f32)actor->current.pos.x - (gabi::at<cXyz>(position)->x);
        f32 squared = gabi::fmadds(x, x, z * z);
        if (squared < 200.0f) {
            callback->ret = 0;
            return nullptr;
        }
    }
    return actor;
}

/* 02114250 */
BOOL coming2_position_is_none_obj(cXyz *position) {
    gabi::Local<SafetyCallback> callback;
    callback->pos.v = gabi::ea(position);
    callback->ret = 1;
    coming2ActorSearch(ptr(COMING2_SAFETY_CALLBACK), callback.get());
    return callback->ret == 1;
}

/* 02113AB0 */
BOOL daComing2::Act_c::check_in_large_sea(const cXyz *position) {
    return std::fabs((f32)position->x) < 349000.0f && std::fabs((f32)position->z) < 349000.0f;
}

/* 02113AE8 */
BOOL daComing2::Act_c::get_water_height(be<f32> *height, const cXyz *position) {
    constexpr f32 invalidHeight = -1000000000.0f;
    f32 wave = invalidHeight;
    if (coming2SeaCheckArea((f32)position->x, (f32)position->z))
        wave = coming2SeaWave((f32)position->x, (f32)position->z);
    else {
        constexpr u32 water = 0x10462DC0;
        auto *wtr = gabi::at<Coming2WaterHD>(water);
        if ((coming2Guards()->water) == 0) {
            coming2Guards()->water = 1;
            coming2WaterConstruct(ptr(water));
            coming2RegisterStatic(ptr(0x101B3C64));
        }
        f32 x = position->x, y = position->y, z = position->z;
        wtr->mPos.x = x;
        wtr->mPos.z = z;
        wtr->mPos.y = y - 1000.0f;
        wtr->mUpper = y + 1000.0f;
        if (coming2WaterCheck(ptr(play(), 0x12A0), ptr(water)))
            wave = (wtr->mHeight);
    }
    *height = wave;
    if (!(wave > invalidHeight))
        return 0;
    constexpr u32 ground = 0x10462E10;
    auto *gnd_work = gabi::at<Coming2GroundHD>(ground);
    if ((coming2Guards()->ground) == 0) {
        coming2Guards()->ground = 1;
        coming2GroundConstruct(ptr(ground));
        gnd_work->mFlags = 1;
        gnd_work->mGroup.v = ground + 0x40;
        gnd_work->mVtable = COMING2_GROUND_BASE_VTABLE;
        gnd_work->mGroupFlags[1] = 0;
        gnd_work->mGroupFlags[6] = 0;
        gnd_work->mPolyVtable = COMING2_GROUND_POLY_VTABLE;
        gnd_work->mGroupFlags[0] = 1;
        gnd_work->mPoly.v = ground + 0x4C;
        gnd_work->mCheckVtable = COMING2_GROUND_CHECK_VTABLE;
        gnd_work->mGroupFlags[4] = 0;
        gnd_work->mGroupFlags[5] = 0;
        gnd_work->mGroupFlags[3] = 0;
        gnd_work->mGroupFlags[2] = 0;
        gnd_work->mGroupVtable = COMING2_GROUND_GROUP_VTABLE;
        coming2RegisterStatic(ptr(0x101B3C70));
    }
    f32 x = position->x, y = position->y, z = position->z;
    gnd_work->mPos.x = x;
    gnd_work->mPos.z = z;
    gnd_work->mPos.y = y + 20000.0f;
    f32 floor = coming2GroundCross(ptr(play(), 0x12A0), ptr(ground));
    return !(floor > wave - 100.0f);
}

/* 0211321C */
void daComing2::Act_c::init_barrelN_info(s32 index) {
    auto &barrel = mBarrelInfo[index];
    barrel.unk_00 = fpcM_ERROR_PROCESS_ID_e;
    barrel.unk_04 = 0;
    barrel.unk_06 = 0;
    barrel.unk_08 = current.pos;
}

/* 02113254 */
void daComing2::Act_c::init_barrel_info() {
    for (s32 index = 0; index < 15; ++index)
        init_barrelN_info(index);
}

/* 021132F4 */
void daComing2::Act_c::init_flag_info() {
    mStartBuoy[0].unk_00 = fpcM_ERROR_PROCESS_ID_e;
    mStartBuoy[1].unk_00 = fpcM_ERROR_PROCESS_ID_e;
}

/* 02113288 */
void daComing2::Act_c::init_coming_info() {
    unk_2A4 = 0;
    unk_2A6 = 600;
    unk_29C = gabi::ftoi(coming2RandomF(2.0f)) & 1;
    unk_29A = 0;
    unk_2A8 = 0;
}

/* 02113E50 */
BOOL daComing2::Act_c::make_coming_param(cXyz *position, be<s32> *type, be<s32> *item,
                                         be<u8> *hasFlag) {
    gabi::Local<cXyz> waterPosition;
    *waterPosition = unk_4A8;
    if (!get_water_height(&waterPosition->y, waterPosition.get()))
        return 0;
    *position = *waterPosition;
    *type = 1;
    *hasFlag = 0;
    u32 index = (s32)(s16)unk_2A4;
    *item = index < 15 ? coming_make_item_no_table[index] : dItemNo_RECOVER_FAIRY_e;
    return 1;
}

/* 021136D8 */
BOOL daComing2::Act_c::request_barrel_exitN(s32 index) {
    u32 id = mBarrelInfo[index].unk_00;
    if (id == fpcM_ERROR_PROCESS_ID_e)
        return 1;
    gabi::Local<be<u32>> barrel;
    if (coming2SearchByID(id, barrel.get()) && *barrel != 0) {
        gabi::at<Coming2BarrelHD>(*barrel)->mExitReq = 1;
        return 1;
    }
    return 0;
}

/* 02113D2C */
BOOL daComing2::Act_c::request_all_barrel_exit() {
    s32 completed = 0;
    for (s32 index = 0; index < 15; ++index)
        if (request_barrel_exitN(index))
            ++completed;
    return completed == 15;
}

/* 02113DA8 */
BOOL daComing2::Act_c::request_all_flag_exit() {
    gabi::Local<be<u32>> buoy;
    for (s32 index = 0; index < 2; ++index) {
        u32 id = mStartBuoy[index].unk_00;
        if (id != fpcM_ERROR_PROCESS_ID_e && coming2SearchByID(id, buoy.get()) && *buoy != 0) {
            gabi::at<Coming2BarrelHD>(*buoy)->mExitReq = 1;
            mStartBuoy[index].unk_00 = fpcM_ERROR_PROCESS_ID_e;
        }
    }
    return mStartBuoy[0].unk_00 == fpcM_ERROR_PROCESS_ID_e &&
           mStartBuoy[1].unk_00 == fpcM_ERROR_PROCESS_ID_e;
}

/* 02113754 */
void daComing2::Act_c::barrel_execute(s32 index) {
    auto &barrel = mBarrelInfo[index];
    u32 id = barrel.unk_00;
    if (id == fpcM_ERROR_PROCESS_ID_e)
        return;
    gabi::Local<be<u32>> found;
    if (coming2SearchByID(id, found.get()) == 1) {
        s16 timer = barrel.unk_04; // HD: the reload occurs before reading the found actor's flag.
        if (*found != 0)
            barrel.unk_06 = (gabi::at<Coming2BarrelHD>(*found)->mItemGive);
        if (timer == 0)
            request_barrel_exitN(index);
        else if (timer > 0)
            barrel.unk_04 = timer - 1;
    } else if (barrel.unk_06 == 0) {
        init_barrelN_info(index);
        coming_process_init(0);
    }
    // HD: accepts any nonzero flag here, unlike GC's final equality with 1.
    if (barrel.unk_06 != 0) {
        if ((u32)(s32)(s16)unk_2A4 >= 15)
            coming_process_init(0);
        else {
            request_barrel_exitN(index);
            init_barrelN_info(index);
            unk_29A = 1;
        }
    }
}

/* 02113888 */
void daComing2::Act_c::barrel_main() {
    for (s32 index = 0; index < 15; ++index)
        barrel_execute(index);
}

/* 02113F18 */
BOOL daComing2::Act_c::coming_clear_init() {
    u32 currentPlayer = player();
    if (currentPlayer)
        unk_299 = (gabi::at<fopAc_ac_c>(currentPlayer)->current.roomNo);
    return 1;
}

/* 02113F5C */
void daComing2::Act_c::coming_clear_main() {
    if (request_all_barrel_exit() && request_all_flag_exit()) {
        init_barrel_info();
        init_coming_info();
        init_flag_info();
        coming_process_init(1);
    }
}

/* 02113FBC */
BOOL daComing2::Act_c::coming_wait_init() {
    unk_2A6 = 600;
    unk_2A4 = 0;
    return 1;
}

/* 02113FD4 */
void daComing2::Act_c::coming_wait_main() {
    u32 currentPlayer = player();
    if (!currentPlayer)
        return;
    if ((gabi::at<Coming2PlayHD>(play())->mPlayerStatus) & COMING2_SHIP_RIDE) {
        if ((f32)unk_2A0 > 35.0f &&
            (s32)coming2GetEventReg(ptr(dComIfGs_ea(), 0x644), COMING2_POINT_EVENT_REG) >= 2) {
            s16 timer = unk_2A6;
            if (timer > 0) {
                timer = (s16)(timer - 1);
                unk_2A6 = timer;
            }
            if ((s8)unk_299 != (gabi::at<fopAc_ac_c>(currentPlayer)->current.roomNo) &&
                timer == 0) {
                unk_440.y = current.pos.y;
                mStartStat.unk_00 = shape_angle.y;
                unk_440.z = current.pos.z;
                unk_440.x = current.pos.x;
                coming_process_init(2);
            }
        } else
            coming_process_init(1);
    } else {
        unk_299 = (gabi::at<fopAc_ac_c>(currentPlayer)->current.roomNo);
        coming_process_init(1);
    }
}

/* 021140E0 */
BOOL daComing2::Act_c::coming_setF_init() {
    u32 currentPlayer = player();
    if (!currentPlayer)
        return 0;
    unk_298 = (gabi::at<fopAc_ac_c>(currentPlayer)->current.roomNo);
    // HD: consumes two points as two distinct get/decrement/set operations.
    for (s32 i = 0; i < 2; ++i) {
        u32 count = coming2GetEventReg(ptr(dComIfGs_ea(), 0x644), COMING2_POINT_EVENT_REG);
        u32 decremented = count - 1;
        u32 value = (s32)decremented < 0 ? 0 : (decremented & 0xFF);
        coming2SetEventReg(ptr(dComIfGs_ea(), 0x644), COMING2_POINT_EVENT_REG, value);
    }
    return 1;
}

/* 02114298 */
void daComing2::Act_c::coming_setF_main() {
    f32 lateral;
    do {
        lateral = coming2RandomFX(1600.0f);
    } while (!(std::fabs(lateral) > 700.0f));
    struct Positions {
        cXyz point[2];
    };
    gabi::Local<Positions> offsets;
    f32 forward = (f32)unk_2A0 * 120.0f;
    offsets->point[0].set(lateral + 500.0f, 0.0f, forward);
    offsets->point[1].set(lateral + -500.0f, 0.0f, forward);
    coming2MatrixTrans(ptr(0x1048D0CC), (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    coming2MatrixYRotate(ptr(0x1048D0CC), (s32)(s16)shape_angle.y);
    gabi::Local<Mtx34> matrix;
    coming2MatrixCopy(ptr(0x1048D0CC), matrix.get());
    gabi::Local<cXyz> position;
    gabi::Local<csXyz> rotation;
    for (s32 index = 0; index < 2; ++index) {
        if (mStartBuoy[index].unk_00 == fpcM_ERROR_PROCESS_ID_e) {
            coming2MatrixVector(matrix.get(), &offsets->point[index], position.get());
            if (get_water_height(&position->y, position.get()) &&
                check_in_large_sea(position.get()) &&
                coming2_position_is_none_obj(position.get()) && no_ship_obstacle(position.get())) {
                coming2AngleConstruct(rotation.get(), 0, (s32)(s16)mStartStat.unk_00, 0);
                u32 id = coming2ActorCreate(fpcNm_Obj_Barrel2_e, 0x117F043F, position.get(), -1,
                                            rotation.get(), 0, -1, 0);
                mStartBuoy[index].unk_00 = id;
                if (id != fpcM_ERROR_PROCESS_ID_e)
                    mStartBuoy[index].unk_04 = *position;
            }
        }
    }
    if (mStartBuoy[0].unk_00 != fpcM_ERROR_PROCESS_ID_e &&
        mStartBuoy[1].unk_00 != fpcM_ERROR_PROCESS_ID_e)
        coming_process_init(3);
    else
        coming_process_init(0);
}

/* 021144B4 */
BOOL daComing2::Act_c::coming_checkS_init() {
    unk_46C = 0;
    return 1;
}

/* 021144C4 */
void daComing2::Act_c::coming_checkS_main() {
    if ((s16)unk_46C < 180) {
        f32 ax = old.pos.x, az = old.pos.z;
        f32 bx = current.pos.x, bz = current.pos.z;
        f32 cx = mStartBuoy[0].unk_04.x, cz = mStartBuoy[0].unk_04.z;
        f32 dx = mStartBuoy[1].unk_04.x, dz = mStartBuoy[1].unk_04.z;
        f32 segmentX = bx - ax, segmentZ = bz - az;
        f32 side0 = gabi::fmsubs(segmentX, cz - az, segmentZ * (cx - ax));
        f32 side1 = gabi::fmsubs(segmentX, dz - az, segmentZ * (dx - ax));
        if (side0 * side1 < 0.0f) {
            f32 buoyX = cx - dx, buoyZ = cz - dz;
            f32 oldSide = gabi::fmsubs(buoyX, az - dz, buoyZ * (ax - dx));
            f32 newSide = gabi::fmsubs(buoyX, bz - dz, buoyZ * (bx - dx));
            if (oldSide * newSide < 0.0f) {
                request_all_flag_exit();
                coming_process_init(4);
            }
        }
        unk_46C = (s16)unk_46C + 1;
    } else {
        u32 currentPlayer = player();
        if (currentPlayer) {
            unk_299 = (gabi::at<fopAc_ac_c>(currentPlayer)->current.roomNo);
            coming_process_init(0);
        }
    }
}

/* 02114604 */
BOOL daComing2::Act_c::coming_game_init() {
    s8 startingRoom = unk_298;
    u32 count = unk_294;
    unk_29A = 1;
    unk_299 = startingRoom;
    unk_294 = count + 1;
    unk_2A8 = 0;
    return 1;
}

/* 02114630 */
void daComing2::Act_c::coming_game_main() {
    u32 originalIndex = (s32)(s16)unk_2A4;
    if (originalIndex >= 15)
        return;
    if ((s16)unk_29A == 1) {
        gabi::Local<cXyz> position;
        gabi::Local<be<s32>> type, item;
        gabi::Local<be<u8>> hasFlag;
        if (mBarrelInfo[originalIndex].unk_00 == fpcM_ERROR_PROCESS_ID_e &&
            make_coming_param(position.get(), type.get(), item.get(), hasFlag.get())) {
            if (check_in_large_sea(position.get())) {
                if (coming2_position_is_none_obj(position.get()) &&
                    no_ship_obstacle(position.get())) {
                    s32 barrelType = *type, droppedItem = *item;
                    u32 flag = *hasFlag;
                    gabi::Local<csXyz> rotation;
                    coming2AngleConstruct(rotation.get(), 0, (s32)(s16)mStartStat.unk_00, 0);
                    u32 parameters = (droppedItem & 0x3F) | 0x007F0000 | ((u32)barrelType << 24) |
                                     ((flag ^ 1) << 8) | 0x10000000;
                    u32 id = coming2ActorCreate(fpcNm_Obj_Barrel2_e, parameters, position.get(), -1,
                                                rotation.get(), 0, -1, 0);
                    mBarrelInfo[(s16)unk_2A4].unk_00 = id;
                    if (id != fpcM_ERROR_PROCESS_ID_e) {
                        unk_29A = 0;
                        mBarrelInfo[originalIndex].unk_06 = 0;
                        mBarrelInfo[(s16)unk_2A4].unk_08 = *position;
                        mBarrelInfo[(s16)unk_2A4].unk_04 = 0x93 - (s16)unk_2A4;
                        // HD: stores 1 and returns immediately; GC stores 0 then increments below.
                        unk_2A8 = 1;
                        unk_2A4 = (s16)unk_2A4 + 1;
                        return;
                    }
                }
            } else
                coming_process_init(0);
        }
        s16 timer = unk_2A8;
        unk_2A8 = timer + 1;
        if (timer >= 60)
            coming_process_init(0);
    } else
        unk_2A8 = 0;
}

/* 02113304 */
BOOL daComing2::Act_c::coming_process_init(s32 process) {
    if ((u32)process < 5 && dispatch(this, COMING2_INIT_TABLE + (u32)process * 8)) {
        unk_46E = process;
        return 1;
    }
    return 0;
}

/* 02113680 */
void daComing2::Act_c::coming_process_main() {
    s32 process = unk_46E;
    if ((u32)process < 5)
        dispatch(this, COMING2_MAIN_TABLE + (u32)process * 8);
}

/* 021133BC */
s32 daComing2::Act_c::_create() {
    if (!(actor_condition & 8)) {
        if (this) {
            coming2ActorConstruct(this);
            __vtbl = COMING2_ACTOR_VTABLE;
        }
        actor_condition = (u32)actor_condition | 8;
    }
    unk_299 = -1;
    init_barrel_info();
    init_coming_info();
    init_flag_info();
    coming_process_init(0);
    u32 currentPlayer = player();
    unk_440.x = current.pos.x;
    unk_440.z = current.pos.z;
    unk_440.y = current.pos.y;
    mStartStat.unk_00 = shape_angle.y;
    if (currentPlayer)
        unk_299 = (gabi::at<fopAc_ac_c>(currentPlayer)->current.roomNo);
    return 4;
}

/* 021138E0 */
BOOL daComing2::Act_c::_execute() {
    gabi::Local<be<u32>> id;
    *id = parentActorID;
    if (*id != fpcM_ERROR_PROCESS_ID_e) {
        u32 ship = coming2ActorSearch(ptr(COMING2_SEARCH_BY_ID), id.get());
        if (ship) {
            chase_ship();
            renew_scope_info();
            if ((gabi::at<Coming2PlayHD>(play())->mEventRunning) != 0 ||
                (gabi::at<Coming2ShipHD>(ship)->mTornadoActor) != 0 ||
                (gabi::at<Coming2ShipHD>(ship)->mWhirlActor) != 0) {
                unk_299 = (gabi::at<fopAc_ac_c>(player())->current.roomNo);
                coming_process_init(0);
            } else
                coming_process_main();
            barrel_main();
        }
    }
    return 1;
}

/* 02114808 */
s32 Mthd_Create(Act_c *actor) { return actor->_create(); }

/* 0211480C */
BOOL Mthd_Delete(Act_c *actor) { return 1; }

/* 02114814 */
BOOL Mthd_Execute(Act_c *actor) { return actor->_execute(); }

/* 02114818 */
BOOL Mthd_Draw(Act_c *actor) { return 1; }

/* 02114A70 */
BOOL Mthd_IsDelete(Act_c *actor) { return 1; }

// HD: compiler-generated initializers and deleting destructors.
/* 02114820 */
void coming2_staticInit() {
    coming2Statics()->counters[2] = 0;
    coming2Statics()->counters[0] = 0;
    coming2Statics()->counters[3] = 0;
    coming2Statics()->counters[1] = 0;
    coming2RegisterStatic(ptr(0x101B3C7C));
    f32 first = (*gabi::at<be<f32>>(COMING2_MINIMUM_INITIAL));
    f32 second = (*gabi::at<be<f32>>(COMING2_MAXIMUM_INITIAL));
    coming2Statics()->minimum = first;
    coming2Statics()->maximum = second;
    coming2InitializeOne(ptr(0x10462D40));
    coming2RegisterStatic(ptr(0x101B3C88));
    coming2InitializeZero(ptr(0x10462D41));
    coming2RegisterStatic(ptr(0x101B3C94));
}

/* 021148B4 */
void coming2_groundCheckDestroy(void *object, u32 flags) {
    if (object) {
        u32 address = gabi::ea(object);
        gabi::at<Coming2GroundHD>(address)->mCheckVtable = COMING2_GROUND_CHECK_DTOR_VTABLE;
        gabi::at<Coming2GroundHD>(address)->mGroupVtable = COMING2_GROUND_GROUP_DTOR_VTABLE;
        gabi::at<Coming2GroundHD>(address)->mPolyVtable = COMING2_PASS_DTOR_VTABLE;
        coming2GroundDestroy(object, 0);
        if (flags & 1)
            coming2Delete(object);
    }
}

/* 0211492C */
void coming2_waterCheckDestroy(void *object, u32 flags) {
    if (object) {
        u32 address = gabi::ea(object);
        gabi::at<Coming2WaterHD>(address)->mVtable = COMING2_WATER_CHECK_VTABLE;
        gabi::at<Coming2WaterHD>(address)->mGroupVtable = COMING2_WATER_GROUP_VTABLE;
        gabi::at<Coming2WaterHD>(address)->mPolyVtable = COMING2_PASS_DTOR_VTABLE;
        coming2CheckDestroy(ptr(address, 0x10), 0);
        if (flags & 1)
            coming2Delete(object);
    }
}

/* 021149A4 */
void coming2_lineCheckDestroy(void *object, u32 flags) {
    if (object) {
        u32 address = gabi::ea(object);
        gabi::at<Coming2LineHD>(address)->mPolyVtable = COMING2_LINE_POLY_DTOR_VTABLE;
        gabi::at<Coming2LineHD>(address)->mGroupVtable = COMING2_PASS_DTOR_VTABLE;
        gabi::at<Coming2LineHD>(address)->mCheckVtable = COMING2_LINE_CHECK_DTOR_VTABLE;
        coming2CheckDestroy(object, 0);
        if (flags & 1)
            coming2Delete(object);
    }
}

/* 02114A1C */
void coming2_destroy(Act_c *actor, u32 flags) {
    if (actor) {
        coming2ActorDestroy(actor, 0);
        if (flags & 1)
            coming2Delete(actor);
    }
}

// clang-format off
/* WWHD glue
data coming_make_item_no_table 101B3C1C
ext 025D54C4 s32 coming2SearchByID(u32 id,void* output)
ext 025D4ED0 void coming2ActorConstruct(fopAc_ac_c* actor)
ext 025D5218 u32 coming2ActorSearch(void* callback,void* data)
ext 025B8BB0 u32 coming2GetEventReg(void* event,u32 flag)
ext 025B8AF4 void coming2SetEventReg(void* event,u32 flag,u32 value)
ext 025D4604 s32 coming2IsActor(fopAc_ac_c* actor)
ext 028F026C void coming2RegisterStatic(void* record)
ext 028ED6F8 void coming2InitializeOne(void* object)
ext 028EAB2C void coming2InitializeZero(void* object)
ext 02008DAC void coming2GroundDestroy(void* object,s32 flags)
ext 0273AF40 void coming2Delete(void* object)
ext 02008B4C void coming2CheckDestroy(void* object,s32 flags)
ext 025D50BC void coming2ActorDestroy(fopAc_ac_c* actor,s32 flags)
ext 028F4384 f32 coming2SquareRoot(f32 value)
ext 028E93CC void coming2MatrixTrans(void* matrix,f32 x,f32 y,f32 z)
ext 025F1C28 void coming2MatrixYRotate(void* matrix,s32 angle)
ext 028E90D4 void coming2MatrixCopy(const void* source,void* destination)
ext 028E8F64 void coming2MatrixVector(const void* matrix,const cXyz* source,cXyz* destination)
ext 02008FEC void coming2LineConstruct(void* object)
ext 024F1AFC void coming2LineSet(void* object,const cXyz* start,const cXyz* end,u32 actor)
ext 02008860 u32 coming2LineCross(void* bg,void* check)
ext 0246B6A4 s32 coming2SeaCheckArea(f32 x,f32 z)
ext 0246BA0C f32 coming2SeaWave(f32 x,f32 z)
ext 024F22DC void coming2WaterConstruct(void* object)
ext 024EF7C0 s32 coming2WaterCheck(void* bg,void* check)
ext 02008E0C void coming2GroundConstruct(void* object)
ext 02008974 f32 coming2GroundCross(void* bg,void* check)
ext 0201A478 void coming2AngleConstruct(csXyz* rotation,s32 x,s32 y,s32 z)
ext 025D5834 u32 coming2ActorCreate(s32 name,u32 parameters,const cXyz* position,s32 room,const csXyz* rotation,void* scale,s32 subtype,u32 parent)
ext 02019918 f32 coming2RandomFX(f32 range)
ext 020198D8 f32 coming2RandomF(f32 range)
*/
