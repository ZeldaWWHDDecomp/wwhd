/* HD-only map cell tests, point chasing and icon pane placement. */
#include "d/hd_screen_btn_map_icon.h"

static HdMapManagerPointer l_mapManager;
static HdMapPoint l_bounds;
enum HdMapPaneFlags : u32 { HdMapPane_Visible = 1, HdMapPane_TranslateChanged = 0x10 };

/* 0263862C */
static f32 hdMap_chasePoint(HdMapPoint* point, HdMapPoint* target, f32 ratio, f32 maximum, f32 minimum) {
    f32 x = point->x;
    f32 y = point->y;
    f32 targetX = target->x;
    f32 targetY = target->y;
    if (x != targetX || y != targetY) {
        f32 dy = y - targetY;
        f32 dx = x - targetX;
        f32 length = hdMapSqrt(gabi::fmadds(dx, dx, dy * dy));
        bool snap = length < minimum;
        if (!snap) {
            f32 step = length * ratio;
            dx = dx * ratio;
            dy = dy * ratio;
            snap = step == 0.0f;
            if (!snap) {
                if (step > maximum || step < minimum) {
                    f32 bound = step > maximum ? maximum : minimum;
                    f32 scale = bound / step;
                    dy = dy * scale;
                    dx = dx * scale;
                }
                x = point->x - dx;
                y = point->y - dy;
                point->x = x;
                point->y = y;
                targetX = target->x;
                targetY = target->y;
            }
        }
        if (snap) {
            x = target->x;
            point->x = x;
            y = target->y;
            point->y = y;
            targetY = target->y;
            targetX = target->x;
        }
    }
    f32 dy = y - targetY;
    f32 dx = x - targetX;
    return hdMapSqrt(gabi::fmadds(dx, dx, dy * dy));
}

static u32 matchesCell(HdMapIcon* self, bool secondary) {
    HdMapIconOwner* owner = self->mpOwner;
    if (owner == NULL) return 0;
    HdMapInfo* info = owner->mpInfo;
    if (info == NULL) return 0;
    HdMapLocations* map = l_mapManager.mpManager->mpLocations;
    HdMapCell* cell = secondary ? &map->mSecondaryCell : &map->mPrimaryCell;
    return cell->x == info->mCell.x && cell->y == info->mCell.y;
}

/* 0263896C */
u32 HdMapIcon::matchesPrimaryCell() { return matchesCell(this, false); }
/* 02639184 */
u32 HdMapIcon::matchesSecondaryCell() { return matchesCell(this, true); }

static void mapPoint(HdMapPoint* out, bool secondary) {
    HdMapLocations* map = l_mapManager.mpManager->mpLocations;
    HdMapCell* cell = secondary ? &map->mSecondaryCell : &map->mPrimaryCell;
    HdMapPoint* position = secondary ? &map->mSecondaryPosition : &map->mPrimaryPosition;
    f32 cellX = (f32)cell->x;
    f32 cellY = (f32)cell->y;
    f32 x = position->x;
    f32 y = position->y;
    out->x = gabi::fnmsubs(cellX, 100000.0f, x);
    out->y = gabi::fnmsubs(cellY, 100000.0f, y);
}

/* 026389EC */
void HdMapIcon::getPrimaryMapPoint(HdMapPoint* out) { mapPoint(out, false); }
/* 02639204 */
void HdMapIcon::getSecondaryMapPoint(HdMapPoint* out) { mapPoint(out, true); }

/* 02638A8C */
u32 HdMapIcon::insideBounds(HdMapPoint* point) {
    f32 x = l_bounds.x;
    f32 y = l_bounds.y;
    f32 negativeX = -x;
    f32 negativeY = -y;
    f32 left, right, bottom, top;
    if (negativeX >= x) { left = x; right = negativeX; }
    else { left = negativeX; right = x; }
    if (negativeY >= y) { bottom = y; top = negativeY; }
    else { bottom = negativeY; top = y; }
    f32 scaleY = 1.0f;
    f32 scaleX = 1.0f;
    HdMapIconPane* pane = mpParentPane;
    if (pane != NULL) {
        scaleY = pane->mScaleY;
        scaleX = pane->mScaleX;
    }
    scaleY = 1.0f / (scaleY * 5.0f);
    scaleX = 1.0f / (scaleX * 5.0f);
    f32 centerX = (left + right) * 0.5f;
    f32 halfX = (right - left) * (scaleX * 0.5f);
    f32 centerY = (bottom + top) * 0.5f;
    f32 halfY = (top - bottom) * (scaleY * 0.5f);
    f32 dx = -(mPosition.x - mOrigin.x);
    f32 dy = -(mPosition.y - mOrigin.y);
    left = (centerX - halfX) + dx;
    right = (centerX + halfX) + dx;
    bottom = (centerY - halfY) + dy;
    top = (centerY + halfY) + dy;
    f32 pointX = point->x;
    f32 pointY = point->y;
    if (left > pointX || pointX > right || bottom > pointY || pointY > top) return 0;
    return 1;
}

