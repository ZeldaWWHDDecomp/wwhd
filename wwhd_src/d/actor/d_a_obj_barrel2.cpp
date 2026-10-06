// Barrel and sea mine actor, ported from GameCube and WWHD disassembly.
// Binary-derived;
#include "d/actor/d_a_obj_barrel2.h"
using daObjBarrel2::Act_c;
using daObjBarrel2::Attr_c;
static gptr<fopAc_ac_c> &tmp_item() {
  return *gabi::at<gptr<fopAc_ac_c>>(0x101C7F30);
}
static Attr_c *attr(Act_c *a, u32 file, u32 expr) {
  if ((u32)a->m410 >= 4)
    gabi::call(0x0273AA24, STR(file), 0x1EA, STR(expr));
  return gabi::at<Attr_c>(0x10025E30 + (u32)a->m410 * 0x74);
}

static bool demo_mode_chk(Act_c *a) {
  WWHD_FUNC(0x0231FD50, bool, a);
  u32 mode = a->m40C;
  return !(mode < 1 || (mode > 3 && (mode < 5 || mode > 8)));
}
VERIFY(0x0231FD50, demo_mode_chk);

static void mode_afl_init(Act_c *a) {
  WWHD_FUNC(0x0231F344, void, a);
  a->m40C = 0;
}
VERIFY(0x0231F344, mode_afl_init);

static void mode_exit_v_init(Act_c *a) {
  WWHD_FUNC(0x02320918, void, a);
  a->m40C = 1;
  a->m464 = 4;
  a->m454 = gabi::load<f32>(0x10025AE8);
}
VERIFY(0x02320918, mode_exit_v_init);

static void mode_exit_h_init(Act_c *a) {
  WWHD_FUNC(0x02320938, void, a);
  a->m40C = 2;
  a->m454 = gabi::load<f32>(0x10025AE8);
}
VERIFY(0x02320938, mode_exit_h_init);

static void mode_exit_mine_init(Act_c *a) {
  WWHD_FUNC(0x02320950, void, a);
  a->m40C = 3;
  a->m454 = gabi::load<f32>(0x10025AEC);
}
VERIFY(0x02320950, mode_exit_mine_init);

static void mode_demo_break1_init(Act_c *a) {
  WWHD_FUNC(0x02321F38, void, a);
  a->m40C = 6;
}
VERIFY(0x02321F38, mode_demo_break1_init);

static void mode_demo_explode1_init(Act_c *a) {
  WWHD_FUNC(0x02322084, void, a);
  a->m40C = 8;
}
VERIFY(0x02322084, mode_demo_explode1_init);

static void mode_demo_explode0_init(Act_c *a) {
  WWHD_FUNC(0x0232061C, void, a);
  u32 co = gabi::load<u32>(gabi::ea(a) + 0x424);
  u32 tg = gabi::load<u32>(gabi::ea(a) + 0x410);
  a->m40C = 7;
  gabi::store<u32>(gabi::ea(a) + 0x424, co & ~1u);
  a->m464 = 2;
  gabi::store<u32>(gabi::ea(a) + 0x410, tg & ~1u);
}
VERIFY(0x0232061C, mode_demo_explode0_init);

static void mode_demo_break0_init(Act_c *a) {
  WWHD_FUNC(0x02320890, void, a);
  u32 tg = gabi::load<u32>(gabi::ea(a) + 0x410);
  u32 co = gabi::load<u32>(gabi::ea(a) + 0x424);
  a->m40C = 5;
  gabi::store<u32>(gabi::ea(a) + 0x410, tg & ~1u);
  a->m464 = 1;
  gabi::store<u32>(gabi::ea(a) + 0x424, co & ~1u);
  gabi::call(0x02320648, a);
}
VERIFY(0x02320890, mode_demo_break0_init);

static void mode_explode_init(Act_c *a) {
  WWHD_FUNC(0x023203B0, void, a);
  a->m464 = 30;
  gabi::call(0x0231FE60, a);
  gabi::call(0x023201B0, a, 300.f);
  gabi::call(0x02320294, a, 130.f);
  u32 co = gabi::load<u32>(gabi::ea(a) + 0x424);
  a->m40C = 4;
  gabi::store<u32>(gabi::ea(a) + 0x424, co & ~1u);
}
VERIFY(0x023203B0, mode_explode_init);

static bool actor_delete(Act_c *a) {
  WWHD_FUNC(0x0231FB78, bool, a);
  dComIfG_resDelete(&a->mPhase, STR(0x10025DE0));
  return true;
}
VERIFY(0x0231FB78, actor_delete);

static void tg_hitCB(Act_c *a, dCcD_GObjInf *obj, fopAc_ac_c *other,
                     dCcD_GObjInf *otherobj) {
  WWHD_FUNC(0x0231E800, void, a, obj, other, otherobj);
  void *hit = gabi::call<void *>(0x02516300, obj);
  if (hit && (gabi::load<u32>(gabi::ea(hit) + 0x10) & 0x20))
    a->m470 = 3;
}
VERIFY(0x0231E800, tg_hitCB);

static void item_give(Act_c *a) {
  WWHD_FUNC(0x023208BC, void, a);
  fopAc_ac_c *item = tmp_item();
  if (item) {
    gabi::call(0x02459F6C, item);
    a->mItemId = 0xFFFFFFFF;
    a->m476 = 1;
    tmp_item() = nullptr;
  }
}
VERIFY(0x023208BC, item_give);

// Returns 0 without an item (the loaded pointer, beqlr) or fopAcM_delete's result (tail branch 02320AD0).
static u32 item_delete(Act_c *a) {
  WWHD_FUNC(0x02320AC0, u32, a);
  fopAc_ac_c *item = tmp_item();
  if (!item)
    return 0;
  return gabi::call<u32>(0x025D57E0, item);
}
VERIFY(0x02320AC0, item_delete);

static void item_drop(Act_c *a) {
  WWHD_FUNC(0x02321CEC, void, a);
  fopAc_ac_c *item = tmp_item();
  if (item) {
    u32 play = dComIfGp_ea();
    fopAc_ac_c *player = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + 0x5B2C));
    a->m45C = gabi::call<f32>(0x025D6924, item, player);
  } else
    a->m45C = gabi::load<f32>(0x10025928);
}
VERIFY(0x02321CEC, item_drop);

static void item_connect_check(Act_c *a) {
  WWHD_FUNC(0x0231FCAC, void, a);
  tmp_item() = nullptr;
  if (a->mItemId != 0xFFFFFFFF) {
    gabi::Local<gptr<fopAc_ac_c>> item;
    if (gabi::call<BOOL>(0x025D54C4, (u32)a->mItemId, item.get())) {
      fopAc_ac_c *found = *item;
      if (found) {
        if (gabi::call<BOOL>(0x0245A000, found))
          tmp_item() = found;
        else
          a->mItemId = 0xFFFFFFFF;
      }
    } else
      a->mItemId = 0xFFFFFFFF;
  }
}
VERIFY(0x0231FCAC, item_connect_check);

static bool solidHeapCB(Act_c *a) {
  WWHD_FUNC(0x0231E7FC, bool, a);
  return gabi::call<bool>(0x0231E670, a);
}
VERIFY(0x0231E7FC, solidHeapCB);

