/* Seagull/jump minigame area marker. Private HD reconstruction. */
#include "d/actor/d_a_tag_so.h"
static daTag_So_HIO_c l_HIO;
static u32 l_redGuard;
static u32 l_otherGuard;
static GXColor l_red;
static GXColor l_other;
static GXColor l_redLiteral;
static GXColor l_otherLiteral;
/* 024B2C74 */
// HD: daTag_So_HIO_c::daTag_So_HIO_c (HD: allocates when this == NULL)
static daTag_So_HIO_c* daTag_So_HIO_c_ct(daTag_So_HIO_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daTag_So_HIO_c*)operator_new(0x10);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1003FC74;
    i_this->mNo = -1;
    i_this->m05 = 0;
    return i_this;
}

/* 024B2B10 */
void daTag_So_c::debugDraw() {
    gabi::Local<cXyz> actorPos;
    actorPos->x = current.pos.x;
    actorPos->y = current.pos.y + 20.0f;
    actorPos->z = current.pos.z;
    /* HD: the GXColor literals are function-local statics initialised on first use */

    if (l_redGuard == 0) {
        l_redGuard = 1;
        memcpy_g(&l_red, &l_redLiteral, sizeof(GXColor));
    }
    if (mType == 1) {
        /* the same colour static is checked again (inline accessor used twice) */
        if (l_redGuard == 0) {
            l_redGuard = 1;
            memcpy_g(&l_red, &l_redLiteral, sizeof(GXColor));
        }
        dLib_debugDrawFan(actorPos, shape_angle.y, 0x3500, mJumpRange, &l_red);
    } else {
        /* HD: a second, unused colour static is initialised on the other path */
        if (l_otherGuard == 0) {
            l_otherGuard = 1;
            memcpy_g(&l_other, &l_otherLiteral, sizeof(GXColor));
        }
    }
}

/* 024B2C3C */
bool daTag_So_c::_draw() {
    if (l_HIO.m05)
        debugDraw();
    return TRUE;
}

/* 024B2A30 */
void daTag_So_c::getArg() {
    u32 param = fopAcM_GetParam(this);
    mRndNum = param & 0xFF;
    s32 paramRadius = (param >> 8) & 0xFF;
    mType = (param >> 16) & 0xFF;
    if (paramRadius == 0xff) {
        mJumpRange = 1600.0f;
    } else {
        mJumpRange = (f32)(paramRadius * 100);
    }
}

/* 024B2A94 */
cPhs_State daTag_So_c::_create() {
    fopAcM_ct(this, daTag_So_c);
    getArg();
    return cPhs_COMPLEATE_e;
}

/* 024B2AFC */
static cPhs_State daTag_SoCreate(void* i_this) {
    return ((daTag_So_c*)i_this)->_create();
}

/* 024B2B00 */
// HD: _delete inlined
static BOOL daTag_SoDelete(void* i_this) {
    return TRUE;
}

/* 024B2B08 */
// HD: _execute inlined
static BOOL daTag_SoExecute(void* i_this) {
    return TRUE;
}

/* 024B2C70 */
static BOOL daTag_SoDraw(void* i_this) {
    return ((daTag_So_c*)i_this)->_draw();
}

/* 024B2D64 */
static BOOL daTag_SoIsDelete(void*) {
    return TRUE;
}

/* 024B2CC4 */
// HD: static initialisation (header statics, l_HIO)
static void __sinit_d_a_tag_so_cpp() {
    sinit_header_statics_z(0x1046E650, 0x101D2130, 0x1046E66C);
    daTag_So_HIO_c_ct(&l_HIO);
}

/* 024B2D6C */
// HD: daTag_So_c deleting destructor (compiler-generated, HD virtual destructor)
static void daTag_So_c_dt(daTag_So_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        tag_so_actor_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

daTag_So_c::daTag_So_c() {
    // HD: explicit actor base construction with this TU virtual table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
}
/* WWHD glue (tools/verify/autoglue.py): HD statics and private bindings
data l_HIO 1046E65C
data l_redGuard 101FDA50
data l_otherGuard 101FDAC4
data l_red 101FEBF4
data l_other 101FEBFC
data l_redLiteral 1003FC58
data l_otherLiteral 1003FC5C
ext 02587684 void dLib_debugDrawFan(cXyz*, s16, s16, f32, const GXColor*)
ext 025D50BC void tag_so_actor_dt(void*, s32)
*/
