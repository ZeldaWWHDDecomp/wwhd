/* Environment process manager. */
#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/SSystem/c_malloc.h"
#include "gc_api/f_pc/f_pc.h"
#include "f_op/f_op_kankyo.h"
// HD: guest callback entry point, passed through without being called here.
using fopKyM_CreateFunc = u32;
// HD: GameCube uses process 0x1C7 for the water pillar.
enum : s32 { fpcNm_WPILLAR_e = 0x1C1 };
namespace dPa_name { enum { ID_IT_SN_YOGAN_HANE00 = 0x80D5 }; }

kankyo_class* fpcEx_SearchByID(u32);
BOOL fopKy_IsKankyo(kankyo_class*);
void cLib_memSet(void*, s32, u32);
u32 fpcM_Delete(void*);
u32 fpcSCtRq_Request(layer_class*, s32, fopKyM_CreateFunc, u32, void*);
base_process_class* fpcM_FastCreate(s32, fopKyM_CreateFunc, u32, fopKyM_prm_class*);

struct KankyoMngHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct KankyoMngPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(KankyoMngHeaderStatics, zero, 0xC);
WWHD_SIZE(KankyoMngHeaderStatics, 0x1C);
void KankyoMngFirstHeaderObject_ct(u8*);
void KankyoMngSecondHeaderObject_ct(u8*);