static void init_mtx(Act_c *a) {
  WWHD_FUNC(0x0231F340, void, a);
  gabi::call<void>(0x0231EEF4, a);
}
VERIFY(0x0231F340, init_mtx);

static s32 MethodCreate(Act_c *a) {
  WWHD_FUNC(0x023221CC, s32, a);
  return gabi::call<s32>(0x0231F350, a);
}
VERIFY(0x023221CC, MethodCreate);

static bool MethodDelete(Act_c *a) {
  WWHD_FUNC(0x023221D0, bool, a);
  return gabi::call<bool>(0x0231FB78, a);
}
VERIFY(0x023221D0, MethodDelete);

static bool MethodExecute(Act_c *a) {
  WWHD_FUNC(0x023221D4, bool, a);
  return gabi::call<bool>(0x0232101C, a);
}
VERIFY(0x023221D4, MethodExecute);

static bool MethodDraw(Act_c *a) {
  WWHD_FUNC(0x023221D8, bool, a);
  return gabi::call<bool>(0x023210E0, a);
}
VERIFY(0x023221D8, MethodDraw);

static void empty_virtual(void *a) { WWHD_FUNC(0x023222FC, void, a); }
VERIFY(0x023222FC, empty_virtual);

static BOOL MethodIsDelete(void *a) {
  WWHD_FUNC(0x0232236C, BOOL, a);
  return TRUE;
}
VERIFY(0x0232236C, MethodIsDelete);

static u32 PrmAbstract(fopAc_ac_c *a, u32 width, u32 shift) {
  WWHD_FUNC(0x02322374, u32, a, width, shift);
  u32 p = fopAcM_GetParam(a);
  u32 mask = (width & 32) ? 0 : (1u << (width & 31));
  u32 val = (shift & 32) ? 0 : (p >> (shift & 31));
  return val & (mask - 1);
}
VERIFY(0x02322374, PrmAbstract);

static BOOL mode_proc_call(Act_c *a) {
  WWHD_FUNC(0x02320968, BOOL, a);
  gabi::call(0x0231EA58, a);
  u32 entry = 0x10025AF0 + (u32)a->m40C * 8;
  s16 delta = gabi::load<s16>(entry), idx = gabi::load<s16>(entry + 2);
  void *p = gabi::at<void>(gabi::ea(a) + delta);
  u32 target;
  if (idx < 0)
    target = gabi::load<u32>(entry + 4);
  else {
    u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(entry + 6));
    target = gabi::load<u32>(vt + (u32)idx * 8 + 4);
  }
  return gabi::call_ptr<BOOL>(target, p);
}
VERIFY(0x02320968, mode_proc_call);

static bool actor_draw(Act_c *a) {
  WWHD_FUNC(0x023210E0, bool, a);
  if ((u32)a->m40C <= 3) {
    settingTevStruct(dKy_getEnvlight(), 0, &a->current.pos, &a->tevStr);
    setLightTevColorType(dKy_getEnvlight(), a->m298, &a->tevStr);
    J3DModel *mdl = a->m298;
    mDoExt_brkAnm *anm = a->m29C;
    J3DModelData *data =
        gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(mdl) + 0xAC));
    f32 frame = gabi::load<f32>(gabi::ea(anm) + 4);
    mDoExt_brkAnm_entry(anm, data, frame);
    mDoExt_modelUpdateDL(a->m298);
  }
  return true;
}
VERIFY(0x023210E0, actor_draw);

static void watercheck_dt(void *obj, s32 flags) {
  WWHD_FUNC(0x02322270, void, obj, flags);
  if (obj) {
    u32 b = gabi::ea(obj);
    gabi::store<u32>(b + 0x20, 0x10025778);
    gabi::store<u32>(b + 0x24, 0x10025798);
    gabi::store<u32>(b + 0x30, 0x10025768);
    gabi::call(0x02008B4C, gabi::at<void>(b + 0x10), 0);
    if (flags & 1)
      operator_delete(obj);
  }
}
VERIFY(0x02322270, watercheck_dt);

static void actor_dt(Act_c *a, s32 flags) {
  WWHD_FUNC(0x02322300, void, a, flags);
  if (a) {
    gabi::call(0x02515A70, &a->mCyl, 2);
    gabi::call(0x02515860, &a->mStts, 2);
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1)
      operator_delete(a);
  }
}
VERIFY(0x02322300, actor_dt);

static void sinit() {
  WWHD_FUNC(0x023221DC, void, (u32)0);
  gabi::store<u32>(0x10469220, 0);
  gabi::store<u32>(0x10469218, 0);
  gabi::store<u32>(0x10469224, 0);
  gabi::store<u32>(0x1046921C, 0);
  gabi::call(0x028F026C, gabi::at<void>(0x101C7F0C));
  gabi::store<f32>(0x1046920C, -3.1415927410125732f);
  gabi::store<f32>(0x10469210, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<void>(0x10469214));
  gabi::call(0x028F026C, gabi::at<void>(0x101C7F18));
  gabi::call(0x028EAB2C, gabi::at<void>(0x10469215));
  gabi::call(0x028F026C, gabi::at<void>(0x101C7F24));
}
VERIFY(0x023221DC, sinit);

static void cull_set_move(Act_c *a) {
  WWHD_FUNC(0x0231FBA8, void, a);
  Attr_c *t[4];
  for (int i = 0; i < 4; i++)
    t[i] = attr(a, 0x10025968, 0x1002597C);
  f32 y = ((f32)t[0]->m18 * (f32)t[1]->m24) * (f32)t[2]->m20;
  gabi::call(0x025D6768, a, 0.f, y, 0.f, (f32)t[3]->m10);
}
VERIFY(0x0231FBA8, cull_set_move);

static void cull_set_draw(Act_c *a) {
  WWHD_FUNC(0x02320EDC, void, a);
  Attr_c *t[5];
  for (int i = 0; i < 5; i++)
    t[i] = attr(a, 0x10025BBC, 0x10025BD0);
  f32 y = ((f32)t[0]->m18 * (f32)t[1]->m24) * (f32)t[2]->m20;
  gabi::call(0x025D6768, a, 0.f, y, 0.f,
             (75.f * (f32)t[3]->m24) * (f32)t[4]->m20);
}
VERIFY(0x02320EDC, cull_set_draw);

static void set_pos_y(Act_c *a) {
  WWHD_FUNC(0x0231ED94, void, a);
  Attr_c *t0 = attr(a, 0x100258A0, 0x100258B4);
  Attr_c *t1 = attr(a, 0x100258A0, 0x100258B4);
  Attr_c *t2 = attr(a, 0x100258A0, 0x100258B4);
  f32 sin = gabi::load<f32>(0x104A44F8 + ((u16)a->m43C >> 3) * 8);
  Attr_c *t3 = attr(a, 0x100258A0, 0x100258B4);
  Attr_c *t4 = attr(a, 0x100258A0, 0x100258B4);
  f32 x = (f32)t0->m50 * (f32)t1->m14;
  f32 sway = gabi::fmsubs(t2->m54, sin, x);
  f32 y = (f32)a->m41C + (f32)a->m450;
  a->current.pos.y = gabi::fmadds(sway * (f32)t3->m20, t4->m24, y);
}
VERIFY(0x0231ED94, set_pos_y);

