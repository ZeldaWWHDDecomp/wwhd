#pragma once
#include "gc_api/gc_api.h"
namespace daObj { u32 PrmAbstract(const fopAc_ac_c*, u32, u32); }
// HD: local cBgW view for priority and correction callback fields.
struct AkabeBgW : dBgW {
    u8 _04[0x75 - 4];
    be<u8> mPriority;
    u8 _76[0xA8 - 0x76];
    be<u32> mpCrrFunc;
    void SetCrrFunc(u32 callback) { mpCrrFunc = callback; }
    void SetPriority(u8 priority) { mPriority = priority; }
};
WWHD_OFFSET(AkabeBgW, mPriority, 0x75);
WWHD_OFFSET(AkabeBgW, mpCrrFunc, 0xA8);
struct AkabeCullAttr { be<f32> lowX, lowY, highZ, highY; };
namespace daObjAkabe {
class Act_c : public fopAc_ac_c {
public:
    // HD: actor construction writes this TU's virtual-destructor table.
    Act_c() { fopAc_ac_c_ct(this); __vtbl = 0x10024A78; }
    enum Prm_e {
        PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0,
        PRM_SCL_W = 2, PRM_SCL_S = 8,
        PRM_ALWAYS_W = 1, PRM_ALWAYS_S = 12,
        PRM_ARG0_W = 4, PRM_ARG0_S = 16,
    };
    s32 prm_get_swSave() const { return daObj::PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }
    s32 prm_get_scl() const { return daObj::PrmAbstract(this, PRM_SCL_W, PRM_SCL_S); }
    s32 prm_get_always() const { return daObj::PrmAbstract(this, PRM_ALWAYS_W, PRM_ALWAYS_S); }
    s32 prm_get_arg0() const { return daObj::PrmAbstract(this, PRM_ARG0_W, PRM_ARG0_S); }
    bool chk_appear();
    void init_scale();
    void init_mtx();
    u8 create_heap();
    cPhs_State _create();
    bool _delete();
    bool _execute();
    request_of_phase_process_class mPhs;
    gptr<AkabeBgW> mpBgW;
    Mtx34 mMtx;
    be<s32> mType;
    be<u8> mbAppear;
    u8 _3ED[3];
};
}
WWHD_OFFSET(daObjAkabe::Act_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjAkabe::Act_c, mpBgW, 0x3B4);
WWHD_OFFSET(daObjAkabe::Act_c, mMtx, 0x3B8);
WWHD_OFFSET(daObjAkabe::Act_c, mType, 0x3E8);
WWHD_OFFSET(daObjAkabe::Act_c, mbAppear, 0x3EC);
WWHD_SIZE(daObjAkabe::Act_c, 0x3F0);
u32 Akabe_create_heap_result(daObjAkabe::Act_c*);
u32 Akabe_chk_appear_result(daObjAkabe::Act_c*);
