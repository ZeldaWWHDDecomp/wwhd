// Master Sword pedestal sword, HD TU 023B37E0..023B3B90.
// Matcher 023B388C says check_demo; the instructions implement init_mtx.
#include "d/actor/d_a_obj_vmsms.h"
#include "bindings.h"
namespace {
template<class T> T read(u32 address) { return *gabi::at<be<T>>(address); }
template<class T> void write(u32 address, T value) { *gabi::at<be<T>>(address) = value; }
void* ptr(u32 address) { return gabi::at<void>(address); }
struct ArchiveName { be<u32> name, vtable; };
}
BOOL vmsms_create_heap(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B37E0, BOOL, sword);
    gabi::Local<ArchiveName> archive;
    archive->vtable = 0x10032ED4;
    archive->name = 0x10032F40;
    u32 data = gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(), 3);
    if (!data) {
        gabi::call(0x0273AA24, ptr(0x10032EFC), 0x5B, ptr(0x10032F10));
        return 0;
    }
    sword->mModel = gabi::call<J3DModel*>(0x025E38E0, ptr(data), 0, 0x11020203);
    return sword->mModel.get() != nullptr;
}
VERIFY(0x023B37E0, vmsms_create_heap);
BOOL vmsms_solidHeapCB(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3888, BOOL, sword);
    return vmsms_create_heap(sword);
}
VERIFY(0x023B3888, vmsms_solidHeapCB);
void vmsms_init_mtx(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B388C, void, sword);
    u32 model = gabi::ea(sword->mModel.get());
    f32 z = sword->scale.z;
    f32 x = sword->scale.x;
    f32 y = sword->scale.y;
    write<f32>(model + 0xBC, x);
    write<f32>(model + 0xC4, z);
    write<f32>(model + 0xC0, y);
}
VERIFY(0x023B388C, vmsms_init_mtx);
s32 vmsms_create(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B38AC, s32, sword);
    u32 condition = sword->actor_condition;
    if (!(condition & 8)) {
        if (sword) {
            fopAc_ac_c_ct(sword);
            condition = sword->actor_condition;
            sword->__vtbl = 0x10032EEC;
        }
        sword->actor_condition = condition | 8;
    }
    u32 save = read<u32>(0x101F84DC);
    if (gabi::call<s32>(0x025B8B94, ptr(save + 0x644), 0x2D04)) return 5;
    s32 phase = gabi::call<s32>(0x02520460, &sword->mPhs, ptr(0x10032F40));
    if (phase == 4) {
        if (!gabi::call<s32>(0x025D63E8, sword, ptr(0x023B3888), 0)) return 5;
        u32 model = gabi::ea(sword->mModel.get());
        sword->cullMtx = model ? model + 0xC8 : 0;
        vmsms_init_mtx(sword);
        return 4;
    }
    return phase;
}
VERIFY(0x023B38AC, vmsms_create);
bool vmsms_delete(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3990, bool, sword);
    gabi::call(0x025204C8, &sword->mPhs, ptr(0x10032F40));
    return true;
}
VERIFY(0x023B3990, vmsms_delete);
bool vmsms_execute(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B39C0, bool, sword);
    u32 play = gabi::call<u32>(0x025200D4);
    if (read<u8>(play + 0x5292)) {
        play = gabi::call<u32>(0x025200D4);
        if (gabi::call<s32>(0x025445B8, ptr(play + 0x52C4), ptr(0x10032F20)))
            gabi::call(0x025D57E0, sword);
    }
    return true;
}
VERIFY(0x023B39C0, vmsms_execute);
bool vmsms_draw(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3A20, bool, sword);
    auto light = dKy_getEnvlight();
    settingTevStruct(light, 0, &sword->current.pos, &sword->tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, sword->mModel.get(), &sword->tevStr);
    mDoExt_modelUpdateDL(sword->mModel.get(), 0);
    return true;
}
VERIFY(0x023B3A20, vmsms_draw);
s32 vmsms_Mthd_Create(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3A7C, s32, sword);
    return vmsms_create(sword);
}
VERIFY(0x023B3A7C, vmsms_Mthd_Create);
bool vmsms_Mthd_Delete(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3A80, bool, sword);
    return vmsms_delete(sword);
}
VERIFY(0x023B3A80, vmsms_Mthd_Delete);
bool vmsms_Mthd_Execute(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3A84, bool, sword);
    return vmsms_execute(sword);
}
VERIFY(0x023B3A84, vmsms_Mthd_Execute);
bool vmsms_Mthd_Draw(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3A88, bool, sword);
    return vmsms_draw(sword);
}
VERIFY(0x023B3A88, vmsms_Mthd_Draw);
void vmsms_staticInit() {
    WWHD_FUNC(0x023B3A8C, void);
    write<u32>(0x1046C8E0, 0);
    write<u32>(0x1046C8D8, 0);
    write<u32>(0x1046C8E4, 0);
    write<u32>(0x1046C8DC, 0);
    gabi::call(0x028F026C, ptr(0x101CDB30));
    write<f32>(0x1046C8CC, -3.1415927410125732f);
    write<f32>(0x1046C8D0, 3.1415927410125732f);
    gabi::call(0x028ED6F8, ptr(0x1046C8D4));
    gabi::call(0x028F026C, ptr(0x101CDB3C));
    gabi::call(0x028EAB2C, ptr(0x1046C8D5));
    gabi::call(0x028F026C, ptr(0x101CDB48));
}
VERIFY(0x023B3A8C, vmsms_staticInit);
void vmsms_staticDtor(void* object, u32 flags) {
    WWHD_FUNC(0x023B3B20, void, object, flags);
    if (object && (flags & 1)) gabi::call(0x0273AF40, object);
}
VERIFY(0x023B3B20, vmsms_staticDtor);
void vmsms_destructor(daObjVmsms_c* sword, u32 flags) {
    WWHD_FUNC(0x023B3B34, void, sword, flags);
    if (sword) {
        gabi::call(0x025D50BC, sword, 0);
        if (flags & 1) gabi::call(0x0273AF40, sword);
    }
}
VERIFY(0x023B3B34, vmsms_destructor);
void vmsms_staticNoop(void* object) { WWHD_FUNC(0x023B3B88, void, object); }
VERIFY(0x023B3B88, vmsms_staticNoop);
BOOL vmsms_IsDelete(daObjVmsms_c* sword) {
    WWHD_FUNC(0x023B3B8C, BOOL, sword);
    return 1;
}
VERIFY(0x023B3B8C, vmsms_IsDelete);
