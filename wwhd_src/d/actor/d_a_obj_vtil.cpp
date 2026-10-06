// Tingle statues, complete HD translation unit 023B5E60..023B73EB.
#include "d/actor/d_a_obj_vtil.h"
#include <cmath>

namespace {
template<class T> T read(u32 address) { return *gabi::at<be<T>>(address); }
template<class T> void write(u32 address, T value) { *gabi::at<be<T>>(address) = value; }
void* ptr(u32 address) { return gabi::at<void>(address); }
u32 address(daObjVtil_c* statue, u32 offset) { return gabi::ea(statue) + offset; }
struct ArchiveName { be<u32> name, vtable; };
void correctBackground(daObjVtil_c* statue) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x024F08A8, &statue->mAcch, ptr(play + 0x12A0));
}
void stopTracking(daObjVtil_c* statue) {
    u32 play = gabi::call<u32>(0x025200D4);
    u32 camera = read<u32>(play + 0x5AF8);
    u32 processID = read<u32>(address(statue, 4));
    gabi::call(0x025052BC, ptr(camera + 0x248), processID);
}
}

daObjVtil_c* vtil_construct(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B5E60, daObjVtil_c*, statue);
    if (!statue) statue = gabi::call<daObjVtil_c*>(0x0273AD10, 0x75C);
    if (!statue) return nullptr;
    gabi::call(0x025D4ED0, statue);
    u32 actor = gabi::ea(statue);
    write<u32>(actor + 0xB4, 0x100331BC);
    gabi::call(0x024F0474, &statue->mAcch);
    write<u32>(actor + 0x3C8, 0x1003318C);
    write<u32>(actor + 0x3D8, 0x1003319C);
    write<u32>(actor + 0x3CC, 0x100331AC);
    write<u8>(actor + 0x3D0, 1);
    gabi::call(0x024EFE94, &statue->mAcchCircle);
    gabi::call(0x0200BD2C, &statue->mCollisionStatus);
    gabi::call(0x02515DA0, ptr(actor + 0x5D8));
    write<u32>(actor + 0x5D4, 0x1004AE88);
    write<u32>(actor + 0x5D8, 0x1004AEC0);
    gabi::call(0x02515FB8, &statue->mCylinder);
    write<u32>(actor + 0x70C, 0x100015A8);
    write<u32>(actor + 0x708, 0x1003317C);
    gabi::call(0x02018590, ptr(actor + 0x710));
    statue->mLight.mHD20 = 1.0f;
    write<u32>(actor + 0x724, 0x1004B150);
    write<u32>(actor + 0x634, 0x1004B108);
    write<u32>(actor + 0x70C, 0x1004B160);
    return statue;
}
VERIFY(0x023B5E60, vtil_construct);

BOOL vtil_create_heap(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B5F70, BOOL, statue);
    u32 type = statue->mStatueType;
    gabi::Local<ArchiveName> archive;
    archive->vtable = 0x10033164;
    archive->name = 0x100332D0;
    u32 resource = read<u32>(0x100332EC + type * 4);
    u32 modelData = gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(), resource);
    if (!modelData) {
        gabi::call(0x0273AA24, ptr(0x100331E0), 0x147, ptr(0x100331D0));
        return 0;
    }
    statue->mModel = gabi::call<J3DModel*>(0x025E38E0, ptr(modelData), 0, 0x11020203);
    return statue->mModel.get() != nullptr;
}
VERIFY(0x023B5F70, vtil_create_heap);

BOOL vtil_heap_callback(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6028, BOOL, statue);
    return vtil_create_heap(statue);
}
VERIFY(0x023B6028, vtil_heap_callback);