static bool mine_chk_range_explode(Act_c *a) {
  WWHD_FUNC(0x0231FD84, bool, a);
  u32 play = dComIfGp_ea();
  void *ship = gabi::at<void>(gabi::load<u32>(play + 0x5B3C));
  Attr_c *t0 = attr(a, 0x100259A4, 0x100259B8);
  Attr_c *t1 = attr(a, 0x100259A4, 0x100259B8);
  f32 sq = (f32)t0->m44 * (f32)t1->m44;
  return ship && (gabi::call<f32>(0x025D69AC, a, ship) < sq);
}
VERIFY(0x0231FD84, mine_chk_range_explode);

static bool mine_chk_range_flash(Act_c *a) {
  WWHD_FUNC(0x023209E4, bool, a);
  u32 play = dComIfGp_ea();
  void *ship = gabi::at<void>(gabi::load<u32>(play + 0x5B3C));
  Attr_c *t0 = attr(a, 0x10025B38, 0x10025B4C);
  Attr_c *t1 = attr(a, 0x10025B38, 0x10025B4C);
  f32 sq = (f32)t0->m40 * (f32)t1->m40;
  return ship && (gabi::call<f32>(0x025D69AC, a, ship) < sq);
}
VERIFY(0x023209E4, mine_chk_range_flash);

static bool mode_exit_v(Act_c *a) {
  WWHD_FUNC(0x0232183C, bool, a);
  f32 velocity = ((f32)a->m454 - 5.f) * 0.96f;
  f32 pos = (f32)a->m450 + velocity;
  a->m454 = velocity;
  a->m450 = pos;
  gabi::call(0x0231ED94, a);
  gabi::call(0x023213F8, a);
  Attr_c *t[4];
  for (int i = 0; i < 4; i++)
    t[i] = attr(a, 0x10025CD8, 0x10025CEC);
  f32 lower =
      -(((f32)t[0]->m18 * (f32)t[1]->m20) * (f32)t[2]->m24) - (f32)t[3]->m30;
  if ((f32)a->m450 > lower)
    return true;
  gabi::call(0x02320294, a, 0.f);
  gabi::call(0x023201B0, a, 0.f);
  return false;
}
VERIFY(0x0232183C, mode_exit_v);

static bool mode_exit_h(Act_c *a) {
  WWHD_FUNC(0x023219D8, bool, a);
  f32 velocity = ((f32)a->m454 - 5.f) * 0.94f;
  f32 pos = (f32)a->m450 + velocity;
  a->m454 = velocity;
  a->m450 = pos;
  gabi::call(0x0231ED94, a);
  gabi::call(0x023213F8, a);
  Attr_c *t[4];
  for (int i = 0; i < 4; i++)
    t[i] = attr(a, 0x10025D14, 0x10025D28);
  f32 lower =
      -(((f32)t[0]->m18 * (f32)t[1]->m20) * (f32)t[2]->m24) - (f32)t[3]->m30;
  if ((f32)a->m450 > lower)
    return true;
  gabi::call(0x02320294, a, 0.f);
  gabi::call(0x023201B0, a, 0.f);
  return false;
}
VERIFY(0x023219D8, mode_exit_h);

static bool mode_exit_mine(Act_c *a) {
  WWHD_FUNC(0x02321B74, bool, a);
  f32 velocity = ((f32)a->m454 - 5.f) * 0.94f;
  f32 pos = (f32)a->m450 + velocity;
  a->m454 = velocity;
  a->m450 = pos;
  gabi::call(0x0231ED94, a);
  gabi::call(0x023213F8, a);
  Attr_c *t[3];
  for (int i = 0; i < 3; i++)
    t[i] = attr(a, 0x10025D50, 0x10025D64);
  f32 lower = -(((f32)t[0]->m18 * (f32)t[1]->m20) * (f32)t[2]->m24) - 50.f;
  if ((f32)a->m450 > lower)
    return true;
  gabi::call(0x02320294, a, 0.f);
  gabi::call(0x023201B0, a, 0.f);
  return false;
}
VERIFY(0x02321B74, mode_exit_mine);

static bool actor_execute(Act_c *a) {
  WWHD_FUNC(0x0232101C, bool, a);
  gabi::call(0x0231FBA8, a);
  gabi::call(0x0231FCAC, a);
  u8 event = 0;
  if (a->m46A != 1) {
    u32 play = dComIfGp_ea();
    if (gabi::load<u8>(play + 0x5292))
      event = 1;
  }
  a->m46C = event;
  if (a->m40C != 0 || !(gabi::load<u32>(gabi::ea(a) + 0x2E4) & 4) ||
      !gabi::call<BOOL>(0x025D6CE8, a) || a->m46C != 0 || a->m474 != 0 ||
      a->m475 != 0)
    gabi::call(0x02320AD4, a);
  gabi::call(0x02320EDC, a);
  return true;
}
VERIFY(0x0232101C, actor_execute);

static bool end_demo() {
  u32 play = dComIfGp_ea();
  if (gabi::call<BOOL>(0x0254457C, gabi::at<void>(play + 0x52C4),
                       STR(0x10025DD4))) {
    dComIfGp_event_reset();
    return false;
  }
  return true;
}
static bool exists_demo() {
  u32 play = dComIfGp_ea();
  void *mgr = gabi::at<void>(play + 0x52C4);
  u32 second = dComIfGp_ea();
  s32 idx = gabi::call<s32>(0x02543F10, gabi::at<void>(second + 0x52C4),
                            STR(0x10025DD4), (u8)0xFF);
  return gabi::call<void *>(0x02544044, mgr, idx) != nullptr;
}

static bool mode_demo_break1(Act_c *a) {
  WWHD_FUNC(0x02322024, bool, a);
  return end_demo();
}
VERIFY(0x02322024, mode_demo_break1);

static bool mode_demo_explode1(Act_c *a) {
  WWHD_FUNC(0x02322168, bool, a);
  gabi::call(0x02321CEC, a);
  return end_demo();
}
VERIFY(0x02322168, mode_demo_explode1);

static bool mode_demo_break0(Act_c *a) {
  WWHD_FUNC(0x02321F44, bool, a);
  if (exists_demo()) {
    if (eventInfo_checkCommandDemoAccrpt(a)) {
      gabi::call(0x023208BC, a);
      gabi::call(0x02321F38, a);
      return true;
    }
    a->m464 = (u32)a->m464 - 1;
    if (a->m464 >= 0) {
      gabi::call(0x025D77DC, a, STR(0x10025DD4), (u16)1, (u16)0xFFFF);
      eventInfo_onCondition(a, 2);
      return true;
    }
  }
  gabi::call(0x023208BC, a);
  return false;
}
VERIFY(0x02321F44, mode_demo_break0);

