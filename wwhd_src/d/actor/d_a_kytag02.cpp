/** Wind rail tag (WWHD). */
#include "bindings.h"
#include "d/actor/d_a_kytag02.h"

struct Kytag02Path_l {
    be<u16> mNum;
    be<u16> mNextID;
    be<u8> mArg0;
    u8 pad[3];
    gptr<void> mPoints;
};

static BOOL daKytag02_Draw(kytag02_class*) {
    WWHD_FUNC(0x021AF97C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021AF97C, daKytag02_Draw);

static BOOL daKytag02_Execute(kytag02_class* actor) {
    WWHD_FUNC(0x021AF984, BOOL, actor);
    u32 play = dComIfGp_ea();
    Kytag02Path_l* initial = gabi::at<Kytag02Path_l>(gabi::ea(actor->mpPath.get()));
    u32 player = gabi::load<u32>(play + 0x5B2C);
    if (initial != nullptr) {
        u32 bestIndex = 0;
        Kytag02Path_l* bestPath = initial;
        Kytag02Path_l* path = initial;
        f32 best = 1000000000.0f;
        for (;;) {
            s32 index = 0;
            if (index < (s32)(u16)path->mNum) {
                u32 offset = 0;
                do {
                    u32 points = gabi::ea(path->mPoints.get());
                    f32 px = gabi::load<f32>(player + 0x314);
                    f32 pz = gabi::load<f32>(player + 0x31C);
                    f32 z = gabi::load<f32>(points + offset + 0xC);
                    f32 x = gabi::load<f32>(points + offset + 4);
                    f32 dz = z - pz;
                    f32 dx = x - px;
                    f32 distance = gabi::call<f32>(0x028F4384,
                                                  gabi::fmadds(dx, dx, dz * dz));
                    u16 count = path->mNum;
                    if (best > distance) {
                        best = distance;
                        bestPath = path;
                        bestIndex = (u32)index;
                    }
                    ++index;
                    offset += 16;
                    if (!(index < (s32)count)) break;
                } while (true);
            }
            if ((u16)path->mNextID == 0xFFFF) break;
            s32 room = actor->current.roomNo;
            path = gabi::call<Kytag02Path_l*>(0x025AB070, path, room);
            if (path == nullptr)
                gabi::call(0x0273AA24, STR(0x10013AB8), 0x6C, STR(0x10013AC8));
        }
        // HD retains GC's comparison against the initial path's count.
        if (bestIndex == (u32)(u16)initial->mNum - 1) --bestIndex;
        u32 offset = bestIndex << 4;
        gabi::Local<cXyz> from, to, direction;
        u32 points = gabi::ea(bestPath->mPoints.get());
        from->x = gabi::load<f32>(points + offset + 4);
        points = gabi::ea(bestPath->mPoints.get());
        from->y = gabi::load<f32>(points + offset + 8);
        points = gabi::ea(bestPath->mPoints.get());
        from->z = gabi::load<f32>(points + offset + 0xC);
        points = gabi::ea(bestPath->mPoints.get());
        to->x = gabi::load<f32>(points + offset + 0x14);
        points = gabi::ea(bestPath->mPoints.get());
        to->y = gabi::load<f32>(points + offset + 0x18);
        points = gabi::ea(bestPath->mPoints.get());
        to->z = gabi::load<f32>(points + offset + 0x1C);
        gabi::call(0x02563F64, from.get(), to.get(), direction.get());
        f32 z = direction->z;
        f32 x = direction->x;
        f32 y = direction->y;
        actor->mWindVec.x = x;
        actor->mWindVec.y = y;
        actor->mWindVec.z = z;
        u32 env = gabi::ea(dKy_getEnvlight());
        gabi::store<u32>(env + 0xA08, gabi::ea(&actor->mWindVec));
        points = gabi::ea(bestPath->mPoints.get());
        u32 strength = gabi::load<u8>(points + offset + 3);
        if (strength == 0xFF) strength = bestPath->mArg0;
        env = gabi::ea(dKy_getEnvlight());
        gabi::store<f32>(env + 0xA1C, (f32)strength / 100.0f);
    }
    return TRUE;
}
VERIFY(0x021AF984, daKytag02_Execute);

static BOOL daKytag02_IsDelete(kytag02_class*) {
    WWHD_FUNC(0x021AFB94, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021AFB94, daKytag02_IsDelete);

static BOOL daKytag02_Delete(kytag02_class*) {
    WWHD_FUNC(0x021AFB9C, BOOL, (u32)0);
    u32 env = gabi::ea(dKy_getEnvlight());
    gabi::store<u32>(env + 0xA08, 0);
    return TRUE;
}
VERIFY(0x021AFB9C, daKytag02_Delete);

static cPhs_State daKytag02_Create(kytag02_class* actor) {
    WWHD_FUNC(0x021AFBC8, cPhs_State, actor);
    dKy_getEnvlight();
    if (!fopAcM_CheckCondition(actor, fopAcCnd_INIT_e)) {
        if (actor != nullptr) {
            fopAc_ac_c_ct(actor);
            actor->__vtbl = 0x10013AA8;
        }
        fopAcM_OnCondition(actor, fopAcCnd_INIT_e);
    }
    u32 index = ((u32)actor->mParameters >> 16) & 0xFF;
    u32 path = 0;
    if (index != 0xFF) {
        s32 room = actor->current.roomNo;
        path = gabi::ea(dPath_GetRoomPath((s32)index, room));
    }
    actor->mpPath = gabi::at<void>(path);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021AFBC8, daKytag02_Create);

static void __sinit_d_a_kytag02_cpp() {
    WWHD_FUNC(0x021AFC60, void, (u32)0);
    // This TU's copies of the common header statics.
    const u32 values = 0x10464D00;
    const u32 descriptors = 0x101B8BB0;
    gabi::store<u32>(values + 0x14, 0);
    gabi::store<u32>(values + 0xC, 0);
    gabi::store<u32>(values + 0x18, 0);
    gabi::store<u32>(values + 0x10, 0);
    __register_global_object(descriptors);
    gabi::store<f32>(values, -3.1415927f);
    gabi::store<f32>(values + 4, 3.1415927f);
    gabi::call(0x028ED6F8, values + 8);
    __register_global_object(descriptors + 0xC);
    gabi::call(0x028EAB2C, values + 9);
    __register_global_object(descriptors + 0x18);
}
VERIFY(0x021AFC60, __sinit_d_a_kytag02_cpp);

static void kytag02_class_dt(kytag02_class* actor, s32 flags) {
    WWHD_FUNC(0x021AFCF4, void, actor, flags);
    if (actor != nullptr) {
        gabi::call(0x025D50BC, actor, 0);
        if (flags & 1) operator_delete(actor);
    }
}
VERIFY(0x021AFCF4, kytag02_class_dt);
