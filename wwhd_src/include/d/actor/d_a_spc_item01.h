/* Special event items, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"
#include "d/actor/d_a_itembase.h"

// HD: item-base tail fields are shifted by 0x114 after the removed shadow ID.
struct daSpcItem01_c : daItemBase_c {
    daSpcItem01_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x10012158;
        dBgS_ObjAcch_ct(&mAcch, {0x1003D03C, 0x1003D05C, 0x1003D04C});
        dBgS_AcchCir_ct(&mAcchCir);
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x1003D02C);
        __vtbl = 0x1003D0A8;
    }
    // HD: DrawBase is dispatched through the HD virtual table.
    BOOL drawBaseVirtual() { return gabi::call_ptr<BOOL>(vfn(daItemBase_VT_DRAWBASE), this); }
    void set_mtx();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateInit();
    BOOL _execute();
    void set_effect();
    void scale_anim();
    void anim_play();
    void move();
    void rotate_item();
    BOOL _draw();
    void setTevStr();

    /* 0x750 */ be<f32> field_0x63C;
    /* 0x754 */ be<s16> field_0x640;
    /* 0x756 */ be<u8> field_0x642;
    /* 0x757 */ be<u8> field_0x643;
    /* 0x758 */ be<u16> field_0x644;
    /* 0x75A */ be<u8> field_0x646;
    /* 0x75B */ be<u8> field_0x647;
};
WWHD_OFFSET(daSpcItem01_c, field_0x63C, 0x750);
WWHD_OFFSET(daSpcItem01_c, field_0x644, 0x758);


enum { dItemNo_JOY_PENDANT_e = 0x1F, dItemNo_SHIELD_e = 0x3B, dItemNo_KNIGHTS_CREST_e = 0x48 };
enum { TEV_TYPE_BG1 = 2 };
namespace daSpcItem01_prm {
inline u8 getItemNo(daSpcItem01_c* p) { return fopAcM_GetParam(p) & 0xFF; }
inline u16 getFlag(daSpcItem01_c* p) { return fopAcM_GetParam(p) >> 8 & 0xFFFF; }
}
namespace dItem_data {
struct FieldResource_view { gptr<const char> mArcName; u8 rest[0x18]; };
struct ItemResource_view { u8 rest[0x20]; be<u16> mHeapSize; u8 pad[2]; };
inline const char* getFieldArc(u32 no) { return gabi::at<FieldResource_view>(field_item_res(no))->mArcName; }
inline u16 getHeapSize(u32 no) { return gabi::at<ItemResource_view>(item_resource(no))->mHeapSize; }
inline BOOL checkAppearEffect(u8 no) { return gabi::call<BOOL>(0x02551B90, no); }
inline u16 getAppearEffect(u8 no) { return gabi::call<u16>(0x02551BBC, no); }
}
// HD: callback implemented in the item-base unit; external callbacks are guest addresses.
static constexpr u32 CheckFieldItemCreateHeap = 0x0218422C;
inline void __dt(daItemBase_c* p) { gabi::call(0x021832F4, p, 0); }
struct SpcItemEnv_view {
 void settingTevStruct(s32 type, cXyz* pos, dKy_tevstr_c* tev) { ::settingTevStruct((dScnKy_env_light_c*)this,type,pos,tev); }
 void setLightTevColorType(J3DModel* model, dKy_tevstr_c* tev) { ::setLightTevColorType((dScnKy_env_light_c*)this,model,tev); }
};

// HD: load all scale components before storing, preserving overlapping-object behavior.
struct SpcItemModel_view : J3DModel {
    void setBaseScale(const cXyz& scale) {
        f32 x = scale.x, y = scale.y, z = scale.z;
        mBaseScale.x = x;
        mBaseScale.y = y;
        mBaseScale.z = z;
    }
};