static bool mode_demo_explode0(Act_c *a) {
  WWHD_FUNC(0x02322090, bool, a);
  gabi::call(0x02321CEC, a);
  if (!exists_demo())
    return false;
  if (eventInfo_checkCommandDemoAccrpt(a)) {
    gabi::call(0x02322084, a);
    return true;
  }
  a->m464 = (u32)a->m464 - 1;
  if (a->m464 < 0)
    return false;
  if (a->m464 == 0) {
    gabi::call(0x025D77DC, a, STR(0x10025DD4), (u16)1, (u16)0xFFFF);
    eventInfo_onCondition(a, 2);
  }
  return true;
}
VERIFY(0x02322090, mode_demo_explode0);

static void trivial_dt(void *a, s32 flags) {
  WWHD_FUNC(0x023222E8, void, a, flags);
  if (a && (flags & 1))
    operator_delete(a);
}
VERIFY(0x023222E8, trivial_dt);

static void item_drop_init(Act_c *a, f32 speed) {
  WWHD_FUNC(0x02320294, void, a, speed);
  fopAc_ac_c *item = tmp_item();
  if (!item) {
    a->m45C = gabi::load<f32>(0x10025928);
    return;
  }
  Attr_c *t0 = attr(a, 0x10025A30, 0x10025A44);
  Attr_c *t1 = attr(a, 0x10025A30, 0x10025A44);
  gabi::Local<cXyz> scale;
  f32 v = (f32)t0->m28 * (f32)t1->m2C;
  scale->set(v, v, v);
  gabi::call(0x02459FA0, item, -7.f, scale.get(), speed, 0.f,
             gabi::at<csXyz>(0x101FFB14));
  u32 play = dComIfGp_ea();
  void *player = gabi::at<void>(gabi::load<u32>(play + 0x5B2C));
  a->m45C = gabi::call<f32>(0x025D6924, item, player);
}
VERIFY(0x02320294, item_drop_init);

static void set_item_position(Act_c *a) {
  WWHD_FUNC(0x023213F8, void, a);
  fopAc_ac_c *item = tmp_item();
  if (!item) {
    a->m45C = gabi::load<f32>(0x10025928);
    return;
  }
  u8 event = a->m46C;
  f32 x = a->current.pos.x;
  if (event) {
    Attr_c *t = attr(a, 0x10025C34, 0x10025C48);
    gabi::Local<cXyz> pos;
    pos->x = x;
    pos->y = (f32)a->current.pos.y + (f32)t->m30;
    pos->z = a->current.pos.z;
    gabi::call(0x0245A028, item, pos.get());
  } else {
    item->current.pos.x = x;
    Attr_c *t = attr(a, 0x10025C34, 0x10025C48);
    item->current.pos.y = (f32)a->current.pos.y + (f32)t->m30;
    f32 z = a->current.pos.z;
    item->speed.y = 0.f;
    item->current.pos.z = z;
  }
  gabi::call(0x02459F90, item);
  u32 play = dComIfGp_ea();
  void *player = gabi::at<void>(gabi::load<u32>(play + 0x5B2C));
  a->m45C = gabi::call<f32>(0x025D6924, item, player);
}
VERIFY(0x023213F8, set_item_position);

static void buoy_jump(Act_c *a, f32 speed) {
  WWHD_FUNC(0x023201B0, void, a, speed);
  if (a->m460 == 0xFFFFFFFF)
    return;
  gabi::Local<be<u32>> id;
  *id = (u32)a->m460;
  void *buoy =
      gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E1234), id.get());
  if (buoy) {
    s16 angle = (s16)gabi::ftoi(cM_rndFX(32768.f));
    u32 b = gabi::ea(buoy);
    gabi::store<f32>(b + 0x340, speed);
    gabi::store<f32>(b + 0x370, 50.f);
    gabi::store<s16>(b + 0x322, angle);
    gabi::call(0x028E90D4, gabi::at<void>(b + 0x3184),
               gabi::at<void>(b + 0x31E4));
    u32 status = gabi::load<u32>(b + 0x2E0);
    gabi::store<u8>(b + 0x321C, 1);
    gabi::store<f32>(b + 0x31F0, 0.f);
    gabi::store<u32>(b + 0x3218, 1);
    gabi::store<f32>(b + 0x3200, 0.f);
    gabi::store<f32>(b + 0x3210, 0.f);
    gabi::store<u32>(b + 0x2E0, status & ~0x80u);
  }
  a->m460 = 0xFFFFFFFF;
}
VERIFY(0x023201B0, buoy_jump);

static void co_hitCB(Act_c *a, dCcD_GObjInf *obj, fopAc_ac_c *other,
                     dCcD_GObjInf *otherobj) {
  WWHD_FUNC(0x0231E84C, void, a, obj, other, otherobj);
  if (!other || gabi::load<s16>(gabi::ea(other) + 0xE) != 0xA5)
    return;
  if (a->m410 != 0) {
    a->m470 = 2;
    return;
  }
  Attr_c *t = attr(a, 0x1002584C, 0x10025860);
  if (!((f32)other->speedF > (f32)t->m38))
    return;
  gabi::Local<cXyz> delta, velocity;
  delta->x = (f32)other->current.pos.x - (f32)a->current.pos.x;
  delta->y = 0;
  delta->z = (f32)other->current.pos.z - (f32)a->current.pos.z;
  velocity->set(other->speed.x, 0, other->speed.z);
  Attr_c *t0 = attr(a, 0x1002584C, 0x10025860);
  Attr_c *t1 = attr(a, 0x1002584C, 0x10025860);
  f32 sq = (f32)t0->m3C * (f32)t1->m3C;
  if (gabi::call<f32>(0x028E8F44, delta.get(), velocity.get()) < sq) {
    a->m470 = 1;
    u32 b = gabi::ea(other) + 0x644;
    gabi::store<u32>(b, gabi::load<u32>(b) | 4);
    return;
  }
  if (a->m46D != 0)
    return;
  u32 b = gabi::ea(other) + 0x644;
  gabi::store<u32>(b, gabi::load<u32>(b) | 0x20);
  a->m454 = -60.f;
  a->m46D = 20;
  if (a && gabi::ea(a) + 0x37C) {
    s32 reverb = gabi::call<s32>(0x02520540, (s8)a->current.roomNo);
    gabi::call(0x025E1A40, 0x6944, gabi::at<void>(gabi::ea(a) + 0x37C), 0,
               reverb);
  }
}
VERIFY(0x0231E84C, co_hitCB);

static bool mine_chk_range_damage(Act_c *a) {
  WWHD_FUNC(0x02320418, bool, a);
  u32 play = dComIfGp_ea();
  fopAc_ac_c *ship = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + 0x5B3C));
  Attr_c *t0 = attr(a, 0x10025A70, 0x10025A84);
  Attr_c *t1 = attr(a, 0x10025A70, 0x10025A84);
  f32 sq = (f32)t0->m48 * (f32)t1->m48;
  if (!ship)
    return false;
  if (!(gabi::call<f32>(0x025D69AC, a, ship) < sq))
    return false;
  Attr_c *t = attr(a, 0x10025A70, 0x10025A84);
  if (!((f32)ship->current.pos.y < (f32)a->current.pos.y + (f32)t->m4C))
    return false;
  play = dComIfGp_ea();
  if (!(gabi::load<u32>(play + 0x5CD8) & 0x10000))
    return false;
  gabi::Local<cXyz> delta, velocity;
  delta->set((f32)ship->current.pos.x - (f32)a->current.pos.x, 0,
             (f32)ship->current.pos.z - (f32)a->current.pos.z);
  velocity->set(ship->speed.x, 0, ship->speed.z);
  t0 = attr(a, 0x10025A70, 0x10025A84);
  t1 = attr(a, 0x10025A70, 0x10025A84);
  sq = (f32)t0->m3C * (f32)t1->m3C;
  if (gabi::call<f32>(0x028E8F44, delta.get(), velocity.get()) < sq) {
    u32 b = gabi::ea(ship) + 0x644;
    gabi::store<u32>(b, gabi::load<u32>(b) | 4);
    return true;
  }
  return false;
}
VERIFY(0x02320418, mine_chk_range_damage);

