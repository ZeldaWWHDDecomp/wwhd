// Projection44 times affine-view34, preserving the single-precision operation order.
#include "agl/ProjectionProduct.h"
#include <atomic>
using namespace agl::env;

/* 0272D680 */
void ProjectionProduct::setProduct(const sead::RenderMatrix44* projection,const sead::RenderMatrix34* view) {
    gabi::AutoFrame frame{gabi::current_function()};
    // All input words precede the first output store, allowing overlapping matrices.
    f32 left[4][4];
    f32 right[3][4];
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column) left[row][column] = projection->mRows[row][column];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column) right[row][column] = view->mRows[row][column];
    for (u32 row = 0; row < 4; ++row) {
        for (u32 column = 0; column < 4; ++column) {
            f32 secondTerm = gabi::fmuls_ppc(left[row][1],right[1][column]);
            f32 firstTwo = gabi::fmadds(left[row][0],right[0][column],secondTerm);
            f32 result = gabi::fmadds(left[row][2],right[2][column],firstTwo);
            if (column == 3) result = gabi::fadds_ppc(result,left[row][3]);
            mRows[row][column] = result;
        }
    }
    std::atomic_thread_fence(std::memory_order_acquire);
}

/* WWHD glue
*/
