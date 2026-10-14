/* HD controller orientation matrices and cursor chase helpers. No GameCube counterpart. */
#pragma once
#include "d/hd_input_ctrl.h"
#include "d/hd_input_cursor.h"
struct hdOrientationMatrix_c {
    union { be<f32> mElements[3][3]; be<u32> mBits[3][3]; };
    void setColumn(u32 column, hdInputVector* vector);
    void setColumns(hdInputVector* first, hdInputVector* second, hdInputVector* third);
    void copy(hdOrientationMatrix_c* source);
    void multiply(hdOrientationMatrix_c* left, hdOrientationMatrix_c* right);
    void identity();
    void inverse(hdOrientationMatrix_c* source);
};
WWHD_SIZE(hdOrientationMatrix_c, 0x24);
WWHD_OFFSET(hdOrientationMatrix_c, mElements, 0);