static bool mode_explode(Act_c *a) {
  WWHD_FUNC(0x02321D54, bool, a);
  a->m464 = (u32)a->m464 - 1;
  if (a->m464 < 0) {
    gabi::call(0x02321CEC, a);
    return false;
  }
  f32 pos = a->m450;
  f32 velocity = gabi::fmadds(pos, -0.01f, a->m454) * 0.94f;
  pos += velocity;
  a->m454 = velocity;
  a->m450 = pos;
  Attr_c *t = attr(a, 0x10025D8C, 0x10025DA0);
  if (a->m450 > (f32)t->m14 * 0.2f) {
    t = attr(a, 0x10025D8C, 0x10025DA0);
    a->m450 = (f32)t->m14 * 0.2f;
  }
  t = attr(a, 0x10025D8C, 0x10025DA0);
  f32 step = (f32)(s16)t->m58;
  f32 rnd = cM_rnd();
  a->m43C = (s16)((s16)a->m43C + (s16)gabi::ftoi(step * (rnd + 1.f)));
  gabi::call(0x0231ED94, a);
  gabi::call(0x02321CEC, a);
  return true;
}
VERIFY(0x02321D54, mode_explode);

static void afl_sway(Act_c *a) {
  WWHD_FUNC(0x0232115C, void, a);
  Attr_c *t0 = attr(a, 0x10025BF8, 0x10025C0C);
  Attr_c *t1 = attr(a, 0x10025BF8, 0x10025C0C);
  f32 maxsq = (f32)t0->m5C * (f32)t1->m5C;
  Attr_c *t = attr(a, 0x10025BF8, 0x10025C0C);
  f32 x = (f32)a->m420.x * (f32)t->m68;
  t = attr(a, 0x10025BF8, 0x10025C0C);
  f32 z = (f32)a->m420.z * (f32)t->m68;
  f32 sq = gabi::fmadds(x, x, z * z);
  if (sq > maxsq) {
    t = attr(a, 0x10025BF8, 0x10025C0C);
    f32 root = gabi::call<f32>(0x028F4384, sq);
    f32 ratio = (f32)t->m5C / root;
    x *= ratio;
    z *= ratio;
  }
  x = (f32)a->m440 - x;
  z = (f32)a->m444 - z;
  t = attr(a, 0x10025BF8, 0x10025C0C);
  f32 ax = -(x * (f32)t->m60);
  t = attr(a, 0x10025BF8, 0x10025C0C);
  f32 az = -(z * (f32)t->m60);
  t = attr(a, 0x10025BF8, 0x10025C0C);
  f32 dx = -((f32)a->m448 * (f32)t->m64);
  t = attr(a, 0x10025BF8, 0x10025C0C);
  f32 vx = (f32)a->m448 + (ax + dx);
  f32 vz = (f32)a->m44C + gabi::fnmsubs(a->m44C, t->m64, az);
  f32 px = (f32)a->m440 + vx;
  a->m448 = vx;
  a->m440 = px;
  f32 pz = (f32)a->m444 + vz;
  a->m44C = vz;
  a->m444 = pz;
}
VERIFY(0x0232115C, afl_sway);

static bool create_heap(Act_c *a) {
  WWHD_FUNC(0x0231E670, bool, a);
  Attr_c *t = attr(a, 0x100257E8, 0x1002581C);
  u16 index = t->m00;
  J3DModelData *mdl =
      (J3DModelData *)dComIfG_getObjectRes(STR(0x10025DE0), index, 0x10025740);
  if (!mdl)
    gabi::call(0x0273AA24, STR(0x100257E8), 0x217, STR(0x100257FC));
  a->m298 = mDoExt_J3DModel__create(mdl, 0x80000, 0x11000022);
  t = attr(a, 0x100257E8, 0x1002581C);
  index = t->m02;
  void *brk = dComIfG_getObjectRes(STR(0x10025DE0), index, 0x10025740);
  if (!brk)
    gabi::call(0x0273AA24, STR(0x100257E8), 0x221, STR(0x1002580C));
  void *anm = gabi::call<void *>(0x0273AD10, 0x78);
  if (anm)
    anm = gabi::call<void *>(0x025E80D0, anm);
  a->m29C = (mDoExt_brkAnm *)anm;
  s32 result = 0;
  if (anm)
    result = gabi::call<s32>(0x025E8154, anm, mdl, brk, 1, 0, 1.f, 0, -1, 0, 0);
  return a->m298 != nullptr && result != 0;
}
VERIFY(0x0231E670, create_heap);

static void set_water_pos(Act_c *a) {
  WWHD_FUNC(0x0231EA58, void, a);
  f32 x = a->current.pos.x, z = a->current.pos.z;
  if (gabi::call<BOOL>(0x0246B6A4, x, z)) {
    x = a->current.pos.x;
    z = a->current.pos.z;
    f32 w0 = gabi::call<f32>(0x0246BA0C, x - 32.f, z - 32.f);
    x = a->current.pos.x;
    z = a->current.pos.z;
    f32 w1 = gabi::call<f32>(0x0246BA0C, x - 32.f, z + 32.f);
    x = a->current.pos.x;
    z = a->current.pos.z;
    f32 w2 = gabi::call<f32>(0x0246BA0C, x + 32.f, z - 32.f);
    gabi::Local<cXyz> u, v, cross;
    u->set(0.f, w1 - w0, 32.f);
    v->set(32.f, w2 - w0, 0.f);
    a->m41C = ((w0 + w1) + w2) * 0.3333333432674408f;
    gabi::call(0x0201B080, u.get(), cross.get(), v.get());
    a->m420.copy(*cross);
    gabi::call(0x0201B3C0, &a->m420, cross.get());
    return;
  }
  u32 guard = 0x10469278, check = 0x10469228;
  if (gabi::load<u32>(guard) == 0) {
    gabi::store<u32>(guard, 1);
    gabi::call(0x024F22DC, gabi::at<void>(check));
    gabi::call(0x028F026C, gabi::at<void>(0x101C7F00));
  }
  x = a->current.pos.x;
  f32 y = a->current.pos.y;
  z = a->current.pos.z;
  gabi::store<f32>(check + 0x38, x);
  gabi::store<f32>(check + 0x40, z);
  gabi::store<f32>(check + 0x3C, y - 1000.f);
  gabi::store<f32>(check + 0x44, y + 1000.f);
  u32 play = dComIfGp_ea();
  BOOL found = gabi::call<BOOL>(0x024EF7C0, gabi::at<void>(play + 0x12A0),
                                gabi::at<void>(check));
  a->m41C = found ? gabi::load<f32>(check + 0x48) : 0.f;
  f32 oldx = a->m42C;
  f32 random = cM_rndFX(0.001f);
  f32 ax = gabi::fmadds(oldx, -0.2f, random);
  f32 oldz = a->m430;
  random = cM_rndFX(0.001f);
  f32 az = gabi::fmadds(oldz, -0.2f, random);
  f32 vx = ((f32)a->m434 + ax) * 0.99f;
  f32 px = (f32)a->m42C + vx;
  f32 vz = ((f32)a->m438 + az) * 0.99f;
  f32 pz = (f32)a->m430 + vz;
  a->m434 = vx;
  a->m42C = px;
  a->m420.y = 1.f;
  a->m420.x = px;
  a->m438 = vz;
  a->m430 = pz;
  a->m420.z = pz;
  gabi::Local<cXyz> out;
  gabi::call(0x0201B31C, &a->m420, out.get());
}
VERIFY(0x0231EA58, set_water_pos);