static void relativePoint(HdMapIcon* self, HdMapPoint* out, bool secondary) {
    if (out == NULL) return;
    if (secondary) self->getSecondaryMapPoint(out);
    else self->getPrimaryMapPoint(out);
    HdMapIconOwner* owner = self->mpOwner;
    if (owner == NULL) return;
    HdMapInfo* info = owner->mpInfo;
    if (info == NULL) return;
    out->x = out->x - (f32)info->mOffsetX;
    out->y = out->y - (f32)info->mOffsetY;
}

/* 02638C10 */
void HdMapIcon::getPrimaryRelativePoint(HdMapPoint* out) { relativePoint(this, out, false); }
/* 026392A4 */
void HdMapIcon::getSecondaryRelativePoint(HdMapPoint* out) { relativePoint(this, out, true); }

static void showPane(HdMapIconPane* pane) {
    pane->mFlags = (pane->mFlags & ~HdMapPane_Visible) + HdMapPane_Visible;
}
static void hidePane(HdMapIconPane* pane) { pane->mFlags = pane->mFlags & ~HdMapPane_Visible; }

static void updateInside(HdMapIcon* self, bool secondary) {
    // HD: retain native stack point offsets across nested coordinate callbacks.
    gabi::NativeFrame<0x20> frame;
    HdMapIconPane* pane = secondary ? self->mpSecondaryPane.get() : self->mpPrimaryPane.get();
    if (pane == NULL) return;
    u32 matches = secondary ? self->matchesSecondaryCell() : self->matchesPrimaryCell();
    if (!matches) { hidePane(pane); return; }
    showPane(pane);
    HdMapPoint* mapped = gabi::at<HdMapPoint>(frame + 0x10);
    mapped->x = 0.0f;
    mapped->y = 0.0f;
    if (secondary) self->getSecondaryMapPoint(mapped);
    else self->getPrimaryMapPoint(mapped);
    if (!self->insideBounds(mapped)) { hidePane(pane); return; }
    showPane(pane);
    HdMapPoint* local = gabi::at<HdMapPoint>(frame + 8);
    local->x = 0.0f;
    local->y = 0.0f;
    if (secondary) self->getSecondaryRelativePoint(local);
    else self->getPrimaryRelativePoint(local);
    f32 x = local->x * self->mPointScale.x;
    f32 y = -(local->y * self->mPointScale.y);
    f32 dy = -(self->mPosition.y * self->mTranslateScale.y);
    f32 dx = self->mPosition.x * self->mTranslateScale.x;
    local->x = x;
    local->y = y;
    f32 scaleX = 1.0f;
    f32 scaleY = 1.0f;
    HdMapIconPane* parent = self->mpParentPane;
    if (parent != NULL) { scaleY = parent->mScaleY; scaleX = parent->mScaleX; }
    x = gabi::fmadds(x, scaleX, dx);
    y = gabi::fmadds(y, scaleY, dy);
    local->x = x;
    local->y = y;
    pane->mTranslateZ = 0.0f;
    pane->mTranslateX = x;
    pane->mTranslateY = y;
    pane->mFlags = pane->mFlags | HdMapPane_TranslateChanged;
}

/* 02638CA8 */
void HdMapIcon::updatePrimaryInside() { updateInside(this, false); }
/* 0263933C */
void HdMapIcon::updateSecondaryInside() { updateInside(this, true); }

static void updateOutside(HdMapIcon* self, bool secondary) {
    gabi::NativeFrame<0x18> frame;
    HdMapIconPane* pane = secondary ? self->mpSecondaryPane.get() : self->mpPrimaryPane.get();
    if (pane == NULL) return;
    u32 matches = secondary ? self->matchesSecondaryCell() : self->matchesPrimaryCell();
    if (!matches) { hidePane(pane); return; }
    showPane(pane);
    HdMapPoint* local = gabi::at<HdMapPoint>(frame + 8);
    local->x = 0.0f;
    local->y = 0.0f;
    if (secondary) self->getSecondaryRelativePoint(local);
    else self->getPrimaryRelativePoint(local);
    if (!hdMapInsideOutsideBounds(self, local)) { hidePane(pane); return; }
    showPane(pane);
    f32 x = local->x * self->mPointScale.x;
    f32 y = -(local->y * self->mPointScale.y);
    local->x = x;
    local->y = y;
    f32 scaleX = 1.0f;
    f32 scaleY = 1.0f;
    HdMapIconPane* parent = self->mpParentPane;
    if (parent != NULL) { scaleY = parent->mScaleY; scaleX = parent->mScaleX; }
    x = x * scaleX;
    y = y * scaleY;
    local->x = x;
    local->y = y;
    pane->mTranslateZ = 0.0f;
    pane->mTranslateX = x;
    pane->mTranslateY = y;
    pane->mFlags = pane->mFlags | HdMapPane_TranslateChanged;
}

/* 02638E98 */
void HdMapIcon::updatePrimaryOutside() { updateOutside(this, false); }
/* 0263947C */
void HdMapIcon::updateSecondaryOutside() { updateOutside(this, true); }

/* WWHD glue (tools/verify/autoglue.py): native HD map context and bounds.
data l_mapManager 101F8344
data l_bounds 104901E8
ext 028F4384 f32 hdMapSqrt(f32)
ext 02638DE8 u32 hdMapInsideOutsideBounds(HdMapIcon*, HdMapPoint*)
*/
