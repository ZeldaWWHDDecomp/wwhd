/**
 * d_file_error.cpp
 * File error - custom screen and picture pane helpers.
 *
 * WWHD, written in GameCube decompilation style and verified against the
 * recompiled original (unit d_file_error).
 */
#include "d/d_file_error.h"

/* 0259CF48 */
void MyScreen::calcMtx() {
    // HD: makeMatrix is dispatched unconditionally; keep the target load before the bounds.
    u32 target = __vtbl->makeMatrix;
    f32 y = mBounds.i.y;
    f32 x = mBounds.i.x;
    gabi::call_ptr<void>(target, this, x, y);
}

// The GameCube header reference is expressed as its guest pointer for the entry ABI.
/* 0259D008 */
J2DPane* MyScreen::createPane(const J2DScrnBlockHeader* pHeader, JSURandomInputStream* pStream, J2DPane* pParent) {
    if (pHeader->mMagic != 'PIC1') {
        return J2DScreen::createPane(pHeader, pStream, pParent);
    }

    // HD: GHS inlines MyPicture's constructor after the allocation.
    MyPicture* picture = (MyPicture*)operator_new(sizeof(MyPicture));
    if (picture != nullptr) {
        J2DPicture__ct(picture, pParent, pStream);
        picture->m134 = 0;
        picture->__vtbl = gabi::at<J2DPane::VTable>(MyPicture::__vt);
    }
    return picture;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0259D0B8 */
static void MyPicture_dtor(MyPicture* i_this, u32 flags) {
    if (i_this != nullptr) {
        J2DPicture__dt(i_this, 0);
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/
