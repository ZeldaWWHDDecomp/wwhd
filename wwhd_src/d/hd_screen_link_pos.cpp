#include "d/hd_gamepad_map.h"
using namespace gabi;
static f32 fdivs(f32 a, f32 b) { return f32(f64(a) / f64(b)); }
static f32 l_hdLinkPosGridSide;
static f32 l_hdLinkPosHalf;

/* 02684698 */
void hdLinkPosGrid(void *self, HdPadPane *pane, HdPadPoint *size, HdPadPoint *positions) {
    f32 width = fdivs(pane->mWidth, l_hdLinkPosGridSide);
    f32 height = fdivs(pane->mHeight, l_hdLinkPosGridSide);
    size->x = width;
    size->y = height;
    f32 half = l_hdLinkPosHalf, left = -fmuls_ppc(pane->mWidth, half),
        top = fmuls_ppc(pane->mHeight, half);
    for (u32 row = 0; row < HD_PAD_GRID_SIDE; ++row)
        for (u32 col = 0; col < HD_PAD_GRID_SIDE; ++col) {
            f32 x = fadds_ppc(left, fmadds(f32(f64(col)), width, fmuls_ppc(width, half)));
            f32 y = fsubs_ppc(top, fmadds(f32(f64(row)), height, fmuls_ppc(height, half)));
            HdPadPoint *out = &positions[row * HD_PAD_GRID_SIDE + col];
            out->x = x;
            out->y = y;
        }
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdLinkPosGridSide 100F93D8
data l_hdLinkPosHalf 100F93DC
*/