void vtil_init_mtx(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B602C, void, statue);
    f32 x = statue->scale.x, y = statue->scale.y, z = statue->scale.z;
    u32 model = gabi::ea(statue->mModel.get());
    write<f32>(model + 0xBC, x);
    write<f32>(model + 0xC0, y);
    write<f32>(model + 0xC4, z);
    x = statue->current.pos.x; y = statue->current.pos.y; z = statue->current.pos.z;
    gabi::call(0x028E93CC, ptr(0x1048D0CC), x, y, z);
    gabi::call(0x025F1B48, ptr(0x1048D0CC), s16(statue->shape_angle.x), s16(statue->shape_angle.y), s16(statue->shape_angle.z));
    // Snapshot the matrix before storing: model storage may overlap the stack matrix.
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i) matrix[i] = read<f32>(0x1048D0CC + i * 4);
    model = gabi::ea(statue->mModel.get());
    for (u32 i = 0; i < 12; ++i) write<f32>(model + 0xC8 + i * 4, matrix[i]);
    gabi::call(0x027F4D5C, statue->mModel.get());
}
VERIFY(0x023B602C, vtil_init_mtx);

void vtil_init_co(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6114, void, statue);
    statue->mCollisionStatus.Init(200, 255, statue);
    gabi::call(0x02516518, &statue->mCylinder, ptr(0x10033300));
    write<u32>(address(statue, 0x63C), address(statue, 0x5BC));
    gabi::call(0x020182E0, ptr(address(statue, 0x710)), &statue->current.pos);
    write<u32>(address(statue, 0x674), read<u32>(0x101FFBA8));
    write<u32>(address(statue, 0x678), read<u32>(0x101FFBAC));
    write<u32>(address(statue, 0x67C), read<u32>(0x101FFBB0));
    u32 x = read<u32>(0x101FFBA8);
    u32 flags = read<u32>(address(statue, 0x68C));
    write<u32>(address(statue, 0x6AC), x);
    write<u32>(address(statue, 0x6B0), read<u32>(0x101FFBAC));
    u32 z = read<u32>(0x101FFBB0);
    write<u32>(address(statue, 0x68C), flags | 1);
    write<u32>(address(statue, 0x6B4), z);
}
VERIFY(0x023B6114, vtil_init_co);

void vtil_init_bgc(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B61B4, void, statue);
    statue->mAcchCircle.SetWall(30.0f, 56.0f);
    statue->mAcch.Set(&statue->current.pos, &statue->old.pos, statue, 1, &statue->mAcchCircle, &statue->speed, &statue->current.angle, &statue->shape_angle);
    u32 flags = statue->mAcch.m_flags;
    write<f32>(address(statue, 0x478), 160.0f);
    statue->mAcch.m_flags = (flags & ~0x408u) | 0x60000;
    gabi::call(0x025D6870, statue, 0);
    correctBackground(statue);
    statue->mAcch.m_flags = u32(statue->mAcch.m_flags) & ~0x80u;
}
VERIFY(0x023B61B4, vtil_init_bgc);

void vtil_renew_attention(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6268, void, statue);
    f32 z = statue->current.pos.z, y = statue->current.pos.y, x = statue->current.pos.x;
    write<f32>(address(statue, 0x398), z);
    write<f32>(address(statue, 0x390), x);
    write<f32>(address(statue, 0x394), y + 160.0f);
}
VERIFY(0x023B6268, vtil_renew_attention);

void vtil_to_wait(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6290, void, statue);
    u32 attack = read<u32>(address(statue, 0x5F8));
    u32 collision = read<u32>(address(statue, 0x624));
    u32 target = read<u32>(address(statue, 0x610));
    statue->gravity = -6.0f;
    write<u32>(address(statue, 0x5F8), attack & ~1u);
    statue->speedF = 0.0f;
    write<u32>(address(statue, 0x610), target | 1);
    write<u32>(address(statue, 0x624), collision | 1);
    statue->mCollisionStatus.Init(200, 255, statue);
    statue->mMode = 0;
}
VERIFY(0x023B6290, vtil_to_wait);

u32 vtil_parameters(daObjVtil_c* statue, u32 width, u32 shift) {
    WWHD_FUNC(0x023B73D0, u32, statue, width, shift);
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 parameters = (shift & 32) ? 0 : (u32(statue->mParameters) >> (shift & 31));
    return parameters & (mask - 1);
}
VERIFY(0x023B73D0, vtil_parameters);

