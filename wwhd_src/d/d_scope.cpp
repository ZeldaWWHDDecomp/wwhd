/* d_scope: the telescope message process (SCP), WWHD. 
 * Translation unit 025BB024..025BB0DF, from the image: after d_save_init (only its __sinit 025BAF90),
 * before d_seafightgame (dSeaFightGame_info_c::checkPutShip 025BB0E0).
 *
 * HD-changed: the whole GameCube telescope screen (J2D screens, button icons, wipe, messages) is
 * gone. Only the five process methods of g_profile_SCP remain, as constants (method table 101EAE20:
 * create, delete, execute, isDelete, draw), plus the header __sinit every game TU gets. */
#include "bindings.h"

namespace d_scope_cpp {

/* 025BB024: dScp_Create -> cPhs_COMPLEATE_e (4) */
static s32 dScp_Create(void* i_this) { WWHD_FUNC(0x025BB024, s32, i_this); return 4; }
VERIFY(0x025BB024, dScp_Create);
/* 025BB02C: dScp_Delete -> TRUE */
static s32 dScp_Delete(void* i_this) { WWHD_FUNC(0x025BB02C, s32, i_this); return 1; }
VERIFY(0x025BB02C, dScp_Delete);
/* 025BB034: dScp_Execute -> TRUE */
static s32 dScp_Execute(void* i_this) { WWHD_FUNC(0x025BB034, s32, i_this); return 1; }
VERIFY(0x025BB034, dScp_Execute);
/* 025BB03C: dScp_Draw -> TRUE (method table slot +0x10) */
static s32 dScp_Draw(void* i_this) { WWHD_FUNC(0x025BB03C, s32, i_this); return 1; }
VERIFY(0x025BB03C, dScp_Draw);
/* 025BB044: dScp_IsDelete -> TRUE (method table slot +0xC) */
static s32 dScp_IsDelete(void* i_this) { WWHD_FUNC(0x025BB044, s32, i_this); return 1; }
VERIFY(0x025BB044, dScp_IsDelete);

/* 025BB04C __sinit: header statics block at 1047C948 ({-pi, pi} from 10054560, two one-byte objects
 * at +8/+9, zeroed 16-byte object at +0xC), each registered for destruction (records 101EAE34, +0xC, +0x18) */
static void d_scope_sinit() {
    WWHD_FUNC(0x025BB04C, void);
    gabi::store<u32>(0x1047C95C, 0); gabi::store<u32>(0x1047C954, 0);
    gabi::store<u32>(0x1047C960, 0); gabi::store<u32>(0x1047C958, 0);
    gabi::call(0x028F026C, 0x101EAE34u);
    f32 lo = gabi::load<f32>(0x10054560), hi = gabi::load<f32>(0x10054564);
    gabi::store<f32>(0x1047C948, lo); gabi::store<f32>(0x1047C94C, hi);
    gabi::call(0x028ED6F8, 0x1047C950u); gabi::call(0x028F026C, 0x101EAE40u);
    gabi::call(0x028EAB2C, 0x1047C951u); gabi::call(0x028F026C, 0x101EAE4Cu);
}
VERIFY(0x025BB04C, d_scope_sinit);

} // namespace d_scope_cpp