static void barrel_sound(Act_c *a, u32 id) {
  if (a && gabi::ea(a) + 0x37C) {
    s32 reverb = gabi::call<s32>(0x02520540, (s8)a->current.roomNo);
    gabi::call(0x025E1A40, id, gabi::at<void>(gabi::ea(a) + 0x37C), 0, reverb);
  }
}
static void *barrel_particle(u32 id, cXyz *pos, csXyz *angle, cXyz *scale,
                             s32 toon = 0, void *color = nullptr) {
  u32 play = dComIfGp_ea();
  void *ctrl = gabi::at<void>(gabi::load<u32>(play + 0x5AB0));
  return gabi::call<void *>(0x025A847C, ctrl, toon, id, pos, angle, scale,
                            (u8)0xFF, nullptr, -1, color, color, nullptr);
}
static void effect_position(Act_c *a, u32 file, u32 expr, cXyz *pos,
                            cXyz *scale) {
  Attr_c *t0 = attr(a, file, expr);
  Attr_c *t1 = attr(a, file, expr);
  f32 mag = (f32)t0->m24 * (f32)t1->m20;
  f32 x = a->current.pos.x;
  t0 = attr(a, file, expr);
  t1 = attr(a, file, expr);
  f32 y = gabi::fmadds((f32)t0->m14 * (f32)t1->m50, mag, a->current.pos.y);
  f32 z = a->current.pos.z;
  pos->set(x, y, z);
  scale->set(mag, mag, mag);
}

static void eff_break(Act_c *a) {
  WWHD_FUNC(0x02320648, void, a);
  barrel_sound(a, 0x691E);
  gabi::Local<cXyz> pos, scale;
  effect_position(a, 0x10025AAC, 0x10025AC0, pos.get(), scale.get());
  barrel_particle(0x460, pos.get(), nullptr, scale.get());
  barrel_particle(0x45F, pos.get(), nullptr, scale.get());
  void *emitter = barrel_particle(0x3E6, pos.get(), nullptr, scale.get(), 0,
                                  gabi::at<void>(gabi::ea(a) + 0x1A8));
  if (emitter) {
    u32 e = gabi::ea(emitter);
    gabi::store<f32>(e + 0x6C, 5.f);
    gabi::store<f32>(e + 0x70, 25.f);
    gabi::store<u16>(e + 0x60, 30);
  }
}
VERIFY(0x02320648, eff_break);

static void eff_explode(Act_c *a) {
  WWHD_FUNC(0x0231FE60, void, a);
  gabi::Local<cXyz> pos, scale;
  effect_position(a, 0x100259EC, 0x10025A00, pos.get(), scale.get());
  barrel_sound(a, 0x6938);
  u32 play = dComIfGp_ea();
  s8 id = gabi::load<s8>(play + 0x5B30);
  play = dComIfGp_ea();
  u32 cam = gabi::load<u32>(play + (s32)id * 0x34 + 0x5AF8);
  gabi::Local<csXyz> angle;
  angle->x = (s16)-gabi::load<s16>(cam + 0x234);
  angle->y = (s16)(gabi::load<s16>(cam + 0x236) + 0x8000);
  angle->z = 0;
  barrel_particle(0xB, pos.get(), angle.get(), scale.get());
  play = dComIfGp_ea();
  void *ctrl = gabi::at<void>(gabi::load<u32>(play + 0x5AB0));
  gabi::call(0x025A866C, ctrl, 0x200A, pos.get(), nullptr, scale.get(),
             (u8)0xFF);
  Attr_c *t0 = attr(a, 0x100259EC, 0x10025A00);
  Attr_c *t1 = attr(a, 0x100259EC, 0x10025A00);
  gabi::call(0x025DAE64, &a->current.pos, (f32)t0->m6C, (f32)t1->m70, 1);
  barrel_particle(0x2041, pos.get(), nullptr, scale.get(), 2);
  barrel_particle(0x3C, pos.get(), nullptr, scale.get());
  void *emitter = barrel_particle(0x3E6, pos.get(), nullptr, scale.get());
  if (emitter) {
    u32 e = gabi::ea(emitter);
    gabi::store<f32>(e + 0x58, 0.4f);
    gabi::store<f32>(e + 0x70, 25.f);
    gabi::store<f32>(e + 0x6C, 10.f);
    gabi::store<u16>(e + 0x60, 40);
  }
}
VERIFY(0x0231FE60, eff_explode);

static void set_mtx(Act_c *a) {
  WWHD_FUNC(0x0231EEF4, void, a);
  s32 type = a->m410;
  bool upright = type == 0 || type == 2 || type == 3;
  Attr_c *t0 = attr(a, 0x100258E4, 0x100258F8);
  Attr_c *t1 = attr(a, 0x100258E4, 0x100258F8);
  f32 mag = (f32)t0->m24 * (f32)t1->m20;
  gabi::Local<cXyz> scaled;
  gabi::call(0x0201AE48, &a->scale, scaled.get(), mag);
  void *buoy = nullptr;
  if (a->m460 != 0xFFFFFFFF) {
    gabi::Local<be<u32>> id;
    *id = (u32)a->m460;
    buoy = gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E1234), id.get());
  }
  J3DModel_setBaseScale(a->m298, scaled.get());
  mDoMtx_stack_c::transS(a->current.pos.x, a->current.pos.y, a->current.pos.z);
  gabi::Local<cXyz> axis;
  axis->set(a->m440, 1.f, a->m444);
  Attr_c *t = attr(a, 0x100258E4, 0x100258F8);
  mDoMtx_stack_c::transM(0.f, (f32)t->m18 * mag, 0.f);
  gabi::Local<u8[16]> quat;
  gabi::call(0x023123D8, quat.get(), axis.get());
  gabi::call(0x025F25CC, quat.get());
  s16 z = a->shape_angle.z;
  if (!upright)
    z = (s16)(z + 0x4000);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s16)a->shape_angle.x,
             (s16)a->shape_angle.y, z);
  t = attr(a, 0x100258E4, 0x100258F8);
  mDoMtx_stack_c::transM(0.f, -((f32)t->m18 * mag), 0.f);
  J3DModel_setBaseTRMtx(a->m298, mDoMtx_stack_c::get());
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &a->m478);
  if (buoy) {
    gabi::Local<cXyz> offset, out;
    if (upright) {
      t = attr(a, 0x100258E4, 0x100258F8);
      offset->set(0.f, ((f32)t->m14 - 5.f) * mag, 0.f);
    } else {
      gabi::call(0x025F1C5C, mDoMtx_stack_c::get(), (s16)-0x4000);
      t = attr(a, 0x100258E4, 0x100258F8);
      offset->set(-((f32)t->m18 * mag), 45.f * mag, 0.f);
    }
    gabi::call(0x028E9044, mDoMtx_stack_c::get(), offset.get(), out.get());
    u32 m = 0x1048D0CC;
    f32 y = gabi::load<f32>(m + 0x1C) + (f32)out->y;
    f32 x = gabi::load<f32>(m + 0xC) + (f32)out->x;
    f32 z = gabi::load<f32>(m + 0x2C) + (f32)out->z;
    gabi::store<f32>(m + 0x1C, y);
    gabi::store<f32>(m + 0xC, x);
    gabi::store<f32>(m + 0x2C, z);
    gabi::call(0x028E90D4, mDoMtx_stack_c::get(),
               gabi::at<void>(gabi::ea(buoy) + 0x3184));
  }
}
VERIFY(0x0231EEF4, set_mtx);

