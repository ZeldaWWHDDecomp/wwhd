/** d_a_kytag02.cpp — wind rail tag.
 * Based on tww d/actor/d_a_kytag02.cpp.
 */
#include "d/actor/d_a_kytag02.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/JSystem/JUTAssert.h"
using Path = kytag02_path::dPath;
using kytag02_path::dPnt;

/* 021AF97C */
static BOOL daKytag02_Draw(kytag02_class*) {
    return TRUE;
}

/* 021AF984 */
static BOOL daKytag02_Execute(kytag02_class* actor) {
    auto* play = reinterpret_cast<kytag02_play_view*>(dComIfGp_get());
    Path* initial = actor->mpPath;
    fopAc_ac_c* player = play->mPlayer;
    if (initial) {
        u32 bestIndex = 0;
        Path* bestPath = initial;
        Path* path = initial;
        f32 best = 1000000000.0f;
        // HD: get_nearpos_rail inlines the GameCube rail traversal.
        for (;;) {
            s32 index = 0;
            if (index < path->m_num) {
                do {
                    dPnt* points = path->m_points;
                    f32 px = player->current.pos.x;
                    f32 pz = player->current.pos.z;
                    f32 z = points[index].m_position.z;
                    f32 x = points[index].m_position.x;
                    f32 dz = z - pz;
                    f32 dx = x - px;
                    f32 distance = std_sqrtf(gabi::fmadds(dx, dx, dz * dz)); // HD: fused single precision.
                    u16 count = path->m_num;
                    if (best > distance) {
                        best = distance;
                        bestPath = path;
                        bestIndex = index;
                    }
                    ++index;
                    if (index >= count)
                        break;
                } while (true);
            }
            if (path->m_nextID == 0xFFFF)
                break;
            s32 room = actor->current.roomNo;
            path = kytag02_nextPath(path, room);
            if (!path)
                JUTAssertion_showAssert("d_a_kytag02.cpp", 0x6C, "calc_path_pt != (0)");
        }
        // HD: retains GC's comparison against the initial path's count.
        if (bestIndex == static_cast<u32>(initial->m_num) - 1)
            --bestIndex;
        cXyz from;
        cXyz to;
        cXyz direction;
        // get_railwind_vec: retain the six separate point-pointer reads and float copies.
        from.x = bestPath->m_points[bestIndex].m_position.x;
        from.y = bestPath->m_points[bestIndex].m_position.y;
        from.z = bestPath->m_points[bestIndex].m_position.z;
        to.x = bestPath->m_points[bestIndex + 1].m_position.x;
        to.y = bestPath->m_points[bestIndex + 1].m_position.y;
        to.z = bestPath->m_points[bestIndex + 1].m_position.z;
        dKyr_get_vectle_calc(&from, &to, &direction);
        f32 z = direction.z;
        f32 x = direction.x;
        f32 y = direction.y;
        actor->mWindVec.x = x;
        actor->mWindVec.y = y;
        actor->mWindVec.z = z;
        kytag02_getEnvlight()->mWind.mpWindVecOverride = &actor->mWindVec;
        dPnt* points = bestPath->m_points;
        u32 strength = points[bestIndex].mArg3;
        if (strength == 0xFF)
            strength = bestPath->mArg0;
        kytag02_getEnvlight()->mWind.mWindStrengthOverride = static_cast<f32>(strength) / 100.0f;
    }
    return TRUE;
}

/* 021AFB94 */
static BOOL daKytag02_IsDelete(kytag02_class*) {
    return TRUE;
}

/* 021AFB9C */
static BOOL daKytag02_Delete(kytag02_class*) {
    kytag02_getEnvlight()->mWind.mpWindVecOverride = nullptr;
    return TRUE;
}

/* 021AFBC8 */
static cPhs_State daKytag02_Create(kytag02_class* actor) {
    dKy_getEnvlight();
    if (!fopAcM_CheckCondition(actor, fopAcCnd_INIT_e)) {
        if (actor) {
            fopAc_ac_c_ct(actor);
            actor->__vtbl = kytag02_class::__vt;
        }
        fopAcM_OnCondition(actor, fopAcCnd_INIT_e);
    }
    u32 index = (fopAcM_GetParam(actor) >> 16) & 0xFF;
    Path* path = nullptr;
    if (index != 0xFF) {
        s32 room = actor->current.roomNo;
        path = reinterpret_cast<Path*>(dPath_GetRoomPath(index, room));
    }
    actor->mpPath = path;
    return cPhs_COMPLEATE_e;
}

/* 021AFC60 */
static void __sinit_d_a_kytag02_cpp() {
    sinit_header_statics(0x10464D00, 0x101B8BB0);
}

/* 021AFCF4 */
static void kytag02_class_dt(kytag02_class* actor, s32 flags) {
    if (actor) {
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operator_delete(actor);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
str 10013AB8
str 10013AC8
ext 025AB070 kytag02_path::dPath* kytag02_nextPath(kytag02_path::dPath* path, s32 room)
ext 02563F64 void dKyr_get_vectle_calc(cXyz* from, cXyz* to, cXyz* direction)
*/
