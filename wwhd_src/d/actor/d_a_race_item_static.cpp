/* WWHD barrel-item static methods. */
#include "d/actor/d_a_race_item.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_item.h"
static const gc_tu_data l_gc_tu = {.headerStatics = {0x1046D650, 0x101CF34C, 0x1046D65C}};
static RaceItemSounds l_itemSounds;

/* 02459F6C */
void daRaceItem_c::raceItemForceGet() {
    u32 type = field_0x644;
    field_0x640 = RaceItem_GetStart;
    if (type == RaceItem_Race)
        raceItemGet();
    else if (type == RaceItem_Normal)
        normalItemGet();
}

/* 02459C08 */
void daRaceItem_c::raceItemGet() {
    u32 item = m_itemNo, amount = 0, sound = JA_SE_LUPY_GET;
    switch (item) {
    case dItemNo_GREEN_RUPEE_e:
        amount = 1;
        break;
    case dItemNo_BLUE_RUPEE_e:
        amount = 5;
        sound = JA_SE_BLUE_LUPY_GET;
        break;
    case dItemNo_YELLOW_RUPEE_e:
        amount = 10;
        break;
    case dItemNo_RED_RUPEE_e:
        amount = 20;
        sound = JA_SE_RED_LUPY_GET;
        break;
    case dItemNo_PURPLE_RUPEE_e:
        amount = 50;
        break;
    case dItemNo_ORANGE_RUPEE_e:
        amount = 100;
        break;
    case dItemNo_SILVER_RUPEE_e:
        amount = 200;
        break;
    default:
        return;
    }
    mDoAud_seStart(sound);
    auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    s32 rupees = game->play.mMiniGameRupee + amount;
    game->play.mMiniGameRupee = rupees > 0 ? rupees : 0;
}

/* 02459EA0 */
void daRaceItem_c::normalItemGet() {
    u32 item = m_itemNo;
    field_0x640 = RaceItem_GetStart;
    execItemGet(item);
    s32 bit = mItemBitNo;
    if (bit != 127) {
        s32 room = home.roomNo;
        RaceItem_onItem(dComIfGs_info(), bit, room);
    }
    item = m_itemNo;
    if (item >= 1 && item <= 6)
        mDoAud_seStart(l_itemSounds.mSounds[item]);
    else if (item >= 9 && item <= 18)
        mDoAud_seStart(l_itemSounds.mSounds[item - 2]);
    else if (item == dItemNo_TRIPLE_HEART_e)
        mDoAud_seStart(JA_SE_HEART_PIECE);
}

/* 02459F90 */
BOOL daRaceItem_c::startOffsetPos() {
    field_0x640 = RaceItem_Wait;
    return true;
}

/* 02459FA0 */
BOOL daRaceItem_c::endOffsetPos(f32 grav, cXyz* scl, f32 vy, f32 forward, csXyz* angle) {
    if (angle) {
        current.angle.x = angle->x;
        current.angle.y = angle->y;
        current.angle.z = angle->z;
    }
    if (scl) {
        scale.x = scl->x;
        scale.y = scl->y;
        scale.z = scl->z;
    }
    speedF = forward;
    gravity = grav;
    field_0x640 = RaceItem_DropStart;
    speed.y = vy;
    return true;
}

/* 0245A000 */
BOOL daRaceItem_c::checkOffsetPos() {
    return !(field_0x645 & RaceItem_BoomerangCarry) && !(actor_status & fopAcStts_HOOK_CARRY_e);
}

/* 0245A028 */
void daRaceItem_c::set_mtx_pos(cXyz* pos) {
    f32 sx = scale.x, sy = scale.y, sz = scale.z;
    J3DModel* model = mpModel;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    f32 x = pos->x, y = pos->y, z = pos->z;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* ---------------- HD: compiler-generated and template code of this translation unit ---------------- */

/* 0245A100 */
static void __sinit_d_a_race_item_static_cpp() {
    sinit_header_statics_z(l_gc_tu.headerStatics[0], l_gc_tu.headerStatics[1], l_gc_tu.headerStatics[2]);
}

/* WWHD glue
data l_itemSounds 10038E42
ext 025E1988 void mDoAud_seStart(u32)
ext 025BA384 void RaceItem_onItem(dSv_info_c*,s32,s32)
*/
