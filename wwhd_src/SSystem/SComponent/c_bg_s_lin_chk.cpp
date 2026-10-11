/* Background line checks, GameCube names and layout, verified against WWHD. */
#include "SSystem/SComponent/c_bg_s_lin_chk.h"


/* 02008F64 */
void cBgS_LinChkNative::ct() {
    gabi::Local<cXyz> zero;
    f32 x = cXyz::Zero.x;
    f32 y = cXyz::Zero.y;
    zero->x = x;
    f32 z = cXyz::Zero.z;
    zero->y = y;
    zero->z = z;
    cM3dGLin_SetStartEnd(&mLin, zero.get(), zero.get());
    cBgS_vectorWords* source = (cBgS_vectorWords*)zero.get();
    cBgS_vectorWords* originalEnd = (cBgS_vectorWords*)&field_0x40;
    // HD: retain the compiler's word copy and its load/store order.
    u32 endZ = source->z;
    u32 endX = source->x;
    mFlag = 0;
    mBackFlag = 0;
    mActorPid = fpcM_ERROR_PROCESS_ID_e;
    originalEnd->x = endX;
    mFrontFlag = 1;
    u32 endY = source->y;
    originalEnd->z = endZ;
    originalEnd->y = endY;
}

/* 02008FEC */
cBgS_LinChkNative::cBgS_LinChkNative() {
    cBgS_Chk_construct(this);
    mpBgW = 0;
    mBackFlag = 0;
    mFlag = 0;
    // HD: GHS writes the base and embedded line vtables explicitly.
    cBgS_Chk::__vtbl = 0x10000F64;
    mPolyIndex = 0xFFFF;
    mBgIndex = 0x100;
    mActorId = fpcM_ERROR_PROCESS_ID_e;
    mPreRoofChk = 0;
    mLin.__vtbl = 0x10000F44;
    cBgS_PolyInfo::__vtbl = 0x10000F74;
    mFrontFlag = 0;
    mPreGroundChk = 0;
    mPreWallChk = 0;
    ct();
}

/* 0200909C */
void cBgS_LinChkNative::Set2(cXyz* pi_start, cXyz* pi_end, fpc_ProcID pi_actor) {
    cM3dGLin_SetStartEnd(&mLin, pi_start, pi_end);
    cBgS_vectorWords* end = (cBgS_vectorWords*)pi_end;
    cBgS_vectorWords* originalEnd = (cBgS_vectorWords*)&field_0x40;
    // HD: endpoint and object can overlap; keep the interleaved word copy.
    u32 x = end->x;
    originalEnd->x = x;
    u32 y = end->y;
    u32 flags = mFlag;
    originalEnd->y = y;
    u32 z = end->z;
    mActorPid = pi_actor;
    mpBgW = 0;
    mActorId = fpcM_ERROR_PROCESS_ID_e;
    mFlag = flags & ~HIT_e;
    originalEnd->z = z;
    mPolyIndex = 0xFFFF;
    mBgIndex = 0x100;
}


/* ---- HD: compiler-generated header statics, including three neighboring TUs ---- */
/* 02009130 */
static void __sinit_c_bg_s_lin_chk_cpp() {
    sinit_header_statics_rodata(0x101FF45C, 0x1018C5DC, 0x101FF468, 0x10000F8C);
}

/* 020091C4 */
static void __sinit_c_bg_s_linked_header_a() {
    sinit_header_statics_rodata(0x101FF478, 0x1018C600, 0x101FF484, 0x10000FC4);
}

/* 02009258 */
static void __sinit_c_bg_s_linked_header_b() {
    sinit_header_statics_rodata(0x101FF494, 0x1018C624, 0x101FF4A0, 0x10000FEC);
}

/* 020092EC */
static void __sinit_c_bg_s_linked_header_c() {
    sinit_header_statics_rodata(0x101FF4B0, 0x1018C648, 0x101FF4BC, 0x10000FF8);
}

/* WWHD glue
ext 02018808 void cM3dGLin_SetStartEnd(cM3dGLin* line, const cXyz* start, const cXyz* end)
ext 02008B60 void* cBgS_Chk_construct(cBgS_Chk* object)
*/