s32 vtil_create(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6310, s32, statue);
    u32 condition = statue->actor_condition;
    if (!(condition & 8)) {
        if (statue) {
            vtil_construct(statue);
            condition = statue->actor_condition;
        }
        statue->actor_condition = condition | 8;
    }
    u32 type = vtil_parameters(statue, 4, 0);
    if (type == 15 || type == 0xFFFFFFFF) type = 0;
    statue->mStatueType = type;
    if (!gabi::call<s32>(0x02520864, read<u32>(0x100332D8 + type * 4), 15)) return 5;
    s32 phase = gabi::call<s32>(0x02520460, &statue->mPhase, ptr(0x100332D0));
    if (phase != 4) return phase;
    if (!gabi::call<s32>(0x025D63E8, statue, ptr(0x023B6028), 0xCC0)) return 5;
    u32 model = gabi::ea(statue->mModel.get());
    statue->cullMtx = model ? model + 0xC8 : 0;
    vtil_init_mtx(statue);
    gabi::call(0x025D674C, statue, -50.0f, 0.0f, -50.0f, 50.0f, 160.0f, 50.0f);
    vtil_init_co(statue);
    vtil_init_bgc(statue);
    statue->gravity = -6.0f;
    vtil_renew_attention(statue);
    u32 attention = read<u32>(address(statue, 0x39C));
    f32 y = statue->current.pos.y, z = statue->current.pos.z;
    write<u32>(address(statue, 0x39C), attention | 0x10);
    write<u8>(address(statue, 0x38C), 0x17);
    statue->eyePos.y = y + 110.0f;
    statue->eyePos.z = z;
    statue->mFirstGroundHit = 1;
    statue->eyePos.x = statue->current.pos.x;
    vtil_to_wait(statue);
    dKy_plight_set(&statue->mLight);
    statue->model = gabi::ea(statue->mModel.get());
    return 4;
}
VERIFY(0x023B6310, vtil_create);

BOOL vtil_delete(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B64BC, BOOL, statue);
    dKy_plight_cut(&statue->mLight);
    statue->model = 0;
    gabi::call(0x025204C8, &statue->mPhase, ptr(0x100332D0));
    return 1;
}
VERIFY(0x023B64BC, vtil_delete);

void vtil_set_sound(daObjVtil_c* statue, s32 sound, s32 mode) {
    WWHD_FUNC(0x023B6508, void, statue, sound, mode);
    gabi::Local<cXyz> position;
    position->x = f32(statue->current.pos.x);
    position->z = f32(statue->current.pos.z);
    u32 processID = read<u32>(address(statue, 4));
    position->y = f32(statue->current.pos.y);
    gabi::call(0x0255F458, position.get(), sound, processID, mode);
}
VERIFY(0x023B6508, vtil_set_sound);

void vtil_hit_co(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B654C, void, statue);
    statue->mCollisionStatus.Move();
    if (gabi::call<s32>(0x025160DC, &statue->mCylinder)) {
        gabi::call(0x02516094, &statue->mCylinder);
        statue->speedF = f32(statue->speedF) * 0.8f;
        return;
    }
    if (gabi::call<s32>(0x025162A4, &statue->mCylinder)) {
        gabi::call(0x023129C4, &statue->eyePos, s32(s8(statue->current.roomNo)), &statue->mCylinder, 13);
        vtil_set_sound(statue, 150, 5);
        gabi::call(0x02312E54, statue, &statue->mCylinder);
        gabi::call(0x0251621C, &statue->mCylinder);
    }
}
VERIFY(0x023B654C, vtil_hit_co);

BOOL vtil_check_sink(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6604, BOOL, statue);
    if (!(u32(statue->mAcch.m_flags) & 0x80000)) return 0;
    return f32(statue->mAcch.m_sea_height) > f32(statue->current.pos.y) + 70.0f;
}
VERIFY(0x023B6604, vtil_check_sink);

void vtil_make_splash(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B663C, void, statue);
    gabi::Local<cXyz> position;
    position->x = f32(statue->current.pos.x);
    position->y = f32(statue->mAcch.m_sea_height);
    position->z = f32(statue->current.pos.z);
    gabi::call(0x025DAE64, position.get(), 0, 1.0f, 0.75f);
}
VERIFY(0x023B663C, vtil_make_splash);