static bool mode_afl(Act_c *a) {
  WWHD_FUNC(0x02321564, bool, a);
  BOOL coming = gabi::call<BOOL>(0x02322374, a, 1, 28);
  f32 pos = a->m450, vel = a->m454;
  f32 limit;
  if (coming) {
    f32 force = (-pos >= 0.f) ? -0.01f : -0.02f;
    vel = gabi::fmadds(pos, force, vel) * 0.92f;
    limit = 0.6f;
  } else {
    vel = gabi::fmadds(pos, -0.01f, vel) * 0.94f;
    limit = 0.2f;
  }
  pos += vel;
  a->m454 = vel;
  a->m450 = pos;
  Attr_c *t = attr(a, 0x10025C98, 0x10025CAC);
  if (a->m450 > (f32)t->m14 * limit) {
    t = attr(a, 0x10025C98, 0x10025CAC);
    a->m450 = (f32)t->m14 * limit;
  }
  t = attr(a, 0x10025C98, 0x10025CAC);
  f32 step = (f32)(s16)t->m58;
  f32 rnd = cM_rnd();
  a->m43C = (s16)((s16)a->m43C + (s16)gabi::ftoi(step * (rnd + 1.f)));
  gabi::call(0x0231ED94, a);
  f32 x = a->current.pos.x, z = a->current.pos.z;
  f32 vx = gabi::fmadds(x - (f32)a->home.pos.x, -0.002f, a->m414) * 0.9f;
  f32 vz = gabi::fmadds(z - (f32)a->home.pos.z, -0.002f, a->m418) * 0.9f;
  f32 nx = x + ((f32)gabi::load<f32>(gabi::ea(a) + 0x3BC) + vx);
  f32 nz = z + ((f32)gabi::load<f32>(gabi::ea(a) + 0x3C4) + vz);
  a->m414 = vx;
  a->m418 = vz;
  a->current.pos.x = nx;
  a->current.pos.z = nz;
  gabi::call(0x0232115C, a);
  gabi::call(0x023213F8, a);
  return true;
}
VERIFY(0x02321564, mode_afl);

static void execute_sub(Act_c *a) {
  WWHD_FUNC(0x02320AD4, void, a);
  s32 hit = a->m470;
  a->m470 = 0;
  if (a->m410 == 2) {
    s32 mode = a->m40C;
    if (mode != 4) {
      BOOL demo = gabi::call<BOOL>(0x0231FD50, a);
      if (!demo) {
        if (gabi::call<BOOL>(0x0231FD84, a))
          gabi::call(0x023203B0, a);
        else if (a->m40C == 4 && gabi::call<BOOL>(0x02320418, a))
          gabi::call(0x0232061C, a);
      } else if (mode == 4 && gabi::call<BOOL>(0x02320418, a))
        gabi::call(0x0232061C, a);
    } else if (gabi::call<BOOL>(0x02320418, a))
      gabi::call(0x0232061C, a);
  }
  if (hit == 1 && !gabi::call<BOOL>(0x0231FD50, a))
    gabi::call(0x02320890, a);
  else if (hit == 2) {
    f32 v = a->m454;
    if (v > 0.f)
      v = 0.f;
    v -= 8.f;
    if (v < -30.f)
      v = -30.f;
    a->m454 = v;
  }
  if (a->m46D)
    a->m46D = (u8)a->m46D - 1;
  Attr_c *t0 = attr(a, 0x10025B7C, 0x10025B90);
  Attr_c *t1 = attr(a, 0x10025B7C, 0x10025B90);
  f32 sq = (f32)t0->m34 * (f32)t1->m34;
  if (a->m45C < sq)
    gabi::call(0x023208BC, a);
  if (a->m474 && a->m40C == 0) {
    if (a->m410 == 0)
      gabi::call(0x02320918, a);
    else if (a->m410 == 1)
      gabi::call(0x02320938, a);
    else if (a->m410 == 2 && a->m468 == 0)
      gabi::call(0x02320950, a);
  }
  bool remove = false;
  if (hit == 3) {
    gabi::call(0x02320648, a);
    gabi::call(0x023201B0, a, 300.f);
    gabi::call(0x02320294, a, 130.f);
    remove = true;
  } else if (gabi::call<BOOL>(0x02320968, a)) {
    gabi::call(0x0231EEF4, a);
    gabi::call(0x020182E0, gabi::at<void>(gabi::ea(a) + 0x510),
               &a->current.pos);
    u32 play = dComIfGp_ea();
    gabi::call(0x0200E240, gabi::at<void>(play + 0x26A4), &a->mCyl);
    gabi::store<f32>(gabi::ea(a) + 0x390, a->current.pos.x);
    Attr_c *t[3];
    for (int i = 0; i < 3; i++)
      t[i] = attr(a, 0x10025B7C, 0x10025B90);
    f32 z = a->current.pos.z;
    f32 y = gabi::fmadds((f32)t[0]->m18 * (f32)t[1]->m20, t[2]->m24,
                         a->current.pos.y);
    f32 x = gabi::load<f32>(gabi::ea(a) + 0x390);
    gabi::store<f32>(gabi::ea(a) + 0x398, z);
    a->eyePos.z = z;
    a->eyePos.x = x;
    gabi::store<f32>(gabi::ea(a) + 0x394, y);
    a->eyePos.y = y;
    if (a->m410 == 2 && gabi::call<BOOL>(0x023209E4, a))
      a->m468 = 1;
    if (a->m40C == 0 && a->m468 &&
        gabi::call<BOOL>(0x025E742C, (mDoExt_brkAnm *)a->m29C))
      gabi::call(0x023203B0, a);
  } else
    remove = true;
  if (a->m475) {
    gabi::call(0x02320AC0, a);
    remove = true;
  }
  if (remove)
    fopAcM_delete(a);
}
VERIFY(0x02320AD4, execute_sub);

