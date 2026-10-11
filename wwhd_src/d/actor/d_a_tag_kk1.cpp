/* Tag - start of Mila's chase. GameCube source style; HD differences are marked below. */
#include "d/actor/d_a_tag_kk1.h"

const gc_tu_data gc_tu = {
    .objAcchDt = {0x1003F678, 0x1003F688},
    .headerStatics = {0x1046E54C, 0x101D1C9C, 0x1046E558},
};
static daTag_Kk1_HIO_c l_HIO;
static u32 a_prm_tbl[3] = {0x43AF0000, 0x41F00000, 0}; // 350.0f, 30.0f, debug off
static u32 l_debugColorReady;
static u8 l_debugColor[4] = {};
static u8 dummy5[4] = {0x00, 0xFF, 0x00, 0x80};

/* 024ACD04 */
static daTag_Kk1_HIO_c* daTag_Kk1_HIO_c__ct(daTag_Kk1_HIO_c* i_this) {
    // HD: GHS allocates when the constructor is called with null.
    if (!i_this) {
        i_this = static_cast<daTag_Kk1_HIO_c*>(operator_new(sizeof(daTag_Kk1_HIO_c)));
        if (!i_this) return nullptr;
    }
    i_this->__vtbl = daTag_Kk1_HIO_c::__vt;
    // HD: copy the complete parameter record as words, including padding.
    for (u32 i = 0; i < 3; ++i) i_this->prm.mWords[i] = a_prm_tbl[i];
    i_this->mNo = -1;
    return i_this;
}
/* 024ACC98 */
BOOL daTag_Kk1_c::_draw() {
    // HD: the debug path initializes a guarded color, though the drawing itself is absent.
    if (l_HIO.prm.field_0x10 && !l_debugColorReady) {
        l_debugColorReady = 1;
        memcpy_g(l_debugColor, dummy5, sizeof(dummy5));
    }
    return true;
}
/* 024ACBC0 */
BOOL daTag_Kk1_c::_execute() {
    // HD: retain sqrtf of PSVECSquareDistance; the no-argument cXyz::abs returns f64.
    f32 distance = std_sqrtf(tagKk1_squareDistance(&current.pos, &dComIfGp_getPlayer(0)->current.pos));
    f32 vert_distance = dComIfGp_getPlayer(0)->current.pos.y - current.pos.y;
    mTagSet = false;
    if (distance < l_HIO.prm.mHorizontalDistance && vert_distance < l_HIO.prm.mVerticalDistance) {
        s16 angle_deviation = dComIfGp_getPlayer(0)->shape_angle.y - current.angle.y;
        angle_deviation = s16(angle_deviation < 0 ? -angle_deviation : angle_deviation);
        if (angle_deviation < 0x1000) mTagSet = true;
    }
    return true;
}
/* 024ACB78 */
BOOL daTag_Kk1_c::_delete() {
    if (l_HIO.mNo >= 0) {
        mDoHIO_deleteChild(l_HIO.mNo);
        l_HIO.mNo = -1;
    }
    return TRUE;
}
/* 024ACAC0 */
cPhs_State daTag_Kk1_c::_create() {
    fopAcM_ct(this, daTag_Kk1_c);
    switch (fpcM_GetName(this)) {
    case fpcNm_TAG_KK1_e: mNameIsWrong = false; break;
    default: return cPhs_ERROR_e;
    }
    if (l_HIO.mNo < 0) {
        // HD: ASCII source label for the original Shift-JIS debug name; guest bytes stay unchanged.
        l_HIO.mNo = mDoHIO_createChild("Mila chase startup tag", &l_HIO);
    }
    // HD: createInit is inlined and the unused fpcM_GetName(&name_int) call is absent.
    return cPhs_COMPLEATE_e;
}
/* 024ACB74 */
static cPhs_State daTag_Kk1_Create(fopAc_ac_c* i_this) { return static_cast<daTag_Kk1_c*>(i_this)->_create(); }
/* 024ACBBC */
static BOOL daTag_Kk1_Delete(daTag_Kk1_c* i_this) { return i_this->_delete(); }
/* 024ACC94 */
static BOOL daTag_Kk1_Execute(daTag_Kk1_c* i_this) { return i_this->_execute(); }
/* 024ACCF8 */
static BOOL daTag_Kk1_Draw(daTag_Kk1_c* i_this) { return i_this->_draw(); }
/* 024ACCFC */
static BOOL daTag_Kk1_IsDelete(daTag_Kk1_c* i_this) { return TRUE; }
/* 024ACD68 */
static void __sinit_d_a_tag_kk1_cpp() {
    __sinit_header_statics();
    daTag_Kk1_HIO_c__ct(&l_HIO);
}
/* 024ACE08 */
static void daTag_Kk1_c__dt(daTag_Kk1_c* i_this, s32 flags) {
    if (!i_this) return;
    __dt(static_cast<fopNpc_npc_c*>(i_this));
    if (flags & 1) operator_delete(i_this);
}

/* WWHD glue
data l_HIO 1046E568 nocheck
data a_prm_tbl 101D1C90
data l_debugColorReady 101FDA48 nocheck
data l_debugColor 101FEBEC nocheck
data dummy5 1003F670
str 1003F6D0 "Mila chase startup tag" renamed in 024ACAC0
ext 028E8DE8 f32 tagKk1_squareDistance(const cXyz* first, const cXyz* second)
*/