void vtil_se_splash(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B668C, void, statue);
    s32 material = 19;
    const u32 polygonOffsets[] = {0x52C, 0x4A0};
    for (u32 offset : polygonOffsets) {
        u32 polygon = address(statue, offset);
        if (read<u16>(polygon + 2) < 256) {
            u32 play = gabi::call<u32>(0x025200D4);
            material = gabi::call<s32>(0x024EECAC, ptr(play + 0x12A0), ptr(polygon));
            break;
        }
    }
    s32 reverb = gabi::call<s32>(0x02520540, s32(s8(statue->current.roomNo)));
    gabi::call(0x025E1A40, 0x6918, &statue->eyePos, material, reverb);
    vtil_set_sound(statue, 125, 5);
}
VERIFY(0x023B668C, vtil_se_splash);

void vtil_to_sink(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6770, void, statue);
    f32 forwardSquared = f32(statue->speedF) * f32(statue->speedF);
    u32 flags = statue->mAcch.m_flags;
    u32 collision = read<u32>(address(statue, 0x624));
    u32 attack = read<u32>(address(statue, 0x5F8));
    u32 target = read<u32>(address(statue, 0x610));
    write<u32>(address(statue, 0x624), collision | 1);
    f32 vertical = statue->speed.y;
    write<u32>(address(statue, 0x5F8), attack & ~1u);
    f32 squared = std::fma(vertical, vertical, forwardSquared);
    write<u32>(address(statue, 0x610), target | 1);
    statue->gravity = -2.0f;
    statue->mAcch.m_flags = ((flags | 8) & ~0x406u) | 0x2000;
    f64 magnitude = gabi::call<f64>(0x028F4384, squared);
    if (magnitude > 30.0) {
        f32 factor = f32(30.0 / magnitude);
        gabi::call(0x028E8E64, &statue->speed, &statue->speed, factor);
        statue->speedF = f32(statue->speedF) * factor;
    }
    vtil_make_splash(statue);
    vtil_se_splash(statue);
    u32 attention = read<u32>(address(statue, 0x39C));
    statue->mMode = 3;
    write<u32>(address(statue, 0x39C), attention & ~0x10u);
}
VERIFY(0x023B6770, vtil_to_sink);

void vtil_make_smoke(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6868, void, statue);
    gabi::call(0x02311CD8, statue, ptr(address(statue, 0x48C)), 1.0f);
}
VERIFY(0x023B6868, vtil_make_smoke);

void vtil_se_smoke(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6878, void, statue);
    u32 play = gabi::call<u32>(0x025200D4);
    s32 material = gabi::call<s32>(0x024EECAC, ptr(play + 0x12A0), ptr(address(statue, 0x4A0)));
    s32 reverb = gabi::call<s32>(0x02520540, s32(s8(statue->current.roomNo)));
    gabi::call(0x025E1A40, 0x6929, &statue->eyePos, material, reverb);
}
VERIFY(0x023B6878, vtil_se_smoke);

BOOL vtil_check_circle(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B68DC, BOOL, statue);
    u32 play = gabi::call<u32>(0x025200D4);
    u32 player = read<u32>(play + 0x5B2C);
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, ptr(player + 0x314), difference.get(), &statue->current.pos);
    gabi::Local<cXyz> horizontal;
    horizontal->x = f32(difference->x);
    horizontal->y = 0.0f;
    horizontal->z = f32(difference->z);
    f64 squared = gabi::call<f64>(0x028E8DD0, horizontal.get());
    f64 magnitude = gabi::call<f64>(0x028F4384, squared);
    return !(magnitude > 190.0);
}
VERIFY(0x023B68DC, vtil_check_circle);

void vtil_make_vib(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B695C, void, statue);
    u32 near = u32(vtil_check_circle(statue)) & 1;
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::Local<cXyz> direction;
    direction->x = 0.0f; direction->y = 1.0f; direction->z = 0.0f;
    gabi::call(0x025CB374, ptr(play + 0x599C), near + 1, 1, direction.get());
}
VERIFY(0x023B695C, vtil_make_vib);