static s32 actor_create(Act_c *a) {
  WWHD_FUNC(0x0231F350, s32, a);
  u32 b = gabi::ea(a);
  u32 condition = gabi::load<u32>(b + 0x2E4);
  if (!(condition & 8)) {
    if (a) {
      fopAc_ac_c_ct(a);
      gabi::store<u32>(b + 0xB4, 0x100257D8);
      gabi::call(0x0200BD2C, gabi::at<void>(b + 0x3BC));
      gabi::call(0x02515DA0, gabi::at<void>(b + 0x3D8));
      gabi::store<u32>(b + 0x3D4, 0x1004AE88);
      gabi::store<u32>(b + 0x3D8, 0x1004AEC0);
      gabi::call(0x02515FB8, &a->mCyl);
      gabi::store<u32>(b + 0x50C, 0x100015A8);
      gabi::store<u32>(b + 0x508, 0x10025758);
      gabi::call(0x02018590, gabi::at<void>(b + 0x510));
      condition = gabi::load<u32>(b + 0x2E4);
      gabi::store<u32>(b + 0x50C, 0x1004B160);
      gabi::store<u32>(b + 0x434, 0x1004B108);
      gabi::store<u32>(b + 0x524, 0x1004B150);
    }
    gabi::store<u32>(b + 0x2E4, condition | 8);
  }
  a->m410 = gabi::call<s32>(0x02322374, a, 2, 24);
  s32 phase = dComIfG_resLoad(&a->mPhase, STR(0x10025DE0));
  if (phase != 4)
    return phase;
  auto attribute = [&]() { return attr(a, 0x1002592C, 0x10025940); };
  Attr_c *t = attribute();
  if (!gabi::call<BOOL>(0x025D63E8, a, gabi::at<void>(0x0231E7FC), (u32)t->m04))
    return 5;
  J3DModel *mdl = a->m298;
  gabi::store<u32>(b + 0x348, mdl ? gabi::ea(mdl) + 0xC8 : 0);
  a->mStts.Init(200, 255, a);
  gabi::call(0x02516518, &a->mCyl, gabi::at<void>(0x10025DEC));
  gabi::store<u32>(b + 0x43C, b + 0x3BC);
  t = attribute();
  gabi::call(0x020184DC, gabi::at<void>(b + 0x510), (f32)t->m08);
  t = attribute();
  gabi::call(0x02018428, gabi::at<void>(b + 0x510), (f32)t->m0C);
  gabi::store<u8>(b + 0x2DE, 0x3D);
  gabi::store<u32>(b + 0x494, 0x0231E800);
  gabi::store<u32>(b + 0x4DC, 0x0231E84C);
  if (a->m410 == 3)
    gabi::store<u32>(b + 0x2E0, gabi::load<u32>(b + 0x2E0) & ~0x180u);
  BOOL coming = gabi::call<BOOL>(0x02322374, a, 1, 28);
  if (coming) {
    Attr_c *t0 = attribute();
    Attr_c *t1 = attribute();
    Attr_c *t2 = attribute();
    a->m450 = -(((f32)t0->m18 * (f32)t1->m20) * (f32)t2->m24) - 50.f;
    if (a->m410 == 2) {
      a->m440 = cM_rndFX(1.f);
      a->m444 = cM_rndFX(1.f);
    } else {
      a->m440 = 0.f;
      a->m444 = 0.f;
    }
  } else {
    a->m450 = 0.f;
    a->m440 = 0.f;
    a->m444 = 0.f;
  }
  a->m448 = 0.f;
  a->m44C = 0.f;
  a->m42C = 0.f;
  a->m430 = 0.f;
  a->m434 = 0.f;
  a->m438 = 0.f;
  gabi::call(0x0231EA58, a);
  gabi::call(0x0231ED94, a);
  gabi::store<f32>(b + 0x390, a->current.pos.x);
  Attr_c *t0 = attribute();
  Attr_c *t1 = attribute();
  Attr_c *t2 = attribute();
  f32 z = a->current.pos.z;
  f32 y = gabi::fmadds((f32)t0->m18 * (f32)t1->m20, t2->m24, a->current.pos.y);
  f32 x = gabi::load<f32>(b + 0x390);
  a->eyePos.z = z;
  a->eyePos.x = x;
  gabi::store<f32>(b + 0x398, z);
  a->eyePos.y = y;
  gabi::store<f32>(b + 0x394, y);
  t0 = attribute();
  t1 = attribute();
  t2 = attribute();
  f32 cy = ((f32)t0->m18 * (f32)t1->m24) * (f32)t2->m20;
  gabi::call(0x025D6768, a, 0.f, cy, 0.f, 300.f);
  gabi::Local<cXyz> pos, scale;
  pos->set(a->current.pos.x, a->current.pos.y, a->current.pos.z);
  gabi::Local<csXyz> angle;
  gabi::call(0x0201A478, angle.get(), (s16)0, (s16)a->home.angle.y, (s16)0);
  t0 = attribute();
  t1 = attribute();
  f32 mag = (f32)t0->m28 * (f32)t1->m2C;
  scale->set(mag, mag, mag);
  s32 item = gabi::call<s32>(0x02322374, a, 6, 0);
  s32 save = gabi::call<s32>(0x02322374, a, 7, 16);
  BOOL spawn = gabi::call<BOOL>(0x02322374, a, 1, 28);
  s8 room = a->home.roomNo;
  a->mItemId = gabi::call<u32>(0x025D86E0, pos.get(), item, save, room,
                               angle.get(), scale.get(), spawn != 0);
  a->m468 = 0;
  a->m460 = 0xFFFFFFFF;
  a->m470 = 0;
  a->m45C = gabi::load<f32>(0x10025928);
  if (!gabi::call<BOOL>(0x02322374, a, 1, 8)) {
    f32 x = a->current.pos.x;
    t0 = attribute();
    t1 = attribute();
    t2 = attribute();
    f32 mag = (f32)t1->m24 * (f32)t2->m20;
    gabi::Local<cXyz> flagpos;
    flagpos->set(x, gabi::fmadds((f32)t0->m14 - 5.f, mag, a->current.pos.y),
                 a->current.pos.z);
    s32 type = a->m410;
    u32 param = (type == 2 || type == 3) ? 2 : 1;
    if (type == 3)
      param |= 0x80000000;
    u32 parent = gabi::load<u32>(b + 4);
    s8 room = a->current.roomNo;
    s32 texture = gabi::call<s32>(0x02322374, a, 1, 10);
    param |= (u32)texture << 8;
    a->m460 = gabi::call<u32>(0x025D5A20, 0x1D1, parent, param, flagpos.get(),
                              room, &a->shape_angle, nullptr, -1, nullptr);
  }
  gabi::call(0x0231F340, a);
  u32 play = dComIfGp_ea();
  a->m46A = gabi::load<s16>(play + 0x513C);
  gabi::call(0x0231F344, a);
  return phase;
}
VERIFY(0x0231F350, actor_create);
