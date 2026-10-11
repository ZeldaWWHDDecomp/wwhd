#include "d/hd_gamepad_map.h"
using namespace gabi;
static f32 fdivs(f32 a, f32 b) { return f32(f64(a) / f64(b)); }
static HdPadSavePointer l_hdGridSave;
static f32 l_hdGridZero;
static f32 l_hdGridHalf;
static HdPadIdTable l_gridIds;

/* 02680DE0 */
HdPadMapCell *hdGridFind(HdPadCellArray *cells, HdPadCoordinate *coordinate) {
    u32 x = u32(s32(s8(coordinate->x)));
    for (u32 i = 0; i < HD_PAD_GRID_CELLS; ++i) {
        HdPadMapCell *cell = cells->mCells[i];
        if (u32(s32(s8(cell->mCoordinate.x))) != x)
            continue;
        u32 y = u32(s32(s8(coordinate->y)));
        if (u32(s32(s8(cell->mCoordinate.y))) == y)
            return cells->mCells[i];
    }
    return 0;
}
/* 02680E68 */
u32 hdGridLookup(HdPadGrid *self, u32 value) {
    for (u32 i = 0; i < 11; ++i)
        if (l_gridIds.mIds[i] == value)
            return i;
    return 0xFFFFFFFFu;
}
/* 026810A8 */
void hdGridBuildGrid(HdPadGrid *self) {
    // HD: retain original frame offsets and uninitialized callback storage.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x90> frame;
    hdPadGridSetup(self);
    hdPadGridSetupPanes(self);
    f32 originX = self->mOriginX, originY = self->mOriginY;
    HdPadPane *pane = self->mOwner->mPane;
    Pair32 scaleBits{hdPadPaneScale(pane)};
    HdPadFloatBits xBits{scaleBits.r3};
    HdPadFloatBits yBits{scaleBits.r4};
    f32 scaleX = xBits.mValue, scaleY = yBits.mValue;
    f32 sizeX = fmuls_ppc(self->mCellSize.x, scaleX), sizeY = fmuls_ppc(self->mCellSize.y, scaleY);
    f32 offsetX = l_hdGridZero, offsetY = offsetX;
    if (self->mOffsetMode == 1) {
        offsetX = fmuls_ppc(self->mOffset.x, scaleX);
        offsetY = fmuls_ppc(self->mOffset.y, scaleY);
    }
    HdPadCoordinate *coordinate = gabi::at<HdPadCoordinate>(frame + 8);
    f32 half = l_hdGridHalf;
    for (u32 row = 0; row < HD_PAD_GRID_SIDE; ++row)
        for (u32 col = 0; col < HD_PAD_GRID_SIDE; ++col) {
            coordinate->x = u8(s32(col) - HD_PAD_GRID_CENTER);
            coordinate->y = u8(s32(row) - HD_PAD_GRID_CENTER);
            HdPadMapCell *cell = hdGridFind(reinterpret_cast<HdPadCellArray *>(self), coordinate);
            if (!cell)
                continue;
            f32 width = self->mCellSize.x, height = self->mCellSize.y;
            f32 x = fadds_ppc(originX, fmadds(f32(f64(col)), width, fmuls_ppc(width, half)));
            f32 y = fsubs_ppc(originY, fmadds(f32(f64(row)), height, fmuls_ppc(height, half)));
            f32 left = fnmsubs(width, half, x), top = fnmsubs(height, half, y),
                right = fmadds(width, half, x), bottom = fmadds(height, half, y);
            cell->mPosition.x = x;
            cell->mPosition.y = y;
            cell->mSize.x = self->mCellSize.x;
            cell->mSize.y = self->mCellSize.y;
            cell->mBounds.mLeft = left;
            cell->mBounds.mTop = top;
            cell->mBounds.mRight = right;
            cell->mBounds.mBottom = bottom;
            x = fadds_ppc(fadds_ppc(originX, offsetX),
                          fmadds(f32(f64(col)), sizeX, fmuls_ppc(sizeX, half)));
            y = fsubs_ppc(fadds_ppc(originY, offsetY),
                          fmadds(f32(f64(row)), sizeY, fmuls_ppc(sizeY, half)));
            left = fnmsubs(sizeX, half, x);
            top = fnmsubs(sizeY, half, y);
            right = fmadds(sizeX, half, x);
            bottom = fmadds(sizeY, half, y);
            cell->mScaledPosition.x = x;
            cell->mScaledPosition.y = y;
            cell->mScaledSize.x = sizeX;
            cell->mScaledSize.y = sizeY;
            cell->mScaledBounds.mLeft = left;
            cell->mScaledBounds.mTop = top;
            cell->mScaledBounds.mRight = right;
            cell->mScaledBounds.mBottom = bottom;
            u32 id = cell->mId;
            cell->mBaseResource = self->mBaseResource;
            cell->mResources[0] = id + self->mResources[0];
            cell->mResources[1] = id + self->mResources[1];
            cell->mResources[2] = id + self->mResources[2];
            cell->mResources[4] = id + self->mResources[4];
            cell->mResources[5] = id + self->mResources[5];
            cell->mResources[6] = id + self->mResources[6];
            u32 index = hdGridLookup(self, id);
            if (s32(index) >= 0) {
                cell->mResources[3] = index + self->mResources[3];
                cell->mResources[7] = index + self->mResources[7];
            }
        }
}
/* 02681400 */
void hdGridCountDiscovered(HdPadGrid *self) {
    self->mDiscoveredCount = 0;
    for (u32 i = 0; i < HD_PAD_GRID_CELLS; ++i) {
        u32 yes = hdPadMapDiscovered(&l_hdGridSave.mSave->mMapData, i);
        HdPadMapCell *cell = self->mCells[i];
        cell->mDiscovered = yes ? 1 : 0;
        if (yes)
            self->mDiscoveredCount = self->mDiscoveredCount + 1;
    }
    if (self->mDiscoveredCount != self->mPreviousCount)
        self->mChanged = 1;
}
/* WWHD glue (tools/verify/autoglue.py)
data l_hdGridSave 101F84DC
data l_hdGridZero 100F920C
data l_hdGridHalf 100F9218
data l_gridIds 100F91E0
*/