BOOL vtil_check_sink_end(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B69BC, BOOL, statue);
    if (!(u32(statue->mAcch.m_flags) & 0x80000)) return 0;
    return f32(statue->mAcch.m_sea_height) > f32(statue->current.pos.y) + 210.0f;
}
VERIFY(0x023B69BC, vtil_check_sink_end);

void vtil_hit_bg(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B69F4, void, statue);
    u32 flags = statue->mAcch.m_flags;
    bool groundHit = (flags & 0x20) != 0;
    BOOL sinking = vtil_check_sink(statue);
    u32 mode = statue->mMode;
    if (mode == 0 || (mode == 2 && !(groundHit || (flags & 0x210)))) {
        if (sinking) vtil_to_sink(statue);
    } else if (mode == 2) {
        if (f32(statue->mPreviousVerticalSpeed) < f32(statue->gravity) - 1.0f) {
            statue->speedF = f32(statue->speedF) * 0.6f;
            return;
        }
        if (groundHit) {
            vtil_make_smoke(statue);
            vtil_se_smoke(statue);
            vtil_make_vib(statue);
        }
        statue->speedF = 0.0f;
        vtil_to_wait(statue);
        stopTracking(statue);
    } else if (mode == 3) {
        if (vtil_check_sink_end(statue)) {
            statue->mDeleteState = 1;
            return;
        }
        if (!sinking) {
            vtil_to_wait(statue);
            stopTracking(statue);
        }
    }
}
VERIFY(0x023B69F4, vtil_hit_bg);

BOOL vtil_execute(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6B70, BOOL, statue);
    vtil_hit_co(statue);
    vtil_hit_bg(statue);
    if (!u8(statue->mDeleteState)) {
        u32 mode = statue->mMode;
        if (mode >= 4) {
            gabi::call(0x0273AA24, ptr(0x10033264), 0x3EA, ptr(0x10033278));
            mode = statue->mMode;
        }
        u32 entry = 0x10033244 + mode * 8;
        u32 adjustedActor = gabi::ea(statue) + s32(read<s16>(entry));
        s32 selector = read<s16>(entry + 2);
        u32 target;
        if (selector < 0) target = read<u32>(entry + 4);
        else {
            s32 vtableOffset = read<s16>(entry + 6);
            u32 vtable = read<u32>(adjustedActor + vtableOffset);
            target = read<u32>(vtable + selector * 8 + 4);
        }
        gabi::call_ptr(target, ptr(adjustedActor));
        vtil_renew_attention(statue);
        vtil_init_mtx(statue);
        gabi::call(0x025165A4, &statue->mCylinder, &statue->current.pos);
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::call(0x0200E240, ptr(play + 0x26A4), &statue->mCylinder);
        f32 y = statue->current.pos.y, z = statue->current.pos.z;
        statue->mLight.mColorB = 0x34;
        statue->mLight.mPos.z = z;
        statue->mLight.mPower = 230.0f;
        statue->mLight.mFluctuation = 250.0f;
        statue->mLight.mColorR = 0x94;
        statue->mLight.mColorG = 0x7F;
        statue->mLight.mPos.y = y + 70.0f;
        statue->mLight.mPos.x = f32(statue->current.pos.x);
    }
    if (u8(statue->mDeleteState) == 1) {
        gabi::call(0x025D57E0, statue);
        statue->mDeleteState = 2;
    }
    return 1;
}
VERIFY(0x023B6B70, vtil_execute);

BOOL vtil_draw(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6D44, BOOL, statue);
    u32 environment = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, ptr(environment), 0, &statue->current.pos, &statue->tevStr);
    environment = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, ptr(environment), statue->mModel.get(), &statue->tevStr);
    gabi::call(0x025E2DE0, statue->mModel.get(), 0);
    return 1;
}
VERIFY(0x023B6D44, vtil_draw);

void vtil_calc_throw(daObjVtil_c* statue, be<f32>* gravityOut, be<f32>* linearOut, be<f32>* angularOut) {
    WWHD_FUNC(0x023B6DA0, void, statue, gravityOut, linearOut, angularOut);
    f32 linear = 0.002f, angular = 0.0002f, gravity = -6.0f;
    if (u32(statue->mAcch.m_flags) & 0x80000) {
        f32 depth = f32(statue->current.pos.y) - f32(statue->mAcch.m_sea_height);
        f32 blend = 0.0f;
        if (depth < 0.0f) {
            if (!(depth > -160.0f)) blend = 0.5f;
            else blend = -(depth * 0.003125f);
        }
        f32 rest = 1.0f - blend;
        linear = std::fma(blend, 0.2f, rest * 0.002f);
        angular = std::fma(blend, 0.02f, rest * 0.0002f);
        gravity = std::fma(blend, 4.0f, -6.0f);
    }
    *linearOut = linear;
    *angularOut = angular;
    *gravityOut = gravity;
}
VERIFY(0x023B6DA0, vtil_calc_throw);

void vtil_to_carry(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6EA8, void, statue);
    u32 collision = read<u32>(address(statue, 0x624));
    u32 attack = read<u32>(address(statue, 0x5F8));
    u32 target = read<u32>(address(statue, 0x610));
    u32 attention = read<u32>(address(statue, 0x39C));
    write<u32>(address(statue, 0x624), collision & ~1u);
    write<u32>(address(statue, 0x610), target | 1);
    write<u32>(address(statue, 0x39C), attention & ~0x10u);
    statue->mMode = 1;
    write<u32>(address(statue, 0x5F8), attack & ~1u);
}
VERIFY(0x023B6EA8, vtil_to_carry);

void vtil_mode_wait(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6EE4, void, statue);
    if (u32(statue->actor_status) & 0x2000) {
        vtil_to_carry(statue);
        statue->mPreviousVerticalSpeed = f32(statue->speed.y);
        correctBackground(statue);
        return;
    }
    u32 flags = statue->mAcch.m_flags;
    if (flags & 0x80) {
        if (!u8(statue->mFirstGroundHit)) {
            vtil_make_smoke(statue);
            vtil_se_smoke(statue);
            vtil_make_vib(statue);
        } else statue->mFirstGroundHit = 0;
        flags = statue->mAcch.m_flags;
        statue->mAcch.m_flags = flags & ~0x80u;
    }
    u32 attention = read<u32>(address(statue, 0x39C));
    write<u32>(address(statue, 0x39C), (flags & 0x20) ? attention | 0x10 : attention & ~0x10u);
    gabi::call(0x025D6870, statue, &statue->mCollisionStatus);
    statue->mPreviousVerticalSpeed = f32(statue->speed.y);
    correctBackground(statue);
}
VERIFY(0x023B6EE4, vtil_mode_wait);

void vtil_to_throw(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B6FE0, void, statue);
    gabi::call<u32>(0x025200D4);
    u32 collision = read<u32>(address(statue, 0x624));
    u32 flags = statue->mAcch.m_flags;
    u32 attack = read<u32>(address(statue, 0x5F8));
    u32 target = read<u32>(address(statue, 0x610));
    statue->speed.y = 27.0f;
    statue->gravity = -6.0f;
    write<u32>(address(statue, 0x5F8), attack | 1);
    write<u32>(address(statue, 0x624), collision | 1);
    write<u32>(address(statue, 0x39C), read<u32>(address(statue, 0x39C)) & ~0x10u);
    statue->speedF = 36.0f;
    write<u32>(address(statue, 0x610), target | 1);
    statue->mAcch.m_flags = (flags & ~0x40Eu) | 0x2000;
    statue->mMode = 2;
}
VERIFY(0x023B6FE0, vtil_to_throw);

void vtil_mode_carry(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B707C, void, statue);
    f32 z = statue->current.pos.z, y = statue->current.pos.y, x = statue->current.pos.x;
    f32 speedX = statue->speed.x, speedY = statue->speed.y, speedZ = statue->speed.z;
    f32 vertical = speedY;
    if (!(u32(statue->actor_status) & 0x2000)) {
        if (f32(statue->speedF) > 0.0f) vtil_to_throw(statue);
        else vtil_to_wait(statue);
        vertical = statue->speed.y;
    }
    statue->mPreviousVerticalSpeed = vertical;
    correctBackground(statue);
    statue->speed.y = speedY;
    statue->current.pos.x = x;
    statue->speed.x = speedX;
    statue->speed.z = speedZ;
    statue->current.pos.y = y;
    statue->current.pos.z = z;
}
VERIFY(0x023B707C, vtil_mode_carry);

void vtil_mode_throw(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B71A0, void, statue);
    gabi::Local<be<f32>> gravity, linear, angular;
    vtil_calc_throw(statue, gravity.get(), linear.get(), angular.get());
    f32 gravityValue = *gravity, linearValue = *linear, angularValue = *angular;
    statue->gravity = gravityValue;
    gabi::call(0x023123C0, statue, &statue->mCollisionStatus, ptr(0x101FFBA8), linearValue, angularValue);
    statue->mPreviousVerticalSpeed = f32(statue->speed.y);
    correctBackground(statue);
}
VERIFY(0x023B71A0, vtil_mode_throw);

void vtil_mode_sink(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B7210, void, statue);
    gabi::call(0x023123C0, statue, &statue->mCollisionStatus, ptr(0x101FFBA8), 0.2f, 0.02f);
    statue->mPreviousVerticalSpeed = f32(statue->speed.y);
    correctBackground(statue);
}
VERIFY(0x023B7210, vtil_mode_sink);

s32 vtil_method_create(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B7270, s32, statue);
    return vtil_create(statue);
}
VERIFY(0x023B7270, vtil_method_create);
BOOL vtil_method_delete(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B7274, BOOL, statue);
    return vtil_delete(statue);
}
VERIFY(0x023B7274, vtil_method_delete);
BOOL vtil_method_execute(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B7278, BOOL, statue);
    return vtil_execute(statue);
}
VERIFY(0x023B7278, vtil_method_execute);
BOOL vtil_method_draw(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B727C, BOOL, statue);
    return vtil_draw(statue);
}
VERIFY(0x023B727C, vtil_method_draw);

void vtil_static_init() {
    WWHD_FUNC(0x023B7280, void);
    write<u32>(0x1046C95C, 0);
    write<u32>(0x1046C960, 0);
    write<u32>(0x1046C964, 0);
    write<u32>(0x1046C968, 0);
    gabi::call(0x028F026C, ptr(0x101CDCD0));
    write<f32>(0x1046C950, -3.1415927410125732f);
    write<f32>(0x1046C954, 3.1415927410125732f);
    gabi::call(0x028ED6F8, ptr(0x1046C958));
    gabi::call(0x028F026C, ptr(0x101CDCDC));
    gabi::call(0x028EAB2C, ptr(0x1046C959));
    gabi::call(0x028F026C, ptr(0x101CDCE8));
}
VERIFY(0x023B7280, vtil_static_init);

void vtil_static_destruct(void* object, u32 flags) {
    WWHD_FUNC(0x023B7314, void, object, flags);
    if (object && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x023B7314, vtil_static_destruct);

void vtil_destruct(daObjVtil_c* statue, u32 flags) {
    WWHD_FUNC(0x023B7328, void, statue, flags);
    if (!statue) return;
    gabi::call(0x02515A70, &statue->mCylinder, 2);
    gabi::call(0x02515860, &statue->mCollisionStatus, 2);
    gabi::call(0x02018034, ptr(address(statue, 0x590)), 2);
    write<u32>(address(statue, 0x3D8), 0x1003319C);
    write<u32>(address(statue, 0x3CC), 0x100331AC);
    gabi::call(0x024EFD9C, &statue->mAcch, 0);
    gabi::call(0x025D50BC, statue, 0);
    if (flags & 1) gabi::call(0x0273AF40, statue);
}
VERIFY(0x023B7328, vtil_destruct);

void vtil_empty_virtual(void* object) {
    WWHD_FUNC(0x023B73C4, void, object);
}
VERIFY(0x023B73C4, vtil_empty_virtual);
BOOL vtil_is_delete(daObjVtil_c* statue) {
    WWHD_FUNC(0x023B73C8, BOOL, statue);
    return 1;
}
VERIFY(0x023B73C8, vtil_is_delete);
